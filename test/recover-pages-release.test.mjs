import assert from 'node:assert/strict';
import test from 'node:test';
import { recoverPagesRelease } from '../scripts/recover-pages-release.mjs';

const accountId = '1'.repeat(32);
const project = 'fixture';
const host = 'fixture.example.org';
const origin = 'https://' + host;
const branch = 'main';
const trust = { host, salt: 'A'.repeat(43), hash: 'b'.repeat(64) };
const bootstrap = '11111111-1111-4111-8111-111111111111';
const signing = '22222222-2222-4222-8222-222222222222';
const other = '33333333-3333-4333-8333-333333333333';
const options = { accountId, host };
const deploymentUrl = id => 'https://' + id.slice(0, 8) + '.fixture.pages.dev';

function fixture({ marker = false, status = 'success', current = signing, changeAfterVerify = false, duplicate = false } = {}) {
  const production = {
    ONLINE_GAL_SITE: { type: 'plain_text', value: JSON.stringify({ v: 1, account: accountId, project, productionBranch: branch, at: '2026-01-01T00:00:00Z', trust }) },
    ONLINE_GAL_CA_BUILD_TOKEN: { type: 'secret_text', value: 'opaque fixture secret' },
    ...(marker ? { ONLINE_GAL_CA_BOOTSTRAP: { type: 'plain_text', value: '1' } } : {}),
  };
  const deployment = (id, day, status = 'success') => ({ id, url: deploymentUrl(id), created_on: '2026-01-0' + day + 'T00:00:00Z',
    latest_stage: { name: status === 'success' ? 'deploy' : 'build', status }, deployment_trigger: { metadata: { branch } } });
  const commands = [];
  const requests = [];
  const runCf = (args, security) => {
    commands.push(args);
    assert.deepEqual(security, { accountId, oauthOnly: true });
    if (args[1] === 'get') return { name: project, production_branch: branch, domains: [host], source: { type: 'github' },
      deployment_configs: { production: { env_vars: production } } };
    assert.deepEqual(args.slice(0, 3), ['pages', 'deployments', 'list']);
    return [deployment(bootstrap, 2), ...(!marker ? [deployment(signing, 3, status)] : []), ...(duplicate ? [deployment(other, 4)] : [])];
  };
  const headers = { 'cache-control': 'no-store' };
  const fetcher = async (request, init) => {
    const url = new URL(request);
    requests.push(url.origin + url.pathname);
    assert.equal(init.credentials, 'omit');
    assert.equal(init.redirect, 'error');
    if (url.pathname === '/__ca/issue') {
      assert.equal(init.method, 'POST');
      return Response.json({ error: 'unauthorized' }, { status: 401, headers });
    }
    const id = url.origin === origin ? current : (url.origin === deploymentUrl(bootstrap) ? bootstrap : signing);
    return Response.json({ url: deploymentUrl(id), branch }, { headers });
  };
  const verified = [];
  const verify = async (candidate, immutableOrigin) => {
    verified.push(immutableOrigin);
    assert.deepEqual(candidate, trust);
    if (changeAfterVerify) current = other;
    return [{ id: 'online-gal.manager', version: '1.0.0', digest: 'c'.repeat(64) }];
  };
  return { runCf, fetcher, verify, commands, requests, verified };
}

test('an existing successful bootstrap is verified without mutating the marker', async () => {
  const f = fixture({ marker: true, current: bootstrap });
  assert.deepEqual(await recoverPagesRelease(project, options, f), { stage: 'bootstrap-ready', branch, bootstrapDeploymentId: bootstrap });
  assert.equal(f.commands.length, 2);
  assert.equal(f.verified.length, 0);
  assert.equal(f.requests.filter(x => x.endsWith('/__ca/issue')).length, 1);
});

test('a signing deployment in progress is reused by ID', async () => {
  const f = fixture({ status: 'active' });
  assert.deepEqual(await recoverPagesRelease(project, options, f), { stage: 'signing-running', branch, bootstrapDeploymentId: bootstrap, signingDeploymentId: signing });
  assert.equal(f.requests.length, 0);
});

test('the signed deployment is verified at its own origin and checked again after verification', async () => {
  const f = fixture();
  const result = await recoverPagesRelease(project, options, f);
  assert.equal(result.stage, 'awaiting-dns');
  assert.equal(result.signingDeploymentId, signing);
  assert.deepEqual(f.verified, [deploymentUrl(signing)]);
  assert.equal(f.requests.length, 4);
  assert.ok(!JSON.stringify(result).includes('opaque fixture secret'));
});

test('a production host switch or ambiguous deployment history stops recovery', async () => {
  const wrong = fixture({ current: other });
  await assert.rejects(recoverPagesRelease(project, options, wrong), /different deployment/);
  assert.equal(wrong.verified.length, 0);
  await assert.rejects(recoverPagesRelease(project, options, fixture({ changeAfterVerify: true })), /different deployment/);
  await assert.rejects(recoverPagesRelease(project, options, fixture({ duplicate: true })), /ambiguous/);
});
