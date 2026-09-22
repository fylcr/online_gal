/**
 * Artemis PFS archive reading.
 *
 * PFS is the Artemis engine's package format (`pf8` and the earlier `pf6`/`pf7`
 * variants). Reading it is what makes a browser-side *catalogue* of an Artemis
 * game possible: the engine's own `system.ini` and the scene scripts live inside
 * `root.pfs`, so a directory listing alone cannot tell whether a title is
 * complete or which scenes it declares.
 *
 * This is an independent reader written against the observed container layout —
 * no engine source is copied. The format:
 *
 * ```text
 * 0x00  "pf"            2 bytes, magic
 * 0x02  '8' | '6' | '7' version nibble as ASCII
 * 0x03  <table length>  4-byte little-endian
 * 0x07  <table>         byte count from the previous field
 * ```
 *
 * The table itself is a 4-byte little-endian entry count followed by one record
 * per entry:
 *
 * ```text
 * <name length>  4-byte LE, high bit masked
 * <name bytes>   UTF-8
 * <data length>  4-byte LE
 * <offset>       4-byte LE
 * <length>       4-byte LE
 * ```
 *
 * Entry names and data are frequently XOR-obfuscated. The keystream is
 * `SHA-1(<the table bytes>)`, which is why the table itself must stay readable:
 * it is the key material. Names are therefore read as plain UTF-8, and the same
 * digest is used to decode entry payloads.
 */

import { sha1, xorWithKey } from './sha1.js';

/**
 * Reads a 32-bit little-endian field.
 *
 * Deliberately **no high-bit masking**. An earlier version cleared bit 31 here, which silently
 * corrupts every offset and size at or above 2 GiB — and real archives do reach there:
 *
 * | archive | size | entries with offset or size >= 2 GiB |
 * | --- | --- | --- |
 * | `root.pfs` | 2.69 GB | 3,940 of 21,587 |
 * | `root.pfs.000` | 3.36 GB | 6,582 of 7,637 |
 *
 * That is 10,522 entries across one game, and the failure is silent: a masked offset points at the
 * wrong bytes rather than raising. Both reference implementations read a plain unsigned 32-bit value
 * here (GARbro's `View.ToUInt32`, artemis-compat's hand-rolled little-endian read), so the mask was
 * this reader's own invention.
 *
 * Bounds are enforced by the callers instead — `MAX_TABLE_BYTES`, `MAX_NAME_BYTES` and
 * `MAX_ENTRIES` — which is where a genuinely implausible value should be rejected.
 */
function readU32(view: DataView, offset: number): number {
  return view.getUint32(offset, true);
}

export interface PfsEntry {
  /** Path inside the archive, with `\` normalised to `/`. */
  readonly path: string;
  /** Absolute offset of the payload within the archive. */
  readonly offset: number;
  /** Payload length in bytes. */
  readonly length: number;
}

export interface PfsArchive {
  /** `pf8`, `pf7` or `pf6`. */
  readonly format: string;
  readonly entries: readonly PfsEntry[];
  /** True when a `pf8` magic was present (the newer container). */
  readonly isPf8: boolean;
  readonly tableLength: number;
  /**
   * Key for decoding entry payloads, derived from the table bytes.
   *
   * Null only when the table was unreadable, in which case payloads cannot be
   * decoded at all.
   */
  readonly payloadKey: Uint8Array | null;
  /**
   * Whether entry payloads are XOR-obfuscated, **decided by the container version**.
   *
   * GARbro's `ArcPFS.cs` is explicit about which versions carry a key:
   *
   * ```csharp
   * // key calculated for archive versions 4, 5, 8 and 9
   * if (version != 8 && version != 9 && version != 4 && version != 5)
   *     return new ArcFile (file, this, dir);      // no PfsArchive, hence no key
   * ```
   *
   * This replaced a "does the decoded payload look like text" heuristic, which was not merely
   * unnecessary but **inverted** on a real archive. `looksLikeText` counts every byte >= 0x20 except
   * 0x7F as printable, so the whole 0x80..0xFF range counts — and obfuscated data is dense there.
   * Measured on `pc\ja\artemis.png` from a version-8 `root.pfs`:
   *
   * ```text
   * raw     first 8:  71 3f 22 a8 d0 9c 96 a6     looksLikeText -> true   (share 0.898)
   * decoded first 8:  89 50 4e 47 0d 0a 1a 0a     looksLikeText -> false  (share 0.598)
   * ```
   *
   * The decoded bytes are a PNG and the encrypted ones are not, yet the heuristic preferred the
   * encrypted form — so every binary asset (image, audio, font) came back still encrypted. The
   * version is authoritative and needs no guessing.
   */
  readonly keyed: boolean;
}

