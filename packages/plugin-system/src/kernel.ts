import type { PluginManifest } from '@online-gal/plugin-host';

import { KernelError } from './errors.js';
import { PACKAGE_FILE, contentTypeOf, encodePath, parsePackage, safeRelativePath, type PluginPackage } from './format.js';
import {
  OFFICIAL_ID_PREFIX,
  formatSource,
  githubBase,
  parseSource,
  type PluginSource,
} from './sources.js';

/**
 * The plugin system kernel: the part of the Service Worker that owns installed
 * plugins and answers the page's requests.
 *
 * Once the Service Worker controls the origin, every same-origin request goes
 * through `route` and `respond`:
 *
 *   浏览器 → 插件系统 → 当前管理器的文件（本地） → 插件系统 → 浏览器
 *
 * The server is only consulted when a plugin is installed or updated (the
 * official registry, or a GitHub tag). Which manager answers is local state, so
 * switching to a third-party manager changes what the page is without the
 * server knowing.
 *
 * Everything the kernel touches comes in through `KernelOptions`, so the same
 * code runs in the Service Worker and in Node tests.
 */
export interface KernelOptions {
  /** Origin the kernel serves, e.g. `https://online-gal.pages.dev`. */
  readonly origin: string;
  readonly caches: CacheStorage;
  readonly fetch: (input: string, init?: RequestInit) => Promise<Response>;
  /** Official plugin installed and activated when no manager is usable. */
  readonly defaultManager: string;
  readonly kernelVersion: string;
  /** Base of raw GitHub content; overridable for tests. */
  readonly githubRaw?: string;
  readonly now?: () => number;
  /** Minimum time between automatic official update checks. */
  readonly updateIntervalMs?: number;
  /** Parallel file downloads per install. */
  readonly concurrency?: number;
  /** Whether any page is open; previous generations are kept while one is. */
  readonly pagesOpen?: () => Promise<boolean>;
  /** Tells open pages about changes (installed, updated, removed). */
  readonly notify?: (message: KernelNotice) => void;
  readonly log?: (message: string, error?: unknown) => void;
}

export type KernelNotice =
  | { readonly type: 'plugin-installed'; readonly id: string; readonly version: string; readonly updated: boolean }
  | { readonly type: 'plugin-removed'; readonly id: string }
  | { readonly type: 'manager-changed'; readonly id: string };

export const DEFAULT_GITHUB_RAW = 'https://raw.githubusercontent.com/';
export const DEFAULT_UPDATE_INTERVAL_MS = 6 * 60 * 60 * 1000;
export const STATE_FORMAT = 1;
export const REGISTRY_FORMAT = 1;

const STATE_CACHE = 'og-kernel';
const PLUGIN_CACHE_PREFIX = 'og-plugin:';
/** Unreferenced plugin caches younger than this may belong to an install in flight. */
const ORPHAN_GRACE_MS = 60 * 60 * 1000;

/** Headers on everything the kernel serves: the engines need cross-origin isolation. */
const ISOLATION_HEADERS: ReadonlyArray<readonly [string, string]> = [
  ['Cross-Origin-Opener-Policy', 'same-origin'],
  ['Cross-Origin-Embedder-Policy', 'credentialless'],
  ['X-Content-Type-Options', 'nosniff'],
];

/** A file left out of the install download; fetched, verified and stored when first requested. */
interface LazyFile {
  readonly path: string;
  readonly size: number;
  readonly sha256: string;
}

interface Generation {
  readonly cache: string;
  /** Every file of the plugin, eager and lazy alike (path lookups use this list). */
  readonly files: readonly string[];
  /** The files in `files` that are fetched on demand; absent when everything was downloaded at install. */
  readonly lazy?: readonly LazyFile[];
  /** Where lazy files are fetched from, ending in `/`; set whenever `lazy` is. */
  readonly base?: string;
}

/** One installed plugin, as persisted. */
export interface InstalledPlugin extends Generation {
  readonly manifest: PluginManifest;
  /** `official:<id>` or `github:<owner>/<repo>@<tag>`. */
  readonly source: string;
  /** SHA-256 of the `plugin.json` bytes that were installed. */
  readonly digest: string;
  readonly bytes: number;
  readonly installedAt: number;
  /**
   * The generation this one replaced, kept while pages are open so a page
   * loaded before an update can still fetch the files it references.
   */
  readonly previous?: Generation;
}

interface KernelState {
  readonly format: typeof STATE_FORMAT;
  readonly activeManager: string | null;
  readonly lastUpdateCheck: number;
  readonly plugins: Readonly<Record<string, InstalledPlugin>>;
}

const EMPTY_STATE: KernelState = Object.freeze({
  format: STATE_FORMAT,
  activeManager: null,
  lastUpdateCheck: 0,
  plugins: Object.freeze({}),
});

/** What the official registry (`/registry/index.json`) says about a plugin. */
export interface RegistryEntry {
  readonly version: string;
  readonly digest: string;
  /** Directory under `/registry/`, ending in `/`, holding `plugin.json` and the files. */
  readonly path: string;
  readonly name?: string;
  readonly description?: string;
  readonly role?: string;
}

export interface Registry {
  readonly format: typeof REGISTRY_FORMAT;
  readonly plugins: Readonly<Record<string, RegistryEntry>>;
}

/** Public view of an installed plugin (the `state` API). */
export interface PluginInfo {
  readonly id: string;
  readonly name: string;
  readonly version: string;
  readonly description?: string;
  readonly provenance: PluginManifest['provenance'];
  readonly role?: PluginManifest['role'];
  readonly source: string;
  readonly bytes: number;
  readonly files: number;
  readonly installedAt: number;
  readonly active: boolean;
}

export interface KernelStatus {
  readonly kernelVersion: string;
  readonly defaultManager: string;
  readonly activeManager: string | null;
  readonly lastUpdateCheck: number;
  readonly plugins: readonly PluginInfo[];
}

export interface UpdateReport {
  readonly updated: readonly string[];
  readonly failed: ReadonlyArray<{ readonly id: string; readonly code: string; readonly message: string }>;
}

