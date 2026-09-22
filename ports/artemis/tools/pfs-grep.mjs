#!/usr/bin/env node
/**
 * Extract only the entries of a `.pfs` chain whose paths match a filter, without
 * materialising the tree.
 *
 * ## Why this exists
 *
 * The real game ships as `root.pfs` (2.7 GB, 21,587 entries) plus
 * `root.pfs.000` (3.4 GB, 7,637 entries). `extract-pfs.mjs` writes every byte,
 * which is the right thing for serving the engine but ~6 GB of disk and many
 * minutes for a question that only needs the scripts. Asking "does the game use
 * tag X" wants `script/` and `system/` — a few megabytes.
 *
 * ## Usage
 *
 *   node pfs-grep.mjs --filter '^(script|system)/' --grep 'seplay|voice' \
 *       .test/test-games/Artemis/root.pfs
 *   node pfs-grep.mjs --filter '\.asb$' --list root.pfs root.pfs.000
 *   node pfs-grep.mjs --filter 'system\.ini$' --dump root.pfs
 *
 * Later archives override earlier ones, matching the engine's patch precedence,
 * so the same path appearing in both a base and a patch pack is reported for the
 * pack that actually wins.
 */
import { open } from 'node:fs/promises';
import { resolve, join } from 'node:path';
import { pathToFileURL } from 'node:url';

const REPO = resolve(import.meta.dirname, '..', '..', '..');
const { readPfsTableFrom, readPfsEntryFrom } = await import(
  pathToFileURL(join(REPO, 'packages', 'runtime-artemis', 'dist', 'pfs.js')).href
);

const argv = process.argv.slice(2);
const opts = { filter: null, grep: null, mode: 'grep', files: [] };
for (let i = 0; i < argv.length; i++) {
  const a = argv[i];
  if (a === '--filter') opts.filter = new RegExp(argv[++i]);
  else if (a === '--grep') opts.grep = new RegExp(argv[++i], 'g');
  else if (a === '--list') opts.mode = 'list';
  else if (a === '--dump') opts.mode = 'dump';
  else if (a === '--tags') opts.mode = 'tags';
  else if (a.startsWith('-')) throw new Error(`unknown option: ${a}`);
  else opts.files.push(a);
}
if (!opts.filter) throw new Error("usage: pfs-grep.mjs --filter '<regex>' [--grep <regex>|--list|--dump|--tags] <archive.pfs>...");

async function openArchive(path) {
  const handle = await open(path, 'r');
  const size = (await handle.stat()).size;
  const source = {
    size: async () => size,
    readRange: async (offset, length) => {
      const buffer = Buffer.allocUnsafe(length);
      const { bytesRead } = await handle.read(buffer, 0, length, offset);
      return new Uint8Array(buffer.subarray(0, bytesRead));
    },
  };
  const archive = await readPfsTableFrom(source);
  if (!archive) throw new Error(`${path}: not a readable .pfs archive`);
  return { handle, size, source, archive };
}

/**
 * Artemis scripts are Shift_JIS unless `system.ini` sets `CHARSET = UTF-8`.
 * Tag names themselves are ASCII, so a lenient decode is enough to find them,
 * but text comparisons need the real encoding or names come out mangled.
 */
function decode(bytes, utf8) {
  return new TextDecoder(utf8 ? 'utf-8' : 'shift_jis', { fatal: false }).decode(bytes);
}

const handles = [];
try {
  // Pass 1: build the winning set of paths across the whole chain.
  const wanted = new Map();
  for (const file of opts.files) {
    const loaded = await openArchive(file);
    handles.push(loaded.handle);
    const name = file.split(/[\\/]/).pop();
    for (const entry of loaded.archive.entries) {
      if (!opts.filter.test(entry.path)) continue;
      wanted.set(entry.path, { entry, loaded, name });
    }
    console.log(`${name}: ${loaded.archive.entries.length} entries, ${wanted.size} match so far`);
  }

  const utf8 = process.env.ARTEMIS_UTF8 === '1';
  const tagCounts = new Map();
  let scanned = 0;
  let totalBytes = 0;

  for (const [path, { entry, loaded, name }] of wanted) {
    if (opts.mode === 'list') {
      console.log(`  ${path}  ${entry.length} B  [${name}]`);
      continue;
    }
    const data = await readPfsEntryFrom(loaded.source, loaded.archive, entry);
    totalBytes += data.length;
    if (opts.mode === 'dump') {
      process.stdout.write(data);
      continue;
    }
    const text = decode(data, utf8);
    scanned++;

    if (opts.mode === 'tags') {
      // Two script forms ship in the wild:
      //   `.asb` — one tag per line, bare name: `seplay id=1 file="x"`.
      //   `.ast` — a block form whose every command is `{"tagname", arg=val,...}`.
      // Both are covered here: an `.ast` command always quotes its tag name as
      // the first element of a brace group, and an `.asb` tag is a line head.
      for (const m of text.matchAll(/\{\s*"([a-z_][a-z0-9_]{1,24})"/g)) {
        tagCounts.set(m[1], (tagCounts.get(m[1]) ?? 0) + 1);
      }
      for (const line of text.split(/\r?\n/)) {
        const m = /^\s*@?([a-z_][a-z0-9_]{1,24})\b(?!\s*[=:"])/.exec(line);
        if (m) tagCounts.set(m[1], (tagCounts.get(m[1]) ?? 0) + 1);
      }
      continue;
    }

    if (opts.grep) {
      for (const line of text.split(/\r?\n/)) {
        opts.grep.lastIndex = 0;
        if (opts.grep.test(line)) console.log(`${path}: ${line.trim().slice(0, 200)}`);
      }
    }
  }

  if (opts.mode === 'tags') {
    const sorted = [...tagCounts].sort((a, b) => b[1] - a[1]);
    console.log(`\n${scanned} files scanned, ${(totalBytes / 1048576).toFixed(1)} MB, ${sorted.length} distinct tags`);
    for (const [tag, count] of sorted) console.log(`${String(count).padStart(7)}  ${tag}`);
  }
} finally {
  for (const handle of handles) await handle.close();
}
