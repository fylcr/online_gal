import test from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, writeFileSync, existsSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

/**
 * The `asyncify` download rows: contract, arity, exactness, and the real driver's suspend path.
 *
 * Three things are being proved here, and they are different in kind. The first is the resolver
 * contract — what a `Uint8Array`, a `null`, a promise and a broken answer each mean to the guest.
 * The second is the **split between the blocking row and the non-blocking ones**: `fa` suspends the
 * guest, while `Qa` and `Jc` return immediately and deliver their callbacks on a later turn, exactly
 * as the glue's XHR does. The third is that a fetch issued through a callback-shaped row happens
 * **exactly once**, and that its completion lands *after* the raw root that issued it has returned —
 * which is the whole point of the enqueue seam, and is not observable without the real driver, so
 * the driver is compiled here rather than faked.
 *
 * The distinction matters because it is the one a wrong implementation gets subtly wrong: an
 * implementation that suspends `Qa`/`Jc` still passes a naive "the callback eventually lands" test
 * while unwinding a guest that was never supposed to block, and re-fetching on the rewind.
 */

const repoRoot = resolve(fileURLToPath(new URL('..', import.meta.url)));
const srcRoot = join(repoRoot, 'packages', 'runtime-artemis', 'src');
const temp = mkdtempSync(join(tmpdir(), 'artemis-download-'));
const out = join(temp, 'out');

/** Compile the host modules this test exercises into `out`, and import them. */
const { createArtemisAsyncifyImports, ArtemisSuspendDriver, ArtemisHostError, ArtemisMemory } =
  await (async () => {
    execFileSync(
      process.execPath,
      [
        join(repoRoot, 'node_modules', 'typescript', 'bin', 'tsc'),
        join(srcRoot, 'host', 'asyncify.ts'),
        join(srcRoot, 'host', 'suspend.ts'),
        // The real memory class is compiled rather than stubbed: the host's `refresh()` discipline
        // is under test, and a hand-written double would be a statement of what the test *hopes*
        // `ArtemisMemory` does. No extra wasm artifact is needed — the driver's own world is the
        // driver, and `WebAssembly.Memory` is built into the runtime.
        join(srcRoot, 'host', 'memory.ts'),
        '--target', 'ES2022',
        '--module', 'ESNext',
        '--moduleResolution', 'bundler',
        '--lib', 'ES2022,DOM',
        '--strict',
        '--skipLibCheck',
        '--rootDir', srcRoot,
        '--outDir', out,
      ],
      { stdio: 'inherit', cwd: repoRoot },
    );
    writeFileSync(join(out, 'package.json'), JSON.stringify({ type: 'module' }));
    const asyncify = await import(pathToFileURL(join(out, 'host', 'asyncify.js')).href);
    const suspend = await import(pathToFileURL(join(out, 'host', 'suspend.js')).href);
    const errno = await import(pathToFileURL(join(out, 'host', 'errno.js')).href);
    const memory = await import(pathToFileURL(join(out, 'host', 'memory.js')).href);
    return {
      createArtemisAsyncifyImports: asyncify.createArtemisAsyncifyImports,
      ArtemisSuspendDriver: suspend.ArtemisSuspendDriver,
      ArtemisHostError: errno.ArtemisHostError,
      ArtemisMemory: memory.ArtemisMemory,
    };
  })();

/**
 * A guest heap the tests can grow.
 *
 * `ArtemisMemory` is the real class in the host, but its `refresh()` discipline is exactly what is
 * under test — a resolver that awaits can let the guest run and grow, and the copy has to see the
 * new buffer. Rebuilding the views here from a swappable `buffer` reproduces that hazard faithfully
 * while keeping the heap small enough to reason about.
 */
function makeHeap(initialBytes = 4096) {
  let buffer = new ArrayBuffer(initialBytes);
  const views = {};
  const refresh = () => {
    views.u8 = new Uint8Array(buffer);
    views.i32 = new Int32Array(buffer);
  };
  refresh();
  const allocations = [];
  let next = 256;
  const heap = {
    get u8() {
      return views.u8;
    },
    get i32() {
      return views.i32;
    },
    get byteLength() {
      return buffer.byteLength;
    },
    /** Doubles the heap, detaching every view a caller may be holding. */
    growTo(bytes) {
      buffer = new ArrayBuffer(bytes);
      refresh();
    },
    malloc: (byteLength) => {
      const pointer = next;
      next += Math.max(byteLength, 1);
      if (next > buffer.byteLength) throw new Error('test heap exhausted');
      allocations.push(pointer);
      return pointer;
    },
    free: (pointer) => {
      const index = allocations.indexOf(pointer);
      if (index >= 0) allocations.splice(index, 1);
    },
    allocations,
    /** Writes a NUL-terminated UTF-8 string and returns its pointer. */
    writeString(text, at = 1024) {
      const encoded = new TextEncoder().encode(`${text}\0`);
      views.u8.set(encoded, at);
      return at;
    },
  };
  return heap;
}

/**
 * An `ArtemisMemory`-shaped wrapper: the host only ever touches `u8`, `i32` and — when it is a real
 * `ArtemisMemory` — `refresh()`.
 *
 * `refresh` is optional, and its absence is the point of this helper: a mock whose accessors rebuild
 * their views on demand is exactly the case that *cannot* notice a heap that grew under it, so the
 * tests that hand one to the host are also the tests that prove the host does not depend on a
 * `refresh` it was never given.
 */
const asMemory = (heap) => ({
  get u8() {
    return heap.u8;
  },
  get i32() {
    return heap.i32;
  },
});

/** A real `WebAssembly.Memory` as the host's heap, through the real `ArtemisMemory` views. */
async function makeWasmHeap(initialBytes = 65536) {
  const { ArtemisMemory } = await import(
    pathToFileURL(join(out, 'host', 'memory.js')).href
  );
  const memory = new WebAssembly.Memory({ initial: initialBytes / 65536, maximum: 1024 });
  return { heap: new ArtemisMemory({ memory }), wasm: memory };
}

/** Bounded wait: a never-settling promise fails the test instead of hanging the suite. */
async function bounded(promise, ms = 2000) {
  let timer;
  const deadline = new Promise((_, reject) => {
    timer = setTimeout(() => reject(new Error('bounded wait expired')), ms);
  });
  try {
    return await Promise.race([Promise.resolve(promise), deadline]);
  } finally {
    clearTimeout(timer);
  }
}

/**
 * Runs `body` with an `unhandledRejection` listener attached, always detaching it.
 *
 * Returns the recorded reasons. A `Promise` is awaited *inside* the window, which is the part that
 * matters: a rejection reported as unhandled is delivered asynchronously, so a listener removed too
 * early — or an assertion made before the delivery — observes nothing whether or not there was
 * anything to observe.
 */
async function observingUnhandled(probe) {
  const orphaned = [];
  const onUnhandled = (reason) => orphaned.push(reason);
  process.on('unhandledRejection', onUnhandled);
  try {
    if (probe) await probe();
    // A grace turn, so a rejection delivered after the probe's own work still lands in the window.
    await sleep(20);
    return orphaned;
  } finally {
    process.off('unhandledRejection', onUnhandled);
  }
}

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

/**
 * A host that records the tasks the callback rows defer, without running them.
 *
 * `Qa` and `Jc` are non-blocking, so their completion is deliberately *not* finished when the import
 * returns. Asserting on the absence of effects right after the call is only meaningful if the task
 * is still pending, which is what this seam provides: `tasks` is drained by the test itself, so the
 * "before" and "after" states are both controllable rather than a race against a microtask.
 */
function makeTaskQueue() {
  const tasks = [];
  return {
    tasks,
    enqueue: (task) => {
      tasks.push(task);
    },
    /** Runs every pending task, including ones enqueued by the tasks themselves. */
    async drain(limit = 8) {
      let ran = 0;
      while (tasks.length > 0) {
        if (ran > limit) throw new Error('task queue did not settle');
        const task = tasks.shift();
        await task();
        ran += 1;
      }
      return ran;
    },
  };
}

/** Waits for a deferred tick, so a queued microtask (the default enqueue) has run. */
const tick = () => new Promise((r) => queueMicrotask(r));

/**
 * A promise whose rejection is never delivered as an unhandled one.
 *
 * Node reports an unhandled rejection only when a rejected promise has no handler by the end of the
 * turn it settles in. A deliberately-failing fetch is therefore only *conditionally* unhandled: if
 * the host already subscribed before it rejects, there is nothing to report — which is the correct
 * behaviour and is indistinguishable from a bug by simply watching for the event. This attaches
 * nothing, so the host's own subscription is what the test is measuring.
 */
let pendingWaits = 0;
function pendingWait() {
  pendingWaits += 1;
  let resolve;
  const promise = new Promise((r) => (resolve = r));
  void promise.catch(() => undefined);
  // Counted on *settlement by the host*, not on `resolve` being reached, and each wait is released
  // exactly once on either arm — the rejection arm included — so the after-hook below reports a leak
  // in the run rather than a property of the helper. A second release is a no-op: a test that
  // settles its own gate and then cleans up in an after-hook must not double-count the release.
  let done = false;
  const settle = (value) => {
    if (done) return;
    done = true;
    pendingWaits -= 1;
    resolve(value);
  };
  return { promise, resolve: settle, reject: (error) => settle(Promise.reject(error)) };
}

