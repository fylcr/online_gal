const ROUTE_KEY = 'ONLINE_GAL_CF_ROUTE_KEY';
const DOMAIN = 'online-gal/cf-route/v1';
const encoder = new TextEncoder();

export function encodeBase64Url(bytes) {
  return btoa(String.fromCharCode(...bytes)).replaceAll('+', '-').replaceAll('/', '_').replace(/=+$/, '');
}

export function decodeRouteKey(value) {
  if (typeof value !== 'string' || !/^[A-Za-z0-9_-]{43}$/.test(value)) throw invalidConfig();
  const bytes = Uint8Array.from(atob(value.replaceAll('-', '+').replaceAll('_', '/') + '='), (char) => char.charCodeAt(0));
  if (bytes.length !== 32 || encodeBase64Url(bytes) !== value) throw invalidConfig();
  return bytes;
}

export function normalizeCfRouteHost(host) {
  const canonical = typeof host === 'string' ? host.toLowerCase().replace(/\.$/, '') : '';
  const labels = canonical.split('.');
  if (canonical.length > 253 || labels.length < 2 || /^\d+$/.test(labels.at(-1)) ||
      labels.some((label) => !/^[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?$/.test(label))) {
    throw invalidConfig();
  }
  return canonical;
}

export function validCfOwnerId(value) {
  return typeof value === 'string' && /^[a-f0-9]{32}$/.test(value);
}

function invalidConfig() {
  return new Error('Invalid Cloudflare Pages route configuration');
}

export async function deriveCfRouteConfig({ accountId, projectName, host, routeKey }) {
  if (typeof accountId !== 'string' || !/^[a-f0-9]{32}$/i.test(accountId) ||
      typeof projectName !== 'string' || !/^[a-z0-9](?:[a-z0-9-]{0,56}[a-z0-9])?$/.test(projectName)) {
    throw invalidConfig();
  }
  const canonicalHost = normalizeCfRouteHost(host);
  const account = accountId.toLowerCase();
  const key = await crypto.subtle.importKey('raw', decodeRouteKey(routeKey), { name: 'HMAC', hash: 'SHA-256' }, false, ['sign']);
  const message = encoder.encode(JSON.stringify([DOMAIN, account, projectName, canonicalHost]));
  const signature = await crypto.subtle.sign('HMAC', key, message);
  return { accountId: account, projectName, host: canonicalHost, segment: encodeBase64Url(new Uint8Array(signature)) };
}

function siteRecord(env) {
  try {
    return env.ONLINE_GAL_SITE ? JSON.parse(env.ONLINE_GAL_SITE) : null;
  } catch {
    throw invalidConfig();
  }
}

async function configuredRoute(env, record) {
  if (!record?.cfRoute && !env[ROUTE_KEY] && !env.ONLINE_GAL_CF_OWNER_ID) return null;
  if (record?.v !== 1 || record.cfRoute?.v !== 1 || !validCfOwnerId(env.ONLINE_GAL_CF_OWNER_ID) ||
      typeof record.productionBranch !== 'string' || !record.productionBranch ||
      record.cfRoute.host !== normalizeCfRouteHost(record.cfRoute.host) ||
      (record.trust?.host !== undefined && record.trust.host !== record.cfRoute.host)) throw invalidConfig();
  const route = await deriveCfRouteConfig({ accountId: record.account, projectName: record.project,
    host: record.cfRoute.host, routeKey: env[ROUTE_KEY] });
  return { ...route, ownerUserId: env.ONLINE_GAL_CF_OWNER_ID };
}

// Runtime bindings do not include the Git build environment. The Function checks host and path.
export async function readCfRuntimeRouteConfig(env) {
  return configuredRoute(env, siteRecord(env));
}

// The build publishes a route only for the production branch of a Git-integrated Pages build.
export async function readCfRouteConfig(env) {
  if (env.CF_PAGES !== '1' || !env.CF_PAGES_BRANCH) return null;
  const record = siteRecord(env);
  if (record?.productionBranch && record.productionBranch !== env.CF_PAGES_BRANCH) return null;
  return configuredRoute(env, record);
}
