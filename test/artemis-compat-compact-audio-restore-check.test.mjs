/**
 * The gap 74f closing named: the integrated test ACKs a voice, but nothing ever restores a save and
 * then asks the engine to census a *new* voice set. This is that one case, over real
 * `CompatWorkerHost` -> real classic worker -> real native ACK, with a fake FS/CABI that actually
 * mutates voices and start counters.
 *
 * Run: node --test test/artemis-compat-compact-audio-restore-check.test.mjs
 */
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { MessageChannel, Worker } from 'node:worker_threads';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const WORKER = join(root, 'packages', 'engine', 'core', 'artemis-compat', 'src', 'compat', 'compat-worker.js');
const HELPER = join(root, 'packages', 'engine', 'core', 'artemis-compat', 'src', 'compat', 'worker-audio.js');

/** Test-only classic-worker bootstrap: real `self`, real `importScripts`, queued early messages. */
const BOOTSTRAP = `
const fs = require('node:fs');
const vm = require('node:vm');
const { fileURLToPath } = require('node:url');
const { parentPort, workerData } = require('node:worker_threads');
const self = globalThis;
const realPost = parentPort.postMessage.bind(parentPort);
let gate = null;
workerData.probe.on('message', (data) => {
  if (typeof data === 'object' && data && data.hold !== undefined) self.ackHold = data.hold | 0;
  if (data && data.release) { const wait = gate; gate = null; if (wait) wait(); }
  workerData.probe.postMessage({ load: self.calls.load, census: self.calls.census, pull: self.calls.pull, ack: self.calls.ack.slice(), voice: self.calls.voice(), ackHold: self.ackHold | 0, gated: !!gate });
});
self.self = self;
self.postMessage = (message) => {
  if (self.ackHold > 0 && message && message.type === 'audio-ack-ok') {
    self.ackHold -= 1;
    return new Promise((resolve) => { gate = resolve; }).then(() => realPost(message));
  }
  realPost(message);
};
self.close = () => parentPort.close();
self.importScripts = (...urls) => {
  for (const url of urls) {
    const text = String(url);
    const file = /^https?:/i.test(text)
      ? fileURLToPath(workerData.glue)
      : text.startsWith('file:') ? fileURLToPath(text) : text;
    vm.runInThisContext(fs.readFileSync(file, 'utf8'), { filename: file });
  }
};
const queued = [];
parentPort.on('message', (data) => {
  if (typeof self.onmessage === 'function') self.onmessage({ data });
  else queued.push(data);
});
require(workerData.script);
for (const data of queued.splice(0)) self.onmessage({ data });
`;

const temp = mkdtempSync(join(tmpdir(), 'artemis-compact-audio-restore-'));
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-compact-audio-restore-')) {
    rmSync(temp, { recursive: true, force: true });
  }
});
const out = join(temp, 'out');
execFileSync(process.execPath, [
  join(root, 'node_modules/typescript/bin/tsc'),
  'packages/engine/core/artemis-compat/src/compat/index.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck',
  '--rootDir', 'packages/engine/core/artemis-compat/src', '--outDir', out,
], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/engine/core/artemis-compat/node_modules'), join(out, 'node_modules'), 'junction');
const BOOTSTRAP_FILE = join(temp, 'bootstrap.cjs');
writeFileSync(BOOTSTRAP_FILE, BOOTSTRAP);
const { CompatWorkerHost } = await import(pathToFileURL(join(out, 'compat/index.js')).href);

/** ARCV v1, exactly what the save test's `arcv` writes. Test bytes, never a user slot. */
function arcv64(pairs) {
  const parts = [Buffer.from('ARCV'), Buffer.alloc(4), Buffer.alloc(4)];
  parts[1].writeUInt32LE(1, 0);
  parts[2].writeUInt32LE(pairs.length, 0);
  for (const [key, value] of pairs) {
    const k = Buffer.from(key), v = Buffer.from(value);
    const kl = Buffer.alloc(4), vl = Buffer.alloc(4);
    kl.writeUInt32LE(k.length, 0); vl.writeUInt32LE(v.length, 0);
    parts.push(kl, k, vl, v);
  }
  const body = Buffer.concat(parts);
  let crc = 0xffffffff;
  for (const byte of body) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit += 1) crc = (crc >>> 1) ^ (0xedb88320 & -(crc & 1));
  }
  const tail = Buffer.alloc(4);
  tail.writeUInt32LE((crc ^ 0xffffffff) >>> 0, 0);
  return Buffer.concat([body, tail]).toString('base64');
}

