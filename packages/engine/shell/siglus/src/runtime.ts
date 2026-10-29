import {
  OPTION_COMPATIBILITY_REPORT,
  OPTION_MEMORY_PROFILE,
  OPTION_STRICT_MODE,
  UnsupportedEngineError,
  type CompatibilityReport,
  type InputEvent,
  type ProbeContext,
  type ProbeResult,
  type RuntimeHost,
  type RuntimeInstance,
  type RuntimeOptions,
  type RuntimeProvider,
  type SaveState,
} from '@online-gal/engine-api';
import { siglusProbeResult } from './probe.js';

/**
 * The host operations a SiglusEngine runtime must provide.
 *
 * This mirrors the C ABI in AetherKiri's `bridge/siglus_runtime/include/siglus_ffi.h`
 * (GPL-3.0-or-later, `AetherKiri/AetherKiri`), renamed to TypeScript conventions.
 * It is declared but not implemented: AetherKiri drives a Rust engine
 * (`siglus_rs`, MPL-2.0) that renders offscreen through wgpu and hands back
 * RGBA frames, and that engine has no browser build. Writing the interface down
 * does two useful things — it records exactly what a WebAssembly port would
 * have to expose, and it stops the runtime from pretending to be a stub that
 * "runs" games.
 *
 * The methods map one-to-one onto the FFI so that a future implementation is a
 * transcription rather than a redesign:
 *
 * | FFI                          | here                    |
 * | ---------------------------- | ----------------------- |
 * | `siglus_ak_create`           | `create`                |
 * | `siglus_ak_open`             | `open`                  |
 * | `siglus_ak_game_screen_size` | `gameScreenSize`        |
 * | `siglus_ak_resize`           | `resize`                |
 * | `siglus_ak_step`             | `step`                  |
 * | `siglus_ak_get_frame_desc`   | `frameDesc`             |
 * | `siglus_ak_read_frame_rgba`  | `readFrameRgba`         |
 * | `siglus_ak_mouse_*`/`touch`  | `sendInput`             |
 * | `siglus_ak_read_audio_f32`   | `readAudio`             |
 * | `siglus_ak_close`/`destroy`  | `close`                 |
 *
 * Notes carried across from the FFI's own contract:
 * - `step` is driven by the host with a millisecond delta; the engine never owns
 *   a clock, which is why `RuntimeInstance.tick(deltaMs)` maps onto it directly.
 * - `readFrameRgba` yields tightly packed top-down RGBA8 rows *after* a
 *   successful `step`; a stride is reported separately so a packed surface can
 *   be blitted row by row.
 * - `gameScreenSize` is the game-native size and presentation upscales it,
 *   rather than resizing the engine.
 * - Audio is interleaved stereo `f32` drained from an in-process FIFO and may be
 *   read from the audio callback thread without blocking — the Web Audio
 *   equivalent is filling an `AudioBuffer` from the same draining loop.
 */
export interface SiglusEngineHost {
  /** Creates an empty shell. `scaleFactor > 0` selects the logical pixel density. */
  create(scaleFactor: number): Promise<number>;
  /** Builds the renderer and boots the scene VM. One game per shell. */
  open(handle: number, gameRoot: string, width: number, height: number): Promise<void>;
  /** Game-native canvas size, used instead of resizing the engine. */
  gameScreenSize(handle: number): Promise<{ width: number; height: number } | null>;
  resize(handle: number, width: number, height: number): Promise<void>;
  /** Advances simulation and renders one frame. Resolves when the game asked to quit. */
  step(handle: number, deltaMs: number): Promise<{ exitRequested: boolean }>;
  frameDesc(handle: number): Promise<{ width: number; height: number; strideBytes: number }>;
  /** Tightly packed top-down RGBA8, exactly `width * height * 4` bytes. */
  readFrameRgba(handle: number): Promise<Uint8Array>;
  sendInput(handle: number, event: InputEvent): Promise<void>;
  /** Drains up to `capacity` interleaved stereo f32 samples; 0 when empty. */
  readAudio(handle: number, capacity: number): Promise<Float32Array>;
  close(handle: number): Promise<void>;
  /** Last error for the shell, for diagnostic reporting. */
  lastError(handle: number): string;
}

/**
 * Host implementations known to this build.
 *
 * Empty by design: the browser cannot execute a Siglus game today, and inventing
 * a placeholder would turn "recognised" into "appears to work". A WebAssembly
 * build of `siglus_rs` registered here is the single change needed to make the
 * runtime playable.
 */
export const siglusEngineHosts: SiglusEngineHost[] = [];

