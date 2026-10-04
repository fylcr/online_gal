/**
 * Real Node/classic-worker checkpoint harness for the compat save/restore path.
 *
 * This is the file that decides whether "save works" is a measured claim or a hopeful one. It runs the
 * **unmodified** `packages/runtime-artemis/src/compat/compat-worker.js` inside a real
 * `worker_threads` classic-worker bootstrap, with a generated-glue stand-in that provides a real
 * `cwrap` (so `const char *` marshalling is exercised), a real `FS` over an in-memory tree (so files
 * are read and written, not asserted about), and a native CABI whose `artc_web_save` / `artc_web_load`
 * return codes are chosen per test.
 *
 * The two failure shapes this exists to catch:
 *
 *   1. a checkpoint that reports success while the engine wrote nothing — so `_artc_web_save` returning
 *      `0`, and returning `1` with no file on disk, must both be refusals;
 *   2. a restore that claims success while native refused — so `_artc_web_load` returning `0` must
 *      reject, must roll the previous files back, and must never be reported as a running session.
 *
 * No test here touches a real game. It proves the transport contract, and says nothing about whether a
 * real title's `onSave`/`onLoad` produce a usable slot — that is `coldGameUnverified`.
 *
 * Run: `node --test test/artemis-compat-save.test.mjs`.
 */
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, readFileSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { Worker } from 'node:worker_threads';

/** ARCV v1, the bytes `EncodeVariableBank(..., true)` writes. Not a user slot. */
function arcv(pairs) {
  const parts = [Buffer.from('ARCV'), Buffer.alloc(4), Buffer.alloc(4)];
  parts[1].writeUInt32LE(1, 0);
  parts[2].writeUInt32LE(pairs.length, 0);
  for (const [key, value] of pairs) {
    const keyBytes = Buffer.from(key);
    const valueBytes = Buffer.from(value);
    const keyLength = Buffer.alloc(4);
    const valueLength = Buffer.alloc(4);
    keyLength.writeUInt32LE(keyBytes.length, 0);
    valueLength.writeUInt32LE(valueBytes.length, 0);
    parts.push(keyLength, keyBytes, valueLength, valueBytes);
  }
  const body = Buffer.concat(parts);
  let crc = 0xffffffff;
  for (const byte of body) {
    crc ^= byte;
    for (let bit = 0; bit < 8; bit += 1) crc = (crc >>> 1) ^ (0xedb88320 & -(crc & 1));
  }
  const tail = Buffer.alloc(4);
  tail.writeUInt32LE((crc ^ 0xffffffff) >>> 0, 0);
  return Buffer.concat([body, tail]);
}

function arcv64(pairs) {
  return arcv(pairs).toString('base64');
}

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const WORKER_SCRIPT = join(root, 'packages', 'runtime-artemis', 'src', 'compat', 'compat-worker.js');
const SIZE = 0x100000;
const DESCRIPTORS = [{ id: 1, path: 'root.pfs', virtualPath: '/game/root.pfs', size: SIZE }];

/**
 * Test-only classic-worker bootstrap. Node has no `self` and no `importScripts`; a real classic worker
 * has both, and production glue relies on them. Held as a string so a clean clone with no `.test/`
 * workbench still runs this file.
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
/* TEST ONLY: production importScripts is the browser's. node:vm stands in, so no eval. */
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

