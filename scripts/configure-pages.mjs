import { spawnSync } from 'node:child_process';
import { parseArgs } from 'node:util';
import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const CF_PACKAGE = 'cf@1.0.0-rc.2';
const SETTINGS = { build_command: 'npm run build:pages', root_dir: '', destination_dir: 'dist/site' };
const USAGE = 'Usage: npm run configure:pages -- <project> [--profile <profile>] [--dry-run | --deploy]';

function hasExpectedSettings(config) {
  return config?.build_command === SETTINGS.build_command &&
    ['', '/', '.'].includes(config.root_dir ?? '') &&
    config.destination_dir?.replace(/^\.?\//, '').replace(/\/$/, '') === SETTINGS.destination_dir;
}

export function configurePages(projectName, { dryRun, deploy, profile } = {}, runCf = invokeCf) {
  if (!/^[a-z0-9][a-z0-9-]*$/.test(projectName ?? '')) throw new Error(USAGE);
  if (dryRun && deploy) throw new Error('Use either --dry-run or --deploy, not both.');
  const cf = (args) => runCf(profile ? [...args, '--profile', profile] : args);
  const readProject = () => {
    const project = cf(['pages', 'get', projectName]);
    if (project?.name !== projectName) throw new Error('cf did not return the requested Pages project.');
    return project;
  };
  let project = readProject();
  if (!['github', 'gitlab'].includes(project.source?.type)) {
    throw new Error('This project has no Git integration. Connect a Git repository in Pages first.');
  }
  if (deploy && !project.production_branch) throw new Error('This project has no production branch configured.');
  const changed = !hasExpectedSettings(project.build_config);
  if (changed && !dryRun) {
    cf(['pages', 'edit', projectName, '--body', JSON.stringify({ build_config: SETTINGS })]);
    project = readProject();
    if (!hasExpectedSettings(project.build_config)) throw new Error('Pages did not retain the requested build settings.');
  }
  let deploymentId;
  if (deploy) {
    const branch = project.production_branch;
    if (!branch) throw new Error('This project has no production branch configured.');
    const deployment = cf(['pages', 'deployments', 'create', projectName, '--branch', branch]);
    deploymentId = deployment?.id;
    if (!deploymentId) throw new Error('No deployment ID returned. Check Pages before retrying.');
  }
  return { changed, deploymentId, settings: { ...SETTINGS } };
}

export function invokeCf(args) {
  const npmArgs = ['exec', '--yes', `--package=${CF_PACKAGE}`, '--', 'cf', ...args];
  const npmCli = process.env.npm_execpath;
  if (!npmCli && process.platform === 'win32') throw new Error('On Windows, run this command through npm run.');
  const result = npmCli
    ? spawnSync(process.execPath, [npmCli, ...npmArgs], { encoding: 'utf8' })
    : spawnSync('npm', npmArgs, { encoding: 'utf8' });
  if (result.error || result.status !== 0) {
    throw new Error(`cf ${args.slice(0, 2).join(' ')} failed. Verify the project, CLI login and account (or run cf directly for details).`);
  }
  if (args[1] === 'edit') return;
  try {
    return JSON.parse(result.stdout);
  } catch {
    throw new Error(`cf ${args.slice(0, 2).join(' ')} did not return JSON. Check Pages before retrying.`);
  }
}

function main() {
  const { values, positionals } = parseArgs({ allowPositionals: true, options: {
    help: { type: 'boolean', short: 'h' }, 'dry-run': { type: 'boolean' }, deploy: { type: 'boolean' }, profile: { type: 'string' },
  } });
  if (values.help) { console.log(USAGE); return; }
  if (positionals.length !== 1) throw new Error(USAGE);
  const [project] = positionals;
  const result = configurePages(project, { dryRun: values['dry-run'], deploy: values.deploy, profile: values.profile });
  console.log(`${project}: ${!result.changed ? 'already configured' : values['dry-run'] ? 'would update' : 'updated'}`);
  console.log(`Build command: ${result.settings.build_command}\nRoot directory: repository root\nOutput directory: ${result.settings.destination_dir}`);
  if (result.deploymentId) console.log(`Production deployment requested: ${result.deploymentId}`);
}

if (process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href) {
  try { main(); }
  catch (error) { console.error(error.message); process.exitCode = 1; }
}
