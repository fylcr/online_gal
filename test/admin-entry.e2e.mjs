import assert from 'node:assert/strict';
import { existsSync } from 'node:fs';
import { readFile, stat } from 'node:fs/promises';
import { createServer } from 'node:http';
import { extname, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';
import { EDGE_PATH, launchEdge, sleep } from '../ports/krkr/kirikiroid2/browser-test/cdp.mjs';

const dist = fileURLToPath(new URL('../apps/app/dist/', import.meta.url));
const prefix = '/__kernel/api/account/cloudflare/';
const segment = 'A'.repeat(43);
const types = { '.html': 'text/html; charset=utf-8', '.js': 'text/javascript', '.css': 'text/css', '.svg': 'image/svg+xml', '.webp': 'image/webp' };
const worker = `self.addEventListener('install', () => self.skipWaiting());
self.addEventListener('activate', event => event.waitUntil(self.clients.claim()));`;

async function listen(server) {
  await new Promise((done, reject) => {
    server.once('error', reject);
    server.listen(0, '127.0.0.1', done);
  });
  return server.address().port;
}

async function pageShows(cdp, message) {
  const expr = `document.body?.innerText.includes(${JSON.stringify(message)})`;
  const end = Date.now() + 8000;
  while (Date.now() < end) {
    try { if (await cdp.evaluate(expr)) return; } catch { /* Navigation may end the previous context. */ }
    await sleep(80);
  }
  assert.fail(`Page did not show ${message}`);
}

test('official admin page imports once on controller acquisition and reflects refresh and logout', {
  skip: !existsSync(EDGE_PATH) || !existsSync(resolve(dist, 'admin/index.html')),
  timeout: 50_000,
}, async (t) => {
  let imported = false;
  let signedIn = false;
  let failFirstImport = true;
  let siteAdminError;
  const requests = [];
  const status = () => signedIn
    ? { signedIn: true, user: { email: 'owner@example.org' }, scopes: [], expiresAt: Date.now() + 60_000,
      siteAdmin: imported && !siteAdminError ? { projectName: 'fixture-project', accountName: 'Fixture' } : null,
      siteAdminError: siteAdminError ?? (imported ? undefined : 'entry-required') }
    : { signedIn: false, pending: null };
  const server = createServer(async (request, response) => {
    const { pathname } = new URL(request.url, 'http://127.0.0.1');
    try {
      if (pathname === '/fixture-sw.js') {
        response.writeHead(200, { 'content-type': 'text/javascript' }).end(worker);
        return;
      }
      if (pathname.startsWith(prefix)) {
        const action = pathname.slice(prefix.length);
        requests.push(action);
        let result = { ok: true };
        if (action === 'import-entry') {
          let body = '';
          for await (const chunk of request) body += chunk;
          if (request.method !== 'POST' || JSON.parse(body).segment !== segment) throw new Error('Bad import');
          if (failFirstImport) {
            failFirstImport = false;
            response.writeHead(503, { 'content-type': 'application/json' }).end('{}');
            return;
          }
          imported = true;
        }
        if (action === 'login') result.login = { userCode: 'FIXTURE-CODE', verificationUri: 'https://example.org/', expiresAt: Date.now() + 60_000, interval: 1 };
        if (action === 'poll') {
          signedIn = true;
          result.poll = { state: 'signed-in', status: status() };
        }
        if (action === 'logout') signedIn = false;
        if (action === 'status' || action === 'logout') result.status = status();
        response.writeHead(200, { 'content-type': 'application/json' }).end(JSON.stringify(result));
        return;
      }
      const root = resolve(dist);
      let file = resolve(root, '.' + pathname);
      if (file !== root && !file.startsWith(root + sep)) throw new Error('Bad path');
      if ((await stat(file)).isDirectory()) file = resolve(file, 'index.html');
      response.writeHead(200, { 'content-type': types[extname(file)] ?? 'application/octet-stream' }).end(await readFile(file));
    } catch {
      response.writeHead(404).end();
    }
  });
  const port = await listen(server);
  t.after(() => { server.closeAllConnections(); server.close(); });
  const debugPortProbe = createServer();
  const debugPort = await listen(debugPortProbe);
  await new Promise(done => debugPortProbe.close(done));
  const browser = await launchEdge({ port: debugPort, enable: ['Page'] });
  t.after(() => browser.stop());
  const { cdp } = browser;
  const base = `http://127.0.0.1:${port}`;
  const navigate = path => cdp.send('Page.navigate', { url: base + path });
  const click = text => cdp.evaluate(`Array.from(document.querySelectorAll('button')).find(button => button.innerText.trim() === ${JSON.stringify(text)}).click()`);

  await navigate(`/admin?source=fixture#cf-entry=${segment}`);
  await pageShows(cdp, '插件系统未就绪');
  assert.equal(requests.length, 0);
  assert.equal(await cdp.evaluate('location.hash.length'), 53);
  await cdp.evaluate("navigator.serviceWorker.register('/fixture-sw.js').then(() => true)");
  await pageShows(cdp, '无法导入管理入口');
  assert.equal(await cdp.evaluate('location.hash.length'), 53);
  await click('重试');
  await pageShows(cdp, '未登录 Cloudflare');
  assert.deepEqual(requests.slice(0, 3), ['import-entry', 'import-entry', 'status']);
  assert.equal(await cdp.evaluate('location.hash + location.search'), '?source=fixture');

  await navigate('/settings/account');
  await pageShows(cdp, '登录 Cloudflare');
  await click('登录 Cloudflare');
  await pageShows(cdp, 'fixture-project');
  assert.ok(requests.includes('poll'));
  await navigate('/admin?source=fixture');
  await pageShows(cdp, 'fixture-project');
  await cdp.send('Page.reload');
  await pageShows(cdp, 'fixture-project');
  for (const [code, label] of [
    ['entry-required', '尚未导入管理入口'], ['entry-invalid', '管理入口已失效'],
    ['not-admin', '不是本站管理员'], ['temporarily-unavailable', 'Cloudflare 管理功能暂不可用'],
  ]) {
    siteAdminError = code;
    await cdp.send('Page.reload');
    await pageShows(cdp, label);
  }
  siteAdminError = undefined;
  await navigate('/settings/account');
  await pageShows(cdp, 'owner@example.org');
  await click('退出');
  await pageShows(cdp, '登录 Cloudflare');
  await navigate('/admin');
  await pageShows(cdp, '未登录 Cloudflare');
  assert.equal(requests.filter(action => action === 'import-entry').length, 2);
});
