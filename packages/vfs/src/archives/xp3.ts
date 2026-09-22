import { matchesAscii, readUtf16le, readXp3Tag, readXp3VarInt } from '../bytes.js';
import { inflateZlib, BaseArchiveReader, type ArchiveEntry } from './reader.js';
import type { ByteSource } from './zip.js';

/**
 * The XP3 signature.
 *
 * Measured from two real archives that agree byte for byte — a shipping
 * commercial title and AetherKiri's bundled demo:
 *
 * ```text
 * 58 50 33 0d 0a 20 0a 1a 8b 67 01   "XP3\r\n \n\x1a\x8bg\x01"
 * ```
 *
 * Written as numeric bytes on purpose. As a string literal this unprintable run
 * lost its `0a` twice while being corrected, and each time the result matched no
 * file at all — silently, because a wrong signature and an absent archive look
 * identical to the caller. Numeric bytes cannot drift that way.
 *
 * The previous constant also expected a leading `0x11` device-control byte.
 * Both the reader and its test fixture had been written from that same
 * misreading, which is why the suite passed while no real archive could be
 * opened.
 */
const XP3_SIGNATURE_BYTES: readonly number[] = [
  0x58, 0x50, 0x33, 0x0d, 0x0a, 0x20, 0x0a, 0x1a, 0x8b, 0x67, 0x01,
];

/** Byte position of the 64-bit index offset, immediately after the signature. */
const XP3_INDEX_OFFSET_AT = XP3_SIGNATURE_BYTES.length;

/** Bytes of header needed to read the signature and the index offset. */
const XP3_HEADER_BYTES = XP3_INDEX_OFFSET_AT + 8;

/**
 * How far past the declared index pointer to look for the first chunk tag.
 *
 * A real archive's header points a few bytes *before* its first `File` chunk —
 * 9 bytes in AetherKiri's demo. The pointer addresses a short preamble rather
 * than the chunk chain itself.
 *
 * The value is not taken from the specification; it was measured by dumping the
 * region each archive's pointer lands in and locating the first tag relative to
 * it. The search is bounded by this constant, so a variant with a longer
 * preamble would need it raised.
 */
const INDEX_PREAMBLE_BYTES = 12;

/**
 * Locates the first chunk tag at or after `offset`.
 *
 * Tries the declared offset first, then a bounded forward search. Returning the
 * position of a real tag rather than trusting the pointer is what makes the
 * reader tolerant of the preamble variants seen in the wild; the search is
 * bounded because scanning a multi-gigabyte file would be worse than failing.
 *
 * ## Known gap: the `0x80` index redirect
 *
 * This is not the whole story. Real commercial KiriKiri2 archives use a second
 * index form that this reader does not implement, and the omission is why they
 * cannot be opened.
 *
 * Taken from `garbro`'s `ArcXP3.TryOpen` (MIT), which handles it, and confirmed
 * on three shipping archives:
 *
 * ```text
 * dir_offset = base + i64le@0x0b
 * if (u32le@dir_offset == 0x80)              // not the index; a redirect
 *     dir_offset = base + i64le@(dir_offset + 9)
 * header_type = u8@dir_offset                // 0 = raw, 1 = zlib
 * raw :  size = i64le@(dir_offset+1); data at dir_offset+9
 * zlib:  packed = i64le@(dir_offset+1), unpacked = i64le@(dir_offset+9);
 *        deflate stream at dir_offset+17
 * ```
 *
 * Note the redirect's second offset sits at **+9**, not +1 — the same shape as a
 * normal index header, where byte 0 is the type and an `i64le` follows.
 *
 * KiriKiri2's own `XP3Archive.cpp` reads an index *size* at that position and
 * treats a zero as fatal, which is what the declared pointer yields here; the
 * redirect branch is what it is missing for these archives.
 *
 * Measured against the three archives, following the redirect and inflating the
 * index yields every entry with correct UTF-16 names: 39,045 entries in
 * `data.xp3`, 878 in `adult.xp3`, 28 in `video.xp3`.
 */
async function findIndexStart(source: ByteSource, offset: number): Promise<number> {
  const window = 64;
  const head = await source.readRange(offset, window);
  // `skip` is inclusive of INDEX_PREAMBLE_BYTES: the tag can sit exactly that
  // far ahead, which is the offset every archive measured so far uses.
  const maxSkip = Math.min(INDEX_PREAMBLE_BYTES, Math.max(0, head.byteLength - 4));
  for (let skip = 0; skip <= maxSkip; skip += 1) {
    const tag = tagAt(head, skip);
    if (tag !== null && CHUNK_TAGS.has(tag)) return offset + skip;
  }
  // Fall back to the declared pointer, so a failure reports at the real place.
  return offset;
}

