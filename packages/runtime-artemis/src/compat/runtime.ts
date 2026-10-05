/**
 * The compat runtime: a `RuntimeInstance` over the classic compat worker.
 *
 * It is a facade and nothing else. The engine runs in the worker, the worker draws into the canvas it
 * was handed, and this class owns exactly three things the worker cannot: the host-driven frame
 * clock, the pack descriptors read out of the game source, and the lifecycle ordering that makes a
 * dispose during a load harmless.
 */

import {
  UnsupportedEngineError,
  isSaveStateFiles,
  type CompatibilityReport,
  type GameSource,
  type InputEvent,
  type LogLevel,
  type RuntimeHost,
  type RuntimeInstance,
  type RuntimeOptions,
  type SaveState,
  type SaveStateFiles,
} from '@online-gal/engine-api';
import { readArtemisCompatConfig, resolveCompatEngine, type ArtemisCompatEngine } from '../configure.js';
import {
  COMPAT_STORE_PREFIX,
  CompatWorkerHost,
  assertRelativePath,
  base64ToBytes,
  validateFileSet,
  type CompatCheckpointFile,
} from './worker-host.js';
import {
  COMPAT_OPEN_MODE,
  COMPAT_OPEN_PACK,
  COMPAT_SAVE_KIND,
  COMPAT_SAVE_MAX_FILES,
  type CompatFileDescriptor,
} from './protocol.js';

/** The answer when no engine was named. Empty URLs, checked by `hasEngine`. */
const NO_ENGINE: ArtemisCompatEngine = { workerUrl: '', engineJsUrl: '', wasmUrl: '' };

/**
 * The engine's own normalised key codes (`web_main.cpp` `Key`, `mac_main.mm:305-318`).
 *
 * These are the ABI: `artc_web_key(int key, int down)` switches on integers, not on DOM strings.
 * The platform switch maps Escape **and** Backspace to 8, Return and keypad Enter to 13, the arrows
 * to 37..40 and space to 32; a key with no entry is reported rather than guessed at, because a
 * fabricated code presses a control the player never touched.
 */
const KEY_ENTER = 13;
const KEY_BACK = 8;
const KEY_ESCAPE = 8;
const KEY_SPACE = 32;
const KEY_LEFT = 37;
const KEY_UP = 38;
const KEY_RIGHT = 39;
const KEY_DOWN = 40;

/** One native frame per 60 Hz of host time. */
const FRAME_INTERVAL_MS = 1000 / 60;

/**
 * An Artemis compat session.
 *
 * The frame contract is the host's, in both directions: `tick(deltaMs)` is what advances the guest —
 * this class installs no `requestAnimationFrame` and the worker owns none — and `render()` is a
 * no-op because the worker already drew into the transferred canvas.
 */
export class CompatArtemisRuntime implements RuntimeInstance {
  readonly runtimeId = 'artemis';

  private readonly host: RuntimeHost;
  private readonly options: RuntimeOptions;
  private readonly engine: ArtemisCompatEngine;

  private page: CompatWorkerHost | null = null;
  private loadPromise: Promise<void> | null = null;
  /** Memoized teardown, so two dispose callers own one release. */
  private disposePromise: Promise<void> | null = null;
  private loaded = false;
  private disposed = false;
  /** True between `pause()` and a `resume()` the host actually wants. */
  private paused = false;
  /** Bumped by pause/resume/dispose: a stale transition must not revive a paused session. */
  private epoch = 0;
  /** Milliseconds accumulated toward the next native frame. */
  private accumulator = 0;
  /** The descriptors the current session opened, kept for diagnostics. */
  private descriptors: readonly CompatFileDescriptor[] = [];
  private loadFailure: string | null = null;
  private readonly notes: string[] = [];
  private readonly onceKeys = new Set<string>();
  /** Frames **requested** from the page. The native counters live in the worker host. */
  private frameCount = 0;
  /** Set by a terminal frame failure: the session is over and no further tick may report again. */
  private frameFailure: string | null = null;
  /**
   * One queue for every store write a save or a restore performs.
   *
   * ponytail: one chain for the whole runtime. Two persists in flight would let the later checkpoint
   * overwrite a file the earlier one had not finished, so the store would hold a mix of both. Split it
   * per slot only if a session ever persists more than one prefix at once.
   */
  private persistQueue: Promise<void> = Promise.resolve();

  constructor(host: RuntimeHost, options: RuntimeOptions) {
    this.host = host;
    this.options = options;
    this.engine = this.resolveEngine();
  }

  /**
   * The engine this instance will run on, resolved once at construction.
   *
   * Read here rather than in `load` because a publication that lands after the instance exists must
   * not move an in-flight session onto a different binary, and because `describe()` has to answer
   * "which engine is this" before anything is loaded.
   *
   * The three cases are distinct and must stay distinct: a game that names nothing gets the
   * **deployment** engine, a game that names a base gets that base, and a game that names `null`
   * gets nothing at all — which is a withdrawal, not an invitation to fall back.
   */
  private resolveEngine(): ArtemisCompatEngine {
    const perGame = this.options.gameConfig?.['engineBase'];
    if (perGame === null) return NO_ENGINE;
    if (perGame === undefined) return resolveCompatEngine(readArtemisCompatConfig()) ?? NO_ENGINE;
    if (typeof perGame !== 'string' && !(perGame instanceof URL)) return NO_ENGINE;
    return resolveCompatEngine({ engineBase: perGame }) ?? NO_ENGINE;
  }

  /**
   * Whether an engine was named for this instance.
   *
   * All three URLs must be present, and `resolveCompatEngine` has already refused a triple with a
   * repeat in it: a worker handed the glue is not an engine, so it must not be advertised as one.
   */
  private hasEngine(): boolean {
    return this.engine.workerUrl !== '' && this.engine.engineJsUrl !== '' && this.engine.wasmUrl !== '';
  }

  // ---------------------------------------------------------------------------------------------
  // Loading
  // ---------------------------------------------------------------------------------------------

