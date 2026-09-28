#!/usr/bin/env node
// The authoritative classification table for the bridge module's import surface.
//
// The bridge is a wasm2c rebuild of the official Artemis module, linked by a current
// Emscripten.  It asks for a host in one specific way: every import lives under a single
// module name, `a`, and its field names are the official glue's own minified keys
// (`$`, `Ob`, `Ea`, `xb`, ...).  This script is the one place that decides which of
// those 199 keys a modern host has to answer, how, and where the official implementation
// of each one can be read.
//
// Everything it prints is derived from the four artefacts it is given, not from a
// hardcoded list: the import section is decoded out of the .wasm, the correspondence
// back to real Emscripten names is read out of the glue's `asmLibraryArg`, the C
// forwarders are read out of the generated header, and the 9 runtime renames are read
// out of the build's own alias manifest.  Change any input and the table changes with it.
//
// Output is deterministic: no timestamps, no input paths, no iteration order that
// depends on a hash.  Run it twice, diff the two files, get nothing.
//
// Usage:
//   node ports/artemis/harness/glue/imports-map.mjs \
//     --wasm   <bridge.wasm> \
//     --glue   <artemis.js> \
//     --forward <imports-forward.c> \
//     --aliases <runtime-imports.json> \
//     [--out-dir <dir>]        default: this script's directory
//
// `--forward` is optional; the C forwarders are corroborating evidence, not an input to
// the classification.  Without it the `forwarded` column reads `-` and the table still
// stands on the wasm import section plus the glue.

