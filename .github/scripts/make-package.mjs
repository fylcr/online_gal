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
 * `packages/engine/core/onscripter-yuri/package.json` declares its dependencies as `workspace:*`:
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
 *   package/LICENSE        the GPL-3 text, which GPL §5(a) requires to travel with the work
 *   package/THIRD_PARTY_NOTICES.md  the upstream notices covering the vendored closure
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
 * change and the idempotence claim is untestable. Seven things vary by default and all seven are
 * pinned here:
 *
 *   * mtime  -- `--mtime=@SOURCE_DATE_EPOCH` (default 1985-10-26T08:00:00Z, the earliest a zip
 *               can represent and a fixed, recognisable constant rather than "now")
 *   * order  -- `--sort=name`, so directory order cannot leak the readdir order of the filesystem
 *   * owner  -- `--owner=0 --group=0 --numeric-owner`, since uid/gid of the build user would
 *               otherwise differ between a runner and a laptop
 *   * mode   -- normalized to 0644 for files and 0755 for directories before `tar` runs, because
 *               tar records the mode of the file it found. See "MODES ARE ENVIRONMENT, NOT INPUT".
 *   * line endings -- normalized to LF in text files before `tar` runs, because the checkout's
 *               line endings belong to whoever checked it out, not to the repository. See "LINE
 *               ENDINGS ARE THE CHECKOUT, NOT THE REPOSITORY".
 *   * gzip timestamp -- `mtime: 0` below, since zlib otherwise records the current time in the
 *               gzip header. This is the one piece `tar -czf` gets wrong: two runs in a row yield
 *               two different files even when the tar stream inside is identical.
 *   * gzip OS byte -- forced to 3, because zlib writes the *host's* OS there. See "THE GZIP OS
 *               BYTE IS THE HOST, NOT THE ARCHIVE".
 *
 * THE EIGHTH THING IS NOT PINNED, IT IS CHECKED: THE BUILD MUST BE FRESH
 *
 * The seven above are all decided *inside* this script, which is why they are pins: given the same
 * tree, this script cannot produce different bytes. The eighth problem cannot be pinned that way,
 * because it happens before the script is ever called -- the `dist/` it is about to archive may be
 * older than the `src/` it was compiled from.
 *
 * CI is immune by construction: every run is a fresh checkout with no `.tsbuildinfo`, so
 * `pnpm run build:packages` is always a full build and the `dist/` it archives is by construction
 * newer than every source file. A developer is not: `tsc -b` is incremental, it rewrites only what
 * changed, and a `git checkout`, a branch switch or a `git pull` advances the mtime of `src/**`
 * without advancing `dist/**` by one byte.
 *
 * The failure mode is the worst kind, because it is silent. This is measured, not imagined: the
 * `@online-gal/runtime-artemis` 0.1.0 tarball that CI published for `580cdd6` was 300,290 B /
 * sha256 b2ce4294..., while a local build of the same commit was 299,954 B / 8af69e97... -- and the
 * two entry lists were identical, with the tar streams the same length. The only files that
 * differed were `dist/host/timefmt.{js,js.map,d.ts.map}`, compiled before the last edit to
 * `src/host/timefmt.ts` had been made. Re-running the build made the local tarball byte-identical
 * to the published one, which is the proof that the build was stale and not the packer.
 *
 * So rather than normalizing a stale `dist/` (which would ship yesterday's answer and call it
 * today's), the packer refuses. It compares the newest mtime under `src/**` against the newest
 * build witness of the package under test *and of every package in the vendored closure* -- an
 * aggregate's bytes include its siblings' `dist/`, so a stale `vfs` is just as wrong as a stale
 * `engines`. The witness is the newer of the newest file in `dist/` and `tsconfig.tsbuildinfo`,
 * and it has to be: `tsc -b` is incremental, so a rebuild that finds nothing to do rewrites no
 * output and leaves `dist/` permanently behind a source file whose content never changed, which is
 * the state an ordinary `git checkout` leaves behind. That case is measured in the section on the
 * guard, together with why letting `tsc` speak for itself is not allowed to hide a real edit. It is
 * a guard and not a pin because nothing this script can do would fix it: only a rebuild can, so
 * the only correct response is to stop and say so. See "BUILD FRESHNESS IS AN INPUT, NOT A
 * PROPERTY OF THE ARCHIVE".
 *
 * AND ONE THING WAS NOT PINNED, NOT CHECKED, AND NOT EVEN NOTICEABLE: A BUILD CACHE IS NOT
 * SOURCE
 *
 * The eight above all concern files this script *stages*. For its entire life the script also
 * staged, without ever saying so, `tsconfig.tsbuildinfo` -- the incremental build cache `tsc -b`
 * writes next to each project. It shipped in every published tarball (2 entries in
 * `runtime-artemis`, 4 in `runtime-kag` and `runtime-ons`, 10 in the `engines` aggregate), and
 * that is a reproducibility hole rather than a tidiness complaint, for a reason worth stating
 * precisely because it is not obvious:
 *
 * `tsconfig.tsbuildinfo` records the sha256 of every source file **as the compiler read it**, i.e.
 * before this script normalizes CRLF to LF. So a checkout whose sources arrive with CRLF line
 * endings produces a cache whose embedded digests describe bytes that no longer exist in the
 * archive. Line-ending normalization cannot repair it, because there is no line ending left to
 * normalize: the contaminated value is a hash *of* the pre-normalization text, stored as a 64
 * character string. Measured, not argued: building this repository once from LF sources and once
 * from a CRLF checkout produced tarballs of identical length (8,130,560 B) whose 10 differing
 * files were exactly the 10 `tsconfig.tsbuildinfo` files in the vendored closure -- 819 B larger
 * in aggregate, sha256 `be179a37...` versus `68c172fe...`. Every other one of the 416 files was
 * byte-identical, which is what makes this diagnostic hard to see by eye and easy to misattribute.
 *
 * The fix is not to normalize the cache, and not to rewrite hashes inside it: both would mean the
 * archive carries a build cache that is *wrong* about the tree it sits in, and a consumer's `tsc`
 * would then trust a cache describing source it does not have. A build cache is not source, and
 * this archive already ships the thing a cache exists to accelerate -- `src/**` and `dist/**`
 * together are sufficient to run a full build, and `tsc -b` regenerates the cache in seconds.
 * Shipping it bought nothing and cost reproducibility, so it is now excluded by name, in one
 * place, for the package under test and for every vendored sibling alike.
 *
 * Note what this is *not*: it is not a concession that line endings are unreproducible. The other
 * 416 files prove they are handled -- the packer rewrites them before hashing, so LF and CRLF
 * checkouts converge. This one file escaped because it stores a *derived* digest of the
 * un-normalized bytes rather than the bytes themselves, and only removal closes that hole.
 *
 * AND ONE THING IS NEITHER PINNED NOR AN ENVIRONMENT LEAK: THE COMMITTED ENGINE BYTES MUST
 * MATCH THE LEDGER
 *
 * The eight above are all about this script's own output. This one is about the repository, and it
 * is here because of a defect that reached a published release.
 *
 * `ports/lib/engine-artifacts.json` pins every engine artifact by size and sha256, and its own
 * `$comment` says what those hashes are for: they name the bytes a consumer can verify. Nothing
 * enforced them. So `packages/runtime-ons/assets/onsyuri.html` was committed as LF 23,817 B while
 * the ledger says 24,399 B -- the same number of characters, the 582 CRs missing, and `git status`
 * clean the whole time, because the worktree file *was* the ledger's bytes. The aggregate package
 * vendors the repository's copy, so `@online-gal/engines` 0.1.0 published 23,817 B of engine while
 * the ledger and the ONS release both said 24,399 B, and the only reason anybody noticed was a human
 * downloading the published tarball and comparing it against the ledger. Two machines had compared
 * their worktrees; both were right and the repository was wrong.
 *
 * So the packer checks the ledger before it stages anything. This is a check and not a pin for the
 * same reason freshness is: no normalization inside this script can recover bytes the repository no
 * longer holds, and the correct response is to stop and say which file, what it should be, and what
 * it is. It runs in CI, which is the point -- `build-packages.yml` calls this script, so the first
 * run of the aggregate tarball would have caught it.
 *
 * There is no flag to switch it off, and that is deliberate. `--skip-freshness` exists because a
 * stale `dist/` is a local condition a rebuild fixes in a second; a file whose committed bytes do
 * not match the ledger is a repository fact, and the only fix is to commit the right bytes. A flag
 * would be a way to publish the defect on purpose.
 *
 * Note the interaction with `--assets-from`, which is the opposite arrangement and still passes:
 * the ONS workflow ships engine bytes from a *fresh build*, while the committed copy is what the
 * aggregate vendors, and the two must agree or the same package exists in two forms. That is why
 * this guard is not skipped when `--assets-from` is given, and why the version rule in
 * `build-packages.yml` exists -- a build that changes the engine bytes has to be committed and the
 * aggregate's version bumped in the same commit, or this fails.
 *
 * WHAT "REPRODUCIBLE" ACTUALLY MEANS HERE
 *
 * Two claims, which used to be confused with each other:
 *
 *   * **Same checkout, different toolchains** -- Windows/Node 26 and Linux/Node 22 running this
 *     script over one working tree produce the same tar stream. This was measured.
 *   * **Different checkouts, different machines** -- a developer's CRLF worktree and the runner's
 *     LF checkout of one commit produce the same tar stream. This is what the line-ending rule
 *     buys, and it is the claim that actually matters, because it is the one that says "rebuilding
 *     on a clean CI machine is not a content change". Until that rule existed the second claim was
 *     silently false, and the LICENSE leak below is the proof.
 *
 * The `.tgz` additionally depends on the compressor, and no pinning inside this script can reach
 * into a future zlib's match search. So this script reports two numbers:
 *
 *   * the sha256 of the **uncompressed tar stream** (`tarSha256`), which is the cross-machine
 *     invariant, and
 *   * the sha256 of the `.tgz` (the `sha256` line), which should be compared against another run of
 *     *this* script on a comparable toolchain.
 *
 * A `tarSha256` mismatch is a content change. A `tarSha256` match with a `.tgz` mismatch is a
 * toolchain difference, and is not a regression.
 *
 * Usage:
 *
 *   node .github/scripts/make-package.mjs --package packages/engine/core/onscripter-yuri --out dist
 *   node .github/scripts/make-package.mjs --package packages/engine/core/onscripter-yuri --out dist \
 *        --assets-from dist/ons          # take engine bytes from a CI build instead of assets/
 *   node .github/scripts/make-package.mjs --package packages/engine/core/onscripter-yuri --out dist \
 *        --skip-freshness                # pack a deliberately stale dist/ (see below)
 *
 * Exit codes: 0 ok · 1 usage · 2 a required input is missing, is invalid, the build is stale, or
 * a committed engine file disagrees with the ledger · 3 write failed.
 */
import { execFileSync } from 'node:child_process';
import { createHash } from 'node:crypto';
import {
  chmodSync,
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
import { isSemVer } from './runtime-release-policy.mjs';

const USAGE = `usage: node .github/scripts/make-package.mjs --package <dir> --out <dir>
         [--assets-from <dir>] [--tarball-name NAME] [--version SEMVER] [--source-date-epoch N] [--json] [--quiet]

  --package      a package directory, e.g. packages/engine/core/onscripter-yuri
  --out          directory to write the .tgz into
  --assets-from  directory holding the built engine files to vendor as package/assets
                 (default: the package's own assets/ directory). The binaries ship once, from
                 this directory; a second copy under dist/assets/ is refused.
  --version      full SemVer written into the packed manifest and tarball name only;
                 the tracked package.json is not modified
  --tarball-name override the output file name
  --source-date-epoch  mtime stamped into every tar entry (default 498073600 = 1985-10-26)
  --skip-freshness     do not refuse to pack when src/ is newer than dist/ (default: refuse)
  --json         print one JSON object on stdout; human output goes to stderr
  --quiet        print only the output path

Before anything is staged the committed engine bytes are verified against
ports/lib/engine-artifacts.json. That check has no flag: a committed file that disagrees with the
ledger is a repository fact, and only a commit can fix it.`;

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
  version: null,
  epoch: 498073600,
  json: false,
  quiet: false,
  skipFreshness: false,
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
    case '--version': options.version = next(); break;
    case '--source-date-epoch': options.epoch = Number(next()); break;
    case '--json': options.json = true; break;
    case '--skip-freshness': options.skipFreshness = true; break;
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

// --- the engine-byte ledger --------------------------------------------------------------------
//
// Placed before the manifest is even read, because the ledger's subject is the repository rather than
// the package being built: an `engines` aggregate is only as trustworthy as the bytes it vendors, and
// `engine-core-onscripter-yuri` is in that closure, so a corrupted engine file poisons every package that ships it.
//
// The lookup is by basename against a fixed set of candidate directories, and that is not a
// convenience -- it is the narrowest thing that catches this defect without catching something else.
// Measured, not guessed: a repo-wide basename search for the krkr ledger's `index.html` also finds
// `ports/ons/harness/index.html`, which is 6,827 B / 253b38d0... where the ledger says 53,590 B /
// 6c8c4994.... Searching the whole tree would either validate an unrelated file or fail the pack for
// something this pack does not contain. The candidate set is instead "the directories the repository
// keeps engine bytes in", and an entry that is not found there is skipped rather than failed, because
// most ledger entries are *build outputs* that only exist on a runner: the six krkr files are
// produced by `.github/scripts/build-krkr.sh` and are committed nowhere. Skipping is the honest
// reading of "every file that is in the repository, must match", and the log says how many were
// checked so a silent zero is visible.
const LEDGER_CANDIDATE_DIRS = ['packages/engine/core/onscripter-yuri/assets', 'assets', 'dist/assets'];
const ledgerPath = join(repoRoot, 'ports', 'lib', 'engine-artifacts.json');
let ledgerChecked = 0;
if (existsSync(ledgerPath)) {
  const ledgerBytes = readFileSync(ledgerPath);
  if (ledgerBytes[0] === 0xef && ledgerBytes[1] === 0xbb && ledgerBytes[2] === 0xbf) {
    fail(2, `${ledgerPath} starts with a UTF-8 BOM (ef bb bf); JSON does not allow one -- re-save it without the BOM and commit that`);
  }
  const ledger = JSON.parse(ledgerBytes.toString('utf8'));
  for (const [group, groupEntry] of Object.entries(ledger)) {
    if (group.startsWith('$') || !Array.isArray(groupEntry?.files)) continue;
    for (const entry of groupEntry.files) {
      const found = LEDGER_CANDIDATE_DIRS
        .map((dir) => join(repoRoot, dir, entry.name))
        .find((candidate) => existsSync(candidate));
      if (!found) continue; // a build output that only exists on the runner; not ours to verify
      const actual = readFileSync(found);
      const actualSha256 = createHash('sha256').update(actual).digest('hex');
      ledgerChecked += 1;
      if (actual.length === entry.size && actualSha256 === entry.sha256) continue;
      const crCount = actual.reduce((n, byte) => n + (byte === 0x0d ? 1 : 0), 0);
      fail(2, `engine bytes do not match ${ledgerPath} (${group}/${entry.name}):
  file     ${relative(repoRoot, found)}
  ledger   ${entry.size} B  sha256 ${entry.sha256}
  actual   ${actual.length} B  sha256 ${actualSha256}${actual.length !== entry.size || crCount > 0 ? `  (${crCount} CR byte(s))` : ''}
  Every consumer that verifies the engine against the ledger will fail on this, and so will every
  package that vendors it -- an aggregate ships the repository's copy, while the engine's own
  workflow ships a fresh build, so the same release would carry two different copies.
  This is almost always a checkout conversion, not a corrupted file. .gitattributes is
  \`${'*'} text=auto eol=lf\`, which rewrites a text file in the worktree; an engine artefact is a
  BUILD OUTPUT whose sha256-pinned bytes ARE the substance, so that subtree must be exempt:
  git add --renormalize is not enough, the committed bytes are what has to change.
    1. add the path to .gitattributes as \`-text -diff\` (see packages/engine/core/onscripter-yuri/assets/**)
    2. restore the correct bytes, then commit them
    3. bump the version of every package that vendors this file -- @online-gal/engines at minimum,
       because its version is what makes a stale aggregate distinguishable from a new one`);
    }
  }
}

const manifestBytes = readFileSync(manifestPath);
// A UTF-8 BOM is not valid JSON, and `JSON.parse` would report it as a syntax error several
// characters into the file rather than where the problem actually is. Worse, the failure would
// surface as "this manifest is broken" on one machine, while a checkout that stripped it would
// package fine -- so it would read as flakiness. JSON forbids a BOM; every manifest in this
// repository is BOM-free, so a BOM means somebody hand-edited the file with a tool that adds one.
// Fail loudly and name the fix instead of silently stripping it: stripping would hide the fact
// that the repository's byte identity depends on these files being byte-exact.
if (manifestBytes[0] === 0xef && manifestBytes[1] === 0xbb && manifestBytes[2] === 0xbf) {
  fail(2, `${manifestPath} starts with a UTF-8 BOM (ef bb bf). JSON does not allow one, and every
  manifest in this repository is BOM-free -- this means the source file needs fixing, not this
  script. Re-save it without the BOM, then commit that.`);
}
const manifest = JSON.parse(manifestBytes.toString('utf8'));
if (!manifest.name) fail(2, `${manifestPath} has no "name"`);
if (!manifest.version) fail(2, `${manifestPath} has no "version"`);
if (options.version !== null) {
  if (!isSemVer(options.version)) {
    fail(1, `--version is not a full SemVer: ${options.version}`);
  }
  manifest.version = options.version;
}

const distDir = join(packageDir, 'dist');
if (!existsSync(distDir)) {
  fail(2, `no dist/ in ${packageDir} -- run the package build first (npm run build:packages)`);
}

// --- the dependency closure -------------------------------------------------------------------
//
// Resolved *before* anything is staged, because both the freshness guard below and the vendoring
// step need it, and because resolving it is the one part of the pack that can fail for a reason
// that has nothing to do with the package under test: a workspace that is not installed. Failing
// early means the staging directory is never created at all.

function resolveClosure(manifest) {
  const order = [];
  const found = new Map();
  const queue = [...Object.keys(manifest.dependencies ?? {})];
  const seen = new Set(queue);
  while (queue.length) {
    const name = queue.shift();
    // pnpm links workspace packages as `../../<dir>` from the package's own node_modules; npm-style
    // `node_modules/<name>` is checked too so the script does not depend on which linker produced the
    // tree. A package that is genuinely not installed is reported rather than silently skipped,
    // because a tarball missing a transitive dep fails at the consumer's first import.
    //
    // The third candidate is the hoisted store `node_modules/.pnpm/node_modules/<name>`, and it is
    // not a nicety: a package whose DIRECT deps are all installed still needs its TRANSITIVE ones
    // vendored, and pnpm does not put those in its own `node_modules` unless they happen to be
    // hoisted. `packages/engines` is the case in point -- its seven direct deps are linked, but its
    // closure reaches `@online-gal/vfs` only through the six runtimes, and `vfs` is linked in
    // `node_modules/.pnpm/node_modules/@online-gal/vfs` alone. Without this candidate the packer
    // aborts with `dependency not installed: @online-gal/vfs` on a workspace that is installed
    // perfectly well, which reads like a broken install rather than a missing lookup path.
    const candidates = [
      join(packageDir, 'node_modules', name),
      join(repoRoot, 'node_modules', name),
      join(repoRoot, 'node_modules', '.pnpm', 'node_modules', name),
    ];
    const located = candidates.find((candidate) => existsSync(join(candidate, 'package.json')));
    if (!located) fail(2, `dependency not installed: ${name} (looked in ${candidates.join(', ')})`);
    found.set(name, located);
    order.push(name);
    const depManifest = JSON.parse(readFileSync(join(located, 'package.json'), 'utf8'));
    for (const next of Object.keys(depManifest.dependencies ?? {})) {
      if (!seen.has(next)) { seen.add(next); queue.push(next); }
    }
  }
  return { order, found };
}

const closure = resolveClosure(manifest);

// --- build freshness --------------------------------------------------------------------------
//
// BUILD FRESHNESS IS AN INPUT, NOT A PROPERTY OF THE ARCHIVE.
//
// Everything this script pins is about the bytes between the tar invocation and the file on disk.
// This one is about the bytes that arrived *already wrong*: a `dist/` compiled from an older `src/`.
// The two are indistinguishable once tarred, and no amount of normalization can tell them apart,
// because a stale build is not malformed -- it is a perfectly good build of a source tree that is
// no longer the one on disk. Only the build can fix it, so the packer's only correct move is to
// refuse and name the fix. See "THE EIGHTH THING IS NOT PINNED, IT IS CHECKED" in the header.
//
// The comparison is newest-src against newest-dist, never newest-src against oldest-dist. A partial
// rebuild legitimately leaves some `dist/` entries older than some `src/` entries -- a `tsc -b` that
// recompiled three of forty files did not become invalid, and comparing against the oldest entry
// would refuse to pack every package that was ever built incrementally. The question is only whether
// *anything* was compiled after the last edit, so the newest of each side is the whole test.
//
// And this test alone is wrong on a repo built with `tsc -b`, which was measured here rather than
// assumed. Two findings, both real:
//
//   1. A single-file-tick false positive. `runtime-ons` came out as src 19:02:28.534 vs dist
//      19:02:28.533 -- 0.7 ms apart, from one build. Directories cannot be witnesses at all: a
//      directory's mtime is when its last entry was created, not a build record, and comparing one
//      against a whole subtree collides with its own files. Only files count, below.
//   2. The real one. `vfs` measured src 22:11:18.105 vs dist 21:40:00.491 -- 30 minutes of apparent
//      staleness -- in a tree where `tsc -b --dry` says every project is up to date, and where
//      re-running the build does NOT close the gap. `tsc -b` is incremental: it skips a project
//      whose CONTENTS did not change, so it rewrites no files and `dist/` stays behind by an amount
//      no rebuild repairs. A source file whose bytes are unchanged but whose mtime moved -- an
//      ordinary `git checkout`, a `git pull`, a branch switch -- leaves the tree there, and that is
//      what this repository looked like after a normal pull. Left to `dist/` alone, this guard would
//      have refused to pack ONS and krkr -- both vendor `vfs` -- over a tree that is in fact fresh.
//      It was proven fresh, not argued: `tsc -b --force` over that tree leaves `packages/vfs/dist`
//      byte-identical.
//
// So the newest `dist/` file is not the only witness, and on its own it is the wrong one.
// `tsc -b` writes `tsconfig.tsbuildinfo` for every project it reaches, INCLUDING the ones it decides
// need no work, and it makes that decision against source *content* rather than mtimes. Its mtime
// is therefore a strictly stronger record: it says "as of here the compiler examined this project's
// sources and found its output current", which is exactly the claim being checked, and it is
// refreshed even on the runs that write no bytes. Taking the newer of the two witnesses resolves
// the measured case exactly -- for `vfs`, the tsbuildinfo at 22:11:20.027 postdates the 22:11:18.105
// edit, and the project is fresh, because the edit changed nothing.
//
// It does not weaken the check against what it exists to catch. The failure it must never miss is
// "edited a source, never rebuilt", and in that order the tsbuildinfo predates the edit, so the
// comparison still fires. What the witness forgives is only a `touch` or checkout that happened and
// was then followed by a build -- and a build is precisely the thing that settles whether the edit
// mattered. It is the same ordering `tsc` itself relies on, because `tsc` reads those same mtimes.
//
// On CI the question does not arise: a fresh checkout has no tsbuildinfo, so the test falls back to
// `dist/` alone, and a full build there always postdates the checkout.
//
// What no timestamp can catch, stated plainly: a content change whose mtime is not newer than both
// witnesses -- an mtime-preserving write, or a timestamp moved backwards. That is beyond what a
// build system can certify either; `tsc`'s own record is consulted for what it compiled, and the
// pinned toolchain decides what it compiles with.
//
// A package with no `dist/`, or no `src/`, is skipped rather than failed. A pure-`src` package has
// nothing to be stale against, and a vendored dependency that ships no compiled output is not
// evidence of anything. Only a package that has BOTH sides is making a claim that can be checked.
function newestMtime(dir) {
  let newest = null;
  let which = null;
  const walk = (current) => {
    for (const entry of readdirSync(current, { withFileTypes: true })) {
      if (entry.name === 'node_modules') continue; // pnpm's links, not this package's sources
      const path = join(current, entry.name);
      const mtime = statSync(path).mtimeMs;
      // Directories are not compiled, and one must never outrank a file: a build leaves directory
      // mtimes at the moment it created the last entry inside them, and a single mtime comparison
      // across a whole subtree is coarse enough that the two sides of the same second collide.
      // Files only -- a directory cannot be "newer than the build" in any way that means staleness.
      if (entry.isDirectory()) {
        walk(path);
      } else if (entry.isFile() && (newest === null || mtime > newest)) {
        newest = mtime;
        which = path;
      }
    }
  };
  if (existsSync(dir)) walk(dir);
  return { newest, which };
}

const freshnessSubjects = [
  { name: manifest.name, dir: packageDir },
  ...closure.order.map((name) => ({
    name,
    dir: closure.found.get(name),
  })),
];

let freshnessChecked = 0;
if (options.skipFreshness) {
  process.stderr.write('make-package: freshness check skipped (--skip-freshness); packaging dist/ as it stands\n');
} else {
  for (const subject of freshnessSubjects) {
    const src = newestMtime(join(subject.dir, 'src'));
    const built = newestMtime(join(subject.dir, 'dist'));
    if (src.newest === null || built.newest === null) continue;
    freshnessChecked += 1;
    // The newer of two witnesses: the newest file `dist/` holds, and `tsconfig.tsbuildinfo`, which
    // `tsc -b` rewrites even on a run that decides to compile nothing. See the note above. A project
    // with no build info was never built by `tsc -b` (an editor's `tsc --noEmit`, or another
    // builder), so it offers no witness and `dist/` stands alone.
    const info = statSync(join(subject.dir, 'tsconfig.tsbuildinfo'), { throwIfNoEntry: false });
    const stamp = Math.max(built.newest, info?.mtimeMs ?? 0);
    if (src.newest > stamp) {
      fail(2, `stale build: ${subject.name} -- ${relative(repoRoot, src.which)} is newer than the newest build witness: ${relative(repoRoot, join(subject.dir, 'dist'))} and ${relative(repoRoot, join(subject.dir, 'tsconfig.tsbuildinfo'))}
  The archive would contain compiled output for an older source tree, and nothing about those bytes
  looks wrong, so the result would be a package that silently differs from the one CI builds.
  Rebuild and pack again:

      pnpm run build:packages`);
    }
  }
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

/**
 * `tsc -b`'s incremental build cache, and nothing else.
 *
 * Excluded from every copy below -- the package under test and each vendored sibling -- by one
 * shared predicate, so there is a single line to change if that ever stops being right. It is not
 * a `node_modules` exclusion: a consumer's `tsc` regenerates it in seconds from the `src/**` and
 * `dist/**` that ship beside it, whereas a stale cache inside an archive is worse than none,
 * because a compiler trusts it. The reason it cannot simply be normalized is in the header: it
 * holds sha256 digests of the sources *as the compiler read them*, so CRLF checkouts wrote
 * digests describing bytes this script is about to rewrite. That is a difference no pass over the
 * output can reach, and removing the file is the only thing that does.
 */
function shipping(src) {
  return basename(src) !== 'tsconfig.tsbuildinfo';
}

// 1. the manifest, with the vendoring change and nothing else.
const shipped = { ...manifest, dependencies: undefined };
delete shipped.dependencies;
// `scripts.build` is `tsc -b && node scripts/copy-runtime-assets.mjs` -- neither the sources nor the
// script ship, so a consumer running it would fail on a missing tsconfig. A packaged artifact that
// advertises a build command it cannot honour is worse than one that advertises none.
delete shipped.scripts;
// The `license` field survives on purpose, and it is worth being explicit about why it is not
// redundant with step 5. `"license": "GPL-3.0-or-later"` is the machine-readable form: it is what a
// consumer's licence scanner reads without opening the archive, and it is what states the obligation
// before the recipient has read a line of the licence text shipped beside it. The text is what makes
// the distribution compliant; the field is what makes the obligation findable. Guarded rather than
// assumed, because a `delete shipped.license` added later would pass every check in this repository --
// nothing here reads the field back.
if (manifest.license && !shipped.license) {
  fail(2, `the shipped manifest lost its "license" field (${manifest.license}); the copy must keep it`);
}
writeFileSync(join(staged, 'package.json'), `${JSON.stringify(shipped, null, 2)}\n`);

// 2. compiled output.
cpSync(distDir, join(staged, 'dist'), { recursive: true, filter: shipping });

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
  cpSync(srcDir, join(staged, 'src'), { recursive: true, filter: shipping });
}

// 3. engine artifacts. `assets/` is what the runtime resolves (`new URL('../assets/', ...)` in
// dist/wasm-runtime.js, whose parent directory is the package root), so a package without it
// imports fine and then fails on the first frame.
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
// what this guard is for. See packages/engine/core/onscripter-yuri/scripts/copy-runtime-assets.mjs.
if (options.assetsFrom && existsSync(join(staged, 'dist', 'assets'))) {
  fail(2, 'dist/assets/ duplicates the engine files that --assets-from supplies; the runtime resolves assets/ at the package root, so ship one copy and stop the build from writing dist/assets/');
}

// 4. the dependency closure, vendored.
const vendored = closure.order;
for (const name of vendored) {
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
  cpSync(closure.found.get(name), target, {
    recursive: true,
    dereference: true,
    filter: (src) => basename(src) !== 'node_modules' && shipping(src),
  });
}

// 5. the licence and the notices.
//
// A `tarball` that carries `"license": "GPL-3.0-or-later"` in its manifest and no licence text is
// not GPL-compliant, it is just self-describing: §5(a) asks for a copy of the licence to travel with
// the work, and a field name in a JSON file is not a copy of the licence. The first build of this
// package shipped zero of `LICENSE`, `COPYING` and `NOTICE` -- which is also what a naive
// `tar -tzf | grep -i licen` finds, and how the omission survived review.
//
// This repository is one GPL work with several packages under it, so the root files are shipped
// verbatim into every package. That is deliberate over-inclusion: the notice file names upstream
// components whose licences travel with the vendored closure, and a recipient of
// `@online-gal/engine-core-onscripter-yuri` receives all four vendored packages' worth of third-party code, so
// giving them the whole notice file is the honest subset. Shipping a *narrower* notice would be
// picking a subset of the obligations by hand, and the only safe hand-picked subset is "everything".
//
// `cpSync` rather than a reformat: the bytes must be the bytes the project publishes, so a
// recipient can `sha256sum` this file against the repository's and get a match. Re-wrapping the text
// to a different width would make that check fail for no benefit.
const licenceFiles = ['LICENSE', 'THIRD_PARTY_NOTICES.md'];
const shippedNotices = [];
for (const name of licenceFiles) {
  const source = join(repoRoot, name);
  if (!existsSync(source)) {
    fail(2, `missing ${source} -- every package here is GPL-3.0-or-later, and §5(a) requires the licence text to travel with the work; a tarball without it is not compliant`);
  }
  cpSync(source, join(staged, name));
  shippedNotices.push(name);
}

// Step 4 copies each vendored package with no filter on file names, so a sibling that carries its
// own `LICENSE`/`NOTICE` already arrives in `node_modules/@online-gal/<name>/`. Nothing here deletes
// them, and that is intentional: a per-package licence that differs from the repository's must
// reach whoever receives that package, and the root files shipped above do not replace it. This count
// is reported rather than asserted, so "the notices travelled" is something the log shows instead of
// something the code claims.
const vendoredNotices = [];
for (const name of vendored) {
  for (const entry of readdirSync(join(staged, 'node_modules', name), { withFileTypes: true })) {
    if (entry.isFile() && /^(LICEN[SC]E|COPYING|NOTICE)/i.test(entry.name)) {
      vendoredNotices.push(`${name}/${entry.name}`);
    }
  }
}

// --- modes ------------------------------------------------------------------------------------
//
// MODES ARE ENVIRONMENT, NOT INPUT.
//
// tar writes the mode of the file it found into the header, so the staging tree's permissions end
// up in the tarball verbatim. That is invisible locally and expensive in CI: the first two builds of
// this package were byte-identical on a laptop and one byte apart on a runner, and the entire cause
// was one file. `actions/upload-artifact` preserves the mode of the file as it sits on the runner,
// so the `--assets-from` copy of `assets/onsyuri.wasm` arrived as 0755 while the same bytes fetched
// by curl locally are 0644:
//
//   CI      2,139,137 B  sha256 95877f89...   -rwxr-xr-x 0/0 4660610 package/assets/onsyuri.wasm
//   local   2,139,136 B  sha256 c30702cb...   -rw-r--r-- 0/0 4660610 package/assets/onsyuri.wasm
//
// The extracted trees were identical -- 209 files, byte for byte -- which is the only reason this was
// diagnosable at all. One permission bit in one header is the entire difference, and it would have
// reappeared on every future run, on whichever machine happened to touch the file differently.
//
// So the modes are not taken from the source at all: the staged tree is normalized to 0644 files and
// 0755 directories, the values a fresh checkout of this repository has under `* text=auto eol=lf`.
// The umask of whoever ran the build never reaches the archive. This runs after staging is complete,
// so it covers the vendored closure and `src/` as well as the engine bytes -- chmod'ing only the
// assets would leave the next file to leak the same way.
function normalizeModes(dir) {
  for (const entry of readdirSync(dir, { withFileTypes: true })) {
    const path = join(dir, entry.name);
    if (entry.isDirectory()) {
      chmodSync(path, 0o755);
      normalizeModes(path);
    } else if (entry.isFile()) {
      chmodSync(path, 0o644);
    }
  }
}
chmodSync(staged, 0o755);
normalizeModes(staged);

// --- line endings --------------------------------------------------------------------------------
//
// LINE ENDINGS ARE THE CHECKOUT, NOT THE REPOSITORY.
//
// The third and last environment leak, and the quietest one. This repository's `.gitattributes` is
// `* text=auto eol=lf` with `core.autocrlf=true`, so the blob in git is LF while a developer's
// worktree is CRLF -- and `git status` reports the CRLF worktree as *clean*, because git compares
// the blob after applying the same conversion. Nothing in the working tree says "this file is
// different from the one in the repository".
//
// The published `@online-gal/runtime-ons` 0.1.2 and a local build of the same commit disagreed, and
// the only file that differed was the licence:
//
//   CI (LF checkout)   35,149 B  sha256 3972dc97...   == git show HEAD:LICENSE, byte for byte
//   local (CRLF worktree) 35,823 B  sha256 230184f6...    exactly 674 bytes larger
//
// 674 is this licence's line count: one CR per line, and nothing else. The two archives' 233 entry
// lists were identical, so it was diagnosable -- but every machine that happened to check out with
// CRLF would have produced a different `.tgz` for a repository state that had not changed at all.
//
// So text files are normalized to LF here, the same way the modes are normalized rather than
// copied: LF is the repository's canonical form (that is what `* text=auto eol=lf` means), and the
// worktree's line endings are a property of the machine, not an input. Two guards keep this from
// eating data:
//
//   * ANY `assets/` directory is skipped, at any depth. Those are engine artefacts whose exact
//     bytes are pinned by hash in `ports/lib/engine-artifacts.json`, and they are committed CRLF:
//     rewriting them would break the identity every consumer checks. The check has to be on the
//     directory NAME and not on a single path, because an aggregate package vendors a whole runtime
//     *with its own `assets/`* under `node_modules/@online-gal/<runtime>/assets/` -- matching only
//     the top-level one silently rewrote the engine bytes inside the vendored copy while leaving the
//     standalone tarball correct, so the same package existed in two byte forms depending on which
//     tarball a consumer installed.
//   * a file containing a NUL byte is treated as binary and left alone. A wasm blob that happens to
//     contain 0x0d must not be rewritten, and NUL is the conventional signal that it is one.
function normalizeLineEndings(dir) {
  let changed = 0;
  for (const entry of readdirSync(dir, { withFileTypes: true })) {
    const path = join(dir, entry.name);
    if (entry.isDirectory()) {
      if (entry.name === 'assets') continue; // pinned engine bytes, byte for byte, at any depth
      changed += normalizeLineEndings(path);
    } else if (entry.isFile()) {
      const bytes = readFileSync(path);
      if (!bytes.includes(0x0d) || bytes.includes(0x00)) continue;
      writeFileSync(path, Buffer.from(bytes.toString('latin1').replaceAll('\r\n', '\n'), 'latin1'));
      changed += 1;
    }
  }
  return changed;
}
const normalizedLineEndings = normalizeLineEndings(staged);

// --- tar --------------------------------------------------------------------------------------

// The filename flattens the scope: `@online-gal/engine-core-onscripter-yuri` is not a legal file name, so the `/`
// becomes a `-` and the tarball is `online-gal-engine-core-onscripter-yuri-<version>.tgz`. npm's own convention
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
let reported = null;

try {
  const tar = execFileSync('tar', tarArgs, { maxBuffer: 1024 * 1024 * 1024 });
  const tarSha256 = createHash('sha256').update(tar).digest('hex');
  const { gzipSync } = await import('node:zlib');
  // `level: 9` is the default, and `mtime: 0` is what Node needs to omit the gzip timestamp; without
  // it Node writes the current time into the header and two runs differ.
  const gz = gzipSync(tar, { level: 9, mtime: 0 });
  // THE GZIP OS BYTE IS THE HOST, NOT THE ARCHIVE.
  //
  // Offset 9 of the gzip header is the OS code, and zlib fills it in from the machine it ran on --
  // 0x03 for Unix, 0x0a for the Windows build of Node. Nothing in the archive depends on it, and it
  // is *not* covered by the CRC32 trailer (the CRC covers the uncompressed data), so rewriting it
  // leaves the file exactly as valid as it was before. Pinning it to 3 costs nothing and stops the
  // `.tgz` from depending on which platform ran the build.
  //
  // This is the second and last place the environment leaked in. The first was the file mode; the
  // deflate stream itself proved stable across zlib versions, because re-compressing the runner's
  // own uncompressed tar locally -- same level, same mtime, only this byte changed -- reproduced the
  // runner's `.tgz` byte for byte. So the deflate output is trustworthy and only the header needed
  // pinning; there was no reason to reach for a different compressor.
  gz[9] = 3;
  writeFileSync(tarballPath, gz);
  reported = { tarSha256, tarSize: tar.length };
} catch (error) {
  fail(3, `tar failed: ${error.message}`);
}

// Measured before the stage is removed; `measure` after the `rmSync` would report zeros and a
// package with no files would look identical to a healthy one in the log.
const stats = measure(staged);
rmSync(stage, { recursive: true, force: true });

const size = statSync(tarballPath).size;
const tgzSha256 = sha256(tarballPath);

if (!options.quiet) {
  process.stderr.write(`make-package: ${manifest.name}@${manifest.version}\n`);
  process.stderr.write(`  ${String(stats.count).padStart(6)} file(s), ${stats.bytes} B staged\n`);
  process.stderr.write(`  vendored: ${vendored.length ? vendored.join(', ') : '(none)'}\n`);
  process.stderr.write(`  notices: ${shippedNotices.join(', ')}${vendoredNotices.length ? ` (+ ${vendoredNotices.join(', ')})` : ''}\n`);
  process.stderr.write(`  line endings: ${normalizedLineEndings ? `${normalizedLineEndings} file(s) normalized to LF` : 'already LF (assets/ exempt, always)'}\n`);
  process.stderr.write(`  freshness: ${options.skipFreshness ? 'SKIPPED (--skip-freshness)' : `${freshnessChecked} package(s) checked, dist/ is not older than src/`}\n`);
  process.stderr.write(`  engine ledger: ${ledgerChecked} committed file(s) verified against ports/lib/engine-artifacts.json\n`);
  process.stderr.write(`  tar      ${String(reported.tarSize).padStart(6)} B uncompressed  sha256 ${reported.tarSha256}\n`);
  process.stderr.write(`  ${String(size).padStart(6)} B  ${tgzSha256}  ${tarballName}\n`);
  process.stderr.write('  (compare tar sha256 across checkouts and machines; the .tgz sha256 is this toolchain\'s)\n');
  process.stderr.write(`make-package: wrote ${tarballPath}\n`);
}

if (options.json) {
  process.stdout.write(`${JSON.stringify({
    package: manifest.name,
    version: manifest.version,
    tarball: tarballName,
    path: tarballPath,
    bytes: size,
    sha256: tgzSha256,
    // The cross-toolchain invariant. A `tarSha256` match with a differing `sha256` means the
    // compressor or the tar implementation differs, not that the content changed.
    tarSha256: reported.tarSha256,
    tarBytes: reported.tarSize,
    fileCount: stats.count,
    stagedBytes: stats.bytes,
    vendored,
    notices: shippedNotices,
    vendoredNotices,
    // How many staged text files this run had to rewrite from the checkout's CRLF to the
    // repository's LF. Non-zero on a Windows worktree, zero in CI; the archive is the same either
    // way, which is the point. A non-zero value on CI means a CRLF file got committed.
    normalizedLineEndings,
    // How many packages were compared before staging: the one under test plus every package in the
    // vendored closure. It is the number that says the guard looked at the whole input and not just
    // the package named on the command line, which is what an aggregate's bytes actually depend on.
    freshness: { checked: freshnessChecked, skipped: options.skipFreshness },
    // How many ledger-pinned engine files are committed in this repository and were checked against
    // `ports/lib/engine-artifacts.json` before staging. On this repository it is 3 (the ons trio);
    // it is 0 in a checkout that has no committed engine bytes, which is the case worth noticing,
    // because a packer that checked nothing must not read as a packer that checked everything.
    ledgerChecked,
    sourceDateEpoch: options.epoch,
  })}\n`);
} else if (options.quiet) {
  process.stdout.write(`${tarballPath}\n`);
}
