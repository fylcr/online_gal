/**
 * Read-only source-upload guard. Worktree mode includes visible new files;
 * --staged reads the index, not newer working copies. Neither mode stages files,
 * builds engines, checks game behaviour, or grants redistribution permission.
 */
import { spawnSync } from 'node:child_process';
import { existsSync, readFileSync, statSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

export const SOURCE_ENTRIES = [
  'README.md', 'LICENSE', 'THIRD_PARTY_NOTICES.md', 'package.json', '.gitignore',
  'docs/source-layout.md', 'ports/README.md', 'ports/env.sh', 'ports/lib/ref.mjs',
  'ports/lib/engine-artifacts.json', '.github/upstream-pins.json',
  '.github/scripts/make-package.mjs', '.github/workflows/build-wasm-reusable.yml',
  '.github/scripts/build-ons.sh', '.github/workflows/build-wasm-ons.yml',
  'ports/ons/README.md', 'ports/ons/build/run-web-build-shimmed.sh',
  'ports/ons/build/ons_host_shim.h', 'ports/ons/build/ons_host_shim.cpp',
  '.github/scripts/build-krkr.sh', '.github/workflows/build-wasm-krkr.yml',
  'ports/krkr/README.md', 'ports/krkr/kirikiroid2/run-build.sh',
  'ports/krkr/kirikiroid2/patch-vlfs-host-source.mjs',
  'ports/krkr/kirikiroid2/overlay-ports/gettext/portfile.cmake',
  'ports/krkr/kirikiroid2/overlay-ports/libexif/portfile.cmake',
  // The shipping Artemis build: the recipe, its patches and overlays, the CI wrapper and the
  // source-offer generator. Dropping any one of them changes what CI compiles into the artifact.
  '.github/scripts/build-artemis-compat.sh', '.github/workflows/build-wasm-artemis-compat.yml',
  'ports/artemis/compat-web/build.sh', 'ports/artemis/compat-web/make-source-bundle.mjs',
  'ports/artemis/compat-web/README.md',
  'ports/artemis/compat-web/patches/0001-pf8-readat-host-source.patch',
  'ports/artemis/compat-web/patches/0002-numeric-strto.patch',
  'ports/artemis/compat-web/patches/0003-tween-loopdelay.patch',
  'ports/artemis/compat-web/patches/0004-animation-dispatch-observe.patch',
  // What the platform build is actually made of: the patch set is named here rather than inferred
  // from the recipes' own patch loops, so a third patch cannot be applied (or silently dropped)
  // without editing this file. See `compatPlatformPatches` for the two-directional check.
  'ports/artemis/compat-web/PLATFORM-PATCHES.txt',
  'ports/artemis/compat-web/overlay/host/web_main.cpp',
  'ports/artemis/compat-web/overlay/cmake/web.cmake',
  // The audio seam. `audio_web.cpp` is compiled *instead of* the pinned `src/audio/audio.cpp`,
  // and `audio_validation.h` is the argument grammar the page's numbers are checked against
  // before any of them reaches the heap. Both are build inputs: an upload or a source bundle
  // without them compiles a different backend than the one that was tested.
  'ports/artemis/compat-web/overlay/audio/audio_web.cpp',
  'ports/artemis/compat-web/overlay/audio/audio_web.h',
  'ports/artemis/compat-web/overlay/audio/audio_validation.h',
  // The portable audio contract test calls that shared validator, and the standalone recipe is
  // what lets the source bundle rebuild without this repository. Both travel with the build.
  'ports/artemis/compat-web/tests/artemis-compat-audio-native.test.mjs',
  'ports/artemis/compat-web/RUN-BUILD.sh',
  'packages/runtime-artemis/src/compat/compat-worker.js',
  // The legacy / diagnostic SDK bridge. Kept required because the records it produced are still
  // cited; it is no longer the shipping Artemis path (see the pins file).
  '.github/scripts/build-artemis-bridge.sh', '.github/workflows/build-wasm-artemis-bridge.yml',
  'ports/artemis/README.md', 'ports/artemis/REWRITE.md', 'ports/artemis/rewrite-names.tsv',
  'ports/artemis/src/util/engine_context.h', 'ports/artemis/reverse/build-bridge-route.sh',
  'ports/artemis/reverse/rw-splice.py', 'ports/artemis/reverse/test-rw-splice-diagnostics.py',
  'ports/artemis/tools/assemble-rewrite-parts.py',
  'packages/runtime-artemis/src/index.ts', 'packages/runtime-artemis/src/host/boot.ts',
  'packages/runtime-ons/src/wasm-runtime.ts', 'packages/runtime-kag/src/wasm-runtime.ts',
  'packages/runtime-kag/src/wasm-config.ts', 'scripts/check-sources.mjs',
  'test/check-sources.test.mjs', 'test/artemis-compat-ci.test.mjs',
];

const ASSET_EXCEPTIONS = new Set([
  'packages/runtime-ons/assets/onsyuri.wasm',
  'ports/artemis/vendor/artemis-sdk/WebAssembly/artemis.wasm',
  'ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm',
  'ports/artemis/vendor/artemis-sdk/Android/arm64-v8a/libartemis.so',
  'ports/artemis/vendor/artemis-sdk/Android/armeabi-v7a/libartemis.so',
]);

export function uploadProblem(file) {
  if (/(^|\/)(?:\.ref|\.test|node_modules|dist|out|build-final|__pycache__|\.vcpkg-binary-cache)(?:\/|$)/.test(file)) {
    return 'local checkout, test workbench, dependency or build output';
  }
  if (/^(?:\.au3|\.audit2|\.audit_f3633|\.work_[^/]*)(?:\/|$)/.test(file) ||
      /^(?:01,W01p|02,W02p|03,W03p)$/.test(file) ||
      /^ports\/artemis\/(?:rewrite-staging|rewrite-fixes|build\/wabt|reverse\/build)(?:\/|$)/.test(file)) {
    return 'local rewrite, audit or downloaded-toolchain workspace';
  }
  if (/^ports\/artemis\/(?:reverse\/(?:\.build-dbg\.sh|build-bridge-route-o1tmp\.sh)|tools\/(?:apply_part34_arms\.py|check_part34_region\.py|rg2\.mjs|rg-full-console\.mjs))$/.test(file)) {
    return 'local diagnostic scratch script';
  }
  if (ASSET_EXCEPTIONS.has(file)) return null;
  if (/\.(?:wasm|wat|o|obj|so|dll|exe|zip|7z|tgz|pyc|pfs(?:\.\d+)?|xp3|nsa|ns2|sar|rpa)$/i.test(file) ||
      /\.(?:tar\.gz|log|tsbuildinfo)$/i.test(file)) {
    return 'game container, downloaded binary or generated output outside documented exceptions';
  }
  return null;
}

/** Last C++ mapping for an id wins; placeholder and header rows are not products. */
export function mappedProducts(source) {
  const products = new Map();
  source.split(/\r?\n/).forEach((line, index) => {
    if (!line.trim() || line.startsWith('#')) return;
    const cols = line.split('\t');
    if (cols[0] !== 'fn') return;
    if (cols.length < 5 || !/^f\d+$/.test(cols[1])) {
      throw new Error(`rewrite-names.tsv:${index + 1}: malformed function row`);
    }
    const relative = cols[4];
    if (!relative.endsWith('.cpp')) return;
    if (!/^[a-zA-Z0-9_/-]+\.cpp$/.test(relative) ||
        relative.startsWith('/') || relative.split('/').some((part) => part === '..')) {
      throw new Error(`rewrite-names.tsv:${index + 1}: unsafe product path ${relative}`);
    }
    const fnid = `f${BigInt(cols[1].slice(1))}`;
    products.set(fnid, `ports/artemis/src/${relative}`);
  });
  return products;
}

function runGit(root, args, { input, statuses = [0] } = {}) {
  const result = spawnSync('git', args, {
    cwd: root, input, encoding: 'utf8', maxBuffer: 32 * 1024 * 1024,
    stdio: ['pipe', 'pipe', 'pipe'],
  });
  if (result.error) throw result.error;
  if (!statuses.includes(result.status)) {
    throw new Error(`git ${args[0]} exited ${result.status}: ${(result.stderr ?? '').trim()}`);
  }
  return result.stdout;
}

/**
 * The shipping Artemis build: what compiles the module, and what the artifact has to carry.
 *
 * This is a source build of the upstream engine, not the SDK route. The two differences that
 * matter for a guard are that the build input is SOURCE at a pinned commit (so the pin has to be
 * a full commit hash, not a branch) and that the output is a permissive-licensed module (so the
 * artifact owes its own source offer, not just a notice).
 */
export const ARTEMIS_COMPAT = {
  key: 'artemis-compat',
  repository: 'https://github.com/Weiss-UltimateSavior/artemis-compat.git',
  workflow: '.github/workflows/build-wasm-artemis-compat.yml',
  buildScript: '.github/scripts/build-artemis-compat.sh',
  recipe: 'ports/artemis/compat-web/build.sh',
  bundleGenerator: 'ports/artemis/compat-web/make-source-bundle.mjs',
  path: '.ref/artemis-compat',
  artifactDir: 'dist/artemis-compat',
  /** The publishable set, in the order the workflow lists it. */
  files: ['artemis-compat.js', 'artemis-compat.wasm', 'artemis-compat-source.tar.gz',
    'artemis-compat-source.tar.gz.sha256', 'LICENSE', 'THIRD_PARTY_NOTICES.md'],
};

/** The legacy SDK bridge. Required to exist, forbidden to be presented as the shipping path. */
export const ARTEMIS_BRIDGE = {
  key: 'artemis-bridge',
  workflow: '.github/workflows/build-wasm-artemis-bridge.yml',
  buildScript: '.github/scripts/build-artemis-bridge.sh',
};

/**
 * The audio sources the source bundle has to carry, and why they cannot come from the pin.
 *
 * The pinned upstream tree has no web audio backend: `src/audio/audio.cpp` is the non-Android
 * stub, and the web target compiles `overlay/audio/audio_web.cpp` in its place. `audio_web.h` is
 * the seam the host and that file share, and `audio_validation.h` is the argument grammar the
 * host applies to the page's numbers. None of the three exists upstream, so none of them is in
 * the `git archive` the bundle copies: they travel only through the generator's entry list.
 *
 * This is the same class of silent hole as the Lua notice path below. A bundle missing one of
 * these still generates, still tars, and still looks complete; what it is missing is the ability
 * to rebuild the module it is offered beside.
 */
const COMPAT_BUNDLE_AUDIO_ENTRIES = [
  'overlay/audio/audio_web.cpp',
  'overlay/audio/audio_web.h',
  'overlay/audio/audio_validation.h',
];

/**
 * The recipe that builds the bundle *without* this repository.
 *
 * `ports/artemis/compat-web/build.sh` is the recipe CI runs, and it is deliberately not
 * relocated: it sources `ports/env.sh` and refuses to run unless its own path ends in
 * `ports/artemis/compat-web/out`. Neither the path nor `ports/env.sh` exists inside the bundle,
 * so shipping it alone gives a recipient a script that cannot run -- which is why the archive
 * carries a second, standalone recipe that reads the bundled sources directly.
 */
const COMPAT_STANDALONE_RECIPE = 'RUN-BUILD.sh';

/**
 * The three places that spell out the patch set, and how to read each one.
 *
 * The two recipes take their patch paths as `for` operands, so the manifest is the operand lines
 * and every patch in the set is named by one -- `RUN-BUILD.sh` also carries them in a
 * missing-file guard, which is the same list and therefore not read twice. The generator names one
 * patch per entry line. Measured over a corpus twice: once with the patch present, once with it
 * absent, so a checkout in which every patch is a new file still reports the same omissions
 * (a missing file is simply not in the set yet, and an undeclared one is what this is for).
 */
const COMPAT_PATCH_DECLARATIONS = [
  ['ports/artemis/compat-web/build.sh', /patches\/[\w.-]+\.patch/],
  ['ports/artemis/compat-web/RUN-BUILD.sh', /patches\/[\w.-]+\.patch/],
  // An entry, not a mention. The generator header comments the retired patch with the same
  // patches/NAME spelling, so a line match would count a comment as shipping it.
  ['ports/artemis/compat-web/make-source-bundle.mjs', /^\s*\['patches\/[\w.-]+\.patch'/],
];

/**
 * The platforms the minimal port is built from, and the manifest that says so.
 *
 * `ports/artemis/compat-web/PLATFORM-PATCHES.txt` is the explicit list. The retired file is
 * `0003-scene-checkpoint.patch` (save/restore), not `0003-tween-loopdelay.patch`. A checkout that
 * still holds an unlisted file must fail loudly rather than have a guard quietly admit it.
 *
 * The list is explicit on purpose. The previous version of this guard read the `for p in ...` loops
 * the recipes spell, which meant the set of patches CI compiles was whatever those loops happened to
 * contain -- the table was derived from the very thing it was supposed to constrain, and a third
 * patch could be added to `patches/` with every declaration still green.
 *
 * `candidates` is the checkout's own view (tracked plus visible), while `parse` is a text reader set
 * by the caller: worktree mode uses the filesystem, `--staged` uses `git show`, so the list is read
 * from the same view as everything else. It is never read from the candidate list, which would make
 * a file that was never committed able to declare itself.
 */
const COMPAT_PLATFORM_PATCH_LIST = 'ports/artemis/compat-web/PLATFORM-PATCHES.txt';

function compatPlatformPatches(candidates, hasFile, parse) {
  if (!hasFile(COMPAT_PLATFORM_PATCH_LIST)) return { names: [], problem:
    `${COMPAT_PLATFORM_PATCH_LIST}: missing; the platform patch set has to be named explicitly, `
    + `because a set inferred from the recipe's patch loop is the set the recipe already applies` };
  const names = parse(COMPAT_PLATFORM_PATCH_LIST).split(/\r?\n/)
    .map((line) => line.replace(/#.*$/, '').trim()).filter(Boolean);
  const dir = 'ports/artemis/compat-web/patches/';
  // The exact set, both directions. A named patch with no file stops the recipe's `patch` call; a
  // patch file the list does not name is the 0003 shape -- present, unapplied, and invisible.
  const problems = [];
  for (const name of names) {
    if (!hasFile(dir + name)) problems.push(`${dir}${name}: named in ${COMPAT_PLATFORM_PATCH_LIST} but missing`);
  }
  for (const file of candidates) {
    const base = file.startsWith(dir) && file.endsWith('.patch') ? file.slice(dir.length) : '';
    if (base && hasFile(file) && !names.includes(base)) {
      problems.push(`${file}: present in the checkout but named in neither ${COMPAT_PLATFORM_PATCH_LIST} `
        + `nor any patch declaration; unmanaged patches are the hole this contract closes`);
    }
  }
  // The patch set the non-platform test compares against, so it is compared once per file rather
  // than once per occurrence: a declaration names a patch in a guard list and again in its loop.
  const declared = [];
  for (const [manifest, pattern] of COMPAT_PATCH_DECLARATIONS) {
    if (!hasFile(manifest)) continue;
    for (const line of parse(manifest).split(/\r?\n/).map((l) => l.replace(/\\$/, '').trim())) {
      if (!pattern.test(line)) continue;
      for (const name of line.match(/[\w.-]+\.patch/g) ?? []) {
        if (names.includes(name) || declared.includes(name)) continue;
        if (hasFile(dir + name)) {
          declared.push(name);
          problems.push(`${manifest}: still applies ${name}. Current platform patches `
            + `${names.length} explicit (${names.join(', ')}); this patch is not one of them, so the `
            + `minimal port neither compiles it nor ships it -- retire the file or list it`);
        }
      }
    }
  }
  return { names, problem: problems.join('\n') || null };
}

/**
 * The managed patch set: the patch files this checkout holds, in whichever view is being read.
 *
 * Taken from the same tracked-plus-visible candidates the rest of the guard walks, then filtered by
 * `hasFile` so the set is what is actually there. Worktree mode therefore sees a patch that is on
 * disk but not yet added, and a deleted patch is absent from both views rather than lingering in
 * the index; the caller already computed the candidate list, and deriving it again from git would
 * disagree with the rest of this file.
 */
function compatPatchSet(candidates, hasFile) {
  const dir = 'ports/artemis/compat-web/patches/';
  return candidates.filter((name) => name.startsWith(dir) && /\.patch$/.test(name) && hasFile(name));
}

/**
 * Checks one Artemis record against the filesystem and the files that spell out its contract.
 * Returns the problems it found; it never throws, because a guard that dies on a malformed record
 * reports nothing about the rest of them.
 */
function artemisCompatProblems(content, hasFile, bucket, candidates) {
  const fail = (message) => bucket.push(message);
  const compat = ARTEMIS_COMPAT;
  const pinsFile = '.github/upstream-pins.json';
  if (!hasFile(pinsFile)) return;
  let pins;
  try {
    pins = JSON.parse(content(pinsFile));
  } catch (error) {
    fail(`${pinsFile}: not valid JSON (${error.message})`);
    return;
  }
  const record = pins[compat.key];
  if (!record || typeof record !== 'object') {
    fail(`${pinsFile}: missing the ${compat.key} record, which is what the shipping Artemis build is`);
    return;
  }

  // The pin is a commit, not a name: the recipe archives it by hash, and a branch would make the
  // "which upstream" question unanswerable after the fact.
  if (record.commit !== (record.commit ?? '').toString() || !/^[a-f0-9]{40}$/.test(record.commit ?? '')) {
    fail(`${pinsFile}: ${compat.key} needs a full 40-character lowercase commit, got ${JSON.stringify(record.commit)}`);
  }
  if (typeof record.repository !== 'string' || !record.repository.startsWith('https://github.com/')) {
    fail(`${pinsFile}: ${compat.key} needs a https://github.com/ repository`);
  } else if (record.repository !== compat.repository) {
    fail(`${pinsFile}: ${compat.key} repository is ${record.repository}, expected ${compat.repository}`);
  }
  if (typeof record.branch !== 'string' || !record.branch) {
    fail(`${pinsFile}: ${compat.key} needs a branch`);
  }

  for (const [key, expected] of [['workflow', compat.workflow], ['build-script', compat.buildScript],
    ['recipe', compat.recipe], ['artifact-dir', compat.artifactDir], ['path', compat.path]]) {
    if (record[key] !== expected) {
      fail(`${pinsFile}: ${compat.key} ${key} is ${JSON.stringify(record[key])}, expected ${expected}`);
    }
  }
  for (const key of ['workflow', 'build-script', 'recipe']) {
    if (typeof record[key] === 'string' && record[key].startsWith('ports/') || record[key]?.startsWith('.github/')) {
      if (!hasFile(record[key])) fail(`${pinsFile}: ${compat.key} ${key} does not exist: ${record[key]}`);
    }
  }

  // The publishable set: exactly the two modules, the source offer with its hash manifest and the
  // license pair. Checked as an exact list, so neither a missing file nor a fifth one passes.
  const files = record.files;
  if (!Array.isArray(files) || files.join(' ') !== compat.files.join(' ')) {
    fail(`${pinsFile}: ${compat.key} files must be exactly ${compat.files.join(' ')} (got ${JSON.stringify(files)})`);
  }
  // A shipping build that the drift check may dispatch would pair an unreviewed engine revision
  // with reviewed patches; a checkout the workflow does not actually fetch would archive a
  // different tree than the one it asserted. Both are contracts stated in the pins file.
  if (record.dispatch !== false) {
    fail(`${pinsFile}: ${compat.key} must set dispatch: false (an upstream move must not auto-start this build)`);
  }
  if (record['git-archive'] !== true) {
    fail(`${pinsFile}: ${compat.key} must set git-archive: true (the recipe archives the pin, it does not build the checkout)`);
  }

  for (const [key, expected] of [['workflow', compat.workflow], ['build-script', compat.buildScript],
    ['recipe', compat.recipe]]) {
    if (record[key] === expected && !hasFile(expected)) {
      fail(`${pinsFile}: ${compat.key} ${key} is missing from the tree: ${expected}`);
    }
  }

  // --- the patch set the source build applies ------------------------------
  //
  // A patch fixes a build input that exists nowhere upstream, so a shipping closure that does not
  // name it does not apply it: the module ships without the fix and the source offer cannot
  // rebuild it. Checking the two reviewed names in SOURCE_ENTRIES is not enough on its own,
  // because the next reviewed patch is by definition not in that list yet. So the manifests are
  // the requirement, not the source of truth: only a patch named in every one of them counts, and
  // a patch that is merely present is reported rather than admitted. Placed before the workflow
  // section because that one returns early on a missing workflow, and this check must not be
  // skipped for a reason that has nothing to do with the patch set.
  if (hasFile(ARTEMIS_COMPAT.recipe)) {
    // The explicit platform set first: it is the contract, and every declaration below is measured
    // against it rather than against the recipe's own patch loop.
    const platform = compatPlatformPatches(candidates, hasFile, content);
    if (platform.problem) for (const problem of platform.problem.split('\n')) fail(problem);
    if (!platform.names.length) {
      fail(`${COMPAT_PLATFORM_PATCH_LIST}: names no platform patch, and the minimal port applies at least one`);
    }
    const patchSet = compatPatchSet(candidates, hasFile);
    if (!patchSet.length) fail(`${ARTEMIS_COMPAT.recipe}: names no managed patch set, so this check has nothing to compare and fails closed`);
    for (const [file, pattern] of COMPAT_PATCH_DECLARATIONS) {
      if (!hasFile(file)) {
        fail(`${file}: missing (it is one of the patching closure; without it a new patch cannot be placed in review)`);
        continue;
      }
      // Folded lines are how the recipes hold a path list, and a patch is named by its file name,
      // so folding is whitespace rather than a way out of the manifest. The manifests spell the
      // name relative to this directory, so the file name is what has to appear.
      const manifest = content(file);
      const lines = manifest.split(/\r?\n/).map((line) => line.replace(/\\$/, '').trim());
      const readsList = /PLATFORM-PATCHES\.txt/.test(manifest);
      const declared = readsList
        ? platform.names.map((name) => `patches/${name}`)
        : lines.filter((line) => pattern.test(line));
      // What is required to be declared is the platform set, not "everything in `patches/`". A
      // retired patch such as 0003 may still sit in the checkout while the minimal port does not
      // apply it; demanding it be declared would force the recipes to keep applying it, which is
      // the opposite of retiring it. A patch that is neither named here nor by the platform list
      // is already reported by `compatPlatformPatches`, once, by name.
      for (const name of platform.names) {
        const patch = `ports/artemis/compat-web/patches/${name}`;
        if (declared.some((line) => line.includes(name))) continue;
        fail(`${patch}: platform patch named in none of ${file} `
          + `(its entry list is ${ARTEMIS_COMPAT.recipe}, RUN-BUILD.sh and make-source-bundle.mjs), `
          + `so the shipping build and the source offer would omit it; name it there or leave it unmanaged`);
      }
      // The other direction: a name the file spells that no patch file backs stops the build at
      // patch time. Read from the whole file rather than the operand lines, because the generator
      // has no existence check of its own and a typo there is a silently smaller bundle.
      //
      // Comments are stripped first. The declarations are prose-heavy on purpose -- each one says
      // in words which patches it applies and why -- and a comment that names a RETIRED patch in
      // order to say it is *not* applied is documentation, not an application. Without this, the
      // honest note is reported as if the file had asked for a patch that is not there, and the
      // only way to keep the gate green would be to delete the explanation.
      const code = manifest.split(/\r?\n/)
        .map((line) => line.replace(/(^|\s)(#|\/\/|\*).*$/, '$1'))
        .join('\n');
      for (const name of code.match(/[\w.-]+\.patch/g) ?? []) {
        if (!patchSet.includes(`ports/artemis/compat-web/patches/${name}`)) {
          fail(`${file}: names ${name}, which is not a managed patch file`);
        }
      }
    }
    // The set the recipes apply has to be the platform set. Where a declaration still names a
    // retired patch, `compatPlatformPatches` reports it once per file, with the platform count, so
    // a failed writer release shows up as a named mismatch rather than a silent pass.
    if (patchSet.length < platform.names.length) {
      fail(`${ARTEMIS_COMPAT.recipe}: managed patch set shrank to ${patchSet.length}, below the `
        + `${platform.names.length} the platform list names`);
    }
  }

  // --- the workflow this record names -------------------------------------
  //
  // Not a YAML parser: this guard runs without dependencies, and what it needs to know is which
  // inputs the caller passes, which it can read the same way GitHub reads it -- textually. The
  // shape is a single reusable-workflow job, so each `key: value` line under `with:` is one input.
  if (!hasFile(compat.workflow)) {
    fail(`${compat.workflow}: missing`);
    return;
  }
  const workflow = content(compat.workflow);
  const inputs = new Map();
  const match = workflow.match(/\n\s*with:\n/);
  if (!match) {
    fail(`${compat.workflow}: no reusable-workflow \`with:\` block, so it passes no build inputs`);
  } else {
    for (const line of workflow.slice(match.index + match[0].length).split(/\r?\n/)) {
      if (/^\s*$/.test(line)) break;
      // Comments between inputs are how the contract is explained, so they are skipped rather
      // than treated as the end of the block. An input's value may itself span lines (a folded
      // `>-` string), so a line that is neither a comment nor a `key:` continues the previous one.
      if (/^\s*#/.test(line)) continue;
      const entry = line.match(/^\s{6,}([a-z][a-z0-9-]*):[ \t]*(.*)$/);
      if (!entry) {
        const last = [...inputs.keys()].pop();
        if (!last || !/^\s{8,}\S/.test(line)) break;
        inputs.set(last, `${inputs.get(last)} ${line.trim()}`);
        continue;
      }
      // A folded scalar (`>-`) is a YAML marker, not part of the value; the continuation lines
      // above are appended after it without the leading space a naive join would leave.
      inputs.set(entry[1], entry[2].trim().replace(/^>-?$/, ''));
    }
    for (const [key, value] of inputs) inputs.set(key, value.trim());
  }
  const expect = (key, value) => {
    if (!inputs.has(key)) fail(`${compat.workflow}: does not pass an input named ${key}`);
    else if (inputs.get(key) !== value) {
      fail(`${compat.workflow}: ${key} is ${JSON.stringify(inputs.get(key))}, expected ${JSON.stringify(value)}`);
    }
  };
  expect('engine', compat.key);
  expect('build-script', compat.buildScript);
  expect('upstream-repository', compat.repository);
  expect('upstream-path', record.path ?? compat.path);
  expect('artifact-dir', compat.artifactDir);
  expect('artifact-name', record['artifact-name'] ?? 'wasm-artemis-compat');
  if (inputs.get('publish') !== 'false') {
    fail(`${compat.workflow}: publish must be false until a real game boots these bytes`);
  }
  if (!inputs.has('package-dir') || inputs.has('package-engine-assets') === false) {
    fail(`${compat.workflow}: the runtime tarball must carry this run's modules`);
  }
  // The worker is a source module beside a page, not the SDK glue, so the committed import map
  // (which is the SDK module's import surface) must not be compared against it.
  if (!inputs.has('glue-import-map') || inputs.get('glue-import-map') !== "''") {
    fail(`${compat.workflow}: glue-import-map must be left empty; the committed table is the SDK module's import surface`);
  }
  // The checkout commit and the pin the recipe archives have to be the SAME reviewed literal.
  // Two spellings in one file is the shape that goes wrong: an override or a half-edit fetches one
  // commit while the recipe archives another, and the run stays green while the artifact's source
  // tarball, manifest and PIN file name a commit that was never compiled. The build script asserts
  // this at run time; checking it here is what makes it a review-time failure instead.
  const commit = inputs.get('upstream-commit');
  if (commit === undefined) {
    fail(`${compat.workflow}: does not pass upstream-commit, so the checkout has no pinned commit`);
  } else if (commit !== `'${record.commit}'`) {
    fail(`${compat.workflow}: upstream-commit is ${JSON.stringify(commit)}, expected the reviewed pin `
      + `'${record.commit}' as a literal; an expression or override can fetch a different commit than `
      + `the ARTC_PIN the recipe archives, which mislabels the artifact's source`);
  }
  // The source pin reaches the build script as `ARTC_PIN` in the build step's shell (see the
  // reusable workflow), or as `ARTC_REF` pointing at the checkout that holds it. One of the two
  // has to be there, or the recipe falls back to its own default pin and the artifact stops being
  // traceable to the record above.
  const buildEnv = inputs.get('build-env') ?? '';
  if (!/\bARTC_(PIN|REF)=/.test(buildEnv)) {
    fail(`${compat.workflow}: build-env carries neither ARTC_PIN nor ARTC_REF, so the build script `
      + `cannot name the pin it built (got ${JSON.stringify(buildEnv)})`);
  } else if (!buildEnv.includes(`ARTC_PIN=${record.commit}`)) {
    fail(`${compat.workflow}: build-env must carry ARTC_PIN=${record.commit}; the recipe archives that `
      + `value, so anything else ships a source offer for a commit the modules were not built from`);
  }
  // --- the build script ----------------------------------------------------
  if (!hasFile(compat.buildScript)) {
    fail(`${compat.buildScript}: missing`);
    return;
  }
  const script = content(compat.buildScript);
  for (const [what, pattern] of [
    ['the tracked recipe', /ports\/artemis\/compat-web\/build\.sh/],
    ['the source-offer generator', /make-source-bundle\.mjs/],
    ['the artifact directory', new RegExp(compat.artifactDir.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'))],
    ['the pinned upstream checkout', /UPSTREAM_PATH/],
    // The wrapper has to bind the workflow's checkout commit to the pin the recipe archives.
    // Without it the two spellings can disagree and the run still succeeds, which is how an
    // artifact ends up carrying a source offer for a commit it was not built from.
    ['the upstream commit the workflow checked out', /UPSTREAM_COMMIT/],
  ]) {
    if (!pattern.test(script)) fail(`${compat.buildScript}: does not name ${what} (${pattern})`);
  }

  // --- the source offer -----------------------------------------------------
  //
  // GPL-3.0 owes the "preferred form for making modifications". Upstream ships no `build.sh`, no
  // `ports/` and no web CMake target, so the patch set and overlays are only a build input
  // *through* the recipe: a bundle without it is a pile of files with no documented way back to
  // the binary. Checked as a property of the generator's fixed entry list, not by running it.
  if (!hasFile(compat.bundleGenerator)) {
    fail(`${compat.bundleGenerator}: missing`);
  } else {
    const bundle = content(compat.bundleGenerator);
    for (const [what, pattern] of [
      ['the tracked build recipe', /\['build\.sh',\s*'build\.sh'\]/],
      ['the upstream GPL text', /\['LICENSE',\s*'LICENSE'\]/],
      // `third_party/lua51/` is the CMake target name; the vendored tree is `lua-5.1.5/`. The
      // wrong path silently ships no Lua notice, because a copy failure is not a build failure.
      ['the Lua 5.1 notice at its real upstream path',
        /third_party\/lua-5\.1\.5\/COPYRIGHT/],
    ]) {
      if (!pattern.test(bundle)) {
        fail(`${compat.bundleGenerator}: does not carry ${what} (${pattern})`);
      }
    }
    if (/lua51\/COPYRIGHT/.test(bundle)) {
      fail(`${compat.bundleGenerator}: reads third_party/lua51/, which does not exist upstream `
        + `(the tree is third_party/lua-5.1.5/); the bundle would ship no Lua notice`);
    }

    // --- the audio sources the bundle has to carry ---------------------------
    //
    // The audio backend is not upstream: `audio_web.cpp` is compiled *instead of* the pinned
    // `src/audio/audio.cpp`, and `audio_validation.h` is the argument grammar the host applies.
    // The bundle copies `src/` from the pinned archive (which contains neither) and then a fixed
    // list of tracked files, so an entry missing from that list is a source offer whose audio
    // sources are absent -- the recipient could not rebuild the module beside it.
    //
    // Checked against the generator's entry list, not by running it: the list is what decides
    // what ships, and a copy failure at generation time is silent.
    for (const file of COMPAT_BUNDLE_AUDIO_ENTRIES) {
      if (!bundle.includes(`'${file}'`)) {
        fail(`${compat.bundleGenerator}: does not carry ${file}; the audio backend is a local `
          + `overlay, so a bundle without it ships a source offer that cannot rebuild the module`);
      }
    }
    // The standalone recipe is what makes the archive actionable off this repository: the
    // tracked `build.sh` refuses to run outside `ports/artemis/compat-web/out` and sources
    // `ports/env.sh`, neither of which exists in the bundle.
    if (!bundle.includes(`'${COMPAT_STANDALONE_RECIPE}', '${COMPAT_STANDALONE_RECIPE}'`)) {
      fail(`${compat.bundleGenerator}: does not carry ${COMPAT_STANDALONE_RECIPE}; the bundled `
        + `recipe sources ports/env.sh and refuses an out-of-tree layout, so the bundle would `
        + `have no documented way to build without this repository`);
    }
  }


  // --- the workflow's own shape -------------------------------------------
  if (/\n\s*push:\s*\n/.test(workflow) === false && !/^on:\s*$/m.test(workflow)) {
    fail(`${compat.workflow}: no \`on:\` trigger block`);
  }
  // The recipe deletes and recreates directories under its own `out/`; a build input that pointed
  // into a checked-out upstream tree would put that deletion somewhere the repository does not
  // control. Stated as a property of the recipe rather than by running it.
  if (!hasFile(compat.recipe)) fail(`${compat.recipe}: missing`);
}

export function inspectSources(root, { staged = false } = {}) {
  const git = (args, options) => runGit(root, args, options);
  const list = (args) => git(args).split('\0').filter(Boolean);
  const tracked = new Set(list(['ls-files', '--cached', '-z']));
  const candidates = staged ? [...tracked] : [...new Set([
    ...tracked, ...list(['ls-files', '--others', '--exclude-standard', '-z']),
  ])];
  const errors = [];
  const warnings = [];
  const pending = new Set();
  const hasFile = (file) => staged ? tracked.has(file) :
    existsSync(join(root, file)) && statSync(join(root, file)).isFile();
  const content = (file) => staged ? git(['show', `:${file}`]) :
    readFileSync(join(root, file), 'utf8');
  const requireFile = (file) => {
    if (!hasFile(file)) {
      errors.push(`${file}: missing from ${staged ? 'index' : 'working tree'}`);
      return false;
    }
    if (!tracked.has(file)) pending.add(file);
    return true;
  };

  for (const file of SOURCE_ENTRIES) requireFile(file);
  for (const file of candidates) {
    const reason = uploadProblem(file);
    if (reason) errors.push(`${file}: ${reason}`);
  }
  if (git(['ls-files', '--unmerged', '-z']).length) {
    return { errors: [...errors, 'Git index has unresolved conflicts'], warnings,
      products: 0, productFiles: 0, pending: [...pending] };
  }

  const mapFile = 'ports/artemis/rewrite-names.tsv';
  const products = hasFile(mapFile) ? mappedProducts(content(mapFile)) : new Map();
  if (products.size === 0) errors.push('Artemis source ledger has no C++ products');
  const paths = [...new Set(products.values())];
  for (const file of paths) requireFile(file);
  if (!staged && paths.length) {
    // check-ignore returns 1 when nothing matches: that is a successful clean check.
    const ignored = git(['check-ignore', '--no-index', '--stdin', '-z'], {
      input: paths.join('\0') + '\0', statuses: [0, 1],
    });
    for (const file of ignored.split('\0').filter(Boolean)) errors.push(`${file}: product is ignored`);
  }

  const manifestFile = 'package.json';
  if (hasFile(manifestFile)) {
    const scripts = JSON.parse(content(manifestFile)).scripts;
    if (scripts?.['check:sources'] !== 'node scripts/check-sources.mjs' ||
        scripts?.['test:sources'] !== 'node --test test/check-sources.test.mjs') {
      errors.push(`${manifestFile}: missing source-check or regression-test entry`);
    }
  }
  for (const name of ['runtime-ons', 'runtime-kag', 'runtime-artemis']) {
    const file = `packages/${name}/package.json`;
    if (!requireFile(file)) continue;
    const pkg = JSON.parse(content(file));
    if (pkg.name !== `@online-gal/${name}` || pkg.private !== true ||
        pkg.license !== 'GPL-3.0-or-later') {
      errors.push(`${file}: runtime name/private/GPL-3.0-or-later declaration changed`);
    }
  }

  const pinsFile = '.github/upstream-pins.json';
  const hasPins = hasFile(pinsFile);
  if (hasPins) {
    let pins = null;
    try {
      pins = JSON.parse(content(pinsFile));
    } catch (error) {
      errors.push(`${pinsFile}: not valid JSON (${error.message})`);
    }
    if (pins) {
      if (!pins.emsdk?.version) errors.push(`${pinsFile}: missing emsdk version`);
      for (const name of ['ons', 'krkr']) {
        const engine = pins.engines?.[name];
        if (!engine || !/^[a-f0-9]{40}$/.test(engine.commit ?? '') ||
            !engine.repository?.startsWith('https://github.com/')) {
          errors.push(`${pinsFile}: invalid ${name} pin`);
          continue;
        }
        for (const key of ['build-script', 'workflow']) {
          if (typeof engine[key] !== 'string' || !engine[key].startsWith('.github/')) {
            errors.push(`${pinsFile}: invalid ${name} ${key}`);
          } else requireFile(engine[key]);
        }
      }
      // FORBIDDEN: the SDK bridge must not be dispatched or triggered automatically. Its own
      // record and workflow are still required above, but as legacy / diagnostic material.
      const bridgeTrigger = hasFile(ARTEMIS_BRIDGE.workflow)
        ? content(ARTEMIS_BRIDGE.workflow).match(/\r?\non:\r?\n([\s\S]*)/)
        : null;
      if (!bridgeTrigger) {
        errors.push(`${ARTEMIS_BRIDGE.workflow}: no \`on:\` trigger block`);
      } else {
        // Only the keys at the `on:` level count: a nested `push:` under some other block is not a
        // trigger, and a comment that explains the removal must not read as one.
        const keys = bridgeTrigger[1].split(/\r?\n/)
          .filter((line) => /^ {2}\S/.test(line))
          .map((line) => line.trim().replace(/:.*$/, ''));
        for (const forbidden of ['push', 'pull_request', 'schedule']) {
          if (keys.includes(forbidden)) {
            errors.push(`${ARTEMIS_BRIDGE.workflow}: is manual/diagnostic only, so its \`on:\` block ` +
              `must not contain ${forbidden}; the automatic SDK rebuild is what this replaced`);
          }
        }
        if (!keys.includes('workflow_dispatch')) {
          errors.push(`${ARTEMIS_BRIDGE.workflow}: must stay dispatchable by hand (workflow_dispatch)`);
        }
      }
      for (const [key, expected] of [['target', 'asyncify'],
        ['module', 'ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm']]) {
        if (pins['artemis-bridge']?.[key] !== expected) {
          errors.push(`${pinsFile}: the legacy artemis-bridge ${key} changed; it is diagnostic only ` +
            `and not the shipping Artemis path (expected ${expected})`);
        }
      }
      if (pins['artemis-bridge']?.module) requireFile(pins['artemis-bridge'].module);
    }
  }
  if (hasPins) artemisCompatProblems(content, hasFile, errors, candidates);

  if (pending.size) warnings.push(`${pending.size} required files exist but are not in the index:\n` +
    [...pending].map((file) => `  ${file}`).join('\n'));
  warnings.push('Retained Artemis SDK and derived products require separate redistribution review; ' +
    'this guard does not establish licensing or game/package acceptance.');
  return { errors, warnings, products: products.size, productFiles: paths.length, pending: [...pending] };
}

function main() {
  const args = process.argv.slice(2);
  if (args.some((arg) => arg !== '--staged')) {
    console.error('usage: node scripts/check-sources.mjs [--staged]');
    process.exitCode = 2;
    return;
  }
  const staged = args.includes('--staged');
  const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
  try {
    const result = inspectSources(root, { staged });
    for (const warning of result.warnings) console.warn(`source warning: ${warning}`);
    for (const error of result.errors) console.error(`source error: ${error}`);
    console.log(`sources ${result.errors.length ? 'FAILED' : 'ok'} (${staged ? 'index' : 'worktree'}): ` +
      `${result.products} mapped functions, ${result.productFiles} product files`);
    if (result.errors.length) process.exitCode = 1;
  } catch (error) {
    console.error(`source check failed: ${error.message}`);
    process.exitCode = 1;
  }
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) main();
