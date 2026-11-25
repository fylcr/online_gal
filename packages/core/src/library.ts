import {
  DefaultProbeContext,
  MemoryGameSource,
  OverlayGameSource,
  RuntimeRegistry,
  type GameSource,
  type RuntimeProvider,
} from '@online-gal/engine-api';
import {
  CompositeGameSource,
  FileListSource,
  FileSystemDirectorySource,
  IndexedDbStore,
  type FileSystemHandlePermission,
  type ImportedFile,
} from '@online-gal/vfs';
import {
  ARCHIVE_EXTENSIONS,
  archiveToGameSource,
  openBlobArchive,
  openGameSourceArchive,
} from '@online-gal/vfs/archives';
import { Database, keyPrefixRange, request } from './db.js';
import { saveOverlayNamespace, saveSlotPrefix } from './storage.js';
import {
  createId,
  type DetectionRecord,
  type GameRecord,
  type GameSourceDescriptor,
} from './types.js';

const DB_NAME = 'online-gal';
const DB_VERSION = 1;
const STORE_GAMES = 'games';
const STORE_FILE_INDEX = 'fileIndex';
const IMPORTED_FILES_NAMESPACE = 'files';

/** The saved directory cannot be read without granting access or picking it again. */
export class DirectoryPermissionError extends Error {
  readonly code = 'directory-permission-required';

  constructor(readonly gameId: string) {
    super('the game folder is unavailable; restore read access or select it again');
    this.name = 'DirectoryPermissionError';
  }
}

/** Extensions that are archives rather than regular game files. */
export function isArchivePath(path: string): boolean {
  const lower = path.toLowerCase();
  return ARCHIVE_EXTENSIONS.some((extension) => lower.endsWith(extension));
}

export interface ImportRequest {
  readonly name: string;
  readonly source: GameSourceDescriptor;
  /** Files, for `files` sources. Ignored for `directory` sources. */
  readonly files?: readonly ImportedFile[];
  /** Overrides the derived display name. */
  readonly title?: string;
}

export interface ImportResult {
  readonly record: GameRecord;
  /** True when an existing entry with the same engine and name was updated. */
  readonly replaced: boolean;
}

export interface GameLibraryOptions {
  readonly registry: RuntimeRegistry;
  readonly dbName?: string;
}

/**
 * Owns the game library: persisted records, per-game file storage, the
 * detection result, and the save slots.
 *
 * File storage lives in the same IndexedDB database as the records, under a
 * per-game prefix, so deleting an entry reclaims its content in one pass. That
 * is the browser counterpart of AetherKiri staging an imported game into a
 * writable application-owned location.
 */
export class GameLibrary {
  private readonly db: Database;

  constructor(private readonly options: GameLibraryOptions) {
    this.db = new Database({
      name: options.dbName ?? DB_NAME,
      version: DB_VERSION,
      stores: [STORE_GAMES, 'saves', 'settings', STORE_FILE_INDEX],
    });
  }

  /* ---------------------------------------------------------------- */
  /* Records                                                          */
  /* ---------------------------------------------------------------- */

  async list(): Promise<readonly GameRecord[]> {
    return this.db.transaction([STORE_GAMES], 'readonly', (transaction) =>
      request<GameRecord[]>(
        transaction.objectStore(STORE_GAMES).getAll() as IDBRequest<GameRecord[]>,
      ),
    );
  }

  async get(id: string): Promise<GameRecord | null> {
    const record = await this.db.transaction([STORE_GAMES], 'readonly', (transaction) =>
      request<GameRecord | undefined>(
        transaction.objectStore(STORE_GAMES).get(id) as IDBRequest<GameRecord | undefined>,
      ),
    );
    return record ?? null;
  }

  async put(record: GameRecord): Promise<void> {
    await this.db.transaction([STORE_GAMES], 'readwrite', (transaction) =>
      request(transaction.objectStore(STORE_GAMES).put(record, record.id)),
    );
  }

  async patch(id: string, patch: Partial<GameRecord>): Promise<GameRecord | null> {
    const existing = await this.get(id);
    if (!existing) return null;
    const updated: GameRecord = { ...existing, ...patch, id: existing.id };
    await this.put(updated);
    return updated;
  }

  /** Removes a game and its application-owned files and saves in one transaction. */
  async remove(id: string): Promise<void> {
    await this.db.transaction([STORE_GAMES, 'saves', STORE_FILE_INDEX], 'readwrite', async (transaction) => {
      const saves = transaction.objectStore('saves');
      const files = transaction.objectStore(STORE_FILE_INDEX);
      await Promise.all([
        request(transaction.objectStore(STORE_GAMES).delete(id)),
        request(saves.delete(keyPrefixRange(saveSlotPrefix(id)))),
        request(saves.delete(keyPrefixRange(`${saveOverlayNamespace(id)}\u0000`))),
        request(files.delete(keyPrefixRange(`${IMPORTED_FILES_NAMESPACE}\u0000${id}/`))),
      ]);
    });
  }

