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
 *              base64); `proof` is { checkpoint, hashes, checkpoint } for the
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
 * TODO: implement once the anonymous edge micro-CA is deployed (design.md K14–K17).
 * Until then `verifyBundle` is a type-safe stub that always resolves without error
 * when no bundle is present (GitHub sources), and rejects with `attestation-invalid`
 * when a bundle is present but invalid JSON — matching the kernel's current stub
 * in `verifyAttestation`.
 */

import { KernelError } from './errors.js';

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

/**
 * Full cryptographic verification of a bundle against the manifest digest and
 * the trust policy from DNS.
 *
 * TODO: implement (design.md K16).
 */
export async function verifyBundle(
  _bundle: AttestationBundle,
  _manifestDigest: string,
  _policy: TrustPolicy,
  _pluginId: string,
): Promise<void> {
  // TODO: implement the seven-step verification described at the top of this file.
  throw invalid('attestation verification is not yet implemented', _pluginId);
}
