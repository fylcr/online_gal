import type { GameSource } from '@online-gal/engine-api';
import { dirname, normalisePath } from './path.js';

/**
 * A `GameSource` that chains several sources together.
 *
 * This is how a KiriKiri game folder is presented to a runtime: loose files at
 * the root take precedence, then the game's `data.xp3`, then any supplementary
 * package. Engine probing sees one merged directory, which matches how the
 * native engines resolve paths.
 */
export class CompositeGameSource implements GameSource {
  readonly name: string;
  readonly capabilities: GameSource['capabilities'];
  private readonly layers: readonly GameSource[];

  constructor(name: string, layers: readonly GameSource[]) {
    if (layers.length === 0) {
      throw new Error('CompositeGameSource requires at least one layer');
    }
    this.name = name;
    this.layers = layers;
    this.capabilities = {
      list: layers.some((layer) => layer.capabilities.list),
      randomAccess: layers.every((layer) => layer.capabilities.randomAccess),
      writable: layers.some((layer) => layer.capabilities.writable),
    };
  }

  get layerCount(): number {
    return this.layers.length;
  }

  async list(dir = ''): Promise<readonly string[]> {
    const merged = new Set<string>();
    for (const layer of this.layers) {
      if (!layer.capabilities.list) continue;
      try {
        for (const entry of await layer.list(dir)) merged.add(entry);
      } catch {
        // A layer that cannot list at this level simply contributes nothing.
      }
    }
    return [...merged].sort();
  }

  private async find(path: string): Promise<GameSource | undefined> {
    for (const layer of this.layers) {
      try {
        if (await layer.exists(path)) {
          if (await layer.isDirectory(path)) continue;
          return layer;
        }
      } catch {
        // Ignore and fall through to the next layer.
      }
    }
    return undefined;
  }

  async readFile(path: string): Promise<Uint8Array> {
    const layer = await this.find(path);
    if (!layer) throw new Error(`no such file in any layer: "${path}"`);
    return layer.readFile(path);
  }

  async readRange(
    path: string,
    offset: number,
    length: number,
  ): Promise<Uint8Array> {
    const layer = await this.find(path);
    if (!layer) throw new Error(`no such file in any layer: "${path}"`);
    if (layer.readRange) return layer.readRange(path, offset, length);
    const whole = await layer.readFile(path);
    return whole.subarray(offset, offset + length);
  }

  async size(path: string): Promise<number> {
    const layer = await this.find(path);
    return layer ? layer.size(path) : -1;
  }

  async exists(path: string): Promise<boolean> {
    return (await this.find(path)) !== undefined;
  }

  async isDirectory(path: string): Promise<boolean> {
    const key = normalisePath(path);
    if (!key) return true;
    if (await this.find(key)) return false;
    for (const layer of this.layers) {
      try {
        if (await layer.isDirectory(key)) return true;
      } catch {
        // Ignore.
      }
    }
    return false;
  }

  /** Every layer that exists, for diagnostics. */
  describeLayers(): readonly string[] {
    return this.layers.map((layer) => layer.name);
  }
}

/** Finds the unique file with `basename` anywhere under the root. */
export async function findUniqueFile(
  source: GameSource,
  filename: string,
): Promise<string | null> {
  const wanted = filename.toLowerCase();
  const matches: string[] = [];
  const queue: string[] = [''];
  const seen = new Set<string>();
  while (queue.length > 0) {
    const dir = queue.shift()!;
    if (seen.has(dir)) continue;
    seen.add(dir);
    let entries: readonly string[];
    try {
      entries = await source.list(dir);
    } catch {
      continue;
    }
    for (const entry of entries) {
      const path = dir ? `${dir}/${entry}` : entry;
      if (entry.endsWith('/')) {
        queue.push(path.slice(0, -1));
      } else if (entry.toLowerCase() === wanted) {
        matches.push(path);
        if (matches.length > 1) return null;
      }
    }
  }
  return matches[0] ?? null;
}

/** Resolves `path`, tolerating case differences. */
export async function resolveCaseInsensitive(
  source: GameSource,
  path: string,
): Promise<string | null> {
  const key = normalisePath(path);
  if (await source.exists(key)) return key;
  const entries = await source.list(dirname(key));
  const leaf = key.slice(key.lastIndexOf('/') + 1).toLowerCase();
  for (const entry of entries) {
    if (entry.toLowerCase() === leaf) {
      const dir = dirname(key);
      return dir ? `${dir}/${entry}` : entry;
    }
  }
  return null;
}
