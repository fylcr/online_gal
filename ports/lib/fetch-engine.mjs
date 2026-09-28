#!/usr/bin/env node
/**
 * Fetches a published engine binary, and refuses to put it in place until it is verified.
 *
 * WHY THIS EXISTS
 *
 * Compiling an engine needs an Emscripten SDK, an upstream checkout that is deliberately not
 * committed, and twenty minutes. Someone who only wants to run the emulator should need none of
 * that, and a web deploy should not rebuild the same wasm on every push. Both are solved the same
 * way: the CI publishes the built binaries once, and everything else downloads them.
 *
 * That introduces exactly one new risk, and it is the reason this file verifies rather than
 * trusts. A downloaded binary that is not hash-checked is worse than no cache at all: the engine
 * loads, misbehaves in a way that looks like an emulation bug, and the real cause — a truncated
 * download, a stale release, an artifact built from the wrong commit — is invisible. So the sha256
 * is checked against the manifest before the file is renamed into place, and a partially written
 * download is never left where a later build would pick it up.
 *
 * Usage:
 *
 *     node ports/lib/fetch-engine.mjs <engine> [--out <dir>] [--force] [--manifest <path-or-url>] [--json]
 *
 * `<engine>` is one of `ons` or `krkr`. The manifest is `ports/lib/engine-artifacts.json`
 * unless `--manifest` names another file or an http(s) URL (the CI half publishes one, and pointing
 * at it is how a release changes where its bytes live without a code change).
 *
 * Exit codes, because CI and the shell wrappers branch on them:
 *
 *     0  the artifact is in place and verified — either it already was, or it was just downloaded
 *     1  bad usage (unknown engine, unknown flag, missing argument)
 *     2  the artifact is unavailable — not present locally and not downloadable
 *     3  the artifact failed verification (size or sha256 mismatch) — nothing was left in place
 *     4  the manifest is missing, unreadable, or has no entry for this engine
 *
 * Nothing here is a dependency: `node:fs`, `node:crypto` and the global `fetch` are enough, and
 * keeping it that way is what lets a runner and a dev machine on Git Bash run the same file.
 */
import { createHash } from 'node:crypto';
import { createReadStream, createWriteStream, existsSync, mkdirSync, readFileSync, renameSync, rmSync, statSync } from 'node:fs';
import { Readable } from 'node:stream';
import { pipeline } from 'node:stream/promises';
import { join, resolve } from 'node:path';

import { repoRoot } from './ref.mjs';

/**
 * Where each engine's binaries go when the manifest does not say.
 *
 * These are the paths the consumers already read, so "fetch" means "put it where the build would
 * have put it" and no consumer needs a second code path. Two of the three are gitignored (the
 * `out/` directories and `.ref/`), which is deliberate: a downloaded binary is a build product and
 * must never be committed by accident. The ONS one is the exception — those assets are tracked, so
 * a fetch there rewrites a tracked file and is meant to be reviewed like any other change.
 */
const ENGINES = {
  ons: {
    dest: 'packages/runtime-ons/assets',
    fromSource: 'bash ports/ons/build/fetch-web-engine.sh --from-source',
  },
  krkr: {
    dest: '.ref/kirikiroid2-web/out/web/release',
    fromSource: 'bash ports/krkr/kirikiroid2/run-build.sh',
  },
  // `artemis` used to be here, pointing at the `web-android` build that task-12 retired. Keeping it
  // would have advertised an engine this tool can only fail on: with the entry gone from
  // `engine-artifacts.json`, `fetch-engine.mjs artemis` reached the manifest and exited 4 ("no
  // usable entry"). Dropping the entry too makes the failure honest and immediate -- it is now a
  // usage error naming the two engines that exist, exit 1. Artemis' wasm comes from
  // `build-wasm-artemis-bridge.yml` instead, which is a rebase of upstream, not a fetch.
};

const DEFAULT_MANIFEST = 'ports/lib/engine-artifacts.json';

/** Sent as `User-Agent`; GitHub's asset endpoint is friendlier with one, and a proxy can tell who called. */
const USER_AGENT = 'online-gal/fetch-engine';