/** Reads the four-character tag at `at`, or null when it is not printable. */
function tagAt(bytes: Uint8Array, at: number): number | null {
  if (at + 4 > bytes.byteLength) return null;
  const literal =
    ((bytes[at]! << 24) | (bytes[at + 1]! << 16) | (bytes[at + 2]! << 8) | bytes[at + 3]!) >>> 0;
  return isPrintableTag(literal) ? literal : null;
}

/** True when `bytes` begins with the numeric signature `magic`. */
function startsWithBytes(bytes: Uint8Array, magic: readonly number[]): boolean {
  if (bytes.length < magic.length) return false;
  for (let index = 0; index < magic.length; index += 1) {
    if (bytes[index] !== magic[index]) return false;
  }
  return true;
}

/** Segment flag values, from the XP3 specification. */
const SEGMENT_PLAIN = 0x00;
const SEGMENT_COMPRESSED = 0x07;

/**
 * Chunk tags.
 *
 * The reader assembles a tag from its four bytes in file order, so `'File'`
 * (bytes 46 69 6c 65) becomes 0x46696c65. Note that this is *not* the
 * little-endian packing used by the surrounding integer fields — XP3 mixes the
 * two, which is why the tag constants are spelled out rather than derived.
 */
const TAG_FILE = 0x46696c65; // 'File'
const TAG_INFO = 0x696e666f; // 'info'
const TAG_SEGM = 0x7365676d; // 'segm'
const TAG_ADLR = 0x61646c72; // 'adlr'

/** Every chunk tag an index chain may contain. */
const CHUNK_TAGS: ReadonlySet<number> = new Set([TAG_FILE, TAG_INFO, TAG_SEGM, TAG_ADLR]);

/** `info` flag bit marking an entry whose size field is key-encrypted. */
const INFO_FLAG_PROTECTED = 0x80000000;

interface Xp3Segment {
  readonly flags: number;
  readonly start: number;
  readonly size: number;
  readonly originalSize: number;
}

interface Xp3Record extends ArchiveEntry {
  readonly segments: readonly Xp3Segment[];
  /** Payload is XORed with the archive key when present. */
  readonly encryptedPayload: boolean;
}

export interface Xp3OpenOptions {
  /**
   * Per-game decryption key (a 64-bit little-endian seed). Only obtainable
   * from the game's own `startup.tjs`; without it, protected archives are
   * identified but cannot be read.
   */
  readonly key?: bigint;
}

/**
 * KiriKiri XP3 package reader.
 *
 * Layout: an 11-byte signature, a 64-bit index offset, then a recursive tree
 * of chunks (`File` → `info` / `segm` / `adlr`) encoded with XP3's
 * variable-length integers. Only the index is parsed up front, so opening a
 * multi-gigabyte package is instant.
 */
export class Xp3ArchiveReader extends BaseArchiveReader {
  readonly format = 'xp3';
  /** True when the archive held protected entries we could not decode. */
  readonly hasUnreadableEntries: boolean;

  private constructor(
    name: string,
    entries: readonly ArchiveEntry[],
    private readonly source: ByteSource,
    private readonly records: ReadonlyMap<string, Xp3Record>,
    private readonly key: bigint | null,
    hasUnreadableEntries: boolean,
  ) {
    super(name, entries, 32 * 1024 * 1024);
    this.hasUnreadableEntries = hasUnreadableEntries;
  }

  static async open(
    name: string,
    source: ByteSource,
    options: Xp3OpenOptions = {},
  ): Promise<Xp3ArchiveReader> {
    const header = await source.readRange(0, XP3_HEADER_BYTES);
    if (!startsWithBytes(header, XP3_SIGNATURE_BYTES)) {
      throw new Error('not an XP3 archive (bad magic)');
    }
    const view = new DataView(header.buffer, header.byteOffset, header.byteLength);
    const declared = Number(view.getBigUint64(XP3_INDEX_OFFSET_AT, true));

    // The header points at a short preamble, not straight at the chunk chain.
    const indexOffset = await findIndexStart(source, declared);

    const key = options.key ?? null;
    const records: Xp3Record[] = [];
    await readIndexChunks(source, indexOffset, records);

    // The `info` size field reveals whether the payload is XOR-obfuscated.
    const hasUnreadable = records.some((record) => record.encryptedPayload) && key === null;

    return new Xp3ArchiveReader(
      name,
      records.map(({ path, size }) => ({ path, size, isDirectory: false })),
      source,
      new Map(records.map((record) => [record.path, record])),
      key,
      hasUnreadable,
    );
  }

