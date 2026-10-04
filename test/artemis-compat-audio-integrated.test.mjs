/**
 * Real Node classic worker + a native-shaped fake PCM engine.
 * Proves credit → PCM copy → natural onended → native ACK. Not audible.
 * Run: node --test test/artemis-compat-audio-integrated.test.mjs
 */
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { Worker } from 'node:worker_threads';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const WORKER = join(root, 'packages', 'runtime-artemis', 'src', 'compat', 'compat-worker.js');
const HELPER = join(root, 'packages', 'runtime-artemis', 'src', 'compat', 'worker-audio.js');
const BOOTSTRAP = `
const fs = require('node:fs');
const vm = require('node:vm');
const { fileURLToPath } = require('node:url');
const { parentPort, workerData } = require('node:worker_threads');
const self = globalThis;
self.self = self;
self.postMessage = (message) => parentPort.postMessage(message);
self.importScripts = (...urls) => {
  for (const url of urls) {
    const file = url.startsWith('file:') ? fileURLToPath(url) : url;
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

const temp = mkdtempSync(join(tmpdir(), 'artemis-audio-int-'));
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-audio-int-')) {
    rmSync(temp, { recursive: true, force: true });
  }
});
const out = join(temp, 'out');
execFileSync(process.execPath, [
  join(root, 'node_modules/typescript/bin/tsc'),
  'packages/runtime-artemis/src/compat/index.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck',
  '--rootDir', 'packages/runtime-artemis/src', '--outDir', out,
], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/runtime-artemis/node_modules'), join(out, 'node_modules'), 'junction');
const bootstrap = join(temp, 'bootstrap.cjs');
writeFileSync(bootstrap, BOOTSTRAP);
const { CompatWorkerHost } = await import(pathToFileURL(join(out, 'compat/index.js')).href);

/** Native-shaped fake: one voice, exact int16, ack refuses a missing handle and a backward prefix. */
function fakeGlue() {
  const body = `
