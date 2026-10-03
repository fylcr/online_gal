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
import { createArtemisAsyncifyImports, ARTEMIS_ASYNCIFY_IMPORTS, type ArtemisAsyncifyHost } from './asyncify.js';
import { createArtemisGlImports, type ArtemisGlHost } from './gl.js';
import { ARTEMIS_ERRNO, ArtemisHostError } from './errno.js';
import { ASYNCIFY_STATE, type ArtemisSuspendDriver } from './suspend.js';
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
  /**
   * Short name → times called, across every bucket, implemented or stubbed.
   *
   * **The number counts entries, not successes.** It is incremented *before* the body runs, so a
   * call that throws or refuses is in here too. A row's *outcome* is {@link honoured}.
   */
  readonly counts: ReadonlyMap<string, number>;
  /**
   * The short names this boot **honoured**, as a measured set: a row joins only when a wrapped
   * **synchronous implemented import body returned normally** during a call. It is not a service or
   * game-correctness claim — a member says only that this body completed synchronously.
   *
   * Rows are absent when the body threw, when it reported or refused anything, when it parked
   * (a returned thenable, or an asyncify unwind), and when the row is stubbed. Excluded wholesale,
   * whether or not some later call would have succeeded: the async forwarders, `fd`, `B` and `nc`
   * ({@link nonSynchronous}). So absence proves no failure either — a name may never have been called.
   */
  readonly honoured: ReadonlySet<string>;
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
  /** The arity-checked form of {@link ArtemisBootSetup.callback}, preferred when both are supplied. */
  readonly invokeCallback?: ArtemisAsyncifyHost['invokeCallback'];
  /** Defers one `Qa`/`Jc` completion out of the current import call. */
  readonly enqueue?: ArtemisAsyncifyHost['enqueue'];
  /** Whether a table-callback is still deliverable after a suspension. */
  readonly isDisposed?: ArtemisAsyncifyHost['isDisposed'];
  /** The synchronous key/value store behind the eight `_emscripten_idb_*` rows. */
  readonly idb?: ArtemisFdHost['idb'];
  /** The fd bucket's own callback convention, which carries an extra `count` argument. */
  readonly fdCallback?: ArtemisFdHost['callback'];
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
  /**
   * The asyncify driver, which is what makes {@link ArtemisBootTable.bootAsync} a real sequence
   * rather than a synchronous one with `await`s sprinkled over it.
   *
   * Supplied as a `Pick` of the driver rather than as the class, for the same reason the `asyncify`
   * bucket takes `sleep` as one method: the boot needs two things from it — the `sleep` seam the
   * guest's own import is built from, and `waitUntilIdle()` to know when the guest has actually
   * finished unwinding and rewinding — and nothing to do with the unwind/rewind control surface.
   *
   * Absent is a supported configuration and not a failure: `bootAsync` still runs the whole
   * sequence, but every await is a plain tick, because with no driver there is no guest suspension
   * to wait out. What must **not** happen is a guest that suspends with nothing driving it, so a
   * row that *needs* the seam (the `asyncify` bucket's `fa`) reports a block instead of pretending
   * the fetch failed, and the boot never answers a suspension `ok`.
   *
   * If it is supplied, `sleep` is installed into the `asyncify` bucket's host for this table, which
   * is the same wiring `ArtemisAsyncifyHost.sleep` documents; a table built by an earlier worker
   * without this field keeps the old behaviour exactly.
   *
   * `wrap` and `result` are the other two halves of the same contract, and both come from the class
   * rather than being reimplemented here: `wrap(name, raw)` is what turns a raw export into one that
   * survives an unwind, and it replays the *same* raw function with the *same* receiver and args, so
   * the boot must hand it the raw export rather than ever calling it directly. `result` is where the
   * final return value lands (`null` for a void return) — a wrapped call's own return is the
   * placeholder, not the answer.
   */
  readonly suspend?: Pick<ArtemisSuspendDriver, 'sleep' | 'waitUntilIdle' | 'wrap' | 'result' | 'state'>;
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
  /**
   * The same sequence, awaited, for a guest compiled with asyncify.
   *
   * Each of the four startup steps that can take time — the constructor pass, every UTF-8 write
   * into the heap, `_main` itself, and the stack restore — is driven by the asyncify driver: the
   * raw export is wrapped so an unwind is caught, the driver is awaited to idle, and the guest is
   * replayed to completion. The sequence, the order and the values are the *same* as {@link boot};
   * what changes is that a step which suspends is a step that is waited out rather than a step that
   * is over.
   *
   * The control and stack helpers (`Xc`/`Yc` stackSave/stackRestore, `Zc` stackAlloc) are raw on
   * purpose. Wrapping stackAlloc would make `argv` an async call, and the guest's own `callMain`
   * allocates it synchronously; wrapping stackRestore would let a rewind re-enter the one call that
   * must only ever undo what the first pass did.
   *
   * A missing `Mc`, `Qc` or `Zc` is a rejection, not a partially-successful boot: the guest cannot
   * have started without them, and reporting `ok` would be the silent-success failure this module
   * exists to prevent.
   */
  bootAsync(instance: WebAssembly.Instance, args?: readonly string[]): Promise<ArtemisBootResult>;
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
  /**
   * Set by {@link ArtemisBootTable.bootAsync} when `_main` threw the literal string `"unwind"`.
   *
   * That throw is not a failure: it is how an asyncify guest enters the GLFW frame loop the glue's
   * own `_emscripten_set_main_loop` installs, and the official `callMain` treats it as "the program is
   * now running" (`else if (e == "unwind") { noExitRuntime = true; return }`). `threw` stays set
   * alongside it, so a caller that wants the raw event still has it; a caller that wants to know
   * whether the guest is alive reads this. Absent everywhere else — a synchronous {@link
   * ArtemisBootTable.boot} never sets it, because a guest that reaches the loop without asyncify has
   * not parked, it has ended.
   */
  readonly mainLoopStarted?: boolean;
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

  /**
   * The measured honoured set. See {@link ArtemisBootTrace.honoured} for the definition; the
   * mechanisms that keep it honest are the two below.
   */
  const honoured = new Set<string>();

  /**
   * Bumped by every {@link report}, and used as the *outcome* half of the honoured rule.
   *
   * A report is the only way this module says "this call could not be served", and it is raised
   * from inside the very call whose outcome is in question — `report` is closed over, not searched
   * for. So the epoch is captured before the body runs and compared after it returns: a call that
   * completed *and* raised no report is the one case that counts. A body that reports and then
   * returns normally is a refusal that returned a neutral value, and it must not be promoted to an
   * honoured row merely because it did not throw.
   */
  let reportEpoch = 0;

  /** Bucket and official name, keyed by short name, from the committed table. */
  const bucketOf: Record<string, string> = {};
  const officialOf: Record<string, string> = {};
  for (const row of spec.rows) {
    if (!(row.name in bucketOf)) {
      bucketOf[row.name] = row.bucket;
      officialOf[row.name] = row.official;
    }
  }

  /**
   * Reports one refusal.
   *
   * The reporter is never trusted to be well-behaved: a report is a *diagnosis*, always secondary to
   * whatever the caller was doing, and it is raised from inside `catch` blocks and from rejection
   * handlers where a logger that throws would replace the failure being described with an unrelated
   * exception — or orphan the real one. Its own failure has nowhere to go, so it is dropped.
   */
  const report = (what: string, detail: string): void => {
    reportEpoch += 1;
    try {
      setup.onBlocked?.(what, detail);
    } catch {
      // Deliberately swallowed: see above.
    }
  };

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
    free: setup.free,
    now: setup.now,
    env: setup.env,
    idb: setup.idb,
    // The writer's convention is the fd bucket's own (`fdCallback`), which carries an extra `count`
    // argument; falling back to `setup.callback` would call it with a `statusText` string where it
    // expects a length.
    callback: setup.fdCallback,
    // The same two seams the asyncify bucket takes. Without them the three async `_emscripten_idb_*`
    // rows would deliver their callbacks on a microtask while the session's own queue is mid-root,
    // and would free a buffer after the driver had already disposed the guest.
    enqueue: setup.enqueue,
    isDisposed: setup.isDisposed,
    onBlocked: report,
  };
  /**
   * The `fd` table, built here rather than inline into {@link implemented} further down, because its
   * own keys are what {@link nonSynchronous} excludes: a body that hands its work to a callback has
   * not completed by the time it returns, so the wrapper cannot call it honoured. Built once and
   * spread into `implemented` — building it twice would give the exclusion set a second, unrelated
   * set of keys.
   */
  const fdImports = createArtemisFdImports(fdHost);

  const nonSynchronous: ReadonlySet<string> = new Set([
    ...Object.keys(ARTEMIS_ASYNC_FORWARDS),
    ...ARTEMIS_ASYNCIFY_IMPORTS,
    ...Object.keys(fdImports),
    'B',
    'nc',
  ]);

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
    invokeCallback: setup.invokeCallback,
    enqueue: setup.enqueue,
    isDisposed: setup.isDisposed,
    resolve: setup.resolve,
    // The guest's own `sleep` import, wired to the driver when a session supplies one. This is the
    // seam `fa` (the blocking download) suspends through; the two callback-shaped rows never touch
    // it, whatever their resolver answers.
    sleep: setup.suspend ? (thunk) => setup.suspend!.sleep(thunk) : undefined,
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
    free: setup.gl?.free ?? setup.free,
    onBlocked: report,
  };

  const implemented: ArtemisImportTable = {
    ...(fdImports as ArtemisImportTable),
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
   * {@link ArtemisBootTrace.counts} counts *entries*, not successes: the increment happens before the
   * body runs, so a call that throws (`_abort`, a C++ `throw`, a `longjmp`) is still attributed, and
   * {@link honoured} is the separate measured outcome of the same run, added here in one place
   * because this is the only wrapper every implemented row goes through.
   *
   * A row joins `honoured` only when its body returned without throwing, reported nothing
   * ({@link reportEpoch} unchanged), did not park — no returned thenable, and no asyncify unwind
   * caught through the driver's state, where the promise is held rather than returned — and is not a
   * stub (`implementedOnly`; most stubs answer a neutral `0` without reporting, so a stub is excluded
   * by construction and {@link blocked} stays the record of which rows were stubbed). Rows in
   * {@link nonSynchronous} never join, by name rather than by outcome. `void` is a normal return and
   * does join.
   */
  const counted = (name: string, body: ImportFn, implementedOnly = true): ImportFn => {
    return (...args: number[]): number | void => {
      counts.set(name, (counts.get(name) ?? 0) + 1);
      const epoch = reportEpoch;
      let answered: number | void;
      try {
        answered = body(...args);
      } catch (error) {
        // Not honoured: the call did not complete. The reason still travels to the caller.
        throw error;
      }
      if (implementedOnly && epoch === reportEpoch) {
        // A thenable means the call deferred rather than finished, whatever it eventually does.
        // The shape is read through `unknown` because {@link ImportFn} declares `number | void`: the
        // async and asyncify rows are *cast* into this table, so a real park cannot be expressed in
        // the type and has to be detected at runtime or not at all.
        const shape = answered as unknown;
        const thenable =
          typeof shape === 'object' &&
          shape !== null &&
          'then' in shape &&
          typeof (shape as { then?: unknown }).then === 'function';
        // The other half of the same fact, and the one a thenable check cannot see: an asyncify row
        // (`fa`, `_emscripten_wget_data`) parks by calling `startUnwind` and then returning a
        // *provisional* `void` while the guest spills its stack. The promise is held by the driver,
        // not by this return, so there is nothing thenable here to test — the only evidence is the
        // driver's asyncify state, and `ArtemisSuspendDriver.#enter` names exactly this signal: "the
        // only signal that the call parked is the state global being `Unwinding` when the raw call
        // comes back". That state is reset to `Normal` at the *root* boundary, which is reached only
        // after this wrapper has already returned, so `Unwinding` here is a park and nothing else.
        // `Rewinding` is deliberately *not* excluded: a replay is a real synchronous execution, so a
        // row that returns normally on that pass has genuinely completed.
        const parked = thenable ||
          setup.suspend?.state === ASYNCIFY_STATE.Unwinding ||
          // The same fact for the rows whose forwarder is asynchronous *by design*:
          // {@link nonSynchronous} is the list, and the two tests above cannot see any of them — a
          // wget forwarder returns `void` while its request is open, and `716748` returns a 0 that
          // means "queued", not "written".
          nonSynchronous.has(name);
        if (!parked) honoured.add(name);
      }
      return answered;
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
      return counted(
        name,
        (): number => {
          blocked.push({ name, official, bucket, args: [] });
          report('system', '_system is refused: this host never forks a subprocess');
          const slot = (setup.errnoLocation?.() ?? 0) | 0;
          if (slot !== 0) memory.i32[slot >>> 2] = ARTEMIS_ERRNO.ENOSYS;
          return -1;
        },
        false,
      );
    }
    return counted(
      name,
      (...args: number[]): number => {
        blocked.push({ name, official, bucket, args: [...args] });
        return 0;
      },
      false,
    );
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
      /**
       * `rd` is an *exterior root*: JavaScript enters the guest here, so this is the one boundary where
       * an unwind can be observed and stopped. `driver.wrap` is what makes it survive one — it replays
       * the *same* raw export with the same receiver and args — and the returned promise is what tells
       * the `gl` main-loop pump the frame is genuinely over rather than merely returned early.
       *
       * The raw export is never called directly when a driver exists, and the driver's own return is not
       * the frame's value: `rd` is void, so what matters is only that the wait settled.
       *
       * Without a driver there is no unwinder to talk to, so the call keeps the exact synchronous shape
       * it has always had: void in, void out, `undefined` returned.
       */
      const rawDynCallV = (index: number): void => {
        (dynCallV as (index: number) => void)(index | 0);
      };
      if (!setup.suspend) {
        glHost.dynCallV = rawDynCallV;
      } else {
        const driver = setup.suspend;
        // One wrapper, built once: `wrap` keys its bookkeeping off the function object it was given, so
        // a fresh wrapper per frame would be a fresh root every time.
        const wrappedDynCallV = driver.wrap('rd', rawDynCallV);
        glHost.dynCallV = (entry: number): void | Promise<void> =>
          Promise.resolve(wrappedDynCallV(entry | 0)).then(async (): Promise<void> => {
            await driver.waitUntilIdle();
          });
      }
    }

    /**
     * The event rows' own calling convention, and the official glue's own lookup order.
     *
     * `artemis.js` has five event sites — keydown, mouse, touch, visibility, wheel — and all five
     * are one shape:
     *
     * ```js
     * if (getDynCaller("iiii", callbackfunc)(eventTypeId, eventDataBlock, userData)) e.preventDefault();
     * ```
     *
     * Four actual arguments, an i32 result used as a boolean, and `getDynCaller` resolving the *table
     * entry* first. The exported `dynCall_iiii` is only the fallback for a slot the table cannot give.
     * Both halves of that are read out of the shipped glue rather than reasoned about: `dynCall_viiii`
     * appears nowhere in the file except inside the `invoke_viiii` asyncify wrapper, so the 5-param
     * shape was never this contract, and the four-argument shape is what the vendored events use.
     *
     * The table is exported as `Lc`. Measured on the shipped bridge by instantiating it: `Lc` is a
     * `WebAssembly.Table` of 3,613 slots and slot 0 is the only null one; the official module has the
     * same shape at 4,278 slots, so the alias is the official scheme and not ours. A 207-function
     * import surface is why `Module.exports()` tallies 1,163 (1,126 functions + 36 globals + this
     * table) — the two numbers were different kinds, never a disagreement about the table.
     *
     * The fallback is `ed`, not `ld`. The shipped glue says so on the page it builds:
     * `var dynCall_iiii=Module["asm"]["ed"]` and `var dynCall_viii=Module["asm"]["ld"]` — and both
     * exports measure 4 parameters on both guests, so the parameter count could never have told them
     * apart. Only the glue's own binding can, which is exactly why the earlier `ld` binding was a
     * false trail: it reached `dynCall_viii` while the rows meant `dynCall_iiii`.
     *
     * Routing therefore mirrors the glue: `Lc.get(index)` when the slot holds a function, the exported
     * forwarder when the table cannot answer. **The two routes do not take the same arguments, and the
     * arities are the proof.** A table entry takes only the **three** real arguments —
     * `(_eventTypeId, _eventDataBlock, _userData)`, the slot having already been resolved by the lookup
     * — while the `ed` forwarder takes the index plus those three, so it is the only route with
     * **four** arguments. Which one answered, with what index and what entry arity, is readable
     * afterwards through {@link ArtemisGlHost.dynCallIiiiRoute}: a wrong table slot and a wrong arity
     * fail identically from the outside, and the next reader has to be able to tell them apart.
     */
    const table = exports['Lc'];
    const dynCallIiii = exports['ed'];
    const hasTable = typeof table === 'object' || typeof table === 'function';
    if (!hasTable) {
      report('gl', 'the bridge does not export Lc (its function table), so an event callback can only be reached through the ed (dynCall_iiii) fallback');
    }
    if (typeof dynCallIiii !== 'function') {
      report('gl', 'the bridge does not export ed (dynCall_iiii), and without Lc a registered event callback could never be called back');
    }
    let route = 'none';
    /**
     * Resolution is **once per event**, not once per pass. The table is live — a later event must see
     * whatever the guest has since written into the slot — so nothing here is cached across events.
     * Within one event the resolved function is frozen, because a rewind re-enters the *same* call:
     * re-looking the slot there would let a table write that the guest has not yet committed split one
     * call across two targets, and the parked pass would come back having called a function that never
     * consumed its arguments (`root returned while still rewinding`).
     *
     * The arity rule above is what makes that captured function callable at all. The two routes do not
     * take the same arguments: a table entry takes the **three** real ones, the `ed` forwarder takes
     * the index plus those three. Both are captured here — the function once, the slot once — so the
     * call site never has to reason about which route it is holding.
     */
    type Resolved = { fn: (...args: number[]) => number; callArgs: number[] };
    const resolveDynCallIiii = (entry: number): Resolved | null => {
      const slot = entry | 0;
      if (hasTable) {
        let fn: unknown = null;
        try {
          fn = (table as WebAssembly.Table).get(slot);   // an index past the table end throws
        } catch {
          fn = null;
        }
        if (typeof fn === 'function') {
          const candidate = fn as (...args: number[]) => number;
          route = `table slot ${slot}`;
          return { fn: candidate, callArgs: [0, 0, 0] };
        }
        route = `table slot ${slot} holds no function`;
      }
      if (typeof dynCallIiii === 'function') {
        route = `forwarder ed, index ${slot}`;
        return { fn: dynCallIiii as (...args: number[]) => number, callArgs: [slot, 0, 0, 0] };
      }
      route = 'no route';
      return null;
    };
    /**
     * Resolves and calls the raw callback, answering the guest's own i32. Synchronous by construction:
     * it does exactly what the glue's `getDynCaller` line does and nothing else.
     */
    const rawDynCallIiii = (local: Resolved, a: number, b: number, c: number): number => {
      const args = local.callArgs;
      args[args.length - 3] = a | 0;
      args[args.length - 2] = b | 0;
      args[args.length - 1] = c | 0;
      return local.fn(...args) | 0;
    };
    if (!setup.suspend) {
      glHost.dynCallIiii = (entry: number, a: number, b: number, c: number): number => {
        const local = resolveDynCallIiii(entry);
        return local === null ? 0 : rawDynCallIiii(local, a, b, c);
      };
    } else {
      const driver = setup.suspend;
      const wrapped = new WeakMap<(...args: number[]) => number, (...args: number[]) => unknown>();
      glHost.dynCallIiii = async (entry: number, a: number, b: number, c: number): Promise<number> => {
        const local = resolveDynCallIiii(entry);
        if (local === null) return 0;
        let adapter = wrapped.get(local.fn);
        if (adapter === undefined) {
          adapter = driver.wrap('ed', local.fn);
          wrapped.set(local.fn, adapter);
        }
        const args = local.callArgs;
        args[args.length - 3] = a | 0;
        args[args.length - 2] = b | 0;
        args[args.length - 1] = c | 0;
        adapter(...args);
        await driver.waitUntilIdle();
        const final = driver.result;
        return typeof final === 'number' ? final : 0;
      };
    }
    glHost.dynCallIiiiRoute = (): string => route;
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
    honoured,
    official: officialOf,
    buckets: bucketOf,
    blocked,
    byBucket,
    reset(): void {
      counts.clear();
      blocked.length = 0;
      // Cleared in the same step as the counts, because the two describe one run: a set left
      // standing after `reset()` would report a session as having honoured rows it has not called
      // since, which is exactly the kind of stale second truth this trace exists to prevent.
      honoured.clear();
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

  /**
   * The literal an asyncify guest throws when it parks in the GLFW frame loop.
   *
   * Compared with `===` against a string on purpose. The glue distinguishes its two unwinds by value
   * (`e !== e + 0 && e !== "unwind"`), and a truthy check here would swallow a zero-length throw or a
   * `String` object — turning a real fault into a reported healthy main loop.
   */
  const ASYNCIFY_UNWIND = 'unwind';

  /** Builds a refusal for a step the guest cannot have taken without, or `undefined` when it can. */
  const requireExport = (
    exports: Record<string, unknown>,
    name: string,
    what: string,
  ): ((...args: number[]) => number | void) | undefined => {
    const found = exports[name];
    if (typeof found !== 'function') {
      report('boot', `the bridge exports no ${name} (${what}), so the async boot cannot run`);
      return undefined;
    }
    return found as (...args: number[]) => number | void;
  };

  /**
   * Checks a pointer a guest allocator answered with, before a single byte is written through it.
   *
   * Every check is here because wasm i32 arrives *signed*: a pointer of 4 GiB - 1 is a perfectly good
   * address and reaches JavaScript as a negative number, so `>>> 0` is the conversion and not a
   * sloppiness — but it must come *after* the finite-integer test, because `NaN >>> 0` is `0` and a
   * wrapped `NaN` would look like a plausible pointer to the bottom of the heap. The range test is
   * what keeps that wrap honest: only the exact signed window a wasm i32 can carry converts.
   *
   * `aligned` is opt-in because the guest stack is not uniformly word-aligned: a `pushUtf8` string
   * may land on any byte, while an `argv` array is read as words and must be 4-aligned.
   */
  const checkPointer = (
    raw: unknown,
    bytes: number,
    what: string,
    aligned = true,
  ): number | undefined => {
    if (typeof raw !== 'number' || !Number.isInteger(raw)) {
      report('boot', `${what} returned ${String(raw)}, which is not an integer pointer`);
      return undefined;
    }
    if (raw < -0x80000000 || raw > 0xffffffff) {
      report('boot', `${what} returned ${String(raw)}, which is outside the 32-bit pointer range`);
      return undefined;
    }
    const pointer = raw >>> 0;
    if (pointer === 0) {
      report('boot', `${what} returned null for ${bytes} bytes of guest stack`);
      return undefined;
    }
    if (aligned && (pointer & 3) !== 0) {
      report('boot', `${what} returned unaligned pointer ${pointer} for ${bytes} bytes of guest stack`);
      return undefined;
    }
    if (pointer + bytes > memory.byteLength) {
      report(
        'boot',
        `${what} returned ${pointer}, and ${bytes} bytes at it run past the ${memory.byteLength}-byte heap`,
      );
      return undefined;
    }
    return pointer;
  };

  /**
   * The startup sequence again, awaited, for a guest compiled with asyncify.
   *
   * The order and the values are the same as {@link boot}; what changes is which of the four steps
   * that can take time are driven. The constructor pass, `_main`, and nothing else, go through the
   * driver: `wrap` is what makes a step survive an unwind, and `waitUntilIdle()` is what turns "the
   * guest parked" into "the guest is replayed until it is genuinely done". The stack helpers
   * (`Xc`/`Yc`/`Zc`) stay raw, because `argv` is built on the guest's stack and `stackRestore` must
   * undo exactly what the first pass did — wrapping either would make a rewind re-enter the one
   * operation that cannot be replayed.
   *
   * With no driver the sequence is the same sequence run strictly: every step runs synchronously, no
   * await is a real park, and each step's own return is the final one. That is the honest reading of
   * "there is no unwinder here" — a guest that *does* suspend has nothing driving it, so the `fa` row
   * reports a block rather than the boot pretending a suspension was answered.
   *
   * A missing `Mc`, `Qc`, `Zc`, `Xc` or `Yc` rejects. None of them is optional: a guest cannot have
   * started without them, and answering `ok` for a boot that never ran is the exact silent-success
   * failure this module exists to prevent.
   */
  const bootAsync = async (
    instance: WebAssembly.Instance,
    args: readonly string[] = [],
  ): Promise<ArtemisBootResult> => {
    const steps: string[] = [];
    const exports = instance.exports as Record<string, unknown>;
    const driver = setup.suspend;
    const encoder = new TextEncoder();
    // `argc` counts the program name as `argv[0]`, exactly as the glue's `callMain` does.
    const argc = args.length + 1;
    const argvBytes = (argc + 1) * 4;

    steps.push('initRuntime: FS.init, TTY.init, SOCKFS root');
    if (!fsInitialised) {
      stdio = initArtemisStandardStreams(fs);
      fsInitialised = true;
    }

    /**
     * The `__ATINIT__` member. Constructors allocate and register statics, so they may suspend and a
     * rewind may replay the pass — which is why the outcome is taken from the driver's `result` after
     * the wait rather than from the wrapped call's own (placeholder) return.
     */
    const callCtors = requireExport(exports, 'Mc', 'the guest constructors');
    if (callCtors === undefined) throw new ArtemisHostError('the bridge exports no Mc');
    steps.push('__ATINIT__: Mc (guest constructors)');
    const wrappedCtors = driver ? driver.wrap('Mc', callCtors) : callCtors;
    await wrappedCtors();
    if (driver) await driver.waitUntilIdle();

    steps.push('preMain: FS.ignorePermissions, __ATMAIN__');
    setup.preMain?.();

    // `stackSave()`, taken *after* the constructors: the pass above has its own stack frame, and a
    // saved pointer from before it would restore to an address the constructors have since used.
    const stackSave = requireExport(exports, 'Xc', 'the guest stackSave');
    if (stackSave === undefined) throw new ArtemisHostError('the bridge exports no Xc');
    const stackAlloc = requireExport(exports, 'Zc', 'the guest stackAlloc');
    if (stackAlloc === undefined) throw new ArtemisHostError('the bridge exports no Zc');
    const stackRestore = requireExport(exports, 'Yc', 'the guest stackRestore');
    if (stackRestore === undefined) throw new ArtemisHostError('the bridge exports no Yc');

    /**
     * Taken raw, because the allocator is guest code that may grow the heap: the view the saved
     * pointer came from is stale the moment it does, and a *signed* pointer is what a wasm i32
     * gives, so both conversions belong before the check rather than after it.
     */
    const rawSavedStack = stackSave();
    memory.refresh();
    const savedStack = checkPointer(rawSavedStack, 0, 'Xc (stackSave)');
    if (savedStack === undefined) throw new ArtemisHostError('stackSave returned an unusable pointer');

    /**
     * `allocateUTF8OnStack(str)`: `stackAlloc(len+1)`, copy the bytes, NUL-terminate.
     *
     * `memory.refresh()` after the allocation is not politeness. The allocator is guest code and may
     * grow the heap; `grow` detaches the old `ArrayBuffer`, leaving every view taken before it as a
     * zero-length view over a dead buffer. Writing through one silently writes nothing, and the guest
     * then reads `argv` entries that point at an empty string with no error anywhere.
     */
    const pushUtf8 = (text: string): number => {
      const encoded = encoder.encode(text);
      const raw = stackAlloc(encoded.length + 1);
      memory.refresh();
      const pointer = checkPointer(raw, encoded.length + 1, 'Zc (stackAlloc)', false);
      if (pointer === undefined) throw new ArtemisHostError(`stackAlloc refused ${encoded.length + 1} bytes`);
      memory.assertRange(pointer, encoded.length + 1);
      memory.u8.set(encoded, pointer);
      memory.u8[pointer + encoded.length] = 0;
      return pointer;
    };

    /**
     * `failed` is set only once there is already a reason to reject, and that reason is kept.
     */
    let failed = false;
    /**
     * Hands the stack pointer back exactly once, on every path — including the failure paths — so a
     * later entry into the guest does not start from a stack the boot left pushed.
     *
     * A throwing `stackRestore` propagates when nothing else has gone wrong, because a boot whose
     * stack was never given back is not a boot that succeeded. Deriving that from `failed` rather than
     * from a flag argument the call sites could get wrong is deliberate: there is exactly one place
     * that decides, and it is here.
     */
    const restore = (): void => {
      try {
        stackRestore(savedStack);
      } catch (error) {
        if (!failed) throw error;
      }
    };

    /**
     * The raw export is wrapped in one indirection rather than handed over directly: the driver calls
     * it through `.apply`, and the raw export is read out of the exports object, so it must stay a
     * plain function of arguments and nothing else.
     *
     * `unwind` is answered *here*, before the driver's `wrap` can see it. `unwind` is what the guest's
     * own `_emscripten_set_main_loop` raises to hand control back to the event loop, and the official
     * `callMain` answers it by declaring the program running — so the loop is recorded as started and
     * the call reports success, rather than being replayed or rejected as a failed boot. Every other
     * error keeps travelling, and `mainLoopStarted` is truthful because the guest really did park.
     */
    let mainLoopStarted = false;
    /**
     * Every operation taken since `savedStack` — reading `Qc`, wrapping it, and every allocation —
     * lives inside this one `try`, so the `finally` below gives the stack pointer back on *all*
     * paths, exactly once. Reading the export is *not* deferred to its use: `exports` is a guest
     * object, `Qc` may be a throwing getter, and `driver.wrap` can refuse outright, so each of those
     * is a failure like any other and none may leak the stack the boot already pushed.
     */
    let result: ArtemisBootResult | undefined;
    try {
      const main = requireExport(exports, 'Qc', '_main');
      if (main === undefined) throw new ArtemisHostError('the bridge exports no Qc');
      const rawMain = (...call: number[]): number | void => {
        try {
          return main(...call);
        } catch (error) {
          if (error !== ASYNCIFY_UNWIND) throw error;
          mainLoopStarted = true;
          return 0;
        }
      };
      const wrappedMain = driver ? driver.wrap('Qc', rawMain) : rawMain;
      const rawArgv = stackAlloc(argvBytes);
      memory.refresh();
      const argv = checkPointer(rawArgv, argvBytes, 'Zc (stackAlloc, argv)');
      if (argv === undefined) throw new ArtemisHostError('stackAlloc refused the argv array');
      memory.assertRange(argv, argvBytes);
      /** `argv[i]` — each slot is one word, and the array was checked to cover every slot. */
      const putWord = (slot: number, value: number): void => {
        memory.i32[(argv + slot * 4) >>> 2] = value | 0;
      };

      const thisProgram = './this.program';
      putWord(0, pushUtf8(thisProgram));
      for (let i = 1; i < argc; i += 1) putWord(i, pushUtf8(args[i - 1] ?? ''));
      // `argv[argc] = 0`: without the terminator the guest iterates past the end of its own arguments.
      putWord(argc, 0);

      steps.push(`callMain: Qc(${argc}, ${argv})`);
      result = { thisProgram, argv, steps };
      const returned = (await wrappedMain(argc, argv)) as number | void;
      if (driver) await driver.waitUntilIdle();
      // The final value is the driver's, never the wrapped call's return: that return is the
      // placeholder of whichever pass happened to come back first, and for a parked guest it is not
      // the value `_main` produced at all. Without a driver there is no placeholder to confuse it
      // with, so the call's own return is the real one.
      return Object.assign(result, {
        returned: driver ? (driver.result ?? 0) : returned,
        ...(mainLoopStarted ? { mainLoopStarted: true } : {}),
        steps: [...steps, '_main returned'],
      });
    } catch (error) {
      // The one throw that is not a failure — `unwind` — has already been answered inside `rawMain`,
      // so anything reaching here rejects, including a `result.threw` the driver kept: a driver that
      // failed is not a guest that is running. The original error is the one that survives, even when
      // `restore()` throws in the `finally`: `failed` is what tells it not to replace this reason.
      failed = true;
      throw error;
    } finally {
      restore();
    }
  };

  return { imports, trace, temp, attachExports, boot, bootAsync };
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
    // Both numbers, because they are different measurements: `called` is every name the guest
    // entered, `served` is the subset whose body completed synchronously without reporting.
    const served = names.filter((name) => trace.honoured.has(name));
    lines.push(
      `${bucket}  ${total} call${total === 1 ? '' : 's'}  ` +
        `(${names.length} names, ${called.length} called, ${served.length} honoured)`,
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
