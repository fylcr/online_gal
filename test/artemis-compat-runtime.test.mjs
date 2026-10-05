/**
 * Independent challenge for the public compat runtime facade.
 *
 * Everything here runs against a **fake worker**: the engine, the canvas transfer and the compositor
 * are all absent, so this file proves the facade's own contract — descriptor discovery, the host
 * frame clock, lifecycle ordering, the input mapping and the honest save refusal — and proves nothing
 * at all about a real game. Run: `node --test test/artemis-compat-runtime.test.mjs`.
 *
 * The fake worker answers with the **real** frame-ok shape (`started`/`presented`/`revision`/`stage`)
 * and with `presented` genuinely distinct from the frame count, so a facade that invented a
 * presented counter per acknowledged frame cannot pass here.
 *
 * Like the worker test, this owns one mkdtemp subtree and compiles the package's `index.ts` into it,
 * so a clean clone with no `.test/` workbench still runs it.
 */
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, readFileSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const PACKAGE = join(root, 'packages', 'runtime-artemis');
const temp = mkdtempSync(join(tmpdir(), 'artemis-compat-runtime-'));
const out = join(temp, 'out');
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-compat-runtime-')) {
    rmSync(temp, { recursive: true, force: true });
  }
});
execFileSync(process.execPath, [
  join(root, 'node_modules/typescript/bin/tsc'),
  'packages/runtime-artemis/src/index.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--noUncheckedIndexedAccess', '--skipLibCheck',
  '--rootDir', 'packages/runtime-artemis/src', '--outDir', out,
], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(PACKAGE, 'node_modules'), join(out, 'node_modules'), 'junction');
const api = await import(pathToFileURL(join(out, 'index.js')).href);

/* --------------------------------------------------------------------------------------------- */
/* A deterministic environment: no canvas transfer, no DOM, no audio.                             */
/* --------------------------------------------------------------------------------------------- */

class FakeOffscreen {
  static instances = [];
  constructor() {
    FakeOffscreen.instances.push(this);
  }
}
globalThis.OffscreenCanvas = FakeOffscreen;
globalThis.crossOriginIsolated = true;
globalThis.SharedArrayBuffer = globalThis.SharedArrayBuffer ?? class {};

/**
 * The browser's own refusal, which is the entire finding this file exists to pin.
 *
 * A fake worker that records whatever it is handed cannot see the defect: `HTMLCanvasElement` is not
 * a transferable object, and the real `postMessage` answers with a `DataCloneError` **before** the
 * worker ever receives the message. A Canvas these classes refuse stands in for that check, so the
 * assertion is about the post, not only about the source.
 */
class DOMException extends Error {
  constructor(message, name = 'Error') {
    super(message);
    this.name = name;
  }
}

const isTransferable = (value) =>
  value instanceof FakeOffscreen || value instanceof globalThis.SharedArrayBuffer;

/**
 * An `HTMLCanvasElement`, modelled on the two facts the transport depends on.
 *
 * `transferControlToOffscreen` is the conversion, and it is **one-shot** on the real platform: a
 * second call throws `InvalidStateError`, and so does the call after `getContext()` has been used.
 * The fixture reproduces both, because a facade that transferred per load instead of per session
 * would otherwise look identical to one that transfers once.
 *
 * `canvas.transferred` is what the conversion produced. It is deliberately **not** on the init
 * message, so a facade that posts the element and lets the fixture's `postMessage` be forgiving
 * cannot pass: the post itself is what refuses a non-transferable value.
 */
function makeCanvas({ transferable = true, width = 640, height = 360, contextUsed = false } = {}) {
  const canvas = {
    width,
    height,
    ownerDocument: null,
    getContext: () => null,
    getBoundingClientRect: () => ({ left: 0, top: 0, width, height }),
  };
  canvas.transfers = 0;
  if (!transferable) return canvas;
  if (contextUsed) {
    // The precondition the platform states: a canvas that already handed out a context cannot be
    // retargeted, and the transfer is what throws — not the post.
    canvas.transferControlToOffscreen = () => {
      canvas.transfers += 1;
      throw new DOMException('canvas has an existing context', 'InvalidStateError');
    };
    return canvas;
  }
  canvas.transferControlToOffscreen = () => {
    if (canvas.transfers > 0) {
      throw new DOMException('already transferred', 'InvalidStateError');
    }
    canvas.transfers += 1;
    const offscreen = new FakeOffscreen();
    offscreen.width = canvas.width;
    offscreen.height = canvas.height;
    offscreen.getContext = () => null;
    canvas.transferred = offscreen;
    return offscreen;
  };
  return canvas;
}

/**
 * A host canvas that is **already** an `OffscreenCanvas`.
 *
 * It is the other transferable shape: no `transferControlToOffscreen` at all, because the platform
 * does not define one on `OffscreenCanvas`. Requiring the method would refuse this host, and calling
 * it defensively would crash — which is why the gate is an OR and the conversion is conditional.
 */
function makeOffscreenHostCanvas() {
  const offscreen = new FakeOffscreen();
  offscreen.width = 640;
  offscreen.height = 360;
  offscreen.getContext = () => null;
  return offscreen;
}

function makeSource({ packs = ['root.pfs'], sizes = {}, readRange = true, list } = {}) {
  const reads = [];
  return {
    reads,
    source: {
      name: 'fixture',
      capabilities: { list: true, randomAccess: readRange, writable: false },
      async list(dir = '') {
        if (list) return list(dir);
        return dir === '' ? packs : [];
      },
      async readFile(path) {
        throw new Error(`the compat transport must not read whole files (${path})`);
      },
      ...(readRange
        ? {
            async readRange(path, offset, length) {
              reads.push({ path, offset, length });
              return new Uint8Array(length);
            },
          }
        : {}),
      async size(path) {
        if (path in sizes) return sizes[path];
        return 4096;
      },
      async exists() {
        return true;
      },
      async isDirectory() {
        return false;
      },
    },
  };
}

function makeHost({ canvas = makeCanvas(), source } = {}) {
  const logs = [];
  const notifications = [];
  const paints = [];
  const parts = source ?? makeSource();
  const gameSource = parts.source ?? parts;
  return {
    canvas,
    source: gameSource,
    reads: parts.reads ?? [],
    logs,
    notifications,
    paints,
    host: {
      canvas,
      source: gameSource,
      store: { name: 'mem', async read() {}, async write() {}, async remove() {}, async list() { return []; } },
      surface: { width: 640, height: 360 },
      requestFrame() {
        paints.push(1);
      },
      log(level, message, data) {
        logs.push({ level, message, data });
      },
      now: () => 0,
      notify(event) {
        notifications.push(event);
      },
      setting: (_key, fallback) => fallback,
    },
  };
}

