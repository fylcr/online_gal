/**
 * Runtime (engine) identity.
 *
 * The set is open: third-party runtime packages register additional ids. The
 * well-known ones mirror the engine table of Tyranor-Next so that the same
 * game directories are recognised in the browser.
 */
export type RuntimeId = string;

import type { OptionKey } from './options.js';

/** Well-known runtime ids shipped with the platform. */
export const WellKnownRuntime = {
  KIRIKIRI: 'krkr2',
  ONSCRIPTER: 'onscripter',
  TYRANO: 'tyrano',
  SIGLUS: 'siglus',
  ARTEMIS: 'artemis',
  RPG_MAKER_MV: 'rpgmaker-mv',
  RPG_MAKER_MZ: 'rpgmaker-mz',
  RENPY: 'renpy',
  WEB: 'web',
} as const;

export type WellKnownRuntimeId =
  (typeof WellKnownRuntime)[keyof typeof WellKnownRuntime];

/**
 * How well the host can actually run a probed game. This is the honesty layer:
 * a game may be perfectly identified yet undeliverable, which the library UI
 * must surface instead of offering a play button that cannot work.
 */
export type RuntimeAvailability =
  /** A runtime provider is registered and can start this game. */
  | 'runnable'
  /** The engine was identified, but no runtime is available here. */
  | 'unavailable'
  /** Identified with low confidence; the host should confirm with the user. */
  | 'uncertain'
  /** Nothing matched. */
  | 'unknown';

export type ProbeConfidence = 'high' | 'medium' | 'low' | 'none';

/** A single piece of evidence collected while probing a game. */
export interface ProbeEvidence {
  /** VFS path that matched, e.g. `data.xp3`. */
  readonly path: string;
  /** Human-readable reason, e.g. `.xp3 archive`. */
  readonly detail: string;
  /** Score contribution of this evidence. */
  readonly weight: number;
}

/**
 * Result of probing a game directory. `score` drives dispatch: the highest
 * score wins, then priority, then registration order — the same tie-break chain
 * AetherKiri uses for its native runtime providers.
 */
export interface ProbeResult {
  readonly runtimeId: RuntimeId;
  readonly displayName: string;
  /** Positive match score, or 0 when the runtime does not recognise the game. */
  readonly score: number;
  readonly confidence: ProbeConfidence;
  /**
   * Path of the script/entry point the runtime should start, relative to the
   * game root. Runtime-specific meaning (e.g. `startup.tjs`).
   */
  readonly startup?: string;
  readonly evidence: readonly ProbeEvidence[];
}

/** Filesystem capabilities a game source may support. */
export interface GameSourceCapabilities {
  /** Entries can be enumerated. */
  readonly list: boolean;
  /** Byte ranges can be read without loading the whole file. */
  readonly randomAccess: boolean;
  /** The source can be written to (needed for in-place save games). */
  readonly writable: boolean;
}

/**
 * Read-only view of a game directory handed to runtimes.
 *
 * Mirrors AetherKiri's web import contract: user-authorised `File`/`Blob`
 * objects are mounted as read-only inputs with on-demand reads, and runtime
 * writes are redirected to a separate persistent layer owned by the host.
 */
export interface GameSource {
  /** Display name of the source, e.g. the directory or archive name. */
  readonly name: string;
  readonly capabilities: GameSourceCapabilities;
  /**
   * Lists entries directly under `dir` (default: root). Directories are
   * returned with a trailing `/`.
   */
  list(dir?: string): Promise<readonly string[]>;
  /** Reads a whole file. Rejects when the path does not exist. */
  readFile(path: string): Promise<Uint8Array>;
  /** Reads a byte range, when `randomAccess` is supported. */
  readRange?(
    path: string,
    offset: number,
    length: number,
  ): Promise<Uint8Array>;
  /** Returns the size in bytes, or -1 when unknown without reading. */
  size(path: string): Promise<number>;
  /** Returns true when the path exists and is a regular file. */
  exists(path: string): Promise<boolean>;
  /** Returns true when the path exists and is a directory. */
  isDirectory(path: string): Promise<boolean>;
}

/** A writable companion layer for saves, config and runtime scratch data. */
export interface WritableStore {
  readonly name: string;
  read(path: string): Promise<Uint8Array | undefined>;
  write(path: string, data: Uint8Array): Promise<void>;
  remove(path: string): Promise<void>;
  list(prefix?: string): Promise<readonly string[]>;
}

export type LogLevel = 'trace' | 'debug' | 'info' | 'warn' | 'error';

export interface LogRecord {
  readonly level: LogLevel;
  readonly runtimeId: RuntimeId;
  readonly message: string;
  readonly at: number;
  /** Optional structured payload for the diagnostics panel. */
  readonly data?: unknown;
}

/**
 * Services the host provides to a runtime. The root analogue of AetherKiri's
 * `engine_runtime_host_v1_t`.
 */
