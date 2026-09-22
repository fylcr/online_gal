/**
 * Demonstrates that `readPfsEntryData`'s "does it look like text" heuristic returns the **encrypted**
 * bytes for a binary asset, on an archive whose encryption is known from its version.
 *
 * The heuristic (packages/runtime-artemis/src/pfs.ts):
 *
 *     if (decodedLooksText && !rawLooksText) return decoded;
 *     return data.slice();          // binary asset → raw, i.e. still encrypted
 *
 * `looksLikeText` counts every byte >= 0x20 except 0x7F as printable — which includes the whole
 * 0x80..0xFF range. Obfuscated data is dense in that range, so encrypted bytes routinely pass the
 * 0.85 threshold, while a decrypted PNG begins 0x89 0x50 0x4E 0x47 0x0D 0x0A 0x1A 0x0A and contains
 * NUL bytes, so it scores *lower*. The test inverts for exactly this case.
 *
 * What should happen instead is settled by GARbro's `ArcPFS.cs`: the payload is keyed only for
 * versions 4, 5, 8 and 9 —
 *
 *     if (version != 8 && version != 9 && version != 4 && version != 5)
 *         return new ArcFile (file, this, dir);          // no PfsArchive, hence no key
 *
 * — and this archive is version 8, so its payload **is** encrypted and the answer is not in doubt.
 *
 * Note the reads: the file is 2.69 GB, so it is read with explicit offsets rather than slurped.
 * `readFileSync` refuses outright (`ERR_FS_FILE_TOO_LARGE`, >2 GiB), which is the same constraint the
 * whole-file `readPfsTable(bytes: Uint8Array)` API runs into on an archive of this size.
 */
import { openSync, readSync, closeSync, fstatSync } from 'node:fs';
import { createHash } from 'node:crypto';

// The pack chain is real game data and is never committed; point ARTEMIS_GAME_DIR at it.
// Default stays beside the other reference checkouts, under the gitignored `.ref/`.
const GAME = process.env.ARTEMIS_GAME_DIR ?? new URL('../../.ref/artemis-game/', import.meta.url).pathname;
const PATH = `${GAME}/root.pfs`;

/** Mirrors `looksLikeText` in packages/runtime-artemis/src/pfs.ts. */
function looksLikeText(bytes) {
  const sample = bytes.subarray(0, Math.min(256, bytes.byteLength));
  if (sample.byteLength === 0) return false;
  let printable = 0;
  for (const byte of sample) {
    if (byte === 0x09 || byte === 0x0a || byte === 0x0d) {
      printable += 1;
      continue;
    }
    if (byte >= 0x20 && byte !== 0x7f) printable += 1;
  }
  return printable / sample.byteLength > 0.85;
}

/** Reads exactly `length` bytes at `offset`, so a 2.69 GB file never has to fit in memory. */
function readAt(fd, offset, length) {
  const buf = Buffer.alloc(length);
  let got = 0;
  while (got < length) {
    const n = readSync(fd, buf, got, length - got, offset + got);
    if (n <= 0) break;
    got += n;
  }
  return buf.subarray(0, got);
}

const fd = openSync(PATH, 'r');
try {
  const fileSize = fstatSync(fd).size;
  const header = readAt(fd, 0, 11);
  const indexSize = header.readUInt32LE(3);
  const count = header.readInt32LE(7);
  const table = readAt(fd, 7, indexSize);
  const key = createHash('sha1').update(table).digest();

  // Walk the table to the first PNG entry, the way `collectEntries` does.
  let cursor = 4;
  let target = null;
  for (let i = 0; i < count; i += 1) {
    const nameLength = table.readInt32LE(cursor);
    cursor += 4;
    const name = table.subarray(cursor, cursor + nameLength).toString('latin1');
    cursor += nameLength;
    const offset = table.readUInt32LE(cursor + 4);
    const length = table.readUInt32LE(cursor + 8);
    cursor += 12;
    if (name.toLowerCase().endsWith('artemis.png')) {
      target = { name, offset, length };
      break;
    }
  }

  const raw = readAt(fd, target.offset, target.length);
  const decoded = Buffer.alloc(raw.length);
  // Phase is `i % key.length` from the *start of the entry*, which is what reading the whole entry
  // gives. A ranged read inside an entry would need `(rangeStart % key.length)` instead — see the
  // note at the end.
  for (let i = 0; i < raw.length; i += 1) decoded[i] = raw[i] ^ key[i % key.length];

  const PNG = Buffer.from([0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a]);
  const pngLike = (b) => b.subarray(0, 8).equals(PNG);
  const hex = (b) => b.subarray(0, 8).toString('hex').match(/../g).join(' ');
  const share = (b) => {
    const n = Math.min(256, b.byteLength);
    let k = 0;
    for (const x of b.subarray(0, n)) if (x >= 0x20 && x !== 0x7f) k += 1;
    return (k / n).toFixed(3);
  };

  console.log(`file size:              ${fileSize.toLocaleString()}`);
  console.log(`version:                ${String.fromCharCode(header[2])}  (GARbro keys versions 4,5,8,9)`);
  console.log(`count:                  ${count.toLocaleString()}`);
  console.log(`entry:                  ${target.name}  (${target.length.toLocaleString()} bytes)`);
  console.log(`key (SHA-1 of table):   ${key.toString('hex')}`);
  console.log('');
  console.log(`raw     first 8:        ${hex(raw)}`);
  console.log(`decoded first 8:        ${hex(decoded)}`);
  console.log(`raw is PNG:             ${pngLike(raw)}`);
  console.log(`decoded is PNG:         ${pngLike(decoded)}`);
  console.log('');
  console.log(`looksLikeText(raw):     ${looksLikeText(raw)}      (byte share ${share(raw)})`);
  console.log(`looksLikeText(decoded): ${looksLikeText(decoded)}      (byte share ${share(decoded)})`);
  console.log('');
  const wouldReturnDecoded = looksLikeText(decoded) && !looksLikeText(raw);
  console.log(`readPfsEntryData returns:      ${wouldReturnDecoded ? 'decoded' : 'RAW — still encrypted'}`);
  console.log(`correct for version 8:         decoded`);
} finally {
  closeSync(fd);
}
