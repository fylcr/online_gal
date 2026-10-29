// Native module-loading regression, not real-engine/game acceptance. Chrome refuses synchronous
// compilation of the real >8 MB bridge on its main thread; both loading branches must use compile().
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, symlinkSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';

const root = resolve(fileURLToPath(new URL('..', import.meta.url)));
const temp = mkdtempSync(join(tmpdir(), 'artemis-module-loading-'));
process.on('exit', () => {
  if (realpathSync(dirname(temp)) === realpathSync(tmpdir()) && basename(temp).startsWith('artemis-module-loading-')) rmSync(temp, { recursive: true, force: true });
});
const out = join(temp, 'out');
execFileSync(process.execPath, [join(root, 'node_modules/typescript/bin/tsc'), 'packages/engine/core/artemis-compat/src/session-core.ts', '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler', '--lib', 'ES2022,DOM', '--strict', '--skipLibCheck', '--rootDir', 'packages/engine/core/artemis-compat/src', '--outDir', out], { cwd: root, stdio: 'inherit' });
writeFileSync(join(out, 'package.json'), '{"type":"module"}');
symlinkSync(join(root, 'packages/engine/core/artemis-compat/node_modules'), join(out, 'node_modules'), 'junction');
const { createArtemisSessionCore, ARTEMIS_CORE_EXPORTS: core, ARTEMIS_ARENA_PAGES: arena } = await import(pathToFileURL(join(out, 'session-core.js')));

const text = new TextEncoder();
const uleb = n => { const bytes = []; do { const byte = n & 127; n >>>= 7; bytes.push(byte | (n ? 128 : 0)); } while (n); return bytes; };
const sleb = n => { const bytes = []; for (;;) { const byte = n & 127; n >>= 7; const more = !((n === 0 && !(byte & 64)) || (n === -1 && (byte & 64))); bytes.push(byte | (more ? 128 : 0)); if (!more) return bytes; } };
const name = s => [...uleb(text.encode(s).length), ...text.encode(s)];
const vec = rows => [...uleb(rows.length), ...rows.flat()];
const section = (n, bytes) => [n, ...uleb(bytes.length), ...bytes];
const i32 = n => [0x41, ...sleb(n)];
const localGet = n => [0x20, ...uleb(n)];
const globalGet = n => [0x23, ...uleb(n)];
const globalSet = n => [0x24, ...uleb(n)];
const increment = n => [...globalGet(n), ...i32(1), 0x6a, ...globalSet(n)];

// Match the native fixture ABI in artemis-session-core.test.mjs; this fixture is inert until boot.
function bridgeBytes() {
  const types = [], byShape = new Map(), functions = [], exports = [], table = [];
  const type = (arity, result = false) => {
    const key = `${arity}/${result}`;
    if (byShape.has(key)) return byShape.get(key);
    const id = types.length;
    types.push([0x60, ...vec(Array.from({ length: arity }, () => [0x7f])), ...vec(result ? [[0x7f]] : [])]);
    byShape.set(key, id);
    return id;
  };
  const imports = [
    [...name('a'), ...name('fa'), 0, ...uleb(type(4))],
    [...name('a'), ...name('_b'), 0, ...uleb(type(3))],
    [...name('a'), ...name('a'), 2, 1, ...uleb(arena.initial), ...uleb(arena.maximum)],
  ];
  const fn = (exportName, arity, result, body) => {
    const index = functions.length + 2;
    functions.push({ type: type(arity, result), body: [0, ...body, 0x0b] });
    exports.push([...name(exportName), 0, ...uleb(index)]);
    return index;
  };
  fn(core.malloc, 1, true, i32(8192));
  fn(core.free, 1, false, increment(1));
  fn(core.errnoLocation, 0, true, i32(3072));
  fn(core.startUnwind, 1, false, [...i32(1), ...globalSet(0)]);
  fn(core.stopUnwind, 0, false, [...i32(0), ...globalSet(0)]);
  fn(core.startRewind, 1, false, [...i32(2), ...globalSet(0)]);
  fn(core.stopRewind, 0, false, [...i32(0), ...globalSet(0)]);
  fn(core.ctors, 0, false, increment(1));
  fn(core.main, 2, true, i32(73));
  fn(core.stackSave, 0, true, globalGet(2));
  fn(core.stackRestore, 1, false, [...localGet(0), ...globalSet(2)]);
  fn(core.stackAlloc, 1, true, globalGet(2));
  for (const [shortName, arity] of [[core.dynCallVi, 2], [core.dynCallVii, 3], [core.dynCallViii, 4], [core.dynCallViiii, 5], [core.dynCallV, 1]]) fn(shortName, arity, false, []);
  fn(core.dynCallIiii, 4, true, i32(1));
  fn(core.setThrew, 2, false, []);
  table.push(fn('callback', 1, false, []));
  exports.push([...name(core.table), 1, 0], [...name('arena'), 2, 0]);
  const globals = [0, 0, 60000].map(value => [0x7f, 1, ...i32(value), 0x0b]);
  return new Uint8Array([0, 97, 115, 109, 1, 0, 0, 0,
    ...section(1, vec(types)), ...section(2, vec(imports)),
    ...section(3, vec(functions.map(f => uleb(f.type)))),
    ...section(4, vec([[0x70, 1, ...uleb(table.length + 1), ...uleb(table.length + 1)]])),
    ...section(6, vec(globals)), ...section(7, vec(exports)),
    ...section(9, vec([[0, ...i32(1), 0x0b, ...vec(table.map(uleb))]])),
    ...section(10, vec(functions.map(f => [...uleb(f.body.length), ...f.body]))),
  ]);
}
const fixture = bridgeBytes();
assert.equal(WebAssembly.validate(fixture), true, 'fixture is actual valid native wasm');
const compiledFixture = await WebAssembly.compile(fixture);

