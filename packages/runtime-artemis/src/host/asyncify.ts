import { ArtemisHostError } from './errno.js';
import type { ArtemisMemory } from './memory.js';

/**
 * The `asyncify` bucket of `ports/artemis/harness/glue/imports-map.json` — 4 rows, 4 short names.
 *
 * Three of them are the emscripten download family, and the fourth is the spill that keeps a large
 * copy from being a `memcpy` the optimiser will try to inline. Together they are the *surface* of
 * Asyncify without the machinery: the glue's bodies call `Asyncify.handleSleep`, which unwinds the
 * wasm stack out to JavaScript, runs a real `XMLHttpRequest`, and rewinds on completion. A host has
 * neither an event loop to unwind into nor a browser to ask, so the honest shape is a **resolver
 * callback**: the host hands the request back to whatever fetches URLs in this session, and the
 * callback writes the result into the guest's out-pointers exactly as the glue's own completion
 * handler does.
 *
 * That is a deliberate difference in mechanism, not in contract. The observable effect of
 * `_emscripten_wget_data(url, pbuffer, pnum, perror)` is the same either way: on success
 * `*pbuffer` is a guest pointer to `*pnum` bytes and `*perror` is 0; on failure `*perror` is 1. When
 * no resolver is supplied this host writes the failure form and records the refusal, rather than
 * inventing a file descriptor for bytes nobody fetched. A stub that returned 0 with a null buffer
 * would hand the guest a segfault three frames later.
 *
 * ## Suspension
 *
 * The glue reaches `handleSleep` from *inside* the import, so the guest's own stack is what unwinds.
 * A host reproduces that with the asyncify driver: {@link ArtemisAsyncifyHost.sleep} is the guest's
 * `sleep` import, and calling it with a thunk parks the guest until the thunk's promise settles.
 *
 * The whole fetch is placed **inside** the thunk — resolve, validate, allocate, copy, write the
 * out-pointers — and that is load-bearing, not tidiness. A rewind replays the raw export from the
 * top; if any of that work sat outside the thunk it would run a second time on the replay pass,
 * fetching the URL twice and leaking a buffer per rewind. Inside the thunk it is replayed *from the
 * recorded value* instead: the driver answers a replayed sleep from its saved slot and never calls
 * the thunk again, so the fetch, the allocation and the writes happen **exactly once** per call.
 *
 * Without a `sleep` seam the resolver is still run, but only its synchronous value can be used. A
 * resolver that returns a `Promise` then cannot be awaited — there is nothing to suspend into — so it
 * is refused as a host defect (see {@link toBytes}) and its rejection is consumed so it never
 * surfaces as an unhandled rejection.
 *
 * ## The two callback-shaped rows do not suspend
 *
 * `Qa` and `Jc` are the *non-blocking* half of the family, and that is the whole difference between
 * them and `fa`. The glue's `async_wget` rows open an XHR and return at once; the browser calls the
 * callback whenever the transfer lands, on a later turn of the event loop. The guest never unwinds
 * for them — there is no frame waiting on the answer, only a function-table entry to call later.
 *
 * So neither row takes the {@link ArtemisAsyncifyHost.sleep} seam, not even when a resolver answers
 * with a `Promise`. A promise is simply the host's own way of saying "the bytes are not here yet":
 * the import still returns immediately (`Jc` even hands back its request handle first), and the
 * completion — allocate, copy, callback, free — is deferred onto {@link ArtemisAsyncifyHost.enqueue}
 * rather than onto the asyncify driver. Wrapping these rows in `sleep` would have the driver unwind a
 * guest that returned of its own accord, and the "replay" would then re-run the raw root to deliver a
 * callback nobody was waiting for. That is the defect this split exists to prevent.
 *
 * `fa` is the blocking row and is the **only** one that calls `sleep`: `Asyncify.handleSleep` is
 * literally what `_emscripten_wget_data` is built on, and the guest's frame expects the bytes on its
 * rewind. One row suspends; the other two never do.
 */

/** One request's worth of decoded strings, so a resolver need not re-read the guest heap. */
export interface ArtemisWgetRequest {
  /** The URL, NUL-terminated in the guest heap. */
  readonly url: string;
  /** HTTP method — `GET` for the two plain callers, `POST` for `_emscripten_async_wget2_data`. */
  readonly method: string;
  /** POST body, for the `wget2` variant. Empty for a GET. */
  readonly body: string;
}

/**
 * A resolver answers a request with the bytes, `null` for a failed transfer, or a `Promise` of
 * either.
 *
 * `null` is a **guest-visible failure** — the transfer did not happen — and each caller reports it
 * in its own way: `_emscripten_wget_data` writes `*perror = 1`, the two callback-shaped rows invoke
 * their `onerror`. Anything that is neither a `Uint8Array`, `null`, nor a promise of one is a
 * **host defect** and is raised as {@link ArtemisHostError}, because it means the resolver contract
 * was broken rather than that a download failed. The two must not be confused: a defect reported as
 * a failed download reads to the guest as a perfectly ordinary network error.
 *
 * A promise is usable from either row kind, but it is delivered differently. `fa` suspends the guest
 * on it, so it needs {@link ArtemisAsyncifyHost.sleep}. `Qa` and `Jc` never suspend: they return at
 * once and let {@link ArtemisAsyncifyHost.enqueue} run the completion when the bytes arrive.
 */
export type ArtemisWgetResolver = (
  request: ArtemisWgetRequest,
) => Uint8Array | null | Promise<Uint8Array | null>;