/**
 * The native stand-in. Every counter the assertions read is mutated here: `census` really changes
 * `voices`, `_artc_web_load` really rewrites the FS slot, and the ACK really refuses an unknown
 * handle, a backward prefix, and a frame count native never produced.
 */
const GLUE = `
var heap = new Uint8Array(1 << 20);
var top = 4096;
var enc = new TextEncoder(), dec = new TextDecoder();
self.Module = globalThis.Module || {};
self.Module.HEAPU8 = heap;
self.Module._malloc = function (n) { var p = top; top += (n + 7) & ~7; return p; };
self.Module._free = function () {};
self.Module.lengthBytesUTF8 = function (s) { return enc.encode(s).length + 1; };
self.Module.UTF8ToString = function (p) { var e = p; while (heap[e]) e++; return dec.decode(heap.subarray(p, e)); };
self.Module.stringToUTF8 = function (s, p) { var b = enc.encode(s); heap.set(b, p); heap[p + b.length] = 0; return b.length + 1; };
self.Module.ccall = function (name, ret, types, args) {
  var fn = self.Module['_' + name];
  if (typeof fn !== 'function') throw new Error('missing _' + name);
  var owned = [];
  var argv = args.map(function (value, i) {
    if (types[i] !== 'string') return value;
    var p = self.Module._malloc(self.Module.lengthBytesUTF8(value));
    self.Module.stringToUTF8(value, p);
    owned.push(p); return p;
  });
  var result = fn.apply(null, argv);
  owned.forEach(function (p) { self.Module._free(p); });
  return result;
};
self.Module.cwrap = function (name, ret, types) {
  return function () { return self.Module.ccall(name, ret, types, Array.prototype.slice.call(arguments)); };
};
self.probe = function (line) { self.postMessage({ type: 'probe', line: line }); };
self.__fs = Object.create(null);
self.__dirs = { '/game': true };
function parentOf(path) { var i = path.lastIndexOf('/'); return i <= 0 ? '/' : path.slice(0, i); }
function listDir(dir) {
  var out = ['.', '..'];
  var prefix = dir === '/' ? '/' : dir + '/';
  var seen = Object.create(null);
  Object.keys(self.__fs).concat(Object.keys(self.__dirs)).forEach(function (path) {
    if (path === dir || path.indexOf(prefix) !== 0) return;
    var rest = path.slice(prefix.length);
    var slash = rest.indexOf('/');
    var name = slash < 0 ? rest : rest.slice(0, slash);
    if (name && !seen[name]) { seen[name] = true; out.push(name); }
  });
  return out;
}
self.Module.FS = {
  mkdirTree: function (path) { self.__dirs[String(path)] = true; },
  writeFile: function (path, data) {
    path = String(path);
    self.__dirs[parentOf(path)] = true;
    self.__fs[path] = Array.prototype.slice.call(data);
  },
  readFile: function (path) {
    if (!(String(path) in self.__fs)) throw new Error('ENOENT: ' + path);
    return Uint8Array.from(self.__fs[String(path)]);
  },
  unlink: function (path) { delete self.__fs[String(path)]; },
  readdir: function (dir) { return listDir(String(dir || '/')); },
  stat: function (path) {
    path = String(path);
    if (self.__dirs[path] && !(path in self.__fs)) return { mode: 16384, size: 0 };
    return { mode: 32768, size: (self.__fs[path] || []).length };
  },
  analyzePath: function (path) { path = String(path); return { exists: path in self.__fs || !!self.__dirs[path] }; },
  rename: function (a, b) { self.__fs[String(b)] = self.__fs[String(a)]; delete self.__fs[String(a)]; }
};
var scenes = {
  one: [{ handle: 1, key: 'voice1', rate: 8000, gain: 500, pan: -250, produced: 2, consumed: 0 }],
  two: [{ handle: 2, key: 'voice2', rate: 8000, gain: 500, pan: 250, produced: 4, consumed: 0, issued: 0 }]
};
var voices = [];
self.currentScene = function () { return voices.length ? voices[0].key : 'none'; };
function voice(h) { for (var i = 0; i < voices.length; i++) if (voices[i].handle === h) return voices[i]; return null; }
self.acks = [];
self.calls = { load: 0, census: 0, pull: 0, ack: [], voice: function () { return voices.map(function (v) { return [v.handle, v.consumed, v.produced]; }); } };
self.Module._artc_web_open = function () { return 1; };
self.Module._artc_web_frame = function () { return 1; };
self.Module._artc_web_started = function () { return self.nativeFrames(); };
self.Module._artc_web_pause = function (v) { self.__nativePaused = !!v; self.probe('pause=' + (v ? 1 : 0)); return 1; };
self.Module._artc_web_audio_pump = function () { self.calls.census += 1; return 1; };
self.Module._artc_web_audio_voice_count = function () { return voices.length; };
self.Module._artc_web_audio_voice_handle = function (i) { return voices[i] ? voices[i].handle : 0; };
self.Module._artc_web_audio_voice_rate = function (h) { var v = voice(h); return v ? v.rate : 0; };
self.Module._artc_web_audio_voice_key = function (h) { var v = voice(h); self.probe('census-key=' + (v ? v.key : 'none')); return v ? v.key : ''; };
self.Module._artc_web_audio_read_stereo = function (h, dest, frames, gainPtr, panPtr) {
  var v = voice(h); if (!v) return -1;
  self.calls.pull += 1;
  self.probe('pull=' + h);
  var issued = v.issued || 0;
  var n = Math.min(frames, 2, v.produced - issued);
  if (n < 0) n = 0;
  v.issued = issued + n;
  var view = new Int16Array(heap.buffer, dest, n * 2);
  for (var i = 0; i < n * 2; i++) view[i] = [0, 16384, -32768, 32767][i % 4];
  new DataView(heap.buffer).setInt32(gainPtr, v.gain, true);
  new DataView(heap.buffer).setInt32(panPtr, v.pan, true);
  return n;
};
self.ackHold = 0;
self.Module._artc_web_audio_ack = function (h, frames) {
  var v = voice(h);
  var code = !v || frames < v.consumed || frames > v.produced ? -1 : 1;
  if (code === 1) v.consumed = frames;
  self.acks.push([h, frames, code]);
  self.calls.ack.push([h, frames, code]);
  self.probe('ack=' + h + '@' + frames + '#' + code);
  return code;
};
self.Module._artc_web_dispose = function () {};
self.Module._artc_web_save = function (file) {
  var name = self.Module.UTF8ToString(file);
  self.__fs['/game/' + name] = Array.prototype.slice.call(self.__slot);
  self.probe('save=' + name);
  return 1;
};
self.Module._artc_web_load = function (file) {
  var name = self.Module.UTF8ToString(file);
  var key = '/game/' + name;
  self.calls.load += 1;
  self.probe('load=' + name + '|' + (key in self.__fs ? 'present' : 'missing'));
  if (!(key in self.__fs)) return 0;
  voices = scenes.two.map(function (v) { return Object.assign({}, v); });
  return 1;
};
self.nativeFrames = function () { return self.__nativeFrames; };
self.setScene = function (name) { voices = scenes[name].map(function (v) { return Object.assign({}, v); }); self.__nativeFrames = 0; };
self.nativeScene = function () { return self.currentScene(); };
self.__nativePaused = false;
self.__nativeFrames = 0;
self.setScene('one');
if (self.Module.onRuntimeInitialized) self.Module.onRuntimeInitialized();
`;

