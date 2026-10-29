/**
 * The WASM engine host, adapted to the engine-api `RuntimeInstance` contract.
 *
 * The runtime itself is not reimplemented here. `startOnsHost` already runs the real
 * OnscripterYuri engine compiled to WebAssembly in a worker, with byte-granular asset
 * loading, audio relayed to the main thread, and input injected through a source-level shim.
 * That is verified working, so this file only translates between its interface and the one
 * the launcher expects — the "wrap, do not rewrite" rule that a previous attempt to
 * reimplement NScripter in TypeScript got wrong.
 *
 * The translation is small because the two models agree on the important points: both are
 * canvas-based, both take normalised coordinates in the engine's own space, and both are
 * driven externally by load/tick/input rather than owning a loop.
 */
import type {
  CompatibilityReport,
  InputEvent,
  RuntimeHost,
  RuntimeInstance,
  RuntimeOptions,
  SaveState,
} from '@online-gal/engine-api';
import { startOnsHost } from './wasm/host.mjs';

/**
 * The engine's own screen resolution.
 *
 * Set from the engine's banner (`Display: 793 x 446`) once it starts. The host reports it
 * through `viewport()`, and input coordinates are expressed in this space — the host's
 * `drawnRect()` maps them onto whatever the canvas currently measures.
 */
const DEFAULT_WIDTH = 793;
const DEFAULT_HEIGHT = 446;

/**
 * Extra options this runtime understands, beyond the common ones.
 *
 * `RuntimeOptions` has no index signature, so an `extends` would be rejected; the two fields
 * are declared alongside it instead.
 */
export interface OnsWasmCreateOptions extends RuntimeOptions {
  /** Base URL for `onsyuri.js` / `onsyuri.wasm`. Defaults to the package's `assets/`. */
  readonly engineBase?: string;
  /** URL of the index describing the game's files. */
  readonly indexUrl?: string;
}

/** The game's file index, as the worker expects it. */
const DEFAULT_INDEX = './onsyuri_index.json';

/**
 * Runs an ONScripter game on the WebAssembly engine.
 *
 * `load()` starts the worker; `tick()` is accepted and ignored, because the engine owns its
 * own loop inside the worker — it yields to the browser through `emscripten_sleep`, which is
 * why the harness needed no animation-frame driver. `render()` is likewise a no-op: the
 * engine draws straight into the transferred `OffscreenCanvas`, so there is no frame for the
 * host to blit.
 */
export class OnsWasmRuntime implements RuntimeInstance {
  readonly runtimeId = 'onscripter-wasm';

  private host: ReturnType<typeof startOnsHost> | null = null;
  private started = false;
  private disposed = false;
  private readonly notes: string[] = [];

  constructor(
    private readonly runtimeHost: RuntimeHost,
    private readonly options: OnsWasmCreateOptions,
  ) {}

  async load(): Promise<void> {
    if (this.started) return;
    const canvas = this.runtimeHost.canvas as HTMLCanvasElement | undefined;
    if (!canvas) {
      throw new Error('the ONScripter WASM runtime needs a canvas from the host');
    }

    // The engine's own files. `engineBase` defaults to the package's own `assets/`, resolved from
    // this module's URL; the index describes the *game*, so it comes from the host.
    //
    // The depth is one `../`, not two. This file compiles to `dist/wasm-runtime.js`, whose parent
    // is the package root, so `../assets/` is `<package>/assets/`. The earlier `../../assets/`
    // pointed one level *above* the package -- `packages/assets/` in the repository, and
    // `node_modules/@online-gal/assets/` once installed -- so every consumer that did not pass
    // `engineBase` explicitly got a 404 for `onsyuri.js`. `.test/runtime-ons-engine-base.test.ts`
    // pins this arithmetic against the built file rather than against this source line.
    //
    // A bundler that rewrites `import.meta.url` still has to set `engineBase` explicitly, which is
    // why it remains an option and a game-config key.
    const engineBase =
      this.options.engineBase ?? (this.fromGameConfig('engineBase') as string | undefined) ?? new URL('../assets/', import.meta.url).href;

    // The game's file index. Read from `gameConfig` first, because that is how the launcher
    // supplies it — `RuntimeSession` forwards the library record's config into `create()` —
    // with the explicit option and then a conventional path as fallbacks.
    const indexUrl =
      (this.fromGameConfig('indexUrl') as string | undefined) ?? this.options.indexUrl ?? DEFAULT_INDEX;
    if (!indexUrl) {
      throw new Error('the ONScripter WASM runtime needs `indexUrl` for the game files');
    }

    this.host = startOnsHost({
      canvas,
      indexUrl,
      wasmJsUrl: new URL('onsyuri.js', engineBase).href,
      engineBase,
      onLog: (text) => this.absorb(text),
      onError: (text) => {
        this.notes.push(text);
        this.absorb(text);
      },
      onWarn: (text) => this.absorb(text),
    });

    await this.waitForEngine();
    this.started = true;
  }

  /**
   * Reads one key from the per-game config.
   *
   * `RuntimeOptions.gameConfig` is the channel the launcher actually uses to pass per-game
   * settings, so the runtime accepts its options there as well as directly. Declared as
   * `unknown` because the shape is the library record's free-form JSON.
   */
  private fromGameConfig(key: string): unknown {
    return this.options.gameConfig?.[key];
  }

