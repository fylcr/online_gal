import type { GameSource, GameSourceCapabilities } from '@online-gal/engine-api';

import { normaliseDir, normalisePath } from './path.js';
import type { ByteSource } from './archives/zip.js';
import type { ArchiveReader } from './archives/reader.js';

/**
 * A file whose bytes are fetched on demand.
 *
 * The shape is the one AetherKiri's browser host settled on — an id plus
 * `offset` and `length` against a `Blob` — because it is the smallest contract
 * that can serve a multi-gigabyte game without ever holding it whole. A caller
 * asks for exactly the bytes it needs and the backing store decides how to
 * produce them: a slice for a local file, a range request over HTTP, or a window
 * inside an archive.
 *
 * ## Why this exists
 *
 * Materialising a visual novel is not viable. The games this project targets
 * carry 2–6 GB of archives, and the ONS runtime already exhausted a 4 GB heap
 * trying; the upstream WASM build has the same wall, which is why its web host
 * mounts `Blob`s and reads ranges instead of preloading. This is that design,
 * with the bookkeeping needed to prove the reads are actually lazy.
 */
export interface LazyFile {
  /** Size in bytes. */
  size(): Promise<number>;
  /**
   * Reads `length` bytes at `offset`.
   *
   * An offset at or past the end yields an empty array rather than an error:
   * callers routinely probe past the end, and throwing there would be noise. A
   * short result means end of file, never a partial failure.
   */
  read(offset: number, length: number): Promise<Uint8Array>;
  /** Optional modification time in milliseconds, for manifests. */
  mtimeMs?(): number;
}

/**
 * A registered file: an id plus its metadata.
 *
 * Mirrors AetherKiri's `fileEntry`. The id is deliberately distinct from the
 * path — paths are rewritten by normalisation and shadowed by later mounts,
 * while an id stays valid for the lifetime of the mount, which is what an engine
 * holding a file handle needs.
 */
export interface LazyFileEntry {
  readonly fileId: number;
  readonly path: string;
  readonly size: number;
  readonly mtimeMs: number;
}

/** Counters that make laziness a checkable property rather than a claim. */
export interface LazyFileStats {
  /** Number of `read` calls served. */
  readonly reads: number;
  /** Bytes actually produced across all reads. */
  readonly bytesRead: number;
  /** Bytes an implementation that loaded whole files would have produced. */
  readonly bytesIfEager: number;
  /** Registered file count. */
  readonly fileCount: number;
}

/** A `LazyFile` over a range-capable byte source: a Blob, an HTTP resource. */
export class RangedLazyFile implements LazyFile {
  constructor(
    private readonly source: ByteSource,
    private readonly knownSize?: number,
    private readonly modifiedAt?: number,
  ) {}

  async size(): Promise<number> {
    return this.knownSize ?? this.source.size();
  }

  async read(offset: number, length: number): Promise<Uint8Array> {
    const total = await this.size();
    const start = Math.max(0, Math.min(offset, total));
    const end = Math.max(start, Math.min(offset + length, total));
    if (end === start) return new Uint8Array(0);
    return this.source.readRange(start, end - start);
  }

  mtimeMs(): number {
    return this.modifiedAt ?? Date.now();
  }
}

/** A `LazyFile` over an in-memory buffer; reads slice it without copying. */
export class BufferLazyFile implements LazyFile {
  constructor(
    private readonly bytes: Uint8Array,
    private readonly modifiedAt?: number,
  ) {}

  async size(): Promise<number> {
    return this.bytes.byteLength;
  }

  async read(offset: number, length: number): Promise<Uint8Array> {
    const start = Math.max(0, Math.min(offset, this.bytes.byteLength));
    const end = Math.max(start, Math.min(offset + length, this.bytes.byteLength));
    return this.bytes.subarray(start, end);
  }

  mtimeMs(): number {
    return this.modifiedAt ?? Date.now();
  }
}

