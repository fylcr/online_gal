// Real filesystem/memory and a model browser/store exercise independently compiled shipping services.
// These tests are not a browser media, audible playback, actual-engine or game acceptance gate.
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
const root = resolve(fileURLToPath(new URL('..', import.meta.url))), temp = mkdtempSync(join(tmpdir(), 'artemis-browser-services-'));
process.on('exit', () => { if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-browser-services-')) rmSync(temp, { recursive: true, force: true }); });
const out = join(temp, 'out');
execFileSync(process.execPath, [join(root, 'node_modules/typescript/bin/tsc'), 'packages/runtime-artemis/src/browser-services.ts', '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler', '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck', '--rootDir', 'packages/runtime-artemis/src', '--outDir', out], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/runtime-artemis/node_modules'), join(out, 'node_modules'), 'junction');
const { createArtemisBrowserServices } = await import(pathToFileURL(join(out, 'browser-services.js')));
const { ArtemisFs } = await import(pathToFileURL(join(out, 'host/tree.js')));
const { ArtemisMemory } = await import(pathToFileURL(join(out, 'host/memory.js')));
const text = new TextEncoder();
const gate = () => { let resolve, reject; const promise = new Promise((a, b) => { resolve = a; reject = b; }); void promise.catch(() => {}); return { promise, resolve, reject }; };
const flush = async () => { for (let i = 0; i < 30; i++) await Promise.resolve(); await new Promise(r => setImmediate(r)); };
const bounded = p => new Promise((resolve, reject) => { const timer = setTimeout(() => reject(Error('service test stalled')), 2000); p.then(v => { clearTimeout(timer); resolve(v); }, e => { clearTimeout(timer); reject(e); }); });
function store() {
  const data = new Map(), reads = [], writes = [];
  return { name: 'service-fixture', data, reads, writes, async read(p) { reads.push(p); return data.get(p)?.slice(); }, async write(p, bytes) { writes.push([p, bytes.slice()]); data.set(p, bytes.slice()); }, async remove(p) { data.delete(p); }, async list(prefix = '') { return [...data.keys()].filter(p => p.startsWith(prefix)); } };
}
class Video {
  listeners = new Map(); videoWidth = 1280; videoHeight = 720; pauses = 0; plays = 0; src = ''; attributes = new Map(); autoplay = false;
  setAttribute(n, v) { this.attributes.set(n, v); }
  removeAttribute(n) { this.attributes.delete(n); if (n === 'src') this.src = ''; }
  addEventListener(n, f) { this.listeners.set(n, [...(this.listeners.get(n) || []), f]); }
  removeEventListener(n, f) { this.listeners.set(n, (this.listeners.get(n) || []).filter(v => v !== f)); }
  dispatch(n) { for (const f of this.listeners.get(n) || []) f({ type: n }); }
  pause() { this.pauses++; }
  async play() { this.plays++; }
  load() {}
  remove() {}
}
function rig(t, config = {}) {
  const backing = config.store || store(), fs = new ArtemisFs(), memory = new ArtemisMemory({ memory: new WebAssembly.Memory({ initial: 1 }) });
  fs.mkdirp('/save/game'); fs.makeWritable('/save/game'); fs.createFile('/system.ini', text.encode('pristine')); fs.createFile('/save/other.dat', new Uint8Array([99]));
  const videos = [], uploads = [], dialogs = [], opens = [], logs = [], reads = [];
  const view = { URL, Blob, alert: s => dialogs.push(['alert', s]), confirm: s => { dialogs.push(['confirm', s]); return true; }, open: (...args) => { opens.push(args); return {}; } };
  const gl = { TEXTURE_2D: 3553, RGBA: 6408, UNSIGNED_BYTE: 5121, texImage2D: (...args) => uploads.push(args) };
  const document = { baseURI: 'https://game.invalid/play/', defaultView: view, createElement(name) { assert.equal(name, 'video'); const video = new Video(); videos.push(video); return video; } };
  const canvas = { ownerDocument: document, getContext: kind => kind === 'webgl' ? gl : null };
  const host = { source: { async list() { throw Error('the game must never be enumerated'); } }, store: backing, canvas, surface: { width: 1280, height: 720 }, log: (...args) => { logs.push(args); if (config.throwLog) throw Error('logger refused'); }, notify() {}, requestFrame() {}, now: () => 0, setting() {} };
  let tail = Promise.resolve(), units = 0;
  const core = { memory, fs, assets: { savePath: '/save/game/', async resolve(path) { reads.push(path); return config.payloadGate ? config.payloadGate.promise : new Uint8Array([1, 2, 3]); } }, driver: { async waitUntilIdle() {} }, enqueue(task) { const run = tail.then(async () => { units++; return task(); }); tail = run.then(() => {}, () => {}); return run; } };
  const services = createArtemisBrowserServices(host, canvas);
  t.after(async () => { config.payloadGate?.resolve(new Uint8Array([1])); await services.dispose(); });
  const c = (code, args = []) => services.runAsmConst({ code, signature: args.length ? 'i' : '', args });
  const pointer = (s, p = 1024) => { memory.refresh(); assert.equal(memory.writeUtf8(p, s, text.encode(s).length + 1), text.encode(s).length); return p; };
  return { backing, fs, memory, videos, uploads, dialogs, opens, logs, reads, core, services, c, pointer, get units() { return units; } };
}
const dataOf = (fs, path) => { const n = fs.lookup(path); return n?.data ? [...n.data.readRange(0, n.size)] : null; };
const state = files => ({ version: 1, files, idb: [] });

test('hydrate, snapshot and flush are save-scoped; files and IDB survive a separate host', async t => {
  const r = rig(t); await r.services.attach(r.core); assert.equal(r.backing.reads.length, 1);
  r.fs.mkdirp('/save/game/nested'); r.fs.createFile('/save/game/nested/a.sav', new Uint8Array([0, 1, 255])); r.services.idb.store('db', 'slot', new Uint8Array([4, 5]));
  const snapshot = await r.services.snapshot(); assert.equal(snapshot.version, 1); assert.deepEqual(snapshot.files, [['nested/a.sav', new Uint8Array([0, 1, 255])]]); assert.deepEqual(snapshot.idb, [['db', 'slot', new Uint8Array([4, 5])]]);
  snapshot.files[0][1][0] = 88; snapshot.idb[0][2][0] = 88; assert.deepEqual(dataOf(r.fs, '/save/game/nested/a.sav'), [0, 1, 255]); assert.deepEqual([...r.services.idb.load('db', 'slot')], [4, 5]);
  await r.services.flush(); assert.equal(r.backing.writes.length, 1); assert.match(r.backing.writes[0][0], /^artemis\//);
  const other = rig(t, { store: r.backing }); await other.services.attach(other.core);
  assert.deepEqual(dataOf(other.fs, '/save/game/nested/a.sav'), [0, 1, 255]); assert.deepEqual([...other.services.idb.load('db', 'slot')], [4, 5]);
  assert.deepEqual(dataOf(other.fs, '/system.ini'), [...text.encode('pristine')]); assert.deepEqual(dataOf(other.fs, '/save/other.dat'), [99]);
});
test('restore replaces deleted save files without touching other roots or carrying a borrowed buffer', async t => {
  const r = rig(t); await r.services.attach(r.core); r.fs.createFile('/save/game/old.sav', new Uint8Array([7])); const bytes = new Uint8Array([8, 9]);
  await r.services.restore(state([['new.sav', bytes]])); bytes[0] = 0;
  assert.equal(r.fs.lookup('/save/game/old.sav'), null); assert.deepEqual(dataOf(r.fs, '/save/game/new.sav'), [8, 9]); assert.deepEqual(dataOf(r.fs, '/save/other.dat'), [99]);
  await r.services.flush(); const other = rig(t, { store: r.backing }); await other.services.attach(other.core); assert.equal(other.fs.lookup('/save/game/old.sav'), null);
});
test('malformed and escaping save snapshots are refused before any filesystem effect', async t => {
  const r = rig(t); await r.services.attach(r.core); r.fs.createFile('/save/game/keep.sav', new Uint8Array([3]));
  for (const path of ['../system.ini', '/system.ini', 'C:/outside', 'a/../../outside', 'a\u0000b', 'a\\..\\outside']) {
    await assert.rejects(r.services.restore(state([[path, new Uint8Array([1])]])), /path|invalid|traversal|relative|escape|snapshot/i);
    assert.deepEqual(dataOf(r.fs, '/save/game/keep.sav'), [3]); assert.deepEqual(dataOf(r.fs, '/system.ini'), [...text.encode('pristine')]);
  }
  await assert.rejects(r.services.restore({ version: 2, files: [], idb: [] }), /version|snapshot/i);
});
test('attach is memoized and an EIO during hydration is a real rejection', async t => {
  const r = rig(t), g = gate(); r.backing.read = () => g.promise; const first = r.services.attach(r.core), second = r.services.attach(r.core);
  assert.equal(first, second); assert.equal(r.c(716732), 1); const boom = Error('hydrate EIO'); g.reject(boom);
  await assert.rejects(bounded(first), e => e === boom); assert.equal(r.c(716732), 0);
});
test('flush has a stable cloned snapshot while the store write is pending and rejects the actual write error', async t => {
  const r = rig(t); await r.services.attach(r.core); r.fs.createFile('/save/game/a.sav', new Uint8Array([1]));
  const g = gate(), entered = gate(); let observed; r.backing.write = (_p, bytes) => { observed = bytes.slice(); entered.resolve(); return g.promise; };
  const pending = r.services.flush(); await bounded(entered.promise); assert.equal(r.c(716732), 1);
  r.fs.unlink('/save/game/a.sav'); r.fs.createFile('/save/game/a.sav', new Uint8Array([9])); const stable = observed.slice();
  const boom = Error('write EIO'); g.reject(boom); await assert.rejects(bounded(pending), e => e === boom); assert.deepEqual(observed, stable); assert.equal(r.c(716732), 0);
});
test('the mount ASM refuses foreign roots and does not rehydrate an already attached save root', async t => {
  const r = rig(t); await r.services.attach(r.core); const reads = r.backing.reads.length;
  r.c(716400, [r.pointer('/save/game/')]); await flush(); assert.equal(r.backing.reads.length, reads);
  assert.throws(() => r.c(716400, [r.pointer('/save/other/')]), /save|mount|root|scope/i); assert.equal(r.backing.reads.length, reads);
});
test('video resolves lazily, becomes ready only after both events and uploads the actual element', async t => {
  const r = rig(t); await r.services.attach(r.core); r.c(711012, [r.pointer('movie/op.mp4')]); assert.equal(r.c(710801), 0); await flush();
  assert.deepEqual(r.reads, ['movie/op.mp4']); assert.equal(r.videos.length, 1); const v = r.videos[0]; assert.ok(v.src.startsWith('blob:')); assert.ok(v.attributes.has('playsinline')); assert.equal(v.plays, 1);
  v.dispatch('playing'); assert.equal(r.c(710801), 0); v.dispatch('timeupdate'); assert.equal(r.c(710801), 1);
  assert.equal(r.c(710922), 1280); assert.equal(r.c(710967), 720); r.c(712081); assert.equal(r.uploads.length, 1); assert.equal(r.uploads[0].at(-1), v);
  assert.equal(r.c(710864), 0); v.dispatch('ended'); assert.equal(r.c(710864), 1); r.c(712047); assert.ok(v.pauses >= 1);
  r.c(710692); assert.equal(r.c(710801), 0); assert.equal([...v.listeners.values()].flat().length, 0);
});
test('reset or dispose during video I/O prevents late playback and owned URL leakage', async t => {
  const g = gate(), r = rig(t, { payloadGate: g }); await r.services.attach(r.core); r.c(711012, [r.pointer('late.mp4')]); await flush(); assert.equal(r.reads.length, 1);
  r.c(710692); await r.services.dispose(); g.resolve(new Uint8Array([7])); await flush();
  assert.equal(r.videos.reduce((n, v) => n + v.plays, 0), 0); assert.equal(r.c(710801), 0);
});
test('dialogs, safe new-window requests and unknown hooks have explicit behavior', async t => {
  const r = rig(t); await r.services.attach(r.core); r.c(712896, [r.pointer('中文')]); assert.equal(r.c(712921, [r.pointer('继续？')]), 1);
  assert.deepEqual(r.dialogs, [['alert', '中文'], ['confirm', '继续？']]); r.c(717384, [r.pointer('https://example.test/')]); assert.equal(r.opens.length, 1); assert.match(String(r.opens[0]), /noopener/);
  assert.throws(() => r.c(717384, [r.pointer('javascript:alert(1)')]), /url|protocol|scheme|unsafe|refus/i); assert.equal(r.opens.length, 1);
  assert.throws(() => r.c(12345), /12345|unknown|unsupported/i);
});
test('a failed fire-and-forget flush has an immediate handler even when diagnostics throw', async t => {
  const r = rig(t, { throwLog: true }); await r.services.attach(r.core); r.backing.write = async () => { throw Error('store refused'); };
  assert.doesNotThrow(() => r.c(716748)); await flush(); assert.equal(r.c(716732), 0); assert.ok(r.logs.length > 0);
});
test('relative engine save paths have one canonical filesystem root and retain the original store namespace', async t => {
  const r = rig(t); r.core.assets.savePath = 'gameone'; await r.services.attach(r.core);
  r.fs.createFile('/gameone/slot.sav', new Uint8Array([17])); await r.services.flush();
  assert.equal(r.backing.writes[0][0], 'artemis/gameone/state-v1.json');
  const reads = r.backing.reads.length; assert.doesNotThrow(() => r.c(716400, [r.pointer('gameone/')])); await flush();
  assert.equal(r.backing.reads.length, reads); assert.throws(() => r.c(716400, [r.pointer('/save/game/')]), /save|mount|root|scope/i);
  const snapshot = await r.services.snapshot(); assert.deepEqual(snapshot.files, [['slot.sav', new Uint8Array([17])]]);
});
test('ASM flush snapshots only after the whole guest queue unit enters', async t => {
  const r = rig(t); await r.services.attach(r.core); r.fs.createFile('/save/game/a.sav', new Uint8Array([1]));
  const blocker = gate(), arrived = gate(); t.after(() => blocker.resolve()); const original = r.core.enqueue.bind(r.core);
  r.core.enqueue = task => original(async () => { arrived.resolve(); await blocker.promise; return task(); });
  assert.doesNotThrow(() => r.c(716748)); await flush(); assert.equal(r.backing.writes.length, 0, 'no live tree snapshot or store write may bypass the serializer');
  await bounded(arrived.promise); assert.equal(r.c(716732), 1); r.fs.unlink('/save/game/a.sav'); r.fs.createFile('/save/game/a.sav', new Uint8Array([9]));
  blocker.resolve(); await bounded(r.services.flush()); assert.equal(r.c(716732), 0);
  const other = rig(t, { store: r.backing }); await other.services.attach(other.core); assert.deepEqual(dataOf(other.fs, '/save/game/a.sav'), [9]);
});
test('rejected lazy video I/O is immediately owned even with a throwing logger', async t => {
  const g = gate(), r = rig(t, { payloadGate: g, throwLog: true }); await r.services.attach(r.core);
  assert.doesNotThrow(() => r.c(711012, [r.pointer('broken.mp4')])); await flush(); assert.equal(r.reads.length, 1);
  g.reject(Error('video read EIO')); await flush(); assert.equal(r.videos.reduce((n, v) => n + v.plays, 0), 0);
  assert.equal(r.c(710801), 0); assert.ok(r.logs.some(args => String(args).includes('video read EIO')), 'the actual asynchronous I/O error is diagnosed');
});
test('pause retains the loaded video, URL, listeners and playback position for resume', async t => {
  const r = rig(t); await r.services.attach(r.core); r.c(711012, [r.pointer('op.mp4')]); await flush();
  const v = r.videos[0], url = v.src; v.currentTime = 12.5; v.dispatch('playing'); v.dispatch('timeupdate');
  const listenerCount = [...v.listeners.values()].flat().length; r.services.pause(); assert.equal(v.src, url); assert.equal(v.currentTime, 12.5);
  assert.equal([...v.listeners.values()].flat().length, listenerCount); assert.ok(v.pauses >= 1); assert.throws(() => r.c(712081), /paused|disabled/i);
  await r.services.resume(); assert.equal(r.videos.length, 1); assert.equal(v.src, url); assert.equal(v.currentTime, 12.5); assert.equal(v.plays, 2);
  r.c(712081); assert.equal(r.uploads.at(-1).at(-1), v);
});
test('a guest string in the grown heap is decoded through refreshed views', async t => {
  const r = rig(t); await r.services.attach(r.core); const old = r.memory.u8; r.memory.memory.grow(1);
  const bytes = text.encode('grown 中文\0'); new Uint8Array(r.memory.memory.buffer).set(bytes, 70000);
  assert.equal(old.byteLength, 0); r.c(712896, [70000]); assert.deepEqual(r.dialogs, [['alert', 'grown 中文']]);
});