  static async probe(source: ByteSource): Promise<boolean> {
    try {
      const header = await source.readRange(0, XP3_SIGNATURE_BYTES.length);
      return startsWithBytes(header, XP3_SIGNATURE_BYTES);
    } catch {
      return false;
    }
  }

  protected async extract(entry: ArchiveEntry): Promise<Uint8Array> {
    const record = this.records.get(entry.path);
    if (!record) throw new Error(`xp3 entry missing: "${entry.path}"`);

    const chunks: Uint8Array[] = [];
    for (const segment of record.segments) {
      let raw = await this.source.readRange(segment.start, segment.size);
      if (record.encryptedPayload) {
        raw = xorDecrypt(raw, this.key);
      }
      const kind = segment.flags & SEGMENT_COMPRESSED;
      if (kind === SEGMENT_PLAIN) {
        chunks.push(raw);
      } else if (kind === SEGMENT_COMPRESSED) {
        chunks.push(await inflateZlib(raw));
      } else {
        throw new Error(
          `"${entry.path}" uses unsupported XP3 segment flags 0x${segment.flags.toString(16)}`,
        );
      }
    }

    if (chunks.length === 0) return new Uint8Array(0);
    if (chunks.length === 1) return chunks[0]!;
    let total = 0;
    for (const chunk of chunks) total += chunk.byteLength;
    const out = new Uint8Array(total);
    let offset = 0;
    for (const chunk of chunks) {
      out.set(chunk, offset);
      offset += chunk.byteLength;
    }
    return out;
  }
}

/**
 * Walks the top-level chunk list at `offset`.
 *
 * The index is a flat run of chunks; `File` nodes carry their own segment
 * tables, so no explicit nesting recursion is required.
 */
async function readIndexChunks(
  source: ByteSource,
  offset: number,
  records: Xp3Record[],
): Promise<void> {
  let cursor = offset;
  // A guard against a corrupt index looping forever.
  for (let guard = 0; guard < 1_000_000; guard += 1) {
    const head = await source.readRange(cursor, 12);
    if (head.byteLength < 12) return;
    const view = new DataView(head.buffer, head.byteOffset, head.byteLength);
    const { tag } = readVarOrLiteralTag(view, 0);
    const chunkSize = Number(view.getBigUint64(4, true));
    const bodyStart = cursor + 12;

    if (tag === TAG_FILE) {
      const body = await source.readRange(bodyStart, chunkSize);
      records.push(parseFileChunk(body));
    } else if (tag !== TAG_ADLR && tag !== TAG_INFO && tag !== TAG_SEGM) {
      // Unrecognised chunk: stop rather than mis-parse the rest of the index.
      return;
    }

    if (chunkSize <= 0) return;
    cursor = bodyStart + chunkSize;
  }
}

/**
 * Reads the four-character chunk tag.
 *
 * XP3 stores tags as four ASCII bytes in file order, so that is tried first.
 * Writers that instead emit the tag through the var-int path produce a
 * different value that decodes to four bytes as well; the caller's tag test
 * disambiguates, which is why this returns the width it consumed.
 */
function readVarOrLiteralTag(
  view: DataView,
  offset: number,
): { tag: number; next: number } {
  if (offset + 4 > view.byteLength) {
    return { tag: 0, next: Math.min(offset + 4, view.byteLength) };
  }
  const literal =
    ((view.getUint8(offset) << 24) |
      (view.getUint8(offset + 1) << 16) |
      (view.getUint8(offset + 2) << 8) |
      view.getUint8(offset + 3)) >>>
    0;

  // A real tag is four printable ASCII bytes; anything else is assumed to be a
  // var-int encoding of the same value.
  if (isPrintableTag(literal)) {
    return { tag: literal, next: offset + 4 };
  }
  const varInt = readXp3VarInt(view, offset);
  return { tag: varInt.value >>> 0, next: varInt.next };
}

/** True when all four bytes of `tag` are printable ASCII. */
function isPrintableTag(tag: number): boolean {
  return (
    (tag >>> 24) >= 0x20 &&
    (tag >>> 24) <= 0x7e &&
    ((tag >>> 16) & 0xff) >= 0x20 &&
    ((tag >>> 8) & 0xff) >= 0x20 &&
    (tag & 0xff) >= 0x20
  );
}

