#!/usr/bin/env node
/**
 * CI entry for the ONS/KRKR immutable release.
 *
 * `preflight` reads the build clock once and writes the selected version to
 * GITHUB_OUTPUT. `publish` checks that same identity again, creates the release,
 * then uploads without --clobber. An upload failure exits non-zero and leaves the
 * created release in place. Neither command deletes a tag or release.
 */
import { appendFileSync, readFileSync } from 'node:fs';
import { execFileSync } from 'node:child_process';
import { isSemVer, planPublication, releaseIdentity } from './runtime-release-policy.mjs';
import { lookup } from './runtime-release-inventory.mjs';

function output(lines) {
  const path = process.env.GITHUB_OUTPUT;
  if (!path) return;
  appendFileSync(path, `${lines.join('\n')}\n`);
}

export function preflightFromEnv(env = process.env, deps = {}) {
  const selectedEngine = env.RELEASE_ENGINE;
  if (selectedEngine !== 'ons' && selectedEngine !== 'krkr') {
    throw Object.assign(new Error('RELEASE_ENGINE must be ons or krkr'), { code: 2 });
  }
  const event = env.EVENT_NAME;
  const clock = deps.clock ? new Date(deps.clock) : new Date();
  const mode = event === 'push' ? 'push' : event === 'workflow_dispatch' ? 'manual' : event;
  const publish = env.PUBLISH === 'true';
  // A missing, blank or invalid manual version, or an unselected publish, is rejected
  // without any API call: only an explicit publish of a complete SemVer is worth an
  // exact-identity lookup.
  let inventory = { tag: [], release: [], asset: [], history: [] };
  if (mode === 'manual' && publish === true && isSemVer(env.MANUAL_VERSION)) {
    inventory = (deps.lookup ?? lookup)(env.REPOSITORY, selectedEngine, env.MANUAL_VERSION);
  }
  const result = planPublication({
    engine: selectedEngine,
    mode,
    publish,
    version: env.MANUAL_VERSION,
    clock,
    before: env.RANGE_BEFORE,
    after: env.RANGE_AFTER,
    repo: env.GITHUB_WORKSPACE || process.cwd(),
    inventory,
    execFile: deps.execFile,
  });
  if (result.action === 'build') {
    const exact = (deps.lookup ?? lookup)(env.REPOSITORY, selectedEngine, result.version);
    const again = planPublication({
      engine: selectedEngine,
      mode: 'manual',
      publish: true,
      version: result.version,
      repo: env.GITHUB_WORKSPACE || process.cwd(),
      inventory: exact,
    });
    if (again.action !== 'build') return { ...again, clock: clock.toISOString() };
  }
  return { ...result, clock: clock.toISOString() };
}

function writePreflight(result) {
  output([
    `action=${result.action}`,
    `build=${result.build}`,
    `release=${result.release}`,
    `version=${result.version ?? ''}`,
    `tag=${result.identity?.tag ?? ''}`,
    `clock=${result.clock ?? ''}`,
  ]);
  process.stdout.write(`${JSON.stringify(result)}\n`);
  if (result.action === 'reject') process.exit(2);
}

export function publishCreated(env = process.env, deps = {}) {
  const engine = env.RELEASE_ENGINE;
  const version = env.SELECTED_VERSION;
  const tag = env.SELECTED_TAG;
  const identity = releaseIdentity(engine, version);
  if (identity.tag !== tag) throw Object.assign(new Error('selected tag does not match the version'), { code: 2 });
  const inventory = (deps.lookup ?? lookup)(env.REPOSITORY, engine, version);
  const check = planPublication({
    engine, mode: 'manual', publish: true, version, repo: env.GITHUB_WORKSPACE || process.cwd(), inventory,
  });
  if (check.action !== 'build') {
    const error = new Error(check.reason || 'second existence check failed');
    error.code = 2;
    throw error;
  }
  const exec = deps.execFile ?? execFileSync;
  const notes = [
    `Built from ${env.UPSTREAM_REPOSITORY} @ ${env.UPSTREAM_COMMIT}.`,
    `Runtime SemVer: ${version}.`,
    `Provenance upstream is not the runtime version.`,
    `Run ${env.RUN_ID}: ${env.SERVER_URL}/${env.REPOSITORY}/actions/runs/${env.RUN_ID}`,
  ].join(' ');
  exec('gh', ['release', 'create', tag, '--title', `wasm: ${engine} ${version}`, '--notes', notes, '--target', env.TARGET_SHA], { stdio: 'pipe' });
  const files = (env.ARTIFACT_FILES || '').split(/\s+/).filter(Boolean).map((name) => `${env.ARTIFACT_DIR}/${name}`);
  files.push('manifest.json');
  if (env.PACKAGE_DIR) {
    const packed = exec('node', ['-e', 'process.stdout.write(require("node:fs").readdirSync("dist/packages").filter(n=>n.endsWith(".tgz")).join("\\n"))'], { encoding: 'utf8' });
    for (const name of packed.split('\n').filter(Boolean)) files.push(`dist/packages/${name}`);
  }
  const uploaded = [];
  try {
    for (const file of files) {
      exec('gh', ['release', 'upload', tag, file], { stdio: 'pipe' });
      uploaded.push(file);
    }
  } catch (cause) {
    const error = new Error(`upload failed after create; kept ${tag} and ${uploaded.length} uploaded asset(s): ${cause.message}`);
    error.code = cause.status || 1;
    throw error;
  }
  return { tag, uploaded };
}

function main() {
  const command = process.argv[2];
  try {
    if (command === 'preflight') writePreflight(preflightFromEnv());
    else if (command === 'publish') {
      const result = publishCreated();
      process.stdout.write(`published ${result.uploaded.length} file(s) to ${result.tag}\n`);
    } else {
      process.stderr.write('usage: runtime-release-gate.mjs <preflight|publish>\n');
      process.exit(1);
    }
  } catch (cause) {
    process.stderr.write(`runtime-release-gate: ${cause.message}\n`);
    process.exit(cause.code || 1);
  }
}

if (process.argv[1]?.endsWith('runtime-release-gate.mjs')) main();
