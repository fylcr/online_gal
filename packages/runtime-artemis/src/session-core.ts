/**
 * The independent asyncify session core.
 *
 * Everything under `host/` is a *piece*: an import table, a driver, an allocator, a filesystem. This
 * module is the thing that owns all of them at once, against one `WebAssembly.Memory`, and answers the
 * only question that matters to a browser host — "is this game running, and how do I stop it".
 *
 * The design decisions, in the order they constrain everything else:
 *
 * **The guest is instantiated with our memory, not the glue's.** The bridge imports `a.a`, so the
 * `WebAssembly.Memory` is ours; the instance cannot disagree with us about growth because there is one
 * memory. That is why {@link ARTEMIS_ARENA_PAGES} is the bridge's own shape (3072/65536) rather than a
 * guess, and why `ArtemisMemory` takes a memory instead of making one.
 *
 * **The queue is the whole concurrency story.** An asyncify guest is single-flight: one root call, one
 * control block, one state global. Every entry into the guest — the boot, a frame, an event — goes
 * through {@link ArtemisSessionCore.enqueue}, which waits for the driver to go idle *before* the task
 * rather than after it, so a queued unit can never start on top of a parked call. The alternative (a
 * promise chain that only serialises its own tails) lets a rAF tick and an event handler overlap,
 * which is exactly the re-entrancy `ArtemisSuspendDriver.wrap` refuses.
 *
 * **The import surface comes from the module, not from a list.** {@link buildSpec} reads
 * `WebAssembly.Module.imports()` and matches every entry against the **short** names the six buckets
 * key on. A function row we cannot place is a **throw**, not a zero: a stub zero is how a stalled
 * engine looks identical to a working one.
 *
 * **Nothing here claims what it cannot do.** Audio is real: the session builds the WebAudio host in
 * `host/audio.ts` around this same memory and allocator, and hands its nineteen rows to the `gl`
 * bucket as one table. That host is lazy in effect — constructing it allocates nothing and creates
 * no `AudioContext`; the context appears only when the guest calls `_alcOpenDevice` /
 * `_alcCreateContext` — so a page with no audio still instantiates and refuses at the guest's first
 * audio call instead of pretending to play sound. There is no media element, and no persistence
 * beyond the filesystem and the optional IndexedDB seam. `pause` and `resume` drive the GL lifecycle
 * and the audio context; `dispose` is terminal. A row that is refused is reported rather than
 * answered.
 */

import type { RuntimeHost } from '@online-gal/engine-api';
import { ArtemisHostError } from './host/errno.js';
import { ArtemisMemory } from './host/memory.js';
import { ArtemisSyncIdb, type ArtemisFdHost } from './host/fd.js';
import {
  ARTEMIS_IMPORT_MODULE,
  ARTEMIS_FD_OFFICIAL,
  initArtemisStandardStreams,
  mountArtemisTree,
} from './host/bridge.js';
import { ArtemisFs } from './host/tree.js';
import type { ArtemisAsmConstRunner, ArtemisAsmConst } from './host/runtime.js';
import { ARTEMIS_RUNTIME_OFFICIAL } from './host/runtime.js';
import { ArtemisSuspendDriver, type ArtemisSuspendingFunction } from './host/suspend.js';
import { ARTEMIS_ASYNC_OFFICIAL } from './host/async.js';
import { ARTEMIS_ASYNCIFY_OFFICIAL } from './host/asyncify.js';
import { ARTEMIS_EH_OFFICIAL } from './host/eh.js';
import {
  ARTEMIS_GL_OFFICIAL,
  type ArtemisGlCanvas,
  type ArtemisGlHost,
  type ArtemisGlLifecycle,
} from './host/gl.js';
import { createArtemisAudioHost, type ArtemisAudioHost } from './host/audio.js';
import {
  createArtemisBootTable,
  type ArtemisBootResult,
  type ArtemisBootSetup,
  type ArtemisBootTable,
  type ArtemisImportRow,
  type ArtemisImportSpec,
} from './host/boot.js';
import type { ArtemisAssets } from './assets.js';

/** The memory shape the bridge declares: `a.a`, 3072 pages now, 65536 pages at most. */
export const ARTEMIS_ARENA_PAGES = { initial: 3072, maximum: 65536 } as const;

/**
 * Guest *exports* this session must have, by the short name the glue's own `asm` object uses.
 *
 * These are not imports and are not in the committed map: they are the bridge's own exports, named in
 * `artemis.js` (`Module["asm"]["Nc"]` is `_malloc`, `Ud` is `_asyncify_start_unwind`). Read out of the
 * shipped binary rather than inferred.
 */
export const ARTEMIS_CORE_EXPORTS = {
  malloc: 'Nc',
  free: 'Oc',
  errnoLocation: 'Pc',
  startUnwind: 'Ud',
  stopUnwind: 'Vd',
  startRewind: 'Wd',
  stopRewind: 'Xd',
  ctors: 'Mc',
  main: 'Qc',
  stackSave: 'Xc',
  stackRestore: 'Yc',
  stackAlloc: 'Zc',
  table: 'Lc',
  dynCallVi: 'bd',
  dynCallVii: 'dd',
  dynCallViii: 'ld',
  dynCallViiii: 'id',
  dynCallIiii: 'ed',
  dynCallV: 'rd',
  setThrew: '_c',
} as const;

/** Where each bucket's official names live, so a miss reads as a bug rather than a shutdown. */
const BUCKET_OFFICIAL: Readonly<Record<string, Readonly<Record<string, string>>>> = {
  fd: ARTEMIS_FD_OFFICIAL,
  runtime: ARTEMIS_RUNTIME_OFFICIAL,
  eh: ARTEMIS_EH_OFFICIAL,
  asyncify: ARTEMIS_ASYNCIFY_OFFICIAL,
  async: ARTEMIS_ASYNC_OFFICIAL,
  gl: ARTEMIS_GL_OFFICIAL,
};

/**
 * Short name to `[bucket, official name]`, built once from the six committed maps.
 *
 * Keyed by **short** name because that is the key every committed map uses and the key the module
 * actually imports: `a.b`, `a.c`, … are the glue's minified short names, while the official names are
 * only what a report needs to speak. The previous reversal keyed by official name and could therefore
 * never match a single module row.
 *
 * The six maps are disjoint on both short and official names, so no bucket can shadow another and the
 * insertion order of {@link BUCKET_OFFICIAL} is not load-bearing.
 */
function shortIndex(): ReadonlyMap<string, readonly [string, string]> {
  const index = new Map<string, readonly [string, string]>();
  for (const [bucket, table] of Object.entries(BUCKET_OFFICIAL)) {
    for (const [short, official] of Object.entries(table)) {
      if (!index.has(short)) index.set(short, [bucket, official]);
    }
  }
  return index;
}

const SHORT_INDEX = shortIndex();

/** The bucket a declared row lands in when none of the six committed maps claims it. */
const TODO_BUCKET = 'todo';

/** One import the guest asked for and no bucket can answer. */
export interface ArtemisUnservedImport {
  readonly name: string;
  readonly official: string;
  readonly bucket: string;
}

/**
 * The module's own import list.
 *
 * Read through the **static** `WebAssembly.Module.imports(module)`, which every engine has carried
 * since 2017 and which is what this project's older `lib` shape actually declares. Calling
 * `module.imports()` on the instance instead is not a shape the runtime guarantees, so a "structural"
 * cast there would have compiled green while asking the engine for a method the module never had.
 *
 * The returned descriptors carry `module`, `name` and `kind` — and no function index. The index is the
 * row's **position among the function rows**, counted here, which is what the committed JSON declares
 * and what the first row (`a.b`) resolves to as `0`.
 */
interface ImportDescriptor {
  readonly module: string;
  readonly name: string;
  readonly kind: string;
}

function moduleImports(module: WebAssembly.Module): readonly ImportDescriptor[] {
  return WebAssembly.Module.imports(module) as readonly ImportDescriptor[];
}

/**
 * The one short name no committed map claims, and the only row allowed to become a `todo`.
 *
 * `Xb` is `_system` — the only row this bridge imports that forks a subprocess, which this host
 * deliberately never does. It is classified explicitly rather than left to the blanket rule, and
 * the boot table answers it with a real refusal (`-1` and `ENOSYS`) instead of the family-wide zero
 * every other stub returns. Any *other* unclaimed row still throws: a row nobody has a reason for
 * is a bridge this host has not read, not a row to be papered over.
 */
const ARTEMIS_TODO_ROW = { name: 'Xb', official: '_system' } as const;

