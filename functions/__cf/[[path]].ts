import { decodeRouteKey, encodeBase64Url, normalizeCfRouteHost, readCfRuntimeRouteConfig, validCfOwnerId, type ConfiguredRoute } from './route-config.mjs';

const API_ORIGIN = 'https://api.cloudflare.com';
const API_BASE = '/client/v4/';
const OAUTH_BASE = 'https://dash.cloudflare.com/oauth2/';
const OAUTH_ENDPOINTS = new Set(['device/auth', 'token', 'revoke']);
const FORWARDED_HEADERS = ['authorization', 'content-type', 'accept', 'content-length'];
const PROOF_TTL_MS = 10 * 60_000;
const encoder = new TextEncoder();

type Fetch = (input: string, init?: RequestInit) => Promise<Response>;
type Env = Record<string, unknown>;

function json(status: number, body: object): Response {
  return Response.json(body, { status, headers: { 'cache-control': 'no-store' } });
}

const fail = (status: number, code: string): Response => json(status, { ok: false, code });

/** Prefer the browser's fetch metadata, falling back to the exact Origin header. */
function sameOrigin(request: Request, self: string): boolean {
  const site = request.headers.get('sec-fetch-site');
  return site !== null ? site === 'same-origin' : request.headers.get('origin') === self;
}

function target(path: string, query: string, method: string): URL | 'method' | null {
  if (path.startsWith('/api/')) {
    const upstream = new URL(`${API_ORIGIN}${API_BASE}${path.slice('/api/'.length)}${query}`);
    return upstream.origin === API_ORIGIN && upstream.pathname.startsWith(API_BASE) ? upstream : null;
  }
  if (!path.startsWith('/oauth/')) return null;
  const endpoint = path.slice('/oauth/'.length);
  if (!OAUTH_ENDPOINTS.has(endpoint)) return null;
  return method === 'POST' ? new URL(`${OAUTH_BASE}${endpoint}`) : 'method';
}

async function forward(request: Request, upstream: URL, upstreamFetch: Fetch): Promise<Response> {
  const headers = new Headers();
  for (const name of FORWARDED_HEADERS) {
    const value = request.headers.get(name);
    if (value !== null) headers.set(name, value);
  }
  const hasBody = request.method !== 'GET' && request.method !== 'HEAD';
  try {
    const answer = await upstreamFetch(upstream.href, {
      method: request.method,
      headers,
      body: hasBody ? request.body : null,
      redirect: 'manual',
      // Node and browsers require half-duplex for streamed request bodies.
      ...(hasBody ? { duplex: 'half' } : {}),
    } as RequestInit);
    const out = new Headers(answer.headers);
    out.delete('set-cookie');
    out.set('cache-control', 'no-store');
    return new Response(answer.body, { status: answer.status, statusText: answer.statusText, headers: out });
  } catch {
    return fail(503, 'temporarily-unavailable');
  }
}

/** The first-deploy Worker calls this explicitly. The site Function only uses the private path. */
export async function handleSetup(request: Request, upstreamFetch: Fetch = fetch): Promise<Response> {
  const url = new URL(request.url);
  const upstream = url.pathname.startsWith('/__cf/') ? target(url.pathname.slice('/__cf'.length), url.search, request.method) : null;
  if (!upstream) return fail(404, 'entry-invalid');
  if (upstream === 'method') return fail(405, 'method-not-allowed');
  if (!sameOrigin(request, url.origin)) return fail(403, 'forbidden');
  return forward(request, upstream, upstreamFetch);
}

async function proofKey(env: Env): Promise<CryptoKey> {
  const material = await crypto.subtle.importKey('raw', decodeRouteKey(env.ONLINE_GAL_CF_ROUTE_KEY), 'HKDF', false, ['deriveKey']);
  return crypto.subtle.deriveKey({ name: 'HKDF', hash: 'SHA-256', salt: encoder.encode('online-gal/cf/v1'),
    info: encoder.encode('forwarding-proof') }, material, { name: 'HMAC', hash: 'SHA-256' }, false, ['sign', 'verify']);
}

async function proofMessage(route: ConfiguredRoute, token: string, expiresAt: number): Promise<Uint8Array<ArrayBuffer>> {
  const digest = encodeBase64Url(new Uint8Array(await crypto.subtle.digest('SHA-256', encoder.encode(token))));
  return encoder.encode(JSON.stringify([route.accountId, route.projectName, route.host, route.segment, route.ownerUserId, digest, expiresAt]));
}

