export interface SetupEnv {
  ONLINE_GAL_SITE?: string;
}

function readState(record: string | undefined) {
  if (!record) return { initialized: false };
  try {
    const value: unknown = JSON.parse(record);
    if (value !== null && typeof value === 'object' && 'v' in value && value.v === 1) {
      return { initialized: true, v: 1 };
    }
  } catch {
    // A malformed record must not lock the site.
  }
  return { initialized: true, warning: 'invalid-site-record' };
}

const VERIFY_BODY_LIMIT = 8192;

async function readVerifyBody(request: Request): Promise<Record<string, unknown>> {
  const reader = request.body?.getReader();
  if (!reader) throw new Error('Missing body');
  const decoder = new TextDecoder();
  let size = 0;
  let text = '';
  try {
    for (;;) {
      const { value, done } = await reader.read();
      if (done) break;
      size += value.byteLength;
      if (size > VERIFY_BODY_LIMIT) {
        void reader.cancel().catch(() => {});
        throw new Error('Body too large');
      }
      text += decoder.decode(value, { stream: true });
    }
    const body: unknown = JSON.parse(text + decoder.decode());
    if (typeof body !== 'object' || body === null || Array.isArray(body)) throw new Error('Invalid body');
    return body as Record<string, unknown>;
  } finally {
    reader.releaseLock();
  }
}

async function verifyToken(request: Request, url: URL, headers: Headers, fetchUpstream: typeof fetch): Promise<Response> {
  const invalid = { ok: false, reason: 'invalid' };
  const unavailable = { ok: false, reason: 'upstream' };
  const json = (body: object, status = 200) => Response.json(body, { status, headers });
  const site = request.headers.get('sec-fetch-site');
  if (site !== null ? site !== 'same-origin' : request.headers.get('origin') !== url.origin) return json({ ok: false }, 403);
  if (request.headers.get('content-type')?.split(';', 1)[0]?.trim().toLowerCase() !== 'application/json') return json({ ok: false }, 400);

  let body: Record<string, unknown>;
  try {
    body = await readVerifyBody(request);
  } catch {
    return json({ ok: false }, 400);
  }

  const { kind, token, accountId } = body;
  if (typeof token !== 'string' || !/^[!-~]{1,512}$/.test(token)) return json({ ok: false }, 400);
  const metadata = (value: unknown) => typeof value === 'string' && !value.includes(token) ? value : undefined;
  const upstream = (path: string, github = false) => fetchUpstream(path, {
    headers: {
      authorization: `Bearer ${token}`,
      accept: github ? 'application/vnd.github+json' : 'application/json',
      'user-agent': 'online-gal-setup',
      ...(github ? { 'x-github-api-version': '2022-11-28' } : {}),
    },
    redirect: 'error',
    signal: AbortSignal.timeout(10_000),
  });

  try {
    if (kind === 'github') {
      const response = await upstream('https://api.github.com/user', true);
      if (response.status !== 200) {
        void response.body?.cancel().catch(() => {});
        return json(response.status === 401 || response.status === 403 ? invalid : unavailable);
      }
      const user = await response.json() as { login?: string };
      const scopes = metadata(response.headers.get('x-oauth-scopes'));
      return json({
        ok: true,
        login: metadata(user.login),
        scopes: scopes === undefined ? null : scopes.split(',').map((scope) => scope.trim()).filter(Boolean),
        expiresAt: metadata(response.headers.get('github-authentication-token-expiration')) ?? null,
      });
    }
    if (kind !== 'cloudflare' || typeof accountId !== 'string' || !/^[0-9a-f]{32}$/.test(accountId)) return json({ ok: false }, 400);

    const baseUrl = 'https://api.cloudflare.com/client/v4';
    let upstreamError = false;
    for (const path of ['/user/tokens/verify', `/accounts/${accountId}/tokens/verify`]) {
      const response = await upstream(`${baseUrl}${path}`);
      if (response.ok) {
        const verification = await response.json() as { success?: boolean; result?: { status?: string; expires_on?: string } };
        if (verification.success === true && verification.result?.status === 'active') {
          return json({ ok: true, status: 'active', expiresOn: metadata(verification.result.expires_on) });
        }
      } else {
        void response.body?.cancel().catch(() => {});
        if (response.status === 429 || response.status >= 500) upstreamError = true;
      }
    }
    return json(upstreamError ? unavailable : invalid);
  } catch {
    return json(unavailable);
  }
}

export function onRequest(
  { request, env }: { request: Request; env: SetupEnv },
  fetchUpstream: typeof fetch = fetch,
): Response | Promise<Response> {
  const headers = new Headers({
    'cache-control': 'no-store',
    'x-content-type-options': 'nosniff',
  });
  const url = new URL(request.url);
  const head = request.method === 'HEAD';
  const notFound = () => new Response(head ? null : 'Not found', { status: 404, headers });
  const methodNotAllowed = (allow: string) => {
    headers.set('allow', allow);
    return new Response(head ? null : 'Method not allowed', { status: 405, headers });
  };

  if (url.pathname === '/__setup/verify') {
    if (env.ONLINE_GAL_SITE !== undefined) return notFound();
    return request.method === 'POST' ? verifyToken(request, url, headers, fetchUpstream) : methodNotAllowed('POST');
  }
  if (url.pathname !== '/__setup/status') return notFound();
  if (request.method !== 'GET' && !head) return methodNotAllowed('GET, HEAD');
  headers.set('content-type', 'application/json; charset=utf-8');
  const body = JSON.stringify({ phase: 'setup', ...readState(env.ONLINE_GAL_SITE) });
  return new Response(head ? null : body, { headers });
}
