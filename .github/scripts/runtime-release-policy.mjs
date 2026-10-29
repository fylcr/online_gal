#!/usr/bin/env node
/**
 * Pure policy for ONS/KRKR runtime releases.
 *
 * The engine workflows call this before any native build. It decides the SemVer,
 * whether the exact push range contains that engine's release inputs, and whether
 * the exact tag/release/asset identity already exists. It never calls GitHub and
 * never writes a release. Callers pass versions, SHAs and paths as argv, not as
 * text spliced into a shell.
 *
 * Exit codes: 0 allow · 1 usage · 2 policy rejection · 3 history or Git failure.
 */
import { execFileSync } from 'node:child_process';
import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

export const NUMERIC = '0|[1-9]\\d*';
export const PRE_ID = `(?:${NUMERIC}|[0-9A-Za-z-]*[A-Za-z-][0-9A-Za-z-]*)`;
export const SEMVER_SOURCE =
  `^(${NUMERIC})\\.(${NUMERIC})\\.(${NUMERIC})` +
  `(?:-((?:${PRE_ID})(?:\\.(?:${PRE_ID}))*))?` +
  `(?:\\+([0-9A-Za-z-]+(?:\\.[0-9A-Za-z-]+)*))?$`;

const SEMVER = new RegExp(SEMVER_SOURCE);
const SHA = /^[0-9a-f]{40}$/;
const ZERO = '0'.repeat(40);

export const RELEASE_INPUTS = {
  ons: [
    'packages/engine/core/onscripter-yuri/src/**',
    'packages/engine/core/onscripter-yuri/scripts/**',
    'packages/engine/core/onscripter-yuri/package.json',
    'packages/engine/core/onscripter-yuri/tsconfig.json',
    'packages/engine/interpreter/onscripter/src/**',
    'packages/engine/interpreter/onscripter/package.json',
    'packages/engine/interpreter/onscripter/tsconfig.json',
  ],
  krkr: [
    'packages/engine/core/kirikiroid2-web/src/**',
    'packages/engine/core/kirikiroid2-web/package.json',
    'packages/engine/core/kirikiroid2-web/tsconfig.json',
    'packages/engine/interpreter/kirikiri-kag3/src/**',
    'packages/engine/interpreter/kirikiri-kag3/package.json',
    'packages/engine/interpreter/kirikiri-kag3/tsconfig.json',
  ],
};

const PACKAGE_DIR = {
  ons: 'packages/engine/core/onscripter-yuri',
  krkr: 'packages/engine/core/kirikiroid2-web',
};

export function isSemVer(value) {
  return typeof value === 'string' && SEMVER.test(value);
}

export function semverCore(value) {
  const match = SEMVER.exec(value);
  if (!match) return null;
  return `${match[1]}.${match[2]}.${match[3]}`;
}

/**
 * SemVer precedence. Build metadata is ignored, so 1.0.0+a and 1.0.0+b compare equal
 * while remaining distinct identities. A prerelease is less than its release.
 */
export function compareSemVer(left, right) {
  const a = SEMVER.exec(left);
  const b = SEMVER.exec(right);
  if (!a || !b) throw new Error('compareSemVer requires two SemVer strings');
  for (let index = 1; index <= 3; index += 1) {
    const delta = Number(a[index]) - Number(b[index]);
    if (delta !== 0) return delta < 0 ? -1 : 1;
  }
  if (a[4] === undefined && b[4] === undefined) return 0;
  if (a[4] === undefined) return 1;
  if (b[4] === undefined) return -1;
  const leftIds = a[4].split('.');
  const rightIds = b[4].split('.');
  const length = Math.min(leftIds.length, rightIds.length);
  for (let index = 0; index < length; index += 1) {
    const x = leftIds[index];
    const y = rightIds[index];
    if (x === y) continue;
    const xNum = /^\d+$/.test(x);
    const yNum = /^\d+$/.test(y);
    if (xNum && yNum) return Number(x) < Number(y) ? -1 : 1;
    if (xNum) return -1;
    if (yNum) return 1;
    return x < y ? -1 : 1;
  }
  if (leftIds.length === rightIds.length) return 0;
  return leftIds.length < rightIds.length ? -1 : 1;
}

