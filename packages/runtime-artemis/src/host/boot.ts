import { createArtemisFdImports, type ArtemisFdHost } from './fd.js';
import { createArtemisRuntimeImports, ArtemisTempRet0, type ArtemisRuntimeHost } from './runtime.js';
import { createArtemisEhImports, type ArtemisEhHost } from './eh.js';
import {
  createArtemisAsyncImports,
  ARTEMIS_ASYNC_FORWARDS,
  ARTEMIS_ASYNC_HELPERS,
  ARTEMIS_ASYNC_OFFICIAL,
  type ArtemisAsyncHost,
  type ArtemisForwardedFn,
} from './async.js';
import { createArtemisAsyncifyImports, type ArtemisAsyncifyHost } from './asyncify.js';
import { createArtemisGlImports, type ArtemisGlHost } from './gl.js';
import { ARTEMIS_ERRNO } from './errno.js';
import type { ArtemisMemory } from './memory.js';
import type { ArtemisFs } from './tree.js';
import type { ImportFn, ArtemisImportTable } from './bridge.js';
import { initArtemisStandardStreams, type ArtemisStandardStreams } from './bridge.js';

/**
 * Assembling every bucket the host has implemented, and naming the rest.
 *
 * This module exists because of one fact about wasm: **an import is not optional.** The bridge
 * declares 207 functions under the module name `a`, and the engine refuses to instantiate until
 * all 207 are supplied with something callable. A host that implements 197 of them and hands the
 * engine nothing for the other 1 does not boot — it fails to link, with a message that names the
 * first missing field and says nothing about which bucket it belonged to.
 *
 * So the remaining rows are supplied, but as something that cannot be mistaken for an
 * implementation: a function that **counts the call**, records the bucket, and answers with the
 * documented refusal — a negative errno for `_system`, a zero for the rest. The count is the point.
 * It turns "the engine is stuck" into "the engine is stuck *here*, at this short name, in this
 * bucket", which is the difference between a bug report and a to-do item.
 *
 * The one row that must never be a silent zero is `Xb` (`_system`). It is the whole `todo` bucket,
 * it has a written reason, and the whole point of the reason is that a port which reaches it is
 * broken. A stub that returned 0 there would let a broken port look like a working one.
 */

/** One row of the bridge's import surface, as the committed classification table records it. */
export interface ArtemisImportRow {
  readonly index: number;
  readonly name: string;
  readonly official: string;
  readonly bucket: string;
}

/**
 * The committed classification table, reduced to what a boot needs.
 *
 * `buckets` is the spec's own bucket → **row count** map, kept because a coverage report has to
 * reconcile its own totals against the spec's and a mismatch means a row was lost somewhere. It is
 * not used to decide what to stub: `rows` is the authority for that, so a stale count in `buckets`
 * can produce a wrong report but never a wrong import table.
 */
export interface ArtemisImportSpec {
  readonly rows: readonly ArtemisImportRow[];
  /** Bucket → how many rows the spec puts in it. */
  readonly buckets: Readonly<Record<string, number>>;
}

/** What one stubbed call did, kept so a session can print the exact sequence it walked. */
export interface ArtemisBlockedCall {
  readonly name: string;
  readonly official: string;
  readonly bucket: string;
  readonly args: readonly number[];
}

/** Everything a boot session can read back about what the guest asked for. */
export interface ArtemisBootTrace {
  /** Short name → times called, across every bucket, implemented or stubbed. */
  readonly counts: ReadonlyMap<string, number>;
  /** Short name → official name, for a report a human can read. */
  readonly official: Readonly<Record<string, string>>;
  /** Short name → bucket, as the committed table classifies it. */
  readonly buckets: Readonly<Record<string, string>>;
  /** Every call that landed on a stub, in order. */
  readonly blocked: readonly ArtemisBlockedCall[];
  /**
   * Totals per bucket, recomputed on every read so it can never drift from {@link counts}.
   * A cached number here would be a second source of truth about the same run, and the whole value
   * of this module is that there is only one.
   */
  readonly byBucket: ReadonlyMap<string, number>;
  /** Zeroes the counters. A session that boots more than once needs this between runs. */
  reset(): void;
}

