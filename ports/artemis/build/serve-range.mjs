/**
 * A minimal static server for the Artemis web-host smoke test.
 *
 * Why not `coi-server.py`: it computes a content SHA-256 to build an ETag, which means reading the
 * whole file — fine for a 17 MB demo, not for the 2.69 GB pack this test exists to stream. It answered
 * a `Range` request with **200**, and the page's byte source refuses anything but **206** precisely
 * because "the server sent the whole file" is the one failure that would otherwise look like slow
 * progress on a multi-gigabyte archive.
 *
 * Also logs every request with its range, so the claims about how much was read can be checked against
 * the server rather than only against the page's own counters.
 *
 * COOP/COEP are sent because the engine build may want them; they cost nothing here.
 */
import { createReadStream, readdirSync, statSync } from 'node:fs';
import { createServer } from 'node:http';
import { extname, join, normalize, resolve } from 'node:path';

const ROOT = resolve(process.argv[2] ?? '.');
const PORT = Number(process.argv[3] ?? 4650);
const LOG = process.env.RANGE_LOG !== '0';

const MIME = {
  '.html': 'text/html; charset=utf-8',
  '.js': 'text/javascript; charset=utf-8',
  '.wasm': 'application/wasm',
  '.pfs': 'application/octet-stream',
  '.000': 'application/octet-stream',
  '.001': 'application/octet-stream',
  '.mp4': 'video/mp4',
  '.txt': 'text/plain; charset=utf-8',
};

const ISO = {
  'Cross-Origin-Opener-Policy': 'same-origin',
  'Cross-Origin-Embedder-Policy': 'require-corp',
  'Cross-Origin-Resource-Policy': 'cross-origin',
  'Accept-Ranges': 'bytes',
  // No caching, deliberately. The three host artefacts (page, worker, wasm) are rebuilt constantly
  // during a port, and a stale `web_worker.js` is indistinguishable from a change that did not work:
  // the page keeps its old behaviour and the diagnostic that was just added simply never runs. That
  // cost a debugging round once, so the server removes the possibility.
  'Cache-Control': 'no-store, must-revalidate',
  'Pragma': 'no-cache',
};

let rangeHits = 0;
let fullSends = 0;
let bytesSent = 0;
// Counted apart from `bytesSent`, which also carries the host artefacts (page, glue, wasm, worker).
// "how much of the 6 GB archive did this actually read" is the number that matters, and mixing in a
// 1.1 MB wasm download would quietly inflate it.
let packBytes = 0;
const isPack = (p) => p.endsWith('.pfs') || /\.pfs\.\d+$/.test(p);

createServer((req, res) => {
  const url = new URL(req.url, 'http://x');
  const pathname = decodeURIComponent(url.pathname);
  // Node on Windows has no `SIGUSR2`, so the counters are read over HTTP instead of by signalling the
  // process. That also makes them readable at any point in a run rather than only at exit.
  if (pathname === '/__totals') {
    res.writeHead(200, { 'Content-Type': 'application/json' });
    res.end(JSON.stringify({ rangeHits, fullSends, bytesSent, packBytes }));
    return;
  }
  // The engine's Android host discovers its pack chain with `opendir` + a `.pfs` suffix test, so it
  // needs a *directory* to look in. There is no real filesystem in the browser, so the shim recreates
  // one in Emscripten's MEMFS from this list. Serving the list beats hardcoding it: the same host then
  // works for any game dropped into ROOT.
  if (pathname === '/__packs') {
    let names = [];
    try {
      names = readdirSync(ROOT).filter((n) => n.endsWith('.pfs'));
    } catch { /* unreadable root: answer with an empty list rather than failing the boot */ }
    // Order matters: the host takes the FIRST listed pack whose chain opens, so `root.pfs` must lead.
    // This mirrors `EngineContext::ResolvePack`, which sorts root.pfs first for the same reason.
    names.sort((a, b) => {
      const ra = a.toLowerCase() === 'root.pfs';
      const rb = b.toLowerCase() === 'root.pfs';
      if (ra !== rb) return ra ? -1 : 1;
      return a < b ? -1 : a > b ? 1 : 0;
    });
    res.writeHead(200, { ...ISO, 'Content-Type': 'application/json' });
    res.end(JSON.stringify(names));
    return;
  }
  const target = join(ROOT, normalize(pathname).replace(/^([/\\])+/, ''));
  if (!target.startsWith(ROOT)) {
    res.writeHead(403, ISO).end('forbidden');
    return;
  }
  let stat;
  try {
    stat = statSync(target);
  } catch {
    res.writeHead(404, ISO).end('not found');
    return;
  }
  if (stat.isDirectory()) {
    res.writeHead(302, { ...ISO, Location: pathname.replace(/\/?$/, '/') + 'index.html' }).end();
    return;
  }

  const type = MIME[extname(target).toLowerCase()] ?? 'application/octet-stream';
  const range = req.headers.range;
  const match = range ? /^bytes=(\d*)-(\d*)$/.exec(range.trim()) : null;

  if (match) {
    const start = match[1] ? Number(match[1]) : 0;
    const end = match[2] ? Math.min(Number(match[2]), stat.size - 1) : stat.size - 1;
    if (start > end || start >= stat.size) {
      res.writeHead(416, { ...ISO, 'Content-Range': `bytes */${stat.size}` }).end();
      return;
    }
    const length = end - start + 1;
    rangeHits += 1;
    bytesSent += length;
    if (isPack(pathname)) packBytes += length;
    if (LOG) {
      console.log(`  206 ${pathname} bytes=${start}-${end} (${length} B)`);
    }
    res.writeHead(206, {
      ...ISO,
      'Content-Type': type,
      'Content-Length': length,
      'Content-Range': `bytes ${start}-${end}/${stat.size}`,
    });
    const stream = createReadStream(target, { start, end });
    stream.on('error', () => res.destroy());
    res.on('close', () => stream.destroy());
    stream.pipe(res);
    return;
  }

  fullSends += 1;
  bytesSent += stat.size;
  if (isPack(pathname)) packBytes += stat.size;
  if (LOG) {
    console.log(`  200 ${pathname} (${stat.size} B) — no Range header`);
  }
  res.writeHead(200, { ...ISO, 'Content-Type': type, 'Content-Length': stat.size });
  const stream = createReadStream(target);
  stream.on('error', () => res.destroy());
  res.on('close', () => stream.destroy());
  stream.pipe(res);
}).listen(PORT, '127.0.0.1', () => {
  console.log(`range server on http://127.0.0.1:${PORT}/  root=${ROOT}`);
});

process.on('SIGUSR2', () => {
  console.log(`--- totals: 206=${rangeHits}  200=${fullSends}  bytes=${bytesSent}  packBytes=${packBytes} ---`);
});
process.on('SIGINT', () => {
  console.log(`--- totals: 206=${rangeHits}  200=${fullSends}  bytes=${bytesSent}  packBytes=${packBytes} ---`);
  process.exit(0);
});
