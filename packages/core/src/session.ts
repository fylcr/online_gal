import {
  RuntimeRegistry,
  type GameSource,
  type InputEvent,
  type LogRecord,
  type RuntimeHost,
  type RuntimeHostEvent,
  type RuntimeInstance,
  type WritableStore,
} from '@online-gal/engine-api';
import {
  attachInput,
  createRuntimeHost,
  GameLoop,
  InputGestureQueue,
  Presenter,
} from '@online-gal/player-host';
import type { AppSettings, GameRecord, SaveSlotRecord } from './types.js';

export interface SessionOptions {
  readonly registry: RuntimeRegistry;
  readonly canvas: HTMLCanvasElement;
  readonly record: GameRecord;
  readonly source: GameSource;
  readonly store: WritableStore;
  readonly settings: AppSettings;
  /** Runtime chosen by detection or forced by the per-game config. */
  readonly runtimeId: string;
  readonly onLog?: (record: LogRecord) => void;
  readonly onEvent?: (event: RuntimeHostEvent) => void;
  readonly onEnded?: (reason: string) => void;
}

/**
 * One running game session.
 *
 * The class is the host side of the runtime contract: it owns the canvas, the
 * frame loop, input plumbing and the save file, while the runtime owns
 * everything the game itself does. Runtimes never schedule their own frames —
 * they ask for one through `RuntimeHost.requestFrame`, and this class decides
 * when the next tick happens.
 */
export class Session {
  private readonly presenter: Presenter;
  private readonly runtime: RuntimeInstance;
  private readonly loop: GameLoop;
  private readonly detachInput: () => void;
  /**
   * Holds complete gestures that arrive while a transition is running.
   *
   * Based on AetherKiri's input queue gate: a click that starts an expensive KAG
   * transition must not let further clicks pile up unbounded, and must not lose
   * them either. See `InputGestureQueue` for the pairing rules.
   */
  private readonly inputGate: InputGestureQueue;
  private readonly logs: LogRecord[] = [];
  private startedAt = 0;
  private accumulatedMs = 0;
  private paused = false;
  private ended = false;
  private endReason: string | null = null;
  private tickCount = 0;
  private lastFpsSample = 0;
  private fps = 0;

  constructor(private readonly options: SessionOptions) {
    this.presenter = new Presenter(options.canvas, {
      settings: {
        textSpeed: options.settings.textSpeed,
        muted: options.settings.mute,
        'volume.bgm': options.settings.volumes.bgm,
        'volume.se': options.settings.volumes.se,
        'volume.voice': options.settings.volumes.voice,
        surface: options.settings.surface,
      },
      onLog: (record) => {
        this.logs.push(record);
        if (this.logs.length > 400) this.logs.shift();
        options.onLog?.(record);
      },
      onEvent: (event) => this.handleEvent(event),
    });

    if (options.record.config.surface) {
      this.presenter.setSurfaceSize(
        options.record.config.surface.width,
        options.record.config.surface.height,
      );
    }

    const host: RuntimeHost = createRuntimeHost({
      presenter: this.presenter,
      source: options.source,
      store: options.store,
      canvas: options.canvas,
      settings: {
        textSpeed: options.settings.textSpeed,
        surface: options.settings.surface,
        mute: options.settings.mute,
        ...options.record.config.options,
      },
      onEvent: options.onEvent,
      runtimeId: options.runtimeId,
    });

    this.runtime = options.registry.create(options.runtimeId, host, {
      runtimeId: options.runtimeId,
      surface: options.record.config.surface ?? options.settings.surface,
      gameConfig: options.record.config as unknown as Record<string, unknown>,
      mode: 'new',
    });

    this.detachInput = attachInput(options.canvas, (event) => this.dispatchInput(event));

    this.inputGate = new InputGestureQueue({
      isBusy: () => this.isEngineBusy(),
      onOverflow: (dropped) =>
        this.logs.length < 400 &&
        options.onLog?.({
          level: 'debug',
          runtimeId: options.runtimeId,
          message: `input queue full; dropped ${dropped} gesture(s) during a transition`,
          at: Date.now(),
        }),
    });

    this.loop = new GameLoop({
      onTick: (deltaMs) => {
        this.runtime.tick(deltaMs);
        this.flushQueuedInput();
      },
      onRender: () => this.runtime.render(),
      maxDeltaMs: 100,
    });
  }

