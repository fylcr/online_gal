import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { createServer } from 'node:http';
import { mkdir, mkdtemp, readFile, rm, stat, writeFile } from 'node:fs/promises';
import { extname, join, resolve, sep } from 'node:path';
import test from 'node:test';
import { fileURLToPath, pathToFileURL } from 'node:url';

import { packDirectory } from '../packages/plugin-system/scripts/pack-plugin.mjs';

const root = fileURLToPath(new URL('..', import.meta.url));
const { K21_PLAYWRIGHT, K21_CHROMIUM, ONLINE_GAL_MANAGER_ARCHIVE, ONLINE_GAL_MANAGER_SHA256 } = process.env;
const required = K21_PLAYWRIGHT && K21_CHROMIUM && ONLINE_GAL_MANAGER_ARCHIVE && ONLINE_GAL_MANAGER_SHA256;
const script = join(root, 'packages/plugin-system/scripts/build-site.mjs');
const mime = {
  '.html': 'text/html; charset=utf-8', '.js': 'text/javascript', '.css': 'text/css', '.json': 'application/json',
  '.svg': 'image/svg+xml', '.webp': 'image/webp', '.png': 'image/png', '.wasm': 'application/wasm',
};

async function startSite(site, raw) {
  const requests = [];
  const server = createServer(async (request, response) => {
    const path = new URL(request.url, 'http://localhost').pathname;
    requests.push(path);
    let file;
    let contents;
    try {
      const base = path.startsWith('/raw/') ? raw : site;
      const relativePath = decodeURIComponent(path.slice(base === raw ? 5 : 1));
      file = resolve(base, relativePath);
      if (file !== base && !file.startsWith(`${base}${sep}`)) {
        response.writeHead(403).end();
        return;
      }
      if ((await stat(file)).isDirectory()) file = join(file, 'index.html');
      contents = await readFile(file);
    } catch {
      // Emulate Pages' SPA fallback for direct navigation to an unknown path.
      if (request.headers['sec-fetch-mode'] !== 'navigate' || path.startsWith('/raw/')) {
        response.writeHead(404).end();
        return;
      }
      file = join(site, 'index.html');
      contents = await readFile(file);
    }
    const headers = await readFile(join(site, '_headers'), 'utf8');
    for (const line of headers.split('\n').slice(1)) {
      if (!line.startsWith('  ')) break;
      const colon = line.indexOf(':');
      response.setHeader(line.slice(0, colon).trim(), line.slice(colon + 1).trim());
    }
    response.writeHead(200, { 'content-type': mime[extname(file)] ?? 'application/octet-stream', 'cache-control': 'no-cache' });
    response.end(contents);
  });
  await new Promise((resolveListen, reject) => {
    server.once('error', reject);
    server.listen(0, '127.0.0.1', resolveListen);
  });
  return { server, origin: `http://127.0.0.1:${server.address().port}`, requests };
}

async function createThirdPartyManager(source, raw) {
  const html = '<!doctype html><title>K21 third-party</title><h1>K21 third-party manager</h1>';
  await mkdir(source);
  await writeFile(join(source, 'index.html'), html);
  const { envelope, bytes } = await packDirectory({ dir: source, manifest: {
    id: 'example.manager', name: 'Third-party fixture', version: '1.0.0', apiVersion: 1, role: 'manager', provenance: 'third-party',
  } });
  const files = join(raw, 'owner/repo/refs/tags/v1');
  await mkdir(files, { recursive: true });
  await Promise.all([
    writeFile(join(files, 'index.html'), html), writeFile(join(files, 'plugin.json'), envelope), writeFile(join(files, 'mf-manifest.json'), bytes),
  ]);
}

