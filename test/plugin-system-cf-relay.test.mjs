import assert from 'node:assert/strict';
import { randomBytes } from 'node:crypto';
import { test } from 'node:test';
import { handle, handleSetup } from '../functions/__cf/[[path]].ts';
import { readCfRouteConfig } from '../functions/__cf/route-config.mjs';
import { CloudflareAccount, MemoryVaultStore, Vault } from '../packages/plugin-system/dist/account/index.js';
import { Kernel, routeRequest } from '../packages/plugin-system/dist/kernel.js';

const host = 'relay.example.invalid';
const origin = `https://${host}`;
const managerId = 'online-gal.manager';
const accountId = 'a'.repeat(32);
const ownerId = 'b'.repeat(32);
const projectName = 'relay-fixture';
const accountPath = '/__kernel/api/account/cloudflare/';
const api = 'https://api.cloudflare.com/client/v4/';
const oauth = 'https://dash.cloudflare.com/oauth2/';
const projectPath = `accounts/${accountId}/pages/projects/${projectName}`;
const logical = '/__cf/api/zones?limit=1';
const secret = () => randomBytes(32).toString('base64url');

async function fixture({ collaborator = false } = {}) {
  let now = 1_800_000_000_000;
  let env = {
    CF_PAGES: '1', CF_PAGES_BRANCH: 'main', ONLINE_GAL_CF_ROUTE_KEY: secret(), ONLINE_GAL_CF_OWNER_ID: ownerId,
    ONLINE_GAL_SITE: JSON.stringify({ v: 1, account: accountId, project: projectName, productionBranch: 'main', cfRoute: { v: 1, host } }),
  };
  let { segment } = await readCfRouteConfig(env);
  const calls = [];
  const network = [];
  const secrets = [];
  const access = [secret(), secret()];
  const refresh = secret();
  const device = secret();
  secrets.push(...access, refresh, device, segment);
  let current = 0;
  let invalidateProof = false;
  let rejectSession = false;
  let blockUser;
  const store = new MemoryVaultStore();
  const upstream = async (url, init) => {
    calls.push(url);
    const headers = new Headers(init.headers);
    assert.equal(headers.has('x-online-gal-cf-proof'), false);
    if (url === `${oauth}device/auth`) {
      return Response.json({ device_code: device, user_code: 'TEST-FIXTURE', verification_uri: 'https://dash.cloudflare.com/activate', expires_in: 300 });
    }
    if (url === `${oauth}token`) {
      const body = new URLSearchParams(await new Response(init.body).text());
      if (body.get('grant_type') === 'refresh_token') {
        assert.equal(body.get('refresh_token') === refresh, true);
        current++;
      } else assert.equal(body.get('device_code') === device, true);
      return Response.json({ access_token: access[current], refresh_token: refresh, expires_in: 300, scope: 'pages:write' });
    }
    if (url === `${oauth}revoke`) {
      assert.equal(new URLSearchParams(await new Response(init.body).text()).get('token') === refresh, true);
      return Response.json({});
    }
    assert.equal(headers.get('authorization') === `Bearer ${access[current]}`, true);
    if (url === `${api}user`) {
      if (blockUser !== undefined) await blockUser();
      if (rejectSession) return Response.json({ success: false }, { status: 401 });
      return Response.json({ success: true, result: { id: collaborator ? 'c'.repeat(32) : ownerId } });
    }
    if (url === `${api}${projectPath}`) return Response.json({ success: true, result: { name: projectName, domains: [host] } });
    if (url === `${api}accounts?page=1`) return Response.json({ success: true, result: [{ id: accountId, name: 'fixture' }] });
    if (url === `${api}accounts/${accountId}/pages/projects?page=1`) {
      return Response.json({ success: true, result: [{ name: projectName, domains: [host] }] });
    }
    if (url === `${api}zones?limit=1`) return Response.json({ success: true, result: [] });
    assert.fail('unexpected upstream operation');
  };
  const fetcher = async (url, init) => {
    const target = new URL(url);
    assert.equal(target.origin, origin);
    const path = target.pathname;
    const isPrivate = path.startsWith(`/__cf/${segment}/`);
    network.push(isPrivate ? path.slice(`/__cf/${segment}`.length) : 'fixed-or-obsolete');
    const headers = new Headers(init?.headers);
    headers.set('sec-fetch-site', 'same-origin');
    const proof = headers.get('x-online-gal-cf-proof');
    if (proof !== null) secrets.push(proof);
    if (isPrivate && invalidateProof && path.endsWith('/api/zones')) headers.delete('x-online-gal-cf-proof');
    const request = new Request(url, { ...init, headers });
    if (isPrivate) return handle(request, env, upstream, () => now);
    if (path.startsWith('/__cf/oauth/') || path.startsWith('/__cf/api/')) return handleSetup(request, upstream);
    return new Response('<!doctype html>fixture', { headers: { 'content-type': 'text/html;charset=UTF-8' } });
  };
  const kernel = (official = true) => new Kernel({
    origin, defaultManager: managerId, kernelVersion: 'test', now: () => now, fetch: fetcher, vault: store,
    clientUrl: async id => ({ manager: `${origin}/admin`, plugin: `${origin}/__plugins/id/main`, kernel: `${origin}/__kernel`, foreign: 'https://other.invalid/admin' })[id],
    caches: {
      open: async () => ({ match: async () => Response.json({ format: 1, activeManager: managerId, lastUpdateCheck: 0,
        plugins: { [managerId]: { source: official ? `official:${managerId}` : 'github:example/manager@v1' } },
      }) }),
      keys: async () => [],
    },
  });
  const send = async (subject, path, { method = 'GET', body, client = 'manager', navigate = false } = {}) => {
    const req = new Request(`${origin}${path}`, { method, headers: {
      'content-type': 'application/json', authorization: 'Bearer page-supplied', 'x-online-gal-cf-proof': 'page-supplied',
    }, ...(body === undefined ? {} : { body: JSON.stringify(body) }) });
    const request = navigate ? { url: req.url, method: req.method, mode: 'navigate', headers: req.headers } : req;
    const route = subject.route(request);
    assert.ok(route);
    const response = await subject.respond(request, route, client);
    const text = await response.text();
    assert.equal(secrets.some(value => text.includes(value)), false, 'response disclosed private material');
    assert.equal(response.headers.has('x-online-gal-cf-proof'), false);
    return { status: response.status, body: JSON.parse(text) };
  };
  const action = (subject, name, body) => send(subject, `${accountPath}${name}`, { method: 'POST', body });
  const status = async subject => (await send(subject, `${accountPath}status`)).body.status;
  return {
    calls, network, store, fetcher, kernel, send, action, status, now: () => now, advance: ms => { now += ms; },
    setInvalidProof: value => { invalidateProof = value; }, setRejectSession: value => { rejectSession = value; },
    setBlockUser: value => { blockUser = value; }, get segment() { return segment; },
    async rotate() {
      env = { ...env, ONLINE_GAL_CF_ROUTE_KEY: secret() };
      ({ segment } = await readCfRouteConfig(env));
      secrets.push(segment);
    },
  };
}

