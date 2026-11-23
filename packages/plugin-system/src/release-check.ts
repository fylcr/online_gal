import { parseBundle, verifyBundle, type TrustPolicy } from './attest.js';
import { KernelError } from './errors.js';
import { BUNDLE_FILE, MANIFEST_FILE, PACKAGE_FILE, parseEnvelope, parsePackage } from './format.js';
import { hex, sha256, text } from './rekor.js';
import { officialPackagePath, parseReleaseTrust, type ReleaseTrust } from './release-metadata.js';
import { fetchTrustRecord, type DohObservation } from './trust-dns.js';

export type { ReleaseTrust } from './release-metadata.js';
export type { DohObservation } from './trust-dns.js';
export const VERIFY_RELEASE_MESSAGE = 'online-gal:verify-release';

export interface VerifiedOfficialPackage {
  readonly id: string;
  readonly version: string;
  readonly digest: string;
  readonly path: string;
}

export interface ReleaseAcceptance {
  readonly ok: boolean;
  readonly resolvers: readonly DohObservation[];
  readonly packages: readonly VerifiedOfficialPackage[];
  readonly error?: { readonly stage: 'doh' | 'packages'; readonly message: string; readonly code?: string };
}

type Fetcher = (url: string, init?: RequestInit) => Promise<Response>;
const request: Fetcher = (url, init) => globalThis.fetch(url, init);
const isRecord = (value: unknown): value is Record<string, unknown> =>
  typeof value === 'object' && value !== null && !Array.isArray(value);

async function fetchOfficialPackages(host: string, policy: TrustPolicy, fetcher: Fetcher): Promise<VerifiedOfficialPackage[]> {
  const origin = `https://${host}`;
  async function read(path: string): Promise<ArrayBuffer> {
    const response = await fetcher(`${origin}/registry/${path}`, {
      cache: 'no-store', credentials: 'omit', redirect: 'error', signal: AbortSignal.timeout(10_000),
    });
    if (!response.ok) throw new Error(`registry/${path} returned HTTP ${response.status}`);
    return response.arrayBuffer();
  }
  const json = (bytes: ArrayBuffer): unknown => JSON.parse(text.decode(bytes));
  const registry = json(await read('index.json'));
  if (!isRecord(registry) || registry.format !== 1 || !isRecord(registry.plugins) ||
      !Object.hasOwn(registry.plugins, 'online-gal.manager')) {
    throw new Error('the official registry does not contain the default manager');
  }
  return Promise.all(Object.entries(registry.plugins).map(async ([id, entry]) => {
    if (!/^[a-z0-9]+(?:[.-][a-z0-9]+)*$/.test(id) || !isRecord(entry) || typeof entry.version !== 'string' ||
        typeof entry.digest !== 'string' || !/^[a-f0-9]{64}$/.test(entry.digest) ||
        typeof entry.path !== 'string' || !entry.path.startsWith(`p/${id}/`) ||
        !/^[a-f0-9]{16}\/$/.test(entry.path.slice(`p/${id}/`.length))) {
      throw new Error('the official registry contains a malformed entry');
    }
    const { path, digest, version } = entry;
    const envelope = await read(`${path}${PACKAGE_FILE}`);
    const pointers = parseEnvelope(json(envelope));
    if (pointers.manifest !== MANIFEST_FILE || pointers.bundle !== BUNDLE_FILE) {
      throw new Error(`${id} does not have a signed format 2 envelope`);
    }
    const manifest = await read(`${path}${MANIFEST_FILE}`);
    if (hex(await sha256(manifest)) !== digest) throw new Error(`${id} manifest does not match its registry digest`);
    const pkg = parsePackage(json(manifest));
    if (pkg.manifest.id !== id || pkg.manifest.version !== version) throw new Error(`${id} manifest does not match its registry identity`);
    const bundle = await read(`${path}${BUNDLE_FILE}`);
    if (await officialPackagePath(id, envelope, manifest, bundle) !== path) {
      throw new Error(`${id} package bytes do not match its immutable directory`);
    }
    await verifyBundle(parseBundle(json(bundle)), manifest, policy, id);
    return { id, version, digest, path };
  }));
}

