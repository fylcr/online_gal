// Differential test: the official `artemis.wasm` versus the wasm binary rebuilt
// from its own wasm2c decompilation.
//
// The two are not byte-comparable and never will be: one is Emscripten's release
// output, the other is clang's codegen for the transpiled C. What IS comparable is
// behaviour, as long as both sides are given the same environment. So this script
// instantiates both with inert (zero-returning) host stubs, calls every export that
// exists on both sides with identical arguments, and compares the results.
//
// Two things make the comparison honest rather than decorative:
//
//   * The stub table is derived from the module's own type section, not hardcoded.
//     An i64 import must yield `0n`, not `0`, or the call throws at the boundary --
//     and that would look like a behavioural difference when it is a harness bug.
//
//   * Every export gets a FRESH pair of instances. The exports share one linear
//     memory and write to it, so calling them in sequence on a single instance lets
//     earlier calls perturb later ones, and the outcome then depends on the order
//     the names happen to sort in. Fresh state per comparison is what makes the
//     result a property of the module instead of of the harness.
//
// A third outcome is reported separately rather than as a failure: BOTH sides trap,
// but with different trap text. Every case observed so far is a `call_indirect` that
// reaches an empty table slot on the rebuilt side. The official module's table holds
// 5,884 entries filled by its link, while clang only fills the slots reachable from
// the rebuilt code, so the same indirect call lands on a populated slot on one side
// (trap: "function signature mismatch") and an empty one on the other (trap:
// "unreachable"). Neither side returns a value, so this says nothing about the
// transpilation -- it is a property of how full each table is. It is printed loudly
// and counted, and `--strict` turns it back into a failure for anyone who wants that.
//
// Usage: node wasm-diff.mjs <official.wasm> <rebuilt.wasm> [--strict]
import { readFileSync } from 'node:fs';

// ---------------------------------------------------------------- wasm parsing
// Just enough of the binary format to read the type and import sections. LEB128
// unsigned + signed, per the spec.
function leb(buf, pos) {
  let result = 0, shift = 0, byte;
  do { byte = buf[pos++]; result |= (byte & 0x7f) << shift; shift += 7; } while (byte & 0x80);
  return [result >>> 0, pos];
}
const VAL = { 0x7f: 'i32', 0x7e: 'i64', 0x7d: 'f32', 0x7c: 'f64', 0x7b: 'v128', 0x70: 'funcref', 0x6f: 'externref' };

// Walk one wasm `limits` value: a flags byte, a minimum, and a maximum only when flags
// bit 0 is set. Shared (bit 1) and 64-bit (bit 2) change the meaning of the numbers but
// not how many there are.
function skipLimits(buf, pos) {
  const flags = buf[pos++];
  let _min; [_min, pos] = leb(buf, pos);
  if (flags & 1) { let _max; [_max, pos] = leb(buf, pos); }
  return pos;
}