/**
 * What the module asks of us, resolved against the six buckets by **short name**.
 *
 * A function row is placed by its short name. {@link ARTEMIS_TODO_ROW} is the single explicit
 * exception, and every other unclaimed row is a **throw** — not a zero and not a stubbed
 * `unclassified` lane — because a row the boot table would stub is exactly how a stalled engine
 * looks identical to a working one: the row has to be named here, while the module is still in hand.
 * The single memory row is not a function and is counted separately by the caller.
 */
export function buildSpec(module: WebAssembly.Module): ArtemisImportSpec {
  const rows: ArtemisImportRow[] = [];
  const buckets: Record<string, number> = {};
  for (const entry of moduleImports(module)) {
    if (entry.module !== ARTEMIS_IMPORT_MODULE) {
      throw hostError(
        `the bridge imports ${entry.module}.${entry.name}, which is not module ` +
          `${ARTEMIS_IMPORT_MODULE}`,
      );
    }
    if (entry.kind === 'memory') {
      if (entry.name !== 'a') {
        throw hostError(`the bridge must import exactly one memory as ${ARTEMIS_IMPORT_MODULE}.a, not ${entry.module}.${entry.name}`);
      }
      buckets.memory = (buckets.memory ?? 0) + 1;
      continue;
    }
    if (entry.kind !== 'function') {
      throw hostError(
        `the bridge imports ${entry.module}.${entry.name} as a ${entry.kind}, which has no bucket`,
      );
    }
    const found = SHORT_INDEX.get(entry.name);
    // `Xb` gets a bucket; `unknown` never does. The refusal that bucket stands for is the boot
    // table's `_system` row, which answers ENOSYS — a documented refusal, not a silent zero.
    const placed =
      found ??
      (entry.name === ARTEMIS_TODO_ROW.name
        ? ([TODO_BUCKET, ARTEMIS_TODO_ROW.official] as const)
        : undefined);
    if (placed === undefined) {
      throw hostError(
        `the bridge imports ${entry.module}.${entry.name} (${entry.name}), which no bucket implements`,
      );
    }
    const index = rows.length;
    rows.push({ index, name: entry.name, official: placed[1], bucket: placed[0] });
    buckets[placed[0]] = (buckets[placed[0]] ?? 0) + 1;
  }
  return { rows, buckets };
}

/**
 * The rows {@link buildSpec} could not place, in module order.
 *
 * Exported because "which import stopped the boot" is the first question a failed instantiation
 * raises, and a caller should be able to ask it without parsing a message. {@link buildSpec} throws
 * on such a row rather than returning one, so this is the shape a *caller-supplied* `importMap` is
 * checked with. It excludes **only** {@link ARTEMIS_TODO_ROW}: the boot table serves `Xb` with a real
 * ENOSYS refusal, so it is a served row — while any other row arriving here is one the boot would
 * have to stub, which is exactly what must not happen.
 */
export function unservedImports(spec: ArtemisImportSpec): readonly ArtemisUnservedImport[] {
  return spec.rows
    .filter((row) => row.bucket === TODO_BUCKET && row.name !== ARTEMIS_TODO_ROW.name)
    .map((row) => ({ name: row.name, official: row.official, bucket: row.bucket }));
}

/**
 * The session's default answer to the fd bucket's callback seam.
 *
 * The fd rows call `(entry, arg, extra?, count?)`, which is the glue's `_emscripten_idb_*` shape
 * rather than a `getDynCaller` signature. This picks the signature the glue itself would have used
 * from the arguments actually present — `vi` with no extra, `vii` with one, `viii` with two — and
 * forwards exactly those values, because the count of arguments **is** the arity claim here: padding
 * to a fixed four would hand the guest callback readings it never asked for.
 *
 * Entry 0 is "no callback" and is answered without touching the guest, which is the glue's own
 * `if (onload)` guard. Returned as a promise so a callback that itself suspends is awaited by the
 * row's own completion rather than released early.
 */
function defaultFdCallback(
  invoke: (signature: string, entry: number, args: number[]) => Promise<void>,
): ArtemisFdHost['callback'] {
  return (entry, arg, extra, count) => {
    if (!entry) return undefined;
    if (extra === undefined) return invoke('vi', entry | 0, [arg | 0]);
    if (count === undefined) return invoke('vii', entry | 0, [arg | 0, extra | 0]);
    return invoke('viii', entry | 0, [arg | 0, extra | 0, count | 0]);
  };
}

/**
 * Checks a caller's map against the spec read out of the module, and throws on any disagreement.
 *
 * This is **verification, not classification**: it never substitutes one for the other, and the
 * session keeps using {@link buildSpec}'s result as `spec` whatever this decides. The supplied map
 * is a claim about the module, and a claim that does not hold is worse than no claim — it would let
 * a row be routed by a name it does not have, which is the silent-stub failure this module exists to
 * prevent. Every function row is compared positionally because the row index is the ABI.
 *
 * Two things are deliberately **not** asserted, because the module cannot speak to them:
 *
 * - The raw ABI fields a committed map carries (`signature`, `typeIndex`, `glueBindingOffset`, …) are
 *   absent from `WebAssembly.Module.imports()`, so a mismatch there cannot be detected here and is
 *   not guessed at.
 * - `buckets` is compared only for the counts the module itself tells us about. `buckets.memory` is
 *   the case that matters: the committed map may omit it, an omitted key is **not** an assertion of
 *   zero, and the function-row bucket totals are derived from the same rows already compared.
 */
function verifyImportMap(supplied: ArtemisImportSpec, actual: ArtemisImportSpec): void {
  const rows = supplied.rows;
  if (!Array.isArray(rows)) {
    throw hostError('the supplied importMap has no rows array to verify against the module');
  }
  if (rows.length !== actual.rows.length) {
    throw hostError(
      `the supplied importMap describes ${rows.length} function row(s), but the module imports ` +
        `${actual.rows.length}`,
    );
  }
  for (const [position, expected] of actual.rows.entries()) {
    const given = rows[position];
    if (given === undefined || given === null || typeof given !== 'object') {
      throw hostError(`the supplied importMap has no row at index ${position}`);
    }
    if (given.index !== undefined && given.index !== position) {
      throw hostError(
        `the supplied importMap row ${position} claims index ${String(given.index)}; rows are matched ` +
          'positionally',
      );
    }
    for (const field of ['name', 'official', 'bucket'] as const) {
      if (given[field] !== expected[field]) {
        throw hostError(
          `the supplied importMap row ${position} has ${field} ${JSON.stringify(given[field])}, but the ` +
            `module imports ${JSON.stringify(expected[field])}`,
        );
      }
    }
  }
  // Only the bucket names the module's own rows produced are checked, and only when the caller
  // claimed a count for them. An absent key is not a claim of zero, so it is not a mismatch.
  for (const [bucket, expected] of Object.entries(actual.buckets)) {
    const claimed = supplied.buckets?.[bucket];
    if (claimed !== undefined && claimed !== expected) {
      throw hostError(
        `the supplied importMap claims ${claimed} row(s) in bucket '${bucket}', but the module imports ` +
          `${expected}`,
      );
    }
  }
}

/**
 * How many arguments the guest's own trampoline passes for each supported signature.
 *
 * The value is the arity the *guest* callback takes, and the glue's own `getDynCaller` string names
 * the **return type first**: `vi` is `(i32) -> void`, `vii` is `(i32, i32) -> void`, `viii` is
 * `(i32, i32, i32) -> void`, `viiii` is `(i32 × 4) -> void`. The signature's own length is therefore
 * the argument count **plus one**, and it is the leading `v` that is not an argument — never a
 * trailing table index, which the dynCall resolves through its own second argument and which a
 * table-entry call does not take at all.
 *
 * Only these four are dispatched; anything else names a shape no row here forwards.
 */
const SIGNATURE_ARITY: Readonly<Record<string, number>> = {
  vi: 1,
  vii: 2,
  viii: 3,
  viiii: 4,
};

/** The signatures {@link ArtemisSessionCore.invokeCallback} dispatches; anything else is refused. */
export const ARTEMIS_CALLBACK_SIGNATURES: readonly string[] = Object.keys(SIGNATURE_ARITY);

/**
 * The guest export each signature falls back to when the function table holds no usable slot.
 *
 * These are the glue's own bindings, read out of the shipped bridge rather than guessed:
 * `dynCall_vi=Module["asm"]["bd"]`, `dynCall_vii` is `dd`, `dynCall_viii` is `ld`, `dynCall_viiii` is
 * `id`. The exported `dynCall_v*` is only ever the fallback — the table is tried first, exactly as
 * `getDynCaller` does it — and a signature with no row here has no dynCall to fall back to.
 */
