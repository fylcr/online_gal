/**
 * Encrypted credential vault (openspec cf-account-admin design.md A4).
 *
 * Every record is sealed with one non-extractable AES-GCM-256 key, a fresh 12-byte iv per write,
 * and `online-gal-vault/v1/<id>` as additional data, so a record moved to another id no longer
 * opens. A record that does not decrypt (key lost, data tampered) counts as absent and is deleted.
 *
 * What this protects: tokens at rest, e.g. a copied browser profile. What it does not: any
 * same-origin script can open the database and use the key, because the key is usable, only not
 * exportable. Isolating credentials from other code on this origin needs a separate origin, which
 * the user deferred to the overall OAuth design.
 */

export const VAULT_DB = 'online-gal-vault';
export const VAULT_DB_VERSION = 1;
const KEY_ID = 'vault-key-v1';
const AAD_PREFIX = 'online-gal-vault/v1/';

/** One sealed record. `id` is a provider (`cloudflare`) or a provider's pending login (`cloudflare#pending`). */
export interface VaultRecord {
  readonly provider: string;
  readonly v: 1;
  readonly iv: Uint8Array;
  readonly ct: ArrayBuffer;
}

/** Where the key and the sealed records live: IndexedDB in the worker, memory in tests. */
export interface VaultStore {
  getKey(): Promise<CryptoKey | undefined>;
  /** Stores `key` unless one exists already; returns the key that is stored afterwards. */
  addKey(key: CryptoKey): Promise<CryptoKey>;
  get(provider: string): Promise<VaultRecord | undefined>;
  put(record: VaultRecord): Promise<void>;
  delete(provider: string): Promise<void>;
}

const encoder = new TextEncoder();
const decoder = new TextDecoder();

export class Vault {
  private key: Promise<CryptoKey> | undefined;

  constructor(
    private readonly store: VaultStore,
    private readonly subtle: SubtleCrypto = crypto.subtle,
  ) {}

  /** The stored value, or `undefined` when there is none or it does not decrypt (then it is deleted). */
  async get<T>(provider: string): Promise<T | undefined> {
    const record = await this.store.get(provider);
    if (record === undefined) return undefined;
    try {
      if (record.v !== 1) throw new Error('unknown record version');
      const key = await this.existingKey();
      if (key === undefined) throw new Error('the vault key is gone');
      const plain = await this.subtle.decrypt(
        { name: 'AES-GCM', iv: record.iv as Uint8Array<ArrayBuffer>, additionalData: aad(provider) },
        key,
        record.ct,
      );
      return JSON.parse(decoder.decode(plain)) as T;
    } catch {
      await this.store.delete(provider);
      return undefined;
    }
  }

  async put(provider: string, value: unknown): Promise<void> {
    const key = await this.usableKey();
    const iv = crypto.getRandomValues(new Uint8Array(12));
    const ct = await this.subtle.encrypt(
      { name: 'AES-GCM', iv, additionalData: aad(provider) },
      key,
      encoder.encode(JSON.stringify(value)),
    );
    await this.store.put({ provider, v: 1, iv, ct });
  }

  delete(provider: string): Promise<void> {
    return this.store.delete(provider);
  }

  private async existingKey(): Promise<CryptoKey | undefined> {
    if (this.key !== undefined) return this.key;
    const stored = await this.store.getKey();
    if (stored !== undefined) this.key = Promise.resolve(stored);
    return stored;
  }

  private usableKey(): Promise<CryptoKey> {
    this.key ??= (async () => {
      const stored = await this.store.getKey();
      if (stored !== undefined) return stored;
      const fresh = (await this.subtle.generateKey({ name: 'AES-GCM', length: 256 }, false, [
        'encrypt',
        'decrypt',
      ])) as CryptoKey;
      // Another worker instance may have stored one first; everyone uses the stored key.
      return this.store.addKey(fresh);
    })().catch((error: unknown) => {
      this.key = undefined;
      throw error;
    });
    return this.key;
  }
}

