/**
 * Portable contract test for the shipping Artemis CI build.
 *
 * It does NOT run the build, and it does not simulate Actions. What it checks is the set of
 * agreements that are all cheap to break and none of which the build itself would notice:
 * the pins record, the workflow that a caller dispatches, the working-directory assumption its
 * inputs encode, the publishable file list, and the guard that owns the rebuild's source offer.
 * `npm run check:sources` reads the same files with the same intent; this test is the part that
 * can be read as a list of what each value is FOR.
 *
 * Everything is read from the repository. No dependencies, no network, no engine, no emsdk.
 */
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const read = (file) => readFileSync(join(ROOT, file), 'utf8');
const json = (file) => JSON.parse(read(file));

const PINS = '.github/upstream-pins.json';
const WORKFLOW = '.github/workflows/build-wasm-artemis-compat.yml';
const BRIDGE_WORKFLOW = '.github/workflows/build-wasm-artemis-bridge.yml';
const SCRIPT = '.github/scripts/build-artemis-compat.sh';
const RECIPE = 'ports/artemis/compat-web/build.sh';
// The reviewed upstream commit. `17200e64` is the fresh re-pull of artemis-compat's default branch
// (`.ref/artemis-compat-refresh/src`, clean, 257 tracked files); the previous `9f622629` is kept
// only as a comparison baseline in docs, and must NOT be what this build compiles.
const PIN = '17200e64ea99681e3c27d694433c0ff78d7a7cee';

/**
 * The `with:` block of the caller, read the way the guard reads it: `key: value` lines at the
 * input indent level, comments skipped, folded values joined. Not a YAML parse -- this test has
 * to run in a clean clone with nothing installed.
 */