  /** Public gesture entry. The rejection reaches the caller; an explicit pause stays paused. */
  unlockAudio(): Promise<void> {
    const page = this.page;
    if (!page) return Promise.reject(new Error('audio unsupported: no session'));
    return page.unlockAudio();
  }

  async load(): Promise<void> {
    if (this.loadPromise) return this.loadPromise;
    const started = this.startLoad();
    // Owned in the same turn it exists: a load that rejects is reported to the caller *and* cannot
    // surface as an unhandled rejection when the caller dropped the promise.
    this.loadPromise = started;
    void started.catch(() => undefined);
    return started;
  }

  /**
   * The real start: capabilities, descriptors, worker, then **one real frame**.
   *
   * Nothing is read here. The pack chain is described by `size` alone — a real title's chain is
   * several GB and preloading it is the one thing this transport exists to avoid — and the worker
   * reads ranges on demand through the shared window.
   *
   * The load does not finish when the worker answers `ready`. `ready` means the engine opened the
   * pack; the Lua framework is not up until the **first frame** boots it (`Boot()` runs inside
   * `artc_web_frame`, not inside `artc_web_open`). A pause applied before that frame reaches a
   * native `artc_web_pause` that returns 0 — "not started" — and the session would report itself
   * paused while the engine had not begun. So the first frame is driven here, under the host's own
   * surface coordinate, and only then is `loaded` set and a pending pause applied.
   */
  private async startLoad(): Promise<void> {
    if (this.disposed) throw new Error('the Artemis compat runtime is disposed');
    const missing = this.missingCapabilities();
    if (missing.length > 0) {
      throw new UnsupportedEngineError(
        'The Artemis compat transport cannot run in this page: it needs a shared window, an ' +
          'OffscreenCanvas and a range-readable game source.',
        'artemis',
        missing,
      );
    }
    const canvas = this.host.canvas;
    if (!canvas) throw new Error('the Artemis compat runtime needs a canvas from the host');
    const surface = this.options.surface;
    // The canvas a real page hands over is an `HTMLCanvasElement`: it cannot be posted, and the
    // worker's `Module.canvas` needs a drawable target, not a detached copy. So the element is
    // *retargeted* — `transferControlToOffscreen()` moves its rendering to an `OffscreenCanvas`
    // that is the same canvas as far as the page is concerned — and the element itself stays the
    // host's, in the DOM, with its gesture handlers.
    const transferred = transferCanvas(canvas);
    const page = new CompatWorkerHost({
      source: this.host.source,
      setting: (key, fallback) => this.host.setting(key, fallback),
      createWorker: () => this.createWorker(this.engine.workerUrl),
      // The engine's own lines, at their own level. A game that prints why it is stopping and then
      // fails is the single hardest failure to see from the page, and this is the whole fix for it.
      onLog: (level, line) => this.log(level, `artemis compat engine: ${line}`),
    });
    this.page = page;
    try {
      const descriptors = await describeGame(this.host.source);
      if (this.disposed) throw new Error('the Artemis compat runtime was disposed while starting');
      this.descriptors = descriptors;
      // The transfer happens here: after both awaits, with the disposal rechecked, so a load
      // cancelled while the pack chain was being described never retargets the page's canvas.
      const offscreen = transferred.postTransfer ? await transferred.postTransfer(surface) : transferred.element;
      if (this.disposed) throw new Error('the Artemis compat runtime was disposed while starting');
      // What the previous session left behind, read before the engine opens: the worker installs it
      // ahead of `artc_web_open`, so a cold boot sees `system.dat` rather than starting from nothing.
      const initialFiles = await this.readPersisted();
      if (this.disposed) throw new Error('the Artemis compat runtime was disposed while starting');
      await page.load({
        engineJsUrl: this.engine.engineJsUrl,
        wasmUrl: this.engine.wasmUrl,
        canvas: offscreen as OffscreenCanvas,
        descriptors,
        // The pack is always the first descriptor: `CompatReadHost` refuses an unknown virtual path
        // with READ_MISSING, which the engine reports as a refusal rather than as an absence.
        surface,
        initialFiles,
        animationTrace: this.options.gameConfig?.['animationTrace'] === true,
      });
      if (this.disposed) throw new Error('the Artemis compat runtime was disposed while starting');
      // The boot frame. It is driven at the host's own surface coordinate, so the engine's resize
      // sees the size the canvas actually has rather than a default it invented.
      await page.frame();
      if (this.disposed) throw new Error('the Artemis compat runtime was disposed while starting');
      const diagnostics = page.diagnostics();
      if (diagnostics['started'] !== 1) {
        throw new Error(
          'the compat engine answered a frame without starting (`artc_web_started` != 1); the ' +
            'first frame must boot the Lua framework',
        );
      }
      this.loaded = true;
      // A pause that arrived during the load parks the session it was waiting for, now that the
      // engine is genuinely running and `artc_web_pause` can return 1.
      if (this.paused) await page.setPaused(true);
      if (this.disposed) throw new Error('the Artemis compat runtime was disposed while starting');
      this.log('info', `artemis: compat engine ready on ${descriptors.length} pack descriptor(s)`);
    } catch (error) {
      if (!this.disposed) this.loadFailure = describeError(error);
      // The session could not start, so the host is told: a fatal that is only reported through a
      // rejected promise leaves a page that asked for a game sitting on a blank canvas.
      this.notifyFatal(this.loadFailure ?? describeError(error));
      this.dispose();
      throw error;
    }
  }

  /**
   * The worker. `new Worker(url)` is the only constructor a classic worker takes — `{ type: 'module' }`
   * would refuse `importScripts`, which is how the generated glue loads — and `classic` is spelled out
   * so the distinction is visible rather than implied by a default.
   */
  private createWorker(url: string): Worker {
    return new Worker(url, { type: 'classic', name: 'artemis-compat' });
  }

  // ---------------------------------------------------------------------------------------------
  // Frame clock
  // ---------------------------------------------------------------------------------------------

