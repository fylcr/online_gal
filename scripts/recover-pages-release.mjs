import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { parseArgs } from 'node:util';
import { invokeCf } from './configure-pages.mjs';
import { normalizeCfRouteHost } from '../functions/__cf/route-config.mjs';

const USAGE = 'Usage: npm run recover:pages-release -- <project> --account <id> --host <host> [--profile <profile>]';
const ID = /^[a-f0-9]{8}(?:-[a-f0-9]{4}){3}-[a-f0-9]{12}$/;
const completed = deployment => deployment.latest_stage?.name === 'deploy' && deployment.latest_stage.status === 'success';
const reusable = deployment => !['failure', 'canceled'].includes(deployment.latest_stage?.status);

function inspectProject(project, { projectName, accountId, host }) {
  const env = project?.deployment_configs?.production?.env_vars ?? {};
  const marker = env.ONLINE_GAL_SITE;
  let record;
  try { record = JSON.parse(marker?.value); } catch { /* Report a sanitized configuration error. */ }
  if (project?.name !== projectName || !['github', 'gitlab'].includes(project.source?.type) || !project.domains?.includes(host) ||
      marker?.type !== 'plain_text' || record?.v !== 1 || record.project !== projectName || record.account !== accountId ||
      !record.productionBranch || record.productionBranch !== project.production_branch || record.trust?.host !== host ||
      !/^[A-Za-z0-9_-]{43}$/.test(record.trust.salt ?? '') || !/^[a-f0-9]{64}$/.test(record.trust.hash ?? '') ||
      !Number.isFinite(Date.parse(record.at))) throw new Error('The initialized release does not match the requested account, project, host and branch.');
  const bootstrap = env.ONLINE_GAL_CA_BOOTSTRAP;
  if (bootstrap != null && (bootstrap.type !== 'plain_text' || bootstrap.value !== '1')) throw new Error('The CA bootstrap marker is malformed.');
  return { after: Date.parse(record.at), trust: record.trust, branch: record.productionBranch, bootstrap: bootstrap != null };
}

async function fetchIdentity(origin, fetcher) {
  const response = await fetcher(`${origin}/.well-known/online-gal-deployment.json`, {
    cache: 'no-store', credentials: 'omit', redirect: 'error', signal: AbortSignal.timeout(15_000),
  });
  if (!response.ok || !/\bno-store\b/i.test(response.headers.get('cache-control') ?? '')) throw new Error('The deployment identity response is unavailable.');
  return response.json();
}

async function checkIdentity(deployment, projectName, branch, origin, fetcher) {
  const expected = `https://${deployment.id.slice(0, 8)}.${projectName}.pages.dev`;
  if (deployment.url !== expected || deployment.deployment_trigger?.metadata?.branch !== branch) {
    throw new Error('Cloudflare returned a mismatched deployment address or branch.');
  }
  const commit = deployment.deployment_trigger.metadata.commit_hash;
  const [immutable, current] = await Promise.all([fetchIdentity(expected, fetcher), fetchIdentity(origin, fetcher)]);
  for (const identity of [immutable, current]) {
    if (identity?.url !== expected || identity.branch !== branch || (commit && identity.commit !== commit)) {
      throw new Error('The production host or deployment address points to a different deployment.');
    }
  }
  if (immutable.commit !== current.commit) throw new Error('The production host and immutable deployment report different commits.');
  return expected;
}

async function checkBootstrap(origin, fetcher) {
  const response = await fetcher(`${origin}/__ca/issue`, {
    method: 'POST', cache: 'no-store', credentials: 'omit', redirect: 'error', signal: AbortSignal.timeout(15_000),
  });
  if (response.status !== 401 || !/\bno-store\b/i.test(response.headers.get('cache-control') ?? '') ||
      (await response.json())?.error !== 'unauthorized') throw new Error('The production CA bootstrap endpoint is not ready.');
}

export async function recoverPagesRelease(projectName, { accountId, host: inputHost, profile } = {},
  { runCf = invokeCf, fetcher = fetch, verify = async (trust, origin) => {
    const { verifyOfficialReleaseAt } = await import('../packages/plugin-system/dist/release-check.js');
    return verifyOfficialReleaseAt(trust, origin);
  } } = {}) {
  if (!/^[a-z0-9](?:[a-z0-9-]{0,56}[a-z0-9])?$/.test(projectName ?? '') || !/^[a-f0-9]{32}$/.test(accountId ?? '') || !inputHost) throw new Error(USAGE);
  const host = normalizeCfRouteHost(inputHost);
  const origin = `https://${host}`;
  const cf = args => runCf(profile ? [...args, '--profile', profile] : args, { accountId, oauthOnly: true });
  const state = inspectProject(cf(['pages', 'get', projectName]), { projectName, accountId, host });
  const { branch } = state;
  const deployments = cf(['pages', 'deployments', 'list', '--project-name', projectName, '--env', 'production', '--page', '1', '--per-page', '25']);
  if (!Array.isArray(deployments)) throw new Error('cf did not return a production deployment list.');
  const relevant = deployments.filter(deployment => ID.test(deployment.id ?? '') && Date.parse(deployment.created_on) > state.after &&
    deployment.deployment_trigger?.metadata?.branch === branch);
  const active = relevant.filter(reusable).sort((a, b) => Date.parse(a.created_on) - Date.parse(b.created_on));
  const limit = state.bootstrap ? 1 : 2;
  if (deployments.length >= 25 || active.length > limit ||
      (active.length === 2 && Date.parse(active[0].created_on) === Date.parse(active[1].created_on))) {
    throw new Error('The release deployment history is ambiguous. Confirm the deployment IDs in Pages.');
  }
  const [bootstrap, signing] = active;
  if (!bootstrap) return { stage: state.bootstrap ? 'bootstrap-needed' : 'bootstrap-history-missing', branch };
  const progress = { branch, bootstrapDeploymentId: bootstrap.id };
  if (!completed(bootstrap)) return { stage: 'bootstrap-running', ...progress };
  if (state.bootstrap) {
    await checkIdentity(bootstrap, projectName, branch, origin, fetcher);
    await checkBootstrap(origin, fetcher);
    await checkIdentity(bootstrap, projectName, branch, origin, fetcher);
    return { stage: 'bootstrap-ready', ...progress };
  }
  if (!signing) {
    await checkIdentity(bootstrap, projectName, branch, origin, fetcher);
    return { stage: 'signing-needed', ...progress };
  }
  const both = { ...progress, signingDeploymentId: signing.id };
  if (!completed(signing)) return { stage: 'signing-running', ...both };
  const signingOrigin = await checkIdentity(signing, projectName, branch, origin, fetcher);
  const packages = await verify(state.trust, signingOrigin);
  await checkIdentity(signing, projectName, branch, origin, fetcher);
  return { stage: 'awaiting-dns', ...both, packages: packages.map(({ id, version, digest }) => ({ id, version, digest })) };
}

async function main() {
  const { values, positionals } = parseArgs({ allowPositionals: true, options: {
    help: { type: 'boolean', short: 'h' }, account: { type: 'string' }, host: { type: 'string' }, profile: { type: 'string' },
  } });
  if (values.help) { console.log(USAGE); return; }
  if (positionals.length !== 1) throw new Error(USAGE);
  const result = await recoverPagesRelease(positionals[0], { accountId: values.account, host: values.host, profile: values.profile });
  console.log(JSON.stringify(result, null, 2));
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  main().catch(error => { console.error(error.message); process.exitCode = 1; });
}
