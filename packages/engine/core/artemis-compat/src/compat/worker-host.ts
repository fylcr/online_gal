/**
 * Page facade: one inflight frame, main-thread pause gate, dispose releases pending.
 * Checkpoint and restore are serialized behind one queue. A missing native pause is a real refusal.
 */
import type { GameSource } from '@online-gal/engine-api';
import { AudioRelay } from './audio-relay.js';
import { CompatReadHost, type CompatReadRequest } from './read-host.js';
import {
  allocateCompatBlock,
  COMPAT_PROTOCOL_VERSION,
  COMPAT_SAVE_MAX_FILE_BYTES,
  COMPAT_SAVE_MAX_FILES,
  COMPAT_SAVE_MAX_TOTAL_BYTES,
  COMPAT_SAVE_SLOT,
  COMPAT_WINDOW_BYTES,
  READ_FATAL,
  type CompatAnimationTraceOptions,
  type CompatFileDescriptor,
  type CompatSharedBlock,
} from './protocol.js';

export interface CompatWorkerInit extends CompatAnimationTraceOptions {
  readonly engineJsUrl: string;
  readonly wasmUrl: string;
  /** Raw source only. The published worker already contains the helper, so production leaves this out. */
  readonly audioHelperUrl?: string;
  /** Transferred to the worker. `null` only when the host has no canvas at all. */
  readonly canvas: OffscreenCanvas | null;
  readonly descriptors: readonly CompatFileDescriptor[];
  readonly windowBytes?: number;
  /** Tests only. Production always uses `READ_TIMEOUT_MS`. */
  readonly readTimeoutMs?: number;
  /**
   * The host surface, published to the engine through `artc_web_resize` before the boot frame.
   *
   * Omitted means the engine keeps the `system.ini` stage size, which is what a headless test wants;
   * a real page passes it so the first frame letterboxes against the canvas it actually has.
   */
  readonly surface?: { readonly width: number; readonly height: number };
  /**
   * Saved files to install before the engine opens, so a cold boot sees the bank the session left.
   *
   * Empty is the honest "no save yet" answer and is allowed. These bytes are **copies**, and the
   * worker validates the whole set before the first write, so nothing here can half-apply.
   */
  readonly initialFiles?: readonly CompatCheckpointFile[];
}

export interface CompatWorkerHostOptions {
  readonly source: GameSource;
  readonly createWorker: () => Worker;
  /** Reads `muted`/`mute`. Absent means unmuted: a missing host is not a mute. */
  readonly setting?: <T>(key: string, fallback: T) => T;
  /** Terminal transport notification, delivered once after demand has been revoked. */
  readonly onFatal?: (reason: string) => void;
  /** Where the engine's own `print`/`printErr` lines go. Absent means they are dropped here. */
  readonly onLog?: (level: 'info' | 'error', line: string) => void;
}

interface Pending {
  readonly resolve: (message: WorkerReply) => void;
  readonly reject: (error: Error) => void;
  readonly timer: ReturnType<typeof setTimeout>;
}

interface WorkerReply {
  readonly type: string;
  readonly id?: number;
  readonly generation?: number;
  readonly credit?: number;
  readonly running?: boolean;
  readonly reason?: string;
  readonly count?: number;
  /** Engine output. Present only on a `log` message. */
  readonly level?: string;
  readonly line?: string;
  /** Native frame counters, present only on a `frame-ok`. Absent means the worker did not report them. */
  readonly started?: number;
  readonly presented?: number;
  readonly revision?: number;
  readonly stage?: number;
  /** The native stage height. A width without a height cannot describe a letterbox. */
  readonly stageHeight?: number;
  readonly nowMs?: number;
  /** `checkpoint-ok` only: the files the engine's save directory held after the native ACK. */
  readonly files?: readonly { readonly path?: unknown; readonly data?: unknown }[];
  /** `restore-ok` only: how many files were installed, and how the native load answered. */
  readonly installed?: number;
  readonly loaded?: number;
  /** `frame-ok` only. Absent means this frame did not carry audio. */
  readonly audio?: AudioFrame;
}

interface AudioVoice { readonly handle: number; readonly rate: number; readonly key: string }
interface AudioBlock extends AudioVoice {
  readonly samples: Int16Array;
  readonly gain1000: number;
  readonly pan1000: number;
}
interface AudioStatus {
  readonly connected: boolean;
  readonly autoplayLocked: boolean;
  readonly playedFrames: number;
  readonly peak: number;
  readonly realGameVerified: false;
  readonly unavailable?: string;
}
interface AudioFrame {
  readonly available: boolean;
  readonly voices?: readonly AudioVoice[];
  readonly blocks?: readonly AudioBlock[];
  readonly unavailable?: string;
}

const REPLY_TIMEOUT_MS = 30_000;
const DISPOSE_TIMEOUT_MS = 1_000;

/**
 * The one key prefix this transport owns in the host's store.
 *
 * Everything the compat runtime persists lives under it, and a fresh `load` reads **only** this
 * prefix — so the game's own packs and assets can never be mistaken for a save, and a save can never
 * overwrite one.
 */
export const COMPAT_STORE_PREFIX = 'artemis-compat/';