const SIGNATURE_FALLBACK: Readonly<Record<string, keyof typeof ARTEMIS_CORE_EXPORTS>> = {
  vi: 'dynCallVi',
  vii: 'dynCallVii',
  viii: 'dynCallViii',
  viiii: 'dynCallViiii',
};

/** What a session observed, so a caller can report rather than guess. */
export interface ArtemisSessionDiagnostics {
  /** Import rows the module declared, by bucket, as `buildSpec` classified them. */
  readonly buckets: Readonly<Record<string, number>>;
  /** Function rows no bucket claimed. Empty on every supported bridge. */
  readonly unserved: readonly ArtemisUnservedImport[];
  /** `true` once `WebAssembly.instantiate` resolved and the exports were attached. */
  readonly instantiated: boolean;
  /** `true` once the allocator and the asyncify control exports were found. */
  readonly wired: boolean;
  /** `true` once the guest entered its GLFW frame loop (`_main` threw `"unwind"`). */
  readonly mainLoopStarted: boolean;
  /** The last terminal failure, or `undefined`. */
  readonly failure?: unknown;
  /** Whether {@link ArtemisSessionCore.dispose} has run. */
  readonly disposed: boolean;
}

/** Everything a session needs to be built. */
export interface ArtemisSessionCoreOptions {
  /** The bridge: bytes, a URL, or an already-compiled module. */
  readonly wasm: string | URL | Uint8Array | WebAssembly.Module;
  /** Where the guest writes its saves, from the game's own `system.ini`. */
  readonly savePath?: string;
  /** The base the assets are resolved against, when the host has no better answer. */
  readonly baseUrl?: string;
  /** How many asset bytes to keep, passed through to the asset seam. */
  readonly cacheBytes?: number;
  /** Runs `_emscripten_asm_const_int` code blocks. Absent means `B` refuses rather than answers 0. */
  readonly runAsmConst?: ArtemisAsmConstRunner;
  /** Delivers an fd callback (a streaming read finishing outside the guest). */
  readonly fdCallback?: ArtemisFdHost['callback'];
  /** The guest's synchronous IndexedDB, when the page has one. */
  readonly idb?: ArtemisSyncIdb;
  /** The GL surface. The guest's `_glfwCreateWindow` binds to this exact element. */
  readonly canvas: HTMLCanvasElement;
  /**
   * Builds the `AudioContext` the guest's `_alcCreateContext` will own.
   *
   * Optional, and passed straight through to `host/audio.ts`: absent, the audio host falls back to
   * `globalThis.AudioContext ?? globalThis.webkitAudioContext`, and a page with neither reports **0
   * devices** rather than fabricating a silent one. Supplying one is how a test drives an
   * `OfflineAudioContext`, and how a page reuses a context it made at first user gesture.
   */
  readonly audioContextFactory?: () => AudioContext;
  /** The guest's `argv`, defaulting to the bridge's own single-element vector. */
  readonly args?: readonly string[];
  /** Merged over the default environment block, exactly as the glue merges its `ENV` global. */
  readonly env?: ArtemisFdHost['env'];
  /**
   * An import classification to check against the module, replacing nothing.
   *
   * It is verified row by row against the spec read out of the module and rejected on any
   * disagreement — including a row the module does not import — because the module's own import list
   * is the only authority on routing. The session keeps using its own reading either way; this field
   * is a claim to confirm, not a classification to substitute.
   */
  readonly importMap?: ArtemisImportSpec;
}

/**
 * A running Artemis bridge.
 *
 * The fields are the seams a browser runtime needs: `memory` and `fs` to inspect, `driver` to ask
 * whether the guest is parked, `bootTable` and `trace` for diagnosis, and the queue to get onto.
 */
export interface ArtemisSessionCore {
  readonly host: RuntimeHost;
  readonly memory: ArtemisMemory;
  readonly fs: ArtemisFs;
  readonly driver: ArtemisSuspendDriver;
  readonly bootTable: ArtemisBootTable;
  /**
   * The one WebAudio host, whose nineteen rows are the `gl` bucket's audio rows.
   *
   * Built once, at construction, and lazy in effect: it creates no `AudioContext` until the guest
   * asks for one, so reading it is always safe and `pause`/`resume`/`dispose` are meaningful from
   * the first call. Handed out so a host can inspect or drive audio without a second table.
   */
  readonly audio: ArtemisAudioHost;
  readonly assets: ArtemisAssets;
  readonly idb?: ArtemisSyncIdb;
  /** The instantiated bridge, once {@link boot} has attached it. */
  readonly instance: WebAssembly.Instance | undefined;
  /** The options this session was built from. */
  readonly options: ArtemisSessionCoreOptions;
  /** The module's own import list, resolved. Readable before `boot()`. */
  readonly spec: ArtemisImportSpec;
  /**
   * The classification the boot actually used.
   *
   * A caller-supplied `options.importMap` is verified against the module and then discarded as a
   * *router*: it may only agree or disagree, and if it disagrees the construction throws. Routing is
   * always the module's own reading, so this is that reading, and it is the same object as
   * {@link ArtemisSessionCore.spec} by construction.
   */
  readonly importSpec: ArtemisImportSpec;
  /** What the boot observed: counts per short name, blocked rows, buckets. */
  readonly trace: ArtemisBootTable['trace'];
  /**
   * Serialises one unit of work against the guest.
   *
   * Waits for the driver to go idle **before** running `task`, so a unit queued behind a parked call
   * starts on a settled guest. Every rejection — from the wait and from the task — is handled, so a
   * failed unit cannot become an orphan, and the returned promise is the real outcome.
   */
  enqueue<T>(task: () => T | Promise<T>): Promise<T>;
  /** Instantiate the module and run the official startup sequence. Explicit: call it when ready. */
  boot(args?: readonly string[]): Promise<ArtemisBootResult>;
  /** Enter one guest function-table entry. Resolves once the driver is idle again. */
  invokeCallback(signature: string, entry: number, args: number[]): Promise<void>;
  /** Stop entering the guest. The listeners, the context and the heap all survive. */
  pause(): Promise<void>;
  /** Start again with exactly one pending frame, if the loop is installed. */
  resume(): Promise<void>;
  /** Terminal, idempotent teardown. */
  dispose(): Promise<void>;
  /** A snapshot, safe to call at any point including after `dispose`. */
  diagnostics(): ArtemisSessionDiagnostics;
}

/** Everything this module refuses to guess at throws one of these. */
function hostError(message: string): ArtemisHostError {
  return new ArtemisHostError(message);
}

/**
 * One outstanding frame, and the means to cancel it.
 *
 * The platform handle and the kind that produced it are stored because the cancel is not
 * interchangeable: a rAF id passed to `clearTimeout` (or a timer id to `cancelAnimationFrame`)
 * cancels nothing while looking exactly like a cancel that worked. Only the session knows which of
 * the two it was handed, so it is the session that has to carry the answer.
 */
interface FrameTicket {
  cancelled: boolean;
  handle: number | ReturnType<typeof setTimeout>;
  kind: 'raf' | 'timer';
  cancel(): void;
}

/** `module.imports()`, filtered to the memory rows, for the one check that has no substitute. */
function memoryImports(module: WebAssembly.Module): readonly string[] {
  return moduleImports(module)
    .filter((entry) => entry.kind === 'memory')
    .map((entry) => `${entry.module}.${entry.name}`);
}

/** A `Uint8Array` independent of the caller's buffer: a compiled module must not share it. */
function cloneBytes(bytes: Uint8Array): Uint8Array {
  return new Uint8Array(bytes.slice());
}

/**
 * Loads and compiles, whichever of the four shapes the caller passed.
 *
 * A relative URL is resolved against `options.canvas.ownerDocument.baseURI`, not against the ambient
 * document: the canvas is the one document reference a session is guaranteed, and a worker or an
 * offscreen context has no usable global to fall back to.
 */
async function loadModule(
  wasm: ArtemisSessionCoreOptions['wasm'],
  options: ArtemisSessionCoreOptions,
): Promise<WebAssembly.Module> {
  if (wasm instanceof WebAssembly.Module) return wasm;
  if (wasm instanceof Uint8Array) {
    return new WebAssembly.Module(cloneBytes(wasm) as unknown as BufferSource);
  }
  const base = options.canvas.ownerDocument?.baseURI;
  const target = typeof wasm === 'string' ? wasm : wasm.href;
  const url = base === undefined ? target : new URL(target, base).href;
  const response = await fetch(url);
  if (!response.ok) {
    throw hostError(`could not fetch the bridge at ${url}: HTTP ${response.status}`);
  }
  const buffer = await response.arrayBuffer();
  return new WebAssembly.Module(buffer);
}

