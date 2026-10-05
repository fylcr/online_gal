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
import { mkdtempSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
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

test('RUN-BUILD refuses a reversed apply before it builds', () => {
  const recipe = readFileSync(join(ROOT, 'ports/artemis/compat-web/RUN-BUILD.sh'), 'utf8');
  assert.match(recipe, /patch --dry-run --forward --binary/);
  assert.match(recipe, /patch --forward --binary/);
  assert.doesNotMatch(recipe, /patch --binary -d "\$BUILD" -p1 --batch/);
});