/**
 * The page-side transport's own dependencies are imported from the same compiled tree; only the
 * worker is replaced, so the facade is exercised against the real `CompatWorkerHost`.
 */
const { CompatWorkerHost } = api;
void CompatWorkerHost;

/** A worker-shaped object the real `CompatWorkerHost` accepts. */
function fakeWorker(script) {
  const listeners = new Map();
  const worker = {
    posted: [],
    transfers: [],
    terminated: false,
    addEventListener(type, fn) {
      listeners.set(type, [...(listeners.get(type) ?? []), fn]);
    },
    postMessage(message, transfer = []) {
      // The structured clone step, before the worker sees anything. A value that is not transferable
      // is refused *here* — a facade that posts the HTML element with the cast still in place fails
      // this, and a data-size DataCloneError is exactly what the real browser throws.
      for (const value of transfer) {
        if (!isTransferable(value)) {
          throw new DOMException(
            `${value?.constructor?.name ?? typeof value} object could not be cloned`,
            'DataCloneError',
          );
        }
      }
      worker.posted.push(message);
      worker.transfers.push(transfer);
      // `emit` delivers one worker reply to the host's `message` listener. It must carry the event
      // shape (`{ data }`), because that is what the listener reads — a bare payload makes every
      // reply arrive as `undefined.data` and the host times out on a message it did receive.
      script?.({ worker, message, emit: (data) => fire('message', { data }) });
    },
    terminate() {
      worker.terminated = true;
    },
  };
  const fire = (type, event) => {
    for (const fn of listeners.get(type) ?? []) fn(event);
  };
  worker.emit = (data) => fire('message', { data });
  worker.fail = (error) => fire('error', error);
  return worker;
}

/**
 * Answers every message the worker host sends, so a session reaches `ready` and can be driven.
 *
 * `frame-ok` carries the **native** counters, exactly as the real worker reports them. `presented`
 * deliberately lags `revision`: the engine's redraw gate skips a present on a static scene, and this
 * fake reproduces that so a facade that counted a present per acknowledged frame would disagree.
 */
function readyScript({ onFrame, presented = null, started = 1, revision = null } = {}) {
  let frames = 0;
  return ({ worker, message, emit }) => {
    if (message.type === 'init') {
      // Reported asynchronously, exactly as a worker would.
      queueMicrotask(() => emit({ type: 'ready', id: message.id }));
      return;
    }
    if (message.type === 'frame') {
      const gate = onFrame?.();
      if (gate === false) return;
      frames += 1;
      // A static scene: revision moves, the redraw gate does not present.
      const nativePresented = presented === null ? frames - 1 : presented;
      const nativeRevision = revision === null ? frames : revision;
      queueMicrotask(() =>
        emit({
          type: 'frame-ok',
          id: message.id,
          running: true,
          started,
          presented: nativePresented,
          revision: nativeRevision,
          stage: 1280,
          stageHeight: 720,
          nowMs: 1000 + frames * 16,
        }),
      );
      return;
    }
    if (message.type === 'pause') {
      queueMicrotask(() => emit({ type: 'ack', id: message.id }));
      return;
    }
    if (message.type === 'input') {
      queueMicrotask(() => emit({ type: 'input-ok', id: message.id, count: 1 }));
      return;
    }
    if (message.type === 'dispose') {
      queueMicrotask(() => emit({ type: 'disposed-ack', id: message.id }));
    }
  };
}

const flush = async () => {
  for (let n = 0; n < 30; n += 1) await Promise.resolve();
  await new Promise((resolve) => setImmediate(resolve));
};

/**
 * Installs a worker factory on the host's `createWorker` seam for the duration of one session.
 *
 * The session is not up until the runtime has driven one frame — the engine's Lua framework boots
 * inside `artc_web_frame`, not inside `artc_web_open` — so a test that just awaits `load()` gets a
 * session with exactly one frame already spent on the boot. `frames` counts everything the worker
 * saw, boot included.
 */
async function rig(t, { hostParts = makeHost(), script = readyScript(), gameConfig } = {}) {
  const workers = [];
  // The runtime builds its own `CompatWorkerHost`; the only seam a test has is `new Worker` itself.
  const realWorker = globalThis.Worker;
  globalThis.Worker = function Worker(url) {
    const worker = fakeWorker(script);
    worker.url = String(url);
    workers.push(worker);
    return worker;
  };
  t.after(() => {
    globalThis.Worker = realWorker;
  });
  // The per-game `engineBase` is what the *provider default* reads when no deployment engine was
  // published, so the deployment engine is published here and the game names none: that is exactly
  // the shape a page uses, and it makes the resolved URL the publication's, not the game's.
  api.configureArtemisCompat({ engineBase: 'https://cdn.invalid/artemis/' });
  const runtime = api.artemisProvider.create(hostParts.host, {
    runtimeId: 'artemis',
    surface: hostParts.host.surface,
    mode: 'new',
    ...(gameConfig === undefined ? {} : { gameConfig }),
  });
  return { ...hostParts, workers, runtime };
}

/** Frames the worker was asked for, boot frame included. */
const framesOf = (worker) => worker.posted.filter((m) => m.type === 'frame').length;

/* --------------------------------------------------------------------------------------------- */

test('runtime load forwards only boolean true animationTrace and leaves default off absent', async t => {
  for (const animationTrace of [undefined, false, '1', true]) {
    const env = await rig(t, { gameConfig: animationTrace === undefined ? {} : { animationTrace } });
    try {
      await env.runtime.load();
      const init = env.workers[0].posted.find(message => message.type === 'init');
      if (animationTrace === true) assert.equal(init.animationTrace, true);
      else assert.equal(Object.hasOwn(init, 'animationTrace'), false);
    } finally {
      env.runtime.dispose(); await env.runtime.whenDisposed();
    }
  }
});

test('the engine resolves to three DISTINCT urls: the RPC worker is never the generated glue', () => {
  api.configureArtemisCompat({ engineBase: 'https://cdn.invalid/artemis/' });
  assert.equal(api.hasArtemisCompatEngine(), true);
  const engine = api.resolveCompatEngine(api.readArtemisCompatConfig());
  // The glue and the binary live in the configured directory.
  assert.equal(engine.engineJsUrl, 'https://cdn.invalid/artemis/artemis-compat.js');
  assert.equal(engine.wasmUrl, 'https://cdn.invalid/artemis/artemis-compat.wasm');
  // The RPC worker does NOT: it is the package's own `dist/compat/compat-worker.js`, and the whole
  // defect this shape removes was that both fields answered to the same glue URL.
  assert.match(engine.workerUrl, /compat-worker\.js$/, 'the worker is the RPC script');
  assert.ok(
    !engine.workerUrl.endsWith('artemis-compat.js'),
    'the worker must never be the generated glue: new Worker(glue) loads a module that never answers',
  );
  const urls = [engine.workerUrl, engine.engineJsUrl, engine.wasmUrl];
  assert.equal(new Set(urls).size, 3, `the three engine URLs must be distinct, got ${JSON.stringify(urls)}`);
});