/**
 * A `LazyFile` that is one entry inside an archive.
 *
 * When the archive can produce the entry as a byte range — which every entry in
 * a real NSA can, because they are stored verbatim — the read goes straight to
 * the source and nothing is decoded. Otherwise it decodes the entry once and
 * serves later windows from that.
 *
 * This is what makes archives usable in a browser. A 1.8 GB NSA exposes 17,525
 * independently readable files without ever being loaded, which is the same
 * property `Blob.slice` gives loose files.
 */
export class ArchiveEntryLazyFile implements LazyFile {
  private decoded: Uint8Array | null = null;

  constructor(
    private readonly reader: ArchiveReader,
    private readonly entryPath: string,
    private readonly entrySize: number,
    /**
     * Range reader for a stored entry, when the enclosing archive offers one.
     * Returning null means "this entry cannot be sliced" and triggers the
     * decode-once path.
     */
    private readonly ranged?: (
      path: string,
      offset: number,
      length: number,
    ) => Promise<Uint8Array | null>,
  ) {}

  async size(): Promise<number> {
    return this.entrySize;
  }

  async read(offset: number, length: number): Promise<Uint8Array> {
    if (this.ranged) {
      const window = await this.ranged(this.entryPath, offset, length);
      if (window !== null) return window;
    }
    if (!this.decoded) this.decoded = await this.reader.readFile(this.entryPath);
    const start = Math.max(0, Math.min(offset, this.decoded.byteLength));
    const end = Math.max(start, Math.min(offset + length, this.decoded.byteLength));
    return this.decoded.subarray(start, end);
  }
}

/**
 * The unified filesystem.
 *
 * Mounts are consulted in reverse, so a later mount shadows an earlier one — the
 * convention a patch or mod layer depends on. Paths are normalised on the way in
 * and on the way out, so `a\b`, `./a/b` and `a/b` all reach the same entry, which
 * real archives need because they record names with backslashes.
 *
 * Reads are counted rather than assumed: `stats()` compares the bytes actually
 * produced against what an eager implementation would have produced, so the
 * lazy claim is testable.
 */
export class LazyFileSystem implements GameSource {
  readonly name: string;
  readonly capabilities: GameSourceCapabilities = {
    list: true,
    randomAccess: true,
    writable: false,
  };

  private readonly mounts: { mountPoint: string; files: Map<string, number> }[] = [];
  private readonly entriesById = new Map<number, LazyFileEntry>();
  private readonly filesById = new Map<number, LazyFile>();
  private nextId = 1;
  private reads = 0;
  private bytesRead = 0;

  constructor(name = 'lazy') {
    this.name = name;
  }

  /** Registers a mount and returns the entries it created. */
  mount(
    mountPoint: string,
    entries: readonly { path: string; file: LazyFile }[],
  ): readonly LazyFileEntry[] {
    const point = normaliseDir(mountPoint);
    const files = new Map<string, number>();
    const created: LazyFileEntry[] = [];

    for (const entry of entries) {
      const relative = normalisePath(entry.path);
      if (!relative || relative.endsWith('/')) continue;
      const full = point ? `${point}/${relative}` : relative;
      // A duplicate inside one mount replaces the earlier one rather than
      // producing an entry that can never be reached.
      const existing = files.get(relative);
      if (existing !== undefined) {
        this.filesById.set(existing, entry.file);
        continue;
      }
      const fileId = this.nextId;
      this.nextId += 1;
      const record: LazyFileEntry = {
        fileId,
        path: full,
        // Sizes are async and `mount` is not, so the manifest fills these in.
        size: -1,
        mtimeMs: entry.file.mtimeMs ? entry.file.mtimeMs() : Date.now(),
      };
      this.entriesById.set(fileId, record);
      this.filesById.set(fileId, entry.file);
      // Keyed by the path *relative to the mount*, so a lookup can normalise the
      // caller's path once and hit the right mount directly.
      files.set(relative, fileId);
      created.push(record);
    }

    this.mounts.push({ mountPoint: point, files });
    return created;
  }

