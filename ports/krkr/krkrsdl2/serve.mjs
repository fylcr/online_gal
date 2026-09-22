/**
 * Serves the compiled krkrsdl2 web build, for validating the baseline.
 *
 * Two things this has to get right, both learned from the ONScripter harness:
 *
 *  - **`application/wasm`.** A wrong MIME type makes streaming instantiation fail; the browser
 *    falls back or refuses outright, and the error points at instantiation rather than at the
 *    server.
 *  - **COOP/COEP.** `SharedArrayBuffer` needs cross-origin isolation. The default build has
 *    threads off, so this is not strictly required yet — but it is the first thing to break when
 *    threads are enabled, and setting it now avoids the header being mistaken for the cause
 *    later.
 *
 * The XP3 the page loads is chosen with `?data=`, which the generated page already supports; the
 * file is served from the game directory rather than copied, so a multi-gigabyte archive is not
 * duplicated on disk.
 */
import { createServer } from 'node:http';
import { createReadStream, statSync } from 'node:fs';
import { extname, join, normalize } from 'node:path';
import { fileURLToPath } from 'node:url';
import { dirname } from 'node:path';

const here = dirname(fileURLToPath(import.meta.url));
/** The compiled build. */
const WEB_ROOT = process.env.KRKR_WEB_ROOT ?? join(here, '..', '..', '.ref', 'krkrsdl2', 'build_web');
/** Where the games live, served under `/games/`. */
const GAME_ROOT = process.env.KRKR_GAME_ROOT ?? join(here, '..', 'test-games', 'kirikiri');
const PORT = Number(process.env.PORT ?? 4630);

/** Running totals, so the read volume can be compared against the archive size. */
const stats = { rangeRequests: 0, rangeBytes: 0, wholeFileRequests: 0 };

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.mjs': 'text/javascript; charset=utf-8',
  '.wasm': 'application/wasm',
  '.xp3': 'application/octet-stream',
  '.css': 'text/css; charset=utf-8',
  '.json': 'application/json; charset=utf-8',
  '.png': 'image/png',
  '.ttf': 'font/ttf',
};

/** Resolves a URL path inside a root, refusing anything that escapes it. */
function resolveIn(root, pathname) {
  const clean = normalize(decodeURIComponent(pathname)).replace(/^(\.\.[/\\])+/, '');
  const full = join(root, clean);
  return full.startsWith(root) ? full : null;
}

function headersFor(path) {
  return {
    'Content-Type': MIME[extname(path).toLowerCase()] ?? 'application/octet-stream',
    // Bytes are read on demand, so a range request is answerable without buffering.
    'Accept-Ranges': 'bytes',
    'Cross-Origin-Opener-Policy': 'same-origin',
    'Cross-Origin-Embedder-Policy': 'require-corp',
    'Cross-Origin-Resource-Policy': 'cross-origin',
    'Cache-Control': 'no-store',
  };
}

/** Streams a file, honouring a single `Range` header. */
function sendFile(request, response, path, status = 200) {
  let stat;
  try {
    stat = statSync(path);
  } catch {
    response.writeHead(404, { 'Content-Type': 'text/plain' });
    response.end('not found');
    return;
  }
  if (stat.isDirectory()) {
    response.writeHead(403, { 'Content-Type': 'text/plain' });
    response.end('is a directory');
    return;
  }

  const headers = { ...headersFor(path), 'Content-Length': String(stat.size) };
  let start = 0;
  let end = stat.size - 1;

  const range = request.headers.range;
  if (range) {
    const match = /^bytes=(\d*)-(\d*)$/.exec(range.trim());
    if (match) {
      if (match[1]) start = Number(match[1]);
      if (match[2]) end = Number(match[2]);
      if (Number.isNaN(start) || Number.isNaN(end) || start > end || start >= stat.size) {
        response.writeHead(416, { 'Content-Range': `bytes */${stat.size}` });
        response.end();
        return;
      }
      headers['Content-Range'] = `bytes ${start}-${end}/${stat.size}`;
      headers['Content-Length'] = String(end - start + 1);
      status = 206;
    }
  }

  response.writeHead(status, headers);
  if (request.method === 'HEAD') {
    response.end();
    return;
  }
  /*
    Range requests are logged.

    This is the only way to observe what the engine actually reads: the port runs on a pthread and
    saturates the thread, so every console request to the page times out. The server sees the truth
    regardless — how many requests, and how many bytes — which is the measurement that decides
    whether ranged loading works.
  */
  if (range) {
    stats.rangeRequests += 1;
    stats.rangeBytes += end - start + 1;
    // Log the first few and then every 200th, so the log stays readable during a long session.
    if (stats.rangeRequests <= 8 || stats.rangeRequests % 200 === 0) {
      console.log(
        `  range ${String(start).padStart(12)} +${String(end - start + 1).padStart(10)} B` +
          `  (${stats.rangeRequests} reqs, ${(stats.rangeBytes / 1024 / 1024).toFixed(2)} MB)`,
      );
    }
  }
  const stream = createReadStream(path, { start, end });
  stream.on('error', () => response.destroy());
  // Without this a client disconnect throws ECONNRESET and takes the process down — the exact
  // crash that made the ONScripter harness server need restarting during long sessions.
  response.on('error', () => stream.destroy());
  stream.pipe(response);
}