export function formatShanghai(clock) {
  const date = clock instanceof Date ? clock : new Date(clock);
  if (Number.isNaN(date.getTime())) throw new Error('build clock is not a valid time');
  const parts = new Intl.DateTimeFormat('en-US', {
    timeZone: 'Asia/Shanghai',
    year: 'numeric',
    month: '2-digit',
    day: '2-digit',
    hour: '2-digit',
    minute: '2-digit',
    hourCycle: 'h23',
  }).formatToParts(date);
  const pick = (type) => parts.find((part) => part.type === type).value;
  return `${pick('year')}${pick('month')}${pick('day')}${pick('hour')}${pick('minute')}`;
}

export function automaticVersion(packageVersion, clock) {
  const core = semverCore(packageVersion);
  if (!core) throw new Error(`tracked package version is not SemVer: ${packageVersion}`);
  const stamp = formatShanghai(clock);
  if (!/^\d{12}$/.test(stamp)) throw new Error(`Shanghai stamp is not 12 digits: ${stamp}`);
  const version = `${core}-beta-${stamp}`;
  if (!isSemVer(version)) throw new Error(`generated version is not SemVer: ${version}`);
  return version;
}

export function encodePathSegment(value) {
  return encodeURIComponent(value).replaceAll('+', '%2B');
}

export function releaseIdentity(engine, version) {
  if (engine !== 'ons' && engine !== 'krkr') throw new Error(`not an immutable engine: ${engine}`);
  if (!isSemVer(version)) throw new Error(`version is not SemVer: ${version}`);
  const tag = `engine-${engine}-${version}`;
  const packedPackage = engine === 'ons'
    ? '@online-gal/engine-core-onscripter-yuri' : '@online-gal/engine-core-kirikiroid2-web';
  // Same flattening as make-package.mjs: strip one leading @, then turn / into -.
  const tarball = `${packedPackage.replace(/^@/, '').replace(/\//g, '-')}-${version}.tgz`;
  return {
    version,
    tag,
    packageVersion: version,
    tarball,
    manifestVersion: version,
    downloadTag: encodePathSegment(tag),
    downloadTarball: encodePathSegment(tarball),
  };
}

export function gitCheckRefFormat(tag, execFile = execFileSync) {
  try {
    execFile('git', ['check-ref-format', '--allow-onelevel', `refs/tags/${tag}`], { stdio: 'pipe' });
    return true;
  } catch {
    return false;
  }
}

function globToRegExp(glob) {
  let source = '^';
  for (let index = 0; index < glob.length; index += 1) {
    const char = glob[index];
    if (char === '*' && glob[index + 1] === '*') {
      source += '.*';
      index += 1;
      if (glob[index + 1] === '/') index += 1;
    } else if (char === '*') source += '[^/]*';
    else source += char.replace(/[|\\{}()[\]^$+?.]/g, '\\$&');
  }
  return new RegExp(`${source}$`);
}

export function matchesReleaseInput(engine, file) {
  const normalized = String(file).replaceAll('\\', '/');
  return RELEASE_INPUTS[engine].some((glob) => globToRegExp(glob).test(normalized));
}

export function classifyPaths(files) {
  const list = [...files];
  return {
    ons: list.some((file) => matchesReleaseInput('ons', file)) ? 1 : 0,
    krkr: list.some((file) => matchesReleaseInput('krkr', file)) ? 1 : 0,
    files: list,
  };
}

function assertSha(name, value) {
  if (typeof value !== 'string' || !SHA.test(value)) {
    const error = new Error(`${name} is not a full 40-hex SHA`);
    error.code = 3;
    throw error;
  }
}

