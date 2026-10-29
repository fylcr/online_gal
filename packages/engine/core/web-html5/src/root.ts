import type { GameSource } from '@online-gal/engine-api';

/** Content types for the extensions HTML5 game engines ship. */
const MIME_TYPES: Readonly<Record<string, string>> = {
  html: 'text/html; charset=utf-8',
  htm: 'text/html; charset=utf-8',
  js: 'text/javascript; charset=utf-8',
  mjs: 'text/javascript; charset=utf-8',
  cjs: 'text/javascript; charset=utf-8',
  json: 'application/json; charset=utf-8',
  css: 'text/css; charset=utf-8',
  svg: 'image/svg+xml',
  png: 'image/png',
  jpg: 'image/jpeg',
  jpeg: 'image/jpeg',
  gif: 'image/gif',
  webp: 'image/webp',
  avif: 'image/avif',
  bmp: 'image/bmp',
  ico: 'image/x-icon',
  mp3: 'audio/mpeg',
  ogg: 'audio/ogg',
  oga: 'audio/ogg',
  m4a: 'audio/mp4',
  wav: 'audio/wav',
  mp4: 'video/mp4',
  webm: 'video/webm',
  ttf: 'font/ttf',
  otf: 'font/otf',
  woff: 'font/woff',
  woff2: 'font/woff2',
  txt: 'text/plain; charset=utf-8',
  csv: 'text/csv; charset=utf-8',
  xml: 'application/xml',
  wasm: 'application/wasm',
  ttf2: 'application/octet-stream',
  rpgmvp: 'image/png',
  rpgmvo: 'audio/ogg',
  rpgmvm: 'audio/mpeg',
  png_: 'image/png',
  m4a_: 'audio/mp4',
  ogg_: 'audio/ogg',
  bin: 'application/octet-stream',
};

export function mimeForPath(path: string): string {
  const dot = path.lastIndexOf('.');
  if (dot === -1) return 'application/octet-stream';
  const extension = path.slice(dot + 1).toLowerCase();
  return MIME_TYPES[extension] ?? 'application/octet-stream';
}

export interface WebRootOptions {
  /** Where the game's entry document lives. Defaults to `index.html`. */
  readonly entry?: string;
  /**
   * Extra virtual paths injected into the file table before the game's own
   * files, used for compatibility shims the game expects to exist.
   */
  readonly shimScripts?: readonly { readonly path: string; readonly source: string }[];
  readonly onMissing?: (path: string) => void;
}

/** A resolved file ready to be handed to the iframe. */
interface WebFile {
  readonly body: Uint8Array;
  readonly type: string;
}

/**
 * Serves an imported game directory to an iframe.
 *
 * The browser equivalent of Tyranor-Next's built-in local HTTP server. Because
 * an imported directory lives in IndexedDB rather than on disk, there is no URL
 * the iframe could load; this class instead resolves virtual paths on demand
 * and hands the bytes to a bootstrap injected into the iframe, which redirects
 * element `src` attributes to blob URLs and patches `fetch`/`XMLHttpRequest`.
 */
export class WebRoot {
  private readonly files = new Map<string, WebFile>();
  private readonly blobUrls = new Map<string, string>();
  private readonly shimScripts: readonly { path: string; source: string }[];
  readonly entry: string;

  constructor(
    private readonly source: GameSource,
    options: WebRootOptions = {},
  ) {
    this.entry = options.entry ?? 'index.html';
    this.shimScripts = options.shimScripts ?? [];
    void options.onMissing;
  }

