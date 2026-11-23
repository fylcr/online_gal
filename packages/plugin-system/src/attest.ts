/**
 * Attestation bundle verification for the plugin system (design.md K15–K16).
 *
 * A `plugin.bundle.json` carries `v` and `channel` (`github` | `microca`). The `github`
 * channel carries the Sigstore bundle GitHub produced in `sigstore` (the verifier is chosen
 * by tasks 10.0 and is not implemented yet). The `microca` channel carries:
 *   - `v`      format version (1)
 *   - `ca`     the anonymous edge micro-CA public key as a JWK (inlined; trusted
 *              only after the DNS TXT whitelist confirms its fingerprint)
 *   - `cred`   a compact JWS (ES256) signed by the micro-CA:
 *                { anon_id, eph_pk (JWK), iat, exp }
 *              exp − iat MUST NOT exceed 600 s
 *   - `sig`    base64url ECDSA-P256 signature of the mf-manifest.json SHA-256,
 *              made with the ephemeral private key whose public half is in `cred`
 *   - `rekor`  Rekor v1 entry:
 *                { body (base64), integratedTime, logIndex, logId, set, proof }
 *              `set` is the Signed Entry Timestamp (Rekor log key, DER format,
 *              base64); `proof` is { checkpoint, hashes, logIndex, treeSize } for the
 *              RFC 6962 inclusion proof
 *
 * Verification steps (all via Web Crypto, no SDK):
 *   1. Parse and type-check the bundle.
 *   2. Verify the `cred` JWS ES256 signature with `ca` and check exp−iat ≤ 600 s.
 *   3. Compute SHA-256(ca_fingerprint | anon_id | salt) and confirm it is in
 *      the DNS TXT whitelist (provided by trust-dns.ts) within its time window.
 *   4. Verify the Rekor SET (DER ECDSA signature by the Rekor public key baked
 *      into sw.ts) over the canonicalised entry header.
 *   5. Verify the RFC 6962 inclusion proof: re-compute the Merkle root from
 *      `hashes` and confirm it matches the signed tree head in the checkpoint.
 *   6. Confirm `rekor.integratedTime` falls within [iat, exp].
 *   7. Verify `sig` (ECDSA-P256) over the manifest digest using `eph_pk`.
 *
 */

import { KernelError } from './errors.js';
import { checkRekor, es256, fromB64, hex, p256, REKOR_V1_SPKI, sha256, text, utf8 } from './rekor.js';
import { hashAllowed } from './trust-dns.js';

export const BUNDLE_FORMAT_VERSION = 1;

/** The JSON shape of `plugin.bundle.json`. */
export interface MicroCaBundle {
  readonly v: typeof BUNDLE_FORMAT_VERSION;
  readonly channel: 'microca';
  /** The anonymous micro-CA public key (JWK, EC P-256). */
  readonly ca: JsonWebKey;
  /** Compact JWS (ES256) signed by the micro-CA: { anon_id, eph_pk, iat, exp }. */
  readonly cred: string;
  /** base64url ECDSA-P256 signature of the mf-manifest.json SHA-256. */
  readonly sig: string;
  /** Rekor v1 transparency log entry and inclusion proof. */
  readonly rekor: RekorEntry;
}

/** `github` channel: the Sigstore bundle GitHub produced, kept unmodified. */
export interface GithubBundle {
  readonly v: typeof BUNDLE_FORMAT_VERSION;
  readonly channel: 'github';
  readonly sigstore: Record<string, unknown>;
}

export type AttestationBundle = MicroCaBundle | GithubBundle;

export interface RekorEntry {
  /** base64-encoded Rekor entry body. */
  readonly body: string;
  readonly integratedTime: number;
  readonly logIndex: number;
  readonly logId: string;
  /** base64-encoded DER Signed Entry Timestamp (Rekor log key signature). */
  readonly set: string;
  /** RFC 6962 Merkle inclusion proof. */
  readonly proof: InclusionProof;
}

export interface InclusionProof {
  /** Signed tree head checkpoint string. */
  readonly checkpoint: string;
  /** base64-encoded sibling hashes, leaf-to-root order. */
  readonly hashes: readonly string[];
  /** Leaf index within the shard, not the global SET log index. */
  readonly logIndex: number;
  readonly treeSize: number;
}

/** What the trust-dns layer hands to the bundle verifier. */
export interface TrustPolicy {
  readonly salt: string;
  /** Each item: `<hex>[:<after>:<before>]` (unix seconds; empty = no bound). */
  readonly hashes: readonly string[];
}