function padded() {
  const buffer = new Uint8Array(fixture.length + 128).fill(0x5a);
  buffer.set(fixture, 64);
  return new Uint8Array(buffer.buffer, 64, fixture.length);
}
function oversized() {
  // Legal custom-section padding, not a truncated/invalid trailing blob or a separate worker probe.
  const paddingSize = 9 * 1024 * 1024;
  const label = name('module-loading-padding');
  const header = [0, ...uleb(label.length + paddingSize), ...label];
  const bytes = new Uint8Array(fixture.length + header.length + paddingSize);
  bytes.set(fixture);
  bytes.set(header, fixture.length);
  assert.ok(bytes.length > 8 * 1024 * 1024);
  assert.equal(WebAssembly.validate(bytes), true);
  return bytes;
}
function environment() {
  const ini = text.encode('[FILE]\nSAVEPATH = module-loading\n[WASM]\nWIDTH = 1280\nHEIGHT = 720\n');
  const reads = [], notifications = [];
  const view = { requestAnimationFrame: () => 1, cancelAnimationFrame() {} };
  const canvas = { width: 1280, height: 720, ownerDocument: { baseURI: 'https://fixture.invalid/game/', defaultView: view, addEventListener() {}, removeEventListener() {}, querySelector: () => canvas }, addEventListener() {}, removeEventListener() {}, getContext: () => null };
  const source = {
    name: 'loading-fixture', capabilities: { randomAccess: true, writable: false },
    async list() { throw Error('fixture must not be enumerated'); },
    async readFile(path) { reads.push(path); if (path === 'system.ini') return ini.slice(); throw Error(`unexpected read ${path}`); },
    async exists(path) { return path === 'system.ini'; }, async isDirectory() { return false; }, async size() { return ini.length; },
  };
  const host = { source, canvas, surface: { width: 1280, height: 720 }, store: { name: 'fixture', async read() {}, async write() {}, async remove() {}, async list() { return []; } }, requestFrame() {}, now: () => 0, log() {}, notify: event => notifications.push(event), setting: () => undefined };
  return { host, canvas, reads, notifications };
}
const realFetch = globalThis.fetch;
const realCompile = WebAssembly.compile;
const realModule = WebAssembly.Module;
const refusal = 'WebAssembly.Compile is disallowed on the main thread, if the buffer size is larger than 8MB.';

