import { KernelError } from '../errors.js';
import type { Vault } from './vault.js';
import type { SiteAdmin } from './cloudflare.js';

const ENTRY = 'cloudflare#entry';
const PROOF_TTL_MS = 10 * 60_000;

export const validCfEntry = (value: unknown): value is string =>
  typeof value === 'string' && /^[A-Za-z0-9_-]{43}$/.test(value);

type RelayFetch = (input: string, init?: RequestInit) => Promise<Response>;
type Endpoint = { readonly segment: string; readonly revision: number };
interface Grant {
  readonly admin: SiteAdmin;
  readonly proof: string;
  readonly expiresAt: number;
}
interface Probe {
  readonly endpoint: Endpoint;
  readonly token: string;
  readonly result: Promise<Grant>;
}

export class PrivateCfRelay {
  private entry: Promise<string | null> | undefined;
  private revision = 0;
  private probe: Probe | undefined;

  constructor(
    private readonly origin: string,
    private readonly fetcher: RelayFetch,
    private readonly now: () => number,
    private readonly vault?: Vault,
  ) {}

  async setEntry(segment: string): Promise<void> {
    if (!validCfEntry(segment)) throw new KernelError('request-invalid', 'invalid Cloudflare entry');
    const previous = this.readEntry();
    this.revision++;
    this.invalidate();
    const write = previous.then(async () => {
      await this.vault?.put(ENTRY, segment);
      return segment;
    });
    this.entry = write.catch(() => previous);
    await write;
  }

  private readEntry(): Promise<string | null> {
    return this.entry ??= this.vault?.get<unknown>(ENTRY).then(value => validCfEntry(value) ? value : null)
      ?? Promise.resolve(null);
  }

  async endpoint(): Promise<Endpoint> {
    const revision = this.revision;
    const segment = await this.readEntry();
    if (segment === null) throw new KernelError('entry-required', 'import the private Cloudflare entry first');
    const endpoint = { segment, revision };
    this.assertCurrent(endpoint);
    return endpoint;
  }

  assertCurrent(endpoint: Endpoint): void {
    if (endpoint.revision !== this.revision) throw new KernelError('entry-invalid', 'the Cloudflare entry changed');
  }

  invalidate(): void {
    this.probe = undefined;
  }

  async request(endpoint: Endpoint, path: string, init: RequestInit): Promise<Response> {
    this.assertCurrent(endpoint);
    let response: Response;
    try {
      response = await this.fetcher(`${this.origin}/__cf/${endpoint.segment}${path}`, {
        ...init, cache: 'no-store', credentials: 'omit', redirect: 'manual', referrerPolicy: 'no-referrer',
      });
    } catch {
      init.signal?.throwIfAborted();
      throw new KernelError('temporarily-unavailable', 'the Cloudflare relay is temporarily unavailable');
    }
    this.assertCurrent(endpoint);
    if (response.type === 'opaqueredirect' || response.redirected || response.status >= 300 && response.status < 400
      || response.headers.get('content-type')?.toLowerCase().includes('text/html')) {
      throw new KernelError('entry-invalid', 'the Cloudflare entry is no longer valid');
    }
    if (response.status === 404 || response.status === 403 || response.status === 503 || response.status === 401) {
      const body = await response.clone().json().catch(() => null);
      this.assertCurrent(endpoint);
      if (body?.ok === false) {
        if (response.status === 404 && body.code === 'entry-invalid') {
          throw new KernelError('entry-invalid', 'the Cloudflare entry is no longer valid');
        }
        if (response.status === 401 && body.code === 'session-expired') {
          throw new KernelError('account-signed-out', 'the Cloudflare session expired');
        }
        if (response.status === 403 && body.code === 'not-admin') {
          throw new KernelError('not-admin', 'the Cloudflare account is not the site owner');
        }
        if (response.status === 403 && body.code === 'proof-invalid') {
          throw new KernelError('proof-invalid', 'the Cloudflare forwarding proof expired or was rejected');
        }
        if (response.status === 503 && body.code === 'temporarily-unavailable') {
          throw new KernelError('temporarily-unavailable', 'the Cloudflare relay is temporarily unavailable');
        }
      }
    }
    return response;
  }

  async grant(endpoint: Endpoint, token: string): Promise<Grant> {
    this.assertCurrent(endpoint);
    const existing = this.probe;
    if (existing?.endpoint.revision === endpoint.revision && existing.token === token) {
      const grant = await existing.result;
      this.assertCurrent(endpoint);
      if (this.probe !== existing) throw new KernelError('proof-invalid', 'the Cloudflare forwarding proof changed');
      if (this.now() < grant.expiresAt) return grant;
      this.invalidate();
      return this.grant(endpoint, token);
    }
    const result = this.probeSite(endpoint, token);
    const probe = { endpoint, token, result };
    this.probe = probe;
    try {
      return await result;
    } catch (error) {
      if (this.probe === probe) this.invalidate();
      throw error;
    }
  }

  private async probeSite(endpoint: Endpoint, token: string): Promise<Grant> {
    const response = await this.request(endpoint, '/site-admin', {
      headers: { authorization: `Bearer ${token}`, accept: 'application/json' },
    });
    const body = await response.json().catch(() => null);
    this.assertCurrent(endpoint);
    const admin = body?.admin;
    const expiresAt = body?.expiresAt;
    const proof = body?.proof;
    const now = this.now();
    if (!response.ok || body?.ok !== true || typeof admin?.accountId !== 'string' || !admin.accountId
      || typeof admin.accountName !== 'string' || typeof admin.projectName !== 'string' || !admin.projectName
      || !Number.isSafeInteger(expiresAt) || expiresAt <= now || typeof proof !== 'string'
      || !/^[1-9][0-9]{0,15}\.[A-Za-z0-9_-]{43}$/.test(proof) || !proof.startsWith(`${expiresAt}.`)) {
      throw new KernelError('temporarily-unavailable', 'the Cloudflare relay returned an invalid administrator response');
    }
    return {
      admin: { accountId: admin.accountId, accountName: admin.accountName, projectName: admin.projectName },
      expiresAt: Math.min(expiresAt, now + PROOF_TTL_MS),
      proof,
    };
  }
}
