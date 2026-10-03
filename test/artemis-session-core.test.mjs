// Native, deliberately synthetic wasm exercises the independent session. It is not an engine/game
// acceptance test and does not pretend to reproduce Binaryen's Asyncify transformation.
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const temp = mkdtempSync(join(tmpdir(), 'artemis-session-core-'));
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-session-core-')) rmSync(temp, { recursive: true, force: true });
});
const out = join(temp, 'out');
execFileSync(process.execPath, [join(root, 'node_modules/typescript/bin/tsc'), 'packages/runtime-artemis/src/session-core.ts', '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler', '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck', '--rootDir', 'packages/runtime-artemis/src', '--outDir', out], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/runtime-artemis/node_modules'), join(out, 'node_modules'), 'junction');
const { createArtemisSessionCore, buildSpec } = await import(pathToFileURL(join(out, 'session-core.js')));

const text = new TextEncoder();
const gate = () => { let resolve, reject; const promise = new Promise((a, b) => { resolve = a; reject = b; }); void promise.catch(() => {}); return { promise, resolve, reject }; };
const flush = async () => { for (let n = 0; n < 30; n++) await Promise.resolve(); await new Promise(r => setImmediate(r)); };
const bounded = (p, ms = 2000) => new Promise((resolve, reject) => { const timer = setTimeout(() => reject(Error('synthetic session stalled')), ms); p.then(v => { clearTimeout(timer); resolve(v); }, e => { clearTimeout(timer); reject(e); }); });
const uleb = n => { const out = []; do { const byte = n & 127; n >>>= 7; out.push(byte | (n ? 128 : 0)); } while (n); return out; };
const sleb = n => { const out = []; for (;;) { const byte = n & 127; n >>= 7; const more = !((n === 0 && !(byte & 64)) || (n === -1 && (byte & 64))); out.push(byte | (more ? 128 : 0)); if (!more) return out; } };
const string = s => [...uleb(text.encode(s).length), ...text.encode(s)];
const vec = rows => [...uleb(rows.length), ...rows.flat()];
const section = (n, bytes) => [n, ...uleb(bytes.length), ...bytes];
const c = n => [0x41, ...sleb(n)];
const lg = n => [0x20, ...uleb(n)];
const gg = n => [0x23, ...uleb(n)];
const gs = n => [0x24, ...uleb(n)];
const call = n => [0x10, ...uleb(n)];
const increment = n => [...gg(n), ...c(1), 0x6a, ...gs(n)];
const store = (address, value) => [...c(address), ...value, 0x36, 2, 0];

function nativeModule({ loop = false, fps = 0, mainTrap = false, unknown = false, mainSleep = false } = {}) {
  const types = [], typeByShape = new Map(), functions = [], exports = [], table = [];
  const type = (arity, result = false) => { const key = `${arity}/${result}`; if (typeByShape.has(key)) return typeByShape.get(key); const id = types.length; types.push([0x60, ...vec(Array.from({ length: arity }, () => [0x7f])), ...vec(result ? [[0x7f]] : [])]); typeByShape.set(key, id); return id; };
  const imports = [
    [...string('a'), ...string('fa'), 0, ...uleb(type(4))],
    [...string('a'), ...string('_b'), 0, ...uleb(type(3))],
    [...string('a'), ...string('a'), 2, 1, ...uleb(3072), ...uleb(65536)],
  ];
  if (unknown) imports[0] = [...string('a'), ...string('unknown_fixture_import'), 0, ...uleb(type(4))];
  const fn = (name, arity, result, body, locals = 0) => {
    const index = functions.length + 2;
    functions.push({ type: type(arity, result), body: [...vec(locals ? [[...uleb(locals), 0x7f]] : []), ...body, 0x0b] });
    exports.push([...string(name), 0, ...uleb(index)]); return index;
  };
  const park = [...c(4096), ...c(5120), ...c(5124), ...c(5128), ...call(0), ...gg(0), 0x04, 0x40, ...c(0), 0x0f, 0x0b];
  fn('Nc', 1, true, [...gg(1), 0x22, 1, ...lg(0), ...c(15), 0x6a, ...c(-16), 0x71, 0x6a, ...gs(1), ...lg(1)], 1);
  fn('Oc', 1, false, [...increment(5), ...lg(0), ...gs(7)]);
  fn('Pc', 0, true, c(3072));
  fn('Ud', 1, false, [...c(1), ...gs(0)]);
  fn('Vd', 0, false, [...c(0), ...gs(0)]);
  fn('Wd', 1, false, [...c(2), ...gs(0)]);
  fn('Xd', 0, false, [...c(0), ...gs(0)]);
  fn('Mc', 0, false, increment(3));
  fn('Xc', 0, true, gg(2));
  fn('Yc', 1, false, [...lg(0), ...gs(2)]);
  fn('Zc', 1, true, [...gg(2), ...lg(0), ...c(15), 0x6a, ...c(-16), 0x71, 0x6b, ...gs(2), ...gg(2)]);
  fn('Qc', 2, true, [...increment(4), ...store(2048, lg(0)), ...store(2052, lg(1)), ...(mainTrap ? [0x00] : []), ...(mainSleep ? park : []), ...(loop ? [...c(1), ...c(fps), ...c(1), ...call(1)] : []), ...c(73)]);
  for (const [signature, arity] of [['bd', 1], ['dd', 2], ['ld', 3], ['id', 4]]) fn(signature, arity + 1, false, Array.from({ length: arity + 1 }, (_, n) => store(2100 + n * 4, lg(n))).flat());
  fn('rd', 1, false, increment(6));
  fn('ed', 4, true, [...store(2180, lg(0)), ...store(2184, lg(1)), ...store(2188, lg(2)), ...store(2192, lg(3)), ...c(1)]);
  fn('park', 0, true, [...park, ...c(37)]);
  for (let arity = 1; arity <= 4; arity++) table.push(fn(`callback${arity}`, arity, false, Array.from({ length: arity }, (_, n) => store(2200 + n * 4, lg(n))).flat()));
  table.push(fn('callbackPark', 1, false, [...increment(8), ...store(2240, lg(0)), ...c(4096), ...c(5120), ...c(5124), ...c(5128), ...call(0), ...gg(0), 0x04, 0x40, 0x0f, 0x0b]));
  table.push(fn('replacement', 1, false, increment(9)));
  exports.push([...string('Lc'), 1, 0], [...string('arena'), 2, 0]);
  for (const [name, index] of [['phase', 0], ['ctorCount', 3], ['mainCount', 4], ['freeCount', 5], ['frameCount', 6], ['lastFree', 7], ['parkCount', 8], ['replacementCount', 9]]) exports.push([...string(name), 3, ...uleb(index)]);
  const globals = [0, 8192, 60000, 0, 0, 0, 0, 0, 0, 0].map(value => [0x7f, 1, ...c(value), 0x0b]);
  const url = [...text.encode('payload.bin'), 0];
  const bytes = new Uint8Array([0, 97, 115, 109, 1, 0, 0, 0,
    ...section(1, vec(types)), ...section(2, vec(imports)),
    ...section(3, vec(functions.map(f => uleb(f.type)))),
    ...section(4, vec([[0x70, 1, ...uleb(table.length + 1), ...uleb(table.length + 1)]])),
    ...section(6, vec(globals)), ...section(7, vec(exports)),
    ...section(9, vec([[0, ...c(1), 0x0b, ...vec(table.map(uleb))]])),
    ...section(10, vec(functions.map(f => [...uleb(f.body.length), ...f.body]))),
    ...section(11, vec([[0, ...c(4096), 0x0b, ...uleb(url.length), ...url]])),
  ]);
  assert.equal(WebAssembly.validate(bytes), true, 'the fixture is real valid native wasm');
  return new WebAssembly.Module(bytes);
}

function environment({ payloadGate, throwPaint = false } = {}) {
  const ini = text.encode('[FILE]\nSAVEPATH = session-native\n[WASM]\nWIDTH = 1280\nHEIGHT = 720\n');
  const reads = [], notifications = [], logs = [], frames = new Map(), canceled = [];
  let next = 1, paint = 0;
  const view = {
    requestAnimationFrame(fn) { const id = next++; frames.set(id, fn); return id; },
    cancelAnimationFrame(id) { canceled.push(id); frames.delete(id); },
  };
  const canvas = { width: 1280, height: 720, ownerDocument: { baseURI: 'https://fixture.invalid/', defaultView: view, addEventListener() {}, removeEventListener() {}, querySelector: () => canvas }, addEventListener() {}, removeEventListener() {}, getContext: () => null };
  const source = {
    name: 'native-fixture', capabilities: { randomAccess: true, writable: false },
    async list() { throw Error('loose game must not be enumerated'); },
    async readFile(path) { reads.push(path); if (path === 'system.ini') return ini.slice(); if (path === 'payload.bin') return payloadGate ? payloadGate.promise : new Uint8Array([7, 8, 9]); throw Error(`unexpected read ${path}`); },
    async exists(path) { return path === 'system.ini' || path === 'payload.bin'; },
    async isDirectory() { return false; }, async size(path) { return path === 'system.ini' ? ini.length : 3; },
  };
  const host = { source, canvas: { deliberately: 'not the GL canvas' }, surface: { width: 1280, height: 720 }, store: { name: 'fixture', async read() {}, async write() {}, async remove() {}, async list() { return []; } }, requestFrame() { paint++; if (throwPaint) throw Error('paint refused'); }, now: () => 0, log: (...args) => logs.push(args), notify: event => notifications.push(event), setting: () => undefined };
  return { host, canvas, reads, notifications, logs, frames, canceled, get paint() { return paint; } };
}
async function rig(t, config = {}, native = {}) {
  const env = environment(config), options = { wasm: nativeModule(native), canvas: env.canvas, ...(config.options || {}) };
  const session = await createArtemisSessionCore(env.host, options);
  t.after(async () => { config.payloadGate?.resolve(new Uint8Array([7, 8, 9])); await session.dispose(); });
  return { ...env, session, options, guest: session.instance.exports };
}
const word = (session, address) => { session.memory.refresh(); return session.memory.i32[address >>> 2]; };

test('native module imports link to the actual memory, lazy assets, and explicit startup', async t => {
  const r = await rig(t), s = r.session;
  assert.ok(s.instance instanceof WebAssembly.Instance); assert.equal(s.options, r.options); assert.equal(s.importSpec, s.spec);
  assert.equal(s.memory.memory.buffer.byteLength, 3072 * 65536); assert.equal(r.guest.ctorCount.value, 0); assert.equal(r.guest.mainCount.value, 0);
  assert.ok(s.fs.lookup('/root.pfs')?.isFile); assert.ok(s.fs.lookup('/system.ini')?.isFile); assert.ok(s.fs.lookup(s.assets.savePath)?.isDir);
  assert.deepEqual(r.reads, ['system.ini']); assert.deepEqual([...await s.assets.resolve('payload.bin')], [7, 8, 9]);
  const first = s.boot(['中']), second = s.boot(['ignored']); assert.equal(first, second, 'only one boot promise');
  const result = await bounded(first); assert.equal(result.returned, 73); assert.notEqual(result.argv, 0); assert.ok(result.steps.length > 1);
  assert.deepEqual([r.guest.ctorCount.value, r.guest.mainCount.value], [1, 1]); assert.equal(word(s, 2048), 2);
  const argv = word(s, 2052); assert.equal(s.memory.utf8(word(s, argv + 4)), '中'); assert.equal(word(s, argv + 8), 0);
});
test('boot uses options.args and a native main trap stays memoized and terminal', async t => {
  const good = await rig(t, { options: { args: ['default-arg'] } }); await bounded(good.session.boot()); assert.equal(word(good.session, 2048), 2);
  const bad = await rig(t, {}, { mainTrap: true }), first = bad.session.boot(); assert.equal(first, bad.session.boot());
  await assert.rejects(bounded(first), WebAssembly.RuntimeError); let ran = false; await assert.rejects(bad.session.enqueue(() => { ran = true; }), WebAssembly.RuntimeError);
  assert.equal(ran, false); assert.equal(bad.notifications.filter(n => n.type === 'fatal').length, 1);
});
test('queue post-idle barrier holds the next operation behind a native parked root', async t => {
  const g = gate(), r = await rig(t, { payloadGate: g }), s = r.session; const root = s.driver.wrap('native park', r.guest.park);
  const order = []; const first = s.enqueue(() => { order.push('park'); return root(); }); const second = s.enqueue(() => order.push('next'));
  await flush(); assert.deepEqual(order, ['park']); assert.equal(s.driver.busy, true); assert.equal(r.reads.filter(p => p === 'payload.bin').length, 1);
  g.resolve(new Uint8Array([4, 5])); await bounded(Promise.all([first, second])); assert.deepEqual(order, ['park', 'next']);
  assert.equal(s.driver.result, 37); assert.equal(word(s, 5124), 2); assert.equal(word(s, 5128), 0); assert.equal(r.guest.phase.value, 0);
});
test('throw undefined is retained once and no queued continuation runs', async t => {
  const r = await rig(t), s = r.session; let ran = false; const first = s.enqueue(() => { throw undefined; }); const second = s.enqueue(() => { ran = true; });
  for (const p of [first, second]) await p.then(() => assert.fail('must reject'), error => assert.equal(error, undefined));
  assert.equal(ran, false); assert.equal(r.notifications.filter(n => n.type === 'fatal').length, 1); assert.ok(Object.hasOwn(s.diagnostics(), 'failure'));
  await s.resume().then(() => assert.fail('must stay terminal'), error => assert.equal(error, undefined));
});
test('native table callbacks and raw fallback forwarders use their distinct exact argument lists', async t => {
  const r = await rig(t), s = r.session;
  for (const [signature, arity] of [['vi', 1], ['vii', 2], ['viii', 3], ['viiii', 4]]) {
    const args = Array.from({ length: arity }, (_, n) => 20 + n); await bounded(s.enqueue(() => s.invokeCallback(signature, arity, args)));
    assert.deepEqual(args.map((_, n) => word(s, 2200 + n * 4)), args);
    await bounded(s.enqueue(() => s.invokeCallback(signature, 50, args)));
    assert.deepEqual(Array.from({ length: arity + 1 }, (_, n) => word(s, 2100 + n * 4)), [50, ...args]);
  }
  await s.invokeCallback('unsupported', 0, []); await assert.rejects(s.invokeCallback('vi', 1, []), /argument|expects|arity/);
});
test('a native table mutation during suspension cannot change the captured root', async t => {
  const g = gate(), r = await rig(t, { payloadGate: g }), s = r.session;
  const pending = s.enqueue(() => s.invokeCallback('vi', 5, [77])); await flush(); assert.equal(s.driver.busy, true); assert.equal(r.guest.parkCount.value, 1);
  r.guest.Lc.set(5, r.guest.replacement); g.resolve(new Uint8Array([1, 2, 3])); await bounded(pending);
  assert.equal(r.guest.parkCount.value, 2); assert.equal(r.guest.replacementCount.value, 0); assert.equal(word(s, 2240), 77);
  await bounded(s.enqueue(() => s.invokeCallback('vi', 5, [88]))); assert.equal(r.guest.replacementCount.value, 1, 'a later independent invocation sees the new slot');
});
test('dispose waits for a native parked callback and releases assets and driver exactly once', async t => {
  const g = gate(), r = await rig(t, { payloadGate: g }), s = r.session;
  const pending = s.enqueue(() => s.invokeCallback('vi', 5, [9])); await flush(); const first = s.dispose(); assert.equal(first, s.dispose());
  assert.equal(s.diagnostics().disposed, true); assert.equal(s.driver.disposed, false, 'the parked callback still owns guest memory');
  g.resolve(new Uint8Array([7])); await bounded(Promise.all([pending, first])); assert.equal(s.driver.disposed, true); assert.equal(r.guest.freeCount.value, 1);
  await assert.rejects(s.assets.resolve('payload.bin'), /disposed/); assert.throws(() => s.instance, /disposed/); await assert.rejects(s.enqueue(() => {}), /disposed/);
});
test('native frame scheduling is serialized behind boot and cancels native handles on pause/dispose', async t => {
  const r = await rig(t, {}, { loop: true }), s = r.session; await bounded(s.boot()); await flush();
  assert.equal(r.guest.frameCount.value, 1); assert.equal(r.frames.size, 1); const [oldId, oldFrame] = [...r.frames][0];
  await s.pause(); assert.ok(r.canceled.includes(oldId), 'pause must actually cancel the platform rAF handle'); assert.equal(r.frames.size, 0);
  oldFrame(); await flush(); assert.equal(r.guest.frameCount.value, 1, 'a late stale callback cannot re-enter');
  await s.resume(); await s.resume(); assert.equal(r.frames.size, 1); const [newId] = [...r.frames][0]; await s.dispose(); assert.ok(r.canceled.includes(newId)); assert.equal(r.frames.size, 0);
});
test('a synchronous platform tick failure is terminal without escaping its native callback', async t => {
  const r = await rig(t, {}, { loop: true, fps: 1 }), s = r.session;
  await bounded(s.boot()); await flush(); assert.equal(r.frames.size, 1);
  const [id, tick] = [...r.frames][0]; r.frames.delete(id);
  const boom = Error('frame clock failed'); r.host.now = () => { throw boom; };
  assert.doesNotThrow(() => tick(), 'the frame callback is fire-and-forget but its failure must be owned');
  await flush(); assert.equal(s.diagnostics().failure, boom); assert.equal(r.notifications.filter(n => n.type === 'fatal').length, 1);
  let ran = false; await assert.rejects(s.enqueue(() => { ran = true; }), e => e === boom); assert.equal(ran, false);
  assert.equal(r.frames.size, 0); await s.resume().then(() => assert.fail('a failed frame is terminal'), e => assert.equal(e, boom));
});
test('a throwing repaint sink does not kill a healthy guest frame', async t => {
  const r = await rig(t, { throwPaint: true }, { loop: true }), s = r.session;
  await bounded(s.boot()); await flush(); const [id, tick] = [...r.frames][0]; r.frames.delete(id);
  assert.doesNotThrow(() => tick()); await flush(); assert.equal(r.guest.frameCount.value, 2);
  assert.equal(r.frames.size, 1); assert.equal(Object.hasOwn(s.diagnostics(), 'failure'), false);
  assert.equal(r.notifications.filter(n => n.type === 'fatal').length, 0);
});
test('an import map inconsistent with the native module is rejected before reading the game', async () => {
  const env = environment(), wasm = nativeModule(), spec = buildSpec(wasm), rows = spec.rows.map((row, n) => ({ ...row, ...(n === 0 ? { name: '_b' } : {}) }));
  await assert.rejects(createArtemisSessionCore(env.host, { wasm, canvas: env.canvas, importMap: { ...spec, rows } }), /import|map|row|match/);
  assert.deepEqual(env.reads, [], 'mismatched metadata must not open the asset seam');
});
test('unknown native imports are refused rather than neutral-stubbed', async () => {
  const env = environment(); await assert.rejects(createArtemisSessionCore(env.host, { wasm: nativeModule({ unknown: true }), canvas: env.canvas }), /unknown_fixture_import/); assert.deepEqual(env.reads, []);
});