/**
 * Token for a release that is not publicly readable.
 *
 * This repository is **private**, and GitHub serves a private repository's release assets only to an
 * authenticated caller: the plain `releases/download/...` URL answers 404 even when the release
 * exists, which is indistinguishable from "not published yet" unless something tries the
 * authenticated route. So when a token is present the download falls back to the API asset endpoint,
 * which is the documented way to fetch a private asset.
 *
 * Read from the environment rather than a flag: a token on a command line lands in the process table,
 * in shell history, and in CI logs that echo the command. `GITHUB_TOKEN` and `GH_TOKEN` are the two
 * names a GitHub runner and the `gh` CLI already use, so a caller usually sets neither deliberately.
 *
 * It stays optional. A public release needs nothing, and a token only ever *adds* a route: the plain
 * URL is always tried first, so nothing about the public case changes.
 */
const TOKEN = (process.env.GITHUB_TOKEN || process.env.GH_TOKEN || '').trim();

/** Auth headers, empty when no token is configured. */
function authHeaders() {
  return TOKEN ? { authorization: `Bearer ${TOKEN}` } : {};
}

/**
 * Matches a browser-facing release asset URL and pulls it apart.
 *
 * Only `github.com/<owner>/<repo>/releases/download/<tag>/<name>` is recognised; every other URL
 * (a `baseUrl` on a static host, a `file.url`) is fetched as-is, because those are whatever the
 * publisher chose and there is no API to translate them into.
 */
function parseReleaseUrl(url) {
  const match = /^https?:\/\/github\.com\/([^/]+)\/([^/]+)\/releases\/download\/([^/]+)\/(.+)$/.exec(url);
  if (!match) return null;
  return { owner: match[1], repo: match[2], tag: decodeURIComponent(match[3]), name: decodeURIComponent(match[4]) };
}

/**
 * The API URL for one asset of a release, plus the `Accept` header that returns its bytes.
 *
 * Returns bytes rather than JSON metadata: `assets[].url` with `Accept: application/octet-stream`
 * is the one documented way to read a private asset's contents, and it is also stable against an
 * asset being re-uploaded, since the lookup is by name at download time rather than by an id baked
 * into the manifest.
 */
async function resolvePrivateAsset(url, signal) {
  const parts = parseReleaseUrl(url);
  if (!parts || !TOKEN) return null;
  const api = `https://api.github.com/repos/${parts.owner}/${parts.repo}/releases/tags/${encodeURIComponent(parts.tag)}`;
  let response;
  try {
    response = await fetch(api, {
      redirect: 'follow',
      headers: { 'user-agent': USER_AGENT, accept: 'application/vnd.github+json', ...authHeaders() },
      signal,
    });
  } catch (cause) {
    throw new UnavailableError(`could not list the release ${parts.tag}: ${describeCause(cause)}`);
  }
  if (!response.ok) {
    throw new UnavailableError(
      `could not list the release ${parts.tag}: HTTP ${response.status}` +
        (response.status === 404 ? ' (the release or the token is not valid for this repository)' : ''),
    );
  }
  const release = await response.json();
  const asset = (release.assets ?? []).find((a) => a.name === parts.name);
  if (!asset) {
    throw new UnavailableError(
      `release ${parts.tag} has no asset named ${parts.name} ` +
        `(it has: ${(release.assets ?? []).map((a) => a.name).join(', ') || 'none'})`,
    );
  }
  return {
    url: `https://api.github.com/repos/${parts.owner}/${parts.repo}/releases/assets/${asset.id}`,
    headers: { accept: 'application/octet-stream' },
  };
}

const MANIFEST_TIMEOUT_MS = 30_000;
const FILE_TIMEOUT_MS = 300_000;

const USAGE = `usage: node ports/lib/fetch-engine.mjs <engine> [options]

  <engine>              one of: ${Object.keys(ENGINES).join(', ')}

  --out <dir>           place the files in <dir> instead of the engine's usual directory
  --force               download even when a verified copy is already present
  --manifest <p|url>    read the manifest from this path or http(s) URL
                        (default: ${DEFAULT_MANIFEST})
  --json                print one JSON object on stdout; human output goes to stderr
  --help, -h            print this message

exit codes: 0 ok · 1 usage · 2 unavailable · 3 verification failed · 4 bad manifest`;

class UsageError extends Error {}
class UnavailableError extends Error {}
class ManifestError extends Error {}
class IntegrityError extends Error {}

