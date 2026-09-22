/**
 * Drives the built KiriKiri web page in a real browser over the DevTools
 * protocol, so the byte-source seam can be checked where it actually runs.
 *
 * Why CDP and not a browser-automation dependency: nothing in this repository
 * ships Playwright/Puppeteer, `node:crypto`-era Node has a global `WebSocket`,
 * and Edge on this machine is Chromium 153 — comfortably past the JSPI
 * requirement the reference project states (137). So the browser is launched
 * with `--remote-debugging-port` and spoken to directly, with no installs.
 *
 * Two modes, run as a pair:
 *   --control  the stock page: the shell discovers the archive with its own
 *              HTTP HEAD and reads ranges itself.
 *   --inject   the same page with a host byte source installed before any
 *              script runs (`Page.addScriptToEvaluateOnNewDocument`). The source
 *              is a lazy HTTP-Range reader instrumented with counters, so the
 *              run proves the shell asked *it* for the size and the bytes —
 *              and that its own HEAD never happened.
 *
 * Usage:
 *   node kiri-browser-probe.mjs --control|--inject [--seconds 90] [--out file.json]
 */
import { writeFileSync } from 'node:fs';

import { launchEdge, sleep } from './cdp.mjs';

const BASE = process.env.KIRI_BASE ?? 'http://127.0.0.1:4630';
const GAME = process.env.KIRI_GAME ?? 'game.zip';
// The page to open, relative to BASE, and its query string. `KIRI_QUERY=''` opens the
// shell's own landing screen, which is how the *serving* half is checked without a game
// (for the app origin the page lives under its mount path, e.g. `krkr2-web/index.html`).
const PAGE = process.env.KIRI_PAGE ?? 'index.html';
const QUERY = process.env.KIRI_QUERY ?? `?game=${encodeURIComponent(GAME)}`;
const PORT = Number(process.env.KIRI_CDP_PORT ?? 9222);

const argv = process.argv.slice(2);
const mode = argv.includes('--inject') ? 'inject' : argv.includes('--control') ? 'control' : null;
if (mode === null) {
  console.error('usage: kiri-browser-probe.mjs --control|--inject [--seconds N] [--out FILE]');
  process.exit(2);
}
const secondsArg = argv.indexOf('--seconds');
const seconds = secondsArg >= 0 ? Number(argv[secondsArg + 1]) : 90;
const outArg = argv.indexOf('--out');
const outFile = outArg >= 0 ? argv[outArg + 1] : null;

/* ------------------------------------------------------------------ */
/* The injected host byte source                                       */
/* ------------------------------------------------------------------ */
//
// A lazy, range-based reader over the served archive, instrumented so the run
// can evidence itself. `stat` deliberately uses a *Range* request rather than a
// HEAD: that keeps "no HEAD for game.zip" a clean discriminator between the
// stock discovery path and the seam.
const INJECTED = `
(() => {
  const probe = { stat: [], readRange: [], bytesReturned: 0, errors: [] };
  window.__probe = probe;
  const sizeCache = new Map();

  async function readRangeOverHttp(url, offset, length) {
    const res = await fetch(url, { headers: { Range: 'bytes=' + offset + '-' + (offset + length - 1) } });
    if (res.status !== 206 && res.status !== 200) {
      throw new Error('range ' + res.status + ' ' + url);
    }
    const buf = await res.arrayBuffer();
    const bytes = new Uint8Array(buf);
    // A 200 means the server ignored the range and handed back the whole file.
    return bytes.byteLength > length ? bytes.subarray(0, length) : bytes;
  }

  window.__onlineGalVfsSource = {
    async stat(url) {
      probe.stat.push(url);
      if (sizeCache.has(url)) return sizeCache.get(url);
      try {
        const res = await fetch(url, { headers: { Range: 'bytes=0-0' } });
        const contentRange = res.headers.get('Content-Range') || '';
        const match = /^bytes \\d+-\\d+\\/(\\d+)$/.exec(contentRange);
        const total = match ? Number(match[1]) : Number(res.headers.get('Content-Length') || 0);
        if (!(total > 0)) return null;
        const info = { size: total, etag: res.headers.get('ETag') || '' };
        sizeCache.set(url, info);
        return info;
      } catch (error) {
        probe.errors.push(String(error && error.message || error));
        return null;
      }
    },
    async readRange(url, offset, length) {
      probe.readRange.push({ url, offset, length });
      try {
        const bytes = await readRangeOverHttp(url, offset, length);
        probe.bytesReturned += bytes.byteLength;
        return bytes;
      } catch (error) {
        probe.errors.push(String(error && error.message || error));
        throw error;
      }
    },
  };
})();
`;

/* ------------------------------------------------------------------ */
/* Minimal CDP client                                                  */
/* ------------------------------------------------------------------ */

/* ------------------------------------------------------------------ */
/* The browser                                                         */
/* ------------------------------------------------------------------ */
//
// Launch flags and the protocol client live in `cdp.mjs`, shared with the app-flow
// check, so the two cannot drift apart on how the browser is started.

const summary = {
  mode,
  url: `${BASE}/${PAGE}${QUERY}`,
  consoleLines: [],
  exceptions: [],
  logEntries: [],
  requests: [],
  responses: [],
  encodedBytesByUrl: {},
  injected: mode === 'inject',
};