const glueFile = join(temp, 'glue.cjs');
writeFileSync(glueFile, `self.__slot = Uint8Array.from(Buffer.from(${JSON.stringify(arcv64([['scene', 'two']]))}, 'base64'));\n` + GLUE);

/** The browser `Worker` surface over a Node thread. `importScripts` maps the URL onto the glue file. */
class NodeWorker {
  constructor(glueUrl) {
    this.listeners = new Map();
    this.probes = [];

    const channel = new MessageChannel();
    this.probePort = channel.port1;
    this.node = new Worker(BOOTSTRAP_FILE, { workerData: { script: WORKER, glue: glueUrl, probe: channel.port2 }, transferList: [channel.port2] });
    this.node.on('message', (data) => {
      if (data && data.type === 'probe') this.probes.push(String(data.line));
      this.fire('message', { data });
    });
    this.node.on('error', (error) => this.fire('error', error));
  }
  nativeCalls() {
    return new Promise((resolve) => {
      this.probePort.once('message', resolve);
      this.probePort.postMessage(0);
    });
  }
  holdAcks(n) {
    return new Promise((resolve) => {
      this.probePort.once('message', resolve);
      this.probePort.postMessage({ hold: n });
    });
  }
  releaseAcks() {
    return new Promise((resolve) => {
      this.probePort.once('message', resolve);
      this.probePort.postMessage({ release: 1 });
    });
  }
  fire(type, event) { for (const fn of this.listeners.get(type) ?? []) fn(event); }
  addEventListener(type, fn) { this.listeners.set(type, [...(this.listeners.get(type) ?? []), fn]); }
  postMessage(message, transfer = []) {
    this.node.postMessage(message, transfer.filter((value) => value instanceof SharedArrayBuffer));
  }
  terminate() { return this.node.terminate(); }
}