/** The 7 short names that appear twice in the import section, and must therefore serve two rows. */
const DUPLICATED_NAMES: ReadonlySet<string> = new Set(['Ea', 'I', 'Jb', 'Ob', 'Q', 'Ta', 'Ub', 'V', 'xb']);

/** What a boot session supplies beyond what the `fd` bucket needs. */
export interface ArtemisBootSetup {
  readonly memory: ArtemisMemory;
  readonly fs: ArtemisFs;
  /** The whole committed table, so the stub set and the coverage report come from one source. */
  readonly spec: ArtemisImportSpec;
  /** A pointer to the guest's `int errno`, for the imports that set it. */
  readonly errnoLocation?: () => number;
  /** The guest's `malloc`. Used by the fd, `eh` and `asyncify` buckets. */
  readonly malloc?: (bytes: number) => number;
  /** The guest's `free`. */
  readonly free?: (pointer: number) => void;
  /** Overrides `Date.now`, so a test can make the clock deterministic. */
  readonly now?: () => number;
  /** Merged over the default environment block, exactly as the glue merges its `ENV` global. */
  readonly env?: Record<string, string>;
  /** Runs an `EM_ASM` snippet. Absent means every `EM_ASM` reports a block. */
  readonly runAsmConst?: ArtemisRuntimeHost['runAsmConst'];
  /** Evaluates guest-supplied script. Absent means `_emscripten_run_script_string` refuses. */
  readonly evaluate?: ArtemisRuntimeHost['evaluate'];
  /** Fetches a URL for the asyncify download rows. Absent means they report a block. */
  readonly resolve?: ArtemisAsyncifyHost['resolve'];
  /** Calls a guest function-table entry — the host's stand-in for the glue's `getDynCaller`. */
  readonly callback?: ArtemisAsyncifyHost['callback'];
  /** Caps heap growth below the module's own 65536-page maximum. */
  readonly maxHeapBytes?: number;
  /**
   * The `gl` bucket's browser half, handed over as one object so a headless session is a single
   * absent field rather than a table built with a dozen holes.
   *
   * Everything here is optional on purpose. `_glfwInit` and `_glfwCreateWindow` without a canvas
   * answer 0 where the official glue answers a live handle, and the guest stops at its own display
   * check — which is the honest outcome, and is recorded as a block instead of being papered over
   * with a context that draws nothing. `dynCallV` is filled in by {@link ArtemisBootTable.attachExports}
   * rather than here, because it is an export of the bridge and the instance does not exist yet.
   */
  readonly gl?: Omit<ArtemisGlHost, 'memory' | 'onBlocked'>;
  /**
   * Runs after `__ATINIT__` and before `_main`, standing in for the glue's `preMain` line
   * `FS.ignorePermissions = false` and for `__ATMAIN__`.
   *
   * Both are glue-side, not guest-side, so a host has nothing to do here and the default is to do
   * nothing. The hook exists because a real session has one thing the glue does not: it decides
   * what a write to a mounted package layer means, and it should be able to do that in the official
   * slot rather than inventing a step of its own.
   */
  readonly preMain?: () => void;
  /** Reports a call this host could not serve. */
  readonly onBlocked?: (what: string, detail: string) => void;
}

/** The import table a boot needs, plus the trace that says what the guest did with it. */
export interface ArtemisBootTable {
  readonly imports: ArtemisImportTable;
  readonly trace: ArtemisBootTrace;
  /** The `setTempRet0` cell, shared by the `runtime` and `eh` tables so a matched type survives. */
  readonly temp: ArtemisTempRet0;
  /**
   * Gives the `eh` and `async` tables the guest exports they need. Must be called after
   * `WebAssembly.instantiate` returns, because all of them are exports of the bridge: `$c`/`ad`
   * for `eh`, then `Xc`/`Yc`/`_c` and the 30 dynCalls for `async`.
   */
  attachExports(instance: WebAssembly.Instance): void;
  /**
   * Runs the official startup sequence, in the official order, against this instance.
   *
   * The sequence is spelled out on the table's own `boot` implementation, which is where the
   * `initRuntime` / `preMain` / `callMain` bodies from the glue are quoted. A caller that
   * only wants an import table never has to call it; a caller that wants a running game does, and
   * calling it twice is harmless because the guest's own init is idempotent.
   */
  boot(instance: WebAssembly.Instance, args?: readonly string[]): ArtemisBootResult;
}

