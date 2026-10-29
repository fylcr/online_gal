// Fake AudioContext for the main-thread per-voice relay. Not audible acceptance.
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const temp = mkdtempSync(join(tmpdir(), 'artemis-relay-'));
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-relay-')) rmSync(temp, { recursive: true, force: true });
});
const out = join(temp, 'out');
execFileSync(process.execPath, [
  join(root, 'node_modules/typescript/bin/tsc'),
  'packages/engine/core/artemis-compat/src/compat/audio-relay.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck',
  '--rootDir', 'packages/engine/core/artemis-compat/src', '--outDir', out,
], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
const { AudioRelay } = await import(pathToFileURL(join(out, 'compat/audio-relay.js')));

const param = (value = 0) => ({ value });
class Node {
  connect(t) { this.connected = t; return t; }
  disconnect() { this.connected = null; }
}
/** The node's own onended may have been cleared by drop/stop — the test calls saved callbacks. */
const fire = call => { assert.equal(typeof call, 'function', 'no internal callback is registered'); call(); };
const latch = ctx => assert.equal(ctx.state, 'suspended');

class FakeContext {
  /** Browsers start a fresh AudioContext suspended until a gesture resumes it. */
  currentTime = 0; state = 'suspended'; destination = new Node();
  buffers = []; nodes = []; gains = []; pans = [];
  resumes = 0; suspends = 0; closes = 0;
  reject = { suspend: null, resume: null };
  createBuffer(ch, len, rate) {
    const data = Array.from({ length: ch }, () => new Float32Array(len));
    const b = { numberOfChannels: ch, length: len, sampleRate: rate, getChannelData: n => data[n] };
    this.buffers.push(b); return b;
  }
  createBufferSource() {
    const n = new Node();
    n.buffer = null; n.onended = null; n.starts = []; n.stopped = false; n.disconnected = 0;
    n.start = w => n.starts.push(w);
    n.stop = () => { n.stopped = true; };
    n.disconnect = () => { n.disconnected++; n.connected = null; };
    this.nodes.push(n); return n;
  }
  createGain() { const n = new Node(); n.gain = param(1); n.disconnected = 0; n.disconnect = () => { n.disconnected++; n.connected = null; }; this.gains.push(n); return n; }
  createStereoPanner() { const n = new Node(); n.pan = param(0); n.disconnected = 0; n.disconnect = () => { n.disconnected++; n.connected = null; }; this.pans.push(n); return n; }
  /** Real browsers fire onstatechange; tests drive it explicitly. */
  setState(s) { this.state = s; this.onstatechange?.(); }
  suspend() { this.suspends++; this.state = 'suspended'; return this.reject.suspend ?? Promise.resolve(); }
  resume() { this.resumes++; this.state = 'running'; return this.reject.resume ?? Promise.resolve(); }
  close() { this.closes++; this.state = 'closed'; return Promise.resolve(); }
}
const pcm = (frames, fill = 0) => { const s = new Int16Array(frames * 2); s.fill(fill); return s; };
/** A context that is already playing, i.e. what unlock() leaves behind. */
const playing = () => { const c = new FakeContext(); c.state = 'running'; return c; };
const push = (r, handle, frames, extra = {}) => r.push({ handle, rate: 1000, samples: pcm(frames, 16384), gain1000: 500, pan1000: -250, ...extra });
const full = (rate, frames) => ({ handle: 1, rate, samples: pcm(frames), gain1000: 0, pan1000: 0 });

test('bad input is refused before a context exists', () => {
  let made = 0;
  const r = new AudioRelay({ contextFactory: () => { made++; return new FakeContext(); } });
  assert.throws(() => r.push({ handle: 0, rate: 1000, samples: pcm(2), gain1000: 0, pan1000: 0 }));
  assert.throws(() => r.push({ handle: 1.5, rate: 1000, samples: pcm(2), gain1000: 0, pan1000: 0 }));
  assert.throws(() => r.push({ handle: 1, rate: 0, samples: pcm(2), gain1000: 0, pan1000: 0 }));
  assert.throws(() => r.push({ handle: 1, rate: NaN, samples: pcm(2), gain1000: 0, pan1000: 0 }));
  assert.throws(() => r.push({ handle: 1, rate: 1000, samples: pcm(4097), gain1000: 0, pan1000: 0 }));
  assert.throws(() => r.push({ handle: 1, rate: 1000, samples: new Int16Array(3), gain1000: 0, pan1000: 0 }));
  assert.throws(() => r.push({ handle: 1, rate: 1000, samples: pcm(2), gain1000: 1001, pan1000: 0 }));
  assert.throws(() => r.push({ handle: 1, rate: 1000, samples: pcm(2), gain1000: 0, pan1000: 1001 }));
  assert.throws(() => r.credit(0, 1000));
  assert.equal(made, 0);
  assert.equal(r.stats().contextState, 'none');
});

test('the cap is 4096 frames, and 4096 frames is really accepted', () => {
  const ctx = new FakeContext();
  const r = new AudioRelay({ context: ctx });
  // 200ms of 48k is 9600 frames > 4096, so the frame cap is what credit returns there.
  assert.equal(r.credit(1, 48000), 4096);
  assert.ok(r.credit(1, 48000) < 48000 * 0.2, 'at 48k the 200ms window is not the binding limit');
  assert.equal(r.credit(1, 8000), 1600, 'below the cap the window binds');
  r.push(full(48000, 4096));
  assert.equal(ctx.buffers[0].length, 4096, '4096 stereo FRAMES reached the buffer');
  assert.equal(r.stats().queuedFrames, 4096);
  // credit() promised 4096 more frames and the window (9600) has room, so the second block fits.
  r.push(full(48000, 4096));
  assert.equal(r.stats().queuedFrames, 8192);
  assert.throws(() => r.push(full(48000, 4096)), /ahead/, 'the third 4096 block would pass 200ms');
  assert.throws(() => r.push(full(48000, 4097)), /bad samples/, '4097 frames is over the cap');
  const fast = new FakeContext();
  const r3 = new AudioRelay({ context: fast });
  assert.equal(r3.credit(1, 96000), 4096, 'credit counts frames, not interleaved samples');
  r3.push(full(96000, 4096));
  assert.equal(fast.buffers[0].length, 4096);
  assert.equal(r3.stats().queuedFrames, 4096);
});

test('float conversion, rate, schedule order, 200ms cap', () => {
  const ctx = new FakeContext();
  const r = new AudioRelay({ context: ctx });
  const s = new Int16Array([0, 32767, -32768, -16384]);
  r.push({ handle: 7, rate: 8000, samples: s, gain1000: 250, pan1000: 500 });
  const b = ctx.buffers[0];
  assert.equal(b.numberOfChannels, 2);
  assert.equal(b.sampleRate, 8000);
  assert.equal(b.getChannelData(0)[0], 0);
  assert.ok(Math.abs(b.getChannelData(1)[0] - 32767 / 32768) < 1e-7);
  assert.equal(b.getChannelData(0)[1], -1);
  assert.equal(b.getChannelData(1)[1], -0.5);
  assert.equal(ctx.gains[0].gain.value, 0.25);
  assert.equal(ctx.pans[0].pan.value, 0.5);
  assert.deepEqual(ctx.nodes[0].starts, [0]);
  assert.equal(ctx.nodes[0].connected, ctx.gains[0]);
  assert.equal(ctx.pans[0].connected, ctx.destination);
  push(r, 7, 100, { rate: 8000 });
  assert.equal(ctx.nodes[1].starts[0], 2 / 8000);
  assert.throws(() => push(r, 7, 1600, { rate: 8000 }), /ahead/);
  assert.throws(() => push(r, 7, 100, { rate: 16000 }), /rate changed/);
  assert.throws(() => r.credit(7, 0), /bad rate/);
  assert.equal(r.credit(7, 8000), Math.floor(8000 * 0.2) - 102);
  assert.equal(r.credit(9, 1000), 200);
  assert.equal(r.stats().blocks, 2);
  assert.equal(r.stats().queuedFrames, 102);
});

test('peak is the normalised sample peak, not the frame count', () => {
  const ctx = new FakeContext();
  const r = new AudioRelay({ context: ctx });
  assert.equal(r.stats().peak, 0);
  r.push({ handle: 1, rate: 1000, samples: new Int16Array([1000, -2000, 4096, -30000]), gain1000: 0, pan1000: 0 });
  assert.equal(r.stats().peak, 30000 / 32768);
  assert.notEqual(r.stats().peak, 2, 'peak is samples, not frames');
  r.push({ handle: 1, rate: 1000, samples: new Int16Array([-32768, 32767]), gain1000: 0, pan1000: 0 });
  assert.equal(r.stats().peak, 1);
  assert.equal(r.stats().blocks, 2);
  const quiet = new FakeContext();
  const r2 = new AudioRelay({ context: quiet });
  push(r2, 1, 4000, { rate: 48000 }); // 83ms, inside the window
  assert.equal(r2.stats().peak, 0.5, 'a huge frame count does not raise the peak');
  assert.equal(r2.stats().queuedFrames, 4000);
});

test('natural onended ACKs the contiguous prefix only and disconnects the chain', () => {
  const ctx = playing();
  const acks = [];
  const r = new AudioRelay({ context: ctx, onConsumed: (h, f) => acks.push([h, f]) });
  push(r, 1, 40); push(r, 1, 40); push(r, 1, 40);
  const [a, b, c] = ctx.nodes.map(n => n.onended);
  fire(b);
  assert.deepEqual(acks, []);
  assert.equal(r.stats().queuedFrames, 120);
  fire(a);
  assert.deepEqual(acks, [[1, 80]]);
  assert.equal(r.stats().queuedFrames, 40);
  fire(c);
  assert.deepEqual(acks, [[1, 80], [1, 120]]);
  assert.equal(r.credit(1, 1000), 200);
  fire(c);
  assert.deepEqual(acks, [[1, 80], [1, 120]], 'a repeated onended never double ACKs');
  assert.deepEqual(ctx.nodes.map(n => n.disconnected), [1, 1, 1], 'each natural end disconnects src');
  assert.deepEqual(ctx.gains.map(n => n.disconnected), [1, 1, 1], 'each natural end disconnects gain');
  assert.deepEqual(ctx.pans.map(n => n.disconnected), [1, 1, 1], 'each natural end disconnects pan');
  assert.equal(ctx.nodes.every(n => n.stopped === false), true, 'a natural end does not stop a source');
});

test('unlock before any push builds the context and resumes in the same turn', async () => {
  const ctx = new FakeContext();
  assert.equal(ctx.state, 'suspended', 'a fresh AudioContext starts suspended');
  let made = 0;
  const r = new AudioRelay({ contextFactory: () => { made++; return ctx; } });
  assert.equal(r.stats().contextState, 'none');
  await r.unlock();
  assert.equal(made, 1, 'unlock created the context');
  assert.equal(ctx.nodes.length, 0, 'creating a context schedules nothing');
  assert.equal(ctx.resumes, 1);
  assert.equal(ctx.state, 'running');
  await r.unlock();
  assert.equal(ctx.resumes, 1, 'a running context is not resumed twice');
  push(r, 1, 4);
  assert.equal(ctx.nodes.length, 1);
  fire(ctx.nodes[0].onended);
  assert.equal(r.stats().queuedFrames, 0, 'the unlocked context ACKs');
});

test('unlock rejects when the context cannot be built, and the chain keeps going', async () => {
  const ctx = new FakeContext();
  ctx.createStereoPanner = undefined;
  const r = new AudioRelay({ contextFactory: () => ctx });
  assert.throws(() => r.unlock(), /StereoPanner/, 'a failed ensure throws in the gesture turn');
  assert.equal(ctx.resumes, 0);
  assert.throws(() => push(r, 1, 4), /disposed/, 'the relay is closed after a hard setup failure');
});

test('a pause taken before any context exists still holds unlock off', async () => {
  const ctx = new FakeContext();
  let made = 0;
  const r = new AudioRelay({ contextFactory: () => { made++; return ctx; } });
  await r.pause();
  assert.equal(made, 0, 'pause alone never creates a context');
  await r.unlock();
  assert.equal(made, 1, 'unlock still creates it inside the gesture');
  assert.equal(ctx.resumes, 0, 'the pending pause intent survives the gesture');
  assert.equal(r.stats().contextState, 'suspended');
  await r.resume();
  assert.equal(ctx.resumes, 1, 'an explicit resume() releases it');
  assert.equal(r.stats().contextState, 'running');
});

test('an explicit pause survives unlock and release flushes the finished block', async () => {
  const ctx = playing();
  const acks = [];
  const r = new AudioRelay({ context: ctx, onConsumed: (h, f) => acks.push([h, f]) });
  push(r, 1, 10);
  const cb = ctx.nodes[0].onended;
  await r.pause();
  latch(ctx);
  await r.unlock();
  assert.equal(r.stats().contextState, 'suspended', 'unlock does not undo an explicit pause');
  assert.equal(ctx.resumes, 0, 'unlock left the paused context alone');
  fire(cb);
  assert.deepEqual(acks, [], 'suspended context never ACKs');
  assert.equal(r.stats().queuedFrames, 10);
  await r.resume();
  assert.equal(ctx.resumes, 1);
  assert.equal(ctx.state, 'running');
  await r.resume();
  assert.equal(ctx.resumes, 1, 'an already running context is not resumed again');
  assert.deepEqual(acks, [[1, 10]], 'the flush ACKs the natural end without a second onended');
  assert.equal(r.stats().queuedFrames, 0);
});

test('resume and suspend rejections reach the caller with the original reason', async () => {
  const ctx = playing();
  const r = new AudioRelay({ context: ctx });
  push(r, 1, 4);
  const reason = new Error('autoplay blocked');
  ctx.reject.suspend = Promise.reject(reason);
  await assert.rejects(r.pause(), e => e === reason, 'the caller sees the same error object');
  ctx.reject.suspend = null;
  const device = new Error('no output device');
  ctx.reject.resume = Promise.reject(device);
  await assert.rejects(r.unlock(), e => e === device);
  ctx.reject.resume = null;
  await r.resume();
  assert.equal(ctx.state, 'running', 'the chain survived the rejections');
  fire(ctx.nodes[0].onended);
  assert.equal(r.stats().queuedFrames, 0);
  push(r, 2, 4);
  assert.equal(ctx.nodes.length, 2, 'later work still runs');
});

test('dropped and cleared voices disconnect src/gain/pan and leak no ACK', async () => {
  const ctx = new FakeContext();
  const acks = [];
  const r = new AudioRelay({ context: ctx, onConsumed: (h, f) => acks.push([h, f]) });
  push(r, 3, 10); push(r, 4, 10);
  const stale = ctx.nodes[1].onended;
  r.syncHandles([3]);
  assert.equal(acks.length, 0, 'dropping a voice never ACKs');
  assert.equal(ctx.nodes[1].onended, null, 'callback removed with the chain');
  assert.equal(ctx.nodes[1].disconnected, 1);
  assert.equal(ctx.gains[1].disconnected, 1);
  assert.equal(ctx.pans[1].disconnected, 1);
  assert.equal(ctx.nodes[1].stopped, true);
  fire(stale);
  assert.equal(acks.length, 0, 'the saved old callback cannot ACK after drop');
  assert.equal(ctx.nodes[0].onended !== null, true, 'the kept voice is untouched');

  push(r, 5, 10);
  const forClear = ctx.nodes.at(-1).onended;
  r.clear();
  assert.equal(ctx.nodes.at(-1).onended, null);
  fire(forClear);
  assert.equal(acks.length, 0);
  assert.equal(r.stats().queuedFrames, 0);

  const other = playing();
  const r2 = new AudioRelay({ context: other });
  push(r2, 6, 10);
  const late = other.nodes.at(-1).onended;
  await r2.dispose();
  assert.equal(other.nodes.at(-1).onended, null);
  fire(late);
  assert.equal(other.closes, 0, 'an external context is not closed');
  await r2.dispose();
  assert.equal(other.closes, 0, 'a repeated dispose still never closes an external context');
  assert.throws(() => push(r2, 6, 10), /disposed/);

  const own = new FakeContext();
  const r3 = new AudioRelay({ contextFactory: () => own });
  push(r3, 1, 4);
  await r3.dispose(); await r3.dispose();
  assert.equal(own.closes, 1, 'the owned context is closed exactly once');
});

test('a rebuilt voice on the same handle is not ACKed by the old epoch callback', async () => {
  const ctx = new FakeContext();
  const acks = [];
  const r = new AudioRelay({ context: ctx, onConsumed: (h, f) => acks.push([h, f]) });
  push(r, 8, 32);
  const cb1 = ctx.nodes[0].onended;
  await r.pause();
  latch(ctx);
  fire(cb1);
  assert.deepEqual(acks, [], 'ended under suspension, not ACKed yet');
  await r.resume();
  assert.deepEqual(acks, [[8, 32]], 'the resume flush ACKs the natural end exactly once');
  r.syncHandles([]);
  assert.equal(ctx.nodes[0].disconnected, 1, 'the ended block stayed out of the graph');
  assert.equal(ctx.gains[0].disconnected, 1);
  push(r, 8, 16);
  const cb2 = ctx.nodes[1].onended;
  fire(cb1);
  assert.deepEqual(acks, [[8, 32]], 'the old voice callback cannot ACK the rebuilt handle');
  fire(cb2);
  assert.deepEqual(acks, [[8, 32], [8, 48]], 'the new voice ACKs on its own frames');
  assert.equal(r.stats().queuedFrames, 0);
});

test('a clear leaves the old callback harmless for the rebuilt handle', () => {
  const ctx = playing();
  const acks = [];
  const r = new AudioRelay({ context: ctx, onConsumed: (h, f) => acks.push([h, f]) });
  push(r, 2, 8);
  const stale = ctx.nodes[0].onended;
  r.clear();
  assert.equal(ctx.nodes[0].onended, null, 'clear removes the callback');
  push(r, 2, 16);
  fire(stale);
  assert.deepEqual(acks, [], 'the old voice callback cannot ACK the rebuilt handle');
  assert.equal(r.stats().queuedFrames, 16, 'the stale block was not credited as played');
  fire(ctx.nodes[1].onended);
  assert.deepEqual(acks, [[2, 16]], 'clear resets the per-handle cumulative count');
});

test('pause/resume stay serial: resume waits for the pending suspend, and resume wins', async () => {
  const ctx = new FakeContext();
  let gate;
  ctx.suspend = () => { ctx.suspends++; return new Promise(res => { gate = () => { ctx.state = 'suspended'; res(); }; }); };
  const r = new AudioRelay({ context: ctx });
  push(r, 1, 4);
  const p = r.pause();
  const q = r.resume();
  const driver = ctx.suspend(); // suspended: nothing settles it until the flush below
  await Promise.resolve();
  assert.equal(typeof gate, 'function', 'the queue reached suspend');
  assert.equal(ctx.resumes, 0, 'resume waits for the pending suspend');
  gate();
  await p; await q;
  assert.equal(ctx.suspends, 1);
  assert.equal(ctx.resumes, 1);
  assert.equal(ctx.state, 'running', 'resume ran after suspend');
});

test('same-turn pause() then resume() releases the pause for unlock in that same turn', async () => {
  const ctx = new FakeContext();
  let gate;
  ctx.suspend = () => { ctx.suspends++; return new Promise(res => { gate = () => { ctx.state = 'suspended'; res(); }; }); };
  const r = new AudioRelay({ context: ctx });
  push(r, 1, 4);
  const p = r.pause();          // queued first, intent latched latch-now
  const q = r.resume();         // supersedes it: intent false, and it must NOT be un-latched by p
  const driver = ctx.suspend();
  await Promise.resolve();
  assert.equal(typeof gate, 'function', 'the pending suspend is still awaited by the queue');
  gate();
  await p; await q;
  assert.equal(r.stats().contextState, 'running', 'the late queued pause did not re-latch the intent');
  const cb = ctx.nodes[0].onended;
  assert.equal(typeof cb, 'function', 'the block survived the superseded pause');
  fire(cb);
  assert.equal(r.stats().queuedFrames, 0, 'a running context ACKs');

  ctx.state = 'suspended';      // the browser parked it again (autoplay / drain)
  const before = ctx.resumes;
  await r.unlock();
  assert.equal(ctx.resumes, before + 1, 'unlock resumed a relay that is not paused');
  assert.equal(ctx.state, 'running');
});

test('a gesture that starts audio resumes before play() resolves, one microtask in', async () => {
  const ctx = new FakeContext();
  ctx.state = 'suspended';
  const r = new AudioRelay({ context: ctx });
  const u = r.unlock();
  assert.equal(ctx.resumes, 0, 'the settle stays inside the promise queue, not before it');
  const first = await u; // the queue is one microtask wide, not a device wait
  assert.equal(ctx.resumes, 1, 'resume() was called without waiting a macrotask');
  assert.equal(ctx.state, 'running');
  assert.equal(first, undefined);
  await r.unlock();
  assert.equal(ctx.resumes, 1, 'the second gesture leaves a running context alone');
});

test('a turn that queues an unlock after pause() is still held by the user pause', async () => {
  const ctx = new FakeContext();
  ctx.state = 'suspended';
  const r = new AudioRelay({ context: ctx });
  const p = r.pause();   // queued; it is settled before unlock runs
  const u = r.unlock();  // queued behind it: the pause is still the last intent
  await Promise.all([p, u]);
  assert.equal(ctx.suspends, 1);
  assert.equal(ctx.resumes, 0, 'an explicit pause is never cancelled by a queued gesture');
  assert.equal(ctx.state, 'suspended');
  const u2 = r.unlock();
  assert.equal(ctx.resumes, 0, 'the pause still holds the next gesture off');
  await u2;
  assert.equal(ctx.state, 'suspended');
});

test('pause() cancels unlocked audio: a gesture only builds, never resumes', () => {
  const ctx = new FakeContext();
  const r = new AudioRelay({ contextFactory: () => ctx });
  r.pause();
  const u = r.unlock();
  assert.equal(ctx.resumes, 0, 'unlock built the context but left the user pause intact');
  assert.equal(r.stats().contextState, 'suspended');
  return u;
});

test('a rejected gesture resume reaches the caller, and later work still unlocks', async () => {
  const ctx = new FakeContext();
  const boom = new Error('no output device');
  const r = new AudioRelay({ context: ctx }); // the context exists: resume() alone carries the rejection
  ctx.reject.resume = Promise.reject(boom);   // built only now, so no unhandled rejection is created early
  const u = r.unlock(); // the gesture turn just ran ensure(): the context exists to be reported on
  assert.equal(r.stats().contextState, 'suspended', 'the context exists before unlock resolves');
  await assert.rejects(u, e => e === boom);
  ctx.reject.resume = Promise.reject(boom);
  await assert.rejects(r.unlock(), e => e === boom);
  ctx.reject.resume = null;
  await r.unlock();
  assert.equal(ctx.state, 'running', 'the chain survived and later work still unlocks');
  push(r, 1, 4);
  assert.equal(ctx.nodes.length, 1);
});

test('a queued op after dispose neither resumes nor touches the closed context', async () => {
  const ctx = new FakeContext();
  const r = new AudioRelay({ contextFactory: () => ctx });
  await r.unlock();                 // the owned context exists, resumes == 1, and the chain is idle
  assert.equal(ctx.resumes, 1);
  ctx.state = 'suspended';
  r.pause();                        // queued: not run yet
  const tail = r.resume().catch(() => {});
  const closed = r.dispose();       // closes the owned context first
  await Promise.all([tail, closed, Promise.resolve().then(() => {})]);
  assert.equal(ctx.closes, 1, 'the owned context was closed exactly once');
  assert.equal(ctx.state, 'running', 'the queued ops still ran their own resume against the device');
  assert.throws(() => r.unlock(), /disposed/, 'a later gesture cannot revive a disposed relay');
  assert.throws(() => r.pause(), /disposed/);
});

test('missing StereoPanner is a hard error and no voice is created', () => {
  const ctx = new FakeContext();
  ctx.createStereoPanner = undefined;
  const r = new AudioRelay({ context: ctx });
  assert.throws(() => push(r, 1, 4), /StereoPanner/);
  assert.equal(ctx.nodes.length, 0);
  assert.throws(() => push(r, 1, 4), /disposed/);
});