export interface RuntimeHost {
  /** Read-only game content. */
  readonly source: GameSource;
  /** Writable layer; defaults to an in-memory store when persistence is off. */
  readonly store: WritableStore;
  /** Canvas the runtime presents frames into. */
  readonly canvas: HTMLCanvasElement;
  /** Surface size in device-independent CSS pixels. */
  readonly surface: { width: number; height: number };
  /** Called by the runtime whenever it wants a repaint. */
  requestFrame(): void;
  log(level: LogLevel, message: string, data?: unknown): void;
  /** Monotonic clock in milliseconds. */
  now(): number;
  /**
   * Emitted when the runtime wants the host shell to react — opening the
   * settings drawer, declaring the session over, changing volume, and so on.
   */
  notify(event: RuntimeHostEvent): void;
  /** Resolves a user-facing settings value, falling back to the default. */
  setting<T>(key: string, fallback: T): T;
}

export type RuntimeHostEvent =
  | { readonly type: 'session-end'; readonly reason?: string }
  | { readonly type: 'request-settings' }
  | { readonly type: 'request-menu'; readonly items: readonly MenuItem[] }
  | { readonly type: 'title'; readonly title: string }
  | { readonly type: 'volume'; readonly channel: AudioChannel; readonly value: number }
  | { readonly type: 'fatal'; readonly message: string };

export interface MenuItem {
  readonly path: string;
  readonly label: string;
  readonly enabled?: boolean;
  readonly checked?: boolean;
}

export type AudioChannel = 'bgm' | 'se' | 'voice' | 'system';

/** Options the host passes when constructing a runtime. */
export interface RuntimeOptions {
  readonly runtimeId: RuntimeId;
  /** Surface size requested by the game, before any user scaling. */
  readonly surface: { width: number; height: number };
  /** Per-game overrides coming from the library entry. */
  readonly gameConfig?: Readonly<Record<string, unknown>>;
  /** Whether this is a cold start or a resume from a save slot. */
  readonly mode: 'new' | 'continue';
}

export type InputEvent =
  | {
      readonly type: 'pointer-down';
      readonly x: number;
      readonly y: number;
      readonly button: number;
      /** DOM pointer identity; the mouse is always 0. */
      readonly pointerId?: number;
    }
  | {
      readonly type: 'pointer-up';
      readonly x: number;
      readonly y: number;
      readonly button: number;
      readonly pointerId?: number;
    }
  | {
      readonly type: 'pointer-move';
      readonly x: number;
      readonly y: number;
      readonly pointerId?: number;
      /** True while the primary button is held, for drag handling. */
      readonly pressed?: boolean;
    }
  | { readonly type: 'pointer-wheel'; readonly x: number; readonly y: number; readonly deltaY: number }
  | { readonly type: 'key-down'; readonly key: string; readonly code: string; readonly repeat: boolean }
  | { readonly type: 'key-up'; readonly key: string; readonly code: string }
  | { readonly type: 'text'; readonly value: string }
  | { readonly type: 'blur' };

/** Serializable save payload produced by a runtime. */
export interface SaveState {
  /** Format version so runtimes can migrate old slots. */
  readonly version: number;
  /** Runtime-specific state. Must be structured-clone friendly. */
  readonly data: unknown;
  /** Small thumbnail of the frame at save time, when available. */
  readonly thumbnail?: string;
  /** Text shown in the save slot list. */
  readonly caption?: string;
}

/** A live game session. The root analogue of AetherKiri's runtime instance
 * plus its `tick`/`send_input`/`read_frame_rgba` triple — except that here the
 * runtime draws straight into the host canvas, so no RGBA readback is needed.
 */
export interface RuntimeInstance {
  readonly runtimeId: RuntimeId;
  /** Resolves once the game is loaded and the first frame may be drawn. */
  load(): Promise<void>;
  /** Advances the simulation. `deltaMs` is wall-clock since the last tick. */
  tick(deltaMs: number): void;
  /** Renders the current frame into the host canvas. */
  render(): void;
  /** Delivers a normalised input event. */
  sendInput(event: InputEvent): void;
  /** Suspends audio and timers. */
  pause(): void;
  resume(): void;
  /** Serialises the session for a save slot. */
  save(): SaveState;
  /** Restores a previously saved session. */
  restore(state: SaveState): Promise<void>;
  /** Releases all resources: audio nodes, object URLs, workers. */
  dispose(): void;
  /** Diagnostics surfaced in the host's engine panel. */
  describe?(): Readonly<Record<string, unknown>>;
  /**
   * Per-command compatibility report for the session so far.
   *
   * The browser counterpart of AetherKiri's plugin debug info: a native build
   * substitutes mock objects for plugins it cannot load and reports them, which
   * is how its compatibility notes are produced. Here the equivalent is the set
   * of script commands an engine issued that the runtime could not honour, so a
   * game that "runs" but misbehaves can be diagnosed without a debugger.
   */
  report?(): CompatibilityReport;
}

