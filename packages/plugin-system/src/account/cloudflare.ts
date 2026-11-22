/**
 * Cloudflare account, held by the kernel (openspec cf-account-admin design.md A1, A3, A5, A6).
 *
 * Signs in with cf's own client id and default scopes through the device flow, keeps the tokens
 * sealed in the vault and in this object's memory only, adds `Authorization` to the official
 * manager's `/__cf/api/**` requests, refreshes before expiry, and decides whether the signed-in
 * account owns this site. Every request leaves through the same-origin relay (`/__cf/**`,
 * functions/__cf/[[path]].ts) because Cloudflare answers no CORS for this site; requests the
 * worker makes itself do not pass through its own fetch handler.
 */
import { KernelError } from '../errors.js';
import { CF_CLIENT_ID, CF_DEFAULT_SCOPES } from './cf-identity.generated.js';
import type { Vault } from './vault.js';

export const CLOUDFLARE = 'cloudflare';
const PENDING = `${CLOUDFLARE}#pending`;
const REFRESH_MARGIN_MS = 60_000;
const SITE_ADMIN_TTL_MS = 10 * 60_000;
const SLOW_DOWN_SECONDS = 5;
const MIN_INTERVAL_SECONDS = 1;
const DEFAULT_INTERVAL_SECONDS = 5;
const MAX_PAGES = 100;
const LOCAL_HOSTS = new Set(['localhost', '127.0.0.1', '[::1]']);

export interface CloudflareUser {
  readonly id: string;
  readonly email: string;
}

/** The Pages project that serves this site and the account that owns it. */
export interface SiteAdmin {
  readonly accountId: string;
  readonly accountName: string;
  readonly projectName: string;
}

/** A device-flow login in progress, as pages may see it: no device code. */
export interface PendingLogin {
  readonly userCode: string;
  readonly verificationUri: string;
  readonly verificationUriComplete?: string;
  readonly expiresAt: number;
  /** Seconds the page waits between polls. */
  readonly interval: number;
}

export type CloudflareStatus =
  | { readonly signedIn: false; readonly pending: PendingLogin | null }
  | {
      readonly signedIn: true;
      readonly user: CloudflareUser | null;
      readonly scopes: readonly string[];
      /** When the current access token expires; it is refreshed before that. */
      readonly expiresAt: number;
      readonly siteAdmin: SiteAdmin | null;
      /** Why the site-admin check could not run; absent when it ran. */
      readonly siteAdminError?: string;
    };

export type PollResult =
  | { readonly state: 'pending'; readonly interval: number }
  | { readonly state: 'slow-down'; readonly interval: number }
  | { readonly state: 'expired' }
  | { readonly state: 'denied' }
  | { readonly state: 'none' }
  | { readonly state: 'signed-in'; readonly status: CloudflareStatus };

interface Credentials {
  readonly accessToken: string;
  readonly refreshToken?: string;
  readonly expiresAt: number;
  readonly scopes: readonly string[];
  readonly user: CloudflareUser | null;
  readonly obtainedAt: number;
}

interface StoredPending extends PendingLogin {
  readonly deviceCode: string;
}

export interface CloudflareAccountOptions {
  /** The site origin; the relay is at `<origin>/__cf/`. */
  readonly origin: string;
  /** Host compared on localhost (design.md A6); written by build-site.mjs. */
  readonly siteHost: string;
  readonly fetch: (input: string, init?: RequestInit) => Promise<Response>;
  readonly vault: Vault;
  readonly now: () => number;
  readonly clientId?: string;
  readonly scopes?: readonly string[];
}

/** Fields copied from a page's request; `authorization` is never one of them. */
const FORWARDED_HEADERS = ['content-type', 'accept'];

const signedOut = (message: string): KernelError => new KernelError('account-signed-out', message);