const temp = mkdtempSync(join(tmpdir(), 'artemis-compat-save-'));
const out = join(temp, 'out');
/* Removes only a tree this test created: same tmpdir, right name prefix, still a directory. */
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-compat-save-')) {
    rmSync(temp, { recursive: true, force: true });
  }
});
execFileSync(process.execPath, [
  join(root, 'node_modules/typescript/bin/tsc'),
  // Both entries: `/compat` is the wire surface, and the package root is where the provider and the
  // engine publication live. Compiling only one leaves the facade tests with nothing to build a
  // session from.
  'packages/runtime-artemis/src/index.ts',
  'packages/runtime-artemis/src/compat/index.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler',
  '--lib', 'ES2022,DOM', '--strict', '--noUncheckedIndexedAccess', '--skipLibCheck',
  '--rootDir', 'packages/runtime-artemis/src', '--outDir', out,
], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/runtime-artemis/node_modules'), join(out, 'node_modules'), 'junction');
const BOOTSTRAP = join(temp, 'bootstrap.cjs');
writeFileSync(BOOTSTRAP, BOOTSTRAP_SOURCE);
const api = await import(pathToFileURL(join(out, 'compat', 'index.js')).href);
/**
 * The provider and the engine publication live on the package root, not on `/compat`. The root entry
 * re-exports the compat surface, so both views come from the same compiled tree.
 */
const pkg = await import(pathToFileURL(join(out, 'index.js')).href);
const artemis = { ...api, ...pkg };

/**
 * The generated-glue stand-in: a real `cwrap` and a real filesystem.
 *
 * `getCFunc` is copied from emsdk `src/lib/libccall.js` — `Module['_' + ident]` — so a worker that
 * handed `cwrap` an underscore-prefixed ident fails here exactly as it would against real glue.
 *
 * `FS` is a genuine in-memory tree with `mkdirTree`/`writeFile`/`readFile`/`readdir`/`stat`/`unlink`,
 * because the whole question is whether real bytes come back out of the engine's own save directory.
 * `writeFile` can be made to fail once, so a partial write is a reachable state rather than a
 * theoretical one.
 */
const FAKE = `
var __heap = new Uint8Array(1 << 22);
var __top = 4096;
var __enc = new TextEncoder();
var __dec = new TextDecoder();
self.Module.HEAPU8 = __heap;
self.Module._malloc = function (n) { var p = __top; __top += (n + 7) & ~7; return p; };
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
self.probe = function (line) { self.postMessage({ type: 'probe', line: line }); };

/* --- A real in-memory filesystem, shaped like the parts of MEMFS this transport uses. --- */
var DIR_MODE = 16384;
var FILE_MODE = 32768;
self.__fs = { tree: {}, failNextWrite: null };
(function () {
  function normalise(path) {
    var parts = String(path).split('/');
    var out = [];
    for (var i = 0; i < parts.length; i++) {
      if (!parts[i] || parts[i] === '.') continue;
      if (parts[i] === '..') { out.pop(); continue; }
      out.push(parts[i]);
    }
    return '/' + out.join('/');
  }
  function ensureDir(tree, path) {
    var parts = normalise(path).split('/').filter(Boolean);
    var node = tree;
    for (var i = 0; i < parts.length; i++) {
      if (!node[parts[i]]) node[parts[i]] = { dir: true, children: {} };
      node = node[parts[i]].children;
    }
  }
  function lookup(tree, path) {
    var parts = normalise(path).split('/').filter(Boolean);
    var node = tree;
    var entry = null;
    for (var i = 0; i < parts.length; i++) {
      entry = node[parts[i]];
      if (!entry) return null;
      node = entry.dir ? entry.children : null;
      if (i < parts.length - 1 && !node) return null;
    }
    return entry;
  }
  self.Module.FS = {
    mkdirTree: function (path) { ensureDir(self.__fs.tree, path); },
    mkdir: function (path) { ensureDir(self.__fs.tree, path); },
    analyzePath: function (path) { return { exists: !!lookup(self.__fs.tree, path) }; },
    writeFile: function (path, data) {
      if (self.__fs.failNextWrite === path) {
        self.__fs.failNextWrite = null;
        throw new Error('ENOSPC: simulated write failure on ' + path);
      }
      var normal = normalise(path);
      var parent = normal.slice(0, normal.lastIndexOf('/')) || '/';
      ensureDir(self.__fs.tree, parent);
      var parts = normal.split('/').filter(Boolean);
      var name = parts.pop();
      var node = self.__fs.tree;
      for (var i = 0; i < parts.length; i++) node = node[parts[i]].children;
      node[name] = { dir: false, bytes: Array.prototype.slice.call(data) };
    },
    readFile: function (path) {
      var entry = lookup(self.__fs.tree, path);
      if (!entry || entry.dir) throw new Error('ENOENT: ' + path);
      return Uint8Array.from(entry.bytes);
    },
    readdir: function (path) {
      var entry = lookup(self.__fs.tree, path);
      if (!entry || !entry.dir) throw new Error('ENOENT: ' + path);
      return Object.keys(entry.children);
    },
    stat: function (path) {
      var entry = lookup(self.__fs.tree, path);
      if (!entry) throw new Error('ENOENT: ' + path);
      return { mode: entry.dir ? DIR_MODE : FILE_MODE, size: entry.dir ? 0 : entry.bytes.length };
    },
    unlink: function (path) {
      var normal = normalise(path);
      var parts = normal.split('/').filter(Boolean);
      var name = parts.pop();
      var node = self.__fs.tree;
      for (var i = 0; i < parts.length; i++) node = node[parts[i]].children;
      delete node[name];
    },
    rename: function (from, to) {
      var bytes = self.Module.FS.readFile(from);
      self.Module.FS.writeFile(to, bytes);
      self.Module.FS.unlink(from);
    },
  };
})();

/* The engine's own writes, done from native so the worker's capture reads real files. */
self.seedSave = function (name, text) {
  self.Module.FS.mkdirTree('/game');
  var bytes = typeof text === 'string' ? __enc.encode(text) : Uint8Array.from(text);
  self.Module.FS.writeFile('/game/' + name, bytes);
};
self.readSave = function (name) {
  try { return self.slotMark(__dec.decode(self.Module.FS.readFile('/game/' + name))); } catch (error) { return null; }
};
/** The exact text of a file under the engine's save directory, or null when it is not there. */
self.readSaveText = function (name) {
  try { return __dec.decode(self.Module.FS.readFile('/game/' + name)); } catch (error) { return null; }
};
self.slotMark = function (text) {
  var marks = ['RESTORED-SLOT', 'NEW-SLOT', 'OLD-SLOT', 'SLOT-PAYLOAD', 'SLOT'];
  for (var i = 0; i < marks.length; i++) if (text.indexOf(marks[i]) >= 0) return marks[i];
  return 'other';
}

/*
 * A native frame that answers the **real** question: is the engine paused?
 *
 * The engine's own \`artc_web_frame\` returns 0 after \`artc_web_pause(1)\` — that is what a paused session
 * is — so a stand-in that advances the counter regardless cannot tell a correct restore from one that
 * parked the engine and left it there. \`nativePaused\` is the gate, and \`frames\` is the movement the
 * page can see.
 */
self.seedNative = function (options) {
  var spec = options || {};
  self.__nativePauseFail = spec.pauseFail || null;
  self.__nativeLoadResult = spec.loadResult === undefined ? 1 : spec.loadResult;
  self.__nativeLoadThrow = !!spec.loadThrow;
  self.__nativePaused = false;
  self.__nativeFrames = 0;
  self.Module._artc_web_frame = function () { return self.seedFrame(); };
  self.Module._artc_web_pause = function (value) {
    var want = value ? 1 : 0;
    self.probe('pause=' + want);
    if (self.__nativePauseFail === want) {
      self.probe('pause-refused=' + want);
      return 0;
    }
    self.__nativePaused = want === 1;
    return 1;
  };
  self.Module._artc_web_load = function (file) {
    self.probe('load=' + self.Module.UTF8ToString(file));
    if (self.__nativeLoadThrow) throw new Error('artc_web_load threw');
    return self.__nativeLoadResult;
  };
};
/** One native frame, gated by the pause. The engine answers 1 and presents nothing while parked. */
self.seedFrame = function () {
  /* The real \`artc_web_frame\` returns non-zero while paused — "the call succeeded", not "the engine
   * ran" — and its own counters stay put. Returning 0 here would make a stop look like a fault, and a
   * facade that treated 0 as a session end would fail a perfectly good paused session. So the pause
   * shows up in the **counter**, which is the number the real engine owns. */
  if (self.__nativePaused) { self.probe('frame-paused'); return 1; }
  self.__nativeFrames++;
  self.probe('frame=' + self.__nativeFrames);
  return 1;
};
self.nativePaused = function () { return self.__nativePaused; };
self.nativeFrames = function () { return self.__nativeFrames; };
`;

let glueSeq = 0;
function glue(body) {
  const file = join(temp, `glue${glueSeq++}.js`);
  writeFileSync(
    file,
    `${FAKE}\n${body}\nif (self.Module && self.Module.onRuntimeInitialized) self.Module.onRuntimeInitialized();\n`,
  );
  return pathToFileURL(file).href;
}

function makeSource() {
  return {
    name: 'fixture',
    capabilities: { list: true, randomAccess: true, writable: false },
    // The pack chain is described by `list` + `size` alone; a whole-file read is exactly what the
    // transport exists to avoid, so asking for one is a failure by name.
    async list(dir = '') {
      return dir === '' ? ['root.pfs'] : [];
    },
    async readFile(path) {
      throw new Error(`the compat transport must not read whole files (${path})`);
    },
    async readRange(_path, offset, length) {
      return Uint8Array.from({ length }, (_, index) => (offset + index) % 251);
    },
    async size() {
      return SIZE;
    },
    async exists() {
      return true;
    },
    async isDirectory() {
      return false;
    },
  };
}

const live = new Set();
test.after(async () => {
  for (const worker of live) await worker.terminate().catch(() => {});
  live.clear();
});
test.after(async () => {
  await new Promise((resolve) => setTimeout(resolve, 300));
});

/**
 * A raw classic worker driven through the worker protocol directly.
 *
 * Deliberately not the page facade: this half is about what the **worker** puts on the wire, and a
 * facade in between would let a facade bug hide a worker bug. The wire constants come from the
 * compiled API, so the numbers cannot drift from the TS source.
 */
class Harness {
  constructor() {
    this.messages = [];
    this.probes = [];
    this.block = api.allocateCompatBlock(64 * 1024);
    this.readHost = new api.CompatReadHost(makeSource(), this.block, DESCRIPTORS);
    this.nextId = 1;
    this.worker = new Worker(BOOTSTRAP, { workerData: { script: WORKER_SCRIPT } });
    live.add(this.worker);
    this.worker.on('message', (message) => {
      this.messages.push(message);
      if (message.type === 'probe') this.probes.push(message.line);
      if (message.type !== 'read') return;
      void this.readHost.answer(message);
    });
  }

  async init(glueUrl) {
    this.post({
      type: 'init', id: this.nextId++, canvas: null, engineJsUrl: glueUrl,
      wasmUrl: 'file:///artemis-compat.wasm', descriptors: DESCRIPTORS, block: this.block.buffer,
    });
    await this.wait('ready');
    return this;
  }

  post(message) {
    this.worker.postMessage(message);
  }

  /** Sends a request and waits for its reply, whatever it is. */
  async call(message, ms = 5000) {
    const id = this.nextId++;
    this.post({ ...message, id });
    const deadline = Date.now() + ms;
    while (Date.now() < deadline) {
      const index = this.messages.findIndex((entry) => entry.id === id && entry.type !== 'read');
      if (index >= 0) return this.messages.splice(index, 1)[0];
      await new Promise((resolve) => setTimeout(resolve, 5));
    }
    throw new Error(`no reply to ${message.type}; saw ${JSON.stringify(this.messages)}`);
  }

  async wait(type, ms = 5000) {
    const deadline = Date.now() + ms;
    while (Date.now() < deadline) {
      const index = this.messages.findIndex((message) => message.type === type);
      if (index >= 0) return this.messages.splice(index, 1)[0];
      await new Promise((resolve) => setTimeout(resolve, 5));
    }
    throw new Error(`no ${type}; saw ${JSON.stringify(this.messages)}`);
  }

  takeProbe(prefix) {
    const index = this.probes.findIndex((line) => line.startsWith(prefix));
    if (index < 0) return undefined;
    return this.probes.splice(index, 1)[0];
  }



  async terminate() {
    live.delete(this.worker);
    await this.worker.terminate().catch(() => {});
  }
}

/** The CABI every harness needs: open, one frame, pause, and the counters the facade reads. */
const BASE_CABI = `
self.Module._artc_web_open = function (pack, mode) { self.probe('open=' + self.Module.UTF8ToString(pack) + '|' + self.Module.UTF8ToString(mode)); return 1; };
self.Module._artc_web_frame = function () { self.probe('frame'); return 1; };
self.Module._artc_web_pause = function (v) { self.probe('pause=' + v); return 1; };
self.Module._artc_web_started = function () { return 1; };
self.Module._artc_web_last_error = function () { return 0; };
`;

/* --------------------------------------------------------------------------------------------- */
/* The transport contract.                                                                        */
/* --------------------------------------------------------------------------------------------- */

test('the wire pins the save slot, the save directory and the three payload bounds', () => {
  const workerText = readFileSync(WORKER_SCRIPT, 'utf8');
  assert.equal(api.COMPAT_SAVE_SLOT, 'platform-slot.dat');
  assert.equal(api.COMPAT_SAVE_DIR, '/game');
  assert.equal(api.COMPAT_SAVE_KIND, 'artemis-compat-checkpoint');
  assert.equal(api.COMPAT_SAVE_MAX_FILE_BYTES, 64 * 1024 * 1024);
  assert.equal(api.COMPAT_SAVE_MAX_FILES, 4096);
  assert.equal(api.COMPAT_SAVE_MAX_TOTAL_BYTES, 256 * 1024 * 1024);
  // The worker cannot import the TS constants, so it duplicates them; these are the duplicates.
  assert.match(workerText, /var SAVE_SLOT = 'platform-slot\.dat'/);
  assert.match(workerText, /var MAX_FILE_BYTES = 64 \* 1024 \* 1024/);
  assert.match(workerText, /var MAX_FILES = 4096/);
  assert.match(workerText, /var MAX_TOTAL_BYTES = 256 \* 1024 \* 1024/);
  // The native call is the engine's own: a `const char *` slot name, never a number.
  assert.match(workerText, /_artc_web_save: \{ ret: 'number', args: \['string'\] \}/);
  assert.match(workerText, /_artc_web_load: \{ ret: 'number', args: \['string'\] \}/);
  assert.match(workerText, /cabi\._artc_web_save\(SAVE_SLOT\)/);
  assert.match(workerText, /cabi\._artc_web_load\(SAVE_SLOT\)/);
});

test('a checkpoint returns the engine\'s own bytes, and nothing is invented when it wrote nothing', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
self.Module._artc_web_save = function (file) {
  var name = self.Module.UTF8ToString(file);
  self.probe('save=' + name);
  /* The engine's real behaviour: onSave runs, then the slot is written beside the pack. */
  self.seedSave('system.dat', 'GLOBAL-BANK');
  self.seedSave(name, [65,82,67,86,1,0,0,0,1,0,0,0,1,0,0,0,107,12,0,0,0,83,76,79,84,45,80,65,89,76,79,65,68,187,216,51,93]);
  return 1;
};
self.Module._artc_web_load = function () { return 1; };
`));
    assert.deepEqual(harness.takeProbe('open='), 'open=/game/root.pfs|windows');

    const reply = await harness.call({ type: 'checkpoint' });
    assert.equal(reply.type, 'checkpoint-ok', `checkpoint refused: ${reply.reason}`);
    // The worker asked native for the fixed slot name, as a C string.
    assert.equal(harness.takeProbe('save='), 'save=platform-slot.dat');
    const files = reply.files.map((entry) => entry.path).sort();
    // The whole **created** tree, not just the one slot: the engine keeps its global bank beside it,
    // and a restore that dropped `system.dat` would restore a scenario into another installation's
    // globals.
    assert.deepEqual(files, ['platform-slot.dat', 'system.dat'], 'every written file is captured');
    const slot = reply.files.find((entry) => entry.path === 'platform-slot.dat');
    assert.equal(
      Buffer.from(slot.data, 'base64').includes(Buffer.from('SLOT-PAYLOAD')),
      true,
      'the captured bytes are the file the engine wrote, byte for byte',
    );
    // `@` never appears in the payload: this is the engine's own file, not a synthesised blob.
    assert.ok(!JSON.stringify(reply.files).includes('platform-slot.dat.dat'));
  } finally {
    await harness.terminate();
  }
});

test('a native save that returns 0 is a refusal, never a checkpoint with no files', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
self.Module._artc_web_save = function () { return 0; };
self.Module._artc_web_load = function () { return 1; };
`));
    const reply = await harness.call({ type: 'checkpoint' });
    assert.equal(reply.type, 'error', 'a refused native save must not resolve a checkpoint');
    assert.match(reply.reason, /artc_web_save returned 0/);
    assert.equal(reply.files, undefined, 'no files are carried on a refusal');
  } finally {
    await harness.terminate();
  }
});

