/**
 * Independent challenge for `CompatReadHost` — the page half of the blocking read window.
 *
 * No worker is started here. The test owns the shared control words directly, so it can place a
 * request's sequence and epoch in any state a real worker could produce — including the states a
 * worker only reaches after a timeout, a restart, or an int32 wrap — and then assert what the host
 * did to the window. A fake `GameSource` counts its own calls, which is how "no source call" is
 * proven rather than assumed.
 *
 * The package's `read-host.ts` is compiled on its own into a temp tree: this file depends on no
 * `.test/` workbench and no built `dist/`, so a clean clone runs it. It is deliberately the single
 * entry point, so a change to `read-host.ts` that needs a sibling module to compile fails here.
 *
 * Run: `node --test test/artemis-compat-read-host.test.mjs`
 */
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const temp = mkdtempSync(join(tmpdir(), 'artemis-compat-read-host-'));
const out = join(temp, 'out');
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-compat-read-host-')) {
    rmSync(temp, { recursive: true, force: true });
  }
});
execFileSync(process.execPath, [
  join(root, 'node_modules/typescript/bin/tsc'),
  'packages/runtime-artemis/src/compat/read-host.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--noUncheckedIndexedAccess', '--skipLibCheck',
  '--rootDir', 'packages/runtime-artemis/src', '--outDir', out,
], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/runtime-artemis/node_modules'), join(out, 'node_modules'), 'junction');

const { CompatReadHost } = await import(pathToFileURL(join(out, 'compat', 'read-host.js')).href);
const { allocateCompatBlock, CONTROL, READ_FATAL, READ_MISSING } = await import(
  pathToFileURL(join(out, 'compat', 'protocol.js')).href
);

/* --------------------------------------------------------------------------------------------- */
/* A deterministic environment: real shared words, a fake source that counts its own calls.       */
/* --------------------------------------------------------------------------------------------- */

const WINDOW_BYTES = 64;
const INT32_MAX = 0x7fffffff;
const U32_MAX = 0xffffffff;

/** A `GameSource` stand-in. `readRange` records every call and can answer with a chosen shape. */
class FakeSource {
  calls = [];
  /** What `readRange` resolves to. A function receives `(path, offset, length)`. */
  answer;
  constructor(answer = (_path, _offset, length) => new Uint8Array(length)) {
    this.answer = answer;
  }
  static of(size = 4096) {
    return new FakeSource((_path, offset, length) => {
      const bytes = new Uint8Array(length);
      // Distinct per-offset bytes, so a window holding the wrong range is visible, not just a size.
      for (let index = 0; index < length; index += 1) bytes[index] = (offset + index) & 0xff;
      return bytes;
    });
  }
  async readRange(path, offset, length) {
    this.calls.push({ path, offset, length });
    const answer = this.answer;
    if (typeof answer === 'function') return answer(path, offset, length);
    return answer;
  }
}

/** A descriptor table the host is expected to accept: `/game/root.pfs` first, then a continuation. */
function descriptors(extra = []) {
  return [
    { id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 4096 },
    { id: 2, path: 'root.pfs.001', virtualPath: '/game/root.pfs.001', size: 2048 },
    ...extra,
  ];
}

/**
 * A host plus a hand-driven worker side.
 *
 * `post` is what the worker does before it waits: publish the sequence, then hand the request to the
 * host. Nothing here waits; the test reads the control words afterwards.
 */
function bench({ source = FakeSource.of(), table = descriptors(), windowBytes = WINDOW_BYTES } = {}) {
  const block = allocateCompatBlock(windowBytes);
  const host = new CompatReadHost(source, block, table);
  const store = (index, value) => Atomics.store(block.control, index, value);
  const load = (index) => Atomics.load(block.control, index) | 0;
  const publish = (sequence, epoch) => {
    store(CONTROL.SEQUENCE, sequence | 0);
    store(CONTROL.EPOCH, epoch | 0);
    store(CONTROL.COUNT, 0);
  };
  return { block, host, source, store, load, publish };
}

/** The request shape the worker posts, with any field overridable so a bad one can be built. */
function request(over = {}) {
  return { type: 'read', fileId: 1, offset: 0, length: 16, sequence: 1, epoch: 1, ...over };
}

/* --------------------------------------------------------------------------------------------- */
/* Raw length: the number that arrived, not a coerced one.                                        */
/* --------------------------------------------------------------------------------------------- */

test('a fractional length is refused instead of being truncated into a valid one', async () => {
  const { host, source, publish, load } = bench();
  publish(1, 1);
  // `1.5 >>> 0` is 1: the old shape served a one-byte read and reported success for a request that
  // asked for one and a half bytes.
  assert.equal(await host.answer(request({ length: 1.5 })), READ_FATAL);
  assert.equal(source.calls.length, 0, 'a malformed length must not reach the source');
  assert.equal(load(CONTROL.COUNT), READ_FATAL);
  assert.match(host.lastReason ?? '', /length/);
});

test('a raw length past 2^32 is refused instead of wrapping to a small one', async () => {
  const { host, source, publish, load } = bench();
  publish(1, 1);
  // `2**32 + 1 >>> 0` is 1, and `2**32 >>> 0` is 0: both would slip past a `>>> 0` gate.
  for (const length of [2 ** 32, 2 ** 32 + 1, 2 ** 32 + 16]) {
    assert.equal(await host.answer(request({ length })), READ_FATAL, `length ${length}`);
  }
  assert.equal(source.calls.length, 0);
  assert.equal(load(CONTROL.COUNT), READ_FATAL);
});

test('a negative, zero, NaN or non-numeric length is refused', async () => {
  const { host, source, publish } = bench();
  publish(1, 1);
  for (const length of [-1, -0, 0, NaN, 16.000000001, '16', null, undefined, true]) {
    assert.equal(await host.answer(request({ length })), READ_FATAL, `length ${String(length)}`);
  }
  assert.equal(source.calls.length, 0);
});

test('a legitimate length larger than 2 GiB but inside a 32-bit field is accepted', async () => {
  // 2.69 GB is a real pack read on a 64-bit engine, and it is a legal `u32`. The window is what caps
  // it, never the field: a host that refused this would be refusing a real read.
  const HUGE = 2_690_000_000;
  const source = new FakeSource((_path, _offset, length) => new Uint8Array(Math.min(length, 1)));
  const block = allocateCompatBlock(WINDOW_BYTES);
  const host = new CompatReadHost(source, block, [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 8_000_000_000 }]);
  Atomics.store(block.control, CONTROL.SEQUENCE, 1);
  Atomics.store(block.control, CONTROL.EPOCH, 1);
  // The window is 64 bytes, so this is a *window* refusal — a different reason from a malformed length.
  assert.equal(await host.answer(request({ length: HUGE })), READ_FATAL);
  assert.match(host.lastReason ?? '', /window/);
  assert.equal(source.calls.length, 0, 'the window is checked before the source is asked');
});