/** True when the payload begins with a PFS magic. */
export function hasPfsMagic(bytes: Uint8Array): boolean {
  return (
    bytes.byteLength >= 3 &&
    bytes[0] === 0x70 &&
    bytes[1] === 0x66 &&
    bytes[2]! >= 0x30 &&
    bytes[2]! <= 0x39
  );
}

/** True when the payload is specifically the `pf8` variant. */
export function hasPf8Magic(bytes: Uint8Array): boolean {
  return hasPfsMagic(bytes) && bytes[2] === 0x38;
}

/** Minimum bytes needed before the table can be located. */
const HEADER_BYTES = 7;
const MAX_TABLE_BYTES = 50 * 1024 * 1024;
const MAX_ENTRIES = 200_000;
const MAX_NAME_BYTES = 4096;

/**
 * Assembles an archive from the version byte and the table bytes.
 *
 * Shared by the in-memory and the ranged entry points so both apply the same validation and the same
 * version-driven key rule. `archiveLength` is the **whole file's** size, which is what bounds every
 * entry offset; the ranged path takes it from the byte source rather than from a buffer length.
 */
function buildArchive(
  versionByte: number,
  table: Uint8Array,
  archiveLength: number,
): PfsArchive | null {
  const tableView = new DataView(table.buffer, table.byteOffset, table.byteLength);
  const declared = Math.min(readU32(tableView, 0), MAX_ENTRIES);
  const entries = collectEntries(table, tableView, declared, archiveLength);
  if (!entries || entries.length === 0) return null;

  // The container version is an ASCII digit at offset 2. GARbro keys the payload for versions 4, 5, 8
  // and 9 only — see `PfsArchive.keyed` for why this is not left to a content heuristic.
  const version = versionByte - 0x30;

  return {
    format: `pf${String.fromCharCode(versionByte)}`,
    entries,
    isPf8: versionByte === 0x38,
    tableLength: table.byteLength,
    // The digest is the payload keystream for the keyed versions. It is still derived for unkeyed
    // containers — the digest is a pure function of the table and costs nothing — but nothing
    // consumes it there, because `keyed` is what `readPfsEntryData` branches on.
    payloadKey: sha1(table),
    keyed: version === 4 || version === 5 || version === 8 || version === 9,
  };
}

/**
 * Attempts to read a PFS table from an **in-memory** archive.
 *
 * Returns null rather than throwing when the payload is not a valid table, so a
 * probe can treat "cannot read" as "no evidence" instead of an error.
 *
 * This needs the whole archive resident, so it cannot serve a large package at all: a 2.69 GB
 * `root.pfs` does not even fit Node's `readFileSync` (`ERR_FS_FILE_TOO_LARGE`, >2 GiB). Use
 * {@link readPfsTableFrom} when the archive can be read by range, which is the normal case for a
 * `GameSource` or a VFS `ByteSource`.
 */