  /**
   * Advances the guest by wall-clock time, at most once per 60 Hz.
   *
   * The frame is **delta-free** — `artc_web_frame()` takes no time argument, the engine reads its own
   * clock — so running N frames to "catch up" would replay the same instant N times and speed the
   * game up rather than catch it up. The backlog is therefore **dropped**, not replayed: the
   * accumulator keeps only the remainder inside one interval (`accumulator %= FRAME_INTERVAL_MS`),
   * so a tick carrying 1000 ms of host time does not leave ~57 frames of debt that the following
   * 16 ms ticks would spend draining into a burst. A stalled tab resumes at the current moment.
   *
   * One native frame per call, and a terminal frame failure ends the session **once**: without that,
   * every later tick would re-report the same fatal to the host and to the log.
   */
  tick(deltaMs: number): void {
    if (this.disposed || this.paused || !this.loaded) return;
    if (this.frameFailure !== null) return;
    const delta = Number.isFinite(deltaMs) && deltaMs > 0 ? deltaMs : 0;
    this.accumulator += delta;
    if (this.accumulator < FRAME_INTERVAL_MS) return;
    // Keep only the remainder inside one interval. A tick carrying 1000 ms therefore leaves at most
    // one interval of debt rather than ~57 frames of it, which the following 16 ms ticks would spend
    // draining into a burst the guest cannot express.
    this.accumulator %= FRAME_INTERVAL_MS;
    const page = this.page;
    if (!page) return;
    // A checkpoint or restore owns the engine until it settles. The accumulator above has already been
    // advanced, so this tick's time is spent rather than banked: a checkpoint that took 200 ms must not
    // come back as twelve frames the moment it finishes.
    if (page.busy) return;
    this.frameCount += 1;
    void page.frame().catch((error: unknown) => {
      // A false frame is a session end and a fatal is terminal; both arrive here. Reported **once**,
      // and never rethrown: a tick is a void call and an escaping rejection would be nobody's to
      // handle. The latch is what stops a dead session from spamming the host every 16 ms.
      if (this.disposed || this.frameFailure !== null) return;
      this.frameFailure = describeError(error);
      this.note(`frame failed: ${this.frameFailure}`);
      this.notifyFatal(this.frameFailure);
      this.dispose();
    });
  }

  /**
   * No-op by design.
   *
   * The worker owns the canvas and draws into it inside `artc_web_frame`; there is no back buffer
   * here to present. A `requestFrame()` from this call would ask the host to repaint pixels nobody
   * re-rendered, which is the second-clock defect this runtime exists to avoid.
   */
  render(): void {}

  // ---------------------------------------------------------------------------------------------
  // Lifecycle
  // ---------------------------------------------------------------------------------------------

  /** Parks the session. The flag is set synchronously; the worker is told in order. */
  pause(): void {
    if (this.disposed) return;
    this.paused = true;
    this.epoch += 1;
    const page = this.page;
    // Nothing loaded yet: the flag alone parks the input path and the next load's first frame.
    if (!page || !this.loaded) return;
    void page.setPaused(true).catch((error: unknown) => this.note(`pause failed: ${describeError(error)}`));
  }

  resume(): void {
    if (this.disposed || !this.loaded) return;
    const epoch = this.epoch;
    const page = this.page;
    if (!page) return;
    void page
      .setPaused(false)
      .then(() => {
        // Rechecked after the await: a pause that arrived while the worker was being told to run
        // owns the session, and clearing the flag here would re-open input into a stopped guest.
        if (epoch === this.epoch && !this.disposed) this.paused = false;
      })
      .catch((error: unknown) => {
        // The flag stays set: a session that did not resume is not running, and reporting it as one
        // would unblock input into a loop that never started.
        this.note(`resume failed: ${describeError(error)}`);
      });
  }

  /**
   * Releases the worker.
   *
   * Synchronous in effect: `CompatWorkerHost.dispose` tears the worker down in the same call and
   * `whenDisposed` is what a caller awaits for the native shutdown acknowledgement. This method is
   * idempotent and has no promise of its own to return, because the interface does not have one.
   */
  dispose(): void {
    if (this.disposed) return;
    this.disposed = true;
    this.epoch += 1;
    this.paused = true;
    this.loaded = false;
    this.page?.dispose();
  }

  /**
   * Resolves once the worker is gone and every persist already on the queue has finished.
   *
   * Never rejects: a teardown that failed is still a teardown. A write that was in flight keeps its
   * place — disposal does not turn a save the caller is still awaiting into a refusal.
   */
  whenDisposed(): Promise<void> {
    const page = this.page;
    const pending = page ? page.whenDisposed() : Promise.resolve();
    return Promise.all([pending.catch(() => undefined), this.persistQueue]).then(() => undefined);
  }

  // ---------------------------------------------------------------------------------------------
  // Input
  // ---------------------------------------------------------------------------------------------

  /**
   * One normalised input event, translated to the native CABI.
   *
   * Nothing is delivered before the session is up, while it is paused, or after disposal — a
   * dispatched event against a stopped guest is a queued action nobody will ever consume, and
   * delivering it would make the next resume execute a click the player made minutes ago.
   *
   * The mapping is the engine's, not the DOM's: `artc_web_key(int key, int down)` takes the engine's
   * own normalised codes, and there is no wire form for wheel or text, so those are reported as
   * unsupported rather than approximated.
   */
  sendInput(event: InputEvent): void {
    if (event.type === 'pointer-down' || event.type === 'key-down') {
      const scope = globalThis as { AudioContext?: unknown; webkitAudioContext?: unknown };
      if (typeof scope.AudioContext === 'function' || typeof scope.webkitAudioContext === 'function') {
        void this.page?.unlockAudio().catch((error: unknown) => this.note(`audio unlock failed: ${describeError(error)}`));
      }
    }
    if (this.disposed || this.paused || !this.loaded) {
      this.noteOnce(
        'input-refused',
        `input was not delivered: ${
          this.disposed ? 'the runtime is disposed' : this.paused ? 'the session is paused' : 'the session has not loaded'
        }`,
      );
      return;
    }
    const page = this.page;
    if (!page) return;
    // Same gate as the frame clock: an input delivered into a running checkpoint would be consumed by
    // the engine's own `onSave`/`onLoad`, which is a script callback, not the scene the player sees.
    if (page.busy) {
      this.noteOnce('input-refused-checkpoint', 'input was not delivered: a checkpoint is in flight');
      return;
    }
    const message = this.translate(event);
    if (message === null) return;
    void page.input(message).catch((error: unknown) => this.note(`input delivery failed: ${describeError(error)}`));
  }

