/**
 * One command for the approved ONS/KRKR release matrix.
 *
 * Every row reports build and release as 0 or 1. GitHub is never contacted:
 * inventory and `gh` are in-memory fakes, and the only Git repositories are
 * temporary fixtures.
 */
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import test from 'node:test';
import { fileURLToPath } from 'node:url';
import { classifyPaths, planPublication } from '../.github/scripts/runtime-release-policy.mjs';
import { preflightFromEnv, publishCreated } from '../.github/scripts/runtime-release-gate.mjs';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const rows = [];
function row(name, build, release) {
  rows.push({ name, build, release });
  assert.equal(build === 0 || build === 1, true, name);
  assert.equal(release === 0 || release === 1, true, name);
}

function repo() {
  const dir = mkdtempSync(join(tmpdir(), 'runtime-release-matrix-'));
  const git = (args) => execFileSync('git', args, { cwd: dir, stdio: 'pipe' });
  git(['init', '-q']);
  git(['config', 'user.email', 'matrix@example.invalid']);
  git(['config', 'user.name', 'matrix']);
  const commit = (file, body) => {
    mkdirSync(join(dir, dirname(file)), { recursive: true });
    writeFileSync(join(dir, file), body);
    git(['add', '--', file]);
    git(['commit', '-q', '-m', file]);
    return git(['rev-parse', 'HEAD']).toString().trim();
  };
  commit('README.md', 'base\n');
  mkdirSync(join(dir, 'packages/engine/core/onscripter-yuri'), { recursive: true });
  mkdirSync(join(dir, 'packages/engine/core/kirikiroid2-web'), { recursive: true });
  writeFileSync(join(dir, 'packages/engine/core/onscripter-yuri/package.json'), '{"version":"0.1.3"}\n');
  writeFileSync(join(dir, 'packages/engine/core/kirikiroid2-web/package.json'), '{"version":"0.1.1"}\n');
  git(['add', '--', 'packages']);
  git(['commit', '-q', '-m', 'packages']);
  return { dir, git, commit, cleanup: () => rmSync(dir, { recursive: true, force: true }) };
}

const empty = { tag: [], release: [], asset: [], history: [] };
const clock = new Date('2026-10-13T23:59:00Z');

