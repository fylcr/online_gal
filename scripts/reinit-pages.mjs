import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { parseArgs } from 'node:util';
import { invokeCf } from './configure-pages.mjs';

const RECORD = 'ONLINE_GAL_SITE';
const ENVIRONMENTS = ['production', 'preview'];
const USAGE = 'Usage: npm run reinit:pages -- <project> [--dry-run]';

export function reinitPages(projectName, { dryRun = false } = {}, runCf = invokeCf) {
  if (!/^[a-z0-9][a-z0-9-]*$/.test(projectName ?? '')) throw new Error(USAGE);
  const readProject = () => {
    const project = runCf(['pages', 'get', projectName]);
    if (project?.name !== projectName) throw new Error('cf did not return the requested Pages project.');
    if (!['github', 'gitlab'].includes(project.source?.type)) throw new Error('This project has no Git integration.');
    if (!project.production_branch) throw new Error('This project has no production branch configured.');
    return project;
  };
  const { production_branch: branch } = readProject();
  if (dryRun) return { branch };

  const deployment_configs = Object.fromEntries(ENVIRONMENTS.map((name) => [name, { env_vars: { [RECORD]: null } }]));
  runCf(['pages', 'edit', projectName, '--body', JSON.stringify({ deployment_configs })]);
  const project = readProject();
  if (ENVIRONMENTS.some((name) => project.deployment_configs?.[name]?.env_vars?.[RECORD] != null)) {
    throw new Error('Pages still reports ONLINE_GAL_SITE. No production deployment was requested.');
  }
  if (project.production_branch !== branch) throw new Error('The production branch changed. No production deployment was requested.');
  const deployment = runCf(['pages', 'deployments', 'create', projectName, '--branch', branch]);
  if (typeof deployment?.id !== 'string' || !/^[a-zA-Z0-9-]+$/.test(deployment.id)) {
    throw new Error('No valid deployment ID returned. Check Pages before retrying.');
  }
  return { branch, deploymentId: deployment.id };
}

function main() {
  const { values, positionals } = parseArgs({ allowPositionals: true, options: {
    help: { type: 'boolean', short: 'h' }, 'dry-run': { type: 'boolean' },
  } });
  if (values.help) { console.log(USAGE); return; }
  if (positionals.length !== 1) throw new Error(USAGE);
  const result = reinitPages(positionals[0], { dryRun: values['dry-run'] });
  if (values['dry-run']) {
    console.log('Would clear ONLINE_GAL_SITE in production and preview. Other variables would be unchanged.');
    console.log(`Would request a production deployment of ${result.branch}.`);
  } else {
    console.log(`Production deployment requested: ${result.deploymentId}`);
  }
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  try { main(); }
  catch (error) { console.error(error.message); process.exitCode = 1; }
}
