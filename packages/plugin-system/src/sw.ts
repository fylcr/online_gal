/**
 * Service Worker entry. Bundled into `/sw.js` by `scripts/build-site.mjs`
 * as a classic script (module workers are missing from older Firefox).
 *
 * Kept deliberately thin: all decisions live in `Kernel`, which is tested in
 * Node. This file only wires the worker's events to it.
 */
import { IndexedDbVaultStore } from './account/vault.js';
import { Kernel, type KernelNotice } from './kernel.js';

declare const __KERNEL_VERSION__: string;
declare const __DEFAULT_MANAGER__: string;
/** Base of raw GitHub content; only tests point it elsewhere. */
declare const __GITHUB_RAW__: string;
/** The site's public host (cf-account-admin design.md A6); `SITE_HOST` at build time. */
declare const __SITE_HOST__: string;

// The package compiles against the DOM lib; these are the few worker-only
// shapes this file needs.
interface ExtendableEvent extends Event {
  waitUntil(promise: Promise<unknown>): void;
}
interface FetchEvent extends ExtendableEvent {
  readonly request: Request;
  /** The client that made the request; empty for navigations. */
  readonly clientId: string;
  respondWith(response: Promise<Response>): void;
}
interface Client {
  readonly url: string;
  postMessage(message: unknown): void;
}
interface WorkerScope {
  readonly location: { readonly origin: string };
  readonly caches: CacheStorage;
  readonly indexedDB: IDBFactory;
  readonly clients: {
    claim(): Promise<void>;
    get(id: string): Promise<Client | undefined>;
    matchAll(options: { type: 'window'; includeUncontrolled?: boolean }): Promise<readonly Client[]>;
  };
  skipWaiting(): Promise<void>;
  fetch(input: string, init?: RequestInit): Promise<Response>;
  addEventListener(type: 'install' | 'activate', listener: (event: ExtendableEvent) => void): void;
  addEventListener(type: 'fetch', listener: (event: FetchEvent) => void): void;
}
declare const self: WorkerScope;

const log = (message: string, error?: unknown): void => {
  if (error === undefined) console.warn(`[plugin-system] ${message}`);
  else console.warn(`[plugin-system] ${message}`, error);
};

const kernel = new Kernel({
  origin: self.location.origin,
  caches: self.caches,
  fetch: (input, init) => self.fetch(input, init),
  defaultManager: __DEFAULT_MANAGER__,
  kernelVersion: __KERNEL_VERSION__,
  githubRaw: __GITHUB_RAW__,
  vault: new IndexedDbVaultStore(self.indexedDB),
  clientUrl: async (id) => (await self.clients.get(id))?.url,
  siteHost: __SITE_HOST__,
  pagesOpen: async () => (await self.clients.matchAll({ type: 'window' })).length > 0,
  notify: (notice: KernelNotice) => {
    void self.clients.matchAll({ type: 'window' }).then((clients) => {
      for (const client of clients) client.postMessage({ source: 'online-gal-kernel', ...notice });
    });
  },
  log,
});

self.addEventListener('install', (event) => {
  // Have a manager in place before the first navigation needs one. A failure
  // (offline, registry down) does not block the kernel: navigation retries it
  // and shows the recovery page if it still cannot.
  event.waitUntil(
    kernel
      .ensureManager()
      .catch((error: unknown) => log('could not install the default manager', error))
      .then(() => self.skipWaiting()),
  );
});

self.addEventListener('activate', (event) => {
  event.waitUntil(self.clients.claim());
});

self.addEventListener('fetch', (event) => {
  const route = kernel.route(event.request);
  if (route === null) return;
  event.respondWith(kernel.respond(event.request, route, event.clientId));
  if (event.request.mode === 'navigate') event.waitUntil(kernel.maybeCheckUpdates());
});
