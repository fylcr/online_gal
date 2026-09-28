#!/usr/bin/env node
/**
 * Install a packed tarball OUTSIDE this repository and prove it is usable as a package.
 *
 * WHY THIS EXISTS
 *
 * `make-package.mjs` builds a tarball that has no `dependencies` and instead carries its whole
 * closure as real directories under `package/node_modules/`. That trick is only worth anything if
 * a consumer can actually install the result, and nothing in this repository checks that: the pack
 * step ends when the bytes are on disk, and every check since has been a statement about the
 * *inside* of the archive (a manifest field, a file list, a digest).
 *
 * The failure this catches is the one that only shows up on a user's machine. A wrong `exports`
 * entry, a `files` list that drops `src/`, a vendoring pass that stops one level short of the
 * closure, a `dist/` that exists in the workspace but not in the archive -- each of those produces
 * a tarball that looks perfect from the inside and throws `ERR_MODULE_NOT_FOUND` the moment it is
 * installed. Previously the first person to find out was whoever ran `pnpm add` on the release.
 *
 * WHY THE TEMPORARY DIRECTORY MUST NOT BE INSIDE THIS REPOSITORY
 *
 * Because pnpm treats a project directory below a workspace root as a workspace member. Installing
 * into, say, `.test/consumer-check/` does not touch that directory and nothing else: pnpm adds an
 * importer for it to the repository-root `pnpm-lock.yaml`. That was observed by hand during the
 * work that produced this script, and it is dangerous precisely because the damage is one directory
 * away from the thing being installed and shows up as a one-line diff in a file nobody looks at
 * before committing.
 *
 * So this script (a) builds the consumer under `RUNNER_TEMP` / `os.tmpdir()` and refuses to run
 * when that lands inside the repository, and (b) re-reads `git status --porcelain` after the
 * install and fails if the repository changed at all -- the root lockfile specifically, but the
 * whole worktree, because a workspace-aware install is not the only way to dirty a checkout.
 * Assertion (b) is itself the regression test: it is what would have caught the hand-run install,
 * and `--allow-in-repo-temp` exists only so that firing can be demonstrated on demand.
 *
 * WHAT IS ASSERTED
 *
 *   1. the tarball exists, is a non-empty file, and has a digest (an empty one is a failure, never
 *      a value: a criterion that prints a blank is worse than no criterion)
 *   2. the consumer's top level has exactly one `@online-gal` package: `engines`
 *   3. the installed manifest declares no `dependencies`
 *   4. `listEngines()`, `engineProviders()` and `ENGINE_PACKAGES` each report 8
 *   5. every vendored package (10 of them) resolves *and* imports, from inside the archive
 *   6. none of those 10 resolves from the consumer's top level -- the executable proof that the
 *      vendored tree is the only tree, and that dropping `dependencies` is not a latent break
 *   7. `runtime-artemis`'s `dist/host/{boot,eh,runtime,asyncify,fd,bridge}.js` deep-import
 *   8. the repository did not change during any of it
 *
 * WHY IT NEEDS NO NETWORK
 *
 * The tarball has no `dependencies`, so there is nothing to fetch. The install runs with
 * `--offline --ignore-scripts`; a registry request here would mean the archive grew dependencies,
 * which is the thing the packer is built to prevent. The consumer's `package.json` copies this
 * repository's own `packageManager` pin so the install uses the same pnpm the release was built
 * with -- the consumer directory is outside the repository and would otherwise fall back to
 * whatever pnpm happens to be the default on the machine. That pin is already materialised by the
 * job this gate runs inside, so resolving it is a cache hit, not a download.
 *
 * Exit codes: 0 every assertion held · 1 an assertion failed, the install failed, or the script
 * was misused.
 */
import { execFileSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import {
  existsSync,
  mkdirSync,
  readFileSync,
  readdirSync,
  realpathSync,
  rmSync,
  statSync,
  writeFileSync,
} from 'node:fs';
import { createRequire } from 'node:module';
import { tmpdir } from 'node:os';
import { basename, dirname, isAbsolute, join, relative, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const USAGE = `usage: node .github/scripts/smoke-consumer.mjs <tarball.tgz> [options]

  <tarball.tgz>   the packed aggregate to install, e.g.
                  dist/packages/online-gal-engines-0.1.3.tgz

  --keep                 leave the temporary consumer directory in place and print its path
  --temp-dir DIR         use DIR instead of a fresh temporary directory
  --allow-in-repo-temp   permit a consumer directory inside this repository. This exists so the
                         lockfile guard can be proven to fire; it is not a supported mode.
  --pnpm CMD             the pnpm to run (default: pnpm from PATH)

Exit codes: 0 every assertion held · 1 an assertion failed, the install failed, or the script
was misused.`;

const here = dirname(fileURLToPath(import.meta.url));
const repoRoot = resolve(here, '..', '..');

/** The values that make the aggregate what it is; a change to any of them is a deliberate act. */
const EXPECTED = {
  topLevelPackages: ['engines'],
  vendoredPackageCount: 10,
  listEngines: 8,
  engineProviders: 8,
  enginePackages: 8,
};
const ARTEMIS_HOST_MODULES = ['boot', 'eh', 'runtime', 'asyncify', 'fd', 'bridge'];

function fail(code, message) {
  process.stderr.write(`smoke-consumer: ${message}\n`);
  process.exit(code);
}

const options = { tarball: null, keep: false, tempDir: null, allowInRepoTemp: false, pnpm: 'pnpm' };
const argv = process.argv.slice(2);
for (let index = 0; index < argv.length; index += 1) {
  const arg = argv[index];
  const next = () => {
    const value = argv[index + 1];
    if (value === undefined) fail(1, `${arg} needs a value`);
    index += 1;
    return value;
  };
  switch (arg) {
    case '--help':
    case '-h': process.stdout.write(`${USAGE}\n`); process.exit(0); break;
    case '--keep': options.keep = true; break;
    case '--allow-in-repo-temp': options.allowInRepoTemp = true; break;
    case '--temp-dir': options.tempDir = next(); break;
    case '--pnpm': options.pnpm = next(); break;
    default:
      if (arg.startsWith('-')) fail(1, `unknown argument ${arg}\n\n${USAGE}`);
      if (options.tarball) fail(1, `expected one tarball path, got a second one: ${arg}`);
      options.tarball = arg;
  }
}
if (!options.tarball) fail(1, `missing <tarball.tgz>\n\n${USAGE}`);

/* ------------------------------------------------------------------ reporting */

const checks = [];
/** Blank is never a value: an empty string, empty list or undefined fails its own check. */
const blank = (value) =>
  value === undefined
  || value === null
  || (typeof value === 'string' && value.trim() === '')
  || (Array.isArray(value) && value.length === 0);

function show(value) {
  if (Array.isArray(value)) return value.length ? value.join(', ') : '(empty)';
  if (value === undefined) return '(undefined)';
  if (value === null) return '(null)';
  if (typeof value === 'object') {
    const keys = Object.keys(value);
    return keys.length ? `{ ${keys.length} exports }` : '{ no exports }';
  }
  return String(value);
}

function check(name, ok, actual, expected) {
  const holds = Boolean(ok) && !blank(actual);
  checks.push({ name, holds, actual: show(actual), expected: expected === undefined ? '' : show(expected) });
  // One assertion is one line. A value carrying newlines is rendered with ` | ` instead, because a
  // `::error::` annotation that ends mid-sentence reads as a truncated log rather than as a value --
  // and the value that most needs to be legible is a diff.
  const oneLine = (value) => show(value).replace(/\s*\n\s*/g, ' | ');
  if (holds) {
    process.stdout.write(`## ok    ${name}: ${oneLine(actual)}\n`);
  } else {
    const want = expected === undefined ? '' : `; expected ${oneLine(expected)}`;
    process.stdout.write(`::error::${name}: got ${oneLine(actual)}${want}\n`);
  }
  return holds;
}

/**
 * `body` returns the value to be judged; `summarise` renders it for the log, because a module
 * object is not printable and a template string over one throws instead of degrading. Whatever
 * `body` throws -- including a resolver answering with the wrong kind of value -- becomes a failed
 * check carrying the error code, never a silent pass.
 *
 * The result is `{ ok, value }` and not a bare boolean, because a caller that passed a path still
 * needs the path after the check has recorded it. Returning only the verdict loses it, and the
 * next statement then reads `true` as a path -- which fails loudly, but as a confusing error in a
 * file nobody is looking at, so the value travels with the verdict.
 */
async function attempt(name, expected, body, summarise = (value) => value) {
  try {
    const value = await body();
    if (value === false || value === undefined || value === null || value === '') {
      check(name, false, value === false ? 'not produced' : `blank (${show(value)})`, expected);
      return { ok: false, value: null };
    }
    check(name, true, summarise(value), expected);
    return { ok: true, value };
  } catch (error) {
    const detail = error && error.code ? `${error.code}: ` : '';
    check(name, false, `${detail}${error && error.message ? error.message : error}`, expected);
    return { ok: false, value: null };
  }
}

/* ------------------------------------------------------------------ identity */

const tarball = resolve(process.cwd(), options.tarball);
const tarballStat = existsSync(tarball) ? statSync(tarball) : null;
const sha256 = (() => {
  try {
    return createHash('sha256').update(readFileSync(tarball)).digest('hex');
  } catch {
    return '';
  }
})();

const consumerDir = options.tempDir
  ? resolve(process.cwd(), options.tempDir)
  : realpathSyncSafe(join(process.env.RUNNER_TEMP || tmpdir(), `smoke-consumer-${process.pid}`));

function realpathSyncSafe(path) {
  try {
    return realpathSync(path);
  } catch {
    return path;
  }
}

const packageManagerPin = (() => {
  try {
    return require_manifest(repoRoot).packageManager || '(none)';
  } catch {
    return '(unreadable)';
  }
})();
function require_manifest(dir) {
  return JSON.parse(readFileSync(join(dir, 'package.json'), 'utf8'));
}

process.stdout.write(`## smoke-consumer: tarball ${tarball}\n`);
process.stdout.write(`## smoke-consumer: ${tarballStat && tarballStat.isFile() ? `${tarballStat.size} B` : 'NOT A FILE'} sha256 ${sha256 || '(unavailable)'}\n`);
process.stdout.write(`## smoke-consumer: consumer ${consumerDir}\n`);
process.stdout.write(`## smoke-consumer: repo ${repoRoot} (packageManager ${packageManagerPin})\n`);

check('tarball exists and is a regular file', Boolean(tarballStat && tarballStat.isFile()),
  tarballStat && tarballStat.isFile() ? `${basename(tarball)}, ${tarballStat.size} B` : `not a file: ${tarball}`);
// The digest is reported as a value on both branches: a blank one is never printed as empty, because
// an error line that reads `got ; expected 64 hex characters` tells a reader nothing about why.
check('tarball digest is computable', sha256.length === 64, sha256 || '(unavailable: unreadable)',
  '64 hex characters');
check('tarball is not empty', Boolean(tarballStat && tarballStat.size > 0),
  tarballStat ? tarballStat.size : '(missing)', '> 0 bytes');

/* ------------------------------------------------- the consumer, outside the repo */

/** `parent` is optional: an empty `parent` asks only "is `child` a real path below something". */
function inside(child, parent) {
  if (typeof child !== 'string' || child === '') return false;
  if (typeof parent !== 'string' || parent === '') return true;
  const rel = relative(parent, child);
  return rel !== '' && !rel.startsWith('..') && !isAbsolute(rel);
}

const consumerInsideRepo = inside(realpathSyncSafe(consumerDir), realpathSyncSafe(repoRoot));
if (consumerInsideRepo && !options.allowInRepoTemp) {
  fail(1, `the consumer directory ${consumerDir} is inside the repository ${repoRoot}.\n`
    + '  A pnpm install below a workspace root is treated as a workspace member and rewrites the\n'
    + '  repository-root pnpm-lock.yaml. Pass --temp-dir with a path outside the repository, or\n'
    + '  --allow-in-repo-temp to demonstrate the failure this script is built to prevent.');
}

// On Windows `pnpm` on PATH is `pnpm.cmd`, which `execFileSync` cannot spawn without a shell; with
// one, a path containing a space is handled by execFile rather than by a shell word split.
const spawnOpts = { encoding: 'utf8', shell: process.platform === 'win32' };
const git = (args) => execFileSync('git', args, { cwd: repoRoot, encoding: 'utf8' });
function worktreeStatus() {
  try {
    return git(['status', '--porcelain']);
  } catch (error) {
    fail(1, `could not read the repository status, so nothing here can be trusted: ${error.message}`);
  }
}
const lockfileLine = (status) => status.split('\n').find((line) => line.endsWith('pnpm-lock.yaml')) || '';

const before = worktreeStatus();
process.stdout.write(`## smoke-consumer: pnpm ${options.pnpm}\n`);

if (!options.keep && existsSync(consumerDir)) rmSync(consumerDir, { recursive: true, force: true });
mkdirSync(consumerDir, { recursive: true });
const consumerManifest = { name: 'smoke-consumer', version: '0.0.0', private: true };
if (packageManagerPin !== '(none)' && packageManagerPin !== '(unreadable)') {
  consumerManifest.packageManager = packageManagerPin;
}
writeFileSync(join(consumerDir, 'package.json'), `${JSON.stringify(consumerManifest, null, 2)}\n`);

const pnpmVersion = (() => {
  try {
    return execFileSync(options.pnpm, ['--version'], { cwd: consumerDir, ...spawnOpts }).trim();
  } catch {
    return '(unavailable)';
  }
})();
process.stdout.write(`## smoke-consumer: pnpm version ${pnpmVersion} in the consumer\n`);

// `file:` needs a native absolute path, and on Windows a leading slash makes pnpm hang, so the spec
// is assembled from the platform's own path rather than from a file: URL.
const fileSpec = `file:${tarball.replace(/\\/g, '/')}`;
process.stdout.write(`## smoke-consumer: ${options.pnpm} add ${fileSpec} --offline --ignore-scripts\n`);

let installOutput = '';
let installError = '';
let installOk = true;
try {
  installOutput = execFileSync(options.pnpm, ['add', fileSpec, '--offline', '--ignore-scripts'], {
    cwd: consumerDir,
    ...spawnOpts,
    stdio: ['ignore', 'pipe', 'pipe'],
    env: { ...process.env, npm_config_yes: 'true' },
  });
} catch (error) {
  installOk = false;
  installOutput = `${error.stdout || ''}`;
  installError = `${error.stderr || ''}`;
}
process.stdout.write(`## smoke-consumer: install exit ${installOk ? 0 : 1}\n`);
// The install is itself an assertion, not a precondition. It was a precondition once, and the
// verdict it produced then said "5/5 assertions held" with exit 0 for a tarball whose install had
// failed outright -- a criterion reporting success about the one thing that never happened.
check('the package installs', installOk, installOk ? 'installed' : 'the install failed', 'installed');
if (!installOk) {
  const tail = `${installOutput}\n${installError}`.trim().split('\n').slice(-25).join('\n');
  process.stderr.write(`smoke-consumer: the install failed; the last lines were:\n${tail}\n`);
}

const after = worktreeStatus();
const lockfileAfter = lockfileLine(after);
check('the install did not change the repository worktree', after === before,
  after === before ? 'unchanged' : `dirty after the install:\n${after}`,
  before === '' ? 'clean before and after' : 'the same as before the install');
check('the repository lockfile is not dirty', lockfileAfter === '',
  lockfileAfter || '(clean)',
  '(empty)');
if (lockfileAfter !== '' && !options.allowInRepoTemp) {
  process.stderr.write(
    'smoke-consumer: repair it with `git checkout -- pnpm-lock.yaml` and move the consumer\n'
    + '  directory outside the repository; see the header of this file.\n',
  );
}

if (!installOk) {
  // Stop here, but through the same verdict: a failed check is already recorded, so the log still
  // ends with a complete count rather than a truncated one, and the exit code is 1 either way.
  report('the install failed, so none of the package assertions were attempted');
}

const installedRoot = join(consumerDir, 'node_modules', '@online-gal');
let enginesDir = null;
try {
  enginesDir = realpathSync(join(installedRoot, 'engines'));
} catch {
  enginesDir = null;
}

const topLevel = existsSync(installedRoot)
  ? readdirSync(installedRoot).filter((name) => !name.startsWith('.')).sort()
  : [];
check('the consumer top level has exactly one @online-gal package',
  topLevel.join('|') === EXPECTED.topLevelPackages.join('|'), topLevel, EXPECTED.topLevelPackages);

if (!enginesDir) {
  report('the aggregate did not land where a package install puts it');
}

const installedManifest = require_manifest(enginesDir);
check('the installed manifest declares no dependencies',
  !('dependencies' in installedManifest),
  'dependencies' in installedManifest ? Object.keys(installedManifest.dependencies).join(', ') : 'absent',
  'absent');
process.stdout.write(`## smoke-consumer: installed ${installedManifest.name}@${installedManifest.version} (${installedManifest.license})\n`);

/* ------------------------------------------------------------------ assertions */

// Bare specifiers are resolved against a require rooted at the directory that should be able to see
// them: this script's own location is inside the repository, so resolving from here would find the
// workspace instead of the consumer.
const consumerRequire = createRequire(join(consumerDir, 'noop.cjs'));
const enginesRequire = createRequire(join(enginesDir, 'noop.cjs'));
const importFile = (path) => import(pathToFileURL(path).href);

const engines = await attempt('the aggregate imports', 'a module object',
  async () => {
    const module = await importFile(consumerRequire.resolve('@online-gal/engines'));
    return Object.keys(module).length ? module : false;
  },
  (module) => `${Object.keys(module).length} exports`);
if (engines.ok) {
  const module = engines.value;
  for (const [name, expected] of [
    ['listEngines', EXPECTED.listEngines],
    ['engineProviders', EXPECTED.engineProviders],
    ['ENGINE_PACKAGES', EXPECTED.enginePackages],
  ]) {
    const exported = module[name];
    const actual = typeof exported === 'function' ? exported().length
      : Array.isArray(exported) ? exported.length
        : `not exported (${typeof exported})`;
    check(`${name} reports the whole engine set`, actual === expected, actual, expected);
  }
  // Read through its own call rather than through a value the checks above may have rejected, so
  // one missing export costs one failed line instead of a thrown error and no count at all.
  const ids = typeof module.listEngines === 'function'
    ? module.listEngines().map((entry) => entry.runtimeId)
    : [];
  const duplicates = ids.filter((id, index) => ids.indexOf(id) !== index);
  check('every runtime id is unique', duplicates.length === 0, duplicates.length,
    '0 duplicates');
  process.stdout.write(`## smoke-consumer: runtime ids ${ids.join(', ') || '(unavailable)'}\n`);
}

const vendoredRoot = join(enginesDir, 'node_modules', '@online-gal');
const vendored = existsSync(vendoredRoot)
  ? readdirSync(vendoredRoot).filter((name) => !name.startsWith('.')).sort()
  : [];
check('the archive vendors the whole closure', vendored.length === EXPECTED.vendoredPackageCount,
  vendored, `${EXPECTED.vendoredPackageCount} packages`);

const vendoredDetails = [];
for (const name of vendored) {
  const specifier = `@online-gal/${name}`;
  const entry = await attempt(`${specifier} resolves inside the archive`, 'a path under the package',
    () => enginesRequire.resolve(specifier),
    (path) => relative(enginesDir, realpathSyncSafe(path)));
  if (!entry.ok) continue;
  const path = entry.value;
  const within = inside(realpathSyncSafe(path), enginesDir);
  check(`${specifier} resolves inside the archive, not beside it`, within,
    within ? relative(enginesDir, realpathSyncSafe(path)) : path, 'a path under the package');
  const loaded = await attempt(`${specifier} imports from inside the archive`, 'a module object', async () => {
    const module = await importFile(path);
    return Object.keys(module).length;
  }, (count) => `${count} exports`);
  if (loaded.ok) {
    vendoredDetails.push(`${specifier} -> ${relative(enginesDir, realpathSyncSafe(path))} (${loaded.value} exports)`);
  }
}
for (const line of vendoredDetails) process.stdout.write(`## smoke-consumer: ${line}\n`);

for (const name of vendored) {
  const specifier = `@online-gal/${name}`;
  let resolved = '';
  let code = 'resolved';
  try {
    resolved = consumerRequire.resolve(specifier);
  } catch (error) {
    code = error.code || 'threw';
  }
  check(`the consumer top level cannot resolve ${specifier}`, code === 'MODULE_NOT_FOUND',
    resolved ? `resolved to ${resolved}` : code, 'MODULE_NOT_FOUND');
}

const artemisDir = join(vendoredRoot, 'runtime-artemis');
for (const moduleName of ARTEMIS_HOST_MODULES) {
  const file = join(artemisDir, 'dist', 'host', `${moduleName}.js`);
  if (!existsSync(file)) {
    check(`runtime-artemis dist/host/${moduleName}.js ships in the archive`, false,
      'missing from the archive', 'a file');
    continue;
  }
  const exports_ = await attempt(`runtime-artemis dist/host/${moduleName}.js deep-imports`, 'a module object',
    async () => Object.keys(await importFile(file)).length, (count) => `${count} exports`);
  if (exports_.ok) process.stdout.write(`## smoke-consumer: host/${moduleName}.js ${exports_.value} exports\n`);
}

/* ------------------------------------------------------------------ verdict */

// The summary line is the verdict; the failed lines above already said everything. Re-printing them
// here used to double every failure in the log, which is how a real signal ends up looking like
// noise -- and a gate whose log is noisy is a gate people stop reading.
function report(forcedMessage) {
  const failed = checks.filter((entry) => !entry.holds);
  process.stdout.write(
    `## smoke-consumer: ${checks.length - failed.length}/${checks.length} assertions held`
    + `${forcedMessage ? ` (${forcedMessage})` : ''}\n`,
  );
  if (!options.keep && existsSync(consumerDir)) rmSync(consumerDir, { recursive: true, force: true });
  else process.stdout.write(`## smoke-consumer: kept ${consumerDir}\n`);
  process.exit(failed.length ? 1 : 0);
}

report();
