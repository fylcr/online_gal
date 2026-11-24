import assert from 'node:assert/strict';
import { createRequire } from 'node:module';
import test from 'node:test';
import { cfRouteOwnerBinding, deriveCfRouteConfig } from '../functions/__cf/route-config.mjs';

const require = createRequire(new URL('../packages/plugin-system/package.json', import.meta.url));
const { build } = require('esbuild');
const compiled = await build({ entryPoints: [new URL('../apps/setup/src/lib/setup.ts', import.meta.url).pathname.replace(/^\/(?=[A-Za-z]:\/)/, '')],
  bundle: true, format: 'esm', platform: 'browser', write: false });
const { SetupClient, clearSetupSession } = await import(`data:text/javascript;base64,${Buffer.from(compiled.outputFiles[0].contents).toString('base64')}`);

const host = 'setup.example.org';
const origin = `https://${host}`;
const owner = { accountId: '1'.repeat(32), accountName: 'Fixture', projectName: 'fixture' };
const user = { id: '2'.repeat(32), email: 'owner@example.org' };
const scopes = ['pages:read', 'pages:write', 'account_api_tokens:create'];
const path = `/__cf/api/accounts/${owner.accountId}/pages/projects/${owner.projectName}`;
const tokens = `/__cf/api/accounts/${owner.accountId}/tokens`;
const ids = ['11111111-1111-4111-8111-111111111111', '22222222-2222-4222-8222-222222222222'];
const deploymentOrigin = id => `https://${id.slice(0, 8)}.${owner.projectName}.pages.dev`;
const envelope = result => Response.json({ success: true, result });

function fixture({ patchLost = false, commitAndSwitch = false, delaySwitch = false, changeBeforeClear = false,
  changeAfterClear = false, buildResponseLost = false, history = [], actualUser = user, authorize = true } = {}) {
  let deployment = 0;
  let switched = false;
  let privateTransport = false;
  let envs = { production: { env_vars: {} }, preview: { env_vars: {} } };
  let tokenCalls = 0;
  const attempts = [];
  const events = [];
  const account = {
    async setEntry(segment) {
      const key = envs.production.env_vars.ONLINE_GAL_CF_ROUTE_KEY.value;
      const route = await deriveCfRouteConfig({ accountId: owner.accountId, projectName: owner.projectName, host, routeKey: key });
      assert.ok(segment === route.segment);
      assert.equal(deployment, 1);
      privateTransport = true;
      events.push('set-entry');
    },
    async status() {
      assert.ok(privateTransport);
      events.push('owner-proof');
      return { signedIn: true, user, siteAdmin: authorize ? owner : null };
    },
    async forward(request, logicalPath) {
      if (delaySwitch && deployment === 1 && !switched && logicalPath.endsWith(ids[0])) {
        switched = true;
        events.push('fixed-read-lost');
        return new Response('<!DOCTYPE html>');
      }
      if (switched) assert.ok(privateTransport, 'fixed transport must stop after first deploy');
      if (logicalPath === '/__cf/api/user') return envelope(actualUser);
      if (logicalPath === `${tokens}/permission_groups`) return envelope([{ name: 'Account API Tokens Write', id: 'fixture-group' }]);
      if (logicalPath === tokens) {
        tokenCalls++;
        return envelope({ value: 'fixture-account-token' });
      }
      if (logicalPath === path) {
        if (request.method === 'PATCH') {
          const body = await request.json();
          if (privateTransport) {
            assert.deepEqual(body, { deployment_configs: { production: { env_vars: { ONLINE_GAL_CA_BOOTSTRAP: null } } } });
            assert.ok(events.includes('owner-proof'));
            events.push('clear-bootstrap');
            delete envs.production.env_vars.ONLINE_GAL_CA_BOOTSTRAP;
            if (changeAfterClear) {
              envs.preview.env_vars.ONLINE_GAL_SITE.value += ' ';
              return Response.json({ success: false }, { status: 503 });
            }
          } else {
            attempts.push(body);
            if (patchLost && attempts.length === 1) return Response.json({ success: false }, { status: 503 });
            envs = structuredClone(body.deployment_configs);
            if (commitAndSwitch) {
              switched = true;
              deployment = 1;
              return Response.json({ success: false }, { status: 503 });
            }
          }
        }
        return envelope({ name: owner.projectName, domains: [host], production_branch: 'main', deployment_configs: envs });
      }
      if (logicalPath.startsWith(`${path}/deployments?env=production`)) return envelope(history);
      if (logicalPath === `${path}/deployments` && request.method === 'POST') {
        if (buildResponseLost) {
          events.push('build-post');
          return Response.json({ success: false }, { status: 503 });
        }
        if (deployment === 1) events.push('create-signing-deployment');
        if (!delaySwitch) switched = true;
        return envelope({ id: ids[deployment++] });
      }
      const id = logicalPath.split('/').at(-1);
      assert.ok(ids.includes(id));
      return envelope({ id, url: deploymentOrigin(id), latest_stage: { name: 'deploy', status: 'success' } });
    },
  };
  const fetcher = async input => {
    const url = new URL(input);
    if (url.pathname === '/__setup/status') {
      if (switched) return new Response('<!DOCTYPE html>', { headers: { 'content-type': 'text/html' } });
      return Response.json({ phase: 'setup', initialized: attempts.length > (patchLost ? 1 : 0) });
    }
    if (url.pathname === '/__ca/issue') {
      if (changeBeforeClear) envs.preview.env_vars.ONLINE_GAL_SITE.value += ' ';
      return Response.json({ error: 'unauthorized' }, { status: 401, headers: { 'cache-control': 'no-store' } });
    }
    assert.equal(url.pathname, '/.well-known/online-gal-deployment.json');
    return Response.json({ url: deploymentOrigin(ids[deployment - 1]), branch: 'main' }, { headers: { 'cache-control': 'no-store', 'access-control-allow-origin': '*' } });
  };
  const verify = async (trust, immutableOrigin) => {
    assert.equal(trust.host, host);
    assert.equal(immutableOrigin, deploymentOrigin(ids[1]));
    events.push('verify');
    return [];
  };
  return { account, attempts, events, fetcher, verify, tokenCalls: () => tokenCalls };
}

