import { IndexedDbStore } from '@online-gal/vfs';
import { Database, keyPrefixRange, request } from './db.js';
import {
  DEFAULT_SETTINGS,
  assertGameId,
  type AppSettings,
  type SaveSlotRecord,
} from './types.js';

const DB_NAME = 'online-gal';
const DB_VERSION = 1;
const STORE_SETTINGS = 'settings';
const STORE_SAVES = 'saves';

export function saveSlotPrefix(gameId: string): string {
  assertGameId(gameId);
  return `${gameId}\u0000`;
}

export function saveOverlayNamespace(gameId: string): string {
  assertGameId(gameId);
  return `saves:${gameId}`;
}

/** Persists global application settings as a single JSON record. */
export class SettingsStore {
  private readonly db: Database;
  private cache: AppSettings | null = null;

  constructor(dbName = DB_NAME) {
    this.db = new Database({
      name: dbName,
      version: DB_VERSION,
      stores: [STORE_SETTINGS, STORE_SAVES, 'games', 'fileIndex'],
    });
  }

  async load(): Promise<AppSettings> {
    if (this.cache) return this.cache;
    const stored = await this.db.transaction([STORE_SETTINGS], 'readonly', (transaction) =>
      request<AppSettings | undefined>(
        transaction.objectStore(STORE_SETTINGS).get('app') as IDBRequest<AppSettings | undefined>,
      ),
    );
    // Merge onto the defaults so a settings schema addition takes effect
    // without a migration.
    this.cache = stored
      ? {
          ...DEFAULT_SETTINGS,
          ...stored,
          volumes: { ...DEFAULT_SETTINGS.volumes, ...(stored.volumes ?? {}) },
          surface: { ...DEFAULT_SETTINGS.surface, ...(stored.surface ?? {}) },
        }
      : { ...DEFAULT_SETTINGS };
    return this.cache;
  }

  async save(settings: AppSettings): Promise<void> {
    this.cache = settings;
    await this.db.transaction([STORE_SETTINGS], 'readwrite', (transaction) =>
      request(transaction.objectStore(STORE_SETTINGS).put(settings, 'app')),
    );
  }

  async patch(patch: Partial<AppSettings>): Promise<AppSettings> {
    const current = await this.load();
    const next: AppSettings = {
      ...current,
      ...patch,
      volumes: { ...current.volumes, ...(patch.volumes ?? {}) },
      surface: { ...current.surface, ...(patch.surface ?? {}) },
    };
    await this.save(next);
    return next;
  }
}

/**
 * Save slots, keyed by game and slot number.
 *
 * State is stored as-is (structured clone) alongside the per-game writable
 * engine filesystem, so a slot plus the engine's own in-game saves always move
 * together.
 */
export class SaveStore {
  private readonly db: Database;

  constructor(private readonly dbName = DB_NAME) {
    this.db = new Database({
      name: dbName,
      version: DB_VERSION,
      stores: [STORE_SAVES, STORE_SETTINGS, 'games', 'fileIndex'],
    });
  }

  /** The engine-visible writable filesystem for a game. */
  fileStore(gameId: string): IndexedDbStore {
    return new IndexedDbStore(saveOverlayNamespace(gameId), this.dbName, STORE_SAVES);
  }

  private key(gameId: string, slot: number): string {
    return `${saveSlotPrefix(gameId)}${String(slot).padStart(4, '0')}`;
  }

  async list(gameId: string): Promise<readonly SaveSlotRecord[]> {
    const slots = await this.db.transaction([STORE_SAVES], 'readonly', (transaction) =>
      request<SaveSlotRecord[]>(
        transaction.objectStore(STORE_SAVES).getAll(keyPrefixRange(saveSlotPrefix(gameId))),
      ),
    );
    return slots.sort((a, b) => a.slot - b.slot);
  }

  async get(gameId: string, slot: number): Promise<SaveSlotRecord | null> {
    const record = await this.db.transaction([STORE_SAVES], 'readonly', (transaction) =>
      request<SaveSlotRecord | undefined>(
        transaction.objectStore(STORE_SAVES).get(this.key(gameId, slot)) as IDBRequest<
          SaveSlotRecord | undefined
        >,
      ),
    );
    return record ?? null;
  }

  async put(record: SaveSlotRecord): Promise<void> {
    await this.db.transaction([STORE_SAVES], 'readwrite', (transaction) =>
      request(transaction.objectStore(STORE_SAVES).put(record, this.key(record.gameId, record.slot))),
    );
  }

  async remove(gameId: string, slot: number): Promise<void> {
    await this.db.transaction([STORE_SAVES], 'readwrite', (transaction) =>
      request(transaction.objectStore(STORE_SAVES).delete(this.key(gameId, slot))),
    );
  }

  /** Exports every slot for a game, for backup or device transfer. */
  async exportAll(gameId: string): Promise<string> {
    const slots = await this.list(gameId);
    const files = await this.fileStore(gameId).list();
    const fileData: Record<string, string> = {};
    const store = this.fileStore(gameId);
    for (const path of files) {
      const data = await store.read(path);
      if (data) fileData[path] = bytesToBase64(data);
    }
    return JSON.stringify(
      { format: 'online-gal-save', version: 1, gameId, slots, files: fileData },
      null,
      2,
    );
  }

  /** Restores slots produced by `exportAll`, merging rather than wiping. */
  async importAll(gameId: string, json: string): Promise<number> {
    const parsed = JSON.parse(json) as {
      format?: string;
      slots?: SaveSlotRecord[];
      files?: Record<string, string>;
    };
    if (parsed.format !== 'online-gal-save') {
      throw new Error('this file is not an online-gal save archive');
    }
    const store = this.fileStore(gameId);
    for (const [path, base64] of Object.entries(parsed.files ?? {})) {
      await store.write(path, base64ToBytes(base64));
    }
    let count = 0;
    for (const slot of parsed.slots ?? []) {
      await this.put({ ...slot, gameId });
      count += 1;
    }
    return count;
  }
}

function bytesToBase64(bytes: Uint8Array): string {
  let binary = '';
  const chunk = 0x8000;
  for (let index = 0; index < bytes.length; index += chunk) {
    binary += String.fromCharCode(...bytes.subarray(index, index + chunk));
  }
  return btoa(binary);
}

function base64ToBytes(base64: string): Uint8Array {
  const binary = atob(base64);
  const out = new Uint8Array(binary.length);
  for (let index = 0; index < binary.length; index += 1) {
    out[index] = binary.charCodeAt(index);
  }
  return out;
}
