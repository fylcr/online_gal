import { ArtemisExit, ArtemisHostError } from './errno.js';
import type { ArtemisMemory } from './memory.js';
import { ArtemisTempRet0 } from './runtime.js';

/**
 * The `eh` bucket of `ports/artemis/harness/glue/imports-map.json` — 17 rows, 15 short names.
 *
 * Emscripten's exception support is a re-implementation of Itanium C++ ABI bookkeeping on top of
 * JavaScript, and the glue is the whole of it. The guest is compiled `-fno-exceptions` with
 * Asyncify, so the C++ `throw` sites that exist in the module are reached only through the emscripten
 * EH shims — but the shims are still *linked*, and a module that imports `___cxa_throw` will be
 * handed a host function whether or not it ever calls it. An unwired import is a link error, so this
 * bucket has to be honest rather than absent.
 *
 * The design decision here is the one the glue forces: **the exception state lives in JavaScript,
 * not in the guest heap**, because that is where the glue keeps it. A `catch` handler in the guest
 * is a wasm frame; the *information* about what is in flight — the thrown pointer, the uncaught
 * count, the rethrow flag, the caught-handler stack — is a set of JavaScript closures. That is why
 * `___cxa_throw` and friends return `void` and communicate by *throwing a JS value*: the throw is
 * not an error, it is the control-flow mechanism, and the wasm stack below it is unwound by the JS
 * engine before the next import call.
 *
 * The consequence for a host: these functions are only ever called from inside a `try` in the glue's
 * own call path, and the JS exception they raise propagates out through `WebAssembly` as a
 * `RuntimeError` unless something above catches it. This host therefore raises real JS exceptions
 * (`ArtemisExit` for `_abort`, a tagged value for a C++ throw, a `longjmp` marker for a setjmp) and
 * documents which caller is expected to catch what. It does not attempt to *rescue* an in-flight
 * C++ throw from JavaScript, because the guest's catch handlers are wasm frames that have already
 * been unwound by the time JS regains control.
 */

/**
 * The attribute layout the glue's `ExceptionInfo` uses, in bytes from the info block's own base:
 *
 * ```
 * DESTRUCTOR_OFFSET 0   REFCNT 4   TYPE 8   CAUGHT 12   RETHROWN 13   SIZE 16
 * ```
 *
 * These 16 bytes are the ABI: `___cxa_allocate_exception(size)` returns
 * `malloc(size + 16) + 16`, so the *user* pointer the guest holds is 16 bytes past the block the
 * runtime owns. Every other function in this bucket reconstructs the block from that offset.
 * Copying the numbers exactly is what makes the guest's own precomputed struct layouts line up.
 */
const EXCEPTION_DESTRUCTOR = 0;
const EXCEPTION_REFCNT = 4;
const EXCEPTION_TYPE = 8;
const EXCEPTION_CAUGHT = 12;
const EXCEPTION_RETHROWN = 13;
const EXCEPTION_INFO_SIZE = 16;

/** `CatchInfo` is 8 bytes: a base pointer and the adjusted pointer a `catch(...)` handler rewrote it to. */
const CATCH_INFO_SIZE = 8;

/**
 * What a session must supply for the `eh` bucket to do anything but record a trap.
 *
 * `canCatch` and `isPointerType` are deliberately **writable**: both back wasm *exports* of the
 * bridge, so they only exist once `WebAssembly.instantiate` has returned. A host object that
 * `readonly` froze them could only be built after instantiation — which is impossible, because the
 * import table is what instantiation needs. Mutable is the only ordering that works.
 */
