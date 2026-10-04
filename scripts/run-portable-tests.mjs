/**
 * Runs the portable test suite: `test/*.test.mjs`.
 *
 * These are the tests that run in a fresh clone. They need no `.ref/` checkout, no
 * `.test/` workbench, no ~13 GB of game data, no engine build and no network — only
 * Node. That is the whole point of the split: `scripts/run-tests.mjs` runs the
 * local workbench in the gitignored `.test/`, which CI can never have, and this one
 * runs what CI must always be able to run. `npm test` keeps its old meaning; CI runs
 * this one, through `pnpm run test:portable`.
 *
 * The directory is discovered rather than listed, and the file names are sorted, so a
 * test dropped into `test/` is picked up by this runner and by CI without anyone
 * editing either file. The sort is deliberate: `node --test` runs a directory in
 * whatever order the filesystem hands it, and a stable order makes two runs of the
 * same commit report the same sequence.
 *
 * Like its `.test/` sibling, this wrapper exists because `node --test` on a glob that
 * matches nothing exits **0**. A clone whose tests were all deleted would print a
 * clean pass having run zero tests, so the empty case is reported for what it is and
 * exits non-zero.
 */
import { spawnSync } from 'node:child_process';
import { existsSync, readdirSync } from 'node:fs';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..');

/**
 * Where the portable tests live.
 *
 * `test/` is the suite; `ports/artemis/compat-web/tests/` holds the one test that belongs to the
 * port it tests, because it compiles that port's overlay sources. It is portable in the same
 * sense as the rest -- no `.ref/`, no build, no network -- so it runs here, with the same
 * discovery and the same empty-suite failure. Listing the directory rather than the file keeps
 * the rule the header states: a test dropped in is picked up without editing this runner.
 */
const SUITES = ['test', 'ports/artemis/compat-web/tests'];

const files = [];
for (const suite of SUITES) {
  const dir = resolve(ROOT, suite);
  if (!existsSync(dir)) continue;
  for (const name of readdirSync(dir).filter((entry) => entry.endsWith('.test.mjs')).sort()) {
    files.push(resolve(dir, name));
  }
}

if (files.length === 0) {
  console.error(
    `No portable tests found in ${SUITES.map((suite) => `\`${suite}/\``).join(' or ')}: there is no\n` +
      '`*.test.mjs` to run. This is not a pass — nothing was executed. The portable suite is what\n' +
      'a fresh clone can run, so an empty one means the checkout is incomplete or the tests were\n' +
      'moved, not that the code is green.',
  );
  process.exit(1);
}

// Default concurrency, one process, and the runner's own reporter: the point is a real
// `node --test` run over real files, not a re-implementation of it.
const run = spawnSync(
  process.execPath,
  ['--test', ...files],
  { cwd: ROOT, stdio: 'inherit' },
);

// A non-zero status is propagated exactly as-is (including the 1 that a failing test
// reports), and a signal death is a failure rather than a silent pass.
if (run.error) {
  console.error(`could not run the portable suite: ${run.error.message}`);
  process.exit(1);
}
if (run.signal) {
  console.error(`the portable suite was killed by ${run.signal}`);
  process.exit(1);
}
process.exit(run.status ?? 1);