export class CompatWorkerHost {
  private readonly source: GameSource;
  private readonly createWorker: () => Worker;
  private readonly setting: (<T>(key: string, fallback: T) => T) | undefined;
  private readonly onFatal: ((reason: string) => void) | undefined;
  private readonly onLog: ((level: 'info' | 'error', line: string) => void) | undefined;
  private worker: Worker | null = null;
  private readHost: CompatReadHost | null = null;
  private block: CompatSharedBlock | null = null;
  private nextId = 1;
  private readonly pending = new Map<number, Pending>();
  private state: 'idle' | 'ready' | 'closed' = 'idle';
  private paused = false;
  private fatal: string | null = null;
  private frameGeneration = 0;
  private frameCredit = 0;
  private frameDemand = false;
  private frameFlight: { readonly generation: number; readonly promise: Promise<boolean> } | null = null;
  private disposeTimer: ReturnType<typeof setTimeout> | null = null;
  private disposedPromise: Promise<void> | null = null;
  private readonly disposeResolvers: Array<() => void> = [];
  private terminated = false;
  private frameClock = 0;
  /** The **native** counters, taken from the worker's own ACK. Never incremented here. */
  private revision: number | null = null;
  private presented: number | null = null;
  private nativeStarted: number | null = null;
  private stage: number | null = null;
  private stageHeight: number | null = null;
  private nowMs: number | null = null;
  private booted = false;
  /** The pause the host last asked for. Read when an operation has to put the engine back. */
  private pauseIntent = false;
  /**
   * One serial queue for everything that mutates the engine's run state: pause, resume, checkpoint and
   * restore. A frame in flight never overlaps a member. See {@link serialize}.
   */
  private operations: Promise<void> = Promise.resolve();
  /** Members queued but not yet settled, including the one running. See {@link busy}. */
  private busyDepth = 0;
  /** Main-thread speaker. One audio RPC is in flight at a time; see {@link audioBusy}. */
  private readonly relay = new AudioRelay({ onConsumed: (handle, frames) => this.acknowledge(handle, frames) });
  private audioBusy = false;
  /** Bumped by clear/restore so an ACK for the previous generation is refused, not posted. */
  private audioEpoch = 0;
  /** Newest cumulative not yet posted, one slot per handle. Never a queue and never a lower value. */
  private pendingAck = new Map<number, { frames: number; epoch: number }>();
  private audio: AudioStatus = { connected: false, autoplayLocked: true, playedFrames: 0, peak: 0, realGameVerified: false };

  constructor(options: CompatWorkerHostOptions) {
    this.source = options.source;
    this.createWorker = options.createWorker;
    this.setting = options.setting;
    this.onLog = options.onLog;
    this.onFatal = options.onFatal;
  }

  async load(init: CompatWorkerInit): Promise<void> {
    if (this.state !== 'idle') throw new Error('already loaded');
    const worker = this.createWorker();
    this.worker = worker;
    const block = allocateCompatBlock(init.windowBytes ?? COMPAT_WINDOW_BYTES);
    this.block = block;
    this.readHost = new CompatReadHost(this.source, block, init.descriptors);
    worker.addEventListener('message', (event: MessageEvent) => this.receive(event.data));
    worker.addEventListener('error', () => this.stop('worker error'));
    worker.addEventListener('messageerror', () => this.stop('worker message error'));
    const message = {
      type: 'init',
      id: this.id(),
      canvas: init.canvas,
      engineJsUrl: init.engineJsUrl,
      wasmUrl: init.wasmUrl,
      ...(init.audioHelperUrl === undefined ? {} : { audioHelperUrl: init.audioHelperUrl }),
      descriptors: init.descriptors,
      block: block.buffer,
      // Saved files, installed by the worker **before** `artc_web_open`. Always an array: an empty
      // one is "no save yet", and the worker refuses anything that is not a list.
      initialFiles: (init.initialFiles ?? []).map((file) => ({ path: file.path, data: file.data })),
      ...(init.readTimeoutMs === undefined ? {} : { readTimeoutMs: init.readTimeoutMs }),
      ...(init.surface === undefined ? {} : { surface: init.surface }),
      ...(init.animationTrace === true ? { animationTrace: true } : {}),
    };
    try {
      // `request` stamps the id, so nothing here may carry one: two id sources is how a reply gets
      // registered under one number and posted under another.
      const reply = await this.request(message, init.canvas ? [init.canvas] : []);
      if (reply.type !== 'ready') throw new Error(reply.reason || reply.type);
      if (this.isClosed()) throw new Error(this.fatal || 'closed');
      this.revokeFrames();
      this.state = 'ready';
      // The engine letterboxes against the canvas size the host declared. Sent before any frame, so
      // the first `artc_web_frame` already maps input and viewport against the real surface.
      if (init.surface !== undefined && init.canvas !== null) {
        await this.input({ kind: 'resize', width: init.surface.width, height: init.surface.height, down: false });
      }
    } catch (error) {
      this.dispose();
      throw error;
    }
  }

  /** External ticks carry demand, not native time. Only the latest demand survives an ACK. */
  demandFrame(): void {
    if (this.state !== 'ready' || this.fatal || this.paused || this.busy) return;
    if (this.frameFlight) {
      this.frameDemand = true;
      return;
    }
    void this.frame().catch(() => undefined); // The owner reports terminal failures through stop.
  }

