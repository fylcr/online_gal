/**
 * Asyncify driver: the JavaScript half of the unwind/rewind machinery.
 *
 * An asyncify guest cannot block. Reaching a call that needs an outside value, it *unwinds*:
 * `startUnwind` spills its stack into a control block and the exported function returns early with a
 * placeholder. JavaScript waits, then replays **the same raw export with the same args and the same
 * receiver** between `startRewind` and `stopRewind`, and the guest resumes where it left off. That
 * replay is the only way the value reaches the frame that asked for it.
 *
 * An unwind is not an exception: after `startUnwind` the import returns 0 normally and the guest
 * returns out of its root of its own accord. The state global is the signal, checked at the root
 * boundary. `busy` is deliberately separate from the state: between the unwind and the rewind the
 * guest is `Normal` (the unwind was stopped) yet a Promise is outstanding and a replay is coming.
 *
 * Single-flight: the control block and the state global are module-wide in the guest, so a second
 * root while one runs is refused, and so is an outside caller entering a wrapped export while the
 * guest is unwound. Genuine *nested* calls are told apart by guest depth: while a root runs,
 * `inGuestDepth > 0` means the guest itself re-entered, which is ordinary re-entrancy.
 */

import { ArtemisHostError } from './errno.js';

/** Guest asyncify states, matching the state global the transformed module keeps. */
export const ASYNCIFY_STATE = { Normal: 0, Unwinding: 1, Rewinding: 2 } as const;

/** Bytes of control-block header: stack start, stack end (exclusive), resume id. */
export const ASYNCIFY_DATA_HEADER_BYTES = 12;

/** One `sleep`/`await` the guest asked for, run once the guest has unwound. `undefined` counts as 0. */
export type ArtemisSuspendThunk = () => number | void | Promise<number>;

/** The four guest control functions driving the mechanism. Any of them may throw. */
export interface ArtemisAsyncifyControl {
  readonly startUnwind: (data: number) => void;
  readonly stopUnwind: () => void;
  readonly startRewind: (data: number) => void;
  readonly stopRewind: () => void;
}

/** The linear memory a control block lives in. `heap32` is re-read every time — the heap moves. */
export interface ArtemisAsyncifyMemory {
  readonly malloc: (bytes: number) => number;
  readonly free: (pointer: number) => void;
  readonly heap32: () => Int32Array;
}

/** Everything the driver needs. */
export interface ArtemisSuspendOptions {
  readonly control: ArtemisAsyncifyControl;
  readonly memory: ArtemisAsyncifyMemory;
  /** Bytes each control block reserves. Bounded on purpose; the guest corrupts silently if wrong. */
  readonly stackSize?: number;
}

/** A wrapped export: same signature, but it may now take as long as its thunks do. */
export type ArtemisSuspendingFunction = (...args: number[]) => number | void;

/** Bookkeeping for the one in-flight root call. */
interface Root {
  readonly fn: (...args: number[]) => number | void;
  readonly args: number[];
  readonly thisArg: unknown;
  /** Control block, or `null` when none is held. Cleared on the path that frees it. */
  pointer: number | null;
  /** Depth of nested wrapped calls; only 0 owns the unwind and the block. */
  depth: number;
  /** What the parked `sleep` asked for, so its replay answers without calling the thunk again. */
  value: number;
}

/** One `waitUntilIdle` caller. Every waiter is answered together — none can be left hanging. */
interface Waiter {
  resolve: () => void;
  reject: (error: unknown) => void;
}

const STACK_SIZE_MIN = 16;
const STACK_SIZE_MAX = 1 << 24;

/**
 * Drives an asyncify guest so its exports may take arbitrarily long. Build the guest's `sleep`
 * import from `driver.sleep`, wrap the raw exports you want to call from JavaScript, await
 * `waitUntilIdle()`.
 */
