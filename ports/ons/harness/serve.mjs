/**
 * Serves the worker-based ONS host.
 *
 * Three roots, because the pieces come from different places:
 *
 *  - `ports/ons/harness/`  the page, the host module and the worker
 *  - the engine build      `onsyuri.js` / `onsyuri.wasm`, served at the root so
 *                          the worker's `importScripts` and `locateFile` resolve
 *  - `/game/`              the real game files, hard-linked so nothing is copied
 *
 * All three are resolved from this file's own location, so the harness works from any checkout and
 * from any working directory. The engine and the game live in the gitignored `.ref/` clone, or
 * wherever `ONS_ENGINE_DIR` / `ONS_GAME_DIR` point.
 *
 * Range support is the point of the exercise: the whole design is that a
 * multi-gigabyte archive is read in windows. `/game/*` therefore answers `206`
 * properly, and a plain `200` for those files would defeat the harness.
 *
 * COOP/COEP are required for `SharedArrayBuffer`.
 */
import { createReadStream, existsSync, statSync } from 'node:fs';
import { createServer } from 'node:http';
import { dirname, extname, join, normalize, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const HERE = dirname(fileURLToPath(import.meta.url));
const ROOT = resolve(HERE, '..', '..', '..');
const HARNESS = resolve(process.env.ONS_HARNESS_DIR ?? HERE);
const ENGINE = resolve(process.env.ONS_ENGINE_DIR ?? join(ROOT, '.ref', 'onsyuri-web', 'onsyuri_web'));
const GAME = resolve(process.env.ONS_GAME_DIR ?? join(ENGINE, 'game'));
const PORT = Number(process.env.PORT ?? 4620);

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.mjs': 'text/javascript; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.wasm': 'application/wasm',
  '.ttf': 'font/ttf',
  '.txt': 'text/plain; charset=utf-8',
  '.nsa': 'application/octet-stream',
  '.exe': 'application/octet-stream',
  '.png': 'image/png',
  '.jpg': 'image/jpeg',
  '.ogg': 'audio/ogg',
  '.mp3': 'audio/mpeg',
};

const ISOLATION = {
  'Cross-Origin-Opener-Policy': 'same-origin',
  'Cross-Origin-Embedder-Policy': 'require-corp',
  'Cross-Origin-Resource-Policy': 'cross-origin',
};

/** Bytes served per path, so the harness can report real transfer volume. */
const served = new Map();

function streamTo(stream, response) {
  stream.on('error', () => response.destroy());
  response.on('error', () => stream.destroy());
  response.on('close', () => stream.destroy());
  stream.pipe(response);
}

function serveFile(target, pathname, request, response) {
  let stats;
  try {
    stats = statSync(target);
    if (stats.isDirectory()) {
      target = join(target, 'index.html');
      stats = statSync(target);
    }
  } catch {
    response.writeHead(404, ISOLATION).end(`not found: ${pathname}`);
    return;
  }

  const type = MIME[extname(target).toLowerCase()] ?? 'application/octet-stream';
  const range = request.headers.range;

  if (range) {
    const match = /^bytes=(\d*)-(\d*)$/.exec(range.trim());
    if (match) {
      const start = match[1] ? Number(match[1]) : 0;
      const end = Math.min(match[2] ? Number(match[2]) : stats.size - 1, stats.size - 1);
      if (Number.isFinite(start) && start <= end && start < stats.size) {
        const length = end - start + 1;
        served.set(pathname, (served.get(pathname) ?? 0) + length);
        response.writeHead(206, {
          'Content-Type': type,
          'Content-Length': length,
          'Content-Range': `bytes ${start}-${end}/${stats.size}`,
          'Accept-Ranges': 'bytes',
          ...ISOLATION,
        });
        streamTo(createReadStream(target, { start, end }), response);
        return;
      }
      response.writeHead(416, { 'Content-Range': `bytes */${stats.size}`, ...ISOLATION }).end();
      return;
    }
  }

  served.set(pathname, (served.get(pathname) ?? 0) + stats.size);
  response.writeHead(200, {
    'Content-Type': type,
    'Content-Length': stats.size,
    'Accept-Ranges': 'bytes',
    ...ISOLATION,
  });
  streamTo(createReadStream(target), response);
}

const server = createServer((request, response) => {
  const url = new URL(request.url ?? '/', 'http://localhost');
  const pathname = decodeURIComponent(url.pathname);

  // Report what has actually been transferred, which is how the lazy claim is
  // checked from the server side as well as from the engine's own counters.
  if (pathname === '/__served') {
    response.writeHead(200, { 'Content-Type': 'application/json', ...ISOLATION });
    response.end(
      JSON.stringify(Object.fromEntries([...served.entries()].sort()), null, 2),
    );
    return;
  }

  if (pathname.startsWith('/game/')) {
    const relative = pathname.replace(/^\/game\//, '');
    const target = join(GAME, normalize(relative));
    if (!target.startsWith(GAME)) {
      response.writeHead(403, ISOLATION).end('forbidden');
      return;
    }
    serveFile(target, pathname, request, response);
    return;
  }

  // Engine assets at the root, so `importScripts('onsyuri.js')` and the wasm
  // resolution both work without special-casing in the worker.
  for (const [base, prefix] of [[HARNESS, ''], [ENGINE, '']]) {
    const relative = pathname.replace(/^\/+/, '') || 'index.html';
    const target = join(base, normalize(relative));
    if (target.startsWith(base) && existsSync(target)) {
      serveFile(target, pathname, request, response);
      void prefix;
      return;
    }
  }

  response.writeHead(404, ISOLATION).end(`not found: ${pathname}`);
});

process.on('uncaughtException', (error) => process.stderr.write(`[uncaught] ${String(error)}\n`));

server.listen(PORT, '127.0.0.1', () => {
  console.log(`ONS worker host: http://127.0.0.1:${PORT}/`);
  console.log(`  harness: ${HARNESS}`);
  console.log(`  engine:  ${ENGINE}`);
  console.log(`  game:    ${GAME}`);
  console.log(`  transfer report: http://127.0.0.1:${PORT}/__served`);
});