/** A one-line-per-argument parser; the CLI is small enough that a library would only add surface. */
function parseArgs(argv) {
  const options = { engine: null, out: null, force: false, manifest: DEFAULT_MANIFEST, json: false, help: false };

  for (let index = 0; index < argv.length; index += 1) {
    const arg = argv[index];
    if (arg === '--help' || arg === '-h') {
      options.help = true;
    } else if (arg === '--force') {
      options.force = true;
    } else if (arg === '--json') {
      options.json = true;
    } else if (arg === '--out' || arg === '--manifest') {
      const value = argv[index + 1];
      if (value === undefined || value.startsWith('--')) throw new UsageError(`${arg} needs a value`);
      if (arg === '--out') options.out = value;
      else options.manifest = value;
      index += 1;
    } else if (arg.startsWith('-')) {
      throw new UsageError(`unknown option ${arg}`);
    } else if (options.engine === null) {
      options.engine = arg;
    } else {
      throw new UsageError(`unexpected argument ${arg}`);
    }
  }

  return options;
}

/**
 * Reads the manifest from a path or a URL.
 *
 * A URL is accepted rather than only a path because that is the seam the CI half needs: when a
 * release publishes its own manifest, pointing at it is how the bytes' location changes without
 * editing this repository.
 */
async function loadManifest(source) {
  if (/^https?:\/\//i.test(source)) {
    let response;
    try {
      response = await fetch(source, {
        redirect: 'follow',
        headers: { 'user-agent': USER_AGENT },
        signal: AbortSignal.timeout(MANIFEST_TIMEOUT_MS),
      });
    } catch (cause) {
      throw new ManifestError(`could not fetch the manifest from ${source}: ${describeCause(cause)}`);
    }
    if (!response.ok) throw new ManifestError(`manifest ${source} returned HTTP ${response.status}`);
    try {
      return JSON.parse(await response.text());
    } catch (cause) {
      throw new ManifestError(`manifest ${source} is not valid JSON: ${describeCause(cause)}`);
    }
  }

  const path = resolve(source);
  if (!existsSync(path)) {
    throw new ManifestError(
      `manifest not found at ${path}\n` +
        'it is generated by the publish workflow; for local use it is committed at ' +
        `${DEFAULT_MANIFEST}`,
    );
  }
  try {
    return JSON.parse(readFileSync(path, 'utf8'));
  } catch (cause) {
    throw new ManifestError(`manifest ${path} is not valid JSON: ${describeCause(cause)}`);
  }
}

/** `fetch` rejects with a bare `TypeError` on a dead network, which reads as a crash unless it is unwrapped. */
function describeCause(cause) {
  if (cause && cause.name === 'TimeoutError') return 'timed out';
  if (cause && cause.cause && cause.cause.code) return `${cause.message} (${cause.cause.code})`;
  return cause && cause.message ? cause.message : String(cause);
}

/**
 * The download URL for one file.
 *
 * Explicit `url` first, then `baseUrl` + name, then `repository`/`releaseTag` + name. The last form
 * is the convention this repository publishes under; all three exist so the manifest can be written
 * with as little repetition as the publisher finds convenient.
 */
function fileUrl(entry, file) {
  if (file.url) return file.url;
  if (entry.baseUrl) return `${String(entry.baseUrl).replace(/\/$/, '')}/${file.name}`;
  if (entry.repository && entry.releaseTag) {
    return `${String(entry.repository).replace(/\/$/, '')}/releases/download/${entry.releaseTag}/${file.name}`;
  }
  return null;
}

/** sha256 of a file as it exists on disk. */
async function sha256File(path) {
  const hash = createHash('sha256');
  for await (const chunk of createReadStream(path)) hash.update(chunk);
  return hash.digest('hex');
}

/**
 * Checks a path against the manifest entry.
 *
 * Size is compared before hashing so the common cases — absent, truncated, obviously wrong — do not
 * pay for reading a 22 MB file. The hash is what actually decides.
 */
async function inspect(path, file) {
  if (!existsSync(path)) return { state: 'absent' };
  const size = statSync(path).size;
  if (size !== file.size) return { state: 'mismatch', size, sha256: null };
  const sha256 = await sha256File(path);
  return sha256 === file.sha256 ? { state: 'ok', size, sha256 } : { state: 'mismatch', size, sha256 };
}

/**
 * Downloads one file to a temporary name and verifies **the bytes on disk**.
 *
 * Verification hashes the file after it is written rather than the stream in flight, and that is the
 * point: what is later renamed into place is exactly what was checked. Nothing is renamed here — see
 * `install` for why the download and the commit are separate steps.
 */