export function changedPaths(before, after, repo, execFile = execFileSync) {
  assertSha('before', before);
  assertSha('after', after);
  if (before === ZERO) {
    const error = new Error('before is the all-zero SHA; refusing to guess the range');
    error.code = 3;
    throw error;
  }
  let output;
  try {
    output = execFile(
      'git',
      ['diff', '--name-status', '--find-renames', '-z', before, after],
      { cwd: repo, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] },
    );
  } catch (cause) {
    const error = new Error(`git diff ${before}..${after} failed: ${cause.message}`);
    error.code = 3;
    throw error;
  }
  const records = output.split('\0').filter((part) => part !== '');
  const files = [];
  for (let index = 0; index < records.length; index += 1) {
    const status = records[index];
    if (status.startsWith('R') || status.startsWith('C')) {
      files.push(records[index + 1], records[index + 2]);
      index += 2;
    } else {
      files.push(records[index + 1]);
      index += 1;
    }
  }
  return files.filter((file) => file !== undefined);
}

function readPackageVersion(engine, repo) {
  const path = resolve(repo, PACKAGE_DIR[engine], 'package.json');
  let manifest;
  try {
    manifest = JSON.parse(readFileSync(path, 'utf8'));
  } catch (cause) {
    const error = new Error(`cannot read ${path}: ${cause.message}`);
    error.code = 3;
    throw error;
  }
  return manifest.version;
}

/**
 * Collision is exact string identity. The exact tag and release endpoints answer
 * 404 for absence. The paginated release list does not: it answers 200 with an
 * empty array when the repository has no releases, so a non-200 history row --
 * including 404 -- is an unknown query (missing repository or missing scope),
 * not an empty history, and fails closed. Every other non-200 status, a missing
 * status, or a thrown lookup is likewise unknown and fail-closed.
 */
export function collisionHits(version, inventory, engine) {
  const hits = [];
  const scoped = engine === 'ons' || engine === 'krkr';
  const tag = scoped ? `engine-${engine}-${version}` : null;
  const tarball = scoped ? releaseIdentity(engine, version).tarball : null;
  for (const kind of ['tag', 'release', 'asset', 'history']) {
    const rows = inventory[kind] ?? [];
    for (const row of rows) {
      if (kind === 'history') {
        if (row.status !== 200) {
          const error = new Error(`history lookup is not a definitive 200 (status ${row.status})`);
          error.code = 2;
          throw error;
        }
      } else if (row.status !== undefined && row.status !== 200 && row.status !== 404) {
        const error = new Error(`${kind} lookup is not a definitive 200 or 404 (status ${row.status})`);
        error.code = 2;
        throw error;
      }
      if (row.status === 404) continue;
      const names = [row.name, row.asset].filter((value) => value !== undefined);
      const scopedHit = scoped && (row.tag === tag || row.version === version || names.includes(tarball));
      const legacyHit = !scoped && (
        row.version === version || row.tag === `engine-ons-${version}` || row.tag === `engine-krkr-${version}`
        || names.includes(version) || names.includes(`engine-ons-${version}`) || names.includes(`engine-krkr-${version}`)
      );
      if (scopedHit || legacyHit) hits.push(kind);
    }
  }
  return [...new Set(hits)];
}