function fakeSession(t) {
  const original = Object.getOwnPropertyDescriptor(globalThis, 'sessionStorage');
  const data = new Map();
  Object.defineProperty(globalThis, 'sessionStorage', { configurable: true, value: {
    getItem: key => data.get(key) ?? null,
    setItem: (key, value) => data.set(key, value),
    removeItem: key => data.delete(key),
  } });
  t.after(() => {
    clearSetupSession();
    if (original) Object.defineProperty(globalThis, 'sessionStorage', original);
    else delete globalThis.sessionStorage;
  });
  return data;
}

test('setup retries reuse one owner binding and route key and switch transport before clearing bootstrap', async (t) => {
  const data = fakeSession(t);
  const f = fixture({ patchLost: true });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  await assert.rejects(client.write(owner, user, scopes), /无法确认 Pages 配置写入结果/);
  assert.equal(client.entryLink(owner), undefined);
  const written = await client.write(owner, user, scopes);
  assert.ok(written.requiresEntry);
  assert.equal(f.tokenCalls(), 1);
  assert.ok(JSON.stringify(f.attempts[0]) === JSON.stringify(f.attempts[1]), 'retry submits the same generated material');
  const { production, preview } = f.attempts[1].deployment_configs;
  assert.equal(production.env_vars.ONLINE_GAL_CF_OWNER_ID.type, 'secret_text');
  assert.ok(production.env_vars.ONLINE_GAL_CF_OWNER_ID.value === user.id);
  const key = production.env_vars.ONLINE_GAL_CF_ROUTE_KEY;
  assert.equal(key.type, 'secret_text');
  assert.equal(key.value.length, 43);
  assert.deepEqual(Object.keys(preview.env_vars), ['ONLINE_GAL_SITE']);
  const { segment } = await deriveCfRouteConfig({ accountId: owner.accountId, projectName: owner.projectName, host, routeKey: key.value });
  assert.ok(client.entryLink(owner) === `/admin#cf-entry=${segment}`);
  const record = JSON.parse(production.env_vars.ONLINE_GAL_SITE.value);
  const ownerBinding = await cfRouteOwnerBinding(owner.accountId, owner.projectName, user.id);
  assert.deepEqual(record.cfRoute, { v: 1, host, ownerBinding });
  const publicData = JSON.stringify([record, preview, written, [...data.values()]]);
  for (const secret of [key.value, segment, user.id]) assert.ok(!publicData.includes(secret));
  const result = await client.advanceRelease(owner, written.trust, undefined, { intervalMs: 1, timeoutMs: 100 }, false, true);
  assert.equal(result.stage, 'awaiting-dns');
  assert.deepEqual(f.events, ['set-entry', 'owner-proof', 'clear-bootstrap', 'create-signing-deployment', 'verify']);
  const recovered = new SetupClient(origin, f.account, f.fetcher, f.verify);
  await assert.rejects(recovered.advanceRelease(owner, written.trust, undefined, undefined, false, true), /cf 命令行恢复/);
});

