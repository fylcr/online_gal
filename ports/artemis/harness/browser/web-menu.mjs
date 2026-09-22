/**
 * Walks the Artemis title menu and reports the first crash.
 *
 * The reported abort happened right after clicking START, loading a save or opening a menu — and those
 * are exactly the paths a boot check never touches. `LOAD` in particular reads save data that may not
 * exist, and `CONFIG` pulls the settings screen, so they exercise more of the engine than the title
 * screen does.
 *
 * Stage coordinates come from the letterbox: the host fits 1280x720 into the canvas and centres it, so
 * a menu row's screen y is `vy + stage_y * scale`. The rows are 46 px apart in screen space at this
 * canvas size, which is what the click list encodes.
 *
 * Usage: node web-menu.mjs [url]
 */
import { writeFileSync } from 'node:fs';
// Real Chrome, not headless Edge: real GPU and a real window size. See chrome.mjs for why.
import { launchBrowser as launchEdge, sleep } from './chrome.mjs';

const URL_ = process.argv[2] ?? 'http://127.0.0.1:4650/artemis-web.html';
const { cdp, stop } = await launchEdge({ port: 9222, enable: ['Runtime', 'Page', 'Log'] });

const lines = [];
const crash = [];
const watch = (t) => { if (/Aborted|threw:|frame failed|RuntimeError|unreachable|out of bounds|assert|terminating/i.test(t)) crash.push(t); };
const note = (t) => { lines.push(t); watch(t); };

cdp.on('Runtime.consoleAPICalled', (p) => note('[' + p.type + '] ' + (p.args ?? []).map((a) => a.value ?? a.description ?? a.type).join(' ')));
cdp.on('Runtime.exceptionThrown', (p) => {
  const d = p.exceptionDetails ?? {};
  note('[exception] ' + (d.exception?.description ?? d.text ?? '(no detail)'));
});
cdp.on('Log.entryAdded', (p) => {
  const e = p.entry ?? {};
  if (/404 \(expected 206\)/.test(e.text ?? '')) return;
  note('[' + (e.source ?? 'log') + '] ' + (e.text ?? ''));
});

async function evalJson(expression) {
  try {
    const r = await cdp.send('Runtime.evaluate', { expression, returnByValue: true });
    return typeof r?.result?.value === 'string' ? JSON.parse(r.result.value) : null;
  } catch { return null; }
}
const state = () => evalJson(`JSON.stringify({href:location.href,
  overlay: document.getElementById('overlay') ? document.getElementById('overlay').textContent : null})`);

async function shot(name) {
  const s = await cdp.send('Page.captureScreenshot', { format: 'png' }).catch(() => null);
  if (s) writeFileSync(new URL('./' + name, import.meta.url), Buffer.from(s.data, 'base64'));
}

try {
  await cdp.send('Page.navigate', { url: URL_ });
  let booted = false;
  for (let i = 0; i < 120 && !booted; i += 1) {
    await sleep(1000);
    const s = await state();
    if (s?.href?.includes('artemis-web.html') && /opened /.test(s.overlay ?? '')) booted = true;
    if (crash.length) break;
  }
  if (!booted) { console.log('never booted'); process.exitCode = 1; throw new Error('no boot'); }
  console.log('booted, canvas=' + (await evalJson(`JSON.stringify({c:(function(){const c=document.getElementById('canvas');return c?c.width+'x'+c.height:null;})()})`)).c);

  async function click(x, y) {
    for (const type of ['mousePressed', 'mouseReleased']) {
      await cdp.send('Input.dispatchMouseEvent', { type, x, y, button: 'left', clickCount: 1, buttons: type === 'mousePressed' ? 1 : 0 });
      await sleep(60);
    }
  }
  async function key(k) {
    for (const type of ['keyDown', 'keyUp']) {
      await cdp.send('Input.dispatchKeyEvent', { type, key: k, code: k, windowsVirtualKeyCode: k === 'Escape' ? 27 : 0, nativeVirtualKeyCode: k === 'Escape' ? 27 : 0 });
      await sleep(60);
    }
  }

  // Title rows, in the order they appear on screen. EXIT is deliberately absent: it ends the session
  // rather than testing it.
  const rows = [
    ['LOAD', 299],
    ['CONFIG', 345],
    ['SPECIAL', 391],
  ];

  for (const [label, y] of rows) {
    if (crash.length) break;
    const before = lines.length;
    console.log(`\n---- clicking ${label} at (60, ${y}) ----`);
    await click(60, y);
    await sleep(7000);
    await shot('artemis-menu-' + label.toLowerCase() + '.png');
    const s = await state();
    console.log('overlay: ' + (s?.overlay ?? '(none)').replace(/\n+/g, ' | '));
    for (const l of lines.slice(before)) console.log('  ' + l);
    // Back to the title for the next row. Escape is the engine's own "leave this screen" key.
    await key('Escape');
    await sleep(2500);
  }
} catch (error) {
  console.log('menu walk ended: ' + (error?.message ?? error));
} finally {
  console.log('\n==== crash lines (' + crash.length + ') ====');
  for (const l of crash.slice(0, 30)) console.log(l);
  console.log('\n==== final overlay ====');
  console.log((await state())?.overlay ?? '(none)');
  await stop();
}
process.exit(process.exitCode ?? 0);