test('official manager imports an encrypted entry and keeps it across worker restarts and logout', async () => {
  const f = await fixture();
  let k = f.kernel();
  assert.equal((await f.action(k, 'login')).body.error.code, 'entry-required');
  assert.equal((await f.action(k, 'import-entry', { segment: 'invalid' })).status, 400);
  assert.equal((await f.action(k, 'import-entry', { segment: f.segment })).status, 200);
  const record = f.store.records.get('cloudflare#entry');
  assert.ok(record && !Buffer.from(record.ct).includes(f.segment));
  assert.equal((await f.action(k, 'login')).body.login.userCode, 'TEST-FIXTURE');
  k = f.kernel();
  const signedIn = (await f.action(k, 'poll')).body.poll.status;
  assert.equal(signedIn.signedIn, true);
  assert.equal(signedIn.siteAdmin.projectName, projectName);
  assert.deepEqual(f.calls, [`${oauth}device/auth`, `${oauth}token`, `${api}user`, `${api}${projectPath}`]);
  assert.equal((await f.send(k, logical)).status, 200);
  assert.equal((await f.status(k)).siteAdmin.projectName, projectName);
  assert.deepEqual(f.network, ['/oauth/device/auth', '/oauth/token', '/site-admin', '/api/zones']);

  f.advance(250_000);
  assert.equal((await f.send(k, logical)).status, 200);
  assert.deepEqual(f.calls.slice(-4), [`${oauth}token`, `${api}user`, `${api}${projectPath}`, `${api}zones?limit=1`]);
  const calls = f.calls.length;
  f.setInvalidProof(true);
  assert.equal((await f.send(k, logical)).body.error.code, 'proof-invalid');
  assert.equal(f.calls.length, calls);
  f.setInvalidProof(false);
  assert.equal((await f.send(k, logical)).status, 200);
  assert.deepEqual(f.calls.slice(-3), [`${api}user`, `${api}${projectPath}`, `${api}zones?limit=1`]);

  await f.rotate();
  assert.equal((await f.send(k, logical)).body.error.code, 'entry-invalid');
  assert.equal((await f.status(k)).siteAdminError, 'entry-invalid');
  assert.equal((await f.action(k, 'import-entry', { segment: f.segment })).status, 200);
  k = f.kernel();
  assert.equal((await f.send(k, logical)).status, 200);
  assert.deepEqual(f.calls.slice(-3), [`${api}user`, `${api}${projectPath}`, `${api}zones?limit=1`]);
  assert.equal((await f.action(k, 'logout')).body.status.signedIn, false);
  assert.equal(f.calls.at(-1), `${oauth}revoke`);
  assert.equal(f.store.records.has('cloudflare'), false);
  assert.equal(f.store.records.has('cloudflare#entry'), true);
  assert.equal((await f.send(k, logical)).status, 401);
});

