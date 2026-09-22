/**
 * KiriKiri's WebAssembly engine, hosted in a framed build.
 *
 * The real engine is compiled to WebAssembly and ships as a self-contained HTML
 * page. Rather than reimplementing it — the lesson of the in-tree TJS2 interpreter
 * in `runtime.ts` — this runtime mounts that page in a same-origin iframe and
 * feeds the game's bytes into it through the host byte-source seam from
 * `wasm-page.ts`. The iframe is the presentation surface: the host canvas is left
 * black, exactly as `WebRuntime` treats its embedded games, and input flows to
 * the engine through the DOM rather than being forwarded from the host.
 */
import type {
  CompatibilityReport,
  InputEvent,
  RuntimeHost,
  RuntimeInstance,
  RuntimeOptions,
  SaveState,
} from '@online-gal/engine-api';
import { createKiriPage, type KiriPageHandle } from './wasm-page.js';
import { findKiriMount, kirikiriWasmPageUrl } from './wasm-config.js';

export interface KiriWasmCreateOptions extends RuntimeOptions {
  /** URL of the built KiriKiri page (its index.html). */
  readonly pageUrl?: string;
  /** Virtual key the shell mounts, e.g. `game.zip` or `data.xp3`. */
  readonly gameKey?: string;
  /** Which shell parameter carries the key. Default `'game'`. */
  readonly param?: 'xp3' | 'game';
  /** Forwarded to the host byte source's URL→key mapping. */
  readonly resolveUrl?: (url: string) => string;
}

/**
 * Runs the KiriKiri WebAssembly build inside a framed page.
 *
 * The host does not composite frames or drive the loop: the engine owns both
 * inside the iframe, so `tick`/`render`/`sendInput` are no-ops and `render()`
 * only backs the canvas in black before the build paints.
 */
export class KiriWasmRuntime implements RuntimeInstance {
  readonly runtimeId = 'krkr2-wasm';

  private handle: KiriPageHandle | null = null;
  private loaded = false;
  private disposed = false;
  private lastError: string | null = null;
  private readonly notes: string[] = [];
  private pageUrl: string | null = null;
  private gameKey: string | null = null;

  constructor(
    private readonly host: RuntimeHost,
    private readonly options: KiriWasmCreateOptions,
  ) {}

  async load(): Promise<void> {
    // Options arrive three ways, in this order of precedence: the launcher passes
    // per-game config through `gameConfig` (a library record's free-form JSON); a
    // direct caller passes the typed fields; and the deployment publishes the build
    // URL once through `configureKirikiriWasm`. The mount key is different — it is a
    // property of the game, so when nobody names one it is derived from the source.
    const gameConfig = this.options.gameConfig;
    const pageUrl =
      (gameConfig?.pageUrl as string | undefined) ??
      this.options.pageUrl ??
      kirikiriWasmPageUrl() ??
      undefined;

    const namedKey = (gameConfig?.gameKey as string | undefined) ?? this.options.gameKey;
    const mount = namedKey === undefined ? await findKiriMount(this.host.source) : null;
    const gameKey = namedKey ?? mount?.key;
    const param =
      (gameConfig?.param as 'xp3' | 'game' | undefined) ??
      this.options.param ??
      mount?.param ??
      'game';
    const resolveUrl =
      (gameConfig?.resolveUrl as ((url: string) => string) | undefined) ??
      this.options.resolveUrl;

    // Validate before touching the DOM: a real page URL and a virtual mount key
    // are non-negotiable, and failing here keeps the error on the load path where
    // the launcher expects it rather than mid-frame-creation.
    if (!pageUrl) {
      throw new Error(
        'the KiriKiri WASM runtime needs "pageUrl" (the built page\'s index.html URL); ' +
          'publish it with configureKirikiriWasm({ pageUrl }), or pass gameConfig.pageUrl ' +
          'or the "pageUrl" option',
      );
    }
    if (!gameKey) {
      throw new Error(
        'the KiriKiri WASM runtime needs "gameKey" (the archive the shell mounts); ' +
          'pass gameConfig.gameKey or the "gameKey" option, or import the game as a ' +
          'single XP3, or as a ZIP that is the whole source',
      );
    }

    this.pageUrl = pageUrl;
    this.gameKey = gameKey;

    // The iframe is pinned over the host canvas, so it is positioned relative to
    // the canvas's offset parent. Mirror the WebRuntime container handling: fall
    // back to the owner document's body and establish a positioning context when
    // the parent is static (otherwise `inset: 0` would resolve to the initial
    // containing block and drift off the canvas).
    const canvas = this.host.canvas;
    const ownerDocument = canvas.ownerDocument ?? document;
    const container = canvas.parentElement ?? ownerDocument.body;
    // Resolve the view through the container's own document instead of the ambient
    // global: a bare `getComputedStyle` only exists on a browser window, so using it
    // here would make this runtime impossible to exercise under jsdom, and would
    // break for any document that is not the current global one.
    const view = ownerDocument.defaultView;
    const position = view ? view.getComputedStyle(container).position : 'static';
    if (position === 'static') container.style.position = 'relative';

    // `createKiriPage` appends the frame and sets its source, so the position fix
    // must run first; the styling below only affects layout and is applied after.
    const handle = createKiriPage({
      document: ownerDocument,
      pageUrl,
      source: this.host.source,
      gameKey,
      param,
      resolveUrl,
      container,
    });
    this.handle = handle;

    // The same framing the WebRuntime applies: the iframe is the visible surface,
    // so it is pinned over the canvas with a black backdrop that hides the
    // pre-paint flash.
    handle.frame.title = 'game';
    handle.frame.setAttribute(
      'sandbox',
      'allow-scripts allow-same-origin allow-pointer-lock allow-popups',
    );
    handle.frame.style.cssText =
      'position:absolute;inset:0;width:100%;height:100%;border:0;background:#000;';

    // `ready()` resolves on the first seam install (after the frame's `load`),
    // so a build that never wires the seam fails fast instead of hanging the host.
    // The timeout is wrapped with the page URL so the failure is diagnosable.
    try {
      await handle.ready();
      this.loaded = true;
      this.host.log('info', `KiriKiri WASM build loaded (${pageUrl})`);
    } catch (error) {
      const message =
        `the KiriKiri WASM build did not mount the host byte source from "${pageUrl}" ` +
        `within the ready timeout: ${String(error)}`;
      this.lastError = message;
      this.notes.push(message);
      throw new Error(message);
    }
  }