/** What a session must supply for the three download rows to fetch anything. */
export interface ArtemisAsyncifyHost {
  /**
   * The guest heap.
   *
   * `refresh` is optional in the *type* only so a test can hand in a plain accessor pair; when it is
   * present it is called after every `malloc` that may have grown the heap, because
   * `memory.grow` detaches the old `ArrayBuffer` and every view taken before it becomes a
   * zero-length view that writes nowhere. A host that *has* a real `ArtemisMemory` must pass it, not
   * an accessor object that mimics it: reading `u8` fresh each time is not the same guarantee, since
   * `malloc` can grow the heap between the read and the write.
   */
  readonly memory: ArtemisMemory & { refresh?(): void };
  /** The guest's `malloc`, needed to hand back a buffer the guest owns and frees. */
  readonly malloc?: (bytes: number) => number;
  /** The guest's `free`, used to release the buffer after a callback. */
  readonly free?: (pointer: number) => void;
  /**
   * Invokes one of the guest's function-table entries — the host's stand-in for the glue's
   * `getDynCaller`. The two `_async_wget*_data` rows are callback-shaped: they take a table index
   * and call it when the transfer lands. Without one they record the request and return the
   * failure form, which is what a browser-less host can honestly do.
   *
   * Prefer {@link ArtemisAsyncifyHost.invokeCallback}, which carries the signature so the host can
   * check the arity. This field is kept for callers that already pass it, and is used when
   * `invokeCallback` is absent.
   */
  readonly callback?: (
    entry: number,
    arg: number,
    buffer: number,
    length: number,
    status: number,
    statusText: string,
  ) => void;
  /**
   * Invokes one guest function-table entry with the **exact** signature the glue uses.
   *
   * `signature` is the glue's own `getDynCaller` string, so a host that delegates to the guest's
   * `dynCall_*` export can pick the right one instead of guessing from the argument count — which
   * matters here because the same row calls two different arities. `args` are already in the
   * signature's own order. The return is `void | Promise<void>` so a callback that itself suspends
   * (an `onload` that blocks on another download) can be awaited.
   */
  readonly invokeCallback?: (
    signature: string,
    entry: number,
    args: number[],
  ) => void | Promise<void>;
  /** Fetches a URL. Absent means this host does no networking. */
  readonly resolve?: ArtemisWgetResolver;
  /**
   * The guest's `sleep` import, from the asyncify driver. Used by **`fa` only** — it is the one row
   * whose guest frame waits for the bytes and therefore unwinds. `Qa` and `Jc` are non-blocking and
   * never touch this seam, whatever their resolver answers.
   *
   * Typed as a `Pick` of the driver's `sleep` rather than the driver itself: the import table needs
   * exactly one method, and taking the whole driver would drag the unwind/rewind control surface
   * into a host object that has no business calling it.
   */
  readonly sleep?: (thunk: () => number | void | Promise<number>) => number;
  /**
   * Defers one completion task out of the current import call — the seam `Qa` and `Jc` use to be
   * non-blocking while still doing their work in a real turn of their own.
   *
   * The task is where **all** of a completion's work happens: the allocation, the copy, the callback
   * invocation and the release. Nothing of that is done in the import body, so the import returns
   * before any of it runs — which is exactly the difference between these rows and `fa`. A session
   * must therefore not free, reuse, or otherwise observe the guest mid-task; the task is the unit.
   *
   * The default is one microtask (`queueMicrotask`), which is enough to get out of the current raw
   * root. A session that drives the same guest through the asyncify driver should pass an enqueue
   * that waits for the driver to go idle first: inside a raw root the guest is on the stack, and
   * re-entering it from the callback would be a nested call at the one moment the driver is mid-root.
   * Waiting there is the whole reason this is a seam rather than a hard-coded microtask.
   *
   * The return value may be a `Promise`, and it is awaited — a task that itself waits (a session that
   * parks until the driver is idle) must not have its failures orphaned, and a rejected task is
   * reported through `onBlocked` rather than left to surface as an unhandled rejection.
   */
  readonly enqueue?: (task: () => void | Promise<void>) => void | Promise<void>;
  /**
   * Releases one teardown, deferred to the same seam as the completion.
   *
   * A late `fa` — one whose guest was disposed while it was parked — can leave bytes it allocated
   * with nobody left to want them, and releasing them from inside the unwound guest's own rewind is
   * neither safe nor correct. When this seam is absent the release happens inline instead, because
   * an allocation that cannot be returned is worse than a release at an awkward moment.
   */
  readonly disposeFree?: (pointer: number) => void;
  /** Reports a call this host could not serve, so a session can log it instead of swallowing it. */
  readonly onBlocked?: (what: string, detail: string) => void;
  /**
   * Whether this row's callback can still be delivered. Checked **immediately before** every
   * invocation, after any await: a callback outstanding across a suspension may be disposed while
   * it waits, and re-entering a torn-down guest is a crash rather than a no-op. Absent means
   * "always deliverable".
   */
  readonly isDisposed?: () => boolean;
}

/** The import table, keyed by the wasm short name. */
export type ArtemisAsyncifyImports = Record<string, (...args: number[]) => number | void>;

/** The glue's `Brotli`/size ceiling is not the issue here; this is a sanity bound on one transfer. */
const MAX_TRANSFER_BYTES = 1 << 30;