  /** The worker message for one event, or `null` when this runtime has no honest mapping for it. */
  private translate(event: InputEvent): {
    readonly kind: 'pointer' | 'pointer-move' | 'key';
    readonly x?: number;
    readonly y?: number;
    readonly down: boolean;
    readonly code?: number;
  } | null {
    switch (event.type) {
      case 'pointer-down':
      case 'pointer-up': {
        const point = this.stagePoint(event.x, event.y);
        if (point === null) return null;
        // The right button is *not* a tap: the engine's own back gesture is key 8, and sending a
        // left press for it would fire whatever control sits under the cursor instead.
        if (event.button === 2) {
          return { kind: 'key', code: KEY_BACK, down: event.type === 'pointer-down' };
        }
        // A middle press has no native meaning: `artc_web_pointer` takes one `down` flag and the
        // engine reads it as the tap key. Sending one would fire a tap the player did not make.
        if (event.button !== 0 && event.button !== undefined) {
          this.noteOnce(
            `pointer-button:${String(event.button)}`,
            `pointer button ${String(event.button)} has no engine code and was not delivered`,
          );
          return null;
        }
        return { kind: 'pointer', x: point.x, y: point.y, down: event.type === 'pointer-down' };
      }
      case 'pointer-move': {
        const point = this.stagePoint(event.x, event.y);
        if (point === null) return null;
        // A move carries no button, and the CABI has no `down` argument for it.
        return { kind: 'pointer-move', x: point.x, y: point.y, down: false };
      }
      case 'key-down':
      case 'key-up': {
        const code = keyCode(event.key, event.code);
        if (code === null) {
          this.noteOnce(`key-unmapped:${event.key}`, `key ${event.key} has no engine code and was not delivered`);
          return null;
        }
        return { kind: 'key', code, down: event.type === 'key-down' };
      }
      default:
        this.noteOnce(
          `unsupported:${String((event as { type: string }).type)}`,
          `input of type ${String((event as { type: string }).type)} is not supported by the compat engine`,
        );
        return null;
    }
  }

  /** The logical stage coordinate, or `null` for a value that is not a number. */
  private stagePoint(x: number, y: number): { x: number; y: number } | null {
    if (!Number.isFinite(x) || !Number.isFinite(y)) {
      this.noteOnce('pointer-nonfinite', 'a pointer event with a non-finite coordinate was refused');
      return null;
    }
    return { x, y };
  }

  // ---------------------------------------------------------------------------------------------
  // Save & restore
  // ---------------------------------------------------------------------------------------------

  /**
   * The engine's own checkpoint, taken inside the worker, then persisted.
   *
   * `artc_web_save` is synchronous and only the worker's thread may call it, so this cannot answer a
   * `SaveState` directly — and it must not answer a fabricated one. What it answers instead is a
   * promise for the **validated file set** the engine actually wrote: the worker reads the bytes back
   * out of its filesystem after the native ACK, and `CompatWorkerHost.checkpoint` refuses a payload
   * whose paths, count or sizes are not something a save could be.
   *
   * Each file is then written under {@link COMPAT_STORE_PREFIX} and **awaited**. The state is returned
   * only once the last write settled, so a caller that resolved holds bytes the store has; any write
   * error — or a disposal that lands while one is in flight — is a rejection, because a partly
   * written slot is not a save and must never be reported as one.
   *
   * Refusing is a rejection. `report()` and `describe()` name the checkpoint's real state, so a caller
   * that ignores this promise cannot read a success out of the diagnostics either.
   */
  async save(): Promise<SaveStateFiles> {
    if (this.disposed) throw new UnsupportedEngineError('the Artemis compat runtime is disposed', 'artemis');
    if (!this.loaded) {
      throw new UnsupportedEngineError('the Artemis compat runtime has not loaded a session', 'artemis');
    }
    const page = this.page;
    if (!page) throw new UnsupportedEngineError('the Artemis compat worker is absent', 'artemis');
    // Queued before the checkpoint, not after it. The worker's own queue is held until this method
    // returns, so a persist that started only after the checkpoint would keep the next save from
    // reaching the worker at all — its writes would then be free to land inside this one's.
    const queued = this.enqueuePersist(() => page.checkpoint());
    const files = await queued;
    return {
      version: 1,
      kind: COMPAT_SAVE_KIND,
      // `data` is deliberately null: the payload is `files`, and a reader that only understands
      // `SaveState.data` must see "nothing to read here" rather than an invented object.
      data: null,
      caption: `ARCV compatibility checkpoint (${files.length} file${files.length === 1 ? '' : 's'})`,
      files: files.map((file) => ({
        path: file.path,
        data: bytesToBase64(file.data),
      })),
      // False, and that is the whole truth of it: the engine installs or rolls back its own tree,
      // but `WritableStore` has no multi-file transaction, so persistence is one write per file and a
      // failure can leave the first of them behind. Claiming atomicity here would be a claim about a
      // store that cannot make it.
      atomic: false,
    };
  }