  /**
   * True while the engine is mid-transition and should not be interrupted.
   *
   * Read from the runtime's own diagnostics rather than pushed by it, because
   * only some runtimes report a wait: when nothing is reported the session stays
   * in the low-latency pass-through path, which is the correct default.
   */
  private isEngineBusy(): boolean {
    const state = this.runtime.describe?.() as
      | { scene?: { waitingOn?: string | null; selecting?: boolean } }
      | undefined;
    const waitingOn = state?.scene?.waitingOn;
    if (!waitingOn) return false;
    // Waits that exist *because* the player should act must never hold input
    // back, or the game would deadlock waiting for a click that sits in a queue.
    return !['click', 'input', 'select', 'history'].includes(waitingOn);
  }

  /** Replays input the gate held back while the engine was busy. */
  private flushQueuedInput(): void {
    if (this.ended || this.inputGate.size === 0) return;
    if (this.isEngineBusy()) return;
    for (const event of this.inputGate.drain()) {
      if (this.ended) break;
      this.runtime.sendInput(event as InputEvent);
    }
  }

  /** How many complete gestures are waiting to be replayed. */
  get queuedInputCount(): number {
    return this.inputGate.size;
  }

  get runtimeId(): string {
    return this.options.runtimeId;
  }

  get gameRecord(): GameRecord {
    return this.options.record;
  }

  get isPaused(): boolean {
    return this.paused;
  }

  get isEnded(): boolean {
    return this.ended;
  }

  get elapsedMs(): number {
    return this.accumulatedMs;
  }

  get frameRate(): number {
    return this.fps;
  }

  get recentLogs(): readonly LogRecord[] {
    return this.logs;
  }

  /** Loads the game and starts the frame loop. */
  async start(): Promise<void> {
    this.startedAt = performance.now();
    await this.runtime.load();
    this.loop.start();
    // A first render before the loop's initial frame avoids a flash of black
    // when a runtime's load() already produced a frame.
    this.runtime.render();
  }

  /**
   * Applies a settings change to a live session.
   *
   * Volumes and text speed are read through `RuntimeHost.setting`, so pushing
   * them into the presenter is enough — no runtime restart needed.
   */
  applySettings(settings: AppSettings): void {
    this.presenter.setSetting('textSpeed', settings.textSpeed);
    this.presenter.setSetting('muted', settings.mute);
    this.presenter.setSetting('volume.bgm', settings.volumes.bgm);
    this.presenter.setSetting('volume.se', settings.volumes.se);
    this.presenter.setSetting('volume.voice', settings.volumes.voice);
    if (!this.options.record.config.surface) {
      this.presenter.setSurfaceSize(settings.surface.width, settings.surface.height);
    }
  }

  pause(): void {
    if (this.paused || this.ended) return;
    this.paused = true;
    this.runtime.pause();
    this.loop.stop();
  }

  resume(): void {
    if (!this.paused || this.ended) return;
    this.paused = false;
    this.runtime.resume();
    this.loop.start();
  }

  /** Stops the session and releases every resource it holds. */
  async stop(): Promise<void> {
    if (this.ended) return;
    this.ended = true;
    this.loop.dispose();
    this.inputGate.reset();
    this.detachInput();
    this.runtime.dispose();
    this.accumulatedMs += performance.now() - this.startedAt;
  }

  private dispatchInput(event: InputEvent): void {
    if (this.ended) return;

    // A press that lands before the user has interacted would be blocked by the
    // browser's autoplay policy anyway; treating any input as the unlocking
    // gesture is what makes audio start on the first click.
    if (this.inputGate.shouldQueue(event)) return;

    this.runtime.sendInput(event);
    if (event.type === 'pointer-down' || event.type === 'key-down') {
      this.presenter.requestFrame();
    }
  }

  private handleEvent(event: RuntimeHostEvent): void {
    if (event.type === 'session-end') {
      this.endReason = event.reason ?? 'ended';
      this.options.onEnded?.(this.endReason);
    }
    if (event.type === 'title') {
      this.options.onEvent?.(event);
    }
  }

