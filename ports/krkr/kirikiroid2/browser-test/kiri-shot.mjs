/**
 * Screenshots the built KiriKiri page against a real XP3 game.
 *
 * The existing probe (`kiri-browser-probe.mjs`) already proves the byte-level behaviour — how much
 * was read, through which channel, with how many calls. What it does not show is whether the engine
 * actually **rendered** the game: `statusText: "Running..."` and a non-default canvas size are
 * consistent with a game that booted and then drew nothing, and with one that failed quietly after
 * sizing its surface.
 *
 * This script exists for that one question, so it does as little as possible: launch, navigate, wait,
 * screenshot, close. It takes the same URL the probe uses, so the two can be compared directly.
 *
 * Usage:
 *   KIRI_QUERY='?xp3=/data.xp3' node kiri-shot.mjs --seconds 60 --out /tmp/xp3.png
 */
import { writeFileSync } from 'node:fs';

import { launchEdge, sleep } from './cdp.mjs';

const BASE = process.env.KIRI_BASE ?? 'http://127.0.0.1:4630';
const PAGE = process.env.KIRI_PAGE ?? 'index.html';
const QUERY = process.env.KIRI_QUERY ?? '?xp3=/data.xp3';
const PORT = Number(process.env.KIRI_CDP_PORT ?? 9223);

const argv = process.argv.slice(2);
const argOf = (name, fallback) => {
  const at = argv.indexOf(name);
  return at >= 0 && argv[at + 1] ? argv[at + 1] : fallback;
};
const seconds = Number(argOf('--seconds', '60'));
const outFile = argOf('--out', '/tmp/kiri-shot.png');

const url = `${BASE}/${PAGE}${QUERY}`;

// `launchEdge` returns an already-connected client plus an idempotent `stop`; it takes care of the
// throwaway profile, so this script does not touch processes or temporary directories itself.
const { cdp, stop } = await launchEdge({ port: PORT, enable: ['Page', 'Runtime'] });

try {
  await cdp.send('Page.navigate', { url });

  // The engine reads a real archive and boots a TJS script, so this is not a fixed delay so much as
  // enough time for the first scene to reach the canvas.
  await sleep(seconds * 1000);

  const state = await cdp.send('Runtime.evaluate', {
    expression: `JSON.stringify({
      title: document.title,
      canvas: (() => { const c = document.querySelector('canvas'); return c ? c.width + 'x' + c.height : 'none'; })(),
      body: (document.body.innerText || '').replace(/\\s+/g, ' ').slice(0, 160),
    })`,
    returnByValue: true,
  });

  const shot = await cdp.send('Page.captureScreenshot', { format: 'png' });
  writeFileSync(outFile, Buffer.from(shot.data, 'base64'));

  console.log('url:  ', url);
  console.log('state:', state.result?.value ?? JSON.stringify(state));
  console.log('shot: ', outFile);
} finally {
  await stop();
}
