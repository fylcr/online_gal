import { ArtemisHostError } from './errno.js';
import type { ArtemisMemory } from './memory.js';

/**
 * The `runtime` bucket of `ports/artemis/harness/glue/imports-map.json` — 8 rows, 8 short names.
 *
 * This is the bucket that has nothing to do with syscalls and everything to do with the *toolchain*.
 * Emscripten leaves behind eight functions that only exist because of how the C++ was compiled:
 * three pthread-mutex-attribute stubs, a pair of `setjmp` scratch registers, and the JS-side
 * escape hatches that `EM_ASM` and `eval` compile into. Each one is small, each is exact, and
 * together they are the difference between a bridge that can boot and one that traps on the first
 * global constructor.
 *
 * Two of them are load-bearing for the boot and are therefore worth their reasoning here:
 *
 * - **`_emscripten_asm_const_int`** is how every `EM_ASM` in the engine reaches the page. The code
 *   number indexes a compile-time table of 14 JavaScript snippets; the signature string says how to
 *   read the arguments out of the guest's word buffer. This host has no page to run them on, so
 *   rather than returning a fabricated zero it *invokes a supplied handler* and records the code.
 *   A game whose bootstrap lives in an `EM_ASM` is then visibly blocked instead of silently
 *   receiving 0 for a canvas size.
 * - **`_emscripten_resize_heap`** is the only way the guest can grow its own linear memory, and it
 *   is where a 201 MB module runs into the 4 GB ceiling. The glue's growth policy is copied
 *   literally, because a host that grew by a different rule would move the guest's data segment
 *   every time a `malloc` tipped over a page boundary.
 */

/** One `EM_ASM` call, as the guest describes it. */
export interface ArtemisAsmConst {
  /** The compile-time constant index. The glue's `ASM_CONSTS` table has 14 entries. */
  readonly code: number;
  /** The signature string, NUL-terminated in the guest heap. A char below `'i'` means a double. */
  readonly signature: string;
  /** The decoded arguments, in order. i32 slots are numbers; f64 slots are numbers too. */
  readonly args: readonly number[];
}

/** Runs one `EM_ASM` snippet and returns its result, which the guest reads back as an i32. */
export type ArtemisAsmConstRunner = (call: ArtemisAsmConst) => number;

/**
 * Emscripten's `setTempRet0` / `getTempRet0` scratch cell.
 *
 * It is a *class* rather than a closure because the `eh` bucket needs the same cell: the
 * `___cxa_find_matching_catch_*` family returns the matched type through it as well as through its
 * own return value, and two cells would mean a type that silently goes missing. A session wires one
 * instance into both tables.
 */
export class ArtemisTempRet0 {
  private value = 0;

  /** `setTempRet0(value)` — stores `value | 0`, because the cell is an i32. */
  set(value: number): void {
    this.value = value | 0;
  }

  /** `getTempRet0()`. */
  get(): number {
    return this.value | 0;
  }
}

/** What a session must supply for `EM_ASM` to reach anything. */
export interface ArtemisRuntimeHost {
  readonly memory: ArtemisMemory;
  /**
   * The shared `setTempRet0` cell. Absent means this table makes its own, which is right for a test
   * that only exercises `runtime` and wrong for a session that also has the `eh` table.
   */
  readonly temp?: ArtemisTempRet0;
  /**
   * Runs an `EM_ASM` snippet. Absent means the page-side script table is unavailable, and every
   * `EM_ASM` reports a block rather than answering 0.
   */
  readonly runAsmConst?: ArtemisAsmConstRunner;
  /** The guest's `malloc`, needed by `_emscripten_run_script_string` to hand a string back. */
  readonly malloc?: (bytes: number) => number;
  /** The guest's `free`, used when a cached string is replaced. */
  readonly free?: (pointer: number) => void;
  /** Evaluates a guest-supplied script string. Absent means `eval` is refused. */
  readonly evaluate?: (source: string) => unknown;
  /** Lets a host cap growth below the module's own 65536-page maximum. */
  readonly maxHeapBytes?: number;
  /** Reports a call this host could not serve, so a session can log it instead of swallowing it. */
  readonly onBlocked?: (what: string, detail: string) => void;
}

/** The import table, keyed by the wasm short name. */
export type ArtemisRuntimeImports = Record<string, (...args: number[]) => number | void>;

/** The glue's `MAXIMUM_MEMORY`: 4 GiB. A wasm32 module cannot address more. */
const MAX_HEAP_BYTES = 4294967296;

/** The glue's `MINIMUM_MEMORY`: 16 MiB, the floor every grow is raised to. */
const MIN_HEAP_BYTES = 16777216;

/** wasm memory grows in 64 KiB pages; the glue aligns every new size up to one. */
const PAGE_BYTES = 65536;

/** The glue's `__ASSERTIONS` slack: how far past the request an over-grow is allowed to reach. */
const GROW_SLACK_BYTES = 100663296;

