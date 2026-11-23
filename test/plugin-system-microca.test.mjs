import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { createHash, generateKeyPairSync, randomBytes, sign } from 'node:crypto';
import { mkdir, mkdtemp, readFile, rm, writeFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import test from 'node:test';
import { fileURLToPath } from 'node:url';

import { onRequest } from '../functions/__ca/issue.ts';
import { microCaCommitment, microCaFingerprint, parseBundle, verifyBundle } from '../packages/plugin-system/dist/index.js';
import { signPlugin } from '../packages/plugin-system/scripts/sign-plugin.mjs';

const p256 = () => generateKeyPairSync('ec', { namedCurve: 'prime256v1' });
const sha256 = (data) => createHash('sha256').update(data).digest();
const token = randomBytes(32).toString('base64url');
const ca = p256();
const env = { ONLINE_GAL_CA_BUILD_TOKEN: token, ONLINE_GAL_CA_PRIVATE_JWK: JSON.stringify(ca.privateKey.export({ format: 'jwk' })) };
const siteUrl = 'https://onlinegal.de5.net/__ca/issue';
const issue = (eph_pk, bearer = token, bindings = env) => onRequest({
  env: bindings,
  request: new Request(siteUrl, {
    method: 'POST', headers: { authorization: `Bearer ${bearer}` }, body: JSON.stringify({ eph_pk }),
  }),
});

test('Pages CA authenticates only the configured token and reports missing secrets', async () => {
  const jwk = p256().publicKey.export({ format: 'jwk' });
  assert.equal((await onRequest({ request: new Request(siteUrl), env })).status, 405);
  assert.equal((await issue(jwk, randomBytes(32).toString('base64url'))).status, 401);
  assert.equal((await issue(jwk, token, {})).status, 503);
  assert.deepEqual(await (await issue(jwk, token, { ONLINE_GAL_CA_BUILD_TOKEN: token })).json(), { error: 'ca-key-not-configured' });
  assert.equal((await issue({})).status, 400);
  const response = await issue(jwk);
  assert.equal(response.status, 200);
  assert.equal(response.headers.get('cache-control'), 'no-store');
  const { ca: publicKey, cred } = await response.json();
  assert.equal('d' in publicKey, false);
  const claims = JSON.parse(Buffer.from(cred.split('.')[1], 'base64url'));
  assert.deepEqual(claims.eph_pk, jwk);
  assert.equal(claims.anon_id, 'cf-pages');
  assert.equal(claims.exp - claims.iat, 600);
});

test('CA and signer produce a bundle accepted by the kernel verifier', async () => {
  const log = p256();
  const spki = log.publicKey.export({ format: 'der', type: 'spki' });
  const logSpki = spki.toString('base64');
  const logID = sha256(spki).toString('hex');
  const bytes = Buffer.from('{"format":1}\n');
  async function fetcher(url, init) {
    if (url === siteUrl) return onRequest({ request: new Request(url, init), env });
    assert.equal(url, 'https://rekor.sigstore.dev/api/v1/log/entries');
    assert.equal(init.redirect, 'error');
    assert.equal(new Headers(init.headers).has('authorization'), false);
    const leaf = Buffer.from(init.body);
    const body = leaf.toString('base64');
    const integratedTime = Math.floor(Date.now() / 1000);
    const logIndex = 4_047_830_186;
    const set = sign('sha256', Buffer.from(JSON.stringify({ body, integratedTime, logID, logIndex })), log.privateKey);
    const sibling = sha256(Buffer.from([0, 82]));
    const leafHash = sha256(Buffer.concat([Buffer.from([0]), leaf]));
    const root = sha256(Buffer.concat([Buffer.from([1]), sibling, leafHash]));
    const note = `rekor.sigstore.dev - 2\n2\n${root.toString('base64')}\n`;
    const noteSig = Buffer.concat([Buffer.from(logID.slice(0, 8), 'hex'), sign('sha256', Buffer.from(note), log.privateKey)]);
    const checkpoint = `${note}\n\u2014 rekor.sigstore.dev ${noteSig.toString('base64')}\n`;
    return Response.json({ uuid: { body, integratedTime, logID, logIndex, verification: {
      signedEntryTimestamp: set.toString('base64'),
      inclusionProof: { checkpoint, logIndex: 1, treeSize: 2, hashes: [sibling.toString('hex')] },
    } } });
  }
  const bundle = parseBundle(await signPlugin(bytes, { token, fetcher }));
  const salt = 'test';
  const hash = await microCaCommitment(await microCaFingerprint(bundle.ca), 'cf-pages', salt);
  const verify = (candidate, input = bytes, hashes = [hash]) =>
    verifyBundle(candidate, input, { salt, hashes }, 'online-gal.manager', { logSpki });
  await verify(bundle);
  await assert.rejects(verify(bundle, Buffer.from('{}')), /does not match the bundle/);
  await assert.rejects(verify(bundle, bytes, []), /not permitted by trust policy/);
  await assert.rejects(verify(bundle, bytes, [hash + ':' + (bundle.rekor.integratedTime + 1) + ':']), /not permitted/);
  const badSet = structuredClone(bundle);
  badSet.rekor.logIndex++;
  await assert.rejects(verify(badSet), /invalid Rekor SET/);
  const badProof = structuredClone(bundle);
  badProof.rekor.proof.hashes[0] = Buffer.alloc(32).toString('base64');
  await assert.rejects(verify(badProof), /invalid Rekor checkpoint/);
  await assert.rejects(verifyBundle(bundle, bytes, { salt, hashes: [hash] }, 'online-gal.manager'), /invalid Rekor entry metadata/);
});

test('site build skips an absent token and fails when the CA cannot be reached', async (t) => {
  const root = await mkdtemp(join(tmpdir(), 'onlinegal-microca-'));
  t.after(async () => rm(root, { recursive: true, force: true }));
  const app = join(root, 'app');
  await mkdir(app);
  await writeFile(join(app, 'index.html'), '<body>manager</body>');
  const out = join(root, 'site');
  const script = fileURLToPath(new URL('../packages/plugin-system/scripts/build-site.mjs', import.meta.url));
  const url = 'data:text/javascript,' + encodeURIComponent('globalThis.fetch = async () => { throw new Error("CA unreachable"); };');
  const child = (value) => spawnSync(process.execPath, ['--import', url, script, '--app-dist', app, '--out', out], {
    env: { ...process.env, ONLINE_GAL_CA_BUILD_TOKEN: value }, encoding: 'utf8', timeout: 10_000,
  });
  const unsigned = child('');
  assert.equal(unsigned.status, 0, unsigned.stderr);
  assert.match(unsigned.stdout, /ONLINE_GAL_CA_BUILD_TOKEN/);
  const registry = JSON.parse(await readFile(join(out, 'registry/index.json'), 'utf8'));
  const path = join(out, 'registry', registry.plugins['online-gal.manager'].path);
  assert.deepEqual(JSON.parse(await readFile(join(path, 'plugin.json'), 'utf8')), { format: 2, manifest: 'mf-manifest.json' });
  await assert.rejects(readFile(join(path, 'plugin.bundle.json')), { code: 'ENOENT' });

  const unavailable = child(token);
  assert.equal(unavailable.status, 1, unavailable.stderr);
  assert.match(unavailable.stderr, /micro-CA request failed/);
  assert.equal(unavailable.stderr.includes(token), false);
  await assert.rejects(readFile(join(out, 'registry/index.json')), { code: 'ENOENT' });
});