test('setup rejects a mismatched OAuth user before creating any configuration', async () => {
  const f = fixture({ actualUser: { id: '3'.repeat(32) } });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  await assert.rejects(client.write(owner, user, scopes), /Cloudflare 用户无法核对/);
  assert.equal(f.tokenCalls(), 0);
  assert.equal(f.attempts.length, 0);
});

test('setup preserves the bootstrap marker and skips signing deployment when the private relay denies owner proof', async (t) => {
  fakeSession(t);
  const f = fixture({ authorize: false });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  const written = await client.write(owner, user, scopes);
  await assert.rejects(client.advanceRelease(owner, written.trust, undefined, { intervalMs: 1, timeoutMs: 100 }, false, true), /无法通过私有入口核对/);
  assert.deepEqual(f.events, ['set-entry', 'owner-proof']);
});

test('a committed initialization with a lost acknowledgment recovers the same entry after cutover', async () => {
  const f = fixture({ commitAndSwitch: true });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  await assert.rejects(client.write(owner, user, scopes), /无法确认 Pages 配置写入结果/);
  const written = await client.write(owner, user, scopes);
  assert.ok(written.requiresEntry);
  assert.match(client.entryLink(owner), /^\/admin#cf-entry=[A-Za-z0-9_-]{43}$/);
  assert.equal(f.attempts.length, 1);
  assert.equal(f.tokenCalls(), 1);
  assert.deepEqual(f.events, ['set-entry', 'owner-proof']);
});

test('an unprovable write after cutover requires cf recovery instead of completion', async () => {
  const f = fixture({ commitAndSwitch: true, authorize: false });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  await assert.rejects(client.write(owner, user, scopes), /无法确认 Pages 配置写入结果/);
  await assert.rejects(client.write(owner, user, scopes), /cf 命令行恢复/);
  assert.equal(client.entryLink(owner), undefined);
  assert.equal(f.attempts.length, 1);
});

test('bootstrap reads can recover from a cutover between status and a fixed-relay API read', async (t) => {
  fakeSession(t);
  const f = fixture({ delaySwitch: true });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  const written = await client.write(owner, user, scopes);
  const result = await client.advanceRelease(owner, written.trust, undefined, { intervalMs: 1, timeoutMs: 100 }, false, true);
  assert.equal(result.stage, 'awaiting-dns');
  assert.deepEqual(f.events, ['fixed-read-lost', 'set-entry', 'owner-proof', 'clear-bootstrap', 'create-signing-deployment', 'verify']);
});

test('a changed setup record blocks bootstrap removal', async (t) => {
  fakeSession(t);
  const f = fixture({ changeBeforeClear: true });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  const written = await client.write(owner, user, scopes);
  await assert.rejects(client.advanceRelease(owner, written.trust, undefined, undefined, false, true), /cf 命令行恢复/);
  assert.deepEqual(f.events, ['set-entry', 'owner-proof']);
});

test('retry after an unconfirmed clear compares with the original records before creating a signing build', async (t) => {
  fakeSession(t);
  const f = fixture({ changeAfterClear: true });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  const written = await client.write(owner, user, scopes);
  await assert.rejects(client.advanceRelease(owner, written.trust, undefined, undefined, false, true), /HTTP 503/);
  await assert.rejects(client.advanceRelease(owner, written.trust, undefined, undefined, false, true), /cf 命令行恢复/);
  assert.deepEqual(f.events, ['set-entry', 'owner-proof', 'clear-bootstrap']);
});

test('a deployment POST with unknown outcome is not replayed while history is empty', async (t) => {
  fakeSession(t);
  const f = fixture({ buildResponseLost: true });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  const written = await client.write(owner, user, scopes);
  await assert.rejects(client.advanceRelease(owner, written.trust, undefined, undefined, false, true), /HTTP 503/);
  await assert.rejects(client.advanceRelease(owner, written.trust, undefined, undefined, true, true), /cf 命令行恢复/);
  assert.deepEqual(f.events, ['build-post']);
});

test('two plausible bootstrap builds require explicit recovery', async (t) => {
  fakeSession(t);
  const f = fixture({ history: ids.map((id, offset) => ({ id, created_on: new Date(Date.now() + (offset + 1) * 1000).toISOString(),
    deployment_trigger: { metadata: { branch: 'main' } }, latest_stage: { status: 'active' } })) });
  const client = new SetupClient(origin, f.account, f.fetcher, f.verify);
  const written = await client.write(owner, user, scopes);
  await assert.rejects(client.advanceRelease(owner, written.trust, undefined, undefined, false, true), /cf 命令行恢复/);
  assert.deepEqual(f.events, []);
});