test('a withdrawn engine is a withdrawal, and a bare configure() publishes the bundled default again', () => {
  api.configureArtemisCompat({ engineBase: null });
  assert.equal(api.hasArtemisCompatEngine(), false, 'a withdrawn engine is not an engine');
  assert.equal(api.artemisProvider.availability, 'unavailable');

  // `configureArtemisCompat()` with nothing is a REAL publication: the only way back from a
  // withdrawal. Treating it as a no-op would leave the provider unavailable forever.
  const generation = api.artemisCompatGeneration();
  api.configureArtemisCompat();
  assert.equal(api.artemisCompatGeneration(), generation + 1, 'a bare configure() is a publication');
  assert.equal(api.hasArtemisCompatEngine(), true, 'the bundled default is back');
  const bundled = api.resolveCompatEngine(api.readArtemisCompatConfig());
  assert.match(bundled.engineJsUrl, /assets\/artemis-compat\.js$/);
  assert.match(bundled.wasmUrl, /assets\/artemis-compat\.wasm$/);
  assert.match(bundled.workerUrl, /compat\/compat-worker\.js$/);

  // `configure({})` is the same publication.
  api.configureArtemisCompat({});
  assert.equal(api.hasArtemisCompatEngine(), true);
});

test('a game-level engineBase that is not a locator is a hard error, and the config is frozen', () => {
  // A malformed per-game engine is a configuration error. Withdrawing it silently would be
  // indistinguishable from the game having asked for no engine.
  assert.throws(
    () => api.configureArtemisCompat({ engineBase: 'artemis' }),
    /neither a URL nor a path/,
    'a virtual directory name must not silently become "no engine"',
  );
  assert.throws(() => api.configureArtemisCompat({ engineBase: 42 }), TypeError);

  api.configureArtemisCompat({ engineBase: 'https://cdn.invalid/artemis/' });
  const published = api.readArtemisCompatConfig();
  assert.equal(Object.isFrozen(published), true, 'the publication is a frozen copy');
  assert.throws(() => {
    published.engineBase = 'https://evil.invalid/';
  }, TypeError);
  // Mutating the copy must not move the publication.
  assert.equal(api.resolveCompatEngine(api.readArtemisCompatConfig()).engineJsUrl, 'https://cdn.invalid/artemis/artemis-compat.js');
});

test('an override that collapses two engine URLs into one is refused, not silently accepted', () => {
  // The exact defect: the worker pointing at the glue. An engine whose worker is not a worker is not
  // an engine, so it must not be advertised as one.
  api.configureArtemisCompat({
    engineBase: 'https://cdn.invalid/artemis/',
    workerUrl: 'https://cdn.invalid/artemis/artemis-compat.js',
  });
  assert.equal(api.hasArtemisCompatEngine(), false, 'a worker handed the glue must not resolve');
  api.configureArtemisCompat({ engineBase: 'https://cdn.invalid/artemis/' });
  assert.equal(api.hasArtemisCompatEngine(), true);
});

test('an unconfigured engine is a refusal, and the provider says so without a play button', async () => {
  api.configureArtemisCompat({ engineBase: 'https://cdn.invalid/artemis/' });
  const parts = makeHost();
  const runtime = api.artemisProvider.create(parts.host, {
    runtimeId: 'artemis',
    surface: parts.host.surface,
    mode: 'new',
    gameConfig: { engineBase: null },
  });
  // The deployment engine is still advertised, but this *game* asked for none: it is unavailable.
  assert.equal(runtime.describe().available, false);
  await assert.rejects(() => runtime.load(), /cannot run in this page/);
  assert.equal(runtime.report().severity, 'unsupported');
  runtime.dispose();
  await runtime.whenDisposed();

  // With no deployment engine either, the provider itself stops advertising a play button.
  api.configureArtemisCompat({ engineBase: null });
  assert.equal(api.artemisProvider.availability, 'unavailable');
});

test('a configured engine is advertised, and an unconfigured one is not', () => {
  api.configureArtemisCompat({ engineBase: 'https://cdn.invalid/artemis/' });
  const parts = makeHost();
  const runtime = api.artemisProvider.create(parts.host, {
    runtimeId: 'artemis',
    surface: parts.host.surface,
    mode: 'new',
    gameConfig: { engineBase: 'https://games.invalid/engine/' },
  });
  assert.equal(runtime.runtimeId, 'artemis');
  assert.equal(runtime.describe().engine, 'compat');
  assert.equal(runtime.describe().engineSource, 'configured');
  assert.equal(runtime.describe().available, true);
  assert.equal(api.artemisProvider.availability, 'runnable');
  runtime.dispose();
});

test('the provider defaults to compat, and only an explicit per-game bridge reaches the legacy runtime', () => {
  api.configureArtemisCompat({ engineBase: 'https://cdn.invalid/artemis/' });
  const parts = makeHost();
  const compat = api.artemisProvider.create(parts.host, {
    runtimeId: 'artemis',
    surface: parts.host.surface,
    mode: 'new',
  });
  assert.equal(compat.constructor.name, 'CompatArtemisRuntime');
  compat.dispose();

  // A published legacy bridge does **not** move the default: the compat engine is what runs.
  api.configureArtemisWasm({ wasm: new Uint8Array([0, 97, 115, 109]) });
  const stillCompat = api.artemisProvider.create(parts.host, {
    runtimeId: 'artemis',
    surface: parts.host.surface,
    mode: 'new',
  });
  assert.equal(stillCompat.constructor.name, 'CompatArtemisRuntime');
  stillCompat.dispose();

  const legacy = api.artemisProvider.create(parts.host, {
    runtimeId: 'artemis',
    surface: parts.host.surface,
    mode: 'new',
    gameConfig: { wasm: new Uint8Array([0, 97, 115, 109]) },
  });
  assert.equal(legacy.constructor.name, 'ArtemisRuntime');
  legacy.dispose();
  api.configureArtemisWasm({ wasm: null });
});

