/**
 * Checks out the pinned cf CLI source into `.ref/cf` (openspec/changes/cf-account-admin, task 2.2).
 *
 *   node packages/cf-browser/scripts/fetch-upstream.mjs [--repo <url-or-path>]
 *
 * `.ref/` is gitignored reference material, so a fresh clone or CI runner has no checkout and
 * must fetch exactly the commit in `upstream.json`. The checkout is sparse: the CLI's `src/`
 * and `package.json`, and the two root license files the bundle ships with. Tests are left out.
 * `--repo` points at another clone of the same history (a local mirror), never another project:
 * the commit check below still decides what is built.
 */
import { execFileSync } from 'node:child_process';
import { existsSync, readFileSync } from 'node:fs';
import { dirname, join, relative, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

const pkgRoot = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const repoRoot = resolve(pkgRoot, '../..');
const pin = JSON.parse(readFileSync(join(pkgRoot, 'upstream.json'), 'utf8'));
export const checkout = join(repoRoot, '.ref', 'cf');

const SPARSE = [
  `/${pin.path}/src/`,
  `!/${pin.path}/src/**/__tests__/`,
  `/${pin.path}/package.json`,
  '/LICENSE-MIT',
  '/LICENSE-APACHE',
];

const git = (args, cwd = checkout) =>
  execFileSync('git', args, { cwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'inherit'], env: { ...process.env, MSYS_NO_PATHCONV: '1' } }).trim();

/** The pinned commit, or an error naming what is checked out instead. */
export function assertPinned(dir = checkout) {
  if (!existsSync(join(dir, '.git'))) throw new Error(`${dir} is not a checkout; run node packages/cf-browser/scripts/fetch-upstream.mjs`);
  const head = git(['rev-parse', 'HEAD'], dir);
  if (head !== pin.commit) throw new Error(`${dir} is at ${head}, but upstream.json pins ${pin.commit}`);
  return join(dir, pin.path);
}

function main() {
  const at = process.argv.indexOf('--repo');
  const repo = at === -1 ? pin.repository : process.argv[at + 1];
  if (!repo) throw new Error('--repo needs a value');

  // Never write outside .ref/cf, whatever the path resolution does.
  const rel = relative(join(repoRoot, '.ref'), checkout);
  if (rel !== 'cf' || rel.startsWith('..') || rel.includes(sep)) throw new Error(`refusing to write to ${checkout}`);

  if (!existsSync(join(checkout, '.git'))) {
    console.log(`cloning ${repo} into ${checkout}`);
    git(['clone', '--filter=blob:none', '--no-checkout', repo, checkout], repoRoot);
  }
  git(['sparse-checkout', 'set', '--no-cone', ...SPARSE]);
  const present = (() => {
    try {
      git(['cat-file', '-e', `${pin.commit}^{commit}`]);
      return true;
    } catch {
      return false;
    }
  })();
  if (!present) git(['fetch', '--filter=blob:none', 'origin', pin.commit]);
  git(['checkout', '--detach', '--quiet', pin.commit]);
  assertPinned();
  console.log(`.ref/cf at ${pin.commit}`);
}

if (resolve(process.argv[1] ?? '') === fileURLToPath(import.meta.url)) {
  try {
    main();
  } catch (error) {
    console.error(error instanceof Error ? error.message : error);
    process.exit(1);
  }
}