  /**
   * Waits for the engine to report its resolution.
   *
   * That message is the first proof the engine is up and drawing, so it is a better signal
   * than a fixed delay. The timeout exists so a game that fails to start surfaces as an error
   * instead of hanging the launcher.
   */
  private async waitForEngine(timeoutMs = 60_000): Promise<void> {
    const deadline = Date.now() + timeoutMs;
    while (Date.now() < deadline) {
      const viewport = this.host?.viewport();
      if (viewport?.engine) return;
      await new Promise((resolve) => setTimeout(resolve, 100));
    }
    throw new Error(
      `the ONScripter engine did not start within ${timeoutMs} ms` +
        (this.notes.length ? `; last note: ${this.notes[this.notes.length - 1]}` : ''),
    );
  }

  /**
   * Log lines from the engine, forwarded to the host's diagnostics.
   *
   * Only lines that are not the engine's own chatter are kept as notes; the audio counters in
   * particular repeat for the whole session, and retaining them would grow without bound.
   */
  private absorb(text: string): void {
    if (/^## worker: audio \d+ callbacks/.test(text)) return;
    if (this.notes.length < 200) this.notes.push(text);
  }

  /**
   * No-op: the engine advances itself inside the worker.
   *
   * Present because the contract requires it. A driver would be actively harmful here — the
   * engine's loop already yields through `emscripten_sleep`, and a second clock stepping it
   * would fight that.
   */
  tick(): void {}

  /** No-op: the engine draws directly into the transferred canvas. */
  render(): void {}

  /**
   * Forwards one normalised input event.
   *
   * Coordinates arrive in the engine's screen space and are passed through unchanged; the
   * host maps them onto the canvas. With the canvas sized to the engine's resolution — which
   * `index.html` does — that mapping is the identity, and it stays correct under either
   * scaling mode because the host computes the drawn area itself.
   */
  sendInput(event: InputEvent): void {
    if (!this.host || this.disposed) return;

    switch (event.type) {
      case 'pointer-down':
        this.host.sendInput('mousedown', { x: event.x, y: event.y, button: event.button });
        return;
      case 'pointer-up':
        this.host.sendInput('mouseup', { x: event.x, y: event.y, button: event.button });
        return;
      case 'pointer-move':
        this.host.sendInput('mousemove', { x: event.x, y: event.y });
        return;
      case 'pointer-wheel':
        // The shim has no wheel constructor, so this would fall through to the inert DOM
        // path. Recorded rather than silently dropped.
        this.absorb('wheel input is not supported by the ONScripter shim yet');
        return;
      case 'key-down':
        this.host.sendInput('keydown', { key: event.key, code: event.code });
        return;
      case 'key-up':
        this.host.sendInput('keyup', { key: event.key, code: event.code });
        return;
      case 'text':
        // ONScripter takes text through its own input boxes rather than raw codepoints.
        return;
      case 'blur':
        // The engine has no blur handling; its audio relay pauses with the page anyway.
        return;
      default:
        return;
    }
  }

  /**
   * Suspend and resume.
   *
   * The engine's own loop cannot be paused from outside, so what is suspended is the audio
   * relay — the part that keeps producing sound while the tab is in the background.
   */
  pause(): void {
    this.host?.audio?.suspend?.();
  }

  resume(): void {
    this.host?.audio?.resume?.();
  }

  /**
   * A save state that carries the engine's own save directory.
   *
   * ONScripter writes saves through its `--save-dir`, which the worker mounts on `IDBFS`.
   * Capturing that directory is the engine's own notion of a save, so this reports which
   * path it is rather than trying to invent a serialisation of the engine's memory.
   */
  save(): SaveState {
    return {
      version: 1,
      // The engine owns persistence: its `save` command writes into `--save-dir`, which
      // `IDBFS` syncs. Mirroring engine state here would duplicate it and risk divergence.
      data: { saveDirectory: '/onsyuri_save/onscript', persistence: 'idbfs' },
    };
  }

  /** No-op for the same reason `save()` reports the engine's own directory. */
  async restore(): Promise<void> {}

  dispose(): void {
    if (this.disposed) return;
    this.disposed = true;
    this.host?.terminate?.();
    this.host = null;
  }

  describe(): Readonly<Record<string, unknown>> {
    const viewport = this.host?.viewport();
    return {
      runtime: 'onscripter-wasm',
      engine: 'OnscripterYuri (WebAssembly, built from source)',
      started: this.started,
      screen: viewport?.engine ?? { width: DEFAULT_WIDTH, height: DEFAULT_HEIGHT },
      drawn: viewport?.drawn ?? null,
      scaleMode: this.host?.scaleMode?.() ?? 'fill',
      audio: this.host?.audioStats?.() ?? null,
      notes: this.notes.slice(-20),
    };
  }

  report(): CompatibilityReport {
    // The engine is a real build rather than an interpreter, so there is no command table to
    // audit. What can be reported honestly is that everything the engine ran, it ran; the
    // notes carry whatever the host could not forward (wheel input, for instance).
    return {
      runtimeId: this.runtimeId,
      severity: this.notes.some((note) => /not supported|failed/i.test(note)) ? 'degraded' : 'clean',
      unsupported: [],
      approximated: [],
      supportedCommandCount: 0,
    };
  }
}