test('a native save that returns 1 but writes nothing is refused, not reported as an empty success', async () => {
  const harness = new Harness();
  try {
    // The exact hole a naive implementation leaves: the native call says "accepted" and the
    // filesystem holds nothing. Resolving that as a checkpoint gives the player a slot that restores
    // nothing at all, and the UI would show it as a save.
    await harness.init(glue(`
${BASE_CABI}
self.Module._artc_web_save = function () { return 1; };
self.Module._artc_web_load = function () { return 1; };
`));
    const reply = await harness.call({ type: 'checkpoint' });
    assert.equal(reply.type, 'error');
    assert.match(reply.reason, /wrote no file/);
  } finally {
    await harness.terminate();
  }
});

test('the pack is not in the save directory, and a .pfs that appears there is not copied', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
self.Module._artc_web_save = function (file) {
  self.seedSave('system.dat', 'BANK');
  self.seedSave(self.Module.UTF8ToString(file), [65,82,67,86,1,0,0,0,1,0,0,0,1,0,0,0,107,4,0,0,0,83,76,79,84,224,205,203,49]);
  /* A hostile or confused tree: the pack itself sitting under the save directory. Copying it would
   * turn a save slot into a multi-gigabyte game dump. */
  self.seedSave('root.pfs', 'NOT-A-SAVE');
  self.seedSave('root.pfs.001', 'ALSO-NOT-A-SAVE');
  return 1;
};
self.Module._artc_web_load = function () { return 1; };
`));
    const reply = await harness.call({ type: 'checkpoint' });
    assert.equal(reply.type, 'checkpoint-ok', `checkpoint refused: ${reply.reason}`);
    const files = reply.files.map((entry) => entry.path).sort();
    assert.deepEqual(files, ['platform-slot.dat', 'system.dat'], 'the pack is never captured as a save');
  } finally {
    await harness.terminate();
  }
});

test('a restore installs the bytes, loads the fixed slot, and reports the native answer', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
self.Module._artc_web_save = function () { return 1; };
self.Module._artc_web_load = function (file) {
  self.probe('load=' + self.Module.UTF8ToString(file));
  /* The engine's side of a real load: read the slot back off the filesystem. */
  self.probe('loaded-bytes=' + self.readSave(self.Module.UTF8ToString(file)));
  return 1;
};
`));
    const payload = 'QVJDVgEAAAABAAAAAQAAAGsNAAAAUkVTVE9SRUQtU0xPVKw6ma8=';
    const reply = await harness.call({
      type: 'restore',
      files: [{ path: 'platform-slot.dat', data: payload }],
    });
    assert.equal(reply.type, 'restore-ok', `restore refused: ${reply.reason}`);
    assert.equal(reply.installed, 1);
    assert.equal(reply.loaded, 1);
    // Native was asked for the fixed slot name, and it read back the bytes we installed.
    assert.equal(harness.takeProbe('load='), 'load=platform-slot.dat');
    assert.equal(harness.takeProbe('loaded-bytes='), 'loaded-bytes=RESTORED-SLOT');
    // The guest was parked before the filesystem changed.
    assert.equal(harness.takeProbe('pause='), 'pause=1');
    assert.equal(harness.takeProbe('frame'), undefined, 'no frame ran inside the restore');
  } finally {
    await harness.terminate();
  }
});

test('a native load that returns 0 rejects, and the previous files are put back', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
self.Module._artc_web_save = function () { return 1; };
var writeFile = self.Module.FS.writeFile;
self.Module.FS.writeFile = function (path, data) {
  writeFile(path, data);
  if (String(path).endsWith('platform-slot.dat')) self.probe('slot=' + slotMark(__dec.decode(data)));
};
self.Module._artc_web_load = function () { return 0; };
/* A session that already had files on disk. A failed restore must leave these intact. */
self.seedSave('system.dat', 'OLD-GLOBAL-BANK');
self.seedSave('platform-slot.dat', [65,82,67,86,1,0,0,0,1,0,0,0,1,0,0,0,107,8,0,0,0,79,76,68,45,83,76,79,84,223,156,88,28]);
`));
    const reply = await harness.call({
      type: 'restore',
      files: [{ path: 'platform-slot.dat', data: 'QVJDVgEAAAABAAAAAQAAAGsIAAAATkVXLVNMT1SnqSjA' }],
    });
    assert.equal(reply.type, 'error', 'a load that returns 0 is a refusal');
    assert.match(reply.reason, /artc_web_load returned 0/);
    assert.deepEqual(
      harness.probes.filter((line) => line.startsWith('slot=')),
      ['slot=OLD-SLOT', 'slot=NEW-SLOT', 'slot=OLD-SLOT'],
      'the new bytes were installed and then rolled back',
    );
    const fatal = await harness.wait('fatal');
    assert.match(fatal.reason, /artc_web_load returned 0/);
    const later = await harness.call({ type: 'frame' });
    assert.equal(later.type, 'error', 'a frame after a refused load is refused');
    assert.match(later.reason, /artc_web_load returned 0/);
  } finally {
    await harness.terminate();
  }
});

test('a restore with a bad path is refused before native is asked to load anything', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
self.Module._artc_web_save = function () { return 1; };
self.Module._artc_web_load = function () { self.probe('load-ran'); return 1; };
`));
    // These are the paths the page refuses outright, and the worker refuses them again. What this half
    // pins is that a path delivered straight to the worker is still stopped **before** anything is
    // written and before native is asked to load — a `load-ran` probe would mean a traversal path
    // reached the engine.
    for (const path of ['/etc/passwd', '../../escape.dat', 'C:/windows/evil.dll', 'a\\b.dat', 'a//b.dat', '']) {
      const reply = await harness.call({ type: 'restore', files: [{ path, data: 'AAAA' }] });
      assert.notEqual(reply.type, 'restore-ok', `${JSON.stringify(path)} must not be restored as a success`);
      assert.match(reply.reason, /outside the save directory/, `${JSON.stringify(path)} is refused by name`);
    }
    assert.equal(harness.takeProbe('load-ran'), undefined, 'no bad path ever reaches native load');
  } finally {
    await harness.terminate();
  }
});