test('a length that crosses the 32-bit offset ceiling is refused by the size subtraction', async () => {
  // A pack whose size sits just below 2^32. The offset is a legal `u32` and inside the file, but
  // `offset + length` leaves the 32-bit range: a summed check on wrapped words sees a small number
  // and lets the read through, while the subtraction form `length > size - offset` sees the real end.
  const source = new FakeSource((_path, _offset, length) => new Uint8Array(length));
  const block = allocateCompatBlock(WINDOW_BYTES);
  const size = U32_MAX - 7;
  const host = new CompatReadHost(source, block, [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size }]);
  Atomics.store(block.control, CONTROL.SEQUENCE, 1);
  Atomics.store(block.control, CONTROL.EPOCH, 1);
  const offset = U32_MAX - 15;
  const length = 16;
  assert.ok(offset + length > U32_MAX, 'the exact sum really did leave the 32-bit range');
  assert.ok((offset + length) >>> 0 < length, 'a wrapped sum would read as a small, passing one');
  assert.ok(offset < size && offset + length > size, 'the request straddles the end of the descriptor');
  assert.equal(await host.answer(request({ offset, length })), READ_FATAL);
  assert.match(host.lastReason ?? '', /past end/);
  assert.equal(source.calls.length, 0);
});

test('an offset past 2 GiB inside the file still reads, and the requested bytes land', async () => {
  const { source, publish, load, block } = bench();
  const offset = 3 * 1024 * 1024 * 1024;
  const size = offset + 4096;
  const big = new CompatReadHost(source, block, [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size }]);
  publish(1, 1);
  assert.equal(await big.answer(request({ offset, length: 16 })), 16);
  assert.deepEqual(source.calls, [{ path: 'root.pfs', offset, length: 16 }]);
  assert.deepEqual([...block.payload.subarray(0, 16)], Array.from({ length: 16 }, (_, i) => (offset + i) & 0xff));
  assert.equal(load(CONTROL.COUNT), 16);
  assert.equal(load(CONTROL.SEQUENCE), 2);
});

