import assert from 'node:assert/strict';
import { test } from 'node:test';
import { handle, handleSetup } from '../functions/__cf/[[path]].ts';
import { encodeBase64Url, readCfRouteConfig } from '../functions/__cf/route-config.mjs';

const accountId = 'a'.repeat(32);
const ownerUserId = 'b'.repeat(32);
const projectName = 'isolated-fixture';
const host = 'admin.example.invalid';
const origin = `https://${host}`;
const token = 'fixture-owner-token';
const now = 1800000000000;
const routeKey = (byte) => encodeBase64Url(new Uint8Array(32).fill(byte));
const env = {
  CF_PAGES: '1', CF_PAGES_BRANCH: 'main', ONLINE_GAL_CF_ROUTE_KEY: routeKey(1), ONLINE_GAL_CF_OWNER_ID: ownerUserId,
  ONLINE_GAL_SITE: JSON.stringify({ v: 1, account: accountId, project: projectName, productionBranch: 'main', cfRoute: { v: 1, host } }),
};
const { segment } = await readCfRouteConfig(env);
const prefix = `/__cf/${segment}`;
const apiBase = 'https://api.cloudflare.com/client/v4/';
const projectPath = `accounts/${accountId}/pages/projects/${projectName}`;

function request(path, { url = origin, credential = token, proof, secFetchSite = 'same-origin', method = 'GET' } = {}) {
  return new Request(`${url}${path}`, { method, headers: {
    'sec-fetch-site': secFetchSite, ...(credential ? { authorization: `Bearer ${credential}` } : {}),
    ...(proof ? { 'x-online-gal-cf-proof': proof } : {}),
  } });
}

async function expectError(response, status, code) {
  assert.equal(response.status, status);
  assert.deepEqual(await response.json(), { ok: false, code });
  assert.equal(response.headers.get('cache-control'), 'no-store');
}

function upstream({ user = ownerUserId, project = { name: projectName, domains: [host] }, status = 200 } = {}) {
  const calls = [];
  return { calls, fetch: async (url, init) => {
    calls.push(url);
    assert.equal(init.headers.get?.('x-online-gal-cf-proof'), init.headers instanceof Headers ? null : undefined);
    if (url === `${apiBase}user`) return Response.json({ success: status === 200, result: { id: user } }, { status });
    if (url === `${apiBase}${projectPath}`) return Response.json({ success: true, result: project });
    return Response.json({ success: true }, { headers: { 'set-cookie': 'fixture=1' } });
  } };
}

test('build and runtime share one derived route; preview and invalid owner remain disabled', async () => {
  assert.equal(await readCfRouteConfig({ ...env, CF_PAGES_BRANCH: 'preview' }), null);
  assert.equal(await readCfRouteConfig({ ...env, CF_PAGES: '' }), null);
  assert.equal(await readCfRouteConfig({ CF_PAGES: '1', CF_PAGES_BRANCH: 'main' }), null);
  await assert.rejects(readCfRouteConfig({ ...env, ONLINE_GAL_CF_OWNER_ID: '' }), /Invalid Cloudflare Pages route configuration/);
  const otherOwner = await readCfRouteConfig({ ...env, ONLINE_GAL_CF_OWNER_ID: 'c'.repeat(32) });
  assert.equal(otherOwner.segment, segment);
});

test('private Function rejects obsolete entry, wrong host, invalid config and cross-origin before upstream', async () => {
  const mock = upstream();
  for (const item of [request('/__cf/api/user'), request(`${prefix}bad/site-admin`),
    request(`${prefix}/site-admin`, { url: `https://${projectName}.pages.dev` }),
    request(`${prefix}/site-admin`, { url: `http://${host}` }),
    request(`${prefix}/site-admin`, { url: `${origin}:8443` })]) {
    await expectError(await handle(item, env, mock.fetch), 404, 'entry-invalid');
  }
  await expectError(await handle(request(`${prefix}/site-admin`), { ...env, ONLINE_GAL_CF_OWNER_ID: undefined }, mock.fetch), 404, 'entry-invalid');
  await expectError(await handle(request(`${prefix}/site-admin`, { secFetchSite: 'cross-site' }), env, mock.fetch), 403, 'forbidden');
  await expectError(await handle(request(`${prefix}/site-admin`, { method: 'POST' }), env, mock.fetch), 405, 'method-not-allowed');
  await expectError(await handle(request(`${prefix}/site-admin`, { credential: null }), env, mock.fetch), 401, 'session-expired');
  assert.deepEqual(mock.calls, []);
});