  /** Resolves a virtual path (no leading slash) to bytes and a content type. */
  async resolve(rawPath: string): Promise<WebFile | null> {
    let path = rawPath.split('?')[0]!.split('#')[0]!;
    path = decodeURIComponent(path).replace(/\\/g, '/').replace(/^\.?\//, '');
    if (path === '' || path.endsWith('/')) path += this.entry;

    const shim = this.shimScripts.find((candidate) => candidate.path === path);
    if (shim) {
      return {
        body: new TextEncoder().encode(shim.source),
        type: mimeForPath(path),
      };
    }

    const cached = this.files.get(path);
    if (cached) return cached;

    try {
      const exists = await this.source.exists(path);
      if (!exists) return null;
      if (await this.source.isDirectory(path)) {
        return this.resolve(`${path}/${this.entry}`);
      }
      const body = await this.source.readFile(path);
      const file: WebFile = { body, type: mimeForPath(path) };
      this.files.set(path, file);
      return file;
    } catch {
      return null;
    }
  }

  /** Returns a reusable blob URL for a path, creating it on first use. */
  async blobUrlFor(path: string): Promise<string | null> {
    const existing = this.blobUrls.get(path);
    if (existing) return existing;
    const file = await this.resolve(path);
    if (!file) return null;
    const blob = new Blob([file.body as BlobPart], { type: file.type });
    const url = URL.createObjectURL(blob);
    this.blobUrls.set(path, url);
    return url;
  }

  /** Reads the entry document, rewiring relative references to blob URLs. */
  async renderEntry(): Promise<string> {
    const file = await this.resolve(this.entry);
    if (!file) {
      throw new Error(`the game has no "${this.entry}" to start`);
    }
    let html = new TextDecoder('utf-8', { fatal: false }).decode(file.body);
    html = await this.inlineLocalScripts(html);
    html = await this.injectBootstrap(html);
    return html;
  }

  /**
   * Inlines `<script src="...">` from the game itself.
   *
   * A blob-backed page cannot issue classic subresource requests, so the game's
   * own scripts are inlined in document order. Deferred and module scripts are
   * preserved as inline scripts with the same execution timing (`defer` runs
   * after parsing, which inline scripts cannot express, so those are emitted in
   * place with a `DOMContentLoaded` guard).
   */
  private async inlineLocalScripts(html: string): Promise<string> {
    const pattern = /<script\b([^>]*?)\bsrc\s*=\s*["']([^"']+)["']([^>]*)>\s*<\/script>/gi;
    const matches = [...html.matchAll(pattern)];
    let output = html;

    for (const match of matches) {
      const full = match[0];
      const attributes = `${match[1] ?? ''} ${match[3] ?? ''}`;
      const src = match[2]!;
      if (/^(https?:)?\/\//i.test(src) || src.startsWith('data:')) continue;

      const file = await this.resolve(src);
      if (!file) continue;
      const code = new TextDecoder('utf-8', { fatal: false }).decode(file.body);
      const isModule = /\btype\s*=\s*["']module["']/i.test(attributes);
      const escaped = code.replace(/<\/script>/gi, '<\\/script>');
      const replacement = isModule
        ? `<script type="module">\n${escaped}\n</script>`
        : `<script>\n${escaped}\n</script>`;
      output = output.replace(full, replacement);
    }
    return output;
  }

  /** Injects the compatibility bootstrap as the very first script. */
  private async injectBootstrap(html: string): Promise<string> {
    const bootstrap = this.bootstrapSource();
    const tag = `<script>${bootstrap}</script>`;
    if (/<head[^>]*>/i.test(html)) {
      return html.replace(/<head([^>]*)>/i, `<head$1>${tag}`);
    }
    if (/<html[^>]*>/i.test(html)) {
      return html.replace(/<html([^>]*)>/i, `<html$1>${tag}`);
    }
    return `${tag}${html}`;
  }

  /**
   * The bootstrap that runs inside the iframe.
   *
   * It cannot import anything, so it is emitted as a self-contained script. It
   * requests files from the host over `postMessage` and rewrites loads.
   */
  private bootstrapSource(): string {
    return `(function () {
  'use strict';
  var CHANNEL = '__ogfs__';
  var pending = new Map();
  var seq = 0;
  var objectUrls = new Map();

  function request(path) {
    return new Promise(function (resolve) {
      var id = ++seq;
      pending.set(id, resolve);
      parent.postMessage({ channel: CHANNEL, kind: 'read', id: id, path: path }, '*');
    });
  }

  window.addEventListener('message', function (event) {
    var data = event.data;
    if (!data || data.channel !== CHANNEL || data.kind !== 'data') return;
    var resolve = pending.get(data.id);
    if (!resolve) return;
    pending.delete(data.id);
    resolve(data.body ? new Uint8Array(data.body) : null);
  });

  function toBlobUrl(path, bytes, type) {
    if (objectUrls.has(path)) return objectUrls.get(path);
    var url = URL.createObjectURL(new Blob([bytes], { type: type || 'application/octet-stream' }));
    objectUrls.set(path, url);
    return url;
  }

  var MIME = ${JSON.stringify(MIME_TYPES)};
  function mimeFor(path) {
    var dot = path.lastIndexOf('.');
    if (dot === -1) return 'application/octet-stream';
    return MIME[path.slice(dot + 1).toLowerCase()] || 'application/octet-stream';
  }

  // --- rewrite element loads -------------------------------------------------
  function rewrite(el) {
    var raw = el.getAttribute('data-ogfs-src');
    if (!raw) return;
    request(raw).then(function (bytes) {
      if (!bytes) return;
      el.src = toBlobUrl(raw, bytes, mimeFor(raw));
    });
  }

  var ATTRS = ['src', 'href', 'data'];
  var originalSetAttribute = Element.prototype.setAttribute;
  Element.prototype.setAttribute = function (name, value) {
    if (ATTRS.indexOf(name) !== -1 && typeof value === 'string' && !/^(blob:|data:|https?:|\\/\\/|#)/.test(value)) {
      originalSetAttribute.call(this, 'data-ogfs-src', value);
      var self = this;
      request(value).then(function (bytes) {
        if (!bytes) { originalSetAttribute.call(self, name, value); return; }
        originalSetAttribute.call(self, name, toBlobUrl(value, bytes, mimeFor(value)));
      });
      return;
    }
    originalSetAttribute.call(this, name, value);
  };

  // --- patch fetch ----------------------------------------------------------
  var nativeFetch = window.fetch ? window.fetch.bind(window) : null;
  window.fetch = function (input, init) {
    var url = typeof input === 'string' ? input : (input && input.url) || '';
    if (url && !/^(blob:|data:|https?:|\\/\\/)/.test(url)) {
      return request(url).then(function (bytes) {
        if (!bytes) return nativeFetch ? nativeFetch(input, init) : Promise.reject(new Error('missing ' + url));
        return new Response(bytes, { status: 200, headers: { 'Content-Type': mimeFor(url) } });
      });
    }
    return nativeFetch ? nativeFetch(input, init) : Promise.reject(new Error('fetch unavailable'));
  };

  // --- patch XMLHttpRequest -------------------------------------------------
  var NativeXhr = window.XMLHttpRequest;
  var nativeOpen = NativeXhr.prototype.open;
  var nativeSend = NativeXhr.prototype.send;
  NativeXhr.prototype.open = function (method, url) {
    this.__ogfsUrl = url;
    this.__ogfsRest = Array.prototype.slice.call(arguments, 2);
    if (!url || /^(blob:|data:|https?:|\\/\\/)/.test(url)) {
      nativeOpen.apply(this, arguments);
      return;
    }
  };
  NativeXhr.prototype.send = function (body) {
    var self = this;
    if (!this.__ogfsUrl || /^(blob:|data:|https?:|\\/\\/)/.test(this.__ogfsUrl)) {
      return nativeSend.call(this, body);
    }
    request(this.__ogfsUrl).then(function (bytes) {
      if (bytes === null) {
        nativeOpen.apply(self, [ 'GET', self.__ogfsUrl ].concat(self.__ogfsRest || []));
        return nativeSend.call(self, body);
      }
      var url = toBlobUrl(self.__ogfsUrl, bytes, mimeFor(self.__ogfsUrl));
      nativeOpen.apply(self, [ 'GET', url ].concat(self.__ogfsRest || []));
      nativeSend.call(self, body);
    });
  };

  // --- initial pass once the DOM is parsed ----------------------------------
  function sweep() {
    var nodes = document.querySelectorAll('[data-ogfs-src]');
    for (var i = 0; i < nodes.length; i += 1) rewrite(nodes[i]);
  }

  document.addEventListener('DOMContentLoaded', function () {
    var nodes = document.querySelectorAll('script[src],link[href],img[src],audio[src],video[src],source[src]');
    for (var i = 0; i < nodes.length; i += 1) {
      var el = nodes[i];
      var attribute = el.tagName === 'LINK' ? 'href' : 'src';
      var value = el.getAttribute(attribute);
      if (!value || /^(blob:|data:|https?:|\\/\\/|#)/.test(value)) continue;
      originalSetAttribute.call(el, 'data-ogfs-src', value);
    }
    sweep();
  });

  window.__OGFS__ = { request: request, toBlobUrl: toBlobUrl, mimeFor: mimeFor };

  // --- NW.js / Electron compatibility shims ---------------------------------
  if (!window.nw && !window.require) {
    window.nw = undefined;
  }
  if (!window.process) {
    window.process = { platform: 'browser', versions: { node: '0.0.0' }, env: {} };
  }
  if (!window.__dirname) window.__dirname = '';
})();
`;
  }

  /** Releases blob URLs held for the game files. */
  dispose(): void {
    for (const url of this.blobUrls.values()) URL.revokeObjectURL(url);
    this.blobUrls.clear();
    this.files.clear();
  }
}

export { MIME_TYPES };