const server = createServer((request, response) => {
  const url = new URL(request.url ?? '/', `http://127.0.0.1:${PORT}`);
  let pathname = url.pathname;

  /*
    Every request is logged, not only the ranged ones.

    Logging only ranges made two very different failures look identical: "the page never started"
    and "the page started but the archive probe returned nothing" both produced a range count of
    zero. Logging the whole request line distinguishes them in one glance — a `HEAD` with no
    following `GET` means the size probe ran and was rejected, while no requests at all means the
    page did not load. Requests for the module itself are excluded so the log stays readable.
  */
  if (!/\.(js|wasm|html|css|png|ttf)$/.test(pathname) && pathname !== '/') {
    console.log(`  ${request.method} ${pathname}${request.headers.range ? '  range=' + request.headers.range : ''}`);
  }

  try {
    /*
      Diagnostic channel from inside the page.

      The engine occupies its thread, so the page's log and the console cannot be read while it
      runs — every protocol request times out. A synchronous request from the engine-side bridge
      still reaches here, and lands in this log, which is the only working observation point for
      code that runs on that thread.
    */
    if (pathname === '/__diag') {
      console.log(`  [page] ${url.searchParams.get('event')}: ${url.searchParams.get('detail')}`);
      response.writeHead(200, { 'Content-Type': 'text/plain', 'Cache-Control': 'no-store' });
      response.end('ok');
      return;
    }

    if (pathname.startsWith('/games/')) {
      const path = resolveIn(GAME_ROOT, pathname.slice('/games'.length));
      if (!path) {
        response.writeHead(403).end('forbidden');
        return;
      }
      sendFile(request, response, path);
      return;
    }

    if (pathname === '/') pathname = '/launcher.html';
    // The generated page is still reachable, for comparing behaviour against the stock build.
    if (pathname === '/generated.html') pathname = '/index.html';
    const path = resolveIn(WEB_ROOT, pathname);
    if (!path) {
      response.writeHead(403).end('forbidden');
      return;
    }
    sendFile(request, response, path);
  } catch (error) {
    response.writeHead(500, { 'Content-Type': 'text/plain' });
    response.end(String(error));
  }
});

// A long session must not die on a client that closed early.
process.on('uncaughtException', (error) => {
  if (error.code === 'ECONNRESET' || error.code === 'EPIPE') return;
  console.error('uncaught:', error);
});
process.on('unhandledRejection', (error) => console.error('unhandled:', error));

server.listen(PORT, '127.0.0.1', () => {
  console.log(`krkrsdl2 web build on http://127.0.0.1:${PORT}/`);
  console.log(`  web root:  ${WEB_ROOT}`);
  console.log(`  game root: ${GAME_ROOT}`);
  console.log(`  try:       http://127.0.0.1:${PORT}/?archive=/games/kag3-demo.xp3`);
});

/** Reports the read volume, and only when it has changed. */
let lastReported = -1;
const report = () => {
  // Only when something happened.
  //
  // Printing unconditionally every 30 seconds buried the per-request lines that the log exists
  // for: a periodic "0 requests" overwrote the evidence of what the page did or did not do, and
  // reading the log became guesswork.
  if (stats.rangeRequests === lastReported && lastReported !== -1) return;
  lastReported = stats.rangeRequests;
  console.log(
    `## range requests: ${stats.rangeRequests}  bytes: ${(stats.rangeBytes / 1024 / 1024).toFixed(2)} MB`,
  );
};
process.on('SIGINT', () => {
  report();
  process.exit(0);
});
setInterval(report, 15000).unref();
