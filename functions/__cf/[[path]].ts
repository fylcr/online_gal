/**
 * Same-origin relay to Cloudflare (openspec/changes/cf-account-admin design.md A2).
 *
 * Neither `api.cloudflare.com` nor `dash.cloudflare.com/oauth2/*` answers CORS for this site, so
 * the browser cannot reach them directly. This Pages Function forwards exactly two families of
 * requests and nothing else:
 *
 *   /__cf/api/<rest>?<query>       any method -> https://api.cloudflare.com/client/v4/<rest>?<query>
 *   /__cf/oauth/{device/auth,token,revoke}  POST -> https://dash.cloudflare.com/oauth2/<same>
 *
 * The token passes through here (accepted by the user, m01973 item 4), so the relay never logs a
 * header or a body. It forwards only a fixed header allowlist (no Cookie, no client IP), strips
 * `set-cookie` from the answer and marks it `no-store`. The same-origin check only stops a
 * drive-by page from using a signed-in visitor's browser; it is not abuse protection, which is
 * deferred (design.md Deferred).
 */

const API_ORIGIN = 'https://api.cloudflare.com';
const API_BASE = '/client/v4/';
const OAUTH_BASE = 'https://dash.cloudflare.com/oauth2/';
const OAUTH_ENDPOINTS = new Set(['device/auth', 'token', 'revoke']);
const FORWARDED_HEADERS = ['authorization', 'content-type', 'accept', 'content-length'];

type Fetch = (input: Request | string, init?: RequestInit) => Promise<Response>;

const plain = (status: number, message: string): Response =>
  new Response(message, {
    status,
    headers: { 'content-type': 'text/plain; charset=utf-8', 'cache-control': 'no-store' },
  });

/** Same-origin when the browser says so; with no Sec-Fetch-Site at all, the Origin must match. */
function sameOrigin(request: Request, self: string): boolean {
  const site = request.headers.get('sec-fetch-site');
  if (site !== null) return site === 'same-origin';
  return request.headers.get('origin') === self;
}

/** The upstream URL for a relay path, or `null` when the path is not one this relay serves. */
function target(url: URL, method: string): URL | 'method' | null {
  const path = url.pathname;
  if (path.startsWith('/__cf/api/')) {
    // The pathname is still percent-encoded, so encoded slashes and dot segments reach the URL
    // parser below unchanged; the origin and prefix are re-checked after it resolves them.
    const upstream = new URL(`${API_ORIGIN}${API_BASE}${path.slice('/__cf/api/'.length)}${url.search}`);
    if (upstream.origin !== API_ORIGIN || !upstream.pathname.startsWith(API_BASE)) return null;
    return upstream;
  }
  if (path.startsWith('/__cf/oauth/')) {
    const endpoint = path.slice('/__cf/oauth/'.length);
    if (!OAUTH_ENDPOINTS.has(endpoint)) return null;
    if (method !== 'POST') return 'method';
    return new URL(`${OAUTH_BASE}${endpoint}`);
  }
  return null;
}

export async function handle(request: Request, upstreamFetch: Fetch = fetch): Promise<Response> {
  const url = new URL(request.url);
  const upstream = target(url, request.method);
  if (upstream === null) return plain(404, 'Not found');
  if (upstream === 'method') return plain(405, 'Method not allowed');
  if (!sameOrigin(request, url.origin)) return plain(403, 'Forbidden');

  const headers = new Headers();
  for (const name of FORWARDED_HEADERS) {
    const value = request.headers.get(name);
    if (value !== null) headers.set(name, value);
  }
  const hasBody = request.method !== 'GET' && request.method !== 'HEAD';
  const answer = await upstreamFetch(upstream.href, {
    method: request.method,
    headers,
    body: hasBody ? request.body : null,
    redirect: 'manual',
    // Streaming request bodies need half-duplex outside Workers (Node's undici, browsers).
    ...(hasBody ? { duplex: 'half' } : {}),
  } as RequestInit);

  const out = new Headers(answer.headers);
  out.delete('set-cookie');
  out.set('cache-control', 'no-store');
  return new Response(answer.body, { status: answer.status, statusText: answer.statusText, headers: out });
}

export const onRequest = (context: { request: Request }): Promise<Response> => handle(context.request);