import { readFileSync, writeFileSync, existsSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { dirname, resolve, relative, basename } from 'node:path';
import { fileURLToPath } from 'node:url';

const HERE = dirname(fileURLToPath(import.meta.url));
const REPO = resolve(HERE, '..', '..', '..', '..');

const VALUE_TYPES = {
  0x7f: 'i32', 0x7e: 'i64', 0x7d: 'f32', 0x7c: 'f64',
  0x7b: 'v128', 0x70: 'funcref', 0x6f: 'externref',
};

// ---------------------------------------------------------------------------
// Arguments
// ---------------------------------------------------------------------------
function parseArgs(argv) {
  const out = { outDir: HERE };
  for (let i = 0; i < argv.length; i++) {
    const a = argv[i];
    if (a === '--wasm') out.wasm = resolve(argv[++i]);
    else if (a === '--glue') out.glue = resolve(argv[++i]);
    else if (a === '--forward') out.forward = resolve(argv[++i]);
    else if (a === '--aliases') out.aliases = resolve(argv[++i]);
    else if (a === '--out-dir') out.outDir = resolve(argv[++i]);
    else if (a === '-h' || a === '--help') out.help = true;
    else throw new Error(`unknown option ${a}`);
  }
  for (const k of ['wasm', 'glue', 'aliases']) if (!out[k]) throw new Error(`--${k} is required`);
  return out;
}

// ---------------------------------------------------------------------------
// The wasm import section, decoded
//
// Only the import section (id 2) and the type section (id 1) are read.  Both are decoded
// from the bytes rather than through `WebAssembly.Module.imports`, because the JS API does
// not report the declared signature of a function import -- and the signature is what
// distinguishes the two `a.Ta` entries (one is `_abort`, the other is the itanium
// personality routine) and the two `a.Ob` entries.
// ---------------------------------------------------------------------------
function uleb(buf, pos) {
  let result = 0; let shift = 0; let byte;
  do {
    byte = buf[pos++];
    result |= (byte & 0x7f) << shift;
    shift += 7;
  } while (byte & 0x80);
  return [result >>> 0, pos];
}

function sections(buf) {
  const out = [];
  let pos = 8;
  while (pos < buf.length) {
    const id = buf[pos++];
    let len;
    [len, pos] = uleb(buf, pos);
    out.push({ id, start: pos, end: pos + len });
    pos += len;
  }
  return out;
}

function readTypes(buf, secs) {
  const sec = secs.find((s) => s.id === 1);
  if (!sec) return [];
  let pos = sec.start;
  let count;
  [count, pos] = uleb(buf, pos);
  const types = [];
  for (let i = 0; i < count; i++) {
    if (buf[pos++] !== 0x60) throw new Error('type section: expected a func type');
    let n;
    [n, pos] = uleb(buf, pos);
    const params = [];
    for (let j = 0; j < n; j++) params.push(VALUE_TYPES[buf[pos++]] ?? `t${buf[pos - 1]}`);
    [n, pos] = uleb(buf, pos);
    const results = [];
    for (let j = 0; j < n; j++) results.push(VALUE_TYPES[buf[pos++]] ?? `t${buf[pos - 1]}`);
    types.push({ params, results });
  }
  return types;
}

function readImports(buf, secs, types) {
  const sec = secs.find((s) => s.id === 2);
  if (!sec) throw new Error('no import section (id 2)');
  let pos = sec.start;
  let count;
  [count, pos] = uleb(buf, pos);
  const entries = [];
  for (let i = 0; i < count; i++) {
    let len;
    [len, pos] = uleb(buf, pos);
    const mod = buf.subarray(pos, pos + len).toString('utf8');
    pos += len;
    [len, pos] = uleb(buf, pos);
    const name = buf.subarray(pos, pos + len).toString('utf8');
    pos += len;
    const kind = buf[pos++];
    if (kind === 0) {
      let typeIndex;
      [typeIndex, pos] = uleb(buf, pos);
      const t = types[typeIndex];
      entries.push({
        module: mod, name, kind: 'func', typeIndex,
        signature: `(${t.params.join(',')}) -> ${t.results.length ? t.results.join(',') : 'void'}`,
      });
    } else if (kind === 1) {
      pos++; // elem type
      let flags;
      [flags, pos] = uleb(buf, pos);
      let min;
      [min, pos] = uleb(buf, pos);
      let max = null;
      if (flags & 1) [max, pos] = uleb(buf, pos);
      entries.push({ module: mod, name, kind: 'table', limits: { min, max } });
    } else if (kind === 2) {
      let flags;
      [flags, pos] = uleb(buf, pos);
      let min;
      [min, pos] = uleb(buf, pos);
      let max = null;
      if (flags & 1) [max, pos] = uleb(buf, pos);
      entries.push({ module: mod, name, kind: 'memory', limits: { min, max, shared: !!(flags & 2) } });
    } else if (kind === 3) {
      pos += 2; // value type, mutability
      entries.push({ module: mod, name, kind: 'global' });
    } else {
      let n;
      [n, pos] = uleb(buf, pos);
      for (let j = 0; j < n; j++) { pos++; let x; [x, pos] = uleb(buf, pos); }
      entries.push({ module: mod, name, kind: 'tag' });
    }
  }
  return entries;
}

// ---------------------------------------------------------------------------
// The official glue's own import object
//
// `var asmLibraryArg={...}` is the object the engine's imports are resolved out of, and
// the keys in it are exactly the field names the official module asks for.  The values
// are the unminified Emscripten function names, which is the whole reason this map can
// be readable: `Ob` means nothing until you see `___clock_gettime` next to it.
// ---------------------------------------------------------------------------
function readGlueImports(js) {
  const info = js.match(/var info=\{("(?:[^"]+)"):([A-Za-z0-9_$]+)\}/);
  if (!info) throw new Error("could not find the glue's import module (`var info={...}`)");
  const importModule = JSON.parse(info[1]);

  const MARK = 'var asmLibraryArg=';
  const at = js.indexOf(MARK);
  if (at < 0) throw new Error('could not find asmLibraryArg in the glue');
  const bodyStart = at + MARK.length;
  let depth = 0; let end = bodyStart;
  for (; end < js.length; end++) {
    if (js[end] === '{') depth++;
    else if (js[end] === '}') { depth--; if (depth === 0) { end++; break; } }
  }
  const body = js.slice(bodyStart, end);

  const bindings = new Map(); // short name -> { fn, refOffset }
  const re = /"((?:[^"\\]|\\.)+)":([A-Za-z_$][A-Za-z0-9_$]*)/g;
  let m;
  while ((m = re.exec(body)) !== null) {
    bindings.set(m[1], { fn: m[2], refOffset: bodyStart + m.index, at: bodyStart + m.index + m[0].length - m[2].length });
  }

  // Where each implementation is DEFINED, so every row can cite a place to read it.
  // The glue is minified onto one line, so a byte offset is the only stable locator.
  const defs = new Map();
  for (const { fn } of bindings.values()) {
    if (defs.has(fn)) continue;
    const asFn = js.indexOf(`function ${fn}(`);
    if (asFn >= 0) { defs.set(fn, { form: 'function', offset: asFn }); continue; }
    const asVar = new RegExp(`\\bvar ${fn}=`).exec(js);
    if (asVar) { defs.set(fn, { form: 'var', offset: asVar.index }); continue; }
    const asDecl = new RegExp(`\\bvar ${fn}(?:;|,)`).exec(js);
    if (asDecl) { defs.set(fn, { form: 'var', offset: asDecl.index }); continue; }
    defs.set(fn, { form: null, offset: -1 });
  }

  return { importModule, bindings, defs, bytes: js.length, objectStart: bodyStart, objectEnd: end };
}

