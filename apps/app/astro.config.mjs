import { createReadStream, existsSync, statSync } from 'node:fs';
import { extname, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

import { defineConfig } from 'astro/config';
import svelte from '@astrojs/svelte';

import {
  artemisAssetPlugin,
  artemisAssetsIntegration,
  inspectArtemisAssets,
} from './scripts/artemis-assets.mjs';

/**
 * The launcher is a fully client-side application: every engine runs in the
 * browser and the library lives in IndexedDB. Astro's static output is
 * therefore the only mode that makes sense — there is no server to render on.
 *
 * The COOP/COEP headers are served for parity with the cross-origin-isolated
 * requirements of threaded WebAssembly engines (AetherKiri needs them for its
 * Web build). The browsers this platform targets do not require them, but
 * setting them keeps a WebAssembly runtime working if one is added later, and
 * they also make `SharedArrayBuffer` available for future audio or decode
 * workers.
 */
const ISOLATION_HEADERS = {
  'Cross-Origin-Opener-Policy': 'same-origin',
  'Cross-Origin-Embedder-Policy': 'credentialless',
};

/* ------------------------------------------------------------------ */
/* The KiriKiri WebAssembly build                                      */
/* ------------------------------------------------------------------ */
//
// The real KiriKiri engine is a build, not a package: `ports/krkr/kirikiroid2/run-build.sh`
// produces `out/web/release/` inside the reference checkout, and the launcher has to
// serve that page from *its own origin* for two reasons. The framed page needs the same
// cross-origin isolation the app has (the engine uses `SharedArrayBuffer` through JSPI),
// and the host byte-source seam is installed into a same-origin frame's window — a
// cross-origin frame's window is opaque to us.
//
// Serving it through middleware rather than copying ~30 MB into `public/` keeps the
// build output the single source of truth, so a rebuild is picked up on reload with no
// sync step. The mount path is `/krkr2-web/`, which is the URL the app publishes to the
// provider (`apps/app/src/lib/platform.ts`).
//
// Not covered here: a *production* `astro build` emits only what is in `public/`, so the
// served build below is a development/preview arrangement. Bundling the artifacts into a
// production build is a separate packaging step.
const REPO_ROOT = resolve(fileURLToPath(import.meta.url), '../../..');
const KIRIKIRI_MOUNT = '/krkr2-web/';
const KIRIKIRI_BUILD_DIR = resolve(REPO_ROOT, '.ref/kirikiroid2-web/out/web/release');

const CONTENT_TYPES = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.mjs': 'text/javascript; charset=utf-8',
  '.wasm': 'application/wasm',
  '.json': 'application/json; charset=utf-8',
  '.webmanifest': 'application/manifest+json',
  '.css': 'text/css; charset=utf-8',
  '.zip': 'application/zip',
  '.ttf': 'font/ttf',
  '.png': 'image/png',
  '.svg': 'image/svg+xml',
};

/**
 * Streams one file out of the build directory, refusing anything outside it.
 *
 * `resolve` plus a prefix check is what keeps a traversal (`/krkr2-web/../../etc/passwd`)
 * from escaping the mount, since the path arrives from the request.
 */
function serveBuildFile(request, response, next) {
  // Vite's post-hook walks the stack with no request during middlewareMode startup.
  if (!request || typeof request.url !== 'string') return typeof next === 'function' ? next() : undefined;
  const url = request.url ?? '';
  if (!url.startsWith(KIRIKIRI_MOUNT)) {
    next();
    return;
  }
  if (!existsSync(KIRIKIRI_BUILD_DIR)) {
    response.statusCode = 503;
    response.setHeader('Content-Type', 'text/plain; charset=utf-8');
    response.end(
      'The KiriKiri WebAssembly build is not present. Run ' +
        'ports/krkr/kirikiroid2/run-build.sh to produce it.',
    );
    return;
  }
  const requested = decodeURIComponent(url.slice(KIRIKIRI_MOUNT.length).split('?')[0]);
  const file = resolve(KIRIKIRI_BUILD_DIR, requested === '' ? 'index.html' : requested);
  if (file !== KIRIKIRI_BUILD_DIR && !file.startsWith(KIRIKIRI_BUILD_DIR + sep)) {
    response.statusCode = 403;
    response.end('forbidden');
    return;
  }
  if (!existsSync(file) || !statSync(file).isFile()) {
    response.statusCode = 404;
    response.end('not found');
    return;
  }

  // The framed page is isolated in its own right: without these it would load but
  // fail to start a threaded engine, and the failure looks like a hang.
  for (const [name, value] of Object.entries(ISOLATION_HEADERS)) {
    response.setHeader(name, value);
  }
  response.setHeader(
    'Content-Type',
    CONTENT_TYPES[extname(file).toLowerCase()] ?? 'application/octet-stream',
  );
  // A 22 MB wasm and a 7 MB asset zip are streamed rather than buffered.
  createReadStream(file).pipe(response);
}

/** Mounts the build directory on the dev and preview servers alike. */
function kirikiriBuild() {
  return {
    name: 'online-gal:kirikiri-build',
    configureServer(server) {
      server.middlewares.use(serveBuildFile);
    },
    configurePreviewServer(server) {
      server.middlewares.use(serveBuildFile);
    },
  };
}

/* ------------------------------------------------------------------ */
/* The Artemis compat WebAssembly build                                */
/* ------------------------------------------------------------------ */
//
// `artemis-assets.mjs` is the single gate over `dist/artemis-compat/`. Its snapshot is taken once,
// here, and the *same* object feeds the compile-time flag, the dev middleware and the production
// copy below — three readers of one fact, so a build cannot advertise an engine it did not ship, or
// ship bytes it withdrew. A withdrawn engine is a log line, not a failed build: the other engines
// are unaffected, and a stub would turn "not built here" into a hang in the browser.
const ARTEMIS = inspectArtemisAssets();
if (!ARTEMIS.available) {
  console.warn(`[artemis] withdrawn, the rest of the app builds unchanged: ${ARTEMIS.reason}`);
} else {
  // ponytail: one warn line. This gate matches a manifest to the bytes beside it; it is not a release.
  console.warn('[artemis] candidate static gate, not a released body');
}

export default defineConfig({
  output: 'static',
  integrations: [svelte(), artemisAssetsIntegration(ARTEMIS)],
  server: {
    host: '127.0.0.1',
    port: 4321,
    headers: ISOLATION_HEADERS,
  },
  vite: {
    // A compile-time boolean: the page either publishes a working engine URL or never mentions one.
    define: { __ARTEMIS_ASSETS_AVAILABLE__: JSON.stringify(ARTEMIS.available) },
    build: {
      target: 'es2022',
      sourcemap: true,
    },
    worker: {
      format: 'es',
    },
    plugins: [kirikiriBuild(), artemisAssetPlugin(ARTEMIS)],
  },
});
