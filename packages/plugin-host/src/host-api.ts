import type { GameSource, WritableStore } from '@online-gal/engine-api';
import { BudgetMeter } from './budget.js';
import { GrantLedger, type PluginCapability } from './capabilities.js';
import { PluginError } from './errors.js';
import type { PluginRegistration, PluginRuntimeInfo } from './registry.js';

export const PLUGIN_LOG_LEVELS = ['debug', 'info', 'warn', 'error'] as const;

export type PluginLogLevel = (typeof PLUGIN_LOG_LEVELS)[number];

export interface PluginLogRecord {
  readonly pluginId: string;
  readonly level: PluginLogLevel;
  readonly message: string;
  readonly at: number;
}

export interface HostNetworkRequest {
  readonly url: string;
  readonly method?: string;
  readonly headers?: Readonly<Record<string, string>>;
  readonly body?: Uint8Array;
}

export interface HostNetworkResponse {
  readonly status: number;
  readonly headers: Readonly<Record<string, string>>;
  readonly body: Uint8Array;
}

/**
 * The host's own outbound path.
 *
 * A plugin never sees `fetch`: it gets this function, which the host supplies
 * only when it means to allow network use at all, and which owns allowlists,
 * credentials and rate limits. Absent proxy means "no network", not "fall back
 * to fetch".
 */
export type HostNetworkProxy = (request: HostNetworkRequest) => Promise<HostNetworkResponse>;

/** Read-only game access. Every method is root-bounded and capability-checked. */
export interface PluginVfs {
  list(dir?: string): Promise<readonly string[]>;
  readFile(path: string): Promise<Uint8Array>;
  readRange(path: string, offset: number, length: number): Promise<Uint8Array>;
  size(path: string): Promise<number>;
  exists(path: string): Promise<boolean>;
  isDirectory(path: string): Promise<boolean>;
}

/** The plugin's private writable slice, rooted at `plugins/<pluginId>/`. */
export interface PluginStore {
  read(path: string): Promise<Uint8Array | undefined>;
  write(path: string, data: Uint8Array): Promise<void>;
  remove(path: string): Promise<void>;
  list(prefix?: string): Promise<readonly string[]>;
}

export interface PluginNet {
  request(request: HostNetworkRequest): Promise<HostNetworkResponse>;
}

/**
 * Kernel administration, for the plugin that acts as the manager.
 *
 * It is the same surface the page bootstrap has on `PluginHost`, minus booting:
 * where plugins come from is still the host's decision.
 */
export interface PluginManager {
  list(): readonly PluginRuntimeInfo[];
  rejections(): readonly PluginRegistration[];
  grant(pluginId: string, capability: PluginCapability): void;
  revoke(pluginId: string, capability: PluginCapability): void;
  unload(pluginId: string): Promise<void>;
  /** Called after any plugin changes state. Dropped when the subscriber stops. */
  subscribe(listener: () => void): () => void;
}

/** A service as consumers see it: its methods, nothing else. */
export type PluginService = Readonly<Record<string, (...args: readonly unknown[]) => unknown>>;

/** What a plugin's `setup` and its hooks receive. Nothing else is exposed. */
export interface PluginContext {
  readonly pluginId: string;
  readonly vfs: PluginVfs;
  readonly store: PluginStore;
  readonly net: PluginNet;
  readonly log: (...args: readonly unknown[]) => void;
  readonly notify: (message: string, level?: PluginLogLevel) => void;
  readonly now: () => number;
  /**
   * Returns a service this plugin declared in `requires`. The facade stops
   * working the moment its provider stops.
   */
  use<T extends object = PluginService>(service: string): T;
  /** Kernel administration. Every call needs `plugins.manage`. */
  readonly plugins: PluginManager;
}

/** What the kernel does for a context that the context cannot do on its own. */
export interface KernelBridge {
  use(pluginId: string, service: string): object;
  manager(pluginId: string): PluginManager;
}

export interface HostApiOptions {
  /** The mounted game. Absent at page open, before a game is chosen. */
  readonly source?: GameSource;
  /** Host persistence. Absent means plugins have no store. */
  readonly store?: WritableStore;
  readonly notify?: (message: string, level: PluginLogLevel) => void;
  readonly net?: HostNetworkProxy;
  readonly logger?: (record: PluginLogRecord) => void;
  /** Injected by tests to drive the time budget without sleeping. */
  readonly clock?: () => number;
}

const MAX_PATH_LENGTH = 4096;
const MAX_LOG_ARGUMENT = 2000;
const MAX_NOTIFICATION = 2000;
const DRIVE_PREFIX = /^[a-zA-Z]:/;

function outsideRoot(pluginId: string, path: string): PluginError {
  return new PluginError(
    'path-outside-root',
    `plugin "${pluginId}" requested a path outside its root: ${JSON.stringify(path)}`,
    pluginId,
  );
}