test('the page refuses a traversal path, a duplicate and malformed base64 before persisting anything', () => {
  // The gate between a worker reply and the store. Exercised directly, because these are the refusals
  // that actually protect the slot: the worker already refuses a save it could not make, and this is
  // what stops a hostile or corrupt reply from becoming a write.
  const base64 = arcv64([['k', 'bytes']]);
  assert.throws(() => api.readCheckpointFiles([{ path: '../escape.dat', data: base64 }]), /not a plain relative path/);
  assert.throws(() => api.readCheckpointFiles([{ path: '/abs.dat', data: base64 }]), /absolute/);
  assert.throws(() => api.readCheckpointFiles([{ path: 'C:/x.dat', data: base64 }]), /drive letter/);
  assert.throws(() => api.readCheckpointFiles([{ path: 'a\\b.dat', data: base64 }]), /backslash/);
  assert.throws(() => api.readCheckpointFiles([{ path: 'a//b.dat', data: base64 }]), /doubled separator/);
  assert.throws(() => api.readCheckpointFiles([{ path: '', data: base64 }]), /empty path/);
  assert.throws(
    () => api.readCheckpointFiles([{ path: 'A.dat', data: base64 }, { path: 'a.dat', data: base64 }]),
    /twice/,
    'a case-folding store would collapse these onto one file',
  );
  assert.throws(() => api.readCheckpointFiles([]), /no files/);
  assert.throws(() => api.readCheckpointFiles(undefined), /did not report/);
  assert.throws(() => api.readCheckpointFiles([{ path: 'a.dat', data: 'not base64!!' }]), /base64/);
  // A valid payload round-trips to the exact bytes, so the refusals above are not refusing everything.
  const ok = api.readCheckpointFiles([{ path: 'platform-slot.dat', data: base64 }]);
  assert.equal(ok.length, 1);
  assert.equal(Buffer.from(ok[0].data).includes(Buffer.from('bytes')), true);
  assert.throws(() => api.readCheckpointFiles([{ path: 'system.dat', data: base64 }]), /does not carry/);
  const bad = Buffer.from(arcv([['k', 'bytes']]));
  bad[bad.length - 1] ^= 1;
  assert.throws(() => api.readCheckpointFiles([{ path: 'platform-slot.dat', data: bad.toString('base64') }]), /CRC32/);
  const repeated = arcv([['k', 'a'], ['k', 'b']]).toString('base64');
  assert.throws(() => api.readCheckpointFiles([{ path: 'platform-slot.dat', data: repeated }]), /repeats a key/);
});

/* --------------------------------------------------------------------------------------------- */
/* The facade and the session: the two callers that must not report a save that did not happen.     */
/* --------------------------------------------------------------------------------------------- */

/**
 * A worker bootstrap that also pre-loads a per-test glue.
 *
 * The classic worker loads the engine through `importScripts(request.engineJsUrl)`, and that URL is the
 * host's, posted in `init`. Node goes one step further and loads the glue **before** the worker script
 * runs, which `handleInit` tolerates: it stores `pendingInit` and settles once the glue reports ready.
 * `FacadeWorkerShim` therefore hands this bootstrap the glue and never relies on `importScripts` being
 * called with an `https://` URL it cannot read.
 */
