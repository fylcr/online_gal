import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { createContext, runInContext } from 'node:vm';
import test from 'node:test';

const source = readFileSync(new URL('../packages/engine/core/artemis-compat/src/compat/worker-audio.js', import.meta.url), 'utf8');
const sandbox = { self: {} };
createContext(sandbox);
runInContext(source, sandbox, { filename: 'worker-audio.js' });
const createArtcAudioBridge = sandbox.self.createArtcAudioBridge;

const IDENTS = [
  'artc_web_audio_pump', 'artc_web_audio_voice_count', 'artc_web_audio_voice_handle',
  'artc_web_audio_voice_rate', 'artc_web_audio_voice_key', 'artc_web_audio_read_stereo',
  'artc_web_audio_ack', 'artc_web_audio_played_frames',
];

function harness(opts = {}) {
  const voices = (opts.voices || [{ handle: 1, key: 'bgm', rate: 44100, gain: 250, pan: -250 }])
    .map((voice) => ({ produced: 0, consumed: 0, gain: 250, pan: -250, ...voice }));
  const byHandle = new Map(voices.map((voice) => [voice.handle, voice]));
  const state = { reads: [], mallocs: [], frees: [], idents: [], acks: [], pumps: 0, counts: 0, handles: 0, rates: 0, played: 0 };
  const Module = {};
  const heapBuffer = new ArrayBuffer(1 << 20);
  Module.HEAPU8 = new Uint8Array(heapBuffer);
  let top = 4096;
  const encode = (text) => new TextEncoder().encode(text);
  const put = (addr, text) => {
    const bytes = encode(text);
    Module.HEAPU8.set(bytes, addr);
    Module.HEAPU8[addr + bytes.length] = 0;
    return addr;
  };
  voices.forEach((voice, index) => { voice.ptr = put(256 + index * 64, voice.key); });
  Module.UTF8ToString = (p) => {
    if (typeof p !== 'number' || !p) throw new Error('UTF8ToString wants a pointer, got ' + p);
    const heapView = Module.HEAPU8;
    let end = p;
    while (heapView[end]) end++;
    return new TextDecoder().decode(heapView.subarray(p, end));
  };
  Module.stringToUTF8 = (text, p) => {
    const bytes = encode(text);
    Module.HEAPU8.set(bytes, p);
    Module.HEAPU8[p + bytes.length] = 0;
    return bytes.length + 1;
  };
  Module.lengthBytesUTF8 = (text) => encode(text).length + 1;
  Module._malloc = (n) => {
    state.mallocs.push(n);
    if (opts.malloc) return opts.malloc(n, Module);
    const p = top;
    const next = p + ((n + 7) & ~7);
    if (!Module.HEAPU8 || next > Module.HEAPU8.byteLength) return 0;
    top = next;
    return p;
  };
  Module._free = (p) => { state.frees.push(p); };
  Module.cwrap = (name, ret, types) => {
    state.idents.push(name);
    return function () {
      const args = Array.prototype.slice.call(arguments);
      const owned = [];
      const argv = args.map((value, i) => {
        if (!types || types[i] !== 'string') return value;
        const p = Module._malloc(Module.lengthBytesUTF8(value));
        Module.stringToUTF8(value, p);
        owned.push(p);
        return p;
      });
      const fn = Module['_' + name];
      if (typeof fn !== 'function') throw new Error('missing _' + name);
      const result = fn.apply(null, argv);
      owned.forEach((p) => Module._free(p));
      return ret === 'string' ? Module.UTF8ToString(result) : result;
    };
  };
  const native = {
    artc_web_audio_pump: () => { state.pumps++; return opts.pump === undefined ? 1 : opts.pump; },
    artc_web_audio_voice_count: () => { state.counts++; return opts.count === undefined ? voices.length : opts.count; },
    artc_web_audio_voice_handle: (index) => { state.handles++; return voices[index] ? voices[index].handle : 0; },
    artc_web_audio_voice_rate: (handle) => {
      state.rates++;
      if (opts.rate) return opts.rate(handle);
      const voice = byHandle.get(handle);
      return voice ? voice.rate : 0;
    },
    artc_web_audio_voice_key: (handle) => {
      const voice = byHandle.get(handle);
      return voice ? voice.ptr : 0;
    },
    artc_web_audio_read_stereo: (handle, dest, frames, gainPtr, panPtr) => {
      state.reads.push({ handle, dest, frames });
      if (opts.read) return opts.read({ handle, dest, frames, gainPtr, panPtr, Module, byHandle });
      const voice = byHandle.get(handle);
      if (!voice) return -1;
      const view = Module.HEAPU8;
      const pcm = new Int16Array(view.buffer, view.byteOffset + dest, frames * 2);
      for (let i = 0; i < pcm.length; i++) pcm[i] = i + 1;
      const data = new DataView(view.buffer, view.byteOffset);
      data.setInt32(gainPtr, voice.gain, true);
      data.setInt32(panPtr, voice.pan, true);
      voice.produced += frames;
      return frames;
    },
    artc_web_audio_ack: (handle, n) => {
      state.acks.push(n);
      if (opts.ack) return opts.ack(handle, n);
      const voice = byHandle.get(handle);
      if (!voice || n > voice.produced || n < voice.consumed) return -1;
      voice.consumed = n;
      return 1;
    },
    artc_web_audio_played_frames: (keyPtr) => {
      state.played++;
      const key = Module.UTF8ToString(keyPtr);
      const voice = voices.find((item) => item.key === key);
      return voice ? voice.consumed : 0;
    },
  };
  for (const ident of IDENTS) {
    Module['__' + ident] = () => { throw new Error('double underscore ' + ident); };
    Module[ident] = () => { throw new Error('unprefixed ' + ident); };
    if (!opts.omit || !opts.omit.includes(ident)) Module['_' + ident] = native[ident];
  }
  return { Module, state, byHandle, voices, bridge: createArtcAudioBridge(Module) };
}