/**
 * The parts of a fetch event's request the kernel reads. A real `Request`
 * satisfies it; tests pass plain objects because `new Request()` refuses
 * `mode: 'navigate'`.
 */
export type KernelRequest = Pick<Request, 'url' | 'method' | 'mode' | 'headers' | 'json'>;

export type Route =
  | { readonly kind: 'manager' }
  | { readonly kind: 'plugin'; readonly id: string; readonly path: string }
  | { readonly kind: 'kernel-page' }
  | { readonly kind: 'api'; readonly name: string }
  | { readonly kind: 'not-found' };

/**
 * Decides synchronously whether the kernel answers a request (a Service Worker
 * must call `respondWith` before its fetch handler returns). `null` means the
 * request goes to the network untouched.
 */
export function routeRequest(url: URL, method: string, origin: string): Route | null {
  if (url.origin !== origin) return null;
  const path = url.pathname;
  // The kernel's own script and the registry must always reach the server:
  // that is how the kernel and the plugins get updated.
  if (path === '/sw.js' || path.startsWith('/registry/')) return null;
  if (path.startsWith('/__kernel/api/')) return { kind: 'api', name: path.slice('/__kernel/api/'.length) };
  if (method !== 'GET' && method !== 'HEAD') return null;
  if (path === '/__kernel' || path === '/__kernel/') return { kind: 'kernel-page' };
  if (path.startsWith('/__kernel/')) return { kind: 'not-found' };
  if (path === '/__plugins' || path.startsWith('/__plugins/')) {
    const rest = path.slice('/__plugins/'.length);
    const slash = rest.indexOf('/');
    const id = slash < 0 ? rest : rest.slice(0, slash);
    return { kind: 'plugin', id, path: slash < 0 ? '' : rest.slice(slash + 1) };
  }
  return { kind: 'manager' };
}

type Resolution =
  | { readonly file: string }
  | { readonly redirect: string }
  | null;

/**
 * Maps a URL path (still percent-encoded, relative to the plugin root) to one
 * of the plugin's files, the way a static host does: `a/` → `a/index.html`,
 * `a` → redirect to `a/` when only `a/index.html` exists, `a` → `a.html`.
 * Lookups only ever hit the plugin's validated file list, so no URL can name a
 * file outside it.
 */
export function resolvePath(files: ReadonlySet<string>, encoded: string): Resolution | 'bad-path' {
  let rel: string;
  try {
    rel = decodeURIComponent(encoded);
  } catch {
    return 'bad-path';
  }
  if (rel === '' || rel.endsWith('/')) {
    const index = `${rel}index.html`;
    return files.has(index) ? { file: index } : null;
  }
  if (files.has(rel)) return { file: rel };
  if (files.has(`${rel}/index.html`)) return { redirect: `${rel}/` };
  if (files.has(`${rel}.html`)) return { file: `${rel}.html` };
  return null;
}

function fileKey(origin: string, path: string): string {
  return `${origin}/__kernel/file/${encodePath(path)}`;
}

function hex(buffer: ArrayBuffer): string {
  let out = '';
  for (const byte of new Uint8Array(buffer)) out += byte.toString(16).padStart(2, '0');
  return out;
}

async function sha256(bytes: ArrayBuffer): Promise<string> {
  return hex(await crypto.subtle.digest('SHA-256', bytes));
}

function isRecord(value: unknown): value is Record<string, unknown> {
  return typeof value === 'object' && value !== null && !Array.isArray(value);
}

function errorCode(error: unknown): string {
  if (isRecord(error) && typeof error.code === 'string') return error.code;
  return 'internal';
}

function errorMessage(error: unknown): string {
  return error instanceof Error ? error.message : String(error);
}

const STATUS_BY_CODE: Readonly<Record<string, number>> = {
  'request-invalid': 400,
  'source-invalid': 400,
  'manifest-invalid': 422,
  'api-version-mismatch': 422,
  'package-invalid': 422,
  'registry-invalid': 502,
  'provenance-mismatch': 422,
  'reserved-id': 422,
  'source-conflict': 409,
  'not-a-manager': 422,
  'too-large': 413,
  'not-in-registry': 404,
  'not-installed': 404,
  'manager-in-use': 409,
  'fetch-failed': 502,
  'integrity-failed': 502,
  'storage-missing': 503,
};

function json(status: number, body: unknown): Response {
  const headers = new Headers({ 'content-type': 'application/json', 'cache-control': 'no-store' });
  for (const [name, value] of ISOLATION_HEADERS) headers.set(name, value);
  return new Response(JSON.stringify(body), { status, headers });
}

function errorResponse(error: unknown): Response {
  const code = errorCode(error);
  return json(STATUS_BY_CODE[code] ?? 500, { ok: false, error: { code, message: errorMessage(error) } });
}

function html(status: number, body: string): Response {
  const headers = new Headers({ 'content-type': 'text/html; charset=utf-8', 'cache-control': 'no-store' });
  for (const [name, value] of ISOLATION_HEADERS) headers.set(name, value);
  return new Response(body, { status, headers });
}

function text(status: number, body: string): Response {
  const headers = new Headers({ 'content-type': 'text/plain; charset=utf-8', 'cache-control': 'no-store' });
  for (const [name, value] of ISOLATION_HEADERS) headers.set(name, value);
  return new Response(body, { status, headers });
}

/** Parses a single `bytes=` range; `null` = serve everything, `'unsatisfiable'` = 416. */
function parseRange(header: string | null, size: number): { start: number; end: number } | null | 'unsatisfiable' {
  if (header === null) return null;
  const match = /^bytes=(\d*)-(\d*)$/.exec(header.trim());
  // Multiple ranges or other units: a full 200 response is a valid answer.
  if (match === null) return null;
  const [, rawStart = '', rawEnd = ''] = match;
  if (rawStart === '' && rawEnd === '') return 'unsatisfiable';
  if (rawStart === '') {
    const suffix = Number(rawEnd);
    if (suffix === 0 || size === 0) return 'unsatisfiable';
    return { start: Math.max(0, size - suffix), end: size - 1 };
  }
  const start = Number(rawStart);
  const end = rawEnd === '' ? size - 1 : Math.min(Number(rawEnd), size - 1);
  if (start >= size || end < start) return 'unsatisfiable';
  return { start, end };
}

