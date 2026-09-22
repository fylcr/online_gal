/**
 * Runs the package test suite.
 *
 * The suite lives in `.test/`, which is gitignored on purpose: it is a local workbench
 * that depends on `.ref/` checkouts and ~13 GB of real game data, neither of which can be
 * redistributed (see `README.md`, "测试"). A fresh clone therefore has no tests at all.
 *
 * That is the reason this wrapper exists. `node --test ".test/*.test.ts"` exits **0**
 * when the glob matches nothing, so a clone without the workbench would print a clean
 * pass having run zero tests — a false green that would be worse than a failure. Here the
 * absence is reported for what it is and exits non-zero.
 */
import { spawnSync } from 'node:child_process';
import { existsSync, readdirSync } from 'node:fs';

const SUITE = '.test';

const hasSuite =
  existsSync(SUITE) && readdirSync(SUITE).some((name) => name.endsWith('.test.ts'));

if (!hasSuite) {
  console.error(
    `No test suite found: \`${SUITE}/\` is a gitignored local workbench and is not part of this\n` +
      'repository, so this clone has no tests to run. This is not a pass — nothing was\n' +
      'executed. See README.md, section "测试", for what the suite needs and how to restore it.',
  );
  process.exit(1);
}

const run = spawnSync(
  process.execPath,
  ['--test', `${SUITE}/*.test.ts`],
  { stdio: 'inherit' },
);

process.exit(run.status ?? 1);
