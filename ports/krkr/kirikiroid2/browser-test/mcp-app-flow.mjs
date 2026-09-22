/**
 * The browser evaluation, run through this machine's `chrome-devtools-mcp` instead of a
 * hand-rolled CDP client.
 *
 * Why this exists next to `kiri-app-flow.mjs`: that harness speaks the DevTools protocol
 * itself and drives Edge; this one drives the MCP server the DSH connector panel exposes
 * (`~/.dsh/chrome-devtools-mcp`, 29 tools, real Chrome 151 on port 9222). Two independent
 * control channels agreeing on the same result is a stronger statement than either alone,
 * and it exercises the MCP setup the user actually has.
 *
 * The server is lazy — it does not start Chrome until the first `tools/call` — so the
 * process can be inspected (`tools/list`) without a browser window appearing.
 *
 * Usage:
 *   node mcp-app-flow.mjs [--base URL] [--game FILE] [--out FILE] [--screenshot FILE]
 */
import { spawn } from 'node:child_process';
import { writeFileSync } from 'node:fs';
import { repoRoot, repoRootUrl } from '../../lib/ref.mjs';

const LAUNCHER = 'C:\\Users\\ASUS\\.dsh\\chrome-devtools-mcp\\launch.mjs';
const argv = process.argv.slice(2);
const argOf = (flag, fallback) => {
  const index = argv.indexOf(flag);
  return index >= 0 && argv[index + 1] ? argv[index + 1] : fallback;
};

const BASE = argOf('--base', process.env.KIRI_APP_BASE ?? 'http://127.0.0.1:4321');
const GAME = argOf('--game', `${repoRoot}/ports/krkr/kirikiroid2/browser-test/game.zip`);
const OUT = argOf('--out', null);
const SCREENSHOT = argOf('--screenshot', null);
const ARCHIVE_BYTES = 14_247_494;
const KEY = GAME.split('/').pop();
/** Declared to the server so it may read files under the checkout (`upload_file` needs this). */
const WORKSPACE_ROOT_URI = repoRootUrl;

const summary = { base: BASE, game: GAME, calls: [], tools: 0 };
const record = (name, text) => {
  summary.calls.push({ name, text: String(text ?? '').slice(0, 400) });
};

/* ------------------------------------------------------------------ */
/* MCP client                                                          */
/* ------------------------------------------------------------------ */

const child = spawn(process.execPath, [LAUNCHER], { stdio: ['pipe', 'pipe', 'pipe'] });
let stderr = '';
child.stderr.on('data', (chunk) => {
  stderr += String(chunk);
});

let buffer = '';
const pending = new Map();
child.stdout.on('data', (chunk) => {
  buffer += chunk.toString();
  let index;
  while ((index = buffer.indexOf('\n')) >= 0) {
    const line = buffer.slice(0, index).trim();
    buffer = buffer.slice(index + 1);
    if (!line) continue;
    try {
      const message = JSON.parse(line);
      // Server → client *requests* must be answered, not queued: `roots/list` is how
      // chrome-devtools-mcp learns which local paths `upload_file` may read, and it denies
      // every path when the client declares no roots.
      if (message.method === 'roots/list' && message.id !== undefined) {
        child.stdin.write(
          `${JSON.stringify({
            jsonrpc: '2.0',
            id: message.id,
            result: { roots: [{ uri: WORKSPACE_ROOT_URI, name: repoRoot.split(/[\/]/).pop() ?? 'workspace' }] },
          })}\n`,
        );
        continue;
      }
      const resolve = pending.get(message.id);
      if (resolve) {
        pending.delete(message.id);
        resolve(message);
      }
    } catch {
      /* not a JSON-RPC line */
    }
  }
});

let nextId = 1;
function send(method, params, timeoutMs = 180_000) {
  return new Promise((resolve, reject) => {
    const id = nextId++;
    pending.set(id, resolve);
    child.stdin.write(`${JSON.stringify({ jsonrpc: '2.0', id, method, params })}\n`);
    setTimeout(() => reject(new Error(`timeout after ${timeoutMs} ms: ${method}`)), timeoutMs);
  });
}

