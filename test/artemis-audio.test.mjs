// Compiled shipping audio with a deterministic WebAudio model; not audible browser/game acceptance.
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
const root = resolve(fileURLToPath(new URL('..', import.meta.url))), temp = mkdtempSync(join(tmpdir(), 'artemis-audio-'));
process.on('exit', () => { if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-audio-')) rmSync(temp, { recursive: true, force: true }); });
const out = join(temp, 'out');
execFileSync(process.execPath, [join(root, 'node_modules/typescript/bin/tsc'), 'packages/runtime-artemis/src/host/audio.ts', '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler', '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck', '--rootDir', 'packages/runtime-artemis/src', '--outDir', out], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
const { createArtemisAudioHost } = await import(pathToFileURL(join(out, 'host/audio.js')));
const { ArtemisMemory } = await import(pathToFileURL(join(out, 'host/memory.js')));
const flush = async () => { for (let i = 0; i < 20; i++) await Promise.resolve(); };
const param = (value = 0) => ({ value, setValueAtTime(v) { this.value = v; }, linearRampToValueAtTime(v) { this.value = v; }, cancelScheduledValues() {} });
class Node {
  connections = []; disconnected = false;
  connect(target) { this.connections.push(target); return target; }
  disconnect() { this.disconnected = true; }
}
class FakeContext {
  currentTime = 0; sampleRate = 1000; state = 'running'; destination = new Node(); buffers = []; nodes = []; gains = []; panners = []; resumes = 0; suspends = 0; closes = 0;
  createBuffer(channels, length, rate) {
    const data = Array.from({ length: channels }, () => new Float32Array(length));
    const buffer = { numberOfChannels: channels, length, sampleRate: rate, duration: length / rate, getChannelData: n => data[n], copyToChannel: (a, n) => data[n].set(a) };
    this.buffers.push(buffer); return buffer;
  }
  createBufferSource() {
    const node = new Node(); Object.assign(node, { context: this, buffer: null, playbackRate: param(1), loop: false, loopStart: 0, loopEnd: 0, onended: null, starts: [], stops: 0, start(when = 0, offset = 0, duration) { this.starts.push([when, offset, duration]); }, stop() { this.stops++; }, finish() { this.onended?.(); } });
    this.nodes.push(node); return node;
  }
  createGain() { const node = new Node(); node.gain = param(1); this.gains.push(node); return node; }
  createPanner() {
    const node = new Node(); Object.assign(node, { positionX: param(), positionY: param(), positionZ: param(), orientationX: param(), orientationY: param(), orientationZ: param(), distanceModel: 'inverse', refDistance: 1, maxDistance: 10000, rolloffFactor: 1, setPosition(x, y, z) { this.positionX.value = x; this.positionY.value = y; this.positionZ.value = z; }, setOrientation(x, y, z) { this.orientationX.value = x; this.orientationY.value = y; this.orientationZ.value = z; } });
    this.panners.push(node); return node;
  }
  async resume() { this.resumes++; this.state = 'running'; }
  async suspend() { this.suspends++; this.state = 'suspended'; }
  async close() { this.closes++; this.state = 'closed'; }
}
function rig(t, options = {}) {
  const wasm = new WebAssembly.Memory({ initial: 1 }), memory = new ArtemisMemory({ memory: wasm }), context = new FakeContext(), reports = [];
  let allocations = 0;
  const host = createArtemisAudioHost({ memory, malloc: n => { allocations++; return 8192; }, contextFactory: () => context, onBlocked: (a, b) => reports.push([a, b]), ...options });
  t.after(() => host.dispose()); const a = host.imports;
  const device = a.Pa(0), id = a.Oa(device, 0); assert.notEqual(device, 0); assert.notEqual(id, 0); assert.equal(a.Na(id), 1);
  const read = p => { memory.refresh(); return memory.i32[p >>> 2]; };
  const ids = values => { memory.refresh(); memory.i32.set(values, 512 >>> 2); return 512; };
  const buffer = (bytes = new Uint8Array([0, 128, 255, 128]), format = 4352, rate = 4) => { a.La(1, 256); const id = read(256); memory.refresh(); memory.u8.set(bytes, 1024); a.Ha(id, format, 1024, bytes.length, rate); return id; };
  const source = () => { a.Ma(1, 260); return read(260); };
  const query = (s, p) => { memory.refresh(); memory.i32[264 >>> 2] = -999; a.oa(s, p, 264); return read(264); };
  return { host, a, wasm, memory, context, reports, buffer, source, ids, query, read, get allocations() { return allocations; } };
}
test('all nineteen actual short imports exist and OpenAL errors clear only when consumed', t => {
  const r = rig(t), names = 'F G Ga Ha Ia Ja Ka La Ma Na Oa Pa R ga oa vc wc xc yc'.split(' ');
  for (const name of names) assert.equal(typeof r.a[name], 'function');
  assert.equal(r.a.G(), 0); r.a.R(999); r.a.wc(999, 0, 0); assert.equal(r.a.G(), 40961); assert.equal(r.a.G(), 0);
  assert.equal(r.a.Na(999), 1, 'the measured SDK binding returns 1 for every nonzero id'); assert.notEqual(r.a.G(), 0);
});
test('PCM mono8 and interleaved stereo16 are copied and deinterleaved from the actual heap', t => {
  const r = rig(t), mono = r.buffer(), monoSource = r.source(); r.a.Ga(monoSource, 4105, mono); r.a.R(monoSource);
  assert.deepEqual([...r.context.nodes.at(-1).buffer.getChannelData(0)], [-1, 0, 127 / 128, 0]);
  const bytes = new Uint8Array(8), view = new DataView(bytes.buffer); [-32768, 32767, 0, -16384].forEach((v, i) => view.setInt16(i * 2, v, true));
  const stereo = r.buffer(bytes, 4355, 2), stereoSource = r.source(); r.a.Ga(stereoSource, 4105, stereo); r.a.R(stereoSource);
  const b = r.context.nodes.at(-1).buffer; assert.equal(b.numberOfChannels, 2);
  assert.deepEqual([...b.getChannelData(0)], [-1, 0]); assert.deepEqual([...b.getChannelData(1)], [32767 / 32768, -0.5]);
  r.memory.u8.fill(0, 1024, 1032); assert.deepEqual([...b.getChannelData(1)], [32767 / 32768, -0.5]); assert.equal(r.a.G(), 0);
});
test('buffer upload refreshes after real growth and rejects malformed full ranges before mutation', t => {
  const r = rig(t); r.a.La(1, 256); const id = r.read(256), stale = r.memory.u8; r.wasm.grow(1);
  new Uint8Array(r.wasm.buffer).set([255, 128], 70000); r.a.Ha(id, 4352, 70000, 2, 2);
  assert.equal(stale.byteLength, 0); const source = r.source(); r.a.Ga(source, 4105, id); r.a.R(source);
  assert.deepEqual([...r.context.nodes.at(-1).buffer.getChannelData(0)], [127 / 128, 0]);
  const before = r.context.buffers.length; r.a.Ha(id, 4355, 70000, 3, 2); assert.equal(r.a.G(), 40963);
  r.a.Ha(id, 4352, 131071, 4, 2); assert.notEqual(r.a.G(), 0); assert.equal(r.context.buffers.length, before);
});
test('a static source creates a real connected buffer source and obeys gain, pitch and ended state', t => {
  const r = rig(t), b = r.buffer(), s = r.source(); r.a.Ga(s, 4105, b); r.a.wc(s, 4106, 0.25); r.a.wc(s, 4099, 2); r.a.R(s);
  const n = r.context.nodes.at(-1); assert.ok(n.buffer); assert.equal(n.starts.length, 1); assert.equal(n.playbackRate.value, 2);
  assert.ok(n.connections.length > 0); assert.equal(r.context.gains.at(-1).gain.value, 0.25); assert.equal(r.query(s, 4112), 4114);
  n.finish(); assert.equal(r.query(s, 4112), 4116); assert.equal(r.a.G(), 0);
});
test('streaming buffers stay queued until processed and explicitly unqueued', t => {
  const r = rig(t), b1 = r.buffer(), b2 = r.buffer(), s = r.source(); r.a.yc(s, 2, r.ids([b1, b2])); assert.equal(r.query(s, 4117), 2); assert.equal(r.query(s, 4118), 0);
  r.a.R(s); const first = r.context.nodes[0]; first.finish(); assert.equal(r.query(s, 4118), 1); assert.equal(r.query(s, 4117), 2);
  r.a.Ia(s, 1, 512); assert.equal(r.read(512), b1); assert.equal(r.query(s, 4117), 1); assert.equal(r.query(s, 4118), 0);
  const second = r.context.nodes.find(n => n !== first && n.buffer === r.context.buffers[1]); assert.ok(second, 'the next actual PCM buffer is scheduled'); second.finish();
  assert.equal(r.query(s, 4118), 1); assert.equal(r.query(s, 4112), 4116); r.a.Ia(s, 1, 512); assert.equal(r.read(512), b2); assert.equal(r.query(s, 4117), 0);
});
test('pause preserves the PCM offset and stale ended callbacks cannot finish a resumed source', t => {
  const r = rig(t), b = r.buffer(), s = r.source(); r.a.Ga(s, 4105, b); r.a.R(s); const old = r.context.nodes.at(-1), ended = old.onended;
  r.context.currentTime = 0.25; r.a.ga(s); assert.equal(r.query(s, 4112), 4115); r.a.R(s); const current = r.context.nodes.at(-1);
  assert.notEqual(current, old); assert.ok(Math.abs(current.starts[0][1] - 0.25) < 1e-6, 'resume starts at the saved offset');
  const nodes = r.context.nodes.length; ended?.(); assert.equal(r.query(s, 4112), 4114); assert.equal(r.query(s, 4118), 0);
  assert.equal(r.context.nodes.length, nodes, 'a stale onended cannot schedule duplicate playback'); assert.equal(r.a.G(), 0);
});
test('queue and unqueue validate every id and output range before mutating source state', t => {
  const r = rig(t), b = r.buffer(), s = r.source(); r.a.yc(s, 2, r.ids([b, 999])); assert.notEqual(r.a.G(), 0); assert.equal(r.query(s, 4117), 0);
  r.a.yc(s, 1, r.ids([b])); r.a.xc(s); assert.equal(r.query(s, 4118), 1); r.a.Ia(s, 1, 65535); assert.notEqual(r.a.G(), 0);
  assert.equal(r.query(s, 4117), 1); assert.equal(r.query(s, 4118), 1); r.a.Ia(s, 1, 512); assert.equal(r.read(512), b);
});
test('a queued PCM buffer cannot be overwritten or deleted while owned by a source', t => {
  const r = rig(t), b = r.buffer(), s = r.source(); r.a.yc(s, 1, r.ids([b])); const before = r.context.buffers.length;
  r.a.Ha(b, 4352, 1024, 4, 4); assert.equal(r.a.G(), 40964); assert.equal(r.context.buffers.length, before);
  r.a.Ka(1, r.ids([b])); assert.equal(r.a.G(), 40964); r.a.xc(s); r.a.Ia(s, 1, 512); r.a.Ka(1, r.ids([b])); assert.equal(r.a.G(), 0);
});
test('cached string allocation refreshes a grown heap and returns a terminated UTF-8 value', t => {
  let memory, wasm, count = 0; const context = new FakeContext(), reports = [];
  wasm = new WebAssembly.Memory({ initial: 1 }); memory = new ArtemisMemory({ memory: wasm });
  const h = createArtemisAudioHost({ memory, contextFactory: () => context, malloc: () => { count++; wasm.grow(1); return 70000; }, onBlocked: (...a) => reports.push(a) }); t.after(() => h.dispose());
  const device = h.imports.Pa(0), id = h.imports.Oa(device, 0); assert.equal(h.imports.Na(id), 1);
  const p = h.imports.F(45057); assert.equal(p, 70000); memory.refresh(); assert.ok(memory.utf8(p).length > 0); assert.equal(h.imports.F(45057), p); assert.equal(count, 1); assert.deepEqual(reports, []);
});
test('host pause/resume uses the real context and disposal closes once and invalidates late ended', async t => {
  const r = rig(t), b = r.buffer(), s = r.source(); r.a.Ga(s, 4105, b); r.a.R(s); const node = r.context.nodes.at(-1), ended = node.onended;
  await r.host.pause(); assert.equal(r.context.suspends, 1); await r.host.resume(); assert.ok(r.context.resumes >= 1);
  await r.host.dispose(); await r.host.dispose(); assert.equal(r.context.closes, 1); assert.ok(node.stops >= 1); assert.equal(node.disconnected, true); assert.doesNotThrow(() => ended?.());
  const before = r.context.nodes.length; r.a.R(s); assert.equal(r.context.nodes.length, before, 'a disposed host cannot create new nodes');
});