export class Kernel {
  private readonly origin: string;
  private readonly githubRaw: string;
  private readonly now: () => number;
  private readonly updateInterval: number;
  private readonly concurrency: number;

  private loaded: Promise<KernelState> | null = null;
  private current: KernelState = EMPTY_STATE;
  private queue: Promise<unknown> = Promise.resolve();
  private readonly fileSets = new WeakMap<Generation, ReadonlySet<string>>();
  private readonly lazyMaps = new WeakMap<Generation, ReadonlyMap<string, LazyFile>>();
  /** Lazy downloads in flight, so concurrent requests for one file share one fetch. */
  private readonly lazyInflight = new Map<string, Promise<void>>();
  private readonly openCaches = new Map<string, Promise<Cache>>();
  private autoUpdate: Promise<void> | null = null;

  constructor(private readonly options: KernelOptions) {
    this.origin = options.origin.replace(/\/$/, '');
    this.githubRaw = options.githubRaw ?? DEFAULT_GITHUB_RAW;
    this.now = options.now ?? Date.now;
    this.updateInterval = options.updateIntervalMs ?? DEFAULT_UPDATE_INTERVAL_MS;
    this.concurrency = Math.max(1, options.concurrency ?? 6);
  }

  // ---------------------------------------------------------------- state

  private async state(): Promise<KernelState> {
    this.loaded ??= this.load();
    return this.loaded;
  }

  private stateKey(): string {
    return `${this.origin}/__kernel/state.json`;
  }

  private async load(): Promise<KernelState> {
    let state = EMPTY_STATE;
    try {
      const cache = await this.options.caches.open(STATE_CACHE);
      const stored = await cache.match(this.stateKey());
      if (stored !== undefined) {
        const value: unknown = await stored.json();
        if (isRecord(value) && value.format === STATE_FORMAT && isRecord(value.plugins)) {
          state = value as unknown as KernelState;
        } else {
          this.options.log?.('kernel state has an unknown format; starting empty');
        }
      }
    } catch (error) {
      this.options.log?.('kernel state could not be read; starting empty', error);
    }
    this.current = state;
    // Not awaited: `load` can run inside a queued mutation, and cleanup queues behind it.
    void this.collectGarbage().catch((error: unknown) => this.options.log?.('cache cleanup failed', error));
    return state;
  }

  /** Persists first, then swaps: a failed write leaves the old state in force. */
  private async commit(next: KernelState): Promise<void> {
    const cache = await this.options.caches.open(STATE_CACHE);
    await cache.put(
      this.stateKey(),
      new Response(JSON.stringify(next), { headers: { 'content-type': 'application/json' } }),
    );
    this.current = next;
    this.loaded = Promise.resolve(next);
  }

  /** Mutations run one at a time; reads see the last committed state. */
  private exclusive<T>(task: () => Promise<T>): Promise<T> {
    const run = this.queue.then(task, task);
    this.queue = run.catch(() => undefined);
    return run;
  }

  // ---------------------------------------------------------------- status

  async status(): Promise<KernelStatus> {
    const state = await this.state();
    return {
      kernelVersion: this.options.kernelVersion,
      defaultManager: this.options.defaultManager,
      activeManager: state.activeManager,
      lastUpdateCheck: state.lastUpdateCheck,
      plugins: Object.values(state.plugins)
        .map((plugin) => this.info(plugin, state))
        .sort((a, b) => a.id.localeCompare(b.id)),
    };
  }

  private info(plugin: InstalledPlugin, state: KernelState): PluginInfo {
    const { manifest } = plugin;
    return {
      id: manifest.id,
      name: manifest.name,
      version: manifest.version,
      ...(manifest.description === undefined ? {} : { description: manifest.description }),
      provenance: manifest.provenance,
      ...(manifest.role === undefined ? {} : { role: manifest.role }),
      source: plugin.source,
      bytes: plugin.bytes,
      files: plugin.files.length,
      installedAt: plugin.installedAt,
      active: state.activeManager === manifest.id,
    };
  }

  // ---------------------------------------------------------------- registry

  async registry(): Promise<Registry> {
    const url = `${this.origin}/registry/index.json`;
    const bytes = await this.fetchBytes(url, { cache: 'no-store' });
    let value: unknown;
    try {
      value = JSON.parse(new TextDecoder().decode(bytes));
    } catch {
      throw new KernelError('registry-invalid', 'the official registry is not valid JSON');
    }
    if (!isRecord(value) || value.format !== REGISTRY_FORMAT || !isRecord(value.plugins)) {
      throw new KernelError('registry-invalid', `the official registry is not format ${REGISTRY_FORMAT}`);
    }
    for (const [id, entry] of Object.entries(value.plugins)) {
      if (
        !isRecord(entry) ||
        typeof entry.version !== 'string' ||
        typeof entry.digest !== 'string' ||
        !/^[0-9a-f]{64}$/.test(entry.digest) ||
        typeof entry.path !== 'string' ||
        !entry.path.endsWith('/') ||
        safeRelativePath(entry.path.slice(0, -1)) === null
      ) {
        throw new KernelError('registry-invalid', `the official registry entry for "${id}" is malformed`);
      }
    }
    return value as unknown as Registry;
  }

  // ---------------------------------------------------------------- install

  /**
   * Installs (or updates to) what `ref` points at. The previous version keeps
   * serving until the new one is fully downloaded and verified; a failure at
   * any point leaves the installed state exactly as it was.
   */
  install(ref: string, options: { readonly activate?: boolean } = {}): Promise<PluginInfo> {
    const source = parseSource(ref);
    return this.exclusive(async () => {
      const plugin = await this.installLocked(source, options.activate === true, null);
      return this.info(plugin, await this.state());
    });
  }

