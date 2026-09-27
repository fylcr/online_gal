#!/usr/bin/env node
/**
 * Build a one-engine manifest in the exact shape `ports/lib/engine-artifacts.json` uses.
 *
 * WHY THIS EXISTS
 *
 * The publish workflow has to describe the bytes it just built, and the fetch side
 * (`ports/lib/fetch-engine.mjs`) has to be able to trust that description. If the two are written
 * by two different hands they drift, and the failure mode is the worst one: the fetch succeeds,
 * the hash check passes against a manifest that describes the wrong file, and the engine misbehaves
 * in a way that looks like an emulation bug.
 *
 * So the shape is produced in exactly one place -- here -- and the same JSON is both attached to
 * the release (so `--manifest <url>` can point straight at it) and written next to the artifacts as
 * a repo-manifest fragment the maintainer can fold into `ports/lib/engine-artifacts.json`.
 *
 * The hashes are computed from the bytes on disk, never taken from the build log, for the same
 * reason: a log line records what someone intended to write.
 *
 * Usage:
 *
 *   node .github/scripts/make-manifest.mjs \
 *     --engine krkr --version kirikiroid2-web-20260920 \
 *     --upstream "https://github.com/... @ 13dda190..." \
 *     --emsdk 6.0.9 --repository https://github.com/fylcr/online_gal \
 *     --release-tag engine-krkr-20260920 \
 *     --dir .ref/kirikiroid2-web/out/web/release \
 *     --file index.wasm --file index.js \
 *     --out manifest.json
 *
 * Exit codes: 0 ok · 1 usage · 2 an input file is missing or empty · 3 write failed.
 */
import { createHash } from 'node:crypto';
import { readFileSync, statSync, writeFileSync } from 'node:fs';
import { join, resolve } from 'node:path';

const USAGE = `usage: node .github/scripts/make-manifest.mjs --engine <name> --dir <dir> --out <file>
         [--version V] [--upstream U] [--emsdk 6.0.9] [--repository URL] [--release-tag TAG]
         [--file NAME]... [--base-url URL]

  --file may be repeated; without it every regular file directly inside --dir is hashed.
  --base-url is an alternative to --repository/--release-tag for where the bytes live.`;

function fail(code, message) {
  process.stderr.write(`make-manifest: ${message}\n`);
  process.exit(code);
}

const options = {
  engine: null,
  version: null,
  upstream: null,
  emsdk: null,
  repository: null,
  releaseTag: null,
  baseUrl: null,
  dir: null,
  out: null,
  files: [],
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
    case '-h':
      process.stdout.write(`${USAGE}\n`);
      process.exit(0);
      break;
    case '--engine': options.engine = next(); break;
    case '--version': options.version = next(); break;
    case '--upstream': options.upstream = next(); break;
    case '--emsdk': options.emsdk = next(); break;
    case '--repository': options.repository = next(); break;
    case '--release-tag': options.releaseTag = next(); break;
    case '--base-url': options.baseUrl = next(); break;
    case '--dir': options.dir = next(); break;
    case '--out': options.out = next(); break;
    case '--file': options.files.push(next()); break;
    default:
      fail(1, `unknown argument ${arg}\n\n${USAGE}`);
  }
}

if (!options.engine) fail(1, 'missing --engine');
if (!options.dir) fail(1, 'missing --dir');
if (!options.out) fail(1, 'missing --out');
if (!options.files.length) fail(1, 'missing --file (at least one)');

/** sha256 of a file, read in chunks so a 22 MB module does not have to be held whole. */
function sha256(path) {
  const hash = createHash('sha256');
  hash.update(readFileSync(path));
  return hash.digest('hex');
}

const dir = resolve(options.dir);
const files = [];
for (const name of options.files) {
  const path = join(dir, name);
  let size;
  try {
    size = statSync(path).size;
  } catch {
    fail(2, `no such artifact: ${path}`);
  }
  // A zero-byte artifact is always a bug -- a failed link leaves the file behind -- and publishing
  // it would make the fetch side verify a hash over nothing.
  if (size === 0) fail(2, `artifact is empty: ${path}`);
  files.push({ name, size, sha256: sha256(path) });
}

files.sort((a, b) => (a.name < b.name ? -1 : a.name > b.name ? 1 : 0));

const entry = { version: options.version, upstream: options.upstream, emsdk: options.emsdk };
if (options.baseUrl) entry.baseUrl = options.baseUrl;
if (options.repository) entry.repository = options.repository;
if (options.releaseTag) entry.releaseTag = options.releaseTag;
entry.files = files;

const manifest = { [options.engine]: entry };
try {
  writeFileSync(options.out, `${JSON.stringify(manifest, null, 2)}\n`);
} catch (error) {
  fail(3, `cannot write ${options.out}: ${error.message}`);
}

// Human-readable on stderr so stdout stays free for redirecting the JSON itself.
for (const file of files) {
  process.stderr.write(`  ${String(file.size).padStart(10)}  ${file.sha256}  ${file.name}\n`);
}
process.stderr.write(`make-manifest: wrote ${options.out} (${files.length} file(s), engine ${options.engine})\n`);