export function readPfsTable(bytes: Uint8Array): PfsArchive | null {
  if (bytes.byteLength < HEADER_BYTES) return null;
  if (!hasPfsMagic(bytes)) return null;

  const header = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const tableLength = readU32(header, 3);
  if (
    tableLength <= 0 ||
    tableLength > MAX_TABLE_BYTES ||
    HEADER_BYTES + tableLength > bytes.byteLength
  ) {
    return null;
  }

  return buildArchive(
    bytes[2]!,
    bytes.subarray(HEADER_BYTES, HEADER_BYTES + tableLength),
    bytes.byteLength,
  );
}

/**
 * The shape of a random-access byte source — structurally identical to VFS's `ByteSource`, so a
 * `GameSource` adapts to it with `gameSourceByteSource(source, path)`.
 *
 * Declared structurally rather than imported so this package stays usable without a VFS instance
 * (the CLI, the tests, a `Blob`).
 */
export interface PfsByteSource {
  size(): Promise<number>;
  readRange(offset: number, length: number): Promise<Uint8Array>;
}

/**
 * Reads a PFS table from a byte source, touching **only the header and the index**.
 *
 * This is what makes a multi-gigabyte package inspectable. For the 2.69 GB `root.pfs` it reads
 * `11 + 1,185,775` bytes — about **1.13 MB, or 0.042%** of the archive — where the in-memory reader
 * would have to hold all of it.
 *
 * Note that the *entire* index must be read even though the entry records stop part-way through it
 * (at 1,013,063 of 1,185,775 for that archive). The payload keystream is `SHA-1(index)`, so the 172 KB
 * tail is hashed even though it is never parsed — skipping it would produce a different digest and
 * every payload would decode to noise. 1.13 MB is therefore the floor, not the record table's size.
 */
export async function readPfsTableFrom(source: PfsByteSource): Promise<PfsArchive | null> {
  const total = await source.size();
  if (total < HEADER_BYTES) return null;

  const header = await source.readRange(0, HEADER_BYTES);
  if (header.byteLength < HEADER_BYTES || !hasPfsMagic(header)) return null;

  const headerView = new DataView(header.buffer, header.byteOffset, header.byteLength);
  const tableLength = readU32(headerView, 3);
  if (tableLength <= 0 || tableLength > MAX_TABLE_BYTES || HEADER_BYTES + tableLength > total) {
    return null;
  }

  const table = await source.readRange(HEADER_BYTES, tableLength);
  if (table.byteLength < tableLength) return null;

  return buildArchive(header[2]!, table, total);
}

/**
 * Reads one entry's bytes from a byte source, applying the XOR layer when the container is keyed.
 *
 * The keystream phase is 0 because the read starts at the entry's own offset. A read of a sub-range
 * *inside* an entry would need `rangeStart % key.length` instead — the same correction GARbro's
 * `ByteStringEncryptedStream` makes with `m_base_pos = start_pos % m_key.Length`. Offsets are already
 * archive-absolute in the table, so no adjustment is needed here.
 */
export async function readPfsEntryFrom(
  source: PfsByteSource,
  archive: PfsArchive,
  entry: PfsEntry,
): Promise<Uint8Array | null> {
  const length = Math.max(0, entry.length);
  if (length === 0) return new Uint8Array(0);

  const total = await source.size();
  if (entry.offset < 0 || entry.offset >= total) return null;

  // Clamp to the end of the archive rather than trusting the record, the way `readPfsEntryData` does.
  const want = Math.min(length, total - entry.offset);
  const data = await source.readRange(entry.offset, want);
  if (data.byteLength === 0) return null;

  if (!archive.keyed || !archive.payloadKey) return data;
  return xorWithKey(data, archive.payloadKey);
}

/**
 * Walks the entry records.
 *
 * Returns null when the walk does not stay in bounds, which means the payload
 * is not really a PFS table.
 */
