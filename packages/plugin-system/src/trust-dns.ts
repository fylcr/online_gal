/**
 * DNS TXT trust-anchor queries for plugin attestation (design.md K16).
 *
 * The trust record lives at `_trust.<domain>` and is formatted as:
 *
 *   v=1; salt=<base64url>; hashes=<item>,<item>,...
 *
 * Each item is either a 64-character hex commitment or a commitment with an
 * optional `(after, before)` time window. The record is read through both
 * public DNS-over-HTTPS resolvers and is accepted only when the responses
 * agree and at least one resolver reports DNSSEC authentication.
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
  /** Effective DNS TTL in seconds, capped at one hour. */
  readonly ttl?: number;
  /** Absolute expiry in milliseconds, derived from fetchedAt and ttl. */
  readonly expiresAt?: number;
}

export interface DohOptions {
  readonly fetch: (url: string, init?: RequestInit) => Promise<Response>;
  readonly now?: () => number;
  /** Receives browser-visible observations even if the combined policy rejects them. */
  readonly observe?: (results: readonly DohObservation[]) => void;
}

export interface DohObservation {
  readonly resolver: (typeof DOH_RESOLVERS)[number];
  readonly cors: 'readable' | 'unavailable';
  readonly httpStatus: number | null;
  readonly ad: boolean | null;
  readonly txt: string | null;
  readonly error?: string;
}

interface DohAnswer {
  readonly name?: unknown;
  readonly type?: unknown;
  readonly TTL?: unknown;
  readonly data?: unknown;
}

interface DohResponse {
  readonly Status?: unknown;
  readonly AD?: unknown;
  readonly TC?: unknown;
  readonly CD?: unknown;
  readonly Answer?: unknown;
}

interface ResolverRecord {
  readonly record: TrustRecord;
  readonly authenticated: boolean;
  readonly ttl: number;
}

const TXT_TYPE = 16;
const TRUST_VERSION = '1';
const HEX_HASH = /^[0-9a-f]{64}$/i;
const BASE64URL = /^[A-Za-z0-9_-]+$/;
const DOMAIN = /^[A-Za-z0-9](?:[A-Za-z0-9.-]*[A-Za-z0-9])?\.?$/;
const MAX_TTL_SECONDS = 60 * 60;
const DOH_TIMEOUT_MS = 10_000;

function invalid(message: string): KernelError {
  return new KernelError('attestation-invalid', message);
}

function unavailable(message: string): KernelError {
  return new KernelError('attestation-unavailable', message);
}

function canonicalName(name: string): string {
  return name.toLowerCase().replace(/\.$/, '');
}

/**
 * Decodes DNS JSON presentation text. A TXT RR is one or more quoted
 * character-strings; the strings are concatenated without adding spaces.
 * Decimal DNS escapes (\\DDD) and escaped octets are accepted, but bare text
 * and unterminated/partially escaped strings are rejected.
 */
function decodeTxtData(data: string): string {
  let out = '';
  let index = 0;
  let segments = 0;
  while (index < data.length) {
    while (index < data.length && /\s/.test(data[index] ?? '')) index += 1;
    if (index >= data.length) break;
    if (data[index] !== '"') throw invalid('DNS TXT data must contain quoted character-strings');
    segments += 1;
    index += 1;
    let closed = false;
    let segmentBytes = 0;
    while (index < data.length) {
      const character = data[index++];
      if (character === '"') {
        closed = true;
        break;
      }
      if (character !== '\\') {
        segmentBytes += new TextEncoder().encode(character).byteLength;
        if (segmentBytes > 255) throw invalid('DNS TXT character-string exceeds 255 octets');
        out += character;
        continue;
      }
      if (index >= data.length) throw invalid('DNS TXT data has an incomplete escape');
      if (/^[0-9]$/.test(data[index] ?? '')) {
        const decimal = data.slice(index, index + 3);
        if (!/^[0-9]{3}$/.test(decimal)) throw invalid('DNS TXT data has an incomplete decimal escape');
        const value = Number(decimal);
        if (value > 255) throw invalid('DNS TXT data has an invalid decimal escape');
        segmentBytes += 1;
        if (segmentBytes > 255) throw invalid('DNS TXT character-string exceeds 255 octets');
        out += String.fromCharCode(value);
        index += 3;
      } else {
        segmentBytes += 1;
        if (segmentBytes > 255) throw invalid('DNS TXT character-string exceeds 255 octets');
        out += data[index++];
      }
    }
    if (!closed) throw invalid('DNS TXT data has an unterminated string');
  }
  if (segments === 0) throw invalid('DNS TXT data has no character-strings');
  return out;
}