function callInputs(source) {
  const match = source.match(/\n\s*with:\n/);
  assert.ok(match, `${WORKFLOW} must call the reusable workflow with a \`with:\` block`);
  const inputs = new Map();
  for (const line of source.slice(match.index + match[0].length).split(/\r?\n/)) {
    if (/^\s*$/.test(line)) break;
    if (/^\s*#/.test(line)) continue;
    const entry = line.match(/^\s{6,}([a-z][a-z0-9-]*):[ \t]*(.*)$/);
    if (!entry) {
      const last = [...inputs.keys()].pop();
      assert.ok(last && /^\s{8,}\S/.test(line), `unexpected line outside the \`with:\` block: ${line}`);
      inputs.set(last, `${inputs.get(last)} ${line.trim()}`);
      continue;
    }
    // A folded scalar (`>-`) is a YAML marker, not part of the value.
    inputs.set(entry[1], entry[2].trim().replace(/^>-?$/, ''));
  }
  for (const [key, value] of inputs) inputs.set(key, value.trim());
  return inputs;
}

test('the shipping pin is a reviewed commit, not a name or a short hash', () => {
  const pins = json(PINS);
  const record = pins['artemis-compat'];
  assert.ok(record, `${PINS} must carry the artemis-compat record`);
  assert.match(record.commit, /^[a-f0-9]{40}$/);
  assert.equal(record.commit, PIN, 'the pin recorded here is the one the artifacts were built from');
  assert.equal(record.branch, 'main');
  assert.equal(record.repository, 'https://github.com/Weiss-UltimateSavior/artemis-compat.git');
  // Sibling of `engines`, never iterated by the drift check: an upstream move must not start a
  // build, because the patch set in ports/artemis/compat-web/ is reviewed against THIS commit.
  assert.equal(record.dispatch, false);
  assert.equal(record['git-archive'], true, 'the recipe archives the pin rather than building the checkout');
  assert.ok(!('engines' in pins) === false && !(record.engine in (pins.engines ?? {})),
    'artemis-compat must not be an `engines` key: those are dispatchable');
});

test('the workflow passes the pin to the build script instead of letting it default', () => {
  const source = read(WORKFLOW);
  const inputs = callInputs(source);
  const buildEnv = inputs.get('build-env') ?? '';
  assert.match(buildEnv, new RegExp(`\\bARTC_PIN=${PIN}\\b`),
    'the reviewed pin must reach the recipe as ARTC_PIN; the recipe has its own default, so a run '
    + 'that passes nothing still builds SOMETHING and looks correct');
  assert.doesNotMatch(buildEnv, /\bARTC_REF=/,
    'ARTC_REF is left to the build script, which refuses to run unless it equals UPSTREAM_PATH');

  // The pin is written in three places and a dispatch input could only move one of them, so the
  // run would fetch one commit while the recipe archived another and still come out green -- with
  // a source tarball, a PIN file and a release tag naming a commit that was never compiled.
  assert.equal(inputs.get('upstream-commit'), `'${PIN}'`,
    'upstream-commit must be the reviewed pin literally, not an expression that an input can bend');
  assert.doesNotMatch(inputs.get('upstream-commit'), /\$\{\{/,
    'no template expression may reach the checkout commit: the pin is a reviewed constant');
  const dispatch = source.match(/\n {2}workflow_dispatch:\n([\s\S]*?)\n {2}\w/);
  assert.ok(dispatch, 'workflow_dispatch must still exist, with its inputs spelled out');
  assert.ok(!/^\s+upstream_commit:/m.test(dispatch[1]),
    'there must be no upstream_commit dispatch input: an arbitrary override records a new manifest '
    + 'SHA while the archived source stays at the reviewed pin, which is false provenance');
  // Matched on non-comment lines only: the workflow explains why the override was removed, and a
  // whole-file search would flag that explanation instead of a live expression.
  const code = source.split(/\r?\n/).filter((line) => !/^\s*#/.test(line)).join('\n');
  assert.doesNotMatch(code, /inputs\.upstream_commit/,
    'no executable line may read a dispatch-supplied commit');
});

test('the build script refuses to run when the checkout and the archive pin disagree', () => {
  // The static half of the guarantee above. The workflow spells the pin twice, so the runtime
  // check is what makes a half-edit a failed step instead of a mislabelled artifact.
  const script = read(SCRIPT);
  assert.match(script, /set -euo pipefail/, 'an unset variable must stop the script, not expand empty');
  assert.match(script, /UPSTREAM_COMMIT:?\??=/, 'the script must read the workflow-exported commit');
  assert.match(script, /"\$UPSTREAM_COMMIT" != "\$ARTC_PIN"/,
    'the checked-out commit and the archived pin must be compared before anything is built');
  assert.ok(/exit 1/.test(script.slice(script.indexOf('$UPSTREAM_COMMIT" != "$ARTC_PIN"'))),
    'a mismatch has to fail the run');
  // The recursive delete has to be resolved and pinned to the exact expected root first: an
  // unverified `rm -rf "$DIST"` is indistinguishable from a correct one until the day it is not.
  assert.match(script, /DIST_ABS="\$REPO_ROOT\/\$DIST"/);
  assert.match(script, /"\$DIST_ABS" = "\$REPO_ROOT\/dist\/artemis-compat"/);
  assert.match(script, /rm -rf "\$DIST_ABS"/);
  assert.doesNotMatch(script, /rm -rf "\$DIST"/, 'the unchecked form must not come back');
});

test('the hash manifest names the published files instead of a glob', () => {
  const script = read(SCRIPT);
  // `sha256sum ./*` writes `./`-prefixed names that are not the artifact's own names, so a
  // consumer checking the downloaded set has to rewrite them. Naming the six files keeps the
  // manifest usable as published, and it cannot pick up a seventh file that rode into dist.
  assert.doesNotMatch(script, /sha256sum \.\/\*/,
    'a glob writes ./LICENSE rather than LICENSE, and would silently cover any extra file');
  assert.match(script, /sha256sum artemis-compat\.js artemis-compat\.wasm artemis-compat-source\.tar\.gz/);
  assert.match(script, /sha256sum --check --quiet/,
    'the manifest is verified in the run that publishes it, not only written');
  assert.ok(!/artemis-compat-source\.tar\.gz\.sha256\s*\\?\s*$/m.test(
    script.slice(script.indexOf('sha256sum artemis-compat.js'))),
  'the manifest must not list itself');
});

test('the checkout the workflow fetches is the tree the recipe archives', () => {
  const inputs = callInputs(read(WORKFLOW));
  const record = json(PINS)['artemis-compat'];
  assert.equal(inputs.get('upstream-path'), record.path);
  assert.equal(inputs.get('upstream-path'), '.ref/artemis-compat');
  assert.equal(inputs.get('skip-upstream-checkout'), 'false',
    'the pin has to be fetched by the workflow: with skip-upstream-checkout the recipe would clone '
    + 'it itself and the run would have no step that asserts the pin landed at that path');
  const script = read(SCRIPT);
  assert.match(script, /\[ "\$ARTC_REF" = "\$UPSTREAM_PATH" \]/,
    'the build script must assert its archive source is the path the workflow checked out');
  assert.match(script, /UPSTREAM_PATH/, 'the build script must read the workflow-exported path');
});

test('the artifact is a source build, not an SDK build', () => {
  // The two things that would make this an SDK build: the retained official module as an input,
  // or the committed import map of the SDK module's import surface as a gate.
  const inputs = callInputs(read(WORKFLOW));
  assert.equal(inputs.get('glue-import-map'), "''",
    'the committed import map is the 207-import SDK module\'s import surface; this module is a '
    + 'different module and the table says nothing about it');
  const script = read(SCRIPT);
  assert.doesNotMatch(script, /vendor\/artemis-sdk/,
    'no path under the retained SDK may be an input to the shipping build');
  // Matched on an executable line: the header explains the distinction in prose, and a whole-file
  // search would flag the explanation instead of the code.
  const code = script.split(/\r?\n/).filter((line) => !/^\s*#/.test(line)).join('\n');
  assert.doesNotMatch(code, /wasm2c/, 'wasm2c is the SDK route, not this one');
  const recipe = read(RECIPE);
  assert.match(recipe, /Weiss-UltimateSavior\/artemis-compat\.git/, 'the recipe clones the GPL source');
});

test('the artifact carries exactly the modules, the source offer and the license pair', () => {
  const record = json(PINS)['artemis-compat'];
  const expected = ['artemis-compat.js', 'artemis-compat.wasm', 'artemis-compat-source.tar.gz',
    'artemis-compat-source.tar.gz.sha256', 'LICENSE', 'THIRD_PARTY_NOTICES.md'];
  assert.deepEqual(record.files, expected);
  const inputs = callInputs(read(WORKFLOW));
  // `artifact-files` is what upload-artifact publishes and what make-manifest.mjs checks, so it
  // has to be the same set the record describes, in the same order.
  assert.deepEqual(inputs.get('artifact-files').split(/\s+/), expected);
  assert.equal(inputs.get('artifact-dir'), record['artifact-dir']);
  assert.equal(inputs.get('package-dir'), 'packages/runtime-artemis');
  assert.equal(inputs.get('package-engine-assets'), 'true',
    'the compat worker loads both modules by URL beside the package');
  assert.equal(inputs.get('publish'), 'false',
    'nothing is released until a real game boots these bytes; audio is still the upstream stub');
});

test('the workflow that builds the shipping module runs on changes to its own inputs', () => {
  const source = read(WORKFLOW);
  assert.match(source, /^\non:\n/m, 'a workflow needs triggers');
  for (const path of [`'${RECIPE.replace('build.sh', '**')}'`, `'${SCRIPT}'`]) {
    assert.ok(source.includes(path), `${WORKFLOW} must rebuild when ${path} changes`);
  }
  // The worker answers `artcReadRange` against this module, so a change on either side has to run.
  // Covered as the whole package, not just `src/compat/`: the tarball this run builds is packed
  // from the package, so a copy or a manifest edit changes the published bytes too.
  assert.ok(source.includes("'packages/runtime-artemis/**'"),
    'the worker answers `artcReadRange` against this module, so a change on either side has to run');
  assert.ok(source.includes("'packages/vfs/**'"),
    'the VFS provider is an input to the package step, so a copy-only change still has to run');
  const pullRequest = source.match(/\n {2}pull_request:\n([\s\S]*?)\n {2}\w/);
  assert.ok(pullRequest, 'pull_request is what shows an author a stale contract before merge');
  assert.ok(pullRequest[1].includes(`'${RECIPE.replace('build.sh', '**')}'`));
});

test('the legacy SDK bridge is manual only and keeps its frozen diagnostic record', () => {
  const bridge = read(BRIDGE_WORKFLOW);
  const triggers = bridge.match(/\non:\n([\s\S]*)/);
  assert.ok(triggers, `${BRIDGE_WORKFLOW} must still declare triggers`);
  const keys = triggers[1].split(/\r?\n/).filter((line) => /^ {2}\S/.test(line)).map((line) => line.trim());
  // A push or pull_request trigger here is what made every change to ports/artemis/src/** start a
  // 30-minute SDK rebuild whose output did not depend on the checkout it ran against.
  for (const forbidden of ['push:', 'pull_request:', 'schedule:']) {
    assert.ok(!keys.some((key) => key.startsWith(forbidden)),
      `${BRIDGE_WORKFLOW} is manual/diagnostic only; ${forbidden} must not come back`);
  }
  assert.ok(keys.some((key) => key.startsWith('workflow_dispatch:')),
    'the legacy route stays reproducible by hand');
  const record = json(PINS)['artemis-bridge'];
  // Frozen evidence, not a shipping statement: the historical transpile hashes stay, and the
  // record does not claim to be the route the shipping module comes from.
  assert.equal(record.target, 'asyncify');
  assert.equal(record.module, 'ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm');
  assert.equal(record.expected.asyncify.bytes, 107593501);
  assert.equal(record.expected.asyncify.md5, 'd895a74b11ba767b06c82d722c407517');
  assert.equal(record.expected.sync.md5, '080822b6e14345d339312d1e1bcfd0aa');
});

test('the artifact filenames are the ones the recipe emits and the host expects', () => {
  // Two renames that would each be silent: CMake `OUTPUT_NAME` decides the module filenames, and
  // the manifest checks the names in `artifact-files`. If they disagree the build is green and
  // the manifest step is the only thing that fails, 40 minutes in.
  const cmake = read('ports/artemis/compat-web/overlay/cmake/web.cmake');
  assert.match(cmake, /OUTPUT_NAME "artemis-compat"/);
  const record = json(PINS)['artemis-compat'];
  assert.ok(record.files.includes('artemis-compat.js') && record.files.includes('artemis-compat.wasm'));
  // The host ABI the module must export: the worker binds these by name through cwrap, so a
  // rename on the C++ side is a runtime `missing _artc_web_*` rather than a link error.
  const host = read('ports/artemis/compat-web/overlay/host/web_main.cpp');
  const exported = new Set([...host.matchAll(/EMSCRIPTEN_KEEPALIVE[^;]*?\b(artc_web_[a-z_]+)\s*\(/g)]
    .map((match) => match[1]));
  for (const name of ['artc_web_open', 'artc_web_frame', 'artc_web_pointer', 'artc_web_pointer_move',
    'artc_web_key', 'artc_web_resize', 'artc_web_started', 'artc_web_last_error',
    'artc_web_pause', 'artc_web_dispose', 'artc_web_save', 'artc_web_load']) {
    assert.ok(exported.has(name), `the compat host must still export ${name}`);
  }
  // The two frame-loop exports the freeze removed must stay removed: the host drives the tick.
  for (const name of ['artc_web_start', 'artc_web_stop']) {
    assert.ok(!exported.has(name), `${name} must not come back; the host owns the frame loop`);
  }
  assert.match(read('ports/artemis/compat-web/README.md'), /no `requestAnimationFrame`/);
});

test('the shipping pin, the workflow literal and the recipe default are one commit', () => {
  // Four files spell the same commit, and each one is a separate chance to drift. The runtime
  // check in the build script catches a workflow that disagrees with itself; this catches the
  // pins record and the recipe default, which nothing executes.
  const pin = json(PINS)['artemis-compat'].commit;
  assert.equal(pin, PIN);
  const source = read(WORKFLOW);
  const occurrences = source.match(new RegExp(PIN, 'g')) ?? [];
  assert.equal(occurrences.length, 2,
    'the full pin appears exactly twice: the checkout commit and the build-env value');
  assert.ok(source.includes(`upstream-commit: '${PIN}'`));
  assert.ok(source.includes(`build-env: ARTC_PIN=${PIN}`));
  // The version strings are the short form, so they have to be derived from the same pin rather
  // than typed: a version tag naming a commit the modules were not built from is the same class of
  // false provenance as an overriding dispatch input, and it is the one a reader trusts most.
  const short = PIN.slice(0, 7);
  assert.ok(source.includes(`version: compat-${short}`));
  assert.ok(source.includes(`release-tag: engine-artemis-compat-${short}`));
  assert.match(read(RECIPE), new RegExp(`PIN="\\$\\{ARTC_PIN:-${PIN}\\}"`),
    'the recipe default and the pins record must not drift apart');
  assert.ok(read('ports/artemis/compat-web/make-source-bundle.mjs').includes(`'${PIN}'`),
    'the source-offer generator defaults to the same pin');
});

test('the source offer is generated from the pin and carries the patches that were applied', () => {
  // The module is GPL-3.0 and built from GPL-3.0 source, so the artifact has to answer for that
  // source. The bundle is generated by the recipe's own directory, from the pinned archive plus
  // the patch set -- not from whatever happens to be checked out.
  const script = read(SCRIPT);
  assert.match(script, /make-source-bundle\.mjs/);
  assert.match(script, /--out "\$DIST"/);
  assert.match(script, /--repo "\$REPO_ROOT\/\$ARTC_REF"/,
    'the offer must archive the checkout this run built, not a local refresh path');
  assert.match(read('ports/artemis/compat-web/make-source-bundle.mjs'), /--repo <dir>/);
  const pin = json(PINS)['artemis-compat'].commit;
  const bundle = read('ports/artemis/compat-web/make-source-bundle.mjs');
  assert.ok(bundle.includes(pin), 'the bundle generator defaults to the same pin');
  for (const file of ['README.md', 'patches/0001-pf8-readat-host-source.patch',
    'patches/0002-numeric-strto.patch', 'overlay/host/web_main.cpp', 'overlay/cmake/web.cmake']) {
    assert.ok(bundle.includes(`'${file}'`), `the source offer must carry ${file}`);
  }
});

test('the source offer carries a build recipe, not just the patches it should apply', () => {
  // The pinned upstream tree has no `build.sh`, no `ports/` and no web CMake target: the overlays
  // and patches are only a build input *through* the recipe. An offer without it is a pile of
  // files with no documented way back to the binary, which GPL-3.0's "preferred form for making
  // modifications" is not satisfied by.
  const bundle = read('ports/artemis/compat-web/make-source-bundle.mjs');
  assert.ok(bundle.includes(`['build.sh', 'build.sh']`),
    'the tracked recipe must ride in the archive at the path it runs from');
  // The licences travel as upstream files, never as text quoted into a notice: a wrong path here
  // is a bundle whose Lua notice is a paragraph this repository wrote.
  assert.ok(bundle.includes(`['LICENSE', 'LICENSE']`));
  assert.ok(bundle.includes(`['third_party/lua-5.1.5/COPYRIGHT', 'third_party/lua-5.1.5/COPYRIGHT']`),
    'Lua 5.1 lives at third_party/lua-5.1.5/, not third_party/lua51/ (that is only the CMake target)');
  assert.ok(!bundle.includes('lua51/COPYRIGHT'),
    'third_party/lua51/ does not exist upstream; copying that path would ship no notice');
});

test('the source offer carries the local audio sources, which the pin does not contain', () => {
  // The audio backend is not upstream. `overlay/audio/audio_web.cpp` is compiled *instead of* the
  // pinned `src/audio/audio.cpp`, `audio_web.h` is the seam the host shares with it, and
  // `audio_validation.h` is the argument grammar the host applies to the page's numbers. The
  // bundle copies `src/` from the pin -- which has none of the three -- plus a fixed entry list,
  // so an entry missing here is a source offer that cannot rebuild the module beside it.
  const bundle = read('ports/artemis/compat-web/make-source-bundle.mjs');
  for (const file of ['overlay/audio/audio_web.cpp', 'overlay/audio/audio_web.h',
    'overlay/audio/audio_validation.h']) {
    assert.ok(bundle.includes(`'${file}'`),
      `the source offer must carry ${file}; it exists only in this repository, so the bundled `
      + 'src/ tree alone cannot reproduce the audio backend');
  }
  // Every one of them has to be a *required source entry* too, or an upload draft can drop one
  // while the generator keeps pointing at it. Existence on disk is the guard's job
  // (`npm run check:sources`), which reads the same list this test reads.
  const guard = read('scripts/check-sources.mjs');
  for (const file of ['ports/artemis/compat-web/overlay/audio/audio_web.cpp',
    'ports/artemis/compat-web/overlay/audio/audio_web.h',
    'ports/artemis/compat-web/overlay/audio/audio_validation.h']) {
    assert.ok(guard.includes(`'${file}'`), `${file} must be a required source entry`);
  }
});

test('the source offer ships a recipe that builds without this repository', () => {
  // `build.sh` is shipped verbatim for its build logic, but it cannot run inside the archive:
  // it sources `ports/env.sh` and refuses any root not ending in `ports/artemis/compat-web/out`,
  // and the archive has neither. Shipping only that script gives a recipient a recipe that
  // refuses to start, so the archive carries a standalone one alongside it.
  const bundle = read('ports/artemis/compat-web/make-source-bundle.mjs');
  assert.ok(bundle.includes(`['RUN-BUILD.sh', 'RUN-BUILD.sh']`),
    'the standalone recipe must ride in the archive at the path it runs from');
  const guard = read('scripts/check-sources.mjs');
  assert.ok(guard.includes("'ports/artemis/compat-web/RUN-BUILD.sh'"),
    'the standalone recipe is a build input and must be a required source entry');

  const recipe = read('ports/artemis/compat-web/RUN-BUILD.sh');
  // It has to apply both patches and append the web CMake target itself: those are the three
  // steps `build.sh` performs that the pinned tree cannot supply.
  for (const patch of ['0001-pf8-readat-host-source.patch', '0002-numeric-strto.patch']) {
    assert.ok(recipe.includes(patch), `RUN-BUILD.sh must apply ${patch}`);
  }
  assert.match(recipe, /cat "\$HERE\/overlay\/cmake\/web\.cmake" >>"\$BUILD\/CMakeLists\.txt"/,
    'RUN-BUILD.sh must append the web CMake target, which the pinned CMakeLists.txt lacks');
  assert.match(recipe, /ARTC_BUILD_WEB=ON/, 'the web target has to be switched on');
  // It must not reach for the repository it is no longer inside. Checked on the executable
  // lines only: the script *explains* in a comment that `build.sh` sources `ports/env.sh` and
  // why that makes it unrunnable here, and a whole-file search would flag that explanation.
  const code = recipe.split(/\r?\n/).filter((line) => !/^\s*#/.test(line)).join('\n');
  assert.doesNotMatch(code, /ports\/env\.sh/,
    'the bundled recipe cannot source ports/env.sh: the archive contains no ports/ directory');
  assert.doesNotMatch(code, /\bgit\b|clone|https?:\/\//,
    'the archive must build from its own sources, not by cloning upstream again');
});

test('tar is given repo-relative paths, because GNU tar reads D: as a remote host', () => {
  // Measured, not theorised: `tar -cf D:/path/x.tar.gz` exits 2 with
  // `Cannot connect to D: resolve failed`. Passing the resolved absolute form turns every run into
  // a failure regardless of how correct the tree is, and on a runner the message reads like an
  // infrastructure problem rather than a path-spelling one.
  const bundle = read('ports/artemis/compat-web/make-source-bundle.mjs');
  assert.match(bundle, /paths\.relative\(tarball\)/,
    'the -cf argument must be repo-relative, never the absolute Windows path');
  assert.doesNotMatch(bundle, /'-cf', tarball/, 'the absolute form is the bug this replaced');
  const script = read(SCRIPT);
  // The wrapper stages into an absolute path too, but it hands that to `cp`, not to tar.
  assert.match(script, /cp "\$JS" "\$WASM" "\$DIST\/"/);
});

test('the CI wrapper does not name a runtime the checkout does not have', () => {
  // `-j "$(nproc)"` on a runner with dozens of cores is how a wasm link gets OOM-killed, and the
  // retry-after-timeout reads as flakiness rather than as the parallelism the run chose.
  const script = read(SCRIPT);
  assert.doesNotMatch(script, /\bnproc\b/,
    'the job count must be fixed (-j2), never derived from the runner core count');
  assert.match(script, /bash ports\/artemis\/compat-web\/build\.sh -j "\$JOBS"|build\.sh -j 2/,
    'the recipe must be invoked with a small fixed -j');
  assert.match(script, /JOBS=2|JOBS:-2|JOBS="\$\{?JOBS/);
});

/* ------------------------------------------------------------------ */
/* Which app files make each workflow re-run.                          */
/* ------------------------------------------------------------------ */

/**
 * `apps/app/scripts/artemis-assets.mjs` verifies a `dist/artemis-compat/` directory against the
 * sha256 manifest the compat run produces, and `apps/app/astro.config.mjs` mounts it. Neither
 * changes the engine binary. What they change is what a *given* artifact is accepted as -- so a
 * change to either one has to re-run the compat workflow that produced the artifact, or the pair
 * (artifact SHA, acceptance rule) can drift silently. These two names are therefore in the `push`
 * and `pull_request` `paths` of both workflows, spelled exactly: not `apps/**` (the rest of the app
 * is not an input to those bytes) and not `apps/app/scripts/**` (a glob over a directory one file
 * lives in; the next script added there is not automatically an input either).
 *
 * Read with the same tiny line parser the rest of this file uses, not a YAML dependency: the
 * workflows' own `paths` blocks are flat `- '...'` lists, and this test has to run in a clean
 * clone with nothing installed.
 */
const PACKAGES_WORKFLOW = '.github/workflows/build-packages.yml';

/** The two exact app inputs. No glob may stand in for either one. */
const APP_INPUTS = [
  'apps/app/astro.config.mjs',
  'apps/app/scripts/artemis-assets.mjs',
];

/**
 * The `paths:` list under one trigger key. `push` and `pull_request` are the only triggers here;
 * the block ends at the next line indented two spaces (`workflow_dispatch:`, `concurrency:`),
 * which is what keeps a `paths` under a different trigger from being read instead.
 */
function paths(workflow, trigger) {
  const source = read(workflow);
  const block = source.match(
    new RegExp(`\\n {2}${trigger}:\\n([\\s\\S]*?)(?=\\n {2}\\S|$)`),
  );
  assert.ok(block, `${workflow} must declare a \`${trigger}:\` trigger with its inputs`);
  // `paths:` is not always the first key (a `push` trigger lists `branches:` before it), so it is
  // found anywhere at the trigger's own indent level -- indented relative to the capture start.
  const list = block[1].match(/^ {4}paths:\n([\s\S]*)/m);
  assert.ok(list, `${workflow}'s \`${trigger}:\` trigger must carry an explicit \`paths:\` list`);
  return {
    source,
    entries: [...list[1].matchAll(/^\s*- '([^']+)'$/gm)].map((match) => match[1]),
    raw: list[1],
  };
}

test('every fixed upstream commit is one full exact SHA, with no override path anywhere', () => {
  // Moving the pin is a reviewed edit in four places, and a half-edit is the failure this guards:
  // a run that checks out one commit while the recipe archives another ships a source tarball, a
  // PIN file and a release tag that name a commit nobody compiled. Every literal is asserted to be
  // the SAME full 40-hex SHA, so a shortened form or a second commit cannot slip through.
  const FULL = /^[a-f0-9]{40}$/;
  const pins = json(PINS);
  assert.match(pins['artemis-compat'].commit, FULL, `${PINS}: the compat pin must be a full exact SHA`);
  assert.equal(pins['artemis-compat'].commit, PIN);

  const source = read(WORKFLOW);
  const inputs = callInputs(source);
  assert.equal(inputs.get('upstream-commit'), `'${PIN}'`,
    'the checked-out commit is a literal reviewed SHA, never an expression');
  assert.equal(inputs.get('build-env'), `ARTC_PIN=${PIN}`,
    'the pin the recipe archives is the same literal as the checkout');
  // No dispatch input may carry a commit, and no shell/expression override may reach either spot.
  // `${{ }}` around a commit is how a dispatch input, a matrix or a reusable-workflow input would
  // get in; the header comment already says there is deliberately no `upstream_commit` input.
  const dispatch = source.match(/\n {2}workflow_dispatch:\n([\s\S]*?)\n {2}\w/);
  assert.ok(dispatch, `${WORKFLOW} must still declare workflow_dispatch, with its inputs spelled out`);
  for (const line of dispatch[1].split(/\r?\n/)) {
    const input = line.match(/^ {6}([a-z][a-z0-9_-]*):/);
    assert.ok(!input || !/commit|sha/i.test(input[1]),
      `${WORKFLOW} must not offer a dispatch input named ${input?.[1]}: the pin cannot be bent at run time`);
  }
  for (const key of ['upstream-commit', 'build-env']) {
    const value = inputs.get(key) ?? '';
    assert.ok(!/\$\{\{/.test(value),
      `${WORKFLOW}: ${key} must be a literal, not an expression that could fetch a different commit`);
  }
  // No `upstream_override` of any spelling, and no env var that could substitute for the pin.
  assert.ok(!/upstream[_-]override/i.test(source),
    `${WORKFLOW} must not define an upstream_override; the pin only moves by a reviewed edit`);
  assert.ok(!/ARTC_PIN\s*[:=]\s*\$\{?(?!ARTC_PIN)/.test(source),
    'ARTC_PIN must not be re-derived from anything else in the workflow');
  // The pins file records the compat SHA once. A second record naming the same engine would be a
  // second place to keep in sync, and the drift check dispatches `engines` on its own.
  for (const [name, engine] of Object.entries(pins.engines ?? {})) {
    assert.notEqual(engine.repository, pins['artemis-compat'].repository,
      `${PINS}: ${name} must not point at the compat repository; that map is dispatchable`);
  }
  // The previous pin must not survive anywhere as a build input. It is still allowed in docs as a
  // comparison baseline, but no file that decides what CI compiles may name it.
  for (const file of [PINS, WORKFLOW, SCRIPT]) {
    assert.ok(!read(file).includes('9f622629'),
      `${file} still names the superseded pin 9f622629; the build would compile or archive the old `
      + 'commit while the fresh re-pull is what was reviewed');
  }
});

test('the compat workflow re-runs on each app input on push and on pull_request', () => {
  for (const trigger of ['push', 'pull_request']) {
    const { entries } = paths(WORKFLOW, trigger);
    assert.ok(entries.length > 0, `${WORKFLOW} ${trigger} must list paths`);
    for (const file of APP_INPUTS) {
      assert.ok(entries.includes(file),
        `${WORKFLOW} must re-run on ${trigger} when ${file} changes: that file decides whether the `
        + 'artifact this workflow produced is accepted, so the artifact and its acceptance rule '
        + 'have to move together');
    }
    // The whole point of naming the two files: a glob makes every unrelated app edit start a
    // 40-minute engine build, and this run does not produce app bytes.
    for (const glob of ['apps/**', 'apps/app/**', 'apps/app/scripts/**']) {
      assert.ok(!entries.includes(glob),
        `${WORKFLOW} ${trigger} must not list '${glob}': two exact names are the contract, a glob `
        + 'schedules an engine build for files that are not inputs to the bytes');
    }
  }
});

test('the app inputs do not displace the engine inputs those workflows already had', () => {
  // The failure this guards against is a `paths` list that was rewritten rather than extended:
  // the two app names added, an engine input quietly dropped, and the workflow green on a change
  // it should have rebuilt from. The two triggers do not carry the same list (`push` sees the
  // pins record and the reusable workflow, `pull_request` a narrower set), so each is asserted
  // against the inputs it is actually contracted to carry.
  const expected = {
    [WORKFLOW]: {
      push: [
        'ports/artemis/compat-web/**',
        'packages/runtime-artemis/**',
        'packages/vfs/**',
        '.github/scripts/build-artemis-compat.sh',
        '.github/upstream-pins.json',
        'pnpm-lock.yaml',
      ],
      pull_request: [
        'ports/artemis/compat-web/**',
        'packages/runtime-artemis/**',
        'packages/vfs/**',
        '.github/scripts/build-artemis-compat.sh',
      ],
    },
    [PACKAGES_WORKFLOW]: {
      push: [
        'packages/**',
        'test/**',
        'scripts/run-portable-tests.mjs',
        'pnpm-lock.yaml',
        'LICENSE',
      ],
      pull_request: [
        'packages/**',
        'test/**',
        'scripts/run-portable-tests.mjs',
        'package.json',
      ],
    },
  };
  for (const [workflow, triggers] of Object.entries(expected)) {
    for (const [trigger, inputs] of Object.entries(triggers)) {
      const { entries } = paths(workflow, trigger);
      for (const file of inputs) {
        assert.ok(entries.includes(file),
          `${workflow} ${trigger} must keep '${file}'; the app inputs are an addition to this `
          + 'list, never a replacement for it');
      }
    }
  }
});

test('the portable suite that reads the app seam is what re-runs on its change', () => {
  // The compat workflow runs no portable suite, so its guarantee is the artifact contract alone.
  // `build-packages` DOES run it, and that suite imports both files -- so its trigger has to
  // name them for the same reason it names `test/**`.
  for (const trigger of ['push', 'pull_request']) {
    const { entries } = paths(PACKAGES_WORKFLOW, trigger);
    for (const file of APP_INPUTS) {
      assert.ok(entries.includes(file),
        `${PACKAGES_WORKFLOW} must re-run on ${trigger} when ${file} changes: the portable suite `
        + 'below loads it, so a change there is a change to what this job tests');
    }
  }
  // The test that reads them is covered by `test/**`, which this workflow already lists, so it
  // is not named a second time -- naming it would be a second place to keep in sync.
  const { entries } = paths(PACKAGES_WORKFLOW, 'push');
  assert.ok(entries.includes('test/**'),
    `${PACKAGES_WORKFLOW} discovers its suite through 'test/**'; without it a new test file is invisible`);
  assert.ok(!entries.includes('test/artemis-compat-app-assets.test.mjs'),
    'the individual test file is covered by test/** on this workflow and must not be listed twice');
});

test('the platform patch set is 2 explicit, and no declaration still applies a retired patch', () => {
  // The minimal port's set is named by one file, not inferred from the recipes' patch loops: a
  // table derived from the thing it constrains constrains nothing, which is how a third patch could
  // previously land in `patches/` with every declaration still green.
  //
  // What the earlier version of this test asserted -- that `0003-scene-checkpoint.patch` is applied
  // everywhere and shipped in the offer -- is exactly what the minimal port retires. The patch is a
  // save/restore FEATURE, not a platform adaptation, so it is not a build input to this port.
  //
  // `scripts/check-sources.mjs` enforces the same list against the checkout; this test is the part
  // that can be read as a statement of what the list is FOR, and it runs without a git repository.
  const LIST = 'ports/artemis/compat-web/PLATFORM-PATCHES.txt';
  const names = read(LIST).split(/\r?\n/)
    .map((line) => line.replace(/#.*$/, '').trim()).filter(Boolean);
  assert.deepEqual(names, ['0001-pf8-readat-host-source.patch', '0002-numeric-strto.patch'],
    `${LIST} must name exactly the two platform adaptations, in application order; the count is `
    + 'the contract CI compiles, so adding or removing one is a deliberate edit to this test too');

  // Every named patch exists, and is one of the two the port is built from.
  for (const name of names) {
    assert.ok(read(`ports/artemis/compat-web/patches/${name}`).length > 0,
      `the platform list names ${name}, so the file has to be there`);
  }

  // Nothing in the closure still APPLIES a patch that is not on the list. Counted as patch
  // references that are not the two above, with comments stripped: each declaration explains in
  // prose which patches it applies, and a line saying `0003` is *not* applied is documentation.
  const RETIRED = '0003-scene-checkpoint.patch';
  for (const file of [RECIPE, 'ports/artemis/compat-web/RUN-BUILD.sh',
    'ports/artemis/compat-web/make-source-bundle.mjs']) {
    const code = read(file).split(/\r?\n/)
      .map((line) => line.replace(/(^|\s)(#|\/\/|\*).*$/, '$1')).join('\n');
    for (const name of code.match(/[\w.-]+\.patch/g) ?? []) {
      assert.ok(names.includes(name),
        `${file} references ${name}, which is not one of the ${names.length} platform patches; `
        + 'retired patches must not be applied be anywhere in the closure');
    }
    assert.ok(!code.includes(RETIRED),
      `${file} must not apply ${RETIRED}; it is retired from the minimal port`);
  }
  // The portable runner still has to discover the port's own tests/ directory. That is a separate
  // requirement from the patch set (the audio and read-host regressions live there too), so it is
  // asserted here rather than dropped along with the scene-specific case above.
  const runner = read('scripts/run-portable-tests.mjs');
  assert.ok(runner.includes("'ports/artemis/compat-web/tests'"),
    "the port's own tests/ directory holds regressions the portable runner has to reach: a suite it "
    + 'never scans is a regression nothing runs');
});

test('the compat workflow names the one test file that pins its artifact contract', () => {
  // `test/**` is deliberately not in the compat list. The single file that fails when the asset
  // seam and the recipe drift is, because an editor who breaks the contract by "just fixing the
  // test" would otherwise never see the compat run go red.
  for (const trigger of ['push', 'pull_request']) {
    const { entries } = paths(WORKFLOW, trigger);
    assert.ok(entries.includes('test/artemis-compat-app-assets.test.mjs'),
      `${WORKFLOW} ${trigger} must re-run when the app/artifact contract test changes`);
    assert.ok(!entries.includes('test/**'),
      `${WORKFLOW} ${trigger} must not list all of 'test/**': this job builds engine bytes, and the `
      + 'rest of the suite is not an input to them');
  }
});

test('the two listed app files exist, and are the files the contract test imports', () => {
  // A `paths` entry that names a path no longer in the tree is a trigger that can never fire,
  // and it looks exactly like a working one in review.
  for (const file of APP_INPUTS) {
    assert.doesNotThrow(() => read(file), `${file} is listed as a trigger input and must exist`);
  }
  const contract = read('test/artemis-compat-app-assets.test.mjs');
  for (const file of APP_INPUTS) {
    assert.ok(contract.includes(file),
      `the contract test must still import ${file}; if it stops, this trigger reasons about a `
      + 'file nothing loads');
  }
});
