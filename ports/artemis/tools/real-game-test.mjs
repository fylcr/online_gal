#!/usr/bin/env node
/**
 * Does a rebuilt engine actually run a real game?
 *
 * ## Why this exists
 *
 * `wasm-replace-test.mjs` answers a narrower question: *would* the official glue be able to
 * load this module. That is a contract check against inert stubs. It cannot tell you whether
 * the engine boots, fetches the game's files on demand, brings up a window, and draws the
 * right pixels -- all of which depend on the host actually answering the 198 calls the module
 * makes, and on both tenants of linear memory agreeing where their halves end.
 *
 * The only honest test of that is the one that produced the answer in the first place:
 * serve the real game with the candidate engine, open it in a real browser, and compare what
 * it draws against the same page running the official engine.
 *
 * ## What it does
 *
 *   1. Deploys two trees with `official-web-deploy.py`: one with the official wasm, one with
 *      the candidate. Same game, same glue, same page -- the engine is the only difference.
 *   2. Serves both over HTTP (the engine cannot read local files, and asyncify fetches every
 *      asset).
 *   3. Drives a real Chrome over CDP: waits for the engine to settle, samples the canvas
 *      *inside* requestAnimationFrame (outside it the buffer is already cleared and
 *      everything reads black -- including the official build, which is how that trap was
 *      found), hashes pixels, and pulls the page's console.
 *   4. Verifies the game was actually fetched on demand, since a module that renders a
 *      welcome screen without reading any of the game has not run the game.
 *   5. Reports the two side by side and exits non-zero when the candidate did not render or
 *      did not fetch the game.
 *
 * ## Usage
 *
 *     node real-game-test.mjs --game <loose tree> --engine <candidate.wasm> [options]
 *
 *     --game DIR        the tree extract-pfs.mjs produced (must contain system.ini)
 *     --engine FILE     the candidate module to put in artemis.wasm's place
 *     --flavour NAME    asyncify (default) or plain
 *     --port N          first port to serve on; the control takes N+1 (default 4710)
 *     --hashlist        also write hashlist.txt (asyncify mode wants it)
 *     --wait MS         how long to let the engine settle before sampling (default 240000)
 *     --json            machine-readable result on stdout
 *     --keep            leave the servers running for inspection
 *
 * ## Exit status
 *
 * 0 -- the candidate rendered a non-black frame and fetched the game
 * 1 -- it did not, or the control did not (in which case nothing about the candidate is
 *      established and the report says so)
 */

