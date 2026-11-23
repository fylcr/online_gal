// Disposable fixture for a Git-integrated Pages deployment. Never use production credentials.
import assert from 'node:assert/strict';
import { createHmac } from 'node:crypto';
import { spawnSync } from 'node:child_process';
import { readFileSync, readdirSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';

const host = 'online-gal-task106-probe.pages.dev';
const projectName = 'online-gal-task106-probe';
const { TASK151_ACCOUNT: accountId, TASK151_KEY: key, CF_PAGES_URL: deployment, CF_PAGES_BRANCH: branch } = process.env;
const env = { ...process.env };
assert.equal(process.env.CF_PAGES, '1');
assert.match(accountId, /^[a-f0-9]{32}$/);
assert.match(branch, /^test\/task151-private-relay-/);
const url = new URL(deployment);
assert.ok(url.hostname.endsWith(`.${host}`));
const keyBytes = Buffer.from(key ?? '', 'base64url');
if (keyBytes.byteLength !== 32 || keyBytes.toString('base64url') !== key) {
  throw new Error('Invalid isolated route fixture');
}
const input = JSON.stringify(['online-gal/cf-route/v1', accountId, projectName, host]);
const segment = createHmac('sha256', keyBytes).update(input).digest('base64url');
delete env.ONLINE_GAL_CA_BOOTSTRAP;
delete env.ONLINE_GAL_CA_BUILD_TOKEN;
delete env.ONLINE_GAL_CA_PRIVATE_JWK;
delete env.CLOUDFLARE_ACCOUNT_TOKEN;
env.ONLINE_GAL_SITE = JSON.stringify({ v: 1 });
const built = spawnSync('npm', ['run', 'build:pages'], { env, stdio: 'inherit' });
assert.equal(built.status, 0, 'site build failed');

const out = 'dist/site';
const source = `const encoder = new TextEncoder();
export async function onRequest({ request, env }) {
  const host = ${JSON.stringify(host)};
  const project = ${JSON.stringify(projectName)};
  const config = env.TASK151_ACCOUNT;
  const secret = env.TASK151_KEY;
  if (!/^[a-z0-9]{32}$/.test(config) || !/^[A-Za-z0-9_-]{43}$/.test(secret)) {
    return new Response(null, { status: 500, headers: { 'x-task151-function': 'missing-config' } });
  }
  const bytes = Uint8Array.from(atob(secret.replaceAll('-', '+').replaceAll('_', '/') + '='), ch => ch.charCodeAt(0));
  const key = await crypto.subtle.importKey('raw', bytes, { name: 'HMAC', hash: 'SHA-256' }, false, ['sign']);
  const message = encoder.encode(JSON.stringify(['online-gal/cf-route/v1', config, project, host]));
  const signature = await crypto.subtle.sign('HMAC', key, message);
  const segment = btoa(String.fromCharCode(...new Uint8Array(signature))).replaceAll('+', '-').replaceAll('/', '_').replace(/=+$/, '');
  const url = new URL(request.url);
  const matched = url.pathname === '/__cf/' + segment + '/site-admin';
  const canonical = url.hostname === host;
  console.log('task151 invocation');
  return Response.json({ matched, canonical }, {
    status: matched && canonical ? 200 : 404,
    headers: { 'cache-control': 'no-store', 'x-task151-function': 'invoked' },
  });
}`;
writeFileSync('functions/__cf/[[path]].ts', source);
const routes = { version: 1, include: [`/__cf/${segment}/*`], exclude: [] };
writeFileSync(join(out, '_routes.json'), JSON.stringify(routes));
writeFileSync(join(out, 'task151-probe.json'), JSON.stringify({ branch, deployment: url.origin, publicFilesScanned: true }));
function checkPublicFiles(directory) {
  for (const entry of readdirSync(directory, { withFileTypes: true })) {
    const path = join(directory, entry.name);
    if (entry.isDirectory()) checkPublicFiles(path);
    if (!entry.isFile() || path === join(out, '_routes.json')) continue;
    const bytes = readFileSync(path);
    assert.ok(!bytes.includes(key) && !bytes.includes(segment), 'Public file contains a fixture secret');
  }
}
checkPublicFiles(out);
