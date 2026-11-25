/** Minimal promise wrapper over IndexedDB, shared by every store in the app. */

export interface DatabaseOptions {
  readonly name: string;
  readonly version: number;
  /** Object store names to create on upgrade. */
  readonly stores: readonly string[];
  readonly onUpgrade?: (db: IDBDatabase, transaction: IDBTransaction) => void;
}

export class Database {
  private db: IDBDatabase | null = null;
  private opening: Promise<IDBDatabase> | null = null;

  constructor(private readonly options: DatabaseOptions) {}

  open(): Promise<IDBDatabase> {
    if (this.db) return Promise.resolve(this.db);
    if (this.opening) return this.opening;

    this.opening = new Promise<IDBDatabase>((resolve, reject) => {
      const request = indexedDB.open(this.options.name, this.options.version);
      request.onupgradeneeded = (event) => {
        const db = request.result;
        const transaction = request.transaction!;
        for (const store of this.options.stores) {
          if (!db.objectStoreNames.contains(store)) db.createObjectStore(store);
        }
        this.options.onUpgrade?.(db, transaction);
        void event;
      };
      request.onsuccess = () => {
        this.db = request.result;
        // A version change from another tab invalidates this connection.
        this.db.onversionchange = () => {
          this.db?.close();
          this.db = null;
          this.opening = null;
        };
        resolve(request.result);
      };
      request.onerror = () => reject(request.error);
      request.onblocked = () =>
        reject(new Error('the database upgrade is blocked by another open tab'));
    });
    return this.opening;
  }

  async transaction<T>(
    stores: readonly string[],
    mode: IDBTransactionMode,
    action: (transaction: IDBTransaction) => Promise<T> | T,
  ): Promise<T> {
    const db = await this.open();
    return new Promise<T>((resolve, reject) => {
      const transaction = db.transaction(stores as string[], mode);
      let result: T;
      let settled = false;

      transaction.oncomplete = () => {
        if (!settled) {
          settled = true;
          resolve(result);
        }
      };
      transaction.onerror = () => {
        if (!settled) {
          settled = true;
          reject(transaction.error);
        }
      };
      transaction.onabort = () => {
        if (!settled) {
          settled = true;
          reject(transaction.error ?? new Error('transaction aborted'));
        }
      };

      Promise.resolve(action(transaction)).then(
        (value) => {
          result = value;
        },
        (error) => {
          if (!settled) {
            settled = true;
            reject(error);
          }
        },
      );
    });
  }

  async close(): Promise<void> {
    this.db?.close();
    this.db = null;
    this.opening = null;
  }
}

/** Awaits an `IDBRequest`, rejecting on error. */
export function request<T>(idbRequest: IDBRequest<T>): Promise<T> {
  return new Promise<T>((resolve, reject) => {
    idbRequest.onsuccess = () => resolve(idbRequest.result);
    idbRequest.onerror = () => reject(idbRequest.error);
  });
}

/** Selects string keys starting with a prefix ending in a delimiter. */
export function keyPrefixRange(prefix: string): IDBKeyRange {
  const end = prefix.slice(0, -1) + String.fromCharCode(prefix.charCodeAt(prefix.length - 1) + 1);
  return IDBKeyRange.bound(prefix, end, false, true);
}

/** Opens a cursor and collects every record. */
export function getAll<T>(store: IDBObjectStore): Promise<T[]> {
  return request(store.getAll() as IDBRequest<T[]>);
}

/** Serialises an operation per key so concurrent writes cannot interleave. */
export class KeyedQueue {
  private readonly chains = new Map<string, Promise<unknown>>();

  run<T>(key: string, task: () => Promise<T>): Promise<T> {
    const previous = this.chains.get(key) ?? Promise.resolve();
    const next = previous.then(task, task);
    this.chains.set(
      key,
      next.catch(() => undefined),
    );
    return next;
  }
}