  /** Explicit callers join the current credit; joining never creates another demand. */
  frame(): Promise<boolean> {
    if (this.fatal) return Promise.reject(new Error(this.fatal));
    if (this.state !== 'ready') return Promise.reject(new Error(this.state === 'closed' ? 'closed' : 'not ready'));
    if (this.paused || this.busy) return Promise.resolve(true);
    if (this.frameFlight) return this.frameFlight.promise;
    const generation = this.frameGeneration;
    const credit = ++this.frameCredit;
    this.frameClock += 1;
    const promise = this.request({ type: 'frame', generation, credit }).then((reply) => {
      if (reply.type === 'error' || reply.running === false) throw new Error(reply.reason || 'engine stopped');
      if (generation !== this.frameGeneration || this.state !== 'ready') return true;
      if (reply.generation !== generation || reply.credit !== credit) throw new Error('invalid frame credit ACK');
      if (reply.type !== 'frame-ok') {
        throw new Error(reply.reason || 'engine stopped');
      }
      this.revision = count(reply.revision);
      this.presented = count(reply.presented);
      this.nativeStarted = count(reply.started);
      this.stage = count(reply.stage);
      this.stageHeight = count(reply.stageHeight);
      this.nowMs = typeof reply.nowMs === 'number' && Number.isFinite(reply.nowMs) ? reply.nowMs : null;
      this.booted = reply.started === 1;
      this.noteAudio(reply.audio);
      return true;
    }).catch((error: unknown) => {
      const failure = error instanceof Error ? error : new Error(String(error));
      if (this.state !== 'closed') this.stop(failure.message);
      throw failure;
    });
    this.frameFlight = { generation, promise };
    // Own both outcomes even when a public caller drops its join promise.
    void promise.then(() => this.releaseFrame(promise), () => this.releaseFrame(promise));
    return promise;
  }

  private releaseFrame(promise: Promise<boolean>): void {
    if (this.frameFlight?.promise !== promise) return;
    const generation = this.frameFlight.generation;
    this.frameFlight = null;
    const demanded = this.frameDemand;
    this.frameDemand = false;
    if (demanded && generation === this.frameGeneration) this.demandFrame();
  }

  private revokeFrames(): void {
    this.frameGeneration += 1;
    this.frameDemand = false;
  }

  /** Hold across the facade's persistence queue, before its worker operation can start. */
  holdFrames(): () => void {
    this.revokeFrames();
    this.busyDepth += 1;
    let held = true;
    return () => {
      if (!held) return;
      held = false;
      this.busyDepth -= 1;
    };
  }

  private isClosed(): boolean { return this.state === 'closed'; }

  /** Gesture entry. Creates and resumes the context now, and never clears an explicit pause. */
  unlockAudio(): Promise<void> {
    return this.relay.unlock();
  }

  /** Drop queued PCM and retire its ACKs before a restore replaces the voice set. */
  clearAudio(): void {
    this.audioEpoch += 1;
    this.pendingAck.clear();
    this.audio = { ...this.audio, playedFrames: 0 };
    this.relay.clear();
  }

  /**
   * Main-thread gate first. The transition runs on the shared queue, and the worker must answer with 1.
   *
   * The ordering is a **queue**, not a race: pause, resume, checkpoint and restore are the same kind of
   * member, so a `resume()` posted while a `pause()` is still being acknowledged runs after it, and
   * neither ever overlaps a frame that was already on the wire. Every member reaches the engine in call
   * order, so the engine is walked through each state the host asked for and ends on the latest one.
   *
   * The gate (`this.paused`) is set **synchronously**, at the moment the transition is queued. A frame
   * arriving between two transitions therefore sees the newest intent immediately, and — because the
   * queue is already busy at that moment — no frame is put on the wire while this one is pending.
   */
  setPaused(paused: boolean): Promise<void> {
    this.paused = paused;
    this.pauseIntent = paused;
    return this.serialize(async () => {
      // Closed or already failed: nothing to post. A later intent that superseded this one still runs
      // its own member — the first call must not skip just because the latest intent is not this one.
      if (this.state !== 'ready' || this.fatal) return;
      const reply = await this.request({ type: 'pause', paused });
      if (reply.type !== 'ack') throw new Error(reply.reason || 'pause refused');
      await (paused ? this.relay.pause() : this.relay.resume());
    }).catch((error: unknown) => {
      this.stop(error instanceof Error ? error.message : String(error));
      throw error;
    });
  }

  /**
   * One input event, as the worker's own `deliver` expects it.
   *
   * The three kinds are the ones the native CABI can actually receive: a pointer press/release, a
   * plain move, and a key. Sending a kind native has no function for is not an approximation — the
   * worker counts zero native calls, and the caller sees that number.
   */
  async input(event: {
    readonly kind: 'pointer' | 'pointer-move' | 'key' | 'resize' | 'input';
    readonly x?: number;
    readonly y?: number;
    readonly width?: number;
    readonly height?: number;
    readonly down: boolean;
    readonly key?: string;
    readonly code?: number;
  }): Promise<number> {
    const reply = await this.request({ type: 'input', ...event });
    if (reply.type === 'input-ok') return Number(reply.count ?? 0);
    throw new Error(reply.reason || 'input refused');
  }

  /**
   * Asks the worker for the engine's own checkpoint. The **worker returns 1** before we read a byte.
   *
   * `artc_web_save` is synchronous and only the worker's thread may call it, so the truthful order is:
   * the worker writes the engine's files, reads them back out of its filesystem, and sends the bytes
   * here. Nothing is fabricated on this side — a worker that could not checkpoint sends `error`, and
   * this rejects rather than resolving a slot built from nothing.
   *
   * The returned `files` are validated here, not trusted: a path that could escape the store, a count
   * past the set bound, a per-file size past the native `max_bytes`, or a total that exceeds the sum
   * bound is refused **before** anything is persisted.
   */
  checkpoint(): Promise<CompatCheckpointFiles> {
    return this.serialize(async () => {
      if (this.state !== 'ready' || this.fatal) throw new Error(this.fatal || 'the session is not ready');
      const reply = await this.request({ type: 'checkpoint' });
      if (reply.type !== 'checkpoint-ok') throw new Error(reply.reason || 'checkpoint refused');
      return readCheckpointFiles(reply.files);
    });
  }