/**
 * Waits for the condition to hold, without letting a rejection from the host escape.
 *
 * The `catch` is not decoration: while the condition is still false the wait is genuinely pending,
 * and a rejection arriving in that window — a rejected fetch the host has already subscribed to —
 * would otherwise surface as an unhandled rejection of *this* promise, which is a property of the
 * probe rather than of the host.
 */
async function until(condition, ms = 2000) {
  const deadline = Date.now() + ms;
  while (Date.now() < deadline) {
    if (condition()) return;
    await sleep(1);
  }
  throw new Error('until: condition never held');
}

/** At the end of the test file every hand-built pending wait really did settle. */
test.after(() => {
  assert.equal(pendingWaits, 0, 'every pending wait handed to the host was settled by the run');
});

const bytesOf = (text) => new TextEncoder().encode(text);

test.after(() => {
  const target = resolve(temp);
  assert.ok(target.startsWith(resolve(tmpdir())), `unexpected temp root: ${target}`);
  assert.ok(existsSync(target));
  assert.equal(basename(target), basename(temp), 'resolved path must be the fresh mkdtemp dir');
  assert.equal(basename(target).startsWith('artemis-download-'), true, `unexpected prefix: ${basename(target)}`);
  rmSync(target, { recursive: true, force: true });
});

// --- the resolver contract ---------------------------------------------------------------------

test('fa: a synchronous resolver fills all three out-pointers and returns void', () => {
  const heap = makeHeap();
  const report = [];
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: (request) => {
      assert.equal(request.method, 'GET');
      assert.equal(request.url, 'https://example.test/a.bin');
      return bytesOf('hello');
    },
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const url = heap.writeString('https://example.test/a.bin');
  const pbuffer = 16;
  const pnum = 20;
  const perror = 24;

  // The import's type is void, so nothing but the out-pointers carries the answer.
  assert.equal(imports.fa(url, pbuffer, pnum, perror), undefined);
  const buffer = heap.i32[pbuffer >>> 2] >>> 0;
  assert.equal(heap.i32[pnum >>> 2], 5);
  assert.equal(heap.i32[perror >>> 2], 0);
  assert.deepEqual([...heap.u8.subarray(buffer, buffer + 5)], [...bytesOf('hello')]);
  // Allocated once, and not freed: the guest owns the buffer and frees it itself.
  assert.deepEqual(heap.allocations, [256]);
  assert.deepEqual(report, []);
});

test('fa: a resolver answering null is a failed transfer — perror 1, no allocation', () => {
  const heap = makeHeap();
  const report = [];
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => null,
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const url = heap.writeString('https://example.test/missing.bin');
  const pbuffer = 16;
  const pnum = 20;
  const perror = 24;

  assert.equal(imports.fa(url, pbuffer, pnum, perror), undefined);
  assert.equal(heap.i32[perror >>> 2], 1, 'null is the guest-visible failure form');
  assert.deepEqual(heap.allocations, [], 'a failed transfer allocates nothing');
  // A resolver deliberately answering null is an ordinary failed transfer, not a blocked call: the
  // report channel is for what the host could not do, and this host did exactly what was asked.
  assert.deepEqual(report, []);
});

test('fa: no resolver at all writes the failure form and records the refusal', () => {
  const heap = makeHeap();
  const report = [];
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const url = heap.writeString('https://example.test/a.bin');
  const perror = 24;

  assert.equal(imports.fa(url, 16, 20, perror), undefined);
  assert.equal(heap.i32[perror >>> 2], 1);
  assert.equal(report.length, 1, 'the refusal is reported once, not once per layer');
  assert.match(report[0][1], /no resolver/);
});

test('fa: a resolver that throws is a host defect, not a failed download', () => {
  const heap = makeHeap();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => {
      throw new Error('network exploded');
    },
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.throws(() => imports.fa(url, 16, 20, 24), (error) => {
    assert.ok(error instanceof ArtemisHostError, `expected ArtemisHostError, got ${error?.constructor?.name}`);
    assert.match(error.message, /resolver threw: Error: network exploded/);
    return true;
  });
  // Nothing was written: a defect must not also look like a reported failure.
  assert.equal(heap.i32[24 >>> 2], 0);
});

test('a non-Uint8Array, non-null answer is refused as a host defect', () => {
  const heap = makeHeap();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => [1, 2, 3],
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.throws(() => imports.fa(url, 16, 20, 24), (error) => {
    assert.ok(error instanceof ArtemisHostError);
    assert.match(error.message, /expected Uint8Array, null, or a Promise/);
    return true;
  });
});

test('a promise without a sleep seam is refused honestly, and its rejection is not orphaned', async () => {
  const heap = makeHeap();
  const orphaned = [];
  const onUnhandled = (reason) => orphaned.push(reason);
  process.on('unhandledRejection', onUnhandled);
  try {
    const imports = createArtemisAsyncifyImports({
      memory: asMemory(heap),
      malloc: heap.malloc,
      free: heap.free,
      // No `sleep`: `fa` is the blocking row, and without the seam nothing can suspend the guest, so
      // a promise has no way back to the frame that asked for the bytes.
      resolve: () => Promise.reject(new Error('late failure')),
    });
    const url = heap.writeString('https://example.test/a.bin');

    assert.throws(() => imports.fa(url, 16, 20, 24), (error) => {
      assert.ok(error instanceof ArtemisHostError);
      assert.match(error.message, /this row cannot wait for it/);
      return true;
    });

    // The rejection above would otherwise surface as an unhandled rejection nobody can act on.
    await sleep(20);
    process.off('unhandledRejection', onUnhandled);
    assert.deepEqual(orphaned, []);
  } finally {
    process.off('unhandledRejection', onUnhandled);
  }
});

test('Qa and Jc need no sleep seam: a Promise from the resolver does not suspend the guest', async () => {
  // The mirror image of the test above, and the defect this split exists to prevent. These rows are
  // non-blocking, so a promise buys them deferral and nothing else — they must not demand a `sleep`
  // and must not unwind even when one is installed.
  const heap = makeHeap();
  const queue = makeTaskQueue();
  const seen = [];
  let resolveBytes;
  const pending = new Promise((r) => (resolveBytes = r));
  const report = [];
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    // No `sleep` at all, and a resolver that answers a Promise: neither row may refuse this.
    resolve: () => pending,
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const url = heap.writeString('https://example.test/a.bin');
  const method = heap.writeString('GET', 2048);
  const body = heap.writeString('', 3072);

  // Both imports return at once, with nothing delivered and nothing allocated.
  assert.equal(imports.Qa(url, 7, 5, 0), undefined);
  assert.equal(imports.Jc(url, method, body, 9, 0, 5, 0, 0), 0);
  assert.deepEqual(seen, [], 'no callback has landed yet — the transfer has not resolved');
  assert.deepEqual(heap.allocations, [], 'and nothing was allocated for bytes that do not exist');
  assert.deepEqual(report, [], 'a Promise is not a defect for a non-blocking row');
  // Nothing is queued *yet*: the resolver answered a Promise, so both rows attached their
  // continuation and returned, and `Qa`/`Jc` do not await the network before returning. The
  // completions are queued by that continuation, which is a later turn.
  assert.equal(queue.tasks.length, 0, 'nothing is queued while the network is still pending');

  resolveBytes(bytesOf('later'));
  // Settling the transfer queues both completions on a fresh turn, so the queue has to be observed
  // after that turn rather than drained straight away — the tasks do not exist yet.
  await new Promise((r) => setImmediate(r));
  assert.equal(queue.tasks.length, 2, 'one queued completion per row once the transfer resolved');
  await queue.drain();

  assert.equal(seen.length, 2, 'one callback per row once the transfer resolved');
  assert.deepEqual(
    seen.map((s) => s.entry).sort(),
    [5, 5],
    'both onload entries, not the error entries',
  );
  assert.equal(seen.find((s) => s.signature === 'viii').args[0], 7, 'Qa onload got its arg');
  assert.equal(seen.find((s) => s.signature === 'viiii').args[0], 0, 'Jc onload got its handle');
  assert.deepEqual(report, []);
});

// --- exact callback arity ----------------------------------------------------------------------