test('collaborator cannot turn project access into a forwarding proof', async () => {
  const f = await fixture({ collaborator: true });
  const k = f.kernel();
  await f.action(k, 'import-entry', { segment: f.segment });
  await f.action(k, 'login');
  const status = (await f.action(k, 'poll')).body.poll.status;
  assert.equal(status.signedIn, true);
  assert.equal(status.siteAdmin, null);
  assert.equal(status.siteAdminError, 'not-admin');
  assert.equal((await f.send(k, logical)).body.error.code, 'not-admin');
  assert.deepEqual(f.calls, [`${oauth}device/auth`, `${oauth}token`, `${api}user`, `${api}user`]);
});

test('the worker rejects unrelated clients, network path calls and navigations before fetching', async () => {
  const f = await fixture();
  const k = f.kernel();
  for (const [subject, client] of [[k, ''], [k, 'missing'], [k, 'plugin'], [k, 'kernel'], [k, 'foreign'], [f.kernel(false), 'manager']]) {
    for (const [path, options] of [[logical, {}], [`${accountPath}import-entry`, { method: 'POST', body: { segment: f.segment } }]]) {
      assert.equal((await f.send(subject, path, { ...options, client })).body.error.code, 'account-forbidden');
    }
  }
  for (const path of ['/__cf', '/__cf/oauth/token', `/__cf/${f.segment}/api/user`]) {
    assert.equal((await f.send(k, path)).body.error.code, 'account-forbidden');
  }
  assert.equal((await f.send(k, logical, { navigate: true })).status, 403);
  for (const path of ['/__ca/issue', '/registry/index.json', '/registry/p/a/b/plugin.bundle.json', '/sw.js']) {
    assert.equal(routeRequest(new URL(path, origin), 'GET', origin), null);
  }
  assert.deepEqual(f.network, []);
});

test('setup keeps the fixed relay before handoff and a supplied entry in memory', async () => {
  const f = await fixture();
  const setup = new CloudflareAccount({ origin, siteHost: host, now: f.now, fetch: f.fetcher, vault: new Vault(f.store) });
  await setup.login();
  assert.equal((await setup.poll()).status.siteAdmin.projectName, projectName);
  assert.ok(f.network.every(path => path === 'fixed-or-obsolete'));
  await setup.setEntry(f.segment);
  assert.equal(f.store.records.has('cloudflare#entry'), false);
  assert.equal((await setup.status()).siteAdmin.projectName, projectName);
  assert.equal(f.network.at(-1), '/site-admin');
});

test('an in-flight probe cannot restore administrator access after logout', async () => {
  const f = await fixture();
  let k = f.kernel();
  await f.action(k, 'import-entry', { segment: f.segment });
  await f.action(k, 'login');
  await f.action(k, 'poll');
  k = f.kernel();
  let release;
  let reached;
  const wait = new Promise(resolve => { release = resolve; });
  const ready = new Promise(resolve => { reached = resolve; });
  f.setBlockUser(async () => { reached(); await wait; });
  const pending = f.status(k);
  await ready;
  assert.equal((await f.action(k, 'logout')).body.status.signedIn, false);
  release();
  assert.equal((await pending).signedIn, false);
  assert.equal((await f.send(k, logical)).status, 401);
});

test('a rejected upstream session clears the credential without deleting the entry', async () => {
  const f = await fixture();
  const k = f.kernel();
  await f.action(k, 'import-entry', { segment: f.segment });
  await f.action(k, 'login');
  await f.action(k, 'poll');
  f.setRejectSession(true);
  assert.equal((await f.status(f.kernel())).signedIn, false);
  assert.equal(f.store.records.has('cloudflare'), false);
  assert.equal(f.store.records.has('cloudflare#entry'), true);
});