  /**
   * Installs a validated set of files and asks the engine to load the slot.
   *
   * The worker stops first: it parks the frame loop and tells native to pause, then installs the files,
   * then calls `artc_web_load`. A `0` from native is a **rejection**, and the worker rolls its own
   * files back before reporting it — a load that half-applied is worse than one that failed. Once the
   * load is accepted the worker puts the engine back into the run state the page last asked for, and
   * waits for native to answer `1` for that too: a restore that leaves a running session paused is the
   * opposite defect, and one that resumes a session the player paused is worse.
   */
  restore(files: readonly CompatCheckpointFile[]): Promise<{ readonly installed: number }> {
    return this.serialize(async () => {
      if (this.state !== 'ready' || this.fatal) throw new Error(this.fatal || 'the session is not ready');
      // The run state travels **with** the restore, read at the moment the request is posted: the queue
      // has settled every earlier transition by then, so `pauseIntent` is the newest one there is.
      this.clearAudio();
      const reply = await this.request({ type: 'restore', files, paused: this.pauseIntent });
      if (reply.type !== 'restore-ok') throw new Error(reply.reason || 'restore refused');
      const loaded = typeof reply.loaded === 'number' ? reply.loaded : 0;
      if (loaded !== 1) throw new Error('the engine refused the restored checkpoint');
      this.noteAudio(reply.audio);
      return { installed: typeof reply.installed === 'number' ? reply.installed : 0 };
    });
  }

  /**
   * One run-state queue for every caller that must not interleave with a frame or with each other.
   *
   * A checkpoint and a restore are both "the engine is doing something else right now", and a pause is
   * the moment a frame must not be running — three members on one thread would interleave native calls,
   * and a checkpoint queued behind a pause would otherwise be free to start while the frame that pause
   * was waiting for is still on the wire. Each member runs after the previous one settles, including
   * after a rejected one, because a queue that broke on the first failure would strand every later
   * caller. The caller still sees its own rejection.
   */
  private serialize<T>(operation: () => Promise<T>): Promise<T> {
    // Closed at enqueue time, before `then`: a frame asked in the same turn must already see busy.
    this.revokeFrames();
    this.busyDepth += 1;
    const run = async (): Promise<T> => {
      try {
        // A member that outlived the session settles without touching the engine.
        if (this.isClosed()) throw new Error(this.fatal || 'closed');
        // Settle whatever frame is already on the wire before this member touches the engine. The
        // promise is cleared by its own handlers, so the current value is a frame someone started
        // before this call — never one this member's own gate would have refused.
        await this.frameFlight?.promise;
        if (this.isClosed()) throw new Error(this.fatal || 'closed');
        return await operation();
      } finally {
        this.busyDepth -= 1;
      }
    };
    const next = this.operations.then(run, run);
    // The queue must survive a refused member; the caller still sees the rejection.
    this.operations = next.then(
      () => undefined,
      () => undefined,
    );
    return next;
  }

  /** True while a transition or an operation is queued or running. Read by the frame gate. */
  get busy(): boolean {
    return this.busyDepth > 0;
  }

  dispose(): void {
    if (this.state === 'closed') return;
    this.state = 'closed';
    this.revokeFrames();
    // Any transition still queued behind the queue finds `state !== 'ready'` and stays silent: a
    // pause posted to a terminated worker is a request nobody can answer.
    this.paused = true;
    this.pauseIntent = true;
    this.audioEpoch += 1;
    this.pendingAck.clear();
    void this.relay.dispose();
    this.readHost?.close();
    const failure = new Error(this.fatal || 'disposed');
    for (const entry of this.pending.values()) {
      clearTimeout(entry.timer);
      entry.reject(failure);
    }
    this.pending.clear();
    const worker = this.worker;
    if (!worker) {
      this.finishDispose();
      return;
    }
    this.disposeTimer = setTimeout(() => this.finishDispose(), DISPOSE_TIMEOUT_MS);
    try {
      worker.postMessage({ type: 'dispose', id: 0 });
    } catch {
      this.finishDispose();
    }
  }

  /**
   * Resolves once the worker is gone: the native shutdown acknowledgement, or the dispose fallback.
   *
   * **One** promise, shared by every caller. The gate it used to install was a single slot, so a
   * second `whenDisposed()` before termination silently replaced the first caller's resolver and left
   * that caller hanging forever. Memoizing the promise makes "the worker is gone" one fact with one
   * answer, however many people ask.
   *
   * Calling it before any `dispose()` is a wait on nothing: a host that was never loaded has no
   * worker, and a caller that wants to wait must `dispose()` first. That is the order every caller in
   * this package uses, and it is the order the interface makes natural — `dispose()` is synchronous.
   */
  whenDisposed(): Promise<void> {
    if (this.terminated) return Promise.resolve();
    return (this.disposedPromise ??= new Promise<void>((resolve) => {
      this.disposeResolvers.push(resolve);
    }));
  }

  describe(): Readonly<Record<string, unknown>> {
    return {
      protocol: COMPAT_PROTOCOL_VERSION,
      state: this.state,
      paused: this.paused,
      pauseIntent: this.pauseIntent,
      queueBusy: this.busy,
      audio: this.audio,
    };
  }

