/**
 * Clicks into the running Artemis web host and reports what changed.
 *
 * Rendering the title screen proves the compositor works; it does not prove input reaches the engine.
 * The click is dispatched through CDP rather than by calling the host's exported entry point, so the
 * whole path is exercised: page `pointerdown` -> worker message -> `artc_web_pointer` -> Lua.
 *
 * Usage: node web-input.mjs [url] [x] [y]
 */
import { writeFileSync } from 'node:fs';
import { launchEdge, sleep } from '../../krkr/kirikiroid2/browser-test/cdp.mjs';

const URL_ = process.argv[2] ?? 'http://127.0.0.1:4650/artemis-web.html';
const X = Number(process.argv[3] ?? 60);
const Y = Number(process.argv[4] ?? 205);

const { cdp, stop } = await launchEdge({ port: 9334, enable: ['Runtime', 'Page', 'Log'] });

const lines = [];
const push = (tag, text) => {
  const line = `[${tag}] ${text}`;
  lines.push(line);
  console.log(line);
};
cdp.on('Runtime.exceptionThrown', (p) => {
  const d = p.exceptionDetails ?? {};
  push('exception', d.exception?.description ?? d.text ?? '(no detail)');
});
cdp.on('Log.entryAdded', (p) => {
  const e = p.entry ?? {};
  if (!/404 \(expected 206\)/.test(e.text ?? '')) push(e.source ?? 'log', e.text ?? '');
});

async function status() {
  const probe = await cdp.send('Runtime.evaluate', {
    expression: `JSON.stringify({
      overlay: document.getElementById('overlay').textContent,
      canvas: (function(){const c=document.getElementById('canvas');return c?c.width+'x'+c.height:null;})()
    })`,
    returnByValue: true,
  });
  return JSON.parse(probe.result.value);
}

async function shot(name) {
  const s = await cdp.send('Page.captureScreenshot', { format: 'png' });
  writeFileSync(new URL('./' + name, import.meta.url), Buffer.from(s.data, 'base64'));
}

await cdp.send('Page.navigate', { url: URL_ });

// Wait for the boot to finish rather than for a fixed time: the pack index read is the slow part and
// it depends on what the browser has cached between runs.
let before = null;
for (let i = 0; i < 60; i += 1) {
  await sleep(1000);
  const s = await status();
  if (/opened /.test(s.overlay)) {
    before = s;
    break;
  }
}
if (!before) {
  console.log('never reached "opened"; final overlay:');
  console.log((await status()).overlay);
  await stop();
  process.exit(1);
}
console.log('booted. canvas=' + before.canvas);
await shot('artemis-input-before.png');

const probe = await cdp.send('Runtime.evaluate', {
  expression: `JSON.stringify((function(){const c=document.getElementById('canvas');const r=c.getBoundingClientRect();
    return {left:r.left,top:r.top,w:r.width,h:r.height};})())`,
  returnByValue: true,
});
const rect = JSON.parse(probe.result.value);
const cx = rect.left + X * (rect.w / (rect.w || 1));
const cy = rect.top + Y * (rect.h / (rect.h || 1));
console.log(`clicking at css(${cx}, ${cy}) — canvas rect ${rect.w}x${rect.h} at ${rect.left},${rect.top}`);

for (const type of ['mousePressed', 'mouseReleased']) {
  await cdp.send('Input.dispatchMouseEvent', {
    type, x: cx, y: cy, button: 'left', clickCount: 1, buttons: type === 'mousePressed' ? 1 : 0,
  });
  await sleep(60);
}

await sleep(6000);
const after = await status();
console.log('\n==== after click ====');
console.log(after.overlay);
await shot('artemis-input-after.png');

const beforeStats = /frames (\d+)\s+presented (\d+)\s+revision (\d+)/.exec(before.overlay);
const afterStats = /frames (\d+)\s+presented (\d+)\s+revision (\d+)/.exec(after.overlay);
if (beforeStats && afterStats) {
  console.log(`revision ${beforeStats[3]} -> ${afterStats[3]}   presented ${beforeStats[2]} -> ${afterStats[2]}`);
}

console.log('\nscreenshots: artemis-input-before.png / artemis-input-after.png');
await stop();
process.exit(0);