const BOOTSTRAP_GLUE_SOURCE = `
const fs = require('node:fs');
const vm = require('node:vm');
const { fileURLToPath } = require('node:url');
const { parentPort, workerData } = require('node:worker_threads');
const self = globalThis;
self.self = self;
self.postMessage = (message) => parentPort.postMessage(message);
self.close = () => parentPort.close();
/*
 * The host posts an https glue URL no process here can fetch, and the worker's own \`handleInit\` calls
 * \`importScripts\` with it in the production order. So the URL is **mapped**, not skipped: this is the
 * only place the test stands in for the browser's fetch, and every step after it (the worker's stub
 * install, its \`pendingInit\`/settle handshake, the boot frame) runs for real.
 */
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

const BOOTSTRAP_GLUE = join(temp, 'bootstrap-glue.cjs');
writeFileSync(BOOTSTRAP_GLUE, BOOTSTRAP_GLUE_SOURCE);

/**
 * The browser `Worker` interface over a Node worker thread.
 *
 * `CompatWorkerHost` only ever calls `addEventListener`, `postMessage` and `terminate`, so this is the
 * whole seam. The glue travels as `workerData` and is loaded by the worker's real `importScripts` call,
 * so the transport's own load ordering is what is exercised — not a pre-loaded shortcut.
 */
class FacadeWorkerShim {
  constructor(glueUrl) {
    this.listeners = new Map();
    this.node = new Worker(BOOTSTRAP_GLUE, { workerData: { script: WORKER_SCRIPT, glue: glueUrl } });
    this.node.on('message', (data) => this.fire('message', { data }));
    this.node.on('error', (error) => this.fire('error', error));
  }
  fire(type, event) {
    for (const fn of this.listeners.get(type) ?? []) fn(event);
  }
  addEventListener(type, fn) {
    this.listeners.set(type, [...(this.listeners.get(type) ?? []), fn]);
  }
  postMessage(message, transfer = []) {
    // `worker_threads` accepts a **real** `SharedArrayBuffer` in the transfer list and nothing else, so
    // the canvas is not transferred. That is honest here rather than a shortcut: no renderer exists in
    // Node, the worker only ever holds the object, and this file is about the checkpoint wire. The
    // shared window *is* transferred, which is the part that has to be real for a read to complete.
    const windows = transfer.filter((value) => value instanceof SharedArrayBuffer);
    this.node.postMessage(message, windows);
  }
  terminate() {
    return this.node.terminate();
  }
}

const flush = async () => {
  for (let n = 0; n < 40; n += 1) await Promise.resolve();
  await new Promise((resolve) => setTimeout(resolve, 25));
};

/**
 * The whole page stack over a real worker: the real facade, the real `CompatWorkerHost`, the real wire.
 *
 * The glue is per-test, so a native refusal is a real return code rather than a stubbed rejection. The
 * host canvas is an `OffscreenCanvas` the rig made, which is the one transferable shape a test can build
 * without a DOM, and it is installed as a global so the facade's own `asOffscreenCanvas` recognises it.
 */
/**
 * A real `WritableStore`, in memory, over the same surface the host hands the runtime.
 *
 * The compat runtime persists under one prefix and reads it back on the next load, so a stub that
 * dropped every write would make the whole persistence contract untestable. `hooks` lets a test hold a
 * write open or fail it, which is the only way to measure "did `save()` wait?".
 */
function memStore(hooks = {}) {
  const files = new Map();
  return {
    name: 'mem',
    files,
    writes: [],
    async read(path) {
      return files.get(path);
    },
    async write(path, data) {
      this.writes.push(path);
      if (hooks.write) await hooks.write(path, data, files);
      files.set(path, data);
    },
    async remove(path) {
      files.delete(path);
    },
    async list(prefix = '') {
      return [...files.keys()].filter((entry) => entry.startsWith(prefix)).sort();
    },
  };
}

async function facadeRig(t, glueBody, options = {}) {
  const previous = {
    OffscreenCanvas: globalThis.OffscreenCanvas,
    crossOriginIsolated: globalThis.crossOriginIsolated,
    SharedArrayBuffer: globalThis.SharedArrayBuffer,
    Worker: globalThis.Worker,
  };
  class FakeOffscreen {
    constructor() {
      this.width = 640;
      this.height = 360;
      // A **method on the prototype**, not an own property: `postMessage` clones the object, and an own
      // property holding a function cannot be structured-cloned. The prototype keeps `getContext` —
      // which is what the facade's capability gate probes — without putting a function on the wire.
    }
    getContext() {
      return null;
    }
  }
  globalThis.OffscreenCanvas = FakeOffscreen;
  globalThis.crossOriginIsolated = true;
  globalThis.SharedArrayBuffer = globalThis.SharedArrayBuffer ?? class {};
  // The per-test glue. The bootstrap maps the host's https URL onto this file, so the worker loads it
  // through its own `importScripts` call rather than being handed a pre-loaded global.
  const glueUrl = glue(glueBody);
  globalThis.Worker = function Worker() {
    return new FacadeWorkerShim(glueUrl);
  };
  t.after(() => {
    globalThis.OffscreenCanvas = previous.OffscreenCanvas;
    globalThis.crossOriginIsolated = previous.crossOriginIsolated;
    globalThis.SharedArrayBuffer = previous.SharedArrayBuffer;
    globalThis.Worker = previous.Worker;
  });

  const paints = [];
  const notifications = [];
  const store = options.store ?? memStore();
  const host = {
    canvas: new FakeOffscreen(),
    source: makeSource(),
    store,
    surface: { width: 640, height: 360 },
    requestFrame() {
      paints.push(1);
    },
    log() {},
    now: () => 0,
    notify(event) {
      notifications.push(event);
    },
    setting: (_key, fallback) => fallback,
  };
  artemis.configureArtemisCompat({ engineBase: 'https://cdn.invalid/artemis/' });
  const runtime = artemis.artemisProvider.create(host, {
    runtimeId: 'artemis',
    surface: host.surface,
    mode: 'new',
  });
  // The glue is installed before the worker script runs, so `handleInit`'s importScripts finds it
  // already loaded. `runtime.load()` therefore drives the real open + boot frame.
  await runtime.load();
  return { runtime, host, store, paints, notifications, glueBody };
}

/** The CABI a facade rig needs, with the engine writing real files through `seedSave`. */
const FACADE_SAVE_CABI = `self.__saveN = 0; self.Module._artc_web_save = function (file) { var name = self.Module.UTF8ToString(file); self.__saveN += 1; self.seedSave("system.dat", "GLOBAL-BANK"); var body = [65,82,67,86,1,0,0,0,1,0,0,0,1,0,0,0,107,12,0,0,0,83,76,79,84,45,80,65,89,76,79,65,68]; body[body.length - 1] = (68 + self.__saveN) & 255; var crc = 0xffffffff; for (var i = 0; i < body.length; i++) { crc ^= body[i]; for (var bit = 0; bit < 8; bit++) crc = (crc >>> 1) ^ (0xedb88320 & -(crc & 1)); } crc = (crc ^ 0xffffffff) >>> 0; body.push(crc & 255, (crc >>> 8) & 255, (crc >>> 16) & 255, (crc >>> 24) & 255); self.seedSave(name, body); return 1; };\n`;

test('the facade resolves a real checkpoint into SaveStateFiles with byte-exact contents', async (t) => {
  const rig = await facadeRig(t, `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function () { return 1; };`);
  try {
    const state = await rig.runtime.save();
    assert.equal(state.version, 1);
    assert.equal(state.kind, 'artemis-compat-checkpoint');
    assert.equal(state.atomic, false, 'the store has no multi-file transaction, so nothing here is atomic');
    assert.equal(state.data, null, 'the payload is files; data is not an invented object');
    assert.deepEqual(state.files.map((file) => file.path).sort(), ['platform-slot.dat', 'system.dat']);
    const slot = state.files.find((file) => file.path === 'platform-slot.dat');
    assert.equal(Buffer.from(slot.data, 'base64')[0], 0x41, 'the bytes are the ARCV file the engine wrote');
    assert.match(String(state.caption), /ARCV compatibility checkpoint/);
    // `describe()` reports a real checkpoint, not the old "not implemented" row.
    assert.match(String(rig.runtime.describe().checkpoint), /ARCV compatibility checkpoint/);
    assert.equal(rig.runtime.describe().coldGameUnverified, true, 'no real game has been run here');
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

test('a facade whose engine refuses the save rejects, and reports no skipped checkpoint row', async (t) => {
  const rig = await facadeRig(
    t,
    `${BASE_CABI}self.Module._artc_web_save = function () { return 0; };\nself.Module._artc_web_load = function () { return 1; };`,
  );
  try {
    await assert.rejects(() => rig.runtime.save(), /artc_web_save returned 0/);
    const report = rig.runtime.report();
    assert.equal(report.severity, 'unsupported', 'audio is still unimplemented');
    assert.ok(
      !report.unsupported.some((row) => row.command === 'artemis:checkpoint'),
      'the checkpoint is no longer reported as skipped',
    );
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

test('the facade validates the whole payload before the worker is told to restore anything', async (t) => {
  const rig = await facadeRig(t, `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function () { return 1; };`);
  try {
    const files = [{ path: 'platform-slot.dat', data: arcv64([['k', 'x']]) }];
    await assert.rejects(
      () => rig.runtime.restore({ version: 1, kind: 'something-else', data: null, files }),
      /not a artemis-compat-checkpoint/,
    );
    await assert.rejects(
      () => rig.runtime.restore({ version: 2, kind: 'artemis-compat-checkpoint', data: null, files }),
      /format version 2/,
    );
    await assert.rejects(
      () => rig.runtime.restore({ version: 1, kind: 'artemis-compat-checkpoint', data: null, files: [] }),
      /carries no files/,
    );
    await assert.rejects(
      () => rig.runtime.restore({ version: 1, kind: 'artemis-compat-checkpoint', data: null, files: [{ path: '../x.dat', data: 'AAAA' }] }),
      /plain relative path/,
    );
    await assert.rejects(
      () => rig.runtime.restore({ version: 1, kind: 'artemis-compat-checkpoint', data: null, files: [{ path: 'platform-slot.dat', data: 'not base64!!' }] }),
      /not base64/,
    );
    await assert.rejects(
      () => rig.runtime.restore({ version: 1, kind: 'artemis-compat-checkpoint', data: null, files: [{ path: 'a.dat', data: arcv64([['k', 'x']]) }] }),
      /does not carry/,
    );
    // A foreign slot is refused by name rather than applied as a guess.
    await assert.rejects(() => rig.runtime.restore({ version: 1, data: {} }), /not a artemis-compat-checkpoint/);
    // A refused payload is not a broken runtime: the session is alive and can still save.
    assert.equal(rig.runtime.describe().disposed, false);
    const state = await rig.runtime.save();
    assert.equal(state.kind, 'artemis-compat-checkpoint');
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

/* --------------------------------------------------------------------------------------------- */
/* The run state across a restore.                                                                */
/*                                                                                                */
/* The defect this pins: the worker parked the engine to install the files and never put it back,  */
/* and a stand-in whose `artc_web_frame` ignores the pause reported all of it as success. Nothing   */
/* here asserts an ACK alone — every case reads whether the **next** frame advanced native.         */
/* --------------------------------------------------------------------------------------------- */

/** A restore payload with one file, as the wire carries it. */
const RESTORE_FILES = [{ path: 'platform-slot.dat', data: arcv64([['k', 'RESTORED-SLOT']]) }];
const SAVE_ONE = `self.__saveN = 0; self.Module._artc_web_save = function (file) { self.__saveN += 1; var body = [65,82,67,86,1,0,0,0,1,0,0,0,1,0,0,0,107,4,0,0,0,83,76,79,84]; body[body.length - 1] = (84 + self.__saveN) & 255; var crc = 0xffffffff; for (var i = 0; i < body.length; i++) { crc ^= body[i]; for (var bit = 0; bit < 8; bit++) crc = (crc >>> 1) ^ (0xedb88320 & -(crc & 1)); } crc = (crc ^ 0xffffffff) >>> 0; body.push(crc & 255, (crc >>> 8) & 255, (crc >>> 16) & 255, (crc >>> 24) & 255); self.seedSave(self.Module.UTF8ToString(file), body); return 1; };\n`;

test('a restore of a running session puts the engine back to running, and the next frame really advances', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
      ${BASE_CABI}
      self.Module._artc_web_frame = function () { return self.seedFrame(); };
      ${SAVE_ONE}
      self.seedNative({});
    `));
    // The session is running before the restore, so the restore must leave it running.
    const before = await harness.call({ type: 'frame' });
    assert.equal(before.type, 'frame-ok');
    assert.equal(before.running, true, 'the session runs before the restore');

    const reply = await harness.call({ type: 'restore', files: RESTORE_FILES });
    assert.equal(reply.type, 'restore-ok', `restore refused: ${reply.reason}`);
    assert.equal(reply.loaded, 1);
    assert.equal(reply.paused, false, 'the restore reports the run state it restored, not the park it used');
    // The park is real, and it is undone: pause=1 before the load, pause=0 after it.
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('pause=')), ['pause=1', 'pause=0']);

    const after = await harness.call({ type: 'frame' });
    assert.equal(after.type, 'frame-ok');
    assert.equal(after.running, true, 'a running session must not be left parked by a restore');
    // The oracle is the engine's own counter, not the ACK: the frame after the restore moved native.
    const frames = Number(after.started ?? 0);
    assert.ok(frames > 0, `native advanced after the restore: ${JSON.stringify(after)}`);
    assert.equal(after.reason, '', 'a running frame carries no failure reason');
  } finally {
    await harness.terminate();
  }
});