test('a file id or offset that is not a plain integer is refused', async () => {
  const { host, source, publish } = bench();
  // Each request gets its own live publish: `answer` advances the sequence on the way out, so a
  // second request against the first one's sequence would be refused as stale for the wrong reason.
  const ask = (over) => {
    publish(1, 1);
    return host.answer(request(over));
  };
  assert.equal(await ask({ fileId: 1.5 }), READ_FATAL);
  assert.equal(await ask({ fileId: 0 }), READ_FATAL);
  assert.equal(await ask({ fileId: -1 }), READ_FATAL);
  assert.equal(await ask({ fileId: 99 }), READ_MISSING, 'a legal but unknown id is still MISSING');
  assert.equal(await ask({ offset: -1 }), READ_FATAL);
  assert.equal(await ask({ offset: 1.5 }), READ_FATAL);
  assert.equal(await ask({ offset: 2 ** 32 }), READ_FATAL);
  assert.equal(source.calls.length, 0);
});

/* --------------------------------------------------------------------------------------------- */
/* Descriptor boundaries.                                                                         */
/* --------------------------------------------------------------------------------------------- */

test('a duplicate descriptor id is refused at construction, not silently kept last', async () => {
  const source = FakeSource.of();
  const block = allocateCompatBlock(WINDOW_BYTES);
  const table = [
    { id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 4096 },
    { id: 1, path: 'other.pfs', virtualPath: '/game/other.pfs', size: 4096 },
  ];
  assert.throws(() => new CompatReadHost(source, block, table), /declared twice/);
  const samePath = [
    { id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 4096 },
    { id: 2, path: 'other.pfs', virtualPath: '/game/root.pfs', size: 4096 },
  ];
  assert.throws(() => new CompatReadHost(source, block, samePath), /virtual path/);
});

test('descriptor ids and paths outside the declared shape are refused', () => {
  const block = allocateCompatBlock(WINDOW_BYTES);
  const source = FakeSource.of();
  const bad = [
    [{ id: 0, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 16 }, /not positive/],
    [{ id: -1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 16 }, /not positive/],
    [{ id: 1.5, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 16 }, /not positive/],
    [{ id: INT32_MAX + 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 16 }, /not positive/],
    [{ id: 1, path: '/root.pfs', virtualPath: '/game/root.pfs', size: 16 }, /path \/root\.pfs is absolute/],
    [{ id: 1, path: 'C:/root.pfs', virtualPath: '/game/root.pfs', size: 16 }, /carries a drive letter/],
    [{ id: 1, path: 'a\\b.pfs', virtualPath: '/game/a\\b.pfs', size: 16 }, /contains a backslash/],
    [{ id: 1, path: 'a/../b.pfs', virtualPath: '/game/a/../b.pfs', size: 16 }, /not a plain path/],
    [{ id: 1, path: 'a//b.pfs', virtualPath: '/game/a//b.pfs', size: 16 }, /not a plain path/],
    [{ id: 1, path: 'a\0b.pfs', virtualPath: '/game/a\0b.pfs', size: 16 }, /contains a NUL/],
    [{ id: 1, path: 'root.pfs', virtualPath: 'root.pfs', size: 16 }, /outside \/game/],
    [{ id: 1, path: 'root.pfs', virtualPath: '/other/root.pfs', size: 16 }, /outside \/game/],
    [{ id: 1, path: 'root.pfs', virtualPath: '/game/../etc/passwd', size: 16 }, /not a plain path/],
    [{ id: 1, path: 'root.pfs', virtualPath: '/game//root.pfs', size: 16 }, /not a plain path/],
    [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: -1 }, /byte count/],
    [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 1.5 }, /byte count/],
    [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: Infinity }, /byte count/],
    [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: Number.MAX_SAFE_INTEGER + 1 }, /byte count/],
    [{ id: 1, path: '', virtualPath: '/game/root.pfs', size: 16 }, /empty path/],
    [{ id: 1, path: 'root.pfs', virtualPath: '', size: 16 }, /empty virtualPath/],
  ];
  for (const [descriptor, expected] of bad) {
    assert.throws(() => new CompatReadHost(source, block, [descriptor]), expected, JSON.stringify(descriptor));
  }
});