async function download(url, target, file) {
  const temporary = `${target}.part-${process.pid}-${Date.now()}`;
  try {
    let response;
    try {
      response = await fetch(url, {
        redirect: 'follow',
        headers: { 'user-agent': USER_AGENT, ...authHeaders() },
        signal: AbortSignal.timeout(FILE_TIMEOUT_MS),
      });
      // A private repository answers 404 on the browser-facing URL even for an asset that exists, so a
      // 404 while a token is configured means "try the API" rather than "not published". Any other
      // status, and every status when no token is set, is reported as it is.
      if (!response.ok && response.status === 404 && TOKEN) {
        const asset = await resolvePrivateAsset(url, AbortSignal.timeout(FILE_TIMEOUT_MS));
        if (asset) {
          response = await fetch(asset.url, {
            redirect: 'follow',
            headers: { 'user-agent': USER_AGENT, ...authHeaders(), ...asset.headers },
            signal: AbortSignal.timeout(FILE_TIMEOUT_MS),
          });
        }
      }
    } catch (cause) {
      throw new UnavailableError(`could not download ${file.name} from ${url}: ${describeCause(cause)}`);
    }
    if (!response.ok) {
      throw new UnavailableError(`could not download ${file.name} from ${url}: HTTP ${response.status}`);
    }
    if (!response.body) throw new UnavailableError(`could not download ${file.name} from ${url}: empty response`);

    await pipeline(Readable.fromWeb(response.body), createWriteStream(temporary));

    const size = statSync(temporary).size;
    const sha256 = await sha256File(temporary);
    if (size !== file.size || sha256 !== file.sha256) {
      throw new IntegrityError(
        `${file.name} failed verification: expected ${file.size} B sha256 ${file.sha256}, ` +
          `got ${size} B sha256 ${sha256}`,
      );
    }
    return { temporary, size, sha256 };
  } catch (cause) {
    rmSync(temporary, { force: true });
    throw cause;
  }
}

/**
 * Replaces the destination files with verified temporaries, all or nothing.
 *
 * An engine is a set, not a list: `onsyuri.js` and `onsyuri.wasm` are two halves of one module, and a
 * directory holding a new `.js` beside a stale `.wasm` loads and then fails in whatever the mismatched
 * pair happens to do — a failure that looks like an engine bug and is not one. So a partial fetch is
 * never left behind. Every file is downloaded and verified before any is moved into place, so the
 * observable states are "all previous" and "all new".
 *
 * Each old file is moved aside rather than deleted, so if a rename fails halfway the originals can be
 * put back; they are only unlinked once every rename has succeeded.
 */
function install(pending, dest) {
  const backups = [];
  const placed = [];
  let failure = null;
  try {
    for (const { file, temporary, size, sha256 } of pending) {
      const target = join(dest, file.name);
      const backup = `${target}.old-${process.pid}-${Date.now()}`;
      if (existsSync(target)) {
        renameSync(target, backup);
        backups.push({ backup, target });
      }
      // Windows refuses `rename` onto an existing file, and the old one has been moved aside above.
      renameSync(temporary, target);
      placed.push({ name: file.name, size, sha256, action: 'downloaded' });
    }
  } catch (cause) {
    failure = cause;
  }
  if (failure) {
    // Undo this half-finished replace: drop what was placed, put every original back.
    for (const { name } of placed) rmSync(join(dest, name), { force: true });
    for (const { backup, target } of backups) {
      if (existsSync(backup)) {
        rmSync(target, { force: true });
        renameSync(backup, target);
      }
    }
    throw failure;
  }
  // Success: the originals are no longer needed.
  for (const { backup } of backups) rmSync(backup, { force: true });
  return placed;
}

/** Removes this run's temporaries whether or not it succeeded. */
function discard(pending) {
  for (const { temporary } of pending) rmSync(temporary, { force: true });
}

