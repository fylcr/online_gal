/**
 * Type declarations for the ported harness modules.
 *
 * These are the verified worker host and its audio relay, copied verbatim from the test
 * harness. They are plain JavaScript rather than TypeScript for a reason: the worker is
 * loaded by URL into a `Worker`, and the audio relay is imported by the host, so neither
 * benefits from being compiled — and rewriting them as TypeScript would mean re-verifying
 * behaviour that already works.
 *
 * Only the surface the runtime adapter uses is declared.
 */

/** Options accepted by `startOnsHost`. */
export interface OnsHostOptions {
  /** Canvas whose control is transferred to the worker. */
  canvas: HTMLCanvasElement;
  /** URL of the index describing the game's files. */
  indexUrl: string;
  /** URL of the generated Emscripten loader. */
  wasmJsUrl: string;
  /** Base URL the worker resolves engine and game resources against. */
  engineBase: string;
  display?: { width?: number; height?: number; devicePixelRatio?: number };
  onLog?: (text: string) => void;
  onError?: (text: string) => void;
  onWarn?: (text: string) => void;
  onExited?: (stats: { rangesServed: number; bytesServed: number }) => void;
  onFrame?: () => void;
}

/** Audio relay handle, as the host exposes it. */
export interface OnsAudioRelay {
  start(): Promise<boolean>;
  suspend?(): void;
  resume?(): void;
  stats(): Record<string, unknown>;
}

/** The worker host. */
export interface OnsHost {
  worker: Worker;
  audio: OnsAudioRelay;
  post(message: unknown): void;
  probe(): void;
  /**
   * Delivers an already-normalised input event. Coordinates are in engine screen space.
   */
  sendInput(type: string, payload?: Record<string, unknown>): void;
  audioStats(): Record<string, unknown>;
  /** The engine's resolution and the rectangle the drawing occupies. */
  viewport(): {
    engine: { width: number; height: number } | null;
    drawn: { left: number; top: number; width: number; height: number } | null;
  };
  scaleMode(): 'fill' | 'contain';
  setScaleMode(mode: 'fill' | 'contain'): 'fill' | 'contain';
  readMouseState(): Promise<{ x: number; y: number } | null>;
  probePointer(cols?: number, rows?: number): Promise<unknown>;
  lastProbe(): unknown;
  diagnostics(): { kinds: Record<string, number>; lastProbe: unknown };
  terminate?(): void;
}

/** Starts the worker host for a canvas. */
export function startOnsHost(options: OnsHostOptions): OnsHost;
