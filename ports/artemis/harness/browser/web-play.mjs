/**
 * Plays the Artemis web host for a while and reports the first crash, if any.
 *
 * The failure this exists to catch is an `Aborted()` raised from inside a frame, which happens after
 * minutes of play rather than at boot — so a boot-only check cannot see it. The engine is driven the
 * way a player drives it: leave the title menu, then click through dialogue.
 *
 * Usage: node web-play.mjs [url] [seconds]
 */
import { writeFileSync } from 'node:fs';
// Real Chrome, not headless Edge. See chrome.mjs for why: real GPU, and a real window size — the
// headless harness runs at 750x485, a tenth of the pixels a player actually has.
import { launchBrowser as launchEdge, sleep } from './chrome.mjs';

const URL_ = process.argv[2] ?? 'http://127.0.0.1:4650/artemis-web.html';
const SECONDS = Number(process.argv[3] ?? 300);

const { cdp, stop } = await launchEdge({ port: 9222, enable: ['Runtime', 'Page', 'Log'] });

const consoleLines = [];
const crash = [];
const watch = (text) => {
  if (/Aborted|threw:|frame failed|RuntimeError|unreachable|out of bounds|call stack|assert/i.test(text)) {
    crash.push(text);
  }
};
cdp.on('Runtime.consoleAPICalled', (p) => {
  const text = (p.args ?? []).map((a) => a.value ?? a.description ?? a.type).join(' ');
  consoleLines.push(`[${p.type}] ${text}`);
  watch(text);
});
cdp.on('Runtime.exceptionThrown', (p) => {
  const d = p.exceptionDetails ?? {};
  const text = d.exception?.description ?? d.text ?? '(no detail)';
  consoleLines.push(`[exception] ${text}`);
  watch(text);
});
cdp.on('Log.entryAdded', (p) => {
  const e = p.entry ?? {};
  const text = e.text ?? '';
  if (/404 \(expected 206\)/.test(text)) return;
  consoleLines.push(`[${e.source ?? 'log'}] ${text}`);
  watch(text);
});

async function probe(expression) {
  try {
    const r = await cdp.send('Runtime.evaluate', { expression, returnByValue: true });
    return typeof r?.result?.value === 'string' ? JSON.parse(r.result.value) : null;
  } catch {
    return null;
  }
}

const state = () => probe(`JSON.stringify({href: location.href,
  overlay: document.getElementById('overlay') ? document.getElementById('overlay').textContent : null,
  canvas:(function(){const c=document.getElementById('canvas');return c?c.width+'x'+c.height:null;})()})`);

try {
  await cdp.send('Page.navigate', { url: URL_ });

  // Wait for **our** document, not merely for a document: a page target can exist before the
  // navigation commits, and reading `#overlay` out of that one yields a null overlay that reads like a
  // host failure. Re-navigating is cheap; misreading the state is not.
  let loaded = null;
  for (let i = 0; i < 60 && !loaded; i += 1) {
    await sleep(500);
    const s = await state();
    if (s && typeof s.href === 'string' && s.href.includes('artemis-web.html') && s.overlay !== null) {
      loaded = s;
    } else if (i === 20) {
      await cdp.send('Page.navigate', { url: URL_ });
    }
  }
  if (!loaded) {
    console.log('navigation never committed. last state: ' + JSON.stringify(await state()));
    process.exitCode = 1;
    throw new Error('not loaded');
  }

  let opened = null;
  for (let i = 0; i < 120 && !opened; i += 1) {
    await sleep(1000);
    const s = await state();
    if (s && /opened /.test(s.overlay ?? '')) opened = s;
    if (crash.length) break;
  }
  if (!opened) {
    console.log('never booted. overlay:\n' + ((await state())?.overlay ?? '(none)'));
    process.exitCode = 1;
    throw new Error('not booted');
  }
  console.log('booted: canvas=' + opened.canvas);

  const rect = await probe(`JSON.stringify((function(){const r=document.getElementById('canvas').getBoundingClientRect();
    return {left:r.left,top:r.top,w:r.width,h:r.height};})())`);

  // Clicks are given in **stage** coordinates (1280x720) and mapped through the host's letterbox, so
  // the same script is correct at any window size. Passing screen pixels would silently miss every
  // target as soon as the browser window is not the headless 750x485.
  const scale = Math.min(rect.w / 1280, rect.h / 720);
  const originX = rect.left + (rect.w - 1280 * scale) / 2;
  const originY = rect.top + (rect.h - 720 * scale) / 2;

  async function click(stageX, stageY) {
    const x = originX + stageX * scale;
    const y = originY + stageY * scale;
    for (const type of ['mousePressed', 'mouseReleased']) {
      await cdp.send('Input.dispatchMouseEvent', {
        type, x, y, button: 'left', clickCount: 1,
        buttons: type === 'mousePressed' ? 1 : 0,
      });
      await sleep(50);
    }
  }

  // Leave the title menu first: that click starts the game, so it has to land on START.
  await click(100, 297);
  console.log('clicked START');

  const started = Date.now();
  while ((Date.now() - started) / 1000 < SECONDS && !crash.length) {
    await sleep(2500);
    // Middle of the stage: the engine falls back to `onpush key 1`, which is what advances dialogue
    // when no interactive layer is registered under the cursor.
    await click(640, 510);
    const s = await state();
    const m = /frames (\d+)\s+presented (\d+)\s+revision (\d+)/.exec(s?.overlay ?? '');
    const err = /last error (.*)/.exec(s?.overlay ?? '');
    console.log(`t=${Math.round((Date.now() - started) / 1000)}s frames=${m ? m[1] : '?'} ` +
                `presented=${m ? m[2] : '?'} revision=${m ? m[3] : '?'} lastError=${err ? err[1] : '?'}`);
  }
} catch (error) {
  console.log('play loop ended: ' + (error?.message ?? error));
} finally {
  console.log('\n==== crash lines (' + crash.length + ') ====');
  for (const line of crash.slice(0, 40)) console.log(line);
  console.log('\n==== console tail (50) ====');
  for (const line of consoleLines.slice(-50)) console.log(line);
  console.log('\n==== final overlay ====');
  console.log((await state())?.overlay ?? '(none)');
  const s = await cdp.send('Page.captureScreenshot', { format: 'png' }).catch(() => null);
  if (s) writeFileSync(new URL('./artemis-play.png', import.meta.url), Buffer.from(s.data, 'base64'));
  console.log('\nscreenshot: artemis-play.png');
  // Always, even on the failure paths above: an orphaned headless browser keeps playing (and keeps
  // streaming the pack) long after the harness that started it has gone.
  await stop();
}
process.exit(process.exitCode ?? 0);
