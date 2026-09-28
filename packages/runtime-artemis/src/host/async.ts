/**
 * The `async` bucket: 30 rows, 30 short names, one shape.
 *
 * asyncify rewrites every indirect call in the guest into a call to an `invoke_*` trampoline, and
 * that trampoline is the frame that unwinds. The official glue implements all 30 the same way
 * (`ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js`, `invoke_iii` at byte 269172):
 *
 * ```js
 * function invoke_iii(index, a1, a2) {
 *   var sp = stackSave();
 *   try { return dynCall_iii(index, a1, a2); }
 *   catch (e) { stackRestore(sp); if (e !== e + 0 && e !== "longjmp") throw e; _setThrew(1, 0); }
 * }
 * ```
 *
 * Three things in that body are load-bearing, and the host reproduces all three.
 *
 * The `dynCall_*` it calls is not the guest's function table — it is the module's own **exported**
 * dynCall, which emscripten generates alongside the trampoline. So this bucket needs no `Lc` access
 * and no arity bookkeeping: the host hands the arguments to a function of the module's own making
 * and the wasm engine does the table lookup, the argument marshalling and the `__THREW__` save and
 * restore. The glue's own forwarder says so — `var dynCall_iii=Module["dynCall_iii"]=Module["asm"]["cd"]`
 * — and the disassembly of `cd` confirms it wraps an internal call rather than doing the work here.
 *
 * The `catch` re-raises anything that is not an unwind. The glue's test `e !== e + 0 && e !==
 * "longjmp"` accepts a **number** (which is what `___cxa_throw` throws — a bare exception pointer)
 * and the string `"longjmp"` (which is what `_emscripten_longjmp` throws). This host throws tagged
 * classes instead, so it must recognise its own too, or a C++ exception passing through a trampoline
 * would be re-raised where the glue would have swallowed it and recorded the throw.
 *
 * `stackRestore` runs for *every* error, including the ones that are re-raised. That is not tidiness
 * in the original: a host that restored only on the swallow path would leave the stack pointer moved
 * on precisely the frames that must survive to unwind further.
 *
 * What the host adds is only the wiring, because the pieces are exports and do not exist until the
 * module is instantiated. {@link ArtemisAsyncHost} carries them and `attachExports` in `boot.ts`
 * fills them from the instance, exactly as it does for `$c` and `ad`.
 *
 * ## How the dynCall for each row was identified
 *
 * From the glue, not guessed, and the guess would have failed: **arity does not identify the
 * dynCall** (0 of these 30 imports have a same-arity export) and **wasm type barely does** (only 5
 * of the 30 have a *unique* same-type export; the other 25 have several candidates). The glue's
 * forwarder line is the only unambiguous statement of which export implements which `invoke_*`, and
 * it gives all 30 with the export's wasm type equal to the import's. Those pairs are the
 * {@link ARTEMIS_ASYNC_FORWARDS} table below, which the reconciler re-derives from the glue and
 * compares entry for entry.
 *
 * No argument is ever interpreted here. A wasm i32 arrives as a signed number, an i64 as a
 * `BigInt` and an f32 as a number, and all three are forwarded verbatim, so a row whose signature
 * carries an i64 (`wb`, `invoke_jiiii`) is exactly as correct as one that does not.
 */

import { ArtemisCppThrow, ArtemisLongjmp } from './eh.js';

/**
 * A wasm function reached through an export and called with its own argument types intact.
 *
 * `unknown` rather than `number` because the honest type of a forwarded call includes whatever the
 * engine produced, and a `BigInt` argument has to be passable without a cast at every call site.
 */
export type ArtemisForwardedFn = (...args: unknown[]) => number | void;

/** The import table, keyed by the wasm short name. */
export type ArtemisAsyncImports = Record<string, (...args: number[]) => number | void>;

/**
 * The pieces of the trampoline that live in the module and can therefore only be bound afterwards.
 *
 * Every field is optional and every one of them is filled from an *export* by `attachExports`, for
 * the same reason `eh.ts` cannot wire `$c` and `ad` at construction time: there is no instance yet.
 */
export interface ArtemisAsyncHost {
  /** The guest's `stackSave` — the module's short export `Xc`. */
  stackSave?: () => number;
  /** The guest's `stackRestore` — the module's short export `Yc`. */
  stackRestore?: (value: number) => void;
  /** The guest's `_setThrew` — the module's short export `_c`. */
  setThrew?: (env: number, value: number) => void;
  /** The dynCalls, keyed by the module's short export name, e.g. `cd` for `dynCall_iii`. */
  dynCall?: Readonly<Record<string, ArtemisForwardedFn>>;
  /** Reports a call this host could not serve, so a session can log it instead of swallowing it. */
  readonly onBlocked?: (what: string, detail: string) => void;
}