function parseTrustText(text: string, fetchedAt: number): TrustRecord {
  const fields = new Map<string, string>();
  for (const field of text.split(';')) {
    const separator = field.indexOf('=');
    if (separator <= 0) throw invalid('DNS trust TXT contains a malformed field');
    const key = field.slice(0, separator).trim();
    const value = field.slice(separator + 1).trim();
    if (fields.has(key)) throw invalid(`DNS trust TXT contains duplicate field "${key}"`);
    if (key !== 'v' && key !== 'salt' && key !== 'hashes') {
      throw invalid(`DNS trust TXT contains an unknown field "${key}"`);
    }
    fields.set(key, value);
  }
  if (fields.get('v') !== TRUST_VERSION) throw invalid('DNS trust TXT has an unsupported version');
  const salt = fields.get('salt');
  if (salt === undefined || salt === '' || !BASE64URL.test(salt)) {
    throw invalid('DNS trust TXT salt must be a non-empty base64url value');
  }
  const hashesText = fields.get('hashes');
  if (hashesText === undefined || hashesText === '') throw invalid('DNS trust TXT hashes must not be empty');
  const hashes = hashesText.split(',').map((item) => item.trim());
  for (const item of hashes) {
    const parts = item.split(':');
    if (parts.length !== 1 && parts.length !== 3) throw invalid('DNS trust TXT contains a malformed hash entry');
    if (!HEX_HASH.test(parts[0] ?? '')) throw invalid('DNS trust TXT contains a hash that is not 64 hex characters');
    for (const bound of parts.slice(1)) {
      if (bound !== '' && (!/^[0-9]+$/.test(bound) || !Number.isSafeInteger(Number(bound)))) {
        throw invalid('DNS trust TXT contains an invalid hash time window');
      }
    }
    if (parts.length === 3 && parts[1] !== '' && parts[2] !== '' && Number(parts[1]) >= Number(parts[2])) {
      throw invalid('DNS trust TXT hash time window is empty or reversed');
    }
  }
  return { salt, hashes, fetchedAt };
}

function recordContent(record: TrustRecord): string {
  return `${record.salt}\u0000${record.hashes.join('\u0000')}`;
}

function parseTtl(value: unknown): number {
  if (typeof value !== 'number' || !Number.isSafeInteger(value) || value < 0) {
    throw invalid('DNS TXT answer has an invalid TTL');
  }
  return value;
}

function parseDohAnswers(answer: unknown, owner: string, fetchedAt: number, observeTxt: (txt: string) => void): { record: TrustRecord; ttl: number } {
  if (!Array.isArray(answer)) throw invalid('DoH response has no answer section');
  const records = answer.filter(
    (item): item is DohAnswer =>
      typeof item === 'object' && item !== null && !Array.isArray(item) &&
      (item as DohAnswer).type === TXT_TYPE &&
      typeof (item as DohAnswer).name === 'string' &&
      canonicalName((item as DohAnswer).name as string) === canonicalName(owner),
  );
  if (records.length !== 1) {
    throw invalid(records.length === 0 ? 'DoH response contains no TXT trust record at the queried owner' : 'DoH response contains ambiguous TXT trust records');
  }
  const item = records[0];
  if (item === undefined) throw invalid('DoH response contains no TXT trust record at the queried owner');
  if (typeof item.data !== 'string') throw invalid('DNS TXT answer data is not a string');
  const txt = decodeTxtData(item.data);
  observeTxt(txt);
  return { record: parseTrustText(txt, fetchedAt), ttl: parseTtl(item.TTL) };
}

type ObservationState = { -readonly [K in keyof DohObservation]: DohObservation[K] };

function responseRecord(value: unknown, owner: string, fetchedAt: number, observation: ObservationState): ResolverRecord {
  if (typeof value !== 'object' || value === null || Array.isArray(value)) throw invalid('DoH response is not an object');
  const response = value as DohResponse;
  if (typeof response.AD === 'boolean') observation.ad = response.AD;
  if (response.Status !== 0) throw invalid('DoH response returned a DNS error status');
  if (response.AD !== true && response.AD !== false) throw invalid('DoH response has no valid DNSSEC authentication flag');
  if (response.TC !== undefined && typeof response.TC !== 'boolean') throw invalid('DoH response has an invalid truncation flag');
  if (response.CD !== undefined && typeof response.CD !== 'boolean') throw invalid('DoH response has an invalid DNSSEC checking flag');
  if (response.TC === true) throw invalid('DoH response is truncated');
  if (response.CD === true) throw invalid('DoH response disabled DNSSEC checking');
  const parsed = parseDohAnswers(response.Answer, owner, fetchedAt, (txt) => { observation.txt = txt; });
  return { record: parsed.record, authenticated: response.AD, ttl: parsed.ttl };
}