let browser = null;
let cdp;
try {
  browser = await launchEdge({ port: PORT, enable: ['Page', 'Runtime', 'Log', 'Network'] });
  cdp = browser.cdp;

  cdp.on('Runtime.consoleAPICalled', (p) => {
    const text = p.args.map((a) => (a.value !== undefined ? String(a.value) : a.description ?? `[${a.type}]`)).join(' ');
    summary.consoleLines.push({ type: p.type, text });
  });
  cdp.on('Runtime.exceptionThrown', (p) => {
    summary.exceptions.push(p.exceptionDetails?.exception?.description ?? p.exceptionDetails?.text ?? 'unknown');
  });
  cdp.on('Log.entryAdded', (p) => {
    summary.logEntries.push({ level: p.entry.level, text: p.entry.text });
  });
  cdp.on('Network.requestWillBeSent', (p) => {
    summary.requests.push({
      requestId: p.requestId,
      method: p.request.method,
      url: p.request.url,
      range: p.request.headers?.Range ?? null,
      timestamp: p.timestamp,
    });
  });
  cdp.on('Network.responseReceived', (p) => {
    summary.responses.push({ url: p.response.url, status: p.response.status, mimeType: p.response.mimeType, fromDiskCache: p.response.fromDiskCache });
  });
  cdp.on('Network.loadingFinished', (p) => {
    const request = summary.requests.find((r) => r.requestId === p.requestId);
    const url = request?.url ?? `#${p.requestId}`;
    summary.encodedBytesByUrl[url] = (summary.encodedBytesByUrl[url] ?? 0) + (p.encodedDataLength ?? 0);
  });

  if (mode === 'inject') {
    await cdp.send('Page.addScriptToEvaluateOnNewDocument', { source: INJECTED });
  }

  await cdp.send('Page.navigate', { url: summary.url });

  // Let the page boot: instantiate the 22 MB wasm, mount the archive, run KAG.
  const deadline = Date.now() + seconds * 1000;
  while (Date.now() < deadline) {
    await sleep(1000);
    try {
      const state = await cdp.send('Runtime.evaluate', {
        expression: '({ isolated: self.crossOriginIsolated === true, hasVlfs: typeof window.VLFS !== "undefined", seam: typeof window.__onlineGalVfsSource !== "undefined" })',
        returnByValue: true,
      });
      const value = state.result?.value;
      if (value?.hasVlfs) summary.pageStateEarly = value;
    } catch { /* navigating */ }
  }

  const final = await cdp.send('Runtime.evaluate', {
    expression: `(() => {
      const probe = window.__probe || null;
      const canvas = document.querySelector('canvas');
      const status = document.querySelector('#status') || null;
      return {
        crossOriginIsolated: self.crossOriginIsolated === true,
        hasVlfs: typeof window.VLFS !== 'undefined',
        seamInstalled: typeof window.__onlineGalVfsSource !== 'undefined',
        probe: probe ? {
          statCalls: probe.stat.length,
          statUrls: probe.stat,
          readRangeCalls: probe.readRange.length,
          sampleReads: probe.readRange.slice(0, 8),
          bytesReturned: probe.bytesReturned,
          errors: probe.errors.slice(0, 5),
        } : null,
        canvas: canvas ? { width: canvas.width, height: canvas.height } : null,
        statusText: status ? (status.textContent || '').slice(0, 200) : null,
        title: document.title,
        bodyText: (document.body ? document.body.innerText : '').slice(0, 300),
      };
    })()`,
    returnByValue: true,
  });
  summary.final = final.result?.value ?? null;
} catch (error) {
  summary.fatal = String(error?.stack ?? error);
} finally {
  await browser?.stop();
}

/* ------------------------------------------------------------------ */
/* Derive the headline numbers                                         */
/* ------------------------------------------------------------------ */

const gameUrl = `${BASE}/${GAME}`;
// Exact match, not `endsWith`: the navigation URL is
// `.../index.html?game=game.zip`, which also ends with the archive name and would
// otherwise be counted as an unconditional archive GET.
const gameRequests = summary.requests.filter((r) => r.url === gameUrl);
summary.derived = {
  gameHeadRequests: gameRequests.filter((r) => r.method === 'HEAD').length,
  gameRangeRequests: gameRequests.filter((r) => r.range !== null).length,
  gameUnconditionalGets: gameRequests.filter((r) => r.method === 'GET' && r.range === null).length,
  gameBytesTransferred: summary.encodedBytesByUrl[gameUrl] ?? 0,
  seamBytesReturned: summary.final?.probe?.bytesReturned ?? 0,
  seamStatCalls: summary.final?.probe?.statCalls ?? 0,
  seamReadRangeCalls: summary.final?.probe?.readRangeCalls ?? 0,
  wasmBytesTransferred: summary.encodedBytesByUrl[`${BASE}/index.wasm`] ?? 0,
  hostSeamLogSeen: summary.consoleLines.some((l) => l.text.includes('host byte-source')),
  errors: summary.consoleLines.filter((l) => l.type === 'error').map((l) => l.text.slice(0, 200)),
  exceptionCount: summary.exceptions.length,
};

console.log(JSON.stringify(summary, null, 2));
if (outFile) writeFileSync(outFile, JSON.stringify(summary, null, 2));
process.exit(0);
