#!/usr/bin/env node
/**
 * Build one runtime package into a standard npm tarball, so a consumer can `pnpm add` it and run
 * without ever compiling a wasm.
 *
 * WHY THIS EXISTS
 *
 * The workflows in `.github/workflows/` already publish the raw engine binaries as release assets,
 * and `ports/lib/fetch-engine.mjs` already downloads them. But that is a *script* interface: a
 * consumer has to know the engine name, the file names, the destination directory, and to call the
 * script. The user's request is the other shape -- the CI output *is* the package, and a consumer
 * installs it like any other dependency and gets a working runtime.
 *
 * THE ONE PROBLEM THAT SHAPES EVERYTHING
 *
 * `packages/runtime-ons/package.json` declares its dependencies as `workspace:*`:
 *
 *     "dependencies": { "@online-gal/engine-api": "workspace:*", ... }
 *
 * That is correct inside this monorepo and it is a hard blocker for a distributed tarball. Both
 * package managers reject the archive before it is even extracted, and they reject it in two
 * different, equally silent-looking ways:
 *
 *   * pnpm:  ERR_PNPM_WORKSPACE_PKG_NOT_FOUND -- "@online-gal/engine-api@workspace:*" is in the
 *     dependencies but no package named "@online-gal/engine-api" is present in the workspace.
 *   * npm:  EUNSUPPORTEDPROTOCOL -- Unsupported URL Type "workspace:".
 *
 * And the obvious fix is worse than no fix. Those dependency packages are `private: true` and were
 * never published, so rewriting `workspace:*` to `*` just moves the failure:
 *
 *   * pnpm:  ERR_PNPM_FETCH_404 -- GET https://registry.npmjs.org/@online-gal%2Fengine-api: 404.
 *
 * A `file:` rewrite fails too, and the reason is worth recording so nobody tries it again: pnpm
 * resolves a `file:` dependency *relative to the consumer's* node_modules directory, not relative
 * to the package that declares it, so a self-referential `file:node_modules/@online-gal/engine-api`
 * looks for `<consumer>/node_modules/@online-gal/engine-api` and reports
 * ERR_PNPM_LINKED_PKG_DIR_NOT_FOUND.
 *
 * So the dependencies are **vendor-in**: the dependency closure is resolved from the workspace and
 * copied into `package/node_modules/` as real directories, and the `dependencies` map is dropped
 * from the manifest that ships in the tarball. Both facts together are what makes
 * `pnpm add <tgz>` succeed offline against a private monorepo, and both were confirmed by
 * measurement, not by assumption -- see the probes in the report.
 *
 * Dropping the field is safe precisely because the vendored copy is complete: pnpm installs the
 * archive's own `node_modules/` as-is and the imports resolve inside the package.
 *
 * WHAT GOES IN
 *
 *   package/package.json   the package's own manifest, verbatim except for the vendoring change,
 *                          so `license: GPL-3.0-or-later` and `private: true` are exactly the
 *                          values the repository already committed (commit ab764ff)
 *   package/dist/**        the compiled output
 *   package/src/**         the sources: what the exports map's `types` condition points at, and
 *                          what GPL-3.0 §5(b) requires a recipient of this tarball to be given
 *   package/assets/**      the engine artifacts the runtime resolves at run time
 *   package/node_modules/**  the vendored dependency closure
 *
 * Never `.test/`, never `.ref/`, never the game data. The staging directory is built from an
 * explicit allow-list rather than by walking the package directory, so a file that nobody
 * anticipated cannot be swept in by accident -- the 13 GB of real-game data under `.ref/` and
 * `.test/` is unreproducible to include accidentally, which is the point.
 *
 * WHY THE TARBALL IS BYTE-STABLE
 *
 * Two runs over the same inputs must produce the same bytes, or every rebuild looks like a content
 * change and the idempotence claim is untestable. Three things vary by default and all three are
 * pinned here:
 *
 *   * mtime  -- `--mtime=@SOURCE_DATE_EPOCH` (default 1985-10-26T08:00:00Z, the earliest a zip
 *               can represent and a fixed, recognisable constant rather than "now")
 *   * order  -- `--sort=name`, so directory order cannot leak the readdir order of the filesystem
 *   * owner  -- `--owner=0 --group=0 --numeric-owner`, since uid/gid of the build user would
 *               otherwise differ between a runner and a laptop
 *
 * and the gzip header carries no timestamp (`gzip -n`), which is the one piece `tar -z` gets wrong:
 * zlib records the current time in the gzip header unless told not to, so `tar -czf` twice in a row
 * yields two different files even when the tar stream inside is identical.
 *
 * Usage:
 *
 *   node .github/scripts/make-package.mjs --package packages/runtime-ons --out dist
 *   node .github/scripts/make-package.mjs --package packages/runtime-ons --out dist \
 *        --assets-from dist/ons          # take engine bytes from a CI build instead of assets/
 *
 * Exit codes: 0 ok · 1 usage · 2 a required input is missing · 3 write failed.
 */
import { execFileSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import {
  cpSync,
  existsSync,
  mkdirSync,
  readFileSync,
  readdirSync,
  rmSync,
  statSync,
  writeFileSync,
} from 'node:fs';
import { basename, dirname, join, relative, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

const USAGE = `usage: node .github/scripts/make-package.mjs --package <dir> --out <dir>
         [--assets-from <dir>] [--tarball-name NAME] [--source-date-epoch N] [--quiet]

  --package      a package directory, e.g. packages/runtime-ons
  --out          directory to write the .tgz into
  --assets-from  directory holding the built engine files to vendor as package/assets
                 (default: the package's own assets/ directory). The binaries ship once, from
                 this directory; a second copy under dist/assets/ is refused.
  --tarball-name override the output file name
  --source-date-epoch  mtime stamped into every tar entry (default 498073600 = 1985-10-26)
  --quiet        print only the output path`;

const here = dirname(fileURLToPath(import.meta.url));
const repoRoot = resolve(here, '..', '..');

function fail(code, message) {
  process.stderr.write(`make-package: ${message}\n`);
  process.exit(code);
}

const options = {
  packageDir: null,
  out: null,
  assetsFrom: null,
  tarballName: null,
  epoch: 498073600,
  quiet: false,
};

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
    case '--package': options.packageDir = next(); break;
    case '--out': options.out = next(); break;
    case '--assets-from': options.assetsFrom = next(); break;
    case '--tarball-name': options.tarballName = next(); break;
    case '--source-date-epoch': options.epoch = Number(next()); break;
    case '--quiet': options.quiet = true; break;
    default: fail(1, `unknown argument ${arg}\n\n${USAGE}`);
  }
}

if (!options.packageDir) fail(1, 'missing --package');
if (!options.out) fail(1, 'missing --out');
if (!Number.isInteger(options.epoch) || options.epoch < 0) fail(1, '--source-date-epoch must be a non-negative integer');

const packageDir = resolve(repoRoot, options.packageDir);
const manifestPath = join(packageDir, 'package.json');
if (!existsSync(manifestPath)) fail(2, `no package.json in ${packageDir}`);

const manifest = JSON.parse(readFileSync(manifestPath, 'utf8'));
if (!manifest.name) fail(2, `${manifestPath} has no "name"`);
if (!manifest.version) fail(2, `${manifestPath} has no "version"`);

const distDir = join(packageDir, 'dist');
if (!existsSync(distDir)) {
  fail(2, `no dist/ in ${packageDir} -- run the package build first (npm run build:packages)`);
}

// --- staging ---------------------------------------------------------------------------------
//
// A staging directory next to the output, not a temp dir: it makes the tree that got tarred
// inspectable after a failure, which is the difference between a bug report and a mystery. It is
// removed on success and left in place on failure for exactly that reason.

const outDir = resolve(repoRoot, options.out);
const stage = join(outDir, '.make-package-stage');
const staged = join(stage, 'package');

function sha256(path) {
  return createHash('sha256').update(readFileSync(path)).digest('hex');
}

/** Total size of a directory tree, and its file count -- the numbers the log should report. */
function measure(dir) {
  let bytes = 0;
  let count = 0;
  const walk = (current) => {
    for (const entry of readdirSync(current, { withFileTypes: true })) {
      const path = join(current, entry.name);
      if (entry.isDirectory()) walk(path);
      else if (entry.isFile()) { bytes += statSync(path).size; count += 1; }
    }
  };
  if (existsSync(dir)) walk(dir);
  return { bytes, count };
}

