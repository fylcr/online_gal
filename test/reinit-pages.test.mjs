import assert from 'node:assert/strict';
import test from 'node:test';
import { reinitPages } from '../scripts/reinit-pages.mjs';

const name = 'test-project';
const branch = 'release';
const marker = { type: 'plain_text', value: '{"v":1}' };

function stub({ retain = false, source = { type: 'github' }, production_branch = branch } = {}) {
  const project = { name, source, production_branch, deployment_configs: {
    production: { env_vars: { ONLINE_GAL_SITE: marker, OTHER_SECRET: { type: 'secret_text', value: 'test-only-secret' } } },
    preview: { env_vars: { ONLINE_GAL_SITE: marker, UNRELATED: { type: 'plain_text', value: 'keep' } } },
  } };
  const calls = [];
  const runCf = (args) => {
    calls.push(args);
    if (args[1] === 'get') return structuredClone(project);
    if (args[1] === 'edit') {
      assert.equal(args[3], '--body');
      const { deployment_configs } = JSON.parse(args[4]);
      assert.deepEqual(deployment_configs, {
        production: { env_vars: { ONLINE_GAL_SITE: null } },
        preview: { env_vars: { ONLINE_GAL_SITE: null } },
      });
      delete project.deployment_configs.production.env_vars.ONLINE_GAL_SITE;
      if (!retain) delete project.deployment_configs.preview.env_vars.ONLINE_GAL_SITE;
      return;
    }
    assert.deepEqual(args, ['pages', 'deployments', 'create', name, '--branch', branch]);
    return { id: 'test-deployment' };
  };
  return { calls, runCf, project };
}

test('reinit dry run only reads the project and never exposes the settings', () => {
  const { runCf, calls } = stub();
  assert.deepEqual(reinitPages(name, { dryRun: true }, runCf), { branch });
  assert.deepEqual(calls, [['pages', 'get', name]]);
});

test('reinit clears only both markers, rereads settings, and triggers the production branch', () => {
  const { runCf, calls, project } = stub();
  assert.deepEqual(reinitPages(name, {}, runCf), { branch, deploymentId: 'test-deployment' });
  assert.deepEqual(calls.map((args) => args[1]), ['get', 'edit', 'get', 'deployments']);
  assert.deepEqual(Object.keys(project.deployment_configs.production.env_vars), ['OTHER_SECRET']);
  assert.deepEqual(Object.keys(project.deployment_configs.preview.env_vars), ['UNRELATED']);
});

test('reinit does not deploy while either environment still reports a marker', () => {
  const { runCf, calls } = stub({ retain: true });
  assert.throws(() => reinitPages(name, {}, runCf), /No production deployment was requested/);
  assert.deepEqual(calls.map((args) => args[1]), ['get', 'edit', 'get']);
});

test('reinit refuses direct upload projects and projects without a production branch', () => {
  for (const options of [{ source: null }, { production_branch: '' }]) {
    const { runCf, calls } = stub(options);
    assert.throws(() => reinitPages(name, {}, runCf), /no Git integration|no production branch/);
    assert.deepEqual(calls, [['pages', 'get', name]]);
  }
});