  /**
   * What this page half observed, in the read-only shape the runtime publishes.
   *
   * `frameClock` is the one number this class owns: it counts frames **requested**. Every other
   * counter is the engine's, forwarded from the worker's `frame-ok`, so `presented` stays put across
   * a frame the compositor's redraw gate skipped. A field the worker has not reported is `null` —
   * "unverified", never a zero that reads as "the engine presented nothing".
   */
  diagnostics(): Readonly<Record<string, unknown>> {
    return {
      state: this.state,
      booted: this.booted,
      frameClock: this.frameClock,
      revision: this.revision,
      presented: this.presented,
      started: this.nativeStarted,
      stage: this.stage,
      stageHeight: this.stageHeight,
      nowMs: this.nowMs,
      paused: this.paused,
      fatal: this.fatal,
    };
  }

  /** One audio exchange per frame. A second frame joins the first instead of posting another RPC. */
  private noteAudio(frame: AudioFrame | undefined): void {
    if (!frame) return;
    if (this.audioBusy) { this.audio = { ...this.audio, connected: frame.available }; return; }
    if (this.state !== 'ready') return;
    if (!frame.available) {
      this.audio = { ...this.audio, connected: false, unavailable: frame.unavailable ?? 'unavailable' };
      return;
    }
    const master = this.relayMuted() ? 0 : 1;
    const credits = (frame.voices ?? [])
      .map((voice) => ({ handle: voice.handle, frames: this.relay.credit(voice.handle, voice.rate) }))
      .filter((credit) => credit.frames > 0);
    for (const block of frame.blocks ?? []) this.play(block, master);
    if (credits.length === 0) {
      this.audio = { ...this.audio, connected: true };
      return;
    }
    const epoch = this.audioEpoch;
    this.audioBusy = true;
    void this.request({ type: 'audio-pull', credits })
      .then((pulled) => {
        if (epoch !== this.audioEpoch || this.state !== 'ready') return;
        for (const block of pulled.audio?.blocks ?? []) this.play(block, master);
        this.audio = { ...this.audio, connected: true };
      })
      .catch(() => undefined)
      .finally(() => this.releaseAudio());
  }

  private play(block: AudioBlock, master: number): void {
    const raw = block.samples;
    const samples = raw instanceof Int16Array ? raw.slice()
      : raw && typeof (raw as { length?: number }).length === 'number' ? Int16Array.from(raw as ArrayLike<number>)
      : null;
    if (!samples) return;
    try {
      this.relay.push({
        handle: block.handle, rate: block.rate, samples,
        gain1000: Math.max(0, Math.min(1000, Math.round(block.gain1000 * master))),
        pan1000: block.pan1000,
      });
    } catch {
      /* ahead of the window or a dead context: the next frame asks again */
    }
    const stats = this.relay.stats();
    this.audio = { ...this.audio, peak: stats.peak, autoplayLocked: stats.contextState !== 'running' };
  }

  private acknowledge(handle: number, frames: number): void {
    if (this.state !== 'ready') return;
    const epoch = this.audioEpoch;
    // One audio RPC stays in flight. A newer cumulative waits in the per-handle slot, not a second post.
    if (this.audioBusy) { this.pendingAck.set(handle, { frames, epoch }); return; }
    this.sendAck(handle, frames, epoch);
  }

  private sendAck(handle: number, frames: number, epoch: number): void {
    this.audioBusy = true;
    void this.request({ type: 'audio-ack', handle, frames })
      .then((reply) => {
        // Only a real ACK=1 (`audio-ack-ok`) counts, and a late older reply must not lower it.
        if (epoch !== this.audioEpoch || reply.type !== 'audio-ack-ok' || frames <= this.audio.playedFrames) return;
        this.audio = { ...this.audio, playedFrames: frames };
      })
      .catch(() => undefined)
      .finally(() => this.releaseAudio());
  }

  /** The in-flight audio RPC ended. Post the newest unconfirmed cumulative, one handle, then stop. */
  private releaseAudio(): void {
    this.audioBusy = false;
    if (this.state !== 'ready') { this.pendingAck.clear(); return; }
    for (const [handle, next] of this.pendingAck) {
      this.pendingAck.delete(handle);
      if (next.epoch !== this.audioEpoch) continue;
      this.sendAck(handle, next.frames, next.epoch);
      return;
    }
  }

  private relayMuted(): boolean {
    const read = this.setting;
    if (!read) return false;
    return read<boolean>('muted', false) || read<boolean>('mute', false);
  }

  private receive(message: WorkerReply & Partial<CompatReadRequest>): void {
    if (!message || typeof message.type !== 'string') return;
    if (message.type === 'read') {
      const request = message as unknown as CompatReadRequest;
      void this.readHost?.answer(request).then((count) => {
        if (count === READ_FATAL) this.stop(this.readHost?.lastReason || 'read failed');
      });
      return;
    }
    if (message.type === 'fatal') {
      this.stop(message.reason || 'worker fatal');
      return;
    }
    // The engine's own stdout/stderr. Forwarded rather than stored: a game that fails silently on the
    // worker's console is the failure mode this exists to remove, and a page can only show what it
    // was sent.
    if (message.type === 'log') {
      try {
        this.onLog?.(message.level === 'error' ? 'error' : 'info', String(message.line ?? ''));
      } catch {
        // A logger that throws must not stop the transport.
      }
      return;
    }
    if (message.type === 'disposed-ack') {
      this.finishDispose();
      return;
    }
    if (typeof message.id !== 'number') return;
    const entry = this.pending.get(message.id);
    if (!entry) return;
    this.pending.delete(message.id);
    clearTimeout(entry.timer);
    entry.resolve(message);
  }

