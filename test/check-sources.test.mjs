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
const CHECKER = fileURLToPath(new URL('../scripts/check-sources.mjs', import.meta.url));
const row = (id, file) => `fn\t${id}\t—\tartc::Fixture\t${file}\tR1\tfixture\n`;
const env = { ...process.env };
for (const key of ['GIT_DIR', 'GIT_WORK_TREE', 'GIT_INDEX_FILE', 'GIT_OBJECT_DIRECTORY',
  'GIT_ALTERNATE_OBJECT_DIRECTORIES', 'GIT_CONFIG_PARAMETERS', 'GIT_CONFIG_COUNT']) delete env[key];

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
    'artemis-bridge': { target: 'asyncify', module: INPUT },
  };
  put('.github/upstream-pins.json', JSON.stringify(pins));
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
test('Artemis rejects sync shipping and requires its exact asyncify input', (t) => {
  const f = fixture(t);
  f.pins['artemis-bridge'] = { target: 'sync', module: INPUT.replace('/asyncify', '') };
  f.put('.github/upstream-pins.json', JSON.stringify(f.pins));
  const errors = f.check().errors;
  assert.ok(errors.some((error) => error.includes('shipping target must be asyncify')));
  assert.ok(errors.some((error) => error.includes('unexpected Artemis asyncify input')));
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