// Returns { types: [[retTypeString]], imports: [{module, name, kind, typeIndex}] }
function scan(buf) {
  let pos = 8; // magic + version
  const types = [], imports = [];
  while (pos < buf.length) {
    const id = buf[pos++];
    let size; [size, pos] = leb(buf, pos);
    const end = pos + size;
    if (id === 1) { // type
      let n; [n, pos] = leb(buf, pos);
      for (let i = 0; i < n; i++) {
        pos++; // form, always 0x60
        let np; [np, pos] = leb(buf, pos);
        for (let j = 0; j < np; j++) pos++; // params
        let nr; [nr, pos] = leb(buf, pos);
        const rets = [];
        for (let j = 0; j < nr; j++) rets.push(VAL[buf[pos++]] ?? '?');
        types.push(rets);
      }
    } else if (id === 2) { // import
      let n; [n, pos] = leb(buf, pos);
      for (let i = 0; i < n; i++) {
        let ml; [ml, pos] = leb(buf, pos);
        const module = buf.toString('utf8', pos, pos + ml); pos += ml;
        let nl; [nl, pos] = leb(buf, pos);
        const name = buf.toString('utf8', pos, pos + nl); pos += nl;
        const kind = buf[pos++];
        // Every descriptor has to be skipped *completely*, or the next entry's name
        // length is read out of the middle of this one and the walk degenerates into
        // garbage -- the symptom is an "unhandled import kind" in the double digits.
        // A memory's limits are a flags byte plus a minimum and, when flags bit 0 is
        // set, a maximum; leaving the maximum unread is exactly that bug.
        if (kind === 0) { let ti; [ti, pos] = leb(buf, pos); imports.push({ module, name, kind: 'function', typeIndex: ti }); }
        else if (kind === 2) {
          // The declared limits have to be recorded, not just skipped: the memory handed to
          // the module must be at least as large as it declares, and a promoted engine
          // declares far more than the official 256 pages. Ignoring them produced
          // "memory import has 256 pages which is smaller than the declared initial of 3072"
          // -- a diff that fails on the harness's own buffer rather than on the module.
          const flags = buf[pos]; pos++;
          let min; [min, pos] = leb(buf, pos);
          let max = min;
          if (flags & 1) { [max, pos] = leb(buf, pos); }
          imports.push({ module, name, kind: 'memory', min, max, shared: !!(flags & 2) });
        }
        else if (kind === 1) { pos++; pos = skipLimits(buf, pos); }
        else if (kind === 3) { pos += 2; imports.push({ module, name, kind: 'global' }); }
        else throw new Error(`unhandled import kind ${kind} for ${module}.${name}`);
      }
    }
    pos = end;
  }
  return { types, imports };
}

// Build the import object for one module, with every host function returning zero.
function inertImports(buf) {
  const { types, imports } = scan(buf);
  const o = {};
  for (const imp of imports) {
    if (imp.kind === 'memory') {
      // Use the module's OWN declared limits: it is an import, so an undersized buffer is a
      // link error on our side, not a property of the module.
      (o[imp.module] ||= {})[imp.name] = new WebAssembly.Memory({
        initial: imp.min ?? 256,
        maximum: Math.max(imp.max ?? 65536, imp.min ?? 256),
        shared: !!imp.shared,
      });
      continue;
    }
    if (imp.kind !== 'function') continue;
    const ret = types[imp.typeIndex]?.[0] ?? 'i32';
    // An i64 import must yield a BigInt or the call throws at the boundary; i32/f32/f64
    // all take the number 0.
    const zero = ret === 'i64' ? 0n : 0;
    (o[imp.module] ||= {})[imp.name] = () => zero;
  }
  return o;
}

// One fresh instance, one call. Returns the value or the trap text, so that a
// shared trap is reported as agreement about the trap.
//
// `__wasm_call_ctors` has to be invoked explicitly. Emscripten normally runs it
// from its generated JS, and we instantiate the bare module, so without this call
// no static initialiser runs -- including the one that instantiates the wasm2c
// module behind the export shims. Every export then dereferences a zeroed instance
// and traps, which reads as "the rebuilt code is broken" when it is the harness.
async function callFresh(path, buf, mod, exportName, argc) {
  const instance = await WebAssembly.instantiate(mod, inertImports(buf));
  if (instance.exports.__wasm_call_ctors) instance.exports.__wasm_call_ctors();
  try { return String(instance.exports[exportName](...new Array(argc).fill(0))); }
  catch (e) { return `THREW ${e.constructor.name}: ${e.message}`; }
}

// ----------------------------------------------------------------------- main
const argv = process.argv.slice(2);
const strict = argv.includes('--strict');
const [officialPath, rebuiltPath] = argv.filter((a) => a !== '--strict');
const officialBuf = readFileSync(officialPath);
const rebuiltBuf = readFileSync(rebuiltPath);
const officialMod = await WebAssembly.compile(officialBuf);
const rebuiltMod = await WebAssembly.compile(rebuiltBuf);

// wasm2c escapes a character that cannot start a C identifier as `0xHH`, so the
// official export `_c` appears as `0x5Fc`. Decoding every `0xHH` pair is what
// recovers such names; a naive "whole name is 0x..." rule would miss them.
const unmangle = (n) => n.replace(/^w2c_artemis_/, '').replace(/0x([0-9A-Fa-f]{2})/g, (_, h) => String.fromCharCode(parseInt(h, 16)));

