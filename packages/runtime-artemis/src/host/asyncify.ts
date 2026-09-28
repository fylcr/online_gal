import { ArtemisHostError } from './errno.js';
import type { ArtemisMemory } from './memory.js';

/**
 * The `asyncify` bucket of `ports/artemis/harness/glue/imports-map.json` — 4 rows, 4 short names.
 *
 * Three of them are the emscripten download family, and the fourth is the spill that keeps a large
 * copy from being a `memcpy` the optimiser will try to inline. Together they are the *surface* of
 * Asyncify without the machinery: the glue's bodies call `Asyncify.handleSleep`, which unwinds the
 * wasm stack out to JavaScript, runs a real `XMLHttpRequest`, and rewinds on completion. A host has
 * neither an event loop to unwind into nor a browser to ask, so the honest shape is a **resolver
 * callback**: the host hands the request back to whatever fetches URLs in this session, and the
 * callback writes the result into the guest's out-pointers exactly as the glue's own completion
 * handler does.
 *
 * That is a deliberate difference in mechanism, not in contract. The observable effect of
 * `_emscripten_wget_data(url, pbuffer, pnum, perror)` is the same either way: on success
 * `*pbuffer` is a guest pointer to `*pnum` bytes and `*perror` is 0; on failure `*perror` is 1. When
 * no resolver is supplied this host writes the failure form and records the refusal, rather than
 * inventing a file descriptor for bytes nobody fetched. A stub that returned 0 with a null buffer
 * would hand the guest a segfault three frames later.
 */

/** One request's worth of decoded strings, so a resolver need not re-read the guest heap. */
export interface ArtemisWgetRequest {
  /** The URL, NUL-terminated in the guest heap. */
  readonly url: string;
  /** HTTP method — `GET` for the two plain callers, `POST` for `_emscripten_async_wget2_data`. */
  readonly method: string;
  /** POST body, for the `wget2` variant. Empty for a GET. */
  readonly body: string;
}

/** A resolver answers a request synchronously. A session with a real fetcher returns its bytes. */
export type ArtemisWgetResolver = (request: ArtemisWgetRequest) => Uint8Array | null;

/** What a session must supply for the three download rows to fetch anything. */
export interface ArtemisAsyncifyHost {
  readonly memory: ArtemisMemory;
  /** The guest's `malloc`, needed to hand back a buffer the guest owns and frees. */
  readonly malloc?: (bytes: number) => number;
  /** The guest's `free`, used to release the buffer after a callback. */
  readonly free?: (pointer: number) => void;
  /**
   * Invokes one of the guest's function-table entries — the host's stand-in for the glue's
   * `getDynCaller`. The two `_async_wget*_data` rows are callback-shaped: they take a table index
   * and call it when the transfer lands. Without one they record the request and return the
   * failure form, which is what a browser-less host can honestly do.
   */
  readonly callback?: (
    entry: number,
    arg: number,
    buffer: number,
    length: number,
    status: number,
    statusText: string,
  ) => void;
  /** Fetches a URL. Absent means this host does no networking. */
  readonly resolve?: ArtemisWgetResolver;
  /** Reports a call this host could not serve, so a session can log it instead of swallowing it. */
  readonly onBlocked?: (what: string, detail: string) => void;
}

/** The import table, keyed by the wasm short name. */
export type ArtemisAsyncifyImports = Record<string, (...args: number[]) => number | void>;

/** The glue's `Brotli`/size ceiling is not the issue here; this is a sanity bound on one transfer. */
const MAX_TRANSFER_BYTES = 1 << 30;