function aad(provider: string): Uint8Array<ArrayBuffer> {
  return encoder.encode(`${AAD_PREFIX}${provider}`);
}

/** In-memory store for tests; `records` is exposed so tests can inspect what would be on disk. */
export class MemoryVaultStore implements VaultStore {
  key: CryptoKey | undefined;
  readonly records = new Map<string, VaultRecord>();

  async getKey(): Promise<CryptoKey | undefined> {
    return this.key;
  }
  async addKey(key: CryptoKey): Promise<CryptoKey> {
    this.key ??= key;
    return this.key;
  }
  async get(provider: string): Promise<VaultRecord | undefined> {
    return this.records.get(provider);
  }
  async put(record: VaultRecord): Promise<void> {
    this.records.set(record.provider, record);
  }
  async delete(provider: string): Promise<void> {
    this.records.delete(provider);
  }
}

/**
 * IndexedDB store: database `online-gal-vault` v1 with `keys` (the CryptoKey, stored by
 * structured clone, which Cache Storage cannot do) and `credentials` (records keyed by provider).
 */
export class IndexedDbVaultStore implements VaultStore {
  private db: Promise<IDBDatabase> | undefined;

  constructor(private readonly factory: IDBFactory = indexedDB) {}

  async getKey(): Promise<CryptoKey | undefined> {
    return (await this.request('keys', 'readonly', (store) => store.get(KEY_ID))) as CryptoKey | undefined;
  }

  async addKey(key: CryptoKey): Promise<CryptoKey> {
    try {
      await this.request('keys', 'readwrite', (store) => store.add(key, KEY_ID));
      return key;
    } catch (error) {
      if (!(error instanceof DOMException && error.name === 'ConstraintError')) throw error;
      const stored = await this.getKey();
      if (stored === undefined) throw error;
      return stored;
    }
  }

  async get(provider: string): Promise<VaultRecord | undefined> {
    return (await this.request('credentials', 'readonly', (store) => store.get(provider))) as VaultRecord | undefined;
  }

  async put(record: VaultRecord): Promise<void> {
    await this.request('credentials', 'readwrite', (store) => store.put(record));
  }

  async delete(provider: string): Promise<void> {
    await this.request('credentials', 'readwrite', (store) => store.delete(provider));
  }

  private open(): Promise<IDBDatabase> {
    this.db ??= new Promise<IDBDatabase>((resolve, reject) => {
      const request = this.factory.open(VAULT_DB, VAULT_DB_VERSION);
      request.onupgradeneeded = () => {
        const db = request.result;
        if (!db.objectStoreNames.contains('keys')) db.createObjectStore('keys');
        if (!db.objectStoreNames.contains('credentials')) db.createObjectStore('credentials', { keyPath: 'provider' });
      };
      request.onsuccess = () => {
        const db = request.result;
        // Another tab upgrading the schema: let it, and reopen on next use.
        db.onversionchange = () => {
          db.close();
          this.db = undefined;
        };
        resolve(db);
      };
      request.onerror = () => reject(request.error ?? new Error('cannot open the vault database'));
      request.onblocked = () => reject(new Error('the vault database is blocked by another tab'));
    }).catch((error: unknown) => {
      this.db = undefined;
      throw error;
    });
    return this.db;
  }

  private async request(
    name: 'keys' | 'credentials',
    mode: IDBTransactionMode,
    action: (store: IDBObjectStore) => IDBRequest,
  ): Promise<unknown> {
    const db = await this.open();
    return new Promise((resolve, reject) => {
      const tx = db.transaction(name, mode);
      const request = action(tx.objectStore(name));
      let result: unknown;
      request.onsuccess = () => {
        result = request.result;
      };
      tx.oncomplete = () => resolve(result);
      tx.onerror = () => reject(request.error ?? tx.error ?? new Error('vault transaction failed'));
      tx.onabort = () => reject(request.error ?? tx.error ?? new Error('vault transaction aborted'));
    });
  }
}
