import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const temp = mkdtempSync(join(tmpdir(), 'artemis-fd-idb-'));
process.on('exit', () => { if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-fd-idb-')) rmSync(temp, { recursive: true, force: true }); });
const out = join(temp, 'out');
execFileSync(process.execPath, [join(root, 'node_modules/typescript/bin/tsc'), 'packages/runtime-artemis/src/host/boot.ts', '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler', '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck', '--rootDir', 'packages/runtime-artemis/src', '--outDir', out], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/runtime-artemis/node_modules'), join(out, 'node_modules'), 'junction');
const { createArtemisFdImports, ArtemisSyncIdb } = await import(pathToFileURL(join(out, 'host/fd.js')));
const { createArtemisBootTable } = await import(pathToFileURL(join(out, 'host/boot.js')));
const { ArtemisMemory } = await import(pathToFileURL(join(out, 'host/memory.js')));
const { ArtemisFs } = await import(pathToFileURL(join(out, 'host/tree.js')));
const gate = () => { let resolve, reject; const promise = new Promise((a, b) => { resolve = a; reject = b; }); void promise.catch(() => {}); return { promise, resolve, reject }; };
const flush = async () => { for (let n = 0; n < 20; n++) await Promise.resolve(); };
function rig(overrides = {}, boot = false) {
  const wasm = new WebAssembly.Memory({ initial: 1 }), memory = new ArtemisMemory({ memory: wasm }), idb = new ArtemisSyncIdb();
  const tasks = [], freed = [], callbacks = [], allocations = [], reports = [];
  memory.writeUtf8(1024, 'db'); memory.writeUtf8(1100, 'id'); memory.u8.set([1, 2, 3], 1200);
  idb.store('db', 'id', new Uint8Array([7, 8, 9]));
  const host = { memory, fs: new ArtemisFs(), idb, malloc: length => { allocations.push(length); return 8192; }, free: pointer => freed.push(pointer), callback: (...args) => callbacks.push(args), enqueue: task => { const done = gate(); tasks.push({ task, done }); return done.promise; }, onBlocked: (a, b) => reports.push([a, b]), ...overrides };
  const imports = boot ? createArtemisBootTable({ ...host, spec: { rows: [], buckets: {} }, fdCallback: host.callback }).imports : createArtemisFdImports(host);
  const start = index => { const { task, done } = tasks[index]; const run = Promise.resolve().then(task); run.then(done.resolve, done.reject); return run; };
  return { wasm, memory, idb, tasks, freed, callbacks, allocations, reports, imports, start };
}

test('Cc snapshots keys and bytes at arrival and defers the whole store/callback', async () => {
  const r = rig(); r.imports.Cc(1024, 1100, 1200, 3, 17, 5, 6); assert.equal(r.tasks.length, 1); assert.deepEqual(r.callbacks, []);
  assert.deepEqual([...r.idb.load('db', 'id')], [7, 8, 9]); r.memory.writeUtf8(1024, 'xx'); r.memory.writeUtf8(1100, 'yy'); r.memory.u8.fill(99, 1200, 1203);
  await r.start(0); assert.deepEqual([...r.idb.load('db', 'id')], [1, 2, 3]); assert.equal(r.idb.exists('xx', 'yy'), false); assert.deepEqual(r.callbacks, [[5, 17, undefined, undefined]]);
});
test('Dc keeps the queue unit and its allocation alive until a parked callback settles', async () => {
  const g = gate(), r = rig({ callback: () => g.promise }); r.imports.Dc(1024, 1100, 17, 5, 6);
  assert.deepEqual(r.allocations, []); let settled = false; const run = r.start(0); run.then(() => { settled = true; }, () => { settled = true; }); await flush();
  assert.deepEqual(r.allocations, [3]); assert.deepEqual([...r.memory.u8.slice(8192, 8195)], [7, 8, 9]); assert.deepEqual(r.freed, []);
  assert.equal(settled, false, 'the serializer must still own the callback root'); g.resolve(); await run; assert.deepEqual(r.freed, [8192]);
});
test('Dc callback rejection releases once but remains the actual queue rejection', async () => {
  const boom = Error('IDB callback failed'), g = gate(), r = rig({ callback: () => g.promise }); r.imports.Dc(1024, 1100, 17, 5, 6);
  const run = r.start(0); const checked = assert.rejects(run, error => error === boom); await flush(); assert.deepEqual(r.freed, []);
  g.reject(boom); await checked; await flush(); assert.deepEqual(r.freed, [8192]);
});
test('Dc refreshes real grown memory before copying and frees after completion', async () => {
  let r; r = rig({ malloc: length => { r.allocations.push(length); r.wasm.grow(1); return 8192; } }); const stale = r.memory.u8;
  r.imports.Dc(1024, 1100, 17, 5, 6); await r.start(0); assert.equal(stale.byteLength, 0);
  assert.deepEqual([...new Uint8Array(r.wasm.buffer, 8192, 3)], [7, 8, 9]); assert.deepEqual(r.callbacks, [[5, 17, 8192, 3]]); assert.deepEqual(r.freed, [8192]);
});
test('queued IDB work cannot allocate or call back after disposal', async () => {
  let disposed = false; const r = rig({ isDisposed: () => disposed }); r.imports.Dc(1024, 1100, 17, 5, 6); disposed = true; await r.start(0);
  assert.deepEqual(r.allocations, []); assert.deepEqual(r.callbacks, []); assert.deepEqual(r.freed, []);
});
test('Ec snapshots keys and reports the exact vii callback shape', async () => {
  const r = rig(); r.imports.Ec(1024, 1100, 17, 5, 6); r.memory.writeUtf8(1024, 'changed'); await r.start(0);
  assert.deepEqual(r.callbacks, [[5, 17, 1, undefined]]);
});
test('entry zero is a no-op but an asynchronous load still releases its temporary buffer', async () => {
  const r = rig(); r.imports.Dc(1024, 1100, 17, 0, 0); await r.start(0); assert.deepEqual(r.callbacks, []); assert.deepEqual(r.freed, [8192]);
});
for (const pointer of [-1, 0x100000000, 1.5]) test(`invalid numeric allocation ${pointer} is never copied or freed`, async () => {
  const r = rig({ malloc: () => pointer }); r.imports.Dc(1024, 1100, 17, 5, 6); await r.start(0);
  assert.deepEqual(r.callbacks, [[6, 17, undefined, undefined]]); assert.deepEqual(r.freed, []);
});
test('a valid owned pointer whose full payload is out of bounds is released exactly once', async () => {
  const r = rig({ malloc: () => 65535 }); r.imports.Dc(1024, 1100, 17, 5, 6); await r.start(0);
  assert.deepEqual(r.callbacks, [[6, 17, undefined, undefined]]); assert.deepEqual(r.freed, [65535]);
});
test('Hc validates all output slots before allocation and transfers successful ownership', () => {
  const r = rig(); r.imports.Hc(1024, 1100, 1020, 1028, 65535); assert.deepEqual(r.allocations, []);
  r.imports.Hc(1024, 1100, 2048, 2052, 2056); assert.deepEqual([r.memory.i32[512], r.memory.i32[513], r.memory.i32[514]], [8192, 3, 0]); assert.deepEqual(r.freed, []);
});
test('throwing diagnostics cannot replace a rejected enqueue or orphan it', async () => {
  const r = rig({ enqueue: () => Promise.reject(Error('enqueue refused')), onBlocked: () => { throw Error('logger refused'); } });
  assert.doesNotThrow(() => r.imports.Dc(1024, 1100, 17, 5, 6)); await flush(); assert.deepEqual(r.allocations, []);
});
test('boot forwards the same serializer, callback, and disposal seams to the fd bucket', async () => {
  let disposed = false; const r = rig({ isDisposed: () => disposed }, true); r.imports.Ec(1024, 1100, 17, 5, 6); assert.equal(r.tasks.length, 1); await r.start(0);
  assert.deepEqual(r.callbacks, [[5, 17, 1, undefined]]); disposed = true; r.imports.Ec(1024, 1100, 17, 5, 6); assert.equal(r.tasks.length, 1);
});
