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
const PIN = '9f622629f595db85e6f76c45c06edde29d7d9961';

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