  tick(): void {
    // The embedded engine owns its loop and timing; a second clock stepping it
    // would fight the build's own schedule, so this is intentionally inert.
  }

  render(): void {
    const context = this.host.canvas.getContext('2d');
    if (!context) return;
    // The iframe is the visible surface; paint a neutral backdrop behind it before
    // the build draws its first frame, matching the WebRuntime treatment.
    if (!this.loaded) {
      context.fillStyle = '#000000';
      context.fillRect(0, 0, this.host.canvas.width, this.host.canvas.height);
    }
  }

  sendInput(_event: InputEvent): void {
    // Input goes to the iframe through the DOM, so nothing to forward.
  }

  pause(): void {
    // The embedded shell exposes no control channel from the outside, so there is
    // nothing to suspend: audio is not suspended. Pretending to control a surface
    // that is not wired would only mislead both the host and the engine.
  }

  resume(): void {
    // See `pause()`: no external control channel exists to drive a resume.
  }

  save(): SaveState {
    // The engine runs inside the frame and owns persistence through its own
    // storage layer — the seam feeds it archives, not saves. Mirroring engine state
    // here would duplicate it and risk divergence, so this records identity rather
    // than serialising engine memory. This is the same reasoning
    // `OnsWasmRuntime.save()` uses for the ONScripter engine.
    return {
      version: 1,
      data: {
        runtime: 'krkr2-wasm',
        page: this.pageUrl,
        key: this.gameKey,
        persistence: 'engine',
      },
      caption: 'KiriKiri engine session',
    };
  }

  async restore(): Promise<void> {
    // No-op: saves are owned by the engine inside the frame, so there is nothing
    // for the host to inject — the same reasoning as `save()` above.
  }

  dispose(): void {
    if (this.disposed) return;
    this.disposed = true;
    this.handle?.dispose();
    this.handle = null;
  }

  describe(): Readonly<Record<string, unknown>> {
    return {
      engine: 'KiriKiri (WebAssembly engine)',
      pageUrl: this.pageUrl,
      gameKey: this.gameKey,
      loaded: this.loaded,
      error: this.lastError,
      notes: this.notes,
    };
  }

  /**
   * Compatibility report for a real WebAssembly build.
   *
   * A faithful engine build has no command table to audit: every KAG tag that
   * reached the seam ran as compiled, so there are no "unsupported" entries to
   * report. The only honest signal is whether the host observed a failure it
   * could not forward to the launcher — carried in `notes`.
   */
  report(): CompatibilityReport {
    return {
      runtimeId: this.runtimeId,
      severity: this.notes.some((note) => /not supported|failed/i.test(note))
        ? 'degraded'
        : 'clean',
      unsupported: [],
      approximated: [],
      supportedCommandCount: 0,
    };
  }
}
