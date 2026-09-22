import type { GameSource } from '@online-gal/engine-api';
import { decodeImage, type DecodedImage } from '@online-gal/image-codec';
import { normalisePath } from '@online-gal/vfs';

interface CacheEntry {
  readonly image: DecodedImage;
  bytes: number;
  lastUsed: number;
}

export interface AssetManagerOptions {
  /** Soft cap on decoded pixel memory before old entries are evicted. */
  readonly cacheLimitBytes?: number;
  readonly onMissing?: (path: string) => void;
}

/**
 * Loads and caches images and audio for a session.
 *
 * Only decoded pixel memory is capped; the eviction policy is least-recently
 * used because KAG scenes cycle through a small working set of backgrounds and
 * character sprites, and evicting in-use art causes visible stalls.
 */
export class AssetManager {
  private readonly images = new Map<string, CacheEntry>();
  private readonly inflight = new Map<string, Promise<DecodedImage>>();
  private bytes = 0;
  private clock = 0;
  private readonly cacheLimit: number;

  constructor(
    private readonly source: GameSource,
    private readonly options: AssetManagerOptions = {},
  ) {
    this.cacheLimit = options.cacheLimitBytes ?? 192 * 1024 * 1024;
  }

  get cachedImageCount(): number {
    return this.images.size;
  }

  get cachedBytes(): number {
    return this.bytes;
  }

  /** Resolves a path to its real (case-correct) VFS path. */
  private async resolve(path: string): Promise<string | null> {
    const key = normalisePath(path);
    if (await this.source.exists(key)) return key;
    // KAG scripts are notoriously inconsistent about case.
    const dir = key.includes('/') ? key.slice(0, key.lastIndexOf('/')) : '';
    const leaf = key.slice(key.lastIndexOf('/') + 1).toLowerCase();
    for (const entry of await this.source.list(dir)) {
      if (entry.toLowerCase() === leaf) return dir ? `${dir}/${entry}` : entry;
    }
    return null;
  }

  async loadImage(path: string): Promise<DecodedImage> {
    const key = normalisePath(path).toLowerCase();
    const cached = this.images.get(key);
    if (cached) {
      this.clock += 1;
      cached.lastUsed = this.clock;
      return cached.image;
    }
    const pending = this.inflight.get(key);
    if (pending) return pending;

    const task = (async () => {
      const resolved = await this.resolve(path);
      if (!resolved) throw new Error(`image not found: "${path}"`);
      const bytes = await this.source.readFile(resolved);
      const decoded = await decodeImage(bytes, { path: resolved });
      this.store(key, decoded);
      return decoded;
    })();

    this.inflight.set(key, task);
    try {
      return await task;
    } finally {
      this.inflight.delete(key);
    }
  }

  /** Loads an image, returning null instead of throwing. */
  async tryLoadImage(path: string): Promise<DecodedImage | null> {
    try {
      return await this.loadImage(path);
    } catch {
      this.options.onMissing?.(path);
      return null;
    }
  }

  private store(key: string, image: DecodedImage): void {
    const size = image.width * image.height * 4;
    this.clock += 1;
    this.images.set(key, { image, bytes: size, lastUsed: this.clock });
    this.bytes += size;

    while (this.bytes > this.cacheLimit && this.images.size > 1) {
      let oldestKey: string | null = null;
      let oldest = Number.POSITIVE_INFINITY;
      for (const [candidate, entry] of this.images) {
        if (entry.lastUsed < oldest) {
          oldest = entry.lastUsed;
          oldestKey = candidate;
        }
      }
      if (!oldestKey) break;
      const entry = this.images.get(oldestKey)!;
      this.release(entry.image);
      this.images.delete(oldestKey);
      this.bytes -= entry.bytes;
    }
  }

  private release(image: DecodedImage): void {
    const source = image.source as { close?: () => void };
    source.close?.();
  }

  /** Warms the cache for a list of paths, ignoring failures. */
  async preload(paths: readonly string[]): Promise<void> {
    await Promise.all(paths.map((path) => this.tryLoadImage(path)));
  }

  clear(): void {
    for (const entry of this.images.values()) this.release(entry.image);
    this.images.clear();
    this.inflight.clear();
    this.bytes = 0;
  }
}