test('Qa: onload is viii [arg, buffer, length]; a freeing host releases the buffer after it', async () => {
  const heap = makeHeap();
  const seen = [];
  const freed = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: (pointer) => {
      freed.push(pointer);
      heap.free(pointer);
    },
    resolve: () => bytesOf('abcd'),
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => {
      seen.push({ signature, entry, args });
      // Read the buffer the way the guest's callback would, before any free.
      const [arg, buffer, length] = args;
      seen[seen.length - 1].read = [...heap.u8.subarray(buffer, buffer + length)];
      seen[seen.length - 1].arg = arg;
    },
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.equal(imports.Qa(url, 7, 5, 0), undefined);
  // Non-blocking: the import returned first, and the callback still has not happened.
  assert.deepEqual(seen, [], 'the callback is not delivered inside the import body');

  await queue.drain();
  assert.equal(seen.length, 1);
  assert.equal(seen[0].signature, 'viii', 'onload is the glue\'s getDynCaller("viii", onload)');
  assert.equal(seen[0].entry, 5);
  assert.equal(seen[0].args.length, 3, 'exactly three arguments, no status pair');
  assert.equal(seen[0].arg, 7);
  assert.deepEqual(seen[0].read, [...bytesOf('abcd')]);
  assert.deepEqual(freed, [256], 'freed after the callback, as the glue\'s trailing _free does');
});

test('Qa: onerror is vi [arg] — one argument, not the load arity', async () => {
  const heap = makeHeap();
  const seen = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => null,
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.equal(imports.Qa(url, 7, 5, 6), undefined);
  await queue.drain();
  assert.equal(seen.length, 1);
  assert.equal(seen[0].signature, 'vi', 'onerror is the glue\'s getDynCaller("vi", onerror)');
  assert.equal(seen[0].entry, 6);
  assert.deepEqual(seen[0].args, [7], 'exactly the arg, nothing else');
});

test('Jc: onload is viiii [handle, arg, buffer, length] and the handle is returned first', async () => {
  const heap = makeHeap();
  const seen = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => bytesOf('wxyz'),
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => {
      seen.push({ signature, entry, args });
      seen[seen.length - 1].read = [...heap.u8.subarray(args[2], args[2] + args[3])];
    },
  });
  const url = heap.writeString('https://example.test/a.bin');
  const method = heap.writeString('GET', 2048);
  const body = heap.writeString('', 3072);

  const handle = imports.Jc(url, method, body, 9, 0, 5, 0, 0);
  assert.equal(handle, 0, 'the first handle from the glue\'s counter');
  // The load-bearing ordering: the handle is the call's answer, produced *before* the transfer is
  // even resolved. A guest may abort a request whose bytes have not landed.
  assert.deepEqual(seen, [], 'the callback has not landed at the moment the handle is returned');

  await queue.drain();
  assert.equal(seen.length, 1);
  assert.equal(seen[0].signature, 'viiii');
  assert.equal(seen[0].entry, 5);
  assert.equal(seen[0].args.length, 4, 'exactly handle, arg, buffer, length');
  assert.equal(seen[0].args[0], handle, 'the handle is the one this call returned');
  assert.equal(seen[0].args[1], 9);
  assert.deepEqual(seen[0].read, [...bytesOf('wxyz')]);

  // Handles are per call and monotonic, as Browser.getNextWgetRequestHandle is.
  const second = imports.Jc(url, method, body, 9, 0, 5, 0, 0);
  assert.equal(second, 1, 'the next handle, taken before the first transfer resolved');
  await queue.drain();
});

test('Jc: onerror is viiii [handle, arg, status, statusText] with the string coerced to 0', async () => {
  const heap = makeHeap();
  const seen = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => null,
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
  });
  const url = heap.writeString('https://example.test/a.bin');
  const method = heap.writeString('GET', 2048);
  const body = heap.writeString('', 3072);

  const handle = imports.Jc(url, method, body, 9, 0, 0, 6, 0);
  await queue.drain();
  assert.equal(seen.length, 1);
  assert.equal(seen[0].signature, 'viiii');
  assert.equal(seen[0].entry, 6);
  assert.equal(seen[0].args.length, 4, 'the error arity is the same viiii, different meaning');
  assert.equal(seen[0].args[0], handle);

  // The official glue passes `http.statusText` — a string — into a wasm i32 slot, so V8 coerces it
  // to 0. Passing a pointer here instead would write a value the real build never produces.
  const officialStatusText = 'Not Found';
  assert.equal(officialStatusText | 0, 0, 'the coercion the guest actually observes');
  assert.equal(seen[0].args[3], 0, 'statusText slot is the coerced 0, never an invented pointer');
});

test('Jc: onprogress is viiii [handle, arg, loaded, total] and fires before onload', async () => {
  const heap = makeHeap();
  const order = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => bytesOf('123456'),
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => {
      order.push({ entry, args, signature });
    },
  });
  const url = heap.writeString('https://example.test/a.bin');
  const method = heap.writeString('GET', 2048);
  const body = heap.writeString('', 3072);

  const handle = imports.Jc(url, method, body, 9, 0, 5, 0, 4);
  await queue.drain();
  assert.equal(order.length, 2);
  assert.equal(order[0].entry, 4, 'progress first, as the XHR event order has it');
  assert.deepEqual(order[0].args, [handle, 9, 6, 6]);
  assert.equal(order[1].entry, 5);
});

test('Jc: a suspending onprogress is followed by onload only after it settles', async () => {
  // The glue gets this ordering from the event loop. Here it has to be enforced: an `onprogress`
  // that awaits must finish before `onload` is entered, or the load reads a buffer the progress
  // handler is still walking.
  const heap = makeHeap();
  const order = [];
  let releaseProgress;
  const progressGate = new Promise((r) => (releaseProgress = r));
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => bytesOf('123456'),
    enqueue: queue.enqueue,
    invokeCallback: async (signature, entry, args) => {
      order.push(entry);
      if (entry === 4) await progressGate;
    },
  });
  const url = heap.writeString('https://example.test/a.bin');
  const method = heap.writeString('GET', 2048);
  const body = heap.writeString('', 3072);

  imports.Jc(url, method, body, 9, 0, 5, 0, 4);
  // `drain` awaits each task, and this row's task awaits the progress handler — which is the
  // property under test. Awaiting the drain *before* releasing the gate would therefore wait for a
  // gate that is only released after it: a deadlock of the harness, not of the host. Start the
  // drain, let it enter the handler, and hold the only promise that is still outstanding.
  const draining = queue.drain();
  await new Promise((r) => setImmediate(r));
  assert.deepEqual(order, [4], 'onload waits: progress started and is still running');

  releaseProgress();
  await draining;
  assert.deepEqual(order, [4, 5], 'onload only after the progress handler settled');
});

test('the legacy callback seam still works when invokeCallback is absent', async () => {
  const heap = makeHeap();
  const calls = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => bytesOf('ab'),
    enqueue: queue.enqueue,
    // The old seam: no signature, six positional arguments.
    callback: (entry, arg, buffer, length, status, statusText) =>
      calls.push({ entry, arg, buffer, length, status, statusText }),
  });
  const url = heap.writeString('https://example.test/a.bin');

  imports.Qa(url, 7, 5, 0);
  await queue.drain();
  assert.equal(calls.length, 1);
  assert.equal(calls[0].entry, 5);
  assert.equal(calls[0].arg, 7);
  assert.equal(calls[0].length, 2);
  assert.deepEqual([...heap.u8.subarray(calls[0].buffer, calls[0].buffer + 2)], [...bytesOf('ab')]);
});

test('a callback whose entry is 0 is simply not called, as the glue\'s `if (onload)` guard', async () => {
  const heap = makeHeap();
  const seen = [];
  const freed = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: (pointer) => freed.push(pointer),
    resolve: () => bytesOf('ab'),
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.equal(imports.Qa(url, 7, 0, 0), undefined);
  await queue.drain();
  assert.deepEqual(seen, [], 'no entry, no invocation');
  assert.equal(freed.length, 1, 'the buffer is still released — nothing is leaked');
});

// --- a suspending callback ----------------------------------------------------------------------

test('Qa: a deferred callback holds the buffer until it settles, then frees it', async () => {
  const heap = makeHeap();
  let release;
  const gate = new Promise((r) => (release = r));
  let readAtCallback = null;
  const freed = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: (pointer) => freed.push(pointer),
    resolve: () => bytesOf('held'),
    enqueue: queue.enqueue,
    invokeCallback: async (signature, entry, args) => {
      // The callback itself suspends: the buffer must still be alive here, and after the await.
      readAtCallback = [...heap.u8.subarray(args[1], args[1] + args[2])];
      await gate;
    },
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.equal(imports.Qa(url, 7, 5, 0), undefined);
  // Same shape as the Jc ordering test above: the queued task does not finish until the callback
  // does, so the gate is released *after* the drain has entered the handler and this test holds the
  // drain's own promise rather than awaiting it into a deadlock.
  const draining = queue.drain();
  await new Promise((r) => setImmediate(r));
  assert.deepEqual(readAtCallback, [...bytesOf('held')]);
  assert.deepEqual(freed, [], 'not freed while the callback is still running');

  release();
  await draining;
  assert.deepEqual(freed, [256], 'released once the callback genuinely completed');
});