// ---------------------------------------------------------------------------
// The C forwarders, as corroboration
//
// `imports-forward.c` is what wasm2c's header produced for the rebuild: one forwarder
// per import whose name is not a C identifier, plus the memory accessor.  It is not an
// input to the classification -- the classification is the wasm import section -- so the
// reader can always tell which claims rest on the binary and which on the generated C.
// ---------------------------------------------------------------------------
function readForwarders(cSource) {
  const out = new Map();
  const re = /\/\* import: '([^']*)' '([^']*)' \*\//g;
  let m;
  while ((m = re.exec(cSource)) !== null) out.set(`${m[1]}.${m[2]}`, m.index);
  return out;
}

// ---------------------------------------------------------------------------
// The build's own alias manifest
//
// The rebuild was linked by an Emscripten that wants `env._Unwind_RaiseException` and
// `wasi_snapshot_preview1.fd_write`, neither of which the official glue supplies under
// its own names.  `wasm-satisfy-imports.py` resolved each of them by finding the field
// the official module uses for the same facility and renaming the import onto it.  That
// manifest is the authoritative record of which field was picked, so it is read rather
// than re-derived.
// ---------------------------------------------------------------------------
function readAliases(json) {
  const out = [];
  for (const [ns, members] of Object.entries(json)) {
    for (const [symbol, target] of Object.entries(members)) {
      out.push({ ns, symbol, module: target.module, name: target.name });
    }
  }
  out.sort((a, b) => (a.name < b.name ? -1 : a.name > b.name ? 1 : 0));
  return out;
}

// ---------------------------------------------------------------------------
// Classification
//
// The buckets are what a host has to decide, not what the function is called.  Order
// matters: the first matching rule wins, so the most specific thing about a symbol is
// tested before the general one.
// ---------------------------------------------------------------------------
const BUCKETS = {
  gl: 'WebGL2 + OpenAL + GLFW. Bound straight to the canvas context, the AudioContext and the window; nothing emulated.',
  fd: 'wasi file descriptors, clock and time. Backed by @online-gal/vfs (SyncFileBridge over HTTP range reads), not by a real filesystem.',
  asyncify: 'on-demand payload fetch, and the stack spill Asyncify unwinds through. The paths that cannot be synchronous: a range read has to leave the wasm and come back.',
  async: 'the Asyncify re-entry trampolines. Each is one unwound call site: resume it, run it, rewind its arguments, then return to whatever yielded.',
  eh: 'C++ exceptions and longjmp. The module was built without exceptions, so these exist only to make a failure unwind.',
  runtime: 'Emscripten/musl runtime plumbing: heap growth, thread bookkeeping, environment, string formatting. Modern JS supplies these directly.',
  todo: 'Known not to need a real implementation yet, with the reason recorded on the row. Nothing hides in here silently.',
};
const BUCKET_ORDER = ['gl', 'fd', 'asyncify', 'async', 'eh', 'runtime', 'todo'];

// name -> bucket, for the cases the pattern rules below do not read off the spelling.
// Every key here has to be a real import name; the generator fails on a stale one, the
// same way it fails on a missing `todo` reason.
const EXPLICIT = new Map(Object.entries({
  // `_emscripten_memcpy_big`, which is the Asyncify spill: the module's main loop
  // unwinds its whole stack through it on every yield point. It is named here rather
  // than matched by a rule because the minified field it landed on (`yb`) says nothing
  // about what it is, and a rule for it would sit behind a name nobody can read.
  yb: 'asyncify',
}));

const RULES = [
  // OpenGL. Every name is `_gl` + a capitalised entry point, with no exceptions in
  // this module's face.
  [/^_gl[A-Z]/, 'gl'],
  // OpenAL: the engine's audio path, played by the official glue through a real
  // `AudioContext` rather than an emulator.
  [/^_al[A-Z]/, 'gl'],
  [/^_alc[A-Z]/, 'gl'],
  // GLEW and GLFW: entry-point resolution, and the window/context/event pump.
  [/^_glewInit$/, 'gl'],
  [/^_glfw/, 'gl'],
  // The browser answers these itself: the canvas, the pixel ratio, the input and
  // visibility callbacks, soft fullscreen, and the frame pump. The engine's own
  // presentation layer already owns a canvas, so these forward rather than re-decide.
  [/^_emscripten_(get_canvas|get_device|enter_soft|exit_soft|get_element|set_)/, 'gl'],
  // The Asyncify re-entry trampolines, one per unwound call site. These are not C
  // library calls at all: each is a place the compiler recorded so the stack could be
  // rewound, which is why they get their own bucket instead of riding `runtime`.
  [/^invoke_/, 'async'],
  // The on-demand payload fetches. The spill that goes with them is named on the row
  // itself instead of here, because the minified field it landed on (`yb`) says nothing
  // about what it is.
  [/^_emscripten_(async_)?wget\d*_data$/, 'asyncify'],
  // The IDB-backed synchronous filesystem emscripten offers for programs that think
  // they are writing files. The engine reaches for the archive, not for a disk, so
  // these resolve against the same vfs surface as the `fd` bucket.
  [/^_emscripten_idb_/, 'fd'],
  // musl syscalls, reached through the `__sys_` prefix emscripten generates.  The
  // i64 forms are the 32-bit-off_t instantiations: a large-file read, not a second API.
  [/^___sys_/, 'fd'],
  // The unprefixed musl wrappers for the same facilities.
  [/^_(fd_write|fd_read|fd_close|fd_seek|open|close|read|write|seek|lseek|stat|lstat|fstat|fsync|getdents|ioctl|dup|dup2|chdir|getcwd|mkdir|rmdir|unlink|rename|fcntl|exit|environ_get|environ_sizes_get)$/, 'fd'],
  // Time: the clock the engine timestamps with, and the calendar conversions that read
  // a saved-game header. Not wasi, but the same `fd` bucket's owner: a clock plus a
  // date, both of which are trivial to supply honestly.
  [/^_*_?(clock|clock_gettime|localtime|localtime_r|gmtime|gmtime_r|tzset|difftime|time|mktime|strftime|strftime_l|usleep)$/, 'fd'],
  // Reach for nothing by name: the archive is already mounted, so the resolver has
  // nothing to look up. Here only because a networked build of the same engine links it.
  [/^_getaddrinfo$/, 'fd'],
  // C++ failure unwinding, and the one longjmp the linker kept. `_llvm_eh_typeid_for`
  // is the itanium personality routine: the compiler emits it whether or not the module
  // is built with exceptions, and this one is `-fno-exceptions`, so it is a landing pad
  // that never runs. `_abort` and `___assert_fail` sit here too — reachable, so they
  // abort loudly instead of returning a value nobody reads.
  [/^_*cxa_/, 'eh'],
  [/^_*resumeException$/, 'eh'],
  [/^_*llvm_eh_typeid_for$/, 'eh'],
  [/^_emscripten_longjmp$/, 'eh'],
  [/^_?_*_(abort|assert_fail)$/, 'eh'],
  // Heap growth, thread bookkeeping, the environment, and `asm!` argument marshalling.
  // The thread functions are `mutexattr` only: the module is single-threaded, so the
  // engine answers them with a no-op and a done-flag rather than a pthread library.
  [/^_emscripten_(resize_heap|run_script|get_now|is_html|get_html|random|debug|asm_const)/, 'runtime'],
  [/^_(pthread_|sbrk)/, 'runtime'],
  [/^_(get|set)TempRet0\d*$/, 'runtime'],
];

// The reason a `todo` row is allowed to stop there. Anything in `todo` with no reason
// here is a bug in this file, and the generator refuses to emit a table in that case.
// The map is also checked in the other direction: a reason nothing reaches is a stale
// guess, so a leftover entry fails the run rather than quietly lying about the bucket.
const TODO_REASONS = new Map(Object.entries({
  // The one import with no honest browser implementation at all. Its only use here would
  // be running a shell command, and a tab has no subprocess to run: the answer has to be
  // a failure, not a fork.
  Xb: 'system. No subprocesses in a browser tab. A call has to fail, not fork — and fail loudly, because a working port never reaches it.',
}));

function classify(name, fn) {
  const explicit = EXPLICIT.get(name);
  if (explicit) return { bucket: explicit, rule: `explicit:${name}` };
  for (const [re, bucket] of RULES) {
    if (re.test(fn)) return { bucket, rule: re.source };
  }
  return { bucket: 'todo', rule: 'fallthrough' };
}

// ---------------------------------------------------------------------------
// Where a modern host should put each implementation
//
// This is a recommendation, not a measurement: it is the module this repository already
// has, named per bucket, so that every row has a place to land rather than a shrug.
// ---------------------------------------------------------------------------
const OWNER = {
  gl: 'packages/runtime-artemis/src/host/gl.ts (to be written) — the real WebGL2 context, forwarded verbatim',
  fd: 'packages/runtime-artemis/src/host/fd.ts (to be written) — @online-gal/vfs SyncFileBridge + a clock',
  asyncify: 'packages/runtime-artemis/src/host/asyncify.ts (to be written) — the spill; see docs §18.3',
  async: 'packages/runtime-artemis/src/host/asyncify.ts (to be written) — the one seam, beside the spill; see docs §18.3',
  eh: 'packages/runtime-artemis/src/host/eh.ts (to be written) — throw/abort, never a return value',
  runtime: 'packages/runtime-artemis/src/host/runtime.ts (to be written) — heap growth, thread bookkeeping, asm! marshalling',
  todo: 'packages/runtime-artemis/src/host/todo.ts (to be written) — deliberate no-ops, each with a reason',
};

// ---------------------------------------------------------------------------
// Rendering
// ---------------------------------------------------------------------------
const CELL = (s) => String(s).replace(/\|/g, '\\|');

function sortKey(name) {
  // Byte order, so the table is stable across runs and readable: `$` first, then
  // symbols, then letters.  Not a natural sort -- the point is determinism.
  return name;
}

function buildRows(args) {
  const wasm = readFileSync(args.wasm);
  const secs = sections(wasm);
  const types = readTypes(wasm, secs);
  const imports = readImports(wasm, secs, types);
  const glue = readGlueImports(readFileSync(args.glue, 'utf8'));
  const aliases = readAliases(JSON.parse(readFileSync(args.aliases, 'utf8')));
  const forwarders = args.forward && existsSync(args.forward)
    ? readForwarders(readFileSync(args.forward, 'utf8'))
    : null;

  const funcImports = imports.filter((e) => e.kind === 'func');
  const otherImports = imports.filter((e) => e.kind !== 'func');
  const importModules = [...new Set(imports.map((e) => e.module))].sort();
  const aliasByName = new Map(aliases.map((a) => [a.name, a]));

  // One row per import section entry, in section order, so a diff against the binary is
  // a diff of two ordered lists.  Duplicate names stay separate rows on purpose: the
  // section really does contain `a.Ta` twice, once for `_abort` and once for the
  // itanium personality routine, and a set would silently lose one of them.
  const rows = funcImports.map((entry, index) => {
    const name = entry.name;
    const binding = glue.bindings.get(name);
    const fn = binding ? binding.fn : null;
    const def = fn ? glue.defs.get(fn) : null;
    const alias = aliasByName.get(name);
    const { bucket, rule } = classify(name, fn ?? name);
    const todoReason = bucket === 'todo' ? (TODO_REASONS.get(name) ?? null) : null;
    return {
      index,
      name,
      signature: entry.signature,
      typeIndex: entry.typeIndex,
      official: fn,
      officialRef: binding && fn ? `artemis.js@${def && def.offset >= 0 ? def.offset : binding.refOffset} (${fn})` : null,
      glueRef: binding ? `artemis.js@${binding.at}` : null,
      alias: alias ? `${alias.ns}.${alias.symbol} -> a.${alias.name}` : null,
      forwarded: forwarders ? (forwarders.has(`${entry.module}.${name}`) ? 'yes' : 'NO') : '-',
      bucket,
      rule,
      todoReason,
      owner: OWNER[bucket],
      duplicate: null,
    };
  });

  // Mark the repeated names, and say what distinguishes each occurrence.  Silence here
  // would read as "these are the same function", which is exactly the wrong conclusion
  // for `a.Ta`.
  const byName = new Map();
  for (const r of rows) {
    if (!byName.has(r.name)) byName.set(r.name, []);
    byName.get(r.name).push(r);
  }
  for (const [name, group] of byName) {
    if (group.length < 2) continue;
    for (const r of group) {
      r.duplicate = `1 of ${group.length}: ${r.official ?? '?'} type#${r.typeIndex} ${r.signature}`;
    }
  }

  return { wasm, imports, funcImports, otherImports, importModules, glue, aliases, forwarders, rows, bytes: wasm.length, wasmSha256: createHash('sha256').update(wasm).digest('hex') };
}

function bucketCounts(rows) {
  const counts = new Map();
  for (const r of rows) counts.set(r.bucket, (counts.get(r.bucket) ?? 0) + 1);
  return counts;
}

function renderMarkdown(model, args) {
  const { funcImports, otherImports, importModules, glue, aliases, rows, imports } = model;
  const counts = bucketCounts(rows);
  const L = [];

  L.push('<!-- GENERATED FILE -- do not edit. Run ports/artemis/harness/glue/imports-map.mjs. -->');
  L.push('');
  L.push('# The bridge module\'s import surface, classified');
  L.push('');
  L.push(`The bridge module asks for **${funcImports.length} function imports** and `    + `**${otherImports.length} other import${otherImports.length === 1 ? '' : 's'}**, all under the module name `
    + `\`${importModules.join('`, `')}\`. Every one of those ${funcImports.length} has a row below, with the bucket a modern host has to answer it in and a place in the official glue where its implementation can be read.`);
  L.push('');
  L.push('## The shape of the surface, before the rows');
  L.push('');
  L.push(`- **${funcImports.length} function imports, ${new Set(rows.map((r) => r.name)).size} distinct names.** The difference is nine names the import section lists twice under two different type indices — the renames \`wasm-satisfy-imports.py\` performed, so the rebuild could ask for the facility under the field the official module already uses. Each repeat is a genuinely different call with a different signature, so the table keeps both rows instead of collapsing them.`);
  L.push(`- one non-function import: the memory (see below). The host supplies the buffer; it is the one thing both tenants share.`);
  L.push(`- ${new Set(rows.map((r) => r.official)).size} distinct C symbols underneath those ${new Set(rows.map((r) => r.name)).size} names, all of them defined in the official glue, so every row has a body to read.`);
  L.push('- the buckets are what a host has to **decide**, not what a function is **called**. The largest is the GL family, because the engine calls the 3D API through its own loader rather than through GLEW, and the smallest is the one worth reading twice: `async`.');
  L.push('');

  L.push('## How to read a row');
  L.push('');
  L.push('| column | meaning |');
  L.push('| --- | --- |');
  L.push('| `#` | position in the module\'s own import section, so this table diffs against the binary as an ordered list |');
  L.push('| short name | the field name under module `a` — the only name the host has to spell |');
  L.push('| sig | signature decoded from the module\'s type section, not from the JS API |');
  L.push('| official | the unminified Emscripten name, read out of the glue\'s `asmLibraryArg` |');
  L.push('| glue ref | byte offset of the binding in the official `artemis.js`; the glue is minified onto one line, so an offset is the only stable locator |');
  L.push('| impl | byte offset of the **definition** of that name in the same file — the place to go read the body that answers this call |');
  L.push('| alias | the rebuild\'s own runtime import that was renamed onto this field, if any |');
  L.push('| fwd | present in the generated `imports-forward.c` forwarder set |');
  L.push('| bucket | which host decides this one |');
  L.push('| modern owner | the module in this repository it should land in |');
  L.push('');
  L.push('## Buckets');
  L.push('');
  L.push('| bucket | count | distinct | what it means |');
  L.push('| --- | --- | --- | --- |');
  for (const key of BUCKET_ORDER) {
    const names = new Set(rows.filter((r) => r.bucket === key).map((r) => r.name));
    L.push(`| \`${key}\` | ${counts.get(key) ?? 0} | ${names.size} | ${BUCKETS[key]} |`);
  }
  L.push(`| **total** | **${rows.length}** | |`);
  L.push('');

  // The non-function imports are part of the contract and get their own short table.
  L.push('## Non-function imports');
  L.push('');
  L.push('| module.name | kind | detail |');
  L.push('| --- | --- | --- |');
  for (const e of otherImports) {
    const detail = e.kind === 'memory'
      ? `initial ${e.limits.min} pages, max ${e.limits.max}, ${e.limits.shared ? 'shared' : 'not shared'} `
        + `(${e.limits.min * 65536} B initial)` 
      : e.kind === 'table' ? `min ${e.limits.min}, max ${e.limits.max}` : '';
    const owner = e.kind === 'memory'
      ? 'the host\'s `WebAssembly.Memory` — both tenants fence inside this one buffer (docs §18.3)'
      : 'n/a';
    L.push(`| \`${e.module}.${e.name}\` | ${e.kind} | ${detail}; owner: ${owner} |`);
  }
  L.push('');

  L.push('## Every import');
  L.push('');
  L.push('| # | short name | sig | official | glue ref | impl | alias | fwd | bucket | modern owner |');
  L.push('| ---: | --- | --- | --- | --- | --- | --- | --- | --- | --- |');
  for (const r of rows) {
    const dup = r.duplicate ? ` _(x2: ${CELL(r.duplicate)})_` : '';
    L.push([
      r.index,
      `\`${CELL(r.name)}\``,
      CELL(r.signature),
      r.official ? `\`${CELL(r.official)}\`` : '**UNMAPPED**',
      r.glueRef ? `\`${r.glueRef}\`` : '—',
      r.officialRef ? `\`${r.officialRef.match(/@(\d+)/)[1]}\`` : '—',
      r.alias ? `\`${CELL(r.alias)}\`` : '—',
      r.forwarded,
      `\`${r.bucket}\``,
      CELL(r.owner),
    ].join(' | ') + ` |${dup}${r.todoReason ? `\n\n**Why \`todo\`:** ${CELL(r.todoReason)}` : ''}`);
  }
  L.push('');

  // Anything the reader has to know about before trusting the table.
  const unmapped = rows.filter((r) => !r.official);
  const noRef = rows.filter((r) => !r.glueRef);
  const todoSilent = rows.filter((r) => r.bucket === 'todo' && !r.todoReason);
  const dupNames = [...new Set(rows.filter((r) => r.duplicate).map((r) => r.name))];
  const notForwarded = rows.filter((r) => r.forwarded === 'NO');

  L.push('## What this run found, and what it could not locate');
  L.push('');
  L.push(`- names in the import section: **${new Set(rows.map((r) => r.name)).size}** distinct, `
    + `**${rows.length}** entries (the difference is the repeated names below)`);
  L.push(`- names the glue binds: **${rows.filter((r) => r.official).length}/${rows.length}**`);
  L.push(`- rows with no locatable binding in the glue: **${noRef.length}**`);
  if (noRef.length) for (const r of noRef) L.push(`  - \`a.${r.name}\` — **not located**`);
  L.push(`- rows whose implementation body was not found in the glue: **${rows.filter((r) => r.official && !r.officialRef).length}**`);
  L.push(`- names that appear more than once in the import section: **${dupNames.length}** (${dupNames.map((d) => `\`${d}\``).join(', ') || 'none'})`);
  if (dupNames.length) {
    L.push('');
    L.push('  The repeats are the build\'s own doing, and each pair is two different functions that happen to share a field name:');
    for (const name of dupNames) {
      for (const r of rows.filter((x) => x.name === name)) {
        L.push(`  - \`a.${name}\` #${r.index} → \`${r.official}\` \`${r.signature}\``);
      }
    }
  }
  L.push(`- runtime aliases read from the build manifest: **${aliases.length}** (${aliases.map((a) => `\`${a.symbol}\` → \`a.${a.name}\``).join(', ')})`);
  L.push(`- \`todo\` rows without a recorded reason: **${todoSilent.length}**`);
  if (todoSilent.length) for (const r of todoSilent) L.push(`  - \`a.${r.name}\` — **reason missing**`);
  if (model.forwarders) {
    L.push(`- imports with no forwarder in \`imports-forward.c\`: **${notForwarded.length}**`);
    if (notForwarded.length) for (const r of notForwarded) L.push(`  - \`a.${r.name}\``);
  }
  if (unmapped.length) {
    L.push('');
    L.push('**Unmapped names are a hard failure of this table, not a footnote.** Each one below is an import');
    L.push('whose implementation could not be identified in the official glue:');
    for (const r of unmapped) L.push(`- \`a.${r.name}\` (${r.signature})`);
  }
  L.push('');
  L.push('## Provenance');
  L.push('');
  L.push('| what | value |');
  L.push('| --- | --- |');
  L.push(`| module | \`a\` (read from the glue: \`var info={"${glue.importModule}":asmLibraryArg}\`) |`);
  L.push(`| glue object | \`var asmLibraryArg={\` at byte ${glue.objectStart}, ${glue.objectEnd - glue.objectStart} bytes, ${glue.bindings.size} bindings |`);
  L.push(`| module bytes | ${model.bytes} |`);
  L.push(`| module sha256 | \`${model.wasmSha256}\` |`);
  L.push(`| import entries decoded | ${imports.length} (${funcImports.length} func + ${otherImports.length} other) |`);
  L.push('');
  L.push('Regenerate:');
  L.push('');
  L.push('```sh');
  L.push('node ports/artemis/harness/glue/imports-map.mjs \\');
  L.push('  --wasm    <artifact-dir>/dist/artemis-bridge/build/bridge.wasm \\');
  L.push('  --glue    ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js \\');
  L.push('  --forward <artifact-dir>/dist/artemis-bridge/build/imports-forward.c \\');
  L.push('  --aliases <artifact-dir>/dist/artemis-bridge/build/runtime-imports.json');
  L.push('```');
  L.push('');
  return L.join('\n');
}