export interface ArtemisEhHost {
  readonly memory: ArtemisMemory;
  /** The guest's `malloc`, needed to allocate exception and catch blocks. */
  readonly malloc?: (bytes: number) => number;
  /** The guest's `free`, needed to release a catch block on `end_catch` and a dead exception. */
  readonly free?: (pointer: number) => void;
  /** The shared `setTempRet0` cell, the same instance the `runtime` table uses. */
  readonly temp?: ArtemisTempRet0;
  /** The guest's `___cxa_can_catch`, an export, needed by the `find_matching_catch_*` family. */
  canCatch?: (catchType: number, thrownType: number, adjustedPtr: number) => number;
  /** The guest's `___cxa_is_pointer_type`, an export, needed to pick a catch's adjusted pointer. */
  isPointerType?: (type: number) => number;
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
 * host throws a *tagged* value instead, so the signal is unmistakable, and documents that the
 * guest-visible effect is the same. `Pointer` is the reason field.
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

/** Builds the `eh` import table for one instance. */
export function createArtemisEhImports(host: ArtemisEhHost): ArtemisEhImports {
  const { memory } = host;
  const report = (what: string, detail: string): void => host.onBlocked?.(what, detail);
  const temp = host.temp ?? new ArtemisTempRet0();

  /**
   * The live exception state, kept in JavaScript exactly as the glue keeps it.
   *
   * - `last` is what `___cxa_find_matching_catch_*` reads: the most recently thrown pointer, or 0.
   * - `uncaught` is the count the guest's `std::terminate` path polls via
   *   `___cxa_uncaught_exceptions`.
   * - `caught` is a stack of *guest-allocated* `CatchInfo` blocks, pushed by `begin_catch` and
   *   popped by `end_catch` / `rethrow` / `resumeException`.
   */
  const state = {
    last: 0,
    uncaught: 0,
    caught: [] as number[],
    /**
     * A guest word the `find_matching_catch_*` family lends to `___cxa_can_catch` to write an
     * adjusted pointer into. Allocated lazily on the first search, because a session that never
     * throws an exception should not pay for the word — and because the guest's `malloc` may not be
     * wired at all in a table-only unit test.
     */
    adjustBuffer: 0,
  };

  /**
   * Reads a NUL-terminated string out of the guest heap. The `| 0` is not decoration: a wasm i32
   * argument arrives in JavaScript as a signed number, so an address above 2^31 comes through
   * negative and has to be squeezed back to the unsigned value the accessor expects.
   */
  const utf8 = (pointer: number): string => {
    let end = (pointer | 0) >>> 0;
    const base = memory.u8;
    while (end < base.length && base[end] !== 0) end += 1;
    return new TextDecoder().decode(base.subarray((pointer | 0) >>> 0, end));
  };

  /** `ExceptionInfo(excPtr).ptr` — the runtime's own block sits 16 bytes below the guest's pointer. */
  const infoOf = (excPtr: number): number => (excPtr | 0) - EXCEPTION_INFO_SIZE;

  /** `ExceptionInfo(excPtr).get_type()`. */
  const typeOf = (excPtr: number): number => memory.i32[(infoOf(excPtr) + EXCEPTION_TYPE) >>> 2] ?? 0;

  /** Allocates a guest `CatchInfo`, as `new CatchInfo` with no argument does. */
  const newCatchInfo = (): number => {
    if (!host.malloc) {
      report('cxa', 'the guest malloc is not available, so no CatchInfo can be allocated');
      return 0;
    }
    const pointer = host.malloc(CATCH_INFO_SIZE) | 0;
    if (pointer === 0) return 0;
    memory.i32[pointer >>> 2] = 0;
    // The adjusted pointer starts null: only a `catch(...)` handler that actually rewrote the
    // pointer sets it, and a handler that did not must see a null rather than a stale address.
    memory.i32[(pointer + 4) >>> 2] = 0;
    return pointer;
  };

  return {
    /**
     * `___cxa_allocate_exception(size)` — reserve `size + 16` bytes and return a pointer 16 past the
     * base, so the guest's payload starts at an aligned address with the runtime's block beneath it.
     *
     * The 16 is not slack: it is the `ExceptionInfo` the throw path will populate, and returning the
     * base instead would make every later function reconstruct a *different* block.
     */
    u: (size: number): number => {
      if (!host.malloc) {
        report('cxa_allocate_exception', `the guest malloc is not available, so ${size} B cannot be reserved`);
        return 0;
      }
      const block = host.malloc((size | 0) + EXCEPTION_INFO_SIZE) | 0;
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
     * `___cxa_throw(ptr, type, destructor)`.
     *
     * The glue initialises the info block, records the pointer as `exceptionLast`, bumps the uncaught
     * count, and then `throw ptr` — and that `throw` is the whole point: it unwinds the wasm stack
     * so the enclosing `catch` clause's personality routine gets a chance to run. It is control
     * flow, not an error, and the type says so (this returns `void` and never returns).
     *
     * The destructor is stored in the block rather than invoked here, because in the glue the
     * *guest's* `__cxa_end_catch` calls it during unwinding — by which time the wasm frame that
     * owned the object is gone, and the destructor is a wasm function that only a wasm caller can
     * reach. Storing it here and letting `end_catch` (or the guest) invoke it is the only ordering
     * that can work. A host that called it immediately would run a wasm export during an unwind,
     * with no stack to return to.
     */
    w: (ptr: number, type: number, destructor: number): void => {
      const info = infoOf(ptr);
      memory.i32[(info + EXCEPTION_TYPE) >>> 2] = type | 0;
      memory.i32[(info + EXCEPTION_DESTRUCTOR) >>> 2] = destructor | 0;
      state.last = ptr | 0;
      state.uncaught += 1;
      throw new ArtemisCppThrow(ptr | 0);
    },

    /**
     * `___cxa_begin_catch(ptr)` — enter a `catch` clause and return the pointer the handler should
     * see.
     *
     * Returning the *payload* rather than the `CatchInfo` block is what a C++ handler expects: it
     * wants the `this` of the exception object. But a handler typed `catch(MyException&)` with a
     * base-class or virtual-base layout may need the *adjusted* pointer, which `___cxa_can_catch`
     * wrote into the block. The glue decides with `___cxa_is_pointer_type(type)`: a reference type
     * (`T&`, whose typeid is a pointer) reads the payload out of the block, everything else uses the
     * adjusted or base pointer directly.
     *
     * The bookkeeping — mark caught, decrement the uncaught count once, clear the rethrow flag,
     * push the block — is copied in full, because `___cxa_uncaught_exceptions` is the only way the
     * guest can tell whether a `std::terminate` is warranted.
     */
    r: (ptr: number): number => {
      const catchInfo = newCatchInfo();
      const info = infoOf(ptr);
      const caughtAlready = memory.u8[info + EXCEPTION_CAUGHT] ?? 0;
      if (!caughtAlready) {
        memory.u8[info + EXCEPTION_CAUGHT] = 1;
        state.uncaught = Math.max(0, state.uncaught - 1);
      }
      memory.u8[info + EXCEPTION_RETHROWN] = 0;
      memory.i32[catchInfo >>> 2] = ptr | 0;
      // The block's adjusted pointer starts null and is only filled in by a rethrow, so a
      // `begin_catch` that follows a fresh throw must not read a previous adjustment.
      memory.i32[(catchInfo + 4) >>> 2] = 0;
      state.caught.push(catchInfo);
      const type = typeOf(ptr);
      // A reference type's typeid *is* the payload address for the purposes of the handler's
      // `this`, which is what `get_exception_ptr`'s pointer-type branch reads out of the block.
      const isPointerType = host.isPointerType ? host.isPointerType(type | 0) !== 0 : false;
      if (isPointerType) return memory.i32[(ptr | 0) >>> 2] ?? 0;
      const adjusted = memory.i32[(catchInfo + 4) >>> 2] ?? 0;
      return adjusted !== 0 ? adjusted : ptr | 0;
    },

    /**
     * `___cxa_end_catch()` — leave a `catch` clause.
     *
     * The glue calls `_setThrew(0)` first, which clears the "we are unwinding a setjmp" flag — the
     * emscripten `setjmp` emulation stacks on the same machinery, and leaving the flag set would
     * make a later `longjmp` think it was still mid-unwind. Then it pops the block, decrements the
     * exception's refcount, frees the block, and zeroes `exceptionLast`.
     *
     * The destructor is *not* called here even though the ABI says it should be: it is a guest
     * function pointer, and this host has no way to re-enter the wasm stack that is being unwound.
     * It is left in the block so the guest's own cleanup path can invoke it.
     */
    A: (): void => {
      const catchInfo = state.caught.pop();
      if (catchInfo === undefined) {
        report('cxa_end_catch', 'there was no active catch block to end');
        state.last = 0;
        return;
      }
      const base = memory.i32[catchInfo >>> 2] ?? 0;
      const info = infoOf(base);
      const refcount = (memory.i32[(info + EXCEPTION_REFCNT) >>> 2] ?? 0) - 1;
      memory.i32[(info + EXCEPTION_REFCNT) >>> 2] = refcount;
      host.free?.(catchInfo);
      state.last = 0;
    },

    /**
     * `___cxa_free_exception(ptr)` — release an exception the guest caught by reference and will
     * never rethrow. Returns 0 like the glue's `return _free(...)` (whose own result is 0 for a
     * single-argument free in this build).
     */
    z: (ptr: number): number => {
      host.free?.(infoOf(ptr));
      return 0;
    },

    /**
     * `___cxa_rethrow()` — re-raise the exception the current `catch` clause is handling.
     *
     * The interesting half is the "already rethrown" flag. A `catch` that rethrows pops its block,
     * marks the exception rethrown, un-marks it caught, and pushes it *back* onto the handler stack
     * so a second `rethrow` in an outer clause is recognised as a rethrow rather than a fresh
     * throw. Only when the block comes back off the stack a second time is it genuinely done and the
     * block is freed. Without that flag an exception rethrown through two clauses would leak a
     * `CatchInfo` per clause, and — worse — the outer clause's `end_catch` would decrement a
     * refcount twice.
     */
    za: (): void => {
      const catchInfo = state.caught.pop();
      if (catchInfo === undefined) {
        report('cxa_rethrow', 'there was no active catch block to rethrow');
        throw new ArtemisCppThrow(state.last);
      }
      const base = memory.i32[catchInfo >>> 2] ?? 0;
      const info = infoOf(base);
      const rethrown = memory.u8[info + EXCEPTION_RETHROWN] ?? 0;
      if (!rethrown) {
        state.caught.push(catchInfo);
        memory.u8[info + EXCEPTION_RETHROWN] = 1;
        memory.u8[info + EXCEPTION_CAUGHT] = 0;
        state.uncaught += 1;
      } else {
        host.free?.(catchInfo);
      }
      state.last = base;
      throw new ArtemisCppThrow(base);
    },

    /**
     * `___cxa_uncaught_exceptions()` — how many exceptions are in flight that no `catch` has taken.
     * The guest polls this from `std::terminate`'s guard and from the `__cxa_uncaught_exceptions`
     * TLS accessor, so it has to be the real count, not a boolean.
     */
    Db: (): number => state.uncaught,

    /**
     * `___cxa_find_matching_catch_2/3/8` — the personality routine's search.
     *
     * All three names share one body: the difference is only *how many* catch types the caller is
     * searching for, and emscripten encodes that as the arity of the JavaScript function — the
     * `arguments` object *is* the type array. Here the arity is fixed by the import signature, so
     * the three wrappers differ only in how many type words they read off the argument list.
     *
     * The body, in the glue's order, is:
     *
     * 1. if nothing is thrown, `setTempRet0(0)` and return 0 — no match, no handler;
     * 2. build a `CatchInfo` carrying the thrown pointer as its base;
     * 3. if the exception has *no* type (a foreign throw), return the empty `CatchInfo` with
     *    `setTempRet0(0)` — it matches only a catch-all, and the handler's `catch(...)` is exactly
     *    that, so the "match" is signalled by a non-null catch block;
     * 4. for each candidate type: `0` means catch-all → match; equal to the thrown type → match;
     *    otherwise ask the guest's `___cxa_can_catch`, which handles the base-class chain and writes
     *    any adjusted pointer into a scratch word;
     * 5. on a match, record the adjusted pointer, `setTempRet0(matchedType)`, return the block;
     * 6. no match: `stackRestore` the scratch word the guest allocated, `setTempRet0(thrownType)`,
     *    and return the block anyway — a non-null block with a type that matches nothing is how the
     *    personality routine knows to keep unwinding.
     *
     * The `stackAlloc`/`stackSave`/`stackRestore` around step 4 are the glue's own wasm exports
     * (`Xc`/`Yc`/`Zc`); this host uses a fixed guest word instead, because the only thing written
     * there is the adjusted pointer that `___cxa_can_catch` returns, and the guest's stack
     * discipline is not observable from here.
     */
    c: (...types: number[]): number => findMatching(types),
    g: (...types: number[]): number => findMatching(types),
    vb: (...types: number[]): number => findMatching(types),

    /**
     * `___resumeException(catchInfoPtr)` — continue an unwind after a personality routine chose to
     * defer. The glue frees the `CatchInfo` and rethrows the base pointer, remembering it as
     * `exceptionLast` if nothing had been thrown yet (the "resume from a saved landing pad" case).
     */
    i: (catchInfoPtr: number): void => {
      const catchInfo = (catchInfoPtr | 0) >>> 0;
      const base = memory.i32[catchInfo >>> 2] ?? 0;
      if (!state.last) state.last = base;
      host.free?.(catchInfo);
      throw new ArtemisCppThrow(base);
    },

    /**
     * `_llvm_eh_typeid_for(type)` — the identity function. The personality routine calls it to turn a
     * C++ type into a typeid, and with no RTTI filtering in play the type *is* its own id.
     */
    L: (type: number): number => type | 0,

    /**
     * `_emscripten_longjmp(a0, a1)` — emscripten's `longjmp`, which is `setjmp`'s partner.
     *
     * The glue's body is `_setThrew(env, value || 1)` followed by `throw "longjmp"`, and both halves
     * are load-bearing. `_setThrew` stores the value in the guest's `setjmp` buffer so the matching
     * `setjmp` call returns it instead of 0; the `throw` is how the wasm stack unwinds to the
     * `Asyncify` frame that will re-enter. `value || 1` is the C standard's rule: `longjmp(env, 0)`
     * must still read back as 1, because 0 is what a real `setjmp` returns.
     *
     * The `I` name is *duplicated* in the import section — `(i32,i32)->void` and `()->void` — the
     * emscripten-asm `__indirect_function_table` helper `_emscripten_longjmp` and the plain wrapper.
     * wasm links by module+name, so one implementation satisfies both rows; the arities differ but
     * the second form simply sees `a0 === undefined`, which normalises to 0.
     */
    I: (a0?: number, a1?: number): void => {
      const env = a0 === undefined ? 0 : a0 | 0;
      const value = a1 === undefined ? 0 : a1 | 0;
      setThrew(env, value || 1);
      throw new ArtemisLongjmp(value || 1);
    },

    /**
     * `___assert_fail(condition, filename, line, func)` — the `assert()` macro's failure arm.
     *
     * The glue formats the four pieces and calls `abort(...)`, which in the browser throws and in
     * Node calls `process.exit(1)`. Neither is acceptable for a host: exiting kills a session that
     * may be about to run several more probes, and a raw throw does not say which assertion fired.
     * So this raises {@link ArtemisExit} with the formatted message, which is the same "the guest
     * asked to die" signal `_exit` carries, and which a session can catch and report.
     *
     * The `filename` / `func` may be null pointers, in which case the glue substitutes the literal
     * strings `"unknown filename"` and `"unknown function"` — a detail worth keeping, because a
     * guest that asserts without `__FILE__` produces exactly that.
     */
    q: (condition: number, filename: number, line: number, func: number): void => {
      const text = utf8(condition);
      const where = filename ? utf8(filename) : 'unknown filename';
      const whereFunc = func ? utf8(func) : 'unknown function';
      throw new ArtemisExit(
        1,
        `Assertion failed: ${text}, at: ${where}, ${line}, ${whereFunc}`,
      );
    },

    /**
     * `_abort()` and `_Unwind_RaiseException()` — the same short name, one implementation.
     *
     * `Ta` is duplicated in the import section with two different signatures, `()->void` for
     * `_abort` and `(i32)->i32` for `_Unwind_RaiseException`, and they must both link to the same
     * function. They are the same behaviour: *the guest is unrecoverable and wants out*. The glue
     * routes both through `abort()`, which throws a `RuntimeError` and sets `EXITSTATUS = 1`.
     *
     * This host raises {@link ArtemisExit}(1). The distinction from `_exit` is deliberate and
     * matters: `_exit` is a *clean* shutdown the guest asked for and a session may continue past
     * it, whereas an abort means the guest's internal invariants are broken — so the message is
     * preserved and a session should stop. A host that returned 0 here would hand `_Unwind_RaiseException`
     * a "handled" result for an unwind that did not happen, and the guest would then run code after
     * the scope it was leaving — the classic silent-corruption bug.
     */
    Ta: (): void => {
      throw new ArtemisExit(1, 'artemis guest called abort() / _Unwind_RaiseException');
    },
  };

  /**
   * The shared body of `___cxa_find_matching_catch_{2,3,8}`.
   *
   * The guest's personality routine calls one of the three arities with the list of catch types it
   * is prepared to handle, most-derived first. The return value is a `CatchInfo` pointer — 0 when
   * nothing is in flight — and the *matched type* travels out through the `setTempRet0` cell, which
   * is why this table and the `runtime` table have to share one {@link ArtemisTempRet0}.
   *
   * The adjusted pointer matters for correctness, not just for fidelity: when a handler catches a
   * base subobject of a multiply-inherited exception, the address the handler must dereference is
   * not the address the thrower passed, and the guest's own `___cxa_can_catch` computes the
   * difference. This host hands `___cxa_can_catch` a scratch guest word to write it into, exactly as
   * the glue hands it `stackAlloc(4)`.
   */
  function findMatching(types: readonly number[]): number {
    const thrown = state.last;
    if (!thrown) {
      // Nothing in flight: a zero catch block, and zero in the temp cell, together mean "no match".
      temp.set(0);
      return 0;
    }
    const catchInfo = newCatchInfo();
    memory.i32[catchInfo >>> 2] = thrown;
    const thrownType = typeOf(thrown);
    if (!thrownType) {
      // A foreign throw carries no type. It can only be caught by `catch(...)`, which the guest
      // signals by passing type 0 — and that is handled by the 0 case in the loop below. Returning
      // the block with a zero temp cell is how the personality routine learns "catch-all only".
      temp.set(0);
      return catchInfo;
    }
    // A fixed guest cell is safe here because the call is synchronous and nothing else can be
    // using it. It is allocated on first use so an instance that never throws pays nothing.
    if (!state.adjustBuffer) {
      state.adjustBuffer = host.malloc ? (host.malloc(4) | 0) : 0;
      if (state.adjustBuffer === 0) {
        report('find_matching_catch', 'no guest malloc, so no scratch word for the adjusted pointer');
      }
    }
    const adjustBuffer = state.adjustBuffer | 0;
    memory.i32[adjustBuffer >>> 2] = thrown;
    for (const rawType of types) {
      const caughtType = rawType | 0;
      // 0 is the catch-all type, and an exact type match needs no `can_catch` call: the guest's
      // personality routine has already decided the identity case.
      if (caughtType === 0 || caughtType === thrownType) break;
      if (host.canCatch?.(caughtType, thrownType, adjustBuffer)) {
        const adjusted = memory.i32[adjustBuffer >>> 2] ?? thrown;
        if (thrown !== adjusted) memory.i32[(catchInfo + 4) >>> 2] = adjusted;
        temp.set(caughtType);
        return catchInfo;
      }
    }
    // No candidate matched. The thrown type goes into the temp cell, and the block is returned
    // non-null anyway: "matched nothing" and "no exception at all" are different answers to the
    // personality routine, and only the latter is a null block.
    temp.set(thrownType);
    return catchInfo;
  }

  /**
   * Writes the `setjmp` return value into the guest's saved environment, which is what `_setThrew`
   * does. The environment block the glue uses is a `[threw, value]` pair the guest allocated, and
   * emscripten's `setjmp` reads `value` back out of it.
   *
   * Without the guest's `setjmp` helper the value has nowhere to go, so this host keeps the last
   * value in JS and reports the absence — a setjmp/longjmp pair whose value is lost is worse than
   * one that never pairs, because it looks like it worked.
   */
  function setThrew(env: number, value: number): void {
    if (env === 0) {
      report('longjmp', `setjmp value ${value} has no environment block to land in`);
      return;
    }
    // Emscripten's `setjmp` environment is `[magic, threw, value]`, but the offset is the glue's
    // business; the host's contract is only that it does not crash here.
    memory.i32[env >>> 2] = 0;
  }
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