export class CloudflareAccount {
  private readonly clientId: string;
  private readonly scopes: readonly string[];
  /** Decrypted credentials: `undefined` = not loaded yet, `null` = signed out. */
  private cached: Credentials | null | undefined;
  private refreshing: Promise<Credentials> | undefined;
  /** Bumped by login, refresh and sign-out; work started under an older epoch must not store results. */
  private epoch = 0;
  private siteAdminCache: { readonly epoch: number; readonly at: number; readonly admin: SiteAdmin | null } | undefined;
  private siteAdminInflight: { readonly epoch: number; readonly result: Promise<{ admin: SiteAdmin | null; error?: string }> } | undefined;

  constructor(private readonly options: CloudflareAccountOptions) {
    this.clientId = options.clientId ?? CF_CLIENT_ID;
    this.scopes = options.scopes ?? CF_DEFAULT_SCOPES;
  }

  // ------------------------------------------------------------ status

  async status(): Promise<CloudflareStatus> {
    const credentials = await this.credentials();
    if (credentials === null) return { signedIn: false, pending: await this.pendingView() };
    let check: { admin: SiteAdmin | null; error?: string };
    try {
      check = await this.siteAdmin();
    } catch (error) {
      if (error instanceof KernelError && error.code === 'account-signed-out') {
        return { signedIn: false, pending: await this.pendingView() };
      }
      throw error;
    }
    const current = (await this.credentials()) ?? credentials;
    return {
      signedIn: true,
      user: current.user,
      scopes: current.scopes,
      expiresAt: current.expiresAt,
      siteAdmin: check.admin,
      ...(check.error === undefined ? {} : { siteAdminError: check.error }),
    };
  }

  // ------------------------------------------------------------ device flow

  /** Starts a device-flow login (RFC 8628), replacing any login in progress. */
  async login(): Promise<PendingLogin> {
    const response = await this.oauth('device/auth', {
      client_id: this.clientId,
      scope: [...this.scopes, 'offline_access'].join(' '),
    });
    const body = await readJson(response);
    const deviceCode = stringMember(body, 'device_code');
    const userCode = stringMember(body, 'user_code');
    const verificationUri = stringMember(body, 'verification_uri');
    const expiresIn = numberMember(body, 'expires_in');
    if (!response.ok || deviceCode === undefined || userCode === undefined || verificationUri === undefined || expiresIn === undefined) {
      throw new KernelError('account-failed', `Cloudflare did not start the login (${describe(response, body)})`);
    }
    const complete = stringMember(body, 'verification_uri_complete');
    const pending: StoredPending = {
      deviceCode,
      userCode,
      verificationUri,
      ...(complete === undefined ? {} : { verificationUriComplete: complete }),
      expiresAt: this.options.now() + expiresIn * 1000,
      interval: Math.max(numberMember(body, 'interval') ?? DEFAULT_INTERVAL_SECONDS, MIN_INTERVAL_SECONDS),
    };
    await this.options.vault.put(PENDING, pending);
    return view(pending);
  }

  /**
   * One token request for the login in progress. The page calls this every `interval` seconds:
   * the worker may be stopped between polls, so the kernel keeps no timer of its own.
   */
  async poll(): Promise<PollResult> {
    const pending = await this.options.vault.get<StoredPending>(PENDING);
    if (pending === undefined) return { state: 'none' };
    if (this.options.now() >= pending.expiresAt) {
      await this.options.vault.delete(PENDING);
      return { state: 'expired' };
    }

    const response = await this.oauth('token', {
      grant_type: 'urn:ietf:params:oauth:grant-type:device_code',
      device_code: pending.deviceCode,
      client_id: this.clientId,
    });
    const body = await readJson(response);
    const error = stringMember(body, 'error');
    switch (error) {
      case undefined:
        break;
      case 'authorization_pending':
        return { state: 'pending', interval: pending.interval };
      case 'slow_down': {
        const slower = { ...pending, interval: pending.interval + SLOW_DOWN_SECONDS };
        await this.options.vault.put(PENDING, slower);
        return { state: 'slow-down', interval: slower.interval };
      }
      case 'access_denied':
        await this.options.vault.delete(PENDING);
        return { state: 'denied' };
      case 'expired_token':
        await this.options.vault.delete(PENDING);
        return { state: 'expired' };
      default:
        await this.options.vault.delete(PENDING);
        throw new KernelError('account-failed', `Cloudflare refused the login: ${error}`);
    }

    const accessToken = stringMember(body, 'access_token');
    const expiresIn = numberMember(body, 'expires_in');
    if (!response.ok || accessToken === undefined || expiresIn === undefined) {
      // A 5xx or 429 may pass; anything else will not turn into a grant on the next poll.
      if (response.status >= 500 || response.status === 429) return { state: 'pending', interval: pending.interval };
      await this.options.vault.delete(PENDING);
      throw new KernelError('account-failed', `Cloudflare answered the login with an unusable token (${describe(response, body)})`);
    }

    const now = this.options.now();
    const refreshToken = stringMember(body, 'refresh_token');
    const scope = stringMember(body, 'scope');
    const credentials: Credentials = {
      accessToken,
      ...(refreshToken === undefined ? {} : { refreshToken }),
      expiresAt: now + expiresIn * 1000,
      scopes: scope === undefined ? [] : scope.split(' ').filter((item) => item !== ''),
      user: await this.fetchUser(accessToken),
      obtainedAt: now,
    };
    this.epoch++;
    await this.save(credentials);
    await this.options.vault.delete(PENDING);
    return { state: 'signed-in', status: await this.status() };
  }

