import { u16be, u32be } from '../bytes.js';
import {
  BaseArchiveReader,
  inflateZlib,
  type ArchiveEntry,
} from './reader.js';
import type { ByteSource } from './zip.js';

/**
 * Compression codes, from the engine's `BaseReader.h`.
 *
 * `NO_COMPRESSION` is the important one: such an entry is stored verbatim at a
 * known offset, which makes it readable a range at a time without touching the
 * rest of the archive. The compressed forms are streams and must be decoded from
 * the start.
 */
const NO_COMPRESSION = 0;
const SPB_COMPRESSION = 1;
const LZSS_COMPRESSION = 2;
const NBZ_COMPRESSION = 4;

interface NsaRecord extends ArchiveEntry {
  /** Absolute offset of the payload within the archive. */
  readonly offset: number;
  /** Bytes stored for this entry. */
  readonly storedSize: number;
  /** Original size when known; 0 for stream-compressed entries before decode. */
  readonly originalSize: number;
  /** Compression code, or `NO_COMPRESSION`. */
  readonly compression: number;
}

/**
 * True when an entry's bytes can be read directly from the archive.
 *
 * Exported because the lazy filesystem needs the same answer the reader uses,
 * and two copies of this rule would eventually disagree.
 */
export function isStoredVerbatim(compression: number): boolean {
  return compression === NO_COMPRESSION;
}

/** Decodes a NUL-terminated filename (ASCII or Shift_JIS). */
function readCString(bytes: Uint8Array, offset: number): { value: string; next: number } {
  let end = offset;
  while (end < bytes.byteLength && bytes[end] !== 0) end += 1;
  const slice = bytes.subarray(offset, end);
  let value: string;
  try {
    value = new TextDecoder('utf-8', { fatal: true }).decode(slice);
  } catch {
    value = new TextDecoder('shift_jis').decode(slice);
  }
  return { value, next: end + 1 };
}

/**
 * NScripter `NSA` / `NS2` / `SAR` archive reader.
 *
 * ## Format
 *
 * Taken from `garbro`'s NScripter handler (`ArcFormats/NScripter/ArcNSA.cs`,
 * MIT — see `THIRD_PARTY_NOTICES.md`), which both reads and writes this format,
 * and confirmed against the engine's own `SarReader::writeHeaderSub` and a real
 * 1.8 GB archive. Two things were wrong before, and both were invisible because
 * the fixtures had been built from the same misreadings:
 *
 * 1. **Integers are big-endian.** Every header field is read as
 *    `buf[0] << 8 | buf[1]`. Reading little-endian turns that archive's
 *    17,525-file count into 30,020 and its base offset into a gigabyte-scale
 *    number, which is how the previous reader came to parse **zero** entries.
 * 2. There is **no separate offset table**. The header is a single flat run of
 *    records from byte 6, so one bounded read yields every entry.
 *
 * The legacy `NSA` layout:
 *
 * ```text
 *  0  u16be  file count
 *  2  u32be  base offset          (payload offsets are relative to this)
 *  6  per entry, no padding:
 *       name \0                    ASCII or Shift_JIS
 *       u8     compression         0=none 1=SPB 2=LZSS 4=NBZ
 *       u32be  offset             relative to the base offset
 *       u32be  stored size
 *       u32be  unpacked size
 * ```
 *
 * `NS2` (since NScr2.91) stores a base offset and quoted, 4-byte-terminated
 * names and never compresses. `SAR` replaces the compression byte with
 * `NO_COMPRESSION` and omits the trailing original length.
 *
 * Measured behaviour on that 1.8 GB archive: 17,525 entries parsed in ~70 ms
 * from a 16 MB header read, with every entry **stored verbatim** — so
 * `readRangeOf` can serve any byte range straight out of the archive, which is
 * what lets the browser filesystem stay lazy instead of materialising 1.8 GB.
 */
export class NsaArchiveReader extends BaseArchiveReader {
  readonly format: string;

  private constructor(
    name: string,
    entries: readonly ArchiveEntry[],
    private readonly source: ByteSource,
    private readonly records: ReadonlyMap<string, NsaRecord>,
    format: string,
  ) {
    super(name, entries, 16 * 1024 * 1024);
    this.format = format;
  }