  private async installLocked(
    source: PluginSource,
    activate: boolean,
    knownRegistry: Registry | null,
  ): Promise<InstalledPlugin> {
    let base: string;
    let expected: RegistryEntry | null = null;
    if (source.kind === 'official') {
      const registry = knownRegistry ?? (await this.registry());
      const entry = registry.plugins[source.id];
      if (entry === undefined) {
        throw new KernelError('not-in-registry', `the official registry has no plugin "${source.id}"`, source.id);
      }
      expected = entry;
      base = `${this.origin}/registry/${entry.path}`;
    } else {
      base = githubBase(source, this.githubRaw);
    }

    const packageBytes = await this.fetchBytes(`${base}${PACKAGE_FILE}`);
    const digest = await sha256(packageBytes);
    if (expected !== null && digest !== expected.digest) {
      throw new KernelError('integrity-failed', `${PACKAGE_FILE} does not match the registry digest`, source.kind === 'official' ? source.id : null);
    }
    let raw: unknown;
    try {
      raw = JSON.parse(new TextDecoder().decode(packageBytes));
    } catch {
      throw new KernelError('package-invalid', `${PACKAGE_FILE} is not valid JSON`);
    }
    const pkg = parsePackage(raw);
    const { manifest } = pkg;
    this.checkProvenance(source, pkg, expected);
    if (activate && manifest.role !== 'manager') {
      throw new KernelError('not-a-manager', `"${manifest.id}" is not a manager`, manifest.id);
    }

    const state = await this.state();
    const existing = state.plugins[manifest.id];
    if (existing !== undefined && !sameOrigin(parseSource(existing.source), source)) {
      throw new KernelError(
        'source-conflict',
        `"${manifest.id}" is already installed from ${existing.source}; uninstall it before installing it from ${formatSource(source)}`,
        manifest.id,
      );
    }

    if (existing !== undefined && existing.digest === digest && (await this.intact(existing))) {
      // Same package already in place: nothing to download.
      const unchanged: InstalledPlugin = { ...existing, source: formatSource(source) };
      const next: KernelState = {
        ...state,
        activeManager: activate ? manifest.id : state.activeManager,
        plugins: { ...state.plugins, [manifest.id]: unchanged },
      };
      if (next.activeManager !== state.activeManager || unchanged.source !== existing.source) {
        await this.commit(next);
        if (next.activeManager !== state.activeManager) this.options.notify?.({ type: 'manager-changed', id: manifest.id });
      }
      return unchanged;
    }

    const cache = `${PLUGIN_CACHE_PREFIX}${manifest.id}:${this.now().toString(36)}-${Math.random().toString(36).slice(2, 8)}`;
    let bytes: number;
    try {
      bytes = await this.download(base, pkg, cache);
    } catch (error) {
      await this.dropCache(cache);
      throw error;
    }

    const lazy = lazyOf(pkg);
    const record: InstalledPlugin = {
      manifest,
      source: formatSource(source),
      digest,
      bytes,
      installedAt: this.now(),
      cache,
      files: pkg.files.map((file) => file.path),
      ...(lazy === null ? {} : { lazy, base }),
      ...(existing === undefined ? {} : { previous: generationOf(existing) }),
    };
    const next: KernelState = {
      ...state,
      activeManager: activate ? manifest.id : state.activeManager,
      plugins: { ...state.plugins, [manifest.id]: record },
    };
    try {
      await this.commit(next);
    } catch (error) {
      await this.dropCache(cache);
      throw error;
    }
    // Only one previous generation is kept; the one before it goes now.
    if (existing?.previous !== undefined) await this.dropCache(existing.previous.cache);

    this.options.notify?.({ type: 'plugin-installed', id: manifest.id, version: manifest.version, updated: existing !== undefined });
    if (activate && state.activeManager !== manifest.id) this.options.notify?.({ type: 'manager-changed', id: manifest.id });
    void this.collectGarbage().catch((error: unknown) => this.options.log?.('cache cleanup failed', error));
    return record;
  }

  private checkProvenance(source: PluginSource, pkg: PluginPackage, entry: RegistryEntry | null): void {
    const { manifest } = pkg;
    if (source.kind === 'official') {
      if (manifest.provenance !== 'official') {
        throw new KernelError('provenance-mismatch', `the official registry served "${manifest.id}" with provenance "${manifest.provenance}"`, manifest.id);
      }
      if (manifest.id !== source.id) {
        throw new KernelError('provenance-mismatch', `asked the registry for "${source.id}" but got "${manifest.id}"`, manifest.id);
      }
      if (entry !== null && manifest.version !== entry.version) {
        throw new KernelError('provenance-mismatch', `the registry lists "${manifest.id}" ${entry.version} but the package is ${manifest.version}`, manifest.id);
      }
      return;
    }
    if (manifest.provenance !== 'third-party') {
      throw new KernelError('provenance-mismatch', `a plugin from GitHub must declare provenance "third-party" (got "${manifest.provenance}")`, manifest.id);
    }
    if (manifest.id.startsWith(OFFICIAL_ID_PREFIX)) {
      throw new KernelError('reserved-id', `plugin ids starting with "${OFFICIAL_ID_PREFIX}" are reserved for official plugins`, manifest.id);
    }
  }

  /** Downloads and verifies every eager file into a fresh cache and returns the byte total; lazy files wait for their first request. */
  private async download(base: string, pkg: PluginPackage, cacheName: string): Promise<number> {
    const cache = await this.options.caches.open(cacheName);
    const files = pkg.files.filter((file) => file.lazy !== true);
    const controller = new AbortController();
    let next = 0;
    let total = 0;
    const worker = async (): Promise<void> => {
      while (!controller.signal.aborted) {
        const file = files[next++];
        if (file === undefined) return;
        await this.fetchInto(cache, base, file, pkg.manifest.id, controller.signal);
        total += file.size;
      }
    };
    const workers = Array.from({ length: Math.min(this.concurrency, files.length) }, () =>
      worker().catch((error: unknown) => {
        controller.abort();
        throw error;
      }),
    );
    const results = await Promise.allSettled(workers);
    const failure = results.find((result): result is PromiseRejectedResult => result.status === 'rejected');
    if (failure !== undefined) throw failure.reason;
    return total;
  }