  /**
   * Posts one message and waits for its reply.
   *
   * The id is stamped **here**, at the single point where the pending entry is registered, and any
   * id already on `message` is discarded. Two id sources is not a style question: the previous shape
   * let `load` compute an id for the message and `request` compute a second one for the map, so the
   * reply arrived for a number nobody was waiting on and the caller sat out the full timeout.
   */
  private request(message: object, transfer: Transferable[] = []): Promise<WorkerReply> {
    return new Promise((resolve, reject) => {
      const id = this.id();
      const timer = setTimeout(() => {
        this.pending.delete(id);
        this.stop(`${(message as { type: string }).type} timed out`);
        reject(new Error(this.fatal || 'timed out'));
      }, REPLY_TIMEOUT_MS);
      this.pending.set(id, { resolve, reject, timer });
      try {
        // ponytail: the transfer list is the only way an OffscreenCanvas reaches the worker.
        if (!this.worker || this.state === 'closed') throw new Error(this.fatal || 'closed');
        this.worker.postMessage({ ...message, id }, transfer);
      } catch (error) {
        clearTimeout(timer);
        this.pending.delete(id);
        reject(error instanceof Error ? error : new Error(String(error)));
      }
    });
  }

  private stop(reason: string): void {
    if (this.fatal || this.state === 'closed') return;
    this.fatal = reason;
    this.dispose();
    try { this.onFatal?.(reason); } catch { /* notification cannot undo terminal teardown */ }
  }

  private finishDispose(): void {
    if (this.terminated) return;
    this.terminated = true;
    if (this.disposeTimer) clearTimeout(this.disposeTimer);
    this.disposeTimer = null;
    const worker = this.worker;
    this.worker = null;
    this.readHost = null;
    this.block = null;
    try {
      worker?.terminate();
    } catch {
      /* already dead */
    }
    for (const resolve of this.disposeResolvers.splice(0)) resolve();
  }
  private id(): number {
    const id = this.nextId;
    this.nextId = id >= 0x7fffffff ? 1 : id + 1;
    return id;
  }
}

/** A native integer field from a reply, or `null` when the worker did not report one. */
function count(value: unknown): number | null {
  return typeof value === 'number' && Number.isSafeInteger(value) ? value : null;
}

/** One captured file: a validated relative path and its exact bytes. */
export interface CompatCheckpointFile {
  readonly path: string;
  readonly data: Uint8Array;
}

export type CompatCheckpointFiles = readonly CompatCheckpointFile[];

/**
 * Validates the worker's `checkpoint-ok` payload before any caller may persist it.
 *
 * Every rule here is enforced **before** a byte reaches the store, because the alternative is a slot
 * whose record points at files a restore would then refuse — or worse, at a path that escapes the save
 * directory when it is written back. The three bounds mirror `save_storage.cpp`: a per-file ceiling of
 * `max_bytes`, and a set that is neither too numerous nor too large to be a save the player made.
 *
 * And the set has to contain the slot. The names in a payload are otherwise free-form strings, so a
 * `checkpoint-ok` that carried anything the worker happened to find would be persisted as a real save
 * and only refuse at load time — the one place a player finds out their save was never one.
 */
export function readCheckpointFiles(files: unknown): CompatCheckpointFiles {
  if (!Array.isArray(files)) throw new Error('the checkpoint did not report its files');
  const entries: CompatCheckpointFile[] = [];
  for (const entry of files) {
    if (typeof entry?.path !== 'string' || typeof entry.data !== 'string') {
      throw new Error('the checkpoint reported a malformed file entry');
    }
    // `base64ToBytes` is the strict half of the wire: length, padding and unused bits are all
    // checked before the output array is allocated.
    entries.push({ path: entry.path, data: base64ToBytes(entry.data) });
  }
  return validateFileSet(entries, { requireSlot: true, allowEmpty: false });
}

/**
 * The same rules for a set that is already in bytes: a restore payload, or the store read on load.
 *
 * `requireSlot` is the difference between the two callers. A checkpoint **is** its slot — a payload
 * without `platform-slot.dat` restores nothing — so a restore or a capture must carry a well-formed
 * ARCV slot. A hydration set is different: the engine may have been closed before it ever saved one,
 * and the banks beside it are exactly what a cold boot needs. So the set bounds, the path rules and
 * the byte rules still apply, and only the slot requirement is dropped.
 */
export function validateFileSet(
  files: readonly CompatCheckpointFile[],
  options: { readonly requireSlot: boolean; readonly allowEmpty: boolean },
): CompatCheckpointFiles {
  if (files.length > COMPAT_SAVE_MAX_FILES) {
    throw new Error(`${files.length} files is past the ${COMPAT_SAVE_MAX_FILES} bound`);
  }
  if (files.length === 0 && !options.allowEmpty) throw new Error('the checkpoint reported no files');
  const seen = new Set<string>();
  const out: CompatCheckpointFile[] = [];
  let total = 0;
  let slot = false;
  for (const entry of files) {
    const path = entry?.path;
    const bytes = entry?.data;
    if (typeof path !== 'string' || !(bytes instanceof Uint8Array)) {
      throw new Error('the checkpoint has a malformed file entry');
    }
    assertRelativePath(path);
    // Case-insensitive collision: two entries that differ only in case would land on one file in a
    // case-folding store, so the second would silently replace the first — a restore of a different
    // set than the one captured.
    const folded = path.toLowerCase();
    if (seen.has(folded)) throw new Error(`the checkpoint reported ${path} twice`);
    seen.add(folded);
    if (bytes.byteLength > COMPAT_SAVE_MAX_FILE_BYTES) {
      throw new Error(`${path} is past the ${COMPAT_SAVE_MAX_FILE_BYTES}-byte per-file bound`);
    }
    total += bytes.byteLength;
    if (total > COMPAT_SAVE_MAX_TOTAL_BYTES) {
      throw new Error(`the checkpoint is past the ${COMPAT_SAVE_MAX_TOTAL_BYTES}-byte total bound`);
    }
    if (path === COMPAT_SAVE_SLOT) {
      if (options.requireSlot) {
        const malformed = slotPayloadError(bytes);
        if (malformed) throw new Error(`the checkpoint's ${COMPAT_SAVE_SLOT} is not a usable slot: ${malformed}`);
      }
      slot = true;
    }
    out.push({ path, data: bytes });
  }
  if (options.requireSlot && !slot) throw new Error(`the checkpoint does not carry ${COMPAT_SAVE_SLOT}`);
  return out;
}

