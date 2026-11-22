import assert from 'node:assert/strict';
import test from 'node:test';
import { configurePages } from '../scripts/configure-pages.mjs';

const projectName = 'test-project';
const correct = { build_command: 'npm run build:pages', root_dir: '', destination_dir: 'dist/site' };

function stub(overrides) {
  const project = { name: projectName, source: { type: 'github' }, production_branch: 'production', ...overrides };
  const calls = [];
  const cli = (args) => {
    calls.push(args);
    switch (args[1]) {
      case 'get': return structuredClone(project);
      case 'edit':
        assert.deepEqual(JSON.parse(args[4]), { build_config: correct });
        project.build_config = { ...project.build_config, ...correct };
        return;
      case 'deployments': return { id: 'new-deployment' };
      default: assert.fail('unexpected cf command');
    }
  };
  return { project, cli, calls };
}

const operations = (calls) => calls.map((args) => args[1]);

test('writes only Pages build settings and rereads the named project without deploying', () => {
  const { project, cli, calls } = stub({ build_config: { build_command: '', root_dir: '/apps/app', build_caching: true } });
  assert.deepEqual(configurePages(projectName, {}, cli), { changed: true, deploymentId: undefined, settings: correct });
  assert.deepEqual(operations(calls), ['get', 'edit', 'get']);
  assert.equal(project.build_config.build_caching, true);
});

test('deployment is explicit and uses the existing production branch and profile', () => {
  const { cli, calls } = stub({ build_config: { ...correct, root_dir: '/', destination_dir: '/dist/site/' } });
  const result = configurePages(projectName, { deploy: true, profile: 'operator' }, cli);
  assert.equal(result.changed, false);
  assert.equal(result.deploymentId, 'new-deployment');
  assert.deepEqual(calls, [
    ['pages', 'get', projectName, '--profile', 'operator'],
    ['pages', 'deployments', 'create', projectName, '--branch', 'production', '--profile', 'operator'],
  ]);
});

test('dry run and correctly configured projects only read the project', () => {
  for (const [build_config, dryRun, changed] of [[{}, true, true], [correct, false, false]]) {
    const { cli, calls } = stub({ build_config });
    assert.equal(configurePages(projectName, { dryRun }, cli).changed, changed);
    assert.deepEqual(operations(calls), ['get']);
  }
});

test('rejects non-Git projects, incorrect targets, or a missing deployment branch', () => {
  for (const [overrides, options, error] of [
    [{ source: null }, {}, /no Git integration/],
    [{ name: 'other' }, {}, /requested Pages project/],
    [{ production_branch: '' }, { deploy: true }, /no production branch/],
  ]) {
    const { cli, calls } = stub(overrides);
    assert.throws(() => configurePages(projectName, options, cli), error);
    assert.deepEqual(operations(calls), ['get']);
  }
  const { cli, calls } = stub();
  assert.throws(() => configurePages(projectName, { deploy: true, dryRun: true }, cli), /not both/);
  assert.deepEqual(calls, []);
});

test('does not deploy if Pages fails to retain the requested settings', () => {
  const calls = [];
  assert.throws(() => configurePages(projectName, { deploy: true }, (args) => {
    calls.push(args);
    return { name: projectName, source: { type: 'gitlab' }, production_branch: 'main' };
  }), /did not retain/);
  assert.deepEqual(operations(calls), ['get', 'edit', 'get']);
});