  /** Registers one file at the root, returning its entry. */
  addFile(path: string, file: LazyFile): LazyFileEntry {
    const created = this.mount('', [{ path, file }]);
    const entry = created[0];
    if (!entry) throw new Error(`could not register "${path}"`);
    return entry;
  }

  /** Resolves a path to its entry, honouring mount order. */
  resolve(path: string): LazyFileEntry | undefined {
    const needle = normalisePath(path).toLowerCase();
    if (!needle) return undefined;
    for (let index = this.mounts.length - 1; index >= 0; index -= 1) {
      const mount = this.mounts[index]!;
      const point = mount.mountPoint.toLowerCase();
      // Try the path as written, then as relative to this mount's prefix, so a
      // caller can address a mounted game by either its full or its inner path.
      const candidates = [needle];
      if (point && needle.startsWith(`${point}/`)) {
        candidates.push(needle.slice(point.length + 1));
      }
      for (const candidate of candidates) {
        const id = mount.files.get(candidate);
        if (id !== undefined) return this.entriesById.get(id);
      }
    }
    return undefined;
  }

  /** Reads a window of a file by id — the primitive an engine host calls. */
  async readAt(fileId: number, offset: number, length: number): Promise<Uint8Array> {
    const file = this.filesById.get(fileId);
    if (!file) throw new Error(`unknown fileId ${fileId}`);
    const bytes = await file.read(offset, length);
    this.reads += 1;
    this.bytesRead += bytes.byteLength;
    return bytes;
  }

  /** Reads a whole file by id. Materialises it, so prefer {@link readAt}. */
  async readAll(fileId: number): Promise<Uint8Array> {
    const file = this.filesById.get(fileId);
    if (!file) throw new Error(`unknown fileId ${fileId}`);
    return this.readAt(fileId, 0, await file.size());
  }

  /**
   * The manifest a browser host needs: path, size, mtime and id per file.
   *
   * Sizes are resolved here rather than at mount time, which is what lets
   * mounting an archive be a metadata-only operation.
   */
  async manifest(): Promise<{
    files: { path: string; size: number; mtimeMs: number; fileId: number }[];
  }> {
    const files = [];
    for (const record of this.entriesById.values()) {
      const size = await this.filesById.get(record.fileId)!.size();
      files.push({
        path: record.path,
        size,
        mtimeMs: record.mtimeMs,
        fileId: record.fileId,
      });
    }
    files.sort((left, right) => left.path.localeCompare(right.path));
    return { files };
  }

  /** Laziness counters, with the eager baseline for comparison. */
  async stats(): Promise<LazyFileStats> {
    let bytesIfEager = 0;
    for (const record of this.entriesById.values()) {
      bytesIfEager += await this.filesById.get(record.fileId)!.size();
    }
    return {
      reads: this.reads,
      bytesRead: this.bytesRead,
      bytesIfEager,
      fileCount: this.entriesById.size,
    };
  }

  /* ------------------------------------------------------------------ */
  /* GameSource                                                          */
  /* ------------------------------------------------------------------ */