/** Calls one tool and returns its text content, which is all chrome-devtools-mcp returns. */
async function callTool(name, args = {}) {
  const response = await send('tools/call', { name, arguments: args });
  const text = response.result?.content?.map((part) => part.text ?? '').join('\n') ?? '';
  record(name, text || JSON.stringify(response.error ?? response.result));
  if (response.error) throw new Error(`${name}: ${response.error.message}`);
  return text;
}

/** Evaluates a function in the page and parses the JSON it returns. */
async function evaluate(pageId, body) {
  const text = await callTool('evaluate_script', {
    pageId,
    // `async`, so a body may await a promise (IndexedDB reads do); the tool awaits the
    // returned promise before reporting.
    function: `async () => { ${body} }`,
  });
  const match = /```json\s*([\s\S]*?)```/.exec(text) ?? /([{[][\s\S]*[}\]])/.exec(text);
  if (!match) return null;
  try {
    return JSON.parse(match[1]);
  } catch {
    return null;
  }
}

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

async function poll(label, fn, timeoutMs, intervalMs = 1500) {
  const deadline = Date.now() + timeoutMs;
  let last;
  while (Date.now() < deadline) {
    try {
      last = await fn();
      if (last) return last;
    } catch (error) {
      last = `failed: ${String(error).slice(0, 160)}`;
    }
    await sleep(intervalMs);
  }
  throw new Error(`timed out waiting for ${label}; last: ${JSON.stringify(last)?.slice(0, 240)}`);
}

/**
 * Installed before the player page's own scripts: it intercepts the platform's assignment of
 * the host byte source and wraps `readRange`, so the run can report how much of the archive
 * the engine asked for. A plain property assignment cannot be hooked after the fact.
 */
const MEASURE_INIT_SCRIPT = `
(() => {
  const KEY = '__onlineGalVfsSource';
  let current = null;
  Object.defineProperty(window, KEY, {
    configurable: true,
    get() { return current; },
    set(value) {
      if (value && typeof value.readRange === 'function' && !value.__measured) {
        const originalReadRange = value.readRange.bind(value);
        const originalStat = value.stat ? value.stat.bind(value) : null;
        const stats = { readRangeCalls: 0, bytesReturned: 0, statCalls: 0 };
        value.readRange = async (url, offset, length) => {
          const bytes = await originalReadRange(url, offset, length);
          stats.readRangeCalls += 1;
          stats.bytesReturned += bytes.byteLength;
          return bytes;
        };
        if (originalStat) {
          value.stat = async (url) => { stats.statCalls += 1; return originalStat(url); };
        }
        value.__measured = stats;
      }
      current = value;
    },
  });
})();
`;

/* ------------------------------------------------------------------ */
/* The evaluation                                                      */
/* ------------------------------------------------------------------ */

