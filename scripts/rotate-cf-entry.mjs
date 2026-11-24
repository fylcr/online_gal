import { randomBytes } from 'node:crypto';
import { mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { parseArgs } from 'node:util';
import { cfRouteOwnerBinding, deriveCfRouteConfig, normalizeCfRouteHost, validCfOwnerId } from '../functions/__cf/route-config.mjs';
import { invokeCf } from './configure-pages.mjs';

const RECORD = 'ONLINE_GAL_SITE';
const KEY = 'ONLINE_GAL_CF_ROUTE_KEY';
const OWNER = 'ONLINE_GAL_CF_OWNER_ID';
const USAGE = 'Usage: npm run rotate:cf-entry -- <project> --account <id> --host <host> [--profile <profile>] [--apply --entry-file <path>]';

function readInitializedProject(project, projectName, accountId, host) {
  const branch = project?.production_branch;
  if (project?.name !== projectName || !branch || !['github', 'gitlab'].includes(project.source?.type)) {
    throw new Error('cf did not return the requested Git-integrated Pages project and production branch.');
  }
  const hasHost = project.domains?.some(domain => {
    try { return normalizeCfRouteHost(domain) === host; } catch { return false; }
  });
  if (!hasHost) throw new Error('The official host is not bound to this Pages project.');
  const env = project.deployment_configs?.production?.env_vars ?? {};
  const preview = project.deployment_configs?.preview?.env_vars ?? {};
  if (preview[OWNER] != null || preview[KEY] != null) throw new Error('Preview contains private relay configuration. Review the project before rotating.');
  let record;
  try { record = JSON.parse(env[RECORD]?.value); } catch { /* Report only the configuration category. */ }
  if (env[RECORD]?.type !== 'plain_text' || record?.v !== 1 || record.account !== accountId || record.project !== projectName ||
      (record.productionBranch !== undefined && record.productionBranch !== branch) ||
      (record.trust !== undefined && record.trust?.host !== host)) {
    throw new Error('The initialized project record does not match the account, project, branch and host.');
  }
  if (env.ONLINE_GAL_CA_BOOTSTRAP != null) throw new Error('Resume the in-progress release before rotating its entry.');
  const bound = record.cfRoute !== undefined || env[OWNER] != null || env[KEY] != null;
  if (bound && (record.cfRoute?.v !== 1 || record.cfRoute.host !== host || record.productionBranch !== branch ||
      env[OWNER]?.type !== 'secret_text' || env[KEY]?.type !== 'secret_text')) {
    throw new Error('The existing private relay configuration is incomplete. Review it before rotating.');
  }
  return { branch, record, bound };
}

export async function rotateCfEntry(projectName, { accountId, host: inputHost, profile, apply = false, entryFile } = {}, runCf = invokeCf) {
  if (!/^[a-z0-9](?:[a-z0-9-]{0,56}[a-z0-9])?$/.test(projectName ?? '') || !/^[a-f0-9]{32}$/.test(accountId ?? '') ||
      !inputHost || apply !== Boolean(entryFile)) throw new Error(USAGE);
  const host = normalizeCfRouteHost(inputHost);
  const cf = args => runCf(profile ? [...args, '--profile', profile] : args, { accountId, oauthOnly: true });
  const read = () => readInitializedProject(cf(['pages', 'get', projectName]), projectName, accountId, host);
  const before = read();
  const user = cf(['user', 'get']);
  const ownerBinding = validCfOwnerId(user?.id) ? await cfRouteOwnerBinding(accountId, projectName, user.id) : undefined;
  if (!ownerBinding || (before.bound ? ownerBinding !== before.record.cfRoute.ownerBinding : user.id !== before.record.by)) {
    throw new Error('The current cf login cannot be matched to the initialization owner.');
  }
  const result = { action: before.bound ? 'rotate' : 'bind', branch: before.branch };
  if (!apply) return result;

  const fresh = read();
  if (fresh.branch !== before.branch || fresh.bound !== before.bound || JSON.stringify(fresh.record) !== JSON.stringify(before.record)) {
    throw new Error('The Pages initialization state changed. Review it before retrying.');
  }
  const key = randomBytes(32).toString('base64url');
  const { segment } = await deriveCfRouteConfig({ accountId, projectName, host, routeKey: key });
  const secret = value => ({ type: 'secret_text', value });
  const env_vars = { [KEY]: secret(key) };
  const record = before.bound ? before.record : {
    ...before.record, by: '', productionBranch: before.branch, cfRoute: { v: 1, host, ownerBinding },
  };
  if (!before.bound) {
    env_vars[OWNER] = secret(user.id);
    env_vars[RECORD] = { type: 'plain_text', value: JSON.stringify(record) };
  }
  const temp = mkdtempSync(join(tmpdir(), 'online-gal-entry-'));
  try {
    const requestFile = join(temp, 'patch.json');
    writeFileSync(requestFile, JSON.stringify({ deployment_configs: { production: { env_vars } } }), { flag: 'wx', mode: 0o600 });
    writeFileSync(entryFile, `https://${host}/admin#cf-entry=${segment}\n`, { flag: 'wx', mode: 0o600 });
    try {
      cf(['pages', 'edit', projectName, '--body', `@${requestFile}`]);
    } catch {
      throw new Error('The Pages write could not be confirmed. The new entry file was kept. Check the project before retrying.');
    }
  } finally {
    rmSync(temp, { recursive: true, force: true });
  }
  const after = read();
  if (!after.bound || after.branch !== before.branch || JSON.stringify(after.record) !== JSON.stringify(record)) {
    throw new Error('Pages did not retain the expected configuration. The new entry file was kept.');
  }
  const deployment = cf(['pages', 'deployments', 'create', projectName, '--branch', before.branch]);
  if (!/^[a-f0-9]{8}(?:-[a-f0-9]{4}){3}-[a-f0-9]{12}$/.test(deployment?.id ?? '')) {
    throw new Error('No deployment ID was returned. Check the Pages deployment history before retrying.');
  }
  return { ...result, deploymentId: deployment.id };
}

async function main() {
  const { values, positionals } = parseArgs({ allowPositionals: true, options: {
    help: { type: 'boolean', short: 'h' }, account: { type: 'string' }, host: { type: 'string' },
    profile: { type: 'string' }, apply: { type: 'boolean' }, 'entry-file': { type: 'string' },
  } });
  if (values.help) { console.log(USAGE); return; }
  if (positionals.length !== 1) throw new Error(USAGE);
  const result = await rotateCfEntry(positionals[0], {
    accountId: values.account, host: values.host, profile: values.profile, apply: values.apply, entryFile: values['entry-file'],
  });
  console.log(result.deploymentId
    ? `Private entry saved to the specified file. Production deployment requested (${result.deploymentId}).`
    : `Would ${result.action} the private relay for the production branch (${result.branch}).`);
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  main().catch(error => { console.error(error.message); process.exitCode = 1; });
}