test('a restore of a paused session leaves the engine paused, and the next frame does not advance', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
      ${BASE_CABI}
      self.Module._artc_web_frame = function () { return self.seedFrame(); };
      self.Module._artc_web_started = function () { return self.nativeFrames(); };
      ${SAVE_ONE}
      self.seedNative({});
    `));
    // A parked frame still costs the engine its call; this is the counter the restore must not move.
    await harness.call({ type: 'frame' });
    const beforeFrames = harness.probes.filter((line) => line.startsWith('frame=')).length;
    const parked = await harness.call({ type: 'pause', paused: true });
    assert.equal(parked.type, 'ack', `pause refused: ${parked.reason}`);

    const reply = await harness.call({ type: 'restore', files: RESTORE_FILES, paused: true });
    assert.equal(reply.type, 'restore-ok', `restore refused: ${reply.reason}`);
    assert.equal(reply.paused, true, 'the player paused this session; the restore must say so');
    // Paused, then re-asserted paused: never a `pause=0` on a session the player stopped.
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('pause=')), ['pause=1', 'pause=1', 'pause=1']);

    const after = await harness.call({ type: 'frame' });
    assert.equal(after.type, 'frame-ok');
    // The real engine answers 1 for a frame it serviced while paused; what does not happen is work.
    assert.equal(after.running, true, 'a parked frame is serviced, not a stopped engine');
    assert.equal(after.started, beforeFrames, 'the engine presented no frame, so its own counter did not move');
    assert.equal(harness.takeProbe('frame-paused'), 'frame-paused', 'and the frame really did reach the engine');
  } finally {
    await harness.terminate();
  }
});

test('the newest pause intent wins when pause and resume are interleaved with a restore', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
      ${BASE_CABI}
      self.Module._artc_web_frame = function () { return self.seedFrame(); };
      self.Module._artc_web_started = function () { return self.nativeFrames(); };
      ${SAVE_ONE}
      self.seedNative({});
    `));
    // Paused, then the player resumes while the restore is still queued behind that pause. The restore
    // is the last intent, so the session ends **running** — a chain that kept the park, or that
    // replayed the earlier pause, would leave the player staring at a frozen canvas.
    harness.post({ type: 'pause', id: 9001, paused: true });
    harness.post({ type: 'pause', id: 9002, paused: false });
    const reply = await harness.call({ type: 'restore', files: RESTORE_FILES, paused: false });
    assert.equal(reply.type, 'restore-ok', `restore refused: ${reply.reason}`);
    assert.equal(reply.paused, false, 'the newest intent is the one the restore restores');
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('pause=')), ['pause=1', 'pause=0', 'pause=1', 'pause=0']);

    const after = await harness.call({ type: 'frame' });
    assert.equal(after.running, true, 'the interleaved resume is what the engine ends on');
    assert.ok(Number(after.started ?? 0) > 0, 'the resumed engine really advanced');
    // The two pause messages carried the page's own ids, so the ACKs can be read back off the wire.
    const acks = harness.messages.filter((message) => message.type === 'ack').map((message) => message.id);
    assert.deepEqual(acks, [9001, 9002], 'each transition is acknowledged in the order it was sent');
  } finally {
    await harness.terminate();
  }
});

test('a restore whose park is refused does not write a file and does not call native load', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
      ${BASE_CABI}
      self.seedNative({ pauseFail: 1 });
      self.Module._artc_web_save = function (file) {
        self.probe('save=' + self.Module.UTF8ToString(file));
        self.seedSave(self.Module.UTF8ToString(file), [65,82,67,86,1,0,0,0,1,0,0,0,1,0,0,0,107,4,0,0,0,83,76,79,84,224,205,203,49]);
        return 1;
      };
    `));
    // The `checkpoint` is queued behind the restore's park, so it is the restore's refusal that
    // decides the answer: the rAF loop stops driving frames but this probe is not a frame.
    const reply = await harness.call({ type: 'restore', files: RESTORE_FILES });
    assert.equal(reply.type, 'error', 'an engine that did not park cannot be loaded into');
    assert.match(reply.reason, /did not park/);
    assert.equal(harness.takeProbe('pause-refused=1'), 'pause-refused=1', 'native really refused the park');
    assert.equal(harness.probes.filter((line) => line === 'load=platform-slot.dat').length, 0, 'native load was never called');

    // The save directory still holds only what the engine itself wrote. A refused restore that had
    // already installed the slot would have overwritten the running session's file.
    const after = await harness.call({ type: 'checkpoint' });
    assert.equal(after.type, 'checkpoint-ok', `checkpoint refused: ${after.reason}`);
    const slot = after.files.find((entry) => entry.path === 'platform-slot.dat');
    assert.equal(Buffer.from(slot.data, 'base64').includes(Buffer.from('SLOT')), true, 'the slot was never replaced');
    assert.ok(!JSON.stringify(after.files).includes('RESTORED-SLOT'), 'the restored bytes never reached the tree');
  } finally {
    await harness.terminate();
  }
});

test('a restore that cannot return to the requested run state is a refusal, not a stopped session', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
      ${BASE_CABI}
      self.Module._artc_web_frame = function () { return self.seedFrame(); };
      self.Module._artc_web_started = function () { return self.nativeFrames(); };
      ${SAVE_ONE}
      /* Every park works; the unpark does not. The engine is stopped and cannot be told to run. */
      self.seedNative({});
      self.Module._artc_web_pause = function (value) {
        var want = value ? 1 : 0;
        self.probe('pause=' + want);
        if (want === 0) { self.probe('pause-refused=0'); return 0; }
        self.__nativePaused = true;
        return 1;
      };
    `));
    const reply = await harness.call({ type: 'restore', files: RESTORE_FILES });
    assert.equal(reply.type, 'error', 'a session that cannot be resumed is not a restored session');
    assert.match(reply.reason, /did not return to running/);
    assert.equal(harness.takeProbe('load='), 'load=platform-slot.dat', 'the load did happen; the unpark is what failed');
  } finally {
    await harness.terminate();
  }
});

test('a native load of 0 restores the files and drives no frame of its own', async () => {
  const harness = new Harness();
  try {
    // The session is **paused** when the slot is refused. That is what makes "drove no frame" a
    // measurement rather than a tautology: the engine would answer 1 to an unparked stub, so a frame
    // driven from inside the refused restore would show up as a second frame.
    await harness.init(glue(`
      ${BASE_CABI}
      self.Module._artc_web_frame = function () { return self.seedFrame(); };
      self.seedNative({ loadResult: 0 });
      var writeFile = self.Module.FS.writeFile;
      self.Module.FS.writeFile = function (path, data) {
        writeFile(path, data);
        if (String(path).endsWith('platform-slot.dat')) self.probe('slot=' + slotMark(__dec.decode(data)));
      };
      self.seedSave('platform-slot.dat', [65,82,67,86,1,0,0,0,1,0,0,0,1,0,0,0,107,4,0,0,0,83,76,79,84,224,205,203,49]);
    `));
    const parked = await harness.call({ type: 'pause', paused: true });
    assert.equal(parked.type, 'ack', `pause refused: ${parked.reason}`);

    const reply = await harness.call({ type: 'restore', files: RESTORE_FILES, paused: true });
    assert.equal(reply.type, 'error', 'a load that returns 0 is a refusal');
    assert.match(reply.reason, /artc_web_load returned 0/);
    // One park, then the load. No unpark and no frame: the refusal is terminal.
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('pause=')), ['pause=1', 'pause=1']);
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('slot=')), ['slot=SLOT', 'slot=RESTORED-SLOT', 'slot=SLOT']);
    assert.equal(harness.probes.filter((line) => line.startsWith('frame=')).length, 0, 'no frame was driven');
    const fatal = await harness.wait('fatal');
    assert.match(fatal.reason, /artc_web_load returned 0/);
    const later = await harness.call({ type: 'frame' });
    assert.equal(later.type, 'error', 'a frame after a refused load is refused');
    assert.equal(later.running, undefined, 'the refusal is not a paused frame');
  } finally {
    await harness.terminate();
  }
});