async function queryResolver(
  observation: ObservationState,
  domain: string,
  fetcher: DohOptions['fetch'],
  fetchedAt: number,
): Promise<ResolverRecord> {
  const { resolver } = observation;
  const separator = resolver.includes('?') ? '&' : '?';
  const owner = `_trust.${domain}`;
  const url = `${resolver}${separator}name=${encodeURIComponent(owner)}&type=TXT`;
  const controller = new AbortController();
  let timeout: ReturnType<typeof setTimeout> | undefined;
  const request = Promise.resolve().then(() =>
    fetcher(url, { mode: 'cors', credentials: 'omit', redirect: 'error', cache: 'no-store', headers: { accept: 'application/dns-json' }, signal: controller.signal }),
  ).then(async (response) => {
    observation.cors = 'readable';
    observation.httpStatus = response.status;
    if (!response.ok) throw invalid(`DNS trust resolver ${resolver} returned HTTP ${response.status}`);
    let body: unknown;
    try {
      body = await response.json();
    } catch {
      throw invalid(`DNS trust resolver ${resolver} returned invalid JSON`);
    }
    return responseRecord(body, owner, fetchedAt, observation);
  });
  try {
    const timeoutPromise = new Promise<never>((_, reject) => {
      timeout = setTimeout(() => reject(unavailable(`DNS trust resolver ${resolver} timed out`)), DOH_TIMEOUT_MS);
    });
    return await Promise.race([request, timeoutPromise]);
  } catch (error) {
    controller.abort();
    const failure = error instanceof KernelError ? error : unavailable(`DNS trust resolver ${resolver} could not be reached`);
    observation.error = failure.message;
    throw failure;
  } finally {
    if (timeout !== undefined) clearTimeout(timeout);
  }
}

/**
 * Fetches and parses the `_trust.<domain>` TXT record from both DoH resolvers.
 * Transport failures are retryable only when no resolver produced a response;
 * any reached malformed, unsigned, contradictory, or DNSSEC-invalid response
 * is a terminal attestation failure.
 */
export async function fetchTrustRecord(domain: string, options: DohOptions): Promise<TrustRecord> {
  if (!DOMAIN.test(domain) || domain.includes('/') || domain.includes('\\') || domain.includes('?') || domain.includes('#')) {
    throw invalid('DNS trust domain is invalid');
  }
  const fetchedAt = options.now?.() ?? Date.now();
  const observations: ObservationState[] = DOH_RESOLVERS.map((resolver) => ({
    resolver, cors: 'unavailable', httpStatus: null, ad: null, txt: null,
  }));
  const results = await Promise.allSettled(
    observations.map((observation) => queryResolver(observation, domain, options.fetch, fetchedAt)),
  );
  options.observe?.(observations.map((observation) => ({ ...observation })));
  const rejected = results.filter((result): result is PromiseRejectedResult => result.status === 'rejected');
  const accepted = results
    .filter((result): result is PromiseFulfilledResult<ResolverRecord> => result.status === 'fulfilled')
    .map((result) => result.value);
  const invalidFailure = rejected.find(
    (result) => result.reason instanceof KernelError && result.reason.code === 'attestation-invalid',
  );
  if (invalidFailure !== undefined) throw invalidFailure.reason;
  if (accepted.length === 0) throw unavailable('all DNS trust resolvers failed');
  const first = accepted[0];
  if (first === undefined) throw unavailable('all DNS trust resolvers failed');
  if (accepted.some(({ record }) => recordContent(record) !== recordContent(first.record))) {
    throw invalid('DNS trust resolvers returned different trust records');
  }
  if (!accepted.some(({ authenticated }) => authenticated)) {
    throw invalid('DNS trust resolvers did not authenticate the trust record');
  }
  const ttl = Math.min(...accepted.map(({ ttl }) => ttl), MAX_TTL_SECONDS);
  return {
    ...first.record,
    ttl,
    expiresAt: fetchedAt + ttl * 1000,
  };
}

/** Returns true when a commitment is present and within its strict time window. */
export function hashAllowed(hash: string, policy: TrustPolicy, atTime: number): boolean {
  if (!Number.isFinite(atTime)) return false;
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