test('Qa: a callback that throws is reported, and the buffer is still released', async () => {
  const heap = makeHeap();
  const report = [];
  const freed = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: (pointer) => freed.push(pointer),
    resolve: () => bytesOf('ab'),
    enqueue: queue.enqueue,
    invokeCallback: () => {
      throw new Error('guest callback bug');
    },
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const url = heap.writeString('https://example.test/a.bin');

  // Guest code failing is not a host defect: it must not escape as a throw out of the import.
  assert.equal(imports.Qa(url, 7, 5, 0), undefined);
  await queue.drain();
  assert.equal(report.length, 1);
  assert.match(report[0][1], /callback 5 threw: Error: guest callback bug/);
  assert.deepEqual(freed, [256], 'a throwing callback must not leak the buffer');
});

test('Qa: a rejecting callback is reported rather than left unhandled', async () => {
  const heap = makeHeap();
  const report = [];
  const orphaned = [];
  const onUnhandled = (reason) => orphaned.push(reason);
  process.on('unhandledRejection', onUnhandled);
  try {
    const imports = createArtemisAsyncifyImports({
      memory: asMemory(heap),
      malloc: heap.malloc,
      free: heap.free,
      resolve: () => bytesOf('ab'),
      invokeCallback: () => Promise.reject(new Error('async callback bug')),
      onBlocked: (what, detail) => report.push([what, detail]),
    });
    const url = heap.writeString('https://example.test/a.bin');

    assert.equal(imports.Qa(url, 7, 5, 0), undefined);
    await sleep(30);
    process.off('unhandledRejection', onUnhandled);
    assert.deepEqual(orphaned, []);
    assert.equal(report.length, 1);
    assert.match(report[0][1], /callback 5 rejected: Error: async callback bug/);
  } finally {
    process.off('unhandledRejection', onUnhandled);
  }
});

test('isDisposed suppresses a callback that is delivered after teardown', async () => {
  const heap = makeHeap();
  const seen = [];
  let disposed = false;
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => bytesOf('ab'),
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
    isDisposed: () => disposed,
  });
  const url = heap.writeString('https://example.test/a.bin');

  disposed = true;
  assert.equal(imports.Qa(url, 7, 5, 0), undefined);
  await queue.drain();
  assert.deepEqual(seen, [], 'a disposed host does not re-enter the guest');
});

test('isDisposed is re-checked after a Promise wait, before any allocation or callback', async () => {
  // The window that matters: the resolver's promise is the await, and the guest can be torn down
  // while the transfer is in flight. Everything with a side effect — allocate, callback, free — must
  // be skipped, not merely un-delivered, so nothing is allocated for a guest that is gone.
  const heap = makeHeap();
  const seen = [];
  const freed = [];
  let disposed = false;
  let resolveBytes;
  const pending = new Promise((r) => (resolveBytes = r));
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: (bytes) => heap.malloc(bytes),
    free: (pointer) => {
      freed.push(pointer);
      heap.free(pointer);
    },
    resolve: () => pending,
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
    isDisposed: () => disposed,
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.equal(imports.Qa(url, 7, 5, 0), undefined);
  assert.deepEqual(heap.allocations, [], 'nothing allocated before the bytes exist');
  assert.deepEqual(seen, [], 'and no callback before the transfer resolves');

  disposed = true;
  resolveBytes(bytesOf('ab'));
  // The completion that was queued consumed the promise while it was still pending, so the disposal
  // has to be observed on the *continuation* — after the settle, not at enqueue time.
  await tick();
  await queue.drain();

  assert.deepEqual(heap.allocations, [], 'a disposed host allocates nothing after the wait');
  assert.deepEqual(seen, [], 'and enters no callback');
  assert.deepEqual(freed, [], 'and releases nothing, since nothing was taken');
});

// --- memory growth ------------------------------------------------------------------------------

test('fa: a resolver that grows the heap during the await still writes through a live view', async () => {
  // The hazard: the pointers were decoded and `memory.u8` was read *before* the await, and a grow
  // in between detaches that view. A host that cached HEAP32 at construction would write the out-
  // pointers into a detached buffer — silently, since typed-array writes past a detached buffer
  // throw nowhere — and the guest would read back zeros.
  const world = makeSuspendWorld();
  const { driver, sleepImport } = makeSuspendingGuest(world);
  let stale = world.heap.i32;
  let grew = false;
  const imports = createArtemisAsyncifyImports({
    memory: {
      get u8() {
        return world.heap.u8;
      },
      get i32() {
        return world.heap.i32;
      },
    },
    malloc: world.heap.malloc,
    free: world.heap.free,
    sleep: sleepImport,
    resolve: () => {
      return sleep(5).then(() => {
        // Grow while the guest is suspended, so the views read before the await are now stale.
        stale = world.heap.i32;
        world.heap.growTo(131072);
        grew = true;
        return bytesOf('grown');
      });
    },
  });

  const url = world.heap.writeString('https://example.test/a.bin');
  const pbuffer = 16;
  const pnum = 20;
  const perror = 24;
  const root = driver.wrap('root', () => {
    imports.fa(url, pbuffer, pnum, perror);
    return 0;
  });

  assert.equal(root(), 0);
  await bounded(driver.waitUntilIdle());
  await sleep(10);

  assert.equal(grew, true, 'the heap really did grow mid-transfer');
  assert.notEqual(stale, world.heap.i32, 'and the pre-await view really is a different object');
  assert.equal(world.heap.i32[perror >>> 2], 0);
  assert.equal(world.heap.i32[pnum >>> 2], 5);
  const buffer = world.heap.i32[pbuffer >>> 2] >>> 0;
  assert.deepEqual([...world.heap.u8.subarray(buffer, buffer + 5)], [...bytesOf('grown')]);
  assert.equal(driver.state, 0);
});

// --- the memcpy spill ---------------------------------------------------------------------------

test('yb: copies exactly num bytes from src, and does not extend the range past dest', () => {
  const heap = makeHeap();
  const imports = createArtemisAsyncifyImports({ memory: asMemory(heap) });
  heap.u8.fill(0);
  for (let i = 0; i < 8; i += 1) heap.u8[100 + i] = i + 1;

  // dest sits *past* src+num. The old `Math.max(dest, src+num)` form would copy up to dest and
  // smear untouched bytes over the destination; the glue's `(src+num)>>>0` copies exactly 4.
  assert.equal(imports.yb(200, 100, 4), 0);
  assert.deepEqual([...heap.u8.subarray(200, 208)], [1, 2, 3, 4, 0, 0, 0, 0]);

  // The region between the source's end and the destination is untouched, which is the assertion
  // that actually distinguishes the two implementations.
  heap.u8.fill(0);
  for (let i = 0; i < 8; i += 1) heap.u8[100 + i] = 0xaa;
  for (let i = 0; i < 8; i += 1) heap.u8[150 + i] = 0xbb;
  imports.yb(200, 100, 4);
  assert.deepEqual(
    [...heap.u8.subarray(150, 158)],
    [0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb, 0xbb],
    'bytes between src+num and dest are never read or written by the copy',
  );
  assert.deepEqual([...heap.u8.subarray(200, 204)], [0xaa, 0xaa, 0xaa, 0xaa]);
});

test('yb: a negative num copies nothing, matching the glue\'s wrap-then-clamp', () => {
  const heap = makeHeap();
  const imports = createArtemisAsyncifyImports({ memory: asMemory(heap) });
  heap.u8.fill(0);
  heap.u8[100] = 7;
  heap.u8[200] = 9;

  assert.equal(imports.yb(200, 100, -4), 0);
  assert.equal(heap.u8[200], 9, 'dest is untouched');
  assert.equal(heap.u8[100], 7, 'src is untouched');
});

// --- the real driver: one fetch, one allocation -------------------------------------------------

/**
 * The whole point of putting the fetch inside the driver's thunk: a rewind replays the raw export
 * from the top, so anything outside the thunk runs twice. This drives the *real* `ArtemisSuspendDriver`
 * — start/stop unwind, rewind to the same root — and counts.
 */
function makeSuspendWorld() {
  const heap = makeHeap(65536);
  const calls = [];
  let next = 4096;
  const driverMemory = {
    malloc: (bytes) => {
      const pointer = next;
      next += bytes;
      calls.push(['malloc', pointer, bytes]);
      return pointer;
    },
    free: (pointer) => calls.push(['free', pointer]),
    heap32: () => heap.i32,
  };
  const control = {
    startUnwind: (data) => calls.push(['startUnwind', data]),
    stopUnwind: () => calls.push(['stopUnwind']),
    startRewind: (data) => calls.push(['startRewind', data]),
    stopRewind: () => calls.push(['stopRewind']),
  };
  const count = (name) => calls.filter((c) => c[0] === name).length;
  return { heap, calls, driverMemory, control, count };
}

/** A driver-backed `sleep` seam, plus a root call that really reaches the download rows. */
function makeSuspendingGuest(suspendWorld, rowName) {
  const driver = new ArtemisSuspendDriver({
    control: suspendWorld.control,
    memory: suspendWorld.driverMemory,
    stackSize: 256,
  });
  // The guest's `sleep` import, exactly as a session would wire it.
  const sleepImport = (thunk) => driver.sleep(thunk);
  return { driver, sleepImport };
}