/** Reads one guest export by its short name, refusing with the name the diagnosis needs. */
function requireExport<T>(
  exports: Readonly<Record<string, unknown>>,
  shortName: string,
  what: string,
): T {
  const found = exports[shortName];
  if (typeof found !== 'function') {
    throw hostError(`the bridge exports no ${shortName} (${what})`);
  }
  return found as T;
}

/**
 * Opens the asset seam over the host's game source.
 *
 * The three options are the ones `createArtemisAssets` documents, and a failure to find a
 * configuration is reported rather than fatal: a bridge can be instantiated and diagnosed without a
 * game behind it, and the follow-up work that wires the media path is the one that needs assets.
 */
async function openAssets(
  host: RuntimeHost,
  options: ArtemisSessionCoreOptions,
): Promise<ArtemisAssets> {
  const { createArtemisAssets } = await import('./assets.js');
  return createArtemisAssets(host.source, {
    ...(options.savePath === undefined ? {} : { savePath: options.savePath }),
    ...(options.baseUrl === undefined ? {} : { baseUrl: options.baseUrl }),
    ...(options.cacheBytes === undefined ? {} : { cacheBytes: options.cacheBytes }),
  });
}

/** The view `ArtemisMemory` wants, so the arena never has to reach for a global `WebAssembly`. */
interface ArenaMemory {
  readonly buffer: ArrayBuffer;
  grow(delta: number): number;
}

/**
 * Instantiates the bridge and wires one session around it.
 *
 * The returned object is inert until {@link ArtemisSessionCore.boot} is called. This function builds
 * the filesystem, compiles the module, constructs the boot table and instantiates; the caller is the
 * one who knows whether the media, the canvas and the persistence layer are ready, and a session that
 * started the guest before them would boot a game into a world that does not exist yet.
 */