  /** Revokes the refresh token if it can and always forgets the credentials and any login in progress. */
  async logout(): Promise<void> {
    const credentials = await this.credentials();
    this.epoch++;
    this.siteAdminCache = undefined;
    if (credentials?.refreshToken !== undefined) {
      try {
        await this.oauth('revoke', {
          client_id: this.clientId,
          token_type_hint: 'refresh_token',
          token: credentials.refreshToken,
        });
      } catch {
        // Offline or refused: the local copy goes regardless (design.md A5).
      }
    }
    await this.clear();
  }

  // ------------------------------------------------------------ relay

  /**
   * Forwards a page's `/__cf/api/**` request with the account's token. The page's own
   * `Authorization` is dropped; `pathAndQuery` starts with `/__cf/api/`.
   */
  async forward(request: Request, pathAndQuery: string): Promise<Response> {
    const token = await this.token();
    const headers = new Headers();
    for (const name of FORWARDED_HEADERS) {
      const value = request.headers.get(name);
      if (value !== null) headers.set(name, value);
    }
    headers.set('authorization', `Bearer ${token}`);
    const hasBody = request.method !== 'GET' && request.method !== 'HEAD';
    let answer: Response;
    try {
      answer = await this.options.fetch(`${this.options.origin}${pathAndQuery}`, {
        method: request.method,
        headers,
        ...(hasBody ? { body: await request.arrayBuffer() } : {}),
        redirect: 'manual',
        ...(request.signal === undefined ? {} : { signal: request.signal }),
      });
    } catch (error) {
      if (request.signal?.aborted === true) throw error;
      throw new KernelError('fetch-failed', `cannot reach Cloudflare: ${error instanceof Error ? error.message : String(error)}`);
    }
    const out = new Headers(answer.headers);
    out.set('cache-control', 'no-store');
    return new Response(answer.body, { status: answer.status, statusText: answer.statusText, headers: out });
  }

  // ------------------------------------------------------------ tokens

  /** A usable access token, refreshed first when it expires within a minute. */
  private async token(): Promise<string> {
    const credentials = await this.credentials();
    if (credentials === null) throw signedOut('not signed in to Cloudflare');
    if (credentials.expiresAt - this.options.now() >= REFRESH_MARGIN_MS) return credentials.accessToken;
    return (await this.refresh(credentials)).accessToken;
  }

  /** One refresh at a time; everyone waiting gets its result. */
  private refresh(credentials: Credentials): Promise<Credentials> {
    this.refreshing ??= this.refreshOnce(credentials).finally(() => {
      this.refreshing = undefined;
    });
    return this.refreshing;
  }

