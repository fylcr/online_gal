import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import {
  mkdirSync, mkdtempSync, readFileSync, realpathSync, rmSync, writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import { inspectSources, mappedProducts, SOURCE_ENTRIES, uploadProblem } from '../scripts/check-sources.mjs';

const MAP = 'ports/artemis/rewrite-names.tsv';
const INPUT = 'ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm';
const PRODUCT = 'ports/artemis/src/util/first.cpp';
const COMPAT_PIN = '9f622629f595db85e6f76c45c06edde29d7d9961';
const COMPAT_WORKFLOW = '.github/workflows/build-wasm-artemis-compat.yml';
const BRIDGE_WORKFLOW = '.github/workflows/build-wasm-artemis-bridge.yml';
const CHECKER = fileURLToPath(new URL('../scripts/check-sources.mjs', import.meta.url));
const row = (id, file) => `fn\t${id}\t—\tartc::Fixture\t${file}\tR1\tfixture\n`;
const env = { ...process.env };
for (const key of ['GIT_DIR', 'GIT_WORK_TREE', 'GIT_INDEX_FILE', 'GIT_OBJECT_DIRECTORY',
  'GIT_ALTERNATE_OBJECT_DIRECTORIES', 'GIT_CONFIG_PARAMETERS', 'GIT_CONFIG_COUNT']) delete env[key];

/**
 * The shipping Artemis record, spelled the way the pins file and the workflow spell it. Written
 * out rather than imported so that a change to the guard's expectation cannot silently redefine
 * the fixture it is tested against.
 */
function compatRecord() {
  return {
    repository: 'https://github.com/Weiss-UltimateSavior/artemis-compat.git',
    branch: 'main',
    commit: COMPAT_PIN,
    path: '.ref/artemis-compat',
    workflow: COMPAT_WORKFLOW,
    'build-script': '.github/scripts/build-artemis-compat.sh',
    recipe: 'ports/artemis/compat-web/build.sh',
    'artifact-name': 'wasm-artemis-compat',
    'artifact-dir': 'dist/artemis-compat',
    files: ['artemis-compat.js', 'artemis-compat.wasm', 'artemis-compat-source.tar.gz',
      'artemis-compat-source.tar.gz.sha256', 'LICENSE', 'THIRD_PARTY_NOTICES.md'],
    dispatch: false,
    'git-archive': true,
  };
}

/** A reusable-workflow caller fixture: the `with:` block is what the guard reads. */
function compatWorkflow(overrides = {}) {
  const inputs = {
    engine: 'artemis-compat',
    'build-script': '.github/scripts/build-artemis-compat.sh',
    'upstream-repository': 'https://github.com/Weiss-UltimateSavior/artemis-compat.git',
    'upstream-branch': 'main',
    'upstream-commit': `'${COMPAT_PIN}'`,
    'upstream-path': '.ref/artemis-compat',
    'skip-upstream-checkout': 'false',
    'artifact-name': 'wasm-artemis-compat',
    'artifact-dir': 'dist/artemis-compat',
    'artifact-files': 'artemis-compat.js artemis-compat.wasm',
    'build-env': `ARTC_PIN=${COMPAT_PIN}`,
    version: 'compat-9f62262',
    'package-dir': 'packages/runtime-artemis',
    'package-engine-assets': 'true',
    publish: 'false',
    'glue-import-map': "''",
    ...overrides,
  };
  const lines = Object.entries(inputs)
    .filter(([, value]) => value !== undefined)
    .map(([key, value]) => `      ${key}: ${value}`)
    .join('\n');
  return `name: fixture\non:\n  push:\n    branches: [main]\n\njobs:\n  compat:\n    uses: ./.github/workflows/build-wasm-reusable.yml\n    with:\n${lines}\n`;
}

const COMPAT_SCRIPT = [
  '#!/usr/bin/env bash',
  'set -euo pipefail',
  ': "${UPSTREAM_PATH:?}"',
  ': "${UPSTREAM_COMMIT:?}"',
  'bash ports/artemis/compat-web/build.sh -j 2',
  'node ports/artemis/compat-web/make-source-bundle.mjs --out dist/artemis-compat',
].join('\n');

/**
 * The source-offer generator fixture: only the entry lists matter to the guard, which reads them
 * as text rather than running the generator (it needs an extracted upstream tree and a real tar).
 */
const COMPAT_BUNDLE = [
  "export const EXTRA_ENTRIES = [",
  "  ['README.md', 'SOURCE-BUNDLE.md'],",
  "  ['build.sh', 'build.sh'],",
  // The standalone recipe and the three audio overlay sources. All four are build inputs that
  // exist nowhere upstream, so the guard requires them in the generator's entry list; a fixture
  // without them would fail every test that expects a clean bundle.
  "  ['RUN-BUILD.sh', 'RUN-BUILD.sh'],",
  "  ['patches/0001-pf8-readat-host-source.patch', 'patches/0001-pf8-readat-host-source.patch'],",
  "  ['overlay/audio/audio_web.cpp', 'overlay/audio/audio_web.cpp'],",
  "  ['overlay/audio/audio_web.h', 'overlay/audio/audio_web.h'],",
  "  ['overlay/audio/audio_validation.h', 'overlay/audio/audio_validation.h'],",
  "];",
  "export const UPSTREAM_ENTRIES = [",
  "  ['LICENSE', 'LICENSE'],",
  "  ['third_party/lua-5.1.5/COPYRIGHT', 'third_party/lua-5.1.5/COPYRIGHT'],",
  "];",
].join('\n');

const BRIDGE_YML = `name: legacy\non:\n  workflow_dispatch:\n    inputs:\n      publish:\n        type: boolean\n\njobs:\n  bridge:\n    uses: ./.github/workflows/build-wasm-reusable.yml\n`;

function command(executable, args, { cwd, input } = {}) {
  const result = spawnSync(executable, args, {
    cwd, input, env, encoding: 'utf8', stdio: ['pipe', 'pipe', 'pipe'],
  });
  if (result.error) throw result.error;
  assert.equal(result.status, 0, `${executable} ${args.join(' ')}: ${result.stderr}`);
  return result.stdout;
}

function fixture(t) {
  const parent = realpathSync(tmpdir());
  const root = realpathSync(mkdtempSync(join(parent, 'online-gal-sources-')));
  t.after(() => {
    // Only remove the exact directory this fixture created, never a supplied path.
    assert.equal(dirname(root), parent);
    assert.match(basename(root), /^online-gal-sources-/);
    assert.equal(realpathSync(root), root);
    rmSync(root, { recursive: true, force: false });
  });
  const put = (file, content = 'fixture\n') => {
    const target = resolve(root, file);
    assert.ok(target.startsWith(root + '/'.replace('/', process.platform === 'win32' ? '\\' : '/')));
    mkdirSync(dirname(target), { recursive: true });
    writeFileSync(target, content);
  };
  const git = (...args) => command('git', args, { cwd: root });
  git('init', '--quiet');
  for (const file of SOURCE_ENTRIES) put(file);
  put('.gitignore', '.ref/\n.test/\n.au3/\nnode_modules/\nout/\n');
  put('package.json', JSON.stringify({ scripts: {
    'check:sources': 'node scripts/check-sources.mjs',
    'test:sources': 'node --test test/check-sources.test.mjs',
  } }));
  for (const name of ['runtime-ons', 'runtime-kag', 'runtime-artemis']) {
    put(`packages/${name}/package.json`, JSON.stringify({
      name: `@online-gal/${name}`, private: true, license: 'GPL-3.0-or-later',
    }));
  }
  const pins = {
    emsdk: { version: '6.0.9' },
    engines: Object.fromEntries(['ons', 'krkr'].map((name) => [name, {
      repository: `https://github.com/example/${name}.git`, commit: 'a'.repeat(40),
      'build-script': `.github/scripts/build-${name}.sh`,
      workflow: `.github/workflows/build-wasm-${name}.yml`,
    }])),
    'artemis-compat': compatRecord(),
    'artemis-bridge': { target: 'asyncify', module: INPUT },
  };
  put('.github/upstream-pins.json', JSON.stringify(pins));
  // The shipping Artemis build's contract files. A fixture that omitted them would pass the guard
  // for the wrong reason: every one of these is a path the guard is supposed to require.
  put(COMPAT_WORKFLOW, compatWorkflow());
  put(BRIDGE_WORKFLOW, BRIDGE_YML);
  put('.github/scripts/build-artemis-compat.sh', COMPAT_SCRIPT);
  put('.github/scripts/build-artemis-bridge.sh');
  put('ports/artemis/compat-web/build.sh');
  put('ports/artemis/compat-web/make-source-bundle.mjs', COMPAT_BUNDLE);
  put(MAP, row('f1', 'util/first.cpp') + row('f2', 'script/second.cpp') + row('f3', '—'));
  put(PRODUCT);
  put('ports/artemis/src/script/second.cpp');
  put(INPUT, '');
  git('add', '--all');
  return { root, put, git, pins, check: (options) => inspectSources(root, options) };
}

const forbidden = [
  '.ref/engine/source.cpp', '.test/game/system.ini', 'node_modules/module/index.js',
  'dist/krkr/index.js', 'ports/ons/out/engine.js', 'ports/krkr/build-final/index.js',
  'ports/artemis/tools/__pycache__/audit.pyc', '.vcpkg-binary-cache/archive.txt',
  '.au3/note.md', '.audit2/report.md', '.audit_f3633/result.txt', '.work_f3785/part.cpp',
  '01,W01p', '02,W02p', '03,W03p', 'ports/artemis/rewrite-staging/util/part.cpp',
  'ports/artemis/rewrite-fixes/fix.cpp', 'ports/artemis/build/wabt/source.c',
  'ports/artemis/reverse/build/zig/tool.txt', 'ports/artemis/reverse/.build-dbg.sh',
  'ports/artemis/reverse/build-bridge-route-o1tmp.sh', 'ports/artemis/tools/apply_part34_arms.py',
  'ports/artemis/tools/check_part34_region.py', 'ports/artemis/tools/rg2.mjs',
  'ports/artemis/tools/rg-full-console.mjs', 'game/root.pfs', 'game/root.pfs.001',
  'game/data.XP3', 'game/arc.nsa', 'game/arc.ns2', 'game/arc.sar', 'game/data.rpa',
  'engine.wasm', 'engine.wat', 'engine.o', 'engine.obj', 'engine.so', 'engine.dll',
  'tool.exe', 'assets.zip', 'assets.7z', 'package.tgz', 'package.tar.gz', 'build.log',
  'project.tsbuildinfo',
];
for (const file of forbidden) {
  test(`upload policy rejects ${file}`, () => assert.ok(uploadProblem(file)));
}

const allowed = [
  'ports/ons/build/run-web-build-shimmed.sh', 'ports/krkr/kirikiroid2/overlay-ports/gettext/portfile.cmake',
  'ports/artemis/src/render/compositor_hide.cpp', 'ports/artemis/reverse/rw-splice.py',
  'ports/artemis/reverse/test-rw-splice-diagnostics.py', 'ports/artemis/tools/assemble-rewrite-parts.py',
  'test/check-sources.test.mjs', 'docs/source-layout.md',
  'packages/runtime-ons/assets/onsyuri.wasm',
  'ports/artemis/vendor/artemis-sdk/WebAssembly/artemis.wasm', INPUT,
  'ports/artemis/vendor/artemis-sdk/Android/arm64-v8a/libartemis.so',
  'ports/artemis/vendor/artemis-sdk/Android/armeabi-v7a/libartemis.so',
];
for (const file of allowed) {
  test(`upload policy preserves ${file}`, () => assert.equal(uploadProblem(file), null));
}
test('asset exceptions do not permit arbitrary vendor binaries', () => {
  assert.ok(uploadProblem('ports/artemis/vendor/artemis-sdk/extra.wasm'));
  assert.ok(uploadProblem('packages/runtime-ons/assets/extra.wasm'));
});

test('ledger uses numeric function identity and the last C++ mapping', () => {
  const products = mappedProducts('# fixture\n' + row('f001', 'util/old.cpp') +
    row('f1', 'util/new.cpp') + row('f2', 'util/context.h') + row('f3', '—') +
    'rt\tstate\tname\tutil/context.h\tfixture\n');
  assert.deepEqual([...products], [['f1', 'ports/artemis/src/util/new.cpp']]);
});
test('a later placeholder or header does not erase an effective product', () => {
  assert.equal(mappedProducts(row('f1', 'util/first.cpp') + row('f1', '—') +
    row('f1', 'util/context.h')).get('f1'), PRODUCT);
});
test('ledger rejects malformed function rows', () => {
  for (const source of ['fn\tfno\tx\ty\tutil/a.cpp', 'fn\tf1\tx']) {
    assert.throws(() => mappedProducts(source), /malformed function row/);
  }
});
test('ledger rejects absolute, parent-traversal and backslash product paths', () => {
  for (const file of ['/util/a.cpp', '../util/a.cpp', 'util/../../a.cpp', 'D:/a.cpp', 'util\\a.cpp']) {
    assert.throws(() => mappedProducts(row('f1', file)), /unsafe product path/);
  }
});

test('clean fixtures pass worktree and staged checks without changing the index', (t) => {
  const f = fixture(t);
  const index = readFileSync(join(f.root, '.git/index'));
  const status = f.git('status', '--porcelain');
  for (const staged of [false, true]) {
    const result = f.check({ staged });
    assert.deepEqual(result.errors, []);
    assert.equal(result.products, 2);
    assert.equal(result.productFiles, 2);
    assert.deepEqual(result.pending, []);
    assert.ok(result.warnings.some((warning) => warning.includes('redistribution review')));
  }
  assert.deepEqual(readFileSync(join(f.root, '.git/index')), index);
  assert.equal(f.git('status', '--porcelain'), status);
});
test('untracked required navigation passes worktree with a warning but fails index mode', (t) => {
  const f = fixture(t);
  f.git('rm', '--cached', 'docs/source-layout.md');
  assert.deepEqual(f.check().errors, []);
  assert.ok(f.check().pending.includes('docs/source-layout.md'));
  assert.ok(f.check({ staged: true }).errors.some((error) => error.includes('docs/source-layout.md: missing')));
});
test('a staged ledger cannot hide an unstaged new product', (t) => {
  const f = fixture(t);
  f.put(MAP, row('f1', 'util/first.cpp') + row('f4', 'render/new.cpp'));
  f.put('ports/artemis/src/render/new.cpp');
  f.git('add', MAP);
  assert.deepEqual(f.check().errors, []);
  assert.ok(f.check().pending.includes('ports/artemis/src/render/new.cpp'));
  assert.ok(f.check({ staged: true }).errors.some((error) => error.includes('render/new.cpp: missing')));
});
test('missing mapped products fail closed instead of being silently skipped', (t) => {
  const f = fixture(t);
  f.put(MAP, row('f1', 'util/missing.cpp'));
  assert.ok(f.check().errors.some((error) => error.includes('util/missing.cpp: missing')));
  assert.deepEqual(f.check({ staged: true }).errors, []);
});
test('tracked product deletion is visible in worktree but not substituted into index mode', (t) => {
  const f = fixture(t);
  const target = resolve(f.root, PRODUCT);
  assert.equal(target, join(f.root, 'ports/artemis/src/util/first.cpp'));
  rmSync(target);
  assert.ok(f.check().errors.some((error) => error.includes('util/first.cpp: missing')));
  assert.deepEqual(f.check({ staged: true }).errors, []);
});
test('ignored products fail even when they were already tracked', (t) => {
  const f = fixture(t);
  f.put('.gitignore', 'ports/artemis/src/util/first.cpp\n');
  assert.ok(f.check().errors.some((error) => error.includes(`${PRODUCT}: product is ignored`)));
});
test('ignored game workbenches stay local, forced tracked ones fail', (t) => {
  const f = fixture(t);
  f.put('.test/game/system.ini');
  assert.deepEqual(f.check().errors, []);
  f.git('add', '--force', '.test/game/system.ini');
  assert.ok(f.check().errors.some((error) => error.startsWith('.test/game/system.ini:')));
  assert.ok(f.check({ staged: true }).errors.some((error) => error.startsWith('.test/game/system.ini:')));
});
test('visible new scratch and binary files are checked, not just tracked files', (t) => {
  const f = fixture(t);
  f.put('.audit2/report.md');
  f.put('game/data.xp3', '');
  const errors = f.check().errors;
  assert.ok(errors.some((error) => error.startsWith('.audit2/report.md:')));
  assert.ok(errors.some((error) => error.startsWith('game/data.xp3:')));
  assert.deepEqual(f.check({ staged: true }).errors, []);
});
test('staged package declarations are read from the index, not newer worktree bytes', (t) => {
  const f = fixture(t);
  f.put('packages/runtime-artemis/package.json', JSON.stringify({
    name: '@online-gal/runtime-artemis', private: false, license: 'MIT',
  }));
  assert.ok(f.check().errors.some((error) => error.includes('runtime-artemis/package.json: runtime')));
  assert.deepEqual(f.check({ staged: true }).errors, []);
});
test('root manifest must include the real check and regression-test commands', (t) => {
  const f = fixture(t);
  f.put('package.json', JSON.stringify({ scripts: { 'check:sources': 'echo passed' } }));
  assert.ok(f.check().errors.some((error) => error.includes('missing source-check')));
});
test('upstream commits and recipe paths must be valid', (t) => {
  const f = fixture(t);
  f.pins.engines.ons.commit = 'main';
  f.pins.engines.krkr.workflow = 'missing.yml';
  f.put('.github/upstream-pins.json', JSON.stringify(f.pins));
  const errors = f.check().errors;
  assert.ok(errors.some((error) => error.includes('invalid ons pin')));
  assert.ok(errors.some((error) => error.includes('invalid krkr workflow')));
});
test('Artemis requires the shipping compat record, its pin and its exact file set', (t) => {
  const f = fixture(t);
  delete f.pins['artemis-compat'];
  f.put('.github/upstream-pins.json', JSON.stringify(f.pins));
  assert.ok(f.check().errors.some((error) => error.includes('missing the artemis-compat record')));

  f.pins['artemis-compat'] = compatRecord();
  f.pins['artemis-compat'].commit = 'main';
  f.put('.github/upstream-pins.json', JSON.stringify(f.pins));
  assert.ok(f.check().errors.some((error) => error.includes('full 40-character lowercase commit')));

  f.pins['artemis-compat'] = compatRecord();
  f.pins['artemis-compat'].files = ['artemis-compat.js'];
  f.put('.github/upstream-pins.json', JSON.stringify(f.pins));
  assert.ok(f.check().errors.some((error) => error.includes('files must be exactly')));
});
test('Artemis refuses a shipping build that would auto-dispatch or build the checkout', (t) => {
  const f = fixture(t);
  f.pins['artemis-compat'] = { ...compatRecord(), dispatch: true, 'git-archive': false };
  f.put('.github/upstream-pins.json', JSON.stringify(f.pins));
  const errors = f.check().errors;
  assert.ok(errors.some((error) => error.includes('must set dispatch: false')));
  assert.ok(errors.some((error) => error.includes('must set git-archive: true')));
});
test('Artemis fails when the shipping workflow stops passing its build inputs', (t) => {
  const f = fixture(t);
  f.put(COMPAT_WORKFLOW, compatWorkflow({ 'build-script': undefined }));
  assert.ok(f.check().errors.some((error) => error.includes('does not pass an input named build-script')));

  f.put(COMPAT_WORKFLOW, compatWorkflow({ 'glue-import-map': 'ports/artemis/harness/glue/imports-map.json' }));
  assert.ok(f.check().errors.some((error) => error.includes('glue-import-map must be left empty')));

  f.put(COMPAT_WORKFLOW, compatWorkflow({ 'build-env': 'BRIDGE_FLAVOUR=asyncify' }));
  assert.ok(f.check().errors.some((error) => error.includes('carries neither ARTC_PIN nor ARTC_REF')));

  f.put(COMPAT_WORKFLOW, compatWorkflow({ publish: 'true' }));
  assert.ok(f.check().errors.some((error) => error.includes('publish must be false')));
});
test('Artemis refuses a checkout commit that is not the pin the recipe archives', (t) => {
  // The false-provenance shape, and the reason the dispatch override was removed: if the checkout
  // can name a different commit than the ARTC_PIN the recipe archives, the run still succeeds and
  // the artifact ships a source tarball, a manifest and a PIN file for a commit it was not built
  // from. All three variants below have to fail rather than pass green.
  const f = fixture(t);

  // A different literal commit.
  f.put(COMPAT_WORKFLOW, compatWorkflow({ 'upstream-commit': `'${'b'.repeat(40)}'` }));
  assert.ok(f.check().errors.some((error) => error.includes('expected the reviewed pin')),
    'a checkout commit other than the recorded pin must fail');

  // The old dispatch-override expression: any expression can name a commit at run time that the
  // reviewed file does not.
  f.put(COMPAT_WORKFLOW, compatWorkflow({
    'upstream-commit': `\${{ inputs.upstream_commit || '${COMPAT_PIN}' }}`,
  }));
  assert.ok(f.check().errors.some((error) => error.includes('expected the reviewed pin')),
    'an expression that can override the pin must fail, not be accepted as reviewable');

  // The pin present but unattached to the value the recipe archives.
  f.put(COMPAT_WORKFLOW, compatWorkflow({ 'build-env': `ARTC_PIN=${'c'.repeat(40)}` }));
  assert.ok(f.check().errors.some((error) => error.includes('build-env must carry ARTC_PIN')),
    'ARTC_PIN has to be the same commit as the checkout, not merely an ARTC_ pin');

  // And the corrected form passes, so the check above is not vacuous.
  f.put(COMPAT_WORKFLOW, compatWorkflow());
  assert.deepEqual(f.check().errors, []);
});
test('Artemis refuses a source offer that omits the build recipe or a licence', (t) => {
  // A bundle without `build.sh` is not the preferred form for making modifications: upstream has
  // no recipe, no `ports/` and no web CMake target, so the patches and overlays are only a build
  // input through that script. A bundle reading `third_party/lua51/` (the CMake target name)
  // instead of `third_party/lua-5.1.5/` (the vendored tree) ships no Lua notice at all.
  const f = fixture(t);
  const BUNDLE = 'ports/artemis/compat-web/make-source-bundle.mjs';

  f.put(BUNDLE, "const EXTRA_ENTRIES = [['README.md', 'SOURCE-BUNDLE.md']];\n");
  let errors = f.check().errors;
  assert.ok(errors.some((error) => error.includes('does not carry the tracked build recipe')),
    'a source offer without the recipe must fail');
  assert.ok(errors.some((error) => error.includes('does not carry the upstream GPL text')));
  assert.ok(errors.some((error) => error.includes('Lua 5.1 notice at its real upstream path')));

  f.put(BUNDLE, "const ENTRIES = [['build.sh', 'build.sh'], ['LICENSE', 'LICENSE'], "
    + "['third_party/lua51/COPYRIGHT', 'third_party/lua51/COPYRIGHT']];\n");
  errors = f.check().errors;
  assert.ok(errors.some((error) => error.includes('reads third_party/lua51/')),
    'the wrong Lua path is the failure that ships no notice, so it is named explicitly');
  // The presence check fires too, and that is correct rather than redundant: this stub has no
  // `lua-5.1.5/COPYRIGHT` entry AND reads a path that does not exist, and both facts are true of
  // the file. The point of the assertion above is that the *reason* is named, not that the other
  // check stays silent.
  assert.ok(errors.some((error) => error.includes('does not carry the Lua 5.1 notice')));
});
test('Artemis refuses a source offer that omits the audio overlay or the standalone recipe', (t) => {
  // The audio backend is the part of the shipping build that exists nowhere upstream: the pinned
  // tree's `src/audio/audio.cpp` is the non-Android stub, and the web target compiles
  // `overlay/audio/audio_web.cpp` in its place. The bundle copies `src/` from the pin, which
  // contains none of the overlay, so these three files reach the archive only through the
  // generator's entry list -- and a list missing one still generates and still tars.
  const f = fixture(t);
  const BUNDLE = 'ports/artemis/compat-web/make-source-bundle.mjs';

  // Drop each audio entry in turn from an otherwise-clean entry list. Each removal has to be
  // caught by name: a single generic "source offer incomplete" would not tell a reader which
  // build input went missing.
  for (const file of ['overlay/audio/audio_web.cpp', 'overlay/audio/audio_web.h',
    'overlay/audio/audio_validation.h']) {
    f.put(BUNDLE, COMPAT_BUNDLE.replace(new RegExp(`^\\s*\\['${file.replace(/[./]/g, '\\$&')}'.*\\n`, 'm'), ''));
    assert.ok(f.check().errors.some((error) => error.includes(`does not carry ${file}`)),
      `a bundle without ${file} must fail: it cannot rebuild the module beside it`);
  }

  // The standalone recipe. `build.sh` is shipped for its build logic but cannot run in the
  // archive (it sources `ports/env.sh` and refuses an out-of-tree root), so without this entry
  // the offer documents a build it cannot perform.
  f.put(BUNDLE, COMPAT_BUNDLE.replace(/^\s*\['RUN-BUILD\.sh'.*\n/m, ''));
  assert.ok(f.check().errors.some((error) => error.includes('does not carry RUN-BUILD.sh')),
    'a bundle with no runnable recipe must fail');

  // And the untouched fixture is clean, so the removals above are what caused each failure.
  f.put(BUNDLE, COMPAT_BUNDLE);
  assert.deepEqual(f.check().errors, []);
});
test('Artemis fails when the build script stops naming the recipe or the source offer', (t) => {
  const f = fixture(t);
  f.put('.github/scripts/build-artemis-compat.sh', '#!/usr/bin/env bash\nset -eu\n: "${UPSTREAM_PATH:?}"\n');
  const errors = f.check().errors;
  assert.ok(errors.some((error) => error.includes('does not name the tracked recipe')));
  assert.ok(errors.some((error) => error.includes('does not name the source-offer generator')));
});
test('a missing shipping workflow fails rather than being skipped', (t) => {
  const f = fixture(t);
  rmSync(resolve(f.root, COMPAT_WORKFLOW));
  // Both modes, because each has its own way of not noticing: worktree mode reads the disk, index
  // mode reads the index, and a file can be gone from one and still present in the other.
  f.git('rm', '--cached', '--quiet', COMPAT_WORKFLOW);
  for (const staged of [false, true]) {
    const errors = f.check({ staged }).errors;
    assert.ok(
      errors.some((error) => error.startsWith(`${COMPAT_WORKFLOW}: missing`)),
      `${staged ? 'index' : 'worktree'} mode must name the missing shipping workflow`,
    );
    // The pin record still names that file as the shipping workflow, so both facts are reported.
    assert.ok(errors.some((error) => error.includes('artemis-compat workflow')));
  }
});
test('the legacy SDK bridge may not regain an automatic trigger', (t) => {
  const f = fixture(t);
  f.put(BRIDGE_WORKFLOW, BRIDGE_YML.replace('  workflow_dispatch:', '  push:\n    branches: [main]\n  workflow_dispatch:'));
  assert.ok(f.check().errors.some((error) => error.includes('must not contain push')));

  f.put(BRIDGE_WORKFLOW, `name: legacy\non:\n  pull_request:\n    paths: ['ports/artemis/src/**']\n`);
  const errors = f.check().errors;
  assert.ok(errors.some((error) => error.includes('must not contain pull_request')));
  assert.ok(errors.some((error) => error.includes('must stay dispatchable by hand')));
});
test('the legacy bridge record may not change its frozen diagnostic input', (t) => {
  const f = fixture(t);
  f.pins['artemis-bridge'] = { target: 'sync', module: INPUT.replace('/asyncify', '') };
  f.put('.github/upstream-pins.json', JSON.stringify(f.pins));
  const errors = f.check().errors;
  assert.ok(errors.some((error) => error.includes('legacy artemis-bridge target changed')));
  assert.ok(errors.some((error) => error.includes('legacy artemis-bridge module changed')));
});
test('a header-only ledger cannot masquerade as shipped C++ products', (t) => {
  const f = fixture(t);
  f.put(MAP, row('f1', 'util/context.h') + row('f2', '—'));
  assert.ok(f.check().errors.includes('Artemis source ledger has no C++ products'));
});
test('conflicted index is rejected before any ambiguous blob is read', (t) => {
  const f = fixture(t);
  const blob = f.git('hash-object', 'README.md').trim();
  command('git', ['update-index', '--index-info'], { cwd: f.root, input:
    `0 ${'0'.repeat(40)}\tREADME.md\n100644 ${blob} 1\tREADME.md\n100644 ${blob} 2\tREADME.md\n` });
  assert.ok(f.check({ staged: true }).errors.includes('Git index has unresolved conflicts'));
});
test('unknown CLI flags exit 2 rather than reporting a successful source check', () => {
  const result = spawnSync(process.execPath, [CHECKER, '--unknown'], { env, encoding: 'utf8' });
  if (result.error) throw result.error;
  assert.equal(result.status, 2);
  assert.match(result.stderr, /usage:/);
  assert.equal(result.stdout, '');
});