/**
 * The engine's own checkpoint format, read all the way to the end of the bank.
 *
 * A copy of the worker's `slotPayloadError`, in the language of this half of the wire. Both copies
 * exist because the worker cannot import this module and this module cannot run in the worker — and
 * both are checked against a real payload by the test, because the same bytes must reach the same
 * answer on either side. `''` means this is a well-formed ARCV v1 checkpoint; anything else is why it
 * is not, phrased as the reason a refusal can be read.
 *
 * A checkpoint is **two** independently checksummed records, not one. `EncodeVariableBank` writes the
 * ARCV bank and its own CRC32; `EncodeSceneCommands` then appends an ARCL scene trailer carrying a
 * **second** CRC32. So the file's last four bytes are the trailer's CRC, not the bank's, and a guard
 * that reads one whole-file CRC minus four bytes refuses every slot a patched engine saves. Each record
 * is verified over its own span instead, and neither stands in for the other: a bad bank CRC is a
 * refusal before the trailer is even looked at.
 */
const SLOT_HEADER = 16;
const SLOT_COUNT_LIMIT = 65536;
const SLOT_KEY_LIMIT = 1024 * 1024;
const SLOT_VALUE_LIMIT = 16 * 1024 * 1024;
/* The ARCL v1 bounds `DecodeSceneCommands` states: five replayable layer tags, 1024 commands, 64
 * attributes per command, and a 64 KiB ceiling on every length-prefixed string. */
const SCENE_COUNT_LIMIT = 1024;
const SCENE_ATTR_LIMIT = 64;
const SCENE_STRING_LIMIT = 64 * 1024;
const SCENE_TAGS = new Set(['lyc', 'lyprop', 'lyevent', 'lytween', 'anime']);

/** ARCV v1 from `DecodeVariableBank`, optionally followed by the ARCL v1 trailer. `''` is well-formed. */
export function slotPayloadError(bytes: Uint8Array): string {
  if (bytes.byteLength < SLOT_HEADER) return 'it is too short to be an ARCV checkpoint';
  if (bytes.byteLength > COMPAT_SAVE_MAX_FILE_BYTES) return 'it is past the 64 MiB checkpoint bound';
  if (bytes[0] !== 0x41 || bytes[1] !== 0x52 || bytes[2] !== 0x43 || bytes[3] !== 0x56) {
    return 'it does not begin with the ARCV magic';
  }
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  if (view.getUint32(4, true) !== 1) return 'it is not ARCV version 1';
  const count = view.getUint32(8, true);
  if (count > SLOT_COUNT_LIMIT) return 'its bank declares more than 65536 entries';
  const keys = new Set<string>();
  let position = 12;
  for (let entry = 0; entry < count; entry += 1) {
    if (bytes.byteLength - position < 4) return 'a bank length runs past the end of the checkpoint';
    const keyLength = view.getUint32(position, true);
    position += 4;
    if (keyLength > SLOT_KEY_LIMIT || keyLength > bytes.byteLength - position) return 'a bank entry runs past the end of the checkpoint';
    let key = '';
    for (let index = 0; index < keyLength; index += 1) key += String.fromCharCode(bytes[position + index] ?? 0);
    if (keys.has(key)) return 'the bank repeats a key';
    keys.add(key);
    position += keyLength;
    if (bytes.byteLength - position < 4) return 'a bank length runs past the end of the checkpoint';
    const valueLength = view.getUint32(position, true);
    position += 4;
    if (valueLength > SLOT_VALUE_LIMIT || valueLength > bytes.byteLength - position) return 'a bank entry runs past the end of the checkpoint';
    position += valueLength;
  }
  /* The bank ends where its own structure says it does, which is the only place a trailer can start.
   * A scan for the four magic bytes would match one spelled inside a bank value; a real prefix cannot. */
  if (bytes.byteLength - position < 4) return 'a bank length runs past the end of the checkpoint';
  if (view.getUint32(position, true) !== crc32(bytes, position)) return 'its CRC32 does not match its bytes';
  const trailer = position + 4;
  if (trailer === bytes.byteLength) return '';
  return sceneTrailerError(bytes, view, trailer);
}

/**
 * The optional ARCL v1 trailer, read from the offset the bank's own CRC ended at.
 *
 * The same walk as `DecodeSceneCommands`, allocation-free and bounded by the trailer's own end. `at` is
 * never a guess: only a bank that ended where its structure said it does reaches this function, so the
 * magic is required exactly there and trailing bytes that are not a trailer are a refusal rather than
 * something to ignore.
 */
