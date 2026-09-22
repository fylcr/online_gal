import type { GameSource } from '@online-gal/engine-api';
import { ArchiveGameSource } from '../sources.js';
import { dirname, normaliseDir, normalisePath } from '../path.js';

/** One entry inside an archive. */
export interface ArchiveEntry {
  /** Full path inside the archive, normalised, no leading slash. */
  readonly path: string;
  /** Uncompressed size in bytes, or -1 when unknown. */
  readonly size: number;
  /** True for explicit directory records. */
  readonly isDirectory: boolean;
}

/**
 * Read interface every archive format implements.
 *
 * Reads are lazy: constructing a reader parses only the index, never the
 * payload. For a multi-gigabyte XP3 that is the difference between instant and
 * unusable.
 */
export interface ArchiveReader {
  readonly name: string;
  readonly format: string;
  list(dir?: string): readonly string[];
  has(path: string): boolean;
  isDirectory(path: string): boolean;
  readFile(path: string): Promise<Uint8Array>;
  size(path: string): number;
  /** Sum of all uncompressed entry sizes. */
  totalSize(): number;
  entryCount(): number;
}

/** Builds an index from a flat entry list, modelling directories implicitly. */
export class ArchiveIndex {
  private readonly files = new Map<string, ArchiveEntry>();
  private readonly lower = new Map<string, string>();
  private readonly byBasename = new Map<string, string[]>();
  private readonly dirs = new Set<string>(['']);

  constructor(entries: readonly ArchiveEntry[]) {
    for (const entry of entries) {
      const path = normalisePath(entry.path);
      if (!path) continue;
      if (entry.isDirectory || path.endsWith('/')) {
        this.dirs.add(path.replace(/\/$/, ''));
        this.registerAncestors(path);
        continue;
      }
      const record: ArchiveEntry = { ...entry, path };
      this.files.set(path, record);
      this.lower.set(path.toLowerCase(), path);
      const base = path.slice(path.lastIndexOf('/') + 1).toLowerCase();
      const bucket = this.byBasename.get(base);
      if (bucket) bucket.push(path);
      else this.byBasename.set(base, [path]);
      this.registerAncestors(path);
    }
  }

  private registerAncestors(path: string): void {
    let parent = dirname(path);
    while (parent) {
      if (this.dirs.has(parent)) break;
      this.dirs.add(parent);
      parent = dirname(parent);
    }
  }

  /**
   * Resolves a requested path to a stored one.
   *
   * Archives from the visual-novel world are routinely inconsistent about
   * case, so resolution falls back to a case-insensitive match and finally to
   * a unique basename match — the same leniency the native runtimes apply.
   */
  resolve(path: string): ArchiveEntry | undefined {
    const key = normalisePath(path);
    const exact = this.files.get(key);
    if (exact) return exact;
    const lowered = this.lower.get(key.toLowerCase());
    if (lowered) return this.files.get(lowered);
    const base = key.slice(key.lastIndexOf('/') + 1).toLowerCase();
    const candidates = this.byBasename.get(base);
    if (candidates && candidates.length === 1) {
      return this.files.get(candidates[0]!);
    }
    return undefined;
  }

  has(path: string): boolean {
    return this.resolve(path) !== undefined;
  }

  isDirectory(path: string): boolean {
    const key = normaliseDir(path);
    if (key === '') return true;
    if (this.files.has(key)) return false;
    return this.dirs.has(key);
  }

  list(dir = ''): readonly string[] {
    const key = normaliseDir(dir);
    const prefix = key ? `${key}/` : '';
    const out = new Set<string>();
    for (const path of this.files.keys()) {
      if (!path.startsWith(prefix)) continue;
      const rest = path.slice(prefix.length);
      if (!rest) continue;
      const slash = rest.indexOf('/');
      out.add(slash === -1 ? rest : `${rest.slice(0, slash)}/`);
    }
    for (const candidate of this.dirs) {
      if (!candidate || !candidate.startsWith(prefix)) continue;
      const rest = candidate.slice(prefix.length);
      if (!rest) continue;
      const slash = rest.indexOf('/');
      out.add(slash === -1 ? `${rest}/` : `${rest.slice(0, slash)}/`);
    }
    return [...out].sort();
  }

  get count(): number {
    return this.files.size;
  }

  sumSize(): number {
    let total = 0;
    for (const entry of this.files.values()) {
      if (entry.size > 0) total += entry.size;
    }
    return total;
  }

  paths(): readonly string[] {
    return [...this.files.keys()].sort();
  }
}

/**
 * Base class holding the shared index, the decompression cache and the
 * path-resolution policy.
 */
export abstract class BaseArchiveReader implements ArchiveReader {
  protected readonly index: ArchiveIndex;
  abstract readonly format: string;
  private readonly cache = new Map<string, Uint8Array>();
  private cacheBytes = 0;

  constructor(
    readonly name: string,
    entries: readonly ArchiveEntry[],
    protected readonly cacheLimitBytes = 64 * 1024 * 1024,
  ) {
    this.index = new ArchiveIndex(entries);
  }

  list(dir = ''): readonly string[] {
    return this.index.list(dir);
  }

  has(path: string): boolean {
    return this.index.has(path);
  }

  isDirectory(path: string): boolean {
    return this.index.isDirectory(path);
  }

  size(path: string): number {
    return this.index.resolve(path)?.size ?? -1;
  }

  totalSize(): number {
    return this.index.sumSize();
  }

  entryCount(): number {
    return this.index.count;
  }

  paths(): readonly string[] {
    return this.index.paths();
  }

  async readFile(path: string): Promise<Uint8Array> {
    const entry = this.index.resolve(path);
    if (!entry) throw new Error(`no such entry in ${this.format}: "${path}"`);
    const cached = this.cache.get(entry.path);
    if (cached) return cached;
    const data = await this.extract(entry);
    if (data.byteLength <= this.cacheLimitBytes) {
      if (this.cacheBytes + data.byteLength > this.cacheLimitBytes) {
        // Simple eviction: drop everything rather than track LRU order.
        this.cache.clear();
        this.cacheBytes = 0;
      }
      this.cache.set(entry.path, data);
      this.cacheBytes += data.byteLength;
    }
    return data;
  }

  /** Decompresses a single entry. Implemented per format. */
  protected abstract extract(entry: ArchiveEntry): Promise<Uint8Array>;

  /** Exposes the resolved (real, case-correct) path of an entry. */
  protected resolveEntry(path: string): ArchiveEntry | undefined {
    return this.index.resolve(path);
  }

  asGameSource(): GameSource {
    return new ArchiveGameSource(this.name, this);
  }
}

/** Inflates a raw DEFLATE stream using the platform's `DecompressionStream`. */
export async function inflateRaw(data: Uint8Array): Promise<Uint8Array> {
  const stream = new Blob([data as BlobPart])
    .stream()
    .pipeThrough(new DecompressionStream('deflate-raw'));
  return new Uint8Array(await new Response(stream).arrayBuffer());
}

/** Inflates a zlib-wrapped stream. */
export async function inflateZlib(data: Uint8Array): Promise<Uint8Array> {
  const stream = new Blob([data as BlobPart])
    .stream()
    .pipeThrough(new DecompressionStream('deflate'));
  return new Uint8Array(await new Response(stream).arrayBuffer());
}
