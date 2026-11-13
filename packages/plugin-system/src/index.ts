/**
 * The online-gal plugin system: a Service Worker that acts as the local
 * server for the site.
 *
 * The page registers it once; from then on every same-origin request is
 * answered by the kernel from installed plugins (the active manager is the
 * site itself), and the real server is only where plugins are downloaded from:
 * the official registry on Cloudflare Pages, or tagged GitHub repositories for
 * third-party plugins.
 *
 * `sw.ts` is the worker entry and is bundled by `scripts/build-site.mjs`; this
 * module exports the parts that tests and tools use directly.
 */
export * from './errors.js';
export * from './format.js';
export * from './sources.js';
export * from './kernel.js';