test('fa under the real driver: one fetch, one allocation, unwound then rewound to the same root', async () => {
  const world = makeSuspendWorld();
  const { driver, sleepImport } = makeSuspendingGuest(world);

  let fetches = 0;
  const allocated = [];
  const imports = createArtemisAsyncifyImports({
    memory: {
      get u8() {
        return world.heap.u8;
      },
      get i32() {
        return world.heap.i32;
      },
    },
    malloc: (bytes) => {
      allocated.push(bytes);
      return world.heap.malloc(bytes);
    },
    free: world.heap.free,
    sleep: sleepImport,
    // Deliberately deferred, so the guest genuinely suspends rather than completing inline.
    resolve: () => {
      fetches += 1;
      return sleep(5).then(() => bytesOf('payload'));
    },
  });

  const url = world.heap.writeString('https://example.test/a.bin');
  const pbuffer = 16;
  const pnum = 20;
  const perror = 24;

  // The guest's own export, wrapped so the driver can unwind it. It reaches `fa`, then has to be
  // replayed from the top — which is the replay that would double-fetch if the fetch were outside
  // the thunk.
  let passes = 0;
  const raw = () => {
    passes += 1;
    imports.fa(url, pbuffer, pnum, perror);
    // A pass-distinguishing return: the unwind pass hands back the driver's placeholder 0, and the
    // replayed pass returns whatever the transfer actually left in `perror`.
    return passes === 1 ? 0x5a : perror;
  };
  const root = driver.wrap('root', raw);

  // The unwind pass returns the driver's own placeholder, *not* the guest's return: the root is
  // mid-flight with a promise outstanding, and only the replay can finish it. The 0x5a the raw
  // export computed is discarded by the driver, which is why the replay (not this pass) is what
  // establishes the call's real result.
  assert.equal(root(), 0);
  assert.equal(driver.state, 0, 'the unwind was stopped at the boundary');
  assert.equal(driver.busy, true, 'a promise is outstanding, so the driver is still busy');
  assert.equal(world.count('startUnwind'), 1);

  await bounded(driver.waitUntilIdle());
  await sleep(10);

  assert.equal(driver.state, 0);
  assert.equal(driver.busy, false);
  assert.equal(driver.inGuestDepth, 0);
  for (const fn of ['startUnwind', 'stopUnwind', 'startRewind', 'stopRewind']) {
    assert.equal(world.count(fn), 1, `${fn} called once`);
  }

  // The heart of it: the guest ran twice, but the work behind the download happened once.
  assert.equal(passes, 2, 'the root really was replayed from the top');
  assert.equal(fetches, 1, 'the fetch is inside the thunk, so the rewind does not re-issue it');
  assert.deepEqual(allocated, [7], 'one allocation of the payload length, not two');
  assert.equal(driver.result, perror, 'the replayed pass produced the final root value');

  // And the value written during that single pass is what the guest ends up with.
  assert.equal(world.heap.i32[perror >>> 2], 0);
  assert.equal(world.heap.i32[pnum >>> 2], 7);
  const buffer = world.heap.i32[pbuffer >>> 2] >>> 0;
  assert.deepEqual([...world.heap.u8.subarray(buffer, buffer + 7)], [...bytesOf('payload')]);
});

test('Qa under the real driver: the guest never unwinds, the root runs once, the callback is queued', async () => {
  // The corrected contract. `Qa` is non-blocking, so the raw root returns of its own accord: no
  // unwind, no rewind, no replay. The fetch is issued once, from inside the import, and the callback
  // is delivered by an enqueued task — which a real session would park until the driver is idle.
  const world = makeSuspendWorld();
  const { driver, sleepImport } = makeSuspendingGuest(world);

  let fetches = 0;
  const allocated = [];
  const delivered = [];
  const freed = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: {
      get u8() {
        return world.heap.u8;
      },
      get i32() {
        return world.heap.i32;
      },
    },
    malloc: (bytes) => {
      allocated.push(bytes);
      return world.heap.malloc(bytes);
    },
    free: (pointer) => {
      freed.push(pointer);
      world.heap.free(pointer);
    },
    // Installed, and deliberately never reached by this row: a `Qa` that suspends would unwind a
    // guest that was meant to keep running. Its absence from the control call counts below proves it.
    sleep: sleepImport,
    // Deferred on purpose: this is the case the old implementation got wrong.
    resolve: () => {
      fetches += 1;
      return sleep(5).then(() => bytesOf('cb'));
    },
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => {
      delivered.push({ signature, entry, args, read: [...world.heap.u8.subarray(args[1], args[1] + args[2])] });
    },
  });

  const url = world.heap.writeString('https://example.test/a.bin');
  let passes = 0;
  const root = driver.wrap('root', () => {
    passes += 1;
    imports.Qa(url, 7, 5, 0);
    return 0;
  });

  // The raw root returns its own value — not the driver's placeholder — because nothing parked it.
  assert.equal(root(), 0);
  assert.equal(passes, 1, 'the root ran exactly once');
  assert.equal(driver.busy, false, 'nothing is outstanding: the guest was never unwound');
  assert.equal(driver.state, 0);
  assert.equal(driver.inGuestDepth, 0);
  assert.equal(fetches, 1, 'the fetch was issued once, from the import body');
  assert.deepEqual(allocated, [], 'the allocation belongs to the deferred task, which has not run');
  assert.deepEqual(delivered, [], 'and no callback has been entered');
  // Nothing is queued *yet*, and that is the contract rather than a detail: only a **settled**
  // answer is enqueued, so a queue that waits for the driver cannot be parked behind a network
  // operation. Queueing the wait itself is the deadlock — the completion would sit behind a fetch
  // whose own turn cannot run until the root that issued it is gone.
  assert.equal(queue.tasks.length, 0, 'the pending network wait is not itself queued');

  // No unwind machinery was touched at all: this is the assertion the wrong implementation fails,
  // because suspending `Qa` would have called startUnwind here.
  for (const fn of ['startUnwind', 'stopUnwind', 'startRewind', 'stopRewind']) {
    assert.equal(world.count(fn), 0, `${fn} was never called: Qa does not suspend the guest`);
  }

  // The transfer lands, and *now* exactly one effect is queued — one task per call, not per turn.
  await until(() => queue.tasks.length === 1, 2000);
  assert.deepEqual(allocated, [], 'still nothing allocated: the task has not been drained');
  assert.equal(fetches, 1, 'and the settlement did not re-issue the fetch');

  // Now let the queued completion run, as a driver-idle-aware enqueue would.
  await queue.drain();
  await bounded(driver.waitUntilIdle());
  await sleep(10);

  assert.equal(passes, 1, 'the root is never replayed: one pass, start to finish');
  assert.equal(fetches, 1, 'and the fetch is not re-issued');
  assert.deepEqual(allocated, [2], 'the completion allocated exactly once');
  assert.equal(delivered.length, 1, 'the callback is invoked once');
  assert.equal(delivered[0].signature, 'viii');
  assert.deepEqual(delivered[0].read, [...bytesOf('cb')]);
  assert.equal(freed.length, 1, 'and the buffer was released exactly once');
  assert.equal(driver.state, 0);
  assert.equal(driver.busy, false);
});

test('Jc under the real driver: the handle is the call\'s answer, the callback is queued after the root', async () => {
  const world = makeSuspendWorld();
  const { driver, sleepImport } = makeSuspendingGuest(world);

  let fetches = 0;
  const delivered = [];
  const allocated = [];
  const queue = makeTaskQueue();
  const imports = createArtemisAsyncifyImports({
    memory: {
      get u8() {
        return world.heap.u8;
      },
      get i32() {
        return world.heap.i32;
      },
    },
    malloc: (bytes) => {
      allocated.push(bytes);
      return world.heap.malloc(bytes);
    },
    free: world.heap.free,
    sleep: sleepImport,
    resolve: () => {
      fetches += 1;
      return sleep(5).then(() => bytesOf('jc'));
    },
    enqueue: queue.enqueue,
    invokeCallback: (signature, entry, args) => delivered.push({ signature, entry, args }),
  });

  const url = world.heap.writeString('https://example.test/a.bin');
  const method = world.heap.writeString('GET', 2048);
  const body = world.heap.writeString('', 3072);
  let handle = null;
  const root = driver.wrap('root', () => {
    handle = imports.Jc(url, method, body, 9, 0, 5, 0, 0);
    return handle;
  });

  assert.equal(root(), 0);
  // `driver.result` is what the *root* actually produced, and it has to be the handle. The old test
  // asserted the callback's copy of the handle and never looked here, so an implementation that
  // returned a different value — or parked the root and returned the driver's placeholder — stayed
  // green on a false comparison.
  assert.equal(handle, 0, 'the handle was returned by the call');
  assert.equal(driver.result, 0, 'and it is what the root exported, not a placeholder');
  assert.equal(driver.busy, false);
  assert.equal(driver.state, 0);
  assert.equal(fetches, 1);
  assert.deepEqual(delivered, [], 'the callback has not landed yet');
  assert.equal(queue.tasks.length, 0, 'the pending network wait is not itself queued');
  for (const fn of ['startUnwind', 'stopUnwind', 'startRewind', 'stopRewind']) {
    assert.equal(world.count(fn), 0, `${fn} was never called: Jc does not suspend the guest`);
  }

  // Only the *settled* completion is queued, so the effect appears once the bytes exist.
  await until(() => queue.tasks.length === 1, 2000);
  assert.deepEqual(allocated, [], 'still nothing allocated until the task is drained');

  await queue.drain();
  await bounded(driver.waitUntilIdle());
  await sleep(10);

  assert.equal(delivered.length, 1);
  assert.equal(delivered[0].signature, 'viiii');
  assert.equal(delivered[0].args.length, 4, 'handle, arg, buffer, length');
  assert.equal(delivered[0].args[0], handle, 'the delivered handle is the one the call returned');
  assert.equal(delivered[0].args[1], 9, 'the arg is carried through');
  assert.deepEqual(allocated, [2], 'exactly one allocation, in the deferred task');
  assert.equal(driver.state, 0);
  assert.equal(driver.busy, false);
});