// Preserve instanceof/static methods. Only sync construction is refused; real compile still runs.
async function withBrowser(run, { compile = bytes => realCompile(bytes), fetch = () => { throw Error('unexpected fetch'); } } = {}) {
  const moduleDescriptor = Object.getOwnPropertyDescriptor(WebAssembly, 'Module');
  const oldCompile = WebAssembly.compile, oldFetch = globalThis.fetch;
  const stats = { sync: 0, compiled: [], fetched: [] };
  WebAssembly.Module = new Proxy(realModule, { construct() { stats.sync++; throw new RangeError(refusal); } });
  WebAssembly.compile = bytes => { stats.compiled.push(bytes); return compile(bytes); };
  globalThis.fetch = async (url, init) => { stats.fetched.push(String(url)); return fetch(url, init); };
  try { return await run(stats); }
  finally {
    Object.defineProperty(WebAssembly, 'Module', moduleDescriptor);
    WebAssembly.compile = oldCompile;
    globalThis.fetch = oldFetch;
  }
}
async function loadAndCheck(wasm, stats) {
  const env = environment();
  const session = await createArtemisSessionCore(env.host, { wasm, canvas: env.canvas });
  try {
    assert.equal(session.instance instanceof WebAssembly.Instance, true);
    assert.equal(session.diagnostics().wired, true);
    assert.equal(session.instance.exports.Lc instanceof WebAssembly.Table, true);
    assert.equal(session.memory.memory.buffer.byteLength, arena.initial * 65536);
    assert.deepEqual(env.reads, ['system.ini']);
    assert.equal(stats.sync, 0);
  } finally { await session.dispose(); }
}