/** Registers a host implementation, returning a deregistration function. */
export function registerSiglusEngineHost(host: SiglusEngineHost): () => void {
  siglusEngineHosts.unshift(host);
  return () => {
    const index = siglusEngineHosts.indexOf(host);
    if (index !== -1) siglusEngineHosts.splice(index, 1);
  };
}

/**
 * What a browser build of the engine would require.
 *
 * Surfaced through the compatibility report and the engine panel so the reason a
 * Siglus game cannot start is specific rather than a generic "unsupported".
 */
export const SIGLUS_MISSING_CAPABILITIES: readonly string[] = [
  'siglus_rs 的 WebAssembly 构建（场景 VM、资源加载、键槽动画）',
  '离屏渲染到可读回的 RGBA 表面（wgpu 在浏览器中对应 WebGPU）',
  'Scene.pck 解包与键槽资源解码',
  'Gameexe.dat 加密配置表的解码器（无公开实现）',
  'OP/ED 视频与音频混音器的 Web 后端',
];

/**
 * A SiglusEngine session.
 *
 * `load()` fails immediately with an actionable message. That is the honest
 * behaviour: the alternative — a runtime that constructs, reports success and
 * then renders nothing — is indistinguishable to the player from a broken game,
 * and produces no useful diagnostic.
 */
export class SiglusRuntime implements RuntimeInstance {
  readonly runtimeId = 'siglus';
  private readonly missing: readonly string[];

  constructor(
    private readonly host: RuntimeHost,
    private readonly options: RuntimeOptions,
  ) {
    this.missing = [...SIGLUS_MISSING_CAPABILITIES];
  }

  async load(): Promise<void> {
    const reason =
      'SiglusEngine games are recognised but cannot run in this build: ' +
      'the engine is implemented in Rust (siglus_rs) and has no WebAssembly port yet.';
    this.host.log('error', reason);
    for (const capability of this.missing) {
      this.host.log('info', `missing: ${capability}`);
    }
    throw new UnsupportedEngineError(reason, 'siglus', this.missing);
  }

  tick(): void {
    // Never reached: `load()` rejects, so the host never starts the loop.
  }

  render(): void {
    const context = this.host.canvas.getContext('2d');
    if (!context) return;
    context.fillStyle = '#000000';
    context.fillRect(0, 0, this.host.canvas.width, this.host.canvas.height);
  }

  sendInput(): void {
    // No engine to receive input.
  }

  pause(): void {}
  resume(): void {}

  save(): SaveState {
    return {
      version: 1,
      data: { kind: 'siglus-unavailable' },
      caption: 'SiglusEngine 运行时不可用',
    };
  }

  async restore(): Promise<void> {
    // Nothing to restore.
  }

  dispose(): void {}

  describe(): Readonly<Record<string, unknown>> {
    return {
      available: false,
      reason: 'siglus_rs has no WebAssembly port',
      missingCapabilities: this.missing,
      surface: this.options.surface,
    };
  }

  report(): CompatibilityReport {
    return {
      runtimeId: this.runtimeId,
      severity: 'unsupported',
      unsupported: this.missing.map((capability) => ({
        command: capability,
        count: 1,
        handling: 'skipped' as const,
        note: 'required to run this engine',
      })),
      approximated: [],
      supportedCommandCount: 0,
    };
  }
}

/**
 * SiglusEngine provider.
 *
 * Priority matches ONScripter's, as in AetherKiri, so that a directory matching
 * both families resolves deterministically through the registry's tie-break
 * rather than by registration accident.
 */
export const siglusProvider: RuntimeProvider = {
  runtimeId: 'siglus',
  displayName: 'SiglusEngine',
  priority: 90,
  description:
    'Recognises SiglusEngine titles (Scene.pck, Gameexe.ini/.dat) and reads their configuration. Execution requires a WebAssembly build of siglus_rs, which does not exist yet.',
  accepts: ['Scene.pck', 'Gameexe.ini', 'Gameexe.dat'],
  containers: ['pck'],
  optionKeys: [OPTION_MEMORY_PROFILE, OPTION_STRICT_MODE, OPTION_COMPATIBILITY_REPORT],
  // The engine exists only as a native Rust library with no browser build, so
  // the provider deliberately advertises itself as unable to start a game.
  availability: siglusEngineHosts.length > 0 ? 'runnable' : 'unavailable',

  async probe(game: ProbeContext): Promise<ProbeResult | null> {
    return siglusProbeResult(game);
  },

  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance {
    return new SiglusRuntime(host, options);
  },
};