test('the pack chain is described by size alone: root.pfs first, continuations after, nothing read', async () => {
  const { source, reads } = makeSource({
    packs: ['root.pfs.001', 'movie.pfs', 'root.pfs', 'root.pfs.000', 'readme.txt'],
    sizes: { 'root.pfs': 2_800_000_000, 'root.pfs.000': 3_000_000_000, 'root.pfs.001': 1024, 'movie.pfs': 64 },
  });
  const descriptors = await api.describeGame(source);
  assert.deepEqual(
    descriptors.map((entry) => [entry.id, entry.path, entry.virtualPath]),
    [
      [1, 'root.pfs', '/game/root.pfs'],
      [2, 'root.pfs.000', '/game/root.pfs.000'],
      [3, 'root.pfs.001', '/game/root.pfs.001'],
      [4, 'movie.pfs', '/game/movie.pfs'],
    ],
  );
  assert.equal(descriptors[0].size, 2_800_000_000);
  assert.ok(descriptors[0].size > 2 ** 31, 'a pack past 2 GiB keeps its exact size');
  assert.deepEqual(reads, [], 'describing a chain must not read a single byte of it');
});

test('a directory with one pack and no root.pfs still opens it as the engine pack', async () => {
  const { source } = makeSource({ packs: ['data.pfs'] });
  const descriptors = await api.describeGame(source);
  assert.deepEqual(descriptors.map((entry) => entry.virtualPath), ['/game/root.pfs']);
  assert.equal(descriptors[0].path, 'data.pfs');
});

test('a source with no pack, and one that cannot read ranges, are both refused by name', async () => {
  await assert.rejects(() => api.describeGame(makeSource({ packs: ['readme.txt'] }).source), /no pack in fixture/);
  await assert.rejects(() => api.describeGame(makeSource({ readRange: false }).source), /cannot/);
});

test('load hands the worker the THREE urls, transfers the canvas, and drives one boot frame', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await runtime.load();
  assert.equal(runtime.describe().loaded, true);
  const worker = workers[0];
  // `new Worker` took the RPC script, not the glue. This is the whole point of the URL split.
  assert.match(worker.url, /compat-worker\.js$/, 'the worker constructor got the RPC script');
  assert.ok(!worker.url.endsWith('artemis-compat.js'), 'new Worker must not be handed the glue');
  const init = worker.posted[0];
  assert.equal(init.type, 'init');
  assert.equal(init.wasmUrl, 'https://cdn.invalid/artemis/artemis-compat.wasm');
  // `importScripts` gets the glue — a URL that is *different* from the one `new Worker` got.
  assert.equal(init.engineJsUrl, 'https://cdn.invalid/artemis/artemis-compat.js');
  assert.notEqual(init.engineJsUrl, worker.url, 'the glue the worker imports is not the worker itself');
  // The worker gets the **converted** object, never the element. `HTMLCanvasElement` is not
  // transferable, and the post above would already have refused it with a DataCloneError.
  assert.notEqual(init.canvas, parts.canvas, 'the element itself must never be posted: it cannot be cloned');
  assert.equal(init.canvas, parts.canvas.transferred, 'the worker is handed what the conversion produced');
  assert.ok(init.canvas instanceof FakeOffscreen, 'and that is an OffscreenCanvas, not a stand-in');
  assert.notEqual(init.canvas, null, 'a real page always has a canvas to transfer');
  assert.equal(parts.canvas.transfers, 1, 'the element is retargeted exactly once per session');
  assert.deepEqual(worker.transfers[0], [parts.canvas.transferred], 'the OFFSCREEN object is the transfer list');
  assert.deepEqual(init.descriptors.map((d) => d.virtualPath), ['/game/root.pfs']);
  assert.ok(init.block instanceof SharedArrayBuffer, 'the shared window reaches the worker');
  // The host's surface reaches the engine before the boot frame, so the first frame letterboxes
  // against the canvas the page actually has.
  assert.deepEqual(
    worker.posted.filter((m) => m.type === 'input').map((m) => [m.kind, m.width, m.height]),
    [['resize', 640, 360]],
  );

  // Exactly one frame so far: the boot frame the load itself drove. Nothing else has ticked.
  assert.equal(framesOf(worker), 1, 'the load drives exactly one boot frame and no more');

  // The native counters in `describe()` are the engine's, and the load verified `started === 1`.
  let workerState = runtime.describe().worker;
  assert.equal(workerState.started, 1, 'the boot frame reported the engine as started');
  // `presented` is the worker's number. The fake reports `frames - 1`, so a facade that added one
  // per acknowledged frame would read 1 here instead of 0.
  assert.equal(workerState.presented, 0, 'presented is the native count, not a page-side counter');
  assert.equal(workerState.revision, 1, 'revision is the native count');
  assert.equal(workerState.frameClock, 1, 'frameClock is the one number the page owns');
  // The stage is reported on both axes. A width alone cannot describe a letterbox, and a facade
  // that forwarded only `stage` leaves a reader unable to tell a 1280x720 stage from a 1280x0 one.
  assert.equal(workerState.stage, 1280, 'the native stage width is forwarded');
  assert.equal(workerState.stageHeight, 720, 'the native stage height is forwarded, not dropped');

  runtime.tick(16);
  runtime.tick(16);
  assert.equal(framesOf(worker), 2, 'two 16 ms ticks cross one 16.7 ms interval exactly once');
  await flush();
  runtime.tick(17);
  assert.equal(framesOf(worker), 3, 'one native frame per 60 Hz');
  await flush();
  workerState = runtime.describe().worker;
  assert.equal(workerState.frameClock, 3);
  assert.equal(workerState.presented, 2, 'still the native number, one behind the frame count');
  assert.equal(workerState.revision, 3);

  // `render` is not a second clock and does not ask the host for a repaint.
  runtime.render();
  assert.deepEqual(parts.paints, [], 'the worker draws into the canvas; render() must not schedule one');
  runtime.dispose();
  await runtime.whenDisposed();
  assert.equal(worker.terminated, true);
});

/* --------------------------------------------------------------------------------------------- */
/* The canvas is CONVERTED, not asserted: HTMLCanvasElement -> OffscreenCanvas.                    */
/*                                                                                                 */
/* The defect these pin: the load posted `canvas as unknown as OffscreenCanvas`. A TypeScript       */
/* assertion is a claim, and its only runtime effect was a DataCloneError inside postMessage. A     */
/* fake worker accepts anything, so the cast passed; the real browser refused every session.       */
/* --------------------------------------------------------------------------------------------- */

