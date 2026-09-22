/**
 * Mounts the KiriKiri WebAssembly build in a same-origin iframe and feeds the
 * game's bytes into it through the host byte-source seam.
 *
 * The build is a self-contained `index.html` produced from the reference repo's
 * `shell.html`. It has no knowledge of this host's `GameSource`, so the host
 * mounts its file system by installing `window.__onlineGalVfsSource` (the seam
 * documented in `host-source.ts`) onto the frame's window. Which virtual file to
 * expose is signalled through a URL parameter rather than an HTTP path: `?xp3=`
 * names one archive mounted lazily and `?game=` names a whole ZIP. That key is
 * virtual and never an HTTP URL, so no `fetch` and no server routing are
 * involved — the parameter only tells the shell *which* source key to ask the seam
 * for.
 *
 * A frame's window is replaced the moment its navigation starts, so an install
 * performed before `load` lands on the `about:blank` document and is discarded.
 * The seam is therefore installed on every `load` event, and `ready()` resolves
 * on the first such install — "the loaded page carries the seam" is the only
 * thing the host can observe from the outside, and it is the thing that matters,
 * since the shell reads the seam lazily, after wasm instantiation.
 */

import type { GameSource } from '@online-gal/engine-api';
import {
  HOST_BYTE_SOURCE_KEY,
  hostByteSourceFromGameSource,
  installHostByteSource,
  type HostByteSource,
} from '@online-gal/vfs';

/** Options for {@link createKiriPage}. */
export interface KiriPageOptions {
  /**
   * The document that owns the iframe. Taken explicitly (rather than the global
   * `document`) so the frame can be created against a non-default document, e.g.
   * inside a test fixture or a shadow tree.
   */
  readonly document: Document;
  /** URL of the built page (`index.html`). A `?game=`/`?xp3=` parameter is appended to it. */
  readonly pageUrl: string;
  /** Game bytes are read through this source. */
  readonly source: GameSource;
  /** Virtual key the shell is told to mount (e.g. `data.xp3`). */
  readonly gameKey: string;
  /**
   * Which URL parameter names the mount: `xp3` (default, one archive, lazy) or
   * `game` (a whole ZIP).
   */
  readonly param?: 'xp3' | 'game';
  /** Overrides the default URL-to-key mapping the seam uses to resolve reads. */
  readonly resolveUrl?: (url: string) => string;
  /** Element to append the frame to. Defaults to `document.body`. */
  readonly container?: Element | null;
}

/** A handle on the live iframe created by {@link createKiriPage}. */
export interface KiriPageHandle {
  /** The iframe element, already appended to its container. */
  readonly frame: HTMLIFrameElement;
  /** The byte source installed into the frame's window. */
  readonly hostSource: HostByteSource;
  /**
   * Resolves once an install of the host source has happened, or rejects when the
   * build has not wired up the seam within `timeoutMs` (default 30_000).
   */
  ready(timeoutMs?: number): Promise<void>;
  /** Removes the frame and tears down listeners/timers. Safe to call twice. */
  dispose(): void;
}

/**
 * Builds the framed page URL by appending the game key as a query parameter.
 *
 * A literal `?` in `pageUrl` is respected (the parameter is joined with `&`),
 * and the key is percent-encoded because it is an arbitrary virtual path that may
 * contain characters illegal in a URL. The key is never an HTTP URL: it is the
 * seam's own address for the file the shell should mount.
 */
function buildPageUrl(pageUrl: string, param: string, gameKey: string): string {
  const separator = pageUrl.includes('?') ? '&' : '?';
  return `${pageUrl}${separator}${param}=${encodeURIComponent(gameKey)}`;
}

/**
 * Creates a same-origin iframe for the KiriKiri WASM build and installs the host
 * byte source into it on every `load` event.
 *
 * See the module docs for why the install is repeated: the frame's window is
 * replaced when it navigates from `about:blank`, so an install done only before
 * `load` would be lost.
 */
