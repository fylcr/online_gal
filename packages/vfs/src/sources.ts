import type { GameSource, WritableStore } from '@online-gal/engine-api';
import { dirname, joinPath, normaliseDir, normalisePath } from './path.js';

/**
 * Extensions the DOM typings do not yet cover.
 *
 * `FileSystemDirectoryHandle.entries()` and the `permission` methods are
 * shipped by every browser that implements the File System Access API but are
 * still absent from some TypeScript DOM lib versions, so they are declared
 * here instead of being cast away at each call site.
 */
export interface FileSystemHandlePermission {
  queryPermission?(descriptor?: { mode?: 'read' | 'readwrite' }): Promise<PermissionState>;
  requestPermission?(descriptor?: { mode?: 'read' | 'readwrite' }): Promise<PermissionState>;
}

export type AsyncDirectoryEntries = AsyncIterableIterator<[string, FileSystemHandle]>;

export interface FileSystemDirectoryHandleWithEntries extends FileSystemDirectoryHandle {
  entries(): AsyncDirectoryEntries;
  keys?(): AsyncIterableIterator<string>;
  values?(): AsyncIterableIterator<FileSystemHandle>;
}

/** Legacy `FileSystemEntry` shape used by drag-and-drop `webkitGetAsEntry`. */
export interface LegacyFileSystemEntry {
  readonly isFile: boolean;
  readonly isDirectory: boolean;
  readonly name: string;
  file(success: (file: File) => void, error?: (error: unknown) => void): void;
  createReader(): {
    readEntries(
      success: (entries: LegacyFileSystemEntry[]) => void,
      error?: (error: unknown) => void,
    ): void;
  };
}

/** Splits a full relative path into segments. */
function segmentsOf(path: string): readonly string[] {
  const key = normalisePath(path);
  return key ? key.split('/') : [];
}

function assertHandleName(name: string): void {
  if (name.startsWith('.') || name.includes('/') || name.includes('\\')) {
    throw new Error(`unsafe path segment: "${name}"`);
  }
}

/**
 * A `GameSource` over a directory the user authorised through the File System
 * Access API.
 *
 * Reads stay on-demand: nothing is copied into memory up front, which is what
 * makes multi-gigabyte game directories viable in a browser.
 */
export class FileSystemDirectorySource implements GameSource {
  readonly capabilities = {
    list: true,
    randomAccess: true,
    writable: false,
  } as const;

  constructor(
    readonly name: string,
    private readonly root: FileSystemDirectoryHandle,
  ) {}

  static async fromHandle(handle: FileSystemDirectoryHandle): Promise<FileSystemDirectorySource> {
    return new FileSystemDirectorySource(handle.name, handle);
  }

  private async resolveFile(
    path: string,
    create = false,
  ): Promise<FileSystemFileHandle | null> {
    const parts = segmentsOf(path);
    if (parts.length === 0) return null;
    let dir: FileSystemDirectoryHandle = this.root;
    for (let i = 0; i < parts.length - 1; i += 1) {
      const segment = parts[i]!;
      assertHandleName(segment);
      try {
        dir = await dir.getDirectoryHandle(segment, { create });
      } catch {
        return null;
      }
    }
    const leaf = parts[parts.length - 1]!;
    assertHandleName(leaf);
    try {
      return await dir.getFileHandle(leaf, { create });
    } catch {
      return null;
    }
  }

  private async resolveDir(
    path: string,
  ): Promise<FileSystemDirectoryHandle | null> {
    const parts = segmentsOf(path);
    let dir: FileSystemDirectoryHandle = this.root;
    for (const segment of parts) {
      assertHandleName(segment);
      try {
        dir = await dir.getDirectoryHandle(segment);
      } catch {
        return null;
      }
    }
    return dir;
  }

  async list(dir = ''): Promise<readonly string[]> {
    const handle = await this.resolveDir(dir);
    if (!handle) return [];
    const out: string[] = [];
    const iterable = handle as FileSystemDirectoryHandleWithEntries;
    if (typeof iterable.entries !== 'function') {
      throw new Error(
        'this browser does not expose FileSystemDirectoryHandle.entries(); ' +
          'import the folder through the file picker instead',
      );
    }
    for await (const [entryName, entry] of iterable.entries()) {
      out.push(entry.kind === 'directory' ? `${entryName}/` : entryName);
    }
    return out.sort();
  }

  async readFile(path: string): Promise<Uint8Array> {
    const handle = await this.resolveFile(path);
    if (!handle) throw new Error(`no such file: "${path}"`);
    const file = await handle.getFile();
    return new Uint8Array(await file.arrayBuffer());
  }

