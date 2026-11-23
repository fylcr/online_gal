interface Env {
  ONLINE_GAL_CA_BUILD_TOKEN?: string;
  ONLINE_GAL_CA_PRIVATE_JWK?: string;
}

const keyPart = /^[A-Za-z0-9_-]{43}$/;
const encoder = new TextEncoder();

function publicKey(input: unknown): JsonWebKey | null {
  if (typeof input !== 'object' || input === null) return null;
  const { kty, crv, x, y } = input as JsonWebKey;
  if (kty !== 'EC' || crv !== 'P-256' || typeof x !== 'string' || typeof y !== 'string' ||
      !keyPart.test(x) || !keyPart.test(y)) return null;
  return { kty, crv, x, y };
}

const base64url = (bytes: Uint8Array): string =>
  btoa(String.fromCharCode(...bytes)).replace(/\+/g, '-').replace(/\//g, '_').replace(/=+$/, '');
const encodeJson = (input: unknown): string => base64url(encoder.encode(JSON.stringify(input)));

function reply(status: number, body: object): Response {
  return Response.json(body, {
    status,
    headers: { 'cache-control': 'no-store', 'x-content-type-options': 'nosniff' },
  });
}

async function tokenMatches(expected: string, provided: string): Promise<boolean> {
  // Web Crypto verifies the MAC in constant time; both tokens have the same fixed length.
  const key = await crypto.subtle.generateKey({ name: 'HMAC', hash: 'SHA-256' }, false, ['sign', 'verify']);
  const mac = await crypto.subtle.sign('HMAC', key, encoder.encode(expected));
  return crypto.subtle.verify('HMAC', key, mac, encoder.encode(provided));
}

export async function onRequest({ request, env }: { request: Request; env: Env }): Promise<Response> {
  if (request.method !== 'POST') {
    const response = reply(405, { error: 'method-not-allowed' });
    response.headers.set('allow', 'POST');
    return response;
  }
  const token = env.ONLINE_GAL_CA_BUILD_TOKEN;
  if (!token || !keyPart.test(token)) return reply(503, { error: 'build-token-not-configured' });
  const provided = /^Bearer ([A-Za-z0-9_-]{43})$/i.exec(request.headers.get('authorization') ?? '')?.[1];
  if (!provided || !await tokenMatches(token, provided)) return reply(401, { error: 'unauthorized' });
  if (!env.ONLINE_GAL_CA_PRIVATE_JWK) return reply(503, { error: 'ca-key-not-configured' });

  let eph_pk: JsonWebKey | null = null;
  try {
    const body = await request.json() as { eph_pk?: unknown } | null;
    eph_pk = publicKey(body?.eph_pk);
    if (!eph_pk) return reply(400, { error: 'invalid-ephemeral-key' });
    await crypto.subtle.importKey('jwk', eph_pk, { name: 'ECDSA', namedCurve: 'P-256' }, false, ['verify']);
  } catch {
    return reply(400, { error: 'invalid-ephemeral-key' });
  }

  try {
    const privateKey = JSON.parse(env.ONLINE_GAL_CA_PRIVATE_JWK) as JsonWebKey;
    const ca = publicKey(privateKey);
    if (!ca || typeof privateKey.d !== 'string' || !keyPart.test(privateKey.d)) {
      return reply(503, { error: 'ca-key-invalid' });
    }
    const key = await crypto.subtle.importKey('jwk', { ...ca, d: privateKey.d }, { name: 'ECDSA', namedCurve: 'P-256' }, false, ['sign']);
    const iat = Math.floor(Date.now() / 1000);
    const jws = `${encodeJson({ alg: 'ES256' })}.${encodeJson({ anon_id: 'cf-pages', eph_pk, iat, exp: iat + 600 })}`;
    const sig = await crypto.subtle.sign({ name: 'ECDSA', hash: 'SHA-256' }, key, encoder.encode(jws));
    return reply(200, { ca, cred: `${jws}.${base64url(new Uint8Array(sig))}` });
  } catch {
    return reply(503, { error: 'ca-key-invalid' });
  }
}
