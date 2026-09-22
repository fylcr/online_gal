/**
 * Runs the real XP3 game through **chrome-devtools-mcp**, rather than a hand-rolled CDP client.
 *
 * Why this exists alongside `kiri-browser-probe.mjs` and `kiri-shot.mjs`: those two speak the
 * DevTools protocol directly, which is fine for instrumenting a page but means every browser
 * facility has to be re-implemented (launching, screenshotting, console capture). The MCP server
 * already does all of it and is the channel this project's other browser checks use, so this script
 * re-checks the real-XP3 run through it — an independent route to the same conclusion.
 *
 * The browser is the MCP's own Chrome (launcher at `~/.dsh/chrome-devtools-mcp/launch.mjs`, which
 * brings up Chrome on port 9222 lazily on the first tool call). It therefore starts from a **fresh
 * profile**, so the archive must come over the network and the byte count means something — no OPFS
 * spill from an earlier run can be mistaken for on-demand loading.
 *
 * Usage:
 *   KIRI_QUERY='?xp3=/data.xp3' node mcp-real-xp3.mjs \
 *     --url http://127.0.0.1:4630/index.html?xp3=/data.xp3 \
 *     --archive-bytes 2312777571 \
 *     --seconds 90 --out /tmp/mcp-xp3.json --screenshot /tmp/mcp-xp3.png
 */
import { spawn } from 'node:child_process';
import { writeFileSync } from 'node:fs';
import { repoRoot, repoRootUrl } from '../../lib/ref.mjs';

const LAUNCHER = 'C:\\Users\\ASUS\\.dsh\\chrome-devtools-mcp\\launch.mjs';
/** Declared so the server may read files under the checkout; `upload_file` needs it. */
const WORKSPACE_ROOT_URI = repoRootUrl;

const argv = process.argv.slice(2);
const argOf = (name, fallback) => {
  const at = argv.indexOf(name);
  return at >= 0 && argv[at + 1] ? argv[at + 1] : fallback;
};

const URL_ = argOf('--url', `${process.env.KIRI_BASE ?? 'http://127.0.0.1:4630'}/index.html${process.env.KIRI_QUERY ?? '?xp3=/data.xp3'}`);
const ARCHIVE_BYTES = Number(argOf('--archive-bytes', '2312777571'));
const SECONDS = Number(argOf('--seconds', '90'));
const OUT = argOf('--out', null);
const SHOT = argOf('--screenshot', null);

const summary = { url: URL_, archiveBytes: ARCHIVE_BYTES, calls: [] };
const record = (name, text) => summary.calls.push({ name, text: String(text ?? '').slice(0, 300) });

/* ------------------------------------------------------------------ */
/* MCP client (stdio JSON-RPC)                                          */
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
      // Server → client requests must be answered, not queued: `roots/list` is how the server learns
      // which local paths it may read, and it denies every path when no roots are declared.
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
function send(method, params, timeoutMs = 240_000) {
  return new Promise((resolve, reject) => {
    const id = nextId++;
    pending.set(id, resolve);
    child.stdin.write(`${JSON.stringify({ jsonrpc: '2.0', id, method, params })}\n`);
    setTimeout(() => reject(new Error(`timeout after ${timeoutMs} ms: ${method}`)), timeoutMs);
  });
}

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
    // `async`, so a body may await a promise; the tool awaits the returned promise.
    function: `async () => { ${body} }`,
  });
  const match = /```json\s*([\s\S]*?)```/.exec(text) ?? /([{[][\s\S]*[}\]])/.exec(text);
  if (!match) return null;
  try {
    let value = JSON.parse(match[1]);
    // The result arrives **double-encoded**: the tool wraps the returned value in a fenced block, and
    // because the page function returns `JSON.stringify(...)`, the fenced body is a JSON *string*
    // whose contents are the JSON object. Parsing once yields a string, so `value.archiveBytes` would
    // be `undefined` and the report would print `?` / `0` — which reads as "the page read nothing",
    // the exact opposite of the truth. Unwrap the second layer when present.
    if (typeof value === 'string') {
      try {
        value = JSON.parse(value);
      } catch {
        /* genuinely a string; leave it */
      }
    }
    return value;
  } catch {
    return null;
  }
}

const sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

/* ------------------------------------------------------------------ */
/* The run                                                              */
/* ------------------------------------------------------------------ */