/** One unsupported command encountered while running a game. */
export interface UnsupportedCommand {
  /** Command or tag name, e.g. `movie` or `playplugin`. */
  readonly command: string;
  /** How many times it was issued. */
  readonly count: number;
  /** First line of the script it appeared on, when known. */
  readonly line?: number;
  /** What the runtime did instead: skipped it, or approximated it. */
  readonly handling: 'skipped' | 'approximated';
  /** Optional note, e.g. what the approximation was. */
  readonly note?: string;
}

/**
 * A session's compatibility summary.
 *
 * `severity` is a coarse judgement for the library UI: `clean` means every
 * command was honoured, `degraded` means the game ran with substitutions, and
 * `unsupported` means at least one command was silently skipped in a way that
 * is likely to affect play.
 */
export interface CompatibilityReport {
  readonly runtimeId: RuntimeId;
  readonly severity: 'clean' | 'degraded' | 'unsupported';
  readonly unsupported: readonly UnsupportedCommand[];
  /** Commands handled by an approximation, with the substitution described. */
  readonly approximated: readonly UnsupportedCommand[];
  /** Total distinct commands the runtime did honour. */
  readonly supportedCommandCount: number;
}

/** Builds a report from collected command tallies. */
export function summariseCompatibility(
  runtimeId: RuntimeId,
  entries: readonly UnsupportedCommand[],
  supportedCommandCount: number,
): CompatibilityReport {
  const approximated = entries.filter((entry) => entry.handling === 'approximated');
  const skipped = entries.filter((entry) => entry.handling === 'skipped');
  return {
    runtimeId,
    severity: skipped.length > 0 ? 'unsupported' : approximated.length > 0 ? 'degraded' : 'clean',
    unsupported: skipped,
    approximated,
    supportedCommandCount,
  };
}

/**
 * A runtime provider: the unit of extensibility.
 *
 * Contract notes carried over from AetherKiri's provider ABI:
 * - descriptors must stay valid for the lifetime of the registry;
 * - `probe` must not construct a runtime and must not throw for a valid path;
 * - `probe` returns 0 for "not mine" and a positive score for a match;
 * - runtime instances are created and destroyed exactly once.
 */
export interface RuntimeProvider {
  readonly runtimeId: RuntimeId;
  readonly displayName: string;
  /**
   * Higher priority wins ties. Use this for a runtime that should beat a more
   * generic sibling at equal score (e.g. RPG Maker MZ over MV).
   */
  readonly priority: number;
  /** One-line description shown in the engine panel. */
  readonly description?: string;
  /** Engines this provider claims to support, for the library UI. */
  readonly accepts?: readonly string[];
  /**
   * Option keys this runtime honours.
   *
   * Ported from AetherKiri's `engine_option_t` handling: a provider accepts a
   * flat key/value pair and ignores unknown keys. Declaring the supported set
   * lets the launcher render only knobs that do something instead of accepting
   * settings that are silently dropped.
   */
  readonly optionKeys?: readonly OptionKey[];
  /**
   * Archival and script containers this runtime can read, advertised so the
   * importer can warn before a multi-gigabyte import rather than after.
   */
  readonly containers?: readonly string[];
  /**
   * Whether this provider can actually start a game.
   *
   * Defaults to `runnable`. A provider that recognises its engine but has no
   * executing backend — for example one whose engine is a native library with
   * no WebAssembly port — declares `unavailable` so the library reports the
   * limitation instead of offering a play button that fails on click. This is
   * the one piece of provider self-knowledge the UI cannot infer.
   */
  readonly availability?: RuntimeAvailability;
  /**
   * Scores how well this runtime matches `game`. Must be side-effect free.
   * Never throws: an unreadable directory scores 0.
   */
  probe(game: ProbeContext): Promise<ProbeResult | null>;
  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance;
}

/** Read-side helpers handed to `probe`, so providers never touch the raw FS. */
export interface ProbeContext {
  readonly source: GameSource;
  /** Root-relative, `/`-separated, no leading slash. */
  has(path: string): Promise<boolean>;
  /** True when any entry matches the case-insensitive suffix or glob tail. */
  hasSuffix(suffix: string): Promise<boolean>;
  hasEvery(paths: readonly string[]): Promise<boolean>;
  /** Reads a small text file, returning undefined when missing or oversized. */
  readText(path: string, maxBytes?: number): Promise<string | undefined>;
  /** Lists a directory. Missing directories yield an empty list. */
  list(dir?: string): Promise<readonly string[]>;
  /** True when `dir` contains at least one file matching `pattern`. */
  dirContains(dir: string, pattern: RegExp): Promise<boolean>;
}

export class RuntimeError extends Error {
  override readonly name = 'RuntimeError';
  override readonly cause?: unknown;

  constructor(message: string, readonly runtimeId: RuntimeId, cause?: unknown) {
    super(message);
    this.cause = cause;
  }
}

/** Unsupported engine, surfaced to the user with an actionable message. */
export class UnsupportedEngineError extends Error {
  override readonly name = 'UnsupportedEngineError';

  constructor(
    message: string,
    readonly runtimeId: RuntimeId | null,
    readonly alternatives: readonly string[] = [],
  ) {
    super(message);
  }
}
