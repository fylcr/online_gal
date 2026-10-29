import { ArtemisExit, ArtemisHostError } from './errno.js';
import type { ArtemisMemory } from './memory.js';
import { ArtemisTempRet0 } from './runtime.js';

/**
 * The `eh` bucket of `ports/artemis/harness/glue/imports-map.json` — 17 rows, 15 short names.
 *
 * Emscripten's exception support is a re-implementation of the Itanium C++ ABI on top of
 * JavaScript, and this file is that re-implementation. The guest is compiled `-fno-exceptions` with
 * Asyncify, but the emscripten EH shims are still *linked*, so a module that imports `___cxa_throw`
 * is handed a host function whether or not it ever calls one — an unwired import is a link error.
 *
 * Two facts shape everything below, and both come from the official glue rather than from what the
 * ABI "usually" looks like:
 *
 * 1. **The exception state lives in JavaScript, not in the guest heap.** The thrown pointer, the
 *    uncaught count, the rethrow flag and the handler stack are emscripten's closures. That is why
 *    `___cxa_throw` returns `void` and communicates by *throwing a JS value*: the throw is the
 *    control-flow mechanism, and the wasm stack below it is unwound before the next import runs.
 *    The *landing-pad* state (`_setThrew`) is the exception to this: it is not in the glue at all,
 *    only in the wasm module, so it is reached through the guest's export.
 * 2. **The two blocks are plain guest memory with a fixed header.** An exception is 16 bytes of
 *    `ExceptionInfo` underneath the payload pointer the guest holds; a catch record is 8 bytes of
 *    `CatchInfo` containing a base pointer and an optional adjustment. Every function here is a
 *    read or a write of those two layouts, which is why the offsets below are copied exactly.
 *
 * ## What this file does not do
 *
 * It does not *rescue* an in-flight C++ throw from JavaScript: the guest's `catch` handlers are wasm
 * frames that JS can only re-enter through the guest's own exports, so a host function must raise
 * (as the glue does) and let the guest's unwinder decide. It also does not free an exception block
 * on a path where the guest has not released it — see {@link ArtemisEhHost.runDestructor} and `A`.
 *
 * ## Delegation, and the one divergence
 *
 * Destructors and the `find_matching_catch` scratch word are the SDK's own guest exports
 * (`$c`/`ad`/`gd`/`Xc`/`Yc`/`Zc`, wired by `boot.ts` after instantiation) because only the guest can
 * run a wasm destructor or write a word at the guest's stack pointer. The single divergence is the
 * scratch word's allocation when those exports are absent: the glue takes `stackAlloc(4)`, this host
 * takes one lazy `malloc(4)` and keeps it. Bounded (one word per session) and reported, but it is a
 * divergence, not a copy.
 */

/**
 * The exception header, 16 bytes under the pointer `___cxa_allocate_exception` returns:
 * destructor, refcount, type, caught, rethrown.
 */
export const ARTEMIS_EXCEPTION_INFO_ATTRS = {
  DESTRUCTOR_OFFSET: 0,
  REFCOUNT_OFFSET: 4,
  TYPE_OFFSET: 8,
  CAUGHT_OFFSET: 12,
  RETHROWN_OFFSET: 13,
  SIZE: 16,
} as const;

/** `CatchInfo`: base pointer at `+0`, adjusted pointer at `+4`. */
export const ARTEMIS_CATCH_INFO_SIZE = 8;

const EXCEPTION_DESTRUCTOR = ARTEMIS_EXCEPTION_INFO_ATTRS.DESTRUCTOR_OFFSET;
const EXCEPTION_REFCNT = ARTEMIS_EXCEPTION_INFO_ATTRS.REFCOUNT_OFFSET;
const EXCEPTION_TYPE = ARTEMIS_EXCEPTION_INFO_ATTRS.TYPE_OFFSET;
const EXCEPTION_CAUGHT = ARTEMIS_EXCEPTION_INFO_ATTRS.CAUGHT_OFFSET;
const EXCEPTION_RETHROWN = ARTEMIS_EXCEPTION_INFO_ATTRS.RETHROWN_OFFSET;
const EXCEPTION_INFO_SIZE = ARTEMIS_EXCEPTION_INFO_ATTRS.SIZE;
const CATCH_INFO_SIZE = ARTEMIS_CATCH_INFO_SIZE;
/** `CatchInfo.base` / `CatchInfo.adjusted` — the two words, at their ABI offsets. */
const CATCH_BASE = 0;
const CATCH_ADJUSTED = 4;

/**
 * What a session must supply for the `eh` bucket to do anything but record a trap.
 *
 * The guest-owned fields are deliberately **writable**: `canCatch`, `isPointerType`, `stackAlloc`
 * and the rest back wasm *exports* of the bridge, so they only exist once
 * `WebAssembly.instantiate` has returned — later than the import table that instantiation needs.
 */