test('release matrix reports every approved 0/1 row', () => {
  const tree = repo();
  try {
    const onsBefore = tree.commit('docs/readme.md', 'docs\n');
    const onsAfter = tree.commit('packages/engine/core/onscripter-yuri/src/index.ts', 'export {}\n');
    const ons = planPublication({ engine: 'ons', mode: 'push', clock, before: onsBefore, after: onsAfter, repo: tree.dir, inventory: empty });
    row('runtime ONS-only', ons.build, ons.release);
    const krkr = planPublication({ engine: 'krkr', mode: 'push', clock, before: onsBefore, after: onsAfter, repo: tree.dir, inventory: empty });
    row('runtime ONS-only does not build KRKR', krkr.build, krkr.release);

    const docsBefore = tree.git(['rev-parse', 'HEAD']).toString().trim();
    const docsAfter = tree.commit('docs/guide.md', 'guide\n');
    const docs = planPublication({ engine: 'ons', mode: 'push', clock, before: docsBefore, after: docsAfter, repo: tree.dir, inventory: empty });
    row('docs-only', docs.build, docs.release);
    const siblingAfter = tree.commit('packages/vfs/src/index.ts', 'vfs\n');
    const sibling = classifyPaths(['packages/vfs/src/index.ts', 'packages/player-host/src/index.ts']);
    row('sibling-only', sibling.ons, sibling.krkr);
    assert.equal(siblingAfter.length, 40);

    const manual = planPublication({ engine: 'ons', mode: 'manual', publish: true, version: '1.0.0+a', repo: tree.dir, inventory: empty });
    row('manual valid +metadata', manual.build, manual.release);
    for (const [name, input] of [
      ['manual blank', { publish: true, version: ' ' }],
      ['manual false', { publish: false, version: '1.0.0' }],
      ['manual invalid', { publish: true, version: 'v1.2.3' }],
    ]) {
      const result = planPublication({ engine: 'ons', mode: 'manual', ...input, repo: tree.dir, inventory: empty });
      row(name, result.build, result.release);
    }
    const duplicate = planPublication({
      engine: 'ons', mode: 'manual', publish: true, version: '1.0.0+a', repo: tree.dir,
      inventory: { tag: [{ tag: 'engine-ons-1.0.0+a', status: 200 }], release: [], asset: [], history: [] },
    });
    row('manual duplicate', duplicate.build, duplicate.release);
    const otherMetadata = planPublication({
      engine: 'ons', mode: 'manual', publish: true, version: '1.0.0+b', repo: tree.dir,
      inventory: { tag: [{ tag: 'engine-ons-1.0.0+a', status: 200 }], release: [], asset: [], history: [] },
    });
    row('distinct +metadata', otherMetadata.build, otherMetadata.release);

    const renamedBefore = tree.commit('packages/engine/core/onscripter-yuri/src/old.ts', 'old\n');
    tree.git(['mv', 'packages/engine/core/onscripter-yuri/src/old.ts', 'packages/engine/core/onscripter-yuri/src/renamed.ts']);
    tree.git(['commit', '-q', '-m', 'rename']);
    const renamedAfter = tree.git(['rev-parse', 'HEAD']).toString().trim();
    const renamed = planPublication({ engine: 'ons', mode: 'push', clock, before: renamedBefore, after: renamedAfter, repo: tree.dir, inventory: empty });
    row('multi-commit rename', renamed.build, renamed.release);

    // A 404 on the release list means the repository or token scope is unknown;
    // it must abort the preflight instead of looking like an empty history.
    const listUnknown = { tag: [{ status: 404 }], release: [{ status: 404 }], asset: [], history: [{ status: 404 }] };
    const unknownHistory = preflightFromEnv({
      RELEASE_ENGINE: 'ons', EVENT_NAME: 'push', PUBLISH: 'true',
      RANGE_BEFORE: onsBefore, RANGE_AFTER: onsAfter,
      REPOSITORY: 'example/online_gal', GITHUB_WORKSPACE: tree.dir,
    }, { clock, lookup: () => listUnknown });
    assert.match(unknownHistory.reason, /history lookup is not a definitive 200 \(status 404\)/);
    row('history list 404 fails closed before the build marker', unknownHistory.build, unknownHistory.release);

    let historyFailed = false;
    try { planPublication({ engine: 'ons', mode: 'push', clock, before: '0'.repeat(40), after: renamedAfter, repo: tree.dir, inventory: empty }); }
    catch { historyFailed = true; }
    row('first push zero before', historyFailed ? 0 : 1, 0);

    const workflow = readFileSync(join(ROOT, '.github/workflows/build-wasm-ons.yml'), 'utf8');
    assert.equal(workflow.includes('\n  pull_request:'), false);
    row('PR has no ONS expensive trigger', 0, 0);
    const drift = readFileSync(join(ROOT, '.github/workflows/check-upstream-drift.yml'), 'utf8');
    assert.equal(drift.includes('gh workflow run'), false);
    row('drift no dispatch', 0, 0);
    const packages = readFileSync(join(ROOT, '.github/workflows/build-packages.yml'), 'utf8');
    assert.match(packages, /g\+\+ libz-dev/);
    assert.match(packages, /inputs\.publish == true/);
    row('packages push release', 0, 0);

    const ledger = readFileSync(join(ROOT, 'ports/lib/engine-artifacts.json'), 'utf8');
    assert.match(ledger, /engine-ons-0\.7\.7beta/);
    assert.match(ledger, /engine-krkr-20260920/);
    row('catalog pins not mutated into a release', 0, 0);
    for (const file of ['apps/app/package.json', 'package.json', 'pnpm-lock.yaml', 'ports/ons/build/fetch-web-engine.sh', 'LICENSE', '.github/scripts/make-package.mjs']) {
      const classified = classifyPaths([file]);
      assert.equal(classified.ons, 0, file);
      assert.equal(classified.krkr, 0, file);
    }
    row('Astro lock ports licenses and scripts', 0, 0);
  } finally {
    tree.cleanup();
  }

  const calls = [];
  const fake = (command, args) => {
    calls.push([command, ...args]);
    if (args[0] === 'release' && args[1] === 'upload' && args.at(-1) === 'manifest.json') {
      const error = new Error('simulated upload failure');
      error.status = 1;
      throw error;
    }
    if (args[0] === '-e') return 'online-gal-engine-core-onscripter-yuri-1.0.0+a.tgz\n';
    return '';
  };
  const lookup = () => empty;
  let kept = false;
  try {
    publishCreated({
      RELEASE_ENGINE: 'ons', SELECTED_VERSION: '1.0.0+a', SELECTED_TAG: 'engine-ons-1.0.0+a',
      REPOSITORY: 'example/online_gal', UPSTREAM_REPOSITORY: 'https://example.invalid/ons', UPSTREAM_COMMIT: 'a'.repeat(40),
      TARGET_SHA: 'b'.repeat(40), RUN_ID: '1', SERVER_URL: 'https://github.example', ARTIFACT_DIR: 'dist/ons',
      ARTIFACT_FILES: 'onsyuri.wasm', PACKAGE_DIR: 'packages/engine/core/onscripter-yuri', GITHUB_WORKSPACE: ROOT,
    }, { execFile: fake, lookup });
  } catch (cause) {
    kept = /kept engine-ons-1\.0\.0\+a/.test(cause.message);
    assert.equal(cause.message.includes('delete'), false);
  }
  assert.equal(kept, true);
  assert.equal(calls.some((call) => call.includes('--clobber') || call.includes('delete')), false);
  row('upload-after-create failure keeps release', 0, 1);

  process.stdout.write(`${JSON.stringify(rows, null, 2)}\n`);
});