  /** Fetches one file, checks its size and SHA-256, and only then stores it. */
  private async fetchInto(
    cache: Cache,
    base: string,
    file: { readonly path: string; readonly size: number; readonly sha256: string },
    owner: string | null,
    signal?: AbortSignal,
  ): Promise<void> {
    const bytes = await this.fetchBytes(`${base}${encodePath(file.path)}`, signal === undefined ? {} : { signal });
    if (bytes.byteLength !== file.size || (await sha256(bytes)) !== file.sha256) {
      throw new KernelError('integrity-failed', `file "${file.path}" does not match its size or sha256`, owner);
    }
    await cache.put(
      fileKey(this.origin, file.path),
      new Response(bytes, {
        headers: { 'content-type': contentTypeOf(file.path), 'content-length': String(file.size) },
      }),
    );
  }

  private async fetchBytes(url: string, init: RequestInit = {}): Promise<ArrayBuffer> {
    let response: Response;
    try {
      response = await this.options.fetch(url, init);
    } catch (error) {
      if (init.signal?.aborted) throw error;
      throw new KernelError('fetch-failed', `could not fetch ${url}: ${errorMessage(error)}`);
    }
    if (!response.ok) {
      throw new KernelError('fetch-failed', `fetching ${url} returned HTTP ${response.status}`);
    }
    return response.arrayBuffer();
  }

  /** Whether a generation's cache still has its first and last eager file (lazy files may not be fetched yet). */
  private async intact(generation: Generation): Promise<boolean> {
    const cache = await this.cache(generation.cache);
    const lazy = this.lazyFiles(generation);
    const eager = generation.files.filter((path) => !lazy.has(path));
    const first = eager[0];
    const last = eager[eager.length - 1];
    if (first === undefined || last === undefined) return false;
    return (await cache.match(fileKey(this.origin, first))) !== undefined &&
      (await cache.match(fileKey(this.origin, last))) !== undefined;
  }

  // ---------------------------------------------------------------- other mutations

  uninstall(id: string): Promise<void> {
    return this.exclusive(async () => {
      const state = await this.state();
      const plugin = state.plugins[id];
      if (plugin === undefined) throw new KernelError('not-installed', `"${id}" is not installed`, id);
      if (state.activeManager === id) {
        throw new KernelError('manager-in-use', `"${id}" is the active manager; activate another manager first`, id);
      }
      const plugins = { ...state.plugins };
      delete plugins[id];
      await this.commit({ ...state, plugins });
      await this.dropCache(plugin.cache);
      if (plugin.previous !== undefined) await this.dropCache(plugin.previous.cache);
      this.options.notify?.({ type: 'plugin-removed', id });
    });
  }

  activate(id: string): Promise<void> {
    return this.exclusive(async () => {
      const state = await this.state();
      const plugin = state.plugins[id];
      if (plugin === undefined) throw new KernelError('not-installed', `"${id}" is not installed`, id);
      if (plugin.manifest.role !== 'manager') {
        throw new KernelError('not-a-manager', `"${id}" is not a manager`, id);
      }
      if (state.activeManager === id) return;
      await this.commit({ ...state, activeManager: id });
      this.options.notify?.({ type: 'manager-changed', id });
    });
  }

  /** Installs newer versions of installed official plugins. Third-party plugins only change when asked. */
  checkUpdates(): Promise<UpdateReport> {
    return this.exclusive(async () => {
      const registry = await this.registry();
      const updated: string[] = [];
      const failed: Array<{ id: string; code: string; message: string }> = [];
      const state = await this.state();
      for (const plugin of Object.values(state.plugins)) {
        const source = parseSource(plugin.source);
        if (source.kind !== 'official') continue;
        const entry = registry.plugins[source.id];
        if (entry === undefined || entry.digest === plugin.digest) continue;
        try {
          await this.installLocked(source, false, registry);
          updated.push(source.id);
        } catch (error) {
          failed.push({ id: source.id, code: errorCode(error), message: errorMessage(error) });
        }
      }
      await this.commit({ ...(await this.state()), lastUpdateCheck: this.now() });
      return { updated, failed };
    });
  }

  /** Runs `checkUpdates` when the last check is older than the interval; never throws. */
  maybeCheckUpdates(): Promise<void> {
    if (this.autoUpdate !== null) return this.autoUpdate;
    const run = (async () => {
      const state = await this.state();
      if (this.now() - state.lastUpdateCheck < this.updateInterval) return;
      const report = await this.checkUpdates();
      for (const failure of report.failed) this.options.log?.(`update of ${failure.id} failed: ${failure.message}`);
    })()
      .catch((error: unknown) => this.options.log?.('update check failed', error))
      .finally(() => {
        this.autoUpdate = null;
      });
    this.autoUpdate = run;
    return run;
  }

  /**
   * Makes sure a manager can answer navigations: when none is active, installs
   * and activates the default official manager.
   */
  ensureManager(): Promise<void> {
    return this.exclusive(async () => {
      const state = await this.state();
      if (state.activeManager !== null && state.plugins[state.activeManager] !== undefined) return;
      const fallback = Object.values(state.plugins).find((plugin) => plugin.manifest.role === 'manager');
      if (fallback !== undefined) {
        await this.commit({ ...state, activeManager: fallback.manifest.id });
        return;
      }
      await this.installLocked({ kind: 'official', id: this.options.defaultManager }, true, null);
      // The registry was just read and the newest version installed, so the first navigation need not check again.
      await this.commit({ ...(await this.state()), lastUpdateCheck: this.now() }).catch((error: unknown) => this.options.log?.('could not record the first update check', error));
    });
  }

  /** Re-downloads a plugin whose cached files went missing, from the source it was installed from. */
  private repair(id: string): Promise<void> {
    return this.exclusive(async () => {
      const state = await this.state();
      const plugin = state.plugins[id];
      if (plugin === undefined || (await this.intact(plugin))) return;
      // Force a full download: the digest still matches, the bytes are gone.
      await this.commit({ ...state, plugins: { ...state.plugins, [id]: { ...plugin, digest: '' } } });
      await this.installLocked(parseSource(plugin.source), false, null);
    });
  }