test('cwrap receives the C ident and string returns round-trip as UTF-8', () => {
  const { bridge, state, byHandle } = harness();
  for (const ident of state.idents) assert.equal(ident.startsWith('_'), false, ident);
  assert.ok(state.idents.includes('artc_web_audio_voice_key'));
  const blocks = bridge.pull([{ handle: 1, frames: 4, rate: NaN, key: 'lie' }]);
  assert.equal(blocks.length, 1);
  assert.equal(blocks[0].rate, 44100);
  assert.equal(blocks[0].key, 'bgm');
  assert.equal(blocks[0].samples.length, 8);
  assert.equal(blocks[0].samples[0], 1);
  assert.equal(blocks[0].gain1000, 250);
  assert.equal(blocks[0].pan1000, -250);
  assert.equal(byHandle.get(1).produced, 4);
  bridge.dispose();
});

test('census reports real voices and does not invent an empty success', () => {
  const two = harness({
    voices: [
      { handle: 1, key: 'bgm', rate: 44100 },
      { handle: 2, key: 'se', rate: 0 },
      { handle: 3, key: 'voice', rate: 384000 },
    ],
  });
  const census = two.bridge.census();
  assert.equal(census.available, true);
  assert.equal(census.pumped, true);
  assert.deepEqual(Array.from(census.voices, (voice) => voice.handle), [1, 3]);
  assert.deepEqual(Array.from(census.voices, (voice) => voice.key), ['bgm', 'voice']);
  assert.equal(census.unavailable.length, 0);
  const idle = harness({ pump: 0 });
  const quiet = idle.bridge.census();
  assert.equal(quiet.available, true);
  assert.equal(quiet.voices.length, 0);
  assert.equal(quiet.pumped, false);
  assert.equal(idle.state.counts, 0);
  const none = harness({ count: 0 });
  assert.equal(none.bridge.census().voices.length, 0);
  assert.equal(none.bridge.census().available, true);
});