function collectEntries(
  table: Uint8Array,
  view: DataView,
  declared: number,
  archiveLength: number,
): PfsEntry[] | null {
  const entries: PfsEntry[] = [];
  let cursor = 4;

  for (let index = 0; index < declared; index += 1) {
    if (cursor + 4 > table.byteLength) break;
    const nameLength = readU32(view, cursor);
    cursor += 4;
    if (nameLength <= 0 || nameLength > MAX_NAME_BYTES) break;
    if (cursor + nameLength > table.byteLength) return null;

    const nameBytes = table.subarray(cursor, cursor + nameLength);
    cursor += nameLength;

    const rawName = decodeName(nameBytes);
    if (rawName === null) return null;

    // Three 32-bit fields follow the name: a per-entry data length, the payload
    // offset, and the stored length.
    if (cursor + 12 > table.byteLength) return null;
    const declaredLength = readU32(view, cursor);
    const offset = readU32(view, cursor + 4);
    const length = readU32(view, cursor + 8);
    cursor += 12;

    // A record whose offset reaches past the archive is a misread, not a real
    // entry; rejecting it keeps a bad reading from looking like a good one.
    if (offset > archiveLength) return null;

    entries.push({
      path: rawName.replace(/\\/g, '/'),
      offset,
      length: length > 0 ? length : declaredLength,
    });
  }

  return entries;
}

/**
 * Decodes an entry name as UTF-8, rejecting names that are clearly misread.
 *
 * Obfuscation with the wrong key produces control characters and replacement
 * characters; requiring a plausible path keeps that from being accepted.
 */
function decodeName(bytes: Uint8Array): string | null {
  let suspicious = 0;
  for (const byte of bytes) {
    if (byte === 0) return null;
    if (byte < 0x09 || (byte > 0x0d && byte < 0x20)) suspicious += 1;
  }
  if (suspicious > 0) return null;

  try {
    return new TextDecoder('utf-8', { fatal: true }).decode(bytes);
  } catch {
    // Non-UTF-8 names exist in older archives; Shift_JIS is the usual fallback.
    try {
      return new TextDecoder('shift_jis').decode(bytes);
    } catch {
      return null;
    }
  }
}

/**
 * Extracts the first entry whose path matches `wanted`.
 *
 * Used to pull `system.ini` out of an archive without expanding the whole
 * package, which matters because Artemis packages are frequently hundreds of
 * megabytes.
 */
export function findPfsEntry(
  archive: PfsArchive,
  wanted: string,
): PfsEntry | undefined {
  const target = wanted.toLowerCase().replace(/\\/g, '/');
  return archive.entries.find(
    (entry) => entry.path.toLowerCase() === target,
  );
}

/** True when any entry path ends with `suffix` (case-insensitive). */
export function hasPfsEntryEndingWith(
  archive: PfsArchive,
  suffix: string,
): boolean {
  const target = suffix.toLowerCase();
  return archive.entries.some((entry) => entry.path.toLowerCase().endsWith(target));
}

/**
 * Reads one entry's bytes, applying the XOR layer when the archive used one.
 *
 * The keystream is the same SHA-1 table digest, indexed by the byte's position *within the payload*
 * rather than within the archive — reading a whole entry starts at phase 0. (A ranged read inside an
 * entry would need `rangeStart % key.length` as the phase, which is what GARbro's
 * `ByteStringEncryptedStream` does with `m_base_pos = start_pos % m_key.Length`.)
 *
 * Whether to apply it comes from the container version (`PfsArchive.keyed`), not from inspecting the
 * result. The content heuristic this replaced was inverted on a real archive — see `keyed` for the
 * measurements.
 */
export function readPfsEntryData(
  bytes: Uint8Array,
  archive: PfsArchive,
  entry: PfsEntry,
): Uint8Array | null {
  const start = entry.offset;
  const end = Math.min(bytes.byteLength, start + Math.max(0, entry.length));
  if (start < 0 || start >= bytes.byteLength || end <= start) return null;
  const data = bytes.subarray(start, end);

  if (!archive.keyed || !archive.payloadKey) return data.slice();
  return xorWithKey(data, archive.payloadKey);
}

/** Heuristic: does this buffer look like the text scripts Artemis stores? */
export function looksLikeText(bytes: Uint8Array): boolean {
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