  private async refreshOnce(credentials: Credentials): Promise<Credentials> {
    const epoch = this.epoch;
    if (credentials.refreshToken === undefined) {
      await this.clear();
      throw signedOut('the Cloudflare session expired');
    }
    const response = await this.oauth('token', {
      grant_type: 'refresh_token',
      refresh_token: credentials.refreshToken,
      client_id: this.clientId,
    });
    const body = await readJson(response);
    const error = stringMember(body, 'error');
    if (error === 'invalid_grant' || error === 'invalid_client') {
      if (this.epoch === epoch) await this.clear();
      throw signedOut(`Cloudflare ended the session (${error})`);
    }
    const accessToken = stringMember(body, 'access_token');
    const expiresIn = numberMember(body, 'expires_in');
    if (error !== undefined || !response.ok || accessToken === undefined || expiresIn === undefined) {
      // Not a verdict on the session; keep the credentials and let a later request try again.
      throw new KernelError('account-failed', `Cloudflare did not refresh the session (${describe(response, body)})`);
    }
    if (this.epoch !== epoch) throw signedOut('signed out while refreshing');
    const now = this.options.now();
    const scope = stringMember(body, 'scope');
    const next: Credentials = {
      ...credentials,
      accessToken,
      refreshToken: stringMember(body, 'refresh_token') ?? credentials.refreshToken,
      expiresAt: now + expiresIn * 1000,
      scopes: scope === undefined ? credentials.scopes : scope.split(' ').filter((item) => item !== ''),
      obtainedAt: now,
    };
    this.epoch++;
    await this.save(next);
    return next;
  }

  private async credentials(): Promise<Credentials | null> {
    if (this.cached === undefined) {
      this.cached = (await this.options.vault.get<Credentials>(CLOUDFLARE)) ?? null;
    }
    return this.cached;
  }

  private async save(credentials: Credentials): Promise<void> {
    this.cached = credentials;
    this.siteAdminCache = undefined;
    await this.options.vault.put(CLOUDFLARE, credentials);
  }

  private async clear(): Promise<void> {
    this.cached = null;
    this.epoch++;
    this.siteAdminCache = undefined;
    await this.options.vault.delete(CLOUDFLARE);
    await this.options.vault.delete(PENDING);
  }

  private async pendingView(): Promise<PendingLogin | null> {
    const pending = await this.options.vault.get<StoredPending>(PENDING);
    if (pending === undefined) return null;
    if (this.options.now() >= pending.expiresAt) {
      await this.options.vault.delete(PENDING);
      return null;
    }
    return view(pending);
  }

  // ------------------------------------------------------------ site admin

  /** Whether the account owns the Pages project serving this host (design.md A6). Failures are not cached. */
  private siteAdmin(): Promise<{ admin: SiteAdmin | null; error?: string }> {
    const now = this.options.now();
    const cache = this.siteAdminCache;
    if (cache !== undefined && cache.epoch === this.epoch && now - cache.at < SITE_ADMIN_TTL_MS) {
      return Promise.resolve({ admin: cache.admin });
    }
    if (this.siteAdminInflight?.epoch === this.epoch) return this.siteAdminInflight.result;

    const result = (async (): Promise<{ admin: SiteAdmin | null; error?: string }> => {
      let admin: SiteAdmin | null;
      try {
        admin = await this.findSiteProject();
      } catch (error) {
        if (error instanceof KernelError && error.code === 'account-signed-out') throw error;
        return { admin: null, error: error instanceof Error ? error.message : String(error) };
      }
      // The search may have refreshed the token (a new epoch); the answer is about this account all the same.
      this.siteAdminCache = { epoch: this.epoch, at: this.options.now(), admin };
      return { admin };
    })().finally(() => {
      if (this.siteAdminInflight?.result === result) this.siteAdminInflight = undefined;
    });
    this.siteAdminInflight = { epoch: this.epoch, result };
    return result;
  }