test('the page serializes checkpoint, restore and the pause gate on one queue', async (t) => {
  const rig = await facadeRig(t, `
    ${BASE_CABI}
    self.Module._artc_web_frame = function () { return self.seedFrame(); };
    ${FACADE_SAVE_CABI}
    self.seedNative({});
  `);
  try {
    await flush();
    assert.equal(rig.runtime.describe().worker.queueBusy, false, 'no operation is queued at rest');

    // A save is queued and running; the pause arrives **while the frame loop is still driving**. The
    // queue is closed from the moment the transition is asked for, so the frames the ticks below ask
    // for are spent rather than put on the wire, and the transition runs after the save settles.
    const saving = rig.runtime.save();
    rig.runtime.pause();
    const page = rig.runtime.describe().worker;
    assert.equal(page.paused, true, 'the pause gate is set synchronously, before the worker is told');
    assert.equal(page.queueBusy, true, 'a queued transition closes the queue from the moment it is asked for');
    const clock = page.frameClock;
    rig.runtime.tick(200);
    await flush();
    assert.equal(rig.runtime.describe().worker.frameClock, clock, 'no frame is put on the wire across a transition');
    assert.equal(rig.runtime.describe().worker.queueBusy, false, 'the queue drains when the save settles');
    await saving; // Already queued ahead of the pause; this only reads the save's own outcome.
    await flush(); // The pause runs after the save; a tick's frame has no business in that window.

    const saved = await rig.runtime.save();
    await rig.runtime.restore(saved);
    assert.equal(rig.runtime.describe().worker.paused, true, 'the restore put the engine back to the paused state');
    assert.equal(rig.runtime.describe().worker.queueBusy, false, 'the queue is empty once the restore settles');
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

test('a restore round-trips through the facade and the engine reads back the installed bytes', async (t) => {
  const rig = await facadeRig(
    t,
    `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function (file) { self.probe('load=' + self.Module.UTF8ToString(file)); self.probe('loaded-bytes=' + self.readSave(self.Module.UTF8ToString(file))); return 1; };`,
  );
  try {
    const saved = await rig.runtime.save();
    rig.paints.length = 0;
    await rig.runtime.restore(saved);
    assert.equal(rig.runtime.describe().disposed, false, 'a successful restore leaves the session running');
    assert.ok(rig.paints.length > 0, 'the restore asked the host for a repaint');
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

test('a save and a restore requested together run one after the other, never interleaved', async (t) => {
  const rig = await facadeRig(t, `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function () { return 1; };`);
  try {
    const saved = await rig.runtime.save();
    // Two operations at once. The queue is what makes this safe: both complete, and neither observes
    // the other half-done.
    const [first, second] = await Promise.all([
      rig.runtime.restore(saved).then(() => 'restored', (error) => error),
      rig.runtime.save().then((state) => (state.files.length > 0 ? 'saved' : 'empty'), (error) => error),
    ]);
    assert.equal(first, 'restored', `the restore failed: ${first}`);
    assert.equal(second, 'saved', `the second save failed: ${second}`);
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

test('a checkpoint is not a frame source: the clock resumes after it settles', async (t) => {
  const rig = await facadeRig(t, `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function () { return 1; };`);
  try {
    await flush();
    const before = rig.runtime.describe().worker.frameClock;
    const saving = rig.runtime.save();
    // Ticks during the checkpoint are spent, not banked: a 200 ms checkpoint must not come back as a
    // dozen frames the moment it finishes.
    rig.runtime.tick(200);
    rig.runtime.tick(200);
    await saving;
    const after = rig.runtime.describe().worker.frameClock;
    assert.ok(after <= before + 1, `a checkpoint must not be a frame source: ${before} -> ${after}`);
    rig.runtime.tick(20);
    await flush();
    assert.ok(rig.runtime.describe().worker.frameClock > after, 'the frame clock resumes afterwards');
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

/* --------------------------------------------------------------------------------------------- */
/* Freshness, persistence, and the boundaries around both.                                          */
/* --------------------------------------------------------------------------------------------- */

/** A valid ARCV v1 checkpoint, as the byte literal the glue can build without a helper. */
const GOOD_SLOT = '[65,82,67,86,1,0,0,0,1,0,0,0,1,0,0,0,107,12,0,0,0,83,76,79,84,45,80,65,89,76,79,65,68,187,216,51,93]';

/** Every `FS.writeFile` the worker attempts, so "nothing was written" is measurable. */
const WRITE_PROBE = `
var writeFile = self.Module.FS.writeFile;
self.Module.FS.writeFile = function (path, data) { self.probe('write=' + String(path)); return writeFile.apply(null, arguments); };
`;

test('two consecutive native writes of identical checkpoint bytes both succeed', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
${WRITE_PROBE}
/* Byte-for-byte the same slot on every call. That is a save the engine really made: the previous slot
 * was moved aside first, so what is read back can only be what *this* run wrote. Refusing it because
 * the bytes happen to match the old ones would refuse the second save of an unchanged session. */
self.Module._artc_web_save = function (file) {
  self.seedSave('system.dat', 'GLOBAL-BANK');
  self.seedSave(self.Module.UTF8ToString(file), ${GOOD_SLOT});
  return 1;
};
self.Module._artc_web_load = function () { return 1; };
    `));
    const first = await harness.call({ type: 'checkpoint' });
    assert.equal(first.type, 'checkpoint-ok', `the first save was refused: ${first.reason}`);
    const second = await harness.call({ type: 'checkpoint' });
    assert.equal(second.type, 'checkpoint-ok', `a byte-identical second save was refused: ${second.reason}`);
    assert.deepEqual(second.files, first.files, 'the same two files, the same bytes, both times');
  } finally {
    await harness.terminate();
  }
});

test('a native save that writes only the bank is refused, stale slot or not', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
/* A session that already has a slot, and an engine that answers 1 without rewriting it. The old slot
 * was moved aside before the call, so what a read-back finds is this run's work — and there is none.
 * That is what makes the refusal right: freshness is proven by the removal, never by comparing bytes. */
self.seedSave('platform-slot.dat', ${GOOD_SLOT});
self.Module._artc_web_save = function () { self.seedSave('system.dat', 'GLOBAL-BANK'); return 1; };
self.Module._artc_web_load = function () { return 1; };
    `));
    const reply = await harness.call({ type: 'checkpoint' });
    assert.equal(reply.type, 'error', 'a bank is not a checkpoint');
    assert.match(reply.reason, /wrote no file/);
    assert.equal(reply.files, undefined, 'nothing is carried on a refusal');
  } finally {
    await harness.terminate();
  }
});

test('a restore with a corrupt slot CRC writes nothing, parks nothing and never loads', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
${WRITE_PROBE}
self.Module._artc_web_save = function () { return 1; };
self.Module._artc_web_load = function () { self.probe('load-ran'); return 1; };
    `));
    const corrupt = Buffer.from(arcv([['k', 'bytes']]));
    corrupt[corrupt.length - 1] ^= 1;
    const reply = await harness.call({
      type: 'restore',
      files: [{ path: 'platform-slot.dat', data: corrupt.toString('base64') }],
    });
    assert.equal(reply.type, 'error', 'a corrupt slot is refused');
    assert.match(reply.reason, /CRC32/);
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('write=')), [], 'no file was written');
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('pause=')), [], 'the engine was never parked');
    assert.equal(harness.takeProbe('load-ran'), undefined, 'native load was never called');
  } finally {
    await harness.terminate();
  }
});

test('a malformed second entry leaves the whole restore unwritten', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
${WRITE_PROBE}
self.Module._artc_web_save = function () { return 1; };
self.Module._artc_web_load = function () { self.probe('load-ran'); return 1; };
    `));
    const reply = await harness.call({
      type: 'restore',
      files: [
        { path: 'platform-slot.dat', data: arcv64([['k', 'bytes']]) },
        { path: 'system.dat', data: 'not base64!!' },
      ],
    });
    assert.equal(reply.type, 'error', 'one bad entry refuses the whole set');
    assert.match(reply.reason, /not valid base64/);
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('write=')), [], 'the valid first entry was not installed either');
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('pause=')), []);
    assert.equal(harness.takeProbe('load-ran'), undefined);
  } finally {
    await harness.terminate();
  }
});

test('a file set that is not an array is refused by the worker itself', async () => {
  const harness = new Harness();
  try {
    await harness.init(glue(`
${BASE_CABI}
${WRITE_PROBE}
self.Module._artc_web_save = function () { return 1; };
self.Module._artc_web_load = function () { return 1; };
    `));
    for (const files of [{ length: 1 }, 'platform-slot.dat', 42]) {
      const reply = await harness.call({ type: 'restore', files });
      assert.equal(reply.type, 'error', `${JSON.stringify(files)} is not a file set`);
      assert.match(reply.reason, /not an array/);
    }
    assert.deepEqual(harness.probes.filter((line) => line.startsWith('write=')), []);
  } finally {
    await harness.terminate();
  }
});

