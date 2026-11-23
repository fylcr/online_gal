// Builds only the disposable preview used to verify the Pages branch alias.
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { readFileSync, writeFileSync } from 'node:fs';

const previewBranch = 'test/task151-private-relay-preview-20261123';
const productionBranch = 'test/task151-private-relay-20261123';
const project = 'online-gal-task106-probe';
assert.equal(process.env.CF_PAGES, '1');
assert.equal(process.env.CF_PAGES_BRANCH, previewBranch);
const deployment = new URL(process.env.CF_PAGES_URL);
assert.ok(deployment.hostname.endsWith(`.${project}.pages.dev`));
for (const name of ['ONLINE_GAL_CF_OWNER_ID', 'ONLINE_GAL_CF_ROUTE_KEY', 'ONLINE_GAL_CA_PRIVATE_JWK', 'ONLINE_GAL_CA_BUILD_TOKEN']) {
  assert.ok(!process.env[name], `Unexpected preview binding: ${name}`);
}
const env = { ...process.env, ONLINE_GAL_SITE: JSON.stringify({ v: 1, project, productionBranch }) };
const build = spawnSync('npm', ['run', 'build:pages'], { env, stdio: 'inherit' });
assert.equal(build.status, 0, 'preview build failed');
const routes = JSON.parse(readFileSync('dist/site/_routes.json', 'utf8'));
assert.ok(!routes.include.some(path => path.startsWith('/__cf')));
writeFileSync('dist/site/task151-preview.json', JSON.stringify({ branch: previewBranch, deployment: deployment.origin }));