async function run(options, log) {
  if (!(options.engine in ENGINES)) {
    throw new UsageError(`unknown engine ${JSON.stringify(options.engine)}; expected one of: ${Object.keys(ENGINES).join(', ')}`);
  }

  const engine = ENGINES[options.engine];
  const dest = options.out ? resolve(options.out) : join(repoRoot, engine.dest);
  const manifest = await loadManifest(options.manifest);
  const entry = manifest[options.engine];

  // Fails loudly rather than skipping the check: an engine with no manifest entry means the
  // publisher and the consumer disagree, and "assume it is fine" is the failure this file exists to
  // prevent. A missing `files` array is the same disagreement.
  if (!entry || !Array.isArray(entry.files) || entry.files.length === 0) {
    throw new ManifestError(
      `the manifest has no usable entry for ${options.engine}\n` +
        `expected: { "${options.engine}": { "version": "...", "upstream": "...", "emsdk": "...", "files": [ ... ] } }`,
    );
  }

  mkdirSync(dest, { recursive: true });

  const result = {
    engine: options.engine,
    version: entry.version ?? null,
    upstream: entry.upstream ?? null,
    emsdk: entry.emsdk ?? null,
    dest,
    status: 'present',
    files: [],
  };

  // Phase 1: download and verify everything, touching no destination file. A failure here leaves the
  // existing engine exactly as it was, which is the whole reason the two phases are separate.
  const pending = [];
  try {
    for (const file of entry.files) {
      const target = join(dest, file.name);
      const found = await inspect(target, file);

      if (found.state === 'ok' && !options.force) {
        log(`${file.name}: already present and verified (${file.size} B)`);
        result.files.push({ name: file.name, size: file.size, sha256: file.sha256, action: 'verified' });
        continue;
      }

      if (found.state === 'mismatch') {
        log(
          `${file.name}: present but does not match the manifest ` +
            `(${found.size} B, sha256 ${found.sha256 ?? 'n/a'}) — replacing it`,
        );
      }

      const url = fileUrl(entry, file);
      if (!url) {
        throw new ManifestError(
          `no download URL for ${file.name}: the manifest needs a per-file "url", a "baseUrl", ` +
            'or both "repository" and "releaseTag"',
        );
      }

      log(`${file.name}: downloading ${url}`);
      const { temporary, size, sha256 } = await download(url, target, file);
      pending.push({ file, temporary, size, sha256 });
    }

    // Phase 2: commit. Only reached when every file verified, so the destination moves from one
    // complete set to the next and never through a mixed one.
    if (pending.length) {
      const placed = install(pending, dest);
      for (const file of placed) {
        log(`${file.name}: verified sha256 ${file.sha256} -> ${join(dest, file.name)}`);
        result.files.push(file);
      }
      result.status = 'fetched';
    }
  } finally {
    discard(pending);
  }

  if (result.status === 'present') {
    log(`== ${options.engine} engine artifacts are in place and verified (${dest}) ==`);
  } else {
    log(`== ${options.engine} engine artifacts fetched and verified (${dest}) ==`);
  }
  return result;
}

/** Quiet exit: `process.exitCode` rather than `process.exit()`, so a piped stdout is not truncated. */
async function main() {
  const argv = process.argv.slice(2);
  let options;
  try {
    options = parseArgs(argv);
  } catch (cause) {
    process.stderr.write(`${cause.message}\n\n${USAGE}\n`);
    process.exitCode = 1;
    return;
  }

  if (options.help) {
    process.stdout.write(`${USAGE}\n`);
    return;
  }
  if (options.engine === null) {
    process.stderr.write(`missing <engine>\n\n${USAGE}\n`);
    process.exitCode = 1;
    return;
  }

  // In `--json` mode stdout carries only the object, so every human line goes to stderr instead.
  const log = (line) => {
    const stream = options.json ? process.stderr : process.stdout;
    stream.write(`${line}\n`);
  };

  try {
    const result = await run(options, log);
    if (options.json) process.stdout.write(`${JSON.stringify(result, null, 2)}\n`);
  } catch (cause) {
    const engine = options.engine;
    const hint = engine in ENGINES ? `\nbuild it from source instead: ${ENGINES[engine].fromSource}` : '';

    if (cause instanceof UsageError) {
      process.stderr.write(`${cause.message}\n\n${USAGE}\n`);
      process.exitCode = 1;
    } else if (cause instanceof UnavailableError) {
      process.stderr.write(`${cause.message}${hint}\n`);
      process.exitCode = 2;
    } else if (cause instanceof IntegrityError) {
      process.stderr.write(
        `${cause.message}\nthe engine was left as it was: no file was replaced\n`,
      );
      process.exitCode = 3;
    } else if (cause instanceof ManifestError) {
      process.stderr.write(`${cause.message}\n`);
      process.exitCode = 4;
    } else {
      // Anything unexpected still leaves a one-line summary rather than a bare stack trace, but the
      // stack goes to stderr so the cause is not lost.
      process.stderr.write(`fetch-engine failed: ${cause && cause.message ? cause.message : cause}${hint}\n`);
      if (cause && cause.stack) process.stderr.write(`${cause.stack}\n`);
      process.exitCode = 2;
    }
  }
}

await main();