  /**
   * Applies a checkpoint to the running engine, then persists the same set.
   *
   * The payload is validated **in full** before the worker is told anything: a bad path, a size past
   * the native bound, or a version this runtime does not understand is refused here, with the engine
   * untouched. What the worker then does is park the guest, back the old files up, install, call
   * `artc_web_load`, and roll back if the engine answers `0`.
   *
   * Persistence follows the native load **and** its run-state ACK — never before, so a refused restore
   * cannot overwrite the store with a checkpoint the engine did not accept. A store write that fails
   * then ends the instance: the engine is already carrying these bytes while the store holds a
   * mixture, and the next session would hydrate from files that never belonged together. There is no
   * rollback for a multi-file store, so termination is the only honest answer.
   */
  async restore(state: SaveState): Promise<void> {
    if (this.disposed) throw new UnsupportedEngineError('the Artemis compat runtime is disposed', 'artemis');
    const files = readCheckpointPayload(state);
    const page = this.page;
    if (!page || !this.loaded) {
      throw new UnsupportedEngineError('the Artemis compat runtime has no session to restore into', 'artemis');
    }
    try {
      await this.enqueuePersist(async () => {
        await page.restore(files);
        return files;
      });
    } catch (error) {
      // A refused load — or a store that could not hold what the engine already applied — leaves the
      // engine in a state this facade cannot vouch for. Ending the session is the only honest outcome;
      // reporting success would be claiming a restore that did not happen.
      this.note(`restore failed: ${describeError(error)}`);
      this.dispose();
      throw error;
    }
    this.host.requestFrame();
  }

  // ---------------------------------------------------------------------------------------------
  // Persistence
  // ---------------------------------------------------------------------------------------------

  /**
   * Queues one persist behind whatever save or restore is already writing.
   *
   * The chain is joined before this call returns, so a rejection still occupies its place: the next
   * caller waits for this attempt to finish refusing, and never starts its own writes in the gap.
   */
  private enqueuePersist(
    produce: () => Promise<readonly CompatCheckpointFile[]>,
  ): Promise<readonly CompatCheckpointFile[]> {
    const run = this.persistQueue.then(async () => {
      // Checked at this persist's own turn. A dispose that landed while an earlier write was in
      // flight does not rewind that write; it refuses every persist that had not started yet.
      if (this.disposed) throw new UnsupportedEngineError('the Artemis compat runtime was disposed while saving', 'artemis');
      const files = await produce();
      await this.persist(files);
      return files;
    });
    this.persistQueue = run.then(
      () => undefined,
      () => undefined,
    );
    return run;
  }

  private async persist(files: readonly CompatCheckpointFile[]): Promise<void> {
    const store = this.host.store;
    for (const file of files) {
      await store.write(COMPAT_STORE_PREFIX + file.path, file.data);
    }
  }

  /**
   * The files this transport persisted, validated as one set.
   *
   * Only this prefix is read, so a game's own assets are never candidates; and `assertRelativePath`
   * refuses a `.pfs` by name in case one is ever listed. An empty set is the honest answer for a
   * first run, and a banks-only set is a valid cold start — the slot requirement belongs to a
   * checkpoint, not to a boot.
   *
   * Every entry is read and validated **before** the worker is built, so a bad entry fails the load
   * instead of half-installing a save directory. A listed file that cannot be read is a refusal: a
   * silently dropped one hydrates the engine from a set that never existed.
   */
  private async readPersisted(): Promise<readonly CompatCheckpointFile[]> {
    const store = this.host.store;
    const files: CompatCheckpointFile[] = [];
    for (const key of await store.list(COMPAT_STORE_PREFIX)) {
      if (!key.startsWith(COMPAT_STORE_PREFIX)) continue;
      const path = key.slice(COMPAT_STORE_PREFIX.length);
      assertRelativePath(path);
      const bytes = await store.read(key);
      if (bytes === undefined) throw new Error(`the store listed ${key} but could not read it`);
      files.push({ path, data: ownedBytes(bytes) });
    }
    return validateFileSet(files, { requireSlot: false, allowEmpty: true });
  }

  // ---------------------------------------------------------------------------------------------
  // Diagnostics
  // ---------------------------------------------------------------------------------------------

  private log(level: LogLevel, message: string): void {
    try {
      this.host.log(level, message);
    } catch {
      // A logger that throws must not turn a diagnosis into a failure.
    }
  }

  private note(message: string): void {
    if (this.notes.length < 64) this.notes.push(message);
    this.log('warn', `artemis compat: ${message}`);
  }

  /** Reports a repeating condition once. A note per event would be a log flood, not a diagnosis. */
  private noteOnce(key: string, message: string): void {
    if (this.onceKeys.has(key)) return;
    this.onceKeys.add(key);
    this.note(message);
  }

  /** Tells the host the session is over. A host that refuses the notification is still diagnosed. */
  private notifyFatal(message: string): void {
    try {
      this.host.notify({ type: 'fatal', message });
    } catch (error) {
      this.note(`the host refused a fatal notification: ${describeError(error)}`);
    }
  }

  /**
   * The rows this instance has not satisfied, recomputed on every call.
   *
   * The four capability rows are the transport's own requirements, read without constructing
   * anything. `crossOriginIsolated` is not decoration: without it `SharedArrayBuffer` is absent, and
   * without a shared window the worker's `Atomics.wait` in `exchange()` throws — a load that
   * "succeeds" and then hangs on the first pack read. The canvas row needs a canvas the host can
   * actually transfer, because the worker draws into the transferred object and has no other one.
   */
  private missingCapabilities(): readonly string[] {
    const rows: string[] = [];
    if (!this.hasEngine()) {
      rows.push('Artemis compat 引擎（worker + glue + wasm）：需由部署方通过 configureArtemisCompat() 提供');
    }
    const scope = globalThis as {
      crossOriginIsolated?: boolean;
      SharedArrayBuffer?: unknown;
      OffscreenCanvas?: unknown;
    };
    if (scope.crossOriginIsolated !== true) {
      rows.push('crossOriginIsolated：阻塞式 pack 读取需要 SharedArrayBuffer');
    }
    if (typeof scope.SharedArrayBuffer !== 'function') {
      rows.push('SharedArrayBuffer：worker 的 Atomics.wait 窗口');
    }
    if (typeof scope.OffscreenCanvas !== 'function') {
      rows.push('OffscreenCanvas：worker 只能接手 transferred canvas，不能被 handed 元素');
    }
    const canvas = this.host.canvas as
      | { transferControlToOffscreen?: unknown; getContext?: unknown }
      | null
      | undefined;
    // Two shapes are genuinely transferable, and the gate must not collapse them:
    //
    //   - an `HTMLCanvasElement`, which converts itself through `transferControlToOffscreen`;
    //   - an `OffscreenCanvas` the host already made, which is transferable as-is and — by the
    //     platform's own rule — has no `transferControlToOffscreen` at all.
    //
    // So this is an OR, not a conjunction: requiring the method would refuse a host that handed over
    // an OffscreenCanvas, and requiring the constructor would refuse a page whose global is absent
    // while its element can still convert. The one shape that is *not* transferable is an element
    // that cannot be retargeted, and that is refused here rather than at the post, where it would
    // surface as a `DataCloneError` after the worker had already been built.
    const canvasAlreadyOffscreen = typeof OffscreenCanvas === 'function' && canvas instanceof OffscreenCanvas;
    const canvasCanConvert = !!canvas && typeof canvas.transferControlToOffscreen === 'function';
    if ((!canvasAlreadyOffscreen && !canvasCanConvert) || !hasDrawableSurface(canvas)) {
      rows.push('可 transfer 的 canvas：宿主未提供 canvas，或它既不是 OffscreenCanvas 也无法 transferControlToOffscreen');
    }
    if (typeof this.host.source.readRange !== 'function') {
      rows.push('GameSource.readRange：compat 传输按 range 读 pack，不做整包预载');
    }
    return rows;
  }

