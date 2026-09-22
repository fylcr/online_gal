import type { GameSource } from '@online-gal/engine-api';
import {
  matchesAscii,
  u16le,
  u32le,
  u64leToNumber,
  u64le,
} from '../bytes.js';
import {
  BaseArchiveReader,
  inflateRaw,
  type ArchiveEntry,
} from './reader.js';

const SIG_EOCD = 'PK\u0005\u0006';
const SIG_EOCD64 = 'PK\u0006\u0006';
const SIG_EOCD64_LOCATOR = 'PK\u0006\u0007';
const SIG_CENTRAL = 'PK\u0001\u0002';
const SIG_LOCAL = 'PK\u0003\u0004';

const FLAG_ENCRYPTED = 0x0001;
const FLAG_UTF8 = 0x0800;

const METHOD_STORE = 0;
const METHOD_DEFLATE = 8;

interface ZipEntryRecord extends ArchiveEntry {
  readonly method: number;
  readonly flags: number;
  readonly localHeaderOffset: number;
  readonly compressedSize: number;
}

/** Minimal read-side access to the archive bytes. */
export interface ByteSource {
  size(): Promise<number>;
  readRange(offset: number, length: number): Promise<Uint8Array>;
}

/** Adapts a `GameSource` file into a `ByteSource`. */
export function gameSourceByteSource(
  source: GameSource,
  path: string,
): ByteSource {
  return {
    async size() {
      const declared = await source.size(path);
      if (declared >= 0) return declared;
      return (await source.readFile(path)).byteLength;
    },
    async readRange(offset, length) {
      if (source.readRange) return source.readRange(path, offset, length);
      const whole = await source.readFile(path);
      return whole.subarray(offset, offset + length);
    },
  };
}

/** Adapts a `Blob`. */
export function blobByteSource(blob: Blob): ByteSource {
  return {
    async size() {
      return blob.size;
    },
    async readRange(offset, length) {
      return new Uint8Array(
        await blob.slice(offset, offset + length).arrayBuffer(),
      );
    },
  };
}

/**
 * Decodes a ZIP entry name.
 *
 * When the UTF-8 flag is absent the bytes are officially CP437; in practice
 * Japanese Windows-authored archives are Shift_JIS. UTF-8 is tried strictly
 * first and Shift_JIS is the fallback, which resolves the overwhelming
 * majority of real archives without shipping a CP437 table.
 */
function decodeZipName(bytes: Uint8Array, flags: number): string {
  if (flags & FLAG_UTF8) {
    return new TextDecoder('utf-8').decode(bytes);
  }
  try {
    return new TextDecoder('utf-8', { fatal: true }).decode(bytes);
  } catch {
    try {
      return new TextDecoder('shift_jis').decode(bytes);
    } catch {
      return new TextDecoder('utf-8').decode(bytes);
    }
  }
}

/**
 * ZIP reader (also covers the `data.zip`/`*.zip` packages some engines ship).
 *
 * ZIP64 is supported for archives over 4 GB, which matters because visual
 * novel packages regularly exceed that.
 */
export class ZipArchiveReader extends BaseArchiveReader {
  readonly format = 'zip';

  private constructor(
    name: string,
    entries: readonly ArchiveEntry[],
    private readonly source: ByteSource,
    private readonly records: ReadonlyMap<string, ZipEntryRecord>,
  ) {
    super(name, entries);
  }

  static async open(
    name: string,
    source: ByteSource,
  ): Promise<ZipArchiveReader> {
    const total = await source.size();
    const parsed = await parseCentralDirectory(source, total);
    const entries: ArchiveEntry[] = [];
    const records = new Map<string, ZipEntryRecord>();
    for (const record of parsed) {
      const isDirectory = record.path.endsWith('/');
      entries.push({
        path: record.path,
        size: record.size,
        isDirectory,
      });
      if (!isDirectory) records.set(record.path, record);
    }
    return new ZipArchiveReader(name, entries, source, records);
  }

  /** Cheap magic-number check so the importer can pick a reader. */
  static async probe(source: ByteSource): Promise<boolean> {
    try {
      if ((await source.size()) < 4) return false;
      const head = await source.readRange(0, 4);
      return (
        matchesAscii(head, SIG_LOCAL) ||
        matchesAscii(head, 'PK\u0005\u0006') ||
        matchesAscii(head, 'PK\u0007\u0008')
      );
    } catch {
      return false;
    }
  }

  protected async extract(entry: ArchiveEntry): Promise<Uint8Array> {
    const record = this.records.get(entry.path);
    if (!record) throw new Error(`zip entry missing: "${entry.path}"`);
    if (record.flags & FLAG_ENCRYPTED) {
      throw new Error(
        `"${entry.path}" is encrypted; password-protected ZIP archives are not supported`,
      );
    }

    const header = await this.source.readRange(record.localHeaderOffset, 30);
    if (!matchesAscii(header, SIG_LOCAL)) {
      throw new Error(`corrupt local header for "${entry.path}"`);
    }
    const nameLength = u16le(new DataView(header.buffer, header.byteOffset, 30), 26);
    const extraLength = u16le(new DataView(header.buffer, header.byteOffset, 30), 28);
    const dataOffset = record.localHeaderOffset + 30 + nameLength + extraLength;

    const compressed = await this.source.readRange(
      dataOffset,
      record.compressedSize,
    );

    if (record.method === METHOD_STORE) return compressed;
    if (record.method === METHOD_DEFLATE) return inflateRaw(compressed);
    throw new Error(
      `"${entry.path}" uses unsupported ZIP compression method ${record.method}`,
    );
  }
}