  /** Serialises the session, including a thumbnail of the current frame. */
  save(slot: number, caption?: string): SaveSlotRecord {
    const state = this.runtime.save();
    const thumbnail = this.captureThumbnail();
    return {
      gameId: this.options.record.id,
      slot,
      createdAt: Date.now(),
      caption: caption ?? state.caption ?? `存档 ${slot}`,
      state,
      ...(thumbnail ? { thumbnail } : {}),
      playTimeMs: this.accumulatedMs + (performance.now() - this.startedAt),
    };
  }

  async restore(record: SaveSlotRecord): Promise<void> {
    await this.runtime.restore(record.state as Parameters<RuntimeInstance['restore']>[0]);
    this.presenter.requestFrame();
  }

  /** Captures the canvas as a small PNG data URL for the save list. */
  captureThumbnail(width = 240): string | null {
    const canvas = this.options.canvas;
    if (!canvas.width || !canvas.height) return null;
    const height = Math.max(1, Math.round((canvas.height / canvas.width) * width));
    const scratch = document.createElement('canvas');
    scratch.width = width;
    scratch.height = height;
    const context = scratch.getContext('2d');
    if (!context) return null;
    try {
      context.drawImage(canvas, 0, 0, width, height);
      return scratch.toDataURL('image/png');
    } catch {
      // A tainted canvas (an embedded cross-origin frame) cannot be captured.
      return null;
    }
  }

  /**
   * Per-command compatibility detail for the current session.
   *
   * The runtime's `describe()` carries only the summary counts, because it is
   * polled every second; the full list is fetched on demand when the player
   * opens the compatibility panel.
   */
  compatibilityDetail(): readonly {
    command: string;
    count: number;
    handling: 'skipped' | 'approximated';
    line?: number;
    note?: string;
  }[] {
    const report = this.runtime.report?.();
    if (!report) return [];
    return [...report.unsupported, ...report.approximated].sort(
      (a, b) => b.count - a.count,
    );
  }

  /** Diagnostics for the engine panel. */
  describe(): Readonly<Record<string, unknown>> {
    return {
      runtimeId: this.options.runtimeId,
      game: this.options.record.name,
      elapsedMs: Math.round(this.accumulatedMs + (performance.now() - this.startedAt)),
      frames: this.tickCount,
      fps: this.fps,
      paused: this.paused,
      ended: this.ended,
      endReason: this.endReason,
      surface: `${this.presenter.surfaceSize.width}x${this.presenter.surfaceSize.height}`,
      runtime: this.runtime.describe?.() ?? null,
    };
  }

  /** Called by the launcher's own diagnostics tick. */
  sampleFps(now: number): void {
    this.tickCount += 1;
    if (now - this.lastFpsSample < 500) return;
    this.fps = Math.round((this.tickCount * 1000) / (now - this.lastFpsSample));
    this.tickCount = 0;
    this.lastFpsSample = now;
  }
}

export interface LaunchOptions {
  readonly registry: RuntimeRegistry;
  readonly canvas: HTMLCanvasElement;
  readonly record: GameRecord;
  readonly source: GameSource;
  readonly store: WritableStore;
  readonly settings: AppSettings;
}

/**
 * Chooses the runtime for a game and starts a session.
 *
 * The selection order mirrors the probe dispatcher: an explicit per-game
 * override wins, then the detection result, then the first provider that
 * recognises the directory at launch time (which catches games imported before
 * their engine's runtime was registered).
 */
export async function launchSession(options: LaunchOptions): Promise<Session> {
  const runtimeId = await resolveRuntime(options);
  const session = new Session({ ...options, runtimeId });
  await session.start();
  return session;
}

async function resolveRuntime(options: LaunchOptions): Promise<string> {
  const forced = options.record.config.forcedRuntimeId;
  if (forced) {
    if (!options.registry.get(forced)) {
      throw new Error(
        `the configured runtime "${forced}" is not available in this build; ` +
          `available: ${options.registry.list().map((provider) => provider.runtimeId).join(', ')}`,
      );
    }
    return forced;
  }

  const detected = options.record.detection.runtimeId;
  if (detected && options.registry.get(detected)) return detected;

  const outcome = await options.registry.discover(options.source);
  if (outcome.selected && options.registry.get(outcome.selected.runtimeId)) {
    return outcome.selected.runtimeId;
  }

  throw new Error(
    'no runtime on this platform recognises this game. ' +
      'Supported engines: ' +
      options.registry
        .list()
        .map((provider) => `${provider.displayName} (${provider.runtimeId})`)
        .join(', '),
  );
}