  private async findSiteProject(): Promise<SiteAdmin | null> {
    const hostname = new URL(this.options.origin).hostname.toLowerCase();
    const host = LOCAL_HOSTS.has(hostname) ? this.options.siteHost.toLowerCase() : hostname;
    for await (const account of this.pages<{ id: string; name: string }>('/accounts')) {
      for await (const project of this.pages<{ name: string; subdomain?: string; domains?: string[] }>(
        `/accounts/${encodeURIComponent(account.id)}/pages/projects`,
      )) {
        const names = [project.subdomain, ...(project.domains ?? [])].filter((name): name is string => typeof name === 'string');
        if (names.some((name) => name.toLowerCase() === host)) {
          return { accountId: account.id, accountName: account.name, projectName: project.name };
        }
      }
    }
    return null;
  }

  /** Every item of a paginated v4 list. */
  private async *pages<T>(path: string): AsyncGenerator<T> {
    for (let page = 1; page <= MAX_PAGES; page++) {
      const body = await this.api(`${path}?page=${page}`);
      const items = Array.isArray(body.result) ? (body.result as T[]) : [];
      yield* items;
      const info = isRecord(body.result_info) ? body.result_info : {};
      const total = typeof info.total_pages === 'number' ? info.total_pages : page;
      if (items.length === 0 || page >= total) return;
    }
  }

  private async api(path: string): Promise<Record<string, unknown>> {
    const token = await this.token();
    let response: Response;
    try {
      response = await this.options.fetch(`${this.options.origin}/__cf/api${path}`, {
        headers: { authorization: `Bearer ${token}`, accept: 'application/json' },
      });
    } catch (error) {
      throw new Error(`cannot reach Cloudflare: ${error instanceof Error ? error.message : String(error)}`);
    }
    const body = await readJson(response);
    if (!response.ok || !isRecord(body) || body.success !== true) {
      const errors = isRecord(body) && Array.isArray(body.errors) ? body.errors : [];
      const first = errors.find(isRecord);
      const message = first !== undefined && typeof first.message === 'string' ? first.message : `HTTP ${response.status}`;
      throw new Error(`GET ${path}: ${message}`);
    }
    return body;
  }

  private async fetchUser(accessToken: string): Promise<CloudflareUser | null> {
    try {
      const response = await this.options.fetch(`${this.options.origin}/__cf/api/user`, {
        headers: { authorization: `Bearer ${accessToken}`, accept: 'application/json' },
      });
      const body = await readJson(response);
      const result = isRecord(body) && isRecord(body.result) ? body.result : undefined;
      if (!response.ok || result === undefined || typeof result.id !== 'string') return null;
      return { id: result.id, email: typeof result.email === 'string' ? result.email : '' };
    } catch {
      return null;
    }
  }

  private async oauth(endpoint: 'device/auth' | 'token' | 'revoke', fields: Record<string, string>): Promise<Response> {
    try {
      return await this.options.fetch(`${this.options.origin}/__cf/oauth/${endpoint}`, {
        method: 'POST',
        headers: { 'content-type': 'application/x-www-form-urlencoded', accept: 'application/json' },
        body: new URLSearchParams(fields).toString(),
      });
    } catch (error) {
      throw new KernelError('fetch-failed', `cannot reach Cloudflare: ${error instanceof Error ? error.message : String(error)}`);
    }
  }
}

function view(pending: StoredPending): PendingLogin {
  const { deviceCode: _secret, ...visible } = pending;
  return visible;
}

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === 'object' && value !== null && !Array.isArray(value);
}

async function readJson(response: Response): Promise<unknown> {
  try {
    return JSON.parse(await response.text()) as unknown;
  } catch {
    return undefined;
  }
}

function stringMember(body: unknown, key: string): string | undefined {
  if (!isRecord(body)) return undefined;
  const value = body[key];
  return typeof value === 'string' && value !== '' ? value : undefined;
}

function numberMember(body: unknown, key: string): number | undefined {
  if (!isRecord(body)) return undefined;
  const value = body[key];
  return typeof value === 'number' && Number.isFinite(value) && value > 0 ? value : undefined;
}

function describe(response: Response, body: unknown): string {
  const status = `HTTP ${response.status}`;
  const description = stringMember(body, 'error_description') ?? stringMember(body, 'error');
  return description === undefined ? status : `${status}: ${description}`;
}