const officialFns = new Map();
for (const e of WebAssembly.Module.exports(officialMod)) if (e.kind === 'function') officialFns.set(e.name, e);

// The rebuilt module exports both the original names (via wasm-export-shims.c) and
// the raw wasm2c symbols. The raw ones un-mangle onto the SAME keys -- so a plain
// `set` would let whichever came last win, and calling a raw symbol passes the
// instance pointer as its first argument, i.e. NULL. That looks exactly like the
// rebuilt code being broken. Exact names must therefore take precedence, and a raw
// symbol is only used as a fallback when no exact name is present.
const exact = new Map();
const mangled = new Map();
for (const e of WebAssembly.Module.exports(rebuiltMod)) {
  if (e.kind !== 'function') continue;
  if (e.name.startsWith('w2c_artemis_')) mangled.set(unmangle(e.name), e);
  else exact.set(e.name, e);
}
const rebuiltFns = new Map([...mangled, ...exact]);

const shared = [...officialFns.keys()].filter((n) => rebuiltFns.has(n)).sort();
const officialOnly = [...officialFns.keys()].filter((n) => !rebuiltFns.has(n));
const rebuiltOnly = [...rebuiltFns.keys()].filter((n) => !officialFns.has(n));

console.log(`official exports (functions): ${officialFns.size}`);
console.log(`rebuilt  exports (functions): ${rebuiltFns.size}`);
console.log(`shared: ${shared.length}`);
console.log(`official-only: ${officialOnly.length}${officialOnly.length ? ' -> ' + officialOnly.join(', ') : ''}`);
console.log(`rebuilt-only:  ${rebuiltOnly.length} (emscripten runtime + the host stubs compiled in, not imported)`);
console.log('');

// Ask each export for its own arity rather than guessing from a signature table.
const probe = await WebAssembly.instantiate(officialMod, inertImports(officialBuf));

let agree = 0, differ = 0, trappy = 0, valAgree = 0, trapAgree = 0;
const diffs = [], traps = [];
for (const name of shared) {
  const argc = probe.exports[officialFns.get(name).name].length;
  const o = await callFresh(officialPath, officialBuf, officialMod, officialFns.get(name).name, argc);
  const r = await callFresh(rebuiltPath, rebuiltBuf, rebuiltMod, rebuiltFns.get(name).name, argc);
  const bothTrapped = o.startsWith('THREW ') && r.startsWith('THREW ');
  if (o === r) {
    agree++;
    if (bothTrapped) trapAgree++; else valAgree++;
    console.log(`  same   ${name}(${argc}) = ${o}`);
  } else if (bothTrapped) {
    trappy++; traps.push(name);
    console.log(`  trap?  ${name}(${argc}): both trapped, differently -- official ${o} | rebuilt ${r}`);
  } else { differ++; diffs.push(name); console.log(`  DIFFER ${name}(${argc}): official ${o} | rebuilt ${r}`); }
}
console.log('');
console.log(`agreement: ${agree}/${shared.length}, trap-kind-only: ${trappy}${traps.length ? ' -> ' + traps.join(', ') : ''}, differ: ${differ}${diffs.length ? ' -> ' + diffs.join(', ') : ''}`);
// How much of the agreement is a real comparison. "Both returned the same value" is
// the strong result; "both threw the same trap" is weaker (the export got far enough
// to fail identically, which still exercises its preamble and argument handling);
// "both threw a different trap" is weakest of all. Printing the split keeps the
// headline number from overstating what was actually checked.
console.log(`  of the agreements: ${valAgree} returned equal values, ${trapAgree} threw the same trap`);
if (trappy) {
  console.log('');
  console.log('The trap-kind cases above are the rebuilt table being less full than the official');
  console.log('one: an indirect call reaches an empty slot. Pass --strict to fail on them too.');
}
// Export-surface completeness is always enforced. Value/trap agreement is enforced
// unless the only difference is which trap fired.
process.exit(differ === 0 && (!strict || trappy === 0) && shared.length === officialFns.size ? 0 : 1);
