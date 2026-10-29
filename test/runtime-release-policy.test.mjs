/**
 * Source-level contract for the ONS/KRKR release policy.
 *
 * No compiler, no npm install, and no GitHub call. Git fixtures are temporary
 * repositories created under the OS temp directory, never the main checkout.
 */
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, readFileSync, writeFileSync, mkdirSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import {
  automaticVersion,
  changedPaths,
  classifyPaths,
  collisionHits,
  compareSemVer,
  encodePathSegment,
  gitCheckRefFormat,
  isSemVer,
  planPublication,
  releaseIdentity,
  semverCore,
} from '../.github/scripts/runtime-release-policy.mjs';
import { lookup } from '../.github/scripts/runtime-release-inventory.mjs';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const read = (path) => readFileSync(join(ROOT, path), 'utf8');
const code = (path) => read(path).split(/\r?\n/).filter((line) => !/^\s*#/.test(line)).join('\n');

test('SemVer accepts the approved forms and rejects prefixes, leading zeroes and blanks', () => {
  for (const value of ['0.0.1-beta-202610140759', '1.0.0+a', '1.0.0+b', '1.0.0-beta.2', '0.1.3-rc.1+sha']) {
    assert.equal(isSemVer(value), true, value);
  }
  for (const value of ['v1.2.3', '01.2.3', '1.02.3', '1.2.03', '', ' ', '1.0.0 ', ' 1.0.0', '1', '1.0', '1.0.0-01', '1.0.0+', '\uFF11.0.0', '1.0.0\u00A0', '1.0.0\n', '1.0.0\t']) {
    assert.equal(isSemVer(value), false, JSON.stringify(value));
  }
  assert.equal(semverCore('0.1.3-beta-202610140759+meta'), '0.1.3');
  assert.equal(automaticVersion('0.1.3', new Date('2026-10-13T23:59:00Z')), '0.1.3-beta-202610140759');
  assert.equal(automaticVersion('0.1.1', new Date('2026-10-14T16:00:00Z')), '0.1.1-beta-202610150000');
  const once = new Date('2026-10-14T00:00:30Z');
  assert.equal(automaticVersion('0.1.3', once), automaticVersion('0.1.3', new Date(once.getTime() + 29000)));
  assert.equal(compareSemVer('1.0.0-beta-202610140759', '1.0.0'), -1);
  assert.equal(compareSemVer('1.0.0+a', '1.0.0+b'), 0);
  assert.notEqual('1.0.0-beta-2', '1.0.0-beta.2');
  assert.equal(compareSemVer('1.0.0-beta-2', '1.0.0-beta.2') === 0, false);
});

test('release identity keeps plus and encodes only the URL path segment', () => {
  const identity = releaseIdentity('ons', '1.0.0+a');
  assert.equal(identity.tag, 'engine-ons-1.0.0+a');
  assert.equal(identity.packageVersion, '1.0.0+a');
  assert.equal(identity.tarball, 'online-gal-engine-core-onscripter-yuri-1.0.0+a.tgz');
  assert.equal(identity.manifestVersion, '1.0.0+a');
  assert.equal(identity.downloadTag, 'engine-ons-1.0.0%2Ba');
  assert.equal(encodePathSegment('engine-krkr-1.0.0+b'), 'engine-krkr-1.0.0%2Bb');
  assert.equal(gitCheckRefFormat(identity.tag), true);
  assert.deepEqual(releaseIdentity('krkr', '1.0.0+b').tag, 'engine-krkr-1.0.0+b');
});

test('exact metadata does not collide and unknown lookup status fails closed', () => {
  const distinct = { tag: [{ tag: 'engine-ons-1.0.0+a', status: 200 }], release: [], asset: [], history: [] };
  assert.deepEqual(collisionHits('1.0.0+b', distinct), []);
  assert.deepEqual(collisionHits('1.0.0+a', distinct), ['tag']);
  assert.throws(() => collisionHits('1.0.0+b', { tag: [{ status: 401 }], release: [], asset: [], history: [] }), /401/);
  assert.throws(() => collisionHits('1.0.0+b', { tag: [{ status: 403 }], release: [], asset: [], history: [] }), /403/);
  assert.throws(() => collisionHits('1.0.0+b', { tag: [{ status: 500 }], release: [], asset: [], history: [] }), /500/);
  assert.throws(() => collisionHits('1.0.0+b', { tag: [], release: [], asset: [], history: [{ status: 0 }] }), /not a definitive/);
  // The exact tag and release endpoints answer 404 for absence.
  const absentExact = { tag: [{ status: 404 }], release: [{ status: 404 }], asset: [], history: [] };
  assert.deepEqual(collisionHits('1.0.0+b', absentExact), []);
  // The release list is 200 with an empty array when nothing matches, so a 404
  // history row is an unknown repository or scope, not an empty history.
  assert.throws(
    () => collisionHits('1.0.0+b', { tag: [{ status: 404 }], release: [{ status: 404 }], asset: [], history: [{ status: 404 }] }),
    /history lookup is not a definitive 200 \(status 404\)/,
  );
  for (const status of [401, 403, 500, 0]) {
    const rows = { tag: [{ status: 404 }], release: [{ status: 404 }], asset: [], history: [{ status }] };
    assert.throws(() => collisionHits('1.0.0+b', rows), new RegExp(`history lookup is not a definitive 200 \\(status ${status}\\)`), String(status));
  }
});

function fixtureRepo() {
  const dir = mkdtempSync(join(tmpdir(), 'runtime-release-policy-'));
  const git = (args) => execFileSync('git', args, { cwd: dir, stdio: 'pipe' });
  git(['init', '-q']);
  git(['config', 'user.email', 'policy-test@example.invalid']);
  git(['config', 'user.name', 'policy-test']);
  const commit = (file, body, message) => {
    mkdirSync(join(dir, dirname(file)), { recursive: true });
    writeFileSync(join(dir, file), body);
    git(['add', '--', file]);
    git(['commit', '-q', '-m', message]);
    return git(['rev-parse', 'HEAD']).toString().trim();
  };
  return { dir, git, commit, cleanup: () => rmSync(dir, { recursive: true, force: true }) };
}

test('push range classifies only package-owned inputs, including rename and delete', () => {
  const repo = fixtureRepo();
  try {
    repo.commit('README.md', 'base\n', 'base');
    const base = repo.commit('packages/engine/core/onscripter-yuri/src/old.ts', 'old\n', 'add');
    repo.git(['mv', 'packages/engine/core/onscripter-yuri/src/old.ts', 'packages/engine/core/onscripter-yuri/src/new.ts']);
    repo.git(['commit', '-q', '-m', 'rename']);
    const renamed = repo.git(['rev-parse', 'HEAD']).toString().trim();
    const renamedFiles = changedPaths(base, renamed, repo.dir);
    assert.equal(classifyPaths(renamedFiles).ons, 1);
    assert.equal(classifyPaths(renamedFiles).krkr, 0);
    assert.ok(renamedFiles.includes('packages/engine/core/onscripter-yuri/src/old.ts'));
    assert.ok(renamedFiles.includes('packages/engine/core/onscripter-yuri/src/new.ts'));

    repo.git(['rm', '-q', 'packages/engine/core/onscripter-yuri/src/new.ts']);
    repo.git(['commit', '-q', '-m', 'delete']);
    const deleted = repo.git(['rev-parse', 'HEAD']).toString().trim();
    assert.equal(classifyPaths(changedPaths(renamed, deleted, repo.dir)).ons, 1);

    const sibling = repo.commit('packages/vfs/src/index.ts', 'sibling\n', 'sibling');
    assert.deepEqual(classifyPaths(changedPaths(deleted, sibling, repo.dir)), {
      ons: 0, krkr: 0, files: ['packages/vfs/src/index.ts'],
    });
    for (const file of [
      'packages/engine/core/onscripter-yuri/README.md',
      'packages/engine/core/onscripter-yuri/assets/x.wasm',
      'ports/ons/build/x',
      'pnpm-lock.yaml',
      'LICENSE',
      '.github/scripts/make-package.mjs',
    ]) {
      assert.equal(classifyPaths([file]).ons, 0, file);
      assert.equal(classifyPaths([file]).krkr, 0, file);
    }
    assert.equal(classifyPaths(['packages/engine/core/kirikiroid2-web/src/index.ts']).krkr, 1);
    assert.equal(classifyPaths(['packages/tjs2-bytecode/src/index.ts']).krkr, 0);
    assert.throws(() => changedPaths('0'.repeat(40), renamed, repo.dir), /all-zero/);
    assert.throws(() => changedPaths('not-a-sha', renamed, repo.dir), /40-hex/);
    assert.throws(() => changedPaths('a'.repeat(40), renamed, repo.dir), /failed/);
  } finally {
    repo.cleanup();
  }
});

test('preflight rejects collision before the build marker and keeps existing bytes', () => {
  const repo = fixtureRepo();
  try {
    writeFileSync(join(repo.dir, 'package.json'), '{"version":"0.1.3"}\n');
    mkdirSync(join(repo.dir, 'packages/engine/core/onscripter-yuri'), { recursive: true });
    writeFileSync(join(repo.dir, 'packages/engine/core/onscripter-yuri/package.json'), '{"version":"0.1.3"}\n');
    const before = repo.commit('packages/engine/core/onscripter-yuri/src/index.ts', 'one\n', 'one');
    const after = repo.commit('packages/engine/core/onscripter-yuri/src/index.ts', 'two\n', 'two');
    const clock = new Date('2026-10-13T23:59:00Z');
    const inventory = { tag: [{ tag: 'engine-ons-0.1.3-beta-202610140759', status: 200 }], release: [], asset: [], history: [] };
    const blocked = planPublication({
      engine: 'ons', mode: 'push', clock, before, after, repo: repo.dir, inventory,
    });
    assert.equal(blocked.build, 0);
    assert.equal(blocked.release, 0);
    assert.match(blocked.reason, /exact identity exists/);
    const clear = planPublication({
      engine: 'ons', mode: 'push', clock, before, after, repo: repo.dir,
      inventory: { tag: [{ status: 404 }], release: [], asset: [], history: [] },
    });
    assert.equal(clear.build, 1);
    assert.equal(clear.version, '0.1.3-beta-202610140759');
    const manual = planPublication({ engine: 'ons', mode: 'manual', publish: true, version: '1.0.0+a', repo: repo.dir, inventory: { tag: [], release: [], asset: [], history: [] } });
    assert.equal(manual.build, 1);
    assert.equal(manual.identity.downloadTag, 'engine-ons-1.0.0%2Ba');
    for (const input of [
      { publish: false, version: '1.0.0' },
      { publish: true, version: '' },
      { publish: true, version: 'v1.2.3' },
      { publish: true, version: '1.0.0+a' },
    ]) {
      const result = planPublication({
        engine: 'ons', mode: 'manual', publish: input.publish, version: input.version, repo: repo.dir,
        inventory: input.version === '1.0.0+a' ? inventoryFor('1.0.0+a') : { tag: [], release: [], asset: [], history: [] },
      });
      assert.equal(result.build, 0, JSON.stringify(input));
      assert.equal(result.release, 0, JSON.stringify(input));
    }
  } finally {
    repo.cleanup();
  }
});

function inventoryFor(version) {
  return { tag: [{ tag: `engine-ons-${version}`, status: 200 }], release: [], asset: [], history: [] };
}

test('caller workflows narrow paths and do not publish from a pull request', () => {
  const ons = code('.github/workflows/build-wasm-ons.yml');
  const krkr = code('.github/workflows/build-wasm-krkr.yml');
  assert.match(ons, /packages\/engine\/core\/onscripter-yuri\/src\/\*\*/);
  assert.match(ons, /packages\/engine\/core\/onscripter-yuri\/scripts\/\*\*/);
  assert.doesNotMatch(ons, /pnpm-lock\.yaml/);
  assert.doesNotMatch(ons, /packages\/engine-api/);
  assert.doesNotMatch(krkr, /packages\/engine\/core\/kirikiroid2-web\/\*\*/);
  assert.match(krkr, /packages\/engine\/core\/kirikiroid2-web\/src\/\*\*/);
  assert.match(ons, /default: false/);
  assert.match(krkr, /default: false/);
  assert.match(ons, /github\.event_name == 'workflow_dispatch' && inputs\.publish == true/);
  assert.match(ons, /github\.event_name == 'push' \|\|/);
  assert.doesNotMatch(ons, /version: 0\.7\.7beta/);
  assert.doesNotMatch(krkr, /version: kirikiroid2-web-20260920/);
  assert.match(read('.github/workflows/build-wasm-artemis-compat.yml'), /publish: false/);
  const bridge = read('.github/workflows/build-wasm-artemis-bridge.yml');
  assert.match(bridge, /inputs\.publish \|\| vars\.ARTEMIS_PUBLISH_ARTIFACTS == 'true'/);
  assert.doesNotMatch(read('.github/workflows/check-upstream-drift.yml'), /gh workflow run/);
});

test('packages workflow still builds on push and keeps the native probes', () => {
  const source = read('.github/workflows/build-packages.yml');
  assert.match(source, /github\.event_name == 'workflow_dispatch' && inputs\.publish == true/);
  assert.match(source, /sudo apt-get install -y g\+\+ libz-dev/);
  assert.match(source, /pnpm run test:portable/);
  assert.doesNotMatch(code('.github/workflows/build-wasm-ons.yml'), /pull_request:[\s\S]*ports\/ons\/build/);
});

test('historical catalog pins are unchanged', () => {
  const ledger = read('ports/lib/engine-artifacts.json');
  assert.match(ledger, /"releaseTag": "engine-ons-0\.7\.7beta"/);
  assert.match(ledger, /"releaseTag": "engine-krkr-20260920"/);
  const fetchEngine = read('ports/lib/fetch-engine.mjs');
  assert.match(fetchEngine, /encodeURIComponent\(entry\.releaseTag\)/);
  assert.match(fetchEngine, /encodeURIComponent\(file\.name\)/);
  assert.doesNotMatch(fetchEngine, /latest/);
});

test('lookup reads the paginated release history and keeps only the exact identity', () => {
  const pages = [
    [
      { tag_name: 'engine-ons-1.0.0+a', assets: [{ name: 'online-gal-engine-core-onscripter-yuri-1.0.0+a.tgz' }] },
      { tag_name: 'engine-krkr-1.0.0+b', assets: [{ name: 'online-gal-engine-core-kirikiroid2-web-1.0.0+b.tgz' }] },
    ],
    [
      { tag_name: 'engine-ons-nightly', assets: [{ name: 'online-gal-engine-core-onscripter-yuri-1.0.0+b.tgz' }] },
      { tag_name: 'engine-ons-1.0.0+b', assets: [{ name: 'onsyuri.wasm' }, { name: 'online-gal-engine-core-onscripter-yuri-1.0.0+b.tgz' }] },
    ],
  ];
  const body = `${JSON.stringify(pages[0])}${JSON.stringify(pages[1])}`;
  const listCalls = [];
  const notFound = () => { const error = new Error('HTTP 404: Not Found'); error.stderr = 'HTTP 404: Not Found'; throw error; };
  const fake = (command, args) => {
    if (args.includes('--paginate')) { listCalls.push([command, ...args]); return body; }
    return notFound();
  };
  const rowsB = lookup('example/online_gal', 'ons', '1.0.0+b', fake);
  assert.equal(listCalls.length, 1);
  assert.equal(listCalls[0].some((arg) => arg === '/repos/example/online_gal/releases'), true);
  assert.equal(listCalls[0].includes('--jq'), false);
  assert.deepEqual(rowsB.history.map((row) => row.tag), ['engine-ons-nightly', 'engine-ons-1.0.0+b']);
  assert.equal(rowsB.history[0].name, 'online-gal-engine-core-onscripter-yuri-1.0.0+b.tgz');
  assert.equal(rowsB.history[1].name, 'online-gal-engine-core-onscripter-yuri-1.0.0+b.tgz');
  assert.deepEqual(collisionHits('1.0.0+b', rowsB, 'ons'), ['history']);
  assert.deepEqual(collisionHits('1.0.0+a', rowsB, 'ons'), []);
  const rowsA = lookup('example/online_gal', 'ons', '1.0.0+a', fake);
  assert.deepEqual(rowsA.history.map((row) => row.tag), ['engine-ons-1.0.0+a']);
  const rowsKrkr = lookup('example/online_gal', 'krkr', '1.0.0+b', fake);
  assert.deepEqual(rowsKrkr.history.map((row) => row.tag), ['engine-krkr-1.0.0+b']);
});

test('a history list 404 is fail closed while an exact tag or release 404 is absence', () => {
  const notFound = () => { const error = new Error('HTTP 404: Not Found'); error.stderr = 'HTTP 404: Not Found'; throw error; };
  const emptyHistory = (command, args) => (args.includes('--paginate') ? '[]' : notFound());
  const absent = lookup('example/online_gal', 'ons', '1.0.0+b', emptyHistory);
  assert.deepEqual(absent.tag, [{ status: 404, tag: undefined }]);
  assert.deepEqual(absent.release, [{ status: 404 }]);
  assert.deepEqual(absent.history, []);
  assert.deepEqual(collisionHits('1.0.0+b', absent, 'ons'), []);
  const listUnknown = lookup('example/online_gal', 'ons', '1.0.0+b', notFound);
  assert.deepEqual(listUnknown.tag, [{ status: 404, tag: undefined }]);
  assert.deepEqual(listUnknown.history, [{ status: 404 }]);
  assert.throws(() => collisionHits('1.0.0+b', listUnknown, 'ons'), /history lookup is not a definitive 200 \(status 404\)/);
  for (const status of [401, 403, 500]) {
    const denied = () => { const error = new Error(`HTTP ${status}`); error.stderr = `gh: HTTP ${status}`; throw error; };
    const rows = lookup('example/online_gal', 'ons', '1.0.0+b', denied);
    assert.throws(() => collisionHits('1.0.0+b', rows, 'ons'), new RegExp(String(status)));
    assert.throws(() => collisionHits('1.0.0+b', rows, 'ons'), /not a definitive/);
  }
  const offline = () => { throw new Error('getaddrinfo ENOTFOUND api.github.com'); };
  const rows = lookup('example/online_gal', 'ons', '1.0.0+b', offline);
  assert.equal(rows.history[0].status, 0);
  assert.throws(() => collisionHits('1.0.0+b', rows, 'ons'), /not a definitive/);
});

test('a history list 404 rejects the plan before the build marker', () => {
  const repo = fixtureRepo();
  try {
    const unknownList = { tag: [{ status: 404 }], release: [{ status: 404 }], asset: [], history: [{ status: 404 }] };
    const planned = planPublication({
      engine: 'ons', mode: 'manual', publish: true, version: '1.0.0+b', repo: repo.dir, inventory: unknownList,
    });
    assert.equal(planned.build, 0);
    assert.equal(planned.release, 0);
    assert.match(planned.reason, /history lookup is not a definitive 200 \(status 404\)/);
    const clear = planPublication({
      engine: 'ons', mode: 'manual', publish: true, version: '1.0.0+b', repo: repo.dir,
      inventory: { tag: [{ status: 404 }], release: [{ status: 404 }], asset: [], history: [] },
    });
    assert.equal(clear.build, 1);
    assert.equal(clear.release, 1);
  } finally {
    repo.cleanup();
  }
});

test('make-package validates --version with the shared SemVer rule', () => {
  const source = read('.github/scripts/make-package.mjs');
  assert.match(source, /import \{ isSemVer \} from '\.\/runtime-release-policy\.mjs'/);
  assert.match(source, /if \(!isSemVer\(options\.version\)\)/);
  assert.doesNotMatch(source, /const SEMVER\b/);
});

test('reusable fetch-depth keeps a non-falsy zero for the exact push range', () => {
  const source = read('.github/workflows/build-wasm-reusable.yml');
  assert.match(source, /fetch-depth: \$\{\{ inputs\.immutable-release && github\.event_name == 'push' && '0' \|\| '1' \}\}/);
  assert.doesNotMatch(source, /fetch-depth:[^\n]*&& 0 \|\| 1/);
});