/**
 * Rejects the shapes that escape a root *before* anything normalises them.
 *
 * Normalisation happens downstream (`@online-gal/vfs` clamps `..`), but that is
 * a convenience for host code, not a boundary: a plugin path that tries to leave
 * its root is refused here and never reaches the normaliser, so the refusal can
 * be reported and counted instead of silently mapping somewhere unexpected.
 */
function assertRelativePath(pluginId: string, path: unknown, allowEmpty = false): string {
  if (typeof path !== 'string' || (path.length === 0 && !allowEmpty)) {
    throw new PluginError('path-outside-root', 'path must be a non-empty string', pluginId);
  }
  if (path.length > MAX_PATH_LENGTH) throw outsideRoot(pluginId, path);
  if (path.includes('\0')) throw outsideRoot(pluginId, path);
  if (DRIVE_PREFIX.test(path) || path.startsWith('\\\\') || path.startsWith('//')) {
    throw outsideRoot(pluginId, path);
  }
  if (path.startsWith('/') || path.startsWith('\\')) throw outsideRoot(pluginId, path);
  const segments = path.split(/[\\/]/);
  if (segments.some((segment) => segment === '..')) throw outsideRoot(pluginId, path);
  return path;
}

function describe(...args: readonly unknown[]): string {
  return args
    .map((arg) => {
      if (typeof arg === 'string') return arg;
      if (arg instanceof Error) return `${arg.name}: ${arg.message}`;
      try {
        return JSON.stringify(arg) ?? String(arg);
      } catch {
        return String(arg);
      }
    })
    .join(' ')
    .slice(0, MAX_LOG_ARGUMENT);
}

function copyBytes(bytes: Uint8Array): Uint8Array {
  return new Uint8Array(bytes);
}

function unavailable(pluginId: string, what: string): PluginError {
  return new PluginError('capability-unavailable', `${what} (requested by "${pluginId}")`, pluginId);
}

/**
 * Builds the only object graph a plugin is allowed to touch.
 *
 * Two rules hold for every method here:
 *
 * 1. The grant is required on *every* call, not checked once at setup, so a
 *    revoked capability stops working immediately even if the plugin kept the
 *    context in a closure.
 * 2. The budget is checked at every host boundary, which is the only place the
 *    host can see a plugin's resource use at all.
 *
 * Bytes handed to a plugin are copies: mutating them cannot corrupt a cache or
 * another plugin's view of the same file.
 */
export class HostApi {
  readonly context: PluginContext;