import { spawn } from 'node:child_process';
import { randomBytes } from 'node:crypto';
import { existsSync, mkdtempSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const HERE = dirname(fileURLToPath(import.meta.url));
const REPO = resolve(HERE, '..', '..', '..');
const DEPLOY = join(REPO, 'ports', 'artemis', 'build', 'official-web-deploy.py');

// Windows absolute paths are not importable as-is: the ESM loader wants a file:// URL and
// rejects a bare `d:\...` with ERR_UNSUPPORTED_ESM_URL_SCHEME. `pathToFileURL` is the fix.
const { ensureChrome, sleep } = await import(
  pathToFileURL(join(REPO, 'ports', 'artemis', 'harness', 'browser', 'chrome.mjs')).href
);

// ---------------------------------------------------------------------------
// arguments
// ---------------------------------------------------------------------------
function parseArgs(argv) {
  // A cold boot streams roughly 18 MB of game data through one wasm thread and needs well over
  // 90 s before the first frame, so a shorter wait reports "still loading" as a broken module.
  const out = { flavour: 'asyncify', port: 4710, cdpPort: 9222, hashlist: false, wait: 240000, json: false, keep: false };
  for (let i = 0; i < argv.length; i += 1) {
    const arg = argv[i];
    const next = () => argv[++i];
    if (arg === '--game') out.game = next();
    else if (arg === '--engine') out.engine = next();
    else if (arg === '--flavour') out.flavour = next();
    else if (arg === '--port') out.port = Number(next());
    else if (arg === '--cdp-port') out.cdpPort = Number(next());
    else if (arg === '--wait') out.wait = Number(next());
    else if (arg === '--no-control') out.noControl = true;
    else if (arg === '--hashlist') out.hashlist = true;
    else if (arg === '--json') out.json = true;
    else if (arg === '--json-out') out.jsonOut = next();
    else if (arg === '--savepath') out.savepath = next();
    else if (arg === '--keep') out.keep = true;
    else if (arg === '-h' || arg === '--help') out.help = true;
    else throw new Error(`unknown argument: ${arg}`);
  }
  return out;
}

function usage() {
  console.log(`usage: node real-game-test.mjs --game <loose tree> --engine <candidate.wasm> [options]

  --game DIR      the tree extract-pfs.mjs produced (must contain system.ini)
  --engine FILE   candidate module to serve as artemis.wasm
  --flavour NAME  asyncify (default) or plain
  --port N        first HTTP port; the control runs on N+1 (default 4710)
  --cdp-port N    the browser's debugging port (default 9222)
  --hashlist      also write hashlist.txt
  --wait MS       settle time before sampling (default 240000)
  --no-control    skip the official control run (candidate smoke test only)
  --json          machine-readable result on stdout
  --json-out FILE write the result to FILE
  --savepath S    fixed cache key instead of a fresh one each run
  --keep          leave the servers up`);
}

// ---------------------------------------------------------------------------
// serving
// ---------------------------------------------------------------------------
function run(command, args, options = {}) {
  return new Promise((resolvePromise, reject) => {
    const child = spawn(command, args, { stdio: 'inherit', ...options });
    child.on('error', reject);
    child.on('exit', (code) => (code === 0 ? resolvePromise() : reject(new Error(`${command} exited ${code}`))));
  });
}

/**
 * Start `python -m http.server` in `dir` and wait until it answers.
 *
 * Windows python is found by probing candidates: `python3` resolves to a Store App Execution
 * Alias on this box that fails silently with no output, so a bare `python3` is not usable.
 */
async function findPython() {
  for (const candidate of ['python', 'python3', 'py', 'C:/Python314/python']) {
    try {
      await new Promise((res, rej) => {
        const c = spawn(candidate, ['--version'], { stdio: 'ignore' });
        c.on('error', rej);
        c.on('exit', (code) => (code === 0 ? res() : rej(new Error('bad exit'))));
      });
      return candidate;
    } catch {
      /* try the next one */
    }
  }
  throw new Error('no working python interpreter found');
}

async function serve(python, dir, port) {
  const child = spawn(python, ['-m', 'http.server', String(port), '--bind', '127.0.0.1'], {
    cwd: dir,
    stdio: 'ignore',
    detached: false,
  });
  const deadline = Date.now() + 15000;
  while (Date.now() < deadline) {
    try {
      const res = await fetch(`http://127.0.0.1:${port}/web/artemis.html`);
      if (res.ok) return child;
    } catch {
      /* not up yet */
    }
    await sleep(300);
  }
  child.kill();
  throw new Error(`server on ${port} never answered`);
}

// ---------------------------------------------------------------------------
// page measurement
// ---------------------------------------------------------------------------
/**
 * The in-page probe.
 *
 * Sampling has to happen inside `requestAnimationFrame`. The engine renders with
 * `preserveDrawingBuffer: false`, so once the compositor has taken the frame the buffer is
 * cleared and `readPixels` returns all black -- which is exactly what happened on the first
 * attempt, and it read black for the *official* build too. That is what proved the method
 * wrong rather than the module.
 *
 * The result is deliberately a plain object of numbers and strings: it is returned through
 * `Runtime.evaluate` with `returnByValue`, so anything richer would not survive.
 */
const PROBE = (settleMs, sampleCount) => `(async () => {
  const deadline = Date.now() + ${settleMs};
  const canvas = document.querySelector('canvas');
  if (!canvas) return { error: 'no canvas element' };

  const gl = canvas.getContext('webgl2') || canvas.getContext('webgl');
  if (!gl) return { error: 'no webgl context' };

  // Confirm the frame loop is actually running before believing any black reading: a hidden
  // or frozen tab yields a canvas that never draws, and that looks identical to a module that
  // failed to render. Reporting it separately is the difference between "the engine drew
  // nothing" and "this bench cannot see anything".
  let rafTicks = 0;
  const countRaf = () => { rafTicks += 1; requestAnimationFrame(countRaf); };
  requestAnimationFrame(countRaf);

  const sampleOnce = () => new Promise((done) => {
    // A frozen or hidden tab never fires requestAnimationFrame, and this promise then never
    // settles -- which hangs the whole probe, and with it Runtime.evaluate, forever. A timer
    // turns that from a hang into a reported fact.
    let answered = false;
    const bail = setTimeout(() => {
      if (!answered) done({ stalled: true, mean: 0, max: 0, lit: 0, hash: 'stalled', w: canvas.width, h: canvas.height });
    }, 5000);
    requestAnimationFrame(() => {
      answered = true;
      clearTimeout(bail);
      const w = canvas.width, h = canvas.height;
      const px = new Uint8Array(w * h * 4);
      gl.readPixels(0, 0, w, h, gl.RGBA, gl.UNSIGNED_BYTE, px);
      let sum = 0, max = 0, lit = 0, hash = 0;
      for (let i = 0; i < px.length; i += 4) {
        const v = (px[i] + px[i + 1] + px[i + 2]) / 3;
        sum += v;
        if (v > max) max = v;
        if (v > 8) lit += 1;
      }
      // FNV-1a over a strided subset: enough to tell two frames apart, cheap enough inline.
      for (let i = 0; i < px.length; i += 997) { hash ^= px[i]; hash = (hash * 16777619) >>> 0; }
      done({
        mean: Math.round((sum / (px.length / 4)) * 100) / 100,
        max, lit: Math.round((lit / (w * h)) * 10000) / 100,
        hash: hash.toString(16),
        w, h,
      });
    });
  });

  // Sample until the budget runs out, keeping the best frame seen.
  //
  // An earlier version stopped as soon as the picture held still, and that is wrong for this
  // engine: a cold deployment is *stably black while the game downloads* (the asyncify route
  // reads every asset over HTTP on demand), so a stability test returns during the download
  // and reports a working module as a black screen. Early exit therefore requires a lit frame;
  // otherwise the whole budget is spent, because "nothing yet" and "nothing ever" look the
  // same from here and only time tells them apart.
  const settled = [];
  let last = null, stable = 0;
  const MIN_SAMPLES = 8;
  const fetchedCount = () =>
    performance.getEntriesByType('resource').filter((e) => e.name.includes('/game/')).length;

  while (Date.now() < deadline) {
    const now = await sampleOnce();
    now.fetched = fetchedCount();
    if (last && now.hash === last.hash) stable += 1; else stable = 0;
    settled.push(now);
    last = now;
    if (now.max > 8 && stable >= 3 && settled.length >= MIN_SAMPLES) break;
    await new Promise((r) => setTimeout(r, 500));
  }

  // The best frame, not merely the last one. A title screen that fades, or a transition caught
  // mid-flight, can leave the final sample dimmer than the frame that proves the engine draws.
  const best = settled.reduce((a, b) => (a && a.max >= b.max ? a : b), null);
  const tail = settled.slice(-${sampleCount});
  const frames = [...new Set(tail.map((s) => s.hash))];
  const fetched = performance.getEntriesByType('resource').filter((e) => e.name.includes('/game/'));
  const bytes = fetched.reduce((n, e) => n + (e.transferSize || e.decodedBodySize || 0), 0);

  return {
    frames: frames.length,
    hash: best ? best.hash : null,
    mean: best ? best.mean : 0,
    max: best ? best.max : 0,
    lit: best ? best.lit : 0,
    finalHash: tail.length ? tail[tail.length - 1].hash : null,
    finalMax: tail.length ? tail[tail.length - 1].max : 0,
    canvas: settled.length ? settled[0].w + 'x' + settled[0].h : 'none',
    rafTicks,
    samples: settled.length,
    gameRequests: fetched.length,
    gameBytes: bytes,
    calledRun: !!(window.Module && window.Module.calledRun),
    // The candidate can export diagnostics; the official build simply will not have them.
    trapCode: (() => {
      try {
        const asm = window.Module && window.Module.asm;
        if (asm && typeof asm.artc_get_trap === 'function') {
          const v = asm.artc_get_trap();
          return typeof v === 'bigint' ? Number(v) : v;
        }
      } catch (e) { /* not a probe build */ }
      return null;
    })(),
    // The refused address matters more than the trap code when two address spaces share one
    // linear memory: the code says "out of bounds", the offset says *where*, and that is what
    // distinguishes the guest running past its fence from a genuine wild pointer.
    oob: (() => {
      try {
        const asm = window.Module && window.Module.asm;
        if (!asm || typeof asm.artc_get_oob_offset !== 'function') return null;
        const num = (v) => (typeof v === 'bigint' ? Number(v) : v);
        return {
          offset: num(asm.artc_get_oob_offset()),
          len: num(asm.artc_get_oob_len()),
          size: num(asm.artc_get_oob_size()),
        };
      } catch (e) { return null; }
    })(),
    // The function profile, when the engine was built with it. The arrays live in the linear
    // memory both sides share, so HEAPU32 reads them directly -- no accessor, no copy. Only
    // the non-zero entries come back, which is the whole point: the interesting number is how
    // *few* of the 5,579 functions a real game reaches.
    profile: (() => {
      try {
        const m = window.Module;
        const asm = m && m.asm;
        if (!asm || typeof asm.artc_profile_capacity !== 'function' || !m.HEAPU32) return null;
        const capacity = Number(asm.artc_profile_capacity());
        const countsPtr = Number(asm.artc_profile_base()) >>> 2;
        const orderPtr = Number(asm.artc_profile_order_base()) >>> 2;
        const hits = [];
        for (let i = 0; i < capacity; i += 1) {
          const n = m.HEAPU32[countsPtr + i];
          if (n) hits.push([i, n, m.HEAPU32[orderPtr + i]]);
        }
        return { capacity, touched: hits.length, hits };
      } catch (e) { return { error: String(e && e.message || e) }; }
    })(),
  };
})()`;

/**
 * Open `url` in its own tab, measure it, and close the tab.
 *
 * A dedicated tab matters: `launchBrowser` attaches to whichever page target it finds first,
 * and on a machine where the user's browser is already open that is somebody else's tab.
 * Creating and closing our own keeps the run self-contained, and keeps the two measurements
 * from sharing a renderer -- which would let one engine's state leak into the other's.
 */
async function measure(cdpPort, url, waitMs) {
  // `cdpPort` is the *browser's* debugging port, not the page's HTTP port: `/json/new` is a
  // CDP endpoint. Passing the server port here answers 501, which is how this was caught.
  //
  // The tab opens on `about:blank` and is navigated *after* the session is attached, so
  // nothing the page logs or throws is missed. Opening it directly on `url` starts the load
  // immediately, and everything the module says during start-up -- including why it failed --
  // happens before the listener exists.
  const created = await fetch(
    `http://127.0.0.1:${cdpPort}/json/new?${encodeURIComponent('about:blank')}`,
    { method: 'PUT' },
  );
  if (!created.ok) throw new Error(`could not open a tab on CDP ${cdpPort}: HTTP ${created.status}`);
  const target = await created.json();

  const { Cdp } = await import(
    pathToFileURL(join(REPO, 'ports', 'krkr', 'kirikiroid2', 'browser-test', 'cdp.mjs')).href
  );
  const cdp = new Cdp(target.webSocketDebuggerUrl);
  await cdp.connect();

  const console_ = [];
  cdp.on('Runtime.consoleAPICalled', (event) => {
    const text = (event.args || [])
      .map((a) => (a.value !== undefined ? a.value : a.description || a.type))
      .join(' ');
    console_.push(`[${event.type}] ${text}`);
  });
  cdp.on('Runtime.exceptionThrown', (event) => {
    const d = event.exceptionDetails || {};
    console_.push(`[exception] ${d.exception?.description || d.text || 'unknown'}`);
  });
  // `Log` carries the browser's own messages -- failed subresource loads among them -- which
  // is where "Download file X failed" appears rather than on the console API.
  cdp.on('Log.entryAdded', (event) => {
    const e = event.entry || {};
    console_.push(`[log:${e.level}] ${e.text || ''}`);
  });

  try {
    await cdp.send('Runtime.enable');
    await cdp.send('Page.enable');
    await cdp.send('Log.enable').catch(() => {});
    // Bring the tab to the front before measuring. `requestAnimationFrame` -- which the probe
    // must use, because the engine's buffer is cleared once the compositor has taken the frame
    // -- does not fire in a hidden tab. Without this the canvas stays at its initial 300x150
    // and every sample reads black, for the official build as much as for the candidate, which
    // is exactly what the first run of this script reported.
    await cdp.send('Page.bringToFront').catch(() => {});

    // Wipe anything a previous run left on this origin before loading. The engine caches every
    // downloaded file in IndexedDB keyed by SAVEPATH, and ports are reused between runs, so a
    // stale cache is read by the next engine to land on the same port.
    const origin = new URL(url).origin;
    await cdp.send('Storage.clearDataForOrigin', {
      origin,
      storageTypes: 'indexdb,websql,locals_storage,service_workers,cache_storage',
    }).catch(() => {});

    await cdp.send('Page.navigate', { url });
    await sleep(600);

    // A watchdog on the evaluate itself. `Cdp.send` rejects only when the browser answers with
    // an error; a page that never answers leaves the promise pending and the script hanging
    // with no output, which is strictly worse than a failure. The budget is the settle time
    // plus generous slack for the samples themselves.
    const budget = waitMs + 60000;
    const result = await Promise.race([
      cdp.evaluate(PROBE(waitMs, 6)),
      new Promise((_, reject) => setTimeout(
        () => reject(new Error(`probe did not answer within ${Math.round(budget / 1000)} s`)),
        budget,
      )),
    ]);
    return { ...result, console: console_.slice(-20) };
  } finally {
    cdp.close();
    // Close only the tab this run opened, so the user's browser is left as it was found.
    await fetch(`http://127.0.0.1:${cdpPort}/json/close/${target.id}`).catch(() => {});
  }
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
async function main() {
  const args = parseArgs(process.argv.slice(2));
  if (args.help) return usage();
  if (!args.game || !args.engine) {
    usage();
    throw new Error('--game and --engine are both required');
  }
  if (!existsSync(join(args.game, 'system.ini'))) {
    throw new Error(`${args.game}/system.ini not found; run ports/artemis/tools/extract-pfs.mjs first`);
  }
  if (!existsSync(args.engine)) throw new Error(`candidate not found: ${args.engine}`);
  if (!existsSync(DEPLOY)) throw new Error(`deployer not found: ${DEPLOY}`);

  const CdpPort = args.cdpPort;
  const work = mkdtempSync(join(tmpdir(), 'artc-real-game-'));
  const control = join(work, 'control');
  const candidate = join(work, 'candidate');
  const controlPort = args.port;
  const candidatePort = args.port + 1;
  const game = resolve(args.game);

  console.error(`== deploying ==`);
  // A fresh cache key per run. `SAVEPATH` is the IndexedDB key the asyncify engine files its
  // downloads under, and an origin is scheme+host+port -- so reusing a port across runs reuses
  // the cache, and a run can silently read entries an earlier, different engine wrote there.
  // Deriving it from a random token keeps every run cold and makes two runs incomparable only
  // when they should be.
  const runToken = args.savepath || `artc_test_${randomBytes(4).toString('hex')}`;
  console.error(`  cache key: ${runToken}`);
  const common = ['--game', game, '--flavour', args.flavour, '--savepath', runToken];
  if (args.hashlist) common.push('--hashlist');
  if (!args.noControl) {
    console.error(`  control   (official ${args.flavour}) -> ${controlPort}`);
    await run(await findPython(), [DEPLOY, ...common, '--out', control, '--port', String(controlPort)]);
  }
  console.error(`  candidate (${args.engine}) -> ${candidatePort}`);
  await run(await findPython(), [
    DEPLOY, ...common, '--out', candidate, '--port', String(candidatePort),
    '--engine-wasm', resolve(args.engine),
  ]);

  // The engine maps a game path to `<ASYNCIFY_URL><path>`; the deployer writes an absolute
  // URL for its own port, so the trees are already pointed at themselves. What is left is to
  // expose the game at `/game/` in each tree, which the deployer expects the caller to do.
  const link = (tree) => {
    const target = join(tree, 'game');
    if (existsSync(target)) return;
    const rel = resolve(game);
    // A junction on Windows, a symlink elsewhere; both make `/game/...` resolve.
    const isWindows = process.platform === 'win32';
    const child = isWindows
      ? spawn('cmd', ['/c', 'mklink', '/J', target, rel], { stdio: 'ignore' })
      : spawn('ln', ['-s', rel, target], { stdio: 'ignore' });
    return new Promise((res) => child.on('exit', res));
  };
  if (!args.noControl) await link(control);
  await link(candidate);

  const python = await findPython();
  const servers = [];
  const report = { flavour: args.flavour, engine: resolve(args.engine), control: null, candidate: null };

  try {
    console.error(`== serving ==`);
    if (!args.noControl) servers.push(await serve(python, control, controlPort));
    servers.push(await serve(python, candidate, candidatePort));

    console.error(`== driving Chrome ==`);
    // The user's own Chrome if it is already up (same profile, same flags the MCP launcher
    // would use), otherwise one is started detached and left running.
    const info = await ensureChrome(CdpPort);
    console.error(`     connected to ${info.Browser || 'Chrome'} on CDP ${CdpPort}`);

    if (!args.noControl) {
      console.error(`== measuring the official build (this settles on its own) ==`);
      report.control = await measure(CdpPort, `http://127.0.0.1:${controlPort}/web/artemis.html`, args.wait);
    }

    console.error(`== measuring the candidate ==`);
    report.candidate = await measure(CdpPort, `http://127.0.0.1:${candidatePort}/web/artemis.html`, args.wait);
  } finally {
    if (!args.keep) for (const s of servers) s.kill();
  }

  const controlRun = report.control;
  const candidateRun = report.candidate;

  // A control that drew nothing invalidates the comparison, and saying so is more useful
  // than reporting a candidate "failure" that only reflects a broken bench.
  const stalled = (r) => r && !r.error && r.samples > 0 && r.frames === 1 && r.hash === 'stalled';
  // With --no-control there is no official tab to vouch for the bench, so the candidate is
  // judged on its own signals (it fetched the game and drew something).
  const controlOk = args.noControl ? true
    : Boolean(controlRun && !controlRun.error && !stalled(controlRun)
      && controlRun.max >= 16 && controlRun.gameRequests > 0);
  const candidateRendered = candidateRun && !candidateRun.error && candidateRun.max >= 16;
  const candidateRanGame = candidateRun && candidateRun.gameRequests > 0;
  // Compare the *settled* frames, not the best ones. The best frame is whatever happened to be
  // brightest during the run, so it lands on a different animation phase depending on how long
  // each tab was sampled -- two identical engines can easily pick different "best" frames. The
  // settled frame is the one both engines converge on, and that is the one that must match.
  const sameFrame = !args.noControl && controlOk && candidateRendered
    && candidateRun.finalHash != null && candidateRun.finalHash === controlRun.finalHash;

  report.controlSkipped = Boolean(args.noControl);
  report.controlOk = controlOk;
  report.candidateRendered = candidateRendered;
  report.candidateRanGame = candidateRanGame;
  report.pixelIdentical = sameFrame;
  report.pass = Boolean(controlOk && candidateRendered && candidateRanGame);

  if (args.jsonOut) {
    writeFileSync(args.jsonOut, JSON.stringify(report, null, 2));
    console.error(`report written to ${args.jsonOut}`);
  }
  if (args.json) {
    console.log(JSON.stringify(report, null, 2));
  } else {
    const row = (label, r) => {
      if (!r) return console.log(`  ${label.padEnd(10)} (no result)`);
      if (r.error) return console.log(`  ${label.padEnd(10)} ERROR ${r.error}`);
      console.log(
        `  ${label.padEnd(10)} canvas ${String(r.canvas).padEnd(9)} ` +
          `mean ${String(r.mean).padStart(7)}  max ${String(r.max).padStart(3)}  ` +
          `lit ${String(r.lit).padStart(6)}%  frames ${String(r.frames).padStart(2)}  ` +
          `settled ${String(r.finalHash).padEnd(9)}  game ${String(r.gameRequests).padStart(4)} files`,
      );
    };
    console.log(`\n== real game through the official page ==`);
    if (args.noControl) console.log('  official   (skipped: --no-control)');
    else row('official', controlRun);
    row('candidate', candidateRun);
    if (controlRun && typeof controlRun.rafTicks === 'number') {
      console.log(`  frame loop ran ${controlRun.rafTicks} ticks on the official tab, ` +
        `${candidateRun && candidateRun.rafTicks} on the candidate tab`);
    }
    if (candidateRun && typeof candidateRun.trapCode === 'number' && candidateRun.trapCode >= 0) {
      console.log(`  candidate trapped with code ${candidateRun.trapCode} ` +
        `(1 = out-of-bounds or call-stack exhaustion, 5 = explicit unreachable)`);
    } else if (candidateRun && candidateRun.trapCode === -1) {
      console.log("  candidate recorded no trap (-1 is the probe's never-trapped sentinel)");
    }
    if (candidateRun && candidateRun.oob && candidateRun.oob.offset !== undefined) {
      console.log(`  candidate refused access: offset ${candidateRun.oob.offset} ` +
        `len ${candidateRun.oob.len} against memory size ${candidateRun.oob.size}`);
    }
    if (candidateRun && candidateRun.console && candidateRun.console.length) {
      console.log(`  candidate console (last ${candidateRun.console.length}):`);
      for (const line of candidateRun.console) console.log(`    ${line}`);
    }
    console.log('');
    if (stalled(controlRun) || stalled(candidateRun)) {
      console.log('FAIL: the frame loop stalled -- requestAnimationFrame never fired inside the');
      console.log('      probe. That is a bench problem (hidden or throttled tab), not a module');
      console.log('      problem: nothing about either engine is established by this run.');
    } else if (!controlOk) {
      console.log('FAIL: the official build did not render on this bench, so nothing about');
      console.log('      the candidate is established. Check --game and the console output.');
    } else if (!candidateRanGame) {
      console.log('FAIL: the candidate never fetched the game. It did not reach the point of');
      console.log('      loading a real title, so it cannot be called working.');
    } else if (!candidateRendered) {
      console.log('FAIL: the candidate fetched the game but drew nothing.');
    } else if (args.noControl) {
      console.log('PASS: the candidate rendered the real game (official control skipped).');
    } else if (sameFrame) {
      console.log('PASS: the candidate rendered the real game, and its settled frame is');
      console.log('      pixel-identical to the official build\'s.');
    } else {
      console.log('PASS: the candidate rendered the real game. Its settled frame differs from');
      console.log('      the official build\'s, which may be timing rather than behaviour -- run');
      console.log('      with --json and compare hashes across runs before reading into it.');
    }
  }

  if (!args.keep) {
    // Removing a tree that holds Windows junctions can fail with EPERM while the mapping is
    // still being torn down. Retry briefly; and a cleanup failure must never change the
    // verdict, which is why this is caught rather than left to throw.
    for (let attempt = 0; attempt < 5; attempt += 1) {
      try {
        rmSync(work, { recursive: true, force: true });
        break;
      } catch (e) {
        if (attempt === 4) console.error(`note: could not remove ${work}: ${e.message}`);
        else await sleep(400);
      }
    }
  }
  else console.error(`\nkept: ${work}  (control :${controlPort}, candidate :${candidatePort})`);

  process.exit(report.pass ? 0 : 1);
}

main().catch((error) => {
  console.error(`error: ${error.message}`);
  process.exit(2);
});