var heap = new ArrayBuffer(1 << 20);
var top = 4096;
self.Module.HEAPU8 = new Uint8Array(heap);
self.Module._malloc = function (n) { var p = top; top += n; return p; };
self.Module._free = function () {};
var voices = [{ handle: 1, key: 'bgm', rate: 8000, gain: 500, pan: -250, produced: 0, consumed: 0 }];
var pcm = [0, 16384, -32768, 32767];
function voice(h) { for (var i = 0; i < voices.length; i++) if (voices[i].handle === h) return voices[i]; return null; }
self.Module._artc_web_open = function () { return 1; };
self.Module._artc_web_frame = function () { return 1; };
self.Module._artc_web_started = function () { return 1; };
self.Module._artc_web_pause = function () { return 1; };
self.Module._artc_web_audio_pump = function () { return 1; };
self.Module._artc_web_audio_voice_count = function () { return voices.length; };
self.Module._artc_web_audio_voice_handle = function (i) { return voices[i] ? voices[i].handle : 0; };
self.Module._artc_web_audio_voice_rate = function (h) { var v = voice(h); return v ? v.rate : 0; };
self.Module._artc_web_audio_voice_key = function (h) { var v = voice(h); return v ? v.key : ''; };
self.Module._artc_web_audio_read_stereo = function (h, dest, frames, gainPtr, panPtr) {
  var v = voice(h); if (!v) return -1;
  var n = Math.min(frames, pcm.length / 2);
  var view = new Int16Array(self.Module.HEAPU8.buffer, dest, n * 2);
  for (var i = 0; i < n * 2; i++) view[i] = pcm[i];
  new DataView(self.Module.HEAPU8.buffer).setInt32(gainPtr, v.gain, true);
  new DataView(self.Module.HEAPU8.buffer).setInt32(panPtr, v.pan, true);
  v.produced += n;
  return n;
};
self.acks = [];
self.Module._artc_web_audio_ack = function (h, frames) {
  var v = voice(h);
  self.acks.push([h, frames, !!v]);
  if (!v || frames < v.consumed || frames > v.produced) return -1;
  v.consumed = frames; return 1;
};
self.Module._artc_web_dispose = function () {};
self.Module.cwrap = function (name) { return function () { return self.Module['_' + name].apply(null, arguments); }; };
`;
  const file = join(temp, 'glue.js');
  writeFileSync(file, body + '\nif (self.Module && self.Module.onRuntimeInitialized) self.Module.onRuntimeInitialized();\n');
  return pathToFileURL(file).href;
}

class NodeWorker {
  constructor() {
    this.node = new Worker(bootstrap, { workerData: { script: WORKER } });
    this.listeners = new Map();
    this.node.on('message', (data) => this.fire('message', { data }));
    this.node.on('error', (error) => this.fire('error', error));
  }
  fire(type, event) { for (const fn of this.listeners.get(type) ?? []) fn(event); }
  addEventListener(type, fn) { this.listeners.set(type, [...(this.listeners.get(type) ?? []), fn]); }
  postMessage(message, transfer = []) { this.node.postMessage(message, transfer); }
  terminate() { return this.node.terminate(); }
}

class FakeContext {
  currentTime = 0; state = 'running'; destination = {};
  nodes = [];
  createBuffer(_ch, len, rate) {
    const data = [new Float32Array(len), new Float32Array(len)];
    return { length: len, sampleRate: rate, getChannelData: (n) => data[n] };
  }
  createBufferSource() {
    const n = { buffer: null, onended: null, start() {}, stop() { this.stopped = true; }, connect() {}, disconnect() {} };
    this.nodes.push(n); return n;
  }
  createGain() { return { gain: { value: 1 }, connect() {}, disconnect() {} }; }
  createStereoPanner() { return { pan: { value: 0 }, connect() {}, disconnect() {} }; }
  suspend() { this.state = 'suspended'; return Promise.resolve(); }
  resume() { this.state = 'running'; return Promise.resolve(); }
  close() { this.state = 'closed'; return Promise.resolve(); }
}

const ctx = new FakeContext();
globalThis.AudioContext = function AudioContext() { return ctx; };

const live = new Set();
test.after(async () => { for (const worker of live) await worker.terminate(); });

test('credit pulls a copied block, a natural end ACKs, and a dead handle does not', async () => {
  const worker = new NodeWorker();
  live.add(worker);
  let host = null;
  try {
  host = new CompatWorkerHost({
    source: { name: 'fixture', capabilities: { randomAccess: true, writable: false }, async readRange() { return new Uint8Array(); } },
    createWorker: () => worker,
  });
  await host.load({
    engineJsUrl: fakeGlue(), wasmUrl: 'file:///artemis-compat.wasm', canvas: null, descriptors: [],
    audioHelperUrl: pathToFileURL(HELPER).href,
  });
  const frame = await host.frame();
  assert.equal(frame, true);
  await new Promise((resolve) => setTimeout(resolve, 50));
  assert.equal(ctx.nodes.length, 1, 'one block was scheduled');
  assert.ok(Math.abs(ctx.nodes[0].buffer.getChannelData(1)[0] - 16384 / 32768) < 1e-6, 'PCM was copied, not a heap view');
  const described = host.describe().audio;
  assert.equal(described.connected, true);
  assert.equal(described.realGameVerified, false, 'a fake never claims the game was heard');
  assert.ok(described.peak > 0 && described.peak <= 1);
  const ended = ctx.nodes[0].onended;
  ctx.nodes[0].onended();
  await new Promise((resolve) => setTimeout(resolve, 50));
  assert.equal(host.describe().audio.playedFrames, 2, 'the natural prefix was ACKed');

  await host.setPaused(true);
  assert.equal(ctx.state, 'suspended', 'pause suspends the context after the native ACK');
  ended();
  await new Promise((resolve) => setTimeout(resolve, 20));
  assert.equal(host.describe().audio.playedFrames, 2, 'a paused generation does not ACK again');

  host.clearAudio();
  ended();
  await new Promise((resolve) => setTimeout(resolve, 20));
  assert.equal(host.describe().audio.playedFrames, 0, 'a cleared generation retires its ACK');

  host.clearAudio();
  const again = await host.frame();
  assert.equal(again, true, 'a frame after clear still censuses');
  host.dispose();
  ended();
  await Promise.race([host.whenDisposed(), new Promise((r) => setTimeout(r, 1500))]);
  assert.equal(ctx.state, 'closed', 'dispose closes the owned context');
  } finally {
    try { host?.dispose(); } catch { /* already closed */ }
    live.delete(worker);
    await worker.terminate().catch(() => undefined);
  }
});
