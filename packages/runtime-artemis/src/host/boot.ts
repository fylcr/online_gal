import { createArtemisFdImports, type ArtemisFdHost } from './fd.js';
import { createArtemisRuntimeImports, ArtemisTempRet0, type ArtemisRuntimeHost } from './runtime.js';
import { createArtemisEhImports, type ArtemisEhHost } from './eh.js';
import { createArtemisAsyncifyImports, type ArtemisAsyncifyHost } from './asyncify.js';
import { ARTEMIS_ERRNO } from './errno.js';
import type { ArtemisMemory } from './memory.js';
import type { ArtemisFs } from './tree.js';
import type { ImportFn, ArtemisImportTable } from './bridge.js';

/**
 * Assembling every bucket the host has implemented, and naming the rest.
 *
 * This module exists because of one fact about wasm: **an import is not optional.** The bridge
 * declares 207 functions under the module name `a`, and the engine refuses to instantiate until
 * all 207 are supplied with something callable. A host that implements 68 of them and hands the
 * engine nothing for the other 139 does not boot — it fails to link, with a message that names the
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
   * Gives the `eh` table the two guest exports it needs. Must be called after
   * `WebAssembly.instantiate` returns, because both are exports of the bridge.
   */
  attachExports(instance: WebAssembly.Instance): void;
}

/**
 * Builds the complete 207-name import table: 68 names implemented, 139 counted stubs.
 *
 * The implemented set is the union of the four buckets this package has — `fd` (41), `runtime` (8),
 * `eh` (15) and `asyncify` (4) — which is 68 distinct short names covering 76 of the 207 rows, the
 * difference being the duplicated names that serve two rows each.
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
  const { memory, spec } = setup;
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

  const implemented: ArtemisImportTable = {
    ...(createArtemisFdImports(fdHost) as ArtemisImportTable),
    ...(createArtemisRuntimeImports(runtimeHost) as ArtemisImportTable),
    ...(createArtemisEhImports(ehHost) as ArtemisImportTable),
    ...(createArtemisAsyncifyImports(asyncifyHost) as ArtemisImportTable),
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
   * Wires the two guest helpers the `eh` table needs, after the instance exists.
   *
   * Both are wasm *exports* of the bridge, so they can only be reached once
   * `WebAssembly.instantiate` has returned — which is after the import table was needed. That
   * ordering is why they arrive as a second call rather than a constructor argument.
   *
   * They are exported under the **minified short names** `$c` and `ad`, not under the long C++
   * names. The long names are the *imports*; the exports are what the glue's `asm` object holds.
   * Looking only for the long names reports "unavailable" for two functions that are right there,
   * and a `find_matching_catch` with no real subobject check quietly accepts a handler the engine
   * would have rejected. Hence the short-name lookup, and hence the loud report if even that fails.
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

  return { imports, trace, temp, attachExports };
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
