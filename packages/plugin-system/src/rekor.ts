import type { RekorEntry, InclusionProof } from './attest.js';

export const REKOR_V1_SPKI = 'MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAE2G2Y+2tabdTV5BcGiBIx0a9fAFwrkBbmLSGtks4L3qX6yYY0zufBnhC8Ur/iy55GhWP/9A/bY2LhC30M9+RYtw==';
export const es256 = { name: 'ECDSA', hash: 'SHA-256' };
export const p256 = { name: 'ECDSA', namedCurve: 'P-256' };
export const utf8 = new TextEncoder();
export const text = new TextDecoder('utf-8', { fatal: true });
export const hex = (bytes: Uint8Array): string => Array.from(bytes, (b) => b.toString(16).padStart(2, '0')).join('');
const b64 = (bytes: Uint8Array): string => btoa(String.fromCharCode(...bytes));
export const fromB64 = (value: string): Uint8Array<ArrayBuffer> => Uint8Array.from(atob(value), (c) => c.charCodeAt(0));
export const sha256 = async (bytes: BufferSource): Promise<Uint8Array<ArrayBuffer>> =>
  new Uint8Array(await crypto.subtle.digest('SHA-256', bytes));

export function derToRaw(der: Uint8Array): Uint8Array<ArrayBuffer> {
  if (der[0] !== 0x30 || der[1] !== der.length - 2 || der.length > 72) throw new Error('invalid ECDSA DER sequence');
  let offset = 2;
  const raw = new Uint8Array(64);
  for (let i = 0; i < 2; i++) {
    if (der[offset++] !== 0x02) throw new Error('invalid ECDSA DER integer');
    const size = der[offset++] ?? 0;
    const int = der.subarray(offset, offset + size);
    offset += size;
    const first = int[0];
    if (first === undefined || int.length !== size || first >= 128 ||
        (size > 1 && first === 0 && (int[1] ?? 0) < 128) || size > (first === 0 ? 33 : 32)) {
      throw new Error('invalid ECDSA DER integer');
    }
    const unsigned = size === 33 ? int.subarray(1) : int;
    raw.set(unsigned, (i + 1) * 32 - unsigned.length);
  }
  if (offset !== der.length) throw new Error('trailing ECDSA DER data');
  return raw;
}

async function merkleRoot(body: Uint8Array, proof: InclusionProof): Promise<string> {
  let { logIndex: index, treeSize } = proof;
  if (!Number.isSafeInteger(index) || !Number.isSafeInteger(treeSize) || index < 0 || index >= treeSize || proof.hashes.length > 53) {
    throw new Error('invalid Rekor proof coordinates');
  }
  const leaf = new Uint8Array(1 + body.length);
  leaf.set(body, 1);
  let node = await sha256(leaf);
  let end = treeSize - 1;
  const branch = new Uint8Array(65);
  branch[0] = 1;
  for (const value of proof.hashes) {
    const sibling = fromB64(value);
    if (end === 0 || sibling.length !== 32) throw new Error('invalid Rekor proof path');
    if (index % 2 === 1 || index === end) {
      branch.set(sibling, 1);
      branch.set(node, 33);
      while (index > 0 && index % 2 === 0) {
        index = Math.floor(index / 2);
        end = Math.floor(end / 2);
      }
    } else {
      branch.set(node, 1);
      branch.set(sibling, 33);
    }
    node = await sha256(branch);
    index = Math.floor(index / 2);
    end = Math.floor(end / 2);
  }
  if (end !== 0) throw new Error('incomplete Rekor proof path');
  return b64(node);
}

export async function checkRekor(r: RekorEntry, digest: string, signature: Uint8Array, eph: CryptoKey, logSpki: string): Promise<void> {
  const logDer = fromB64(logSpki);
  const logId = hex(await sha256(logDer));
  if (r.logId !== logId || !Number.isSafeInteger(r.logIndex) || r.logIndex < 0 || !Number.isSafeInteger(r.integratedTime)) {
    throw new Error('invalid Rekor entry metadata');
  }
  const key = await crypto.subtle.importKey('spki', logDer, p256, false, ['verify']);
  // The SET signs lexically sorted JSON with the API's uppercase logID.
  const entry = utf8.encode(JSON.stringify({ body: r.body, integratedTime: r.integratedTime, logID: r.logId, logIndex: r.logIndex }));
  if (!await crypto.subtle.verify(es256, key, derToRaw(fromB64(r.set)), entry)) throw new Error('invalid Rekor SET');

  const body = fromB64(r.body);
  const record = JSON.parse(text.decode(body));
  const loggedSig = derToRaw(fromB64(record?.spec?.signature?.content));
  const loggedKey = text.decode(fromB64(record?.spec?.signature?.publicKey?.content));
  const pem = /^-----BEGIN PUBLIC KEY-----\r?\n([A-Za-z0-9+/=\r\n]+)\r?\n-----END PUBLIC KEY-----\r?\n?$/.exec(loggedKey);
  const ephDer = new Uint8Array(await crypto.subtle.exportKey('spki', eph));
  if (record.kind !== 'hashedrekord' || record.apiVersion !== '0.0.1' || record.spec?.data?.hash?.algorithm !== 'sha256' ||
      record.spec.data.hash.value !== digest || b64(loggedSig) !== b64(signature) || !pem ||
      pem[1]?.replace(/[\r\n]/g, '') !== b64(ephDer)) throw new Error('Rekor body does not match the bundle');

  const root = await merkleRoot(body, r.proof);
  const checkpoint = /^(rekor\.sigstore\.dev(?: - [0-9]+)?\n([0-9]+)\n([A-Za-z0-9+/=]+)\n)\n\u2014 rekor\.sigstore\.dev ([A-Za-z0-9+/=]+)\n$/.exec(r.proof.checkpoint);
  const signed = checkpoint?.[1];
  const signatureNote = checkpoint?.[4];
  if (!signed || !signatureNote || checkpoint[2] !== String(r.proof.treeSize) || checkpoint[3] !== root) {
    throw new Error('invalid Rekor checkpoint');
  }
  const note = fromB64(signatureNote);
  if (hex(note.subarray(0, 4)) !== logId.slice(0, 8) ||
      !await crypto.subtle.verify(es256, key, derToRaw(note.subarray(4)), utf8.encode(signed))) {
    throw new Error('invalid Rekor checkpoint signature');
  }
}