  async readRange(
    path: string,
    offset: number,
    length: number,
  ): Promise<Uint8Array> {
    const handle = await this.resolveFile(path);
    if (!handle) throw new Error(`no such file: "${path}"`);
    const file = await handle.getFile();
    const blob = file.slice(offset, offset + length);
    return new Uint8Array(await blob.arrayBuffer());
  }

  async size(path: string): Promise<number> {
    const handle = await this.resolveFile(path);
    if (!handle) return -1;
    const file = await handle.getFile();
    return file.size;
  }

  async exists(path: string): Promise<boolean> {
    return (await this.resolveFile(path)) !== null;
  }

  async isDirectory(path: string): Promise<boolean> {
    if (!normalisePath(path)) return true;
    if (await this.resolveFile(path)) return false;
    return (await this.resolveDir(path)) !== null;
  }
}

/**
 * A `GameSource` over a flat list of `File` objects.
 *
 * This covers `<input type="file" webkitdirectory>`, drag-and-drop of a folder,
 * and ZIP entries that were expanded by the browser. Each entry carries its
 * path relative to the imported root.
 */
export interface ImportedFile {
  /** Path relative to the game root, using `/` separators. */
  readonly path: string;
  readonly file: Blob;
}

export class FileListSource implements GameSource {
  private readonly files = new Map<string, Blob>();
  private readonly dirs = new Set<string>(['']);
  readonly capabilities = {
    list: true,
    randomAccess: true,
    writable: false,
  } as const;

  constructor(
    readonly name: string,
    entries: readonly ImportedFile[],
  ) {
    for (const entry of entries) {
      const key = normalisePath(entry.path);
      if (!key) continue;
      this.files.set(key, entry.file);
      let parent = dirname(key);
      while (parent) {
        if (this.dirs.has(parent)) break;
        this.dirs.add(parent);
        parent = dirname(parent);
      }
    }
  }

  get fileCount(): number {
    return this.files.size;
  }

  paths(): readonly string[] {
    return [...this.files.keys()].sort();
  }

  /** Builds a source from a `FileList` produced by a directory input. */
  static fromFileList(name: string, files: FileList | readonly File[]): FileListSource {
    const entries: ImportedFile[] = [];
    for (const file of Array.from(files)) {
      const relative =
        (file as File & { webkitRelativePath?: string }).webkitRelativePath ||
        file.name;
      // Drop the selected folder's own name so the game root is the folder.
      const parts = normalisePath(relative).split('/');
      const path = parts.length > 1 ? parts.slice(1).join('/') : parts[0]!;
      entries.push({ path, file });
    }
    return new FileListSource(name, entries);
  }

  async list(dir = ''): Promise<readonly string[]> {
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
      if (!candidate.startsWith(prefix)) continue;
      const rest = candidate.slice(prefix.length);
      if (!rest) continue;
      const slash = rest.indexOf('/');
      out.add(slash === -1 ? `${rest}/` : `${rest.slice(0, slash)}/`);
    }
    return [...out].sort();
  }

  async readFile(path: string): Promise<Uint8Array> {
    const blob = this.files.get(normalisePath(path));
    if (!blob) {
      if (this.dirs.has(normalisePath(path))) {
        throw new Error(`"${path}" is a directory`);
      }
      throw new Error(`no such file: "${path}"`);
    }
    return new Uint8Array(await blob.arrayBuffer());
  }

  async readRange(
    path: string,
    offset: number,
    length: number,
  ): Promise<Uint8Array> {
    const blob = this.files.get(normalisePath(path));
    if (!blob) throw new Error(`no such file: "${path}"`);
    return new Uint8Array(await blob.slice(offset, offset + length).arrayBuffer());
  }

  async size(path: string): Promise<number> {
    const blob = this.files.get(normalisePath(path));
    return blob ? blob.size : -1;
  }

  async exists(path: string): Promise<boolean> {
    const key = normalisePath(path);
    return this.files.has(key) || this.dirs.has(key);
  }

  async isDirectory(path: string): Promise<boolean> {
    const key = normalisePath(path);
    if (!key) return true;
    if (this.files.has(key)) return false;
    return this.dirs.has(key);
  }
}

