/**
 * Verifies the **production** Artemis reader against a real 2.69 GB game archive.
 *
 * The unit tests use synthetic fixtures; this uses the bytes a shipping game actually ships. It is
 * the check that would have caught the inverted decryption heuristic without needing any test case to
 * be invented, because the real archive says plainly what the answer is: `pc\ja\artemis.png` must
 * decode to a PNG.
 *
 * `readPfsTable` cannot be used here — it takes the whole archive as one `Uint8Array`, and this file
 * is 2.69 GB (Node's own `readFileSync` refuses above 2 GiB with `ERR_FS_FILE_TOO_LARGE`). So the
 * table is read by offset and the archive object is assembled directly. `readPfsEntryData` — the
 * function under test — is then called unmodified, on a buffer holding exactly the encrypted entry.
 *
 * The window trick is sound because the XOR phase is relative to the **start of the entry**: passing
 * `{offset: 0, length: n}` over a buffer that begins at the real entry offset is equivalent to
 * reading the whole entry from the archive. (A ranged read *inside* an entry would not be, and would
 * need `rangeStart % 20` — noted in `readPfsEntryData`'s documentation.)
 */
import { openSync, readSync, closeSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { resolve } from 'node:path';

import { readPfsEntryData, findPfsEntry } from '../../../packages/engine/core/artemis-compat/dist/pfs.js';

// The pack chain is real game data and is never committed; point ARTEMIS_GAME_DIR at it.
// Default stays beside the other reference checkouts, under the gitignored `.ref/`.
const GAME = process.env.ARTEMIS_GAME_DIR ?? resolve(import.meta.dirname, '..', '..', '..', '.ref', 'artemis-game');
const PATH = `${GAME}/root.pfs`;

function readAt(fd, offset, length) {
  const buf = Buffer.alloc(length);
  let got = 0;
  while (got < length) {
    const n = readSync(fd, buf, got, length - got, offset + got);
    if (n <= 0) break;
    got += n;
  }
  return new Uint8Array(buf.buffer, buf.byteOffset, got);
}

const fd = openSync(PATH, 'r');
try {
  const header = readAt(fd, 0, 11);
  const indexSize = new DataView(header.buffer, header.byteOffset, 11).getUint32(3, true);
  const table = readAt(fd, 7, indexSize);
  const count = new DataView(table.buffer, table.byteOffset, table.byteLength).getInt32(0, true);

  // Walk the table exactly as `collectEntries` does.
  const entries = [];
  const view = new DataView(table.buffer, table.byteOffset, table.byteLength);
  let cursor = 4;
  for (let i = 0; i < count; i += 1) {
    const nameLength = view.getUint32(cursor, true);
    cursor += 4;
    const name = new TextDecoder('shift_jis').decode(table.subarray(cursor, cursor + nameLength));
    cursor += nameLength;
    const offset = view.getUint32(cursor + 4, true);
    const length = view.getUint32(cursor + 8, true);
    cursor += 12;
    entries.push({ path: name.replace(/\\/g, '/'), offset, length });
  }

  // Assembled the way `readPfsTable` would, including the version-derived `keyed` flag under test.
  const version = header[2] - 0x30;
  const archive = {
    format: `pf${String.fromCharCode(header[2])}`,
    entries,
    isPf8: header[2] === 0x38,
    tableLength: indexSize,
    payloadKey: new Uint8Array(createHash('sha1').update(table).digest()),
    keyed: [4, 5, 8, 9].includes(version),
  };

  console.log(`archive:  ${PATH}`);
  console.log(`version:  ${version}   keyed: ${archive.keyed}   entries: ${entries.length.toLocaleString()}`);
  console.log('');

  const PNG = [0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a];
  const OGG = [0x4f, 0x67, 0x67, 0x53]; // "OggS"
  const results = [];

  for (const want of ['pc/ja/artemis.png', 'font/extra_28.rft']) {
    const entry = findPfsEntry(archive, want) ?? entries.find((e) => e.path === want);
    if (!entry) {
      console.log(`  ${want}: not found`);
      continue;
    }
    // Exactly the payload, so the production function sees real encrypted bytes from offset 0.
    const window = readAt(fd, entry.offset, entry.length);
    const decoded = readPfsEntryData(window, archive, { path: entry.path, offset: 0, length: entry.length });
    const head = [...decoded.subarray(0, 8)];
    const hex = head.map((b) => b.toString(16).padStart(2, '0')).join(' ');
    const isPng = head.slice(0, 8).every((b, i) => b === PNG[i]);
    const isOgg = head.slice(0, 4).every((b, i) => b === OGG[i]);
    // RFT fonts carry a decompressed style name; a readable ASCII run is the tell.
    const textish = [...decoded.subarray(0, 64)].filter((b) => b >= 0x20 && b < 0x7f).length;
    console.log(`  ${entry.path}  (${entry.length.toLocaleString()} bytes)`);
    console.log(`    decoded first 8: ${hex}`);
    console.log(`    PNG magic: ${isPng}    OggS magic: ${isOgg}    ASCII bytes in first 64: ${textish}`);
    results.push({ path: entry.path, isPng, isOgg });
  }

  console.log('');
  const png = results.find((r) => r.path.endsWith('.png'));
  if (png && !png.isPng) {
    console.error('FAIL: the production reader did not decode the PNG payload correctly');
    process.exitCode = 1;
  } else if (png) {
    console.log('PASS: the production reader decoded a real encrypted PNG payload');
  }
} finally {
  closeSync(fd);
}