  async list(dir?: string): Promise<readonly string[]> {
    const prefix = normaliseDir(dir ?? '').toLowerCase();
    const names = new Set<string>();

    // A mount point is a directory, not an entry. Files mounted under a prefix
    // are therefore listed by their path *relative to that prefix* when the
    // mount root is what is being listed — so a game mounted at
    // `webgames/game1` shows its own contents, while the root shows `webgames/`.
    const mountPoints = this.mounts
      .map((mount) => mount.mountPoint)
      .filter((point) => point.length > 0)
      .sort((left, right) => right.length - left.length);

    for (const point of mountPoints) {
      const lowered = point.toLowerCase();
      if (!prefix) {
        names.add(`${point.slice(0, point.indexOf('/') >= 0 ? point.indexOf('/') : point.length)}/`);
        continue;
      }
      if (lowered.startsWith(`${prefix}/`)) {
        const rest = point.slice(prefix.length + 1);
        names.add(`${rest}/`);
      } else if (`${prefix}/`.startsWith(`${lowered}/`)) {
        // The queried directory is inside this mount; nothing to add here.
        continue;
      }
    }

    for (const mount of this.mounts) {
      const point = mount.mountPoint;
      for (const id of mount.files.values()) {
        const record = this.entriesById.get(id);
        if (!record) continue;
        const lowered = record.path.toLowerCase();
        if (prefix) {
          if (!lowered.startsWith(`${prefix}/`)) continue;
          const rest = record.path.slice(prefix.length + 1);
          names.add(rest.includes('/') ? `${rest.slice(0, rest.indexOf('/'))}/` : rest);
        } else {
          names.add(
            record.path.includes('/')
              ? `${record.path.slice(0, record.path.indexOf('/'))}/`
              : record.path,
          );
        }
      }
      void point;
    }
    return [...names].sort();
  }

  async readFile(path: string): Promise<Uint8Array> {
    const entry = this.resolve(path);
    if (!entry) throw new Error(`no such file: "${path}"`);
    return this.readAll(entry.fileId);
  }

  /** Reads a byte range without materialising the file. */
  async readRange(path: string, offset: number, length: number): Promise<Uint8Array> {
    const entry = this.resolve(path);
    if (!entry) throw new Error(`no such file: "${path}"`);
    return this.readAt(entry.fileId, offset, length);
  }

  async size(path: string): Promise<number> {
    const entry = this.resolve(path);
    if (!entry) throw new Error(`no such file: "${path}"`);
    return this.filesById.get(entry.fileId)!.size();
  }

  async exists(path: string): Promise<boolean> {
    return this.resolve(path) !== undefined;
  }

  async isDirectory(path: string): Promise<boolean> {
    const needle = normaliseDir(path);
    if (!needle) return true;
    const lowered = needle.toLowerCase();
    for (const mount of this.mounts) {
      // A mount point is a directory even before anything is looked up in it.
      if (mount.mountPoint.toLowerCase() === lowered) return true;
      for (const id of mount.files.values()) {
        const record = this.entriesById.get(id);
        if (record && record.path.toLowerCase().startsWith(`${lowered}/`)) return true;
      }
    }
    return false;
  }

  /**
   * The list of mount points, so a host can reproduce the virtual tree the
   * engine expects (`/webgames/<id>` in AetherKiri's layout).
   */
  mountPoints(): readonly string[] {
    return this.mounts.map((mount) => mount.mountPoint);
  }
}

/** Where a mounted entry's bytes come from, for building a tree. */
export type ArchiveRangeReader = (
  path: string,
  offset: number,
  length: number,
) => Promise<Uint8Array | null>;

/**
 * Builds lazy entries for every file in an archive.
 *
 * Nothing is read from the archive here beyond the index the reader already
 * parsed, so mounting a 1.8 GB NSA costs the header read and nothing more.
 * `rangeReader` is optional: pass the archive's own ranged reader and stored
 * entries are served as ranges rather than decoded.
 */
export function archiveEntries(
  reader: ArchiveReader,
  rangeReader?: ArchiveRangeReader,
): { path: string; file: LazyFile }[] {
  const entries: { path: string; file: LazyFile }[] = [];
  const walk = (dir: string): void => {
    for (const name of reader.list(dir)) {
      const path = dir ? `${dir}/${name}` : name;
      if (name.endsWith('/')) {
        walk(path.replace(/\/$/, ''));
        continue;
      }
      const size = reader.size(path);
      entries.push({
        path,
        file: new ArchiveEntryLazyFile(reader, path, size, rangeReader),
      });
    }
  };
  walk('');
  return entries;
}
