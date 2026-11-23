import assert from 'node:assert/strict';
import { readFileSync, existsSync, mkdtempSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import test from 'node:test';
import { rotateCfEntry } from '../scripts/rotate-cf-entry.mjs';
import { deriveCfRouteConfig, cfRouteOwnerBinding } from '../functions/__cf/route-config.mjs';

const accountId = '1'.repeat(32);
const ownerId = '2'.repeat(32);
const projectName = 'fixture';
const host = 'fixture.example.org';
const branch = 'main';
const id = 'aabbccdd-0000-4000-8000-001122334455';
const ownerBinding = await cfRouteOwnerBinding(accountId, projectName, ownerId);
const record = { v: 1, account: accountId, project: projectName, by: ownerId, at: '2026-01-01T00:00:00Z',
  productionBranch: branch, trust: { host, salt: 'fixture-salt', hash: '3'.repeat(64) } };
const text = value => ({ type: 'plain_text', value: JSON.stringify(value) });
function fixture({ bound = false, user = ownerId, failEdit = false } = {}) {
  const original = bound ? { ...record, by: '', cfRoute: { v: 1, host, ownerBinding } } : record;
  const production = { ONLINE_GAL_SITE: text(original), ONLINE_GAL_CA_BUILD_TOKEN: { type: 'secret_text' } };
  const preview = { ONLINE_GAL_SITE: text({ ...record, by: '' }) };
  if (bound) {
    production.ONLINE_GAL_CF_OWNER_ID = { type: 'secret_text' };
    production.ONLINE_GAL_CF_ROUTE_KEY = { type: 'secret_text' };
  }
  const patches = [];
  const commands = [];
  const runCf = (args, options) => {
    assert.deepEqual(options, { accountId, oauthOnly: true });
    assert.deepEqual(args.slice(-2), ['--profile', 'isolated']);
    const action = args.slice(0, 2).join(' ');
    commands.push(action);
    if (action === 'pages get') return { name: projectName, domains: [host], production_branch: branch,
      source: { type: 'github' }, deployment_configs: { production: { env_vars: production }, preview: { env_vars: preview } } };
    if (action === 'user get') return { id: user };
    if (action === 'pages edit') {
      assert.equal(args[3], '--body');
      assert.ok(args[4].startsWith('@'));
      const file = args[4].slice(1);
      const patch = JSON.parse(readFileSync(file, 'utf8'));
      patches.push({ patch, file });
      if (failEdit) throw new Error('opaque cf failure');
      const vars = structuredClone(patch.deployment_configs.production.env_vars);
      delete vars.ONLINE_GAL_CF_ROUTE_KEY.value;
      if (vars.ONLINE_GAL_CF_OWNER_ID) delete vars.ONLINE_GAL_CF_OWNER_ID.value;
      Object.assign(production, vars);
      return;
    }
    assert.deepEqual(args.slice(0, -2), ['pages', 'deployments', 'create', projectName, '--branch', branch]);
    return { id };
  };
  return { runCf, patches, commands, original };
}
const options = { accountId, host, profile: 'isolated' };
function isolatedOutput(t) {
  const folder = mkdtempSync(join(tmpdir(), 'online-gal-rotate-test-'));
  t.after(() => rmSync(folder, { recursive: true, force: true }));
  return join(folder, 'entry.txt');
}

for (const bound of [false, true]) {
  test(bound ? 'rotation preserves the bound owner and release record' : 'legacy migration binds only the original owner', async t => {
    const f = fixture({ bound });
    const entryFile = isolatedOutput(t);
    const result = await rotateCfEntry(projectName, { ...options, apply: true, entryFile }, f.runCf);
    assert.deepEqual(result, { action: bound ? 'rotate' : 'bind', branch, deploymentId: id });
    const [{ file, patch }] = f.patches;
    assert.ok(!existsSync(file));
    assert.deepEqual(Object.keys(patch.deployment_configs), ['production']);
    const vars = patch.deployment_configs.production.env_vars;
    assert.deepEqual(Object.keys(vars).sort(), bound ? ['ONLINE_GAL_CF_ROUTE_KEY'] :
      ['ONLINE_GAL_CF_OWNER_ID', 'ONLINE_GAL_CF_ROUTE_KEY', 'ONLINE_GAL_SITE']);
    if (!bound) {
      assert.deepEqual(vars.ONLINE_GAL_CF_OWNER_ID, { type: 'secret_text', value: ownerId });
      assert.deepEqual(JSON.parse(vars.ONLINE_GAL_SITE.value), { ...record, by: '', cfRoute: { v: 1, host, ownerBinding } });
    }
    const route = await deriveCfRouteConfig({ accountId, projectName, host, routeKey: vars.ONLINE_GAL_CF_ROUTE_KEY.value });
    assert.ok(readFileSync(entryFile, 'utf8') === 'https://' + host + '/admin#cf-entry=' + route.segment + '\n');
    assert.equal(f.commands.at(-1), 'pages deployments');
  });
}

test('read-only mode does not mutate and a different account user is refused', async () => {
  for (const bound of [false, true]) {
    const authorized = fixture({ bound });
    assert.deepEqual(await rotateCfEntry(projectName, options, authorized.runCf), { action: bound ? 'rotate' : 'bind', branch });
    assert.deepEqual(authorized.commands, ['pages get', 'user get']);
    const unauthorized = fixture({ bound, user: '4'.repeat(32) });
    await assert.rejects(rotateCfEntry(projectName, options, unauthorized.runCf), /cannot be matched/);
    assert.deepEqual(unauthorized.commands, ['pages get', 'user get']);
  }
});

test('an ambiguous write leaves a recoverable entry and requests no deployment', async t => {
  const f = fixture({ bound: true, failEdit: true });
  const entryFile = isolatedOutput(t);
  await assert.rejects(rotateCfEntry(projectName, { ...options, apply: true, entryFile }, f.runCf), /entry file was kept/);
  assert.ok(existsSync(entryFile));
  assert.ok(!existsSync(f.patches[0].file));
  assert.ok(!f.commands.includes('pages deployments'));
});