/** Builds the `asyncify` import table for one instance. */
export function createArtemisAsyncifyImports(host: ArtemisAsyncifyHost): ArtemisAsyncifyImports {
  const { memory } = host;
  const report = (what: string, detail: string): void => host.onBlocked?.(what, detail);
  const malloc = host.malloc;
  const free = host.free;

  /**
   * Reads a NUL-terminated string out of the guest heap.
   *
   * The `| 0` is not decoration: a wasm i32 argument arrives in JavaScript as a *signed* number, so
   * a heap address above 2^31 comes through negative and has to be squeezed back to the unsigned
   * value the memory accessor expects.
   */
  const utf8 = (pointer: number): string => {
    let end = (pointer | 0) >>> 0;
    const base = memory.u8;
    while (end < base.length && base[end] !== 0) end += 1;
    const decoder = new TextDecoder();
    return decoder.decode(base.subarray((pointer | 0) >>> 0, end));
  };

  /**
   * Copies bytes into a guest buffer the guest owns.
   *
   * The glue does `_malloc(n)` then `HEAPU8.set(bytes, buffer>>>0)`. Copying is not optional: the
   * caller's `Uint8Array` may be a view into the guest heap itself, and handing that back as if it
   * were a separate allocation would alias the source and the destination.
   *
   * Returns 0 when there is no allocator or the request exceeds {@link MAX_TRANSFER_BYTES}, which is
   * the same condition that makes the glue's `_malloc` return 0 and the copy a no-op.
   */
  const toGuestBuffer = (bytes: Uint8Array): number => {
    if (!malloc || bytes.length > MAX_TRANSFER_BYTES) return 0;
    const buffer = malloc(bytes.length) | 0;
    if (buffer === 0) return 0;
    memory.u8.set(bytes, buffer >>> 0);
    return buffer;
  };

  /**
   * Runs the resolver, if there is one, and returns the bytes.
   *
   * A resolver that throws is a host defect, not a guest-visible failure, so it is re-raised as
   * {@link ArtemisHostError} rather than being reported as a failed download.
   */
  const fetch = (request: ArtemisWgetRequest, what: string): Uint8Array | null => {
    if (!host.resolve) {
      report(what, `no resolver is installed, so ${request.method} ${request.url} was not fetched`);
      return null;
    }
    let bytes: Uint8Array | null;
    try {
      bytes = host.resolve(request);
    } catch (error) {
      throw new ArtemisHostError(`artemis ${what} resolver threw: ${String(error)}`);
    }
    return bytes ?? null;
  };

  /** `Browser.getNextWgetRequestHandle` — the glue's monotonic request-handle counter. */
  const handles = { next: 0 };

  return {
    /**
     * `_emscripten_wget_data(url, pbuffer, pnum, perror)` — the Asyncify blocking download.
     *
     * The glue sleeps on `Asyncify.handleSleep` and fills the three out-pointers from the
     * completion callback. Here the resolution is synchronous, so the same three writes happen
     * before returning, and the return value stays 0 because the import's type is `void`.
     *
     * `perror` is the one that matters: it is 0 on success and 1 on any failure, including "this
     * host has no network". A caller that only checks `perror` behaves identically under both.
     */
    fa: (url: number, pbuffer: number, pnum: number, perror: number): void => {
      const request = { url: utf8(url), method: 'GET', body: '' };
      const bytes = fetch(request, 'wget_data');
      if (bytes === null) {
        memory.i32[(perror | 0) >>> 2] = 1;
        return;
      }
      const buffer = toGuestBuffer(bytes);
      if (buffer === 0) {
        report('wget_data', `${request.url} resolved to ${bytes.length} B but the guest could not take it`);
        memory.i32[(perror | 0) >>> 2] = 1;
        return;
      }
      memory.i32[(pbuffer | 0) >>> 2] = buffer;
      memory.i32[(pnum | 0) >>> 2] = bytes.length;
      memory.i32[(perror | 0) >>> 2] = 0;
    },

    /**
     * `_emscripten_async_wget_data(url, arg, onload, onerror)` — the callback-shaped download.
     *
     * The glue calls `onload(arg, buffer, length)` with a buffer it frees immediately afterwards,
     * so the callback must copy what it wants. This host does the same: the buffer is handed over
     * and then released, which is why `free` is called after the callback and not before.
     */
    Qa: (url: number, arg: number, onload: number, onerror: number): void => {
      const request = { url: utf8(url), method: 'GET', body: '' };
      const bytes = fetch(request, 'async_wget_data');
      if (bytes === null) {
        if (onerror && host.callback) host.callback(onerror | 0, arg | 0, 0, 0, 0, 'no resolver');
        return;
      }
      const buffer = toGuestBuffer(bytes);
      if (buffer === 0) {
        report('async_wget_data', `${request.url} resolved to ${bytes.length} B but the guest could not take it`);
        if (onerror && host.callback) host.callback(onerror | 0, arg | 0, 0, 0, 0, 'out of memory');
        return;
      }
      if (onload && host.callback) host.callback(onload | 0, arg | 0, buffer, bytes.length, 0, '');
      free?.(buffer);
    },

    /**
     * `_emscripten_async_wget2_data(url, request, param, arg, free, onload, onerror, onprogress)`.
     *
     * The XHR-shaped download: it carries an HTTP method, a body, a `free` flag telling it whether to
     * release the buffer after the callback, and an `onprogress` it may or may not have. Returns a
     * request handle so the caller can abort it later — the glue hands back a counter entry.
     *
     * The status arithmetic the glue uses is worth copying exactly: a 2xx status succeeds, and so
     * does status 0 on a non-`http:` URL, which is how a `file://` or data-URI read succeeds. A
     * non-2xx goes to `onerror` with the status and its text.
     */
    Jc: (
      url: number,
      request: number,
      param: number,
      arg: number,
      freeFlag: number,
      onload: number,
      onerror: number,
      onprogress: number,
    ): number => {
      const decoded: ArtemisWgetRequest = {
        url: utf8(url),
        method: utf8(request),
        body: utf8(param),
      };
      const handle = handles.next;
      handles.next += 1;
      // A non-2xx status is the only thing the onerror path can carry, so it is decided before the
      // request even runs: this host has one success form and one failure form, not a status space.
      const bytes = fetch(decoded, 'async_wget2_data');
      if (bytes === null) {
        if (onerror && host.callback) {
          host.callback(onerror | 0, arg | 0, 0, 0, 0, 'no resolver');
        }
        return handle;
      }
      const buffer = toGuestBuffer(bytes);
      if (buffer === 0) {
        report('async_wget2_data', `${decoded.url} resolved to ${bytes.length} B but the guest could not take it`);
        if (onerror && host.callback) host.callback(onerror | 0, arg | 0, 0, 0, 0, 'out of memory');
        return handle;
      }
      if (onprogress && host.callback) {
        host.callback(onprogress | 0, arg | 0, 0, bytes.length, 0, '');
      }
      if (onload && host.callback) {
        host.callback(onload | 0, arg | 0, buffer, bytes.length, 0, '');
      }
      if (freeFlag) free?.(buffer);
      return handle;
    },

    /**
     * `_emscripten_memcpy_big(dest, src, num)` — the Asyncify spill copy.
     *
     * The glue is one `copyWithin` and returns nothing, so this returns 0. `num` is an i32, so it
     * can be negative if the caller asked for an absurd length; the glue would then copy
     * `src+num < src` bytes, i.e. none, because `copyWithin`'s end argument clamps. Squeezing the
     * end to at least the start reproduces that clamp rather than throwing on a range check the
     * original does not have.
     *
     * A `subarray` is deliberately not used: `copyWithin` on the live `u8` view is one pass over the
     * bytes, and an overlapping copy in the wrong direction would need a temporary, which the glue
     * does not build either — wasm's own `memory.copy` is byte-accurate in both directions, and this
     * host is faithful to what the *glue* does here rather than to what the wasm spec would allow.
     */
    yb: (dest: number, src: number, num: number): number => {
      const start = (dest | 0) >>> 0;
      const from = (src | 0) >>> 0;
      const end = Math.max(start, from + (num | 0));
      memory.u8.copyWithin(start, from, end);
      return 0;
    },
  };
}

/** The bridge's 4 `asyncify` short names, in the order `imports-map.json` first lists them. */
export const ARTEMIS_ASYNCIFY_IMPORTS: readonly string[] = ['Jc', 'Qa', 'fa', 'yb'];

/** The official name behind each `asyncify` short name, for a report a human can read. */
export const ARTEMIS_ASYNCIFY_OFFICIAL: Readonly<Record<string, string>> = {
  Jc: '_emscripten_async_wget2_data',
  Qa: '_emscripten_async_wget_data',
  fa: '_emscripten_wget_data',
  yb: '_emscripten_memcpy_big',
};