/** The `ASM_CONSTS` code numbers the shipped build actually contains, from the glue's table. */
const KNOWN_ASM_CONST_CODES: ReadonlySet<number> = new Set([
  710692, 710801, 710864, 710922, 710967, 711012, 712047, 712081, 712896, 712921, 716400, 716732,
  716748, 717384,
]);

/** Builds the `runtime` import table for one instance. */
export function createArtemisRuntimeImports(host: ArtemisRuntimeHost): ArtemisRuntimeImports {
  const { memory } = host;
  const report = (what: string, detail: string): void => host.onBlocked?.(what, detail);

  /**
   * `setTempRet0` / `getTempRet0` — emscripten's single scratch i32 for a return value that has to
   * travel out of a function whose wasm signature returns nothing.
   *
   * The `___cxa_find_matching_catch_*` family returns its matched type through this slot as well as
   * through its own return value, so this is shared state, not a private scratchpad: `eh.ts` reads
   * and writes the same cell.
   */
  const temp = host.temp ?? new ArtemisTempRet0();

  /** `_emscripten_run_script_string`'s cached result buffer, kept on the function as the glue does. */
  const scriptBuffer = { pointer: 0, size: 0 };

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

  return {
    /**
     * `_pthread_mutexattr_init(attr)` — an empty body in the glue, returning undefined → 0.
     *
     * There is no pthread support to configure, so there is nothing to write. The guest still
     * believes it holds a `pthread_mutexattr_t`; since every one of the three attribute functions
     * is a no-op, the guest's own initialisation of the struct it later passes around is the only
     * state that matters, and the guest allocated it.
     */
    Bb: (): number => 0,

    /** `_pthread_mutexattr_destroy(attr)` — likewise empty. Freeing is the guest's `pthread_mutex_destroy`. */
    Aa: (): number => 0,

    /** `_pthread_mutexattr_settype(attr, type)` — likewise empty; the glue returns undefined → 0. */
    Ab: (): number => 0,

    /**
     * `setTempRet0(value)` — writes emscripten's scratch return cell.
     *
     * The glue's body is `setTempRet0($i|0)`, and the `| 0` is the whole contract: the cell is an
     * i32, so a double or a 64-bit value coming in is truncated to 32 bits before it is stored.
     */
    C: (value: number): void => {
      temp.set(value);
    },

    /** `getTempRet0()` — reads that cell back. The glue's `getTempRet0()|0` is a no-op on an i32. */
    b: (): number => temp.get(),

    /**
     * `_emscripten_asm_const_int(code, sigPtr, argbuf)`.
     *
     * The glue reads its arguments with `readAsmConstArgs(sigPtr, argbuf)`, and the reader's rules
     * are the part worth being exact about, because they are not what a C signature suggests:
     *
     * - the signature is a run of ASCII chars terminated by a NUL, not a `,`-separated list;
     * - **a char below `'i'` (105) means the argument is a double**, everything above means i32 —
     *   so `d` is a double and `i` is an int, which reads backwards but is what the table does;
     * - `argbuf` is a **word index into the guest's i32/f64 space**, not a byte address: it is
     *   `>>= 2`'d first, and each argument consumes one or two words;
     * - a double is skipped forward to an even word index, so a caller that packed `i32, double`
     *   wrote a pad word.
     *
     * `EMU_ASM_DOUBLE` is what makes a char below `'i'` a double, and the guest encodes it in the
     * signature the same way. Copying the reader rather than the C type is the only faithful choice.
     */
    B: (code: number, sigPtr: number, argbuf: number): number => {
      const signature = utf8(sigPtr);
      const args: number[] = [];
      // `argbuf` arrives as a byte address and is divided by 4 to become a word index, so every
      // read below is indexed by word. A double occupies two words, hence `buf++` twice.
      let word = (argbuf | 0) >>> 2;
      for (let index = 0; index < signature.length; index += 1) {
        const char = signature.charCodeAt(index);
        const isDouble = char < 105;
        // A double must land on an even word index; `buf&1` is the glue's test, and `buf++` the fix.
        if (isDouble && word & 1) word += 1;
        if (isDouble) {
          args.push(memory.f64[word >>> 1] ?? 0);
          word += 1;
        } else {
          args.push(memory.i32[word] ?? 0);
        }
        word += 1;
      }
      if (!KNOWN_ASM_CONST_CODES.has(code)) {
        report('asm_const', `code ${code} is not in the shipped 14-entry ASM_CONSTS table`);
      }
      if (!host.runAsmConst) {
        report('asm_const', `EM_ASM code ${code} needs a runAsmConst handler this host does not have`);
        // The glue would have thrown out of `ASM_CONSTS[code].apply`, so the return of 0 here is
        // only reached on a path that already reported itself as blocked.
        return 0;
      }
      let result: number;
      try {
        result = host.runAsmConst({ code, signature, args });
      } catch (error) {
        throw new ArtemisHostError(`artemis EM_ASM code ${code} threw: ${String(error)}`);
      }
      return result | 0;
    },

    /**
     * `_emscripten_run_script_string(ptr)`.
     *
     * The glue `eval`s the string and, if the result is not null, copies it into a cached guest
     * buffer with room for the NUL and returns that pointer. The buffer is grown only when the
     * result outgrows it, and the old one is freed — an allocation discipline worth copying,
     * because the guest frees this pointer and would be reading freed memory if a second, shorter
     * result reused a stale buffer without resizing.
     *
     * With no evaluator installed this returns 0, which is also what the glue returns when the
     * script evaluates to `null`. The difference is reported, so the two are not confused.
     */
    Sa: (ptr: number): number => {
      const source = utf8(ptr);
      if (!host.evaluate) {
        report('run_script_string', `refused to eval ${source.length} B of guest-supplied script`);
        return 0;
      }
      let value: unknown;
      try {
        value = host.evaluate(source);
      } catch (error) {
        throw new ArtemisHostError(`artemis run_script_string threw: ${String(error)}`);
      }
      if (value === null || value === undefined) return 0;
      const text = `${String(value)}`;
      const bytes = new TextEncoder().encode(text);
      if (scriptBuffer.size < bytes.length + 1) {
        if (scriptBuffer.pointer) host.free?.(scriptBuffer.pointer);
        if (!host.malloc) {
          report('run_script_string', 'the guest malloc is not available, so the result cannot be returned');
          return 0;
        }
        scriptBuffer.pointer = host.malloc(bytes.length + 1) | 0;
        scriptBuffer.size = scriptBuffer.pointer === 0 ? 0 : bytes.length + 1;
      }
      if (scriptBuffer.pointer === 0) return 0;
      memory.u8.set(bytes, scriptBuffer.pointer >>> 0);
      // The guest reads this as a C string, so the terminator is part of the contract, not padding.
      memory.u8[scriptBuffer.pointer + bytes.length] = 0;
      return scriptBuffer.pointer;
    },

    /**
     * `_emscripten_resize_heap(requestedSize)` — the guest's only way to grow its own memory.
     *
     * The glue's policy is peculiar and deliberate, and it is copied here line for line:
     *
     * 1. a request above 4 GiB is refused outright, because wasm32 cannot address it;
     * 2. every candidate is raised to at least 16 MiB, so the first grow does not produce a
     *    pathologically small heap;
     * 3. each of four attempts grows by `oldSize * (1 + 0.2/cutDown)` with `cutDown` doubling, so
     *    the first attempt over-grows most and later ones are gentler — the sequence is a hedge
     *    against a platform that refuses a large grow;
     * 4. each candidate is capped at `requestedSize + 96 MiB`, which bounds how much slack a
     *    single call can add, and at 4 GiB;
     * 5. the size is rounded up to a whole 64 KiB page, because `memory.grow` takes pages.
     *
     * The 0.2 first-slope matters: growing a 192 MiB heap by 20% costs 38 MiB, and a game that
     * asks for 4 KiB more does not get 38 MiB. The `cutDown` ramp is what stops that repeating.
     */
    zb: (requestedSize: number): number => {
      const want = requestedSize >>> 0;
      if (want > MAX_HEAP_BYTES) {
        report('resize_heap', `requested ${want} B exceeds the 4 GiB wasm32 ceiling`);
        return 0;
      }
      const ceiling = Math.min(MAX_HEAP_BYTES, host.maxHeapBytes ?? MAX_HEAP_BYTES);
      const oldSize = memory.byteLength;
      for (let cutDown = 1; cutDown <= 4; cutDown *= 2) {
        const overGrown = oldSize * (1 + 0.2 / cutDown);
        const capped = Math.min(overGrown, want + GROW_SLACK_BYTES);
        const target = Math.min(ceiling, Math.max(MIN_HEAP_BYTES, want, capped));
        const pages = Math.ceil(target / PAGE_BYTES);
        const delta = pages - memory.pages;
        if (delta <= 0) return 1;
        try {
          // `grow` detaches the old ArrayBuffer, which is why every view has to be re-derived —
          // `ArtemisMemory.grow` does that, and a stale view would read zeros from here on.
          memory.grow(delta);
          return 1;
        } catch (error) {
          report('resize_heap', `growing to ${pages} pages failed: ${String(error)}`);
        }
      }
      return 0;
    },
  };
}

/** The bridge's 8 `runtime` short names, in the order `imports-map.json` first lists them. */
export const ARTEMIS_RUNTIME_IMPORTS: readonly string[] = [
  'Aa', 'Ab', 'B', 'Bb', 'C', 'Sa', 'b', 'zb',
];

/** The official name behind each `runtime` short name, for a report a human can read. */
export const ARTEMIS_RUNTIME_OFFICIAL: Readonly<Record<string, string>> = {
  Aa: '_pthread_mutexattr_destroy',
  Ab: '_pthread_mutexattr_settype',
  B: '_emscripten_asm_const_int',
  Bb: '_pthread_mutexattr_init',
  C: '_setTempRet0',
  Sa: '_emscripten_run_script_string',
  b: '_getTempRet0',
  zb: '_emscripten_resize_heap',
};