test('Qa under the real driver: the completion is queued while a real driver root is still parked', async (t) => {
  // The ordering that motivates the seam, and the *guest's own control flow* around it.
  //
  // The sequence below is what a real Asyncify guest compiles to, and it is the part a naive test
  // gets wrong. `Qa` is non-blocking, so a real guest calls it **once**: from its one normal entry
  // into the function. `fa` blocks, so the guest calls it and the transformed code then does
  // `if (Asyncify.state === Unwinding) return;` — the early return that lets the wasm stack spill.
  // The rewind replays the same export from the top, and this time the guard sends it straight to
  // the *resume* label: the `fa` sleep is answered from the driver's own recorded slot and the
  // earlier side effect — `Qa` — is not reached a second time.
  //
  // Re-running `Qa` on the rewind pass is the defect a fake guest hides. It re-issues a fetch the
  // guest issued once and queues a second completion for a callback the guest never asked for
  // twice, and because nothing in the assertion *counts* completions it still looks green: one call
  // is attributed to `fa`, the extra one to nobody.
  //
  // The deferred `Qa` and `fa` settlements are distinct on purpose. Settling `Qa` while the root is
  // parked is the window under test, and it has to be reachable without settling `fa` as well —
  // with one shared gate the two cannot be told apart, and an implementation that ran the Qa
  // completion at the wrong moment would still pass.
  const world = makeSuspendWorld();
  const { driver, sleepImport } = makeSuspendingGuest(world);

  const order = [];
  const observed = [];
  const allocated = [];
  let enqueued = 0;
  const queue = makeTaskQueue();
  const faWait = pendingWait();
  const qaWait = pendingWait();
  // A failing assertion must not leave a gate outstanding: the after-hook below reads an unsettled
  // wait as an orphaned promise, and a test's own failure is not a defect in the host. Releasing is
  // idempotent, so this is a no-op once the test has settled them itself.
  t.after(() => {
    qaWait.resolve();
    faWait.resolve();
  });

  const imports = createArtemisAsyncifyImports({
    memory: {
      get u8() {
        return world.heap.u8;
      },
      get i32() {
        return world.heap.i32;
      },
    },
    malloc: (bytes) => {
      allocated.push(bytes);
      return world.heap.malloc(bytes);
    },
    free: world.heap.free,
    sleep: sleepImport,
    resolve: (request) => {
      // Deferred and *distinct*: `fa`'s fetch parks the guest, and the nested `Qa` fetch settles
      // first, so the completion under test runs with the root still parked.
      if (request.url.endsWith('fa.bin')) {
        order.push('resolve:fa');
        return faWait.promise.then(() => bytesOf('fa-payload'));
      }
      order.push('resolve:qa');
      return qaWait.promise.then(() => bytesOf('qa-payload'));
    },
    // A driver-aware enqueue: the completion waits for the driver to be idle before running, which is
    // the whole reason this cannot be a hard-coded microtask. Mixing a raw `queueMicrotask` here
    // would deliver the callback while the guest is unwound — the state this test pins down. The
    // count is kept because "handed to the queue" and "run by the queue" are two different moments
    // here, and the requirement is about the first: the completion must be *offered* the moment its
    // bytes exist, and *held* until the driver is idle.
    enqueue: (task) => {
      enqueued += 1;
      return driver.waitUntilIdle().then(() => {
        queue.enqueue(task);
      });
    },
    invokeCallback: (signature, entry, args) => {
      observed.push({ signature, entry, args, busy: driver.busy, state: driver.state });
    },
  });

  const faUrl = world.heap.writeString('https://example.test/fa.bin', 128);
  const qaUrl = world.heap.writeString('https://example.test/qa.bin');
  const pbuffer = 16;
  const pnum = 20;
  const perror = 24;

  // The guest's export, with the transformed shape: one normal entry, an early return on the way
  // out, and a rewind that resumes at `fa` rather than re-running the side effect above it.
  //
  // The replay is recognised by the driver's own state, which is what a compiled guest reads as
  // `Asyncify.state`: `0` Normal is the one real entry, `1` Unwinding is the spill on the way out,
  // and `2` Rewinding is the replay. `Qa` sits above the resume label, so it is reached on the
  // Normal pass only; the replay falls straight through to `fa`, whose `sleep` answers from the
  // driver's recorded slot without calling the thunk again.
  let passes = 0;
  const guest = () => {
    passes += 1;
    // The transformed `if (Asyncify.state === Unwinding) return;` on the way out: the spill is
    // already done and the frame must not run on past the blocking call.
    if (driver.state === 1) return 0;
    if (driver.state === 0) {
      // Normal pass only: the non-blocking row the guest issues exactly once.
      imports.Qa(qaUrl, 7, 5, 6);
      if (driver.state !== 0) return 0;
    }
    // The resume point: reached by fallthrough on the normal pass and jumped to by the rewind.
    imports.fa(faUrl, pbuffer, pnum, perror);
    if (driver.state !== 0) return 0;
    return 1;
  };
  const root = driver.wrap('root', guest);

  assert.equal(root(), 0, 'the first pass unwinds at the blocking row');
  assert.equal(passes, 1, 'one pass so far: the guest has not been replayed');
  assert.equal(order.filter((step) => step === 'resolve:qa').length, 1, 'Qa fetched once');
  assert.equal(order.filter((step) => step === 'resolve:fa').length, 1, 'fa fetched once too');
  // The parked signal after `root()` returns is `busy`, not `state`: the unwind pass leaves the
  // driver back in `Normal` (`stopUnwind` has run, the stack is spilled) while a Promise is still
  // outstanding, so `state` alone would call this "finished". `busy` is what says a replay is due.
  assert.equal(driver.busy, true, 'the driver is parked on the blocking row');
  assert.equal(driver.state, 0, 'unwound and stopped: Normal again, with a replay still owed');
  assert.equal(observed.length, 0, 'nothing guest-visible has happened yet');
  assert.deepEqual(allocated, [], 'Qa allocated nothing before its bytes existed');

  // Settle Qa FIRST, while the root is still parked on fa. This is the window that matters, and an
  // already-rejected answer is handed over deliberately: a host that subscribed eagerly would have
  // had the rejection delivered with nobody listening, so this is also the "held behind the queue"
  // case rather than a second test for it.
  qaWait.reject(new Error('qa transfer aborted'));

  // The completion is *offered* now — the rejection was observed and turned into an effect without a
  // listener gap — but the effect is held behind the driver's idle, so the guest is untouched.
  await until(() => enqueued === 1, 2000);
  assert.equal(enqueued, 1, 'the settled completion was handed to the queue while the root was parked');

  // THE assertions. The completion has been offered and has *not* run: the root is still parked, so
  // nothing may have been allocated and no callback may have been entered.
  assert.equal(driver.busy, true, 'the root is still parked on the blocking row');
  assert.equal(driver.state, 0, 'and still owed a replay: the rewind has not happened yet');
  assert.equal(queue.tasks.length, 0, 'the completion is held: the driver is not idle yet');
  assert.equal(observed.length, 0, 'no callback is entered while the guest is unwound');
  assert.deepEqual(allocated, [], 'and nothing was allocated into the parked guest');

  // The outer wait is deliberately left pending until now: it resolves on the rewind, which is the
  // very thing being waited for.
  faWait.resolve();

  await bounded(driver.waitUntilIdle());
  // The held completion lands now, on the driver's idle — once, and only now.
  await until(() => queue.tasks.length > 0, 2000);
  const ran = await queue.drain();
  assert.equal(ran, 1, 'exactly one task ran: the single Qa completion');
  await sleep(10);

  assert.equal(passes, 2, 'the root really was replayed from the top');
  assert.equal(
    order.filter((step) => step === 'resolve:qa').length,
    1,
    'the replayed pass resumed at fa: the Qa side effect was not re-run',
  );
  assert.equal(order.filter((step) => step === 'resolve:fa').length, 1, 'and fa was not re-issued');
  assert.equal(driver.busy, false);
  assert.equal(driver.state, 0);
  // `invokeCallback` is shared by both rows, so anything `fa` reports lands here too; the completion
  // being proved is specifically `Qa`'s, and there is exactly one across the whole run.
  const qaCalls = observed.filter((call) => call.signature === 'vi' || call.signature === 'viii');
  assert.equal(observed.length, 1, `only Qa's completion is guest-visible, saw ${JSON.stringify(observed)}`);
  assert.equal(qaCalls.length, 1, 'the queued completion ran once, after the driver went idle');
  // A rejected transfer is the guest-visible *error*, and this guest passed a real `onerror` entry.
  assert.equal(observed[0].entry, 6, 'Qa onerror, the entry this guest passed');
  assert.equal(observed[0].signature, 'vi', 'the error arity, not the load arity');
  assert.equal(observed[0].busy, false, 'delivered only once no root was parked');
  assert.equal(observed[0].state, 0);
  assert.deepEqual(observed[0].args, [7], 'the error arity carries the arg and nothing else');
  // And the blocking row still did its own job, in the same run.
  assert.equal(world.heap.i32[perror >>> 2], 0, 'fa succeeded');
  assert.equal(world.heap.i32[pnum >>> 2], 10);
  assert.deepEqual(allocated, [10], 'exactly one allocation, and only after the replay finished');
});