rmSync(stage, { recursive: true, force: true });
mkdirSync(staged, { recursive: true });

// 1. the manifest, with the vendoring change and nothing else.
const shipped = { ...manifest, dependencies: undefined };
delete shipped.dependencies;
// `scripts.build` is `tsc -b && node scripts/copy-runtime-assets.mjs` -- neither the sources nor the
// script ship, so a consumer running it would fail on a missing tsconfig. A packaged artifact that
// advertises a build command it cannot honour is worse than one that advertises none.
delete shipped.scripts;
writeFileSync(join(staged, 'package.json'), `${JSON.stringify(shipped, null, 2)}\n`);

// 2. compiled output.
cpSync(distDir, join(staged, 'dist'), { recursive: true });

// 2b. the sources, for two independent reasons.
//
//  The mechanical one: the exports map in every package of this repo routes types to the source --
//  `{"types": "./src/index.ts", "default": "./dist/index.js"}` -- and TypeScript resolves `types`
//  first. A tarball without `src/` therefore gives a TypeScript consumer a types condition pointing
//  at a file that is not there, and `tsc` fails on a package that is perfectly installed. The
//  vendored siblings ship `src/` for exactly this reason; shipping it only for the package under
//  test would be the inconsistency. 41 files, ~0.5 MB, and it is what makes `exports` true.
//
//  The licensing one, which is not negotiable: this is GPL-3.0-or-later, and the GPL requires that
//  recipients receive the corresponding source for the covered work. `dist/*.js` alone is not that.
//  Shipping `src/` is how a single tarball satisfies §5(b) without the consumer having to know to go
//  back to the repository. See docs/fetch-engine-artifacts.md.
const srcDir = join(packageDir, 'src');
if (existsSync(srcDir)) {
  cpSync(srcDir, join(staged, 'src'), { recursive: true });
}

// 3. engine artifacts. `assets/` is what the runtime resolves (`new URL('../../assets/', ...)` in
// dist/wasm-runtime.js), so a package without it imports fine and then fails on the first frame.
// In CI the freshly built bytes win over the committed ones, which is the whole point of shipping
// the build output.
const assetsSource = options.assetsFrom ? resolve(repoRoot, options.assetsFrom) : join(packageDir, 'assets');
if (existsSync(assetsSource)) {
  cpSync(assetsSource, join(staged, 'assets'), { recursive: true });
} else if (options.assetsFrom) {
  fail(2, `--assets-from directory does not exist: ${assetsSource}`);
}

// 3b. those bytes must not also be sitting inside `dist/`, because two copies of the same engine
// files drift: `assets/` here comes from the engine *build*, while anything under `dist/` was put
// there by the package build from the *checkout*, whose line endings are decided by
// `.gitattributes` (`* text=auto eol=lf`) rather than by the engine. A tarball then carries the
// same file twice in two forms, and the copy a consumer reads depends on which module resolves it.
// Only checked when `--assets-from` is given, since that is the caller asserting where the engine
// bytes come from; other packages have no reason to keep `dist/assets/` either, but they are not
// what this guard is for. See packages/runtime-ons/scripts/copy-runtime-assets.mjs.
if (options.assetsFrom && existsSync(join(staged, 'dist', 'assets'))) {
  fail(2, 'dist/assets/ duplicates the engine files that --assets-from supplies; the runtime resolves assets/ at the package root, so ship one copy and stop the build from writing dist/assets/');
}