test('Cloudflare user identity is checked before querying the single bound project', async () => {
  for (const [status, code] of [[401, 'session-expired'], [403, 'not-admin'], [429, 'temporarily-unavailable']]) {
    const mock = upstream({ status });
    await expectError(await handle(request(`${prefix}/site-admin`), env, mock.fetch), status === 429 ? 503 : status, code);
    assert.deepEqual(mock.calls, [`${apiBase}user`]);
  }
  const collaborator = upstream({ user: 'c'.repeat(32) });
  await expectError(await handle(request(`${prefix}/site-admin`), env, collaborator.fetch), 403, 'not-admin');
  assert.deepEqual(collaborator.calls, [`${apiBase}user`]);
  for (const project of [{ name: 'wrong', domains: [host] }, { name: projectName, domains: ['elsewhere.example.invalid'] }]) {
    const mock = upstream({ project });
    await expectError(await handle(request(`${prefix}/site-admin`), env, mock.fetch), 403, 'not-admin');
    assert.deepEqual(mock.calls, [`${apiBase}user`, `${apiBase}${projectPath}`]);
  }
});

test('proof pins entry, owner, host and token, expires after ten minutes and is never forwarded', async () => {
  const mock = upstream();
  const response = await handle(request(`${prefix}/site-admin`), env, mock.fetch, () => now);
  assert.equal(response.status, 200);
  assert.equal(response.headers.get('cache-control'), 'no-store');
  const { ok, admin, expiresAt, proof } = await response.json();
  assert.equal(ok, true);
  assert.deepEqual(admin, { accountId, accountName: accountId, projectName });
  assert.equal(expiresAt, now + 600_000);
  const api = `${prefix}/api/zones?account.id=${accountId}`;
  for (const [options, time] of [[{}, now], [{ proof: `${proof}x` }, now], [{ proof, credential: 'refreshed' }, now], [{ proof }, expiresAt]]) {
    await expectError(await handle(request(api, options), env, mock.fetch, () => time), 403, 'proof-invalid');
  }
  await expectError(await handle(request(api, { proof }), { ...env, ONLINE_GAL_CF_OWNER_ID: 'c'.repeat(32) }, mock.fetch, () => now), 403, 'proof-invalid');
  const rotated = { ...env, ONLINE_GAL_CF_ROUTE_KEY: routeKey(2) };
  const newEntry = (await readCfRouteConfig(rotated)).segment;
  await expectError(await handle(request(api, { proof }), rotated, mock.fetch, () => now), 404, 'entry-invalid');
  await expectError(await handle(request(`/__cf/${newEntry}/api/zones`, { proof }), rotated, mock.fetch, () => now), 403, 'proof-invalid');
  const proxied = await handle(request(api, { proof }), env, mock.fetch, () => now);
  assert.equal(proxied.status, 200);
  assert.equal(proxied.headers.get('set-cookie'), null);
  assert.deepEqual(mock.calls, [`${apiBase}user`, `${apiBase}${projectPath}`, `${apiBase}zones?account.id=${accountId}`]);
});

test('OAuth polling still forwards through both the private entry and setup Worker', async () => {
  const calls = [];
  const fetcher = async (url, init) => {
    calls.push(url);
    assert.equal(init.method, 'POST');
    assert.equal(await new Response(init.body).text(), 'grant_type=device-code');
    assert.equal(init.headers.get('cookie'), null);
    return Response.json({ error: 'authorization_pending' }, { status: 400 });
  };
  for (const [path, run] of [[`${prefix}/oauth/token`, (req) => handle(req, env, fetcher)], ['/__cf/oauth/token', (req) => handleSetup(req, fetcher)]]) {
    const res = await run(new Request(`${origin}${path}`, { method: 'POST', headers: { origin, cookie: 'ignored=1' }, body: 'grant_type=device-code' }));
    assert.equal(res.status, 400);
    assert.deepEqual(await res.json(), { error: 'authorization_pending' });
  }
  assert.deepEqual(calls, Array(2).fill('https://dash.cloudflare.com/oauth2/token'));
});
