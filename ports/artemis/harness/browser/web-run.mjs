/**
 * Drives the Artemis web host in a real browser and reports what it saw.
 *
 * The engine runs in a worker, so the page's own overlay is the observation channel rather than the
 * page console: the worker's `postMessage` log lines land in `#overlay` through page code, which keeps
 * "what the host said" separate from "what the browser happened to forward". Console entries are still
 * collected, because an exception inside the glue never reaches the overlay at all.
 *
 * Usage: node web-run.mjs [url] [seconds]
 */
import { writeFileSync } from 'node:fs';
import { launchEdge, sleep } from '../../krkr/kirikiroid2/browser-test/cdp.mjs';

const URL_ = process.argv[2] ?? 'http://127.0.0.1:4650/artemis-web.html';
const SECONDS = Number(process.argv[3] ?? 40);

const { cdp, stop } = await launchEdge({ port: 9333, enable: ['Runtime', 'Page', 'Log'] });

const consoleLines = [];
const push = (tag, text) => {
  const line = `[${tag}] ${text}`;
  consoleLines.push(line);
  console.log(line);
};

cdp.on('Runtime.consoleAPICalled', (p) => {
  const text = (p.args ?? []).map((a) => a.value ?? a.description ?? a.type).join(' ');
  push(p.type ?? 'log', text);
});
cdp.on('Runtime.exceptionThrown', (p) => {
  const d = p.exceptionDetails ?? {};
  push('exception', d.exception?.description ?? d.text ?? '(no detail)');
});
cdp.on('Log.entryAdded', (p) => {
  const e = p.entry ?? {};
  push(e.source ?? 'log', e.text ?? '');
});

await cdp.send('Page.navigate', { url: URL_ });

let overlay = '';
let drawable = null;
for (let i = 0; i < SECONDS; i += 1) {
  await sleep(1000);
  const probe = await cdp.send('Runtime.evaluate', {
    expression: `JSON.stringify({
      overlay: document.getElementById('overlay') ? document.getElementById('overlay').textContent : null,
      canvas: (function(){ const c = document.getElementById('canvas'); return c ? c.width + 'x' + c.height : null; })()
    })`,
    returnByValue: true,
  });
  const value = probe?.result?.value;
  if (!value) continue;
  const parsed = JSON.parse(value);
  if (parsed.overlay !== overlay) {
    overlay = parsed.overlay ?? '';
    console.log('---- overlay @' + (i + 1) + 's ----');
    console.log(overlay);
  }
  drawable = parsed.canvas;
}

console.log('\n==== console/exception capture (' + consoleLines.length + ' entries) ====');
const interesting = consoleLines.filter((l) => !/Download the React/.test(l));
for (const line of interesting.slice(0, 60)) console.log(line);
if (interesting.length > 60) console.log('... ' + (interesting.length - 60) + ' more');

console.log('\n==== final overlay ====');
console.log(overlay);
console.log('\ncanvas backing store: ' + drawable);

const shot = await cdp.send('Page.captureScreenshot', { format: 'png' });
writeFileSync(new URL('./artemis-shot.png', import.meta.url), Buffer.from(shot.data, 'base64'));
console.log('screenshot: ports/artemis/harness/browser/artemis-shot.png');

await stop();
process.exit(0);