try {
  const init = await send('initialize', {
    protocolVersion: '2024-11-05',
    capabilities: { roots: { listChanged: false } },
    clientInfo: { name: 'kiri-mcp-eval', version: '1.0' },
  });
  summary.server = init.result?.serverInfo ?? null;
  child.stdin.write(`${JSON.stringify({ jsonrpc: '2.0', method: 'notifications/initialized' })}\n`);

  const list = await send('tools/list', {});
  summary.tools = (list.result?.tools ?? []).length;
  if (summary.tools !== 29) throw new Error(`expected 29 tools, got ${summary.tools}`);

  /* 1. the library page, in a fresh page of the MCP's Chrome */
  //
  // Leftovers are closed first: this browser outlives the harness (the launcher starts it
  // detached), so every earlier run leaves a page with a running wasm engine behind, and a
  // pile of those starves the next run's compilation — observed as the shell sitting at
  // "Preparing engine... (3 remaining)" until the poll gave up.
  const existing = await callTool('list_pages');
  const existingIds = [...existing.matchAll(/^\s*(\d+):/gm)].map((m) => Number(m[1]));
  summary.closedPages = existingIds;
  for (const id of existingIds) {
    try {
      await callTool('close_page', { pageId: id });
    } catch {
      // A page that is already gone is not a problem.
    }
  }

  // …and then a fresh, isolated storage context, which is the part that matters: the shell
  // allows **one engine per site**, enforced through shared storage, so a page opened in this
  // long-lived profile is refused with "Engine Already Running" whenever an earlier run's page
  // still holds it (observed: the frame mounts, the archive is even parsed through the seam,
  // and the engine is then refused). An isolated context has its own storage, so the run
  // starts clean the way the Edge harness does with a throwaway profile.
  await callTool('new_page', {
    url: `${BASE}/`,
    isolatedContext: `kiri-eval-${Date.now()}`,
    timeout: 60_000,
  });
  const pagesText = await callTool('list_pages');
  // The MCP opens the page as a *new* entry alongside whatever was already open, and its
  // tools take `pageId` as a number — a string is rejected by input validation.
  const pageLine =
    pagesText.split('\n').find((line) => line.includes('[selected]')) ??
    pagesText.split('\n').find((line) => /^\d+:/.test(line.trim()));
  const pageId = Number(/(\d+):/.exec(pageLine ?? '')?.[1] ?? Number.NaN);
  summary.pageId = pageId;
  summary.pageLine = pageLine?.trim() ?? null;
  if (!Number.isFinite(pageId)) {
    throw new Error(`no page id in list_pages output: ${pagesText.slice(0, 200)}`);
  }

  /* 2. import the archive through the app's own archive input */
  //
  // This Chrome keeps its profile, unlike the Edge harness that uses a throwaway one, so the
  // KiriKiri shell's service worker survives between runs and controls the page from the
  // second visit on. That is recorded, then cleared: a controlled page can be served a stale
  // `index.html` (missing the host-source patch) out of the SW cache, which stalls the engine
  // at "Preparing engine...".
  summary.serviceWorker = await evaluate(
    pageId,
    `const controller = navigator.serviceWorker.controller
       ? navigator.serviceWorker.controller.scriptURL
       : null;
     const registrations = navigator.serviceWorker.getRegistrations
       ? (await navigator.serviceWorker.getRegistrations()).map((r) => r.scope)
       : [];
     const keys = typeof caches !== 'undefined' ? await caches.keys() : [];
     return { controller, registrations, cacheKeys: keys };`,
  );
  summary.serviceWorkerCleared = await evaluate(
    pageId,
    `const registrations = navigator.serviceWorker.getRegistrations
       ? await navigator.serviceWorker.getRegistrations()
       : [];
     for (const registration of registrations) await registration.unregister();
     const keys = typeof caches !== 'undefined' ? await caches.keys() : [];
     await Promise.all(keys.map((key) => caches.delete(key)));
     return { unregistered: registrations.length, clearedCaches: keys };`,
  );

  const snapshot = await callTool('take_snapshot', { pageId });
  // A `<input type=file>` surfaces in the accessibility tree as a **button** carrying a
  // `value` (the chosen-file label), not as an `input` — which is worth knowing, since
  // matching on the tag name finds nothing at all. This page has exactly two: the folder
  // input first, then the archive input, so the last one is the archive.
  const fileInputUids = [...snapshot.matchAll(/uid=(\S+)\s+button[^\n]*\bvalue="/g)].map((m) => m[1]);
  summary.fileInputUids = fileInputUids;
  if (fileInputUids.length === 0) {
    throw new Error(`no file-input buttons in the snapshot:\n${snapshot.slice(0, 600)}`);
  }
  const archiveUid = fileInputUids[fileInputUids.length - 1];
  await callTool('upload_file', { pageId, uid: archiveUid, filePaths: [GAME] });

  const playHref = await poll(
    'the imported game card',
    () =>
      evaluate(
        pageId,
        `const link = document.querySelector('a[href^="/play?id="]'); return link ? link.getAttribute('href') : null;`,
      ),
    240_000,
  );
  summary.playUrl = `${BASE}${playHref}`;

  summary.record = await evaluate(
    pageId,
    `const records = await new Promise((resolve) => {
       const request = indexedDB.open('online-gal');
       request.onsuccess = () => {
         const store = request.result.transaction('games', 'readonly').objectStore('games');
         const all = store.getAll();
         all.onsuccess = () => resolve(all.result.map((r) => ({
           name: r.name,
           runtimeId: r.detection && r.detection.runtimeId,
           score: r.detection && r.detection.score,
         })));
         all.onerror = () => resolve([]);
       };
       request.onerror = () => resolve([]);
     });
     return records;`,
  );

  /* 3. launch it, with the seam hook installed before the page's own scripts */
  await callTool('navigate_page', {
    pageId,
    type: 'url',
    url: summary.playUrl,
    initScript: MEASURE_INIT_SCRIPT,
    timeout: 120_000,
  });

  summary.frame = await poll(
    'the frame and the seam',
    async () => {
      const value = await evaluate(
        pageId,
        `const frame = document.querySelector('iframe');
         const win = frame && frame.contentWindow;
         const doc = win && win.document;
         if (!frame || !win || !doc) return null;
         const seam = typeof win.__onlineGalVfsSource !== 'undefined';
         const status = doc.querySelector('#status');
         const text = status && status.textContent ? status.textContent.trim() : '';
         const canvas = doc.querySelector('canvas');
         return {
           src: frame.src,
           seamInstalled: seam,
           status: text,
           title: doc.title,
           canvas: canvas ? { width: canvas.width, height: canvas.height } : null,
         };`,
      );
      if (!value || !value.seamInstalled) return null;
      // Wait for the engine to actually be running, not merely past the boot text: the
      // archive is read after the shell starts probing, so measuring earlier reports zero.
      if (!value.status || !/running/i.test(value.status)) return null;
      return value;
    },
    300_000,
    3000,
  );

  await sleep(4000);
  summary.seamMeasurement = await evaluate(
    pageId,
    `const stats = window.__onlineGalVfsSource && window.__onlineGalVfsSource.__measured;
     return stats || null;`,
  );

  /* 4. the evidence: network, console, screenshot */
  const network = await callTool('list_network_requests', { pageId, pageSize: 200 });
  const archiveLines = network
    .split('\n')
    .filter((line) => line.includes(`/${KEY}`) && !line.includes('index.html'));
  summary.network = {
    archiveRequests: archiveLines.length,
    archiveLines: archiveLines.slice(0, 6).map((line) => line.trim().slice(0, 160)),
  };

  const consoleText = await callTool('list_console_messages', {
    pageId,
    pageSize: 200,
    includeStackTraces: false,
  });
  summary.console = {
    hostSeamLine: /host byte-source[^\n]*/.exec(consoleText)?.[0]?.trim() ?? null,
    errors: consoleText
      .split('\n')
      .filter((line) => /\[error\]/i.test(line))
      .slice(0, 5)
      .map((line) => line.trim().slice(0, 200)),
  };

  if (SCREENSHOT) {
    await callTool('take_screenshot', { pageId, format: 'png', filePath: SCREENSHOT });
    summary.screenshot = SCREENSHOT;
  }

  /* 5. input: click the frame's canvas through the MCP's own click tool when it is in the
        snapshot, otherwise say so rather than claiming an input result */
  const playSnapshot = await callTool('take_snapshot', { pageId });
  const canvasUid = /uid=(\S+)[^\n]*canvas/i.exec(playSnapshot)?.[1] ?? null;
  summary.input = { canvasUidInSnapshot: canvasUid, clicked: false };
  if (canvasUid) {
    await callTool('click', { pageId, uid: canvasUid });
    await sleep(4000);
    summary.input.clicked = true;
    if (SCREENSHOT) {
      await callTool('take_screenshot', {
        pageId,
        format: 'png',
        filePath: SCREENSHOT.replace(/\.png$/, '-after-click.png'),
      });
    }
  }
} catch (error) {
  summary.fatal = String(error?.stack ?? error);
} finally {
  child.kill();
}

summary.derived = {
  detectedEngine: summary.record?.[0]?.runtimeId ?? null,
  frameMounted: Boolean(summary.frame?.src?.includes('/krkr2-web/index.html')),
  seamInstalled: summary.frame?.seamInstalled ?? false,
  engineStatus: summary.frame?.status ?? null,
  archiveHttpRequests: summary.network?.archiveRequests ?? null,
  seamCalls: summary.seamMeasurement?.readRangeCalls ?? null,
  seamBytes: summary.seamMeasurement?.bytesReturned ?? null,
  seamPercentOfArchive:
    summary.seamMeasurement?.bytesReturned != null
      ? Number(((summary.seamMeasurement.bytesReturned / ARCHIVE_BYTES) * 100).toFixed(2))
      : null,
  consoleErrors: summary.console?.errors?.length ?? null,
};

console.log(JSON.stringify(summary, null, 2));
if (OUT) writeFileSync(OUT, JSON.stringify(summary, null, 2));
if (summary.fatal) console.error('\nfatal:', summary.fatal);
process.exit(summary.fatal ? 1 : 0);