test('a missing required export is unavailable and pull does not decode', () => {
  const broken = harness({ omit: ['artc_web_audio_voice_count'] });
  const census = broken.bridge.census();
  assert.equal(census.available, false);
  assert.equal(census.voices, undefined);
  assert.ok(census.unavailable.includes('artc_web_audio_voice_count'));
  assert.equal(broken.state.pumps, 0);
  assert.throws(() => broken.bridge.pull([{ handle: 1, frames: 8 }]), /artc_web_audio_voice_count is unavailable/);
  assert.equal(broken.state.reads.length, 0);
  assert.equal(broken.byHandle.get(1).produced, 0);
  const noRead = harness({ omit: ['artc_web_audio_read_stereo'] });
  assert.equal(noRead.bridge.census().available, true);
  assert.throws(() => noRead.bridge.pull([{ handle: 1, frames: 4 }]), /artc_web_audio_read_stereo is unavailable/);
  assert.equal(noRead.state.reads.length, 0);
  const optional = harness({ omit: ['artc_web_audio_played_frames'] });
  const played = optional.bridge.playedFrames('bgm');
  assert.equal(played.available, false);
  assert.equal(Object.hasOwn(played, 'frames'), false);
  assert.match(played.unavailable, /artc_web_audio_played_frames/);
  assert.equal(optional.bridge.pull([{ handle: 1, frames: 2 }]).length, 1);
  assert.equal(optional.bridge.census().available, true);
});

test('pump, count, and rate keep NaN, fractions, and huge values', () => {
  for (const pump of [1.5, NaN, 2]) {
    const box = harness({ pump });
    assert.throws(() => box.bridge.census(), (err) => String(err.message).includes('answered ' + pump));
    assert.equal(box.state.counts, 0);
  }
  for (const count of [1.5, NaN, -1, 257, 1e21]) {
    const box = harness({ count });
    assert.throws(() => box.bridge.census(), /artc_web_audio_voice_count answered/);
    assert.equal(box.state.handles, 0);
  }
  for (const rate of [44100.5, NaN, -3, 384001, 1e21]) {
    const box = harness({ rate: () => rate });
    assert.throws(() => box.bridge.census(), (err) => String(err.message).includes('answered ' + rate));
    assert.equal(box.state.reads.length, 0);
    const pullBox = harness({ rate: () => rate });
    assert.throws(() => pullBox.bridge.pull([{ handle: 1, frames: 4 }]), (err) => String(err.message).includes('answered ' + rate));
    assert.equal(pullBox.state.reads.length, 0);
    assert.equal(pullBox.byHandle.get(1).produced, 0);
  }
  const edge = harness({ voices: [{ handle: 1, key: 'bgm', rate: 1 }] });
  assert.equal(edge.bridge.census().voices[0].rate, 1);
});

test('the whole request list is validated before any decode', () => {
  const good = { handle: 1, frames: 8 };
  const cases = [
    [good, { handle: 1.5, frames: 8 }],
    [good, { handle: NaN, frames: 8 }],
    [good, { handle: 0, frames: 8 }],
    [good, { handle: Number.MAX_SAFE_INTEGER + 1, frames: 8 }],
    [good, { handle: 1, frames: 8 }],
    [{ handle: 1, frames: 0 }, { handle: NaN, frames: 1 }],
    [{ handle: 1, frames: 4097 }],
    [{ handle: 1, frames: 1.5 }],
    [{ handle: 1, frames: NaN }],
    [{ handle: 1, frames: -1 }],
    Array.from({ length: 257 }, (_, index) => ({ handle: index + 1, frames: 1 })),
  ];
  for (const requests of cases) {
    const box = harness();
    assert.throws(() => box.bridge.pull(requests));
    assert.equal(box.state.reads.length, 0);
    assert.equal(box.state.rates, 0);
    assert.equal(box.byHandle.get(1).produced, 0);
  }
  const box = harness();
  assert.throws(() => box.bridge.pull('nope'), /array/);
  assert.equal(box.state.reads.length, 0);
  assert.equal(box.bridge.pull([]).length, 0);
});