  /** Deletes everything the kernel stored. The page unregisters the worker afterwards. */
  reset(): Promise<void> {
    return this.exclusive(async () => {
      for (const name of await this.options.caches.keys()) {
        if (name === STATE_CACHE || name.startsWith(PLUGIN_CACHE_PREFIX)) await this.options.caches.delete(name);
      }
      this.openCaches.clear();
      this.current = EMPTY_STATE;
      this.loaded = Promise.resolve(EMPTY_STATE);
    });
  }

  /**
   * Drops previous generations once no page is open, and plugin caches no
   * state references (an install interrupted by the worker being stopped).
   */
  collectGarbage(): Promise<void> {
    return this.exclusive(async () => {
      let state = await this.state();
      const pagesOpen = (await this.options.pagesOpen?.()) ?? false;
      if (!pagesOpen) {
        const stale = Object.values(state.plugins).filter((plugin) => plugin.previous !== undefined);
        if (stale.length > 0) {
          const plugins = { ...state.plugins };
          for (const plugin of stale) {
            const { previous: _previous, ...rest } = plugin;
            plugins[plugin.manifest.id] = rest;
          }
          state = { ...state, plugins };
          await this.commit(state);
          for (const plugin of stale) if (plugin.previous !== undefined) await this.dropCache(plugin.previous.cache);
        }
      }
      const referenced = new Set<string>();
      for (const plugin of Object.values(state.plugins)) {
        referenced.add(plugin.cache);
        if (plugin.previous !== undefined) referenced.add(plugin.previous.cache);
      }
      for (const name of await this.options.caches.keys()) {
        if (!name.startsWith(PLUGIN_CACHE_PREFIX) || referenced.has(name)) continue;
        const stamp = /:([0-9a-z]+)-[0-9a-z]*$/.exec(name)?.[1];
        const created = stamp === undefined ? 0 : parseInt(stamp, 36);
        if (this.now() - created > ORPHAN_GRACE_MS) await this.dropCache(name);
      }
    });
  }

  private cache(name: string): Promise<Cache> {
    let cache = this.openCaches.get(name);
    if (cache === undefined) {
      cache = this.options.caches.open(name);
      this.openCaches.set(name, cache);
    }
    return cache;
  }

  private async dropCache(name: string): Promise<void> {
    this.openCaches.delete(name);
    await this.options.caches.delete(name);
  }

  private files(generation: Generation): ReadonlySet<string> {
    let set = this.fileSets.get(generation);
    if (set === undefined) {
      set = new Set(generation.files);
      this.fileSets.set(generation, set);
    }
    return set;
  }

  private lazyFiles(generation: Generation): ReadonlyMap<string, LazyFile> {
    let map = this.lazyMaps.get(generation);
    if (map === undefined) {
      map = new Map((generation.lazy ?? []).map((file) => [file.path, file] as const));
      this.lazyMaps.set(generation, map);
    }
    return map;
  }

  /**
   * Downloads a lazy file the first time it is requested, verifying it before it is
   * stored. Concurrent requests for the same file share one download. Does nothing for
   * a file that is not lazy; the caller then reports it as missing.
   */
  private fetchLazy(generation: Generation, cache: Cache, path: string): Promise<void> {
    const entry = this.lazyFiles(generation).get(path);
    if (entry === undefined || generation.base === undefined) return Promise.resolve();
    const key = `${generation.cache}\0${path}`;
    let pending = this.lazyInflight.get(key);
    if (pending === undefined) {
      pending = this.fetchInto(cache, generation.base, entry, null).finally(() => {
        this.lazyInflight.delete(key);
      });
      this.lazyInflight.set(key, pending);
    }
    return pending;
  }

  // ---------------------------------------------------------------- serving

  route(request: KernelRequest): Route | null {
    return routeRequest(new URL(request.url), request.method, this.origin);
  }

  async respond(request: KernelRequest, route: Route): Promise<Response> {
    try {
      switch (route.kind) {
        case 'api':
          return await this.api(request, route.name);
        case 'kernel-page':
          return html(200, kernelPage(this.options.kernelVersion));
        case 'not-found':
          return text(404, 'Not found');
        case 'plugin':
          return await this.servePlugin(request, route.id, route.path);
        case 'manager':
          return await this.serveManager(request);
      }
    } catch (error) {
      this.options.log?.(`kernel failed to answer ${request.url}`, error);
      if (request.mode === 'navigate') return html(503, recoveryPage(errorCode(error), errorMessage(error)));
      return text(503, `plugin system error: ${errorMessage(error)}`);
    }
  }

  private async serveManager(request: KernelRequest): Promise<Response> {
    const navigate = request.mode === 'navigate';
    let state = await this.state();
    if (state.activeManager === null || state.plugins[state.activeManager] === undefined) {
      if (!navigate) return text(503, 'no manager is installed');
      await this.ensureManager();
      state = await this.state();
    }
    const id = state.activeManager;
    const manager = id === null ? undefined : state.plugins[id];
    if (manager === undefined) return html(503, recoveryPage('not-installed', 'no manager is installed'));

    const url = new URL(request.url);
    try {
      return await this.serveFrom(request, manager, url.pathname.slice(1), '/', true);
    } catch (error) {
      if (!(error instanceof KernelError && error.code === 'storage-missing') || !navigate) throw error;
      // The browser evicted the files; fetch them again from where they came from.
      await this.repair(manager.manifest.id);
      const repaired = (await this.state()).plugins[manager.manifest.id];
      if (repaired === undefined) throw error;
      return this.serveFrom(request, repaired, url.pathname.slice(1), '/', true);
    }
  }

