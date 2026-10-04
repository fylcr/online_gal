/**
 * The release policy of `.github/workflows/build-packages.yml`, pinned as text.
 *
 * The policy is one sentence: **only a dispatched run that explicitly asks for `publish: true`
 * releases anything; a push or a PR builds, tests, packs, smoke-installs and uploads the archive,
 * and stops there.** It is written this way because a release asserts the bytes were accepted, the
 * acceptance oracle is a real game booting them, and no runner can run one.
 *
 * Read as text on purpose. This file has no dependencies, like every other test in `test/`, so it
 * runs in a fresh clone: an unquoted `if:` scalar very nearly became `false` once, and the check
 * that matters is the one that reads the actual characters rather than a parse node.
 */
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const WORKFLOW = '.github/workflows/build-packages.yml';
const source = readFileSync(join(ROOT, WORKFLOW), 'utf8');

/** Executable lines only: this workflow explains its own history in comments. */
const code = source.split(/\r?\n/).filter((line) => !/^\s*#/.test(line)).join('\n');

test('the publish step is the only step that can create a release, and a push is not a condition', () => {
  const step = code.match(/- name: Publish the aggregate package\n\s*if: (.+)\n/);
  assert.ok(step, `${WORKFLOW} must still have the publish step, with an \`if:\` on it`);
  const condition = step[1].trim();
  assert.equal(condition, `\${{ github.event_name == 'workflow_dispatch' && inputs.publish == true }}`,
    'the release condition is the dispatch plus an explicit request, and nothing else');
  // The old form released on every push to `main`, which is the policy this replaced.
  assert.doesNotMatch(condition, /push/, 'a push must never be a condition of the release step');
  // One release path only: the tag lives in this step, and no other step may name a release.
  assert.ok(!/- name: [^\n]*[Rr]elease/.test(code.replace(step[0], '')),
    'no step other than the publish step may mention a release');
  assert.equal((code.match(/\bgh release\b/g) ?? []).length, 3,
    'the three `gh release` calls are all inside the publish step: view, create, upload');
  // The dispatch input has to stay a `workflow_dispatch` input -- a `pull_request` input does not
  // exist, and moving it would leave the condition above permanently false.
  const dispatch = source.match(/\n {2}workflow_dispatch:\n([\s\S]*?)\n {2}\w/);
  assert.ok(dispatch, 'workflow_dispatch must still declare its inputs');
  assert.match(dispatch[1], /^ {6}publish:\n(?: {8}[^\n]*\n)*? {8}default: false\n/m,
    'the dispatch input must default to false: "run this workflow" must not imply "publish"');
});

test('what a push and a PR still do: build, pack, smoke-install and upload the artifact', () => {
  // The release moved, not the pipeline. If any of these ever stops running on `push`, the
  // "nothing publishes" claim above becomes "nothing happens", which is a different change.
  assert.match(source, /^\non:\n {2}push:\n {4}branches: \[main\]/m, 'push: to main must still trigger');
  assert.match(source, /\n {2}pull_request:\n/m, 'pull_request must still trigger');
  for (const step of ['Build the packages', 'Run the portable test suite', 'Pack the packages',
    'Smoke-test the aggregate as an installed package']) {
    assert.ok(code.includes(`- name: ${step}`), `the ${step} step is what a push still runs`);
  }
  const upload = code.match(/- name: Upload package\n\s*uses: actions\/upload-artifact@v4\n[\s\S]*?retention-days: \d+/);
  assert.ok(upload, 'the packages artifact must still be uploaded, unconditionally');
  assert.doesNotMatch(upload[0], /\n\s*if:/,
    'the artifact upload must not be gated: it is where the un-published bytes stay inspectable');
  assert.ok(code.includes('package-files: ${{ steps.package.outputs.files }}'),
    'the job outputs survive so a release can name the file without re-deriving it');
});

test('no push-reachable step publishes an npm package', () => {
  // This repository is private and publishes no npm package; `pnpm publish`/`npm publish` in the
  // workflow would be a second release path nobody would look for.
  assert.doesNotMatch(code, /\b(?:npm|pnpm) publish\b/,
    'no step may publish to a registry -- the release is a GitHub Release or nothing');
  assert.doesNotMatch(code, /NODE_AUTH_TOKEN|NPM_TOKEN|registry-url/,
    'a publish token in this workflow is a second release path by construction');
});