/** Builds the `asyncify` import table for one instance. */
export function createArtemisAsyncifyImports(host: ArtemisAsyncifyHost): ArtemisAsyncifyImports {
  const { memory } = host;
  /**
   * Reports one refusal.
   *
   * A report is a *diagnosis*, always secondary to whatever effect the caller was performing, so the
   * reporter it was handed is not trusted to be well-behaved: `report` is called from inside `catch`
   * blocks and from promise rejection handlers, and a logger that throws there would replace the
   * failure being described with an unrelated unhandled exception out of a handler that could not
   * throw in the first place. Its own failure has nowhere to go — reporting *that* would be the same
   * loop one level down — so it is dropped.
   */
  const report = (what: string, detail: string): void => {
    try {
      host.onBlocked?.(what, detail);
    } catch {
      // Deliberately swallowed: see above.
    }
  };
  const malloc = host.malloc;
  const free = host.free;
  const sleep = host.sleep;

  /**
   * Reads a NUL-terminated string out of the guest heap.
   *
   * The `| 0` is not decoration: a wasm i32 argument arrives in JavaScript as a *signed* number, so
   * a heap address above 2^31 comes through negative and has to be squeezed back to the unsigned
   * value the memory accessor expects.
   */
  const utf8 = (pointer: number): string => {
    // Read fresh, after the refresh: a string that arrives after a grow would otherwise be decoded
    // through a view over the detached buffer, which reads as an empty string rather than throwing.
    refreshMemory();
    let end = (pointer | 0) >>> 0;
    const base = memory.u8;
    while (end < base.length && base[end] !== 0) end += 1;
    const decoder = new TextDecoder();
    return decoder.decode(base.subarray((pointer | 0) >>> 0, end));
  };

  /**
   * Narrows whatever the resolver produced to bytes, `null`, or a refusal.
   *
   * `canAwait` says whether the *caller* has a way to wait for a promise. `fa` can, through `sleep`;
   * `Qa` and `Jc` can too, but never by suspending the guest — their wait is the deferred completion
   * task, which is a later turn rather than an unwind. Only a row with no way at all to wait would
   * have to refuse here, and no row is in that position, so a promise is refused only when the
   * caller says it cannot accommodate one — which answers `null` would be a lie about: the transfer
   * did not fail, the host did. Its rejection is observed here, at the moment the promise is seen, so
   * a caller that holds the returned promise without subscribing to it cannot turn a resolver's
   * failure into an unhandled rejection.
   */
  const toBytes = (value: unknown, request: ArtemisWgetRequest, what: string, canAwait: boolean): Uint8Array | null | Promise<Uint8Array | null> => {
    if (value === null) return null;
    if (value instanceof Uint8Array) return value;
    if (isThenable(value)) {
      // Subscribed *here*, synchronously, at the moment the promise is seen: an unobserved rejection
      // is what produces an unhandled-rejection report naming no row, and the only way not to have
      // that window is to attach the handler before returning. The refusal below throws a different
      // thing, so the two are not alternatives — the guard is attached either way.
      const observed = Promise.resolve(value as PromiseLike<Uint8Array | null>).catch(
        (error: unknown) => {
          report(what, `resolver rejected: ${String(error)}`);
          return null;
        },
      );
      if (canAwait) return observed;
      throw new ArtemisHostError(
        `artemis ${what} resolver answered ${request.method} ${request.url} with a Promise, but this ` +
          'row cannot wait for it: the guest cannot be suspended, so the transfer would be ' +
          'reported as complete before its bytes exist. Install the asyncify driver\'s sleep.',
      );
    }
    throw new ArtemisHostError(
      `artemis ${what} resolver answered ${request.method} ${request.url} with ${typeof value} ` +
        '(expected Uint8Array, null, or a Promise of either)',
    );
  };

  /**
   * Runs the resolver and narrows its answer, mapping a throw and a defect to distinct things.
   *
   * A resolver that throws is a host defect too — it is not the same as resolving `null`, which is
   * the resolver's way of saying the transfer failed.
   *
   * `canAwait` is passed by the caller rather than inferred from the presence of `sleep`, because the
   * two are not the same question: `fa` waits by suspending the guest, while `Qa` and `Jc` wait by
   * deferring their completion to a later turn — and only `fa` may take the `sleep` seam, so the
   * mechanism cannot be read off the seam's presence. Both rows do await a promise; neither blocks.
   */
  const callResolver = (
    request: ArtemisWgetRequest,
    what: string,
    canAwait: boolean,
    // Whether a rejection should travel on to the caller instead of being flattened to `null`. The
    // callback rows want `null`: a rejected fetch is their ordinary `onerror`, and the guest is told
    // through the callback. The blocking row wants the rejection *itself*, because its thunk promise
    // is what `sleep` fails the root with — flattening it there would report a transfer failure the
    // root never hears about, and let `waitUntilIdle` resolve as though the download had landed.
    rethrowRejection = false,
  ): Uint8Array | null | Promise<Uint8Array | null> => {
    if (!host.resolve) {
      // Reported here, and the caller writes its own failure form. The report is the diagnosis;
      // the out-pointer write is the guest-visible effect, and the two are deliberately separate.
      report(what, `no resolver is installed, so ${request.method} ${request.url} was not fetched`);
      return null;
    }
    let value: unknown;
    try {
      value = host.resolve(request);
    } catch (error) {
      throw new ArtemisHostError(`artemis ${what} resolver threw: ${String(error)}`);
    }
    // A promise is handed straight back when the caller has an actual way to wait for it: through
    // `sleep` for the blocking row, or through the deferred completion of a callback row. The
    // narrowing of a *settled* answer is the awaiting caller's job, so this is not a second contract
    // — it is the same one, applied one turn later. `toBytes` subscribes as it narrows, so the
    // promise this returns is already handled: awaiting it is a convenience, not an obligation.
    if (isThenable(value)) {
      // Subscribed *here*, synchronously, at the moment the promise is seen: an unobserved rejection
      // is what produces an unhandled-rejection report naming no row, and the only way not to have
      // that window is to attach the handler before returning. A rejection is mapped to `null` so a
      // waiting caller takes the ordinary failure path — the same shape as a resolver that resolves
      // `null` — while the report below keeps the reason. The blocking row asks for the rejection
      // itself instead, and gets it: the handler is still attached first, so it is never unobserved
      // either way, and rethrowing from inside a `.catch` that is already in place costs nothing.
      const observed = Promise.resolve(value as PromiseLike<Uint8Array | null>).catch(
        (error: unknown) => {
          report(what, `resolver rejected: ${String(error)}`);
          // Carried onward only when the caller can actually wait for it. A caller about to be
          // refused below has no way to observe a rejection, and the promise this handler produces
          // would be dropped on the floor by the very throw that refuses it — turning an honest
          // refusal into an unhandled rejection of a promise nobody holds.
          if (rethrowRejection && canAwait) throw error;
          return null;
        },
      );
      // The narrowing of the *settled* answer is the awaiting caller's job, so a caller that cannot
      // wait is refused here rather than handed a promise it has no way to park on: the refusal is a
      // defect in the resolver, not a transfer failure, so it throws instead of resolving `null`.
      if (!canAwait) {
        throw new ArtemisHostError(
          `artemis ${what} resolver answered ${request.method} ${request.url} with a Promise, but this ` +
            'row cannot wait for it: the guest cannot be suspended, so the transfer would be ' +
            'reported as complete before its bytes exist. Install the asyncify driver\'s sleep.',
        );
      }
      return observed;
    }
    if (value === null) return null;
    // The ordinary answer: the bytes are already here, so there is nothing to wait for and no
    // narrowing left to do. This is the arm the two callback rows take for every synchronous
    // resolver, and leaving it out is not "the caller validates" — it would refuse the only answer
    // a resolver is *supposed* to give.
    if (value instanceof Uint8Array) return value;
    throw new ArtemisHostError(
      `artemis ${what} resolver answered ${request.method} ${request.url} with ${typeof value} ` +
        '(expected Uint8Array, null, or a Promise of either)',
    );
  };

  /**
   * Re-derives the memory's views when it is a real `ArtemisMemory`.
   *
   * The `memory.u8` accessor is read for each use, which is enough for a heap object that rebuilds
   * its views on demand — but not enough for the real one, where the *allocator itself* is what
   * grows the heap: `malloc` runs the guest, the guest runs `memory.grow`, and the `ArrayBuffer`
   * behind every view is detached before this function ever reads it again. A detached view does not
   * throw on write; it drops the bytes. So when the memory can refresh, it is refreshed.
   */
  const refreshMemory = (): void => (memory as { refresh?: () => void }).refresh?.();

  /**
   * Whether a value can index the heap: a wasm i32 by value, or the same address in unsigned form.
   *
   * Both spellings are legitimate at this boundary and neither is the "right" one — the guest's
   * `dynCall` hands over signed numbers, while a host that reads an address out of the heap itself
   * gets it from a `Uint32Array` and sees it unsigned — so the whole 32-bit space is accepted and
   * `>>> 0` normalises it at the use site.
   */
  const isHeapAddress = (pointer: number): boolean =>
    Number.isInteger(pointer) && pointer >= -0x80000000 && pointer <= 0xffffffff;

  /**
   * The exclusive end of `[p, p + n)`, or `NaN` when the pair is not a usable range.
   *
   * The `>>> 0` this used to end with was itself the bug it was meant to guard: every wasm i32
   * pointer arrives as a *signed* number, so `0xfffffff0 + 64` is a perfectly ordinary pair whose
   * sum lands just past 2^32 — and wrapping it made a range that runs off the end of any heap
   * address space look like the small in-heap range `[ptr, ptr + 48)`. Ranges are therefore summed
   * in JavaScript's own integer space, where `4 GiB - 16 + 64` stays what it is, and every caller
   * compares the result against the heap length below it.
   */
  const endOf = (p: number, n: number): number => {
    if (!isHeapAddress(p) || p < 0) return NaN;
    if (!Number.isInteger(n) || n < 0) return NaN;
    const end = p + n;
    if (end > 0xffffffff) return NaN;
    return end;
  };

  /** Whether `[ptr, ptr + len)` lies inside the heap as it stands right now. */
  const inHeap = (ptr: number, len: number): boolean => {
    const end = endOf(ptr, len);
    if (!Number.isFinite(end)) return false;
    return end <= memory.u8.length;
  };

  /**
   * Copies bytes into a guest buffer the guest owns.
   *
   * The glue does `_malloc(n)` then `HEAPU8.set(bytes, buffer>>>0)`. Copying is not optional: the
   * caller's `Uint8Array` may be a view into the guest heap itself, and handing that back as if it
   * were a separate allocation would alias the source and the destination.
   *
   * The `>>> 0` on the returned pointer is the wasm i32 coercion, and it is kept deliberately: an
   * allocator that hands back a pointer above 2^31 gives it to JavaScript as a *negative* number,
   * and the guest expects the unsigned form back. What it must not do is launder a pointer that is
   * not one — `NaN>>>0` is `0` and `1e21>>>0` is a plausible-looking address — so the value is
   * checked as the integer it is, by {@link isAllocatorPointer}, before it is coerced. The same
   * applies to the alignment the `i32` out-pointers in {@link writeOutPointers} need.
   *
   * The zero this returns is the glue's own `_malloc(0)` answer, and every caller treats it as "the
   * buffer was not handed over": nothing is released, because a pointer only becomes this file's to
   * release once it has been accepted as one.
   *
   * Returns 0 when there is no allocator, the request exceeds {@link MAX_TRANSFER_BYTES}, or the
   * allocator answered something that is not a usable heap pointer — which is the same condition
   * that makes the glue's `_malloc` return 0 and its copy a no-op.
   *
   * The source is copied, if it aliases the heap, *before* the allocation rather than after. `HEAP8`
   * and every sibling view sit over the same `ArrayBuffer`, and `_malloc` grows that buffer by
   * replacing it — which detaches the old one and leaves a heap view at zero length. A copy taken
   * after the allocator would read nothing and hand the guest an uninitialised buffer whose length
   * it is told is correct, which is worse than a failure. One temporary before the allocator runs
   * removes the whole class, since the bytes are then an ordinary outside array the grow cannot
   * reach.
   */
  const toGuestBuffer = (bytes: Uint8Array): number => {
    if (!malloc || bytes.length > MAX_TRANSFER_BYTES) return 0;
    // Nothing may be allocated for a guest that has been torn down: the buffer would be handed to a
    // heap nobody owns any more, and there is no callback left to release it.
    if (host.isDisposed?.()) return 0;
    if (bytes.buffer !== undefined && bytes.buffer === memory.u8.buffer) {
      // The bytes are a view into this very heap — `crypto.getRandomValues(HEAP8.subarray(p, p+n))`
      // is the canonical shape. `set` reads the source as it writes the destination, so an
      // allocation that happens to precede the source would smear it forward. Copying first costs
      // one temporary and removes the entire class.
      bytes = new Uint8Array(bytes);
    }
    const raw: unknown = malloc(bytes.length);
    // A pointer only becomes an owned allocation once it is this file's to give back; until then
    // there is nothing to release, and a failure to allocate is simply a failure to allocate.
    if (!isAllocatorPointer(raw)) return 0;
    // Ownership starts here: from this line on, every path out either hands the buffer to the guest
    // or releases it exactly once. That covers the whole window — the refresh, the range check and
    // the copy — because the allocator has already run guest code by now: a `refresh` that throws
    // and a range `inHeap` cannot rule out are the same situation as a `set` that refuses the write,
    // and all three leave a real allocation nobody was told about.
    const buffer = raw >>> 0;
    try {
      refreshMemory();
      // Re-derived after the refresh, and only then is the range judged against it.
      const heap = memory.u8;
      if (buffer === 0 || !inHeap(buffer, bytes.length)) {
        release('malloc', buffer);
        return 0;
      }
      if (host.isDisposed?.()) {
        release('malloc', buffer);
        return 0;
      }
      heap.set(bytes, buffer);
    } catch (error) {
      // The release is attempted once and its own failure is reported *beside* the primary diagnosis
      // rather than instead of it; nothing hands this pointer anywhere again. Those two possible
      // diagnoses are a throw out of the copy and a throw out of the memory refresh, which is why
      // this says "copying/refreshing". A zero from the range check releases for the same reason a
      // throw does — the guest is never handed a buffer it was not told about.
      release('malloc', buffer);
      report(
        'malloc',
        `copying ${bytes.length} B into guest buffer ${buffer} threw: ${String(error)}`,
      );
      return 0;
    }
    return buffer;
  };

  /**
   * Whether an allocator's answer is a pointer this host can own and hand back.
   *
   * A wasm i32 crosses back into JavaScript as a *signed* number, so the whole unsigned range has to
   * be accepted: an allocator handing out a pointer above 2^31 gives it here as a negative number,
   * and `>>> 0` at the use site restores what the guest expects. What must not pass is a value that
   * is not a pointer at all — `NaN>>>0` is 0 and `1e21>>>0` is a plausible-looking address — so the
   * value is checked as the integer it is before it is coerced, and zero is rejected because zero is
   * what an allocator returns when it failed.
   */
  const isAllocatorPointer = (raw: unknown): raw is number =>
    typeof raw === 'number' &&
    Number.isInteger(raw) &&
    raw !== 0 &&
    raw >= -0x80000000 &&
    raw <= 0xffffffff;

  /**
   * Writes the three out-pointers of `_emscripten_wget_data`, all of them or none.
   *
   * `perror` is written **last**: it is the flag the caller waits for, and a guest that observes
   * `*perror = 0` before the other two have landed reads a half-written answer. Each address is
   * bounded and 4-byte aligned against the *current* heap — the `i32` index divides by 4, so an
   * unaligned address would write over its neighbours — and if any of the three is unusable nothing
   * is written at all rather than a partial success being reported.
   */
  const writeOutPointers = (
    pbuffer: number,
    pnum: number,
    perror: number,
    buffer: number,
    length: number,
  ): boolean => {
    for (const pointer of [pbuffer, pnum, perror]) {
      // Checked as the signed integer wasm hands over, in the *unsigned* space the heap is indexed
      // in: a slot at 0xFFFFFFF0 arrives as -16, and rejecting it because it is "negative" would
      // refuse a perfectly ordinary address. `pointer >>> 0` is then the guest's own index.
      if (!isHeapAddress(pointer) || (pointer >>> 0) % 4 !== 0) return false;
      if (!inHeap(pointer >>> 0, 4)) return false;
    }
    // Nothing is written into a heap this host no longer owns: a slot is only worth filling while a
    // guest is there to read it, and the refresh below would otherwise rebuild views over a
    // torn-down memory.
    if (host.isDisposed?.()) return false;
    refreshMemory();
    const heap = memory.i32;
    heap[(pbuffer | 0) >>> 2] = buffer | 0;
    heap[(pnum | 0) >>> 2] = length | 0;
    // Last, and only once the answer it describes is complete.
    heap[(perror | 0) >>> 2] = 0;
    return true;
  };

  /**
   * Writes the failure form — `*perror = 1` — into a slot that is checked first.
   *
   * The failure path needs the same validation as the success path and for a worse reason: it is the
   * path a broken guest reaches, so the address may be the broken part. An unchecked `>>> 0` would
   * turn `NaN` into `0` and write the guest's heap header; a bounded write either lands or is
   * reported.
   */
  const writeError = (perror: number): boolean => {
    // Normalised to the unsigned space before the bound and the alignment are judged: a slot at
    // 0xFFFFFFF0 arrives as -16 and is a perfectly ordinary address, while a bare `>>> 0` elsewhere
    // would turn `NaN` into the heap header.
    if (!isHeapAddress(perror) || (perror >>> 0) % 4 !== 0) return false;
    if (!inHeap(perror >>> 0, 4)) return false;
    // A failure written into a heap that has been torn down is not a report anybody can act on.
    if (host.isDisposed?.()) return false;
    refreshMemory();
    memory.i32[(perror | 0) >>> 2] = 1;
    return true;
  };

  /**
   * Delivers one callback invocation, in the signature the glue would have used.
   *
   * `invokeCallback` is preferred because it carries the signature; `callback` is the older,
   * signature-less seam and is called with the leading positional arguments it has always taken.
   * A throwing callback is reported rather than propagated: it is guest code failing, and it must
   * not look like a host defect in the table's own body.
   */
  const deliver = (signature: string, entry: number, args: number[]): void | Promise<void> => {
    const invoke = host.invokeCallback;
    if (invoke) return invoke(signature, entry, args);
    if (!host.callback) return undefined;
    switch (signature) {
      case 'vi':
        host.callback(entry, args[0] ?? 0, 0, 0, 0, '');
        return undefined;
      case 'viii':
        host.callback(entry, args[0] ?? 0, args[1] ?? 0, args[2] ?? 0, 0, '');
        return undefined;
      default:
        host.callback(
          entry,
          args[0] ?? 0,
          args[1] ?? 0,
          args[2] ?? 0,
          args[3] ?? 0,
          '',
        );
        return undefined;
    }
  };

  /**
   * Invokes a callback if this host can reach it and it is still wanted, reporting any failure.
   *
   * A row whose `entry` is 0 has no callback, which the glue treats as "nothing to report" — the
   * `if (onload)` guard in its own body. Waiting for a promise-valued delivery before releasing the
   * buffer is the caller's job; this only hands it over.
   */
  const invokeCallback = (
    what: string,
    signature: string,
    entry: number,
    args: number[],
  ): void | Promise<void> => {
    if (!entry) return undefined;
    if (!host.invokeCallback && !host.callback) return undefined;
    if (host.isDisposed?.()) return undefined;
    let outcome: void | Promise<void>;
    try {
      outcome = deliver(signature, entry | 0, args);
    } catch (error) {
      report(what, `callback ${entry} threw: ${String(error)}`);
      return undefined;
    }
    if (!isThenable(outcome)) return undefined;
    // A callback that itself suspends: the failure has to be reported, and an unhandled rejection
    // from guest code is not a diagnosis anybody can act on. The rejection is consumed here, by
    // returning an already-handled promise, so a caller that awaits the delivery cannot be left
    // holding a rejected value of its own.
    const handled = Promise.resolve(outcome as PromiseLike<void>).then(
      () => undefined,
      (error: unknown) => {
        report(what, `callback ${entry} rejected: ${String(error)}`);
      },
    );
    return handled;
  };

  /** One attempt at something with an effect, so a failure can be retried without losing the reason. */
  type Attempt = () => void | Promise<void>;

  /** Makes one best-effort call and returns the first error it produced, or `null`. */
  const attemptOnce = (work: Attempt): unknown => {
    try {
      const outcome = work();
      if (isThenable(outcome)) {
        // A release seam that answers with a promise is outside this file's reach; the call itself
        // was accepted, so it counts as the one attempt and its own rejection is at least observed.
        void Promise.resolve(outcome as PromiseLike<void>).catch((error: unknown) => {
          report('free', `release seam rejected: ${String(error)}`);
        });
      }
      return null;
    } catch (error) {
      return error;
    }
  };

  /**
   * Releases a guest buffer, **once**.
   *
   * There is deliberately no retry. The old retry argued that a lost release is worse than a
   * repeated one, and that is not true of an allocator: `free` is not idempotent, so a second call
   * on a pointer the first one already returned is a double free — a heap corruption whose symptom
   * appears somewhere else entirely, long after the row that caused it. A leak is a bounded cost in
   * a guest that is about to be torn down; a double free is an unbounded one in a guest that is
   * still running. An allocator that throws has already had its chance, and the refusal is recorded
   * rather than re-attempted.
   */
  const release = (what: string, pointer: number): void => {
    const error = attemptOnce(() => free?.(pointer));
    if (error !== null) report(what, `free(${pointer}) threw: ${String(error)}`);
  };

  /**
   * Releases a buffer a guest's late departure left behind, through the same deferred seam as every
   * other effect.
   *
   * Called only when the guest that owned the buffer is gone, so the release is not ordered against
   * anything and there is no rewind to avoid: it is pushed out of the current turn anyway, because
   * the caller may be running inside the unwound guest's own replay and an allocator is not
   * guaranteed to be re-entrant there. With no seam the release happens inline, since leaking the
   * allocation outright is strictly worse.
   *
   * A seam that refuses — throws, or rejects — is the end of it. There is exactly one release per
   * allocation: falling back to an inline `free` after a seam *accepted* the call but failed later
   * would be a second release of a pointer the first one may well have already returned, and a
   * double free corrupts the heap this whole file exists to protect. The failure is reported and the
   * pointer is abandoned.
   */
  const releaseDeferred = (what: string, pointer: number): void => {
    if (!host.disposeFree) {
      release(what, pointer);
      return;
    }
    try {
      const outcome = host.disposeFree(pointer);
      if (isThenable(outcome)) {
        void Promise.resolve(outcome as PromiseLike<void>).catch((error: unknown) => {
          report(what, `disposeFree rejected for ${pointer}; not retried: ${String(error)}`);
        });
      }
    } catch (error) {
      report(what, `disposeFree threw for ${pointer}; not retried: ${String(error)}`);
    }
  };

  /**
   * Runs `body` inside the driver's sleep when one is installed, so the guest unwinds and rewinds
   * around it, and otherwise runs it inline.
   *
   * Returning the sleep's value rather than a promise is what keeps the import bodies synchronous
   * from the guest's point of view: `sleep` unwinds and answers 0 on the first pass, and the driver
   * replays the guest once the promise settles — a replay that reaches this row again and answers
   * from the driver's recorded slot **without calling the thunk a second time**. So `body` runs
   * exactly once per call, which is the whole reason the fetch lives inside it.
   *
   * `body` returns the promise when there is one, so the driver knows what to park on; a
   * synchronous body has nothing to wait for and simply completes.
   */
  const suspend = (body: () => number | void | Promise<number>): number | void => {
    if (!sleep) {
      body();
      return;
    }
    return sleep(body);
  };

  /**
   * Schedules one completion task out of the current import call.
   *
   * This is the seam that makes `Qa` and `Jc` non-blocking: the import body resolves and returns, and
   * everything with a side effect — allocate, copy, callback, free — happens in here instead, on a
   * later turn. The default is a microtask, which is enough to leave the current raw root; a session
   * that also drives the asyncify driver passes an enqueue that waits for it to go idle, because a
   * callback re-entering the guest while a raw root is still on the stack is a nested call at the
   * worst possible moment.
   *
   * **What is deferred is the completion, and nothing else.** The `enqueue` seam is the *actual*
   * session's own queue, so it is exactly the thing that serializes guest entry, and anything put
   * inside the task waits behind every other queued turn. A transfer's own request must therefore
   * be issued *before* the task — by {@link callResolver}, in the import body — because a session
   * that parks the queue on the network would otherwise wait for a network operation to start a
   * fetch that has not been issued yet. The import body issues; the task delivers.
   *
   * A task is never run twice and never left unobserved: a synchronous throw and a rejected promise
   * both end as one report, so a session sees one diagnosis per failed completion rather than an
   * unhandled rejection with no attribution. The task is also not started at all when the host is
   * already disposed — the caller has torn the guest down, and there is nothing left to deliver to.
   *
   * `task` may return a promise, and the **whole** task is awaited. The enqueue seam's own promise is
   * awaited too, which is what makes `host.enqueue(...)` as a whole mean "this completion is
   * finished" — and that matters when the seam *is* the turn: a session whose `enqueue` returns a
   * promise that settles once the driver is idle hands the caller a completion that is still running
   * if it is not waited for.
   *
   * The returned promise resolves when the completion is over — successfully, or with its failure
   * already reported. Nothing is left unobserved on any of the three paths (no seam, a throwing
   * seam, a rejecting seam), so a caller may await it or ignore it without producing an unhandled
   * rejection either way.
   */
  const enqueueTask = (what: string, task: () => void | Promise<void>): Promise<void> => {
    // The task's own failure is rebuilt as a *reported* promise: the rejection never escapes, and
    // the promise a seam awaits resolves even when the completion inside it failed. A completion
    // that could not be delivered to a guest that is gone is not a seam failure.
    const run = (): Promise<void> =>
      Promise.resolve()
        .then(() => {
          // Re-checked inside the task, not only at scheduling time: a task deferred across an await
          // can easily outlive the guest it belongs to, and the wait is exactly the window where
          // that happens.
          if (host.isDisposed?.()) return;
          return task();
        })
        .then(
          () => undefined,
          (error: unknown) => {
            report(what, `deferred completion threw: ${String(error)}`);
          },
        );
    if (host.isDisposed?.()) return Promise.resolve();
    if (!host.enqueue) return run();
    let outcome: void | Promise<void>;
    try {
      outcome = host.enqueue(run);
    } catch (error) {
      report(what, `enqueue threw: ${String(error)}`);
      return Promise.resolve();
    }
    if (isThenable(outcome)) {
      // The task's own failure is already consumed above; this consumes the *seam* failing, which is
      // a different thing and is reported as such. `run` is never called by this path, so a seam
      // that schedules nothing and rejects models a queue that died — the completion is not run, and
      // there is nothing left to orphan.
      return Promise.resolve(outcome as PromiseLike<void>).then(
        () => undefined,
        (error: unknown) => {
          report(what, `enqueue rejected: ${String(error)}`);
        },
      );
    }
    return Promise.resolve();
  };

  /**
   * Settles a resolver's answer for the two callback rows: bytes now, or bytes after a promise.
   *
   * **The subscription is taken here, in the import body, and that is the whole point.** The
   * resolver's promise is what makes the transfer happen, and a session's `enqueue` seam is
   * *serialized*: a task waiting on the network would park the queue for as long as the bytes take,
   * so a completion that was already ready — a cached answer, a `Promise.resolve` — would sit behind
   * it and be delivered late. Subscribing now means the network is watched independently of the
   * queue; the queue only ever carries an effect, which is short and cannot depend on a fetch.
   *
   * The completion is therefore *not* run here. Once the answer has settled **and** been validated,
   * exactly one {@link enqueueTask} carries the effect — `settle` on an answer, `failure` on a
   * defect — and the guest is entered only from inside it. A synchronous answer takes the same road
   * rather than running inline, because the import body is where the queue must not be jumped: the
   * effect is queued either way, so the two arms differ in when the queue is asked, not in whether
   * it is.
   *
   * The returned promise settles when the *decision* does — the subscription, the validation and
   * the enqueue — and never rejects: every path is terminal, and a queue that has already died is
   * `enqueueTask`'s own report to make rather than a rejection from here.
   */
  const settleAsync = (
    request: ArtemisWgetRequest,
    what: string,
    answer: Uint8Array | null | Promise<Uint8Array | null>,
    settle: (bytes: Uint8Array | null) => void | Promise<void>,
    failure: (detail: string) => void | Promise<void>,
  ): Promise<void> => {
    const narrowed = (resolved: unknown): Uint8Array | null => {
      try {
        // `canAwait` is true, so this narrowing never throws for a promise — the settled value is
        // what is left to judge, and a resolver that answers its own promise with something else is
        // the defect `failure` is for. Returning a rejection instead of throwing keeps the refusal
        // off the unhandled-rejection channel: nothing else holds this promise.
        const bytes = toBytes(resolved, request, what, true);
        if (isThenable(bytes)) return null;
        return bytes as Uint8Array | null;
      } catch (error) {
        return Promise.reject(error) as unknown as Uint8Array | null;
      }
    };
    const enqueued = (body: () => void | Promise<void>): Promise<void> => {
      // Nothing is queued for a guest that is already gone: there is no heap to write and no
      // callback left to enter.
      if (host.isDisposed?.()) return Promise.resolve();
      return enqueueTask(what, body);
    };
    if (!isThenable(answer)) {
      const settled = narrowed(answer);
      if (isThenable(settled)) {
        return Promise.resolve(settled as PromiseLike<Uint8Array | null>).then(
          (bytes) => enqueued(() => settle(bytes)),
          (error: unknown) => enqueued(() => failure(`resolver answered with ${String(error)}`)),
        );
      }
      return enqueued(() => settle(settled as Uint8Array | null));
    }
    return Promise.resolve(answer as PromiseLike<Uint8Array | null>).then(
      (resolved) => {
        // After the wait, before anything is allocated or any callback is entered — and before the
        // queue is asked, since there is nothing left to deliver to a torn-down guest.
        if (host.isDisposed?.()) return undefined;
        const settled = narrowed(resolved);
        if (isThenable(settled)) {
          return Promise.resolve(settled as PromiseLike<Uint8Array | null>).then(
            (bytes) => enqueued(() => settle(bytes)),
            (error: unknown) =>
              enqueued(() => failure(`resolver answered its Promise with ${String(error)}`)),
          );
        }
        return enqueued(() => settle(settled as Uint8Array | null));
      },
      // `toBytes` has already attached a catch, so a rejection reaches here only through a promise
      // that had been handed out before that; the handler is kept because an unhandled rejection is
      // exactly what this promise's ownership rules exist to prevent.
      (error: unknown) => enqueued(() => failure(`resolver rejected: ${String(error)}`)),
    );
  };

  /** `Browser.getNextWgetRequestHandle` — the glue's monotonic request-handle counter. */
  const handles = { next: 0 };

  return {
    /**
     * `_emscripten_wget_data(url, pbuffer, pnum, perror)` — the Asyncify blocking download.
     *
     * The glue sleeps on `Asyncify.handleSleep` and fills the three out-pointers from the
     * completion callback. The whole completion handler is the thunk here, so the writes happen on
     * the rewind pass and the return value stays 0 because the import's type is `void`.
     *
     * `perror` is the one that matters: it is 0 on success and 1 on any failure — including "this
     * host has no network" and "the resolver answered null". A caller that only checks `perror`
     * behaves identically under both. A resolver that breaks its contract instead throws out of
     * here, because that is a host defect and must not masquerade as a failed download.
     */
    fa: (url: number, pbuffer: number, pnum: number, perror: number): void => {
      const request = { url: utf8(url), method: 'GET', body: '' };
      const fail = (detail: string): void => {
        report('wget_data', detail);
        if (host.isDisposed?.()) return;
        if (!writeError(perror)) {
          report('wget_data', `could not report the failure: perror ${perror} is not a usable i32 slot`);
        }
      };
      const settle = (bytes: Uint8Array | null): void => {
        if (host.isDisposed?.()) return;
        if (bytes === null) {
          // Already reported by the resolver seam, which knows *why* there were no bytes. Reporting
          // again here would log the same refusal twice for one call.
          if (!writeError(perror)) {
            report('wget_data', `could not report the failure: perror ${perror} is not a usable i32 slot`);
          }
          return;
        }
        const buffer = toGuestBuffer(bytes);
        if (buffer === 0) {
          fail(`${request.url} resolved to ${bytes.length} B but the guest could not take it`);
          return;
        }
        if (host.isDisposed?.()) {
          // Nothing has been written yet, and the buffer was allocated for a guest that is gone. The
          // failure path would only write into a freed heap, so the bytes are released instead.
          release('wget_data', buffer);
          return;
        }
        if (!writeOutPointers(pbuffer, pnum, perror, buffer, bytes.length)) {
          // All-or-none: nothing was written, so the bytes this call allocated are nobody's. The
          // guest is about to be told the transfer failed, and handing it a buffer to free that it
          // was never told about would be a leak it cannot even see. The primary diagnosis is the
          // bad address; a release that also fails is reported beside it, never instead of it.
          release('wget_data', buffer);
          fail(
            `out-pointers ${pbuffer}/${pnum}/${perror} are not aligned, in-heap i32 slots for a ` +
              `${bytes.length} B transfer`,
          );
        }
      };
      suspend(() => {
        const bytes = callResolver(request, 'wget_data', sleep !== undefined, true);
        // A promise means the driver parks the guest on it; the validation, the allocation and the
        // writes then happen in the continuation, still inside this one thunk. The replay that
        // follows answers from the driver's recorded slot without re-entering `body`, so the fetch
        // is issued once and the buffer is allocated once.
        if (isThenable(bytes)) {
          return Promise.resolve(bytes).then(
            (resolved) => {
              // The one place disposal is invisible to the guest: it cannot be told, because the
              // frame that would read `perror` is about to be abandoned. So a handle that goes away
              // while parked gets the *only* treatment left to give it — nothing is written at all,
              // and the resolver's own bytes are left alone, since they were never a guest
              // allocation and there is no longer a heap to copy them into.
              if (host.isDisposed?.()) return 0;
              const checked = toBytes(resolved, request, 'wget_data', false);
              if (isThenable(checked)) {
                throw new ArtemisHostError(
                  `artemis wget_data resolver answered ${request.method} ${request.url} with a Promise ` +
                    'for a Promise: the awaiting caller narrows a settled answer once, so a promise ' +
                    'here is a defect in the resolver.',
                );
              }
              settle(checked as Uint8Array | null);
              return 0;
            },
            (error: unknown) => {
              // A rejected answer is an ordinary download failure, not a defect in the host: upstream
              // `_emscripten_wget_data` reports a network error by setting `perror` to 1 and waking the
              // guest. Throwing here would instead fail `sleep`'s thunk and take the whole root down,
              // which is a failure mode the guest never asked for — it checks `perror`, not the thunk.
              // The rejection was already reported where the promise was first seen, so this arm only
              // owes the guest-visible effect.
              if (host.isDisposed?.()) return 0;
              fail(
                `${request.method} ${request.url} could not be fetched: ${String(error)}`,
              );
              return 0;
            },
          );
        }
        // Nothing to wait for. The disposal check is the same one the waiting path makes after its
        // await, because a synchronous answer can follow a `sleep` seam that already tore the guest
        // down: writing `perror` into a freed heap is worse than writing nothing.
        if (host.isDisposed?.()) return 0;
        settle(bytes);
        return 0;
      });
    },

    /**
     * `_emscripten_async_wget_data(url, arg, onload, onerror)` — the callback-shaped download.
     *
     * The glue calls `onload(arg, buffer, length)` with a buffer it frees immediately afterwards,
     * so the callback has to copy what it wants, and calls `onerror(arg)` with one argument. Those
     * arities are the glue's own — `getDynCaller("viii", onload)` and `getDynCaller("vi", onerror)`
     * — and they are passed through as signatures rather than flattened, because a host dispatching
     * to the guest's `dynCall_*` exports cannot recover the arity from the values alone.
     *
     * The buffer is held until a promise-valued callback settles, then freed: an `onload` that
     * itself waits on another download is still reading it, and freeing first would hand it a
     * pointer the guest has already reused. Freedom happens on the failure paths the glue does not
     * reach (`_malloc` returned 0) as well, where there is no buffer to release.
     *
     * **Non-blocking.** The import issues the fetch and returns before the transfer lands, exactly as
     * the glue's `xhr.send()` does; the guest is never unwound for this row. A resolver answering a
     * `Promise` does not change that — the promise is the host's own latency, and everything after it
     * happens on {@link ArtemisAsyncifyHost.enqueue}, never in this body. The allocation is inside the
     * enqueued task too, so a transfer that never lands allocates nothing.
     */
    Qa: (url: number, arg: number, onload: number, onerror: number): void => {
      const request = { url: utf8(url), method: 'GET', body: '' };
      // The guest is owed exactly one of the two callbacks, and this is the only place either is
      // entered. `failure` is the reporting half and `error` the delivery half so the two can be
      // called together on the defect path without the second one reporting the first one again.
      const error = (): void | Promise<void> =>
        invokeCallback('async_wget_data', 'vi', onerror, [arg | 0]);
      const failure = (detail: string): void | Promise<void> => {
        report('async_wget_data', detail);
        return error();
      };
      const settle = (bytes: Uint8Array | null): void | Promise<void> => {
        if (bytes === null) {
          // The resolver seam already reported why there are no bytes; a second report here would
          // log the same refusal twice for one call. Only the guest-visible effect is owed.
          return error();
        }
        const buffer = toGuestBuffer(bytes);
        if (buffer === 0) {
          return failure(`${request.url} resolved to ${bytes.length} B but the guest could not take it`);
        }
        const length = bytes.length;
        const delivered = invokeCallback('async_wget_data', 'viii', onload, [arg | 0, buffer, length]);
        // Awaited before the release so a suspending onload keeps the bytes; a synchronous one
        // releases in the same turn, exactly as the glue's trailing `_free(buffer)` does. The
        // release is chained onto the already-handled delivery, so a rejected load cannot settle a
        // release that never waited for it.
        return Promise.resolve(delivered).then(() => release('async_wget_data', buffer));
      };
      const answer = callResolver(request, 'async_wget_data', true);
      // The answer is subscribed to above, in the import body — the fetch must not wait on the
      // queue. What is deferred is the *effect*, and `settleAsync` enqueues exactly that once the
      // bytes exist: a session whose `enqueue` waits for the asyncify driver to go idle still issues
      // the request before it, and a completion that was ready from the start is not parked behind a
      // network wait that has nothing to do with it.
      void settleAsync(request, 'async_wget_data', answer, settle, failure);
      // Returns immediately: the guest was never unwound for this row and nothing has been written
      // to its heap yet. The callback arrives on a later turn.
    },

    /**
     * `_emscripten_async_wget2_data(url, request, param, arg, free, onload, onerror, onprogress)`.
     *
     * The XHR-shaped download: it carries an HTTP method, a body, a `free` flag telling it whether to
     * release the buffer after the callback, and an `onprogress` it may or may not have. Returns a
     * request handle so the caller can abort it later — the glue hands back a counter entry.
     *
     * **The handle is returned before the transfer is resolved**, and that is the glue's own order:
     * `Browser.getNextWgetRequestHandle()` is taken, the XHR is opened, and the handle is returned
     * at the end of the same synchronous body while the callbacks arrive later. Taking it first also
     * means the handle is a property of the *call*, not of the transfer, so the guest can abort a
     * request whose bytes have not landed yet.
     *
     * **Non-blocking**, like `Qa` and unlike `fa`: a `Promise` from the resolver buys the transfer
     * nothing but deferral, and the completion runs on {@link ArtemisAsyncifyHost.enqueue}. Wrapping
     * this row in `sleep` would unwind the very guest that is supposed to keep running, and the
     * replay would then re-issue the fetch and hand the callback a second handle's worth of work.
     *
     * All four callbacks are the glue's `viiii` — handle, arg, then two more — and the two status
     * slots are the interesting part. `onload` gets `(handle, arg, buffer, length)`; `onerror` gets
     * `(handle, arg, status, statusText)`; `onprogress` gets `(handle, arg, loaded, total)`. The
     * glue passes `http.statusText` — a **string** — straight into a wasm `i32` parameter, so V8
     * coerces it to `0` at the call boundary. This host passes `0` for it, which is the value the
     * guest actually observes; inventing a pointer to a string the guest never asked for would
     * write a value into a register the official build leaves at zero.
     *
     * The status arithmetic the glue uses is worth copying exactly: a 2xx status succeeds, and so
     * does status 0 on a non-`http:` URL, which is how a `file://` or data-URI read succeeds. A
     * non-2xx goes to `onerror` with the status and its text. This host has one success form and one
     * failure form rather than a status space, so a failed transfer reports status 0.
     */
    Jc: (
      url: number,
      request: number,
      param: number,
      arg: number,
      freeFlag: number,
      onload: number,
      onerror: number,
      onprogress: number,
    ): number => {
      const decoded: ArtemisWgetRequest = {
        url: utf8(url),
        method: utf8(request),
        body: utf8(param),
      };
      const handle = handles.next;
      handles.next += 1;
      const error = (): void | Promise<void> =>
        invokeCallback('async_wget2_data', 'viiii', onerror, [handle, arg | 0, 0, 0]);
      const failure = (detail: string): void | Promise<void> => {
        report('async_wget2_data', detail);
        return error();
      };
      const settle = (bytes: Uint8Array | null): void | Promise<void> => {
        if (bytes === null) {
          // Already reported by the seam that produced the `null`; only the guest-visible callback
          // is owed. Reporting here too would log one refusal twice.
          return error();
        }
        const buffer = toGuestBuffer(bytes);
        if (buffer === 0) {
          return failure(`${decoded.url} resolved to ${bytes.length} B but the guest could not take it`);
        }
        const length = bytes.length;
        // Progress first, as the XHR event order has it, and waited for before the load: an
        // `onprogress` that itself suspends must have finished before `onload` is entered, or the
        // two callbacks interleave in the guest and the load reads a buffer the progress handler is
        // still walking. The glue gets that ordering for free from the event loop; here it is
        // explicit, and it is enforced by the promised chain rather than by a `.then` nobody holds.
        const progressed = invokeCallback('async_wget2_data', 'viiii', onprogress, [
          handle,
          arg | 0,
          length,
          length,
        ]);
        const load = (): void | Promise<void> => {
          const delivered = invokeCallback('async_wget2_data', 'viiii', onload, [
            handle,
            arg | 0,
            buffer,
            length,
          ]);
          // `freeFlag` is the guest's own instruction about the buffer's lifetime, so it is honoured
          // on both outcomes, and after a suspending `onload` rather than before it — the callback
          // is still reading the bytes until its own promise settles. A callback that throws or
          // rejects is already a reported, handled delivery, so this chains onto it and the release
          // still happens: the alternative is a buffer the guest was told to expect and never
          // receives.
          return Promise.resolve(delivered).then(() => {
            if (freeFlag) release('async_wget2_data', buffer);
          });
        };
        // Returned, so the whole register phase — progress, then load, then the release — is one
        // awaited unit inside the task rather than a chain the task merely starts.
        return Promise.resolve(progressed).then(load);
      };
      const answer = callResolver(decoded, 'async_wget2_data', true);
      // Enqueued, not awaited: the handle is this call's answer and it is returned below, before any
      // of the transfer's effects happen. The guest may abort a request whose bytes have not landed.
      // As in `Qa`, the request itself was issued and subscribed to above, so a queue that waits for
      // the driver cannot delay the fetch; only the completion goes through the queue, and it goes
      // there when the bytes exist rather than when this row is called.
      void settleAsync(decoded, 'async_wget2_data', answer, settle, failure);
      return handle;
    },

    /**
     * `_emscripten_memcpy_big(dest, src, num)` — the Asyncify spill copy.
     *
     * The glue is `HEAPU8.copyWithin(dest>>>0, src>>>0, (src+num)>>>0)` — the end is `src+num`, and
     * not `max(dest, src+num)`. That difference is not cosmetic: an earlier version here used the
     * `Math.max` form, which *extends* the source range whenever `dest` sits past `src+num` and
     * copies bytes the call never asked for, over the destination. The correct end also reproduces
     * the glue's handling of a negative `num` — `(src+num)>>>0` wraps, and `copyWithin` clamps an
     * end below the start to a zero-length copy, which is what the original does rather than
     * throwing on a range check it does not have.
     *
     * A `subarray` is deliberately not used: `copyWithin` on the live `u8` view is one pass over the
     * bytes, and an overlapping copy in the wrong direction would need a temporary, which the glue
     * does not build either — wasm's own `memory.copy` is byte-accurate in both directions, and this
     * host is faithful to what the *glue* does here rather than to what the wasm spec would allow.
     *
     * The view is re-read for the same reason as everywhere else in this file: a stale `u8` over a
     * detached buffer would silently drop the copy instead of writing it.
     */
    yb: (dest: number, src: number, num: number): number => {
      const start = (dest | 0) >>> 0;
      const from = (src | 0) >>> 0;
      // `num | 0` rather than a signed `num`: the glue's own expression is `(src + num) >>> 0` on
      // i32 values, so a negative `num` descends from `src` and the end lands below the start, which
      // `copyWithin` clamps to a zero-length copy instead of throwing on a range it does not check.
      const end = (from + (num | 0)) >>> 0;
      // Re-read after the refresh: a copy through a view over a detached buffer writes nowhere and
      // reports nothing, which would silently lose the spill.
      refreshMemory();
      memory.u8.copyWithin(start, from, end);
      return 0;
    },
  };
}

/**
 * Proper thenable detection: any object or function exposing a `then` method.
 *
 * A type predicate rather than a plain `boolean`, so the resolver's three-way answer narrows to the
 * promise arm at each call site instead of needing a cast there.
 */
function isThenable(value: unknown): value is PromiseLike<unknown> {
  if (typeof value !== 'object' && typeof value !== 'function') return false;
  if (value === null) return false;
  return typeof (value as { then?: unknown }).then === 'function';
}

/** The bridge's 4 `asyncify` short names, in the order `imports-map.json` first lists them. */
export const ARTEMIS_ASYNCIFY_IMPORTS: readonly string[] = ['Jc', 'Qa', 'fa', 'yb'];

/** The official name behind each `asyncify` short name, for a report a human can read. */
export const ARTEMIS_ASYNCIFY_OFFICIAL: Readonly<Record<string, string>> = {
  Jc: '_emscripten_async_wget2_data',
  Qa: '_emscripten_async_wget_data',
  fa: '_emscripten_wget_data',
  yb: '_emscripten_memcpy_big',
};
