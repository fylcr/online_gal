/**
 * Real Node harness for the classic compat worker.
 * The TS sources are compiled by tsc into a temp ES-module tree first; a worker_threads
 * bootstrap supplies `self`/`importScripts` so the same classic worker file runs unmodified.
 * Run: node --test test/artemis-compat-worker.test.mjs
 *
 * Everything this test needs at runtime is written into one mkdtemp subtree (bootstrap,
 * fake glue), so a clean clone with no `.test/` workbench still runs it. This file owns the
 * tree and removes it on exit.
 */
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, readFileSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { Worker } from 'node:worker_threads';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const WORKER_SCRIPT = join(root, 'packages', 'engine', 'core', 'artemis-compat', 'src', 'compat', 'compat-worker.js');
const PAST = 0x80000000;
const SIZE = 0x80000000 + 4096;
const DESCRIPTORS = [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: SIZE }];

/**
 * Test-only classic-worker bootstrap. Node worker_threads has no `self` and no
 * `importScripts`; production glue runs inside a real classic worker and needs neither
 * shim from here. Held as a string so this test does not depend on the `.test/` workbench.
 */
const BOOTSTRAP_SOURCE = `
const fs = require('node:fs');
const vm = require('node:vm');
const { fileURLToPath } = require('node:url');
const { parentPort, workerData } = require('node:worker_threads');
const self = globalThis;
self.self = self;
self.postMessage = (message) => parentPort.postMessage(message);
self.close = () => parentPort.close();
// TEST ONLY: production importScripts is the browser's. Here node:vm stands in, so no eval.
self.importScripts = (...urls) => {
  for (const url of urls) {
    const file = url.startsWith('file:') ? fileURLToPath(url) : url;
    vm.runInThisContext(fs.readFileSync(file, 'utf8'), { filename: file });
  }
};
const queued = [];
parentPort.on('message', (data) => {
  const canvas = data && data.canvas;
  if (canvas && canvas.buffer instanceof ArrayBuffer && !canvas.getContext) {
    const held = canvas.buffer;
    Object.defineProperty(canvas, 'width', { get() { return new Int32Array(held)[0]; }, set(v) { new Int32Array(held)[0] = v; } });
    Object.defineProperty(canvas, 'height', { get() { return new Int32Array(held)[1]; }, set(v) { new Int32Array(held)[1] = v; } });
    canvas.ctx = { id: 1 };
    canvas.getContext = function () { return this.ctx; };
  }
  if (typeof self.onmessage === 'function') self.onmessage({ data });
  else queued.push(data);
});
require(workerData.script);
for (const data of queued.splice(0)) self.onmessage({ data });
`;