  describe(): Readonly<Record<string, unknown>> {
    const state = this.page?.describe() ?? {};
    const diagnostics = this.page?.diagnostics() ?? {};
    return {
      engine: 'compat',
      engineSource: this.hasEngine() ? 'configured' : 'unconfigured',
      available: !this.disposed && this.hasEngine() && this.loadFailure === null,
      missingCapabilities: this.missingCapabilities(),
      loaded: this.loaded,
      disposed: this.disposed,
      paused: this.paused,
      surface: this.options.surface,
      packs: this.descriptors.map((entry) => `${entry.virtualPath} (${entry.size} B)`),
      // The three URLs, so a reader can see the worker is *not* the glue. Reporting one URL here is
      // how the original defect stayed invisible: both files answered to `workerUrl`.
      engineUrls: { ...this.engine },
      protocol: state['protocol'],
      worker: {
        state: state['state'] ?? 'absent',
        ...state,
        ...diagnostics,
      },
      audio: state['audio'] ?? { connected: false, unsupported: 'no worker' },
      // The checkpoint is implemented; what a reader cannot get from this row is whether a real game
      // produces a usable slot, and that stays unverified until a real title is run.
      checkpoint: 'ARCV compatibility checkpoint via onSave/onLoad; cold game unverified',
      coldGameUnverified: true,
      failure: this.loadFailure,
      frameFailure: this.frameFailure,
      framesRequested: this.frameCount,
      errors: this.notes,
    };
  }

  /**
   * What this session hit, not a summary.
   *
   * A runtime that cannot checkpoint and cannot play audio is not `clean`: those rows are skipped by
   * this build, and a report that called them honoured would be the one answer a report must never
   * give. A load that has not happened adds its own row rather than reporting cleanliness by default.
   */
  report(): CompatibilityReport {
    const audio = this.page?.describe().audio as { connected?: boolean } | undefined;
    const unsupported = audio?.connected ? [] : [
      {
        command: 'artemis:audio',
        count: 1,
        handling: 'skipped' as const,
        note: 'audio is not connected on this session; realGameVerified stays false',
      },
    ];
    for (const capability of this.missingCapabilities()) {
      unsupported.push({ command: capability, count: 1, handling: 'skipped' as const, note: 'supplied by the deployment' });
    }
    if (this.loadFailure !== null) {
      unsupported.push({
        command: 'artemis:load',
        count: 1,
        handling: 'skipped' as const,
        note: this.loadFailure,
      });
    }
    return {
      runtimeId: this.runtimeId,
      severity: unsupported.length > 0 ? 'unsupported' : 'clean',
      unsupported,
      approximated: [],
      supportedCommandCount: this.loaded ? 1 : 0,
    };
  }
}

/**
 * The engine's own key codes.
 *
 * `web_main.cpp` `Key(int key, int down)` switches on normalised integers, not on DOM strings, so
 * the numbers are the ABI — the platform switch that produces them is `mac_main.mm:305-318`. ASCII
 * letters and digits are their own codes; the special keys are the ones the engine names by number,
 * and a key with no entry is reported rather than guessed at — a fabricated code would press a
 * control the player never touched.
 */
export function keyCode(key: string, code: string): number | null {
  if (key.length === 1) {
    const direct = key.toUpperCase().charCodeAt(0);
    if (direct >= 0x20 && direct < 0x7f) return direct;
    return null;
  }
  const named: Readonly<Record<string, number>> = {
    Enter: KEY_ENTER,
    NumpadEnter: KEY_ENTER,
    ' ': KEY_SPACE,
    Spacebar: KEY_SPACE,
    Escape: KEY_ESCAPE,
    Esc: KEY_ESCAPE,
    Backspace: KEY_BACK,
    ArrowLeft: KEY_LEFT,
    ArrowUp: KEY_UP,
    ArrowRight: KEY_RIGHT,
    ArrowDown: KEY_DOWN,
  };
  return named[key] ?? named[code] ?? null;
}

/**
 * Describes the game's pack chain.
 *
 * Three rules, each a measured constraint rather than a preference:
 *
 * - **One root listing, and only the root.** A real deployment is tens of thousands of files; the
 *   loose tree is never walked.
 * - **Only the PFS rows**, so an unrelated large file cannot become descriptor 0.
 * - **Size only, never a read.** A chain is several GB, and `size()` is a metadata call; `readFile`
 *   on a > 2 GiB pack does not even fit in one buffer.
 *
 * Descriptor 0 is `root.pfs` when it is there — the engine's own hardcoded pack — and otherwise the
 * only package in the directory. Its virtual path is `/game/root.pfs` either way, and every later
 * row is `/game/<basename>` so the engine can ask for a continuation by its own name.
 */