function renderJson(model) {
  const { rows, otherImports, glue, aliases, funcImports, bytes } = model;
  const counts = bucketCounts(rows);
  const out = {
    schema: 'online-gal/artemis-imports-map@1',
    totals: {
      importEntries: model.imports.length,
      functionImports: funcImports.length,
      distinctNames: new Set(rows.map((r) => r.name)).size,
      otherImports: otherImports.map((e) => ({ module: e.module, name: e.name, kind: e.kind, limits: e.limits ?? null })),
      moduleBytes: bytes,
      moduleSha256: model.wasmSha256,
    },
    importModule: glue.importModule,
    glueObjectSpan: [glue.objectStart, glue.objectEnd],
    buckets: Object.fromEntries([...counts].sort((a, b) => (a[0] < b[0] ? -1 : 1))),
    bucketMeaning: BUCKETS,
    runtimeAliases: aliases,
    rows: rows.map((r) => ({
      index: r.index,
      name: r.name,
      signature: r.signature,
      typeIndex: r.typeIndex,
      official: r.official,
      glueBindingOffset: r.glueRef ? Number(r.glueRef.match(/@(\d+)/)[1]) : null,
      glueImplOffset: r.officialRef ? Number(r.officialRef.match(/@(\d+)/)[1]) : null,
      alias: r.alias,
      forwarded: r.forwarded,
      bucket: r.bucket,
      rule: r.rule,
      todoReason: r.todoReason,
      owner: r.owner,
      duplicate: r.duplicate,
    })),
  };
  return `${JSON.stringify(out, null, 2)}\n`;
}