test('same-minute rerun and unknown auth fail before the build marker', () => {
  const tree = repo();
  try {
    const before = tree.commit('packages/engine/core/onscripter-yuri/src/index.ts', 'a\n');
    const after = tree.commit('packages/engine/core/onscripter-yuri/src/index.ts', 'b\n');
    const env = {
      RELEASE_ENGINE: 'ons', EVENT_NAME: 'push', PUBLISH: 'true', RANGE_BEFORE: before, RANGE_AFTER: after,
      REPOSITORY: 'example/online_gal', GITHUB_WORKSPACE: tree.dir,
    };
    const existing = { tag: [{ tag: 'engine-ons-0.1.3-beta-202610140759', status: 200 }], release: [], asset: [], history: [] };
    const again = preflightFromEnv(env, { clock, lookup: () => existing });
    assert.equal(again.build, 0);
    assert.match(again.reason, /exact identity exists/);
    assert.throws(() => preflightFromEnv(env, { clock, lookup: () => { throw new Error('network'); } }), /network/);
    const unknown = planPublication({
      engine: 'ons', mode: 'manual', publish: true, version: '1.0.0+b', repo: tree.dir,
      inventory: { tag: [{ status: 401 }], release: [], asset: [], history: [] },
    });
    assert.equal(unknown.build, 0);
    assert.match(unknown.reason, /401/);
  } finally {
    tree.cleanup();
  }
});

test('push preflight looks up the generated version and rejects before returning', () => {
  const tree = repo();
  try {
    const before = tree.commit('docs/one.md', 'one\n');
    const after = tree.commit('packages/engine/core/onscripter-yuri/src/index.ts', 'export {}\n');
    const seen = [];
    const env = {
      RELEASE_ENGINE: 'ons', EVENT_NAME: 'push', PUBLISH: 'true',
      RANGE_BEFORE: before, RANGE_AFTER: after,
      REPOSITORY: 'example/online_gal', GITHUB_WORKSPACE: tree.dir,
    };
    const generated = '0.1.3-beta-202610140759';
    const existing = { tag: [{ status: 200, tag: `engine-ons-${generated}` }], release: [], asset: [], history: [] };
    const result = preflightFromEnv(env, { clock, lookup: (...args) => { seen.push(args); return existing; } });
    assert.equal(result.build, 0);
    assert.equal(result.release, 0);
    assert.equal(result.version, generated);
    assert.match(result.reason, /exact identity exists/);
    assert.equal(result.clock, clock.toISOString());
    assert.equal(seen.length, 1);
    assert.deepEqual(seen[0], ['example/online_gal', 'ons', generated]);
  } finally {
    tree.cleanup();
  }
});

test('manual false blank and invalid versions make no lookup call', () => {
  const tree = repo();
  try {
    let calls = 0;
    const deps = { clock, lookup: () => { calls += 1; return empty; } };
    const base = {
      RELEASE_ENGINE: 'ons', EVENT_NAME: 'workflow_dispatch', REPOSITORY: 'example/online_gal',
      GITHUB_WORKSPACE: tree.dir,
    };
    for (const [publish, version] of [['false', '1.0.0'], ['true', ''], ['true', '   '], ['true', 'v1.2.3']]) {
      const result = preflightFromEnv({ ...base, PUBLISH: publish, MANUAL_VERSION: version }, deps);
      assert.equal(result.build, 0, `${publish} ${version}`);
      assert.equal(result.release, 0, `${publish} ${version}`);
      assert.match(result.reason, /publish|SemVer/, `${publish} ${version}`);
    }
    assert.equal(calls, 0);
    const allowed = preflightFromEnv({ ...base, PUBLISH: 'true', MANUAL_VERSION: '1.0.0' }, deps);
    assert.equal(allowed.build, 1);
    assert.equal(calls, 2);
  } finally {
    tree.cleanup();
  }
});

test('a second collision aborts before any release is created', () => {
  const calls = [];
  const fake = (command, args) => { calls.push([command, ...args]); return ''; };
  const existing = { tag: [{ status: 200, tag: 'engine-ons-1.0.0+a' }], release: [], asset: [], history: [] };
  assert.throws(() => publishCreated({
    RELEASE_ENGINE: 'ons', SELECTED_VERSION: '1.0.0+a', SELECTED_TAG: 'engine-ons-1.0.0+a',
    REPOSITORY: 'example/online_gal', UPSTREAM_REPOSITORY: 'https://example.invalid/ons',
    UPSTREAM_COMMIT: 'a'.repeat(40), TARGET_SHA: 'b'.repeat(40), RUN_ID: '1',
    SERVER_URL: 'https://github.example', ARTIFACT_DIR: 'dist/ons', ARTIFACT_FILES: 'onsyuri.wasm',
    PACKAGE_DIR: 'packages/engine/core/onscripter-yuri', GITHUB_WORKSPACE: ROOT,
  }, { execFile: fake, lookup: () => existing }), /exact identity exists/);
  assert.deepEqual(calls, []);
});