test('an HTML canvas is converted with transferControlToOffscreen, once, before the post', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await runtime.load();
  const canvas = parts.canvas;
  const worker = workers[0];
  const init = worker.posted[0];

  assert.equal(canvas.transfers, 1, 'exactly one conversion per session');
  assert.ok(canvas.transferred instanceof FakeOffscreen, 'the conversion produced a real OffscreenCanvas');
  assert.equal(init.canvas, canvas.transferred, 'the worker is handed the OFFSCREEN result');
  assert.deepEqual(worker.transfers[0], [canvas.transferred], 'and it is that object that travels');

  // The element stays the host's own. Replacing it would unbind every handler the host attached.
  assert.equal(parts.host.canvas, canvas, 'the host keeps its reference to the element');
  assert.equal(canvas.ownerDocument, null, 'the element is not moved into another document');
  assert.equal(canvas.width, 640, 'the surface is applied through the element the host sized');

  // The transfer happens before the post, because an attribute written after it is inert.
  assert.equal(init.type, 'init');
  runtime.dispose();
  await runtime.whenDisposed();
});

test('two concurrent loads are one load: one conversion, one worker, one init', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  // `load()` memoizes its promise. A second conversion would throw `InvalidStateError` on the real
  // platform, so this is not a cosmetic count: it is the difference between a session and a crash.
  await Promise.all([runtime.load(), runtime.load(), runtime.load()]);
  assert.equal(parts.canvas.transfers, 1, 'concurrent loads perform exactly one conversion');
  assert.equal(workers.length, 1, 'and build exactly one worker');
  assert.equal(workers[0].posted.filter((m) => m.type === 'init').length, 1, 'and post exactly one init');
  assert.equal(workers[0].posted[0].canvas, parts.canvas.transferred, 'carrying the converted object');
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a host canvas that is already an OffscreenCanvas is posted as-is, with no phantom method call', async (t) => {
  const offscreen = makeOffscreenHostCanvas();
  // The platform defines no `transferControlToOffscreen` on an OffscreenCanvas. A facade that calls
  // it "defensively" crashes here; a facade that requires it in the gate refuses this host.
  assert.equal(offscreen.transferControlToOffscreen, undefined);
  const parts = makeHost({ canvas: offscreen });
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await runtime.load();
  const init = workers[0].posted[0];
  assert.equal(init.canvas, offscreen, 'the already-offscreen canvas goes to the worker unchanged');
  assert.notEqual(init.canvas, null);
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a canvas that can neither convert nor is already offscreen is refused before a worker exists', async (t) => {
  const canvas = makeCanvas({ transferable: false });
  const parts = makeHost({ canvas });
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await assert.rejects(() => runtime.load(), /cannot run in this page/);
  // The row is the diagnosis, and it must name the canvas rather than the generic sentence: a gate
  // that says "OffscreenCanvas is missing" for an element that simply cannot be retargeted sends a
  // reader to the wrong global. This is the OR the gate needs — a global alone is not a canvas.
  assert.match(runtime.describe().missingCapabilities.join(' '), /transferable|transfer/);
  assert.deepEqual(workers, [], 'no worker may be built for a canvas that cannot be transferred');
  assert.equal(canvas.transfers, 0, 'a refused canvas must not be retargeted on the way out');
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a canvas whose context is already in use is refused at the conversion, not with a phantom transfer', async (t) => {
  // The platform states this precondition and this is what it looks like: the conversion itself
  // throws. The load must carry that refusal out, not swallow it and post something else.
  const canvas = makeCanvas({ contextUsed: true });
  const parts = makeHost({ canvas });
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await assert.rejects(() => runtime.load(), /existing context/);
  assert.equal(runtime.describe().loaded, false, 'a canvas that could not be retargeted is not loaded');
  assert.equal(canvas.transfers, 1, 'the conversion was attempted once, and only once');
  assert.equal(canvas.transferred, undefined, 'and it produced nothing');
  for (const worker of workers) {
    assert.equal(
      worker.posted.filter((m) => m.type === 'init').length,
      0,
      'no init is posted for a canvas that never converted',
    );
    assert.equal(worker.terminated, true, 'and the worker built for it is released');
  }
  await runtime.whenDisposed();
});

test('a transfer that throws rejects the load and leaves no worker behind', async (t) => {
  const canvas = makeCanvas();
  canvas.transferControlToOffscreen = () => {
    canvas.transfers += 1;
    // The conversion can fail for reasons the page cannot see: a canvas that already gave out a
    // context, a detached frame. The failure must be the load's, not a silently absent worker.
    throw new DOMException('cannot transfer', 'InvalidStateError');
  };
  const parts = makeHost({ canvas });
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await assert.rejects(() => runtime.load(), /cannot transfer/);
  assert.equal(runtime.describe().loaded, false, 'a load whose transfer threw is not loaded');
  assert.equal(runtime.describe().failure !== null, true, 'and it is reported as a failure');
  // The worker was built before the transfer, so it must be torn down rather than leaked.
  for (const worker of workers) {
    assert.equal(worker.terminated, true, 'a worker built before a thrown transfer is released');
  }
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a transfer that throws never posts an init, and the session is disposed once however many callers ask', async (t) => {
  const canvas = makeCanvas();
  canvas.transferControlToOffscreen = () => {
    canvas.transfers += 1;
    throw new DOMException('nope', 'InvalidStateError');
  };
  const parts = makeHost({ canvas });
  const { runtime, workers } = await rig(t, { hostParts: parts });
  const outcome = runtime.load().then(
    () => null,
    (error) => error,
  );
  runtime.dispose();
  const result = await outcome;
  assert.ok(result instanceof Error, 'a disposed load does not resolve');
  for (const worker of workers) {
    assert.deepEqual(worker.posted, [], 'nothing was posted to a worker from a failed transfer');
  }
  await Promise.all([runtime.whenDisposed(), runtime.whenDisposed()]);
  assert.equal(runtime.describe().disposed, true);
});

test('a dispose that lands before the transfer is performed leaves the canvas alone', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  // Held open inside `describeGame`, so the dispose lands while the load is still awaiting the pack
  // chain — i.e. strictly before the transfer, which is the ordering the recheck exists for.
  let release;
  const held = new Promise((resolve) => {
    release = resolve;
  });
  const realList = parts.source.list;
  parts.source.list = async (dir = '') => {
    await held;
    return realList(dir);
  };
  const outcome = runtime.load().then(
    () => null,
    (error) => error,
  );
  await flush();
  runtime.dispose();
  release();
  const result = await outcome;
  assert.ok(result instanceof Error, 'the load does not resolve after a dispose');
  assert.match(String(result.message), /disposed/);
  assert.equal(parts.canvas.transfers ?? 0, 0, 'a load cancelled before the transfer never retargets the canvas');
  assert.equal(parts.canvas.transferred, undefined, 'and produces no offscreen object at all');
  for (const worker of workers) {
    assert.equal(worker.posted.filter((m) => m.type === 'init').length, 0, 'and posts no init');
  }
  await runtime.whenDisposed();
});