export function planPublication({ engine, mode, publish, version, clock, before, after, repo, inventory, execFile }) {
  if (engine !== 'ons' && engine !== 'krkr') throw Object.assign(new Error('engine must be ons or krkr'), { code: 1 });
  if (mode !== 'push' && mode !== 'manual') throw Object.assign(new Error('mode must be push or manual'), { code: 1 });
  const git = execFile ?? execFileSync;
  let files = [];
  if (mode === 'push') {
    files = changedPaths(before, after, repo, git);
    if (!classifyPaths(files)[engine]) {
      return { action: 'skip', build: 0, release: 0, files };
    }
  } else if (publish !== true) {
    return { action: 'reject', build: 0, release: 0, reason: 'manual publish is not explicitly true' };
  }

  let selected = version;
  if (mode === 'push') selected = automaticVersion(readPackageVersion(engine, repo), clock);
  else if (typeof selected !== 'string' || selected.trim() === '' || selected !== selected.trim() || !isSemVer(selected)) {
    return { action: 'reject', build: 0, release: 0, reason: 'manual version is missing or not SemVer' };
  }

  const identity = releaseIdentity(engine, selected);
  if (!gitCheckRefFormat(identity.tag, git)) {
    return { action: 'reject', build: 0, release: 0, reason: 'tag failed git check-ref-format', version: selected };
  }
  let hits;
  try {
    hits = collisionHits(selected, inventory ?? { tag: [], release: [], asset: [], history: [] }, engine);
  } catch (cause) {
    return { action: 'reject', build: 0, release: 0, reason: cause.message, version: selected };
  }
  if (hits.length) {
    return { action: 'reject', build: 0, release: 0, reason: `exact identity exists: ${hits.join(',')}`, version: selected };
  }
  return { action: 'build', build: 1, release: 1, version: selected, identity, files };
}

function argValue(argv, name) {
  const index = argv.indexOf(name);
  if (index < 0 || argv[index + 1] === undefined) return null;
  return argv[index + 1];
}

function fail(code, message) {
  process.stderr.write(`runtime-release-policy: ${message}\n`);
  process.exit(code);
}

function main() {
  const argv = process.argv.slice(2);
  if (argv.includes('--help') || argv.includes('-h') || argv[0] === undefined) {
    process.stdout.write(`usage: node .github/scripts/runtime-release-policy.mjs <classify|version|preflight>
  classify needs --files-json <json-array>
  version needs --engine ons|krkr --package-version <semver> --clock <iso>
  preflight needs --engine --mode push|manual [--publish true|false] [--version V]
                  [--clock iso] [--before sha] [--after sha] [--repo dir] [--inventory-json file]
`);
    process.exit(argv[0] === undefined ? 1 : 0);
  }
  const command = argv[0];
  try {
    if (command === 'classify') {
      const parsed = JSON.parse(argValue(argv, '--files-json') ?? 'null');
      if (!Array.isArray(parsed)) fail(1, '--files-json must be an array');
      process.stdout.write(`${JSON.stringify(classifyPaths(parsed))}\n`);
    } else if (command === 'version') {
      process.stdout.write(`${automaticVersion(argValue(argv, '--package-version'), argValue(argv, '--clock'))}\n`);
    } else if (command === 'preflight') {
      const publishText = argValue(argv, '--publish');
      const inventoryPath = argValue(argv, '--inventory-json');
      const result = planPublication({
        engine: argValue(argv, '--engine'),
        mode: argValue(argv, '--mode'),
        publish: publishText === 'true' ? true : publishText === 'false' ? false : publishText,
        version: argValue(argv, '--version') ?? undefined,
        clock: argValue(argv, '--clock') ?? undefined,
        before: argValue(argv, '--before') ?? undefined,
        after: argValue(argv, '--after') ?? undefined,
        repo: argValue(argv, '--repo') ?? process.cwd(),
        inventory: inventoryPath ? JSON.parse(readFileSync(inventoryPath, 'utf8')) : { tag: [], release: [], asset: [], history: [] },
      });
      process.stdout.write(`${JSON.stringify(result)}\n`);
      if (result.action === 'reject') process.exit(2);
    } else fail(1, `unknown command ${command}`);
  } catch (cause) {
    fail(cause.code === 3 ? 3 : 1, cause.message);
  }
}

const invoked = process.argv[1] && import.meta.url === pathToFileURL(resolve(process.argv[1])).href;
if (invoked) main();