export interface ArtemisEhHost {
  readonly memory: ArtemisMemory;
  /** The guest's `malloc`, which allocates the exception and catch blocks. */
  readonly malloc?: (bytes: number) => number;
  /** The guest's `free`, which releases them again. */
  readonly free?: (pointer: number) => void;
  /** The shared `setTempRet0` cell, the same instance the `runtime` table uses. */
  readonly temp?: ArtemisTempRet0;
  /** The guest's `___cxa_can_catch` (`$c`), needed by the `find_matching_catch_*` family. */
  canCatch?: (catchType: number, thrownType: number, adjustedPtr: number) => number;
  /** The guest's `___cxa_is_pointer_type` (`ad`), which picks a catch's adjusted pointer. */
  isPointerType?: (type: number) => number;
  /** The guest's `_setThrew` (`_c`), which `___cxa_end_catch` calls first. */
  setThrew?: (env: number, value: number) => void;
  /** The guest's `stackSave` (`Xc`), paired with {@link ArtemisEhHost.stackAlloc}. */
  stackSave?: () => number;
  /** The guest's `stackAlloc` (`Zc`) — the single word `___cxa_can_catch` writes its result into. */
  stackAlloc?: (bytes: number) => number;
  /** The guest's `stackRestore` (`Yc`), which unwinds the word a failed search allocated. */
  stackRestore?: (pointer: number) => void;
  /**
   * Runs an exception's destructor, which the SDK reaches through the `ii` dynCall — the destructor
   * the guest passes to `___cxa_throw` is a `(i32)->i32` **dynCall index**, not a function pointer.
   *
   * Bound after instantiation from the module's `dynCall_ii` export (`gd`), so a host without it
   * reports the omission instead of guessing an index into the function table.
   */
  runDestructor?: (index: number, excPtr: number) => void;
  /** Reports a call this host could not serve, so a session can log it instead of swallowing it. */
  readonly onBlocked?: (what: string, detail: string) => void;
}

/** The import table, keyed by the wasm short name. */
export type ArtemisEhImports = Record<string, (...args: number[]) => number | void>;

/**
 * The value a `___cxa_throw` or `___cxa_rethrow` raises, and the value `___resumeException` re-raises.
 *
 * The glue throws the bare pointer — `throw ptr` — so a JS `catch` in emscripten's runtime sees a
 * number. That is a real hazard: a number that collides with a host error would be misread. This
 * host throws a *tagged* value instead, so the signal is unmistakable; the guest-visible effect is
 * the same, because what matters to the unwinder above is that a JS throw happened.
 */
export class ArtemisCppThrow extends Error {
  constructor(readonly pointer: number) {
    super(`artemis C++ exception at ${pointer}`);
    this.name = 'ArtemisCppThrow';
  }
}

/** The value `_emscripten_longjmp` raises, matching the glue's `throw "longjmp"`. */
export class ArtemisLongjmp extends Error {
  constructor(readonly value: number) {
    super(`artemis longjmp to ${value}`);
    this.name = 'ArtemisLongjmp';
  }
}

/**
 * A step of the throw machinery that the guest cannot have done, reported rather than guessed at.
 *
 * Thrown — not merely reported through `onBlocked` — because continuing past it would hand the guest
 * a plausible-looking wrong answer: a catch block with no exception underneath it, an unwinder told
 * a handler matched when it did not. The class exists so a session can tell this apart from a
 * genuine C++ throw rather than counting the two together.
 */
export class ArtemisEhFailure extends ArtemisHostError {
  constructor(readonly what: string, detail: string) {
    super(`artemis eh ${what}: ${detail}`);
    this.name = 'ArtemisEhFailure';
  }
}

