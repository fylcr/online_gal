/**
 * The host byte-source seam consulted by `platforms/web/vlfs.js`.
 *
 * The KiriKiri WASM build reads remote bytes through a seam on the page: if the
 * page installs `window.__onlineGalVfsSource` before `vlfs.js` runs, every remote
 * byte read goes through it instead of `fetch`. That seam is intentionally
 * tiny — a single `readRange(url, offset, length, headers?)` method — so a host
 * can answer ranges from any backing store: a `GameSource`, a `LazyFileSystem`,
 * an in-memory cache, or something injected by a wrapper.
 *
 * This module is the platform side of that contract. It turns any
 * range-readable source into a `HostByteSource`, maps the engine's URLs onto the
 * source's keys, caches opened handles per key, and installs itself where the
 * seam looks for it. None of it depends on `fetch`, `document`, `Blob` or
 * `window`, so the module imports cleanly in Node, in workers and in browsers.
 */

import type { GameSource } from '@online-gal/engine-api';
import { normalisePath } from './path.js';

/** The object `platforms/web/vlfs.js` consults on the page as `window.__onlineGalVfsSource`. */
export interface HostByteSource {
  /**
   * Returns at most `length` bytes starting at `offset` for `url`.
   *
   * A result shorter than `length` means end of file rather than an error, and
   * the bytes are returned untouched so the caller decides what to do.
   */
  readRange(
    url: string,
    offset: number,
    length: number,
    headers?: Record<string, string>,
  ): Promise<Uint8Array>;
  /**
   * Size of `url`, plus a strong validator when the source has one.
   *
   * Resolves to `null` when the source does not have the file, which makes the
   * caller fall back to an HTTP `HEAD`. This is what lets a source that has no
   * URL at all — an OPFS directory, a File System Access handle, a file inside
   * an archive — be mounted by the shell.
   */
  stat?(url: string): Promise<{ size: number; etag?: string } | null>;
}

/** The global key the seam reads. */
export const HOST_BYTE_SOURCE_KEY = '__onlineGalVfsSource';

/**
 * A file that can answer byte-range reads; `GameSource` and `LazyFileSystem`
 * both satisfy it.
 */
export interface RangeReadable {
  /** Size in bytes. */
  size(): Promise<number>;
  /**
   * Reads `length` bytes at `offset`, shorter than `length` at end of file.
   * The bytes are passed through unchanged: no padding, no truncation.
   */
  readRange(offset: number, length: number): Promise<Uint8Array>;
}

export interface HostByteSourceOptions {
  /**
   * Maps the URL the engine asked for onto a key this source can open.
   * Defaults to `defaultUrlKey`.
   */
  resolveUrl?(url: string): string;
}

/**
 * Maps a page URL onto a source path.
 *
 * Drop the query and fragment, percent-decode defensively (a malformed `%`
 * must never throw), convert `\` to `/`, strip leading slashes, then
 * `normalisePath`. So `/data.xp3` → `data.xp3`; `data.xp3?v=2#x` → `data.xp3`;
 * `http://host/a/b%20c.xp3?x=1` → `a/b c.xp3`; empty input → `''`.
 */
export function defaultUrlKey(url: string): string {
  const withoutQuery = dropQueryAndFragment(url);
  const decoded = safeDecode(withoutQuery);
  const slashFixed = decoded.replace(/\\/g, '/');
  const pathOnly = stripUrlAuthority(slashFixed);
  return normalisePath(pathOnly.replace(/^\/+/, ''));
}

/** Cuts a `?` or `#` and everything after it from `url`. */
function dropQueryAndFragment(url: string): string {
  const query = url.indexOf('?');
  const hash = url.indexOf('#');
  let cut = -1;
  if (query !== -1 && hash !== -1) cut = Math.min(query, hash);
  else if (query !== -1) cut = query;
  else if (hash !== -1) cut = hash;
  return cut === -1 ? url : url.slice(0, cut);
}

/**
 * Percent-decodes `value`, never throwing on a malformed sequence.
 *
 * `decodeURIComponent` rejects an incomplete escape such as `%E0`; the caller is
 * a host seam and must not surface that as a crash, so malformed runs are left
 * untouched instead.
 */
function safeDecode(value: string): string {
  try {
    return decodeURIComponent(value);
  } catch {
    return value.replace(/%[0-9A-Fa-f]{2}/g, (escape) => {
      try {
        return decodeURIComponent(escape);
      } catch {
        return escape;
      }
    });
  }
}