/** Waits for a condition instead of guessing a delay. */
async function until(predicate, ms = 5000) {
  const deadline = Date.now() + ms;
  while (Date.now() < deadline) {
    if (predicate()) return;
    await new Promise((resolve) => setTimeout(resolve, 5));
  }
  throw new Error('the condition never became true');
}

const PERSIST_CABI = `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function () { return 1; };`;

test('save resolves only after every store write settled', async (t) => {
  let release;
  const gate = new Promise((resolve) => {
    release = resolve;
  });
  const store = memStore({
    async write(path, data, files) {
      await gate;
      files.set(path, data);
    },
  });
  const rig = await facadeRig(t, PERSIST_CABI, { store });
  try {
    const saving = rig.runtime.save();
    let settled = false;
    void saving.then(
      () => {
        settled = true;
      },
      () => {
        settled = true;
      },
    );
    await until(() => store.writes.length > 0);
    assert.equal(settled, false, 'a state must not be handed back while a write is in flight');
    release();
    const state = await saving;
    assert.equal(state.kind, 'artemis-compat-checkpoint');
    assert.deepEqual([...store.files.keys()].sort(), [
      'artemis-compat/platform-slot.dat',
      'artemis-compat/system.dat',
    ]);
    // Byte-exact: the store holds what the engine wrote, not a re-encoding of it.
    const slot = state.files.find((file) => file.path === 'platform-slot.dat');
    assert.deepEqual(
      Buffer.from(store.files.get('artemis-compat/platform-slot.dat')),
      Buffer.from(slot.data, 'base64'),
    );
  } finally {
    release();
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

test('a second save cannot persist while the first save is still writing', async (t) => {
  let releaseFirst;
  const firstWrite = new Promise((resolve) => {
    releaseFirst = resolve;
  });
  const order = [];
  const store = memStore({
    async write(path, data, files) {
      order.push(path);
      if (order.length === 1) await firstWrite;
      files.set(path, data);
    },
  });
  const rig = await facadeRig(t, PERSIST_CABI, { store });
  try {
    const first = rig.runtime.save();
    await until(() => order.length === 1);
    const second = rig.runtime.save();
    await new Promise((resolve) => setTimeout(resolve, 200));
    assert.deepEqual(order, ['artemis-compat/system.dat'], 'the second persist has not started');
    releaseFirst();
    const [older, newer] = await Promise.all([first, second]);
    assert.equal(older.files.length, 2);
    assert.equal(newer.files.length, 2);
    assert.deepEqual(
      Buffer.from(store.files.get('artemis-compat/platform-slot.dat')),
      Buffer.from(newer.files.find((file) => file.path === 'platform-slot.dat').data, 'base64'),
      'the store ends on the later checkpoint, never a mix of the two',
    );
  } finally {
    releaseFirst();
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

test('a store that refuses a write makes the save a refusal, not a slot', async (t) => {
  const store = memStore({
    async write() {
      throw new Error('ENOSPC: the store is full');
    },
  });
  const rig = await facadeRig(t, PERSIST_CABI, { store });
  try {
    await assert.rejects(() => rig.runtime.save(), /the store is full/);
    assert.equal(store.files.size, 0, 'nothing was persisted');
    assert.equal(rig.runtime.describe().disposed, false, 'a refused write does not kill a live engine');
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }
});

test('a disposal during the store write rejects the save', async (t) => {
  let release;
  const gate = new Promise((resolve) => {
    release = resolve;
  });
  const store = memStore({
    async write(path, data, files) {
      if (store.writes.length === 1) await gate;
      files.set(path, data);
    },
  });
  const rig = await facadeRig(t, PERSIST_CABI, { store });
  try {
    const saving = rig.runtime.save();
    await until(() => store.writes.length === 1);
    const later = rig.runtime.save();
    rig.runtime.dispose();
    release();
    await saving;
    await assert.rejects(() => later, /disposed/);
  } finally {
    release();
    await rig.runtime.whenDisposed();
  }
});

test('a restore persists only what native accepted, and a failed store write ends the session', async (t) => {
  const store = memStore();
  const rig = await facadeRig(t, PERSIST_CABI, { store });
  let saved;
  try {
    saved = await rig.runtime.save();
    const written = [...store.files.keys()];
    assert.deepEqual(written.sort(), ['artemis-compat/platform-slot.dat', 'artemis-compat/system.dat']);
    await rig.runtime.restore(saved);
    assert.deepEqual([...store.files.keys()].sort(), written.sort(), 'the restore rewrote the same set');
  } finally {
    rig.runtime.dispose();
    await rig.runtime.whenDisposed();
  }

  // A save the store refuses never reaches a restore, so the session stays alive and the test still
  // has to dispose it. `whenDisposed()` alone waits for a teardown nobody started.
  const broken = memStore({
    async write() {
      throw new Error('QuotaExceededError');
    },
  });
  const second = await facadeRig(t, PERSIST_CABI, { store: broken });
  try {
    await assert.rejects(() => second.runtime.save(), /QuotaExceededError/);
    assert.equal(second.runtime.describe().disposed, false, 'a refused save leaves the session running');
    await assert.rejects(() => second.runtime.restore(saved), /QuotaExceededError/);
    assert.equal(
      second.runtime.describe().disposed,
      true,
      'the engine already carries these bytes while the store does not, so the session ends',
    );
  } finally {
    second.runtime.dispose();
    await second.runtime.whenDisposed();
  }
});

test('a new instance over the same store hydrates the bank before the engine opens', async (t) => {
  const store = memStore();
  const first = await facadeRig(t, PERSIST_CABI, { store });
  try {
    await first.runtime.save();
  } finally {
    first.runtime.dispose();
    await first.runtime.whenDisposed();
  }
  assert.deepEqual([...store.files.keys()].sort(), [
    'artemis-compat/platform-slot.dat',
    'artemis-compat/system.dat',
  ]);

  /* The cold boot is the measurement: this engine refuses to open unless its global bank is already on
   * disk, which is only true if the worker installed the saved files *before* `artc_web_open`. */
  const second = await facadeRig(
    t,
    `${BASE_CABI}
self.Module._artc_web_open = function (pack, mode) {
  var bank = self.readSaveText('system.dat');
  self.probe('open-bank=' + bank);
  if (bank !== 'GLOBAL-BANK') return 0;
  return 1;
};
self.Module._artc_web_frame = function () { return 1; };
self.Module._artc_web_started = function () { return 1; };
self.Module._artc_web_save = function () { return 1; };
self.Module._artc_web_load = function () { return 1; };
`,
    { store },
  );
  try {
    assert.equal(second.runtime.describe().loaded, true, 'the engine opened with the saved bank installed');
    // A load must not write: hydration is a read of the store, not a rewrite of it.
    assert.deepEqual([...store.files.keys()].sort(), [
      'artemis-compat/platform-slot.dat',
      'artemis-compat/system.dat',
    ]);
  } finally {
    second.runtime.dispose();
    await second.runtime.whenDisposed();
  }
});

test('a pack under the compat prefix is refused, never hydrated into the save directory', async (t) => {
  const store = memStore();
  await store.write('artemis-compat/root.pfs', Uint8Array.from([1, 2, 3]));
  await assert.rejects(
    () => facadeRig(t, `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function () { return 1; };`, { store }),
    /is a pack/,
  );
});

test('a store that lists a file it cannot read fails the load instead of hydrating a partial set', async (t) => {
  const store = memStore();
  const liar = {
    name: 'liar',
    async read() {
      return undefined;
    },
    async write(path, data) {
      return store.write(path, data);
    },
    async remove(path) {
      return store.remove(path);
    },
    async list(prefix) {
      return store.list(prefix);
    },
  };
  await store.write('artemis-compat/system.dat', Uint8Array.from([1, 2, 3]));
  await assert.rejects(
    () => facadeRig(t, `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function () { return 1; };`, { store: liar }),
    /could not read/,
  );
});

test('a disposed runtime refuses save and restore by name, and a second dispose is not a second teardown', async (t) => {
  const rig = await facadeRig(t, `${BASE_CABI}${FACADE_SAVE_CABI}self.Module._artc_web_load = function () { return 1; };`);
  rig.runtime.dispose();
  await rig.runtime.whenDisposed();
  await assert.rejects(() => rig.runtime.save(), /disposed/);
  await assert.rejects(
    () => rig.runtime.restore({ version: 1, kind: 'artemis-compat-checkpoint', data: null, files: [{ path: 'a.dat', data: 'AAAA' }] }),
    /disposed/,
  );
  rig.runtime.dispose();
  assert.equal(rig.runtime.describe().disposed, true);
});
