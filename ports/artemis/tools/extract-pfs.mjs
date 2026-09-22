#!/usr/bin/env node
/**
 * Extract an Artemis `.pfs` archive chain into a loose directory tree.
 *
 * ## Why this exists
 *
 * The official Artemis WebAssembly build has two data-delivery modes, and only
 * one of them can host a real commercial game:
 *
 * - **On-memory mode** (the plain `WebAssembly/` build) downloads the whole
 *   `artemis.data` package before the engine starts. That is fine for the SDK
 *   sample (15.8 MB) and impossible for a real game: `pack_file.txt` caps a
 *   single `.pfs` at 2 GB, so a large title ships as `root.pfs` plus
 *   `root.pfs.000`..`root.pfs.NNN` splits, several GB in total, which the
 *   browser would have to hold in a `WebAssembly.Memory` all at once.
 *
 * - **Asyncify mode** (`WebAssembly/asyncify/`) reads loose files over HTTP as
 *   the engine asks for them, guided by a hash list it fetches first. The
 *   engine maps a game path to `<ASYNCIFY_URL><path>` and fetches only that
 *   file, so the only requirement is that the game exists as loose files on the
 *   server — which is what this tool produces.
 *
 * This is the same split-detection the Android host relies on, expressed the
 * other way round: Android reads `root.pfs` through the pack reader, the web
 * engine reads the tree those packs decode to.
 *
 * ## Layering
 *
 * Archives are applied left to right and later entries overwrite earlier ones,
 * matching `findPfsEntry`'s "first archive that has the path wins" order only
 * when the caller passes patch archives *after* the base. Every archive given
 * on the command line is folded into one tree, so the caller decides the
 * precedence by argument order:
 *
 *     extract-pfs.mjs -o game root.pfs root.pfs.000 chs_patch_official.pfs
 *
 * `root.pfs.000` is a *continuation*, not a patch — a real game's images live
 * in it while the base pack holds scripts, sounds and fonts — so it is passed
 * alongside the base rather than replacing it.
 *
 * ## Hash list
 *
 * The engine expects `ASYNCIFY_HASH_LIST_URL` to be a plain-text file with one
 * `path,sha1` line per file, LF separated, **no leading `./`**, and critically
 * **no trailing newline**: the engine splits the body on LF and reports the
 * empty tail as `Detected invalid hash entry. ` (empty argument) which aborts
 * the whole load. Hashes are computed while writing, so no second pass.
 */
import { createHash } from 'node:crypto';
import { mkdir, open, writeFile, rm } from 'node:fs/promises';
import { dirname, join, resolve, sep } from 'node:path';
import { pathToFileURL } from 'node:url';

const REPO = resolve(import.meta.dirname, '..', '..', '..');
const { readPfsTableFrom, readPfsEntryFrom } = await import(
  pathToFileURL(join(REPO, 'packages', 'runtime-artemis', 'dist', 'pfs.js')).href
);

function parseArgs(argv) {
  const opts = { archives: [], out: null, hashlist: null, urlPrefix: '', keep: [], dryRun: false };
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (a === '-o' || a === '--out') opts.out = argv[++i];
    else if (a === '--hashlist') opts.hashlist = argv[++i];
    else if (a === '--url-prefix') opts.urlPrefix = argv[++i];
    else if (a === '--keep') opts.keep.push(argv[++i]);
    else if (a === '--dry-run') opts.dryRun = true;
    else if (a.startsWith('-')) throw new Error(`unknown option: ${a}`);
    else opts.archives.push(a);
  }
  if (!opts.out) throw new Error('usage: extract-pfs.mjs -o <dir> [--hashlist <file>] <archive.pfs>...');
  if (opts.archives.length === 0) throw new Error('at least one archive is required');
  return opts;
}

/**
 * Artemis stores paths with `/`, but a hostile or malformed table could carry
 * `..` or a drive letter. Extracted trees get served over HTTP and written to
 * disk, so refuse anything that escapes the output directory rather than
 * sanitising it silently.
 */