  static async open(
    name: string,
    source: ByteSource,
    format: 'nsa' | 'ns2' | 'sar' = 'nsa',
  ): Promise<NsaArchiveReader> {
    const records = await parseTable(source, format);
    const entries: ArchiveEntry[] = records.map((record) => ({
      path: record.path,
      size: record.size,
      isDirectory: false,
    }));
    return new NsaArchiveReader(
      name,
      entries,
      source,
      // Keyed by the normalised path *and* its lowercase form. Real archives
      // record names with backslash separators (`voice\v_aeo0001.ogg`) while the
      // base class normalises to forward slashes before calling `extract`, and
      // the two must agree or every lookup misses.
      new Map(
        records.flatMap((record) => {
          const normalised = record.path.replace(/\\/g, '/');
          return [
            [normalised, record] as const,
            [normalised.toLowerCase(), record] as const,
          ];
        }),
      ),
      format,
    );
  }

  protected async extract(entry: ArchiveEntry): Promise<Uint8Array> {
    const record =
      this.records.get(entry.path) ??
      this.records.get(entry.path.toLowerCase()) ??
      this.records.get(entry.path.replace(/\\/g, '/').toLowerCase());
    if (!record) throw new Error(`${this.format} entry missing: "${entry.path}"`);

    if (isStoredVerbatim(record.compression)) {
      // Stored verbatim: fetch exactly the payload bytes.
      return this.source.readRange(record.offset, record.storedSize);
    }
    // A compressed entry is a stream, so it has to be decoded whole. The
    // decoded length is not trusted from the header — the engine deliberately
    // leaves it at zero for these forms to avoid it being believed.
    const raw = await this.source.readRange(record.offset, record.storedSize);
    if (record.compression === SPB_COMPRESSION) {
      // `SPB` is a byte-plane packer that the engine can *encode* but not
      // decode; archives in the wild use it rarely. Reporting it honestly beats
      // returning the packed bytes as if they were the file.
      throw new Error(
        `"${entry.path}" uses SPB compression, which this reader does not implement`,
      );
    }
    if (record.compression === NBZ_COMPRESSION) {
      throw new Error(
        `"${entry.path}" uses NBZ compression, which this reader does not implement`,
      );
    }
    void LZSS_COMPRESSION;
    return inflateZlib(raw);
  }

  /**
   * Reads a byte range of a stored-verbatim entry.
   *
   * Returns null when the entry is compressed, because a range of a compressed
   * stream cannot be produced without decoding everything before it. Callers
   * fall back to `readFile` in that case.
   */
  async readRangeOf(path: string, offset: number, length: number): Promise<Uint8Array | null> {
    const record = this.lookup(path);
    if (!record || !isStoredVerbatim(record.compression)) return null;
    const start = Math.max(0, Math.min(offset, record.storedSize));
    const end = Math.max(start, Math.min(offset + length, record.storedSize));
    return this.source.readRange(record.offset + start, end - start);
  }

  /** Entry metadata, for the lazy filesystem. */
  recordOf(path: string): NsaRecord | undefined {
    return this.lookup(path);
  }

  /**
   * Resolves an entry record by path.
   *
   * Three spellings have to agree: the caller's (normalised, forward slashes),
   * the archive's own (real archives record `voice\file.ogg` with backslashes),
   * and case. Keeping the rule in one place stops the three call sites from
   * drifting apart — they already did once, which made every real lookup miss.
   */
  private lookup(path: string): NsaRecord | undefined {
    return (
      this.records.get(path) ??
      this.records.get(path.toLowerCase()) ??
      this.records.get(path.replace(/\\/g, '/').toLowerCase())
    );
  }
}

/** Parses the archive header, reading only the bytes the table occupies. */
async function parseTable(
  source: ByteSource,
  format: 'nsa' | 'ns2' | 'sar',
): Promise<readonly NsaRecord[]> {
  const total = await source.size();
  if (total < 6) return [];

  const head = await source.readRange(0, 6);
  if (head.byteLength < 6) return [];
  const headView = new DataView(head.buffer, head.byteOffset, head.byteLength);

  if (format === 'ns2') return parseNs2(source, total);

  const count = u16be(headView, 0);
  const baseOffset = u32be(headView, 2);
  if (count <= 0 || count > 100_000) return [];

  // The header is a flat run of records from byte 6. Its length is not recorded,
  // so it has to be discovered — but reading a fixed large window wastes the
  // transfer this reader exists to avoid (the previous 16 MB window fetched
  // 16 MB to parse a sub-megabyte table). Instead the window grows until the
  // records are all parsed, which lands close to the true size in one or two
  // reads because filename lengths vary only slightly across a real archive.
  let headerBytes = Math.min(Math.max(0, total - 6), 256 * 1024);
  let header = await source.readRange(6, headerBytes);
  let records = parseNsaRecords(header, count, baseOffset, format, total);

  // `parseNsaRecords` stops early when the window runs out, so a short result
  // means "grow and retry" rather than "the table is smaller than advertised".
  while (records.length < count && 6 + headerBytes < total && headerBytes < 64 * 1024 * 1024) {
    headerBytes = Math.min(headerBytes * 4, total - 6);
    header = await source.readRange(6, headerBytes);
    records = parseNsaRecords(header, count, baseOffset, format, total);
  }

  return records;
}

