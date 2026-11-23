#!/usr/bin/env node
/**
 * Turns a directory into a plugin package: lists every file with its size and
 * SHA-256 and writes `mf-manifest.json` (the content manifest) and a slim
 * `plugin.json` that only points at it (see `src/format.ts`, format 2).
 *
 *   node pack-plugin.mjs <dir> --manifest <manifest.json> [--version <semver>] [--lazy <glob>]... [--bundle <path>] [--out <dir>]
 *
 * `--lazy` marks files the kernel fetches on first request instead of at install
 * time, for example `engines/**` (`**` crosses directories, `*` stays inside one).
 * A `!` prefix keeps matching files eager (`--lazy '**' --lazy '!index.html'`).
 * At least one file must stay eager.
 *
 * Third-party authors run it at the root of their repository, commit the
 * resulting `plugin.json` and `mf-manifest.json`, and push a tag; users install
 * `github:<owner>/<repo>@<tag>`. The official site build uses `packDirectory`
 * directly (see `build-site.mjs`).
 *
 * Needs `tsc -b packages/plugin-system` first: the package is validated with
 * the same `parsePackage` the kernel runs, so a package that packs is a
 * package the kernel accepts.
 */
import { createHash } from 'node:crypto';
import { mkdir, readdir, readFile, stat, writeFile } from 'node:fs/promises';
import { join, relative, resolve, sep } from 'node:path';
import { pathToFileURL } from 'node:url';

import {
  BUNDLE_FILE,
  ENVELOPE_FORMAT,
  MANIFEST_FILE,
  PACKAGE_FILE,
  PACKAGE_FORMAT,
  parseEnvelope,
  parsePackage,
  safeRelativePath,
} from '../dist/index.js';

/** Never part of a package: VCS and editor clutter, dependencies, the package file itself. */
function skipped(rel) {
  const first = rel.split('/')[0];
  const generated = rel === PACKAGE_FILE || rel === MANIFEST_FILE || rel === BUNDLE_FILE;
  return generated || first === 'node_modules' || rel.split('/').some((part) => part.startsWith('.'));
}

async function walk(root, dir = root, out = []) {
  for (const entry of await readdir(dir, { withFileTypes: true })) {
    const abs = join(dir, entry.name);
    const rel = relative(root, abs).split(sep).join('/');
    if (skipped(rel)) continue;
    if (entry.isDirectory()) await walk(root, abs, out);
    else if (entry.isFile()) out.push({ rel, abs });
  }
  return out;
}

const BACKSLASH = String.fromCharCode(92);

/** `**` crosses directories, `*` stays inside one, everything else matches itself. */
function globToRegExp(glob) {
  let out = '';
  for (let i = 0; i < glob.length; i++) {
    const ch = glob[i];
    if (ch === '*' && glob[i + 1] === '*') {
      i++;
      if (glob[i + 1] === '/') {
        i++;
        out += '(?:.*/)?';
      } else out += '.*';
    } else if (ch === '*') out += '[^/]*';
    else out += /[\w/-]/.test(ch) ? ch : BACKSLASH + ch;
  }
  return new RegExp(`^${out}$`);
}

/**
 * @param {{ dir: string, manifest: object, lazy?: string[], bundle?: string }} input `lazy` lists globs of files fetched on first request
 * @returns {Promise<{ pkg: object, bytes: Buffer, envelope: Buffer, digest: string, files: Array<{ path: string, abs: string, size: number, sha256: string, lazy?: true }> }>}
 */
export async function packDirectory({ dir, manifest, lazy = [], bundle }) {
  // A `!glob` entry keeps matching files eager; any other entry makes them lazy.
  const lazyPatterns = lazy.filter((glob) => !glob.startsWith('!')).map(globToRegExp);
  const eagerPatterns = lazy.filter((glob) => glob.startsWith('!')).map((glob) => globToRegExp(glob.slice(1)));
  const isLazy = (rel) => lazyPatterns.some((pattern) => pattern.test(rel)) && !eagerPatterns.some((pattern) => pattern.test(rel));
  const root = resolve(dir);
  const entries = (await walk(root)).sort((a, b) => (a.rel < b.rel ? -1 : a.rel > b.rel ? 1 : 0));
  const files = [];
  for (const { rel, abs } of entries) {
    if (safeRelativePath(rel) === null) {
      throw new Error(`"${rel}" cannot be packed: plugin paths may not contain \\ ? # % : * " < > | or control characters`);
    }
    const data = await readFile(abs);
    const sha256 = createHash('sha256').update(data).digest('hex');
    files.push({ path: rel, abs, size: data.byteLength, sha256, ...(isLazy(rel) ? { lazy: true } : {}) });
  }
  const pkg = {
    format: PACKAGE_FORMAT,
    manifest,
    files: files.map(({ path, size, sha256, lazy: isLazy }) => ({ path, size, sha256, ...(isLazy ? { lazy: true } : {}) })),
  };
  parsePackage(pkg);
  const bytes = Buffer.from(`${JSON.stringify(pkg, null, 1)}\n`);
  // `bytes` is mf-manifest.json and `digest` is its SHA-256, the value the official registry records.
  const envelope = Buffer.from(`${JSON.stringify({ format: ENVELOPE_FORMAT, manifest: MANIFEST_FILE, ...(bundle === undefined ? {} : { bundle }) }, null, 1)}\n`);
  parseEnvelope(JSON.parse(envelope.toString('utf8')));
  return { pkg, bytes, envelope, digest: createHash('sha256').update(bytes).digest('hex'), files };
}

/** Hashes the payload file list for the version suffix. */
export function fileListHash(files) {
  const hash = createHash('sha256');
  for (const file of files) hash.update(`${file.path}\0${file.sha256}${file.lazy ? '\0lazy' : ''}\n`);
  return hash.digest('hex');
}

async function main(argv) {
  const args = { _: [] };
  for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    if (arg === '--lazy') args.lazy = [...(args.lazy ?? []), argv[++i]];
    else if (arg.startsWith('--')) args[arg.slice(2)] = argv[++i];
    else args._.push(arg);
  }
  const dir = args._[0];
  if (dir === undefined || args.manifest === undefined) {
    console.error('usage: pack-plugin.mjs <dir> --manifest <manifest.json> [--version <semver>] [--lazy <glob>]... [--bundle <path>] [--out <dir>]');
    process.exit(2);
  }
  const manifest = JSON.parse(await readFile(args.manifest, 'utf8'));
  if (args.version !== undefined) manifest.version = args.version;
  const { bytes, envelope, digest, files } = await packDirectory({ dir, manifest, lazy: args.lazy, bundle: args.bundle });
  const outDir = resolve(args.out ?? dir);
  await mkdir(outDir, { recursive: true });
  const out = join(outDir, MANIFEST_FILE);
  await writeFile(out, bytes);
  await writeFile(join(outDir, PACKAGE_FILE), envelope);
  const total = files.reduce((sum, file) => sum + file.size, 0);
  const lazyCount = files.filter((file) => file.lazy).length;
  console.log(`${manifest.id} ${manifest.version}: ${files.length} files${lazyCount > 0 ? ` (${lazyCount} lazy)` : ''}, ${total} bytes → ${out} (sha256 ${digest})`);
  if ((await stat(out)).size !== bytes.byteLength) throw new Error('short write');
}

if (import.meta.url === pathToFileURL(process.argv[1] ?? '').href) {
  main(process.argv.slice(2)).catch((error) => {
    console.error(error instanceof Error ? error.message : error);
    process.exit(1);
  });
}