function isRecord(v: unknown): v is Record<string, unknown> {
  return typeof v === 'object' && v !== null && !Array.isArray(v);
}

function invalid(msg: string, id: string | null = null): KernelError {
  return new KernelError('attestation-invalid', msg, id);
}

/**
 * Parses and type-checks a raw bundle value. Throws `attestation-invalid` on
 * any structural problem. Does NOT perform cryptographic verification — call
 * `verifyBundle` for that.
 */
export function parseBundle(input: unknown): AttestationBundle {
  if (!isRecord(input)) throw invalid('plugin.bundle.json must be an object');
  if (input.v !== BUNDLE_FORMAT_VERSION) {
    throw invalid(`plugin.bundle.json version ${String(input.v)} is not supported (expected ${BUNDLE_FORMAT_VERSION})`);
  }
  if (input.channel === 'github') {
    if (!isRecord(input.sigstore)) throw invalid('plugin.bundle.json field "sigstore" must be an object');
    return input as unknown as GithubBundle;
  }
  if (input.channel !== 'microca') {
    throw invalid(`plugin.bundle.json channel ${String(input.channel)} is not supported (expected github or microca)`);
  }
  if (!isRecord(input.ca)) throw invalid('plugin.bundle.json field "ca" must be a JWK object');
  if (typeof input.cred !== 'string' || input.cred === '') throw invalid('plugin.bundle.json field "cred" must be a non-empty string');
  if (typeof input.sig !== 'string' || input.sig === '') throw invalid('plugin.bundle.json field "sig" must be a non-empty string');
  if (!isRecord(input.rekor)) throw invalid('plugin.bundle.json field "rekor" must be an object');
  const r = input.rekor;
  if (typeof r.body !== 'string') throw invalid('rekor.body must be a string');
  if (typeof r.integratedTime !== 'number') throw invalid('rekor.integratedTime must be a number');
  if (typeof r.logIndex !== 'number') throw invalid('rekor.logIndex must be a number');
  if (typeof r.logId !== 'string') throw invalid('rekor.logId must be a string');
  if (typeof r.set !== 'string') throw invalid('rekor.set must be a string');
  if (!isRecord(r.proof)) throw invalid('rekor.proof must be an object');
  const p = r.proof;
  if (typeof p.checkpoint !== 'string') throw invalid('rekor.proof.checkpoint must be a string');
  if (typeof p.logIndex !== 'number' || typeof p.treeSize !== 'number') {
    throw invalid('rekor proof logIndex and treeSize must be numbers');
  }
  if (!Array.isArray(p.hashes) || p.hashes.some((h: unknown) => typeof h !== 'string')) {
    throw invalid('rekor.proof.hashes must be an array of strings');
  }
  return input as unknown as AttestationBundle;
}

const GITHUB_ISSUER = 'https://token.actions.githubusercontent.com';

async function sha256Text(text: string): Promise<string> {
  const digest = await crypto.subtle.digest('SHA-256', new TextEncoder().encode(text));
  return [...new Uint8Array(digest)].map((byte) => byte.toString(16).padStart(2, '0')).join('');
}

/** The commitment format uses `|` as a separator, so a field containing one could be shifted into its neighbour. */
function rejectSeparator(field: string, value: string): void {
  if (value.includes('|')) throw invalid(`${field} must not contain "|"`);
}

/**
 * Splits a GitHub workflow URI (`https://github.com/<owner>/<repo>/.github/workflows/<file>@<ref>`)
 * into the workflow and its ref. Only tag refs are accepted, and the ref is dropped from the
 * commitment so that releasing a new tag does not change the DNS hash (design.md K16).
 */
export function normalizeWorkflowUri(uri: string): { readonly workflowUri: string; readonly ref: string } {
  const at = uri.lastIndexOf('@');
  if (at <= 0) throw invalid(`workflow URI "${uri}" has no @<ref> suffix`);
  const workflowUri = uri.slice(0, at);
  const ref = uri.slice(at + 1);
  if (!ref.startsWith('refs/tags/') || ref === 'refs/tags/') {
    throw invalid(`workflow ref "${ref}" is not a tag (expected refs/tags/*)`);
  }
  return { workflowUri, ref };
}

/**
 * `SHA-256("github|" + issuer + "|" + repoId + "|" + workflowUri + "|" + salt)` as hex.
 * `repoId` is the numeric repository ID from the certificate's Source Repository Identifier extension;
 * binding it means a deleted and re-registered repository name does not match the old hash (design.md K16).
 */