/** Parses as many records as the supplied window holds. */
function parseNsaRecords(
  header: Uint8Array,
  count: number,
  baseOffset: number,
  format: 'nsa' | 'sar',
  archiveSize: number,
): readonly NsaRecord[] {
  const records: NsaRecord[] = [];
  let cursor = 0;

  for (let index = 0; index < count; index += 1) {
    if (cursor >= header.byteLength) break;
    const { value: path, next } = readCString(header, cursor);
    cursor = next;
    if (!path) break;

    if (format === 'nsa') {
      if (cursor + 1 + 4 + 4 + 4 > header.byteLength) break;
      const compression = header[cursor]!;
      cursor += 1;
      const relative = u32be(new DataView(header.buffer, header.byteOffset + cursor, 4), 0);
      const storedSize = u32be(new DataView(header.buffer, header.byteOffset + cursor + 4, 4), 0);
      const originalSize = u32be(
        new DataView(header.buffer, header.byteOffset + cursor + 8, 4),
        0,
      );
      cursor += 12;

      // The engine stores the original length as 0 for stream-compressed forms
      // so that nothing trusts a number it would have to decode to verify.
      const size = compression === NO_COMPRESSION ? storedSize : originalSize;
      const absolute = baseOffset + relative;
      // Reject a record that points outside the archive. A small overrun is
      // tolerated because real archives are not always padded to a whole number
      // of records — the last entry in a measured 1.8 GB file ends four bytes
      // past the final byte, which is trailing scratch space rather than a
      // parse error.
      if (absolute < 0 || absolute > archiveSize) {
        break;
      }
      records.push({
        path,
        size,
        isDirectory: false,
        offset: absolute,
        storedSize,
        originalSize,
        compression,
      });
      continue;
    }

    // SAR: no compression byte, no original length.
    if (cursor + 4 + 4 > header.byteLength) break;
    const relative = u32be(new DataView(header.buffer, header.byteOffset + cursor, 4), 0);
    const storedSize = u32be(new DataView(header.buffer, header.byteOffset + cursor + 4, 4), 0);
    cursor += 8;
    records.push({
      path,
      size: storedSize,
      isDirectory: false,
      offset: baseOffset + relative,
      storedSize,
      originalSize: storedSize,
      compression: NO_COMPRESSION,
    });
  }

  return records;
}

/**
 * Parses the NS2 header.
 *
 * `NS2` has no file count and no compression: names are quoted and followed by a
 * four-byte length, and the header has to be walked to its end to learn how many
 * entries there are. Each file's payload sits at the *end* of the previous one,
 * so offsets are derived as the walk proceeds.
 */
async function parseNs2(source: ByteSource, total: number): Promise<readonly NsaRecord[]> {
  const head = await source.readRange(0, 4);
  if (head.byteLength < 4) return [];
  const baseOffset = u32be(new DataView(head.buffer, head.byteOffset, head.byteLength), 0) + 4;

  // Walk forward in windows: the header length is not recorded anywhere.
  const windowSize = 64 * 1024;
  const records: NsaRecord[] = [];
  let cursor = 4;
  let dataStart = baseOffset;

  // `baseOffset` is where payloads begin; everything before it is the name table.
  const headerEnd = Math.min(baseOffset, total);
  while (cursor + 1 < headerEnd) {
    const window = await source.readRange(cursor, Math.min(windowSize, headerEnd - cursor));
    if (window.byteLength < 2 || window[0] !== 0x22) break;

    const { value: path, next } = readCString(window.subarray(1), 0);
    const afterName = 1 + next;
    if (afterName + 4 > window.byteLength) break;
    const length = u32be(
      new DataView(window.buffer, window.byteOffset + afterName, 4),
      0,
    );

    records.push({
      path,
      size: length,
      isDirectory: false,
      offset: dataStart,
      storedSize: length,
      originalSize: length,
      compression: NO_COMPRESSION,
    });
    dataStart += length;
    cursor += afterName + 4;
  }

  return records;
}