export class ArtemisSuspendDriver {
  readonly #control: ArtemisAsyncifyControl;
  readonly #memory: ArtemisAsyncifyMemory;
  readonly #stackSize: number;

  /** Explicit asyncify state. Never inferred from `busy`. */
  #state: number = ASYNCIFY_STATE.Normal;
  /** A root call is in flight: running, unwound, or being replayed. */
  #busy = false;
  /** A root is parked on a promise. `#busy` alone does not say so: the second await of a root is
   *  parked while `#current` is already set, so this flag is what tells the two apart. */
  #parked = false;
  /** The settlement the parked frame waits on, or `null` when nothing is parked. */
  #pending: Promise<number> | null = null;
  /** The one in-flight root; stays set across an unwind so its replay finds the raw call. */
  #current: Root | null = null;
  /** Bumped whenever a root and its block are released, and on every failure. */
  #generation = 0;
  /** A failure no waiter has consumed yet; it rejects the next and every later one. */
  #failure: unknown = null;
  #hasFailure = false;
  #disposed = false;
  #waiters: Waiter[] = [];
  /** `null` until a root completes; then its numeric return, or `null` for a void return. */
  result: number | null = null;

  constructor(options: ArtemisSuspendOptions) {
    this.#control = options.control;
    this.#memory = options.memory;
    const stackSize = options.stackSize ?? 4096;
    if (!Number.isInteger(stackSize) || stackSize < STACK_SIZE_MIN || stackSize > STACK_SIZE_MAX) {
      throw new ArtemisHostError(
        `stackSize must be an integer in [${STACK_SIZE_MIN}, ${STACK_SIZE_MAX}], got ${String(stackSize)}`,
      );
    }
    this.#stackSize = stackSize;
  }

  /** Whether a root call is in flight. Also true between the unwind and the replay. */
  get busy(): boolean {
    return this.#busy;
  }

  /** The asyncify state: `Normal`, `Unwinding`, or `Rewinding`. */
  get state(): number {
    return this.#state;
  }

  /** Wrapped calls between the current frame and the root. Zero outside a root call. */
  get inGuestDepth(): number {
    return this.#current === null ? 0 : this.#current.depth;
  }

  get disposed(): boolean {
    return this.#disposed;
  }

