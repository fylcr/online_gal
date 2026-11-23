import { createHash, generateKeyPairSync, sign } from 'node:crypto';

import { derToRaw } from '../dist/rekor.js';

const REKOR = 'https://rekor.sigstore.dev/api/v1/log/entries';

async function post(url, body, fetcher, token) {
  const service = token ? 'micro-CA' : 'Rekor';
  let response;
  try {
    response = await fetcher(url, {
      method: 'POST',
      headers: { 'content-type': 'application/json', ...(token ? { authorization: `Bearer ${token}` } : {}) },
      body: JSON.stringify(body),
      redirect: 'error',
      signal: AbortSignal.timeout(20_000),
    });
  } catch {
    throw new Error(`${service} request failed`);
  }
  if (!response.ok) throw new Error(`${service} returned HTTP ${response.status}`);
  try {
    return await response.json();
  } catch {
    throw new Error(`${service} returned invalid JSON`);
  }
}

export async function signPlugin(manifestBytes, { token, siteHost = 'onlinegal.de5.net', fetcher = fetch }) {
  if (!/^[A-Za-z0-9_-]{43}$/.test(token)) throw new Error('ONLINE_GAL_CA_BUILD_TOKEN must be a base64url-encoded 32-byte token');
  if (!/^[a-z0-9.-]+$/i.test(siteHost)) throw new Error('SITE_HOST must be a hostname');
  const { eph_pk, publicPem, der, raw } = (() => {
    const { publicKey, privateKey } = generateKeyPairSync('ec', { namedCurve: 'prime256v1' });
    const der = sign('sha256', manifestBytes, privateKey);
    // Node emits DER by default; the bundle uses its Web Crypto (P1363) form.
    const raw = Buffer.from(derToRaw(der));
    return { eph_pk: publicKey.export({ format: 'jwk' }), publicPem: publicKey.export({ format: 'pem', type: 'spki' }), der, raw };
  })();

  const credential = await post(`https://${siteHost}/__ca/issue`, { eph_pk }, fetcher, token);
  if (!credential?.ca || typeof credential.cred !== 'string' || credential.cred.length === 0) throw new Error('micro-CA returned no credential');
  const reply = await post(REKOR, {
    apiVersion: '0.0.1',
    kind: 'hashedrekord',
    spec: {
      data: { hash: { algorithm: 'sha256', value: createHash('sha256').update(manifestBytes).digest('hex') } },
      signature: { content: der.toString('base64'), publicKey: { content: Buffer.from(publicPem).toString('base64') } },
    },
  }, fetcher);
  const entries = Object.values(reply ?? {});
  const entry = entries[0];
  const proof = entry?.verification?.inclusionProof;
  if (entries.length !== 1 || !entry?.verification?.signedEntryTimestamp || !proof || !Array.isArray(proof.hashes) ||
      proof.hashes.some((h) => typeof h !== 'string' || !/^[0-9a-f]{64}$/i.test(h))) {
    throw new Error('Rekor returned no complete entry proof');
  }
  return {
    v: 1, channel: 'microca', ca: credential.ca, cred: credential.cred, sig: raw.toString('base64url'),
    rekor: {
      body: entry.body, integratedTime: entry.integratedTime, logIndex: entry.logIndex, logId: entry.logID,
      set: entry.verification.signedEntryTimestamp,
      proof: {
        checkpoint: proof.checkpoint, logIndex: proof.logIndex, treeSize: proof.treeSize,
        hashes: proof.hashes.map((h) => Buffer.from(h, 'hex').toString('base64')),
      },
    },
  };
}
