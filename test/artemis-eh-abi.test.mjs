// Native WASM exception-ABI regression. This is not real-game acceptance and loads no SDK.
import assert from 'node:assert/strict';
import test from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, realpathSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join } from 'node:path';
import { pathToFileURL } from 'node:url';

const temporary = mkdtempSync(join(tmpdir(), 'artemis-eh-abi-'));
process.on('exit', () => {
  const target = realpathSync(temporary);
  if (realpathSync(dirname(target)) !== realpathSync(tmpdir()) || !basename(target).startsWith('artemis-eh-abi-')) return;
  rmSync(target, { recursive: true, force: true });
});
const output = join(temporary, 'out');
execFileSync(process.execPath, ['node_modules/typescript/bin/tsc', 'packages/engine/core/artemis-compat/src/host/eh.ts',
  '--target', 'ES2022', '--module', 'ESNext', '--moduleResolution', 'bundler', '--lib', 'ES2022,DOM',
  '--strict', '--skipLibCheck', '--rootDir', 'packages/engine/core/artemis-compat/src', '--outDir', output],
  { cwd: process.cwd(), stdio: 'inherit' });
writeFileSync(join(output, 'package.json'), '{"type":"module"}');
const { createArtemisEhImports, ArtemisCppThrow, ArtemisLongjmp, ARTEMIS_EH_IMPORTS } =
  await import(pathToFileURL(join(output, 'host/eh.js')));
const { ArtemisMemory } = await import(pathToFileURL(join(output, 'host/memory.js')));
const { ArtemisTempRet0 } = await import(pathToFileURL(join(output, 'host/runtime.js')));
const TYPE = 28672, BASE_TYPE = 28688, ADJUSTED = 49152;
const uleb = n => { const out = []; do { let b = n & 127; n >>>= 7; if (n) b |= 128; out.push(b); } while (n); return out; };
const sleb = n => { const out = []; let more; do { let b = n & 127; n >>= 7; more = !((n === 0 && !(b & 64)) || (n === -1 && (b & 64))); if (more) b |= 128; out.push(b); } while (more); return out; };
const vec = rows => [...uleb(rows.length), ...rows.flat()];
const string = text => { const bytes = [...new TextEncoder().encode(text)]; return [...uleb(bytes.length), ...bytes]; };
const section = (id, data) => [id, ...uleb(data.length), ...data];
const constant = n => [0x41, ...sleb(n)];
const local = n => [0x20, ...uleb(n)];
const global = n => [0x23, ...uleb(n)];
const setGlobal = n => [0x24, ...uleb(n)];
const load = [0x28, 2, 0], store = [0x36, 2, 0];

// Correct native signatures, shared memory, and a real call_indirect destructor dispatch.
function nativeGuest(memory, imports) {
  const types = [], typeIds = new Map(), imported = [], functions = [], exports = [];
  const type = (arity, result) => {
    const key = `${arity}/${result}`;
    if (!typeIds.has(key)) { typeIds.set(key, types.length); types.push([0x60, ...vec(Array.from({ length: arity }, () => [0x7f])), ...vec(result ? [[0x7f]] : [])]); }
    return typeIds.get(key);
  };
  const rows = [['c', 0, true], ['g', 1, true], ['vb', 6, true], ['r', 1, true], ['A', 0, false], ['I', 2, false]];
  for (const [name, arity, result] of rows) imported.push([...string('eh'), ...string(name), 0, ...uleb(type(arity, result))]);
  imported.push([...string('eh'), ...string('memory'), 2, 1, 1, 8]);
  const fn = (name, arity, result, body) => {
    const index = rows.length + functions.length;
    functions.push({ type: type(arity, result), body: [0, ...body, 0x0b] });
    exports.push([...string(name), 0, ...uleb(index)]);
    return index;
  };
  for (let i = 0; i < rows.length; i++) {
    const [name, arity, result] = rows[i];
    fn(name, arity, result, [...Array.from({ length: arity }, (_, n) => local(n)).flat(), 0x10, ...uleb(i)]);
  }
  fn('ad', 1, true, [...local(0), ...constant(TYPE), 0x47, 0x04, 0x40, 0x00, 0x0b, ...global(0)]);
  fn('$c', 3, true, [...local(0), ...constant(BASE_TYPE), 0x47, ...local(1), ...constant(TYPE), 0x47, 0x72,
    0x04, 0x40, ...constant(0), 0x0f, 0x0b, ...local(2), ...constant(ADJUSTED), ...store, ...constant(1)]);
  fn('_c', 2, false, [...constant(272), ...local(0), ...store, ...constant(276), ...local(1), ...store]);
  fn('gd', 2, true, [...local(1), ...local(0), 0x11, ...uleb(type(1, true)), 0]);
  const destructor = fn('destructor', 1, true, [...constant(256), ...local(0), ...store,
    ...constant(260), ...constant(260), ...load, ...constant(1), 0x6a, ...store, ...local(0)]);
  fn('growMemory', 0, true, [...constant(1), 0x40, 0]);
  fn('Xc', 0, true, global(1));
  fn('Yc', 1, false, [...local(0), ...setGlobal(1)]);
  fn('Zc', 1, true, [...global(1), ...local(0), ...constant(15), 0x6a, ...constant(-16), 0x71, 0x6b, ...setGlobal(1), ...global(1)]);
  exports.push([...string('pointerType'), 3, 0], [...string('stack'), 3, 1]);
  const bytes = new Uint8Array([0,97,115,109,1,0,0,0,
    ...section(1, vec(types)), ...section(2, vec(imported)),
    ...section(3, vec(functions.map(f => uleb(f.type)))), ...section(4, vec([[0x70, 1, 2, 2]])),
    ...section(6, vec([0, 60000].map(value => [0x7f, 1, ...constant(value), 0x0b]))),
    ...section(7, vec(exports)), ...section(9, vec([[0, ...constant(1), 0x0b, ...vec([uleb(destructor)])]])),
    ...section(10, vec(functions.map(f => [...uleb(f.body.length), ...f.body]))),
  ]);
  assert.equal(WebAssembly.validate(bytes), true, 'valid native EH fixture');
  return new WebAssembly.Instance(new WebAssembly.Module(bytes), { eh: { ...imports, memory } }).exports;
}

