#!/usr/bin/env node
// wasm-replace-test.mjs -- decide whether a rebuilt wasm can stand in for the official one.
//
// Why this is not just `wasm-diff.mjs` again.  wasm-diff answers "is the transpiled code
// the same code?" by calling the shared exports and comparing.  That question is already
// settled (differ: 0).  The question a *replacement* asks is different and strictly
// larger: the official module is only half an engine -- the other half is the JS glue,
// which supplies 199 host functions through an import named `a` and then reads a fixed
// set of short names back off the instance.  A candidate that is byte-for-byte faithful
// to the engine's logic but does not speak that loader contract cannot be dropped in
// where the official was, no matter how well its exports behave in isolation.
//
// So the tester walks the loader contract in order, and stops at the first gate that
// fails -- each gate is strictly stronger than the last, and the gates that fail first
// are the ones that make the later ones moot:
//
//   Gate 1  import face   -- would the glue's own import object construct it at all?
//   Gate 2  export face   -- would every name the glue reads off `Module.asm` resolve?
//   Gate 3  behaviour     -- do the shared exports compute the same things? (wasm-diff)
//   Gate 4  host wiring   -- does it actually call the host, or are the host calls stubs?
//
// Gate 4 is the one that catches the specific trap this pipeline fell into: compiling the
// import stubs *into* the module makes gate 3 look great (the engine logic is intact) while
// gate 1 shows the host ABI is gone.  Gates 1 and 4 are two views of the same fact, and
// having both is deliberate: gate 1 says "the glue cannot hand it a GL context", gate 4
// says "and it would not ask for one anyway".
//
// Usage:
//   node wasm-replace-test.mjs <candidate.wasm> [more.wasm ...]
//   node wasm-replace-test.mjs --official <wasm> --glue <js> --strict <candidate.wasm>
//   node wasm-replace-test.mjs --json <candidate.wasm>
//
// Exit status is 0 only when every candidate passes every gate.
//
// The candidates are split into two journeys, because they fail differently:
//   - "drop-in": gates 1+2 are the whole story, the candidate must speak the glue's ABI.
//   - "the engine is in there": gate 3 tells you whether the *logic* survived, whatever
//     the ABI looks like.  A candidate can pass gate 3 and fail gate 1 (that is exactly
//     the cxx-route / wasm2c rebuilt modules), and the report must say so in one line
//     rather than making the reader reconcile four gates themselves.

import { readFileSync, existsSync } from 'node:fs';
import { spawnSync } from 'node:child_process';
import { dirname, resolve, basename } from 'node:path';
import { fileURLToPath } from 'node:url';

const HERE = dirname(fileURLToPath(import.meta.url));
const REPO = resolve(HERE, '..', '..', '..');

const DEFAULTS = {
  official: resolve(REPO, 'ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm'),
  glue: resolve(REPO, 'ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js'),
};

function parseArgs(argv) {
  const out = { official: DEFAULTS.official, glue: DEFAULTS.glue, strict: false, json: false, candidates: [] };
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (a === '--official') out.official = resolve(argv[++i]);
    else if (a === '--glue') out.glue = resolve(argv[++i]);
    else if (a === '--strict') out.strict = true;
    else if (a === '--json') out.json = true;
    else if (a === '-h' || a === '--help') out.help = true;
    else if (a.startsWith('--')) throw new Error(`unknown option ${a}`);
    else out.candidates.push(resolve(a));
  }
  return out;
}

