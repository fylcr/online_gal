import {
  OPTION_COMPATIBILITY_REPORT,
  OPTION_MEMORY_PROFILE,
  OPTION_SCRIPT_ENCODING,
  OPTION_STRICT_MODE,
  UnsupportedEngineError,
  type CompatibilityReport,
  type InputEvent,
  type LogLevel,
  type ProbeContext,
  type ProbeResult,
  type RuntimeHost,
  type RuntimeInstance,
  type RuntimeOptions,
  type RuntimeProvider,
  type SaveState,
  type UnsupportedCommand,
} from '@online-gal/engine-api';
import { artemisProbeResult } from './probe.js';
import { hasArtemisCompatEngine } from './configure.js';
import { CompatArtemisRuntime } from './compat/runtime.js';
import {
  ARTEMIS_BROWSER_SNAPSHOT_VERSION,
  ArtemisBrowserIdb,
  assertArtemisMountPoint,
  createArtemisBrowserServices,
  type ArtemisBrowserServices,
  type ArtemisBrowserSnapshot,
  type ArtemisBrowserSnapshotFile,
  type ArtemisBrowserSnapshotIdb,
} from './browser-services.js';
import { ArtemisHostError } from './host/errno.js';
import type { FsNode } from './host/fs.js';
import type { ArtemisSyncIdb } from './host/fd.js';
import { ARTEMIS_CALLBACK_SIGNATURES, createArtemisSessionCore } from './session-core.js';
import type {
  ArtemisSessionCore,
  ArtemisSessionCoreOptions,
  ArtemisSessionDiagnostics,
} from './session-core.js';

/**
 * The operation surface an Artemis runtime must provide.
 *
 * Hosts are the *supply* side: something has to hand this package bytes before a game can run.
 * Two very different upstreams exist and both are native. **Artemis** embeds GIF/Lua-era Artemis
 * Engine (`root.pfs`, `.iet` scripts); **RFVP** is a Rust reimplementation. Neither has a native
 * browser build, so the supply side here is a WebAssembly bridge configured through
 * {@link configureArtemisWasm}.
 *
 * Registering a host is *optional and legacy*: it is how an embedder that already owns a bridge
 * injects it. It is not a router — the runtime never dispatches work through this list — and its
 * presence no longer decides whether the provider is runnable, because the configured WebAssembly
 * binary is what a session actually runs on.
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
 * The array lives on: it is the published compatibility surface for embedders that register a
 * bridge. It is deliberately **not** consulted when deciding availability, and a session never
 * routes through it.
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

/** How the bridge may be supplied. `null` withdraws it. */
export type ArtemisWasmSource = string | URL | Uint8Array | WebAssembly.Module;

/** One published option: exactly one of the two carriers is present. */
type ArtemisWasmBinding = { source: ArtemisWasmSource } | { bytes: Uint8Array };

/** The process-wide bridge publication, and the generation a replacement bumps. */
let configuredWasm: ArtemisWasmBinding | null = null;
let configuredGeneration = 0;

function cloneWasmBinding(source: ArtemisWasmSource): ArtemisWasmBinding {
  if (typeof source === 'string') {
    if (source.trim().length === 0) throw new ArtemisHostError('the Artemis bridge URL is blank');
    return { source };
  }
  if (source instanceof URL) return { source };
  if (source instanceof Uint8Array) {
    if (source.byteLength === 0) throw new ArtemisHostError('the Artemis bridge bytes are empty');
    // Cloned on publish *and* on read: a shared array would let a caller rewrite the binary under
    // a live session, and `WebAssembly.instantiate` over a buffer another party is mutating is a
    // corrupt module rather than a fast one.
    return { bytes: source.slice() };
  }
  if (source instanceof WebAssembly.Module) return { source };
  throw new ArtemisHostError('the Artemis bridge must be a string, URL, Uint8Array or WebAssembly.Module');
}

/**
 * Publishes the legacy asyncify bridge.
 *
 * This is the deployment seam, not a per-game setting: an embedder that bundles `artemis.wasm`
 * passes its URL or bytes here once. Passing `null` withdraws the publication.
 *
 * **It is no longer the provider's default engine.** `artemisProvider.create` runs the compat
 * runtime unless the game itself names `wasm` / `wasmUrl`, so this seam is what a *diagnostic*
 * session runs on: the path that compares the legacy bridge against the compat one. A deployment
 * that publishes a bridge here and no compat engine gets `unavailable`, not a silent legacy session.
 */
export function configureArtemisWasm(options: {
  wasm?: string | URL | Uint8Array | WebAssembly.Module | null;
}): void {
  const next = options?.wasm ?? null;
  if (next === null) {
    configuredWasm = null;
    configuredGeneration += 1;
    return;
  }
  const binding = cloneWasmBinding(next);
  configuredWasm = binding;
  configuredGeneration += 1;
}

/** A fresh copy of the published bridge, or `null`. Never the stored binding itself. */
function readConfiguredWasm(): ArtemisWasmBinding | null {
  const binding = configuredWasm;
  if (binding === null) return null;
  return 'bytes' in binding ? { bytes: binding.bytes.slice() } : { source: binding.source };
}

/** Resolves one `gameConfig` entry, honouring only the shapes the caller declared. */
function gameConfigWasms(config: Readonly<Record<string, unknown>> | undefined): ArtemisWasmSource[] {
  if (!config) return [];
  const found: ArtemisWasmSource[] = [];
  const wasm = config['wasm'];
  if (wasm !== undefined && wasm !== null) {
    // A URL instance is not an `instanceof` in every realm a page can hand us, so `typeof` and the
    // `href` shape are checked as well; anything else is a configuration error the caller must see
    // rather than a silently ignored key.
    if (typeof wasm === 'string') found.push(wasm);
    else if (wasm instanceof URL) found.push(wasm);
    else if (wasm instanceof Uint8Array) found.push(wasm);
    else if (wasm instanceof WebAssembly.Module) found.push(wasm);
    else throw new ArtemisHostError('options.gameConfig.wasm is not a URL, bytes or a module');
  }
  // Documented fallback: a game that names only a URL is the common case for a deployed bridge.
  const url = config['wasmUrl'];
  if (url !== undefined && url !== null) {
    if (typeof url === 'string' || url instanceof URL) found.push(url);
    else throw new ArtemisHostError('options.gameConfig.wasmUrl is not a URL');
  }
  return found;
}

/**
 * Why an Artemis game cannot start here.
 *
 * Every row names something the *deployment* has to supply. The former "PFS/.iet are unsupported"
 * and "there is no Lua 5.4" rows are gone: this package indexes PFS archives and interprets
 * `.iet` configuration itself, so those were claims about a build that no longer exists rather
 * than about the engine.
 *
 * These are the *shapes* of the three rows, not a verdict about one session: which of them a
 * runtime has actually satisfied is answered by `describe().missingCapabilities`, which recomputes
 * them from what has been observed.
 */
export const ARTEMIS_MISSING_BRIDGE =
  'Artemis/RFVP 的 WebAssembly 桥接二进制：需由部署方通过 configureArtemisWasm() 提供';

/** A canvas whose WebGL context the guest can render into. */
export const ARTEMIS_MISSING_CANVAS = '浏览器 WebGL 画布（无 WebGL 上下文的页面无法呈现引擎画面）';

/**
 * The WebAudio the OpenAL rows need: a device factory the deployment declared, or WebAudio in the
 * page. Either half satisfies it, and both are read rather than constructed.
 */
export const ARTEMIS_MISSING_AUDIO =
  '浏览器 Web Audio 支持（OpenAL 行需要 AudioContext，缺失时引擎报告 0 个音频设备）';

export const ARTEMIS_MISSING_CAPABILITIES: readonly string[] = [
  ARTEMIS_MISSING_BRIDGE,
  ARTEMIS_MISSING_CANVAS,
  ARTEMIS_MISSING_AUDIO,
];

/**
 * Whether a WebAudio host is wired into a session's `gl` bucket.
 *
 * It is: `ArtemisSessionCoreOptions.audioContextFactory` exists, and `createArtemisSessionCore` hands
 * the `gl` bucket the host `createArtemisAudioHost` builds, so the nineteen `_al*`/`_alc*` rows are
 * live. A declared factory is that host's own device factory; a page that declares none still
 * instantiates normally and refuses at the guest's first audio call rather than at construction.
 */