test('a load whose first frame never starts the engine is refused, not reported as loaded', async (t) => {
  const parts = makeHost();
  // The worker answers `ready` — the pack opened — but `artc_web_started` stays 0. The Lua framework
  // boots inside `artc_web_frame`, so this is a session that cannot be paused or driven.
  const { runtime } = await rig(t, { hostParts: parts, script: readyScript({ started: 0 }) });
  await assert.rejects(() => runtime.load(), /without starting/);
  assert.equal(runtime.describe().loaded, false, 'a session whose engine never started is not loaded');
  assert.deepEqual(parts.notifications.map((event) => event.type), ['fatal']);
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a missing capability refuses the load before any worker exists', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  // `crossOriginIsolated` is the shared-window precondition; without it `Atomics.wait` throws.
  const realIsolated = globalThis.crossOriginIsolated;
  globalThis.crossOriginIsolated = false;
  t.after(() => {
    globalThis.crossOriginIsolated = realIsolated;
  });
  await assert.rejects(() => runtime.load(), /cannot run in this page/);
  assert.deepEqual(workers, [], 'the capability gate runs before a worker is created');
  assert.match(runtime.describe().missingCapabilities.join(' '), /crossOriginIsolated/);
  globalThis.crossOriginIsolated = realIsolated;
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a source that cannot read ranges is refused by the capability gate', async (t) => {
  const parts = makeHost({ source: makeSource({ readRange: false }) });
  const { runtime } = await rig(t, { hostParts: parts });
  await assert.rejects(() => runtime.load(), /cannot run in this page/);
  assert.match(runtime.describe().missingCapabilities.join(' '), /readRange/);
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a stall cannot become a catch-up burst: one native frame per tick, however large the delta', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await runtime.load();
  const worker = workers[0];
  const boot = framesOf(worker);
  // A 1000 ms tick owes ~60 frames. The cap that actually bounds the guest is "one native frame per
  // tick() call", and it holds regardless of how the accumulator is carried:
  //
  //   - `accumulator %= FRAME_INTERVAL_MS` folds the backlog to under one interval;
  //   - `accumulator -= FRAME_INTERVAL_MS` banks ~983 ms of debt.
  //
  // Both run exactly one frame per call here, so this test pins the *observable* guarantee (never a
  // burst) and does not — and cannot — distinguish the two carry rules. The folding rule is what
  // keeps the carried debt bounded; it is stated in `tick()` and is deliberately not claimed as
  // measured behaviour beyond the burst bound below.
  runtime.tick(1000);
  await flush();
  assert.equal(framesOf(worker), boot + 1, 'a single tick advances at most one native frame');

  for (let n = 0; n < 12; n += 1) {
    runtime.tick(16);
    await flush();
  }
  const afterSmallTicks = framesOf(worker) - boot;
  assert.ok(
    afterSmallTicks <= 13,
    `13 ticks after a 1000 ms stall ran ${afterSmallTicks} frames: a stall became a burst`,
  );
  // ...and the clock is still live: the stall left it at the current moment, not stuck.
  const settled = framesOf(worker);
  runtime.tick(100);
  await flush();
  assert.equal(framesOf(worker), settled + 1, 'the frame clock keeps running after the stall');
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a paused session runs no frame and delivers no input; resume restores both', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await runtime.load();
  const worker = workers[0];
  runtime.pause();
  await flush();
  assert.deepEqual(
    worker.posted.filter((m) => m.type === 'pause').map((m) => m.paused),
    [true],
    'the main gate reaches the worker as an ordered pause',
  );
  const before = worker.posted.length;
  runtime.tick(16);
  runtime.tick(16);
  runtime.sendInput({ type: 'pointer-down', x: 10, y: 10, button: 0 });
  assert.equal(runtime.describe().paused, true);
  assert.equal(worker.posted.length, before, 'a paused session sends nothing to the worker');

  runtime.resume();
  await flush();
  assert.equal(runtime.describe().paused, false);
  const atResume = framesOf(worker);
  runtime.tick(17);
  await flush();
  assert.equal(framesOf(worker), atResume + 1, 'resume restores the clock');
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a pause that lands during an in-flight resume is not undone by that resume', async (t) => {
  const parts = makeHost();
  let release;
  const gate = new Promise((resolve) => {
    release = resolve;
  });
  const base = readyScript();
  const script = (event) => {
    // Only the pause is held open; everything else behaves normally.
    if (event.message.type === 'pause') {
      return void gate.then(() => event.emit({ type: 'ack', id: event.message.id }));
    }
    base(event);
  };
  const { runtime, workers } = await rig(t, { hostParts: parts, script });
  await runtime.load();
  const worker = workers[0];
  const boot = framesOf(worker);
  runtime.pause();
  await flush();
  runtime.resume();
  await flush();
  // The host pauses again while the worker is still acknowledging the resume.
  runtime.pause();
  release();
  await flush();
  assert.equal(runtime.describe().paused, true, 'the epoch guard keeps the later pause');
  runtime.tick(16);
  runtime.tick(16);
  assert.equal(framesOf(worker), boot, 'a stale resume must not revive the frame clock');
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a pause and a resume posted back to back reach the worker as one ordered latest-wins pair', async (t) => {
  const parts = makeHost();
  // The worker takes its time with the first pause, so the resume is guaranteed to be queued behind
  // it rather than racing it. On the wire the two must still arrive in call order, newest last.
  let release;
  const gate = new Promise((resolve) => {
    release = resolve;
  });
  let pauses = 0;
  const base = readyScript();
  const script = (event) => {
    if (event.message.type === 'pause') {
      pauses += 1;
      const first = pauses === 1;
      return void (first ? gate : Promise.resolve()).then(() => event.emit({ type: 'ack', id: event.message.id }));
    }
    base(event);
  };
  const { runtime, workers } = await rig(t, { hostParts: parts, script });
  await runtime.load();
  const worker = workers[0];
  runtime.pause();
  runtime.resume();
  release();
  await flush();
  await flush();
  const seen = worker.posted.filter((m) => m.type === 'pause').map((m) => m.paused);
  assert.deepEqual(seen, [true, false], 'the engine is told pause then resume, in that order');
  assert.equal(runtime.describe().paused, false, 'the runtime believed the resume');
  assert.equal(worker.posted.filter((m) => m.type === 'pause').map((m) => m.paused).at(-1), false);
  runtime.dispose();
  await runtime.whenDisposed();
});