test('the descriptors the runtime actually builds are accepted, including the plain root', () => {
  const block = allocateCompatBlock(WINDOW_BYTES);
  // `describeGame` emits `/game/root.pfs` first and `/game/<name>` after — these literals must pass.
  assert.doesNotThrow(() => new CompatReadHost(FakeSource.of(), block, descriptors()));
  assert.doesNotThrow(() =>
    new CompatReadHost(FakeSource.of(), block, [{ id: 1, path: 'root.pfs', virtualPath: '/game', size: 4096 }]),
  );
  // A pack chain is several GB, so a size past 4 GiB is a real pack, not a malformed field.
  assert.doesNotThrow(() =>
    new CompatReadHost(FakeSource.of(), block, [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 8_000_000_000 }]),
  );
});

test('a read past the descriptor size is refused without asking the source', async () => {
  const { host, source, publish } = bench();
  // Descriptor 1 is 4096 B and a read of 16 must start at or below 4080 to fit; descriptor 2 is 2048 B.
  publish(1, 1);
  assert.equal(await host.answer(request({ offset: 4081, length: 16 })), READ_FATAL);
  assert.match(host.lastReason ?? '', /past end/);
  publish(1, 1);
  assert.equal(await host.answer(request({ fileId: 2, offset: 2048, length: 1 })), READ_FATAL);
  publish(1, 1);
  assert.equal(await host.answer(request({ offset: 4096, length: 1 })), READ_FATAL, 'a read starting at EOF is past the end');
  assert.equal(source.calls.length, 0);
});

/* --------------------------------------------------------------------------------------------- */
/* Sequence and epoch: a late answer is never published over a live one.                          */
/* --------------------------------------------------------------------------------------------- */

test('a request whose sequence is not the published one is refused with no source call', async () => {
  const { host, source, publish, load } = bench();
  publish(7, 1);
  // A late duplicate of an already-answered read: its reader has moved on to sequence 7.
  assert.equal(await host.answer(request({ sequence: 6, epoch: 1 })), READ_FATAL);
  assert.equal(source.calls.length, 0);
  assert.equal(load(CONTROL.SEQUENCE), 7, 'the live sequence is untouched');
  assert.equal(load(CONTROL.COUNT), 0);
  assert.match(host.lastReason ?? '', /stale read sequence/);
});

test('a stale request does not clear the read that is actually in flight', async () => {
  const { host, source, publish, load } = bench();
  publish(7, 1);
  let release;
  const gate = new Promise((settle) => {
    release = settle;
  });
  source.answer = async (_path, _offset, length) => {
    await gate;
    return new Uint8Array(length);
  };
  const live = host.answer(request({ sequence: 7, epoch: 1, length: 8 }));
  // While sequence 7 is genuinely in flight, an unrelated late request arrives with another sequence.
  assert.equal(await host.answer(request({ sequence: 3, epoch: 1 })), READ_FATAL);
  assert.equal(source.calls.length, 1, 'only the live read reached the source');
  assert.equal(load(CONTROL.SEQUENCE), 7, 'the live read is still the one the worker waits on');
  release();
  assert.equal(await live, 8, 'the live read still publishes after the stale one was refused');
  assert.equal(load(CONTROL.COUNT), 8);
  assert.equal(load(CONTROL.SEQUENCE), 8);
});

test('a request from a stale epoch does not read the source or publish', async () => {
  const { host, source, publish, load } = bench();
  // The worker restarted: it bumped the epoch and republished the same sequence number.
  publish(3, 5);
  assert.equal(await host.answer(request({ sequence: 3, epoch: 4 })), READ_FATAL);
  assert.equal(source.calls.length, 0);
  assert.equal(load(CONTROL.COUNT), 0);
  assert.match(host.lastReason ?? '', /stale read epoch/);
});

test('a request with no epoch field is served against the live epoch', async () => {
  const { host, publish, load } = bench();
  publish(1, 4);
  const { epoch: _omitted, ...bare } = request({ sequence: 1 });
  assert.equal(await host.answer(bare), 16);
  assert.equal(load(CONTROL.COUNT), 16);
});

test('a completion that lands after the epoch moved is dropped, not written into the window', async () => {
  const { host, source, publish, block, load } = bench();
  publish(1, 1);
  // The worker times out while the source is still resolving: it bumps the epoch and dies.
  source.answer = async (_path, _offset, length) => {
    block.payload.fill(0xaa);
    publish(2, 2);
    return new Uint8Array(length).fill(0xff);
  };
  assert.equal(await host.answer(request()), READ_FATAL);
  assert.equal(load(CONTROL.COUNT), 0, 'a late answer never writes COUNT');
  assert.equal(load(CONTROL.SEQUENCE), 2, 'and never advances the sequence the worker abandoned');
  assert.deepEqual([...block.payload.subarray(0, 4)], [0xaa, 0xaa, 0xaa, 0xaa], 'the window is untouched');
  assert.match(host.lastReason ?? '', /superseded/);
});