function sceneTrailerError(bytes: Uint8Array, view: DataView, at: number): string {
  const end = bytes.byteLength - 4;
  if (bytes.byteLength - at < 16) return 'its ARCL trailer is too short to be a scene checkpoint';
  if (bytes[at] !== 0x41 || bytes[at + 1] !== 0x52 || bytes[at + 2] !== 0x43 || bytes[at + 3] !== 0x4c) {
    return 'it carries bytes after the ARCV bank that are not an ARCL trailer';
  }
  if (view.getUint32(at + 4, true) !== 1) return 'its ARCL trailer is not version 1';
  const count = view.getUint32(at + 8, true);
  if (count > SCENE_COUNT_LIMIT) return 'its ARCL trailer declares more than 1024 commands';
  let position = at + 12;
  const text = (): string | null => {
    if (end - position < 4) return null;
    const length = view.getUint32(position, true);
    position += 4;
    if (length > SCENE_STRING_LIMIT || length > end - position) return null;
    let value = '';
    for (let index = 0; index < length; index += 1) value += String.fromCharCode(bytes[position + index] ?? 0);
    position += length;
    return value;
  };
  for (let command = 0; command < count; command += 1) {
    const name = text();
    if (name === null) return 'an ARCL string runs past the end of the trailer';
    if (!SCENE_TAGS.has(name)) return `its ARCL trailer carries the unknown tag ${name}`;
    if (end - position < 4) return 'an ARCL length runs past the end of the trailer';
    const attrs = view.getUint32(position, true);
    position += 4;
    if (attrs > SCENE_ATTR_LIMIT) return 'an ARCL command declares more than 64 attributes';
    const seen = new Set<string>();
    for (let attr = 0; attr < attrs; attr += 1) {
      const key = text();
      const value = text();
      if (key === null || value === null) return 'an ARCL string runs past the end of the trailer';
      if (seen.has(key)) return 'an ARCL command repeats an attribute';
      seen.add(key);
    }
  }
  if (position !== end) return 'its ARCL trailer does not end where the checkpoint says it does';
  if (view.getUint32(end, true) !== crc32(bytes.subarray(at), end - at)) return 'its ARCL trailer CRC32 does not match its bytes';
  return '';
}

/**
 * CRC32, the polynomial zlib gives `save_storage.cpp` and the one the worker computes over the same
 * bytes. Between the two copies this is the only reason a stored slot cannot be edited without being
 * noticed.
 */
export function crc32(bytes: Uint8Array, end: number): number {
  let crc = 0xffffffff;
  for (let index = 0; index < end; index += 1) {
    crc ^= bytes[index] ?? 0;
    for (let bit = 0; bit < 8; bit += 1) crc = (crc >>> 1) ^ (0xedb88320 & -(crc & 1));
  }
  return (crc ^ 0xffffffff) >>> 0;
}

/**
 * Refuses a path that is not strictly relative to the save directory.
 *
 * The store is keyed by path, so an absolute path, a drive letter, a `..` segment or a backslash is not
 * a cosmetic problem — it is a write outside the slot. `SavePath` on the native side makes the same
 * refusal; this is the page's own copy of it, applied to what comes back from the worker.
 */
export function assertRelativePath(path: string): void {
  if (path === '') throw new Error('a checkpoint file has an empty path');
  if (path.includes('\0')) throw new Error(`checkpoint path ${path} contains a NUL`);
  if (path.startsWith('/') || path.startsWith('\\')) throw new Error(`checkpoint path ${path} is absolute`);
  if (/^[A-Za-z]:/.test(path)) throw new Error(`checkpoint path ${path} carries a drive letter`);
  if (path.includes('\\')) throw new Error(`checkpoint path ${path} contains a backslash`);
  if (path.includes(':')) throw new Error(`checkpoint path ${path} contains a colon`);
  if (path.includes('//')) throw new Error(`checkpoint path ${path} has a doubled separator`);
  if (/\.pfs(\.\d{1,9})?$/i.test(path)) throw new Error(`checkpoint path ${path} is a pack`);
  for (const segment of path.split('/')) {
    if (segment === '' || segment === '.' || segment === '..') {
      throw new Error(`checkpoint path ${path} is not a plain relative path`);
    }
  }
}

/**
 * Base64 to bytes, without `atob`.
 *
 * The engine's payload is binary and `atob` is a DOM global — present in the page, absent in Node, and
 * the same code has to run in both. A strict table also means a base64 string with a stray character is
 * refused here instead of decoding to different bytes than were captured.
 */
export function base64ToBytes(text: string): Uint8Array {
  const alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/';
  if (text.length % 4 !== 0) throw new Error('a checkpoint file is not valid base64');
  const clean = text.replace(/=+$/, '');
  if (text.length - clean.length > 2 || clean.length % 4 === 1) throw new Error('a checkpoint file is not valid base64');
  const size = Math.floor((clean.length * 3) / 4);
  if (size > COMPAT_SAVE_MAX_FILE_BYTES) throw new Error('a checkpoint file is past the 64 MiB save bound');
  const out = new Uint8Array(size);
  let bits = 0;
  let value = 0;
  let index = 0;
  for (const character of clean) {
    const digit = alphabet.indexOf(character);
    if (digit < 0) throw new Error('a checkpoint file is not valid base64');
    value = (value << 6) | digit;
    bits += 6;
    if (bits >= 8) {
      bits -= 8;
      out[index] = (value >> bits) & 0xff;
      index += 1;
    }
  }
  if (bits > 0 && (value & ((1 << bits) - 1)) !== 0) throw new Error('a checkpoint file is not valid base64');
  return index === size ? out : out.subarray(0, index);
}