  private async servePlugin(request: KernelRequest, id: string, path: string): Promise<Response> {
    const state = await this.state();
    const plugin = state.plugins[id];
    if (plugin === undefined) return text(404, `plugin "${id}" is not installed`);
    const prefix = `/__plugins/${id}/`;
    if (new URL(request.url).pathname === `/__plugins/${id}`) {
      return this.redirect(request, prefix);
    }
    return this.serveFrom(request, plugin, path, prefix, false);
  }

  private async serveFrom(
    request: KernelRequest,
    plugin: InstalledPlugin,
    encodedPath: string,
    prefix: string,
    isManager: boolean,
  ): Promise<Response> {
    const resolved = resolvePath(this.files(plugin), encodedPath);
    if (resolved === 'bad-path') return text(400, 'Bad request path');
    if (resolved !== null && 'redirect' in resolved) {
      return this.redirect(request, `${prefix}${encodePath(resolved.redirect.slice(0, -1))}/`);
    }
    if (resolved !== null) {
      const lazy = this.lazyFiles(plugin).has(resolved.file);
      try {
        return await this.serveFile(request, plugin, resolved.file, 200);
      } catch (error) {
        // A lazy file of a version the server no longer hosts (the site was redeployed):
        // look for a newer official version now instead of waiting for the next scheduled check.
        if (!lazy || !(error instanceof KernelError) || error.code !== 'fetch-failed') throw error;
        const fresh = await this.refreshOfficial(plugin.manifest.id).catch(() => undefined);
        if (fresh === undefined || fresh.digest === plugin.digest) throw error;
        return this.serveFrom(request, fresh, encodedPath, prefix, isManager);
      }
    }

    // A page loaded before an update may still ask for files of the version it came from.
    if (plugin.previous !== undefined) {
      const old = resolvePath(this.files(plugin.previous), encodedPath);
      if (old !== null && old !== 'bad-path' && 'file' in old) {
        return this.serveFile(request, plugin.previous, old.file, 200);
      }
    }
    if (isManager && this.files(plugin).has('404.html')) {
      return this.serveFile(request, plugin, '404.html', 404);
    }
    return text(404, 'Not found');
  }

  /**
   * Installs a newer version of an official plugin right away, ignoring the update interval,
   * and returns the plugin as it is afterwards. Third-party plugins only change when asked.
   */
  private async refreshOfficial(id: string): Promise<InstalledPlugin | undefined> {
    await this.exclusive(async () => {
      const plugin = (await this.state()).plugins[id];
      if (plugin === undefined) return;
      const source = parseSource(plugin.source);
      if (source.kind !== 'official') return;
      const registry = await this.registry();
      const entry = registry.plugins[source.id];
      if (entry === undefined || entry.digest === plugin.digest) return;
      await this.installLocked(source, false, registry);
    });
    return (await this.state()).plugins[id];
  }

  private redirect(request: KernelRequest, path: string): Response {
    const url = new URL(request.url);
    const headers = new Headers({ location: `${path}${url.search}` });
    for (const [name, value] of ISOLATION_HEADERS) headers.set(name, value);
    return new Response(null, { status: 308, headers });
  }

  private async serveFile(request: KernelRequest, generation: Generation, path: string, status: number): Promise<Response> {
    const cache = await this.cache(generation.cache);
    const key = fileKey(this.origin, path);
    let stored = await cache.match(key);
    if (stored === undefined) {
      // A lazy file is fetched on first use; anything else that is gone was evicted.
      await this.fetchLazy(generation, cache, path);
      stored = await cache.match(key);
    }
    if (stored === undefined) {
      throw new KernelError('storage-missing', `the stored copy of "${path}" is gone`);
    }
    const headers = new Headers();
    headers.set('content-type', stored.headers.get('content-type') ?? contentTypeOf(path));
    for (const [name, value] of ISOLATION_HEADERS) headers.set(name, value);
    headers.set('accept-ranges', 'bytes');

    const head = request.method === 'HEAD';
    const range = status === 200 ? request.headers.get('range') : null;
    if (range === null) {
      const length = stored.headers.get('content-length');
      if (length !== null) headers.set('content-length', length);
      return new Response(head ? null : stored.body, { status, headers });
    }

    const blob = await stored.blob();
    const wanted = parseRange(range, blob.size);
    if (wanted === 'unsatisfiable') {
      headers.set('content-range', `bytes */${blob.size}`);
      return new Response(null, { status: 416, headers });
    }
    if (wanted === null) {
      headers.set('content-length', String(blob.size));
      return new Response(head ? null : blob, { status, headers });
    }
    headers.set('content-range', `bytes ${wanted.start}-${wanted.end}/${blob.size}`);
    headers.set('content-length', String(wanted.end - wanted.start + 1));
    return new Response(head ? null : blob.slice(wanted.start, wanted.end + 1), { status: 206, headers });
  }

  // ---------------------------------------------------------------- API

  private async api(request: KernelRequest, name: string): Promise<Response> {
    try {
      // A cross-site form can POST here as a top-level navigation; same-origin
      // `fetch` from a plugin page never has this mode.
      if (request.mode === 'navigate') {
        throw new KernelError('request-invalid', 'the kernel API is not reachable by navigation');
      }
      if (request.method === 'GET') {
        if (name === 'state') return json(200, { ok: true, ...(await this.status()) });
        if (name === 'registry') return json(200, { ok: true, registry: await this.registry() });
        throw new KernelError('request-invalid', `unknown kernel API "${name}"`);
      }
      if (request.method !== 'POST') {
        throw new KernelError('request-invalid', `the kernel API does not accept ${request.method}`);
      }
      const contentType = request.headers.get('content-type') ?? '';
      if (!contentType.startsWith('application/json')) {
        throw new KernelError('request-invalid', 'kernel API requests must be application/json');
      }
      let body: unknown;
      try {
        body = await request.json();
      } catch {
        throw new KernelError('request-invalid', 'the request body is not valid JSON');
      }
      if (!isRecord(body)) throw new KernelError('request-invalid', 'the request body must be an object');
      const field = (key: string): string => {
        const value = body[key];
        if (typeof value !== 'string' || value === '') {
          throw new KernelError('request-invalid', `"${key}" must be a non-empty string`);
        }
        return value;
      };

      switch (name) {
        case 'install': {
          const plugin = await this.install(field('source'), { activate: body.activate === true });
          return json(200, { ok: true, plugin });
        }
        case 'uninstall':
          await this.uninstall(field('id'));
          return json(200, { ok: true });
        case 'activate':
          await this.activate(field('id'));
          return json(200, { ok: true });
        case 'update':
          return json(200, { ok: true, ...(await this.checkUpdates()) });
        case 'reset':
          await this.reset();
          return json(200, { ok: true });
        default:
          throw new KernelError('request-invalid', `unknown kernel API "${name}"`);
      }
    } catch (error) {
      return errorResponse(error);
    }
  }
}

