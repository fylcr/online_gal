import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { createHash, randomBytes } from 'node:crypto';
import { mkdtemp, readFile, rm, writeFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import test from 'node:test';
import { fileURLToPath } from 'node:url';

import { createArchive, fetchManagerArtifact, inspectManagerArtifact } from '../packages/plugin-system/scripts/manager-artifact.mjs';

const sha256 = (bytes) => createHash('sha256').update(bytes).digest('hex');
const json = (value) => Buffer.from(JSON.stringify(value));
const appManifest = JSON.parse(await readFile(new URL('../apps/app/plugin.manifest.json', import.meta.url)));
const buildSite = fileURLToPath(new URL('../packages/plugin-system/scripts/build-site.mjs', import.meta.url));

function fixture({ apiVersion = 1, assets = new Map(), mutate } = {}) {
  const files = new Map([['index.html', Buffer.from('<body>K21 fixture</body>')], ...assets]);
  const manifest = Buffer.from(`\n${JSON.stringify({
    format: 1,
    manifest: { ...appManifest, apiVersion, version: '0.1.0+fixture' },
    files: [...files.entries()].map(([path, bytes]) => ({ path, size: bytes.length, sha256: sha256(bytes) })),
  }, null, 4)}\n`);
  const entries = new Map([['plugin.json', json({ format: 2, manifest: 'mf-manifest.json' })], ['mf-manifest.json', manifest], ...files]);
  mutate?.(entries);
  return createArchive(entries);
}

async function inputDir(t) {
  const dir = await mkdtemp(join(tmpdir(), 'online-gal-manager-artifact-'));
  t.after(() => rm(dir, { recursive: true, force: true }));
  return dir;
}

test('manager archive rejects broken files, reserved names, path conflicts and API mismatch', () => {
  assert.throws(() => inspectManagerArtifact(fixture({ mutate: (entries) => entries.set('index.html', Buffer.from('changed')) })), /does not match archive/);
  assert.throws(() => inspectManagerArtifact(fixture({ mutate: (entries) => entries.delete('index.html') })), /missing from archive/);
  assert.throws(() => inspectManagerArtifact(fixture({ assets: new Map([['REGISTRY/index.json', json({})]]) })), /reserved site path/);
  assert.throws(() => inspectManagerArtifact(fixture({ assets: new Map([['Foo/dir', json({})], ['foo', json({})]]) })), /path conflicts/);
  assert.throws(() => inspectManagerArtifact(fixture({ apiVersion: 999 })), /targets plugin API 999/);
  assert.throws(() => createArchive(new Map([['../outside', json({})]])), /unsafe archive path/);
});

test('site artifact assembly needs no Astro output and preserves registry manifest and payload bytes', async (t) => {
  const dir = await inputDir(t);
  const archivePath = join(dir, 'manager.tar.gz');
  const out = join(dir, 'site');
  const archive = fixture({ assets: new Map([['_astro/app.css', Buffer.from('body { color: black }')]]) });
  const manifest = inspectManagerArtifact(archive);
  const preload = 'data:text/javascript,' + encodeURIComponent('globalThis.fetch = async () => { throw new Error("CA unreachable"); };');
  const build = (digest, token = '') => spawnSync(process.execPath, [
    '--import', preload, buildSite, '--manager-artifact', '--manager-archive', archivePath, '--manager-sha256', digest, '--out', out,
  ], { env: { ...process.env, CF_PAGES: '', ONLINE_GAL_CA_BUILD_TOKEN: token }, cwd: dir, encoding: 'utf8', timeout: 15_000 });
  const missing = build(sha256(archive));
  assert.equal(missing.status, 1);
  assert.match(missing.stderr, /ENOENT/);
  await writeFile(archivePath, archive);
  const wrongHash = build('1'.repeat(64));
  assert.equal(wrongHash.status, 1);
  assert.match(wrongHash.stderr, /does not match its locked SHA-256/);

  const run = build(sha256(archive));
  assert.equal(run.status, 0, run.stderr);
  const registry = JSON.parse(await readFile(join(out, 'registry/index.json'), 'utf8'));
  const { digest, path } = registry.plugins[appManifest.id];
  assert.equal(digest, manifest.manifestSha256);
  const packaged = join(out, 'registry', path);
  assert.deepEqual(await readFile(join(packaged, 'mf-manifest.json')), manifest.manifestBytes);
  assert.deepEqual(await readFile(join(packaged, 'plugin.json')), manifest.entries.get('plugin.json'));
  for (const file of manifest.package.files) {
    assert.equal(sha256(await readFile(join(packaged, file.path))), file.sha256);
  }
  assert.deepEqual(await readFile(join(out, '_astro/app.css')), manifest.entries.get('_astro/app.css'));
  const rootHtml = await readFile(join(out, 'index.html'), 'utf8');
  assert.match(rootHtml, /^<body>K21 fixture<script>.*<\/script><\/body>$/s);
  assert.match(rootHtml, /register\('\/sw.js'/);
  assert.deepEqual(JSON.parse(await readFile(join(out, '_routes.json'), 'utf8')).include, ['/__ca/*', '/__cf/*']);

  const token = randomBytes(32).toString('base64url');
  const signingFailure = build(sha256(archive), token);
  assert.equal(signingFailure.status, 1);
  assert.match(signingFailure.stderr, /micro-CA request failed/);
  assert.equal(signingFailure.stderr.includes(token), false);
});

test('locked GitHub download withholds its token from the asset host', async (t) => {
  const dir = await inputDir(t);
  const lockPath = join(dir, 'manager.lock.json');
  const archive = fixture();
  const releaseTag = 'manager-v0.1.0-k21-test';
  const asset = 'online-gal.manager-0.1.0_fixture.tar.gz';
  const token = 'fixture-token';
  const lock = { format: 1, repository: 'owner/repo', releaseTag, asset, archiveSha256: sha256(archive), sourceSha: 'a'.repeat(40) };
  await writeFile(lockPath, json(lock));
  const fetcher = (host) => async (url, options) => {
    if (String(url).includes('/releases/tags/')) {
      assert.equal(options.headers.authorization, `Bearer ${token}`);
      return Response.json({ tag_name: releaseTag, assets: [{ name: asset, id: 1 }] });
    }
    if (String(url).includes('/releases/assets/1')) {
      assert.equal(options.headers.authorization, `Bearer ${token}`);
      assert.equal(options.redirect, 'manual');
      return Response.redirect(`https://${host}/download`);
    }
    assert.equal(String(url), `https://${host}/download`);
    assert.equal(options.headers, undefined);
    return new Response(archive);
  };
  assert.deepEqual(await fetchManagerArtifact({ lockPath, token, fetcher: fetcher('release-assets.githubusercontent.com') }), archive);
  await assert.rejects(fetchManagerArtifact({ lockPath, token, fetcher: fetcher('outside.invalid') }), /outside the allowed download hosts/);
  await writeFile(lockPath, json({ ...lock, archiveSha256: '0'.repeat(64) }));
  await assert.rejects(fetchManagerArtifact({ lockPath, token, fetcher: fetcher('release-assets.githubusercontent.com') }), /does not match its locked SHA-256/);
});