test('a source that returns something other than a Uint8Array is refused, never repaired', async () => {
  const { host, source, publish, load } = bench();
  for (const wrong of [undefined, null, [], { byteLength: 16 }, 16, 'bytes', new ArrayBuffer(16)]) {
    publish(1, 1);
    source.answer = () => wrong;
    assert.equal(await host.answer(request()), READ_FATAL, String(wrong));
  }
  assert.equal(load(CONTROL.COUNT), READ_FATAL);
  assert.match(host.lastReason ?? '', /non-Uint8Array/);
});

test('a source that throws, or answers short, is a fatal read', async () => {
  const { host, source, publish, load } = bench();
  publish(1, 1);
  source.answer = () => {
    throw new Error('backend gone');
  };
  assert.equal(await host.answer(request()), READ_FATAL);
  assert.match(host.lastReason ?? '', /backend gone/);
  publish(1, 1);
  source.answer = () => new Uint8Array(4);
  assert.equal(await host.answer(request()), READ_FATAL);
  assert.match(host.lastReason ?? '', /short read 4\/16/);
  assert.equal(load(CONTROL.COUNT), READ_FATAL);
});

test('an unknown but otherwise legal file id is READ_MISSING, not fatal', async () => {
  const { host, source, publish, load } = bench();
  publish(1, 1);
  assert.equal(await host.answer(request({ fileId: 99 })), READ_MISSING);
  assert.equal(load(CONTROL.COUNT), READ_MISSING);
  assert.equal(load(CONTROL.SEQUENCE), 2, 'an expected absence still releases the waiter');
  assert.equal(source.calls.length, 0);
});

test('a source may return more bytes than asked; exactly the requested length is published', async () => {
  const { host, source, publish, block, load } = bench();
  publish(1, 1);
  source.answer = (_path, _offset, length) => new Uint8Array(length + 32).fill(0x5a);
  assert.equal(await host.answer(request({ length: 16 })), 16);
  assert.equal(load(CONTROL.COUNT), 16);
  assert.deepEqual([...block.payload.subarray(0, 16)], new Array(16).fill(0x5a));
  assert.equal(block.payload[16], 0, 'the extra bytes stay out of the window');
});

/* --------------------------------------------------------------------------------------------- */
/* Sequence arithmetic and the closed guard.                                                      */
/* --------------------------------------------------------------------------------------------- */

test('the sequence wraps through the int32 boundary and still matches what the worker published', async () => {
  // The worker computes `(load + 1) | 0`, so a wrap publishes a negative number and waits on it.
  const { host, source, publish, load } = bench();
  publish(INT32_MAX, 1);
  const next = (INT32_MAX + 1) | 0;
  assert.ok(next < 0, 'the published sequence really did wrap');
  source.answer = (_path, _offset, length) => new Uint8Array(length);
  assert.equal(await host.answer(request({ sequence: INT32_MAX })), 16);
  assert.equal(load(CONTROL.SEQUENCE), next, 'the release posts the wrapped word the worker wakes on');
  assert.equal(load(CONTROL.COUNT), 16);
});

test('a fractional sequence or epoch is refused and does not release the waiter', async () => {
  const { host, source, publish, load } = bench();
  const refused = async (postedSequence, postedEpoch, over) => {
    publish(postedSequence, postedEpoch);
    assert.equal(await host.answer(request(over)), READ_FATAL);
    assert.equal(load(CONTROL.SEQUENCE), postedSequence | 0);
    assert.equal(load(CONTROL.COUNT), 0);
  };
  await refused(1, 1, { sequence: 1.5 });
  await refused(1, 1, { epoch: 1.5 });
  await refused(0, 1, { sequence: NaN });
  await refused(INT32_MAX, 1, { sequence: -2147483649 });
  await refused(1, -2147483648, { epoch: 2147483648 });
  assert.equal(source.calls.length, 0);
});

test('the wrapped sequence INT32_MIN is a real request and is served', async () => {
  const { host, source, publish, load } = bench();
  const wrapped = -2147483648;
  publish(wrapped, 1);
  assert.equal(await host.answer(request({ sequence: wrapped })), 16);
  assert.equal(source.calls.length, 1);
  assert.equal(load(CONTROL.COUNT), 16);
  assert.equal(load(CONTROL.SEQUENCE), wrapped + 1);
});

