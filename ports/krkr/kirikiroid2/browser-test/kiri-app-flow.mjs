/**
 * Drives the platform's own UI in a real browser: import a game, launch it, and check
 * that the KiriKiri WebAssembly engine — not the interpreter — ends up running it.
 *
 * This is the layer the unit tests cannot reach. `kiri-wasm-provider.test.ts` asserts
 * that the provider *would* claim a zip-only game and mount it through the seam, and
 * `kiri-browser-probe.mjs` asserts that the seam serves the engine's reads when the
 * build page is driven directly. Neither exercises the chain that a user actually
 * walks: `Library.svelte` → `GameLibrary.import` → detect → `krkr2-wasm` → `/play` →
 * `KiriWasmRuntime` → framed build → seam.
 *
 * The single-file import is deliberate. A directory of loose files is *not* mountable
 * by the framed shell (it mounts one archive), so the engine declines it and the
 * interpreter handles it; a lone ZIP is the whole-game zip form the shell mounts with
 * `?game=`. Using `game.zip` therefore lands on the engine by the intended route rather
 * than by a special case.
 *
 * Usage:
 *   node kiri-app-flow.mjs [--base URL] [--game FILE] [--out FILE] [--screenshot FILE]
 */
import { writeFileSync } from 'node:fs';

import { launchEdge, sleep } from './cdp.mjs';
import { repoRoot, repoRootPosix } from '../../lib/ref.mjs';
import {
  averageColour,
  brightFraction,
  decodePng,
  frameSignature,
  litFraction,
  luminanceHistogram,
  regionStats,
} from './png.mjs';

const argv = process.argv.slice(2);
const argOf = (flag, fallback) => {
  const index = argv.indexOf(flag);
  return index >= 0 && argv[index + 1] ? argv[index + 1] : fallback;
};

const BASE = argOf('--base', process.env.KIRI_APP_BASE ?? 'http://127.0.0.1:4321');
const GAME = argOf('--game', `${repoRoot}/ports/krkr/kirikiroid2/browser-test/game.zip`);
const OUT = argOf('--out', null);
const SCREENSHOT = argOf('--screenshot', null);
const PORT = Number(argOf('--port', process.env.KIRI_CDP_PORT ?? 9500));

const WANTED_KEY = GAME.split('/').pop();

const summary = {
  base: BASE,
  game: GAME,
  consoleLines: [],
  exceptions: [],
  requests: [],
  importResult: null,
  playUrl: null,
  frame: null,
  engine: null,
  network: null,
};

/** Polls `expression` until it returns a truthy value, or gives up. */
async function waitFor(cdp, label, expression, timeoutMs, intervalMs = 500) {
  const deadline = Date.now() + timeoutMs;
  let last;
  while (Date.now() < deadline) {
    try {
      last = await cdp.evaluate(expression);
      if (last) return last;
    } catch (error) {
      last = `evaluate failed: ${String(error).slice(0, 200)}`;
    }
    await sleep(intervalMs);
  }
  throw new Error(`timed out waiting for ${label}; last value: ${JSON.stringify(last)?.slice(0, 300)}`);
}

/**
 * Hands the archive to the page's file input and waits for the library card.
 *
 * Success is the *card*, not the assignment: Astro prerenders the markup, so the file
 * input exists in the HTML before Svelte hydrates and attaches `onchange` — a `change`
 * delivered in that window is simply lost. The event is therefore re-dispatched a few
 * times, while the file list itself is only assigned when it is empty (a repeated
 * assignment would re-import 14 MB each round). A node id fetched before the app's
 * `onMount` finishes is also stale (`Could not find node with given id`), hence the
 * lookup inside the loop.
 */