/** The lazy entries of a package, or `null` when every file is downloaded at install. */
function lazyOf(pkg: PluginPackage): LazyFile[] | null {
  const lazy = pkg.files
    .filter((file) => file.lazy === true)
    .map(({ path, size, sha256 }) => ({ path, size, sha256 }));
  return lazy.length === 0 ? null : lazy;
}

/** What a replaced version keeps while pages that loaded from it may still ask for its files. */
function generationOf(plugin: Generation): Generation {
  return {
    cache: plugin.cache,
    files: plugin.files,
    ...(plugin.lazy === undefined ? {} : { lazy: plugin.lazy }),
    ...(plugin.base === undefined ? {} : { base: plugin.base }),
  };
}

function sameOrigin(a: PluginSource, b: PluginSource): boolean {
  if (a.kind === 'official' && b.kind === 'official') return a.id === b.id;
  if (a.kind === 'github' && b.kind === 'github') {
    return a.owner.toLowerCase() === b.owner.toLowerCase() && a.repo.toLowerCase() === b.repo.toLowerCase();
  }
  return false;
}

// ---------------------------------------------------------------- built-in pages

function escapeHtml(value: string): string {
  return value.replace(/[&<>"']/g, (char) => `&#${char.charCodeAt(0)};`);
}

const PAGE_STYLE = `body{font:15px/1.6 system-ui,sans-serif;margin:0;background:#111;color:#ddd}
main{max-width:720px;margin:0 auto;padding:32px 20px}h1{font-size:20px;margin:0 0 4px}
small,.muted{color:#888}button{font:inherit;padding:6px 12px;margin:4px 6px 4px 0;border:1px solid #555;background:#222;color:#eee;border-radius:6px;cursor:pointer}
button:disabled{opacity:.5;cursor:default}input{font:inherit;padding:6px 8px;width:60%;background:#000;color:#eee;border:1px solid #555;border-radius:6px}
li{margin:6px 0}code{color:#9cf}.err{color:#f88;white-space:pre-wrap}`;

/** The kernel's own page at `/__kernel/`. It works with no manager at all. */
export function kernelPage(version: string): string {
  return `<!doctype html><html lang="zh-CN"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>插件系统</title><style>${PAGE_STYLE}</style></head><body><main>
<h1>插件系统</h1><small>内核 ${escapeHtml(version)}</small>
<p id="out" class="muted">读取中…</p><ul id="list"></ul>
<p><button data-act="manager">重新安装官方管理器</button><button data-act="update">检查官方更新</button><a href="/"><button>打开管理器</button></a></p>
<p><input id="src" placeholder="github:owner/repo@tag"><button data-act="install">安装</button></p>
<p><button data-act="reset">清除全部插件并重置</button></p>
<p id="err" class="err"></p></main>
<script>
const $=(id)=>document.getElementById(id);
async function call(name,body){const r=await fetch('/__kernel/api/'+name,body===undefined?{}:{method:'POST',headers:{'content-type':'application/json'},body:JSON.stringify(body)});const j=await r.json();if(!j.ok)throw new Error(j.error.code+': '+j.error.message);return j;}
async function refresh(){const s=await call('state');$('out').textContent='当前管理器：'+(s.activeManager??'无');const list=$('list');list.textContent='';
for(const p of s.plugins){const li=document.createElement('li');li.innerHTML='<b></b> <span class="muted"></span> ';li.querySelector('b').textContent=p.name+' '+p.version;li.querySelector('span').textContent=p.id+' · '+p.source+(p.active?' · 当前管理器':'');
if(p.role==='manager'&&!p.active){const b=document.createElement('button');b.textContent='设为管理器';b.onclick=()=>run(()=>call('activate',{id:p.id}));li.append(b);}
if(!p.active){const b=document.createElement('button');b.textContent='卸载';b.onclick=()=>run(()=>call('uninstall',{id:p.id}));li.append(b);}list.append(li);}}
async function run(task){$('err').textContent='';for(const b of document.querySelectorAll('button'))b.disabled=true;try{await task();await refresh();}catch(e){$('err').textContent=String(e.message??e);}finally{for(const b of document.querySelectorAll('button'))b.disabled=false;}}
document.addEventListener('click',(e)=>{const act=e.target.dataset?.act;if(!act)return;
if(act==='manager')run(async()=>{const s=await call('state');await call('install',{source:'official:'+s.defaultManager,activate:true});});
if(act==='update')run(async()=>{const r=await call('update',{});$('out').textContent='已更新：'+(r.updated.join(', ')||'无');});
if(act==='install')run(()=>call('install',{source:$('src').value.trim()}));
if(act==='reset'&&confirm('删除所有已安装的插件并注销插件系统？'))run(async()=>{await call('reset',{});const reg=await navigator.serviceWorker.getRegistration();await reg?.unregister();location.href='/';});});
run(async()=>{});
</script></body></html>`;
}

/** Shown instead of the manager when the kernel cannot serve it. */
export function recoveryPage(code: string, message: string): string {
  return `<!doctype html><html lang="zh-CN"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>插件系统</title><style>${PAGE_STYLE}</style></head><body><main>
<h1>管理器暂时无法打开</h1><p class="err">${escapeHtml(code)}: ${escapeHtml(message)}</p>
<p><button onclick="location.reload()">重试</button><a href="/__kernel/"><button>打开插件系统</button></a></p>
</main></body></html>`;
}