  /**
   * The guest's `sleep` import. A synchronous thunk is answered immediately — nothing to wait for,
   * so the guest never unwinds. A promise-valued thunk parks the call: `startUnwind` runs from
   * *inside* this import, which is what makes the transformed guest spill its stack on its way out.
   */
  sleep = (thunk: ArtemisSuspendThunk): number => {
    if (this.#disposed) throw new ArtemisHostError('sleep on a disposed driver');
    if (typeof thunk !== 'function') throw new ArtemisHostError('sleep expects a thunk function');

    // A replay: this frame already asked for the value and it is known. Answer from the recorded
    // slot, stop the rewind, drop this pass's block. Nothing else — the root is mid-execution and
    // may reach a second sleep before it returns.
    if (this.#state === ASYNCIFY_STATE.Rewinding) {
      const root = this.#current;
      if (root === null) throw new ArtemisHostError('sleep: rewinding with no parked call');
      // The stop comes first: it either succeeds and the state may advance, or it throws and the
      // whole call is failed with the guest still told the truth about where it stands.
      let stopOk = false;
      let stopError: unknown = null;
      try {
        this.#control.stopRewind();
        stopOk = true;
      } catch (error) {
        stopError = error;
      }
      if (!stopOk) {
        this.#fail(stopError);
        throw stopError;
      }
      this.#state = ASYNCIFY_STATE.Normal;
      this.#free(root);
      return normalise(root.value, (error) => this.#fail(error));
    }

    const root = this.#current;
    if (root === null) {
      // No root call, so no unwind could carry a value back to a frame that asked for one.
      // Answering synchronously is still correct for a sync thunk; awaiting outside a call is a
      // mistake, and pretending it worked would hide it.
      return this.#sync(this.#call(thunk));
    }

    const generation = this.#generation;
    let value: number | void | Promise<number> = this.#call(thunk);
    if (!isThenable(value)) {
      value = normalise(value, (error) => this.#fail(error));
      return value;
    }

    // Subscribe to the *thenable as given* before anything can throw: an already-rejected promise
    // must never be orphaned by a failing `#reserve` (an unhandled rejection nobody can observe).
    let promise: Promise<number>;
    try {
      promise = Promise.resolve(value as PromiseLike<number>);
    } catch (error) {
      this.#fail(error);
      throw error;
    }
    const settled = promise.then(
      (resolved) => {
        if (generation !== this.#generation) return 0;
        return normalise(resolved, (error) => this.#fail(error));
      },
      (error: unknown) => {
        // Handled here, so a rejected thunk never surfaces as an unhandled rejection.
        if (generation === this.#generation) this.#fail(error);
        throw error;
      },
    );
    // Terminal catch attached *before* `#reserve`: every rejection of `settled` (the rethrow above,
    // and the rewind subscription's rethrow below) is observed from here on.
    settled.catch(() => undefined);
    try {
      // Reserve only when this frame does not already own a block: the block is freed by the
      // consumed rewind, so the next await allocates a new one.
      if (root.pointer === null) this.#reserve(root);
    } catch (error) {
      this.#fail(error);
      throw error;
    }

    this.#pending = settled;
    this.#parked = true;

    // Subscribed before the unwind: a thunk that settles immediately still arrives even if the
    // guest's own `startUnwind` throws. The rewind runs from a microtask so the guest finishes
    // spilling first; a throw there is recorded through `#fail`, never rethrown into an async
    // context — a rejected `startRewind` must not leave the root hanging with no rewind coming.
    settled.then(
      (resolved) => {
        if (generation !== this.#generation) return;
        queueMicrotask(() => {
          if (generation !== this.#generation) return;
          const parked = this.#current;
          if (this.#state === ASYNCIFY_STATE.Rewinding || parked === null) return;
          if (parked.pointer === null) return;
          try {
            this.#rewind(parked, resolved);
          } catch (error) {
            this.#fail(error);
          }
        });
      },
      () => undefined,
    );
    let pointer = 0;
    try {
      if (root.pointer === null) {
        const error = new ArtemisHostError('sleep: no control block for the unwind');
        this.#fail(error);
        throw error;
      }
      pointer = root.pointer;
      this.#state = ASYNCIFY_STATE.Unwinding;
      this.#control.startUnwind(pointer);
    } catch (error) {
      this.#fail(error);
      throw error;
    }
    return 0;
  };

  /** Run the guest's thunk, recording a throw as the driver failure before it reaches the guest. */
  #call(thunk: ArtemisSuspendThunk): number | void | Promise<number> {
    try {
      return thunk();
    } catch (error) {
      this.#fail(error);
      throw error;
    }
  }

  /**
   * Wrap a raw guest export so it may suspend. `name` is for diagnostics only. Nesting is decided
   * by guest depth: a re-entrant call from an already-running guest is nested and manages nothing.
   */
  wrap<T extends (...args: number[]) => number | void>(
    name: string,
    rawFunction: T,
  ): ArtemisSuspendingFunction {
    const driver = this;
    return function (this: unknown, ...args: number[]): number | void {
      if (driver.#disposed) throw new ArtemisHostError(`${name}: driver disposed`);

      const root = driver.#current;
      if (root === null) {
        if (driver.#busy) {
          if (driver.#parked) {
            // A root is parked and an outside caller wants to re-enter the guest: replaying a
            // half-spilled stack is corruption, not re-entrancy. Nothing is torn down — the root
            // is still parked and its rewind is still coming.
            throw new ArtemisHostError(`${name}: guest is unwound; a root call is already in flight`);
          }
          // A root is running and a *different* call is being driven while it is on the stack.
          // There is one control block and one state global: this cannot be honoured. `#fail` is
          // deliberately not called: this is a refusal, not a failure of the root in flight.
          throw new ArtemisHostError(`${name}: a root call is already running`);
        }
        if (driver.#state !== ASYNCIFY_STATE.Normal) {
          throw new ArtemisHostError(`${name}: asyncify state ${driver.#state} with no root call`);
        }
        if (driver.#hasFailure) {
          // A failed call must not be papered over by a fresh one that appears to succeed.
          throw new ArtemisHostError(`${name}: driver failed and cannot start a new root call`);
        }
        const fresh: Root = {
          fn: rawFunction as (...args: number[]) => number | void,
          args,
          thisArg: this,
          pointer: null,
          depth: 1,
          value: 0,
        };
        driver.#busy = true;
        try {
          return driver.#enter(name, fresh);
        } catch (error) {
          // A throw crossing the boundary means the root is gone even if no `sleep` saw it.
          if (!driver.#hasFailure) driver.#fail(error);
          throw error;
        }
      }

      if (root.depth === 0) throw new ArtemisHostError(`${name}: a root call is already in flight`);
      root.depth += 1;
      try {
        return rawFunction.apply(this, args) as number | void;
      } finally {
        root.depth -= 1;
      }
    };
  }

  /** Wait until the current root call (and any it unwinds into) has finished. */
  waitUntilIdle(): Promise<void> {
    if (this.#hasFailure) {
      // The failure is retained, not consumed: every later waiter is told as well.
      const failure = this.#failure;
      return new Promise<void>((_resolve, reject) => reject(failure));
    }
    if (this.#disposed) return Promise.reject(new ArtemisHostError('driver disposed'));
    // Parked counts as busy: the root frame is waiting on a promise even between the unwind and
    // the replay, and its raw call is mid-flight.
    if (!this.#busy && !this.#parked) return Promise.resolve();
    return new Promise<void>((resolve, reject) => {
      this.#waiters.push({ resolve, reject });
    });
  }

  /**
   * Abandon any in-flight call, rejecting every waiter. Further calls are refused. This is the last
   * word on the driver's outcome, so it never stops the unwind/rewind a second time — `#state` is
   * module-wide and a second stop would move the guest to a state it is no longer in.
   */
  dispose(reason = 'driver disposed'): void {
    if (this.#disposed) return;
    this.#disposed = true;
    // A primary failure may already be recorded (the caller is disposing a failed run). It stays the
    // diagnosis: dispose's own reason is never allowed to overwrite it.
    this.#fail(this.#hasFailure ? this.#failure : new ArtemisHostError(reason));
  }

  /**
   * Runs the outermost frame, first pass or replay.
   *
   * Neither pass throws an unwind: the guest's transformed code returns early by itself, so the
   * only signal that the call parked is the state global being `Unwinding` when the raw call comes
   * back. That is read here, at the boundary, and the unwind is stopped there.
   */
  #enter(name: string, root: Root): number | void {
    const generation = this.#generation;
    this.#current = root;
    if (root.depth <= 0) root.depth = 1;
    let value: number | void;
    try {
      value = root.fn.apply(root.thisArg, root.args);
    } catch (error) {
      this.#fail(error);
      throw error;
    }
    root.depth -= 1;

    if (this.#state === ASYNCIFY_STATE.Unwinding) {
      // The guest spilled its stack and returned early. The unwind stops here, where all its own
      // frames have been left, and the driver returns to `Normal` — still *busy*, because a
      // Promise is outstanding and a replay is coming.
      if (root.pointer === null) {
        const error = new ArtemisHostError(`${name}: guest unwound without a pending sleep`);
        this.#fail(error);
        throw error;
      }
      if (generation === this.#generation) {
        // The stop is attempted first and the state only advances once it has succeeded.
        try {
          this.#control.stopUnwind();
        } catch (error) {
          this.#fail(error);
          throw error;
        }
        this.#state = ASYNCIFY_STATE.Normal;
      }
      return root.value;
    }

    this.#finishRoot(value, generation);
    return value;
  }

  /** One rewind pass: replay the root raw call and let the guest run on to its real return. */
  #rewind(root: Root, resolved: number): void {
    const pointer = root.pointer;
    if (pointer === null) return;
    const generation = this.#generation;
    this.#pending = null;
    this.#parked = false;
    // Rewinding must not begin before the unwind is over, and the state must say `Rewinding` by the
    // time the replay's guest frames look at it — so both control calls come first.
    if (this.#state === ASYNCIFY_STATE.Unwinding) this.#control.stopUnwind();
    // The state is set *before* `startRewind`: should that throw, the teardown in `#fail` still
    // knows a rewind is in progress and stops it, instead of leaving the guest rewinding forever.
    this.#state = ASYNCIFY_STATE.Rewinding;
    this.#control.startRewind(pointer);
    root.value = resolved;
    this.#enter('rewind', root);
  }

  /** The root returned normally: release it and answer every waiter with its result. */
  #finishRoot(value: number | void, generation: number): void {
    if (generation !== this.#generation) return;
    // A root that came back while the state still says `Rewinding` never consumed the value it was
    // replayed with: the rewind error is real and must not be reported as success.
    if (this.#state === ASYNCIFY_STATE.Rewinding) {
      const error = new ArtemisHostError(
        'root returned while still rewinding; the replayed value was never consumed',
      );
      this.#fail(error);
      throw error;
    }
    this.#current = null;
    this.#busy = false;
    this.#parked = false;
    this.#pending = null;
    this.#state = ASYNCIFY_STATE.Normal;
    this.#generation += 1;
    this.result = typeof value === 'number' ? value : null;
    this.#deliver();
  }

  /** Reserve the control block in the layout the guest expects: [start, end, resume id]. */
  #reserve(root: Root): void {
    const total = ASYNCIFY_DATA_HEADER_BYTES + this.#stackSize;
    // Validated *before* any conversion: `>>> 0` would wrap a bad pointer into a plausible one.
    const raw: unknown = this.#memory.malloc(total);
    if (
      typeof raw !== 'number' ||
      !Number.isInteger(raw) ||
      raw <= 0 ||
      raw > 0xffffffff ||
      (raw & 3) !== 0
    ) {
      throw new ArtemisHostError(`bad asyncify control block pointer ${String(raw)} for ${total} bytes`);
    }
    const pointer = raw;
    const start = pointer + ASYNCIFY_DATA_HEADER_BYTES;
    const end = start + this.#stackSize;
    // Owned before the heap is touched: a throwing getter can no longer leak the allocation.
    root.pointer = pointer;
    const heap = this.#memory.heap32();
    // Every bound is checked against the *byte* extent the pointer is used with, and the heap is
    // bounded by uint32 so the guest's 32-bit address arithmetic cannot wrap past either end.
    if (
      !(heap instanceof Int32Array) ||
      heap.byteOffset !== 0 ||
      !Number.isInteger(heap.length) ||
      heap.length < 0 ||
      heap.length > 0xffffffff / 4 ||
      end > 0xffffffff ||
      end > heap.length * 4
    ) {
      throw new ArtemisHostError(`control block at ${pointer} does not fit in the heap`);
    }
    const base = pointer >>> 2;
    heap[base] = start | 0;
    heap[base + 1] = end | 0;
    heap[base + 2] = 0;
  }

  /** Frees the one block this root could hold. Idempotent — a second call frees nothing. */
  #free(root: Root): void {
    const pointer = root.pointer;
    if (pointer === null) return;
    // Clear before freeing: a throwing `free` has still relinquished the block, so no later release
    // can free the same pointer twice, and `#fail`'s own cleanup cannot recurse back into `#fail`.
    root.pointer = null;
    this.#freeBlock(pointer);
  }

  /** Best-effort release inside teardown: a throwing `free` must not replace the primary failure. */
  #freeQuietly(pointer: number): void {
    try {
      this.#memory.free(pointer);
    } catch {
      // Secondary: swallowed on purpose.
    }
  }

  #freeBlock(pointer: number): void {
    this.#memory.free(pointer);
  }

  /** Tear down an in-flight call and record the failure. Waiters are answered by the caller. */
  #fail(error: unknown, delivered = false): void {
    const root = this.#current;
    // Saved before any teardown: `#reset` clears `#current`, and the block must still be released.
    const pointer = root === null ? null : root.pointer;
    if (pointer !== null && root !== null) root.pointer = null;
    if (this.#hasFailure) {
      // A primary failure is already recorded: keep it, but the teardown still runs — dispose needs
      // the reset, the block release, and the delivery even when the failure is not new.
      this.#reset();
      if (pointer !== null) this.#freeQuietly(pointer);
      if (!delivered) this.#deliver();
      return;
    }
    this.#failure = error;
    this.#hasFailure = true;
    this.#reset();
    // Released *after* the reset — the control functions must still see the root — and with a
    // non-throwing free, because a throwing one would re-enter `#fail` with the retained failure.
    if (pointer !== null) this.#freeQuietly(pointer);
    if (!delivered) this.#deliver();
  }

  /** Clear the in-flight bookkeeping and invalidate every outstanding thunk and replay. */
  #reset(): void {
    if (this.#current !== null) {
      try {
        if (this.#state === ASYNCIFY_STATE.Unwinding) this.#control.stopUnwind();
        else if (this.#state === ASYNCIFY_STATE.Rewinding) this.#control.stopRewind();
      } catch {
        // Best effort: teardown continues.
      }
    }
    this.#current = null;
    this.#busy = false;
    // Cleared too: a stale `#parked` would have `wrap` report an unwound guest that no longer exists.
    this.#parked = false;
    this.#pending = null;
    this.#state = ASYNCIFY_STATE.Normal;
    this.#generation += 1;
  }

  /** Answer every waiter. A recorded failure answers them with the failure instead. */
  #deliver(): void {
    const waiters = this.#waiters;
    this.#waiters = [];
    if (waiters.length === 0) return;
    if (this.#hasFailure) {
      const failure = this.#failure;
      for (const waiter of waiters) waiter.reject(failure);
      return;
    }
    for (const waiter of waiters) waiter.resolve();
  }

  /** A `sleep` outside a root call: sync values pass through, promises cannot be resumed. */
  #sync(value: number | void | Promise<number>): number {
    if (isThenable(value)) {
      Promise.resolve(value as PromiseLike<number>).catch(() => undefined);
      throw new ArtemisHostError('sleep: await outside a root call cannot be resumed');
    }
    return value === undefined ? 0 : normalise(value, () => undefined);
  }
}

/**
 * The one place where a `sleep` answer becomes a number: `undefined` is 0 (the thunk returned
 * nothing, which is a perfectly good "wait no time at all"), a finite number passes through, and
 * anything else is a driver failure first and an exception second — the guest's frame asked for a
 * number and must never be handed a wrapped one.
 */
function normalise(value: unknown, onError: (error: unknown) => void): number {
  if (value === undefined) return 0;
  if (typeof value === 'number' && Number.isFinite(value)) return value;
  const error = new ArtemisHostError(
    `sleep: thunk resolved to ${typeof value === 'number' ? String(value) : typeof value}, not a number`,
  );
  onError(error);
  throw error;
}

/** Proper thenable detection: any object or function exposing a `then` method. */
function isThenable(value: unknown): boolean {
  if (typeof value !== 'object' && typeof value !== 'function') return false;
  if (value === null) return false;
  return typeof (value as { then?: unknown }).then === 'function';
}

export default ArtemisSuspendDriver;