function parseFileChunk(rawBody: Uint8Array): Xp3Record {
  /**
   * Re-anchor the chunk to a zero-offset buffer.
   *
   * `ByteSource.readRange` returns a subarray, so `rawBody.byteOffset` is not
   * zero and a `DataView` over it would expose view-relative offsets while
   * `readUtf16le` indexes the array directly. Copying once removes that class
   * of mismatch entirely, and a chunk body is small relative to the archive.
   */
  const body = rawBody.slice();
  const view = new DataView(body.buffer, 0, body.byteLength);
  let cursor = 0;

  const next = (): number => {
    const result = readXp3VarInt(view, cursor);
    cursor = result.next;
    return result.value;
  };

  // `info` node: the tag, then its declared body length.
  const infoTag = readVarOrLiteralTag(view, cursor);
  if (infoTag.tag !== TAG_INFO) {
    throw new Error('unexpected XP3 chunk: expected an "info" node');
  }
  cursor = infoTag.next;
  const infoSize = next();
  const infoStart = cursor;
  const infoEnd = Math.min(infoStart + infoSize, view.byteLength);

  const flags = next();
  const nameLength = next();
  // A name length beyond the chunk means a corrupt or misunderstood `info`
  // node; refusing it keeps a malformed archive from crashing the reader with
  // an out-of-bounds access.
  if (nameLength * 2 > view.byteLength - cursor) {
    throw new Error(
      `XP3 "info" node declares an impossible name length (${nameLength} characters)`,
    );
  }
  const rawName = readUtf16le(body, cursor, nameLength);
  cursor += nameLength * 2;

  let size = 0n;
  if (flags & INFO_FLAG_PROTECTED) {
    // The size itself is key-obfuscated; the XOR keystream is self-synchronising
    // so we can still walk the structure, we just cannot trust the value.
    size = 0n;
  } else if (cursor + 8 <= view.byteLength) {
    size = view.getBigUint64(cursor, true);
  }

  // `info` may be padded, so always resume from its declared end.
  cursor = infoEnd;

  const segments: Xp3Segment[] = [];
  if (cursor + 4 <= view.byteLength) {
    const segmTag = readVarOrLiteralTag(view, cursor);
    if (segmTag.tag === TAG_SEGM) {
      cursor = segmTag.next;
      // The chunk length is metadata describing the segment table; the table
      // itself starts with the segment count, so the length is not skipped.
      const segmSize = readXp3VarInt(view, cursor);
      void segmSize;
      cursor = segmSize.next;
      const count = readXp3VarInt(view, cursor);
      cursor = count.next;
      for (let i = 0; i < count.value; i += 1) {
        if (cursor >= view.byteLength) break;
        const segFlags = readXp3VarInt(view, cursor);
        const start = readXp3VarInt(view, segFlags.next);
        const segSize = readXp3VarInt(view, start.next);
        const originalSize = readXp3VarInt(view, segSize.next);
        cursor = originalSize.next;
        segments.push({
          flags: segFlags.value,
          start: start.value,
          size: segSize.value,
          originalSize: originalSize.value,
        });
      }
    }
  }

  const encryptedPayload = (flags & INFO_FLAG_PROTECTED) !== 0;
  const resolvedSize = encryptedPayload
    ? segments.reduce((sum, segment) => sum + segment.originalSize, 0)
    : Number(size);

  return {
    path: rawName.replace(/\\/g, '/').replace(/^\/+/, ''),
    size: resolvedSize,
    isDirectory: false,
    segments,
    encryptedPayload,
  };
}

/**
 * XOR decryption used by "protected" XP3 entries.
 *
 * An 8-byte little-endian sum acts as the key; each byte is XORed with the low
 * byte of the sum, which is then advanced by the recovered plaintext byte.
 * Without the game's seed the payload is unreadable, so an absent key raises
 * rather than returning garbage.
 */
function xorDecrypt(bytes: Uint8Array, key: bigint | null): Uint8Array {
  if (key === null) {
    throw new Error('XP3 archive is encrypted but no decryption key is known');
  }
  const out = new Uint8Array(bytes.byteLength);
  let sum = key;
  for (let i = 0; i < bytes.byteLength; i += 1) {
    const plain = (bytes[i] ?? 0) ^ Number(sum & 0xffn);
    out[i] = plain;
    sum = (sum + BigInt(plain)) & 0xffffffffffffffffn;
  }
  return out;
}
