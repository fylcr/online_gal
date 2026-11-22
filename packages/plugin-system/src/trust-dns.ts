/**
 * DNS TXT trust-anchor queries for plugin attestation (design.md K16).
 *
 * The trust record lives at `_trust.<domain>` and is formatted as:
 *
 *   v=1; salt=<base64url>; hashes=<item>,<item>,...
 *
 * Each item in `hashes` is one of:
 *   <hex64>                  — always-valid hash
 *   <hex64>:<after>:<before> — hash valid only when unix seconds ∈ (after, before)
 *                              either bound may be empty (no bound on that side)
 *
 * The two channels share one salt and one list; their prefixes keep them apart:
 *   github:  SHA-256("github|" + issuer + "|" + workflowUri + "|" + salt)
 *   microca: SHA-256("microca|" + ca_fingerprint + "|" + anon_id + "|" + salt)
 * where `ca_fingerprint` is SHA-256 of the canonical UTF-8 JSON of the CA JWK
 * (see githubCommitment and microCaCommitment in attest.ts).
 *
 * Queries run in parallel against two DoH resolvers (Cloudflare and Google).
 * At least one must respond with AD=true; when multiple respond the TXT content
 * must agree. All resolvers failing throws `attestation-unavailable` (retryable).
 *
 * Results are cached in the kernel state (salt + hashes + fetch time) so the
 * DNS query does not run on every install — only on install and background
 * update checks.
 *
 * TODO: implement once the micro-CA and DNS record are provisioned (design.md K14–K17).
 */

import { KernelError } from './errors.js';
import type { TrustPolicy } from './attest.js';

export const DOH_RESOLVERS = [
  'https://cloudflare-dns.com/dns-query',
  'https://dns.google/resolve',
] as const;

/** Parsed `_trust.<domain>` TXT record. */
export interface TrustRecord extends TrustPolicy {
  readonly fetchedAt: number;
}

export interface DohOptions {
  readonly fetch: (url: string, init?: RequestInit) => Promise<Response>;
  readonly now?: () => number;
}

/**
 * Fetches and parses the `_trust.<domain>` TXT record from both DoH resolvers,
 * confirms at least one reported AD=true and both agree on content.
 *
 * Throws `attestation-unavailable` when all resolvers fail (network issue,
 * retryable). Throws `attestation-invalid` when records disagree or are
 * structurally invalid.
 *
 * TODO: implement (design.md K16).
 */
export async function fetchTrustRecord(
  _domain: string,
  _options: DohOptions,
): Promise<TrustRecord> {
  // TODO: implement parallel DoH queries, AD-bit check, record agreement check,
  // and TXT parsing.
  throw new KernelError('attestation-unavailable', 'DNS trust-anchor queries are not yet implemented');
}

/**
 * Returns true when `hash` appears in the policy's allowlist and, if the entry
 * carries a time window, the `atTime` unix timestamp (seconds) falls within it.
 */
export function hashAllowed(hash: string, policy: TrustPolicy, atTime: number): boolean {
  for (const item of policy.hashes) {
    const parts = item.split(':');
    const h = parts[0] ?? '';
    if (h.toLowerCase() !== hash.toLowerCase()) continue;
    const after = parts[1] !== undefined && parts[1] !== '' ? Number(parts[1]) : -Infinity;
    const before = parts[2] !== undefined && parts[2] !== '' ? Number(parts[2]) : Infinity;
    if (atTime > after && atTime < before) return true;
  }
  return false;
}