  constructor(
    private readonly pluginId: string,
    private readonly grants: GrantLedger,
    private readonly budget: BudgetMeter,
    private readonly options: HostApiOptions,
    requires: readonly string[] = [],
    bridge?: KernelBridge,
  ) {
    const storeRoot = `plugins/${pluginId}/`;
    const gameSource = (): GameSource => {
      if (options.source === undefined) throw unavailable(pluginId, 'no game source is mounted');
      return options.source;
    };
    const hostStore = (): WritableStore => {
      if (options.store === undefined) throw unavailable(pluginId, 'the host has no writable store');
      return options.store;
    };
    const kernel = (): KernelBridge => {
      if (bridge === undefined) throw unavailable(pluginId, 'this context is not attached to a kernel');
      return bridge;
    };

    const vfs = Object.freeze<PluginVfs>({
      list: async (dir = '') => {
        grants.require('vfs.read');
        budget.checkpoint();
        return gameSource().list(assertRelativePath(pluginId, dir, true));
      },
      readFile: async (path) => {
        grants.require('vfs.read');
        budget.checkpoint();
        const bytes = await gameSource().readFile(assertRelativePath(pluginId, path));
        budget.chargeMemory(bytes.byteLength);
        return copyBytes(bytes);
      },
      readRange: async (path, offset, length) => {
        grants.require('vfs.read');
        budget.checkpoint();
        const source = gameSource();
        const target = assertRelativePath(pluginId, path);
        if (!Number.isInteger(offset) || offset < 0 || !Number.isInteger(length) || length < 0) {
          throw new RangeError('readRange offset and length must be non-negative integers');
        }
        budget.chargeMemory(length);
        // Charged before the read: the reservation is what stops a single huge
        // range request from being turned into a huge allocation. A failed read
        // keeps the charge, which is the conservative side to be wrong on.
        const read = source.readRange;
        if (read === undefined) {
          // A source without random access is not an error the plugin caused.
          throw unavailable(pluginId, 'the mounted game source does not support range reads');
        }
        return copyBytes(await read.call(source, target, offset, length));
      },
      size: async (path) => {
        grants.require('vfs.read');
        budget.checkpoint();
        return gameSource().size(assertRelativePath(pluginId, path));
      },
      exists: async (path) => {
        grants.require('vfs.read');
        budget.checkpoint();
        return gameSource().exists(assertRelativePath(pluginId, path));
      },
      isDirectory: async (path) => {
        grants.require('vfs.read');
        budget.checkpoint();
        return gameSource().isDirectory(assertRelativePath(pluginId, path));
      },
    });

    const store = Object.freeze<PluginStore>({
      read: async (path) => {
        grants.require('vfs.write');
        budget.checkpoint();
        const stored = await hostStore().read(storeRoot + assertRelativePath(pluginId, path));
        if (stored === undefined) return undefined;
        budget.chargeMemory(stored.byteLength);
        return copyBytes(stored);
      },
      write: async (path, data) => {
        grants.require('vfs.write');
        budget.checkpoint();
        const target = hostStore();
        if (!(data instanceof Uint8Array)) {
          throw new TypeError('store.write data must be a Uint8Array');
        }
        budget.chargeMemory(data.byteLength);
        await target.write(storeRoot + assertRelativePath(pluginId, path), copyBytes(data));
      },
      remove: async (path) => {
        grants.require('vfs.write');
        budget.checkpoint();
        await hostStore().remove(storeRoot + assertRelativePath(pluginId, path));
      },
      list: async (prefix = '') => {
        grants.require('vfs.write');
        budget.checkpoint();
        const target = hostStore();
        const scoped = assertRelativePath(pluginId, prefix, true);
        const fullPrefix = storeRoot + scoped;
        // The store's `list` does a plain prefix match, so "plugins/a.plugin"
        // would also match "plugins/a.pluginfoo". Filter on a directory
        // boundary before stripping the private root.
        const boundary = fullPrefix.endsWith('/') ? fullPrefix : `${fullPrefix}/`;
        const entries = await target.list(fullPrefix);
        return entries
          .filter((entry) => entry === fullPrefix || entry.startsWith(boundary))
          .map((entry) => entry.slice(fullPrefix.length).replace(/^\//, ''));
      },
    });

    const net = Object.freeze<PluginNet>({
      request: async (request) => {
        grants.require('net.request');
        budget.checkpoint();
        const proxy = options.net;
        if (proxy === undefined) {
          throw new PluginError(
            'capability-unavailable',
            `plugin "${pluginId}" requested network access but the host has no network proxy configured`,
            pluginId,
          );
        }
        if (typeof request?.url !== 'string' || request.url.length === 0) {
          throw new PluginError('capability-unavailable', 'net.request needs a url', pluginId);
        }
        if (request.body !== undefined) budget.chargeMemory(request.body.byteLength);
        const response = await proxy(request);
        budget.chargeMemory(response.body.byteLength);
        budget.checkpoint();
        return { ...response, body: copyBytes(response.body) };
      },
    });

    const log = (...args: readonly unknown[]): void => {
      // Logging is deliberately ungated: it is write-only, goes to the host's
      // own sink, and a plugin that cannot report why it failed is worse than
      // one that can log a little too much.
      options.logger?.({
        pluginId,
        level: 'info',
        message: describe(...args),
        at: options.clock?.() ?? Date.now(),
      });
    };

    const notify = (message: string, level: PluginLogLevel = 'info'): void => {
      grants.require('ui.notify');
      budget.checkpoint();
      const sink = options.notify;
      if (sink === undefined) {
        throw new PluginError(
          'capability-unavailable',
          `plugin "${pluginId}" requested a notification surface but the host has none`,
          pluginId,
        );
      }
      if (typeof message !== 'string') throw new TypeError('notify message must be a string');
      if (!PLUGIN_LOG_LEVELS.includes(level)) {
        throw new TypeError(`notify level must be one of ${PLUGIN_LOG_LEVELS.join(', ')}`);
      }
      sink(message.slice(0, MAX_NOTIFICATION), level);
    };

    const use = <T extends object = PluginService>(service: string): T => {
      // Undeclared services are refused here, before the kernel is asked: the
      // manifest is what the boot order was computed from, so a service used
      // without being declared may not even be active yet.
      if (typeof service !== 'string' || !requires.includes(service)) {
        throw new PluginError(
          'capability-denied',
          `plugin "${pluginId}" did not declare service "${String(service)}" in "requires"`,
          pluginId,
        );
      }
      budget.checkpoint();
      return kernel().use(pluginId, service) as T;
    };

    const managed = (): PluginManager => {
      grants.require('plugins.manage');
      return kernel().manager(pluginId);
    };
    const plugins = Object.freeze<PluginManager>({
      list: () => managed().list(),
      rejections: () => managed().rejections(),
      grant: (target, capability) => managed().grant(target, capability),
      revoke: (target, capability) => managed().revoke(target, capability),
      unload: async (target) => managed().unload(target),
      subscribe: (listener) => managed().subscribe(listener),
    });

    this.context = Object.freeze({
      pluginId,
      vfs,
      store,
      net,
      log,
      notify,
      now: () => options.clock?.() ?? Date.now(),
      use,
      plugins,
    });
  }
}