test('a same-sequence wrong epoch while a read is in flight does not touch COUNT or the source', { timeout: 3000 }, async () => {
  const { host, source, publish, load, block } = bench();
  publish(7, 1);
  let release;
  const gate = new Promise((settle) => {
    release = settle;
  });
  source.answer = async (_path, _offset, length) => {
    await gate;
    return new Uint8Array(length);
  };
  const live = host.answer(request({ sequence: 7, epoch: 1, length: 8 }));
  await Promise.resolve();
  assert.equal(source.calls.length, 1);
  const countBefore = load(CONTROL.COUNT);
  assert.equal(await host.answer(request({ sequence: 7, epoch: 9, length: 8 })), READ_FATAL);
  assert.equal(source.calls.length, 1, 'the stale epoch must not start a second source read');
  assert.equal(load(CONTROL.COUNT), countBefore, 'COUNT stays untouched while the live read is in flight');
  assert.equal(Atomics.load(block.control, CONTROL.SEQUENCE), 7);
  release();
  assert.equal(await live, 8);
  assert.equal(load(CONTROL.COUNT), 8);
  assert.equal(load(CONTROL.SEQUENCE), 8);
});

test('a stale mismatch does not clear serving, so the next current request is refused', { timeout: 3000 }, async () => {
  const { host, source, publish, load } = bench();
  publish(4, 1);
  let release;
  const gate = new Promise((settle) => {
    release = settle;
  });
  source.answer = async (_path, _offset, length) => {
    await gate;
    return new Uint8Array(length);
  };
  const live = host.answer(request({ sequence: 4, epoch: 1, length: 8 }));
  await Promise.resolve();
  assert.equal(await host.answer(request({ sequence: 1, epoch: 1 })), READ_FATAL);
  publish(5, 1);
  assert.equal(await host.answer(request({ sequence: 5, epoch: 1, length: 4 })), READ_FATAL);
  assert.equal(source.calls.length, 1, 'the second current request must not reach the source');
  assert.equal(load(CONTROL.SEQUENCE), 5);
  assert.equal(load(CONTROL.COUNT), 0);
  release();
  assert.equal(await live, READ_FATAL, 'the original must not publish into the sequence that moved');
  assert.equal(source.calls.length, 1);
  assert.equal(load(CONTROL.COUNT), 0);
  assert.equal(load(CONTROL.SEQUENCE), 5);
});

test('a read that ends exactly at UINT32_MAX is valid; crossing it, even on an 8GB file, is not', async () => {
  const eight = 8_000_000_000;
  const source = new FakeSource((_path, offset, length) => {
    const bytes = new Uint8Array(length);
    for (let index = 0; index < length; index += 1) bytes[index] = (offset + index) & 0xff;
    return bytes;
  });
  const block = allocateCompatBlock(WINDOW_BYTES);
  const host = new CompatReadHost(source, block, [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: eight }]);
  Atomics.store(block.control, CONTROL.SEQUENCE, 1);
  Atomics.store(block.control, CONTROL.EPOCH, 1);
  assert.equal(await host.answer(request({ offset: U32_MAX, length: 1 })), 1);
  assert.equal(source.calls.length, 1);
  Atomics.store(block.control, CONTROL.SEQUENCE, 2);
  Atomics.store(block.control, CONTROL.EPOCH, 1);
  assert.equal(await host.answer(request({ offset: U32_MAX, length: 2, sequence: 2 })), READ_FATAL);
  assert.equal(source.calls.length, 1, 'a length that leaves the u32 address space must not call the source');
  assert.match(host.lastReason ?? '', /past end|address|32/);
});

test('a read asked after close is refused', async () => {
  const { host, source, publish } = bench();
  publish(1, 1);
  host.close();
  assert.equal(await host.answer(request()), READ_FATAL);
  assert.equal(source.calls.length, 0);
});

test('close refuses the read in flight and bumps the epoch so its answer cannot land', async () => {
  const { host, source, publish, block, load } = bench();
  publish(1, 1);
  source.answer = async (_path, _offset, length) => {
    host.close();
    return new Uint8Array(length).fill(0xff);
  };
  assert.equal(await host.answer(request()), READ_FATAL);
  assert.equal(load(CONTROL.EPOCH), 2);
  assert.deepEqual([...block.payload.subarray(0, 4)], [0, 0, 0, 0], 'a closed host never writes the window');
  assert.match(host.lastReason ?? '', /closed/);
});