function sameHost(host: string, origin: string): void {
  if (origin !== `https://${host}`) throw new Error('release trust host does not match the current HTTPS origin');
}

/** Before TXT publication, check the deployed bytes against the candidate trust commitment. */
export async function verifyOfficialRelease(candidate: ReleaseTrust): Promise<VerifiedOfficialPackage[]> {
  const { host, salt, hash } = parseReleaseTrust(candidate);
  if (typeof location !== 'undefined') sameHost(host, location.origin);
  return fetchOfficialPackages(host, { salt, hashes: [hash] }, request);
}

/** Called by the site's actual service worker. Use fresh DoH responses for acceptance. */
export async function checkReleaseFromWorker(candidate: unknown, origin: string, fetcher: Fetcher): Promise<ReleaseAcceptance> {
  let resolvers: readonly DohObservation[] = [];
  let stage: 'doh' | 'packages' = 'doh';
  try {
    const { host, salt, hash } = parseReleaseTrust(candidate);
    sameHost(host, origin);
    const policy = await fetchTrustRecord(host, { fetch: fetcher, observe: (observations) => { resolvers = observations; } });
    if (policy.salt !== salt || policy.hashes.length !== 1 || policy.hashes[0]?.toLowerCase() !== hash) {
      throw new KernelError('attestation-invalid', 'DNS trust TXT does not match the published candidate');
    }
    stage = 'packages';
    const packages = await fetchOfficialPackages(host, policy, fetcher);
    return { ok: true, resolvers, packages };
  } catch (error) {
    return {
      ok: false, resolvers, packages: [], error: {
        stage,
        message: error instanceof Error ? error.message : String(error),
        ...(error instanceof KernelError ? { code: error.code } : {}),
      },
    };
  }
}

function activeWorker(registration: ServiceWorkerRegistration): Promise<ServiceWorker> {
  return new Promise((resolve, reject) => {
    const worker = registration.installing ?? registration.waiting ?? registration.active;
    if (worker === null) {
      reject(new Error('the site Service Worker is unavailable'));
      return;
    }
    const timeout = setTimeout(() => finish(new Error('the site Service Worker did not activate')), 30_000);
    function finish(error?: Error): void {
      clearTimeout(timeout);
      worker?.removeEventListener('statechange', onState);
      if (error !== undefined) reject(error);
      else if (worker !== null) resolve(worker);
    }
    function onState(): void {
      if (worker?.state === 'activated') finish();
      if (worker?.state === 'redundant') finish(new Error('the site Service Worker was replaced'));
    }
    worker.addEventListener('statechange', onState);
    onState();
  });
}

/** After TXT publication, ask the site Service Worker to check DoH and verify the same packages. */
export async function verifyReleaseInServiceWorker(candidate: ReleaseTrust): Promise<ReleaseAcceptance> {
  const trust = parseReleaseTrust(candidate);
  sameHost(trust.host, location.origin);
  const registration = await navigator.serviceWorker.register('/sw.js', { scope: '/', updateViaCache: 'none' });
  await registration.update();
  const worker = await activeWorker(registration);
  return new Promise((resolve, reject) => {
    const { port1, port2 } = new MessageChannel();
    const timeout = setTimeout(() => {
      port1.close();
      reject(new Error('the site Service Worker did not respond to release verification'));
    }, 60_000);
    port1.onmessage = (event: MessageEvent<ReleaseAcceptance>) => {
      clearTimeout(timeout);
      port1.close();
      resolve(event.data);
    };
    try {
      worker.postMessage({ type: VERIFY_RELEASE_MESSAGE, trust }, [port2]);
    } catch (error) {
      clearTimeout(timeout);
      port1.close();
      port2.close();
      reject(error);
    }
  });
}