function rig({ pointerType = false, growOnMalloc = false, nativeStack = false } = {}) {
  const memory = new ArtemisMemory({ memory: new WebAssembly.Memory({ initial: 1, maximum: 8 }) });
  const temp = new ArtemisTempRet0(), allocations = [], freed = [], events = [], observedTypes = [];
  let next = 1024;
  const host = { memory, temp,
    malloc(size) {
      if (growOnMalloc) { memory.memory.grow(1); memory.refresh(); }
      const ptr = next; next += (size + 15) & ~15;
      memory.u8.fill(0xa5, ptr, ptr + size); allocations.push({ ptr, size }); return ptr;
    },
    free(ptr) { freed.push(ptr); events.push(['free', ptr]); },
  };
  const imports = createArtemisEhImports(host), guest = nativeGuest(memory.memory, imports);
  guest.pointerType.value = pointerType ? 1 : 0;
  host.isPointerType = type => { observedTypes.push(type); return guest.ad(type); };
  host.canCatch = guest.$c;
  host.setThrew = guest._c;
  host.runDestructor = (index, ptr) => { events.push(['destructor', index, ptr]); return guest.gd(index, ptr); };
  if (nativeStack) { host.stackSave = guest.Xc; host.stackAlloc = guest.Zc; host.stackRestore = guest.Yc; }
  const word = address => { memory.refresh(); return memory.i32[address >>> 2]; };
  return { imports, guest, host, memory, temp, allocations, freed, events, observedTypes, word };
}
function throwPayload(r, { type = TYPE, destructor = 0 } = {}) {
  const payload = r.imports.u(32), header = payload - 16;
  r.memory.i32[(header + 4) >>> 2] = 77;
  r.memory.u8[header + 12] = 1; r.memory.u8[header + 13] = 1;
  assert.throws(() => r.imports.w(payload, type, destructor), e => e instanceof ArtemisCppThrow && e.pointer === payload);
  return payload;
}

