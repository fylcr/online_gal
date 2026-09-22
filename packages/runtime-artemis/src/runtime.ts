import {
  OPTION_COMPATIBILITY_REPORT,
  OPTION_MEMORY_PROFILE,
  OPTION_SCRIPT_ENCODING,
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
import { artemisProbeResult } from './probe.js';

/**
 * The operation surface an Artemis runtime must provide.
 *
 * Two very different upstreams exist and both are native:
 *
 * - **Artemis** itself embeds Lua 5.1. AetherKiri keeps it behind a private
 *   build flag (`AETHERKIRI_ENABLE_ARTEMIS_RUNTIME`) and has to prefix every Lua
 *   global at compile time, because OnscripterYuri links Lua 5.4 into the same
 *   process and the two ABIs would otherwise bind to each other's symbols.
 * - **RFVP** is a Rust reimplementation, which is what AetherKiri's public
 *   `bridge/rfvp_runtime/` drives through `aether_rfvp_*`.
 *
 * Neither has a browser build. RFVP's bridge is written down here because it is
 * the one that could plausibly be ported: the Rust core already renders into a
 * caller-supplied surface, so a WebAssembly build is a rendering-backend change
 * rather than a redesign.
 *
 * The three options are the ones AetherKiri's provider insists be set *before*
 * opening a game, and rejects afterwards with an invalid-state error — setting
 * them late would change how already-loaded scripts were decoded.
 */
export interface ArtemisEngineHost {
  /** Creates a shell. RFVP's `aether_rfvp_new`. */
  create(): Promise<number>;
  /** Opens a game directory. `startup` is the entry script when known. */
  open(
    handle: number,
    gameRoot: string,
    startup: string | null,
    options: ArtemisOpenOptions,
  ): Promise<void>;
  tick(handle: number, deltaMs: number): Promise<{ terminateRequested: boolean }>;
  setPaused(handle: number, paused: boolean): Promise<void>;
  /** Frame geometry of the latest rendered frame, tightly packed RGBA rows. */
  frameDesc(handle: number): Promise<{ width: number; height: number; serial: number }>;
  readFrameRgba(handle: number): Promise<Uint8Array>;
  sendInput(handle: number, event: InputEvent): Promise<void>;
  free(handle: number): Promise<void>;
  /** Drains the runtime's own log queue, one message per call. */
  logMessage(handle: number): string | null;
  lastError(handle: number): string;
}

/** Options that must be supplied before `open`. */
export interface ArtemisOpenOptions {
  /** Directory writes are redirected to. */
  readonly writablePath: string;
  /** Default font face name, when the game exposes one. */
  readonly font: string;
  /** `sjis` | `gbk` | `utf8` — how script text is decoded. */
  readonly encoding: 'sjis' | 'gbk' | 'utf8';
  /** `auto` | `gpu` | `cpu` — rendering backend preference. */
  readonly renderer: 'auto' | 'gpu' | 'cpu';
}

/**
 * Host implementations known to this build.
 *
 * Empty by design, for the same reason as Siglus: a placeholder would turn
 * "recognised" into "appears to work".
 */
export const artemisEngineHosts: ArtemisEngineHost[] = [];

/** Registers a host implementation, returning a deregistration function. */
export function registerArtemisEngineHost(host: ArtemisEngineHost): () => void {
  artemisEngineHosts.unshift(host);
  return () => {
    const index = artemisEngineHosts.indexOf(host);
    if (index !== -1) artemisEngineHosts.splice(index, 1);
  };
}

/**
 * Why an Artemis game cannot start here.
 *
 * The Lua point is worth stating explicitly: even a hypothetical Lua 5.4/JIT
 * build would not be a drop-in, because Artemis targets 5.1 and the two
 * languages differ in ways that scripts notice (`goto`, integer division,
 * `setfenv`, the removal of `unpack`).
 */
export const ARTEMIS_MISSING_CAPABILITIES: readonly string[] = [
  'RFVP（Rust）或 Artemis（Lua 5.1）的 WebAssembly 构建',
  'Lua 5.1 运行时及其 Artemis 宿主 API（浏览器里没有 Lua 5.4 可替代）',
  'PFS 包解包与 .iet 场景脚本解释器',
  'E-mote 与 PSB 资源解码（.psb/.mtn/.emt）',
  'OP/ED 视频播放后端',
];

/**
 * An Artemis session.
 *
 * `load()` rejects with the specific reason and the capability list. Reporting
 * "recognised but not runnable" is the honest outcome; the alternative is a
 * runtime that starts, renders nothing, and gives the player no way to tell a
 * broken build from a broken game.
 */
export class ArtemisRuntime implements RuntimeInstance {
  readonly runtimeId = 'artemis';
  private readonly missing: readonly string[];

  constructor(_host: RuntimeHost, _options: RuntimeOptions) {
    this.missing = [...ARTEMIS_MISSING_CAPABILITIES];
  }

  async load(): Promise<void> {
    const reason =
      'ArtemisEngine games are recognised but cannot run in this build: ' +
      'the engine is native (RFVP in Rust, or Artemis with embedded Lua 5.1) and has no WebAssembly port yet.';
    throw new UnsupportedEngineError(reason, 'artemis', this.missing);
  }

  tick(): void {}
  render(): void {}
  sendInput(): void {}
  pause(): void {}
  resume(): void {}

  save(): SaveState {
    return {
      version: 1,
      data: { kind: 'artemis-unavailable' },
      caption: 'ArtemisEngine 运行时不可用',
    };
  }

  async restore(): Promise<void> {}

  dispose(): void {}

  describe(): Readonly<Record<string, unknown>> {
    return {
      available: false,
      reason: 'no WebAssembly build of RFVP or Artemis',
      missingCapabilities: this.missing,
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
 * ArtemisEngine provider.
 *
 * Priority 85 places it below Siglus and ONScripter (90) and above the web
 * runtimes: an Artemis directory that also ships an `index.html` should resolve
 * to Artemis, while a directory carrying both Artemis and Siglus markers
 * resolves by score first.
 */
export const artemisProvider: RuntimeProvider = {
  runtimeId: 'artemis',
  displayName: 'ArtemisEngine',
  priority: 85,
  description:
    'Recognises ArtemisEngine titles (root.pfs, system.ini, system/first.iet, numbered .pfs patches) and reads their PFS catalogue and configuration. Execution requires a WebAssembly build of RFVP or Artemis, which does not exist yet.',
  accepts: ['root.pfs', 'system.ini', 'boot.ini', 'system/first.iet', '.pfs', '.iet'],
  containers: ['pfs'],
  optionKeys: [
    OPTION_SCRIPT_ENCODING,
    OPTION_MEMORY_PROFILE,
    OPTION_STRICT_MODE,
    OPTION_COMPATIBILITY_REPORT,
  ],
  availability: artemisEngineHosts.length > 0 ? 'runnable' : 'unavailable',

  async probe(game: ProbeContext): Promise<ProbeResult | null> {
    return artemisProbeResult(game);
  },

  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance {
    return new ArtemisRuntime(host, options);
  },
};