let pageId = null;
try {
  await send('initialize', {
    protocolVersion: '2024-11-05',
    capabilities: { roots: { listChanged: false } },
    clientInfo: { name: 'kiri-mcp-real-xp3', version: '1.0.0' },
  });
  child.stdin.write(`${JSON.stringify({ jsonrpc: '2.0', method: 'notifications/initialized' })}\n`);

  const tools = await send('tools/list', {});
  summary.toolCount = tools.result?.tools?.length ?? 0;

  // Bring up a page at the game URL — but **which page we then talk to is decided from
  // `list_pages`**, never from the prose `new_page` returns.
  //
  // Two traps here, and both present as "the engine did not start" rather than as a client bug:
  //
  //   * The id parsed out of `new_page`'s prose can belong to a pre-existing tab. On a cold MCP
  //     start that is the browser's own new-tab page, and the run then reports
  //     `title: 新标签页`, `isolated: false`, `canvas: none`.
  //   * `new_page(url)` followed by a `navigate_page` can leave **two** tabs at the same URL, and
  //     only the selected one is the engine's (`title: krkr2`); the other still carries the shell's
  //     long HTML title because it never booted.
  await callTool('new_page', { url: URL_ });

  const pickPage = async () => {
    const text = await callTool('list_pages', {});
    const entries = text
      .split('\n')
      .map((line) => /^(\d+):\s*(.*?)\s*\((http[^)]*)\)\s*(\[selected\])?/.exec(line.trim()))
      .filter(Boolean)
      .map((m) => ({ id: Number(m[1]), title: m[2], url: m[3], selected: Boolean(m[4]) }));
    summary.pages = entries;
    return (
      entries.find((e) => e.selected && e.url.includes('4630')) ??
      entries.find((e) => e.url.includes('4630') && e.title === 'krkr2') ??
      entries.find((e) => e.url.includes('4630')) ??
      null
    );
  };

  let target = null;
  for (let attempt = 0; attempt < 20 && target === null; attempt += 1) {
    target = await pickPage();
    if (target === null) await sleep(1000);
  }
  if (target === null) throw new Error(`no page at the game URL; pages=${JSON.stringify(summary.pages)}`);
  pageId = target.id;
  summary.pageId = pageId;
  summary.onGame = true;

  /**
   * Re-resolve the page id before a page-scoped call.
   *
   * A page id is not stable across a long run: after the engine boots, the tab can be replaced and a
   * later call against the old id fails with `Error: No page found` — which reads like the emulator
   * died, when in fact only the handle went stale. Re-resolving is one cheap `list_pages` call and
   * removes that whole failure mode.
   */
  const refreshPage = async () => {
    const found = await pickPage();
    if (found) pageId = found.id;
    return pageId;
  };

  // The engine reads a real 2.3 GB archive and boots a TJS script; wait for it rather than guessing.
  await sleep(SECONDS * 1000);

  // Re-resolve before every page-scoped call: the handle can go stale while the engine boots.
  await refreshPage();
  summary.pageIdAtRead = pageId;

  // What the engine produced, read through an MCP tool that evaluates in the page.
  summary.page = await evaluate(
    pageId,
    `const c = document.querySelector('canvas');
     return JSON.stringify({
       title: document.title,
       isolated: self.crossOriginIsolated === true,
       canvas: c ? c.width + 'x' + c.height : 'none',
       status: (document.body.innerText || '').replace(/\\s+/g, ' ').slice(0, 80),
     });`,
  );

  // Byte accounting, same-origin archive traffic only.
  //
  // The path check matters: an earlier attempt filtered on the URL merely *containing* the archive
  // name and accidentally matched Google Analytics beacons, whose query string embeds the page URL.
  //
  // Every URL is parsed inside a `try`: one malformed resource name would otherwise throw out of the
  // whole function, `evaluate_script` would return an error string, and the measurement would come
  // back `null` — indistinguishable from "the page read nothing", which is the opposite conclusion.
  await refreshPage();
  summary.traffic = await evaluate(
    pageId,
    `const all = performance.getEntriesByType('resource');
     let es = [];
     for (const e of all) {
       try {
         const u = new URL(e.name);
         if (u.origin === location.origin && u.pathname === '/data.xp3') es.push(e);
       } catch {}
     }
     const bytes = es.reduce((a, e) => a + (e.encodedBodySize || e.transferSize || 0), 0);
     return JSON.stringify({
       archiveRequests: es.length,
       archiveBytes: bytes,
       firstSizes: es.slice(0, 6).map(e => e.encodedBodySize || e.transferSize || 0),
       wholeFileResponses: es.filter(e => (e.encodedBodySize || 0) === ${ARCHIVE_BYTES}).length,
     });`,
  );

  // Cross-check with the MCP's own network tool rather than trusting the page's own accounting.
  summary.networkListing = (
    await callTool('list_network_requests', { pageId, pageSize: 400, resourceTypes: ['xhr', 'fetch', 'other'] })
  )
    .split('\n')
    .filter((line) => line.includes('data.xp3')).length;

  await refreshPage();
  const consoleText = await callTool('list_console_messages', { pageId, pageSize: 60, types: ['error', 'warn'] });
  summary.consoleErrors = consoleText
    .split('\n')
    .filter((line) => line.trim())
    .slice(0, 8);

  if (SHOT) {
    await refreshPage();
    await callTool('take_screenshot', { pageId, format: 'png', filePath: SHOT });
    summary.screenshot = SHOT;
  }
} finally {
  try {
    if (pageId) await callTool('close_page', { pageId });
  } catch {
    /* already gone */
  }
  child.stdin.end();
  await sleep(600);
  child.kill('SIGKILL');
}

/* ------------------------------------------------------------------ */
/* Report                                                              */
/* ------------------------------------------------------------------ */

const t = summary.traffic ?? {};
const read = t.archiveBytes ?? 0;
console.log(`url:            ${summary.url}`);
console.log(`tools:          ${summary.toolCount}   pageId: ${summary.pageId}`);
console.log(`page:           ${JSON.stringify(summary.page)}`);
console.log(`archive reqs:   ${t.archiveRequests ?? '?'}`);
console.log(`archive bytes:  ${read.toLocaleString()}  (${((read / ARCHIVE_BYTES) * 100).toFixed(2)}% of ${ARCHIVE_BYTES.toLocaleString()})`);
console.log(`first sizes:    ${JSON.stringify(t.firstSizes ?? [])}`);
console.log(`whole-file resp:${t.wholeFileResponses ?? '?'}   mcp network lines: ${summary.networkListing ?? '?'}`);
console.log(`console errs:   ${summary.consoleErrors?.length ? summary.consoleErrors.join(' | ') : '(none)'}`);
if (summary.screenshot) console.log(`screenshot:     ${summary.screenshot}`);
if (OUT) {
  writeFileSync(OUT, JSON.stringify(summary, null, 2));
  console.log(`json:           ${OUT}`);
}
