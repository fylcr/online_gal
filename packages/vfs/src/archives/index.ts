import { basename, extname } from '../path.js';
import { ArchiveGameSource } from '../sources.js';
import { inflateRaw, inflateZlib, type ArchiveReader } from './reader.js';
import { AsarArchiveReader } from './asar.js';
import { NsaArchiveReader } from './nsa.js';
import { Xp3ArchiveReader } from './xp3.js';
import {
  blobByteSource,
  gameSourceByteSource,
  ZipArchiveReader,
  type ByteSource,
} from './zip.js';

export type { ArchiveReader, ArchiveEntry } from './reader.js';
export { ArchiveIndex, BaseArchiveReader, inflateRaw, inflateZlib } from './reader.js';
export { ZipArchiveReader, blobByteSource, gameSourceByteSource } from './zip.js';
export type { ByteSource } from './zip.js';
export { Xp3ArchiveReader } from './xp3.js';
export type { Xp3OpenOptions } from './xp3.js';
export { NsaArchiveReader } from './nsa.js';
export { AsarArchiveReader } from './asar.js';

/** Archive extensions the importer should treat as packages, not files. */
export const ARCHIVE_EXTENSIONS = [
  '.xp3',
  '.zip',
  '.nsa',
  '.ns2',
  '.sar',
  '.asar',
] as const;

export interface OpenArchiveOptions {
  /** XP3 decryption key, when the game's `startup.tjs` exposed one. */
  readonly xp3Key?: bigint;
}

/** Opens an archive by sniffing its magic number, then falling back to its extension. */
export async function openArchive(
  name: string,
  source: ByteSource,
  options: OpenArchiveOptions = {},
): Promise<ArchiveReader> {
  const extension = extname(name);

  // Magic numbers first: extensions lie, especially in this ecosystem.
  if (await Xp3ArchiveReader.probe(source)) {
    return Xp3ArchiveReader.open(name, source, { key: options.xp3Key });
  }
  if (await ZipArchiveReader.probe(source)) {
    return ZipArchiveReader.open(name, source);
  }

  if (extension === '.asar') {
    return AsarArchiveReader.open(name, source);
  }
  if (extension === '.nsa' || extension === '.ns2' || extension === '.sar') {
    return NsaArchiveReader.open(
      name,
      source,
      extension.slice(1) as 'nsa' | 'ns2' | 'sar',
    );
  }

  throw new Error(
    `unsupported archive format for "${basename(name)}" (${extension || 'no extension'})`,
  );
}

/** Convenience wrapper for a `Blob`/`File`. */
export async function openBlobArchive(
  blob: Blob,
  name = 'archive',
  options: OpenArchiveOptions = {},
): Promise<ArchiveReader> {
  return openArchive(name, blobByteSource(blob), options);
}

/** Convenience wrapper for a file inside a `GameSource`. */
export async function openGameSourceArchive(
  gameSource: import('@online-gal/engine-api').GameSource,
  path: string,
  options: OpenArchiveOptions = {},
): Promise<ArchiveReader> {
  return openArchive(
    basename(path),
    gameSourceByteSource(gameSource, path),
    options,
  );
}

/** Wraps a reader as a `GameSource` for the runtime layer. */
export function archiveToGameSource(reader: ArchiveReader) {
  return new ArchiveGameSource(reader.name, reader);
}