  /* ---------------------------------------------------------------- */
  /* Import                                                           */
  /* ---------------------------------------------------------------- */

  /** Imports a game, stores its files, and detects the engine. */
  async import(request_: ImportRequest): Promise<ImportResult> {
    const id = createId();
    const { source, descriptor } = await this.materialise(id, request_);

    const detection = await this.detect(source);

    // De-duplicate against an existing entry with the same engine and name so
    // re-importing a folder updates rather than duplicates.
    const existing = (await this.list()).find(
      (record) =>
        record.name.toLowerCase() === request_.name.toLowerCase() &&
        record.detection.runtimeId === detection.runtimeId,
    );

    const record: GameRecord = {
      id: existing?.id ?? id,
      name: request_.name,
      ...(request_.title ? { title: request_.title } : {}),
      source: descriptor,
      detection,
      config: existing?.config ?? {},
      ...(existing?.cover ? { cover: existing.cover } : {}),
      addedAt: existing?.addedAt ?? Date.now(),
      lastPlayedAt: existing?.lastPlayedAt ?? null,
      playTimeMs: existing?.playTimeMs ?? 0,
      favorite: existing?.favorite ?? false,
      tags: existing?.tags ?? [],
    };

    if (existing && existing.id !== id) {
      // The new files were written under the fresh id; move them across.
      await this.moveStoredFiles(id, existing.id);
      await this.patch(existing.id, { source: descriptor, detection });
      return { record: (await this.get(existing.id))!, replaced: true };
    }

    await this.put(record);
    return { record, replaced: false };
  }

  /**
   * Builds the `GameSource` for an import and persists any file content.
   *
   * Directory handles are stored by reference, so a game with tens of
   * thousands of files imports instantly and reads on demand. Picked and
   * dropped files are copied into IndexedDB because the `File` objects cannot
   * be reopened after a reload.
   */
  private async materialise(
    id: string,
    request_: ImportRequest,
  ): Promise<{ source: GameSource; descriptor: GameSourceDescriptor }> {
    switch (request_.source.kind) {
      case 'directory': {
        const handle = request_.source.handle;
        if (!handle) throw new Error('a directory import needs a directory handle');
        const source = new FileSystemDirectorySource(handle.name, handle);
        await source.list('');
        return { source, descriptor: { kind: 'directory', handle } };
      }
      case 'files': {
        const files = request_.files ?? [];
        if (files.length === 0) throw new Error('a file import needs at least one file');
        await this.storeFiles(id, files);
        const store = this.fileStore();
        const source = await this.openStoredSource(id, store, request_.name);
        return {
          source,
          descriptor: { kind: 'files', fileCount: files.length },
        };
      }
      case 'local': {
        const path = request_.source.localPath ?? '';
        const source = await this.openLocalSource(path, request_.name);
        return { source, descriptor: request_.source };
      }
      default:
        throw new Error(`unsupported source kind: ${String(request_.source.kind)}`);
    }
  }

  /**
   * Opens a game served from the dev server's configured game root.
   *
   * This is a developer convenience only: it lets a local folder be played
   * without importing, and it is never used by the deployed product path.
   */
  private async openLocalSource(basePath: string, name: string): Promise<GameSource> {
    const base = basePath.replace(/\/+$/, '');
    const index = await fetch(`${base}/__index.json`)
      .then((response) => (response.ok ? (response.json() as Promise<string[]>) : null))
      .catch(() => null);
    if (!index) {
      throw new Error(
        `the dev server exposed no file index at "${base}/__index.json"; ` +
          'import the game directory instead',
      );
    }
    const memory = new MemoryGameSource(name);
    await Promise.all(
      index.map(async (path) => {
        const response = await fetch(`${base}/${path}`);
        if (!response.ok) return;
        memory.addFile(path, new Uint8Array(await response.arrayBuffer()));
      }),
    );
    return this.withArchives(memory, name);
  }

  private fileStore(): IndexedDbStore {
    return new IndexedDbStore(IMPORTED_FILES_NAMESPACE, this.options.dbName ?? DB_NAME, STORE_FILE_INDEX);
  }