async function parseCentralDirectory(
  source: ByteSource,
  total: number,
): Promise<readonly ZipEntryRecord[]> {
  if (total < 22) throw new Error('file is too small to be a ZIP archive');

  // The EOCD sits at the end but may be followed by a comment of up to 64 KiB.
  const tailLength = Math.min(total, 22 + 0xffff);
  const tail = await source.readRange(total - tailLength, tailLength);
  const view = new DataView(tail.buffer, tail.byteOffset, tail.byteLength);

  let eocd = -1;
  for (let i = tail.byteLength - 22; i >= 0; i -= 1) {
    if (matchesAscii(tail, SIG_EOCD, i)) {
      eocd = i;
      break;
    }
  }
  if (eocd === -1) throw new Error('not a ZIP archive (no end-of-central-directory record)');

  let entryCount = u16le(view, eocd + 10);
  let centralSize = u32le(view, eocd + 12);
  let centralOffset = u32le(view, eocd + 16);

  // ZIP64: the 32-bit fields are saturated and the real values live in the
  // ZIP64 EOCD record, located through the locator that precedes the EOCD.
  const needsZip64 =
    entryCount === 0xffff || centralSize === 0xffffffff || centralOffset === 0xffffffff;
  if (needsZip64 && eocd >= 20 && matchesAscii(tail, SIG_EOCD64_LOCATOR, eocd - 20)) {
    const locator = new DataView(tail.buffer, tail.byteOffset + eocd - 20, 20);
    const eocd64Offset = u64leToNumber(locator, 8);
    const header = await source.readRange(eocd64Offset, 56);
    if (!matchesAscii(header, SIG_EOCD64)) {
      throw new Error('corrupt ZIP64 end-of-central-directory record');
    }
    const z64 = new DataView(header.buffer, header.byteOffset, 56);
    entryCount = u64leToNumber(z64, 32);
    centralSize = u64leToNumber(z64, 40);
    centralOffset = u64leToNumber(z64, 48);
  }

  const central = await source.readRange(centralOffset, centralSize);
  const centralView = new DataView(
    central.buffer,
    central.byteOffset,
    central.byteLength,
  );

  const records: ZipEntryRecord[] = [];
  let cursor = 0;
  for (let i = 0; i < entryCount; i += 1) {
    if (cursor + 46 > central.byteLength) break;
    if (!matchesAscii(central, SIG_CENTRAL, cursor)) break;

    const flags = u16le(centralView, cursor + 8);
    const method = u16le(centralView, cursor + 10);
    let compressedSize = u32le(centralView, cursor + 20);
    let size = u32le(centralView, cursor + 24);
    const nameLength = u16le(centralView, cursor + 28);
    const extraLength = u16le(centralView, cursor + 30);
    const commentLength = u16le(centralView, cursor + 32);
    let localHeaderOffset = u32le(centralView, cursor + 42);

    const nameBytes = central.subarray(cursor + 46, cursor + 46 + nameLength);
    const name = decodeZipName(nameBytes, flags);

    // Walk the extra field for the ZIP64 record (id 0x0001).
    let extraCursor = cursor + 46 + nameLength;
    const extraEnd = extraCursor + extraLength;
    while (extraCursor + 4 <= extraEnd) {
      const fieldId = u16le(centralView, extraCursor);
      const fieldSize = u16le(centralView, extraCursor + 2);
      if (fieldId === 0x0001) {
        let field = extraCursor + 4;
        const fieldEnd = field + fieldSize;
        if (size === 0xffffffff && field + 8 <= fieldEnd) {
          size = u64leToNumber(centralView, field);
          field += 8;
        }
        if (compressedSize === 0xffffffff && field + 8 <= fieldEnd) {
          compressedSize = u64leToNumber(centralView, field);
          field += 8;
        }
        if (localHeaderOffset === 0xffffffff && field + 8 <= fieldEnd) {
          localHeaderOffset = u64leToNumber(centralView, field);
        }
        break;
      }
      extraCursor += 4 + fieldSize;
    }

    records.push({
      path: name,
      size,
      isDirectory: name.endsWith('/'),
      method,
      flags,
      localHeaderOffset,
      compressedSize,
    });

    cursor += 46 + nameLength + extraLength + commentLength;
  }

  return records;
}

/** Reports whether a ZIP64 EOCD marker exists, for diagnostics. */
export function hasZip64Marker(bytes: Uint8Array): boolean {
  if (bytes.byteLength < 20) return false;
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  return u64le(view, 0) > 0n;
}