/**
 * Decides whether an error is one of the two unwinds the glue treats as "not a real exception".
 *
 * The glue's own condition is `e !== e + 0 && e !== "longjmp"`, which re-raises anything that is
 * neither a number nor the literal string. This host accepts its tagged classes for the same two
 * cases: {@link ArtemisCppThrow} stands in for the bare pointer the glue throws, and
 * {@link ArtemisLongjmp} for `throw "longjmp"`. Everything else — a `WebAssembly.RuntimeError`, an
 * `ArtemisExit` from `abort`, a host defect — is re-raised, which is what the glue does with it too.
 */
const isUnwind = (error: unknown): boolean => {
  if (typeof error === 'number') return true;
  if (error === 'longjmp') return true;
  return error instanceof ArtemisCppThrow || error instanceof ArtemisLongjmp;
};

/** Builds the `async` import table for one instance. */
export function createArtemisAsyncImports(host: ArtemisAsyncHost): ArtemisAsyncImports {
  const report = (what: string, detail: string): void => host.onBlocked?.(what, detail);

  /**
   * One trampoline, identical for all 30 rows apart from which dynCall it reaches.
   *
   * The void/value distinction in the glue's `return` is not reproduced here because the module's
   * own signature already carries it: a `-> void` export hands back `undefined` and a `-> f64`
   * export hands back the double, and the import's declared result type is fixed by the module
   * either way. `stackSave` is read once before the `try`, as the glue does, because it is the
   * value the `catch` has to restore.
   */
  const trampoline =
    (name: string, exportName: string) =>
    (...args: unknown[]): number | void => {
      const dynCall = host.dynCall?.[exportName];
      if (!dynCall) {
        report(
          name,
          `the module export ${exportName} is not wired, so ${ARTEMIS_ASYNC_OFFICIAL[name]} did not dispatch`,
        );
        return 0;
      }
      const sp = host.stackSave ? host.stackSave() : undefined;
      try {
        return dynCall(...args);
      } catch (error) {
        // Restored for every error, re-raised or not: a stack pointer left moved on a frame that
        // is about to unwind further is a corrupted frame.
        if (sp !== undefined) host.stackRestore?.(sp);
        if (!isUnwind(error)) throw error;
        host.setThrew?.(1, 0);
      }
    };

  return {
    /** `invoke_diii (i32,i32,i32,i32) -> f64` through the export `Nd`. */
    Ba: trampoline('Ba', 'Nd'),
    /** `invoke_fiii (i32,i32,i32,i32) -> f32` through the export `Md`. */
    Ca: trampoline('Ca', 'Md'),
    /** `invoke_iiiiiii (i32×7) -> i32` through the export `md`. */
    D: trampoline('D', 'md'),
    /** `invoke_viiiiiiiiii (i32×11) -> void` through the export `Od`. */
    P: trampoline('P', 'Od'),
    /** `invoke_iiiiiiiii (i32×9) -> i32` through the export `qd`. */
    Ra: trampoline('Ra', 'qd'),
    /** `invoke_iiiiiiii (i32×8) -> i32` through the export `od`. */
    aa: trampoline('aa', 'od'),
    /** `invoke_viiiiiiiiiiiiiii (i32×16) -> void` through the export `Pd`. */
    ba: trampoline('ba', 'Pd'),
    /** `invoke_iiiiiiiiiiii (i32×12) -> i32` through the export `yd`. */
    ca: trampoline('ca', 'yd'),
    /** `invoke_vi (i32,i32) -> void` through the export `bd`. */
    d: trampoline('d', 'bd'),
    /** `invoke_viiii (i32×5) -> void` through the export `id`. */
    e: trampoline('e', 'id'),
    /** `invoke_viii (i32×4) -> void` through the export `ld`. */
    f: trampoline('f', 'ld'),
    /** `invoke_ii (i32,i32) -> i32` through the export `gd`. */
    h: trampoline('h', 'gd'),
    /** `invoke_vii (i32×3) -> void` through the export `dd`. */
    j: trampoline('j', 'dd'),
    /** `invoke_iii (i32,i32,i32) -> i32` through the export `cd`. */
    k: trampoline('k', 'cd'),
    /** `invoke_viiiiii (i32×7) -> void` through the export `nd`. */
    l: trampoline('l', 'nd'),
    /** `invoke_iiii (i32×4) -> i32` through the export `ed`. */
    m: trampoline('m', 'ed'),
    /** `invoke_viif (i32,i32,i32,f32) -> void` through the export `sd`. */
    ma: trampoline('ma', 'sd'),
    /** `invoke_iiiiii (i32×6) -> i32` through the export `fd`. */
    n: trampoline('n', 'fd'),
    /** `invoke_iiiii (i32×5) -> i32` through the export `hd`. */
    o: trampoline('o', 'hd'),
    /** `invoke_v (i32) -> void` through the export `rd`. */
    p: trampoline('p', 'rd'),
    /** `invoke_vffffffiiii (i32,f32×6,i32×4) -> void` through the export `Gd`. */
    rc: trampoline('rc', 'Gd'),
    /** `invoke_viiiii (i32×6) -> void` through the export `jd`. */
    s: trampoline('s', 'jd'),
    /** `invoke_dii (i32,i32,i32) -> f64` through the export `Fd`. */
    sc: trampoline('sc', 'Fd'),
    /** `invoke_fi (i32,i32) -> f32` through the export `td`. */
    t: trampoline('t', 'td'),
    /** `invoke_fiiiii (i32×6) -> f32` through the export `Ed`. */
    tc: trampoline('tc', 'Ed'),
    /** `invoke_viiiiiiiii (i32×10) -> void` through the export `Cd`. */
    uc: trampoline('uc', 'Cd'),
    /** `invoke_vif (i32,i32,f32) -> void` through the export `ud`. */
    v: trampoline('v', 'ud'),
    /** `invoke_jiiii (i32×5) -> i32` through the export `Kd`. */
    wb: trampoline('wb', 'Kd'),
    /** `invoke_i (i32) -> i32` through the export `xd`. */
    x: trampoline('x', 'xd'),
    /** `invoke_viiiiiii (i32×8) -> void` through the export `pd`. */
    y: trampoline('y', 'pd'),
  };
}

