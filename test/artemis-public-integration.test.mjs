// Independent acceptance challenge for the public entry. Real handcrafted native wasm and a
// deterministic DOM/WebAudio model; NOT Binaryen, downloaded-engine, audible or real-game proof.
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const temp = mkdtempSync(join(tmpdir(), 'artemis-public-lead-')), out = join(temp, 'out');
process.on('exit', () => { if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-public-lead-')) rmSync(temp, { recursive: true, force: true }); });
execFileSync(process.execPath, [join(root, 'node_modules/typescript/bin/tsc'), 'packages/engine/core/artemis-compat/src/index.ts', '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler', '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck', '--rootDir', 'packages/engine/core/artemis-compat/src', '--outDir', out], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/engine/core/artemis-compat/node_modules'), join(out, 'node_modules'), 'junction');
const api = await import(pathToFileURL(join(out, 'index.js')));
const enc = new TextEncoder(), dec = new TextDecoder();
const gate = () => { let resolve, reject; const promise = new Promise((a, b) => { resolve = a; reject = b; }); void promise.catch(() => {}); return { promise, resolve, reject }; };
const flush = async () => { for (let n = 0; n < 40; n++) await Promise.resolve(); await new Promise(r => setImmediate(r)); };
const bounded = (promise, label = 'public operation', ms = 2500) => new Promise((resolve, reject) => { const timer = setTimeout(() => reject(Error(`${label} stalled`)), ms); promise.then(v => { clearTimeout(timer); resolve(v); }, e => { clearTimeout(timer); reject(e); }); });
const uleb = n => { const a = []; do { const b = n & 127; n >>>= 7; a.push(b | (n ? 128 : 0)); } while (n); return a; };
const sleb = n => { const a = []; for (;;) { const b = n & 127; n >>= 7; const more = !((n === 0 && !(b & 64)) || (n === -1 && (b & 64))); a.push(b | (more ? 128 : 0)); if (!more) return a; } };
const str = s => [...uleb(enc.encode(s).length), ...enc.encode(s)];
const vec = rows => [...uleb(rows.length), ...rows.flat()];
const section = (id, bytes) => [id, ...uleb(bytes.length), ...bytes];
const c = n => [0x41, ...sleb(n)], lg = n => [0x20, ...uleb(n)], gg = n => [0x23, ...uleb(n)], gs = n => [0x24, ...uleb(n)], call = n => [0x10, ...uleb(n)];
const inc = n => [...gg(n), ...c(1), 0x6a, ...gs(n)];
const store = (address, bytes) => [...c(address), ...bytes, 0x36, 2, 0];
function moduleFixture({ audio = true, loop = true, trap = false } = {}) {
  const types = [], shape = new Map(), imports = [], functions = [], exports = [];
  const type = (arity, result) => { const key = `${arity}/${result}`; if (shape.has(key)) return shape.get(key); const id = types.length; types.push([0x60, ...vec(Array.from({ length: arity }, () => [0x7f])), ...vec(result ? [[0x7f]] : [])]); shape.set(key, id); return id; };
  const index = {};
  for (const [name, arity, result] of [['fa', 4, false], ['_b', 3, false], ['B', 3, true], ['Gc', 5, false], ['Ic', 4, false], ['Pa', 1, true], ['Oa', 2, true], ['Na', 1, true], ['G', 0, true], ['lc', 4, true], ['jc', 4, true], ['kc', 4, true], ['cc', 4, true], ['bc', 4, true], ['ic', 4, true], ['Xb', 1, true]]) { index[name] = imports.length; imports.push([...str('a'), ...str(name), 0, ...uleb(type(arity, result))]); }
  const offset = imports.length;
  imports.push([...str('a'), ...str('a'), 2, 1, ...uleb(3072), ...uleb(65536)]);
  const fn = (name, arity, result, bytes, locals = 0) => { const id = offset + functions.length; functions.push({ type: type(arity, result), body: [...vec(locals ? [[...uleb(locals), 0x7f]] : []), ...bytes, 0x0b] }); exports.push([...str(name), 0, ...uleb(id)]); return id; };
  fn('Nc', 1, true, [...gg(1), 0x22, 1, ...lg(0), ...c(15), 0x6a, ...c(-16), 0x71, 0x6a, ...gs(1), ...lg(1)], 1);
  fn('Oc', 1, false, inc(7)); fn('Pc', 0, true, c(3072));
  fn('Ud', 1, false, [...c(1), ...gs(0)]); fn('Vd', 0, false, [...c(0), ...gs(0)]);
  fn('Wd', 1, false, [...c(2), ...gs(0)]); fn('Xd', 0, false, [...c(0), ...gs(0)]);
  fn('Mc', 0, false, inc(3)); fn('Xc', 0, true, gg(2));
  fn('Yc', 1, false, [...lg(0), ...gs(2)]); fn('Zc', 1, true, [...gg(2), ...lg(0), ...c(15), 0x6a, ...c(-16), 0x71, 0x6b, ...gs(2), ...gg(2)]);
  const audioBoot = audio ? [...c(0), ...call(index.Pa), ...gs(8), ...store(2060, [...gg(8), ...c(0), ...call(index.Oa)]), ...c(2060), 0x28, 2, 0, ...call(index.Na), 0x1a] : [];
  fn('Qc', 2, true, [...inc(4), ...store(2048, [...c(716732), ...c(0), ...c(0), ...call(index.B)]), ...c(4300), ...c(4320), ...c(2052), ...c(2056), ...call(index.Ic), ...audioBoot, ...(trap ? [0x00] : []), ...(loop ? [...c(1), ...c(0), ...c(1), ...call(index._b)] : []), ...c(73)]);
  fn('rd', 1, false, inc(5));
  fn('ed', 4, true, [...inc(6), ...store(2100, lg(1)), ...store(2104, lg(2)), ...store(2108, lg(3)), ...c(1)]);
  const handler = fn('eventHandler', 3, true, [...inc(6), ...store(2100, lg(0)), ...store(2104, lg(1)), ...store(2108, lg(2)), ...c(1)]);
  fn('$c', 3, true, c(0)); fn('ad', 1, true, c(0));
  for (const [name, arity] of [['bd', 1], ['dd', 2], ['ld', 3], ['id', 4]]) fn(name, arity + 1, false, []);
  fn('park', 0, true, [...c(4096), ...c(5120), ...c(5124), ...c(5128), ...call(index.fa), ...gg(0), 0x04, 0x40, ...c(0), 0x0f, 0x0b, ...c(37)]);
  exports.push([...str('Lc'), 1, 0], [...str('arena'), 2, 0]);
  for (const [name, n] of [['ctorCount', 3], ['mainCount', 4], ['frameCount', 5], ['eventCount', 6], ['freeCount', 7]]) exports.push([...str(name), 3, ...uleb(n)]);
  const globals = [0, 8192, 60000, 0, 0, 0, 0, 0, 0].map(value => [0x7f, 1, ...c(value), 0x0b]);
  const data = [[4096, 'payload.bin'], [4200, '#canvas'], [4300, 'db'], [4320, 'slot']].map(([p, text]) => { const bytes = [...enc.encode(text), 0]; return [0, ...c(p), 0x0b, ...uleb(bytes.length), ...bytes]; });
  const bytes = new Uint8Array([0, 97, 115, 109, 1, 0, 0, 0, ...section(1, vec(types)), ...section(2, vec(imports)), ...section(3, vec(functions.map(f => uleb(f.type)))), ...section(4, vec([[0x70, 1, 2, 2]])), ...section(6, vec(globals)), ...section(7, vec(exports)), ...section(9, vec([[0, ...c(1), 0x0b, ...vec([uleb(handler)])]])), ...section(10, vec(functions.map(f => [...uleb(f.body.length), ...f.body]))), ...section(11, vec(data))]);
  assert.equal(WebAssembly.validate(bytes), true, 'actual native fixture validates');
  return new WebAssembly.Module(bytes);
}
class E {
  constructor(type, init = {}) { this.type = type; Object.assign(this, init); this.defaultPrevented = false; }
  preventDefault() { this.defaultPrevented = true; }
}
class Target {
  listeners = new Map(); events = []; parent = null;
  addEventListener(n, f) { this.listeners.set(n, [...(this.listeners.get(n) || []), f]); }
  removeEventListener(n, f) { this.listeners.set(n, (this.listeners.get(n) || []).filter(x => x !== f)); }
  dispatchEvent(e) { this.events.push(e); for (const f of this.listeners.get(e.type) || []) f(e); if (e.bubbles) this.parent?.dispatchEvent(e); return !e.defaultPrevented; }
}
class N { connect(n) { return n; } disconnect() {} }
class Context {
  state = 'running'; destination = new N(); currentTime = 0; sampleRate = 44100; suspends = 0; resumes = 0; closes = 0;
  suspendGate = null; resumeGate = null; closeGate = null;
  createGain() { const n = new N(); n.gain = { value: 1 }; return n; }
  suspend() { this.suspends++; return (this.suspendGate?.promise || Promise.resolve()).then(() => { this.state = 'suspended'; }); }
  resume() { this.resumes++; return (this.resumeGate?.promise || Promise.resolve()).then(() => { this.state = 'running'; }); }
  close() { this.closes++; return (this.closeGate?.promise || Promise.resolve()).then(() => { this.state = 'closed'; }); }
}
function environment(config = {}) {
  const files = config.files || new Map(), reads = [], writes = [], frames = new Map(), canceled = [], contexts = [], logs = [], notifications = [];
  let ticket = 1, paint = 0;
  const view = new Target(); Object.assign(view, { Event: E, MouseEvent: E, WheelEvent: E, KeyboardEvent: E, URL, Blob, requestAnimationFrame(fn) { const n = ticket++; frames.set(n, fn); return n; }, cancelAnimationFrame(n) { canceled.push(n); frames.delete(n); } });
  const doc = new Target(); doc.parent = view; doc.baseURI = 'https://native.invalid/play/'; doc.defaultView = view; doc.activeElement = null; doc.hidden = false; doc.visibilityState = 'visible';
  const canvas = new Target(); canvas.parent = doc; Object.assign(canvas, { width: 1280, height: 720, ownerDocument: doc, getContext: () => null, getBoundingClientRect: () => ({ left: 10, top: 20, width: 640, height: 360 }) });
  doc.querySelector = selector => selector === '#canvas' ? canvas : config.textElement || null;
  doc.createElement = () => { throw Error('unexpected media creation'); };
  const savePath = config.savePath || 'public-native';
  const ini = enc.encode(`[FILE]\nSAVEPATH = ${savePath}\n[WASM]\nWIDTH = 1280\nHEIGHT = 720\n`);
  const source = { name: 'public-native-fixture', capabilities: { randomAccess: true, writable: false }, async list() { throw Error('must never enumerate game'); }, async exists(p) { return p === 'system.ini' || p === 'payload.bin'; }, async readFile(p) { reads.push(p); if (p === 'system.ini') { config.iniEntered?.resolve(); return config.iniGate ? config.iniGate.promise : ini.slice(); } if (p === 'payload.bin') return config.payloadGate ? config.payloadGate.promise : new Uint8Array([8]); throw Error(`unexpected read ${p}`); }, async isDirectory() { return false; }, async size(p) { return p === 'system.ini' ? ini.length : 1; } };
  const store = { name: 'lead', async read(p) { config.readEntered?.resolve(); if (config.readGate) return config.readGate.promise; return files.get(p)?.slice(); }, async write(p, bytes) { writes.push([p, bytes.slice()]); files.set(p, bytes.slice()); if (config.writeGate) return config.writeGate.promise; }, async remove(p) { files.delete(p); }, async list() { return [...files.keys()]; } };
  const factory = () => { const ctx = config.context || new Context(); contexts.push(ctx); return ctx; };
  const host = { canvas, source, store, surface: { width: 1280, height: 720 }, requestFrame() { paint++; }, now: () => 0, log: (...a) => { logs.push(a); if (config.throwLog) throw Error('logger refused'); }, notify: e => notifications.push(e), setting: (_key, fallback) => fallback };
  return { host, canvas, doc, view, ini, store, factory, frames, canceled, contexts, logs, notifications, reads, writes, files, savePath, get paint() { return paint; } };
}
async function rig(t, config = {}, native = {}) {
  const r = environment(config), gameConfig = { wasm: moduleFixture(native), savePath: r.savePath, audioContextFactory: r.factory, ...(config.gameConfig || {}) };
  const runtime = api.artemisProvider.create(r.host, { runtimeId: 'artemis', surface: r.host.surface, mode: 'new', gameConfig });
  t.after(async () => { config.payloadGate?.resolve(new Uint8Array([8])); config.readGate?.resolve(undefined); config.iniGate?.resolve(r.ini); config.context?.suspendGate?.resolve(); config.context?.resumeGate?.resolve(); config.context?.closeGate?.resolve(); runtime.dispose(); await bounded(runtime.whenDisposed(), 'after disposal'); });
  if (config.load !== false) await bounded(runtime.load(), 'public load');
  return { ...r, runtime, get core() { return runtime.session; }, get guest() { return runtime.session?.instance.exports; } };
}
const word = (s, p) => { s.memory.refresh(); return s.memory.i32[p >>> 2]; };
const bytes = (s, p) => { const n = s.fs.lookup(p); return n?.data ? [...n.data.readRange(0, n.size)] : null; };
const persisted = snapshot => enc.encode(api.serialiseArtemisBrowserSnapshot(snapshot));
const saved = (files = [], idb = []) => ({ version: 1, data: { kind: api.ARTEMIS_SAVE_KIND, snapshot: { version: 1, files, idb } } });

test('public provider availability follows configuration and pre-load diagnostics are truthful', async t => {
  api.configureArtemisWasm({ wasm: null }); assert.equal(api.artemisProvider.availability, 'unavailable');
  const r = environment(), runtime = api.artemisProvider.create(r.host, { runtimeId: 'artemis', surface: r.host.surface, mode: 'new' });
  t.after(() => { api.configureArtemisWasm({ wasm: null }); runtime.dispose(); });
  assert.equal(runtime.describe().available, false); await assert.rejects(runtime.load(), /configured|compat/i); assert.equal(runtime.report().severity, 'unsupported');
  // A published legacy bridge no longer makes the provider runnable on its own: the default engine is
  // the compat one, and a `configureArtemisWasm` publication is the diagnostic path, not the router.
  api.configureArtemisWasm({ wasm: moduleFixture({ audio: false, loop: false }) }); assert.equal(api.artemisProvider.availability, 'unavailable');
  api.configureArtemisWasm({ wasm: null }); assert.equal(api.artemisProvider.availability, 'unavailable');
});
test('a game that explicitly names the legacy bridge still gets the legacy runtime', async t => {
  // The diagnostic path is reachable by name and only by name; this is what keeps it meaningful.
  const r = environment();
  const runtime = api.artemisProvider.create(r.host, { runtimeId: 'artemis', surface: r.host.surface, mode: 'new', gameConfig: { wasm: moduleFixture({ audio: false, loop: false }) } });
  t.after(async () => { runtime.dispose(); await bounded(runtime.whenDisposed()); });
  assert.equal(runtime.constructor.name, 'ArtemisRuntime');
  await bounded(runtime.load()); assert.equal(runtime.describe().loaded, true); assert.equal(runtime.session.instance.exports.mainCount.value, 1);
});
test('public load installs real ASM/IDB/audio seams and hydrates before native main', async t => {
  const files = new Map([['artemis/public-native/state-v1.json', persisted({ version: 1, files: [['a.sav', new Uint8Array([7, 8])]], idb: [['db', 'slot', new Uint8Array([4])]] })]]);
  const r = await rig(t, { files }); assert.ok(r.core.instance instanceof WebAssembly.Instance); assert.equal(r.core.idb.load('db', 'slot')[0], 4);
  assert.deepEqual(bytes(r.core, '/public-native/a.sav'), [7, 8]); assert.equal(word(r.core, 2048), 0); assert.equal(word(r.core, 2052), 1, 'main observes pre-hydrated IDB');
  assert.equal(word(r.core, 2056), 0); assert.equal(r.guest.ctorCount.value, 1); assert.equal(r.guest.mainCount.value, 1);
  assert.equal(r.contexts.length, 1, 'configured factory must be forwarded once'); assert.equal(typeof r.core.audio.pause, 'function');
  assert.equal(r.core.bootTable.imports.G(), 0); await flush(); assert.equal(r.frames.size, 1); assert.deepEqual(r.reads, ['system.ini']);
});
test('restore owns one queue entry, clones snapshots, scopes replacement and persists the real state', async t => {
  const r = await rig(t, { savePath: 'gameone/' }); r.core.fs.createFile('/gameone/old.sav', new Uint8Array([1])); r.core.fs.createFile('/tmp/keep', new Uint8Array([99]));
  const data = new Uint8Array([4, 5]), payload = saved([['nested/new.sav', data]], [['db', 'slot', new Uint8Array([6])]]);
  await bounded(r.runtime.restore(payload), 'public restore'); data[0] = 88;
  assert.equal(r.core.fs.lookup('/gameone/old.sav'), null); assert.deepEqual(bytes(r.core, '/gameone/nested/new.sav'), [4, 5]); assert.deepEqual(bytes(r.core, '/tmp/keep'), [99]);
  const snapshot = r.runtime.save(); assert.equal(snapshot.version, 1); assert.deepEqual(snapshot.data.snapshot.files, [['nested/new.sav', new Uint8Array([4, 5])]]);
  snapshot.data.snapshot.files[0][1][0] = 77; assert.deepEqual(bytes(r.core, '/gameone/nested/new.sav'), [4, 5]);
  assert.ok(r.writes.length >= 1, 'restore resolves after persistence'); const parsed = api.parseArtemisBrowserSnapshot(dec.decode(r.writes.at(-1)[1])); assert.deepEqual(parsed.files[0][1], new Uint8Array([4, 5]));
});
test('restore refuses foreign versions and escape paths before mutating any saved data', async t => {
  const r = await rig(t); r.core.fs.createFile('/public-native/keep.sav', new Uint8Array([3]));
  for (const state of [{ ...saved(), version: 2 }, saved([['../system.ini', new Uint8Array([1])]]), { version: 1, data: { kind: 'other', snapshot: { version: 1, files: [], idb: [] } } }]) await assert.rejects(bounded(r.runtime.restore(state)), /version|path|kind|traversal|invalid|escape/);
  assert.deepEqual(bytes(r.core, '/public-native/keep.sav'), [3]); assert.equal(r.writes.length, 0);
});
test('input preserves right-button press/release, one coordinate scaling and healthy event diagnostics', async t => {
  const r = await rig(t), a = r.core.bootTable.imports; assert.equal(a.lc(4200, 9, 0, 1), 0); assert.equal(a.jc(4200, 9, 0, 1), 0); assert.equal(a.cc(1, 9, 0, 1), 0);
  r.runtime.sendInput({ type: 'pointer-down', x: 640, y: 360, button: 2 }); await flush(); const down = r.canvas.events.at(-1);
  assert.equal(down.clientX, 330); assert.equal(down.clientY, 200); assert.equal(down.button, 2); assert.equal(down.buttons, 2); assert.equal(r.guest.eventCount.value, 1);
  r.runtime.sendInput({ type: 'pointer-up', x: 640, y: 360, button: 2 }); await flush(); const up = r.canvas.events.at(-1); assert.equal(up.button, 2); assert.equal(up.buttons, 0); assert.equal(r.guest.eventCount.value, 2);
  r.runtime.sendInput({ type: 'key-down', key: 'Enter', code: 'Enter', repeat: false }); await flush(); assert.equal(r.guest.eventCount.value, 3); assert.equal(r.doc.events.at(-1).keyCode, 13);
  assert.equal(r.core.trace.blocked.some(x => x.name === 'event'), false); assert.equal(r.runtime.report().unsupported.some(x => x.command === 'event'), false);
});
test('pause/dispose synchronously cancel frames and a pending resume cannot revive a later pause', async t => {
  const ctx = new Context(), r = await rig(t, { context: ctx }); await flush(); const before = r.guest.frameCount.value; assert.equal(r.frames.size, 1);
  r.runtime.pause(); assert.equal(r.frames.size, 0, 'pause must stop native frames before returning'); assert.equal(ctx.suspends, 1); await flush(); assert.equal(ctx.state, 'suspended');
  ctx.resumeGate = gate(); r.runtime.resume(); await flush(); assert.equal(ctx.resumes, 1); r.runtime.pause(); ctx.resumeGate.resolve(); await flush(); assert.equal(r.frames.size, 0, 'stale resume cannot undo later pause'); assert.equal(ctx.state, 'suspended', 'a superseded audio resume must repay the later pause too');
  r.runtime.sendInput({ type: 'key-down', key: 'Enter', code: 'Enter', repeat: false }); assert.equal(r.guest.frameCount.value, before);
  r.runtime.resume(); await flush(); assert.equal(r.frames.size, 1); const core = r.core; r.runtime.dispose(); assert.equal(r.frames.size, 0, 'dispose must stop frames before returning');
  await bounded(r.runtime.whenDisposed()); assert.equal(core.driver.disposed, true); assert.equal(ctx.closes, 1); r.runtime.dispose(); await r.runtime.whenDisposed(); assert.equal(ctx.closes, 1);
});
test('dispose during entered hydration owns load, releases the session and never overwrites a failed hydrate', async t => {
  const readGate = gate(), readEntered = gate(), r = await rig(t, { load: false, readGate, readEntered }); const loading = r.runtime.load(); const outcome = loading.then(() => 'resolved', e => e);
  await bounded(readEntered.promise, 'entered hydration'); const core = r.core; assert.ok(core); r.runtime.dispose(); readGate.reject(Error('hydrate EIO challenge'));
  const result = await bounded(outcome, 'failed load'); assert.ok(result instanceof Error); await bounded(r.runtime.whenDisposed()); assert.equal(core.driver.disposed, true); assert.equal(r.writes.length, 0);
  assert.equal(r.runtime.describe().loaded, false); assert.equal(r.contexts.length, 0, 'native main never runs after failed hydration');
});
test('busy native root refuses synchronous snapshots and undefined failure stays terminal in reports', async t => {
  const g = gate(), r = await rig(t, { payloadGate: g }); const wrapped = r.core.driver.wrap('public parked root', r.guest.park), pending = r.core.enqueue(() => wrapped());
  await flush(); assert.equal(r.core.driver.busy, true); const state = r.runtime.save(); assert.ok(state.data.refused || state.data.snapshot === undefined, 'cannot produce a successful half-written snapshot');
  g.resolve(new Uint8Array([1])); await bounded(pending); const failure = r.core.enqueue(() => { throw undefined; }); await failure.then(() => assert.fail('must fail'), e => assert.equal(e, undefined));
  assert.equal(r.runtime.report().severity, 'unsupported'); r.runtime.resume(); await flush(); assert.equal(r.frames.size, 0);
});
test('disposing during actual entered configuration read joins load before announcing disposal', async t => {
  const iniGate = gate(), iniEntered = gate(), r = await rig(t, { load: false, iniGate, iniEntered });
  const loading = r.runtime.load(); const outcome = loading.then(() => 'resolved', e => e); await bounded(iniEntered.promise, 'configuration read entered');
  r.runtime.dispose(); let disposedSettled = false; const finished = r.runtime.whenDisposed().then(v => { disposedSettled = true; return v; });
  await flush(); assert.equal(disposedSettled, false, 'owned but unpublished factory resources must not outlive whenDisposed');
  iniGate.resolve(r.ini); const result = await bounded(outcome); assert.ok(result instanceof Error); await bounded(finished);
  assert.equal(r.runtime.session, null); assert.equal(r.writes.length, 0); assert.equal(r.contexts.length, 0);
});
test('pause records synchronous AudioContext throws without leaking them and still stops GL', async t => {
  const ctx = new Context(), r = await rig(t, { context: ctx }); await flush(); ctx.suspend = () => { throw undefined; };
  assert.doesNotThrow(() => r.runtime.pause()); assert.equal(r.frames.size, 0); await flush(); assert.equal(r.runtime.describe().paused, true);
  assert.ok(r.logs.some(x => String(x[1]).includes('undefined')), 'the exact legal thrown value must be diagnosed');
});
test('direct session pause defeats an in-flight AudioContext resume as well as the GL epoch', async t => {
  const ctx = new Context(), r = await rig(t, { context: ctx }); await bounded(r.core.pause()); assert.equal(ctx.state, 'suspended');
  ctx.resumeGate = gate(); const stale = r.core.resume(); await flush(); assert.equal(ctx.resumes, 1);
  const laterPause = r.core.pause(); ctx.resumeGate.resolve(); await bounded(Promise.all([stale, laterPause]), 'direct audio race');
  assert.equal(r.frames.size, 0); assert.equal(ctx.state, 'suspended', 'epoch guard cannot leave speakers running under a paused GL loop');
});
test('public diagnostics use observed honoured imports and retain explicit undefined failure', async t => {
  const r = await rig(t); const trace = r.core.trace;
  assert.equal(r.runtime.report().supportedCommandCount, trace.honoured.size);
  assert.equal(r.runtime.describe().engine.hasFailure, false);
  assert.equal(r.runtime.describe().engine.failure, undefined, 'healthy diagnostics cannot contain a failure string');
  assert.equal(r.runtime.describe().trace.observedImportCount, trace.counts.size);
  const before = trace.honoured.size; assert.equal(r.core.bootTable.imports.Xb(0), -1);
  assert.equal(trace.honoured.has('Xb'), false); assert.equal(trace.honoured.size, before); assert.equal(r.runtime.report().supportedCommandCount, before);
  const failed = r.core.enqueue(() => { throw undefined; }); await failed.then(() => assert.fail('must reject'), e => assert.equal(e, undefined));
  assert.equal(r.runtime.describe().engine.hasFailure, true); assert.equal(r.runtime.report().severity, 'unsupported'); assert.equal(r.frames.size, 0);
});
test('a newer direct resume wins over stale resume repayment', async t => {
  const ctx = new Context(), r = await rig(t, { context: ctx }); await bounded(r.core.pause());
  ctx.resumeGate = gate(); const older = r.core.resume(); await flush();
  await bounded(r.core.pause()); const newer = r.core.resume(); ctx.resumeGate.resolve();
  await bounded(Promise.all([older, newer]), 'newest direct resume'); await flush();
  assert.equal(ctx.state, 'running'); assert.equal(r.frames.size, 1);
});
test('a newer direct resume waits out an already-entered stale repayment suspension', async t => {
  const ctx = new Context(), r = await rig(t, { context: ctx }); await bounded(r.core.pause());
  ctx.resumeGate = gate(); const older = r.core.resume(); await flush(); await bounded(r.core.pause());
  const entered = gate(); ctx.suspendGate = gate(); const base = ctx.suspend.bind(ctx);
  ctx.suspend = () => { entered.resolve(); return base(); }; ctx.resumeGate.resolve();
  await bounded(entered.promise, 'repayment suspension entered'); const newer = r.core.resume(); await flush();
  ctx.suspendGate.resolve(); await bounded(Promise.all([older, newer]), 'newest resume after entered suspension'); await flush();
  assert.equal(ctx.state, 'running', 'latest resume cannot be undone by an earlier asynchronous suspend'); assert.equal(r.frames.size, 1);
});
test('a per-game legacy bridge boots through the public provider without a compat engine', async t => {
  // Same fixture, reached the same way: a game that names its own bridge. The artefact stays the
  // legacy bridge, so the legacy path keeps its measured coverage instead of being retired by a
  // default that moved.
  const r = environment();
  const runtime = api.artemisProvider.create(r.host, { runtimeId: 'artemis', surface: r.host.surface, mode: 'new', gameConfig: { wasm: moduleFixture({ audio: false, loop: false }) } });
  t.after(async () => { runtime.dispose(); await bounded(runtime.whenDisposed()); });
  await bounded(runtime.load()); assert.equal(runtime.describe().loaded, true); assert.equal(runtime.session.instance.exports.mainCount.value, 1);
});