// ---------------------------------------------------------------------------
// The loader contract, read out of the glue rather than hardcoded.
//
// Hardcoding 199 names would rot the moment the SDK tree changes, and the whole reason
// this pipeline can trust its own verdicts is that they are derived from the artefact
// being checked.  Each of the three extractions below is anchored on a literal that a
// minified-but-not-mangled Emscripten release build always emits.
// ---------------------------------------------------------------------------
function readGlueContract(gluePath) {
  if (!existsSync(gluePath)) throw new Error(`glue not found: ${gluePath}`);
  const js = readFileSync(gluePath, 'utf8');

  // `function createWasm(){var info={"a":asmLibraryArg};...`  -- the module name the
  // engine's imports live under.  Minified, so it is a one- or two-letter name, but it is
  // whatever the glue says it is, not whatever we assume.
  const info = js.match(/var info=\{("(?:[^"]+)")\s*:\s*(\w+)\}/);
  if (!info) throw new Error('could not find the glue\'s wasm import object (`var info={...}`)');
  const importModule = JSON.parse(info[1]);

  // `var asmLibraryArg={"q":___assert_fail,...,"a":wasmMemory,...};var asm=createWasm`
  const argBlock = js.match(/var asmLibraryArg=\{([\s\S]*?)\};var asm=/);
  if (!argBlock) throw new Error('could not find asmLibraryArg in the glue');
  const supplied = new Map();
  for (const m of argBlock[1].matchAll(/"([^"]+)":([A-Za-z0-9_$]+)/g)) supplied.set(m[1], m[2]);

  // `wasmTable=Module["asm"]["Lc"]` -- the name the glue resolves the indirect-call table
  // off the instance under.  It is NOT always `Lc`: the plain build exports its table as
  // `Ic` and the asyncify build as `Lc`, so the name has to be taken from the glue's own
  // assignment rather than assumed. Likewise every other `Module["asm"][X]` the glue reads
  // has to resolve, so collect the set and remember which one the table is.
  const readOff = new Set();
  for (const m of js.matchAll(/Module\["asm"\]\["([^"]+)"\]/g)) readOff.add(m[1]);
  const tableNames = new Set();
  for (const m of js.matchAll(/wasmTable=Module\["asm"\]\["([^"]+)"\]/g)) tableNames.add(m[1]);

  return { importModule, supplied, readOff, tableNames, bytes: js.length };
}

// A WebAssembly.Memory that satisfies an imported memory.
//
// The JS API does not expose a module's declared limits, and guessing is not good enough:
// the link check is exact. A candidate that pulls in its own Emscripten runtime needs a
// bigger buffer than the official 256 pages, and offering the wrong size fails with
// "memory import has a larger maximum size" or "declared initial" -- which reads as the
// candidate being unable to load at all, when the real story is that the tester handed it
// the wrong buffer. So the limits are read from the module's own import section.
function importedMemoryLimits(bytes) {
  const buf = Buffer.isBuffer(bytes) ? bytes : Buffer.from(bytes);
  function uleb(pos) {
    let result = 0, shift = 0, byte;
    do { byte = buf[pos++]; result |= (byte & 0x7f) << shift; shift += 7; } while (byte & 0x80);
    return [result >>> 0, pos];
  }
  let pos = 8;
  while (pos < buf.length) {
    const id = buf[pos++];
    let size; [size, pos] = uleb(pos);
    const end = pos + size;
    if (id === 2) {
      let count; [count, pos] = uleb(pos);
      for (let i = 0; i < count; i++) {
        let len; [len, pos] = uleb(pos); pos += len;   // module
        [len, pos] = uleb(pos); pos += len;            // name
        const kind = buf[pos++];
        if (kind === 2) {
          const flags = buf[pos++];
          let min; [min, pos] = uleb(pos);
          let max = min;
          if (flags & 1) { [max, pos] = uleb(pos); }
          return { min, max, shared: !!(flags & 2) };
        }
        if (kind === 0) { let _; [_ , pos] = uleb(pos); }
        else if (kind === 1) { pos++; const fl = buf[pos++]; let _; [_ , pos] = uleb(pos); if (fl & 1) { [_ , pos] = uleb(pos); } }
        else if (kind === 3) { pos += 2; }
      }
    }
    pos = end;
  }
  return null;
}

function makeImportedMemory(bytes) {
  const limits = bytes ? importedMemoryLimits(bytes) : null;
  const shape = limits
    ? { initial: limits.min, maximum: Math.max(limits.max, limits.min), shared: limits.shared }
    : { initial: 256, maximum: 65536 };
  return new WebAssembly.Memory(shape);
}

// Install one counting stub per imported function.  i64-returning imports need a BigInt
// return, and the declared result type is not visible from the JS API, so the stub flips
// modes on demand: the caller retries once with BigInt after a boundary TypeError.  This
// matters only for the *counts* in gate 4; a throw would show up as a missing call.
function countingImports(module, bytes) {
  const counts = new Map();
  const stubs = [];
  let bigint = false;

  function stubFor(key) {
    const fn = function (...args) {
      counts.set(key, (counts.get(key) || 0) + 1);
      return bigint ? 0n : 0;
    };
    stubs.push(fn);
    return fn;
  }

  const obj = {};
  for (const imp of WebAssembly.Module.imports(module)) {
    obj[imp.module] ||= {};
    const key = `${imp.module}.${imp.name}`;
    if (imp.kind === 'memory') { obj[imp.module][imp.name] = makeImportedMemory(bytes); continue; }
    if (imp.kind === 'table') { obj[imp.module][imp.name] = new WebAssembly.Table({ initial: 0, element: 'anyfunc' }); continue; }
    if (imp.kind === 'global') { obj[imp.module][imp.name] = 0; continue; }
    obj[imp.module][imp.name] = stubFor(key);
  }
  return { obj, counts, useBigInt: () => { bigint = true; } };
}

// Instantiate fresh, run __wasm_call_ctors (nothing else runs static initialisers for a
// bare module -- see wasm-diff.mjs, same trap), then hand back the exports.
async function instantiate(bytes, imports) {
  const { instance } = await WebAssembly.instantiate(bytes, imports);
  if (typeof instance.exports.__wasm_call_ctors === 'function') instance.exports.__wasm_call_ctors();
  return instance.exports;
}

// ---------------------------------------------------------------------------
// Gates
// ---------------------------------------------------------------------------
function gate1ImportFace(module, contract) {
  const imports = WebAssembly.Module.imports(module);
  const onModule = imports.filter((i) => i.module === contract.importModule);
  const imported = new Set(onModule.filter((i) => i.kind === 'function').map((i) => i.name));
  const hasMemoryImport = onModule.some((i) => i.kind === 'memory');

  const glueNames = [...contract.supplied.keys()];
  const glueFns = glueNames.filter((n) => n !== 'a' || contract.supplied.get(n) !== 'wasmMemory');

  const consumed = glueFns.filter((n) => imported.has(n));
  const demanded = [...imported];
  const unsatisfiable = demanded.filter((n) => !contract.supplied.has(n));
  const otherModules = [...new Set(imports.map((i) => i.module))].filter((m) => m !== contract.importModule);

  return {
    glueModule: contract.importModule,
    glueSupplied: glueFns.length,
    memoryImported: hasMemoryImport,
    memorySelfExported: WebAssembly.Module.exports(module).some((e) => e.kind === 'memory'),
    consumedFromGlue: consumed.length,
    demanded,
    unsatisfiable,
    otherModules,
    // Drop-in requires: the glue's module is the only import source, every name it asks
    // for is actually used, and nothing is demanded that the glue does not have.
    pass: otherModules.length === 0 && consumed.length === glueFns.length && unsatisfiable.length === 0 && hasMemoryImport,
  };
}

function gate2ExportFace(module, contract) {
  const exports = WebAssembly.Module.exports(module);
  const byName = new Map(exports.map((e) => [e.name, e.kind]));
  const missing = [];
  const wrongKind = [];
  for (const name of contract.readOff) {
    if (!byName.has(name)) { missing.push(name); continue; }
    const want = contract.tableNames.has(name) ? 'table' : 'function';
    if (byName.get(name) !== want) wrongKind.push(`${name}: has ${byName.get(name)}, glue wants ${want}`);
  }
  return {
    glueReads: contract.readOff.size,
    present: contract.readOff.size - missing.length - wrongKind.length,
    missing,
    wrongKind,
    pass: missing.length === 0 && wrongKind.length === 0,
  };
}

function gate3Behaviour(officialPath, candidatePath, strict) {
  const diff = resolve(HERE, 'wasm-diff.mjs');
  const args = [diff, officialPath, candidatePath];
  if (strict) args.push('--strict');
  const r = spawnSync(process.execPath, args, { encoding: 'utf8', maxBuffer: 64 * 1024 * 1024 });
  const out = `${r.stdout || ''}${r.stderr || ''}`;
  // The summary line is
  //   agreement: 20/66, trap-kind-only: 46 -> $c, Ad, Bd, ... differ: 0
  // and that name list is comma-separated, so the trap-kind tail cannot be matched with a
  // comma-free character class -- it needs a lazy scan up to the `differ:` that follows.
  const m = out.match(/agreement:\s*(\d+)\/(\d+),\s*trap-kind-only:\s*(\d+)[\s\S]*?,\s*differ:\s*(\d+)/);
  if (!m) return { pass: false, error: (out.trim().split('\n').pop() || 'wasm-diff produced no summary'), raw: out };
  const [, agree, total, trapKind, differ] = m;
  return {
    agreed: +agree, total: +total, trapKindOnly: +trapKind, differ: +differ,
    pass: +differ === 0 && (!strict || +trapKind === 0),
    note: +trapKind > 0
      ? `${trapKind} of the shared exports only match in that both trap (the rebuilt indirect-call table is less full than the official one)`
      : null,
  };
}

// Gate 4: instantiate with counting stubs and see whether the candidate actually reaches
// for the host.  A module that compiled the host ABI in as zero-returning stubs will call
// its imports rarely or not at all -- and, more to the point, will not import the 65 GL
// entry points in the first place, which is what makes it unrenderable.
async function gate4HostWiring(module, officialModule, sharedNames, bytes, contract) {
  const report = { importModules: [...new Set(WebAssembly.Module.imports(module).map((i) => i.module))] };

  // The import names are minified (`xa`, `pa`, `qb`), so they mean nothing on their own --
  // `xa` is glDrawElements only because the glue says so. Classify through the contract,
  // which is the same mapping the loader would use, rather than guessing from the name.
  const resolve = (n) => contract.supplied.get(n) || n;
  const isGl = (n) => /_(gl|egl|glew)/i.test(resolve(n));
  // A candidate that words its imports differently still counts: fall back to whatever
  // non-wasi, non-env-runtime functions it declares.
  const hostFns = (mod) => {
    const imps = WebAssembly.Module.imports(mod).filter((i) => i.kind === 'function');
    const onGlueModule = imps.filter((i) => i.module === contract.importModule);
    if (onGlueModule.length) return onGlueModule;
    return imps.filter((i) => i.module !== 'wasi_snapshot_preview1');
  };

  report.hostImports = hostFns(module).length;
  report.glImports = hostFns(module).filter((i) => isGl(i.name)).length;
  const officialGl = new Set(hostFns(officialModule).filter((i) => isGl(i.name)).map((i) => resolve(i.name)));
  const candidateGl = new Set(hostFns(module).filter((i) => isGl(i.name)).map((i) => resolve(i.name)));
  report.glTotal = officialGl.size;
  report.glMissing = [...officialGl].filter((n) => !candidateGl.has(n)).sort();
  report.officialHostImports = hostFns(officialModule).length;

  // Call the engine surface and total up the calls it makes outward.  Driving every shared
  // export (not a sample) is what makes the numbers comparable between candidates -- an
  // export that traps on entry contributes zero, and which ones trap differs per module.
  const probe = sharedNames.slice(0, 80);
  let calls = 0;
  const touched = new Map();
  try {
    for (const name of probe) {
      const imp = countingImports(module, bytes);
      let ex;
      try { ex = await instantiate(bytes, imp.obj); }
      catch (e) { if (/BigInt/i.test(String(e))) { imp.useBigInt(); ex = await instantiate(bytes, imp.obj); } else throw e; }
      const fn = ex[name];
      if (typeof fn !== 'function') continue;
      try { fn(); } catch { /* traps are expected under zero arguments */ }
      for (const [k, v] of imp.counts) { touched.set(k, (touched.get(k) || 0) + v); calls += v; }
    }
  } catch (e) {
    report.error = String(e.message || e);
  }
  report.callsIntoHost = calls;
  report.distinctImportsCalled = touched.size;
  report.sample = [...touched.entries()].sort((a, b) => b[1] - a[1]).slice(0, 6).map(([k, v]) => `${k} x${v}`);
  report.pass = calls > 0 && report.glImports > 0 && report.glMissing.length === 0;
  return report;
}

// ---------------------------------------------------------------------------
function line(s = '') { process.stdout.write(s + '\n'); }
const C = {
  ok: (s) => `\x1b[32m${s}\x1b[0m`,
  bad: (s) => `\x1b[31m${s}\x1b[0m`,
  warn: (s) => `\x1b[33m${s}\x1b[0m`,
  dim: (s) => `\x1b[2m${s}\x1b[0m`,
  b: (s) => `\x1b[1m${s}\x1b[0m`,
};
const verdictMark = (p) => (p ? C.ok('PASS') : C.bad('FAIL'));

async function testCandidate(path, contract, officialBytes, officialModule, opts) {
  if (!existsSync(path)) return { path, error: 'file not found' };
  const bytes = readFileSync(path);
  const module = await WebAssembly.compile(bytes);

  const officialExports = new Set(
    WebAssembly.Module.exports(officialModule).filter((e) => e.kind === 'function').map((e) => e.name));
  const shared = WebAssembly.Module.exports(module)
    .filter((e) => e.kind === 'function' && officialExports.has(e.name))
    .map((e) => e.name).sort();

  const r = { path, bytes: bytes.length, shared: shared.length };
  r.g1 = gate1ImportFace(module, contract);
  r.g2 = gate2ExportFace(module, contract);
  r.g3 = gate3Behaviour(opts.official, path, opts.strict);
  r.g4 = await gate4HostWiring(module, officialModule, shared, bytes, contract);

  // Two independent questions, deliberately not collapsed into one score.
  r.dropIn = r.g1.pass && r.g2.pass;
  r.logicFaithful = r.g3.pass;
  r.runnable = r.dropIn && r.g4.pass;
  return r;
}

function reportOne(r, contract) {
  line();
  line(C.b('='.repeat(78)));
  line(`${C.b(basename(r.path))}  ${C.dim(`${r.bytes.toLocaleString()} bytes`)}`);
  line(C.dim(r.path));
  line(C.b('='.repeat(78)));
  if (r.error) { line(`  ${C.bad('ERROR')} ${r.error}`); return; }

  line(`  ${verdictMark(r.dropIn)}  ${C.b('Gate 1 · import face')}  ${C.dim('would the glue\'s own import object construct it?')}`);
  line(`         glue supplies ${r.g1.glueSupplied} functions under module "${r.g1.glueModule}"`);
  line(`         candidate consumes ${r.g1.consumedFromGlue} of them`);
  line(`         candidate demands ${r.g1.demanded.length}, of which the glue cannot supply ${r.g1.unsatisfiable.length}`);
  if (r.g1.unsatisfiable.length) {
    line(`           ${C.bad('unsatisfiable:')} ${r.g1.unsatisfiable.join(', ')}`);
  }
  if (r.g1.otherModules.length) {
    line(`         candidate imports from other modules: ${r.g1.otherModules.join(', ')}`);
  }
  line(`         memory: ${r.g1.memoryImported ? 'imported (as the official one is)' : C.bad('NOT imported')}` +
       `${r.g1.memorySelfExported ? C.dim(' · exports its own instead') : ''}`);

  line(`  ${verdictMark(r.g2.pass)}  ${C.b('Gate 2 · export face')}  ${C.dim('would every name the glue reads resolve?')}`);
  line(`         glue reads ${r.g2.glueReads} names off Module.asm (the ${[...contract.tableNames].join('/')} table` +
       `${contract.readOff.size > contract.tableNames.size ? ', the asyncify controls, the dynCall slots' : ''})`);
  line(`         present with the right kind: ${r.g2.present}/${r.g2.glueReads}`);
  if (r.g2.missing.length) line(`           ${C.bad('missing:')} ${r.g2.missing.join(', ')}`);
  if (r.g2.wrongKind.length) line(`           ${C.bad('wrong kind:')} ${r.g2.wrongKind.join(', ')}`);

  line(`  ${verdictMark(r.g3.pass)}  ${C.b('Gate 3 · behaviour')}  ${C.dim('do the shared exports compute the same things?')}`);
  if (r.g3.error) line(`         ${C.bad(r.g3.error)}`);
  else {
    line(`         agreement ${r.g3.agreed}/${r.g3.total} · differ ${r.g3.differ} · trap-kind-only ${r.g3.trapKindOnly}`);
    if (r.g3.note) line(C.dim(`         ${r.g3.note}`));
  }

  line(`  ${verdictMark(r.g4.pass)}  ${C.b('Gate 4 · host wiring')}  ${C.dim('does it actually call the host?')}`);
  line(`         host functions imported: ${r.g4.hostImports} (official: ${r.g4.officialHostImports}) · GL among them: ${r.g4.glImports}/${r.g4.glTotal}`);
  if (r.g4.glMissing && r.g4.glMissing.length) {
    line(`           ${C.bad('GL entry points the official module imports and this one does not:')}`);
    line(`           ${r.g4.glMissing.slice(0, 8).join(', ')}${r.g4.glMissing.length > 8 ? ` … +${r.g4.glMissing.length - 8}` : ''}`);
  }
  line(`         calls into the host while driving ${Math.min(80, r.shared)} exports: ${r.g4.callsIntoHost} across ${r.g4.distinctImportsCalled} distinct imports`);
  if (r.g4.sample && r.g4.sample.length) {
    // Show the resolved name when the contract knows it: `a.xa x3` reads as noise, but
    // `a.xa (glDrawElements) x3` is the finding.
    const pretty = r.g4.sample.map((s) => s.replace(/(\S+) x(\d+)/, (_, k, n) => {
      const short = k.split('.').pop();
      const real = contract.supplied.get(short);
      return real && real !== short ? `${k} (${real.replace(/^_/, '')}) x${n}` : `${k} x${n}`;
    }));
    line(C.dim(`           e.g. ${pretty.join(', ')}`));
  }
  if (r.g4.error) line(`           ${C.bad(r.g4.error)}`);

  line();
  if (r.runnable) {
    line(`  ${C.ok('VERDICT: viable replacement')} -- speaks the loader contract and reaches for the host.`);
  } else if (r.logicFaithful && !r.dropIn) {
    line(`  ${C.warn('VERDICT: faithful but not a replacement')} -- the engine logic matches (gate 3), but the`);
    line(`  loader contract does not (gates 1/2). Useful as evidence the transpile is honest; it cannot`);
    line(`  be dropped into the official deployment.`);
  } else if (r.dropIn && !r.g4.pass) {
    line(`  ${C.warn('VERDICT: would load but would not render')} -- the ABI lines up, but it never reaches`);
    line(`  for GL, so nothing would be drawn.`);
  } else {
    line(`  ${C.bad('VERDICT: not a replacement')}`);
  }
}

async function main() {
  const opts = parseArgs(process.argv.slice(2));
  if (opts.help || opts.candidates.length === 0) {
    line('usage: node wasm-replace-test.mjs [--official <wasm>] [--glue <js>] [--strict] [--json] <candidate.wasm> [...]');
    line();
    line('  --official  the module to replace          (default: vendored official asyncify build)');
    line('  --glue      the JS loader it must satisfy  (default: vendored official asyncify glue)');
    line('  --strict    also fail on trap-kind-only mismatches');
    line('  --json      machine-readable output');
    process.exit(opts.candidates.length === 0 ? 2 : 0);
  }

  const contract = readGlueContract(opts.glue);
  const officialBytes = readFileSync(opts.official);
  const officialModule = await WebAssembly.compile(officialBytes);

  const results = [];
  for (const c of opts.candidates) results.push(await testCandidate(c, contract, officialBytes, officialModule, opts));

  if (opts.json) {
    line(JSON.stringify({ official: opts.official, glue: opts.glue, contract: {
      importModule: contract.importModule, supplied: contract.supplied.size, readOff: contract.readOff.size,
    }, results }, null, 2));
  } else {
    line();
    line(C.b(`official : ${opts.official}`));
    line(C.b(`glue     : ${opts.glue}`));
    line(C.dim(`contract : imports under "${contract.importModule}" (${contract.supplied.size} names), ` +
               `reads ${contract.readOff.size} names back off Module.asm`));
    for (const r of results) reportOne(r, contract);
  }

  const allGood = results.every((r) => r.runnable);
  process.exit(allGood ? 0 : 1);
}

main().catch((e) => { console.error(e); process.exit(2); });