test('input reaches native as the engine codes: right is key 8, middle/wheel/text are refused', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  await runtime.load();
  const worker = workers[0];
  // The resize the load sent is an input message too; the event mapping is what is under test.
  const inputs = () => worker.posted.filter((m) => m.type === 'input' && m.kind !== 'resize');

  runtime.sendInput({ type: 'pointer-down', x: 12.5, y: 30.25, button: 0 });
  runtime.sendInput({ type: 'pointer-move', x: 1.5, y: 2.5 });
  runtime.sendInput({ type: 'key-down', key: 'Enter', code: 'Enter', repeat: false });
  runtime.sendInput({ type: 'key-up', key: 'Enter', code: 'Enter' });
  runtime.sendInput({ type: 'key-down', key: 'a', code: 'KeyA', repeat: false });
  runtime.sendInput({ type: 'pointer-down', x: 5, y: 5, button: 2 });
  runtime.sendInput({ type: 'pointer-up', x: 5, y: 5, button: 2 });
  // The middle button has no engine code. Sending it as a left press would fire a tap.
  runtime.sendInput({ type: 'pointer-down', x: 7, y: 7, button: 1 });
  runtime.sendInput({ type: 'pointer-up', x: 7, y: 7, button: 1 });
  runtime.sendInput({ type: 'pointer-wheel', x: 5, y: 5, deltaY: 3 });
  runtime.sendInput({ type: 'text', value: 'x' });
  await flush();

  assert.deepEqual(inputs().map((m) => [m.kind, m.code ?? `${m.x},${m.y}`, m.down]), [
    ['pointer', '12.5,30.25', true],
    ['pointer-move', '1.5,2.5', false],
    ['key', 13, true],
    ['key', 13, false],
    ['key', 65, true],
    ['key', 8, true],
    ['key', 8, false],
  ]);
  assert.equal(inputs().length, 7, 'middle, wheel and text produce no worker input at all');
  assert.match(runtime.describe().errors.join(' '), /not supported by the compat engine/);
  assert.match(runtime.describe().errors.join(' '), /pointer button 1 has no engine code/);
  runtime.dispose();
  await runtime.whenDisposed();
});

test('every named key maps to the code the engine switch uses, and unknown keys are refused', () => {
  // `mac_main.mm:305-318` is the authority: Escape and Backspace are both 8, Return and keypad
  // Enter are both 13, space is 32, the arrows are 37..40.
  const expected = [
    [['Escape', 'Escape'], 8],
    [['Backspace', 'Backspace'], 8],
    [['Enter', 'Enter'], 13],
    [['Enter', 'NumpadEnter'], 13],
    [[' ', 'Space'], 32],
    [['ArrowLeft', 'ArrowLeft'], 37],
    [['ArrowUp', 'ArrowUp'], 38],
    [['ArrowRight', 'ArrowRight'], 39],
    [['ArrowDown', 'ArrowDown'], 40],
  ];
  for (const [[key, code], value] of expected) {
    assert.equal(api.keyCode(key, code), value, `${key}/${code} must map to ${value}`);
  }
  // ASCII letters and digits are their own codes.
  assert.equal(api.keyCode('a', 'KeyA'), 65);
  assert.equal(api.keyCode('7', 'Digit7'), 55);
  // A key the engine does not name is refused, never guessed at. Tab is not in the switch.
  assert.equal(api.keyCode('Tab', 'Tab'), null);
  assert.equal(api.keyCode('Shift', 'ShiftLeft'), null);
  assert.equal(api.keyCode('F5', 'F5'), null);
});

test('input before load is refused by name and never reaches the worker', async (t) => {
  const parts = makeHost();
  const { runtime, workers } = await rig(t, { hostParts: parts });
  runtime.sendInput({ type: 'key-down', key: 'Enter', code: 'Enter', repeat: false });
  assert.deepEqual(workers, [], 'no worker exists before load');
  assert.equal(runtime.describe().loaded, false);
  runtime.dispose();
  await runtime.whenDisposed();
});

test('save is asynchronous and honest: a refused worker rejects, and the descriptor says so', async (t) => {
  const parts = makeHost();
  // The fake worker's `readyScript` answers `init`/`frame`/`pause`/`input`/`dispose` and **nothing**
  // else, so a checkpoint request is never answered. That is the shape under test here: the facade
  // must not invent a checkpoint, and it must not resolve one either.
  const { runtime } = await rig(t, { hostParts: parts });
  await runtime.load();
  const outcome = await runtime.save().then(
    () => null,
    (error) => error,
  );
  assert.ok(outcome instanceof Error, 'save resolves a SaveState or rejects; it never fabricates one');
  assert.match(String(outcome.message), /timed out|checkpoint/, 'the refusal is the worker\'s, by message');
  // The descriptor names the real mechanism rather than claiming "not implemented".
  const described = runtime.describe();
  assert.match(String(described.checkpoint), /ARCV compatibility checkpoint/);
  assert.equal(described.audio.realGameVerified, false);
  assert.equal(described.coldGameUnverified, true);
  // The checkpoint is no longer a skipped row: what is still skipped is audio.
  const report = runtime.report();
  assert.equal(report.severity, 'unsupported', 'audio alone keeps this from being clean');
  assert.deepEqual(
    report.unsupported.map((row) => row.command),
    ['artemis:audio'],
    'the checkpoint row is gone now that the path exists',
  );
  // `restore` validates before it applies: a foreign slot is refused by name, not guessed at.
  await assert.rejects(() => runtime.restore({ version: 1, data: {} }), /not a artemis-compat-checkpoint/);
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a frame failure is terminal: one fatal, the session disposes, and no later tick re-reports', async (t) => {
  const parts = makeHost();
  // The boot frame must succeed or the load fails for a different reason. Every frame after it
  // returns 0 — the engine asked to exit — which is a session end, not a hiccup.
  let frames = 0;
  const base = readyScript();
  const script = (event) => {
    if (event.message.type === 'frame') {
      frames += 1;
      // The boot frame must succeed or the load fails for a different reason. Every frame after it
      // returns 0 — the engine asked to exit — which is a session end, not a hiccup.
      if (frames > 1) {
        return void queueMicrotask(() =>
          event.emit({ type: 'frame-ok', id: event.message.id, running: false, reason: 'script finished' }),
        );
      }
    }
    base(event);
  };
  const { runtime } = await rig(t, { hostParts: parts, script });
  await runtime.load();
  runtime.tick(20);
  await flush();
  assert.deepEqual(
    parts.notifications.map((event) => event.type),
    ['fatal'],
    'the host is told, because a rejected promise nobody awaited would be silent',
  );
  assert.match(parts.notifications[0].message, /script finished/);

  // The failure is terminal. Every later tick is a no-op — a session that keeps reporting the same
  // fatal every 16 ms is a log flood, not a diagnosis.
  for (let n = 0; n < 10; n += 1) runtime.tick(20);
  await flush();
  assert.deepEqual(parts.notifications.map((event) => event.type), ['fatal'], 'exactly one fatal, ever');
  assert.equal(runtime.describe().disposed, true, 'the failure disposed the session');
  assert.equal(runtime.describe().frameFailure, 'script finished');
  runtime.dispose();
  await runtime.whenDisposed();
  assert.equal(runtime.describe().disposed, true);
});