test('produced stays 0 until credit and consumed stays 0 until ACK', () => {
  const box = harness();
  assert.equal(box.bridge.pull([{ handle: 1, frames: 0 }]).length, 0);
  assert.equal(box.state.reads.length, 0);
  assert.equal(box.state.mallocs.length, 0);
  assert.equal(box.byHandle.get(1).produced, 0);
  const eof = harness({ read: () => 0 });
  assert.equal(eof.bridge.pull([{ handle: 1, frames: 8 }]).length, 0);
  assert.equal(eof.byHandle.get(1).produced, 0);
  assert.equal(eof.state.reads.length, 1);
  const credit = harness();
  const blocks = credit.bridge.pull([{ handle: 1, frames: 8, rate: 1, key: 'lie' }]);
  assert.equal(blocks.length, 1);
  assert.equal(credit.byHandle.get(1).produced, 8);
  assert.equal(credit.byHandle.get(1).consumed, 0);
  assert.equal(credit.state.played, 0);
  credit.bridge.ack(1, 8);
  assert.equal(credit.byHandle.get(1).consumed, 8);
  assert.equal(credit.state.played, 0);
});

test('a bad got is an error and not a silent block', () => {
  const refused = harness({ read: () => -1 });
  assert.throws(() => refused.bridge.pull([{ handle: 1, frames: 8 }]), /with -1/);
  assert.equal(refused.byHandle.get(1).produced, 0);
  const over = harness({ read: () => 9 });
  assert.throws(() => over.bridge.pull([{ handle: 1, frames: 8 }]), /wrote 9 frames of 8/);
  const fraction = harness({ read: () => 4.5 });
  assert.throws(() => fraction.bridge.pull([{ handle: 1, frames: 8 }]), /answered 4\.5/);
  const nan = harness({ read: () => NaN });
  assert.throws(() => nan.bridge.pull([{ handle: 1, frames: 8 }]), /answered NaN/);
  const huge = harness({ read: () => 1e21 });
  assert.throws(() => huge.bridge.pull([{ handle: 1, frames: 8 }]), /wrote 1e\+21 frames of 8/);
});

test('PCM is copied out of the heap after a grow, and gain/pan use the C++ clamp', () => {
  const box = harness({
    read({ dest, frames, gainPtr, panPtr, Module, byHandle: voices }) {
      const old = Module.HEAPU8;
      const grown = new Uint8Array(old.byteLength + 65536);
      grown.set(old);
      const pcm = new Int16Array(grown.buffer, dest, frames * 2);
      for (let i = 0; i < pcm.length; i++) pcm[i] = i + 1;
      const data = new DataView(grown.buffer);
      data.setInt32(gainPtr, 321, true);
      data.setInt32(panPtr, -77, true);
      old.fill(0x5a);
      Module.HEAPU8 = grown;
      voices.get(1).produced += frames;
      return frames;
    },
  });
  const [block] = box.bridge.pull([{ handle: 1, frames: 8 }]);
  assert.equal(block.samples[0], 1);
  assert.equal(block.samples[block.samples.length - 1], 16);
  assert.equal(block.samples.buffer.byteLength, 32);
  assert.notEqual(block.samples.buffer, box.Module.HEAPU8.buffer);
  assert.equal(block.gain1000, 321);
  assert.equal(block.pan1000, -77);
  box.Module.HEAPU8.fill(0);
  assert.equal(block.samples[0], 1);
  const wild = harness({
    voices: [{ handle: 1, key: 'bgm', rate: 48000, gain: 5000, pan: -4000 }],
  });
  const [clamped] = wild.bridge.pull([{ handle: 1, frames: 1 }]);
  assert.equal(clamped.rate, 48000);
  assert.equal(clamped.gain1000, 1000);
  assert.equal(clamped.pan1000, -1000);
});

