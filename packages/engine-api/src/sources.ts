import type { GameSource, WritableStore } from './types.js';

function normalise(path: string): string {
  return path
    .replace(/\\/g, '/')
    .replace(/^\.?\//, '')
    .replace(/\/{2,}/g, '/')
    .replace(/\/$/, '');
}

/**
 * An in-memory `GameSource`. Used by tests, by decoding-only fixtures, and as
 * the fallback for archives that were expanded wholesale.
 */
export class MemoryGameSource implements GameSource {
  private readonly files = new Map<string, Uint8Array>();
  private readonly dirs = new Set<string>(['']);
  readonly capabilities = {
    list: true,
    randomAccess: true,
    writable: true,
  } as const;

  constructor(readonly name = 'memory') {}

  addFile(path: string, data: Uint8Array | string): this {
    const key = normalise(path);
    const bytes =
      typeof data === 'string' ? new TextEncoder().encode(data) : data;
    this.files.set(key, bytes);
    this.registerParents(key);
    return this;
  }

  addDirectory(path: string): this {
    const key = normalise(path);
    if (key) {
      this.dirs.add(key);
      this.registerParents(key);
    }
    return this;
  }

  get fileCount(): number {
    return this.files.size;
  }

  paths(): readonly string[] {
    return [...this.files.keys()].sort();
  }

  async list(dir = ''): Promise<readonly string[]> {
    const key = normalise(dir);
    const prefix = key ? `${key}/` : '';
    const out = new Set<string>();
    for (const file of this.files.keys()) {
      if (!file.startsWith(prefix)) continue;
      const rest = file.slice(prefix.length);
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
    const key = normalise(path);
    if (this.dirs.has(key) && !this.files.has(key)) {
      throw new Error(`"${path}" is a directory`);
    }
    const data = this.files.get(key);
    if (!data) throw new Error(`no such file: "${path}"`);
    return data;
  }

  async readRange(
    path: string,
    offset: number,
    length: number,
  ): Promise<Uint8Array> {
    const data = await this.readFile(path);
    return data.subarray(offset, offset + length);
  }

  async size(path: string): Promise<number> {
    const data = this.files.get(normalise(path));
    return data ? data.byteLength : -1;
  }

  async exists(path: string): Promise<boolean> {
    const key = normalise(path);
    return this.files.has(key) || this.dirs.has(key);
  }

  async isDirectory(path: string): Promise<boolean> {
    const key = normalise(path);
    if (!key) return true;
    if (this.files.has(key)) return false;
    return this.dirs.has(key);
  }

  private registerParents(path: string): void {
    let index = path.lastIndexOf('/');
    while (index > 0) {
      this.dirs.add(path.slice(0, index));
      index = path.lastIndexOf('/', index - 1);
    }
  }
}

/** In-memory `WritableStore`, the default when persistence is switched off. */
export class MemoryStore implements WritableStore {
  private readonly files = new Map<string, Uint8Array>();

  constructor(readonly name = 'memory') {}

  async read(path: string): Promise<Uint8Array | undefined> {
    return this.files.get(normalise(path));
  }

  async write(path: string, data: Uint8Array): Promise<void> {
    this.files.set(normalise(path), data);
  }

  async remove(path: string): Promise<void> {
    this.files.delete(normalise(path));
  }

  async list(prefix = ''): Promise<readonly string[]> {
    const key = normalise(prefix);
    return [...this.files.keys()].filter((entry) => entry.startsWith(key)).sort();
  }
}

/**
 * Overlays a writable store on top of a read-only source.
 *
 * This is how browser saves work: the imported game directory is never
 * modified, while `savedata/` and friends are redirected to IndexedDB — the
 * same split AetherKiri applies to its cloud Web build.
 */
export class OverlayGameSource implements GameSource {
  readonly name: string;
  readonly capabilities: GameSource['capabilities'];
  private readonly writePrefix: string;

  constructor(
    private readonly base: GameSource,
    private readonly store: WritableStore,
    options: { name?: string; writePrefix?: string } = {},
  ) {
    this.name = options.name ?? base.name;
    this.writePrefix = options.writePrefix
      ? `${normalise(options.writePrefix)}/`
      : '';
    this.capabilities = {
      list: base.capabilities.list,
      randomAccess: base.capabilities.randomAccess,
      writable: true,
    };
  }

  private storePath(path: string): string {
    return `${this.writePrefix}${normalise(path)}`;
  }

  async list(dir = ''): Promise<readonly string[]> {
    const entries = new Set(await this.base.list(dir));
    const prefix = normalise(dir) ? `${normalise(dir)}/` : '';
    for (const stored of await this.store.list(this.writePrefix)) {
      const relative = stored.slice(this.writePrefix.length);
      if (!relative.startsWith(prefix)) continue;
      const rest = relative.slice(prefix.length);
      if (!rest) continue;
      const slash = rest.indexOf('/');
      entries.add(slash === -1 ? rest : `${rest.slice(0, slash)}/`);
    }
    return [...entries].sort();
  }

  async readFile(path: string): Promise<Uint8Array> {
    const stored = await this.store.read(this.storePath(path));
    if (stored) return stored;
    return this.base.readFile(path);
  }

  async readRange(
    path: string,
    offset: number,
    length: number,
  ): Promise<Uint8Array> {
    const stored = await this.store.read(this.storePath(path));
    if (stored) return stored.subarray(offset, offset + length);
    if (this.base.readRange) return this.base.readRange(path, offset, length);
    const whole = await this.base.readFile(path);
    return whole.subarray(offset, offset + length);
  }

  async size(path: string): Promise<number> {
    const stored = await this.store.read(this.storePath(path));
    if (stored) return stored.byteLength;
    return this.base.size(path);
  }

  async exists(path: string): Promise<boolean> {
    if (await this.store.read(this.storePath(path))) return true;
    return this.base.exists(path);
  }

  async isDirectory(path: string): Promise<boolean> {
    const key = normalise(path);
    if (key) {
      if (await this.store.read(this.storePath(key))) return false;
      const stored = await this.store.list(this.storePath(key));
      if (stored.length > 0) return true;
    }
    return this.base.isDirectory(path);
  }

  /** Writes into the overlay layer only. */
  async writeGameFile(path: string, data: Uint8Array): Promise<void> {
    await this.store.write(this.storePath(path), data);
  }
}