/** What {@link ArtemisBootTable.boot} observed, so a session can report rather than guess. */
export interface ArtemisBootResult {
  /** `argv[0]`, which the glue sets to `./this.program` and the guest's own `main` may read. */
  readonly thisProgram: string;
  /** The pointer `argv` was built at, for anyone who wants to read the guest's view of it. */
  readonly argv: number;
  /** How many of the startup steps ran, in the order they ran. `Mc` is the third. */
  readonly steps: readonly string[];
  /**
   * `undefined` when `_main` **returned**, or the value it threw.
   *
   * A return is a real possibility: `_main` returning is how the glue's `callMain` learns the exit
   * code, and it is the single most informative thing a boot can report — a returned `_main` has
   * run to completion rather than being stopped.
   */
  readonly returned?: number;
  readonly threw?: unknown;
}

/**
 * Builds the complete 207-name import table: 197 names implemented, 1 counted stub.
 *
 * The implemented set is the union of the six buckets this package has — `fd` (41), `runtime` (8),
 * `eh` (15), `asyncify` (4), `async` (30) and `gl` (99) — which is 197 distinct short names covering
 * 206 of the 207 rows, the difference being the duplicated names that serve two rows each. All that
 * is left is `todo` (1), which is `_system`: a deliberate refusal that answers ENOSYS.
 *
 * The `gl` bucket is different from the others in kind, not just in size. Its 99 rows do not answer
 * questions about a filesystem — they answer questions about a rendering context that only exists in a
 * browser, so the bucket takes a canvas from the session and returns a **handle** where the others
 * return a byte count. Within it, 79 rows are real — the GLFW boot path, the whole `_gl*` draw path,
 * the main loop and the 11 `__emscripten_set_*_callback_on_thread` event registrations — and 20 are
 * documented per-call refusals: the 19 `_al*`/`_alc*` audio rows plus
 * `_emscripten_enter_soft_fullscreen`, which returns `-4` for an absent target and refuses otherwise.
 * The event rows are implemented, but with no document to register against (a Node run has none) they
 * answer `EMSCRIPTEN_RESULT_INVALID_TARGET`; that is a per-call answer, not a refusal. The 20 refusals
 * are supplied rather than left as the family-wide zero that made a stalled engine indistinguishable
 * from a working one, and each one reports through `onBlocked` the first time it is asked to do
 * something it will not do.
 * `.test/team-artemis/count-coverage.mjs` measures all of that from the compiled tables rather than
 * counting by hand.
 *
 * Everything else comes straight from the committed table, so adding a bucket is a matter of
 * implementing it and letting the spec's classification decide what falls back to a stub. There is
 * no second, hand-maintained list to drift out of sync.
 *
 * **The memory is supplied by the session, not allocated here.** The bridge imports its memory as
 * `a.a`, so the instance is constructed against whatever `WebAssembly.Memory` the caller passes;
 * an `ArtemisMemory` built here could disagree with it about growth and the two would silently
 * diverge. The session that constructs the instance owns both and hands this module the matching
 * view.
 */