  private async storeFiles(id: string, files: readonly ImportedFile[]): Promise<void> {
    const store = this.fileStore();
    await Promise.all(
      files.map(async (entry) => {
        const bytes = await blobToBytes(entry.file);
        await store.write(`${id}/${entry.path}`, bytes);
      }),
    );
  }

  private async moveStoredFiles(fromId: string, toId: string): Promise<void> {
    const store = this.fileStore();
    const prefix = `${fromId}/`;
    for (const path of await store.list(prefix)) {
      if (!path.startsWith(prefix)) continue;
      const data = await store.read(path);
      if (!data) continue;
      await store.write(`${toId}/${path.slice(prefix.length)}`, data);
      await store.remove(path);
    }
  }

  /**
   * Opens a stored game, merging any archives it contains into the view.
   *
   * A KiriKiri title distributed as `data.xp3` has no loose files to probe, so
   * the archive is mounted as an additional layer. Missing or unreadable
   * archives degrade to the loose files instead of failing the import.
   */
  private async openStoredSource(
    id: string,
    store: IndexedDbStore,
    name: string,
  ): Promise<GameSource> {
    const prefix = `${id}/`;
    const paths = (await store.list(prefix)).filter((path) => path.startsWith(prefix));
    if (paths.length === 0) throw new Error('the stored files for this game are missing; re-import it');
    const loose = paths.map((path) => path.slice(prefix.length));
    const memory = new MemoryGameSource(name);
    const archives: string[] = [];

    for (const path of loose) {
      if (isArchivePath(path)) {
        archives.push(path);
        continue;
      }
      const data = await store.read(`${id}/${path}`);
      if (data) memory.addFile(path, data);
    }

    return this.combine(memory, name, archives, async (archivePath) => {
      const data = await store.read(`${id}/${archivePath}`);
      if (!data) return null;
      const blob = new Blob([data as BlobPart]);
      return blob;
    });
  }

  /**
   * Mounts archives on top of a loose-file view.
   *
   * An archive is mounted twice over, and both halves are load-bearing:
   *
   *  - its **contents** become a layer, because that is how the interpreted runtimes
   *    read a packaged game at all; and
   *  - the archive **file itself** stays reachable by path, in a last layer, because
   *    the framed KiriKiri WebAssembly shell mounts one archive and parses it itself.
   *    Without this second half a `.zip`/`.xp3` imported as a file is expanded out of
   *    existence: the engine's mount probe finds no archive and declines the game,
   *    even though the bytes are right there in the same source.
   *
   * The contents layer comes first so an inner path wins; the archive-file layer is
   * last, so it only answers for the archive's own path.
   */
  private async combine(
    loose: GameSource,
    name: string,
    archives: readonly string[],
    readArchive: (path: string) => Promise<Blob | null>,
  ): Promise<GameSource> {
    if (archives.length === 0) return loose;
    const layers: GameSource[] = [loose];
    const archiveFiles: ImportedFile[] = [];
    for (const archivePath of archives) {
      try {
        const blob = await readArchive(archivePath);
        if (!blob) continue;
        archiveFiles.push({ path: archivePath, file: blob });
        const reader = await openBlobArchive(blob, archivePath);
        layers.push(archiveToGameSource(reader));
      } catch (error) {
        // An encrypted or unsupported package must not block the import; the
        // detection result will report the engine from the remaining evidence.
        void error;
      }
    }
    if (archiveFiles.length > 0) layers.push(new FileListSource(name, archiveFiles));
    return layers.length === 1 ? loose : new CompositeGameSource(name, layers);
  }

  private async withArchives(source: GameSource, name: string): Promise<GameSource> {
    const entries = await source.list('');
    const archives = entries.filter((entry) => isArchivePath(entry));
    return this.combine(source, name, archives, async (path) => {
      const bytes = await source.readFile(path);
      return new Blob([bytes as BlobPart]);
    });
  }

  /* ---------------------------------------------------------------- */
  /* Runtime access                                                   */
  /* ---------------------------------------------------------------- */

  /**
   * Reopens a library entry for play.
   *
   * The writable overlay keeps the imported content read-only while giving the
   * engine a place to write saves and configuration, which is what makes a
   * browser session behave like a real install.
   */
  async openSource(record: GameRecord): Promise<GameSource> {
    const base = await this.openBaseSource(record);
    const store = new IndexedDbStore(saveOverlayNamespace(record.id), this.options.dbName ?? DB_NAME, 'saves');
    return new OverlayGameSource(base, store, { name: record.name, writePrefix: 'userfs' });
  }

