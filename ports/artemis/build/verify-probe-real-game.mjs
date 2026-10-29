/**
 * Runs the **production** Artemis probe against the real 5.8 GB game directory, and measures what it
 * reads.
 *
 * This is the end-to-end check for the ranged path. Before it, `inspectArtemis` read the whole
 * `root.pfs` into memory behind a 512 MB ceiling — so a 2.69 GB package was skipped outright and a
 * packaged title came back looking as though its configuration were missing. The probe now reads the
 * header and the index (~1.13 MB) and then just the `system.ini` entry.
 *
 * `FileSystemDirectorySource` cannot be used here: it wraps the browser's `FileSystemDirectoryHandle`.
 * The source below is a small Node-fs equivalent with `randomAccess: true`, so the production probe
 * runs unmodified against it. It also counts every byte, because "did it work" is not the interesting
 * question — "how much did it read" is.
 */
import { closeSync, existsSync, openSync, readSync, readdirSync, statSync } from 'node:fs';
import { join, resolve } from 'node:path';

import { DefaultProbeContext } from '../../../packages/engine-api/dist/index.js';
import { inspectArtemis } from '../../../packages/engine/core/artemis-compat/dist/index.js';

// The pack chain is real game data and is never committed; point ARTEMIS_GAME_DIR at it.
const ROOT = process.env.ARTEMIS_GAME_DIR ?? resolve(import.meta.dirname, '..', '..', '..', '.ref', 'artemis-game');

/** A `GameSource` over a real directory, with per-read accounting. */
class NodeFsGameSource {
  constructor(root) {
    this.name = 'artemis-real';
    this.capabilities = { list: true, randomAccess: true, writable: false };
    // Normalize ONCE, here. `#full()` compares a resolve()d path against
    // `this.root`, and on Windows resolve() always yields backslashes while a
    // user-supplied `ARTEMIS_GAME_DIR` is typically written with forward slashes
    // ("D:/games/x"). Comparing the two forms made the escape guard reject the
    // root itself, so every `list('')`/`exists(...)` failed and the probe
    // reported a real 2.69 GB package as "no packages found" -- a false negative
    // that looks exactly like the loading-model limitation it was written to
    // rule out. (Measured: the same directory passes with backslashes and fails
    // with forward slashes.)
    this.root = resolve(root);
    this.bytesRead = 0;
    this.reads = [];
  }

  #full(path) {
    const clean = String(path).replace(/^[/\\]+/, '');
    const full = resolve(this.root, clean);
    // Refuse to escape the fixture root, the way the importers do.
    if (!full.startsWith(this.root)) throw new Error(`refusing path outside root: ${path}`);
    return full;
  }

  async list(dir = '') {
    const full = this.#full(dir);
    if (!existsSync(full)) return [];
    return readdirSync(full, { withFileTypes: true }).map((entry) =>
      entry.isDirectory() ? `${entry.name}/` : entry.name,
    );
  }

  async readFile(path) {
    const full = this.#full(path);
    const size = statSync(full).size;
    const fd = openSync(full, 'r');
    try {
      const buf = Buffer.alloc(size);
      let got = 0;
      while (got < size) {
        const n = readSync(fd, buf, got, size - got, got);
        if (n <= 0) break;
        got += n;
      }
      this.bytesRead += got;
      this.reads.push({ path, offset: 0, length: got, whole: true });
      return new Uint8Array(buf.buffer, buf.byteOffset, got);
    } finally {
      closeSync(fd);
    }
  }

  async readRange(path, offset, length) {
    const full = this.#full(path);
    const size = statSync(full).size;
    const want = Math.max(0, Math.min(length, size - offset));
    const buf = Buffer.alloc(want);
    const fd = openSync(full, 'r');
    try {
      let got = 0;
      while (got < want) {
        const n = readSync(fd, buf, got, want - got, offset + got);
        if (n <= 0) break;
        got += n;
      }
      this.bytesRead += got;
      this.reads.push({ path, offset, length: got, whole: got >= size });
      return new Uint8Array(buf.buffer, buf.byteOffset, got);
    } finally {
      closeSync(fd);
    }
  }

  async size(path) {
    try {
      return statSync(this.#full(path)).size;
    } catch {
      return -1;
    }
  }

  async exists(path) {
    try {
      return statSync(this.#full(path)).isFile();
    } catch {
      return false;
    }
  }

  async isDirectory(path) {
    try {
      return statSync(this.#full(path)).isDirectory();
    } catch {
      return false;
    }
  }
}

const source = new NodeFsGameSource(ROOT);
const inspection = await inspectArtemis(new DefaultProbeContext(source));

console.log(`root:               ${ROOT}`);
console.log('');
console.log('--- what the probe concluded ---');
console.log(`  hasRootPfs:       ${inspection.hasRootPfs}`);
console.log(`  hasAnyPfs:        ${inspection.hasAnyPfs}`);
console.log(`  packages:         ${JSON.stringify(inspection.packages)}`);
console.log(`  packageUnreadable:${inspection.packageUnreadable}`);
console.log(`  packageEntries:   ${inspection.packageEntries?.length ?? 0} entries`);
console.log(`  systemIni from:   ${inspection.systemIniSource ?? '(none)'}`);
console.log(`  systemIni:        ${inspection.systemIni ? `${inspection.systemIni.length} chars` : '(none)'}`);
if (inspection.systemIni) {
  const head = inspection.systemIni.split(/\r?\n/).slice(0, 6).join(' | ');
  console.log(`  systemIni head:   ${head.slice(0, 150)}`);
}
console.log('');
console.log('--- what it cost ---');
const archiveBytes = statSync(join(ROOT, 'root.pfs')).size;
console.log(`  root.pfs size:    ${archiveBytes.toLocaleString()}`);
console.log(`  bytes read:       ${source.bytesRead.toLocaleString()}`);
console.log(`  share of archive: ${((source.bytesRead / archiveBytes) * 100).toFixed(3)}%`);
console.log(`  reads:            ${source.reads.length}`);
for (const read of source.reads.slice(0, 8)) {
  const tag = read.whole ? ' WHOLE FILE' : '';
  console.log(`    ${read.path}  off=${read.offset.toLocaleString()} len=${read.length.toLocaleString()}${tag}`);
}
if (source.reads.length > 8) console.log(`    … ${source.reads.length - 8} more`);

const wholeFileReads = source.reads.filter((r) => r.whole);
console.log('');
if (wholeFileReads.length > 0) {
  console.error(`FAIL: ${wholeFileReads.length} whole-file read(s) — the ranged path was not used`);
  process.exitCode = 1;
} else if (!inspection.systemIni) {
  console.error('FAIL: the probe did not recover system.ini from inside root.pfs');
  process.exitCode = 1;
} else if (inspection.packageEntries.length !== 21587) {
  console.error(`FAIL: expected 21,587 entries, got ${inspection.packageEntries.length}`);
  process.exitCode = 1;
} else {
  console.log(
    `PASS: read ${inspection.packageEntries.length.toLocaleString()} entries and system.ini ` +
      `from a 2.69 GB package using ${source.bytesRead.toLocaleString()} bytes ` +
      `(${((source.bytesRead / archiveBytes) * 100).toFixed(3)}%)`,
  );
}