export function createArtemisBootTable(setup: ArtemisBootSetup): ArtemisBootTable {
  const { memory, spec, fs } = setup;
  const counts = new Map<string, number>();
  const blocked: ArtemisBlockedCall[] = [];

  /** Bucket and official name, keyed by short name, from the committed table. */
  const bucketOf: Record<string, string> = {};
  const officialOf: Record<string, string> = {};
  for (const row of spec.rows) {
    if (!(row.name in bucketOf)) {
      bucketOf[row.name] = row.bucket;
      officialOf[row.name] = row.official;
    }
  }

  const report = (what: string, detail: string): void => setup.onBlocked?.(what, detail);

  /** `FS.init.initialized`, the glue's own guard against opening the standard streams twice. */
  let fsInitialised = false;
  /** The fds `createStandardStreams` opened, for a session that wants to route them. */
  let stdio: ArtemisStandardStreams | undefined;

  /**
   * The `setTempRet0` cell. Both the `runtime` and the `eh` table must hold the *same* one: the
   * `___cxa_find_matching_catch_*` family returns a matched type through it, and two cells would
   * lose the type between the two tables that use it.
   */
  const temp = new ArtemisTempRet0();

  const fdHost: ArtemisFdHost = {
    memory,
    fs: setup.fs,
    errnoLocation: setup.errnoLocation,
    malloc: setup.malloc,
    now: setup.now,
    env: setup.env,
    onBlocked: report,
  };
  const runtimeHost: ArtemisRuntimeHost = {
    memory,
    temp,
    runAsmConst: setup.runAsmConst,
    malloc: setup.malloc,
    free: setup.free,
    evaluate: setup.evaluate,
    maxHeapBytes: setup.maxHeapBytes,
    onBlocked: report,
  };
  /**
   * The `eh` host is held by reference so the two guest exports can be filled in after
   * instantiation. `___cxa_can_catch` and `___cxa_is_pointer_type` are *exports* of the bridge, not
   * imports, so a session that wired them at construction time would have no instance to read them
   * from — and a `find_matching_catch` without a real subobject check silently accepts a handler
   * the engine would have rejected.
   */
  const ehHost: ArtemisEhHost = {
    memory,
    malloc: setup.malloc,
    free: setup.free,
    temp,
    onBlocked: report,
  };
  const asyncifyHost: ArtemisAsyncifyHost = {
    memory,
    malloc: setup.malloc,
    free: setup.free,
    callback: setup.callback,
    resolve: setup.resolve,
    onBlocked: report,
  };
  /**
   * Held by reference for the same reason as `ehHost`: every field it ends up with is a wasm
   * *export* of the bridge, and the import table has to exist before the instance does.
   */
  const asyncHost: ArtemisAsyncHost = { onBlocked: report };
  /**
   * Also held by reference, and for a second reason on top of `ehHost`'s: the canvas, the WebGL
   * context and the frame source do not exist until a session has a DOM to take them from. A table
   * built at construction time could only ever see `undefined`, so `gl` gets its host object first and
   * its fields later. `dynCallV` is the guest's `rd`, attached alongside `ehHost`'s `$c`/`ad`.
   */
  const glHost: ArtemisGlHost = {
    ...setup.gl,
    memory,
    // The five event blocks are guest heap, and `malloc` is the guest's allocator. Falling back to
    // the session's keeps one allocator in play: a block handed to a callback has to be the same
    // heap the rest of the guest owns. Without it the eleven event rows answer their neutral value
    // and report, which is honest but is not the same as registering.
    malloc: setup.gl?.malloc ?? setup.malloc,
    onBlocked: report,
  };

  const implemented: ArtemisImportTable = {
    ...(createArtemisFdImports(fdHost) as ArtemisImportTable),
    ...(createArtemisRuntimeImports(runtimeHost) as ArtemisImportTable),
    ...(createArtemisEhImports(ehHost) as ArtemisImportTable),
    ...(createArtemisAsyncifyImports(asyncifyHost) as ArtemisImportTable),
    ...(createArtemisAsyncImports(asyncHost) as ArtemisImportTable),
    ...(createArtemisGlImports(glHost) as ArtemisImportTable),
  };

  const imports: ArtemisImportTable = {};

  /**
   * Wraps a body so every call is counted, whatever happens inside it.
   *
   * The count is incremented *before* the body runs, so a call that throws — `_abort`, a C++
   * `throw`, a `longjmp` — is still attributed. A counter incremented after the call would lose
   * exactly the calls worth knowing about.
   */
  const counted = (name: string, body: ImportFn): ImportFn => {
    return (...args: number[]): number | void => {
      counts.set(name, (counts.get(name) ?? 0) + 1);
      return body(...args);
    };
  };

  for (const [name, body] of Object.entries(implemented)) {
    imports[name] = counted(name, body);
  }

  /**
   * A counted stub.
   *
   * `_system` is the exception, and it is the only one: it answers `-1` and sets `ENOSYS`, which
   * is the refusal the row's own reason demands ("a call has to fail, not fork — and fail
   * loudly"). Every other stub answers 0, because 0 is the honest "nothing happened" for a call
   * whose body was never written, and because a nonzero invented value would send the guest down a
   * path that does not exist in the real engine.
   */
  const stub = (name: string): ImportFn => {
    const official = officialOf[name] ?? name;
    const bucket = bucketOf[name] ?? 'unknown';
    if (name === 'Xb') {
      return counted(name, (): number => {
        blocked.push({ name, official, bucket, args: [] });
        report('system', '_system is refused: this host never forks a subprocess');
        const slot = (setup.errnoLocation?.() ?? 0) | 0;
        if (slot !== 0) memory.i32[slot >>> 2] = ARTEMIS_ERRNO.ENOSYS;
        return -1;
      });
    }
    return counted(name, (...args: number[]): number => {
      blocked.push({ name, official, bucket, args: [...args] });
      return 0;
    });
  };

  for (const row of spec.rows) {
    if (row.name in imports) continue;
    imports[row.name] = stub(row.name);
  }

  /**
   * Wires the guest helpers the `eh` and `async` tables need, after the instance exists.
   *
   * All of them are wasm *exports* of the bridge, so they can only be reached once
   * `WebAssembly.instantiate` has returned — which is after the import table was needed. That
   * ordering is why they arrive as a second call rather than a constructor argument.
   *
   * The two `eh` helpers are exported under the **minified short names** `$c` and `ad`, not under
   * the long C++ names. The long names are the *imports*; the exports are what the glue's `asm`
   * object holds. Looking only for the long names reports "unavailable" for two functions that are
   * right there, and a `find_matching_catch` with no real subobject check quietly accepts a handler
   * the engine would have rejected. Hence the short-name lookup, and hence the loud report if even
   * that fails.
   *
   * The `async` bucket needs three more — `Xc`, `Yc`, `_c` — and the 30 dynCalls, all under the
   * short names the glue's forwarders name. The dynCalls are the *whole* mechanism: the guest's own
   * export does the table lookup, the argument marshalling and the `__THREW__` bookkeeping, so a
   * missing one is reported by name rather than answered with a guess.
   *
   * The `gl` bucket needs one of them: `rd`, the `v` dynCall, because `_emscripten_set_main_loop`
   * stores a *function-table index* and the glue reaches it through `getDynCaller("v", func)`. With
   * no `rd`, that row cannot run a frame, so it refuses and still throws `unwind` — the guest parks
   * where the official one parks instead of quietly never being called.
   */
  const attachExports = (instance: WebAssembly.Instance): void => {
    const exports = instance.exports as Record<string, unknown>;
    const canCatch = exports['$c'];
    const isPointerType = exports['ad'];
    if (typeof canCatch !== 'function') {
      report('eh', 'the bridge exports neither $c nor ___cxa_can_catch, so base-class matching is unavailable');
    } else {
      ehHost.canCatch = (catchType: number, thrownType: number, adjustedPtr: number): number => {
        const call = canCatch as (a: number, b: number, c: number) => number;
        return call(catchType | 0, thrownType | 0, adjustedPtr | 0) | 0;
      };
    }
    if (typeof isPointerType !== 'function') {
      report('eh', 'the bridge exports neither ad nor ___cxa_is_pointer_type, so reference types are guessed');
    } else {
      ehHost.isPointerType = (type: number): number => {
        const call = isPointerType as (a: number) => number;
        return call(type | 0) | 0;
      };
    }

    /**
     * Binds one export by its short name, reporting rather than guessing if it is absent.
     *
     * `what` names the facility so the report reads as a diagnosis rather than a missing key, and
     * `bind` adapts the raw export to the shape the caller expects — which is where the `| 0`
     * coercion belongs, because a wasm i32 comes back as a signed number and the host's own types
     * are unsigned.
     */
    const bindExport = <T>(shortName: string, what: string, bind: (call: Function) => T): T | undefined => {
      const found = exports[shortName];
      if (typeof found !== 'function') {
        report('async', `the bridge does not export ${shortName}, so ${what} is unavailable`);
        return undefined;
      }
      return bind(found as Function);
    };

    asyncHost.stackSave = bindExport(ARTEMIS_ASYNC_HELPERS.stackSave, 'the guest stackSave', (call) => () =>
      (call() as number) | 0,
    );
    asyncHost.stackRestore = bindExport(
      ARTEMIS_ASYNC_HELPERS.stackRestore,
      'the guest stackRestore',
      (call) => (value: number) => {
        call(value | 0);
      },
    );
    asyncHost.setThrew = bindExport(ARTEMIS_ASYNC_HELPERS.setThrew, 'the guest _setThrew', (call) => (
      env: number,
      value: number,
    ) => {
      call(env | 0, value | 0);
    });

    const dynCall: Record<string, ArtemisForwardedFn> = {};
    for (const [name, exportName] of Object.entries(ARTEMIS_ASYNC_FORWARDS)) {
      const call = exports[exportName];
      if (typeof call !== 'function') {
        report('async', `the bridge does not export ${exportName}, so ${name} (${ARTEMIS_ASYNC_OFFICIAL[name] ?? name}) did not dispatch`);
        continue;
      }
      // Bound late on purpose: the row must see a *stable* function object, and every call forwards
      // its arguments verbatim, so no type is inspected here.
      dynCall[exportName] = call as ArtemisForwardedFn;
    }
    asyncHost.dynCall = dynCall;

    /**
     * `gl` gets its dynCall outside {@link bindExport}, because that helper reports every miss as
     * `async` — right for the 30 rows above, wrong for a row that belongs to `gl`. A mislabelled
     * block sends the next reader to the wrong bucket, and this whole module exists to stop a silent
     * zero from hiding where an engine stopped.
     */
    const dynCallV = exports['rd'];
    if (typeof dynCallV !== 'function') {
      report('gl', 'the bridge does not export rd, so _emscripten_set_main_loop cannot reach its frame function');
    } else {
      glHost.dynCallV = (tableIndex: number): void => {
        (dynCallV as (index: number) => void)(tableIndex | 0);
      };
    }

    /**
     * The event rows' own calling convention, `getDynCaller("iiii", callbackfunc)`.
     *
     * The guest passes its event callbacks as real table indexes (measured: 3706…3725), not as host
     * pointers, so a listener can only fire if the host can call INTO the guest's function table with
     * four i32s. Without this the eleven rows would register listeners that are never called — the
     * "registered a success and never called it" shape this bucket exists to reject — so an absent
     * `gd` is reported rather than left silent. It is the same forwarder `asyncHost.callback` uses,
     * which is why the session supplies it once, at `setup.callback`.
     */
    const dynCallIiii = exports['gd'];
    if (typeof dynCallIiii !== 'function') {
      report('gl', 'the bridge does not export gd, so a registered event callback could never be called back');
    } else {
      glHost.dynCallIiii = (entry: number, a: number, b: number, c: number): number =>
        (dynCallIiii as (index: number, a: number, b: number, c: number) => number)(
          entry | 0,
          a | 0,
          b | 0,
          c | 0,
        );
    }
  };

  /**
   * Bucket totals, derived from {@link counts} every time it is read. Deriving beats storing: a
   * stored total is a second thing to keep in step with the counts, and this module exists so that
   * there is exactly one record of what the guest did.
   */
  const byBucket = {
    get(key: string): number | undefined {
      let total = 0;
      for (const [name, count] of counts) {
        if (bucketOf[name] === key) total += count;
      }
      return total;
    },
    has(key: string): boolean {
      return Object.values(bucketOf).includes(key);
    },
    get size(): number {
      return new Set(Object.values(bucketOf)).size;
    },
    entries(): IterableIterator<[string, number]> {
      const buckets = [...new Set(Object.values(bucketOf))].sort();
      return buckets
        .map((bucket): [string, number] => [bucket, this.get(bucket) ?? 0])
        [Symbol.iterator]();
    },
    keys(): IterableIterator<string> {
      return [...new Set(Object.values(bucketOf))].sort()[Symbol.iterator]();
    },
    values(): IterableIterator<number> {
      return [...this.entries()].map(([, total]) => total)[Symbol.iterator]();
    },
    forEach(fn: (value: number, key: string, map: ReadonlyMap<string, number>) => void): void {
      for (const [bucket, total] of this.entries()) fn(total, bucket, byBucket);
    },
    [Symbol.iterator](): IterableIterator<[string, number]> {
      return this.entries();
    },
  } satisfies ReadonlyMap<string, number>;

  const trace: ArtemisBootTrace = {
    counts,
    official: officialOf,
    buckets: bucketOf,
    blocked,
    byBucket,
    reset(): void {
      counts.clear();
      blocked.length = 0;
    },
  };

  /**
   * The official startup sequence, in the official order.
   *
   * The order is not cosmetic, and getting it wrong is what left the guest unable to run. Read
   * straight out of the glue (`ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js`):
   *
   * ```js
   * function initRuntime() { runtimeInitialized = true; if (!Module["noFSInit"] && !FS.init.initialized) FS.init();
   *                           TTY.init(); SOCKFS.root = FS.mount(SOCKFS, {}, null); callRuntimeCallbacks(__ATINIT__) }
   * function preMain()     { FS.ignorePermissions = false; callRuntimeCallbacks(__ATMAIN__) }
   * ```
   *
   * Three things about it are easy to get wrong, and all three were wrong in the first attempt:
   *
   * - **`___wasm_call_ctors` runs *inside* `initRuntime`.** It is the one member of `__ATINIT__`,
   *   and the boot used to call it first of all. Calling it earlier is not a harmless reorder: the
   *   constructors allocate through the same heap and register the same statics that `FS.init` is
   *   about to make visible, and a guest that inspects a static during construction sees nothing.
   * - **`preMain` is not a no-op.** `FS.ignorePermissions = false` is the line that turns the glue's
   *   permissive default off. A host that skipped it would hand the guest a filesystem in which
   *   every write succeeds, and a read-only package layer would be read-write.
   * - **The standard streams are opened here, not later.** `FS.init` ends in `createStandardStreams`,
   *   which opens fd 0 on `/dev/stdin` and fds 1 and 1 on `/dev/stdout` and `/dev/stderr`. Skipping it
   *   leaves the guest with no fds at all, and the first `fprintf(stderr, …)` becomes a bad-fd error
   *   rather than a message.
   *
   * `SOCKFS` has no counterpart here and is deliberately not faked: it is a mount point, and a
   * socket is created by `socketcall`, which is a `gl` row this host does not implement yet. An
   * empty mount would be a directory that claims to be a filesystem.
   *
   * `argv` is built exactly as the glue's `callMain` builds it, with the guest's own `Zc`
   * (`stackAlloc`): `argc + 1` slots, `argv[0]` a pointer to `./this.program` pushed with
   * `allocateUTF8OnStack`, and `argv[argc] = 0`. The terminator is what makes the array safe to
   * iterate, and a host that omits it hands the guest a read past the end of its own arguments.
   */
  const boot = (instance: WebAssembly.Instance, args: readonly string[] = []): ArtemisBootResult => {
    const steps: string[] = [];
    const exports = instance.exports as Record<string, unknown>;

    steps.push('initRuntime: FS.init, TTY.init, SOCKFS root');
    // `FS.init` is guarded by `!FS.init.initialized`; this host tracks the same guard so a second
    // boot of the same instance does not re-open the standard streams onto new fds.
    if (!fsInitialised) {
      const streams = initArtemisStandardStreams(fs);
      stdio = streams;
      fsInitialised = true;
    }

    // `__ATINIT__` has exactly one member, and it is the constructor pass. The export the glue calls
    // by its long name is the bridge's `Mc`; the one *literally named* `__wasm_call_ctors` is inert
    // and reads nothing — a difference the bytes do not announce.
    const callCtors = exports.Mc;
    if (typeof callCtors !== 'function') {
      report('boot', 'the bridge exports no Mc, so the guest constructors never ran');
    } else {
      steps.push('__ATINIT__: Mc (guest constructors)');
      (callCtors as () => void)();
    }

    steps.push('preMain: FS.ignorePermissions, __ATMAIN__');
    // `__ATMAIN__` is empty in this build, and `__ATPOSTRUN__` has one member that is a browser
    // event-listener teardown — neither belongs in a host. They are named here so the shape of the
    // official sequence is visible rather than implied.
    setup.preMain?.();

    // `callMain(args)`: argc+1 slots from the guest's own stackAlloc, argv[0] = thisProgram,
    // argv[argc] = 0, then `entryFunction(argc, argv)`.
    const stackAlloc = exports.Zc as ((bytes: number) => number) | undefined;
    const argc = args.length + 1;
    const argvBytes = (argc + 1) * 4;
    const argv = typeof stackAlloc === 'function' ? stackAlloc(argvBytes) | 0 : 0;
    if (argv === 0) {
      report('boot', 'the bridge exports no Zc, so argv could not be allocated with the guest stack');
    }
    const putWord = (slot: number, value: number): void => {
      memory.i32[((argv + slot * 4) >>> 2)] = value;
    };
    const thisProgram = './this.program';
    // `allocateUTF8OnStack(str)` is `stackAlloc(len+1)` then `stringToUTF8Array`, so the pointer
    // comes from the guest's allocator rather than from a host address.
    const encoder = new TextEncoder();
    const programUtf8 = encoder.encode(thisProgram);
    const programPtr =
      typeof stackAlloc === 'function' ? stackAlloc(programUtf8.length + 1) | 0 : 0;
    if (programPtr !== 0) memory.u8.set(programUtf8, programPtr);
    putWord(0, programPtr);
    for (let i = 1; i < argc; i += 1) {
      const encoded = encoder.encode(args[i - 1] ?? '');
      const ptr = typeof stackAlloc === 'function' ? stackAlloc(encoded.length + 1) | 0 : 0;
      if (ptr !== 0) memory.u8.set(encoded, ptr);
      putWord(i, ptr);
    }
    putWord(argc, 0);

    steps.push(`callMain: Qc(${argc}, ${argv})`);
    const main = exports.Qc as ((c: number, v: number) => number) | undefined;
    const result: ArtemisBootResult = { thisProgram, argv, steps };
    if (typeof main !== 'function') {
      report('boot', 'the bridge exports no Qc, so _main was never called');
      return result;
    }
    try {
      return Object.assign(result, { returned: main(argc, argv) | 0, steps: [...steps, '_main returned'] });
    } catch (error) {
      // `callMain` re-raises anything that is not an `ExitStatus` or the literal `"unwind"`; this
      // host records it instead of dying, because a session that cannot see why the guest stopped
      // cannot fix it.
      return Object.assign(result, { threw: error, steps: [...steps, `_main threw ${String(error)}`] });
    }
  };

  return { imports, trace, temp, attachExports, boot };
}

