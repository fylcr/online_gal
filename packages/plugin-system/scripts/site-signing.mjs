import { parseReleaseTrust } from '../dist/release-metadata.js';

function requiredToken(token) {
  if (typeof token !== 'string' || !/^[A-Za-z0-9_-]{43}$/.test(token) ||
      Buffer.from(token, 'base64url').toString('base64url') !== token) {
    throw new Error('ONLINE_GAL_CA_BUILD_TOKEN must be a base64url-encoded 32-byte token');
  }
  return token;
}

export function siteSigning(env) {
  const token = env.ONLINE_GAL_CA_BUILD_TOKEN;
  if (env.CF_PAGES !== '1') {
    return token ? { mode: 'manual', token: requiredToken(token), host: env.SITE_HOST || undefined } : { mode: 'unsigned' };
  }

  let record;
  try {
    record = JSON.parse(env.ONLINE_GAL_SITE || 'null');
  } catch {
    throw new Error('ONLINE_GAL_SITE must be valid JSON in a Pages site build');
  }
  const branch = record?.productionBranch;
  if (typeof branch === 'string' && branch && env.CF_PAGES_BRANCH && env.CF_PAGES_BRANCH !== branch) {
    return { mode: 'preview' };
  }
  const bootstrap = env.ONLINE_GAL_CA_BOOTSTRAP;
  if (record?.trust === undefined && !bootstrap) return { mode: 'unsigned' };
  if (record?.v !== 1 || typeof branch !== 'string' || !branch || branch !== env.CF_PAGES_BRANCH) {
    throw new Error('Pages signing requires matching production branch metadata in ONLINE_GAL_SITE');
  }
  const trust = parseReleaseTrust(record.trust);
  const buildToken = requiredToken(token);
  if (bootstrap === '1') return { mode: 'bootstrap' };
  if (bootstrap) throw new Error('ONLINE_GAL_CA_BOOTSTRAP must be 1 or unset');
  return { mode: 'required', token: buildToken, host: trust.host, trust };
}