test('Uint8Array loading asynchronously compiles only its nonzero-offset window on a private buffer', async () => {
  const bytes = padded();
  await withBrowser(async stats => {
    await loadAndCheck(bytes, stats);
    assert.equal(stats.compiled.length, 1);
    const handed = stats.compiled[0];
    assert.notEqual(handed.buffer, bytes.buffer);
    assert.equal(handed.byteOffset, 0);
    assert.deepEqual(handed, fixture);
    assert.equal(bytes.buffer.byteLength, fixture.length + 128);
  });
});
test('caller mutation while asynchronous compilation is pending cannot change the copied module', async () => {
  let release;
  const ready = new Promise(resolve => { release = resolve; });
  const bytes = padded();
  await withBrowser(async stats => {
    const pending = loadAndCheck(bytes, stats);
    assert.equal(stats.compiled.length, 1, 'private copy is captured before yielding');
    bytes.fill(0);
    release();
    await pending;
    assert.deepEqual(stats.compiled[0], fixture);
  }, { compile: async bytes => { await ready; return realCompile(bytes); } });
});
test('SharedArrayBuffer-backed view compiles through a non-shared ownership copy', async () => {
  const bytes = new Uint8Array(new SharedArrayBuffer(fixture.length));
  bytes.set(fixture);
  await withBrowser(async stats => {
    await loadAndCheck(bytes, stats);
    assert.equal(stats.compiled[0].buffer instanceof ArrayBuffer, true);
    assert.notEqual(stats.compiled[0].buffer, bytes.buffer);
  });
});
test('relative string, absolute string and URL object preserve resolution and compile the response ArrayBuffer', async () => {
  for (const [source, expected] of [['bridge.wasm', 'https://fixture.invalid/game/bridge.wasm'], ['https://cdn.invalid/bridge.wasm', 'https://cdn.invalid/bridge.wasm'], [new URL('https://cdn.invalid/object.wasm'), 'https://cdn.invalid/object.wasm']]) {
    const buffer = fixture.slice().buffer;
    await withBrowser(async stats => {
      await loadAndCheck(source, stats);
      assert.deepEqual(stats.fetched, [expected]);
      assert.deepEqual(stats.compiled, [buffer]);
      assert.equal(stats.compiled[0], buffer);
    }, { fetch: () => ({ ok: true, async arrayBuffer() { return buffer; } }) });
  }
});
test('an already compiled module remains the same instance without fetch or recompilation', async () => {
  await withBrowser(async stats => {
    const env = environment();
    const session = await createArtemisSessionCore(env.host, { wasm: compiledFixture, canvas: env.canvas });
    try {
      assert.equal(session.options.wasm, compiledFixture);
      assert.equal(session.diagnostics().wired, true);
      assert.deepEqual(stats.compiled, []);
      assert.deepEqual(stats.fetched, []);
      assert.equal(stats.sync, 0);
    } finally { await session.dispose(); }
  });
});
test('HTTP failure keeps URL/status, does not read the body, compile or open the game', async () => {
  const env = environment();
  await withBrowser(async stats => {
    await assert.rejects(createArtemisSessionCore(env.host, { wasm: 'bridge.wasm', canvas: env.canvas }), /could not fetch the bridge at https:\/\/fixture\.invalid\/game\/bridge\.wasm: HTTP 503/);
    assert.deepEqual(stats.compiled, []);
    assert.deepEqual(env.reads, []);
  }, { fetch: () => ({ ok: false, status: 503, arrayBuffer() { assert.fail('HTTP failure body must not be read'); } }) });
});
test('fetch and body-read rejections keep their original identities and open no game', async () => {
  for (const atBody of [false, true]) {
    const failure = Error(atBody ? 'body failed' : 'network failed');
    const env = environment();
    await withBrowser(async stats => {
      await assert.rejects(createArtemisSessionCore(env.host, { wasm: 'bridge.wasm', canvas: env.canvas }), error => error === failure);
      assert.deepEqual(stats.compiled, []);
      assert.deepEqual(env.reads, []);
    }, { fetch: async () => { if (!atBody) throw failure; return { ok: true, async arrayBuffer() { throw failure; } }; } });
  }
});
test('native compile failure is forwarded unchanged on byte and fetched-buffer branches', async () => {
  const invalid = Uint8Array.of(0, 97, 115, 109, 1, 0, 0, 0, 1, 5, 1, 0x60, 0xff);
  assert.equal(WebAssembly.validate(invalid), false);
  for (const wasm of [invalid, 'broken.wasm']) {
    let compilerError;
    const env = environment();
    await withBrowser(async () => {
      await assert.rejects(createArtemisSessionCore(env.host, { wasm, canvas: env.canvas }), error => error === compilerError && error instanceof WebAssembly.CompileError);
      assert.deepEqual(env.reads, []);
      assert.deepEqual(env.notifications, []);
    }, { compile: async bytes => { try { return await realCompile(bytes); } catch (error) { compilerError = error; throw error; } }, fetch: () => ({ ok: true, async arrayBuffer() { return invalid.slice().buffer; } }) });
  }
});
test('non-Error and undefined compile rejections remain actual rejections', async () => {
  for (const failure of [{ code: 'fixture-rejection' }, undefined]) {
    const env = environment();
    await withBrowser(async () => {
      const result = await createArtemisSessionCore(env.host, { wasm: fixture, canvas: env.canvas }).then(() => ({ rejected: false }), error => ({ rejected: true, error }));
      assert.equal(result.rejected, true);
      assert.equal(result.error, failure);
      assert.deepEqual(env.reads, []);
    }, { compile: async () => { throw failure; } });
  }
});
test('actual >8MB bridge fixture loads through core from bytes despite refused synchronous construction', async () => {
  const bytes = oversized();
  await withBrowser(async stats => {
    assert.throws(() => new WebAssembly.Module(bytes), RangeError);
    stats.sync = 0;
    await loadAndCheck(bytes, stats);
    assert.equal(stats.compiled.length, 1);
    assert.equal(stats.compiled[0].byteLength, bytes.byteLength);
  });
});
test('actual >8MB fetched fixture loads through the same core asynchronously', async () => {
  const buffer = oversized().buffer;
  await withBrowser(async stats => {
    await loadAndCheck('large.wasm', stats);
    assert.deepEqual(stats.fetched, ['https://fixture.invalid/game/large.wasm']);
    assert.equal(stats.compiled.length, 1);
    assert.equal(stats.compiled[0], buffer);
  }, { fetch: () => ({ ok: true, async arrayBuffer() { return buffer; } }) });
});
test('browser mocks restore the original globals even when the test body throws', async () => {
  const failure = Error('intentional test-body failure');
  await assert.rejects(withBrowser(async () => { throw failure; }), error => error === failure);
  assert.equal(globalThis.fetch, realFetch);
  assert.equal(WebAssembly.compile, realCompile);
  assert.equal(WebAssembly.Module, realModule);
  assert.equal(new WebAssembly.Module(fixture) instanceof realModule, true);
});