const probes = [];
const ctx = { currentTime: 0, state: 'running', destination: {} };
const sources = [];
ctx.createBuffer = (_ch, len, rate) => {
  const data = [new Float32Array(len), new Float32Array(len)];
  return { length: len, sampleRate: rate, getChannelData: (n) => data[n] };
};
ctx.createBufferSource = () => {
  const node = { buffer: null, onended: null, ended: false, born: -1, start() { this.born = probes.length; }, stop() {}, connect() {}, disconnect() {} };
  sources.push(node);
  return node;
};
ctx.createGain = () => ({ gain: { value: 1 }, connect() {}, disconnect() {} });
ctx.createStereoPanner = () => ({ pan: { value: 0 }, connect() {}, disconnect() {} });
ctx.suspend = () => { ctx.state = 'suspended'; return Promise.resolve(); };
ctx.resume = () => { ctx.state = 'running'; return Promise.resolve(); };
ctx.close = () => { ctx.state = 'closed'; return Promise.resolve(); };
globalThis.AudioContext = function AudioContext() { return ctx; };

const live = new Set();
test.after(async () => {
  for (const worker of live) await worker.terminate().catch(() => undefined);
  live.clear();
});

const flush = async () => { for (let n = 0; n < 40; n += 1) await Promise.resolve(); await new Promise((r) => setTimeout(r, 80)); };