export async function describeGame(source: GameSource): Promise<readonly CompatFileDescriptor[]> {
  if (typeof source.readRange !== 'function') {
    throw new UnsupportedEngineError(
      `the compat transport reads pack ranges, and ${source.name} cannot`,
      'artemis',
    );
  }
  const listing = await source.list('');
  const packs = listing.filter((entry) => !entry.endsWith('/') && /\.pfs(\.\d{1,9})?$/i.test(entry)).sort(comparePacks);
  const base = packs[0];
  if (base === undefined) {
    throw new UnsupportedEngineError(
      `no pack in ${source.name}: the compat engine opens ${COMPAT_OPEN_PACK}, which needs a .pfs at the root`,
      'artemis',
    );
  }
  const descriptors: CompatFileDescriptor[] = [];
  for (const [index, name] of packs.entries()) {
    const size = await source.size(name);
    if (!Number.isSafeInteger(size) || size < 0) {
      throw new UnsupportedEngineError(`${name} has no readable size (${String(size)} bytes)`, 'artemis');
    }
    descriptors.push({
      id: index + 1,
      path: name,
      virtualPath: index === 0 ? COMPAT_OPEN_PACK : `/game/${name}`,
      size,
    });
  }
  return descriptors;
}

/** `root.pfs`, then its continuations numerically, then every other package by name. */
function comparePacks(a: string, b: string): number {
  const rank = (name: string): [number, bigint, string] => {
    const lower = name.toLowerCase();
    if (lower === 'root.pfs') return [0, 0n, lower];
    const continuation = /^root\.pfs\.(\d{1,9})$/.exec(lower);
    if (continuation) return [1, BigInt(continuation[1]!), lower];
    return [2, 0n, lower];
  };
  const left = rank(a);
  const right = rank(b);
  if (left[0] !== right[0]) return left[0] - right[0];
  if (left[1] !== right[1]) return left[1] < right[1] ? -1 : 1;
  return left[2] < right[2] ? -1 : left[2] > right[2] ? 1 : 0;
}

/** A one-line diagnosis. Never throws: a report that itself throws is not a report. */
function describeError(error: unknown): string {
  if (error instanceof Error && typeof error.message === 'string') return error.message;
  try {
    return String(error);
  } catch {
    return 'unknown failure';
  }
}

/**
 * Validates a whole checkpoint **before** any of it is applied.
 *
 * Nothing here is a best-effort check: the worker mutates the engine's filesystem for every file it is
 * handed, so a payload that is wrong must be refused while the engine is still untouched. The rules are
 * the mirror of the ones `readCheckpointFiles` applies on the way out — same path shape, same per-file
 * ceiling, same count and sum bounds — because a set that could not be captured must not be restorable
 * either. The kind and version are checked first: a slot written by a different format is not a
 * checkpoint this runtime understands, and guessing would be applying bytes to a live scene.
 */
function readCheckpointPayload(state: SaveState): readonly CompatCheckpointFile[] {
  if (state.version !== 1) {
    throw new UnsupportedEngineError(
      `this compat checkpoint has format version ${String(state.version)}, and this build reads 1`,
      'artemis',
    );
  }
  // The guard is the only way to read `kind`/`files`, and that is deliberate: a slot that merely
  // *looks* like a `SaveState` is not a checkpoint, and treating one as if it were is how a foreign
  // format gets applied to a live scene.
  if (!isSaveStateFiles(state)) {
    throw new UnsupportedEngineError(
      `this slot is not a ${COMPAT_SAVE_KIND}; it was not written by this runtime`,
      'artemis',
    );
  }
  if (state.kind !== COMPAT_SAVE_KIND) {
    throw new UnsupportedEngineError(
      `this slot is "${state.kind}", not a ${COMPAT_SAVE_KIND}; it was not written by this runtime`,
      'artemis',
    );
  }
  const entries = state.files;
  if (!Array.isArray(entries) || entries.length === 0) {
    throw new UnsupportedEngineError('this compat checkpoint carries no files', 'artemis');
  }
  if (entries.length > COMPAT_SAVE_MAX_FILES) {
    throw new UnsupportedEngineError('this compat checkpoint carries more files than a save can', 'artemis');
  }
  const decoded: CompatCheckpointFile[] = [];
  for (const entry of entries) {
    if (typeof entry?.path !== 'string' || typeof entry.data !== 'string') {
      throw new UnsupportedEngineError('this compat checkpoint has a malformed file entry', 'artemis');
    }
    try {
      decoded.push({ path: entry.path, data: base64ToBytes(entry.data) });
    } catch {
      throw new UnsupportedEngineError(`this compat checkpoint's ${entry.path} is not base64`, 'artemis');
    }
  }
  // The same validator a capture runs: paths, duplicates, per-file and total bounds, and the slot.
  try {
    return validateFileSet(decoded, { requireSlot: true, allowEmpty: false });
  } catch (error) {
    throw new UnsupportedEngineError(`this compat checkpoint is not usable: ${describeError(error)}`, 'artemis');
  }
}

/**
 * A private copy of bytes that arrived from outside.
 *
 * A `SharedArrayBuffer` is refused rather than copied: it is the window the frame loop writes into, so
 * a "saved" file held by a store could change under the engine, and a checkpoint must be the bytes
 * that were on disk at the moment it was taken.
 */
function ownedBytes(bytes: Uint8Array): Uint8Array {
  if (typeof SharedArrayBuffer === 'function' && bytes.buffer instanceof SharedArrayBuffer) {
    throw new Error('a persisted file shares memory with the frame window and cannot be trusted');
  }
  return new Uint8Array(bytes);
}

/**
 * Base64 for a captured file.
 *
 * Paired with `base64ToBytes`, which is the side that has to be strict. This one is only ever handed
 * bytes the worker produced, so the round trip is what matters, not rejecting hostile input.
 */