const ARTEMIS_AUDIO_HOST_WIRED = true;

/** The one thing no factory can supply: a page with no WebAudio at all. */
const ARTEMIS_AUDIO_SEAM =
  'the session audio host takes gameConfig.audioContextFactory; a page without WebAudio refuses at the first audio call';

/** The `data.kind` tag on a snapshot this package produces, so a foreign one is refused. */
export const ARTEMIS_SAVE_KIND = 'artemis-browser-state';

/** `SaveState.version` this runtime writes. */
export const ARTEMIS_SAVE_VERSION = 1;

/** The shape inside `SaveState.data`. */
export interface ArtemisSaveData {
  readonly kind: typeof ARTEMIS_SAVE_KIND;
  readonly snapshot: ArtemisBrowserSnapshot;
}

/** What {@link ArtemisRuntime.whenDisposed} resolves with. */
export interface ArtemisDisposalReport {
  /** True when the terminal save landed. False for every refusal, including "nothing to save". */
  readonly saved: boolean;
  /** Why the terminal save did not happen. Undefined only when `saved` is true. */
  readonly failure?: string;
}

/** Bounds on a snapshot a caller hands `restore`. A save the store cannot hold is a refusal, not a hang. */
const MAX_RESTORE_FILES = 4096;
const MAX_RESTORE_ENTRIES = 16384;
const MAX_RESTORE_BYTES = 256 * 1024 * 1024;

/** Parses `SaveState.data`, refusing anything that is not exactly this runtime's document. */
function readSaveData(data: unknown): ArtemisBrowserSnapshot {
  if (typeof data !== 'object' || data === null) {
    throw new ArtemisHostError('artemis save data is not an object');
  }
  const record = data as Partial<ArtemisSaveData>;
  if (record.kind !== ARTEMIS_SAVE_KIND) {
    throw new ArtemisHostError(`artemis save data kind ${String(record.kind)} is not ${ARTEMIS_SAVE_KIND}`);
  }
  const snapshot = record.snapshot as Partial<ArtemisBrowserSnapshot> | undefined;
  if (typeof snapshot !== 'object' || snapshot === null) {
    throw new ArtemisHostError('artemis save data carries no snapshot');
  }
  if (snapshot.version !== ARTEMIS_BROWSER_SNAPSHOT_VERSION) {
    throw new ArtemisHostError(`artemis snapshot version ${String(snapshot.version)} is not supported`);
  }
  if (!Array.isArray(snapshot.files) || !Array.isArray(snapshot.idb)) {
    throw new ArtemisHostError('artemis snapshot is missing its files or idb array');
  }
  if (snapshot.files.length > MAX_RESTORE_FILES || snapshot.files.length + snapshot.idb.length > MAX_RESTORE_ENTRIES) {
    throw new ArtemisHostError('artemis snapshot declares more entries than this runtime will restore');
  }
  // Every buffer is cloned and every path is checked before anything reaches the filesystem, so a
  // malformed entry cannot leave a half-replaced save root behind.
  let total = 0;
  const files: ArtemisBrowserSnapshotFile[] = (snapshot.files as readonly unknown[]).map(
    (entry): ArtemisBrowserSnapshotFile => {
      if (!Array.isArray(entry) || entry.length !== 2) {
        throw new ArtemisHostError('artemis snapshot file entry is not a [path, bytes] pair');
      }
      const [path, bytes] = entry as readonly unknown[];
      if (typeof path !== 'string' || !(bytes instanceof Uint8Array)) {
        throw new ArtemisHostError('artemis snapshot file entry is not a [string, Uint8Array] pair');
      }
      total += bytes.byteLength;
      if (total > MAX_RESTORE_BYTES) throw new ArtemisHostError('artemis snapshot is larger than this runtime will restore');
      return [path, bytes.slice()];
    },
  );
  const idb: ArtemisBrowserSnapshotIdb[] = (snapshot.idb as readonly unknown[]).map(
    (entry): ArtemisBrowserSnapshotIdb => {
      if (!Array.isArray(entry) || entry.length !== 3) {
        throw new ArtemisHostError('artemis snapshot idb entry is not a [db, id, bytes] triple');
      }
      const [db, id, bytes] = entry as readonly unknown[];
      if (typeof db !== 'string' || typeof id !== 'string' || !(bytes instanceof Uint8Array)) {
        throw new ArtemisHostError('artemis snapshot idb entry is not a [string, string, Uint8Array] triple');
      }
      total += bytes.byteLength;
      if (total > MAX_RESTORE_BYTES) throw new ArtemisHostError('artemis snapshot is larger than this runtime will restore');
      return [db, id, bytes.slice()];
    },
  );
  return { version: ARTEMIS_BROWSER_SNAPSHOT_VERSION, files, idb };
}

/**
 * The key fields an emscripten key block reads.
 *
 * `keyCode`/`which` are not derivable from `key`/`code` by any rule the guest shares — the glue's own
 * table is a fixed list — so the legacy numbers are computed here from the two portable fields. A key
 * with no entry keeps `undefined`, which is reported rather than answered with a fabricated 0.
 */