export function createKiriPage(options: KiriPageOptions): KiriPageHandle {
  if (!options.pageUrl) {
    throw new Error('createKiriPage: `pageUrl` is required');
  }
  if (!options.gameKey) {
    throw new Error('createKiriPage: `gameKey` is required');
  }
  if (!options.source) {
    throw new Error('createKiriPage: `source` is required');
  }

  const param = options.param ?? 'xp3';
  const framedUrl = buildPageUrl(options.pageUrl, param, options.gameKey);

  // The seam resolves its URLs through this source, so the resolve override is
  // threaded straight through.
  const hostSource = hostByteSourceFromGameSource(options.source, {
    resolveUrl: options.resolveUrl,
  });

  const container: Element | null = options.container ?? options.document.body;
  if (!container) {
    throw new Error(
      'createKiriPage: no container to append the frame to ' +
        '(pass `container`, or provide a document with a <body>)',
    );
  }

  const frame = options.document.createElement('iframe');
  container.appendChild(frame);

  /**
   * Reads the frame's current window as a property record the seam can be
   * installed onto.
   *
   * `contentWindow` is `Window | null` and `Window` carries no string index
   * signature in the DOM types, so the cast through `unknown` mirrors the idiom
   * the seam itself uses for `globalThis`.
   */
  const frameWindow = (): Record<string, unknown> | null => {
    const win = frame.contentWindow;
    return win === null ? null : (win as unknown as Record<string, unknown>);
  };

  /**
   * The embedding page's window, which is what a frame running in a document of its own
   * reads through when it needs the source before its own `load` fires.
   *
   * Resolved from the document rather than `globalThis`: in the browser those are the
   * same window, while under jsdom `globalThis` is the Node process, where a stray seam
   * would leak across test files.
   */
  const ownerWindow = (): Record<string, unknown> | null => {
    const win = options.document.defaultView;
    return win === null ? null : (win as unknown as Record<string, unknown>);
  };

  /**
   * True once an install has run for the *loaded* page, i.e. after the frame
   * fired `load`. `ready()` gates on this rather than on the up-front install,
   * because that one only ever reaches the `about:blank` document — resolving on
   * it would report "ready" before the real page exists.
   */
  let installedAfterLoad = false;
  /** Installs the seam onto the frame's current window, if it has one yet. */
  const tryInstall = (): void => {
    const win = frameWindow();
    if (win === null) return;
    installHostByteSource(hostSource, win);
  };

  // Install on every load, and on load only. Setting `src` replaces the frame's
  // window as far as the DOM is concerned — an install done against the
  // `about:blank` document is discarded the moment navigation starts — so `load`
  // is the first moment an install can actually survive to be read.
  const onFrameLoad = (): void => {
    tryInstall();
    installedAfterLoad = true;
  };
  frame.addEventListener('load', onFrameLoad);

  // …and publish it on the embedding page as well, which is what the frame actually
  // reads through when it needs the source *during* its own script execution: the
  // shell decides its load mode and probes the archive before `load` fires, and on a
  // reload the frame's window is new, so a load-time-only install is always too late.
  // The frame's seam helpers prefer their own window and fall back to `window.parent`.
  const owner = ownerWindow();
  if (owner !== null) installHostByteSource(hostSource, owner);

  frame.src = framedUrl;

  // One promise is cached so repeated `ready()` calls share a single outcome.
  let readyPromise: Promise<void> | null = null;
  // The id of the currently pending poll tick, so dispose can cancel it and the
  // frame leaves no timer behind.
  let pendingPoll: ReturnType<typeof setTimeout> | null = null;
  // The pending `ready()` rejection, so `dispose()` can settle it instead of
  // leaving a caller awaiting a frame that no longer exists.
  let rejectReady: ((error: Error) => void) | null = null;

  const ready = (timeoutMs = 30_000): Promise<void> => {
    if (readyPromise !== null) return readyPromise;
    readyPromise = new Promise<void>((resolve, reject) => {
      rejectReady = reject;
      const deadline = Date.now() + timeoutMs;
      const tick = (): void => {
        if (installedAfterLoad) {
          pendingPoll = null;
          rejectReady = null;
          resolve();
          return;
        }
        if (Date.now() >= deadline) {
          pendingPoll = null;
          rejectReady = null;
          reject(
            new Error(
              `createKiriPage: the host byte source was not installed on "${framedUrl}" ` +
                `within ${timeoutMs} ms (seam key "${HOST_BYTE_SOURCE_KEY}")`,
            ),
          );
          return;
        }
        pendingPoll = setTimeout(tick, 25);
      };
      tick();
    });
    return readyPromise;
  };

  let disposed = false;
  const dispose = (): void => {
    if (disposed) return;
    disposed = true;
    if (pendingPoll !== null) {
      clearTimeout(pendingPoll);
      pendingPoll = null;
    }
    if (rejectReady !== null) {
      const reject = rejectReady;
      rejectReady = null;
      reject(
        new Error(
          `createKiriPage: disposed before the host byte source was installed on "${framedUrl}"`,
        ),
      );
    }
    frame.removeEventListener('load', onFrameLoad);
    frame.remove();
    // Both installs are cleared: the embedding page's copy is the one a frame would pick
    // up, so leaving it behind would hand a disposed source to the next frame.
    try {
      const targets = [frameWindow(), ownerWindow()];
      for (const win of targets) {
        // Best-effort: a window may already be detached or cross-origin in edge cases,
        // and disposal must not throw.
        if (win !== null) delete win[HOST_BYTE_SOURCE_KEY];
      }
    } catch {
      // Intentionally ignored: clearing the seam key is best-effort.
    }
  };

  return { frame, hostSource, ready, dispose };
}