test('fa under the real driver: a rejected transfer writes perror 1 and resumes once', async (t) => {
  const world = makeSuspendWorld();
  const { driver, sleepImport } = makeSuspendingGuest(world);

  let fetches = 0;
  const allocated = [];
  const gate = pendingWait();
  const imports = createArtemisAsyncifyImports({
    memory: {
      get u8() {
        return world.heap.u8;
      },
      get i32() {
        return world.heap.i32;
      },
    },
    malloc: (bytes) => {
      allocated.push(bytes);
      return world.heap.malloc(bytes);
    },
    free: world.heap.free,
    sleep: sleepImport,
    // The fetch is issued, the guest parks on it, and only then does it reject. `sleep(5)` would make
    // that ordering a race; a hand-held gate makes "still parked when the rejection lands" a fact the
    // test controls rather than hopes for.
    resolve: () => {
      fetches += 1;
      return gate.promise;
    },
  });
  // A failing assertion must not leave the gate outstanding, and the release is idempotent.
  t.after(() => gate.resolve());

  const url = world.heap.writeString('https://example.test/a.bin');
  const pbuffer = 16;
  const pnum = 20;
  const perror = 24;

  // The guest's exported control flow, matching what Asyncify really compiles to: the unwind pass
  // hands back the driver's placeholder 0, and the replay starts from the top, reaches `fa` again —
  // which answers from the driver's own recorded slot, with no second fetch — and returns the call's
  // real result.
  let passes = 0;
  const raw = () => {
    passes += 1;
    imports.fa(url, pbuffer, pnum, perror);
    if (driver.state !== 0) return 0;
    return 1;
  };
  const root = driver.wrap('root', raw);

  assert.equal(driver.state, 0, 'the guest starts in Normal, so this is the one real entry');
  assert.equal(root(), 0, 'the unwind pass returns the driver placeholder, not the guest return');
  assert.equal(passes, 1);
  assert.equal(fetches, 1, 'the fetch was issued before the guest parked');
  assert.equal(driver.busy, true, 'the transfer is outstanding, so the guest is parked');

  // The rejection lands now, with the guest genuinely unwound — the ordering the old throwing arm got
  // wrong: it failed the root instead of reporting a failed download to the guest.
  gate.reject(new Error('transfer aborted'));

  await bounded(driver.waitUntilIdle());
  await sleep(10);

  // The replay reaches `fa` again and the import answers from the driver's recorded slot: one fetch,
  // one parking, and a failure reported the way upstream reports it.
  assert.equal(passes, 2, 'the root really was unwound once and rewound once');
  assert.equal(fetches, 1, 'the replayed pass did not re-issue the fetch');
  assert.equal(world.heap.i32[perror >>> 2], 1, 'the failure is perror 1, the contract a guest checks');
  assert.equal(driver.busy, false);
  assert.equal(driver.state, 0);
  // A failure is not a transfer: nothing may be allocated, and the out-pointers must stay untouched
  // rather than carry a stale buffer the guest would free.
  assert.deepEqual(allocated, [], 'a failed transfer allocates nothing');
  assert.equal(world.heap.i32[pbuffer >>> 2], 0, 'no buffer was written');
  assert.equal(world.heap.i32[pnum >>> 2], 0, 'and no length was written');
});

test('fa without a driver: the synchronous path is unchanged and needs no seam', () => {
  const heap = makeHeap();
  const report = [];
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => bytesOf('sync'),
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const url = heap.writeString('https://example.test/a.bin');

  // No `sleep` at all: a resolver answering with bytes must still work exactly as before.
  assert.equal(imports.fa(url, 16, 20, 24), undefined);
  assert.equal(heap.i32[24 >>> 2], 0);
  assert.equal(heap.i32[20 >>> 2], 4);
  const buffer = heap.i32[16 >>> 2] >>> 0;
  assert.deepEqual([...heap.u8.subarray(buffer, buffer + 4)], [...bytesOf('sync')]);
  assert.deepEqual(report, []);
});

test('Qa: without an enqueue seam the default is a microtask, still after the import returns', async () => {
  const heap = makeHeap();
  const seen = [];
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => bytesOf('ab'),
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.equal(imports.Qa(url, 7, 5, 0), undefined);
  assert.deepEqual(seen, [], 'not delivered inside the import body, even synchronously resolved');
  await tick();
  assert.equal(seen.length, 1, 'delivered on the following microtask');
  assert.equal(seen[0].signature, 'viii');
});

test('an enqueue that throws is reported, and the import still returns normally', () => {
  const heap = makeHeap();
  const report = [];
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    resolve: () => bytesOf('ab'),
    enqueue: () => {
      throw new Error('queue is gone');
    },
    invokeCallback: () => undefined,
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const url = heap.writeString('https://example.test/a.bin');

  // A broken seam is a host defect to report, not an exception out of a guest import.
  assert.equal(imports.Qa(url, 7, 5, 0), undefined);
  assert.equal(report.length, 1);
  assert.match(report[0][1], /enqueue threw: Error: queue is gone/);

  const method = heap.writeString('GET', 2048);
  const body = heap.writeString('', 3072);
  assert.equal(imports.Jc(url, method, body, 9, 0, 5, 0, 0), 0, 'the handle is still returned');
});

test('Qa: a rejected fetch reports the failure and calls onerror, with no unhandled rejection', async () => {
  const heap = makeHeap();
  const report = [];
  const seen = [];
  const orphaned = [];
  const onUnhandled = (reason) => orphaned.push(reason);
  process.on('unhandledRejection', onUnhandled);
  try {
    const imports = createArtemisAsyncifyImports({
      memory: asMemory(heap),
      malloc: heap.malloc,
      free: heap.free,
      resolve: () => Promise.reject(new Error('transfer aborted')),
      invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
      onBlocked: (what, detail) => report.push([what, detail]),
    });
    const url = heap.writeString('https://example.test/a.bin');

    assert.equal(imports.Qa(url, 7, 5, 6), undefined);
    await sleep(20);
    process.off('unhandledRejection', onUnhandled);

    assert.deepEqual(orphaned, [], 'the rejection is handled, not orphaned');
    assert.equal(seen.length, 1, 'a failed transfer is still guest-visible');
    assert.equal(seen[0].signature, 'vi');
    assert.equal(seen[0].entry, 6, 'onerror, not onload');
    assert.equal(report.length, 1);
    // Normalized: the assertion is that the reason is carried through, not how Node stringifies a
    // rejected `Error`.
    assert.match(report[0][1], /resolver rejected: /);
    assert.match(report[0][1], /transfer aborted/);
  } finally {
    process.off('unhandledRejection', onUnhandled);
  }
});