export async function createArtemisSessionCore(
  host: RuntimeHost,
  options: ArtemisSessionCoreOptions,
): Promise<ArtemisSessionCore> {
  const module = await loadModule(options.wasm, options);

  /**
   * One memory, ours, exactly the shape the bridge declares.
   *
   * Checked rather than assumed: a module that imports two memories, or one under another name, cannot
   * be instantiated against this one, and the failure is worth naming here while the import list is
   * still in hand rather than letting `instantiate` report an anonymous mismatch.
   *
   * This runs **before** the assets are opened: the checks below throw, and an asset seam opened first
   * would be leaked, since a throw here has no controller to run a cleanup.
   */
  const declared = memoryImports(module);
  if (declared.length !== 1 || declared[0] !== `${ARTEMIS_IMPORT_MODULE}.a`) {
    throw hostError(
      `the bridge must import exactly one memory as ${ARTEMIS_IMPORT_MODULE}.a, not ` +
        `${declared.join(', ') || 'none'}`,
    );
  }

  const spec = buildSpec(module);
  const unserved = unservedImports(spec);
  if (unserved.length > 0) {
    throw hostError(
      `the bridge imports ${unserved.length} function row(s) no bucket implements: ` +
        unserved.map((row) => `${row.official || row.name}:${row.bucket}`).join(', '),
    );
  }

  /**
   * A caller-supplied map is **checked against the module**, never used in place of reading it.
   *
   * The module is the only thing in hand that knows what it imports; a map is a claim about a
   * module, and an unverified claim is how a wrong row gets routed or an unknown row gets a neutral
   * stub. The supplied map cannot be used to *route* anything either: routing is by short name from
   * the module's own import list, and a map only says which bucket a name landed in. So a map that
   * disagrees about any function row — count, order, name, official name or bucket — is rejected
   * here, before the asset seam is opened, rather than silently classifying a different module.
   */
  if (options.importMap !== undefined) verifyImportMap(options.importMap, spec);

  /** One seam, opened once and used by the filesystem below, the boot table and the session. */
  const assets = await openAssets(host, options);

  const memory = new ArtemisMemory({
    memory: new WebAssembly.Memory({
      initial: ARTEMIS_ARENA_PAGES.initial,
      maximum: ARTEMIS_ARENA_PAGES.maximum,
    }) as unknown as ArenaMemory,
    label: 'HEAP',
  });
  const fs = new ArtemisFs();
  /** The two writable roots a session always has, plus the game's own save path when it named one. */
  fs.mkdirp('/save');
  fs.mkdirp('/tmp');
  fs.makeWritable('/save');
  fs.makeWritable('/tmp');
  if (assets.savePath) {
    fs.mkdirp(assets.savePath);
    fs.makeWritable(assets.savePath);
  }
  /**
   * The two files the engine reads out of its own packs are mounted, and nothing is enumerated: the
   * game tree is ~29,224 files and is answered lazily by {@link ArtemisAssets.resolve} instead.
   */
  mountArtemisTree(fs, '/', [
    ['/root.pfs', assets.rootPfs],
    ['/system.ini', assets.config],
  ]);

  /** Guest exports, filled by {@link attach}. Every read goes through here, never a captured value. */
  let exports: Readonly<Record<string, unknown>> = {};
  let instance: WebAssembly.Instance | undefined;
  let lifecycle: ArtemisGlLifecycle | undefined;
  let disposed = false;
  let instantiated = false;
  let wired = false;
  let mainLoopStarted = false;
  let failure: unknown;
  let hasFailure = false;
  let frameTicket = 0;
  const frames = new Map<number, FrameTicket>();
  let queueTail: Promise<void> = Promise.resolve();
  /** The one boot in flight, so a second `boot()` joins the first instead of running a second one. */
  let bootPromise: Promise<ArtemisBootResult> | undefined;
  /** The one teardown in flight, for the same reason and with the same identity guarantee. */
  let disposePromise: Promise<void> | undefined;
  /**
   * Which "run" of the loop a call belongs to.
   *
   * `resume` has to wait on the audio device and the driver before it may restart `gl`, and either of
   * those waits can outlive a `pause` or a `dispose` that came after it. The counter is bumped by both,
   * so a resume that started earlier can tell that it is stale and must not revive the loop — a
   * timestamp would be a second source of truth for the same fact, and the count is all the check needs.
   */
  let loopEpoch = 0;
  /**
   * The newest `resume` request, and the only thing that can tell a *stale* resume from a superseded
   * one. {@link loopEpoch} cannot: a `pause` or a `dispose` bumps it, but a second `resume` does not,
   * so a resume that a later `pause` overtook would look current again the moment the host resumed.
   * A stale resume repays the device **only while it is still the newest request** — a newer resume
   * owns the device, and stopping it there would defeat the request that superseded this one.
   */
  let resumeIntent = 0;
  /**
   * One wrapped call per raw guest function, keyed by the function object itself.
   *
   * `driver.wrap` decides nesting from the function object it was given, so a fresh wrapper for the
   * same raw function would be a fresh root every time — and a replayed callback must be the *same*
   * root. The key is the raw function, so a rewind that replays a different pass still finds the one
   * wrapper that owns the parked root.
   */
  const callbackWrappers = new WeakMap<object, ArtemisSuspendingFunction>();

  /**
   * Reads a guest export **through this function**, never by value.
   *
   * The driver and the boot table are both constructed before the instance exists, while the allocator
   * and the four asyncify control functions are exports of that instance. A lookup performed when the
   * closure is *called* is what makes that ordering work; a value captured at construction time would
   * be `undefined` forever. Calling one too early is a real failure, not an empty answer, and says so.
   */
  function requireGuest<T>(name: keyof typeof ARTEMIS_CORE_EXPORTS, what: string): T {
    const shortName = ARTEMIS_CORE_EXPORTS[name];
    return requireExport<T>(exports, shortName, what);
  }

  /** The guest's allocator, late-bound for the same reason as {@link requireGuest}. */
  const guestMalloc = (bytes: number): number => requireGuest<(b: number) => number>('malloc', 'malloc')(bytes);
  const guestFree = (pointer: number): void => {
    requireGuest<(p: number) => void>('free', 'free')(pointer);
  };

  /**
   * The asyncify driver, whose four control functions are guest exports.
   *
   * `control` is deliberately a set of closures rather than a set of bound exports: the control block
   * and the state global live in the guest, so the *functions* that write them can only be read once
   * the instance exists, and the driver has to be constructible before it — it is what `bootAsync`
   * receives. `heap32` is a getter for the same reason, and because `memory.refresh()` replaces the
   * views whenever the guest grows the heap.
   */
  const driver = new ArtemisSuspendDriver({
    control: {
      startUnwind: (data: number) => requireGuest<(v: number) => void>('startUnwind', '_asyncify_start_unwind')(data),
      stopUnwind: () => requireGuest<() => void>('stopUnwind', '_asyncify_stop_unwind')(),
      startRewind: (data: number) => requireGuest<(v: number) => void>('startRewind', '_asyncify_start_rewind')(data),
      stopRewind: () => requireGuest<() => void>('stopRewind', '_asyncify_stop_rewind')(),
    },
    memory: {
      // Re-read through the guest on every call: `malloc` is a guest export that may grow the heap,
      // and a view captured before that growth would point into a detached buffer.
      malloc: (bytes) => {
        const pointer = guestMalloc(bytes);
        memory.refresh();
        return pointer;
      },
      free: (pointer) => {
        guestFree(pointer);
        memory.refresh();
      },
      // `refresh()` before every read for the same reason `malloc` re-reads: the guest's own
      // allocator may have grown the heap since the last view, and a stale `i32` would address the
      // detached buffer the growth left behind rather than the live arena.
      heap32: () => {
        memory.refresh();
        return memory.i32;
      },
    },
  });

  /**
   * The frame source.
   *
   * `gl` asks for a frame and this session owns the cancel, because only the session knows whether it
   * handed out a rAF handle or a timer id. Every ticket is recorded and cancelled by `dispose`; a
   * ticket that fires after a cancel checks its own flag first, and one that fires while disposed does
   * nothing at all. The notify after `tick()` is `host.requestFrame()` — a *paint* request, not a
   * scheduling one, and the reason a frame the guest drew is actually shown.
   */
  const requestFrame = (tick: () => void): void => {
    if (disposed || hasFailure) return;
    const id = (frameTicket += 1);
    const view = options.canvas.ownerDocument?.defaultView;
    // Whatever the platform handed back is retained, because only that exact value can be cancelled
    // later. A ticket that recorded only "cancelled" would leave a real rAF callback (or timer) alive
    // to fire once more — harmless to the guest, but not a cancel.
    const ticket: FrameTicket = {
      cancelled: false,
      handle: 0,
      kind: 'timer',
      cancel(): void {
        // Read through `ticket` rather than closed over: the handle and the kind are written below,
        // after the platform call, and this method is only ever reached afterwards.
        try {
          if (ticket.kind === 'raf' && view && typeof view.cancelAnimationFrame === 'function') {
            view.cancelAnimationFrame(ticket.handle as number);
          } else if (ticket.kind === 'timer') {
            clearTimeout(ticket.handle as ReturnType<typeof setTimeout>);
          }
        } catch {
          // Cancelling is best effort: the flag already stops the callback re-entering the guest, and
          // a platform that throws here must not turn a pause into a rejection.
        }
      },
    };
    frames.set(id, ticket);
    const run = (): void => {
      frames.delete(id);
      if (ticket.cancelled || disposed) return;
      try {
        tick();
      } catch (error) {
        // A frame is the guest's own code running, so a throw here is the guest failing — reported
        // and made terminal, exactly as a queued unit would be, rather than left as an unhandled
        // rejection inside a rAF callback nobody awaits.
        report('frame', `the tick threw: ${String(error)}`);
        // `fail` is terminal and re-throws the *same* value to hand it to an awaiting caller — but
        // this callback is void and nobody awaits it, so owning that throw here is the only way to
        // keep the platform's event handler from leaking it. `catch` runs unconditionally: a bare
        // `fail` call would escape this `catch` block through the surrounding `try`'s own catch path.
        try {
          fail(error);
        } catch {
          // Deliberate re-throw, already recorded and announced exactly once.
        }
      }
      try {
        host.requestFrame();
      } catch (error) {
        // The repaint request is the *host's* half, not the guest's: a host that refuses a paint has
        // not made the guest fatal, and marking it so would turn a missed frame into a dead session.
        report('frame', `the host refused a repaint: ${String(error)}`);
      }
    };
    if (view && typeof view.requestAnimationFrame === 'function') {
      ticket.kind = 'raf';
      ticket.handle = view.requestAnimationFrame(run);
    } else {
      ticket.kind = 'timer';
      ticket.handle = setTimeout(run, 16);
    }
  };

  /**
   * Cancels every outstanding frame, natively and in the record.
   *
   * The platform handle is cancelled **as well as** the flag, and in that order the flag matters
   * least: a callback may already be queued by the platform when this runs, and the flag is what
   * stops that one re-entering the guest. Shared by `pause` and `dispose` so the two cannot drift.
   */
  const cancelFrames = (): void => {
    const outstanding = [...frames.values()];
    frames.clear();
    for (const ticket of outstanding) {
      ticket.cancelled = true;
      try {
        ticket.cancel();
      } catch (error) {
        report('frame', `cancelling the frame handle threw: ${String(error)}`);
      }
    }
  };

  /**
   * The queue: the whole concurrency story of an asyncify guest.
   *
   * The idle wait happens **before** the task, so a unit queued behind a parked call starts on a
   * settled guest rather than on top of one. Both the wait and the task are handled, so a failed unit
   * can never become an orphan; the returned promise is the real outcome for both.
   *
   * Once anything has failed the session is terminal, and every unit queued afterwards is rejected
   * with the **stored first failure without running**: starting a task on a driver that is winding
   * down is corruption, not recovery, and the first reason must not be replaced by whatever the
   * doomed task happens to report. The pre-wait and post-wait `hasFailure` checks are both needed —
   * the first refuses a unit that was already queued behind a failure, the second refuses a unit
   * whose own wait was the thing that surfaced it.
   *
   * `invokeCallback` deliberately does not await this queue from inside a task: it is called *by* the
   * queued completion, so re-entering the queue there would deadlock it against itself.
   */
  const enqueue = <T>(task: () => T | Promise<T>): Promise<T> => {
    const run = queueTail.then(async (): Promise<T> => {
      if (disposed) throw hostError('the session is disposed');
      if (hasFailure) throw failure;
      try {
        await driver.waitUntilIdle();
        if (disposed) throw hostError('the session is disposed');
        if (hasFailure) throw failure;
        const value = await task();
        await driver.waitUntilIdle();
        return value;
      } catch (error) {
        if (disposed) throw error;
        return fail(error);
      }
    });
    // The tail swallows the outcome so one failed unit cannot poison every unit behind it, while the
    // returned promise still carries the real rejection.
    queueTail = run.then(
      () => undefined,
      () => undefined,
    );
    return run;
  };

  /**
   * Enters one guest function-table entry, exactly as the official glue's `getDynCaller` would.
   *
   * The table lookup is captured **once per invocation**: a rewind replays the same raw function with
   * the same arguments, and re-reading `Lc.get(entry)` between the passes would call whichever slot
   * the guest has since put there. The signature decides the arity, and an unsupported one is refused
   * rather than called with a guessed argument list.
   *
   * The captured function is what gets *called* — never the table index. A table-entry call is the
   * `(i32 × n) -> void` shape, so passing the index as a trailing argument (the way `dynCall_*`
   * resolves its own second parameter) would hand the guest callback the index as data.
   *
   * The fallback is the exported `dynCall_*` the glue binds each signature to, and it is only
   * reached when the table slot is not a function — a slot holding a non-function, or a table that
   * has not been wired yet. `requireGuest` is function-only and cannot read `Lc` (the table is an
   * object, not a function), so the table is read straight out of `exports` instead.
   *
   * Deliberately **not** routed through {@link enqueue}: this runs inside an already queued
   * completion, and awaiting that queue from within a task would deadlock it against itself.
   */
  const invokeCallback = async (signature: string, entry: number, args: number[]): Promise<void> => {
    // Entry 0 is "no callback" — the glue's own `if (onload)` guard — so there is nothing to
    // dispatch and nothing for a signature or an arity to describe. Checked **before** both, because
    // a row that asked for nothing is not a row that asked for the wrong thing. The argument count is
    // not inspected here either: entry 0 with any list is still nothing to call.
    if (!entry) return;
    const arity = SIGNATURE_ARITY[signature];
    if (arity === undefined) {
      throw hostError(
        `invokeCallback refuses the signature '${signature}'; supported: ${ARTEMIS_CALLBACK_SIGNATURES.join(', ')}`,
      );
    }
    if (args.length !== arity) {
      throw hostError(`invokeCallback('${signature}') expects ${arity} argument(s), not ${args.length}`);
    }
    const index = entry | 0;
    const table = exports[ARTEMIS_CORE_EXPORTS.table];
    let resolved: unknown;
    if (table && typeof (table as { get?: unknown }).get === 'function') {
      try {
        resolved = (table as { get(index: number): unknown }).get(index);
      } catch {
        resolved = undefined;
      }
    }
    // Resolved once, and the same raw function is replayed by every later pass of this invocation.
    let raw: (...call: number[]) => number | void;
    let name = `callback:${signature}`;
    if (typeof resolved === 'function') {
      raw = resolved as (...call: number[]) => number | void;
    } else {
      const fallback = SIGNATURE_FALLBACK[signature];
      if (fallback === undefined) {
        throw hostError(`the guest function table holds no function at entry ${index}`);
      }
      name = `callback:${signature}:${ARTEMIS_CORE_EXPORTS[fallback]}`;
      raw = requireGuest<(...call: number[]) => number | void>(fallback, `dynCall_${signature}`);
    }
    let wrapped = callbackWrappers.get(raw);
    if (wrapped === undefined) {
      wrapped = driver.wrap(name, raw);
      callbackWrappers.set(raw, wrapped);
    }
    // `await` on a possibly-throwing call: a synchronous throw out of the wrapped call becomes this
    // promise's rejection rather than escaping the async body, and a promise-valued one is awaited.
    let callArgs = args.map((value) => value | 0);
    if (typeof resolved !== 'function') callArgs = [index, ...callArgs];
    await Promise.resolve(wrapped(...callArgs));
    await driver.waitUntilIdle();
  };

  /** Reports a *diagnosis*, never a second failure: a logger that throws must not replace the first. */
  const report = (what: string, detail: string): void => {
    try {
      host.log('warn', `artemis ${what}: ${detail}`);
    } catch {
      // Deliberately swallowed; see above.
    }
  };

  const savePath = assets.savePath;

  /**
   * The one audio host, built **before** the boot table because the `gl` bucket takes its nineteen
   * rows as one all-or-nothing table and checks them at construction.
   *
   * One host, not two: the `gl` bucket's audio rows and {@link ArtemisSessionCore.audio} are the same
   * object, so a pause that reaches the session reaches the same device map the guest is playing
   * into. Lazy in effect, not by deferral — `createArtemisAudioHost` allocates nothing and touches no
   * `AudioContext`; that happens on the guest's own `_alcOpenDevice` / `_alcCreateContext` call. So
   * a page without WebAudio instantiates normally and refuses at the first audio call, which is the
   * honest answer, rather than failing the whole session at construction.
   *
   * `onBlocked` is this session's own non-throwing {@link report}, so an audio diagnosis lands in the
   * host log with the same shape as every other bucket's.
   */
  const audio = createArtemisAudioHost({
    memory,
    malloc: guestMalloc,
    ...(options.audioContextFactory ? { contextFactory: options.audioContextFactory } : {}),
    onBlocked: report,
  });

  /**
   * Initiates a lifecycle call **now** and returns a promise that is already owned.
   *
   * A synchronous throw out of a disposer, a pause or a resume becomes a rejection rather than
   * escaping: the callers below have siblings to reach — the other disposer, the frame cancel, the
   * idle wait — and a throw on the first line would skip all of them. The value travels unchanged,
   * a legal `throw undefined` included.
   */
  const owned = (run: () => void | Promise<void>): Promise<void> => {
    try {
      return Promise.resolve(run());
    } catch (error) {
      return Promise.reject(error);
    }
  };

  /**
   * Every audio lifecycle call this session has started and not yet seen settle.
   *
   * The settle-once ownership above makes the *waiting* shareable, but the check that follows it
   * cannot answer "is the device finally quiet", because the audio host may have entered a real
   * asynchronous `context.suspend()` that nothing here can observe. So every call that can suspend
   * or resume the device is registered here while it is in flight and dropped when it settles, and a
   * resume that is still the newest request waits for the set to empty before it is allowed to make
   * its final decision — that wait is what stops an *earlier* repayment suspension, entered
   * asynchronously and still pending, from landing on top of a device the newest resume just started.
   *
   * A **set**, not a list: identity is the promise, so a call registered twice is one entry and a
   * settled call cannot be waited on twice. Empty is the overwhelmingly common case, so the
   * overwhelmingly common wait is a call that returns immediately.
   *
   * Registration is not a queue and does not defer anything: the lifecycle call itself still happens
   * synchronously, and only the *joining* of it is deferred.
   */
  const pendingAudio = new Set<Promise<void>>();
  const trackAudio = (call: Promise<void>): Promise<void> => {
    pendingAudio.add(call);
    const drop = (): void => {
      pendingAudio.delete(call);
    };
    // Both arms, because a rejection "settles" it just as much as a resolution does; a rejected call
    // left in the set would be waited on forever.
    void call.then(drop, drop);
    return call;
  };
  /** The in-flight audio lifecycle calls, as one promise. Empty means resolved, not pending. */
  const audioQuiet = (): Promise<void> =>
    pendingAudio.size === 0 ? Promise.resolve() : Promise.allSettled([...pendingAudio]).then(() => undefined);

  /**
   * Awaits every already-started sibling and rejects with the first one that failed.
   *
   * The promises are handed in **started**, so nothing here decides when a sibling begins: the callers
   * start them synchronously, which is what makes "stop now" synchronous while the waiting is not.
   *
   * `Promise.all` is wrong here — it settles on the earliest rejection and leaves the rest running
   * unobserved. The flag is what makes a legal `throw undefined` distinguishable from a clean settle;
   * the retained value alone cannot tell those apart. "First" means first in argument order, which is
   * the caller's own priority order and therefore deterministic.
   */
  const settleTogether = async (started: readonly Promise<void>[]): Promise<void> => {
    let failed = false;
    let firstFailure: unknown;
    for (const outcome of await Promise.allSettled(started)) {
      if (outcome.status === 'rejected' && !failed) {
        failed = true;
        firstFailure = outcome.reason;
      }
    }
    if (failed) throw firstFailure;
  };

  /**
   * The setup the boot table is built from.
   *
   * Held in a local, and the table built from it inside a `try` below, because the table is the
   * first thing that can *refuse*: the `gl` bucket throws when `audioImports` is incomplete, and the
   * audio host is already live by then. A refusal has to dispose what exists rather than leak it, and
   * the table's own error is the one the caller needs — so the cleanup's failures are reported, never
   * thrown in its place.
   */
  const bootSetup: ArtemisBootSetup = {
    memory,
    fs,
    spec,
    errnoLocation: () => requireGuest<() => number>('errnoLocation', '___errno_location')(),
    malloc: guestMalloc,
    free: guestFree,
    now: () => host.now(),
    runAsmConst:
      options.runAsmConst ??
      ((call: ArtemisAsmConst): number => {
        throw hostError(
          `the session was given no runAsmConst handler, so EM_ASM code ${call.code} (` +
            `${call.args.length} argument(s)) cannot be run`,
        );
      }),
    enqueue,
    invokeCallback,
    isDisposed: () => disposed,
    idb: options.idb,
    /**
     * The fd bucket's own callback convention, defaulted rather than left absent.
     *
     * `invokeAsync` reports "no function table wired up for entry N" and answers nothing when this
     * seam is missing, so the eight `_emscripten_idb_*` rows would take their failure path on every
     * host that did not pass one — a stalled save looking exactly like a store that works. The
     * default maps the fd convention's `(entry, arg, extra?, count?)` onto the signature the glue's
     * own `getDynCaller` would have used and forwards the leading values verbatim: `vi` for the two
     * one-argument arms, `vii`/`viii` as the extra values appear. It calls
     * {@link ArtemisSessionCore.invokeCallback} **directly** and never through {@link enqueue}: this
     * runs inside an already queued completion, and re-entering the queue from within a task would
     * deadlock it against itself.
     *
     * A caller-supplied `fdCallback` still wins — it is the seam's own override, and the rows that
     * need a different convention keep it.
     */
    fdCallback: options.fdCallback ?? defaultFdCallback(invokeCallback),
    /**
     * `options.env` is the *environment block* (extra variables plus language and executable name),
     * while the boot table's `env` is the variable map alone — the block's own defaults fill in the
     * rest, exactly as the glue's `ENV` merge does.
     */
    env: options.env?.env,
    /** The request carries a `string` body; the seam only ever reads, so no body is forwarded. */
    resolve: (request) =>
      assets.resolve(request.url, request.method, new Uint8Array(0)),
    preMain: () => {
      try {
        fs.mkdirp(savePath);
        fs.makeWritable(savePath);
      } catch (error) {
        report('fs', `could not make ${savePath} writable: ${String(error)}`);
      }
    },
    onBlocked: report,
    suspend: driver,
    gl: {
      canvas: options.canvas as unknown as ArtemisGlHost['canvas'],
      document: options.canvas.ownerDocument ?? undefined,
      window: options.canvas.ownerDocument?.defaultView ?? undefined,
      requestFrame,
      now: () => host.now(),
      /** The `gl` bucket loops back through this session's own queue rather than a second one. */
      enqueue,
      waitUntilIdle: () => driver.waitUntilIdle(),
      isDisposed: () => disposed,
      free: guestFree,
      /** `gl` hands over its controller at the end of its own construction, which is right here. */
      setLifecycle: (controller: ArtemisGlLifecycle) => {
        lifecycle = controller;
      },
      /**
       * The nineteen audio rows, as one table, supplied once.
       *
       * This is the *same* host the session hands back as {@link ArtemisSessionCore.audio}, which is
       * what makes a session-level `pause()` and a guest-level `_alSourcePause` act on one device map
       * — a second table would be a second OpenAL implementation playing into the same speakers.
       */
      audioImports: audio.imports,
    },
  };

  /**
   * The table, built here rather than in the literal above so a refusal can be caught.
   *
   * `gl` validates all nineteen audio rows at construction, and this is the first construction step
   * that can throw with resources already open (the asset seam, the audio host, the driver). The
   * cleanup below is the same shape the instantiate failure uses: every step is attempted, each
   * failure is reported, and the table's own error is rethrown unchanged rather than masked by a
   * cleanup failure.
   */
  let bootTable: ArtemisBootTable;
  try {
    bootTable = createArtemisBootTable(bootSetup);
  } catch (error) {
    try {
      await lifecycle?.dispose();
    } catch (cleanupError) {
      report('cleanup', `the lifecycle failed to dispose: ${String(cleanupError)}`);
    }
    try {
      await audio.dispose();
    } catch (cleanupError) {
      report('cleanup', `the audio host failed to dispose: ${String(cleanupError)}`);
    }
    try {
      assets.dispose();
    } catch (cleanupError) {
      report('cleanup', `the assets failed to dispose: ${String(cleanupError)}`);
    }
    driver.dispose('construction failed');
    throw error;
  }

  /** Binds the guest exports the import table reaches back into, and the allocator the driver needs. */
  function attach(target: WebAssembly.Instance): void {
    exports = target.exports as Readonly<Record<string, unknown>>;
    instance = target;
    bootTable.attachExports(target);
    requireGuest('malloc', 'malloc');
    requireGuest('free', 'free');
    requireGuest('errnoLocation', '___errno_location');
    requireGuest('startUnwind', '_asyncify_start_unwind');
    requireGuest('stopUnwind', '_asyncify_stop_unwind');
    requireGuest('startRewind', '_asyncify_start_rewind');
    requireGuest('stopRewind', '_asyncify_stop_rewind');
    wired = true;
  }

  /**
   * Terminal: the **first** failure is recorded, logged and announced, and the real rejection is
   * returned. Later ones are still rejected, but neither recorded nor announced: the first is the
   * diagnosis, and a second notification would describe a symptom as though it were the cause.
   *
   * The first failure is also the moment the session stops, and it stops **synchronously**, exactly
   * as a `pause` does: a terminal guest can never run again, so a native frame left armed would be a
   * frame that re-enters a dead session, and an audio device left running would keep playing into it.
   * The lifecycle stop is owned rather than thrown (a `gl` that refuses to pause must not skip the
   * frame cancel or the audio park) and the audio park is owned rather than awaited, because this
   * function is synchronous and must rethrow the caller's own value.
   *
   * `hasFailure` exists because `undefined` is a legal thrown value (`throw undefined`), so
   * `failure === undefined` cannot tell "no failure yet" from "the guest threw undefined".
   */
  const fail = (error: unknown): never => {
    if (!hasFailure) {
      hasFailure = true;
      failure = error;
      void owned(() => lifecycle?.pause()).catch((stopError: unknown) =>
        report('failure', `the lifecycle failed to pause: ${String(stopError)}`),
      );
      cancelFrames();
      // Bumped here as well as by `pause` and `dispose`: a resume already in flight is now stale, and
      // its own repayment is what stops the device if its `context.resume()` lands after this park.
      loopEpoch += 1;
      void trackAudio(owned(() => audio.pause())).catch((audioError: unknown) =>
        report('failure', `the audio host failed to pause: ${String(audioError)}`),
      );
      try {
        host.log('error', `artemis session failed: ${String(error)}`);
      } catch {
        // Swallowed for the same reason `report` swallows: the reason travels with the rejection.
      }
      try {
        host.notify({ type: 'fatal', message: String(error) });
      } catch {
        // Same.
      }
    }
    throw error;
  };

  const session: ArtemisSessionCore = {
    host,
    memory,
    fs,
    driver,
    bootTable,
    audio,
    assets,
    ...(options.idb ? { idb: options.idb } : {}),
    get instance(): WebAssembly.Instance {
      if (!instance) throw hostError('the session is disposed');
      return instance;
    },
    options,
    spec,
    importSpec: spec,
    trace: bootTable.trace,
    enqueue,
    /**
     * Instantiate-once, memoised: the returned promise is the same object on every call, so a second
     * `boot()` while the first is in flight joins it rather than running a second startup, and a caller
     * that boots again after a failure receives the *same* rejection instead of a fresh attempt.
     *
     * Not an `async` function on purpose — the identity guarantee above depends on the promise being
     * created here and stored before anything can await it.
     */
    boot(args?: readonly string[]): Promise<ArtemisBootResult> {
      if (disposePromise) {
        return Promise.reject(hostError('boot() cannot run after the session is disposed'));
      }
      const started = bootPromise;
      if (started) return started;
      /**
       * One queued unit, whose *real* outcome is what `boot` resolves with. The outcome is taken from
       * `bootAsync` alone: argv, steps and the frame-loop flag are the guest's own report, and inventing
       * them here would describe a boot that did not happen.
       */
      const running = enqueue(async () => {
        if (!instance) throw hostError('boot() cannot run before the bridge is instantiated');
        const outcome = await bootTable.bootAsync(instance, args ?? options.args ?? []);
        mainLoopStarted = outcome.mainLoopStarted === true;
        report(
          'boot',
          `_main ${mainLoopStarted ? 'entered the frame loop' : 'returned'} ` +
            `after ${outcome.steps.length} step(s)`,
        );
        return outcome;
      });
      bootPromise = running;
      return running;
    },
    invokeCallback,
    /**
     * Stops the loop the moment it is called — the lifecycle, every outstanding frame ticket and the
     * audio device — and only then awaits the queue, so nothing new is scheduled while the tail drains.
     *
     * The three stops are **synchronous** and happen before the first await, because the point of a
     * pause is that no further frame, callback or sample escapes it; the waiting is what is deferred.
     * The queue drains before the driver is asked for idle: a callback still parked in the tail is
     * precisely the work that would otherwise report a loop as quiet while it is still running. A
     * terminal session stays terminally rejected rather than reporting a pause it cannot honour.
     *
     * Every sibling is started and owned here, including when one of them fails, and the first
     * rejection is returned unchanged — `throw undefined` included. The lifecycle stop is one of
     * those siblings now: a `gl` pause that throws synchronously is turned into a rejected promise
     * rather than escaping the call, because a throw on that line would skip the frame cancel and
     * the audio park — the other two halves of "stop now".
     */
    pause(): Promise<void> {
      if (disposed) return Promise.resolve();
      const lifecycleStopped = owned(() => lifecycle?.pause());
      cancelFrames();
      loopEpoch += 1;
      const audioStopped = trackAudio(owned(() => audio.pause()));
      const drained = (async (): Promise<void> => {
        await queueTail;
        if (hasFailure) throw failure;
        await driver.waitUntilIdle();
      })();
      return settleTogether([lifecycleStopped, drained, audioStopped]);
    },
    /**
     * The inverse, and never a revival.
     *
     * The audio device is resumed and the driver asked for idle *before* `gl` is resumed, and the
     * epoch, the disposal and the terminal flag are rechecked in between: a resume that was waiting on
     * either of those when a later `pause` or `dispose` arrived is stale, and reviving the loop there
     * would be exactly the outcome the later call was made to prevent. A stale resume resolves rather
     * than rejects — nothing failed, the caller's request was simply overtaken.
     *
     * Being overtaken is not enough on its own, though: the `context.resume()` this call started may
     * land *after* the pause that overtook it. That pause saw a still-'suspended' device and had
     * nothing to suspend, so without a repayment the speakers would be left running under a stopped
     * loop. The repayment happens here, after this resume's own await has settled, and only while
     * this call is still the newest resume — a resume that superseded it owns the device, and
     * stopping it there would undo a request that came later. A disposed session repays nothing:
     * `dispose` closes the device, and a `resume` on a closed context cannot have landed.
     *
     * The newest resume also **waits out every in-flight audio lifecycle call** before it decides
     * anything, and then resumes the device a second time. The repayment above is asynchronous by
     * nature — a real `context.suspend()` settles when the platform says so, not when it was entered
     * — so an older resume can still have one pending when a newer resume arrives. That newer resume
     * would otherwise see a device that is still 'running' (the suspension has not landed yet), find
     * its own `audio.resume()` a no-op, revive the loop, and then be undone by the earlier suspension
     * landing on top of it. So the wait comes first, the second `audio.resume()` follows it, and the
     * epoch, the intent, the disposal and the terminal flag are rechecked once more before `gl` is
     * revived. Only the newest resume does any of this: it is the one request whose outcome anyone is
     * still waiting for, and an older resume's repayment is left for it to wait out.
     */
    resume(): Promise<void> {
      if (disposed || hasFailure) {
        return hasFailure ? Promise.reject(failure) : Promise.resolve();
      }
      const epoch = loopEpoch;
      const intent = (resumeIntent += 1);
      /**
       * The one repayment, for both places this call can discover it has been overtaken.
       *
       * Only an *overtaken* resume repays, and only while it is still the newest request: a resume
       * that superseded it owns the device, and stopping it here would undo a request that came
       * later. A disposed session repays nothing — `dispose` closes the device, and a `resume` on a
       * closed context cannot have landed. The repayment is registered like every other lifecycle
       * call, because it suspends the device and a resume that comes after it has to wait it out.
       */
      const repay = async (): Promise<void> => {
        if (intent !== resumeIntent || disposed) return;
        try {
          await trackAudio(owned(() => audio.pause()));
        } catch (error) {
          report('resume', `the overtaken resume failed to repay its pause: ${String(error)}`);
        }
      };
      const superseded = (): boolean => disposed || hasFailure || epoch !== loopEpoch;
      const audioRunning = trackAudio(owned(() => audio.resume()));
      const idle = owned(() => driver.waitUntilIdle());
      return settleTogether([audioRunning, idle]).then(async (): Promise<void> => {
        // The newest resume, and only the newest, waits the device out — and it does so *before* the
        // stale check rather than after it, because what it is waiting for is precisely an earlier
        // repayment suspension that the check could otherwise only misread as a device which is
        // 'running' or, worse, one that is about to stop. This cannot deadlock a pause: a pause that
        // arrives while this resume is parked settles its own suspension from its own call and never
        // waits on this one, and this resume then finds itself superseded below and repays.
        if (intent === resumeIntent) await audioQuiet();
        if (superseded()) return repay();
        if (intent !== resumeIntent) return;
        // The wait above proves the device is quiet, not that it is *running*. A resume that arrived
        // while an older suspension was still landing saw `'running'` (the suspension had not been
        // entered yet) and its own `audio.resume()` was therefore a no-op — so the newest resume
        // resumes **again**, now that the suspension has landed, and only this second attempt is the
        // one that can be trusted to have really started the device.
        await trackAudio(owned(() => audio.resume()));
        // Rechecked after that await, because a pause, a failure or a newer resume can all have
        // arrived during it. A newer resume returns here without repaying: it owns the device, and it
        // performs this same wait-and-resume itself, so leaving the device running is its job.
        if (superseded()) return repay();
        if (intent !== resumeIntent) return;
        lifecycle?.resume();
        try {
          host.requestFrame();
        } catch {
          // A host that refuses a repaint is a diagnosis, not a reason to reject a resumed session.
        }
      });
    },
    /**
     * Terminal and idempotent: the same promise every call.
     *
     * `disposed` is set **synchronously** and first, the outstanding frame tickets are cancelled and the
     * loop epoch is bumped before any await, so a frame, a callback or a resume in flight finds a
     * disposed session rather than one that is about to be. `lifecycle.dispose` and `audio.dispose` are
     * both **started** here, once, while the driver and the exports are still live — the lifecycle
     * releases guest allocations through them, and the driver's own teardown is what makes those
     * pointers meaningless — and the audio device is closed before anything the guest could still be
     * holding is released.
     *
     * The queue and the cleanup are awaited **separately**: a rejected cleanup would otherwise hide the
     * queue's own outcome, and the first failure is the diagnosis worth keeping. Both disposals are
     * awaited even when one of them fails, and the first failure is preserved through a flag rather than
     * by comparing the value, because `throw undefined` is legal. Not `async`, for the same
     * promise-identity reason as {@link ArtemisSessionCore.boot}.
     */
    dispose(): Promise<void> {
      const started = disposePromise;
      if (started) return started;
      disposed = true;
      cancelFrames();
      loopEpoch += 1;
      /**
       * Set by the loop below, and **only** by it.
       *
       * The cleanup's failure is a legal `throw undefined`, so the retained value cannot be the flag:
       * `cleanupFailure !== undefined` reads a refused teardown as a clean one. This flag is what the
       * rethrow below is gated on, and the value travels unchanged.
       */
      let cleanupFailed = false;
      let cleanupFailure: unknown;
      const cleaning = (async (): Promise<void> => {
        // The in-flight audio calls are part of "the device is closed now": a suspension or a resume
        // already entered settles after the close, so waiting for it here is what keeps the session's
        // own finishing promise from resolving underneath one still running.
        const settledAudio = audioQuiet();
        for (const outcome of await Promise.allSettled([
          owned(() => lifecycle?.dispose()),
          trackAudio(owned(() => audio.dispose())),
        ])) {
          if (outcome.status === 'rejected' && !cleanupFailed) {
            cleanupFailed = true;
            cleanupFailure = outcome.reason;
          }
        }
        await settledAudio;
      })();
      const finishing = (async (): Promise<void> => {
        const settled = await Promise.allSettled([queueTail, cleaning]);
        try {
          try {
            assets.dispose();
          } finally {
            driver.dispose('session disposed');
            instance = undefined;
            exports = {};
          }
        } finally {
          if (cleanupFailed) throw cleanupFailure;
        }
        for (const outcome of settled) {
          if (outcome.status === 'rejected') report('dispose', `the queue settled with ${String(outcome.reason)}`);
        }
      })();
      disposePromise = finishing;
      return finishing;
    },
    diagnostics(): ArtemisSessionDiagnostics {
      return {
        buckets: spec.buckets,
        unserved,
        instantiated,
        wired,
        mainLoopStarted,
        ...(hasFailure ? { failure } : {}),
        disposed,
      };
    },
  };

  /**
   * The instance, built against the memory, the table and the filesystem above.
   *
   * The overload with an import object *returns an instance*, not `{ module, instance }` — the
   * promise-shaped one is what returns the pair — so the value `instantiate` resolves to is what
   * {@link attach} binds. A refused import's own error is rethrown unchanged below: `LinkError` says
   * which index, and a diagnosis invented here would replace the engine's words with a guess.
   */
  try {
    const created = await WebAssembly.instantiate(module, {
      [ARTEMIS_IMPORT_MODULE]: { ...bootTable.imports, a: memory.memory as WebAssembly.Memory },
    });
    attach(created);
    instantiated = true;
  } catch (error) {
    /**
     * The catch that exists because `session` is not constructed yet: the session's own `dispose` is the
     * one disposer, and reaching it means wiring a throwaway around the very object that just failed.
     *
     * A partial `attach` is why this runs while the driver and the exports are still live — the boot
     * table may already hold exports whose allocations only this driver can name. The cleanup's own
     * failure is reported, never returned: the caller needs the instantiate or attach error, which is
     * the one that says what actually went wrong.
     */
    try {
      await lifecycle?.dispose();
    } catch (cleanupError) {
      report('cleanup', `the lifecycle failed to dispose: ${String(cleanupError)}`);
    }
    /**
     * The audio host is disposed here too, and *after* the lifecycle: the guest may still have had a
     * device open when the instantiate or the attach failed, and a context left running would outlive
     * the session that owns it. It is awaited, not fired and forgotten, because this function's promise
     * is the only place a caller can learn that the teardown finished.
     */
    try {
      await audio.dispose();
    } catch (cleanupError) {
      report('cleanup', `the audio host failed to dispose: ${String(cleanupError)}`);
    }
    try {
      assets.dispose();
    } catch (cleanupError) {
      report('cleanup', `the assets failed to dispose: ${String(cleanupError)}`);
    }
    driver.dispose('construction failed');
    instance = undefined;
    exports = {};
    return fail(error);
  }

  return session;
}