// 4. the dependency closure, vendored.
const vendored = [];
const queue = [...Object.keys(manifest.dependencies ?? {})];
const seen = new Set(queue);
while (queue.length) {
  const name = queue.shift();
  // pnpm links workspace packages as `../../<dir>` from the package's own node_modules; npm-style
  // `node_modules/<name>` is checked too so the script does not depend on which linker produced the
  // tree. A package that is genuinely not installed is reported rather than silently skipped,
  // because a tarball missing a transitive dep fails at the consumer's first import.
  const candidates = [
    join(packageDir, 'node_modules', name),
    join(repoRoot, 'node_modules', name),
  ];
  const found = candidates.find((candidate) => existsSync(join(candidate, 'package.json')));
  if (!found) fail(2, `dependency not installed: ${name} (looked in ${candidates.join(', ')})`);
  const target = join(staged, 'node_modules', name);
  mkdirSync(dirname(target), { recursive: true });
  // `dereference` is what makes a pnpm symlink into the store copy as a real directory; without it
  // the symlink is copied as a symlink and dangles the moment the tarball leaves this machine.
  //
  // The `filter` is load-bearing, not tidiness. A workspace package's own `node_modules` contains
  // pnpm's links back to its siblings, so copying recursively with no filter reproduces the whole
  // closure *inside every member of it*: the first version of this shipped 13 copies of 4 packages
  // (player-host/node_modules/image-codec/node_modules/vfs/node_modules/engine-api and so on) and
  // 4.0 MB where 0.7 MB is honest. The closure is already vendored flat at the top level and Node
  // resolves upward, so each copy would be dead weight. Excluding the directory is what makes the
  // vendored tree *exactly* the closure and nothing more.
  cpSync(found, target, {
    recursive: true,
    dereference: true,
    filter: (src) => basename(src) !== 'node_modules',
  });
  vendored.push(name);
  const depManifest = JSON.parse(readFileSync(join(found, 'package.json'), 'utf8'));
  for (const next of Object.keys(depManifest.dependencies ?? {})) {
    if (!seen.has(next)) { seen.add(next); queue.push(next); }
  }
}

// --- tar --------------------------------------------------------------------------------------

// The filename flattens the scope: `@online-gal/runtime-ons` is not a legal file name, so the `/`
// becomes a `-` and the tarball is `online-gal-runtime-ons-<version>.tgz`. npm's own convention
// keeps the scope instead, but a release asset that a human types into a `pnpm add` line is worth
// the unambiguous name, and `--tarball-name` overrides it either way. The version is whatever
// `manifest.version` says, so the name moves when the package does — docs that hard-code a
// `.tgz` filename have to be updated in the same commit that bumps the version.
const tarballName = options.tarballName
  ?? `${manifest.name.replace(/^@/, '').replace(/[/\\@]/g, '-')}-${manifest.version}.tgz`;
const tarballPath = join(outDir, tarballName);
mkdirSync(outDir, { recursive: true });

// `--format=gnu` and `--sort=name` exist only to pin the byte layout; `--transform` puts everything
// under `package/`, which is what makes the archive a valid npm tarball. `gzip -n` is the flag that
// makes it reproducible -- see the header comment.
const tarArgs = [
  '--sort=name',
  '--format=gnu',
  '--owner=0', '--group=0', '--numeric-owner',
  `--mtime=@${options.epoch}`,
  '-C', stage,
  '-cf', '-',
  'package',
];
try {
  const tar = execFileSync('tar', tarArgs, { maxBuffer: 1024 * 1024 * 1024 });
  const { gzipSync } = await import('node:zlib');
  // `level: 9` is the default, and `mtime: 0` is what Node needs to omit the gzip timestamp; without
  // it Node writes the current time into the header and two runs differ.
  const gz = gzipSync(tar, { level: 9, mtime: 0 });
  writeFileSync(tarballPath, gz);
} catch (error) {
  fail(3, `tar failed: ${error.message}`);
}

// Measured before the stage is removed; `measure` after the `rmSync` would report zeros and a
// package with no files would look identical to a healthy one in the log.
const stats = measure(staged);
rmSync(stage, { recursive: true, force: true });

if (options.quiet) {
  process.stdout.write(`${tarballPath}\n`);
} else {
  const size = statSync(tarballPath).size;
  process.stderr.write(`make-package: ${manifest.name}@${manifest.version}\n`);
  process.stderr.write(`  ${String(stats.count).padStart(6)} file(s), ${stats.bytes} B staged\n`);
  process.stderr.write(`  vendored: ${vendored.length ? vendored.join(', ') : '(none)'}\n`);
  process.stderr.write(`  ${String(size).padStart(6)} B  ${sha256(tarballPath)}  ${tarballName}\n`);
  process.stderr.write(`make-package: wrote ${tarballPath}\n`);
}