export async function githubCommitment(issuer: string, repoId: string, workflowUri: string, salt: string): Promise<string> {
  if (issuer !== GITHUB_ISSUER) throw invalid(`issuer "${issuer}" is not GitHub Actions`);
  if (!/^[0-9]+$/.test(repoId)) throw invalid(`repoId "${repoId}" must be a decimal number string`);
  rejectSeparator('workflowUri', workflowUri);
  return sha256Text(`github|${issuer}|${repoId}|${workflowUri}|${salt}`);
}

/** `SHA-256("microca|" + caFingerprint + "|" + anonId + "|" + salt)` as hex. */
export async function microCaCommitment(caFingerprint: string, anonId: string, salt: string): Promise<string> {
  rejectSeparator('anon_id', anonId);
  return sha256Text(`microca|${caFingerprint}|${anonId}|${salt}`);
}

function ecJwk(jwk: unknown): JsonWebKey {
  if (!isRecord(jwk)) throw new Error('invalid P-256 public key');
  const { crv, kty, x, y } = jwk;
  if (crv !== 'P-256' || kty !== 'EC' || typeof x !== 'string' || typeof y !== 'string' ||
      !/^[A-Za-z0-9_-]{43}$/.test(x) || !/^[A-Za-z0-9_-]{43}$/.test(y) || 'd' in jwk) {
    throw new Error('invalid P-256 public key');
  }
  return { crv, kty, x, y };
}

/** RFC 7638 canonical public JWK fingerprint as lowercase hex. */
export function microCaFingerprint(jwk: JsonWebKey): Promise<string> {
  return sha256Text(JSON.stringify(ecJwk(jwk)));
}

function fromUrl(value: string): Uint8Array<ArrayBuffer> {
  if (!/^[A-Za-z0-9_-]+$/.test(value)) throw new Error('invalid base64url');
  return fromB64(value.replace(/-/g, '+').replace(/_/g, '/'));
}

/** Verify the bundle against the exact content manifest bytes and DNS policy. */
export async function verifyBundle(
  bundle: AttestationBundle,
  manifestBytes: BufferSource,
  policy: TrustPolicy,
  pluginId: string,
  { logSpki = REKOR_V1_SPKI }: { logSpki?: string } = {},
): Promise<void> {
  if (bundle.channel !== 'microca') throw invalid('GitHub attestation verification is not yet implemented', pluginId);
  try {
    const [header, payload, sig, extra] = bundle.cred.split('.');
    if (!header || !payload || !sig || extra !== undefined) throw new Error('invalid credential JWS');
    const meta: unknown = JSON.parse(text.decode(fromUrl(header)));
    if (!isRecord(meta) || meta.alg !== 'ES256' || 'crit' in meta || 'b64' in meta) throw new Error('invalid credential JWS header');
    const ca = await crypto.subtle.importKey('jwk', ecJwk(bundle.ca), p256, false, ['verify']);
    if (!await crypto.subtle.verify(es256, ca, fromUrl(sig), utf8.encode(`${header}.${payload}`))) {
      throw new Error('invalid credential JWS signature');
    }
    const claims: unknown = JSON.parse(text.decode(fromUrl(payload)));
    if (!isRecord(claims)) throw new Error('invalid credential JWS claims');
    const { anon_id, eph_pk, iat, exp } = claims;
    const { integratedTime: time } = bundle.rekor;
    if (typeof anon_id !== 'string' || !anon_id || typeof iat !== 'number' || typeof exp !== 'number' ||
        !Number.isSafeInteger(iat) || !Number.isSafeInteger(exp) || exp <= iat || exp - iat > 600 || time < iat || time > exp) {
      throw new Error('invalid credential validity window');
    }
    const commitment = await microCaCommitment(await microCaFingerprint(bundle.ca), anon_id, policy.salt);
    if (!hashAllowed(commitment, policy, time)) throw new Error('credential not permitted by trust policy');
    const eph = await crypto.subtle.importKey('jwk', ecJwk(eph_pk), p256, true, ['verify']);
    const signature = fromUrl(bundle.sig);
    const digest = hex(await sha256(manifestBytes));
    await checkRekor(bundle.rekor, digest, signature, eph, logSpki);
    // ECDSA with SHA-256 takes the bytes and hashes them once, not twice.
    if (!await crypto.subtle.verify(es256, eph, signature, manifestBytes)) throw new Error('invalid manifest signature');
  } catch (error) {
    throw invalid(error instanceof Error ? error.message : 'micro-CA verification failed', pluginId);
  }
}