test('a frame failure notifies the host once, and the notification is not repeated per tick', async (t) => {
  const parts = makeHost();
  let frames = 0;
  const base = readyScript();
  const script = (event) => {
    if (event.message.type === 'frame') {
      frames += 1;
      // The boot frame must succeed or the load fails for a different reason. Every frame after it
      // returns 0 — the engine asked to exit — which is a session end, not a hiccup.
      if (frames > 1) {
        return void queueMicrotask(() =>
          event.emit({ type: 'frame-ok', id: event.message.id, running: false, reason: 'script finished' }),
        );
      }
    }
    base(event);
  };
  const { runtime } = await rig(t, { hostParts: parts, script });
  await runtime.load();
  runtime.tick(20);
  await flush();
  // A tick still in flight is coalesced by the worker host (one inflight frame), so the reachable
  // hazard is not two simultaneous rejections but a *later* tick reporting the same death again.
  for (let n = 0; n < 5; n += 1) {
    runtime.tick(20);
    await flush();
  }
  assert.deepEqual(
    parts.notifications.map((event) => event.type),
    ['fatal'],
    'the host is told once, because a rejected promise nobody awaited would be silent and five',
  );
  assert.match(parts.notifications[0].message, /script finished/);
  runtime.dispose();
  await runtime.whenDisposed();
});

test('the engine\'s own log lines are forwarded at their own level', async (t) => {
  const parts = makeHost();
  // Everything the transport needs is answered by the standard script; this test only adds log
  // traffic on top of it.
  const script = readyScript();
  const { runtime, workers } = await rig(t, { hostParts: parts, script });
  await runtime.load();
  workers[0].emit({ type: 'log', level: 'error', line: 'lua error: could not open index.lua' });
  workers[0].emit({ type: 'log', level: 'info', line: 'web host: GL 1.0' });
  await flush();
  const forwarded = parts.logs.filter((entry) => entry.message.includes('compat engine'));
  // The engine's lines land where the engine put them, at their own level. Other log lines exist
  // (the runtime's own "ready" note), so only the engine's are compared.
  assert.deepEqual(
    forwarded
      .filter((entry) => entry.message.includes('lua error') || entry.message.includes('GL 1.0'))
      .map((entry) => [entry.level, entry.message.includes('index.lua')]),
    [
      ['error', true],
      ['info', false],
    ],
  );
  runtime.dispose();
  await runtime.whenDisposed();
});

test('a load failure rejects once, tells the host, and leaves nothing running', async (t) => {
  const parts = makeHost();
  const script = ({ message, emit }) => {
    if (message.type === 'init') return void queueMicrotask(() => emit({ type: 'fatal', id: 0, reason: 'read timed out after 30000ms' }));
    if (message.type === 'dispose') return void queueMicrotask(() => emit({ type: 'disposed-ack', id: message.id }));
  };
  const { runtime, workers } = await rig(t, { hostParts: parts, script });
  await assert.rejects(() => runtime.load(), /timed out/);
  // A second caller joins the same attempt rather than starting a second worker.
  await assert.rejects(() => runtime.load(), /timed out/);
  assert.equal(workers.length, 1, 'one load, one worker');
  assert.deepEqual(parts.notifications.map((event) => event.type), ['fatal']);
  // Both callers settle. `whenDisposed()` may hand back a fresh wrapper per call; what must be one
  // fact is the **teardown** behind it, and a hang is what a second caller used to get.
  await Promise.all([runtime.whenDisposed(), runtime.whenDisposed()]);
  runtime.dispose();
  await runtime.whenDisposed();
  assert.equal(workers[0].terminated, true);
});

test('a dispose during a load owns the attempt, and a second dispose is not a second teardown', async (t) => {
  const parts = makeHost();
  let releaseInit;
  const script = ({ message, emit }) => {
    if (message.type === 'init') {
      releaseInit = () => emit({ type: 'ready', id: message.id });
      return;
    }
    if (message.type === 'dispose') return void queueMicrotask(() => emit({ type: 'disposed-ack', id: message.id }));
  };
  const { runtime, workers } = await rig(t, { hostParts: parts, script });
  const loading = runtime.load();
  const outcome = loading.then(() => 'resolved', (error) => error);
  await flush();
  runtime.dispose();
  releaseInit?.();
  const result = await outcome;
  assert.ok(result instanceof Error, 'a disposed load does not resolve');
  assert.match(String(result.message), /disposed/);
  await runtime.whenDisposed();
  runtime.dispose();
  await runtime.whenDisposed();
  assert.equal(workers.length, 1);
  assert.equal(workers[0].terminated, true);
});

test('whenDisposed is one shared promise, and a second caller does not hang', async (t) => {
  const parts = makeHost();
  const { runtime } = await rig(t, { hostParts: parts });
  await runtime.load();
  runtime.dispose();
  const first = runtime.whenDisposed();
  const second = runtime.whenDisposed();
  await Promise.all([first, second]);
  assert.equal(runtime.describe().disposed, true);
});

test('the compiled worker copy is byte-identical to the source the build ships', () => {
  const source = readFileSync(join(PACKAGE, 'src', 'compat', 'compat-worker.js'), 'utf8');
  const built = readFileSync(join(PACKAGE, 'dist', 'compat', 'compat-worker.js'), 'utf8');
  assert.equal(built, readFileSync(join(PACKAGE, 'src', 'compat', 'worker-audio.js'), 'utf8') + '\n' + source, 'dist carries the helper then the worker, byte for byte');
  assert.match(source, /type: 'log'/, 'the worker forwards engine output to the page');
  assert.match(source, /printErr:/, 'the engine\'s stderr is forwarded, not dropped');
});
