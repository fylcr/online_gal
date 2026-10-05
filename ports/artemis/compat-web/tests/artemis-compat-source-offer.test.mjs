/**
 * Source-offer identity. No wasm build.
 *
 * A patched CMake tree and an already-applied patch must be refused. A pure
 * `git archive` of the pin must be accepted and must match that git tree,
 * file for file. The real checkout is optional: missing it skips the archive
 * half and says so. The refusal half always runs.
 */
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import { existsSync, mkdirSync, mkdtempSync, readFileSync, readdirSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import { DEFAULT_PIN, assertExtractDir, gitTree, treeMatchesGit } from '../make-source-bundle.mjs';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..', '..', '..');
const REPO = join(ROOT, '.ref', 'artemis-compat-refresh', 'src');
const PATCH = join(ROOT, 'ports/artemis/compat-web/patches/0001-pf8-readat-host-source.patch');

function gitOk(repo) {
  return spawnSync('git', ['-C', repo, 'cat-file', '-e', `${DEFAULT_PIN}^{commit}`]).status === 0;
}

test('extract target outside .test or ports is refused', () => {
  assert.throws(() => assertExtractDir(ROOT, ROOT), /refusing to extract/);
  assert.throws(() => assertExtractDir(ROOT, join(ROOT, 'dist', 'artemis-compat')), /refusing to extract/);
  assert.equal(assertExtractDir(ROOT, join(ROOT, '.test', 'source-offer-child')),
    resolve(ROOT, '.test', 'source-offer-child'));
});

test('a modified CMake or an already-applied patch is not the pinned tree', () => {
  if (!gitOk(REPO)) {
    console.log(`SKIP archive: no ${DEFAULT_PIN} in ${REPO}`);
    return;
  }
  const expected = gitTree(REPO, DEFAULT_PIN);
  assert.equal(expected.size, 257);
  const dir = mkdtempSync(join(tmpdir(), 'artc-src-'));
  try {
    const tree = join(dir, 'tree');
    mkdirSync(tree);
    const archived = spawnSync('git', ['-C', REPO, 'archive', '--format=tar', '-o', join(dir, 'pin.tar'), DEFAULT_PIN], {
      stdio: 'inherit',
    });
    assert.equal(archived.status, 0);
    assert.equal(spawnSync('tar', ['-xf', 'pin.tar', '-C', 'tree'], { cwd: dir, stdio: 'inherit' }).status, 0);
    treeMatchesGit(tree, expected);
    const cmake = join(tree, 'CMakeLists.txt');
    const original = readFileSync(cmake);
    writeFileSync(cmake, `${original}# not upstream\n`);
    assert.throws(() => treeMatchesGit(tree, expected), /CMakeLists\.txt/);
    writeFileSync(cmake, original);
    writeFileSync(join(dir, 'one.patch'), readFileSync(PATCH));
    const applied = spawnSync('patch', ['--forward', '--binary', '-d', tree, '-p1', '--batch', '-i', join(dir, 'one.patch')], {
      stdio: 'inherit',
    });
    assert.equal(applied.status, 0, 'the fixture patch must apply once to a pure archive');
    assert.throws(() => treeMatchesGit(tree, expected), /does not match git blob/);
    const again = spawnSync('patch', ['--forward', '--binary', '-d', tree, '-p1', '--batch', '-i', join(dir, 'one.patch')], {
      stdio: 'inherit',
    });
    assert.notEqual(again.status, 0, 'an already-applied patch must be non-zero under --forward');
  } finally {
    rmSync(dir, { recursive: true, force: true });
  }
});

test('a non-default --repo archives that pin, and a missing repo or pin is refused', () => {
  const repo = join(ROOT, '.ref', 'artemis-compat');
  if (!gitOk(repo)) { console.log(`SKIP ci repo: no ${DEFAULT_PIN} in ${repo}`); return; }
  const base = join(ROOT, '.test', 'artemis-compat-refresh-ci-path-fix');
  rmSync(base, { recursive: true, force: true });
  mkdirSync(join(base, 'out'), { recursive: true });
  const gen = join(ROOT, 'ports/artemis/compat-web/make-source-bundle.mjs');
  const run = (repoPath, pin) => spawnSync(process.execPath, [gen, '--out', join(base, 'out'), '--work', join(base, 'work'), '--repo', repoPath], {
    cwd: ROOT, encoding: 'utf8', env: { ...process.env, ...(pin === undefined ? {} : { ARTC_PIN: pin }) },
  });
  const wrong = join(base, 'wrong-repo');
  mkdirSync(wrong);
  writeFileSync(join(wrong, 'f'), 'x\n');
  for (const args of [['init'], ['add', 'f'], ['-c', 'user.email=t@example.com', '-c', 'user.name=t', 'commit', '-m', 't']]) {
    assert.equal(spawnSync('git', ['-C', wrong, ...args], { encoding: 'utf8' }).status, 0);
  }
  const missing = run(join(base, 'missing'));
  const badRepo = run(wrong, DEFAULT_PIN);
  const badPin = run(repo, 'zz');
  const ok = run(repo, DEFAULT_PIN);
  writeFileSync(join(base, 'run.log'), [`missing exit=${missing.status}`, missing.stderr, `wrong exit=${badRepo.status}`, badRepo.stderr, `badpin exit=${badPin.status}`, badPin.stderr, `ok exit=${ok.status}`, ok.stdout, ok.stderr].join('\n'));
  assert.notEqual(missing.status, 0);
  assert.match(missing.stderr, /no git repository/);
  assert.notEqual(badRepo.status, 0);
  assert.notEqual(badPin.status, 0);
  assert.equal(ok.status, 0, ok.stderr);
  const tarPath = join(base, 'out', 'artemis-compat-source.tar.gz');
  const hash = createHash('sha256').update(readFileSync(tarPath)).digest('hex');
  writeFileSync(join(base, 'bundle.sha256'), `${hash}  artemis-compat-source.tar.gz\n`);
  const again = run(repo, DEFAULT_PIN);
  assert.equal(again.status, 0, again.stderr);
  assert.equal(createHash('sha256').update(readFileSync(tarPath)).digest('hex'), hash,
    'the same pin and patch list must produce the same archive');
  const extract = join(base, 'extract');
  rmSync(extract, { recursive: true, force: true });
  mkdirSync(extract);
  const extracted = spawnSync('tar', ['-xzf', relative(extract, tarPath).split(String.fromCharCode(92)).join('/'), '-C', '.'], { cwd: extract, encoding: 'utf8' });
  assert.equal(extracted.status, 0, extracted.stderr);
  const top = readdirSync(extract);
  assert.equal(top.length, 1);
  assert.notEqual(top[0], 'src');
  const src = join(extract, top[0], 'src');
  treeMatchesGit(src, gitTree(repo, DEFAULT_PIN), { extra: new Set(['NOTICE']) });
  assert.doesNotMatch(readFileSync(join(src, 'CMakeLists.txt'), 'utf8'), /add_executable\(artemis-compat/);
  assert.equal(readFileSync(join(extract, top[0], 'PIN'), 'utf8').trim(), DEFAULT_PIN);
  for (const rel of ['patches/0001-pf8-readat-host-source.patch', 'patches/0002-numeric-strto.patch', 'patches/0003-tween-loopdelay.patch', 'patches/0004-animation-dispatch-observe.patch', 'overlay/cmake/web.cmake', 'overlay/host/web_main.cpp']) {
    assert.equal(existsSync(join(extract, top[0], rel)), true, rel);
  }
});

test('RUN-BUILD refuses a reversed apply before it builds', () => {
  const recipe = readFileSync(join(ROOT, 'ports/artemis/compat-web/RUN-BUILD.sh'), 'utf8');
  assert.match(recipe, /patch --dry-run --forward --binary/);
  assert.match(recipe, /patch --forward --binary/);
  assert.doesNotMatch(recipe, /patch --binary -d "\$BUILD" -p1 --batch/);
});