  /** Updates an existing directory binding only after the selected folder can be read. */
  async reselectDirectory(id: string, handle: FileSystemDirectoryHandle): Promise<GameRecord> {
    if (!(await hasDirectoryPermission(handle))) throw new DirectoryPermissionError(id);
    const directory = new FileSystemDirectorySource(handle.name, handle);
    await directory.list('');
    const detection = await this.detect(directory);

    return this.db.transaction([STORE_GAMES], 'readwrite', async (transaction) => {
      const games = transaction.objectStore(STORE_GAMES);
      const current = await request<GameRecord | undefined>(games.get(id));
      if (current?.source.kind !== 'directory') throw new Error('the directory game no longer exists');
      const updated: GameRecord = { ...current, source: { kind: 'directory', handle }, detection };
      await request(games.put(updated, id));
      return updated;
    });
  }

  private async openBaseSource(record: GameRecord): Promise<GameSource> {
    const { source } = record;
    switch (source.kind) {
      case 'directory': {
        if (!source.handle || !(await hasDirectoryPermission(source.handle))) {
          throw new DirectoryPermissionError(record.id);
        }
        const directory = new FileSystemDirectorySource(record.name, source.handle);
        let entries: readonly string[];
        try {
          entries = await directory.list('');
        } catch (error) {
          if (error instanceof DOMException &&
              ['NotAllowedError', 'NotFoundError', 'SecurityError'].includes(error.name)) {
            throw new DirectoryPermissionError(record.id);
          }
          throw error;
        }
        const archives = entries.filter((entry) => isArchivePath(entry));
        return this.combine(directory, record.name, archives, async (path) => {
          const bytes = await directory.readFile(path);
          return new Blob([bytes as BlobPart]);
        });
      }
      case 'files':
        return this.openStoredSource(record.id, this.fileStore(), record.name);
      case 'local':
        return this.openLocalSource(source.localPath ?? '', record.name);
      default:
        throw new Error(`unsupported source kind: ${String(source.kind)}`);
    }
  }

  /** Runs every registered provider's probe and records the outcome. */
  async detect(source: GameSource): Promise<DetectionRecord> {
    const outcome = await this.options.registry.discover(source);
    const selected = outcome.selected;

    const availability = ((): DetectionRecord['availability'] => {
      if (!selected) return 'unknown';
      const provider = this.options.registry.get(selected.runtimeId);
      if (!provider) return 'unavailable';
      // A provider that knows it has no working backend outranks the probe's
      // own confidence: identifying an engine is not the same as running it.
      if (provider.availability === 'unavailable') return 'unavailable';
      return selected.confidence === 'low' ? 'uncertain' : 'runnable';
    })();

    return {
      runtimeId: selected?.runtimeId ?? null,
      runtimeName: selected?.displayName ?? null,
      confidence: selected?.confidence ?? 'none',
      score: selected?.score ?? 0,
      availability,
      evidence: selected?.evidence ?? [],
      alternatives: outcome.matches.slice(1, 5).map((match) => ({
        runtimeId: match.runtimeId,
        displayName: match.displayName,
        score: match.score,
      })),
      detectedAt: Date.now(),
    };
  }

  /** Re-detects a stored game, e.g. after an engine was added to the platform. */
  async redetect(id: string): Promise<GameRecord | null> {
    const record = await this.get(id);
    if (!record) return null;
    const source = await this.openSource(record);
    const detection = await this.detect(source);
    return this.patch(id, { detection });
  }

  /** Counts a provider against a source without touching the library. */
  async probeWith(runtimeId: string, source: GameSource) {
    return this.options.registry.probeRuntime(runtimeId, source);
  }
}

async function blobToBytes(blob: Blob): Promise<Uint8Array> {
  return new Uint8Array(await blob.arrayBuffer());
}

/** Permission mode accepted by the File System Access API. */
export type FsPermissionMode = 'read' | 'readwrite';

async function hasDirectoryPermission(handle: FileSystemHandle, mode: FsPermissionMode = 'read'): Promise<boolean> {
  const permissions = handle as FileSystemHandle & FileSystemHandlePermission;
  try {
    return !permissions.queryPermission || (await permissions.queryPermission({ mode })) === 'granted';
  } catch {
    return false;
  }
}

/** Call from a user gesture to request permission on a previously picked handle. */
export async function ensurePermission(handle: FileSystemHandle, mode: FsPermissionMode = 'read'): Promise<boolean> {
  const permissions = handle as FileSystemHandle & FileSystemHandlePermission;
  if (!permissions.requestPermission) return hasDirectoryPermission(handle, mode);
  try {
    return (await permissions.requestPermission({ mode })) === 'granted';
  } catch {
    return false;
  }
}

export { DefaultProbeContext, openGameSourceArchive };