/**
 * Drops a URL scheme and authority, leaving the path.
 *
 * A string with no scheme is returned unchanged, so a bare path such as
 * `/data.xp3` is unaffected. `http://host/a/b` becomes `/a/b`, whose leading
 * slash is then dropped by the caller before normalisation.
 */
function stripUrlAuthority(path: string): string {
  const colon = path.indexOf(':');
  if (colon === -1) return path;
  const firstSlash = path.indexOf('/');
  if (firstSlash === -1 || colon > firstSlash) return path;
  // `scheme://` requires two slashes immediately after the colon.
  if (path.charCodeAt(colon + 1) !== 47 /* / */ || path.charCodeAt(colon + 2) !== 47 /* / */) {
    return path;
  }
  // The authority runs up to the next slash; keep the path after it.
  let end = colon + 3;
  while (end < path.length && path.charCodeAt(end) !== 47 /* / */) end += 1;
  return path.slice(end);
}

/**
 * Builds a `HostByteSource` that resolves keys with `open`.
 *
 * `open` is called at most once per key: the first read of a key opens it and
 * every later read reuses the handle, so repeated reads of one file never reopen
 * it. A key whose `open` rejects is cleared from the cache immediately, so a
 * transient failure retries on the next read instead of sticking around as a
 * cached rejection. A key whose `open` resolves to `null` is remembered as known
 * not to exist and surfaces the standard `no such file` error.
 */
export function createHostByteSource(
  open: (key: string) => Promise<RangeReadable | null> | RangeReadable | null,
  options?: HostByteSourceOptions,
): HostByteSource {
  const resolveUrl = options?.resolveUrl ?? defaultUrlKey;
  const handles = new Map<string, Promise<RangeReadable | null>>();

  const openOnce = (key: string): Promise<RangeReadable | null> => {
    const cached = handles.get(key);
    if (cached) return cached;
    const opened = Promise.resolve(open(key));
    handles.set(key, opened);
    // Only a rejection clears the cache; a resolved `null` means the key is
    // known not to exist and is just as safe to remember.
    void opened.catch(() => {
      handles.delete(key);
    });
    return opened;
  };

  return {
    async readRange(url, offset, length) {
      void length;
      const key = resolveUrl(url);
      const file = await openOnce(key);
      if (file === null) {
        throw new Error(`no such file: "${url}" (key "${key}")`);
      }
      return file.readRange(offset, length);
    },

    async stat(url) {
      const key = resolveUrl(url);
      const file = await openOnce(key);
      // A missing file resolves to `null` rather than throwing: the caller falls
      // back to an HTTP HEAD, and a probe is not an error.
      if (file === null) return null;
      return { size: await file.size() };
    },
  };
}

/**
 * Convenience wrapper that adapts any `GameSource` to the `HostByteSource` seam.
 *
 * The source is probed with `exists` before a read is attempted, so a missing
 * file surfaces the seam's `no such file` error rather than whatever the
 * underlying source would throw on its own. Reads are delegated to the source's
 * own range reader when it has one, and fall back to a whole-file read
 * otherwise, but in either case the bytes are passed through unchanged.
 */
export function hostByteSourceFromGameSource(
  source: GameSource,
  options?: HostByteSourceOptions,
): HostByteSource {
  return createHostByteSource(
    async (key) => {
      if (!(await source.exists(key))) return null;
      const file: RangeReadable = {
        size: () => source.size(key),
        readRange: (offset, length) =>
          source.readRange
            ? source.readRange(key, offset, length)
            : source.readFile(key).then((bytes) => bytes.subarray(offset, offset + length)),
      };
      return file;
    },
    options,
  );
}

/**
 * Installs `source` where the seam looks for it.
 *
 * The target defaults to `globalThis` — the one object present in browsers,
 * workers and Node — so a single call is enough for `vlfs.js` to find
 * `__onlineGalVfsSource` everywhere. The target is taken by parameter, so a
 * caller can install onto a sandbox or a stub without touching the real global,
 * and the module itself never names `window`.
 */
export function installHostByteSource(
  source: HostByteSource,
  target: Record<string, unknown> = globalThis as unknown as Record<string, unknown>,
): void {
  target[HOST_BYTE_SOURCE_KEY] = source;
}
