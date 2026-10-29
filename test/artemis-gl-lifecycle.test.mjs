import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const temp = mkdtempSync(join(tmpdir(), 'artemis-gl-lifecycle-'));
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-gl-lifecycle-')) rmSync(temp, { recursive: true, force: true });
});
const out = join(temp, 'out');
execFileSync(process.execPath, [join(root, 'node_modules/typescript/bin/tsc'), 'packages/engine/core/artemis-compat/src/host/gl.ts', '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler', '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck', '--rootDir', 'packages/engine/core/artemis-compat/src', '--outDir', out], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
const { createArtemisGlImports } = await import(pathToFileURL(join(out, 'host/gl.js')));
// Keep the production-private constant private; this independent expected ABI is the nineteen SDK rows.
const ARTEMIS_GL_AUDIO_ROWS = ['F', 'G', 'Ga', 'Ha', 'Ia', 'Ja', 'Ka', 'La', 'Ma', 'Na', 'Oa', 'Pa', 'R', 'ga', 'oa', 'vc', 'wc', 'xc', 'yc'];
const { ArtemisMemory } = await import(pathToFileURL(join(out, 'host/memory.js')));
const flush = async () => { for (let i = 0; i < 20; i++) await Promise.resolve(); };
const gate = () => { let resolve, reject; const promise = new Promise((a, b) => { resolve = a; reject = b; }); return { promise, resolve, reject }; };
function rig(extra = {}) {
  const frames = [], freed = [], reports = [], listeners = new Map(); let controller;
  const document = { hidden: false, visibilityState: 'visible', querySelector: () => null, addEventListener: (n, f) => listeners.set(n, [...(listeners.get(n) || []), f]), removeEventListener: (n, f) => listeners.set(n, (listeners.get(n) || []).filter(v => v !== f)) };
  const imports = createArtemisGlImports({ memory: new ArtemisMemory({ memory: new WebAssembly.Memory({ initial: 1 }) }), document, malloc: () => 8192, free: p => freed.push(p), requestFrame: f => frames.push(f), dynCallV: () => {}, dynCallIiii: () => 0, setLifecycle: c => controller = c, onBlocked: (a, b) => reports.push([a, b]), ...extra });
  return { imports, frames, freed, reports, listeners, get lifecycle() { return controller; } };
}

test('an incomplete audio table is refused before any lifecycle is published', () => {
  let installed = 0;
  assert.throws(() => rig({ audioImports: { Pa: () => 17 }, setLifecycle: () => installed++ }), /audioImports table without/);
  assert.equal(installed, 0);
});
test('all nineteen audio rows capture their validated bodies and forward each actual argument', async () => {
  const calls = [], audioImports = Object.fromEntries(ARTEMIS_GL_AUDIO_ROWS.map((name, n) => [name, (...args) => { calls.push([name, args]); return n + 100; }]));
  const r = rig({ audioImports });
  for (const [n, name] of ARTEMIS_GL_AUDIO_ROWS.entries()) { audioImports[name] = undefined; assert.equal(r.imports[name](7, 11, 13), n + 100); }
  assert.deepEqual(calls, ARTEMIS_GL_AUDIO_ROWS.map(name => [name, [7, 11, 13]])); assert.deepEqual(r.reports, []); await r.lifecycle.dispose();
});
test('pause, settle while paused, and repeated resume repay exactly one frame', async () => {
  const g = gate(), r = rig({ dynCallV: () => g.promise });
  r.imports._b(1, 0, 0); r.lifecycle.pause(); g.resolve(); await flush(); assert.equal(r.frames.length, 0);
  r.lifecycle.resume(); r.lifecycle.resume(); assert.equal(r.frames.length, 1); await r.lifecycle.dispose();
});
test('resume before settlement clears pause without overlapping the guest', async () => {
  const g = gate(), r = rig({ dynCallV: () => g.promise });
  r.imports._b(1, 0, 0); r.lifecycle.pause(); r.lifecycle.resume(); assert.equal(r.frames.length, 0);
  g.resolve(); await flush(); assert.equal(r.frames.length, 1); await r.lifecycle.dispose();
});
test('the serializer completion, not its task return, is the scheduling barrier', async () => {
  const g = gate(); let task; const r = rig({ enqueue: f => { task = f; return g.promise; } });
  r.imports._b(1, 0, 0); r.lifecycle.pause(); r.lifecycle.resume(); await task(); assert.equal(r.frames.length, 0);
  g.resolve(); await flush(); assert.equal(r.frames.length, 1); await r.lifecycle.dispose();
});
test('dispose with synchronous idle failure frees the owned event block once', async () => {
  const r = rig({ waitUntilIdle: () => { throw Error('idle sync'); } });
  r.imports.cc(1, 0, 0, 4); assert.doesNotThrow(() => r.lifecycle.dispose()); r.lifecycle.dispose();
  assert.deepEqual(r.freed, [8192]); assert.equal(r.reports.filter(a => a[1].includes('idle sync')).length, 1);
});
test('dispose retains an event block until the pending idle wait settles', async () => {
  const g = gate(), r = rig({ waitUntilIdle: () => g.promise });
  r.imports.cc(1, 0, 0, 4); const done = r.lifecycle.dispose(); assert.deepEqual(r.freed, []);
  g.resolve(); await done; r.lifecycle.dispose(); assert.deepEqual(r.freed, [8192]);
});
test('a same-target same-event registration replaces its predecessor', async () => {
  const r = rig(); r.imports.cc(1, 0, 0, 4); r.imports.cc(1, 0, 0, 7);
  assert.equal(r.listeners.get('keydown').length, 1); await r.lifecycle.dispose(); assert.equal(r.listeners.get('keydown').length, 0);
});
test('undefined rejection is terminal and cannot be revived by resume', async () => {
  const g = gate(), r = rig({ dynCallV: () => g.promise }); r.imports._b(1, 0, 0); g.reject(undefined); await flush();
  r.lifecycle.pause(); r.lifecycle.resume(); assert.equal(r.frames.length, 0); await r.lifecycle.dispose();
});
test('a synchronous host failure is terminal and retains its original error', async () => {
  const boom = Error('host failure'), r = rig({ dynCallV: () => { throw boom; } });
  assert.throws(() => r.imports._b(1, 0, 0), e => e === boom); r.lifecycle.pause(); r.lifecycle.resume(); assert.equal(r.frames.length, 0); await r.lifecycle.dispose();
});
for (const asynchronous of [false, true]) test(`${asynchronous ? 'rejected' : 'thrown'} literal unwind is healthy but waits for idle before rearming`, async () => {
  const g = gate(), r = rig({ dynCallV: () => { if (asynchronous) return Promise.reject('unwind'); throw 'unwind'; }, waitUntilIdle: () => g.promise });
  try { r.imports._b(1, 0, 0); } catch (e) { assert.equal(e, 'unwind'); }
  await flush(); assert.equal(r.frames.length, 0, 'the idle barrier is still pending'); g.resolve(); await flush();
  assert.equal(r.frames.length, 1, 'healthy unwind must continue the loop'); assert.equal(r.reports.length, 0); await r.lifecycle.dispose();
});
for (const boom of [new WebAssembly.RuntimeError('unreachable'), 'not-unwind']) test(`only literal unwind is nonterminal: ${String(boom)}`, async () => {
  const r = rig({ dynCallV: () => { throw boom; } }); try { r.imports._b(1, 0, 0); } catch (e) { assert.equal(e, boom); }
  await flush(); r.lifecycle.pause(); r.lifecycle.resume(); assert.equal(r.frames.length, 0); await r.lifecycle.dispose();
});
for (const asynchronous of [false, true]) test(`${asynchronous ? 'rejected' : 'thrown'} serializer unwind also waits for idle and rearms once`, async () => {
  const g = gate(); let idleCalls = 0;
  const r = rig({ enqueue: () => { if (asynchronous) return Promise.reject('unwind'); throw 'unwind'; }, waitUntilIdle: () => { idleCalls++; return g.promise; } });
  try { r.imports._b(1, 0, 0); } catch (e) { assert.equal(e, 'unwind'); }
  await flush(); assert.equal(r.frames.length, 0, 'serializer unwind must not bypass the idle barrier');
  assert.equal(idleCalls, 1, 'the healthy control signal is adopted into exactly one idle wait');
  r.lifecycle.pause(); r.lifecycle.resume(); assert.equal(r.frames.length, 0, 'resume cannot overlap the pending idle wait');
  g.resolve(); await flush(); assert.equal(r.frames.length, 1); assert.deepEqual(r.reports, []); await r.lifecycle.dispose();
});
for (const asynchronous of [false, true]) test(`serializer unwind followed by ${asynchronous ? 'rejected' : 'thrown'} idle failure is terminal`, async () => {
  const g = gate(), boom = Error('idle barrier failed');
  // A broken host may never subscribe to this barrier; that must fail an assertion, not leak a rejection.
  void g.promise.catch(() => {});
  const r = rig({ enqueue: () => { throw 'unwind'; }, waitUntilIdle: () => { if (asynchronous) return g.promise; throw boom; } });
  try { r.imports._b(1, 0, 0); } catch (e) { assert.ok(e === 'unwind' || e === boom); }
  if (asynchronous) g.reject(boom); await flush(); r.lifecycle.pause(); r.lifecycle.resume(); assert.equal(r.frames.length, 0);
  assert.ok(r.reports.some(a => a[1].includes('idle barrier failed')), 'the actual idle failure is diagnosed'); await r.lifecycle.dispose();
});
