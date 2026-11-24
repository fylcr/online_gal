import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { copyFileSync, existsSync, mkdirSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import test from 'node:test';
import { pathToFileURL } from 'node:url';

const systemEnvironment = Object.fromEntries(Object.entries(process.env).filter(([key]) =>
  /^(path|pathext|systemroot|windir|comspec|systemdrive|tmp|temp)$/i.test(key)));
const siteRecord = JSON.stringify({ v: 1 });
const deploymentOrigin = 'https://test-deployment.test-project.pages.dev';

function dispatcher(t) {
  const dir = mkdtempSync(join(tmpdir(), 'online-gal-pages-'));
  t.after(() => rmSync(dir, { recursive: true, force: true }));
  const put = (name, content) => {
    const path = join(dir, name);
    mkdirSync(dirname(path), { recursive: true });
    writeFileSync(path, content);
    return path;
  };
  const script = put('scripts/build-pages.mjs', '');
  copyFileSync(new URL('../scripts/build-pages.mjs', import.meta.url), script);
  put('apps/setup/scripts/build-setup.mjs', `
    import { mkdir, writeFile } from 'node:fs/promises';
    import { join } from 'node:path';
    export async function buildSetup(out) {
      await mkdir(join(out, '_astro'), { recursive: true });
      for (const name of ['index.html', 'sw.js', '_worker.js', '_routes.json', '_headers']) {
        await writeFile(join(out, name), 'setup fixture');
      }
    }
  `);
  const guard = put('network-guard.mjs', `
    import { writeFileSync } from 'node:fs';
    globalThis.fetch = async () => {
      writeFileSync('fetch-attempt', 'yes');
      throw new Error('unexpected network request');
    };
  `);
  const child = put('site-child.mjs', `
    import { mkdirSync, writeFileSync } from 'node:fs';
    writeFileSync('site-call.json', JSON.stringify(process.argv.slice(2)));
    if (process.env.FAIL_SITE === '1') process.exit(3);
    mkdirSync('dist/site', { recursive: true });
    writeFileSync('dist/site/_headers', '/*\\n  X-Content-Type-Options: nosniff\\n');
    writeFileSync('dist/site/_routes.json', '{"version":1,"include":["/__ca/*"],"exclude":[]}');
  `);
  const env = {
    ...systemEnvironment,
    CF_PAGES: '1',
    CF_PAGES_URL: deploymentOrigin,
    CF_PAGES_BRANCH: 'isolated',
    CF_PAGES_COMMIT_SHA: 'fixture-commit',
    GITHUB_OUTPUT: join(dir, 'phase.txt'),
    npm_execpath: child,
  };
  return {
    run(args, overrides) {
      const result = spawnSync(process.execPath, ['--import', pathToFileURL(guard).href, script, ...args], {
        cwd: dir, env: { ...env, ...overrides }, encoding: 'utf8', timeout: 15_000,
      });
      if (result.error) throw result.error;
      return result;
    },
    read: (name) => readFileSync(join(dir, name), 'utf8'),
    exists: (name) => existsSync(join(dir, name)),
  };
}

for (const [label, args, overrides] of [
  ['implicit', [], {}],
  ['explicit', ['--phase=setup'], { ONLINE_GAL_SITE: siteRecord }],
]) {
  test(`${label} setup skips site builds and network requests even when artifact settings are set`, (t) => {
    const fixture = dispatcher(t);
    const result = fixture.run(args, {
      ...overrides,
      ONLINE_GAL_MANAGER_LOCK: 'missing-lock.json',
      ONLINE_GAL_MANAGER_ARCHIVE: 'missing-manager.zip',
      ONLINE_GAL_MANAGER_GITHUB_TOKEN: 'fixture-only',
    });
    assert.equal(result.status, 0, result.stderr);
    assert.equal(fixture.read('phase.txt'), 'phase=setup\n');
    assert.equal(fixture.exists('dist/site/_worker.js'), true);
    assert.equal(fixture.exists('dist/site/.well-known/online-gal-deployment.json'), false);
    assert.equal(fixture.exists('site-call.json'), false);
    assert.equal(fixture.exists('fetch-attempt'), false);
  });
}

for (const [label, settings, expected] of [
  ['source', {}, 'build:site'],
  ['token only', { ONLINE_GAL_MANAGER_GITHUB_TOKEN: 'fixture-only' }, 'build:site'],
  ['lock', { ONLINE_GAL_MANAGER_LOCK: 'isolated-lock.json' }, 'build:site:artifact'],
  ['archive', { ONLINE_GAL_MANAGER_ARCHIVE: 'manager.zip' }, 'build:site:artifact'],
]) {
  test(`${label} site preserves Function routes and emits the S13 deployment identity`, (t) => {
    const fixture = dispatcher(t);
    const result = fixture.run([], { ONLINE_GAL_SITE: siteRecord, ...settings });
    assert.equal(result.status, 0, result.stderr);
    assert.deepEqual(JSON.parse(fixture.read('site-call.json')), ['run', expected]);
    assert.equal(fixture.read('phase.txt'), 'phase=site\n');
    assert.deepEqual(JSON.parse(fixture.read('dist/site/.well-known/online-gal-deployment.json')), {
      url: deploymentOrigin, branch: 'isolated', commit: 'fixture-commit',
    });
    assert.match(fixture.read('dist/site/_headers'), /\/\.well-known\/online-gal-deployment\.json\n  Cache-Control: no-store\n  Access-Control-Allow-Origin: \*/);
    assert.deepEqual(JSON.parse(fixture.read('dist/site/_routes.json')).include, ['/__ca/*']);
  });
}

test('a failed artifact build exits without a site phase or source fallback', (t) => {
  const fixture = dispatcher(t);
  const result = fixture.run([], { ONLINE_GAL_SITE: siteRecord, ONLINE_GAL_MANAGER_LOCK: 'absent.json', FAIL_SITE: '1' });
  assert.equal(result.status, 1);
  assert.deepEqual(JSON.parse(fixture.read('site-call.json')), ['run', 'build:site:artifact']);
  assert.equal(fixture.exists('dist/site/.well-known/online-gal-deployment.json'), false);
  assert.equal(fixture.exists('phase.txt'), false);
});