/** Walks a `DataTransferItemList` from a drop event into `ImportedFile`s. */
export async function fromDataTransfer(
  items: DataTransferItemList,
): Promise<readonly ImportedFile[]> {
  const out: ImportedFile[] = [];

  const walk = async (entry: LegacyFileSystemEntry, prefix: string): Promise<void> => {
    if (entry.isFile) {
      const file = await new Promise<File>((resolve, reject) =>
        entry.file(resolve, reject),
      );
      out.push({ path: joinPath(prefix, file.name), file });
      return;
    }
    if (!entry.isDirectory) return;
    const reader = entry.createReader();
    // `readEntries` returns at most ~100 entries per call; loop until empty.
    for (;;) {
      const batch = await new Promise<LegacyFileSystemEntry[]>((resolve, reject) =>
        reader.readEntries(resolve, reject),
      );
      if (batch.length === 0) break;
      for (const child of batch) {
        await walk(child, joinPath(prefix, entry.name));
      }
    }
  };

  const roots: LegacyFileSystemEntry[] = [];
  for (const item of Array.from(items)) {
    if (item.kind !== 'file') continue;
    const supplier = item as DataTransferItem & {
      webkitGetAsEntry?: () => FileSystemEntry | null;
    };
    // The legacy entry API is not described by the DOM typings, so the value
    // is re-typed onto our own structural interface.
    const entry = supplier.webkitGetAsEntry?.() as unknown as
      | LegacyFileSystemEntry
      | null
      | undefined;
    if (entry) roots.push(entry);
  }

  for (const root of roots) {
    await walk(root, '');
  }

  if (out.length === 0) {
    // Fallback for browsers without `webkitGetAsEntry`.
    for (const item of Array.from(items)) {
      if (item.kind !== 'file') continue;
      const file = item.getAsFile();
      if (file) out.push({ path: file.name, file });
    }
  }

  return out;
}

/** IndexedDB-backed `WritableStore`: saves, config and scratch data. */
export class IndexedDbStore implements WritableStore {
  private db: IDBDatabase | null = null;
  private opening: Promise<IDBDatabase> | null = null;

  constructor(
    readonly name: string,
    private readonly dbName = 'online-gal',
    private readonly storeName = 'saves',
  ) {}

  private open(): Promise<IDBDatabase> {
    if (this.db) return Promise.resolve(this.db);
    if (this.opening) return this.opening;
    this.opening = new Promise<IDBDatabase>((resolve, reject) => {
      const request = indexedDB.open(this.dbName, 1);
      request.onupgradeneeded = () => {
        const db = request.result;
        if (!db.objectStoreNames.contains(this.storeName)) {
          db.createObjectStore(this.storeName);
        }
      };
      request.onsuccess = () => {
        this.db = request.result;
        resolve(request.result);
      };
      request.onerror = () => reject(request.error);
    });
    return this.opening;
  }

  private key(path: string): string {
    return `${this.name}\u0000${normalisePath(path)}`;
  }

  private async run<T>(
    mode: IDBTransactionMode,
    action: (store: IDBObjectStore) => IDBRequest,
  ): Promise<T> {
    const db = await this.open();
    return new Promise<T>((resolve, reject) => {
      const tx = db.transaction(this.storeName, mode);
      const request = action(tx.objectStore(this.storeName));
      request.onsuccess = () => resolve(request.result as T);
      request.onerror = () => reject(request.error);
    });
  }

  async read(path: string): Promise<Uint8Array | undefined> {
    const value = await this.run<unknown>('readonly', (store) =>
      store.get(this.key(path)),
    );
    if (value instanceof Uint8Array) return value;
    if (value instanceof ArrayBuffer) return new Uint8Array(value);
    return undefined;
  }

  async write(path: string, data: Uint8Array): Promise<void> {
    await this.run('readwrite', (store) => store.put(data, this.key(path)));
  }

  async remove(path: string): Promise<void> {
    await this.run('readwrite', (store) => store.delete(this.key(path)));
  }

  async list(prefix = ''): Promise<readonly string[]> {
    const keys = await this.run<IDBValidKey[]>('readonly', (store) =>
      store.getAllKeys(),
    );
    const scope = `${this.name}\u0000`;
    const wanted = normalisePath(prefix);
    return keys
      .map(String)
      .filter((key) => key.startsWith(scope))
      .map((key) => key.slice(scope.length))
      .filter((path) => !wanted || path.startsWith(wanted))
      .sort();
  }
}

/** A `GameSource` view over an archive reader (see `./archives`). */
export interface ArchiveReaderLike {
  readonly name: string;
  list(dir?: string): readonly string[];
  has(path: string): boolean;
  isDirectory(path: string): boolean;
  readFile(path: string): Promise<Uint8Array>;
  size(path: string): number;
  totalSize(): number;
}

/** Adapts an archive reader to the `GameSource` contract. */
export class ArchiveGameSource implements GameSource {
  readonly capabilities = {
    list: true,
    randomAccess: true,
    writable: false,
  } as const;

  constructor(
    readonly name: string,
    private readonly archive: ArchiveReaderLike,
  ) {}

  async list(dir = ''): Promise<readonly string[]> {
    return this.archive.list(dir);
  }

  async readFile(path: string): Promise<Uint8Array> {
    return this.archive.readFile(path);
  }

  async size(path: string): Promise<number> {
    return this.archive.size(path);
  }

  async exists(path: string): Promise<boolean> {
    return this.archive.has(path);
  }

  async isDirectory(path: string): Promise<boolean> {
    return this.archive.isDirectory(path);
  }
}