test('a restores fs bytes, censuses the new voice, and never ACKs the retired one', async () => {
  const worker = new NodeWorker(pathToFileURL(glueFile).href);
  live.add(worker);
  worker.probes = probes;
  let host = null;
  try {
    host = new CompatWorkerHost({
      source: {
        name: 'fixture',
        capabilities: { list: true, randomAccess: true, writable: false },
        async list(dir = '') { return dir === '' ? ['root.pfs'] : []; },
        async readFile(path) { throw new Error(`no whole-file read: ${path}`); },
        async readRange(_path, offset, length) { return Uint8Array.from({ length }, (_, i) => (offset + i) % 251); },
        async size() { return 0x100000; },
        async exists() { return true; },
        async isDirectory() { return false; },
      },
      createWorker: () => worker,
    });

    await host.load({
      engineJsUrl: glueFile,
      wasmUrl: 'file:///artemis-compat.wasm',
      canvas: null,
      descriptors: [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: 0x100000 }],
      audioHelperUrl: pathToFileURL(HELPER).href,
    });
    const audioOf = () => host.describe().audio;

    assert.equal(await host.frame(), true);
    await flush();
    assert.equal(audioOf().connected, true, 'the first frame connected audio');
    const voice1 = sources[0];
    assert.ok(voice1, 'voice1 queued a real PCM block');
    const end = (node) => { node.ended = true; node.onended(); };
    end(voice1);
    await flush();
    const beforeRestore = await worker.nativeCalls();
    assert.deepEqual(beforeRestore.ack, [[1, 2, 1]], 'the retired voice ACK returned 1 for exactly 2 frames');
    assert.deepEqual(beforeRestore.voice, [[1, 2, 2]], 'native consumed the retired 2 frames');
    assert.equal(beforeRestore.load, 0);

    const files = await host.checkpoint();
    assert.deepEqual(files.map((f) => f.path), ['platform-slot.dat']);
    end(voice1);
    await flush();

    const restored = await host.restore(files);
    assert.equal(restored.installed, 1);
    assert.deepEqual(probes.filter((p) => p.startsWith('load=')), ['load=platform-slot.dat|present']);
    assert.deepEqual(probes.filter((p) => p.startsWith('save=')), ['save=platform-slot.dat']);
    const loaded = await worker.nativeCalls();
    assert.equal(loaded.load, 1, 'native_load ran once; the reply is not a scene change');
    assert.deepEqual(loaded.ack, [[1, 2, 1]], 'the retired callback did not ACK');
    assert.deepEqual(loaded.voice, [[2, 0, 4]], 'restore installed the new voice at consumed 0 of 4');

    probes.push('restored');
    assert.equal(await host.frame(), true);
    await flush();
    const restoredAt = probes.indexOf('restored');
    const pending = sources.filter((node) => node.born >= restoredAt && typeof node.onended === 'function');
    assert.equal(pending.length, 2, 'the restored frame queued its blocks from the native pull');
    const armed = await worker.holdAcks(1);
    console.log('ACK_ARMED ' + JSON.stringify({ ackHold: armed.ackHold, gated: armed.gated }));
    end(pending[0]);
    await new Promise((r) => setTimeout(r, 80));
    const during = await worker.nativeCalls();
    console.log('ACK_DURING ' + JSON.stringify({ played: audioOf().playedFrames, ack: during.ack, voice: during.voice, gated: during.gated }));
    end(pending[1]);
    const mid = await worker.nativeCalls();
    console.log('ACK_MID ' + JSON.stringify({ played: audioOf().playedFrames, ack: mid.ack, voice: mid.voice, gated: mid.gated }));
    await worker.releaseAcks();
    await flush();
    const after = await worker.nativeCalls();
    console.log('ACK_AFTER ' + JSON.stringify({ played: audioOf().playedFrames, ack: after.ack, voice: after.voice, gated: after.gated }));
    assert.equal(after.load, 1);
    assert.equal(after.pull, beforeRestore.pull + 2, 'the new voice was pulled twice, both from native');
    assert.deepEqual(after.ack, [[1, 2, 1], [2, 2, 1], [2, 4, 1]], 'both new prefixes returned ACK=1; a refusal is not a consume');
    assert.deepEqual(after.voice, [[2, 4, 4]], 'native consumed is 4');
    assert.equal(audioOf().playedFrames, 4, 'host playedFrames is the native ACK=1 cumulative, not a dropped busy receipt');
    const order = probes.slice(probes.indexOf('restored') + 1)
      .map((p) => p.split('=')[0])
      .filter((name) => name === 'census-key' || name === 'pull' || name === 'ack');
    assert.deepEqual(order.filter((name, i) => name !== order[i - 1]), ['census-key', 'pull', 'ack']);
    assert.ok(after.census > beforeRestore.census && after.pull > beforeRestore.pull);
    assert.ok(order.length > 0, 'census/pull/ack order is non-empty');
  } finally {
    try { host?.dispose(); } catch { /* already closed */ }
    if (host) await Promise.race([host.whenDisposed(), new Promise((r) => setTimeout(r, 1500))]);
    live.delete(worker);
    await worker.terminate().catch(() => undefined);
  }
  assert.equal(ctx.state, 'closed');
});
