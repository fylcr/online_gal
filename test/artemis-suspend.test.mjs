import test from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, writeFileSync, existsSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const repoRoot = resolve(fileURLToPath(new URL('..', import.meta.url)));
const srcRoot = join(repoRoot, 'packages', 'engine', 'core', 'artemis-compat', 'src');
const temp = mkdtempSync(join(tmpdir(), 'artemis-suspend-'));
const out = join(temp, 'out');

const ArtemisSuspendDriver = await (async () => {
  execFileSync(
    process.execPath,
    [
      join(repoRoot, 'node_modules', 'typescript', 'bin', 'tsc'),
      join(srcRoot, 'host', 'suspend.ts'),
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
  const mod = await import(pathToFileURL(join(out, 'host', 'suspend.js')).href);
  return mod.ArtemisSuspendDriver;
})();

/** Fresh heap + recorder: counts malloc/free and logs the four guest control calls. */
function makeWorld() {
  const heap = new Int32Array(4096);
  const calls = [];
  let next = 256;
  const memory = {
    malloc: (bytes) => {
      const pointer = next;
      next += bytes;
      calls.push(['malloc', pointer, bytes]);
      return pointer;
    },
    free: (pointer) => calls.push(['free', pointer]),
    heap32: () => heap,
  };
  const control = {
    startUnwind: (data) => calls.push(['startUnwind', data]),
    stopUnwind: () => calls.push(['stopUnwind']),
    startRewind: (data) => calls.push(['startRewind', data]),
    stopRewind: () => calls.push(['stopRewind']),
  };
  const names = () => calls.map((c) => c[0]);
  const count = (name) => names().filter((n) => n === name).length;
  return { heap, calls, memory, control, count, counts: () => ({ malloc: count('malloc'), free: count('free') }) };
}

/** A control that throws on exactly one of the four calls; the rest record normally. */
function throwingControl(world, which) {
  const boom = () => {
    throw new Error(`${which} boom`);
  };
  return {
    startUnwind: which === 'startUnwind' ? boom : (data) => world.calls.push(['startUnwind', data]),
    stopUnwind: which === 'stopUnwind' ? boom : () => world.calls.push(['stopUnwind']),
    startRewind: which === 'startRewind' ? boom : (data) => world.calls.push(['startRewind', data]),
    stopRewind: which === 'stopRewind' ? boom : () => world.calls.push(['stopRewind']),
  };
}

const newDriver = (world, stackSize = 256) =>
  new ArtemisSuspendDriver({ control: world.control, memory: world.memory, stackSize });

/**
 * Bounded wait: races one promise against a 2000 ms deadline. No polling loop, no interval, and the
 * timer is always cleared, so a never-settling promise fails the test instead of hanging the suite.
 */
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

const sleep = (ms) => new Promise((r) => setTimeout(r, ms));

test.after(() => {
  const target = resolve(temp);
  assert.ok(target.startsWith(resolve(tmpdir())), `unexpected temp root: ${target}`);
  assert.ok(existsSync(target));
  assert.equal(basename(target), basename(temp), 'resolved path must be the fresh mkdtemp dir');
  assert.equal(basename(target).startsWith('artemis-suspend-'), true, `unexpected prefix: ${basename(target)}`);
  rmSync(target, { recursive: true, force: true });
});

test('sync root returns its value and drives no control call', () => {
  const world = makeWorld();
  const d = newDriver(world);
  assert.equal(d.wrap('root', () => 73)(), 73);
  assert.equal(d.result, 73);
  assert.equal(d.state, 0);
  assert.equal(d.busy, false);
  assert.equal(d.inGuestDepth, 0);
  assert.deepEqual(world.calls, []);
  assert.deepEqual(world.counts(), { malloc: 0, free: 0 });
});

test('sync root with an undefined return records a null result and drives no control call', async () => {
  const world = makeWorld();
  const d = newDriver(world);
  const root = d.wrap('root', () => {
    d.sleep(() => 0);
    return undefined;
  });
  const pending = root();
  assert.equal(pending, undefined);
  assert.equal(d.result, null);
  assert.equal(d.state, 0);
  assert.equal(d.busy, false);
  await bounded(d.waitUntilIdle());
  assert.deepEqual(world.calls, []);
  assert.deepEqual(world.counts(), { malloc: 0, free: 0 });
});

test('root() returns 0 at the unwind; the replayed pass settles it with state, this and args kept', async () => {
  const world = makeWorld();
  const d = newDriver(world);
  const receiver = { tag: 'receiver' };
  let seen = null;
  const root = d.wrap('root', function (...args) {
    seen = { args: [...args], thisArg: this };
    if (d.state === 1) return 0;
    const waited = d.sleep(() => sleep(5).then(() => 21));
    if (d.state === 1) return 0;
    return waited + 10;
  });

  // The root literally returns its placeholder 0 the moment it unwinds — it is not a promise, and
  // the real 31 only appears through the replayed pass.
  const immediate = root.apply(receiver, [1, 2, 3]);
  assert.equal(immediate, 0);
  assert.equal(d.state, 0);
  assert.equal(d.busy, true);
  assert.equal(d.inGuestDepth, 0);
  assert.equal(world.count('startUnwind'), 1);
  assert.equal(world.count('stopUnwind'), 1);

  await bounded(d.waitUntilIdle());
  await sleep(10);
  assert.equal(d.result, 31);
  assert.equal(d.state, 0);
  assert.equal(d.busy, false);
  assert.equal(d.inGuestDepth, 0);
  assert.deepEqual(seen.args, [1, 2, 3]);
  assert.equal(seen.thisArg, receiver);
  assert.deepEqual(world.counts(), { malloc: 1, free: 1 });
  for (const fn of ['startUnwind', 'stopUnwind', 'startRewind', 'stopRewind']) {
    assert.equal(world.count(fn), 1, `${fn} called once`);
  }
});

test('external re-entry while parked throws and leaves the driver on its parked root', async () => {
  const world = makeWorld();
  const d = newDriver(world);
  let release;
  const gate = new Promise((r) => (release = r));
  const root = d.wrap('root', () => {
    if (d.state === 1) return 0;
    const waited = d.sleep(() => gate.then(() => 5));
    if (d.state === 1) return 0;
    return waited + 1;
  });
  const other = d.wrap('other', () => 999);

  assert.equal(root(), 0);
  assert.equal(d.busy, true);
  assert.equal(d.inGuestDepth, 0);
  assert.equal(d.state, 0);

  assert.throws(() => other(), /unwound|already in flight/);
  assert.equal(d.busy, true);
  assert.equal(d.state, 0);
  assert.equal(d.inGuestDepth, 0);

  const waiter = d.waitUntilIdle();
  release();
  await bounded(waiter);
  await sleep(10);
  assert.equal(d.result, 6);
  assert.equal(d.state, 0);
  assert.equal(d.busy, false);
  assert.equal(d.inGuestDepth, 0);
  assert.deepEqual(world.counts(), { malloc: 1, free: 1 });
});

test('a second await reuses the block; the replayed pass sees the saved pc and first value', async () => {
  const world = makeWorld();
  const d = newDriver(world);
  let pc = 0;
  let first = 0;
  const root = d.wrap('root', () => {
    if (d.state === 1) return 0;
    if (pc === 0) {
      const waited = d.sleep(() => sleep(5).then(() => 7));
      if (d.state === 1) return 0;
      pc = 1;
      first = waited;
    }
    if (pc === 1) {
      const waited = d.sleep(() => sleep(5).then(() => 8));
      if (d.state === 1) return 0;
      return first + waited;
    }
    return 0;
  });

  // First pass parks on the first await and hands back the placeholder.
  assert.equal(root(), 0);
  assert.equal(world.counts().malloc, 1, 'one block reserved for the first await');
  await sleep(10);
  assert.equal(first, 7, 'the replayed pass consumed the first value');
  assert.equal(pc, 1);
  assert.equal(world.counts().free, 1, 'the first replayed sleep released its block');

  // The root parked a second time and reserved a fresh block for it.
  assert.equal(d.busy, true);
  assert.equal(world.counts().malloc, 2, 'the second await reserves its own block');
  assert.equal(world.count('startUnwind'), 2);

  await bounded(d.waitUntilIdle());
  await sleep(10);
  assert.equal(d.result, 15);
  assert.equal(pc, 1);
  assert.equal(d.state, 0);
  assert.equal(d.busy, false);
  assert.equal(d.inGuestDepth, 0);
  // One free per consumed replay, so two blocks in and two out.
  assert.deepEqual(world.counts(), { malloc: 2, free: 2 });
  for (const fn of ['startUnwind', 'stopUnwind', 'startRewind', 'stopRewind']) {
    assert.equal(world.count(fn), 2, `${fn} called twice`);
  }
});

test('waiters created while parked are all answered when the root completes', async () => {
  const world = makeWorld();
  const d = newDriver(world);
  let release;
  const gate = new Promise((r) => (release = r));
  const root = d.wrap('root', () => {
    if (d.state === 1) return 0;
    const waited = d.sleep(() => gate.then(() => 1));
    if (d.state === 1) return 0;
    return waited;
  });

  assert.equal(root(), 0);
  assert.equal(d.busy, true);
  assert.equal(d.inGuestDepth, 0);

  // Buffered while the root is parked: the rewind is driven from a microtask, so at this instant no
  // completion has happened yet and each waiter simply joins the queue.
  const waiters = [d.waitUntilIdle(), d.waitUntilIdle(), d.waitUntilIdle()];
  const raced = waiters.map((w) => bounded(w));
  raced.forEach((w) => w.catch(() => undefined));
  assert.equal(d.busy, true);

  release();
  await bounded(Promise.all(raced));
  await sleep(10);
  assert.equal(d.result, 1);
  assert.equal(d.state, 0);
  assert.equal(d.busy, false);
  assert.equal(d.inGuestDepth, 0);
  // One park, one unwind, one block: the deliver-on-completion answers every waiter together.
  assert.deepEqual(world.counts(), { malloc: 1, free: 1 });
  for (const fn of ['startUnwind', 'stopUnwind', 'startRewind', 'stopRewind']) {
    assert.equal(world.count(fn), 1, `${fn} called once`);
  }
});

test('a rejected thunk leaves a retained failure that rejects every waiter', async () => {
  const world = makeWorld();
  const d = newDriver(world);
  const failing = d.wrap('failing', () => {
    if (d.state === 1) return 0;
    const waited = d.sleep(() => sleep(5).then(() => Promise.reject(new Error('boom'))));
    if (d.state === 1) return 0;
    return waited;
  });

  const immediate = failing();
  assert.equal(immediate, 0);
  const waiter = d.waitUntilIdle();
  await assert.rejects(bounded(waiter), /boom/);
  // Retained, not consumed: the very next waiter hears the same failure.
  await assert.rejects(bounded(d.waitUntilIdle()), /boom/);
  assert.equal(d.state, 0);
  assert.equal(d.busy, false);
});

test('dispose rejects waiters, and a late promise settles without re-entering', async () => {
  const world = makeWorld();
  const d = newDriver(world);
  let release;
  const gate = new Promise((r) => (release = r));
  const root = d.wrap('root', () => {
    if (d.state === 1) return 0;
    const waited = d.sleep(() => gate.then(() => 4));
    if (d.state === 1) return 0;
    return waited;
  });

  assert.equal(root(), 0);
  assert.equal(d.busy, true);
  assert.equal(d.state, 0);
  assert.equal(d.inGuestDepth, 0);

  const waiters = [d.waitUntilIdle(), d.waitUntilIdle(), d.waitUntilIdle()];
  d.dispose('torn down');
  for (const waiter of waiters) await assert.rejects(bounded(waiter), /torn down/);
  assert.equal(d.disposed, true);
  assert.equal(d.busy, false);
  assert.equal(d.inGuestDepth, 0);
  assert.equal(d.state, 0);

  release();
  await sleep(20);
  assert.deepEqual(world.counts(), { malloc: 1, free: 1 });
  assert.equal(d.state, 0);
  assert.equal(d.busy, false);

  // Disposal refuses rather than tears down: wrapping still works, and the refusal happens when the
  // wrapped export is actually invoked.
  const wrapped = d.wrap('again', () => 1);
  assert.throws(() => wrapped(), /disposed/);
  assert.throws(() => d.sleep(() => 1), /disposed/);
});

test('control throws on startUnwind: the root throws, the driver busy, the block freed', async () => {
  const world = makeWorld();
  const d = newDriver(world);
  const control = throwingControl(world, 'startUnwind');
  const broken = new ArtemisSuspendDriver({ control, memory: world.memory, stackSize: 256 });
  const root = broken.wrap('root', () => {
    if (broken.state === 1) return 0;
    return broken.sleep(() => sleep(5).then(() => 3));
  });

  assert.throws(() => root(), /startUnwind boom/);
  assert.equal(broken.busy, false);
  assert.equal(broken.state, 0);
  assert.equal(broken.inGuestDepth, 0);
  assert.deepEqual(world.counts(), { malloc: 1, free: 1 });
  // The failure is retained: waiting hears it rather than hanging.
  await assert.rejects(bounded(broken.waitUntilIdle()), /startUnwind boom/);
});

test('control throws on stopUnwind: the unwind throw fails the root and frees the block', async () => {
  const world = makeWorld();
  const control = throwingControl(world, 'stopUnwind');
  const d = new ArtemisSuspendDriver({ control, memory: world.memory, stackSize: 256 });
  const root = d.wrap('root', () => {
    if (d.state === 1) return 0;
    const waited = d.sleep(() => sleep(5).then(() => 3));
    if (d.state === 1) return 0;
    return waited;
  });

  assert.throws(() => root(), /stopUnwind boom/);
  assert.equal(d.busy, false);
  assert.equal(d.state, 0);
  assert.equal(d.inGuestDepth, 0);
  assert.deepEqual(world.counts(), { malloc: 1, free: 1 });
  await assert.rejects(bounded(d.waitUntilIdle()), /stopUnwind boom/);
});

test('control throws on startRewind: the root returns 0, then waiting rejects', async () => {
  const world = makeWorld();
  const control = throwingControl(world, 'startRewind');
  const d = new ArtemisSuspendDriver({ control, memory: world.memory, stackSize: 256 });
  const root = d.wrap('root', () => {
    if (d.state === 1) return 0;
    const waited = d.sleep(() => sleep(5).then(() => 3));
    if (d.state === 1) return 0;
    return waited;
  });

  // The unwind itself is clean, so the placeholder comes back and only the rewind fails, later.
  assert.equal(root(), 0);
  await assert.rejects(bounded(d.waitUntilIdle()), /startRewind boom/);
  await sleep(10);
  assert.equal(d.busy, false);
  assert.equal(d.state, 0);
  assert.equal(d.inGuestDepth, 0);
  assert.deepEqual(world.counts(), { malloc: 1, free: 1 });
});

test('control throws on stopRewind: the throw surfaces from the replay and frees the block', async () => {
  const world = makeWorld();
  const control = throwingControl(world, 'stopRewind');
  const d = new ArtemisSuspendDriver({ control, memory: world.memory, stackSize: 256 });
  const root = d.wrap('root', () => {
    if (d.state === 1) return 0;
    const waited = d.sleep(() => sleep(5).then(() => 3));
    if (d.state === 1) return 0;
    return waited;
  });

  assert.equal(root(), 0);
  await assert.rejects(bounded(d.waitUntilIdle()), /stopRewind boom/);
  await sleep(10);
  assert.equal(d.busy, false);
  assert.equal(d.state, 0);
  assert.equal(d.inGuestDepth, 0);
  assert.deepEqual(world.counts(), { malloc: 1, free: 1 });
});