const temp = mkdtempSync(join(tmpdir(), 'artemis-compat-'));
const out = join(temp, 'out');
/* The guard removes only a tree this test created: same tmpdir, right name prefix, still a directory. */
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-compat-')) {
    rmSync(temp, { recursive: true, force: true });
  }
});
execFileSync(process.execPath, [
  join(root, 'node_modules/typescript/bin/tsc'),
  'packages/engine/core/artemis-compat/src/compat/index.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--noUncheckedIndexedAccess', '--skipLibCheck',
  '--rootDir', 'packages/engine/core/artemis-compat/src', '--outDir', out,
], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/engine/core/artemis-compat/node_modules'), join(out, 'node_modules'), 'junction');
const BOOTSTRAP = join(temp, 'bootstrap.cjs');
writeFileSync(BOOTSTRAP, BOOTSTRAP_SOURCE);
const api = await import(pathToFileURL(join(out, 'compat', 'index.js')).href);
const { CompatReadHost, CompatWorkerHost, allocateCompatBlock, CONTROL, READ_FATAL, READ_MISSING } = api;

/**
 * Minimal generated-emscripten stand-in: a real cwrap, so `const char *` marshalling is
 * exercised and so a wrong ident fails here the same way it fails against real glue.
 *
 * `getCFunc` is copied from emsdk `src/lib/libccall.js:13-18` — `Module['_' + ident]`.
 * The fake and the real thing must agree on that prefix, or the fake passes what the real
 * glue would reject (that is exactly how the double-underscore bug stayed green).
 */
const FAKE = `
var __heap = new Uint8Array(1 << 20);
var __top = 4096;
var __enc = new TextEncoder();
var __dec = new TextDecoder();
self.Module.HEAPU8 = __heap;
self.Module.HEAP32 = new Int32Array(__heap.buffer);
self.Module._malloc = function (n) { var p = __top; __top += (n + 7) & ~7; if (__top > __heap.length) throw new Error('fake heap full'); return p; };
self.Module._free = function () {};
self.Module.lengthBytesUTF8 = function (s) { return __enc.encode(s).length + 1; };
self.Module.UTF8ToString = function (p) { var e = p; while (__heap[e]) e++; return __dec.decode(__heap.subarray(p, e)); };
self.Module.stringToUTF8 = function (s, p) { var b = __enc.encode(s); __heap.set(b, p); __heap[p + b.length] = 0; return b.length + 1; };
self.Module.ccall = function (name, ret, types, args) {
  var fn = self.Module['_' + name];
  if (typeof fn !== 'function') throw new Error('missing _' + name);
  var owned = [];
  var argv = args.map(function (value, i) {
    if (types[i] !== 'string') return value;
    var p = self.Module._malloc(self.Module.lengthBytesUTF8(value));
    self.Module.stringToUTF8(value, p);
    owned.push(p);
    return p;
  });
  var result = fn.apply(null, argv);
  owned.forEach(function (p) { self.Module._free(p); });
  return result;
};
self.Module.cwrap = function (name, ret, types) {
  return function () { return self.Module.ccall(name, ret, types, Array.prototype.slice.call(arguments)); };
};
/* Probes travel on the ordinary worker message channel; the worker itself has no test hook. */
self.probe = function (line) { self.postMessage({ type: 'probe', line: line }); };
`;

let glueSeq = 0;
function glue(body) {
  const file = join(temp, `glue${glueSeq++}.js`);
  writeFileSync(file, `${FAKE}\n${body}\nif (self.Module && self.Module.onRuntimeInitialized) self.Module.onRuntimeInitialized();\n`);
  return pathToFileURL(file).href;
}

const byteAt = (offset, index) => (offset + index) % 251;

function makeSource(overrides = {}) {
  const reads = [];
  return {
    reads,
    source: {
      name: 'fixture',
      capabilities: { randomAccess: true, writable: false },
      async readRange(path, offset, length) {
        reads.push({ path, offset, length });
        if (overrides.readRange) return overrides.readRange(path, offset, length);
        return Uint8Array.from({ length }, (_, i) => byteAt(offset, i));
      },
    },
  };
}

/** A raw classic worker plus the page-side read host that answers it. */
class Session {
  constructor({ source, windowBytes }) {
    this.messages = [];
    this.probes = [];
    this.reads = [];
    this.answers = [];
    this.block = allocateCompatBlock(windowBytes);
    this.host = new CompatReadHost(source, this.block, DESCRIPTORS);
    this.worker = new Worker(BOOTSTRAP, { workerData: { script: WORKER_SCRIPT } });
    this.worker.on('error', (error) => this.messages.push({ type: 'worker-error', reason: String(error) }));
    this.worker.on('message', (message) => {
      this.messages.push(message);
      if (message.type === 'probe') this.probes.push(message.line);
      if (message.type !== 'read') return;
      this.reads.push(message);
      // Called synchronously on arrival, so the epoch is captured before any timeout can bump it.
      this.answers.push(this.host.answer(message));
    });
  }
  init(glueUrl, { readTimeoutMs } = {}) {
    this.worker.postMessage({
      type: 'init', id: 1, canvas: null, engineJsUrl: glueUrl,
      wasmUrl: 'file:///artemis-compat.wasm', descriptors: DESCRIPTORS, block: this.block.buffer,
      ...(readTimeoutMs === undefined ? {} : { readTimeoutMs }),
    });
    return this;
  }
  take(type) {
    const index = this.messages.findIndex((message) => message.type === type);
    if (index < 0) return undefined;
    return this.messages.splice(index, 1)[0];
  }
  async wait(type, ms = 5000) {
    const deadline = Date.now() + ms;
    while (Date.now() < deadline) {
      const found = this.take(type);
      if (found) return found;
      await new Promise((resolve) => setTimeout(resolve, 5));
    }
    throw new Error(`no ${type}; saw ${JSON.stringify(this.messages)}`);
  }
  terminate() {
    return this.worker.terminate();
  }
}

/** Node's Worker is an EventEmitter; the facade uses the browser Worker interface. */
class WebWorkerShim {
  constructor(url) {
    this.node = new Worker(BOOTSTRAP, { workerData: { script: WORKER_SCRIPT } });
    this.listeners = new Map();
    this.node.on('message', (data) => this.fire('message', { data }));
    this.node.on('error', (error) => this.fire('error', error));
    this.node.on('exit', () => this.fire('exit', undefined));
    void url;
  }
  fire(type, event) {
    for (const fn of this.listeners.get(type) ?? []) fn(event);
  }
  addEventListener(type, fn) {
    this.listeners.set(type, [...(this.listeners.get(type) ?? []), fn]);
  }
  postMessage(message, transfer = []) {
    this.node.postMessage(message, transfer);
  }
  terminate() {
    return this.node.terminate();
  }
}

const live = new Set();
test.after(async () => {
  for (const worker of live) await worker.terminate().catch(() => {});
  live.clear();
});
function spawn() {
  const worker = new WebWorkerShim();
  live.add(worker);
  return worker;
}

test('CMake adds ENV without dropping the existing runtime exports or native clock ABI', () => {
  const cmake = readFileSync(join(root, 'ports/artemis/compat-web/overlay/cmake/web.cmake'), 'utf8');
  assert.deepEqual(cmake.match(/EXPORTED_RUNTIME_METHODS=([^"\r\n]+)/)[1].split(','),
    ['ccall', 'cwrap', 'HEAPU8', 'FS', 'UTF8ToString', 'ENV']);
  assert.ok(cmake.includes('_artc_web_now_ms,'), 'the existing now_ms ABI remains');
});

test('typed animationTrace reaches preRun Module ENV, off never writes and old glue refuses on', async () => {
  for (const animationTrace of [undefined, false, true]) {
    const { source } = makeSource();
    const worker = spawn();
    const probes = [];
    worker.addEventListener('message', event => event.data.type === 'probe' && probes.push(event.data.line));
    const page = new CompatWorkerHost({ source, createWorker: () => worker });
    try {
      await page.load({
        engineJsUrl: glue(`
var writes = 0;
// Deliberately NOT self.Module. A callback closing over the factory config cannot pass.
var actual = { ENV: new Proxy({ KEEP: 'unchanged' }, { set: function (obj,key,value) { writes++; obj[key]=value; return true; } }) };
var hooks = self.Module.preRun || [];
for (var hook of hooks) hook(actual);
self.Module._artc_web_open = function () { self.probe('hooks=' + hooks.length + ',writes=' + writes + ',env=' + actual.ENV.ARTC_ANIMATION_TRACE + ',keep=' + actual.ENV.KEEP); return 1; };
self.Module._artc_web_dispose = function () {};
`),
        wasmUrl: 'file:///x.wasm', canvas: null, descriptors: DESCRIPTORS, animationTrace,
      });
      assert.deepEqual(probes, [animationTrace === true ? 'hooks=1,writes=1,env=1,keep=unchanged' : 'hooks=0,writes=0,env=undefined,keep=unchanged']);
    } finally {
      page.dispose(); await page.whenDisposed(); await worker.terminate();
    }
  }
  // Both sync and real-style asynchronous preRun must reject explicitly before native open.
  for (const asynchronous of [false, true]) {
    const { source } = makeSource();
    const worker = spawn();
    const probes = [];
    worker.addEventListener('message', event => event.data.type === 'probe' && probes.push(event.data.line));
    const page = new CompatWorkerHost({ source, createWorker: () => worker });
    const file = join(temp, `old-env-${asynchronous}.js`);
    writeFileSync(file, `${FAKE}\n
self.Module._artc_web_open = function () { self.probe('SHOULD_NOT_OPEN'); return 1; };
var init = self.Module.onRuntimeInitialized;
function start() { for (var hook of self.Module.preRun || []) hook({}); init(); }
${asynchronous ? 'setTimeout(start, 1);' : 'start();'}\n`);
    try {
      await assert.rejects(page.load({
        engineJsUrl: pathToFileURL(file).href, wasmUrl: 'file:///x.wasm',
        canvas: null, descriptors: DESCRIPTORS, animationTrace: true,
      }), /animationTrace requires a compat engine exporting ENV; trace was not enabled/);
      assert.deepEqual(probes, []);
    } finally {
      page.dispose(); await page.whenDisposed(); await worker.terminate();
    }
  }
});

/* The native input CABI, spelled for real: key takes two ints, pointer takes two floats. */
const INPUT_CABI = `
self.Module._artc_web_pointer = function (x, y, down) { self.probe('pointer=' + x + ',' + y + ',' + down); };
self.Module._artc_web_key = function (key, down) {
  self.probe('key=' + key + ',' + down + ',args=' + arguments.length + ',keyType=' + typeof key);
};
self.Module._artc_web_resize = function (w, h) { self.probe('resize=' + w + ',' + h); };
self.Module._artc_web_pointer_move = function (x, y) { self.probe('move=' + x + ',' + y); };
`;

test('the compiled transport pins the wire numbers, the open arguments and the CABI names', () => {
  const workerText = readFileSync(WORKER_SCRIPT, 'utf8');
  assert.equal(api.COMPAT_WINDOW_BYTES, 4 * 1024 * 1024);
  assert.equal(api.COMPAT_CONTROL_BYTES, 24);
  assert.equal(api.READ_TIMEOUT_MS, 30_000);
  assert.equal(api.COMPAT_OPEN_PACK, '/game/root.pfs');
  assert.equal(api.COMPAT_OPEN_MODE, 'windows');
  assert.match(workerText, /COMPAT_WINDOW_BYTES = 4 \* 1024 \* 1024/);
  assert.match(workerText, /COMPAT_CONTROL_BYTES = 24/);
  assert.match(workerText, /var READ_TIMEOUT_MS = 30000/);
  assert.match(workerText, /OPEN_PACK = '\/game\/root\.pfs'/);
  assert.match(workerText, /OPEN_MODE = 'windows'/);
  assert.match(workerText, /moduleRef\.cwrap\(/);
  assert.deepEqual(api.OPTIONAL_CABI, [
    '_artc_web_pause', '_artc_web_dispose', '_artc_web_save', '_artc_web_load', '_artc_web_now_ms',
  ]);
  for (const name of api.OPTIONAL_CABI) assert.ok(workerText.includes(name), `${name} is bound`);
});

test('the engine directory is created before open, because nothing in the engine makes one', () => {
  const workerText = readFileSync(WORKER_SCRIPT, 'utf8');
  // The save slot is written beside the pack (`save_dir_ = fs::path(pack).parent_path()`), i.e.
  // under `/game/`. No `create_directories` exists anywhere in the engine, and pack bytes arrive
  // through `artcReadRange`, so no filesystem open ever made that directory. Without this the first
  // save fails with ENOENT and reads as "cannot save" rather than "no directory".
  assert.match(workerText, /var GAME_DIR = '\/game'/, 'the pack parent is named once');
  assert.match(workerText, /FS\.mkdirTree\(GAME_DIR\)/, 'MEMFS gets the directory');
  // The **call**, not the definition: a declared-but-never-invoked helper would satisfy a text
  // match on the body while creating nothing.
  const settle = workerText.slice(workerText.indexOf('function settle()'));
  const call = settle.indexOf('ensureGameDirectory();');
  assert.ok(call >= 0, 'settle() invokes the directory creation');
  assert.ok(
    call < settle.indexOf('_artc_web_open('),
    'the directory exists before the engine opens the pack',
  );
  // The pack bytes are never preloaded into MEMFS: that is the whole point of the range transport. The
  // assertion is about the **pack**, not about writing files at all — a restore legitimately writes the
  // saved files, so banning `FS.writeFile` outright would ban the feature this transport now has. What
  // must never appear is a pack being materialised.
  assert.doesNotMatch(workerText, /preloadPFS|FS\.createDataFile/, 'no pack preloading');
  const packWrites = [...workerText.matchAll(/FS\.writeFile\(([^,]+),/g)].map((match) => match[1]);
  for (const target of packWrites) {
    assert.ok(
      /GAME_DIR|target/.test(target),
      `FS.writeFile target ${target} must be a save path under /game, never the pack`,
    );
  }
  assert.doesNotMatch(workerText, /FS\.writeFile\(OPEN_PACK/, 'the pack is never written to the filesystem');
});


test('cwrap is called with the C ident, never the Module property name', () => {
  const workerText = readFileSync(WORKER_SCRIPT, 'utf8');
  assert.match(workerText, /cwrap\(name\.slice\(1\)/, 'the ident handed to cwrap drops the Module underscore');
  assert.doesNotMatch(workerText, /cwrap\(name,/, 'cwrap must not receive the underscore-prefixed property name');
  const cwrapIdents = [...workerText.matchAll(/moduleRef\.cwrap\(([^,]+),/g)].map((m) => m[1]);
  assert.ok(cwrapIdents.length > 0, 'the worker binds at least one native call');
  for (const ident of cwrapIdents) {
    assert.ok(!ident.includes("'_"), `cwrap ident ${ident} must not carry the Module underscore`);
  }
  // The fake glue resolves `Module['_' + name]`, exactly as emsdk getCFunc does. A worker that
  // passed the property name would ask for `Module['__artc_web_*']`, which holds nothing — so the
  // fake must be able to reject a leading underscore, or it would pass the very bug under test.
  const fake = readFileSync(new URL(import.meta.url), 'utf8');
  const fakeGetCFunc = fake.match(/var fn = self\.Module\[([^\]]+)\];/);
  assert.ok(fakeGetCFunc, 'the fake glue resolves through getCFunc');
  assert.match(fakeGetCFunc[1], /'_' \+ name/, 'the fake prefixes one underscore, like the real glue');
});

test('a leading-underscore ident is what the real glue cannot resolve', () => {
  // The other half of the rule: with `getCFunc` prefixing '_', an ident that already
  // starts with '_' produces a double underscore and throws. This is the old-version failure.
  const mod = { _artc_web_open() {} };
  const getCFunc = (ident) => mod['_' + ident];
  assert.equal(typeof getCFunc('artc_web_open'), 'function');
  assert.throws(() => {
    const fn = getCFunc('_artc_web_open');
    if (typeof fn !== 'function') throw new Error('missing');
  }, /missing/);
});

test('the key CABI is the native two-integer signature, not a C string', () => {
  const workerText = readFileSync(WORKER_SCRIPT, 'utf8');
  // web_main.cpp:500 — void artc_web_key(int key, int down)
  assert.match(workerText, /_artc_web_key: \{ ret: null, args: \['number', 'number'\] \}/);
  assert.doesNotMatch(workerText, /_artc_web_key: \{[^}]*'string'/, 'a key is never passed as a C string');
  assert.doesNotMatch(workerText, /event\.key/, 'no DOM key string reaches the native call');
  // web_main.cpp:492 — void artc_web_pointer(float x, float y, int down)
  assert.match(workerText, /_artc_web_pointer: \{ ret: null, args: \['number', 'number', 'number'\] \}/);
  assert.doesNotMatch(workerText, /event\.x \| 0/, 'pointer coordinates are float, never bitwise-truncated');
  assert.doesNotMatch(workerText, /event\.y \| 0/, 'pointer coordinates are float, never bitwise-truncated');
  // web_main.cpp:496 / :504
  assert.match(workerText, /_artc_web_pointer_move: \{ ret: null, args: \['number', 'number'\] \}/);
  assert.match(workerText, /_artc_web_resize: \{ ret: null, args: \['number', 'number'\] \}/);
  // web_main.cpp:545-546 — save/load take a `const char *` slot name, and both are implemented: the
  // worker calls native with the fixed slot and reads the engine's files back out of the filesystem.
  assert.match(workerText, /_artc_web_save: \{ ret: 'number', args: \['string'\] \}/);
  assert.match(workerText, /_artc_web_load: \{ ret: 'number', args: \['string'\] \}/);
  assert.match(workerText, /cabi\._artc_web_save\(SAVE_SLOT\)/, 'save is called through the bound CABI');
  assert.match(workerText, /cabi\._artc_web_load\(SAVE_SLOT\)/, 'load is called through the bound CABI');
  // A missing native save is still a refusal by name, never an invented success.
  assert.match(workerText, /'save is not implemented'/);
  assert.match(workerText, /'load is not implemented'/);
});

test('a real Node worker reads a >2 GiB source range and a missing patch path costs nothing', async () => {
  const { source, reads } = makeSource();
  const session = new Session({ source });
  try {
    session.init(glue(`
self.Module._artc_web_open = function (pack, mode) {
  self.probe('open=' + self.Module.UTF8ToString(pack) + '|' + self.Module.UTF8ToString(mode));
  self.probe('canvas=' + String(self.document.querySelector('#canvas') === self.Module.canvas));
  var count = self.Module.artcReadRange('/game/root.pfs', ${PAST}, 8, 8);
  self.probe('past=' + count + ',0x' + self.Module.HEAPU8[8].toString(16) + ',' + self.Module.HEAPU8[15].toString(16));
  self.probe('missing=' + self.Module.artcReadRange('/nope', 0, 0, 4));
  self.probe('pastEnd=' + self.Module.artcReadRange('/game/root.pfs', ${SIZE}, 0, 4));
  return 1;
};
`));
    assert.equal((await session.wait('ready')).type, 'ready');
    assert.deepEqual(reads, [{ path: 'root.pfs', offset: PAST, length: 8 }]);
    assert.ok(reads[0].offset > 0x7fffffff, 'an offset past 2 GiB must not sign-wrap');
    assert.deepEqual(session.probes, [
      'open=/game/root.pfs|windows',
      'canvas=true',
      `past=8,0x${byteAt(PAST, 0).toString(16)},${byteAt(PAST, 7).toString(16)}`,
      'missing=-1',
      'pastEnd=-1',
    ]);
    assert.equal(await session.answers[0], 8);
  } finally {
    await session.terminate();
  }
});

test('one request larger than the window is split into at least two reads', async () => {
  const { source, reads } = makeSource();
  const session = new Session({ source, windowBytes: 64 });
  try {
    session.init(glue(`
self.Module._artc_web_open = function () {
  var total = self.Module.artcReadRange('/game/root.pfs', 4096, 0, 200);
  var ok = total === 200;
  for (var i = 0; i < 200; i++) if (self.Module.HEAPU8[i] !== (4096 + i) % 251) ok = false;
  self.probe('split=' + total + ',bytes-ok=' + ok);
  return ok ? 1 : 0;
};
`));
    assert.equal((await session.wait('ready')).type, 'ready');
    assert.ok(reads.length >= 2, `a 200B request in a 64B window must split, got ${reads.length} read(s)`);
    assert.deepEqual(reads.map((read) => read.length), [64, 64, 64, 8]);
    assert.deepEqual(reads.map((read) => read.offset), [4096, 4160, 4224, 4288]);
    assert.deepEqual(session.probes, ['split=200,bytes-ok=true']);
  } finally {
    await session.terminate();
  }
});

test('a short source answer is fatal and its reason reaches the page', async () => {
  const { source } = makeSource({
    readRange: (path, offset, length) => (offset === 0 ? new Uint8Array(length) : new Uint8Array(1)),
  });
  const worker = spawn();
  const page = new CompatWorkerHost({ source, createWorker: () => worker });
  try {
    await assert.rejects(() => page.load({
      engineJsUrl: glue(`
self.Module._artc_web_open = function () {
  if (self.Module.artcReadRange('/game/root.pfs', 0, 0, 8) !== 8) return 0;
  if (self.Module.artcReadRange('/game/root.pfs', 64, 8, 8) !== -1) return 0;
  return 1;
};
`),
      wasmUrl: 'file:///x.wasm', canvas: null, descriptors: DESCRIPTORS,
    }), /short read 1\/8/);
    await page.whenDisposed();
    assert.equal(page.describe().state, 'closed');
  } finally {
    page.dispose();
    await page.whenDisposed();
    await worker.terminate().catch(() => {});
  }
});

test('pre-ready input counts zero; a ready pointer and key reach the native CABI', async () => {
  const { source } = makeSource();
  const session = new Session({ source });
  try {
    session.worker.postMessage({ type: 'input', id: 5, kind: 'key', code: 13, down: true });
    session.init(glue(`
self.Module._artc_web_open = function () { return 1; };
${INPUT_CABI}
`));
    assert.equal((await session.wait('input-ok')).count, 0, 'input before ready must not reach native');
    await session.wait('ready');
    session.worker.postMessage({ type: 'input', id: 6, kind: 'pointer', x: 320.5, y: 180.25, down: true });
    session.worker.postMessage({ type: 'input', id: 7, kind: 'key', code: 13, down: true });
    session.worker.postMessage({ type: 'input', id: 8, kind: 'key', code: 65, down: false });
    await session.wait('input-ok');
    await session.wait('input-ok');
    await session.wait('input-ok');
    assert.deepEqual(session.probes, [
      'pointer=320.5,180.25,1',
      'key=13,1,args=2,keyType=number',
      'key=65,0,args=2,keyType=number',
    ]);
  } finally {
    await session.terminate();
  }
});

test('a read timeout is terminal and a late answer never writes the window', async () => {
  let release = () => {};
  const gate = new Promise((resolve) => {
    release = resolve;
  });
  const { source } = makeSource({ readRange: () => gate.then(() => new Uint8Array(8).fill(7)) });
  const session = new Session({ source, windowBytes: 64 });
  try {
    session.init(glue(`
self.Module._artc_web_open = function () {
  self.probe('frame-before=' + self.Module.artcReadRange('/game/root.pfs', 0, 0, 8));
  return 1;
};
self.Module._artc_web_frame = function () { self.probe('frame'); return 1; };
self.Module._artc_web_pause = function () { return 1; };
`), { readTimeoutMs: 150 });
    await session.wait('read');
    assert.match((await session.wait('fatal')).reason, /read timed out/);
    assert.equal(session.take('ready'), undefined, 'the engine must not be announced ready after a read timeout');
    assert.match((await session.wait('init-failed')).reason, /read timed out/);
    release();
    assert.equal(await session.answers[0], READ_FATAL, 'a late answer is refused');
    assert.equal(session.block.payload[0], 0, 'the window payload must stay untouched');
    assert.equal(Atomics.load(session.block.control, CONTROL.SEQUENCE), session.reads[0].sequence);
    assert.equal(Atomics.load(session.block.control, CONTROL.COUNT), 0);
    assert.deepEqual(session.probes, ['frame-before=-1'], 'no frame may run after the timeout');
  } finally {
    release();
    await session.terminate();
  }
});

test('one inflight frame coalesces, pause is an ordered ack, dispose leaves nothing pending', async () => {
  const { source } = makeSource();
  const worker = spawn();
  const probes = [];
  worker.addEventListener('message', (event) => event.data.type === 'probe' && probes.push(event.data.line));
  const page = new CompatWorkerHost({ source, createWorker: () => worker });
  const results = [];
  const collect = (promise, label) => promise.then(v => results.push(`${label}=${v}`), e => results.push(`${label}!${e.message}`));
  try {
    await page.load({
      engineJsUrl: glue(`
self.Module._artc_web_open = function () { return 1; };
self.Module._artc_web_frame = function () { self.probe('frame'); return 1; };
self.Module._artc_web_pause = function (v) { self.probe('pause=' + v); return 1; };
self.Module._artc_web_dispose = function () { self.probe('dispose'); };
`),
      wasmUrl: 'file:///x.wasm', canvas: null, descriptors: DESCRIPTORS,
    });
    await Promise.all([collect(page.frame(), 'a'), collect(page.frame(), 'b'), collect(page.frame(), 'c')]);
    await collect(page.setPaused(true), 'pause');
    await collect(page.frame(), 'paused');
    await collect(page.checkpoint(), 'checkpoint');
    page.dispose();
    await page.whenDisposed();
    await new Promise((resolve) => setTimeout(resolve, 50));
    // This glue exports no `_artc_web_save`, so the checkpoint is refused by name. That is the point:
    // a build without the native call must say so rather than resolving an empty checkpoint.
    assert.deepEqual(results, ['a=true', 'b=true', 'c=true', 'pause=undefined', 'paused=true', 'checkpoint!save is not implemented']);
    assert.deepEqual(probes, ['frame', 'pause=1', 'dispose'], 'three callers share one native frame, then one ack and one shutdown');
    assert.equal(page.describe().state, 'closed');
  } finally {
    page.dispose();
    await page.whenDisposed();
    await worker.terminate().catch(() => {});
  }
});

test('the canvas is transferred, not cloned', async () => {
  const { source } = makeSource();
  const worker = spawn();
  const page = new CompatWorkerHost({ source, createWorker: () => worker });
  // Node has no OffscreenCanvas. A MessagePort stands in: postMessage refuses an untransferred
  // port outright, so a successful load is proof the transfer list carried it.
  const channel = new MessageChannel();
  const probes = [];
  worker.addEventListener('message', (event) => event.data.type === 'probe' && probes.push(event.data.line));
  try {
    await page.load({
      engineJsUrl: glue(`
self.Module._artc_web_open = function () {
  self.probe('canvas=' + String(self.document.querySelector('#canvas') === self.Module.canvas));
  self.probe('canvasKind=' + (self.Module.canvas && self.Module.canvas.constructor.name));
  return 1;
};
`),
      wasmUrl: 'file:///x.wasm', canvas: channel.port1, descriptors: DESCRIPTORS,
    });
    await new Promise((resolve) => setTimeout(resolve, 50));
    assert.deepEqual(probes, ['canvas=true', 'canvasKind=MessagePort']);
    assert.equal(page.describe().state, 'ready');
  } finally {
    page.dispose();
    await page.whenDisposed();
    channel.port1.close();
    channel.port2.close();
    await worker.terminate().catch(() => {});
  }
});

test('a frame ACK carries the NATIVE counters, not a page-side increment', async () => {
  const { source } = makeSource();
  const session = new Session({ source });
  try {
    session.init(glue(`
var __frames = 0;
self.Module._artc_web_open = function () { return 1; };
/* The engine's redraw gate: revision moves every frame, the present does not. A static ADV scene
   is exactly this, and a worker that reported one presented frame per ACK would be wrong. */
self.Module._artc_web_frame = function () { __frames++; return 1; };
self.Module._artc_web_started = function () { return 1; };
self.Module._artc_web_revision = function () { return __frames; };
self.Module._artc_web_presented = function () { return 5; };
self.Module._artc_web_stage_width = function () { return 1280; };
self.Module._artc_web_now_ms = function () { return 1234.5; };
`));
    await session.wait('ready');
    for (const id of [10, 11, 12]) session.worker.postMessage({ type: 'frame', id });
    const first = await session.wait('frame-ok');
    const second = await session.wait('frame-ok');
    const third = await session.wait('frame-ok');
    assert.deepEqual(
      [first, second, third].map((m) => [m.running, m.started, m.presented, m.revision, m.stage, m.nowMs]),
      [
        [true, 1, 5, 1, 1280, 1234.5],
        [true, 1, 5, 2, 1280, 1234.5],
        [true, 1, 5, 3, 1280, 1234.5],
      ],
      'three ACKs report three different revisions and ONE presented count',
    );
    assert.equal(third.presented, first.presented, 'the redraw gate skipping a present must be visible');
    assert.notEqual(third.revision, first.revision, 'the revision did move');
  } finally {
    await session.terminate();
  }
});

test('a counter the build does not export is ABSENT, never an invented zero', async () => {
  const { source } = makeSource();
  const session = new Session({ source });
  try {
    session.init(glue(`
self.Module._artc_web_open = function () { return 1; };
self.Module._artc_web_frame = function () { return 1; };
/* _artc_web_started / _presented / _revision / _stage_width are deliberately missing here: this is
   what an older or partial build looks like. undefined serializes to an absent field. */
`));
    await session.wait('ready');
    session.worker.postMessage({ type: 'frame', id: 21 });
    const ack = await session.wait('frame-ok');
    assert.equal(ack.running, true);
    for (const field of ['started', 'presented', 'revision', 'stage', 'nowMs']) {
      // `undefined`, not 0. Structured clone keeps the key and drops the value, so the check is on
      // the value: the two must never collapse, because 0 reads as "the engine presented nothing".
      assert.equal(ack[field], undefined, `${field} must be undefined, so the page reports it unverified`);
      assert.notEqual(ack[field], 0, `${field} must never be reported as a zero`);
    }
  } finally {
    await session.terminate();
  }
});

test('a false frame is a session end and reports the native reason', async () => {
  const { source } = makeSource();
  const worker = spawn();
  const page = new CompatWorkerHost({ source, createWorker: () => worker });
  try {
    await page.load({
      engineJsUrl: glue(`
var __reason = self.Module._malloc(16);
self.Module.stringToUTF8('script finished', __reason);
self.Module._artc_web_open = function () { return 1; };
self.Module._artc_web_frame = function () { return 0; };
self.Module._artc_web_pause = function () { return 1; };
self.Module._artc_web_last_error = function () { return __reason; };
`),
      wasmUrl: 'file:///x.wasm', canvas: null, descriptors: DESCRIPTORS,
    });
    await assert.rejects(() => page.frame(), /script finished/);
    await page.whenDisposed();
    assert.equal(page.describe().state, 'closed');
  } finally {
    page.dispose();
    await page.whenDisposed();
    await worker.terminate().catch(() => {});
  }
});

test('the page refuses out-of-range reads and treats an unknown patch path as expected absence', async () => {
  const { source } = makeSource();
  const block = allocateCompatBlock();
  const host = new CompatReadHost(source, block, DESCRIPTORS);
  const sequence = (Atomics.load(block.control, CONTROL.SEQUENCE) + 1) | 0;
  Atomics.store(block.control, CONTROL.SEQUENCE, sequence);
  assert.equal(await host.answer({ fileId: 1, offset: SIZE, length: 8, sequence }), READ_FATAL);
  assert.match(host.lastReason, /read past end/);
  assert.equal(Atomics.load(block.control, CONTROL.COUNT), READ_FATAL);
  Atomics.store(block.control, CONTROL.SEQUENCE, sequence);
  assert.equal(await host.answer({ fileId: 9, offset: 0, length: 8, sequence }), READ_MISSING);
  assert.equal(Atomics.load(block.control, CONTROL.COUNT), READ_MISSING);
});

/* Production get reads canvas.width. Production set assigns it and returns 0, or -4 when the
 * selector misses. Neither exists in this build's glue (name count is 0), so the fake is the
 * boundary, not a claim that native ran. */
const GET_SIZE = `
function _emscripten_get_canvas_element_size(target, width, height) {
  var canvas = self.document.querySelector(self.Module.UTF8ToString(target));
  if (!canvas) return -4;
  self.Module.HEAP32[width >>> 2] = canvas.width;
  self.Module.HEAP32[height >>> 2] = canvas.height;
  return 0;
}
`;

function resizeGlue() {
  return glue(`
${GET_SIZE}
var css = [1280, 720];
self.Module._artc_web_open = function () {
  self.__gl = self.Module.canvas.getContext('webgl2');
  self.probe('open-backing=' + self.Module.canvas.width + 'x' + self.Module.canvas.height);
  return 1;
};
self.Module._artc_web_resize = function (w, h) {
  css[0] = w | 0; css[1] = h | 0;
  self.probe('call=resize');
};
self.Module._artc_web_frame = function () {
  var wp = self.Module._malloc(4), hp = self.Module._malloc(4);
  var sel = self.Module._malloc(16);
  self.Module.stringToUTF8('#canvas', sel);
  _emscripten_get_canvas_element_size(sel, wp, hp);
  var fw = self.Module.HEAP32[wp >>> 2], fh = self.Module.HEAP32[hp >>> 2];
  self.Module._free(wp); self.Module._free(hp); self.Module._free(sel);
  self.probe('present=' + fw + 'x' + fh + ',css=' + css[0] + 'x' + css[1]
    + ',same=' + String(self.document.querySelector('#canvas') === self.Module.canvas)
    + ',ctx=' + String(self.Module.canvas.getContext('webgl2') === self.__gl));
  self.probe('call=frame');
  return 1;
};
self.Module._artc_web_started = function () { return 1; };
`);
}

/** Node has no OffscreenCanvas. The buffer is the only thing that crosses the worker boundary;
 * the bootstrap reattaches width/height/getContext on arrival. A MessagePort cannot carry a size. */
function backing(width, height) {
  const bytes = new ArrayBuffer(8);
  new Int32Array(bytes).set([width, height]);
  return { canvas: { buffer: bytes }, transfer: [bytes], close() {} };
}

async function bootSurface(width, height) {
  const { source } = makeSource();
  const session = new Session({ source });
  const surface = backing(width, height);
  try {
  session.worker.postMessage({
    type: 'init', id: 1, canvas: surface.canvas, engineJsUrl: resizeGlue(),
    wasmUrl: 'file:///artemis-compat.wasm', descriptors: DESCRIPTORS, block: session.block.buffer,
    surface: { width, height },
  }, surface.transfer);
  const deadline = Date.now() + 1500;
  let ready = false;
  while (Date.now() < deadline) {
    const failed = session.messages.find((message) => message.type === 'init-failed' || message.type === 'fatal' || message.type === 'worker-error');
    if (failed) throw new Error(`${failed.type}: ${failed.reason}; saw ${JSON.stringify(session.messages)}`);
    if (session.take('ready')) { ready = true; break; }
    await new Promise((resolve) => setTimeout(resolve, 5));
  }
  if (!ready) throw new Error(`no ready; saw ${JSON.stringify(session.messages)} probes=${JSON.stringify(session.probes)}`);
  } catch (error) {
    await session.terminate();
    throw error;
  }
  const resize = async (w, h) => {
    session.worker.postMessage({ type: 'input', id: 2, kind: 'resize', width: w, height: h });
    await session.wait('input-ok');
  };
  const frame = async () => {
    session.worker.postMessage({ type: 'frame', id: 3 });
    await session.wait('frame-ok');
  };
  return { session, probes: session.probes, surface, resize, frame };
}

test('boot 1280x720: Present reads the transferred backing, not a 300 default', async () => {
  let boot;
  try { boot = await bootSurface(1280, 720); } catch (error) { throw error; }
  try {
    await boot.resize(1280, 720);
    await boot.frame();
    assert.deepEqual(boot.probes.filter((line) => line.startsWith('call=')), ['call=resize', 'call=frame']);
    assert.deepEqual(boot.probes.filter((line) => !line.startsWith('call=')), [
      'open-backing=1280x720',
      'present=1280x720,css=1280x720,same=true,ctx=true',
    ]);
  } finally {
    boot.surface.close();
    await boot.session.terminate();
  }
});

test('resize 1920x1080 then 640x360 updates the same backing Present reads', async () => {
  const boot = await bootSurface(1280, 720);
  try {
    await boot.resize(1280, 720);
    await boot.frame();
    await boot.resize(1920, 1080);
    await boot.frame();
    await boot.resize(640, 360);
    await boot.frame();
    assert.deepEqual(boot.probes.filter((line) => line.startsWith('call=')), [
      'call=resize', 'call=frame', 'call=resize', 'call=frame', 'call=resize', 'call=frame',
    ]);
    assert.deepEqual(boot.probes.filter((line) => !line.startsWith('call=')), [
      'open-backing=1280x720',
      'present=1280x720,css=1280x720,same=true,ctx=true',
      'present=1920x1080,css=1920x1080,same=true,ctx=true',
      'present=640x360,css=640x360,same=true,ctx=true',
    ]);
    await boot.resize(undefined, undefined);
    await boot.frame();
    await boot.resize(Number.NaN, 400);
    await boot.frame();
    assert.deepEqual(boot.probes.filter((line) => line.startsWith('present=')).slice(-2), [
      'present=640x360,css=0x0,same=true,ctx=true',
      'present=640x360,css=0x400,same=true,ctx=true',
    ]);
  } finally {
    boot.surface.close();
    await boot.session.terminate();
  }
});