function bytesToBase64(bytes: Uint8Array): string {
  const alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
  let out = '';
  for (let index = 0; index < bytes.length; index += 3) {
    const b0 = bytes[index] ?? 0;
    const b1 = index + 1 < bytes.length ? (bytes[index + 1] ?? 0) : 0;
    const b2 = index + 2 < bytes.length ? (bytes[index + 2] ?? 0) : 0;
    out += alphabet[b0 >> 2];
    out += alphabet[((b0 & 3) << 4) | (b1 >> 4)];
    out += index + 1 < bytes.length ? alphabet[((b1 & 15) << 2) | (b2 >> 6)] : '=';
    out += index + 2 < bytes.length ? alphabet[b2 & 63] : '=';
  }
  return out;
}

/**
 * Whether the object can be drawn into at all.
 *
 * A plain value passed as a canvas satisfies "it has a `transferControlToOffscreen` function" in a
 * test and then throws inside the page. `getContext` is the smallest honest proxy for "this is a
 * canvas": an `HTMLCanvasElement` and an `OffscreenCanvas` both have it, and the transfer's own
 * precondition is that the canvas has not handed out a context yet.
 */
function hasDrawableSurface(canvas: unknown): boolean {
  return typeof (canvas as { getContext?: unknown } | null | undefined)?.getContext === 'function';
}

/**
 * Retargets the host's canvas and returns what actually goes to the worker.
 *
 * The offscreen result exists only **after** the conversion, so it is the promise's value rather
 * than a field a reader could take before. It is not cached on the element either: the post happens
 * once per load, and a second load must perform a second conversion.
 */
type PostTransfer = (surface: RuntimeOptions['surface']) => Promise<OffscreenCanvas>;

/** Where the pixels go, and how the element is prepared for that. */
interface TransferredCanvas {
  /** The drawable object the element is, for the size attributes that must precede the transfer. */
  readonly element: { width?: number; height?: number };
  /** The conversion, or `null` when the host's canvas already **is** an `OffscreenCanvas`. */
  readonly postTransfer: PostTransfer | null;
}

/**
 * Prepares the host's canvas for the transfer.
 *
 * The defect this replaces handed the worker `canvas as unknown as OffscreenCanvas` — a TypeScript
 * assertion, which is a claim, not a conversion. `HTMLCanvasElement` and `OffscreenCanvas` are
 * distinct types with different lifecycles, and the assertion's only runtime effect was a
 * `DataCloneError` inside `postMessage`: a fake worker accepts anything, so the cast passed every
 * test and the real browser refused all of them.
 *
 * `transferControlToOffscreen()` is the conversion. It keeps the element the page's own — same
 * identity, same DOM node, same gesture handlers — and moves only its rendering target, so no DOM
 * replacement is performed here and none may be: replacing the element would unbind every listener
 * the host attached to it.
 */
function transferCanvas(hostCanvas: unknown): TransferredCanvas {
  const already = asOffscreenCanvas(hostCanvas);
  if (already) {
    // The host already made one. It is transferable as-is and has no `transferControlToOffscreen`;
    // applying a size to it here would be a page-side second guess at geometry the native side owns.
    return { element: already, postTransfer: null };
  }
  const canvas = hostCanvas as Record<string, unknown>;
  const transfer = canvas['transferControlToOffscreen'];
  if (typeof transfer !== 'function') {
    // The capability gate is what refuses this shape; the message here is the honest one for a
    // canvas that reached the transfer point anyway.
    throw new Error(
      'the Artemis compat runtime needs a canvas it can transfer: this host canvas is neither an ' +
        'OffscreenCanvas nor an HTMLCanvasElement with transferControlToOffscreen',
    );
  }
  return {
    element: canvas as { width?: number; height?: number },
    postTransfer: async (surface) => {
      // Attributes must be written through the element the host took its size from. After the
      // transfer they are inert, and the surface the host already published is used unchanged: it
      // is CSS-pixel geometry in the page's own coordinate space, and the native `Resize` reads it
      // in exactly that space. Re-deriving it here would move both the letterbox and `MapPoint`.
      applySurface(canvas, surface);
      // `call` with the element as receiver: the conversion is a method of *this* canvas, and a
      // detached call does not retarget anything.
      const offscreen = transfer.call(canvas);
      if (!asOffscreenCanvas(offscreen)) {
        throw new Error('transferControlToOffscreen did not return an OffscreenCanvas');
      }
      // One `await` after the conversion, so a disposal landing here is seen by the recheck that
      // follows rather than being raced by the post.
      await Promise.resolve();
      return offscreen;
    },
  };
}

/**
 * The value as an `OffscreenCanvas`, or `null`.
 *
 * The constructor is consulted only when it exists: a page without the global can still hand over
 * an element that converts, and a missing constructor must never *reject* a canvas. A host that
 * already passed one is recognised by the absence of `transferControlToOffscreen`, which is a
 * platform rule for `OffscreenCanvas` and not an assumption about the host's object.
 */
function asOffscreenCanvas(value: unknown): OffscreenCanvas | null {
  if (!value || typeof value !== 'object') return null;
  if (typeof (globalThis as { OffscreenCanvas?: unknown }).OffscreenCanvas === 'function' && value instanceof OffscreenCanvas) {
    return value;
  }
  return typeof (value as { transferControlToOffscreen?: unknown }).transferControlToOffscreen === 'function'
    ? null
    : (value as OffscreenCanvas);
}

/**
 * Writes the surface the host declared onto the canvas **before** the transfer.
 *
 * Before, because it is the last moment the element is still the page's; after
 * `transferControlToOffscreen` its `width`/`height` are inert and the size is the offscreen
 * object's. Zero or non-finite values are not written: a canvas sized `0x0` presents nothing, and
 * leaving the host's own attributes alone is the smaller defect than blanking a live canvas.
 */
function applySurface(canvas: Record<string, unknown>, surface?: RuntimeOptions['surface']): void {
  if (!surface) return;
  for (const key of ['width', 'height'] as const) {
    const value = surface[key];
    if (Number.isFinite(value) && value > 0) canvas[key] = value;
  }
}

export { COMPAT_OPEN_MODE, COMPAT_OPEN_PACK };