test('Qa: a Promise resolving to a non-Uint8Array is a reported defect, not a heap corruption', async () => {
  const heap = makeHeap();
  const report = [];
  const seen = [];
  const imports = createArtemisAsyncifyImports({
    memory: asMemory(heap),
    malloc: heap.malloc,
    free: heap.free,
    // Typed as the right thing, resolving to the wrong one.
    resolve: () => Promise.resolve('not bytes'),
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const url = heap.writeString('https://example.test/a.bin');

  assert.equal(imports.Qa(url, 7, 5, 6), undefined);
  await tick();
  await tick();
  assert.deepEqual(heap.allocations, [], 'nothing was copied into the heap');
  assert.equal(seen.length, 1);
  assert.equal(seen[0].entry, 6, 'it takes the failure path');
  assert.equal(report.length, 1);
  assert.match(report[0][1], /expected Uint8Array, null, or a Promise/);
});

test('the four import names are present and the official map is untouched', async () => {
  const heap = makeHeap();
  const imports = createArtemisAsyncifyImports({ memory: asMemory(heap) });
  assert.deepEqual(
    Object.keys(imports).sort(),
    ['Jc', 'Qa', 'fa', 'yb'],
    'the bucket is exactly these four rows',
  );
  // `fa`'s shape is the asyncify contract: 4 arguments and a void return.
  assert.equal(imports.fa.length, 4);
});

/**
 * The rows borrow three things from the world — `memory.u8`, `memory.i32` and, when the world's heap
 * is the real one, `memory.refresh()`. `makeHeap` above grows by swapping its `ArrayBuffer`, so its
 * accessors rebuild their views on demand and can never notice a refresh that did not happen. A real
 * `WebAssembly.Memory` cannot: growing it *detaches* the old buffer, and every view taken before the
 * grow reads `undefined` for the rest of its life. That is the difference these tests exist to
 * measure, and it is why they use the real module rather than the fast mock.
 */
test('Qa: a real heap that grows during malloc is still readable when the bytes are copied', async () => {
  const { heap, wasm } = await makeWasmHeap();
  const report = [];
  const seen = [];
  const bytes = bytesOf('grown heap bytes');

  // The allocator grows the heap the way a real one does when it runs out of room: the memory gets
  // bigger, and the view the host captured before the call is detached in the process.
  let grew = false;
  const malloc = (byteLength) => {
    const pointer = 4096;
    if (!grew) {
      grew = true;
      wasm.grow(1);
    }
    assert.ok(pointer + byteLength <= wasm.buffer.byteLength, 'the allocation fits after the grow');
    return pointer;
  };

  const imports = createArtemisAsyncifyImports({
    memory: heap,
    // The one call that makes a stale view visible as a bug. Without it the copy below writes into a
    // detached buffer — which throws nothing and changes nothing — and the test reads zeroes back.
    refresh: () => heap.refresh(),
    malloc,
    free: () => {},
    resolve: () => bytes,
    invokeCallback: (signature, entry, args) => seen.push({ signature, entry, args }),
    onBlocked: (what, detail) => report.push([what, detail]),
  });

  const url = 2048;
  // Enough of a string that a failed copy is unmissable; the row only hands the pointer onward.
  const encoded = new TextEncoder().encode('https://example.test/a.bin\0');
  new Uint8Array(wasm.buffer).set(encoded, url);

  assert.equal(imports.Qa(url, 7, 5, 6), undefined);
  await tick();
  await tick();

  assert.equal(grew, true, 'the allocator really did grow the heap mid-call');
  assert.equal(report.length, 0, 'nothing was refused or reported');
  assert.equal(seen.length, 1);
  assert.equal(seen[0].entry, 5, 'onload: the transfer succeeded');
  // `viii` is [arg, buffer, length]: the arg is first, and the buffer is the second slot.
  const [pointer, count] = [seen[0].args[1], seen[0].args[2]];
  assert.equal(count, bytes.byteLength);
  assert.notEqual(pointer, 0, 'a successful load hands back a buffer the guest can read');

  // The whole point: the *current* heap — not the one that was current before the grow — holds the
  // bytes. A host that copied through a stale view fails here with zeroes.
  const landed = new Uint8Array(wasm.buffer, pointer, count);
  assert.deepEqual(Array.from(landed), Array.from(bytesOf('grown heap bytes')));
});

test('fa under a real heap: the replayed entry writes its out-pointers into the current heap', async (t) => {
  const { heap, wasm } = await makeWasmHeap();
  const bytes = bytesOf('replayed bytes'), gate = pendingWait();
  const reserved = [], freed = [], allocated = [], report = [];
  let passes = 0, fetches = 0, grew = false;
  const driver = new ArtemisSuspendDriver({
    control: { startUnwind() {}, stopUnwind() {}, startRewind() {}, stopRewind() {} },
    memory: {
      malloc: length => { assert.equal(reserved.length, 0); reserved.push([4096, length]); return 4096; },
      free: pointer => freed.push(pointer),
      heap32: () => { heap.refresh(); return heap.i32; },
    },
    stackSize: 256,
  });
  t.after(() => { gate.resolve(bytes); driver.dispose('test cleanup'); });
  const stale = heap.i32;
  const imports = createArtemisAsyncifyImports({
    memory: heap, refresh: () => heap.refresh(),
    malloc: length => {
      assert.equal(driver.busy, true, 'the network continuation allocates while the root is parked');
      assert.equal(reserved.length, 1, 'the driver control block was reserved before growth');
      assert.equal(passes, 1, 'fa commits bytes before the driver resumes its second pass');
      assert.equal(grew, false); grew = true; wasm.grow(1);
      assert.ok(8192 + length <= wasm.buffer.byteLength);
      allocated.push([8192, length]); return 8192;
    },
    free: () => {}, sleep: thunk => driver.sleep(thunk),
    resolve: request => { fetches++; assert.equal(request.url, 'https://example.test/a.bin'); return gate.promise; },
    onBlocked: (what, detail) => report.push([what, detail]),
  });
  const pbuffer = 1024, pnum = 1040, perror = 1044, url = 2048;
  new TextEncoder().encodeInto('https://example.test/a.bin\0', new Uint8Array(wasm.buffer, url));
  heap.i32[pbuffer >> 2] = 0x11; heap.i32[pnum >> 2] = 0x22; heap.i32[perror >> 2] = 0x33;
  const root = driver.wrap('root', () => {
    passes++; imports.fa(url, pbuffer, pnum, perror);
    return driver.state === 0 ? 1 : 0;
  });
  assert.equal(root(), 0); assert.equal(driver.busy, true);
  assert.deepEqual([passes, fetches, driver.state], [1, 1, 0]);
  assert.deepEqual([heap.i32[pbuffer >> 2], heap.i32[pnum >> 2], heap.i32[perror >> 2]], [0x11, 0x22, 0x33]);
  assert.deepEqual(allocated, []);
  gate.resolve(bytes); await bounded(driver.waitUntilIdle());
  assert.deepEqual([passes, fetches, driver.state, driver.busy, driver.result], [2, 1, 0, false, 1]);
  assert.equal(grew, true); assert.equal(stale.byteLength, 0, 'the real pre-grow heap view was detached');
  assert.equal(wasm.buffer.byteLength, 131072);
  assert.deepEqual([heap.i32[pbuffer >> 2], heap.i32[pnum >> 2], heap.i32[perror >> 2]], [8192, bytes.byteLength, 0]);
  assert.deepEqual(new Uint8Array(wasm.buffer, 8192, bytes.byteLength), bytes);
  assert.deepEqual(allocated, [[8192, bytes.byteLength]]); assert.deepEqual(reserved, [[4096, 268]]);
  assert.deepEqual(freed, [4096]); assert.deepEqual(report, []);
});

test('fa: a late replayed entry writes nothing once the host is disposed', async (t) => {
  const { heap, wasm } = await makeWasmHeap();
  const gate = pendingWait(), bytes = bytesOf('too late'), allocated = [], freed = [], report = [];
  let disposed = false, passes = 0, fetches = 0;
  const driver = new ArtemisSuspendDriver({
    control: { startUnwind() {}, stopUnwind() {}, startRewind() {}, stopRewind() {} },
    memory: { malloc: () => 4096, free: pointer => freed.push(pointer), heap32: () => { heap.refresh(); return heap.i32; } },
    stackSize: 256,
  });
  t.after(() => { gate.resolve(bytes); driver.dispose('test cleanup'); });
  const imports = createArtemisAsyncifyImports({
    memory: heap, refresh: () => heap.refresh(),
    malloc: length => { allocated.push(length); return 8192; }, free: () => {},
    sleep: thunk => driver.sleep(thunk), resolve: () => { fetches++; return gate.promise; },
    isDisposed: () => disposed, onBlocked: (what, detail) => report.push([what, detail]),
  });
  const pbuffer = 1024, pnum = 1040, perror = 1044, url = 2048;
  new TextEncoder().encodeInto('https://example.test/a.bin\0', new Uint8Array(wasm.buffer, url));
  heap.i32[pbuffer >> 2] = 0x11; heap.i32[pnum >> 2] = 0x22; heap.i32[perror >> 2] = 0x33;
  const root = driver.wrap('root', () => {
    passes++; imports.fa(url, pbuffer, pnum, perror);
    return driver.state === 0 ? 1 : 0;
  });
  assert.equal(root(), 0); assert.equal(driver.busy, true);
  assert.deepEqual([passes, fetches], [1, 1]);
  assert.deepEqual([heap.i32[pbuffer >> 2], heap.i32[pnum >> 2], heap.i32[perror >> 2]], [0x11, 0x22, 0x33]);
  // HOST disposal and DRIVER disposal are distinct: keep the real driver alive for a healthy rewind.
  disposed = true; gate.resolve(bytes); await bounded(driver.waitUntilIdle());
  assert.deepEqual([passes, fetches, driver.state, driver.busy, driver.disposed], [2, 1, 0, false, false]);
  assert.deepEqual(allocated, [], 'the host guard runs before any guest allocation');
  assert.deepEqual([heap.i32[pbuffer >> 2], heap.i32[pnum >> 2], heap.i32[perror >> 2]], [0x11, 0x22, 0x33]);
  assert.deepEqual(freed, [4096], 'the driver still releases its own control block once');
  assert.deepEqual(report, [], 'dropping an answer after host disposal is intentionally silent');
});