async function importArchive(cdp, filePath, timeoutMs = 180_000) {
  const selector = 'input[type=file]:not([webkitdirectory])';
  const cardHref = `(() => {
    const link = document.querySelector('a[href^="/play?id="]');
    return link ? link.getAttribute('href') : null;
  })()`;
  const deadline = Date.now() + timeoutMs;
  let dispatched = 0;
  let lastError = null;
  while (Date.now() < deadline) {
    try {
      const href = await cdp.evaluate(cardHref);
      if (href) return href;
      if (dispatched < 3) {
        const held = await cdp.evaluate(
          `(() => { const input = document.querySelector('${selector}'); return input && input.files ? input.files.length : 0; })()`,
        );
        if (held === 0) {
          const { root } = await cdp.send('DOM.getDocument', { depth: -1 });
          const { nodeId } = await cdp.send('DOM.querySelector', {
            nodeId: root.nodeId,
            selector,
          });
          if (!nodeId) throw new Error('the archive input is not in the DOM yet');
          await cdp.send('DOM.setFileInputFiles', { files: [filePath], nodeId });
        }
        await cdp.evaluate(
          `(() => {
             const input = document.querySelector('${selector}');
             input.dispatchEvent(new Event('change', { bubbles: true }));
             return input.files ? input.files.length : -1;
           })()`,
        );
        dispatched += 1;
      }
    } catch (error) {
      lastError = error;
    }
    await sleep(500);
  }
  throw new Error(
    `the imported game never appeared in the library ` +
      `(dispatched ${dispatched} change event(s))${lastError ? `: ${String(lastError)}` : ''}`,
  );
}