function safeRelativePath(entryPath, archiveName) {
  const normalized = entryPath.replace(/\\/g, '/');
  if (normalized.startsWith('/') || /^[a-zA-Z]:/.test(normalized)) {
    throw new Error(`${archiveName}: absolute path in table: ${entryPath}`);
  }
  const parts = normalized.split('/').filter((p) => p.length > 0 && p !== '.');
  if (parts.length === 0 || parts.some((p) => p === '..')) {
    throw new Error(`${archiveName}: path escapes output directory: ${entryPath}`);
  }
  return parts.join(sep);
}

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
  return { path, handle, size, source, archive };
}

const opts = parseArgs(process.argv.slice(2));
const outRoot = resolve(opts.out);

// Collect first, extract second: a malformed path anywhere should fail before
// any bytes touch the disk, and the caller gets the full picture in one run.
const wanted = new Map(); // relative path -> { archive, entry, archiveName }
const handles = [];
let totalBytes = 0;
try {
  for (const archivePath of opts.archives) {
    const loaded = await openArchive(archivePath);
    handles.push(loaded.handle);
    const name = archivePath.split(/[\\/]/).pop();
    for (const entry of loaded.archive.entries) {
      const rel = safeRelativePath(entry.path, name);
      const previous = wanted.get(rel);
      if (previous && !opts.keep.includes(rel)) {
        // Later archive wins: this is how patch packs override base files.
      }
      wanted.set(rel, { rel, entry, archive: loaded, archiveName: name });
      totalBytes += entry.length;
    }
    console.log(
      `${name}: ${loaded.archive.format}, ${loaded.archive.entries.length} entries, ` +
        `${(loaded.size / 1073741824).toFixed(2)} GB`,
    );
  }

  console.log(
    `\nunique paths: ${wanted.size}, total payload: ${(totalBytes / 1073741824).toFixed(2)} GB`,
  );
  if (opts.dryRun) process.exit(0);

  await rm(outRoot, { recursive: true, force: true });
  await mkdir(outRoot, { recursive: true });

  const hashLines = [];
  let done = 0;
  let written = 0;
  let skipped = 0;

  for (const { rel, entry, archive, archiveName } of wanted.values()) {
    const data = await readPfsEntryFrom(archive.source, archive.archive, entry);
    if (data.length !== entry.length) {
      throw new Error(
        `${archiveName}: short read for ${entry.path}: got ${data.length}, expected ${entry.length}`,
      );
    }
    const target = join(outRoot, rel);
    // `system.ini` may be replaced by the caller (the engine needs ASYNCIFY_*),
    // and save data is written by the engine itself, so let those be preserved.
    if (opts.keep.includes(rel)) {
      skipped++;
    } else {
      await mkdir(dirname(target), { recursive: true });
      await writeFile(target, data);
      written++;
    }
    if (opts.hashlist) {
      const sha1 = createHash('sha1').update(data).digest('hex');
      hashLines.push(`${opts.urlPrefix}${rel.split(sep).join('/')},${sha1}`);
    }
    done++;
    if (done % 2000 === 0) {
      const pct = ((done / wanted.size) * 100).toFixed(1);
      console.log(`  ${done}/${wanted.size} (${pct}%) written=${written} kept=${skipped}`);
    }
  }

  console.log(`\nwrote ${written} files (kept ${skipped} existing), ${(totalBytes / 1073741824).toFixed(2)} GB`);

  if (opts.hashlist) {
    hashLines.sort();
    // No trailing newline — see the header comment; the engine mis-parses it as
    // an empty entry and refuses the whole list.
    await writeFile(opts.hashlist, hashLines.join('\n'));
    console.log(`hash list: ${opts.hashlist} (${hashLines.length} entries)`);
  }
} finally {
  for (const handle of handles) await handle.close();
}