async function verifyProof(request: Request, route: ConfiguredRoute, env: Env, token: string | undefined, time: number): Promise<boolean> {
  const header = request.headers.get('x-online-gal-cf-proof');
  const match = header && /^([1-9][0-9]{0,15})\.([A-Za-z0-9_-]{43})$/.exec(header);
  if (!match || !token) return false;
  const expiresAt = Number(match[1]);
  if (!Number.isSafeInteger(expiresAt) || expiresAt <= time || expiresAt > time + PROOF_TTL_MS) return false;
  try {
    return await crypto.subtle.verify('HMAC', await proofKey(env), decodeRouteKey(match[2]), await proofMessage(route, token, expiresAt));
  } catch {
    return false;
  }
}

async function probe<T>(path: string, token: string, upstreamFetch: Fetch): Promise<{ result: T } | { error: Response }> {
  try {
    const response = await upstreamFetch(`${API_ORIGIN}${API_BASE}${path}`, {
      headers: { authorization: `Bearer ${token}`, accept: 'application/json' }, redirect: 'error', signal: AbortSignal.timeout(10_000),
    });
    if (response.status === 401) return { error: fail(401, 'session-expired') };
    if (response.status === 403 || response.status === 404) return { error: fail(403, 'not-admin') };
    if (!response.ok) return { error: fail(503, 'temporarily-unavailable') };
    const body = await response.json() as { success?: boolean; result?: T } | null;
    if (body?.success !== true || !body.result) return { error: fail(503, 'temporarily-unavailable') };
    return { result: body.result };
  } catch {
    return { error: fail(503, 'temporarily-unavailable') };
  }
}

function hasConfiguredHost(domains: unknown, host: string): boolean {
  return Array.isArray(domains) && domains.some((value: unknown) => {
    try { return normalizeCfRouteHost(value) === host; } catch { return false; }
  });
}

async function siteAdmin(route: ConfiguredRoute, env: Env, token: string, upstreamFetch: Fetch, now: () => number): Promise<Response> {
  const user = await probe<{ id?: unknown }>('user', token, upstreamFetch);
  if ('error' in user) return user.error;
  if (!validCfOwnerId(user.result.id) || user.result.id !== route.ownerUserId) return fail(403, 'not-admin');
  const project = await probe<{ name?: unknown; domains?: unknown }>(`accounts/${route.accountId}/pages/projects/${route.projectName}`, token, upstreamFetch);
  if ('error' in project) return project.error;
  if (project.result.name !== route.projectName || !hasConfiguredHost(project.result.domains, route.host)) return fail(403, 'not-admin');
  const expiresAt = now() + PROOF_TTL_MS;
  const signature = await crypto.subtle.sign('HMAC', await proofKey(env), await proofMessage(route, token, expiresAt));
  const proof = `${expiresAt}.${encodeBase64Url(new Uint8Array(signature))}`;
  const admin = { accountId: route.accountId, accountName: route.accountId, projectName: route.projectName };
  return json(200, { ok: true, admin, proof, expiresAt });
}

export async function handle(request: Request, env: Env, upstreamFetch: Fetch = fetch, now = Date.now): Promise<Response> {
  const url = new URL(request.url);
  const route = await readCfRuntimeRouteConfig(env).catch(() => null);
  if (!route || url.protocol !== 'https:' || url.host !== route.host) return fail(404, 'entry-invalid');
  const prefix = `/__cf/${route.segment}`;
  if (!url.pathname.startsWith(`${prefix}/`)) return fail(404, 'entry-invalid');
  const path = url.pathname.slice(prefix.length);
  const isProbe = path === '/site-admin';
  const upstream = isProbe ? null : target(path, url.search, request.method);
  if (!isProbe && !upstream) return fail(404, 'entry-invalid');
  if (upstream === 'method' || (isProbe && request.method !== 'GET')) return fail(405, 'method-not-allowed');
  if (!sameOrigin(request, url.origin)) return fail(403, 'forbidden');
  const token = /^Bearer ([a-z0-9._~+/=-]{1,8192})$/i.exec(request.headers.get('authorization') ?? '')?.[1];
  if (isProbe) return token ? siteAdmin(route, env, token, upstreamFetch, now) : fail(401, 'session-expired');
  if (path.startsWith('/api/') && !(await verifyProof(request, route, env, token, now()))) return fail(403, 'proof-invalid');
  return forward(request, upstream!, upstreamFetch);
}

export const onRequest = (context: { request: Request; env: Env }): Promise<Response> => handle(context.request, context.env);