let browser = null;
try {
  browser = await launchEdge({ port: PORT, enable: ['Page', 'Runtime', 'Log', 'Network', 'DOM'] });
  const { cdp } = browser;

  cdp.on('Runtime.consoleAPICalled', (p) => {
    const text = p.args
      .map((a) => (a.value !== undefined ? String(a.value) : a.description ?? `[${a.type}]`))
      .join(' ');
    summary.consoleLines.push({ type: p.type, text: text.slice(0, 400) });
  });
  cdp.on('Runtime.exceptionThrown', (p) => {
    summary.exceptions.push(
      (p.exceptionDetails?.exception?.description ?? p.exceptionDetails?.text ?? 'unknown').slice(0, 400),
    );
  });
  cdp.on('Network.requestWillBeSent', (p) => {
    summary.requests.push({
      requestId: p.requestId,
      method: p.request.method,
      url: p.request.url,
      range: p.request.headers?.Range ?? null,
    });
  });
  let encoded = 0;
  cdp.on('Network.loadingFinished', (p) => {
    encoded += p.encodedDataLength ?? 0;
  });

  /* --- 1. the library page ------------------------------------------------ */

  await cdp.send('Page.navigate', { url: `${BASE}/` });
  await waitFor(
    cdp,
    'the library page to render its import input',
    `!!document.querySelector('input[type=file]')`,
    60_000,
  );

  /* --- 2. import the game through the real input -------------------------- */

  // Importing stores the bytes in IndexedDB and probes them, so this is the slow step.
  const playHref = await importArchive(cdp, GAME);
  summary.importResult = { playHref };
  summary.playUrl = `${BASE}${playHref}`;

  // Which engine the platform *decided* on, read back from its own store, plus what the
  // engine's mount derivation makes of the source the record reopens. Without this the
  // only symptom of a declined wasm provider is a frame that never appears, which says
  // nothing about why.
  summary.record = await cdp.evaluate(`(async () => {
    const platform = (await import('/src/lib/platform.ts')).getPlatform();
    const vm = await import('${repoRootPosix}/packages/runtime-kag/src/wasm-config.ts');
    const records = await platform.library.list();
    const out = [];
    for (const record of records) {
      let source = null;
      try {
        source = await platform.library.openSource(record);
      } catch (error) {
        out.push({ name: record.name, openSourceError: String(error).slice(0, 200) });
        continue;
      }
      out.push({
        name: record.name,
        sourceKind: record.source && record.source.kind,
        runtimeId: record.detection && record.detection.runtimeId,
        score: record.detection && record.detection.score,
        availability: record.detection && record.detection.availability,
        config: record.config ?? null,
        root: await source.list(''),
        mount: await vm.findKiriMount(source),
      });
    }
    return out;
  })()`);

  /* --- 3. launch it ------------------------------------------------------- */

  await cdp.send('Page.navigate', { url: summary.playUrl });

  // Instrument the seam the platform installs, as early as it appears. The count has to
  // start before the engine's first read, and the app publishes the source synchronously
  // while it mounts the frame, so a tight poll catches it well before the shell — which
  // still has a 22 MB wasm to fetch — asks for bytes.
  const measurementReady = cdp.evaluate(`(async () => {
    for (let attempt = 0; attempt < 400; attempt += 1) {
      const source = window.__onlineGalVfsSource;
      if (source && !source.__measured) {
        const originalReadRange = source.readRange.bind(source);
        const originalStat = source.stat ? source.stat.bind(source) : null;
        const stats = { readRangeCalls: 0, bytesReturned: 0, statCalls: 0, firstRanges: [] };
        source.readRange = async (url, offset, length) => {
          const bytes = await originalReadRange(url, offset, length);
          stats.readRangeCalls += 1;
          stats.bytesReturned += bytes.byteLength;
          if (stats.firstRanges.length < 6) stats.firstRanges.push({ offset, length });
          return bytes;
        };
        if (originalStat) {
          source.stat = async (url) => {
            stats.statCalls += 1;
            return originalStat(url);
          };
        }
        source.__measured = stats;
        return true;
      }
      await new Promise((resolve) => setTimeout(resolve, 20));
    }
    return false;
  })()`);

  // The provider claims the game only if it is published and mountable; the framed
  // build is the visible proof that `krkr2-wasm` won the dispatch.
  const frameSrc = await waitFor(
    cdp,
    'the KiriKiri frame to be mounted',
    `(() => {
       const frame = document.querySelector('iframe');
       return frame && frame.src ? frame.src : null;
     })()`,
    120_000,
    1_000,
  );

  // The engine boots asynchronously inside the frame: the shell fetches a 22 MB wasm and
  // its asset zip from the app origin, then mounts the archive through the seam. Waiting
  // on the shell's own status line *plus* the seam is what distinguishes "the frame is
  // there" from "the engine is actually reading the game".
  const engineStatus = await waitFor(
    cdp,
    'the engine to mount the archive',
    `(() => {
       const frame = document.querySelector('iframe');
       const win = frame && frame.contentWindow;
       if (!win || !win.document) return null;
       const seam = typeof win.__onlineGalVfsSource !== 'undefined';
       const status = win.document.querySelector('#status');
       const text = status && status.textContent ? status.textContent.trim() : '';
       // The shell starts at "Loading..."/"Preparing..." and moves on once the game is
       // mounted; both facts together are the signal.
       if (seam && text && !/^(Loading|Preparing)/i.test(text)) return text;
       return null;
     })()`,
    240_000,
    2_000,
  );

  const detail = await cdp.evaluate(
    `(() => {
       const frame = document.querySelector('iframe');
       const win = frame && frame.contentWindow;
       const doc = win && win.document;
       const canvas = doc && doc.querySelector('canvas');
       return {
         src: frame && frame.src,
         seamInstalled: !!(win && win.__onlineGalVfsSource),
         seamShape: win && win.__onlineGalVfsSource
           ? { readRange: typeof win.__onlineGalVfsSource.readRange, stat: typeof win.__onlineGalVfsSource.stat }
           : null,
         status: doc && doc.querySelector('#status') ? doc.querySelector('#status').textContent.trim() : null,
         title: doc ? doc.title : null,
         canvas: canvas ? { width: canvas.width, height: canvas.height } : null,
       };
     })()`,
  );
  summary.frame = { src: frameSrc, seamInstalled: detail.seamInstalled, seamShape: detail.seamShape };
  summary.engine = {
    status: detail.status ?? engineStatus,
    title: detail.title,
    canvas: detail.canvas,
  };

  /* --- 4. does it actually draw, and does input move it? ------------------ */

  await measurementReady;
  summary.seamMeasurement = await cdp.evaluate(
    `(() => {
       const source = window.__onlineGalVfsSource;
       return source && source.__measured ? source.__measured : null;
     })()`,
  );

  // The engine's own log overlay is a debug surface painted over the game; hiding it is
  // what makes the capture measurable (its white-on-black text would otherwise be the
  // only lit pixels). Its text is read first, because it is the progress signal.
  const logTextBefore = await cdp.evaluate(
    `(() => {
       const frame = document.querySelector('iframe');
       const doc = frame && frame.contentWindow && frame.contentWindow.document;
       const log = doc && doc.getElementById('engine-status');
       if (!log) return null;
       const text = log.textContent || '';
       log.style.display = 'none';
       return text;
     })()`,
  );

  const frameBox = await cdp.evaluate(
    `(() => {
       const frame = document.querySelector('iframe');
       if (!frame) return null;
       const rect = frame.getBoundingClientRect();
       return { x: rect.x, y: rect.y, width: rect.width, height: rect.height };
     })()`,
  );

  /**
   * Captures the frame's rectangle and reduces it to the two measures that matter.
   *
   * The rectangle is read *at capture time* and must be big enough to be the game: the
   * player page lays the frame out over its canvas, and a capture taken before that
   * happens is a few pixels of unrelated UI — which would make a before/after comparison
   * meaningless while still producing a plausible-looking fraction.
   */
  const captureFrame = async (label) => {
    const box = await cdp.evaluate(
      `(() => {
         const frame = document.querySelector('iframe');
         if (!frame) return null;
         const rect = frame.getBoundingClientRect();
         return { x: rect.x, y: rect.y, width: rect.width, height: rect.height };
       })()`,
    );
    if (!box || box.width < 100 || box.height < 100) {
      return { label, tooSmall: box ? { width: box.width, height: box.height } : null };
    }
    const shot = await cdp.send('Page.captureScreenshot', {
      format: 'png',
      clip: { x: box.x, y: box.y, width: box.width, height: box.height, scale: 1 },
    });
    const buffer = Buffer.from(shot.data, 'base64');
    const image = decodePng(buffer);
    return {
      label,
      buffer,
      image,
      width: image.width,
      height: image.height,
      // Threshold 64, not 24: the player page has dark-grey chrome (a bottom bar) that a
      // low cut-off counts as "drawn", which would make a black game canvas look painted.
      litFraction: Number(litFraction(image, 64).toFixed(4)),
      brightFraction: Number(brightFraction(image, 200).toFixed(4)),
      histogram: luminanceHistogram(image),
      colour: averageColour(image),
      signature: frameSignature(image),
    };
  };

  // Let the engine finish its own boot before anything is judged: the demo spends its
  // first seconds black while TJS runs, and a capture taken then says nothing about input.
  await sleep(6000);
  const boot = await captureFrame('after-boot');
  if (SCREENSHOT && boot.buffer) writeFileSync(SCREENSHOT, boot.buffer);

  /**
   * Clicks a point inside the frame, in the frame's own coordinates.
   *
   * Re-reads the rectangle every time so a late layout cannot move the target, and goes
   * through `Input.dispatchMouseEvent` so the browser's own input pipeline delivers it.
   */
  const clickInFrame = async (x, y) => {
    const box = await cdp.evaluate(
      `(() => {
         const frame = document.querySelector('iframe');
         if (!frame) return null;
         const rect = frame.getBoundingClientRect();
         return { x: rect.x, y: rect.y, width: rect.width, height: rect.height };
       })()`,
    );
    if (!box || box.width < 100 || box.height < 100) return false;
    const px = Math.round(box.x + Math.min(Math.max(x, 1), box.width - 1));
    const py = Math.round(box.y + Math.min(Math.max(y, 1), box.height - 1));
    await cdp.send('Input.dispatchMouseEvent', { type: 'mouseMoved', x: px, y: py });
    await cdp.send('Input.dispatchMouseEvent', { type: 'mousePressed', x: px, y: py, button: 'left', clickCount: 1 });
    await cdp.send('Input.dispatchMouseEvent', { type: 'mouseReleased', x: px, y: py, button: 'left', clickCount: 1 });
    return true;
  };

  // First click advances the demo out of its boot screen into the title menu (this is what
  // a player does at the loading screen).
  const advancedPastBoot = await clickInFrame(354, 146);
  await sleep(6000);
  const menu = await captureFrame('title-menu');
  if (SCREENSHOT && menu.buffer) {
    writeFileSync(SCREENSHOT.replace(/\.png$/, '-menu.png'), menu.buffer);
  }

  // The seam counters are the signal the click is judged by. Global pixel statistics cannot
  // carry this claim: the demo's title screen animates (so a signature changes on its own),
  // and both scenes are dark with similar tone (so brightness histograms barely move).
  // Loading a *scene*, on the other hand, must read the archive — and an animation cannot.
  const readSeamCounters = () =>
    cdp.evaluate(
      `(() => {
         const stats = window.__onlineGalVfsSource && window.__onlineGalVfsSource.__measured;
         return stats ? { calls: stats.readRangeCalls, bytes: stats.bytesReturned, statCalls: stats.statCalls } : null;
       })()`,
    );
  const consoleCount = () => summary.consoleLines.length;
  const countersBeforeEntry = await readSeamCounters();
  const consoleBeforeEntry = consoleCount();

  // Then a click **on a menu entry**: entering a scene is what a hit target must cause.
  const clickedEntry = await clickInFrame(354, 110);
  await sleep(8000);
  const afterEntry = await captureFrame('after-menu-entry');
  const countersAfterEntry = await readSeamCounters();
  if (SCREENSHOT && afterEntry.buffer) {
    writeFileSync(SCREENSHOT.replace(/\.png$/, '-after-click.png'), afterEntry.buffer);
  }

  const newConsoleLines = summary.consoleLines
    .slice(consoleBeforeEntry)
    .map((line) => line.text.slice(0, 160));

  // Where a KAG narration box sits: the upper band, inset from the sides. Measured in both
  // captures so the run's output shows the separation rather than asserting a guess.
  const BOX_BAND = { x0: 0.24, y0: 0.02, x1: 0.76, y1: 0.44 };
  const boxBand = (capture) =>
    capture.tooSmall ? null : regionStats(capture.image, BOX_BAND, [120, 150, 180]);

  const sameRegion = (a, b) =>
    !a.tooSmall && !b.tooSmall && a.width === b.width && a.height === b.height;

  summary.render = boot.tooSmall
    ? { tooSmall: boot.tooSmall }
    : { width: boot.width, height: boot.height, litFraction: boot.litFraction, signature: boot.signature };

  const logTextAfter = await cdp.evaluate(
    `(() => {
       const frame = document.querySelector('iframe');
       const doc = frame && frame.contentWindow && frame.contentWindow.document;
       const log = doc && doc.getElementById('engine-status');
       return log ? (log.textContent || '') : null;
     })()`,
  );

  summary.input = {
    bootToMenu: {
      clicked: advancedPastBoot,
      comparable: sameRegion(boot, menu),
      changed: sameRegion(boot, menu) && boot.signature !== menu.signature,
      litFractionBefore: boot.tooSmall ? null : boot.litFraction,
      litFractionAfter: menu.tooSmall ? null : menu.litFraction,
    },
    menuEntry: {
      clicked: clickedEntry,
      comparable: sameRegion(menu, afterEntry),
      // The claim the evaluation rests on: after the click the engine read *more of the
      // archive*, which only a scene change can cause (the title screen animates without
      // reading anything, and both scenes measure alike globally).
      progressed:
        countersBeforeEntry !== null &&
        countersAfterEntry !== null &&
        countersAfterEntry.calls > countersBeforeEntry.calls,
      seamReadsDelta:
        countersBeforeEntry === null || countersAfterEntry === null
          ? null
          : {
              calls: countersAfterEntry.calls - countersBeforeEntry.calls,
              bytes: countersAfterEntry.bytes - countersBeforeEntry.bytes,
            },
      // Supporting evidence, not the assertion: the signature proves the frame was redrawn
      // (which an animated title bar also does, hence "supporting").
      redrawn: sameRegion(menu, afterEntry) && menu.signature !== afterEntry.signature,
      region: sameRegion(menu, afterEntry) ? { width: menu.width, height: menu.height } : null,
      signatureBefore: menu.tooSmall ? null : menu.signature,
      signatureAfter: afterEntry.tooSmall ? null : afterEntry.signature,
      brightFractionBefore: menu.tooSmall ? null : menu.brightFraction,
      brightFractionAfter: afterEntry.tooSmall ? null : afterEntry.brightFraction,
      histogramBefore: menu.tooSmall ? null : menu.histogram,
      histogramAfter: afterEntry.tooSmall ? null : afterEntry.histogram,
      colourBefore: menu.tooSmall ? null : menu.colour,
      colourAfter: afterEntry.tooSmall ? null : afterEntry.colour,
      newConsoleLines,
      // The narration box band, in both states. Reported rather than asserted: the numbers
      // are what a reader needs to judge the automated claim below.
      narrationBandBefore: boxBand(menu),
      narrationBandAfter: boxBand(afterEntry),
      // Automated *supporting* check: the narration panel sits in this band, so the band
      // turns lighter when the click enters the scene. Measured separation is about 1.8x at
      // luma 150 (0.085 -> 0.150), so the bar is set at 1.5x — a data-driven margin, not a
      // guess. It is supporting evidence only: the decisive proof is the screenshot pair
      // (title menu -> narration scene with a text panel), because the modest ratio shows
      // how little global-ish statistics can say about a canvas.
      narrationBandLightened: (() => {
        const before = boxBand(menu);
        const after = boxBand(afterEntry);
        if (!before || !after) return null;
        return after.fractions['150'] >= before.fractions['150'] * 1.5;
      })(),
    },
    logGrew: Boolean(logTextBefore !== null && logTextAfter !== null && logTextAfter.length > logTextBefore.length),
    logCharsBefore: logTextBefore === null ? null : logTextBefore.length,
    logCharsAfter: logTextAfter === null ? null : logTextAfter.length,
  };

  /* --- 5. how the archive's bytes arrived --------------------------------- */

  const gameUrl = `${BASE}/krkr2-web/${WANTED_KEY}`;
  const archiveRequests = summary.requests.filter((r) => r.url.endsWith(`/${WANTED_KEY}`));
  summary.network = {
    archiveRequests: archiveRequests.length,
    archiveHeadRequests: archiveRequests.filter((r) => r.method === 'HEAD').length,
    archiveRangeRequests: archiveRequests.filter((r) => r.range !== null).length,
    archiveUnconditionalGets: archiveRequests.filter((r) => r.method === 'GET' && r.range === null).length,
    encodedDataLengthTotal: encoded,
    // The frame asks the platform for the size and the bytes; that log line is emitted
    // by the patched shell, so it is direct evidence the seam served the mount.
    hostSeamLogSeen: summary.consoleLines.some((l) => l.text.includes('host byte-source')),
    engineErrors: summary.consoleLines.filter((l) => l.type === 'error').map((l) => l.text.slice(0, 200)),
    exceptionCount: summary.exceptions.length,
  };
  summary.gameUrl = gameUrl;
} catch (error) {
  summary.fatal = String(error?.stack ?? error);
} finally {
  await browser?.stop();
}

console.log(JSON.stringify(summary, null, 2));
if (OUT) writeFileSync(OUT, JSON.stringify(summary, null, 2));
process.exit(summary.fatal ? 1 : 0);