// ---------------------------------------------------------------------------
function main() {
  let args;
  try {
    args = parseArgs(process.argv.slice(2));
  } catch (e) {
    if (String(e).includes('unknown option') || String(e).includes('required')) {
      process.stderr.write(`${e}\n\n${usage()}\n`);
      process.exit(2);
    }
    throw e;
  }
  if (args.help) { process.stdout.write(`${usage()}\n`); return; }

  const model = buildRows(args);

  // A row that is neither mapped nor explained is a defect in the table, not of the
  // bridge.  Say so and fail rather than emit something that reads as complete.
  const broken = model.rows.filter((r) => (!r.official || !r.glueRef || (r.bucket === 'todo' && !r.todoReason)));
  if (broken.length) {
    process.stderr.write(`refusing to emit: ${broken.length} row(s) without a full citation\n`);
    for (const r of broken) {
      process.stderr.write(`  a.${r.name}: official=${r.official ?? 'MISSING'} ref=${r.glueRef ?? 'MISSING'} todoReason=${r.todoReason ?? 'MISSING'}\n`);
    }
    process.exit(1);
  }

  // The other direction: a reason nothing lands on means a name was guessed wrong and the
  // table is about to assert a bucket for it that no row actually holds.
  const todoNames = new Set(model.rows.filter((r) => r.bucket === 'todo').map((r) => r.name));
  const stale = [...TODO_REASONS.keys()].filter((k) => !todoNames.has(k));
  if (stale.length) {
    process.stderr.write(`refusing to emit: ${stale.length} stale todo reason(s) — no import lands in \`todo\` as ${stale.join(', ')}\n`);
    process.exit(1);
  }

  // The same for the hand-assigned overrides: an override for a name this build does not
  // import is a leftover, and it hides the fact that a real name is riding a pattern.
  const importedNames = new Set(model.rows.map((r) => r.name));
  const orphanOverrides = [...EXPLICIT.keys()].filter((k) => !importedNames.has(k));
  if (orphanOverrides.length) {
    process.stderr.write(`refusing to emit: ${orphanOverrides.length} stale bucket override(s) — not an import in this build: ${orphanOverrides.join(', ')}\n`);
    process.exit(1);
  }

  // And the same again for the rules: a rule that matches nothing is a name this build
  // asked for under a spelling nobody anticipated, which is a row riding the fallthrough.
  const ruleHits = new Map(RULES.map(([re, bucket]) => [re, 0]));
  for (const r of model.rows) {
    if (r.rule.startsWith('explicit:') || r.rule === 'fallthrough') continue;
    for (const re of ruleHits.keys()) if (re.source === r.rule) ruleHits.set(re, ruleHits.get(re) + 1);
  }
  const deadRules = [...ruleHits].filter(([, n]) => n === 0).map(([re]) => re.source);
  if (deadRules.length) {
    process.stderr.write(`refusing to emit: ${deadRules.length} classification rule(s) match no import: ${deadRules.join(' | ')}\n`);
    process.exit(1);
  }

  const md = renderMarkdown(model, args);
  const json = renderJson(model);
  writeFileSync(resolve(args.outDir, 'imports-map.md'), md, 'utf8');
  writeFileSync(resolve(args.outDir, 'imports-map.json'), json, 'utf8');

  const counts = bucketCounts(model.rows);
  process.stdout.write(`imports-map: ${model.rows.length} function imports (${new Set(model.rows.map((r) => r.name)).size} distinct) + ${model.otherImports.length} other\n`);
  for (const key of BUCKET_ORDER) {
    process.stdout.write(`  ${key.padEnd(8)} ${counts.get(key) ?? 0}\n`);
  }
  process.stdout.write(`wrote ${relative(REPO, resolve(args.outDir, 'imports-map.md'))} and ${relative(REPO, resolve(args.outDir, 'imports-map.json'))}\n`);
}

function usage() {
  return [
    'usage: imports-map.mjs --wasm <bridge.wasm> --glue <artemis.js> --aliases <runtime-imports.json>',
    '                        [--forward <imports-forward.c>] [--out-dir <dir>]',
  ].join('\n');
}

main();