function keyInit(
  key: string,
  code: string,
  repeat: boolean | undefined,
): { key: string; code: string; keyCode: number; which: number; repeat?: boolean } {
  let keyCode = 0;
  if (key.length === 1) keyCode = key.toUpperCase().charCodeAt(0);
  else if (/^F([1-9]|1[0-2])$/.test(key)) keyCode = 111 + Number(key.slice(1));
  else {
    const named: Readonly<Record<string, number>> = {
      Enter: 13,
      Escape: 27,
      Backspace: 8,
      Tab: 9,
      ' ': 32,
      ArrowLeft: 37,
      ArrowUp: 38,
      ArrowRight: 39,
      ArrowDown: 40,
      Delete: 46,
      Home: 36,
      End: 35,
      PageUp: 33,
      PageDown: 34,
      Shift: 16,
      Control: 17,
      Alt: 18,
      Meta: 91,
      CapsLock: 20,
    };
    keyCode = named[key] ?? named[code] ?? 0;
  }
  const init: { key: string; code: string; keyCode: number; which: number; repeat?: boolean } = {
    key,
    code,
    keyCode,
    which: keyCode,
  };
  if (repeat !== undefined) init.repeat = repeat;
  return init;
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
 * Owns a promise at the instant it exists.
 *
 * Attaching the handler now means a rejection can never be reported as unhandled, even when a later
 * synchronous throw in the same body stops the caller from ever awaiting it. The promise is returned
 * unchanged, so an awaiter still sees the original rejection.
 */
function owned<T>(promise: Promise<T>): Promise<T> {
  void promise.catch(() => undefined);
  return promise;
}

/**
 * Awaits every sibling and rethrows the *first* rejection verbatim.
 *
 * Verbatim on purpose: a lifecycle that turned `throw undefined` into a message would lose the only
 * thing the caller had. Every promise is settled, so no sibling is left with an unhandled rejection
 * because another one failed first.
 */
async function settleFirst(...pending: readonly Promise<unknown>[]): Promise<void> {
  const settled = await Promise.allSettled(pending);
  for (const result of settled) {
    if (result.status === 'rejected') throw result.reason;
  }
}

/**
 * Whether the page has a WebAudio implementation at all, read without constructing one.
 *
 * These are the same two globals `createArtemisAudioHost` falls back to when no factory was
 * declared, checked with `typeof`: a probe that called `new AudioContext()` would create the very
 * device the guest is supposed to own, and a capability check must not change what it measures.
 */
function pageHasWebAudio(canvas: HTMLCanvasElement | null | undefined): boolean {
  let view: unknown = null;
  try {
    view = (canvas?.ownerDocument as Document | undefined)?.defaultView ?? null;
  } catch {
    // A canvas that throws on access is not a page with WebAudio.
    view = null;
  }
  const scope = (view ?? globalThis) as {
    AudioContext?: unknown;
    webkitAudioContext?: unknown;
  };
  return (
    typeof scope.AudioContext === 'function' || typeof scope.webkitAudioContext === 'function'
  );
}

/** `MouseEvent.buttons` bit for one `MouseEvent.button` value. An unknown button holds no bit. */
function buttonMask(button: number): number {
  switch (button) {
    case 0:
      return 1;
    case 1:
      return 4;
    case 2:
      return 2;
    case 3:
      return 8;
    case 4:
      return 16;
    default:
      return 0;
  }
}

/** Whether an element is something a player types into. */
function isEditable(element: Element | null): boolean {
  if (!element) return false;
  const tag = typeof element.tagName === 'string' ? element.tagName.toLowerCase() : '';
  if (tag === 'input' || tag === 'textarea' || tag === 'select') return true;
  try {
    const attribute = element.getAttribute?.('contenteditable');
    return attribute !== null && attribute !== undefined && attribute !== 'false';
  } catch {
    return false;
  }
}

/**
 * The editable element that owns the keyboard right now, or `null`.
 *
 * Read from `activeElement` rather than by searching the document: a search finds the *first* input
 * on the page — the host shell's own search box, most of the time — and writing a game's text into
 * it is a hijack, not a delivery.
 */
function editableFocus(doc: Document | null): Element | null {
  const active = doc?.activeElement;
  return isEditable(active ?? null) ? (active ?? null) : null;
}

/** The mount point the services hydrated for a save path, or `null` when it is not one. */
function canonicalMount(savePath: string): string | null {
  if (typeof savePath !== 'string' || savePath.length === 0) return null;
  try {
    return assertArtemisMountPoint(savePath);
  } catch {
    return null;
  }
}

/**
 * The guest's save root, resolved through the same canonical mount the services hydrated.
 *
 * `assets.savePath` is the game's own spelling and may be relative or trailing-slashed
 * (`gameone/`, `/save/game/`); the filesystem holds the canonical mount. Looking the raw spelling up
 * finds nothing, which is how a save silently becomes an empty slot.
 */
function saveRootOf(core: ArtemisSessionCore): FsNode | null {
  const mount = canonicalMount(core.assets.savePath);
  return mount === null ? null : core.fs.lookup(mount);
}

/**
 * A synchronous, side-effect-free walk of the save root.
 *
 * Synchronous on purpose: {@link ArtemisRuntime.save} is a synchronous contract and the guest
 * cannot be awaited from it, so the snapshot has to be a clone taken from the tree as the guest
 * left it. Only the save root is walked — the game tree is read-only and is never enumerated.
 */
function snapshotSaveRoot(root: FsNode): ArtemisBrowserSnapshotFile[] {
  const files: ArtemisBrowserSnapshotFile[] = [];
  const walk = (node: FsNode, prefix: string): void => {
    if (node.isFile) {
      const size = node.size;
      const bytes = node.data ? new Uint8Array(node.data.readRange(0, size)) : new Uint8Array(0);
      files.push([prefix, bytes]);
      return;
    }
    if (!node.isDir) return;
    for (const [name, child] of node.children) {
      // No name is skipped here. The walk starts at the save root, where a `proc` directory is the
      // guest's own data — unlike the filesystem root, which the game tree does not belong in.
      walk(child, prefix.length === 0 ? name : `${prefix}/${name}`);
    }
  };
  walk(root, '');
  return files;
}

/**
 * An Artemis session.
 *
 * The runtime hosts itself: there is no worker, no iframe and no `eval`. The bridge is compiled
 * with `WebAssembly.instantiate` and the guest's GLFW frame loop drives itself through
 * `requestAnimationFrame`, so `tick()` and `render()` are no-ops by design — a host that scheduled
 * a tick through `requestFrame()` would be running a second, competing clock.
 */
export class ArtemisRuntime implements RuntimeInstance {
  readonly runtimeId = 'artemis';

  private readonly host: RuntimeHost;
  private readonly options: RuntimeOptions;

  /** The game's own surface size, or the host's when the game named none. */
  private readonly surface: { width: number; height: number };

  private core: ArtemisSessionCore | null = null;
  private services: ArtemisBrowserServices | null = null;
  /** Set synchronously by `dispose`; every later entry point checks it before touching the guest. */
  private disposed = false;
  /**
   * True between `pause()` and a `resume()` that actually ran.
   *
   * Tracked here rather than read off the guest because a paused guest is still *instantiated*: the
   * bridge's loop is stopped but its listeners are not removed, so a dispatched event would be
   * delivered into a mixer and an input queue nobody is draining.
   */
  private paused = false;
  /** Memoized creation. An identity, not a guard: the second `load` gets the first promise. */
  private loadPromise: Promise<void> | null = null;
  /** True only once `load` has fully resolved. A failed load never sets it. */
  private loaded = false;
  /**
   * The lifecycle chain.
   *
   * Every `pause`/`resume`/`dispose` appends to it, so a `resume` issued in the same turn as a
   * `pause` cannot overtake it: `resume` waits for the pause to have *finished* stopping the guest,
   * not merely to have been called.
   */
  private transition: Promise<void> = Promise.resolve();
  private disposePromise: Promise<ArtemisDisposalReport> | null = null;
  /** The reason the one load attempt failed, once it has. Observed state, read by `available`. */
  private loadFailure: string | null = null;
  /** Reasons reported while tearing down or persisting, kept for `describe()`. */
  private readonly notes: string[] = [];
  /** Keys already reported, so a per-event condition is a note rather than a log flood. */
  private readonly onceKeys = new Set<string>();
  private lastError: string | undefined;
  /**
   * Bumped by every `pause`/`resume`/`dispose` request.
   *
   * A `resume` that is overtaken by a later `pause` still finishes (the audio device and the driver
   * are already on their way), and it must not then report the session as running: the epoch it
   * captured at *request* time is compared before it clears `paused`, so a pause that arrived in
   * between keeps the flag.
   */
  private lifecycleEpoch = 0;
  /** The `resume` step currently queued or running, so a `pause` can wait for it and repay it. */
  private resumeInFlight: Promise<void> | null = null;

  constructor(host: RuntimeHost, options: RuntimeOptions) {
    this.host = host;
    this.options = options;
    this.surface = {
      width: options.surface?.width || host.surface?.width || 0,
      height: options.surface?.height || host.surface?.height || 0,
    };
  }

  /** The live session, for diagnostics and for a host that wants to reach the bridge seams. */
  get session(): ArtemisSessionCore | null {
    return this.core;
  }

  // ---------------------------------------------------------------------------------------------
  // Loading
  // ---------------------------------------------------------------------------------------------

  private bridgeSources(): ArtemisWasmSource[] {
    const sources = gameConfigWasms(this.options.gameConfig);
    // Per-game first, deployment second: a game that pins its own bridge must not silently get the
    // deployment's, and a game that names nothing must still be runnable.
    const configured = readConfiguredWasm();
    if (configured !== null) sources.push('bytes' in configured ? configured.bytes : configured.source);
    return sources;
  }

  /** Whether a bridge is published for this session. Builds nothing and clones nothing. */
  private hasBridge(): boolean {
    try {
      if (gameConfigWasms(this.options.gameConfig).length > 0) return true;
    } catch {
      // A malformed `gameConfig.wasm` is a configuration error `load()` reports in full; for the
      // purposes of a capability row it is simply not a usable bridge.
    }
    return configuredWasm !== null;
  }

  async load(): Promise<void> {
    if (this.loadPromise) return this.loadPromise;
    // Created, owned and stored in one synchronous step, so a second caller in this turn awaits the
    // same attempt instead of starting one, and a caller that drops the promise still cannot leave
    // an unhandled rejection behind.
    const started = owned(this.startLoad());
    this.loadPromise = started;
    return started;
  }

  /**
   * The actual start: services first, then the core, then `boot`.
   *
   * The order is forced by hydration. `services.attach()` reads the player's save out of the host
   * store and writes it into the filesystem, and it has to have done that **before** the guest
   * boots — a guest that started first would look for a save that no longer exists. `attach` is
   * also what binds the core, so the services exist first and the core is handed to them.
   *
   * Each resource is published on the instance in the same synchronous step that creates it. That
   * is what makes `dispose` able to reap a half-built attempt: teardown releases what is on the
   * instance at the instant it runs, so anything still held only in a local variable would be a
   * resource nobody ever releases.
   */
  private async startLoad(): Promise<void> {
    if (this.disposed) throw new ArtemisHostError('the Artemis runtime is disposed');
    const canvas = this.host.canvas;
    if (!canvas) throw new ArtemisHostError('the Artemis runtime needs a canvas from the host');
    const sources = this.bridgeSources();
    const wasm = sources[0];
    if (wasm === undefined) {
      throw new UnsupportedEngineError(
        'No Artemis/RFVP WebAssembly bridge is configured. Supply one with configureArtemisWasm() ' +
          'or a per-game `wasm` / `wasmUrl` setting.',
        'artemis',
        this.missingCapabilities(),
      );
    }
    const audioNote = this.audioNote();
    if (audioNote !== null) this.note(audioNote);

    try {
      // The services are built, assigned and published in one synchronous step, *before* the core
      // is awaited: their `runAsmConst` and `idb` seams are what the core's options name, and the
      // publication is what lets a `dispose` that arrives mid-await reap them.
      const services = createArtemisBrowserServices(this.host, canvas);
      this.services = services;
      if (this.disposed) throw new ArtemisHostError('the Artemis runtime was disposed while starting');
      const core = await createArtemisSessionCore(this.host, this.sessionOptions(canvas, services, wasm));
      // Published the instant it exists, *before* the recheck: a `dispose` that won the race has
      // already run `teardown`, which reaps whatever is on the instance — a core still held only in
      // this local would be a resource nobody ever releases. `disposed` during the await means the
      // disposer took and released this instance; nothing here may release it a second time.
      this.core = core;
      if (this.disposed) throw new ArtemisHostError('the Artemis runtime was disposed while starting');
      // Hydration runs before boot, which is why the sink is installed on the core first.
      await services.attach(core);
      if (this.disposed) throw new ArtemisHostError('the Artemis runtime was disposed while starting');
      await core.boot();
      if (this.disposed) throw new ArtemisHostError('the Artemis runtime was disposed while starting');
      // A pause that arrived while the bridge was compiling parks the guest it was waiting for,
      // instead of leaving a running guest behind a runtime that reports itself paused. It is
      // applied *before* `loaded` is set, so a load that could not park does not claim to be up.
      if (this.paused) await this.park(core, services);
      // Rechecked after the park, which is the last await before `loaded`: a dispose that landed
      // while the guest was parking has already reaped this session, and a load that then claimed
      // to be up would report a session the host released.
      if (this.disposed) throw new ArtemisHostError('the Artemis runtime was disposed while starting');
      this.loaded = true;
    } catch (error) {
      if (!this.disposed) this.loadFailure = describeError(error);
      await this.teardown(describeError(error));
      // The original value, thrown unchanged: a cleanup that failed on its own must not replace
      // the reason the load failed, and a `throw undefined` from the guest is still that throw.
      throw error;
    }
    this.log('info', 'artemis: bridge booted');
  }

  /**
   * The session's own options, with the services' browser seam passed through rather than rebuilt.
   *
   * `runAsmConst` and `idb` are the *same* objects the guest will reach: a second copy would mean
   * the guest's `EM_ASM` blocks run against a different global object and its `_emscripten_idb_*`
   * rows write into a store the runtime never flushes — a save that lands nowhere.
   */
  private sessionOptions(
    canvas: HTMLCanvasElement,
    services: ArtemisBrowserServices,
    wasm: ArtemisSessionCoreOptions['wasm'],
  ): ArtemisSessionCoreOptions {
    const config = this.options.gameConfig;
    // Built as one literal because the core's options are readonly: every field is either known here
    // or must not be set at all, and a half-built object mutated afterwards could not express that.
    const savePath = config?.['savePath'];
    const baseUrl = config?.['baseUrl'];
    const cacheBytes = config?.['cacheBytes'];
    const factory = config?.['audioContextFactory'];
    return {
      wasm,
      canvas,
      args: ['artemis'],
      runAsmConst: services.runAsmConst,
      idb: services.idb,
      // The declared device is handed to the session as its factory, so the session's own audio
      // host is the *one* `AudioContext` on the device. The declaration comes from an untyped game
      // config record, so the narrowed function is cast to the seam's exact shape; a non-function
      // declaration is inert, and `audioNote` says so rather than a silent substitution here.
      ...(typeof factory === 'function' ? { audioContextFactory: factory as () => AudioContext } : {}),
      ...(typeof savePath === 'string' ? { savePath } : {}),
      ...(typeof baseUrl === 'string'
        ? { baseUrl }
        : baseUrl instanceof URL
          ? { baseUrl: baseUrl.toString() }
          : {}),
      ...(typeof cacheBytes === 'number' && Number.isFinite(cacheBytes) ? { cacheBytes } : {}),
    };
  }

  /**
   * Why a declared `gameConfig.audioContextFactory` cannot be honoured, or `null` when it is.
   *
   * A function declaration *is* the session's audio device factory and is forwarded verbatim, so
   * there is nothing to report. Only a non-function declaration is inert, and it is named here rather
   * than silently replaced by the host's own default — the guest would otherwise play into a device
   * the page never asked for.
   */
  private audioNote(): string | null {
    const factory = this.options.gameConfig?.['audioContextFactory'];
    if (factory === undefined || factory === null) return null;
    if (typeof factory !== 'function') {
      return 'options.gameConfig.audioContextFactory is not a function and was ignored';
    }
    return null;
  }

  /**
   * Applies a `pause()` that arrived while the session was still booting.
   *
   * Both stops are *called* — and owned — before either is awaited, so the guest's loop and the
   * audio device stop in the same turn and neither rejection can be orphaned by the other's. A
   * failure is not swallowed: the flag is cleared (the guest is running, and keeping it set would
   * block every later input on a lie) and the reason is rethrown, which is what stops a load that
   * could not park from marking itself loaded.
   */
  private async park(core: ArtemisSessionCore, services: ArtemisBrowserServices): Promise<void> {
    const parked = [owned(core.pause()), owned(Promise.resolve(services.pause()))];
    try {
      await settleFirst(...parked);
    } catch (error) {
      this.paused = false;
      this.note(`pausing the freshly booted session failed: ${describeError(error)}`);
      throw error;
    }
  }

  // ---------------------------------------------------------------------------------------------
  // Lifecycle plumbing
  // ---------------------------------------------------------------------------------------------

  /** Appends to the chain and returns the tail for this step alone. */
  private chain<T>(step: () => Promise<T> | T): Promise<T> {
    const running = this.transition.then(step, step);
    this.transition = running.then(
      () => undefined,
      () => undefined,
    );
    return running;
  }

  /**
   * Releases everything the instance holds right now. Total: no step may throw out of here, because
   * a caller on its way to rethrowing the *original* failure must not have it replaced.
   *
   * Take-and-null, not "dispose once": the disposer may reap a half-built load attempt, and the
   * attempt then reaps what it published after that. Whoever takes a resource owns releasing it, so
   * each is released exactly once — never twice, and never left behind.
   */
  private async teardown(reason: string): Promise<void> {
    const core = this.core;
    const services = this.services;
    this.core = null;
    this.services = null;
    this.loaded = false;
    if (services) {
      try {
        await services.dispose();
      } catch (error) {
        this.note(`disposing the browser services failed: ${describeError(error)}`);
      }
    }
    if (core) {
      try {
        await core.dispose();
      } catch (error) {
        this.note(`disposing the session core failed: ${describeError(error)}`);
      }
    }
    if (core ?? services) this.note(`released after ${reason}`);
  }

  private log(level: LogLevel, message: string): void {
    try {
      this.host.log(level, message);
    } catch {
      // A host whose logger throws must not turn a diagnosis into a failure.
    }
  }

  private note(message: string): void {
    this.lastError = message;
    if (this.notes.length < 64) this.notes.push(message);
    this.log('error', `artemis: ${message}`);
  }

  /** Reports a repeating condition once. A note per event would be a log flood, not a diagnosis. */
  private noteOnce(key: string, message: string): void {
    if (this.onceKeys.has(key)) return;
    this.onceKeys.add(key);
    this.note(message);
  }

  /**
   * Parks the guest in this turn, not in the next one.
   *
   * `core.pause()` cancels the pending native frame, bumps the session's loop epoch and suspends the
   * audio device *before* it awaits anything, so the stop has to be issued here rather than from
   * inside the chain: a pause that only queued work would leave the guest rendering one more frame
   * after the host asked it to stop. The chain then owns the two things a synchronous call cannot —
   * the video element's own pause, and the repayment described below.
   */
  pause(): void {
    this.paused = true;
    this.lifecycleEpoch += 1;
    const core = this.core;
    const overtaken = this.resumeInFlight;
    if (core) {
      // Started and owned in this turn: a rejection is reported rather than left unhandled, and the
      // guest's loop stops while the audio device stops.
      try {
        void owned(core.pause()).catch((error: unknown) =>
          this.note(`pause failed: ${describeError(error)}`),
        );
      } catch (error) {
        this.note(`pause failed: ${describeError(error)}`);
      }
    }
    // The chain's tail is owned immediately, so a rejected step is reported rather than unhandled.
    void this.chain(async () => {
      if (this.disposed) return;
      const session = this.core;
      // Nothing is running: the flag alone parks the input and the next boot.
      if (!session) return;
      try {
        await this.services?.pause();
        if (overtaken) {
          // A resume was already in flight when the host paused. Its `context.resume()` may land
          // after the suspend above — which therefore saw a 'suspended' device and did nothing — so
          // the device would be left running under a paused session. Waiting for that resume and
          // stopping again is the only place that can repay it.
          await overtaken.catch(() => undefined);
          await session.pause();
        }
      } catch (error) {
        this.note(`pause failed: ${describeError(error)}`);
      }
    }).catch((error: unknown) => this.note(`pause chain failed: ${describeError(error)}`));
  }

  resume(): void {
    const epoch = this.lifecycleEpoch;
    const step = this.chain(async () => {
      // Never a revival: a session that failed to load, or that has been disposed, has nothing to
      // resume, and clearing the flag there would re-open input to a guest that is not running.
      if (this.disposed || !this.loaded) return;
      const core = this.core;
      const services = this.services;
      if (!core) return;
      if (this.sessionFailed()) {
        // A session that failed terminally stays failed: resuming would only ask a dead guest to run
        // again. Parking it here is what cancels the frame the failure left queued, so the host is
        // left with a stopped loop rather than one that is about to enter a guest that cannot run.
        this.noteOnce('resume-after-failure', 'resume was refused: the session failed terminally');
        try {
          void owned(core.pause()).catch(() => undefined);
        } catch {
          // Already reported by the pause path; a refused resume is not an exception for the caller.
        }
        return;
      }
      try {
        if (services) await services.resume();
        await core.resume();
        // Only if the host has not asked for a pause since: a resume that lands after one must not
        // clear the flag that pause set, or input would reopen into a stopped guest.
        if (this.lifecycleEpoch === epoch) this.paused = false;
      } catch (error) {
        // The flag stays set: reporting a guest that did not resume as running would unblock input
        // into a loop that is still stopped.
        this.note(`resume failed: ${describeError(error)}`);
      }
    });
    this.resumeInFlight = step;
    const settled = (): void => {
      if (this.resumeInFlight === step) this.resumeInFlight = null;
    };
    // Both handlers, so waiting on an overtaken resume can never surface as an unhandled rejection.
    void step.then(settled, settled);
    void step.catch((error: unknown) => this.note(`resume chain failed: ${describeError(error)}`));
  }

  dispose(): void {
    const started = this.disposePromise;
    if (started) {
      void started.catch(() => undefined);
      return;
    }
    this.disposed = true;
    this.lifecycleEpoch += 1;
    this.paused = true;
    const core = this.core;
    const services = this.services;
    // Both stops are issued — and owned — in this turn, before anything is awaited, for the same
    // reason `pause` is: a dispose that returned with a frame still queued, or a video still
    // uploading, would let the guest run once more after the host released it. The chain pauses
    // again below, which is what awaits the drain the terminal save needs.
    if (services) {
      try {
        services.pause();
      } catch (error) {
        this.note(`pause before dispose failed: ${describeError(error)}`);
      }
    }
    if (core) {
      try {
        void owned(core.pause()).catch((error: unknown) =>
          this.note(`pause before dispose failed: ${describeError(error)}`),
        );
      } catch (error) {
        this.note(`pause before dispose failed: ${describeError(error)}`);
      }
    }
    const running = this.chain(async (): Promise<ArtemisDisposalReport> => {
      let saved = false;
      let failure: string | undefined;
      const core = this.core;
      const services = this.services;
      try {
        if (core) {
          try {
            await core.pause();
          } catch (error) {
            failure = `pausing before the terminal save failed: ${describeError(error)}`;
          }
        }
        if (services) {
          try {
            await services.flush();
            saved = true;
          } catch (error) {
            failure = `the terminal save failed: ${describeError(error)}`;
          }
        } else {
          failure = 'nothing was loaded, so there was nothing to save';
        }
      } finally {
        // One teardown for every path out of this runtime.
        await this.teardown('dispose');
      }
      if (failure !== undefined) this.note(failure);
      else if (saved) this.log('debug', 'artemis: terminal save written');
      return failure === undefined ? { saved } : { saved, failure };
    });
    this.disposePromise = running;
    void running.catch((error: unknown) => this.note(`dispose failed: ${describeError(error)}`));
  }

  /** Test seam: resolves with what the teardown managed to persist. Never rejects. */
  whenDisposed(): Promise<ArtemisDisposalReport> {
    const started = this.disposePromise;
    if (!started) return Promise.resolve({ saved: false, failure: 'the runtime was not disposed' });
    const report = started.then(
      (value) => value,
      (error: unknown) => ({ saved: false, failure: `dispose failed: ${describeError(error)}` }),
    );
    // Joined with the attempt that may still be in flight. A load that is mid-construction owns
    // resources `dispose` cannot see yet — a disposer can only reap what is published — so settling
    // disposal before that attempt has settled would hand the host a runtime whose factory objects
    // are still being built behind it. The attempt's own failure path reaps them.
    const load = this.loadPromise;
    if (!load) return report;
    return load.then(
      () => report,
      () => report,
    );
  }

  /**
   * No-op by design.
   *
   * The guest's own GLFW frame loop is installed by the bridge and drives itself through the view's
   * `requestAnimationFrame`; the session's own `requestFrame` is a *repaint* request, not a
   * scheduling one. A host that stepped the guest from here would be a second clock fighting the
   * first, which is exactly how a port ends up double-ticking its guest.
   */
  tick(): void {}

  /** Asks the host for a repaint. Best effort: a refused paint is not a dead session. */
  render(): void {
    if (this.disposed) return;
    try {
      this.host.requestFrame();
    } catch (error) {
      this.note(`the host refused a repaint: ${describeError(error)}`);
    }
  }

  // ---------------------------------------------------------------------------------------------
  // Input
  // ---------------------------------------------------------------------------------------------

  /**
   * Delivers input to the targets the guest actually listens on.
   *
   * A normalised event carries game coordinates; the DOM wants client pixels, so the one scaling
   * from logical size to the canvas box happens here and nowhere else. Pointer events go to the
   * canvas, keys to the document (which is where the engine registers its listeners) — one target
   * per event, so a listener installed on both does not fire twice.
   */
  private deliver(event: InputEvent): void {
    const canvas = this.host.canvas;
    const doc = canvas.ownerDocument ?? null;
    const view = doc?.defaultView ?? null;
    if (!view) throw new ArtemisHostError('the Artemis runtime needs a view to deliver input to');
    switch (event.type) {
      case 'pointer-down':
      case 'pointer-up':
      case 'pointer-move': {
        const point = this.toClient(canvas, event.x, event.y);
        // A coordinate that is not a number is refused, not fabricated into (0, 0).
        if (point === null) return;
        if (event.type === 'pointer-move') {
          // A move changes no button; `buttons` carries the held-button mask it moved under.
          this.dispatchMouse(canvas, view, 'mousemove', point, undefined, event.pressed ? 1 : 0);
          return;
        }
        // The button travels on *both* edges. A `mouseup` that reported no button would read to the
        // guest as the left button being released, whatever the player actually let go of.
        const down = event.type === 'pointer-down';
        this.dispatchMouse(
          canvas,
          view,
          down ? 'mousedown' : 'mouseup',
          point,
          event.button,
          down ? buttonMask(event.button) : 0,
        );
        return;
      }
      case 'pointer-wheel': {
        const point = this.toClient(canvas, event.x, event.y);
        if (point === null) return;
        this.dispatchWheel(canvas, view, point, event.deltaY);
        return;
      }
      case 'key-down':
      case 'key-up': {
        // A text field that has focus owns the keyboard. The engine registers its listeners on
        // `document`, which the host shell shares, so delivering anyway would take the keys the
        // field — the host's own UI, or the game's text entry — was about to receive.
        if (editableFocus(doc) !== null) {
          this.noteOnce(
            'keyboard-owned-by-field',
            'keyboard input was not delivered: a text field in this document has focus',
          );
          return;
        }
        // Dispatched at the document and allowed to bubble: dispatching at the canvas instead would
        // never reach a document listener.
        const target = (doc ?? canvas) as unknown as EventTarget;
        const init = keyInit(event.key, event.code, event.type === 'key-down' ? event.repeat : undefined);
        const type = event.type === 'key-down' ? 'keydown' : 'keyup';
        this.dispatchKeyboard(target, view, type, init);
        return;
      }
      case 'text': {
        const target = editableFocus(doc);
        if (!target) {
          this.noteOnce(
            'text-no-target',
            'text input was not delivered: no focused input, textarea or contenteditable element',
          );
          return;
        }
        try {
          const input = target as unknown as { value: string; dispatchEvent(event: Event): boolean };
          input.value = event.value;
          input.dispatchEvent(new (view as unknown as { Event: typeof Event }).Event('input', { bubbles: true }));
        } catch (error) {
          this.note(`text input failed: ${describeError(error)}`);
        }
        return;
      }
      case 'blur': {
        const target = (doc ?? view) as unknown as EventTarget;
        try {
          target.dispatchEvent(new (view as unknown as { Event: typeof Event }).Event('blur'));
        } catch (error) {
          this.note(`blur dispatch failed: ${describeError(error)}`);
        }
        return;
      }
      default:
        this.noteOnce(
          `unknown:${String((event as { type: string }).type)}`,
          `input of type ${String((event as { type: string }).type)} is not delivered by this runtime`,
        );
    }
  }

  /**
   * The logical surface the coordinates are expressed in.
   *
   * The canvas's own backing-store size is the bridge's window size once `_glfwCreateWindow` has
   * run, and the host's requested surface before that. One scaling follows from it — logical game
   * pixels to the canvas's client box — so a pointer at game (x, y) lands on the pixel the guest
   * drew there.
   */
  private logicalSize(canvas: HTMLCanvasElement): { width: number; height: number } {
    return {
      width: canvas.width || this.surface.width,
      height: canvas.height || this.surface.height,
    };
  }

  /**
   * Logical game pixels to client pixels, or `null` when the input cannot be placed.
   *
   * Masked on purpose: the coordinate is clamped into the logical surface and a non-finite one is
   * refused, so a hostile or broken caller cannot hand the guest a `NaN` rectangle to hit-test, and
   * a drag that leaves the canvas cannot address a pixel outside the game's own frame.
   */
  private toClient(canvas: HTMLCanvasElement, x: number, y: number): { x: number; y: number } | null {
    if (!Number.isFinite(x) || !Number.isFinite(y)) {
      this.noteOnce('pointer-nonfinite', 'a pointer event with a non-finite coordinate was refused');
      return null;
    }
    const logic = this.logicalSize(canvas);
    // No logical surface to scale against: the game pixels are the best available answer.
    if (!(logic.width > 0) || !(logic.height > 0)) return { x, y };
    const clampedX = Math.min(Math.max(x, 0), logic.width);
    const clampedY = Math.min(Math.max(y, 0), logic.height);
    let box: { left: number; top: number; width: number; height: number };
    try {
      box = canvas.getBoundingClientRect?.() ?? { left: 0, top: 0, width: 0, height: 0 };
    } catch {
      box = { left: 0, top: 0, width: 0, height: 0 };
    }
    const clientX = box.left + (clampedX / logic.width) * box.width;
    const clientY = box.top + (clampedY / logic.height) * box.height;
    // A box that is itself not finite (a detached canvas mid-layout) falls back to game pixels
    // rather than dispatching `NaN`.
    if (!Number.isFinite(clientX) || !Number.isFinite(clientY)) return { x: clampedX, y: clampedY };
    return { x: clientX, y: clientY };
  }

  private dispatchMouse(
    canvas: HTMLCanvasElement,
    view: Window & typeof globalThis,
    type: string,
    point: { x: number; y: number },
    button: number | undefined,
    buttons: number | undefined,
  ): void {
    this.fire(canvas, view, type, (EventCtor) => {
      const init: MouseEventInit = { bubbles: true, cancelable: true, clientX: point.x, clientY: point.y };
      // The button and the held-button mask are separate fields, and both are set from the caller's
      // own numbers: a down carries its mask, an up carries the empty mask it left behind.
      if (button !== undefined) init.button = button;
      if (buttons !== undefined) init.buttons = buttons;
      return new EventCtor(type, init);
    });
  }

  private dispatchWheel(
    canvas: HTMLCanvasElement,
    view: Window & typeof globalThis,
    point: { x: number; y: number },
    deltaY: number,
  ): void {
    this.fire(canvas, view, 'wheel', (EventCtor) => {
      const init = {
        bubbles: true,
        cancelable: true,
        clientX: point.x,
        clientY: point.y,
        deltaY,
        deltaMode: 0,
      } as unknown as MouseEventInit;
      return new EventCtor('wheel', init);
    });
  }

  private dispatchKeyboard(
    target: EventTarget,
    view: Window & typeof globalThis,
    type: string,
    init: { key: string; code: string; keyCode: number; which: number; repeat?: boolean },
  ): void {
    const registrar = view as unknown as {
      KeyboardEvent: new (type: string, init: Record<string, unknown>) => KeyboardEvent;
    };
    try {
      const payload: Record<string, unknown> = {
        bubbles: true,
        cancelable: true,
        key: init.key,
        code: init.code,
      };
      if (init.repeat !== undefined) payload['repeat'] = init.repeat;
      const event = new registrar.KeyboardEvent(type, payload);
      // Emscripten's key block reads `event.keyCode` and `event.which`; both are legacy read-only
      // accessors, so they are defined on this instance rather than passed to the constructor.
      Object.defineProperty(event, 'keyCode', { value: init.keyCode, configurable: true });
      Object.defineProperty(event, 'which', { value: init.which, configurable: true });
      target.dispatchEvent(event);
    } catch (error) {
      this.note(`keyboard dispatch failed: ${describeError(error)}`);
    }
  }

  /** Constructs a DOM event from the target's own realm and dispatches it. Never injects markup. */
  private fire(
    target: EventTarget,
    view: Window & typeof globalThis,
    type: string,
    build: (Ctor: new (type: string, init?: EventInit) => Event) => Event,
  ): void {
    const handle = view as unknown as {
      Event: typeof Event;
      MouseEvent: typeof MouseEvent;
      WheelEvent: typeof WheelEvent;
    };
    try {
      const Ctor =
        type === 'wheel' && typeof handle.WheelEvent === 'function'
          ? handle.WheelEvent
          : typeof handle.MouseEvent === 'function'
            ? handle.MouseEvent
            : handle.Event;
      target.dispatchEvent(build(Ctor as unknown as new (type: string, init?: EventInit) => Event));
    } catch (error) {
      this.note(`input dispatch failed: ${describeError(error)}`);
    }
  }

  sendInput(event: InputEvent): void {
    // A terminal, paused or never-loaded runtime must not enter the guest: the guest's loop is
    // stopped (or was never started) and a dispatched event would be queued against a canvas that
    // is not listening. A load that failed leaves `loaded` false, which is the same refusal.
    if (this.disposed || this.paused || !this.loaded) {
      this.noteOnce(
        'input-refused',
        `input was not delivered: ${
          this.disposed ? 'the runtime is disposed' : this.paused ? 'the session is paused' : 'the session has not loaded'
        }`,
      );
      return;
    }
    try {
      this.deliver(event);
    } catch (error) {
      this.note(`input delivery failed: ${describeError(error)}`);
    }
  }

  // ---------------------------------------------------------------------------------------------
  // Save & restore
  // ---------------------------------------------------------------------------------------------

  /**
   * A synchronous save, which is exactly why it can be refused.
   *
   * The contract is synchronous and the guest's filesystem is only coherent between units of work,
   * so a save taken while the driver is parked mid-call would capture half a write. The honest
   * answer there is a refusal, and the refusal is what makes a successful save mean something.
   *
   * What lands in the slot is the save root and the IndexedDB mirror — the two places the guest
   * persists to — and nothing else: the game tree is read-only, and the guest's own memory image is
   * not in here, so a slot that implied otherwise would be lying. It is a durability point for the
   * filesystem and the idb mirror; it is **not** a VM continuation, and a caller that needs the
   * guest's own memory image has to use the session's own capture.
   */
  save(): SaveState {
    const core = this.core;
    const services = this.services;
    const failure = (caption: string): SaveState => ({
      version: ARTEMIS_SAVE_VERSION,
      data: { kind: ARTEMIS_SAVE_KIND, refused: caption },
      caption,
    });
    if (this.disposed) return failure('ArtemisEngine 会话已释放，未保存');
    if (!this.loaded || !core || !services) return failure('ArtemisEngine 会话尚未启动，未保存');
    // A parked driver is busy too: `busy` covers the unwind/rewind of an asyncify call that has not
    // landed, and a snapshot taken there would be half a guest write.
    if (core.driver.busy || core.driver.inGuestDepth > 0) {
      return failure('引擎正忙（asyncify 调用未落地），未保存');
    }
    try {
      // The canonical mount the services hydrated, never the game's raw spelling of it: `gameone/`
      // and `/save/game/` both mean one directory, and looking up the raw string finds none of them
      // — which is how a save silently becomes an empty slot.
      const root = saveRootOf(core);
      if (root === null) {
        const caption = `ArtemisEngine 存档目录 ${core.assets.savePath || '(未配置)'} 未挂载，未保存`;
        this.note(caption);
        return failure(caption);
      }
      // `createArtemisBrowserServices` hands back the browser mirror, whose entry list is what a
      // snapshot can actually carry. A foreign `idb` cannot be enumerated from here, and a save
      // that dropped the player's IndexedDB records without saying so would be the worse answer.
      if (!(services.idb instanceof ArtemisBrowserIdb)) {
        const caption = 'ArtemisEngine 的 IndexedDB 镜像不是本运行时的实现，未保存';
        this.note(caption);
        return failure(caption);
      }
      const files = snapshotSaveRoot(root);
      const snapshot: ArtemisBrowserSnapshot = {
        version: ARTEMIS_BROWSER_SNAPSHOT_VERSION,
        files,
        idb: services.idb.mirrorEntries(),
      };
      const caption = `ArtemisEngine 存档：${files.length} 个文件 + ${snapshot.idb.length} 条 IndexedDB 记录（保存目录文件与 IDB，不含引擎内存快照）`;
      return { version: ARTEMIS_SAVE_VERSION, data: { kind: ARTEMIS_SAVE_KIND, snapshot }, caption };
    } catch (error) {
      const message = `ArtemisEngine 快照失败：${describeError(error)}`;
      this.note(message);
      return failure(message);
    }
  }

  async restore(state: SaveState): Promise<void> {
    if (this.disposed) throw new ArtemisHostError('the Artemis runtime is disposed');
    const services = this.services;
    if (!this.loaded || !services) {
      throw new ArtemisHostError('the Artemis runtime has not loaded a session to restore into');
    }
    // The *outer* document is checked too, not just `data`: a state written by another runtime or
    // another version of this one is a foreign document, and applying it would be a silent
    // downgrade. `readSaveData` then checks the kind, snapshot version, files, idb, paths and clones.
    if (typeof state !== 'object' || state === null) {
      throw new ArtemisHostError('artemis save state is not an object');
    }
    if (state.version !== ARTEMIS_SAVE_VERSION) {
      throw new ArtemisHostError(`artemis save state version ${String(state.version)} is not supported`);
    }
    const snapshot = readSaveData(state.data);
    // `services.restore` owns the queue: it validates and clones the snapshot, then applies it on the
    // guest's own unit of work. Wrapping it in `core.enqueue` again would wait for the queue from
    // *inside* the queue — a deadlock, not a safety margin.
    await services.restore(snapshot);
    // A restore is only a restore once the store holds it: a successful in-memory swap that was
    // never written back would be undone by the next load, which is the worst of both answers.
    await services.flush();
  }

  // ---------------------------------------------------------------------------------------------
  // Diagnostics
  // ---------------------------------------------------------------------------------------------

  /** The session's own diagnostics, or `null`. Never throws: a diagnosis may not become a failure. */
  private sessionDiagnostics(): ArtemisSessionDiagnostics | null {
    try {
      return this.core?.diagnostics() ?? null;
    } catch {
      return null;
    }
  }

  /**
   * Whether the live session failed terminally.
   *
   * By the *presence* of the key, not its value: session-core reports a terminal failure that way
   * because `throw undefined` is a legal thrown value, and `failure === undefined` cannot tell such
   * a session apart from a healthy one.
   */
  private sessionFailed(): boolean {
    const session = this.sessionDiagnostics();
    return session !== null && 'failure' in session;
  }

  /**
   * Whether this instance could start a session right now.
   *
   * Not a constant: a configured bridge is runnable until an attempt fails, and a failure is
   * observed state that takes it back down. `artemisProvider.availability` answers the
   * deployment-wide question before an instance exists; this answers it for one instance.
   */
  private available(): boolean {
    if (this.disposed) return false;
    if (!this.hasBridge()) return false;
    if (this.loadFailure !== null) return false;
    // A session that died terminally is not available either, even though a bridge is still
    // published: that failure is observed state, and the key's presence is how session-core reports
    // it when the guest threw `undefined`.
    return !this.sessionFailed();
  }

  /**
   * The deployment rows this instance has *not* satisfied, recomputed on every call.
   *
   * Nothing here is asserted from a table: the bridge row follows the publication, the canvas row
   * follows the one observation that proves a WebGL context existed (the guest entering its frame
   * loop — probing `getContext` here would create the very context the guest must own), and the
   * audio row follows the two halves that can satisfy it — a factory the deployment declared, or a
   * page that has WebAudio at all.
   */
  private missingCapabilities(): readonly string[] {
    const rows: string[] = [];
    if (!this.hasBridge()) rows.push(ARTEMIS_MISSING_BRIDGE);
    if (this.sessionDiagnostics()?.mainLoopStarted !== true) rows.push(ARTEMIS_MISSING_CANVAS);
    // The audio seam has two halves and either one satisfies it: a factory the deployment declared,
    // or a page that has WebAudio at all. Both are read, never constructed.
    if (!this.declaredAudioFactory() && !pageHasWebAudio(this.host.canvas)) {
      rows.push(ARTEMIS_MISSING_AUDIO);
    }
    return rows;
  }

  /** Whether `gameConfig.audioContextFactory` declared a device factory this runtime forwards. */
  private declaredAudioFactory(): boolean {
    return typeof this.options.gameConfig?.['audioContextFactory'] === 'function';
  }

  /** What is known about the audio seam, including the exact reason a declared factory is inert. */
  private audioDiagnostics(): Readonly<Record<string, unknown>> {
    const factory = this.options.gameConfig?.['audioContextFactory'];
    const declared = factory !== undefined && factory !== null;
    return {
      host: ARTEMIS_AUDIO_HOST_WIRED ? 'wired' : 'not-wired',
      declared: declared ? (typeof factory === 'function' ? 'function' : 'not-a-function') : 'unset',
      seam: ARTEMIS_AUDIO_SEAM,
    };
  }

  describe(): Readonly<Record<string, unknown>> {
    const core = this.core;
    const session = this.sessionDiagnostics();
    // Read once, from the presence of the key: both the report and the diagnostic below have to
    // answer the same question about the same session, and a second reader could answer differently.
    const sessionFailed = session !== null && 'failure' in session;
    const trace = core?.trace;
    const idb = this.services?.idb;
    return {
      available: this.available(),
      missingCapabilities: this.missingCapabilities(),
      coldGameUnverified: true,
      loaded: this.loaded,
      disposed: this.disposed,
      paused: this.paused,
      surface: this.surface,
      wasm: configuredWasm === null ? 'unconfigured' : 'configured',
      generation: configuredGeneration,
      failure: this.loadFailure,
      audio: this.audioDiagnostics(),
      engine: {
        instantiated: session?.instantiated ?? false,
        wired: session?.wired ?? false,
        mainLoopStarted: session?.mainLoopStarted ?? false,
        disposed: session?.disposed ?? false,
        buckets: session?.buckets ?? {},
        unserved: session?.unserved ?? [],
        // The key's *presence*, not its value. `throw undefined` is a legal terminal failure and
        // session-core reports one that way precisely so such a session is distinguishable from a
        // healthy one, so reading the value here would drop the only evidence there is: `undefined`
        // is what both a healthy session and an `undefined` failure look like.
        hasFailure: sessionFailed,
        // The *value* is published only when there is a failure to publish. Formatting through
        // `describeError` is what makes a terminal `throw undefined` readable — it names the literal
        // rather than dropping the failure — but it must not be applied to a healthy session, whose
        // diagnostics carry no `failure` key at all: `describeError(undefined)` answers the string
        // 'undefined', and a healthy engine reporting a failure named 'undefined' is a lie that also
        // contradicts `hasFailure` right above it. `'failure' in session` is the one honest test —
        // the key's presence, because `throw undefined` is a legal terminal failure, while a healthy
        // session's absent key and an `undefined` failure share the value `undefined` and cannot be
        // told apart by it.
        failure: sessionFailed ? describeError(session?.failure) : undefined,
      },
      trace: trace
        ? {
            blocked: trace.blocked ?? [],
            steps: 'steps' in trace ? trace.steps : undefined,
            // Both measurements, kept distinct because they answer different questions and
            // `CompatibilityReport` (engine-api, closed interface) has room for only one of them:
            // `observedImportCount` is how many distinct short names the boot ever *entered* —
            // refused ones included — and `honouredImportCount` is the subset whose synchronous body
            // completed without reporting. A guest that did nothing and a guest whose every request
            // was refused both report `supportedCommandCount: 0`; only these two tell them apart.
            observedImportCount: trace.counts?.size ?? 0,
            honouredImportCount: trace.honoured?.size ?? 0,
          }
        : null,
      // Kept, but it is *not* a bucket list and must not be read as one: it is the four callback
      // signature strings (`vi`/`vii`/`viii`/`viiii`) `invokeCallback` dispatches and refuses
      // everything else against. The count of commands actually honoured lives in `report()`, which
      // measures the guest; deriving one from this table is the defect this key used to feed.
      buckets: [...ARTEMIS_CALLBACK_SIGNATURES],
      errors: this.notes,
      state: {
        savePath: core?.assets.savePath ?? null,
        saveMountPath: core ? canonicalMount(core.assets.savePath) : null,
        // A name for the idb seam, not a count: enumerating the mirror clones every record, and a
        // diagnostics call must not copy a player's save to answer a question about it.
        idb: idb === undefined ? 'none' : idb instanceof ArtemisBrowserIdb ? 'artemis-browser' : 'foreign',
        loaded: this.loaded,
      },
    };
  }

  /**
   * What this session actually hit, not a zeroed summary.
   *
   * Every blocked call the bridge recorded is reported as a skipped command, because that is
   * literally what it is: the guest asked for an import this build could not serve and the frame
   * went on without it. A session that has not loaded is `unsupported` — not `clean` — since nothing
   * has been observed to be honoured.
   */
  report(): CompatibilityReport {
    const trace = this.core?.trace;
    const blocked = trace?.blocked ?? [];
    const unsupported: UnsupportedCommand[] = blocked.map((entry) => ({
      command: entry.official || entry.name,
      count: 1,
      handling: 'skipped' as const,
      note: `bucket ${entry.bucket}`,
    }));
    const session = this.sessionDiagnostics();
    // `'failure' in session`, not `session.failure !== undefined`: `throw undefined` is a legal
    // terminal failure, and session-core reports one by the *presence* of the key for exactly that
    // reason. Reading the value would call a dead session clean — the one answer a report must never
    // give.
    if (session !== null && 'failure' in session) {
      unsupported.push({
        command: 'artemis:session',
        count: 1,
        handling: 'skipped' as const,
        note: describeError(session.failure),
      });
    }
    if (!this.loaded) {
      // Only while nothing has been observed. Once a session is up, the rows the guest *actually*
      // hit are the ones the trace recorded, and a static list would outrank that evidence.
      for (const capability of this.missingCapabilities()) {
        unsupported.push({
          command: capability,
          count: 1,
          handling: 'skipped' as const,
          note: 'supplied by the deployment, not by this runtime',
        });
      }
    }
    return {
      runtimeId: this.runtimeId,
      severity: unsupported.length > 0 ? 'unsupported' : 'clean',
      unsupported,
      approximated: [],
      // The distinct imports this session actually served, and **only** that. The count used to be
      // `ARTEMIS_CALLBACK_SIGNATURES.length` gated on `wired`, which counts four *callback signature
      // strings* the guest may never have named: a session that had served nothing reported 4, and
      // the number moved when the signature table changed rather than when the guest did anything.
      // `trace.honoured` is the measured set — each member is a short name whose synchronous import
      // body completed without a report or a refusal during a call — so this is now observed, not
      // advertised. `honoured.size`, not `counts.size`: `counts` also holds every *stubbed* row the
      // guest touched, and those were refused rather than honoured.
      supportedCommandCount: trace?.honoured.size ?? 0,
    };
  }
}

/**
 * ArtemisEngine provider.
 *
 * Priority 85 places it below Siglus and ONScripter (90) and above the web runtimes: an Artemis
 * directory that also ships an `index.html` should resolve to Artemis, while a directory carrying
 * both Artemis and Siglus markers resolves by score first.
 *
 * The description states what this runtime *is* — an independently hosted asyncify bridge, not an
 * SDK page, an iframe, or an `eval`ed glue script — and says nothing about having been tested
 * against a particular title.
 */
export const artemisProvider: RuntimeProvider = {
  runtimeId: 'artemis',
  displayName: 'ArtemisEngine',
  priority: 85,
  description:
    'ArtemisEngine：compat 源码构建（classic worker + wasm，主线程 tick 驱动 artc_web_frame）运行时；默认可运行与否取决于是否已通过 configureArtemisCompat() 指定引擎，游戏自带 wasm 时回落到 legacy asyncify 桥接诊断路径。引擎与具体作品的兼容性未经测试。',
  accepts: ['root.pfs', 'system.ini', 'boot.ini', 'system/first.iet', '.pfs', '.iet'],
  containers: ['pfs'],
  optionKeys: [
    OPTION_SCRIPT_ENCODING,
    OPTION_MEMORY_PROFILE,
    OPTION_STRICT_MODE,
    OPTION_COMPATIBILITY_REPORT,
  ],

  // A dynamic getter, not a literal: an embedder can call `configureArtemisCompat()` after the
  // provider was constructed, and a frozen `availability` would keep advertising `unavailable` for a
  // runtime that is now able to run. It answers two separate questions and both must hold — an engine
  // is named, **and** the page can actually serve the three things the transport needs. A
  // SharedArrayBuffer that cannot be shared means no blocking read, so no pack, so no game.
  get availability(): RuntimeProvider['availability'] {
    if (!hasArtemisCompatEngine()) return 'unavailable';
    return compatBrowserReady() ? 'runnable' : 'unavailable';
  },

  async probe(game: ProbeContext): Promise<ProbeResult | null> {
    return artemisProbeResult(game);
  },

  /**
   * A compat session, unless the game explicitly asked for the legacy bridge.
   *
   * The legacy `ArtemisRuntime` is still what runs when `gameConfig.wasm` / `wasmUrl` names a bridge.
   * That is deliberately the *only* way to reach it now: the compat engine is the default because it
   * is the engine this project is moving to, and the asyncify bridge is a diagnostic for comparing
   * against it — not a silent fallback behind it.
   */
  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance {
    return legacyBridgeRequested(options) ? new ArtemisRuntime(host, options) : new CompatArtemisRuntime(host, options);
  },
};

/** Whether this game asked for the legacy asyncify bridge by name. */
function legacyBridgeRequested(options: RuntimeOptions): boolean {
  const config = options.gameConfig;
  if (!config) return false;
  const wasm = config['wasm'];
  if (wasm !== undefined && wasm !== null) return true;
  const url = config['wasmUrl'];
  return url !== undefined && url !== null;
}

/**
 * Whether this page can run a compat session at all.
 *
 * The three rows are the transport's own requirements, read without constructing anything: a shared
 * window it can block on, an `OffscreenCanvas` to transfer, and a game source that can read ranges.
 * A page missing one of them is `unavailable` — the honest answer — rather than a play button that
 * fails on click.
 */
function compatBrowserReady(): boolean {
  const scope = globalThis as {
    crossOriginIsolated?: boolean;
    SharedArrayBuffer?: unknown;
    OffscreenCanvas?: unknown;
  };
  if (scope.crossOriginIsolated !== true) return false;
  if (typeof scope.SharedArrayBuffer !== 'function') return false;
  return typeof scope.OffscreenCanvas === 'function';
}

export type { ArtemisSyncIdb };