/** The 30 short names of the `async` bucket, in the order the import section lists them. */
export const ARTEMIS_ASYNC_IMPORTS: readonly string[] = [
  'Ba',
  'Ca',
  'D',
  'P',
  'Ra',
  'aa',
  'ba',
  'ca',
  'd',
  'e',
  'f',
  'h',
  'j',
  'k',
  'l',
  'm',
  'ma',
  'n',
  'o',
  'p',
  'rc',
  's',
  'sc',
  't',
  'tc',
  'uc',
  'v',
  'wb',
  'x',
  'y',
];

/** The official name behind each short name, for reports that have to speak the guest's language. */
export const ARTEMIS_ASYNC_OFFICIAL: Readonly<Record<string, string>> = {
  Ba: 'invoke_diii',
  Ca: 'invoke_fiii',
  D: 'invoke_iiiiiii',
  P: 'invoke_viiiiiiiiii',
  Ra: 'invoke_iiiiiiiii',
  aa: 'invoke_iiiiiiii',
  ba: 'invoke_viiiiiiiiiiiiiii',
  ca: 'invoke_iiiiiiiiiiii',
  d: 'invoke_vi',
  e: 'invoke_viiii',
  f: 'invoke_viii',
  h: 'invoke_ii',
  j: 'invoke_vii',
  k: 'invoke_iii',
  l: 'invoke_viiiiii',
  m: 'invoke_iiii',
  ma: 'invoke_viif',
  n: 'invoke_iiiiii',
  o: 'invoke_iiiii',
  p: 'invoke_v',
  rc: 'invoke_vffffffiiii',
  s: 'invoke_viiiii',
  sc: 'invoke_dii',
  t: 'invoke_fi',
  tc: 'invoke_fiiiii',
  uc: 'invoke_viiiiiiiii',
  v: 'invoke_vif',
  wb: 'invoke_jiiii',
  x: 'invoke_i',
  y: 'invoke_viiiiiii',
};

/**
 * Which module export implements each row's dynCall.
 *
 * Read from the glue's `var <fn>=Module["<fn>"]=Module["asm"]["<export>"]` forwarders, not inferred:
 * arity picks 0 of these 30 and wasm type picks only 5 uniquely, so any derivation that does not
 * read the glue will eventually bind a plausible neighbour. The export's wasm type is equal to the
 * import's for all 30, which is the second half of the check.
 */
export const ARTEMIS_ASYNC_FORWARDS: Readonly<Record<string, string>> = {
  Ba: 'Nd',
  Ca: 'Md',
  D: 'md',
  P: 'Od',
  Ra: 'qd',
  aa: 'od',
  ba: 'Pd',
  ca: 'yd',
  d: 'bd',
  e: 'id',
  f: 'ld',
  h: 'gd',
  j: 'dd',
  k: 'cd',
  l: 'nd',
  m: 'ed',
  ma: 'sd',
  n: 'fd',
  o: 'hd',
  p: 'rd',
  rc: 'Gd',
  s: 'jd',
  sc: 'Fd',
  t: 'td',
  tc: 'Ed',
  uc: 'Cd',
  v: 'ud',
  wb: 'Kd',
  x: 'xd',
  y: 'pd',
};

/** The three helpers a trampoline needs, as the module's short export names. */
export const ARTEMIS_ASYNC_HELPERS = {
  stackSave: 'Xc',
  stackRestore: 'Yc',
  setThrew: '_c',
} as const;