/** Builds the `eh` import table for one instance. */
export function createArtemisEhImports(host: ArtemisEhHost): ArtemisEhImports {
  const { memory } = host;
  const report = (what: string, detail: string): void => {
    if (host.onBlocked) host.onBlocked(what, detail);
    else throw new ArtemisEhFailure(what, detail);
  };
  const temp = host.temp ?? new ArtemisTempRet0();

  /**
   * Re-derives the heap views, and the reason it is called on both sides of every guest boundary.
   *
   * `ArtemisMemory.u8` / `.i32` are plain fields holding typed arrays, not accessors, so *reading*
   * `memory.i32` does not pick up a new buffer — only a new view does. A guest callee that grows the
   * heap (the allocator does, and so can `$c`, a destructor, or `_c`) detaches the old `ArrayBuffer`
   * and leaves every view taken before it zero-length, so a read through one answers 0 and a write
   * through one writes nothing. Silent, and indistinguishable from a correct zero.
   */
  const refresh = (): void => memory.refresh();

  /**
   * Runs a guest callee between two heap views, so this host sees the buffer the guest left behind.
   *
   * Both sides are needed and neither is redundant: the *pre*-call refresh is what makes the
   * arguments this host passes visible after an earlier boundary, and the *post*-call refresh is what
   * makes the guest's own writes and its possible growth visible after this one.
   */
  const atGuest = <T>(call: () => T): T => {
    refresh();
    const answered = call();
    refresh();
    return answered;
  };

  /**
   * The live exception state, kept in JavaScript exactly as the glue keeps it.
   *
   * - `last` is what `___cxa_find_matching_catch_*` reads: the most recently thrown pointer, or 0.
   * - `uncaught` is the count the guest polls through `___cxa_uncaught_exceptions`.
   * - `caught` is the stack of guest-allocated `CatchInfo` blocks: pushed by `begin_catch`, popped by
   *   `end_catch` / `rethrow` / `resumeException`.
   */
  const state = {
    last: 0,
    uncaught: 0,
    caught: [] as number[],
    /** The stand-in for `stackAlloc(4)` when the guest's stack helpers are not wired. */
    adjustBuffer: 0,
  };

  /**
   * Reads a NUL-terminated string out of the guest heap. The `| 0` is not decoration: a wasm i32
   * arrives in JavaScript as a signed number, so an address above 2^31 comes through negative.
   */
  const utf8 = (pointer: number): string => {
    let end = (pointer | 0) >>> 0;
    const base = memory.u8;
    while (end < base.length && base[end] !== 0) end += 1;
    return new TextDecoder().decode(base.subarray((pointer | 0) >>> 0, end));
  };

  /** `ExceptionInfo(excPtr).ptr` — the header sits 16 bytes below the pointer the guest holds. */
  const infoOf = (excPtr: number): number => (excPtr | 0) - EXCEPTION_INFO_SIZE;
  /** `info.get_type()`. */
  const typeOf = (excPtr: number): number => memory.i32[(infoOf(excPtr) + EXCEPTION_TYPE) >>> 2] ?? 0;
  /** `info.get_destructor()`. */
  const destructorOf = (excPtr: number): number =>
    memory.i32[(infoOf(excPtr) + EXCEPTION_DESTRUCTOR) >>> 2] ?? 0;
  /** `info.get_rethrown()`. */
  const rethrownOf = (excPtr: number): boolean =>
    (memory.u8[infoOf(excPtr) + EXCEPTION_RETHROWN] ?? 0) !== 0;
  /**
   * `new CatchInfo` with no argument, then `set_base_ptr(base)`.
   *
   * The glue never leaves the base unset: every search does
   * `var catchInfo = new CatchInfo; catchInfo.set_base_ptr(thrown)` before it looks at a single
   * candidate. `begin_catch` reads that word back and addresses the exception header *through* it,
   * so a record left null is not a missing detail — it is a poisoned pointer into the header of
   * whatever object happens to live 16 bytes below address 0.
   */
  const newCatchInfo = (base: number): number => {
    if (!host.malloc) {
      report('cxa_begin_catch', 'the guest malloc is not available, so no CatchInfo can be allocated');
      return 0;
    }
    const pointer = atGuest(() => host.malloc!(CATCH_INFO_SIZE)) | 0;
    if (pointer === 0) {
      report('cxa_begin_catch', 'the guest malloc refused 8 bytes for a CatchInfo');
      return 0;
    }
    memory.i32[(pointer + CATCH_BASE) >>> 2] = base | 0;
    memory.i32[(pointer + CATCH_ADJUSTED) >>> 2] = 0;
    return pointer;
  };
  /** `catchInfo.get_base_ptr()`. */
  const baseOf = (catchInfo: number): number => memory.i32[catchInfo >>> 2] ?? 0;
  /** `catchInfo.get_adjusted_ptr()`. */
  const adjustedOf = (catchInfo: number): number => memory.i32[(catchInfo + CATCH_ADJUSTED) >>> 2] ?? 0;

  /**
   * `exception_decRef(info)`. Its whole body, and the reason `end_catch` cannot be a bare decrement:
   *
   * ```js
   * if (info.release_ref() && !info.get_rethrown()) {
   *   var destructor = info.get_destructor();
   *   if (destructor) getDynCaller("ii", destructor)(info.excPtr);
   *   ___cxa_free_exception(info.excPtr);
   * }
   * ```
   *
   * `release_ref()` returns "the previous value was 1", so this is where an exception *dies*: its
   * destructor runs and its block goes back to the allocator, exactly once. The destructor is a
   * `(i32)->i32` dynCall index reached through the guest's `ii` export — not a raw function pointer —
   * and a host that cannot reach it must fail loudly rather than skip the object's teardown.
   */
  const decRef = (excPtr: number): void => {
    const info = infoOf(excPtr);
    const previous = memory.i32[(info + EXCEPTION_REFCNT) >>> 2] ?? 0;
    memory.i32[(info + EXCEPTION_REFCNT) >>> 2] = previous - 1;
    if (previous !== 1) return;
    if ((memory.u8[info + EXCEPTION_RETHROWN] ?? 0) !== 0) return;
    const destructor = memory.i32[(info + EXCEPTION_DESTRUCTOR) >>> 2] ?? 0;
    if (destructor) {
      if (!host.runDestructor) {
        throw new ArtemisEhFailure(
          'exception_decRef',
          `the exception at ${excPtr} reached refcount zero with destructor ${destructor}, but no guest dynCall_ii (gd) is wired to run it`,
        );
      }
      // A destructor is guest code: it may grow the heap, so the free below must not read `memory`
      // through the views this frame started with.
      atGuest(() => host.runDestructor!(destructor, excPtr));
    }
    if (!host.free) {
      throw new ArtemisEhFailure(
        'exception_decRef',
        `the exception at ${excPtr} reached refcount zero, but the guest free is not wired to release it`,
      );
    }
    refresh();
    host.free(info);
  };

  /**
   * Calls the guest's `_setThrew(env, value)` export, the same way `___cxa_end_catch` and `_longjmp`
   * call it.
   *
   * `_setThrew` is **not defined in the SDK glue at all**: it is a wasm export of the bridge (the
   * `_c` row, reached as `asm["_c"]`), and the glue only ever *calls* it — from `___cxa_end_catch()`,
   * from `_longjmp`, and from the `invoke_*` stubs' catch arm. So there is no JS body to copy, and
   * likewise no `__THREW__` variable anywhere in the glue for a host to write: the landing-pad state
   * lives in the wasm module, and only the export can touch it.
   *
   * `env === 0` is the reset — the call `___cxa_end_catch` makes first — and a nonzero `env` marks a
   * live throw. This host cannot tell the two apart without the export, so it reports only the live
   * arm; treating the reset as an error would warn on every ordinary `end_catch`.
   */
  const setThrew = (env: number, value: number): void => {
    if (host.setThrew) {
      atGuest(() => host.setThrew!(env | 0, value | 0));
      return;
    }
    if (env === 0) return;
    report(
      'longjmp',
      `the guest _setThrew export is not wired, so the throw state (env ${env}, value ${value}) has nowhere to go`,
    );
  };

  /**
   * A scratch guest word for `___cxa_can_catch`, standing in for the glue's `stackAlloc(4)`.
   *
   * The restore is handed back to the caller rather than run here, because whether it may run is
   * decided by the caller: see {@link findMatching}. The body is declared `(scratch: number) => void`,
   * not `never`: the "matched, and the scratch word is still live" outcome travels back through the
   * caller's own `matched` flag, not through the callback's return type.
   */
  const withScratch = (
    body: (scratch: number) => void,
    onMissing: () => void,
  ): (() => void) | null => {
    if (host.stackAlloc && host.stackSave) {
      const saved = atGuest(() => host.stackSave!()) | 0;
      const scratch = atGuest(() => host.stackAlloc!(4)) | 0;
      if (scratch === 0) {
        report('find_matching_catch', 'the guest stackAlloc refused 4 bytes for the adjusted-pointer scratch word');
        return null;
      }
      body(scratch);
      return () => host.stackRestore?.(saved);
    }
    if (!host.malloc) {
      report('find_matching_catch', 'no guest malloc or stackAlloc, so there is no scratch word for the adjusted pointer');
      onMissing();
      return null;
    }
    if (!state.adjustBuffer) state.adjustBuffer = atGuest(() => host.malloc!(4)) | 0;
    const scratch = state.adjustBuffer;
    if (scratch === 0) {
      report('find_matching_catch', 'the guest malloc refused 4 bytes for the adjusted-pointer scratch word');
      onMissing();
      return null;
    }
    body(scratch);
    return null;
  };

  /**
   * The shared body of `___cxa_find_matching_catch_{2,3,8}`.
   *
   * The personality routine calls one of the three arities with the catch types it is prepared to
   * handle, most-derived first. The return is a `CatchInfo` pointer — 0 when nothing is in flight —
   * and the *matched type* travels out through the `setTempRet0` cell, which is why this table and
   * the `runtime` table share one {@link ArtemisTempRet0}.
   *
   * Note what the glue does **not** do here: it never asks `___cxa_is_pointer_type`. A reference
   * catch's dereference lives in `get_exception_ptr()`, i.e. in `begin_catch`, not in the search. An
   * earlier revision of this file put that branch here and grew an early `free` to go with it; both
   * are absent from the official body, and both break a real unwind.
   *
   * ## The one thing this search hands over: `stackAlloc(4)`
   *
   * A `can_catch` call is a **guest call, not a memory read**, so the scratch word it needs is a
   * guest stack allocation and the pointer belongs to the *caller*, not to this function. The glue
   * spells that out: the successful arm `return`s from inside the loop and its `stackRestore` is
   * reached only by the identity-match and no-match paths, and `___cxa_find_matching_catch_3`'s own
   * caller is what gives the pointer back. So this host restores on exactly those two paths — see
   * {@link withScratch}'s handed-back restorer and the `matched` guard below — and leaves the 4 bytes
   * allocated after a successful `$c`.
   */
  function findMatching(types: readonly number[]): number {
    const thrown = state.last;
    refresh();
    if (!thrown) {
      temp.set(0);
      return 0;
    }
    const thrownType = typeOf(thrown);
    if (!host.malloc) {
      report('find_matching_catch', 'the guest malloc is not available, so no CatchInfo can be allocated');
      temp.set(0);
      return 0;
    }
    // `new CatchInfo; catchInfo.set_base_ptr(thrown)` — the base is written *before* the search, and
    // `begin_catch` addresses the exception header through it.
    const catchInfo = newCatchInfo(thrown);
    if (catchInfo === 0) {
      temp.set(0);
      return 0;
    }
    if (!thrownType) {
      // A foreign throw carries no type: only `catch(...)` can take it, and the guest signals that
      // with candidate type 0. A zero temp cell says "catch-all only".
      temp.set(0);
      return catchInfo;
    }
    let restoreStack: (() => void) | null = null;
    let matched = false;
    restoreStack = withScratch((scratch) => {
      memory.i32[scratch >>> 2] = thrown;
      for (const rawType of types) {
        const caughtType = rawType | 0;
        // 0 is the catch-all, and an identity match needs no guest call: a memory read has no exit
        // that could take the scratch word away.
        if (caughtType === 0 || caughtType === thrownType) break;
        if (atGuest(() => host.canCatch?.(caughtType, thrownType, scratch))) {
          // The guest writes the base adjustment into the scratch word. If the guest grew the heap
          // inside `$c` (`memory.grow`), that grow installs a *new* heap buffer rather than detaching
          // the old one, so the host's views are stale and must be re-derived; this read therefore
          // updates the host's views before touching the heap again. Without it the host keeps the
          // *pre-call* value 0 and the guest is handed a base with no adjustment: the wrong object,
          // silently.
          memory.refresh();
          const adjusted = memory.i32[scratch >>> 2] ?? thrown;
          if (thrown !== adjusted) {
            // `get_adjusted_ptr()`: the glue writes *only* this word in the `can_catch` arm.
            // `set_base_ptr(adjusted)` is not part of it — `get_base_ptr()` stays the payload the
            // search was given, and `___cxa_begin_catch` addresses the exception header through it.
            // Overwriting the base with the adjustment would make every header read land 48 bytes
            // into the object that was thrown.
            memory.i32[(catchInfo + CATCH_ADJUSTED) >>> 2] = adjusted;
          }
          temp.set(caughtType);
          matched = true;
          // The official `___cxa_find_matching_catch_*` returns from *inside* the loop here and
          // never reaches its `stackRestore`, so the `stackAlloc(4)` stays allocated; the caller
          // restores the pointer itself. `restoreStack` is therefore deliberately not called, and
          // the exact stack lifecycle is the glue's, not an unconditional-restore tidying of it.
          return;
        }
      }
      // No candidate matched: the thrown type goes into the temp cell, and the block is returned
      // non-null anyway — "matched nothing" and "nothing thrown" are different answers.
      temp.set(thrownType);
    }, () => undefined);
    // Identity match, no-match, and every path where no scratch word existed leave the stack where
    // they found it; only a successful `$c` leaves the allocation behind, as the glue does.
    if (!matched) {
      refresh();
      restoreStack?.();
    }
    return catchInfo;
  }

  return {
    /**
     * `___cxa_allocate_exception(size)` — reserve `size + 16` and return a pointer 16 past the base,
     * so the payload starts where the runtime's header ends. That offset *is* the SDK's own body:
     * `return _malloc(size + ExceptionInfoAttrs.SIZE) + ExceptionInfoAttrs.SIZE`. The 16 is not slack
     * in this host either, and not an extra this file adds: `ExceptionInfo(excPtr)` reads its fields
     * at `excPtr - 16`, so a block handed out without the header would make every later
     * reconstruction read the 16 bytes *above* it. What the host does *not* do is rely on the guest
     * having zeroed them: the zeroing would be extra header initialization done by the **host**, not
     * by the SDK body, which returns a pointer into whatever `_malloc` handed back. So this
     * implementation writes every field itself below, because a reused block can carry stale values.
     * {@link ArtemisEhImports.w}'s `___cxa_throw` then initializes the authoritative header.
     */
    u: (size: number): number => {
      if (!host.malloc) {
        report('cxa_allocate_exception', `the guest malloc is not available, so ${size} B cannot be reserved`);
        return 0;
      }
      const block = atGuest(() => host.malloc!((size | 0) + EXCEPTION_INFO_SIZE)) | 0;
      if (block === 0) return 0;
      const pointer = block + EXCEPTION_INFO_SIZE;
      memory.i32[(block + EXCEPTION_DESTRUCTOR) >>> 2] = 0;
      memory.i32[(block + EXCEPTION_REFCNT) >>> 2] = 0;
      memory.i32[(block + EXCEPTION_TYPE) >>> 2] = 0;
      memory.u8[block + EXCEPTION_CAUGHT] = 0;
      memory.u8[block + EXCEPTION_RETHROWN] = 0;
      return pointer;
    },

    /**
     * `___cxa_throw(ptr, type, destructor)` — `info.init(type, destructor)`, remember the pointer,
     * count it uncaught, and `throw ptr`.
     *
     * `init` sets **all five** fields: the type and destructor from the arguments, and refcount 0,
     * caught false, rethrown false. The last three are not redundant with `u`: a guest may reuse an
     * exception block, and a stale refcount of 1 would make the *next* `end_catch` release someone
     * else's reference and destroy the object one catch clause too early.
     *
     * The destructor is stored, not invoked: by the time it should run the frame that owned the
     * object is gone, and only the guest can run it — at refcount zero, from `exception_decRef`.
     *
     * The header writes go in one order and one order only, because a boundary sits in the middle of
     * them: `memory.u8` and `memory.i32` are fields over one `ArrayBuffer`, and the five writes above
     * are only coherent if no guest call separates them. Nothing is called here at all — the throw is
     * a JS `throw`, not a guest call — so the whole header lands on one buffer.
     */
    w: (ptr: number, type: number, destructor: number): void => {
      // No `atGuest` wrapping exists here to refresh the views: this body never calls a guest
      // callee. But it can be the *first* import to run after the guest grew the heap inside its
      // own code (`memory.grow`, an allocator path, a `$c` that grew), and then every view below
      // is a detached one whose writes land nowhere. Hence the entry refresh.
      refresh();
      const info = infoOf(ptr);
      memory.i32[(info + EXCEPTION_TYPE) >>> 2] = type | 0;
      memory.i32[(info + EXCEPTION_DESTRUCTOR) >>> 2] = destructor | 0;
      memory.i32[(info + EXCEPTION_REFCNT) >>> 2] = 0;
      memory.u8[info + EXCEPTION_CAUGHT] = 0;
      memory.u8[info + EXCEPTION_RETHROWN] = 0;
      state.last = ptr | 0;
      state.uncaught += 1;
      throw new ArtemisCppThrow(ptr | 0);
    },

    /**
     * `___cxa_begin_catch(ptr)` — enter a `catch` clause and return the pointer the handler sees.
     *
     * `ptr` is **not** the exception: it is the `CatchInfo` the personality routine's search left
     * behind, and the glue's first act is to wrap it —
     *
     * ```js
     * function ___cxa_begin_catch(ptr) {
     *   var catchInfo = new CatchInfo(ptr);          // wraps, does not allocate
     *   var info = catchInfo.get_exception_info();   // ExceptionInfo over get_base_ptr()
     *   if (!info.get_caught()) { info.set_caught(true); uncaughtExceptionCount-- }
     *   info.set_rethrown(false);
     *   exceptionCaught.push(catchInfo);
     *   exception_addRef(info);
     *   return catchInfo.get_exception_ptr();
     * }
     * ```
     *
     * Three consequences, all of which the previous version of this function got wrong:
     *
     * - **No allocation.** `new CatchInfo(ptr)` stores `ptr` and nothing else. Allocating a second
     *   8-byte record here would hand the handler a handle the `end_catch` stack never sees, and the
     *   search's record would leak.
     * - **No reset of the adjustment.** The adjustment at `+4` was written by the search, and its
     *   meaning is not a reference-catch dereference. `___cxa_begin_catch` below does call
     *   `___cxa_is_pointer_type`, but that is a decision about the *type*, not a pointer dereference:
     *   a pointer-typed catch returns the payload word itself (`memory.i32[base >>> 2]`), while the
     *   adjustment is used only for the other case, as a base adjustment to add. Clearing the word
     *   here would throw away that adjustment and the handler would be handed the unadjusted base.
     * - **`get_exception_info()` is `new ExceptionInfo(this.get_base_ptr())`**, so every header read
     *   and write below is addressed off `base` (the payload), never off `ptr` (the record).
     *
     * The reference taken here is the counterpart of `end_catch`'s release: refcount 0 means "alive
     * and uncaught", so a rethrow keeps the block alive until the outermost `end_catch` runs the
     * destructor and frees it exactly once.
     */
    r: (ptr: number): number => {
      refresh();
      const catchInfo = ptr | 0;
      const base = baseOf(catchInfo);
      if (base === 0) {
        throw new ArtemisEhFailure('cxa_begin_catch', `the catch record at ${ptr} carries no base pointer`);
      }
      const info = infoOf(base);
      const caughtAlready = (memory.u8[info + EXCEPTION_CAUGHT] ?? 0) !== 0;
      if (!caughtAlready) {
        memory.u8[info + EXCEPTION_CAUGHT] = 1;
        state.uncaught = Math.max(0, state.uncaught - 1);
      }
      memory.u8[info + EXCEPTION_RETHROWN] = 0;
      state.caught.push(catchInfo);
      memory.i32[(info + EXCEPTION_REFCNT) >>> 2] = (memory.i32[(info + EXCEPTION_REFCNT) >>> 2] ?? 0) + 1;
      // `___cxa_is_pointer_type` is a guest export, so it is a heap boundary too: the reads below
      // must not go through the views this frame held before the call.
      const isPointerType = host.isPointerType
        ? atGuest(() => host.isPointerType!(typeOf(base) | 0)) !== 0
        : false;
      refresh();
      if (isPointerType) return memory.i32[base >>> 2] ?? 0;
      const adjusted = adjustedOf(catchInfo);
      return adjusted !== 0 ? adjusted : base;
    },

    /**
     * `___cxa_end_catch()` — leave a `catch` clause. The glue's whole body, in order:
     *
     * ```js
     * _setThrew(0);
     * var catchInfo = exceptionCaught.pop();
     * exception_decRef(catchInfo.get_exception_info());   // destructor + ___cxa_free_exception
     * catchInfo.free();                                    // _free(catchInfo.ptr)
     * exceptionLast = 0;
     * ```
     *
     * `_setThrew(0)` is a *guest call* and is not optional: throw state left set from the previous
     * throw is what the next `setjmp` tests, so dropping the reset turns an ordinary return into a
     * phantom longjmp.
     *
     * The free is part of the contract, and so is its order. `exception_decRef` releases the
     * exception *block* (`___cxa_free_exception`, i.e. `_free(excPtr - 16)`) only when the refcount
     * reaches zero from one — and the SDK deliberately leaves the refcount of a rethrown exception
     * alone (`exception_decRef` is guarded by `!get_rethrown()` only for the *destruction*, the
     * decrement itself still happens), so a rethrow chain survives to the outer clause. Then the
     * record itself is freed. Omitting either `_free` leaks on every caught exception; ordering them
     * the other way would free the record `decRef` reads.
     *
     * If `exception_decRef` cannot be completed — no destructor reachable, or no `free` — it throws
     * an {@link ArtemisEhFailure}. The glue has no such arm: it pops the record, runs `decRef` and
     * frees the record with nothing between the three. This host therefore gives up *without*
     * unwinding itself: the record has already been popped and its reference already decremented, so
     * the failure leaves the clause half-torn-down rather than neatly restored, and the honest
     * description is "the throw machinery stopped here", not "nothing changed".
     */
    A: (): void => {
      setThrew(0, 0);
      const catchInfo = state.caught.pop();
      if (catchInfo === undefined) {
        state.last = 0;
        report('cxa_end_catch', 'there was no active catch block to end');
        return;
      }
      decRef(baseOf(catchInfo));
      atGuest(() => host.free?.(catchInfo));
      state.last = 0;
    },

    /**
     * `___cxa_free_exception(ptr)` — give an exception block back without running a destructor.
     * The guest calls this itself when it caught by reference and decided the object is finished.
     */
    z: (ptr: number): number => {
      // `free` is the callee here, so the `atGuest` double-refresh this file uses elsewhere would
      // be redundant; what is not covered by anything else is growth the guest did on its own.
      refresh();
      host.free?.(infoOf(ptr));
      return 0;
    },

    /**
     * `___cxa_rethrow()` — re-raise the exception the current `catch` clause is handling:
     *
     * ```js
     * var catchInfo = exceptionCaught.pop();
     * var info = catchInfo.get_exception_info();
     * var ptr = catchInfo.get_base_ptr();
     * if (!info.get_rethrown()) {
     *   exceptionCaught.push(catchInfo); info.set_rethrown(true); info.set_caught(false);
     *   uncaughtExceptionCount++;
     * } else { catchInfo.free() }
     * exceptionLast = ptr;
     * throw ptr;
     * ```
     *
     * The first rethrow and every later one are the *same* exception seen by different clauses, so
     * the record goes back on the stack and the refcount is untouched — the outer `begin_catch` takes
     * the second reference and the outermost `end_catch` is what finally runs the destructor and
     * frees the block. Only a rethrow of an *already rethrown* exception frees the record.
     */
    za: (): void => {
      refresh();
      const catchInfo = state.caught.pop();
      if (catchInfo === undefined) {
        report('cxa_rethrow', 'there was no active catch block to rethrow');
        throw new ArtemisCppThrow(state.last);
      }
      const base = baseOf(catchInfo);
      if (!rethrownOf(base)) {
        state.caught.push(catchInfo);
        const info = infoOf(base);
        memory.u8[info + EXCEPTION_RETHROWN] = 1;
        memory.u8[info + EXCEPTION_CAUGHT] = 0;
        state.uncaught += 1;
      } else {
        atGuest(() => host.free?.(catchInfo));
      }
      state.last = base;
      throw new ArtemisCppThrow(base);
    },

    /**
     * `___cxa_uncaught_exceptions()` — how many exceptions are in flight that no `catch` has taken.
     * The guest polls this from `std::terminate`'s guard, so it has to be the real count.
     */
    Db: (): number => state.uncaught,

    c: (...types: number[]): number => findMatching(types),
    g: (...types: number[]): number => findMatching(types),
    vb: (...types: number[]): number => findMatching(types),

    /**
     * `___resumeException(catchInfoPtr)` — continue an unwind after a personality routine chose to
     * defer: free the record, remember the base pointer if nothing was in flight (the "resume from a
     * saved landing pad" case), and rethrow. Note that it rethrows the **base**, which for a
     * reference-catching search is the payload, not the adjustment.
     */
    i: (catchInfoPtr: number): void => {
      // An entry refresh, not an `atGuest`: the only guest callee below is `free`, and the base is
      // read *before* it. A guest that grew the heap in its own code pushed the callee through a
      // detached view, so without this the record reads 0 and the rethrow carries the wrong pointer.
      refresh();
      const catchInfo = catchInfoPtr | 0;
      const base = baseOf(catchInfo);
      if (!state.last) state.last = base;
      atGuest(() => host.free?.(catchInfo));
      throw new ArtemisCppThrow(base);
    },

    /**
     * `_llvm_eh_typeid_for(type)` — the identity function: the type *is* its own id, so the
     * personality routine's type comparisons work on the same numbers the guest compiled in.
     */
    L: (type: number): number => type | 0,

    /**
     * `_emscripten_longjmp(env, value)` — `_setThrew(env, value || 1)` and then `throw "longjmp"`.
     *
     * Both halves are load-bearing: `_setThrew` records the throw so the matching `setjmp` reads it
     * back instead of 0, and the JS throw is how the wasm stack unwinds. `value || 1` is the C rule
     * that `longjmp(env, 0)` must still read back as 1, because 0 is what a real `setjmp` returns.
     *
     * The `I` name is duplicated in the import section — `(i32,i32)->void` and `()->void` — and both
     * rows link to this one body; the second form simply sees `env === 0`, which is `_setThrew`'s
     * reset arm in the glue too.
     */
    I: (a0?: number, a1?: number): void => {
      const env = a0 === undefined ? 0 : a0 | 0;
      const value = a1 === undefined ? 0 : a1 | 0;
      setThrew(env, value || 1);
      throw new ArtemisLongjmp(value || 1);
    },

    /**
     * `___assert_fail(condition, filename, line, func)` — `assert()`'s failure arm.
     *
     * The glue formats the four pieces and calls `abort`, which would end the process: its own body is
     * `abort("Assertion failed: " + UTF8ToString(condition) + ", at: " + [filename ? … : "unknown
     * filename", line, func ? … : "unknown function"])` — note that the last three parts are an
     * *array* whose `toString` supplies the commas, and that `abort` then throws a
     * `WebAssembly.RuntimeError`. This host raises {@link ArtemisExit} with the same message instead:
     * the guest asked to die, and a session should be able to see why without losing the run. Null
     * `filename` / `func` become the glue's own literals, which is what a guest compiled without
     * `__FILE__` produces.
     */
    q: (condition: number, filename: number, line: number, func: number): void => {
      refresh();
      const text = utf8(condition);
      const where = filename ? utf8(filename) : 'unknown filename';
      const whereFunc = func ? utf8(func) : 'unknown function';
      throw new ArtemisExit(1, `Assertion failed: ${text}, at: ${where}, ${line}, ${whereFunc}`);
    },

    /**
     * `_abort()` — the `Ta` row. One short name, one implementation.
     *
     * `Ta` is listed twice in the import section, `()->void` and `(i32)->i32`, and both entries are
     * served by this body. `_Unwind_RaiseException` has **no definition of its own in the glue** —
     * the symbol does not appear in it at all — so what the second entry actually names is the
     * module's own import, and returning a "handled" result to it would let the guest run code after
     * the scope it was leaving. Raising is the only answer that cannot do that.
     */
    Ta: (): void => {
      throw new ArtemisExit(1, 'artemis guest called abort() / _Unwind_RaiseException');
    },
  };
}

/** The bridge's 15 distinct `eh` short names, in the order `imports-map.json` first lists them. */
export const ARTEMIS_EH_IMPORTS: readonly string[] = [
  'A', 'Db', 'I', 'L', 'Ta', 'c', 'g', 'i', 'q', 'r', 'u', 'vb', 'w', 'z', 'za',
];

/** The official name behind each `eh` short name, for a report a human can read. */
export const ARTEMIS_EH_OFFICIAL: Readonly<Record<string, string>> = {
  A: '___cxa_end_catch',
  Db: '___cxa_uncaught_exceptions',
  I: '_emscripten_longjmp',
  L: '_llvm_eh_typeid_for',
  Ta: '_abort',
  c: '___cxa_find_matching_catch_2',
  g: '___cxa_find_matching_catch_3',
  i: '___resumeException',
  q: '___assert_fail',
  r: '___cxa_begin_catch',
  u: '___cxa_allocate_exception',
  vb: '___cxa_find_matching_catch_8',
  w: '___cxa_throw',
  z: '___cxa_free_exception',
  za: '___cxa_rethrow',
};