/** Formats a trace as the bucket → short name → call count table a session reports. */
export function formatBootTrace(trace: ArtemisBootTrace): string {
  const lines: string[] = [];
  const namesByBucket = new Map<string, string[]>();
  for (const [name, bucket] of Object.entries(trace.buckets)) {
    const list = namesByBucket.get(bucket) ?? [];
    list.push(name);
    namesByBucket.set(bucket, list);
  }
  for (const bucket of [...namesByBucket.keys()].sort()) {
    const names = (namesByBucket.get(bucket) ?? []).slice().sort();
    const total = trace.byBucket.get(bucket) ?? 0;
    const called = names.filter((name) => (trace.counts.get(name) ?? 0) > 0);
    lines.push(
      `${bucket}  ${total} call${total === 1 ? '' : 's'}  ` +
        `(${names.length} names, ${called.length} called)`,
    );
    for (const name of names) {
      const count = trace.counts.get(name) ?? 0;
      const official = trace.official[name] ?? name;
      lines.push(`  ${name.padEnd(4)} ${String(count).padStart(4)}  ${official}`);
    }
  }
  return lines.join('\n');
}

/** The duplicated short names, exported so a coverage report can explain a row/name mismatch. */
export const ARTEMIS_DUPLICATED_NAMES: ReadonlySet<string> = DUPLICATED_NAMES;

/** Re-exported so a session can build a refusal for `_system` without importing `errno.js`. */
export const ARTEMIS_SYSTEM_ERRNO: number = ARTEMIS_ERRNO.ENOSYS;