test('native RTTI and destructor fixture rejects foreign type words and null table slots', () => {
  const r = rig(); assert.equal(r.guest.ad(TYPE), 0);
  assert.throws(() => r.guest.ad(0), WebAssembly.RuntimeError);
  assert.throws(() => r.guest.gd(0, 1024), WebAssembly.RuntimeError);
  assert.equal(r.guest.gd(1, 1024), 1024); assert.equal(r.word(256), 1024);
  assert.deepEqual([...ARTEMIS_EH_IMPORTS].sort(), Object.keys(r.imports).sort());
});
test('throw initializes all five fields in the 16-byte header below the payload', () => {
  const r = rig(), payload = throwPayload(r, { destructor: 1 });
  assert.deepEqual(r.allocations[0], { ptr: payload - 16, size: 48 });
  assert.deepEqual([r.word(payload - 16), r.word(payload - 12), r.word(payload - 8)], [1, 0, TYPE]);
  assert.deepEqual([...r.memory.u8.slice(payload - 4, payload - 2)], [0, 0]);
  assert.equal(r.imports.Db(), 1);
});
test('native g/r import signatures preserve an existing CatchInfo without reallocating it', () => {
  const r = rig(), payload = throwPayload(r), record = r.guest.g(TYPE);
  assert.equal(r.word(record), payload); assert.equal(r.word(record + 4), 0);
  assert.equal(r.temp.get(), TYPE); assert.deepEqual(r.observedTypes, [], 'search does not call pointer RTTI');
  const before = r.allocations.length;
  assert.equal(r.guest.r(record), payload); assert.equal(r.allocations.length, before);
  assert.deepEqual(r.observedTypes, [TYPE]); assert.equal(r.word(payload - 12), 1);
  assert.equal(r.imports.Db(), 0); r.guest.A();
  assert.deepEqual(r.freed, [payload - 16, record]);
});
test('can_catch writes a base adjustment that begin_catch retains', () => {
  const r = rig(), payload = throwPayload(r), record = r.guest.g(BASE_TYPE);
  assert.equal(r.temp.get(), BASE_TYPE); assert.equal(r.word(record + 4), ADJUSTED);
  assert.equal(r.guest.r(record), ADJUSTED); assert.equal(r.word(record + 4), ADJUSTED);
  r.guest.A(); assert.deepEqual(r.freed, [payload - 16, record]);
});
test('pointer exceptions dereference the payload only at begin_catch, without early free', () => {
  const r = rig({ pointerType: true }), payload = throwPayload(r);
  r.memory.i32[payload >>> 2] = 53248;
  const record = r.guest.g(BASE_TYPE);
  assert.equal(r.temp.get(), BASE_TYPE); assert.equal(r.word(record + 4), ADJUSTED);
  assert.deepEqual(r.freed, []); assert.deepEqual(r.observedTypes, []);
  assert.equal(r.guest.r(record), 53248); r.guest.A();
  assert.deepEqual(r.freed, [payload - 16, record]);
});
test('zero-arity c, six-arity vb, catch-all and unmatched search preserve their real ABI', () => {
  const r = rig(), payload = throwPayload(r);
  for (const record of [r.guest.c(), r.guest.g(0), r.guest.g(123), r.guest.vb(123, BASE_TYPE, 456, 789, 987, 654)]) {
    assert.equal(r.word(record), payload); assert.equal(r.freed.includes(record), false);
  }
  assert.equal(r.temp.get(), BASE_TYPE);
  const record = r.guest.g(123); assert.equal(r.temp.get(), TYPE); assert.equal(r.word(record + 4), 0);
});
test('empty search and zero-type throw do not query foreign RTTI', () => {
  const r = rig(); assert.equal(r.guest.c(), 0); assert.equal(r.temp.get(), 0);
  const payload = throwPayload(r, { type: 0 }), record = r.guest.c();
  assert.equal(r.word(record), payload); assert.equal(r.temp.get(), 0); assert.deepEqual(r.observedTypes, []);
  assert.throws(() => r.imports.i(record), e => e instanceof ArtemisCppThrow && e.pointer === payload);
});
test('end_catch dispatches gd with the actual table index, then frees both blocks once', () => {
  const r = rig(), payload = throwPayload(r, { destructor: 1 }), record = r.guest.g(TYPE);
  r.guest.r(record); r.guest.A();
  assert.equal(r.word(256), payload); assert.equal(r.word(260), 1);
  assert.deepEqual(r.events, [['destructor', 1, payload], ['free', payload - 16], ['free', record]]);
  assert.deepEqual([r.word(272), r.word(276)], [0, 0]); assert.equal(r.guest.c(), 0);
});
test('native destructor and pointer-RTTI traps propagate rather than being rescued', () => {
  const r = rig(), payload = throwPayload(r, { destructor: 0x7fffffff }), record = r.guest.g(TYPE);
  r.guest.r(record); assert.throws(() => r.guest.A(), WebAssembly.RuntimeError); assert.deepEqual(r.freed, []);
  assert.equal(r.word(payload - 12), 0);
  const other = rig(), ptr = throwPayload(other), catchRecord = other.guest.g(TYPE);
  other.memory.i32[(ptr - 8) >>> 2] = 0;
  assert.throws(() => other.guest.r(catchRecord), WebAssembly.RuntimeError);
});
test('rethrow keeps the first reference until both nested catches end', () => {
  const r = rig(), payload = throwPayload(r, { destructor: 1 }), first = r.guest.g(TYPE);
  r.guest.r(first); assert.throws(() => r.imports.za(), e => e instanceof ArtemisCppThrow && e.pointer === payload);
  assert.equal(r.imports.Db(), 1); assert.equal(r.word(payload - 12), 1); assert.deepEqual(r.freed, []);
  const second = r.guest.g(TYPE); r.guest.r(second);
  assert.equal(r.word(payload - 12), 2); assert.equal(r.imports.Db(), 0);
  r.guest.A(); assert.equal(r.word(payload - 12), 1); assert.deepEqual(r.freed, [second]);
  r.guest.A(); assert.equal(r.word(payload - 12), 0);
  assert.deepEqual(r.freed, [second, payload - 16, first]); assert.equal(r.word(260), 1);
});
test('resumeException frees only its supplied record and rethrows the base payload', () => {
  const r = rig(), payload = throwPayload(r), record = r.guest.g(TYPE);
  assert.throws(() => r.imports.i(record), e => e instanceof ArtemisCppThrow && e.pointer === payload);
  assert.deepEqual(r.freed, [record]); assert.equal(r.imports.Db(), 1);
});
test('allocations and guest can_catch growing memory do not leave stale heap views', () => {
  const r = rig({ growOnMalloc: true }), payload = throwPayload(r);
  const canCatch = r.host.canCatch;
  r.host.canCatch = (...args) => { r.memory.memory.grow(1); return canCatch(...args); };
  const record = r.guest.g(BASE_TYPE);
  assert.equal(r.memory.pages, 5); assert.equal(r.word(record + 4), ADJUSTED);
  assert.equal(r.guest.r(record), ADJUSTED); r.guest.A();
  assert.deepEqual(r.freed, [payload - 16, record]);
});
test('a guest destructor may grow memory before the allocator reads its release', () => {
  const r = rig(), payload = throwPayload(r, { destructor: 1 }), record = r.guest.g(TYPE);
  const original = r.host.runDestructor;
  r.host.runDestructor = (...args) => { r.memory.memory.grow(1); return original(...args); };
  r.guest.r(record); r.guest.A(); assert.equal(r.word(260), 1);
  assert.deepEqual(r.freed, [payload - 16, record]);
});
test('guest stack scratch restores on identity and no-match, but retains successful can_catch allocation', () => {
  const r = rig({ nativeStack: true }); throwPayload(r);
  assert.equal(r.guest.stack.value, 60000);
  r.guest.g(TYPE); assert.equal(r.guest.stack.value, 60000);
  r.guest.g(123); assert.equal(r.guest.stack.value, 60000);
  r.guest.g(BASE_TYPE); assert.equal(r.guest.stack.value, 59984);
  r.guest.Yc(60000);
});
test('throw imports refresh after direct guest memory growth before initializing the header', () => {
  const r = rig(), payload = r.imports.u(32);
  r.guest.growMemory();
  assert.throws(() => r.imports.w(payload, TYPE, 0), ArtemisCppThrow);
  assert.equal(r.word(payload - 8), TYPE);
  assert.equal(r.word(payload - 12), 0);
  assert.equal(r.word(r.guest.g(TYPE)), payload);
});
test('begin_catch and rethrow refresh when the guest grows between consecutive imports', () => {
  const r = rig(), payload = throwPayload(r), record = r.guest.g(TYPE);
  r.guest.growMemory();
  assert.equal(r.guest.r(record), payload);
  assert.equal(r.word(payload - 12), 1);
  r.guest.growMemory();
  assert.throws(() => r.imports.za(), e => e instanceof ArtemisCppThrow && e.pointer === payload);
  r.memory.refresh();
  assert.deepEqual([...r.memory.u8.slice(payload - 4, payload - 2)], [0, 1], 'caught cleared; rethrown set');
  assert.equal(r.imports.Db(), 1);
});
test('resumeException refreshes its record before throwing the base after guest growth', () => {
  const r = rig(), payload = throwPayload(r), record = r.guest.g(TYPE);
  r.guest.growMemory();
  assert.throws(() => r.imports.i(record), e => e instanceof ArtemisCppThrow && e.pointer === payload);
  assert.deepEqual(r.freed, [record]);
});
test('signed adjusted pointers round-trip without replacing a valid payload base', () => {
  const r = rig(), payload = throwPayload(r), record = r.guest.g(TYPE);
  r.memory.i32[(record + 4) >>> 2] = 0xffffff00 | 0;
  assert.equal(r.guest.r(record), -256); assert.equal(r.word(record), payload); r.guest.A();
});
test('native longjmp import uses the guest _c(env,value||1) before raising its marker', () => {
  const r = rig();
  assert.throws(() => r.guest.I(4096, 0), e => e instanceof ArtemisLongjmp && e.value === 1);
  assert.deepEqual([r.word(272), r.word(276)], [4096, 1]);
  assert.throws(() => r.guest.I(4096, 7), ArtemisLongjmp);
  assert.deepEqual([r.word(272), r.word(276)], [4096, 7]);
});
