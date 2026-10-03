// Boots the real Artemis host bridge through its own startup sequence. It compiles boot.ts +
// suspend.ts with the repo's tsc, links the workspace dependency into the temp output so bridge.ts's
// bare `@online-gal/vfs/archives` import resolves, then drives a guest with the real driver.
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { lstatSync, mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, relative, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const repoRoot = resolve(fileURLToPath(new URL('..', import.meta.url)));
const inside = (parent, child) => { const rel = relative(parent, child); return rel !== '' && !rel.startsWith('..'); };
const deferred = () => {
  let resolve, reject;
  const promise = new Promise((res, rej) => { resolve = res; reject = rej; });
  return { promise, resolve, reject };
};
const advance = () => new Promise((done) => setImmediate(done));
// Bounds an await so a stalled guest fails instead of hanging, and never leaks the timer.
const bounded = (promise, ms = 2000) => new Promise((settle, fail) => {
  const timer = setTimeout(() => fail(new Error(`boot did not settle within ${ms}ms`)), ms);
  promise.then((value) => { clearTimeout(timer); settle(value); }, (error) => { clearTimeout(timer); fail(error); });
});

const workDir = mkdtempSync(join(tmpdir(), 'artemis-boot-'));
// Registered before anything is imported, so a failing compile or import still cleans up.
process.on('exit', () => {
  if (realpathSync(dirname(workDir)) !== realpathSync(tmpdir())) return;
  if (!basename(workDir).startsWith('artemis-boot-')) return;
  rmSync(workDir, { recursive: true, force: true });
});

const outDir = join(workDir, 'out');
execFileSync(process.execPath, [join(repoRoot, 'node_modules', 'typescript', 'bin', 'tsc'),
  'packages/runtime-artemis/src/host/boot.ts', 'packages/runtime-artemis/src/host/suspend.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck',
  '--rootDir', 'packages/runtime-artemis/src', '--outDir', outDir], { cwd: repoRoot, stdio: 'inherit' });
writeFileSync(join(outDir, 'package.json'), '{"type":"module"}');

// The output is in the OS temp dir, so the bare `@online-gal/vfs` specifier can only resolve
// through this one link into the workspace. Resolution is asserted from inside the output.
const link = join(outDir, 'node_modules');
symlinkSync(join(repoRoot, 'packages', 'runtime-artemis', 'node_modules'), link, 'junction');
assert.equal(lstatSync(link).isSymbolicLink(), true, 'the node_modules link is a real symlink');
const probe = join(outDir, 'resolve-probe.mjs');
writeFileSync(probe, "export const url = import.meta.resolve('@online-gal/vfs/archives');\n");
const resolvedVfs = realpathSync(fileURLToPath((await import(pathToFileURL(probe).href)).url));
assert.ok(inside(join(repoRoot, 'packages', 'vfs'), resolvedVfs), `resolved outside the workspace: ${resolvedVfs}`);

const { createArtemisBootTable } = await import(pathToFileURL(join(outDir, 'host', 'boot.js')).href);
const { ArtemisSuspendDriver, ASYNCIFY_STATE } = await import(pathToFileURL(join(outDir, 'host', 'suspend.js')).href);
const { ArtemisMemory } = await import(pathToFileURL(join(outDir, 'host', 'memory.js')).href);
const { ArtemisFs } = await import(pathToFileURL(join(outDir, 'host', 'tree.js')).href);

// One real guest: a one-page heap, the real driver over it (four no-op control functions and an
// allocator handing out 4-aligned blocks), and the five exports boot actually touches.
function makeRig(overrides, setup = {}) {
  const memory = new ArtemisMemory({ memory: new WebAssembly.Memory({ initial: 1 }) });
  const calls = { mc: 0, qc: 0, restores: 0, seen: [] };
  let sp = 60000;
  let next = 1024;
  const driver = new ArtemisSuspendDriver({
    control: { startUnwind() {}, stopUnwind() {}, startRewind() {}, stopRewind() {} },
    memory: { malloc: (bytes) => { const at = next; next += bytes + 16; return at; }, free() {}, heap32: () => memory.i32 },
  });
  const guest = {
    Mc() { calls.mc += 1; },
    Xc() { return sp; },
    Yc(saved) { sp = saved; calls.restores += 1; },
    Zc(bytes) { sp -= Math.ceil(bytes / 16) * 16; return sp; },
    Qc(argc, argv) { calls.qc += 1; calls.seen.push([argc, argv]); return 73; },
  };
  Object.assign(guest, typeof overrides === 'function' ? overrides(driver, calls) : overrides);
  const table = createArtemisBootTable({ memory, fs: new ArtemisFs(), spec: { rows: [], buckets: {} }, suspend: driver, ...setup });
  return { memory, driver, calls, stack: () => sp, instance: { exports: guest }, table };
}

test('honoured trace distinguishes normal returns from refusals, throws and stubs, and resets together', () => {
  const audioNames = 'F G Ga Ha Ia Ja Ka La Ma Na Oa Pa R ga oa vc wc xc yc'.split(' ');
  const audioImports = Object.fromEntries(audioNames.map(name => [name, () => 0])); audioImports.Ja = () => { throw undefined; };
  const rig = makeRig(undefined, { gl: { audioImports }, spec: { rows: [
    { name: 'Aa', official: '_pthread_mutexattr_destroy', bucket: 'runtime' },
    { name: 'B', official: '_emscripten_asm_const_int', bucket: 'runtime' },
    { name: 'Ja', official: '_alGenSources', bucket: 'gl' },
    { name: 'Xb', official: '_system', bucket: 'todo' },
    { name: 'unimplemented', official: '_unimplemented_fixture', bucket: 'todo' },
  ], buckets: {} } });
  assert.equal(rig.table.trace.honoured.size, 0);
  assert.equal(rig.table.imports.Aa(0), 0); rig.table.imports.Aa(0);
  assert.equal(rig.table.imports.B(716732, 0, 0), 0, 'missing ASM hook reports before neutral return');
  let caught = false; try { rig.table.imports.Ja(1, 256); } catch (error) { caught = true; assert.equal(error, undefined); } assert.equal(caught, true);
  assert.equal(rig.table.imports.Xb(0), -1); assert.equal(rig.table.imports.unimplemented(), 0);
  assert.deepEqual([...rig.table.trace.honoured], ['Aa'], 'only measured synchronous non-refused outcome is honoured');
  assert.equal(rig.table.trace.counts.size, 5); assert.equal(rig.table.trace.counts.get('Aa'), 2); assert.equal(rig.table.trace.blocked.length, 2);
  rig.table.trace.reset(); assert.equal(rig.table.trace.honoured.size, 0); assert.equal(rig.table.trace.counts.size, 0); assert.equal(rig.table.trace.blocked.length, 0);
  rig.driver.dispose();
});

test('a real asyncify park does not count the provisional void import return as honoured', async t => {
  const pending = deferred(); let allocations = 16384;
  const rig = makeRig(undefined, { malloc: bytes => { const p = allocations; allocations += bytes + 16; return p; }, free() {}, resolve: () => pending.promise,
    spec: { rows: [{ name: 'fa', official: '_emscripten_wget_data', bucket: 'asyncify' }], buckets: {} } });
  rig.memory.writeUtf8(2048, 'payload.bin', 12);
  t.after(async () => { pending.resolve(new Uint8Array([7])); await rig.driver.waitUntilIdle().catch(() => {}); rig.driver.dispose(); });
  const root = rig.driver.wrap('honoured parked import', () => { rig.table.imports.fa(2048, 256, 260, 264); return 0; });
  root(); assert.equal(rig.driver.busy, true); assert.equal(rig.table.trace.counts.get('fa'), 1);
  assert.equal(rig.table.trace.honoured.has('fa'), false, 'void placeholder while unwinding is not a completed download');
  pending.resolve(new Uint8Array([7])); await bounded(rig.driver.waitUntilIdle());
  assert.equal(rig.memory.i32[264 >>> 2], 0); assert.equal(rig.memory.i32[260 >>> 2], 1);
});

test('deferred and silent-neutral imports cannot inflate the honoured lower bound', async t => {
  const entered = deferred(), transfer = deferred(); void transfer.promise.catch(() => {});
  const rig = makeRig(undefined, { runAsmConst: () => 0, resolve: () => { entered.resolve(); return transfer.promise; }, malloc: () => 20000, free() {} });
  t.after(async () => { transfer.resolve(new Uint8Array([7])); await advance(); rig.driver.dispose(); });
  rig.memory.writeUtf8(2048, 'payload.bin', 12);
  rig.table.imports.Qa(2048, 0, 0, 0);
  await bounded(entered.promise);
  assert.equal(rig.table.trace.honoured.has('Qa'), false, 'nonblocking transfer has not completed at the return');
  assert.equal(rig.table.imports.nc(0, 720, 0, 0), 0);
  assert.equal(rig.table.trace.honoured.has('nc'), false, 'invalid window dimensions did not create a window');
  assert.equal(rig.table.imports.B(716748, 0, 0), 0);
  assert.equal(rig.table.trace.honoured.has('B'), false, 'queued ASM persistence has no synchronous completion evidence');
});

test('async forward exclusions use guest import names rather than dynCall export names', () => {
  const reached = [];
  const rig = makeRig({
    bd: (...args) => { reached.push(['bd', ...args]); },
    rd: (...args) => { reached.push(['rd', ...args]); },
    _c() {},
  });
  try {
    rig.table.attachExports(rig.instance);
    rig.table.imports.d(7, 19);
    rig.table.imports.p(11);
    assert.deepEqual(reached, [['bd', 7, 19], ['rd', 11]], 'actual forward bodies ran normally');
    assert.equal(rig.table.trace.counts.get('d'), 1);
    assert.equal(rig.table.trace.counts.get('p'), 1);
    assert.equal(rig.table.trace.honoured.has('d'), false, 'd is the import, bd is only its guest export');
    assert.equal(rig.table.trace.honoured.has('p'), false, 'p is the import, rd is only its guest export');
  } finally { rig.driver.dispose(); }
});

test('bootAsync writes a UTF-8 argv with a null terminator and reports _main', async () => {
  const rig = makeRig();
  const result = await bounded(rig.table.bootAsync(rig.instance, ['中', 'x']));
  assert.equal(result.returned, 73);
  assert.equal(result.thisProgram, './this.program');
  assert.deepEqual([rig.calls.mc, rig.calls.qc, rig.calls.restores], [1, 1, 1]);
  const [argc, argv] = rig.calls.seen[0];
  assert.equal(argc, 3);
  assert.equal(argv, result.argv);
  const word = (slot) => rig.memory.u32[(argv >>> 2) + slot];
  assert.deepEqual([0, 1, 2, 3].map((slot) => (word(slot) === 0 ? 0 : rig.memory.utf8(word(slot)))), ['./this.program', '中', 'x', 0]);
  // '中' is U+4E2D: three UTF-8 bytes and then the terminator, not one truncated code unit.
  assert.deepEqual([...rig.memory.u8.slice(word(1), word(1) + 4)], [0xe4, 0xb8, 0xad, 0]);
  assert.equal(rig.stack(), 60000, 'Yc put the stack pointer back');
});

test('constructors and _main park one after the other on the real driver', async () => {
  const ctorGate = deferred();
  const mainGate = deferred();
  const rig = makeRig((driver, calls) => ({
    Mc() {
      calls.mc += 1;
      driver.sleep(() => ctorGate.promise);
      if (driver.state !== ASYNCIFY_STATE.Normal) return;
    },
    Qc(argc, argv) {
      calls.qc += 1;
      calls.seen.push([argc, argv]);
      const value = driver.sleep(() => mainGate.promise);
      return driver.state === ASYNCIFY_STATE.Normal ? value : 0;
    },
  }));
  const booting = rig.table.bootAsync(rig.instance, []);
  await advance();
  assert.deepEqual([rig.calls.mc, rig.calls.qc, rig.calls.restores], [1, 0, 0], 'unwound inside Mc');
  ctorGate.resolve(0);
  await advance();
  assert.deepEqual([rig.calls.mc, rig.calls.qc, rig.calls.restores], [2, 1, 0], 'Mc replayed, _main parked');
  mainGate.resolve(91);
  const result = await bounded(booting);
  assert.equal(result.returned, 91, 'the value is the driver result, not the replay placeholder');
  assert.deepEqual([rig.calls.qc, rig.calls.restores], [2, 1]);
  assert.deepEqual(rig.calls.seen, [[1, result.argv], [1, result.argv]], 'both passes saw the same argv');
});

test('a literal unwind throw starts the main loop and leaves the driver usable', async () => {
  const rig = makeRig({ Qc() { throw 'unwind'; } });
  const result = await bounded(rig.table.bootAsync(rig.instance, []));
  assert.equal(result.mainLoopStarted, true);
  assert.equal(result.returned, 0);
  assert.equal(rig.calls.restores, 1);
  assert.equal(rig.driver.wrap('Qc', () => 19)(), 19, 'a later root call still runs');
});

test('a throwing constructor rejects with its own error, before main or the stack', async () => {
  const boom = new Error('ctor boom');
  const rig = makeRig({ Mc() { throw boom; } });
  await assert.rejects(bounded(rig.table.bootAsync(rig.instance, [])), (error) => error === boom);
  assert.deepEqual([rig.calls.qc, rig.calls.restores], [0, 0]);
});

test('a throwing _main rejects with its own error and still restores the stack', async () => {
  const boom = new Error('main boom');
  const rig = makeRig({ Qc() { throw boom; } });
  await assert.rejects(bounded(rig.table.bootAsync(rig.instance, [])), (error) => error === boom);
  assert.deepEqual([rig.calls.restores, rig.stack()], [1, 60000]);
});

test('an allocator that refuses the argv array fails the boot and restores once', async () => {
  const rig = makeRig({ Zc() { return 0; } });
  await assert.rejects(bounded(rig.table.bootAsync(rig.instance, [])), /stackAlloc refused the argv array/);
  assert.deepEqual([rig.calls.qc, rig.calls.restores], [0, 1]);
});

test('a throwing stackRestore cannot replace the primary failure', async () => {
  const primary = new Error('main boom');
  const rig = makeRig((driver, calls) => ({
    Qc() { throw primary; },
    Yc() { calls.restores += 1; throw new Error('restore boom'); },
  }));
  await assert.rejects(bounded(rig.table.bootAsync(rig.instance, [])), (error) => error === primary);
  assert.equal(rig.calls.restores, 1);
});

test('a stackRestore that throws with no other failure is the rejection', async () => {
  const secondary = new Error('restore boom');
  const rig = makeRig((driver, calls) => ({ Yc() { calls.restores += 1; throw secondary; } }));
  await assert.rejects(bounded(rig.table.bootAsync(rig.instance, [])), (error) => error === secondary);
  assert.equal(rig.calls.restores, 1);
});

test('a throwing main-export getter still restores the raw stack once', async () => {
  const boom = Error('main getter failed'), rig = makeRig();
  Object.defineProperty(rig.instance.exports, 'Qc', { get() { throw boom; } });
  await assert.rejects(bounded(rig.table.bootAsync(rig.instance, [])), error => error === boom);
  assert.deepEqual([rig.calls.restores, rig.stack()], [1, 60000]);
});

test('a main-wrapper failure before argv allocation still restores once', async () => {
  const boom = Error('main wrap failed'), rig = makeRig();
  const wrap = rig.driver.wrap.bind(rig.driver);
  rig.driver.wrap = (name, fn) => { if (name === 'Qc') throw boom; return wrap(name, fn); };
  await assert.rejects(bounded(rig.table.bootAsync(rig.instance, [])), error => error === boom);
  assert.deepEqual([rig.calls.restores, rig.calls.qc, rig.stack()], [1, 0, 60000]);
});

test('the exterior rd frame waits for its real driver replay before scheduling another frame', async (t) => {
  const gate = deferred(), frames = [], seen = []; let lifecycle;
  const rig = makeRig(driver => ({
    rd(entry) {
      seen.push(entry);
      driver.sleep(() => gate.promise);
      if (driver.state !== ASYNCIFY_STATE.Normal) return;
    },
  }), { gl: { requestFrame: tick => frames.push(tick), setLifecycle: c => lifecycle = c } });
  t.after(async () => { gate.resolve(0); await rig.driver.waitUntilIdle().catch(() => {}); await lifecycle.dispose(); rig.driver.dispose(); });
  rig.table.attachExports(rig.instance);
  rig.table.imports._b(7, 0, 0);
  await advance();
  assert.deepEqual(seen, [7]); assert.equal(rig.driver.busy, true); assert.equal(frames.length, 0);
  gate.resolve(0); await bounded(rig.driver.waitUntilIdle()); await advance();
  assert.deepEqual(seen, [7, 7], 'the same exterior entry and arguments were replayed');
  assert.equal(frames.length, 1, 'scheduling waits for the actual resumed frame, not its placeholder');
});

test('an exterior event captures its Lc function once across a parked replay', async (t) => {
  const gate = deferred(), handlers = new Map(), seen = []; let lifecycle, lookups = 0, replacementCalls = 0, prevented = 0, current;
  const document = { addEventListener: (name, fn) => handlers.set(name, fn), removeEventListener: (name, fn) => { if (handlers.get(name) === fn) handlers.delete(name); }, querySelector: () => null };
  const rig = makeRig(driver => {
    current = (...args) => {
      seen.push(args);
      const answer = driver.sleep(() => gate.promise);
      return driver.state === ASYNCIFY_STATE.Normal ? answer : 0;
    };
    return { Lc: { get(index) { assert.equal(index, 7); lookups++; return current; } } };
  }, { malloc: () => 8192, free() {}, gl: { document, setLifecycle: c => lifecycle = c } });
  t.after(async () => { gate.resolve(1); await rig.driver.waitUntilIdle().catch(() => {}); await lifecycle.dispose(); rig.driver.dispose(); });
  rig.table.attachExports(rig.instance);
  assert.equal(rig.table.imports.cc(1, 33, 0, 7), 0);
  handlers.get('keydown')({ key: 'Enter', code: 'Enter', keyCode: 13, charCode: 0, preventDefault() { prevented++; } });
  await advance(); assert.equal(rig.driver.busy, true); assert.deepEqual(seen, [[2, 8192, 33]]);
  current = () => { replacementCalls++; return 0; };
  gate.resolve(1); await bounded(rig.driver.waitUntilIdle()); await advance();
  assert.equal(lookups, 1, 'table resolution belongs to arrival, not each rewind pass');
  assert.equal(replacementCalls, 0, 'a table mutation while parked must not replace the suspended raw function');
  assert.deepEqual(seen, [[2, 8192, 33], [2, 8192, 33]]);
  assert.equal(prevented, 1, 'the callback verdict comes from the completed driver result');
});
