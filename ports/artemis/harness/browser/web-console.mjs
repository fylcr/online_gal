/**
 * Launches Chrome, opens the game, and reads the engine's console — including what it already logged.
 *
 * Why this instead of the MCP: the chrome-devtools-mcp tools are not exposed to this session at all
 * (the Host has them cached under `__custom__/chrome-devtools`, but the session's tool list was fixed
 * before that listing succeeded), and the two chrome namespaces that *are* exposed both fail at
 * dispatch. CDP straight to the same browser is the path that works.
 *
 * `Log.enable` is what makes this useful: Chrome replays the entries it has already buffered for the
 * target, so lines emitted before anyone thought to look are still readable. The interesting ones —
 * the `lytween …` resolutions — are emitted during boot and during any animation, not on demand.
 *
 * Usage: node web-console.mjs [seconds] [url]
 */
import { launchBrowser, sleep } from './chrome.mjs';

const SECONDS = Number(process.argv[2] ?? 30);
const GAME = process.argv[3] ?? 'http://127.0.0.1:4650/artemis-web.html';
const PORT = Number(process.env.DSH_CHROME_DEBUG_PORT || 9222);

// Log/Runtime are enabled **through the launcher**, before the navigation, exactly as the working
// checks do. Enabling them afterwards on a freshly opened about:blank target loses the document the
// navigation creates, and the collected stream comes back empty.
const { cdp, stop } = await launchBrowser({ port: PORT, enable: ['Runtime', 'Page', 'Log'] });

const lines = [];
const push = (tag, text) => lines.push(`[${tag}] ${text}`);
cdp.on('Log.entryAdded', (p) => push(p.entry?.source ?? 'log', p.entry?.text ?? ''));
cdp.on('Runtime.consoleAPICalled', (p) =>
  push(p.type ?? 'log', (p.args ?? []).map((a) => a.value ?? a.description ?? a.type).join(' ')));
cdp.on('Runtime.exceptionThrown', (p) =>
  push('exception', p.exceptionDetails?.exception?.description ?? p.exceptionDetails?.text ?? ''));

// Log.enable is what replays the target's buffered history; Runtime.enable adds the console API stream.
await cdp.send('Log.enable');
await cdp.send('Runtime.enable');

async function overlay() {
  try {
    const r = await cdp.send('Runtime.evaluate', {
      expression: `(function(){const o=document.getElementById('overlay');
        return JSON.stringify({href:location.href, text:o?o.textContent:null});})()`,
      returnByValue: true,
    });
    return typeof r?.result?.value === 'string' ? JSON.parse(r.result.value) : null;
  } catch {
    return null;
  }
}

await cdp.send('Page.navigate', { url: GAME });
console.log('navigating to ' + GAME);

let booted = false;
for (let i = 0; i < 120 && !booted; i += 1) {
  await sleep(1000);
  const s = await overlay();
  if (s?.href?.includes('artemis-web.html') && /opened /.test(s.text ?? '')) booted = true;
}
console.log(booted ? 'booted' : 'did not reach "opened" in 120 s');

await sleep(SECONDS * 1000);

const strip = (l) => l.replace(/^\[[^\]]+\]\s*/, '').replace(/^\[artc\]\s*/, '');
const uniq = (arr) => [...new Set(arr)];
const only = (re) => lines.filter((l) => re.test(l));

console.log(`\n==== captured ${lines.length} console lines ====`);

const tweens = only(/lytween /);
console.log(`\n---- lytween (${tweens.length}, deduplicated) ----`);
for (const l of uniq(tweens.map(strip))) console.log(l);

const dropped = uniq(only(/lytween dropped/).map(strip));
if (dropped.length) {
  console.log('\n---- DROPPED tweens (no such layer) ----');
  for (const l of dropped) console.log(l);
}

const bad = uniq(only(/Aborted|exception|threw|assert|not found/i).map(strip));
if (bad.length) {
  console.log('\n---- errors / warnings ----');
  for (const l of bad.slice(0, 25)) console.log(l);
}

console.log('\n---- page overlay ----');
console.log((await overlay())?.text ?? '(unavailable)');

await stop();
process.exit(0);