test('null malloc, a null heap, and an out-of-span block are refused before decode', () => {
  const empty = harness({ malloc: () => 0 });
  assert.throws(() => empty.bridge.pull([{ handle: 1, frames: 4 }]), /malloc refused/);
  assert.equal(empty.state.frees.length, 0);
  assert.equal(empty.state.reads.length, 0);
  const blind = harness();
  blind.Module.HEAPU8 = null;
  assert.throws(() => blind.bridge.pull([{ handle: 1, frames: 4 }]), /heap unavailable/);
  assert.equal(blind.state.mallocs.length, 0);
  assert.equal(blind.state.reads.length, 0);
  const oob = harness({ malloc: (n, Module) => Module.HEAPU8.byteLength });
  assert.throws(() => oob.bridge.pull([{ handle: 1, frames: 4 }]), /malloc span/);
  assert.deepEqual(oob.state.frees, [1 << 20]);
  assert.equal(oob.state.reads.length, 0);
  const odd = harness({ malloc: () => 4097 });
  assert.throws(() => odd.bridge.pull([{ handle: 1, frames: 4 }]), /malloc span/);
  assert.deepEqual(odd.state.frees, [4097]);
  assert.equal(odd.state.reads.length, 0);
});

test('the PCM slab is malloc\'d once and freed once', () => {
  const box = harness();
  box.bridge.pull([{ handle: 1, frames: 4 }]);
  box.bridge.pull([{ handle: 1, frames: 4 }]);
  assert.deepEqual(box.state.mallocs, [4096 * 4 + 8]);
  const slab = box.state.frees.slice();
  box.bridge.dispose();
  box.bridge.dispose();
  assert.equal(box.state.frees.length, 1);
  assert.equal(slab.length, 0);
  assert.throws(() => box.bridge.pull([{ handle: 1, frames: 1 }]), /disposed/);
  assert.throws(() => box.bridge.census(), /disposed/);
  assert.throws(() => box.bridge.ack(1, 0), /disposed/);
});

test('ACK accepts only a native 1 and never moves past produced', () => {
  const box = harness({ read: ({ frames, byHandle: voices }) => { voices.get(1).produced += 4; return 4; } });
  box.bridge.pull([{ handle: 1, frames: 8 }]);
  assert.equal(box.byHandle.get(1).produced, 4);
  assert.equal(box.byHandle.get(1).consumed, 0);
  assert.throws(() => box.bridge.ack(1, 5), /with -1/);
  assert.equal(box.byHandle.get(1).consumed, 0);
  const calls = box.state.acks.length;
  for (const bad of [4.5, NaN, -1, Number.MAX_SAFE_INTEGER + 1]) {
    assert.throws(() => box.bridge.ack(1, bad), /bad cumulative frames/);
  }
  assert.equal(box.state.acks.length, calls);
  const receipt = box.bridge.ack(1, 4);
  assert.equal(receipt.handle, 1);
  assert.equal(receipt.frames, 4);
  assert.equal(box.byHandle.get(1).consumed, 4);
  assert.equal(box.state.played, 0);
  assert.throws(() => box.bridge.ack(1, 3), /with -1/);
  assert.equal(box.byHandle.get(1).consumed, 4);
  const clock = box.bridge.playedFrames('bgm');
  assert.equal(clock.available, true);
  assert.equal(clock.frames, 4);
  assert.equal(box.bridge.playedFrames('missing').frames, 0);
  const fuzzy = harness({ ack: () => 1.5 });
  assert.throws(() => fuzzy.bridge.ack(1, 0), /with 1\.5/);
  assert.equal(fuzzy.byHandle.get(1).consumed, 0);
  const nan = harness({ ack: () => NaN });
  assert.throws(() => nan.bridge.ack(1, 0), /with NaN/);
  assert.equal(nan.byHandle.get(1).consumed, 0);
});

test('the helper does not schedule a frame or decode', () => {
  assert.doesNotMatch(source, /(?<!\|)\| *0/);
  assert.doesNotMatch(source, /requestAnimationFrame|setTimeout|setInterval|decodeAudioData|decodeFile|AudioContext/);
});