test('assembled manager retains deep links, offline, and one-guarded hard reload', {
  skip: !required && 'set K21_PLAYWRIGHT, K21_CHROMIUM, ONLINE_GAL_MANAGER_ARCHIVE and ONLINE_GAL_MANAGER_SHA256', timeout: 60_000,
}, async (t) => {
  const workRoot = join(root, '.test');
  await mkdir(workRoot, { recursive: true });
  const work = await mkdtemp(join(workRoot, 'manager-browser-'));
  t.after(() => rm(work, { recursive: true, force: true }));
  const site = join(work, 'site');
  const raw = join(work, 'raw');
  await createThirdPartyManager(join(work, 'third-party'), raw);
  const { server, origin, requests } = await startSite(site, raw);
  t.after(() => {
    server.closeAllConnections();
    return new Promise((done) => server.close(done));
  });
  execFileSync(process.execPath, [script, '--manager-artifact', '--manager-archive', resolve(ONLINE_GAL_MANAGER_ARCHIVE),
    '--manager-sha256', ONLINE_GAL_MANAGER_SHA256, '--out', site, '--github-raw', `${origin}/raw/`], {
    cwd: root, timeout: 15_000, env: { ...process.env, CF_PAGES: '', ONLINE_GAL_CA_BUILD_TOKEN: '' },
  });

  const { chromium } = await import(pathToFileURL(join(K21_PLAYWRIGHT, 'index.mjs')).href);
  const browser = await chromium.launch({ executablePath: K21_CHROMIUM, headless: true });
  t.after(() => browser.close());
  const context = await browser.newContext();
  const page = await context.newPage();
  const documents = [];
  page.on('response', (response) => {
    if (response.request().isNavigationRequest()) documents.push(response.fromServiceWorker());
  });
  const first = await page.goto(`${origin}/settings/`);
  assert.equal(first.status(), 200);
  assert.equal(first.fromServiceWorker(), false);
  assert.equal(first.headers()['cross-origin-embedder-policy'], 'credentialless');
  assert.match(await page.title(), /设置/);
  assert.equal(await page.evaluate(() => crossOriginIsolated), true);
  await page.waitForFunction(() => Boolean(navigator.serviceWorker.controller));
  const reload = await page.reload();
  assert.equal(reload.fromServiceWorker(), true);
  assert.equal(reload.headers()['cross-origin-opener-policy'], 'same-origin');
  assert.equal(await page.evaluate(() => crossOriginIsolated), true);

  await context.setOffline(true);
  const offline = await page.reload();
  assert.equal(offline.fromServiceWorker(), true);
  assert.match(await page.title(), /设置/);
  await context.setOffline(false);

  const installed = await page.evaluate(async () => {
    const response = await fetch('/__kernel/api/install', {
      method: 'POST', headers: { 'content-type': 'application/json' },
      body: JSON.stringify({ source: 'github:owner/repo@v1', activate: true }),
    });
    return { status: response.status, body: await response.json() };
  });
  assert.equal(installed.status, 200, JSON.stringify(installed.body));
  assert.equal(installed.body.plugin.id, 'example.manager');
  await page.goto(`${origin}/`);
  await page.getByRole('heading', { name: 'K21 third-party manager' }).waitFor();
  await page.evaluate(() => sessionStorage.removeItem('online-gal.reload'));
  const cdp = await context.newCDPSession(page);
  await cdp.send('Page.enable');
  documents.length = 0;
  await cdp.send('Page.reload', { ignoreCache: true });
  await page.waitForFunction(() => Boolean(sessionStorage.getItem('online-gal.reload')));
  await page.getByRole('heading', { name: 'K21 third-party manager' }).waitFor();
  assert.deepEqual(documents, [false, true]);

  documents.length = 0;
  const wait = page.waitForEvent('load');
  await cdp.send('Page.reload', { ignoreCache: true });
  await wait;
  await page.evaluate(() => new Promise((done) => {
    navigator.serviceWorker.ready.then(() => setTimeout(done, 500));
  }));
  assert.deepEqual(documents, [false]);
  assert.match(await page.title(), /online_gal/);
  assert.ok(requests.some((path) => path.startsWith('/registry/p/online-gal.manager/')));
});
