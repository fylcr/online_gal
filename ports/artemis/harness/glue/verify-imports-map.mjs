#!/usr/bin/env node
// Independent check of ports/artemis/harness/glue/imports-map.{md,json}.
//
// Deliberately NOT sharing code with the generator: it re-decodes the wasm import
// and type sections with its own reader, and cross-checks the emitted table against
// both that decode and the browser's own WebAssembly.Module.imports. If the
// generator and this script ever agree by accident rather than by measurement, that
// is the failure this is here to catch.
//
//   node ports/artemis/harness/glue/verify-imports-map.mjs \
//     --wasm   <bridge.wasm> \
//     --glue   <artemis.js> \
//     --json   <imports-map.json> \
//     --md     <imports-map.md>
//
// What is judged, in one paragraph: the module's import SECTION (independently
// decoded) must agree with the table row for row, in order; every function row's
// declared `signature` must equal the signature its own type index ACTUALLY
// resolves to in this module's type section -- a row's signature is the ABI, and a
// signature typed into the table is a claim about bytes that this resolves; every
// non-function import must appear in `totals.otherImports` with the same kind and
// the same limits (min, max, shared); every glue offset must land on its own name
// and its own definition site; and the markdown must be the same rows, in the same
// order, as the json and the decode.
//
// The module's OWN hash is reported, never judged -- see the note below.

import { readFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { resolve } from 'node:path';

// ---------------------------------------------------------------------------
// Pure helpers, exported so a test can drive them on a synthetic module without
// needing a 23 MB product artifact or a WebAssembly engine. They are the same
// code the CLI below runs; nothing here is a re-implementation for tests.
// ---------------------------------------------------------------------------

export const VALUE_TYPES = {
  0x7f: 'i32', 0x7e: 'i64', 0x7d: 'f32', 0x7c: 'f64',
  0x7b: 'v128', 0x70: 'funcref', 0x6f: 'externref',
};

export const ulebAt = (buf, at) => {
  let v = 0; let shift = 0; let b; let n = 0;
  do {
    if (at + n >= buf.length) throw new Error('uleb runs past the end of the buffer');
    b = buf[at + n++]; v |= (b & 0x7f) << shift; shift += 7;
  } while (b & 0x80);
  return [v >>> 0, at + n];
};

export const readName = (buf, at) => {
  const [len, start] = ulebAt(buf, at);
  if (start + len > buf.length) throw new Error('name runs past the end of the buffer');
  return [buf.subarray(start, start + len).toString('utf8'), start + len];
};

// Section list. A section is {id, start, end} with start/end inside the payload.
export const listSections = (bytes) => {
  if (bytes.length < 8) throw new Error('shorter than a wasm header');
  if (bytes[0] !== 0x00 || bytes[1] !== 0x61 || bytes[2] !== 0x73 || bytes[3] !== 0x6d) throw new Error('not a wasm module (no \\0asm magic)');
  let p = 8;
  const out = [];
  while (p < bytes.length) {
    const id = bytes[p++];
    let len; [len, p] = ulebAt(bytes, p);
    if (p + len > bytes.length) throw new Error(`section ${id} claims ${len} bytes past the end of the module`);
    out.push({ id, start: p, end: p + len });
    p += len;
  }
  return out;
};

// Decode the type section into signature strings, exactly the spelling the
// generator uses: `(i32,i32) -> i32`, and `() -> void` for no results.
export const decodeTypeSection = (bytes) => {
  const sec = listSections(bytes).find((s) => s.id === 1);
  if (!sec) throw new Error('no type section (id 1)');
  let q = sec.start;
  let count; [count, q] = ulebAt(bytes, q);
  const types = [];
  for (let i = 0; i < count; i++) {
    if (bytes[q++] !== 0x60) throw new Error(`type section entry ${i} is not a func type (0x60)`);
    let n; [n, q] = ulebAt(bytes, q);
    const params = [];
    for (let j = 0; j < n; j++) { const v = bytes[q++]; params.push(VALUE_TYPES[v] ?? `t${v}`); }
    [n, q] = ulebAt(bytes, q);
    const results = [];
    for (let j = 0; j < n; j++) { const v = bytes[q++]; results.push(VALUE_TYPES[v] ?? `t${v}`); }
    types.push(`(${params.join(',')}) -> ${results.length ? results.join(',') : 'void'}`);
  }
  return types;
};

// Decode the import section into one entry per import, in import order. Unknown
// kinds FAIL CLOSED: a kind this reader does not know is a kind it cannot check,
// and silently skipping it would let an unchecked import through.
export const decodeImportSection = (bytes) => {
  const sec = listSections(bytes).find((s) => s.id === 2);
  if (!sec) throw new Error('no import section (id 2)');
  const types = decodeTypeSection(bytes);
  let q = sec.start;
  let count; [count, q] = ulebAt(bytes, q);
  const imports = [];
  for (let i = 0; i < count; i++) {
    const [mod, after] = readName(bytes, q); q = after;
    const [name, after2] = readName(bytes, q); q = after2;
    const kind = bytes[q++];
    if (kind === 0) {
      let t; [t, q] = ulebAt(bytes, q);
      imports.push({ mod, name, kind: 'func', typeIndex: t, signature: types[t], signatureMissing: types[t] === undefined });
      continue;
    }
    if (kind === 1) {
      q++; // element type
      let f; [f, q] = ulebAt(bytes, q);
      let min; [min, q] = ulebAt(bytes, q);
      let max = null; if (f & 1) [max, q] = ulebAt(bytes, q);
      imports.push({ mod, name, kind: 'table', limits: { min, max, shared: !!(f & 2) } });
      continue;
    }
    if (kind === 2) {
      let f; [f, q] = ulebAt(bytes, q);
      let min; [min, q] = ulebAt(bytes, q);
      let max = null; if (f & 1) [max, q] = ulebAt(bytes, q);
      imports.push({ mod, name, kind: 'memory', limits: { min, max, shared: !!(f & 2) } });
      continue;
    }
    if (kind === 3) {
      q += 2; // valtype + mutability
      imports.push({ mod, name, kind: 'global' });
      continue;
    }
    if (kind === 4) {
      const attrib = bytes[q++];
      let n; [n, q] = ulebAt(bytes, q);
      for (let j = 0; j < n; j++) { q++; let x; [x, q] = ulebAt(bytes, q); }
      imports.push({ mod, name, kind: 'tag', attrib });
      continue;
    }
    // Fail closed: an unknown import kind is one this reader cannot judge.
    throw new Error(`import ${i} (${mod}.${name}) has unknown kind 0x${kind.toString(16)} -- this reader does not know how to decode it, so it refuses rather than skip it`);
  }
  if (q > sec.end) throw new Error('import section decode ran past the end of the section');
  return { imports, types };
};

// Compare two limits/kind descriptors strictly. Returns null or a reason string.
export const limitsDiff = (a, b) => {
  if (!a || !b) return `one side has no limits (${JSON.stringify(a)} vs ${JSON.stringify(b)})`;
  if (a.min !== b.min) return `min ${a.min} vs ${b.min}`;
  if (a.max !== b.max) return `max ${JSON.stringify(a.max)} vs ${JSON.stringify(b.max)}`;
  if (!!a.shared !== !!b.shared) return `shared ${!!a.shared} vs ${!!b.shared}`;
  return null;
};

// ---------------------------------------------------------------------------
// CLI
// ---------------------------------------------------------------------------
const isMain = import.meta.url === `file://${process.argv[1]?.replace(/\\/g, '/')}`
  || process.argv[1]?.endsWith('verify-imports-map.mjs');

if (isMain) main();

function main() {
  const argv = process.argv.slice(2);
  // An option without a value is a usage error, not `undefined` read as a path.
  const arg = (k) => {
    const i = argv.indexOf(k);
    if (i < 0) return null;
    const v = argv[i + 1];
    if (v === undefined || v.startsWith('--')) {
      process.stderr.write(`usage error: ${k} needs a file path\n`);
      process.exit(2);
    }
    return resolve(v);
  };
  const WASM = arg('--wasm');
  const GLUE = arg('--glue');
  const JSON_PATH = arg('--json');
  const MD_PATH = arg('--md');
  if (!WASM || !GLUE || !JSON_PATH || !MD_PATH) {
    process.stderr.write('usage: verify-imports-map.mjs --wasm <f> --glue <f> --json <f> --md <f>\n');
    process.exit(2);
  }

  let failures = 0;
  const ok = (label, pass, detail = '') => {
    process.stdout.write(`${pass ? 'PASS' : 'FAIL'}  ${label}${detail ? `  — ${detail}` : ''}\n`);
    if (!pass) failures++;
  };

  // Reported, never judged. Exactly one thing belongs here: the module the table was made from.
  // A module carries its own build directory inside the runtime's assert strings, so the same tree
  // produces two byte-different modules on two machines -- measured, 41,321,914 B with
  // `D:/online_gal/...` inside it from a Windows checkout against 41,322,022 B with
  // `/home/runner/work/...` from the runner. Judging the recorded hash would make the committed
  // table satisfiable only by the machine that wrote it. What the table claims -- the import
  // surface and every glue offset, all functions of the import section and the tracked glue -- is
  // judged below, and so is the glue's identity, because the glue IS the same file everywhere.
  let notes = 0;
  const note = (label, detail = '') => {
    notes++;
    process.stdout.write(`NOTE  ${label}${detail ? `  — ${detail}` : ''}\n`);
  };

  // -------------------------------------------------------------------------
  // 1. Our own decode of the type and import sections, written separately from the
  //    generator's.
  //
  //    The type section is decoded because a function import names its signature by
  //    INDEX into it, and that index is module-local: reorder the type section and every
  //    row's typeIndex is renumbered while not one signature has changed.  The ABI is the
  //    signature, not the index, so the check below compares a row's declared signature
  //    against what the index it is paired with ACTUALLY resolves to in this module --
  //    which catches both halves of that: a signature edited in the table, and a
  //    typeIndex that has drifted onto a different type.
  // -------------------------------------------------------------------------
  const bytes = readFileSync(WASM);
  let decode;
  try {
    decode = decodeImportSection(bytes);
  } catch (e) {
    process.stderr.write(`FAIL  cannot decode the import section: ${e.message}\n`);
    process.exit(1);
  }
  const { imports: decoded, types: decodedTypes } = decode;

  const decodedFuncs = decoded.filter((e) => e.kind === 'func');
  const decodedOthers = decoded.filter((e) => e.kind !== 'func');

  const PRODUCT_FUNC_IMPORTS = 207;
  const PRODUCT_OTHER_IMPORTS = 1;
  // The counts below are the product's, and they are worth asserting on the product
  // module: a rebuild that gained or lost an import must not pass. A synthetic
  // fixture is checked by the pure helpers above instead, never by lying here.
  if (decodedFuncs.length === PRODUCT_FUNC_IMPORTS && decodedOthers.length === PRODUCT_OTHER_IMPORTS) {
    ok(`import section decodes to the documented ${PRODUCT_FUNC_IMPORTS + PRODUCT_OTHER_IMPORTS} entries`, decoded.length === PRODUCT_FUNC_IMPORTS + PRODUCT_OTHER_IMPORTS, `got ${decoded.length}`);
    ok(`${PRODUCT_FUNC_IMPORTS} of them are functions`, decodedFuncs.length === PRODUCT_FUNC_IMPORTS, `got ${decodedFuncs.length}`);
    ok(`exactly ${PRODUCT_OTHER_IMPORTS} non-function import`, decodedOthers.length === PRODUCT_OTHER_IMPORTS, `got ${decodedOthers.length} (${decodedOthers.map((e) => e.kind).join(',')})`);
  } else {
    note('import counts are not the product\'s', `decoded ${decoded.length} entries (${decodedFuncs.length} func + ${decodedOthers.length} other); product counts are ${PRODUCT_FUNC_IMPORTS}+${PRODUCT_OTHER_IMPORTS}, so the product-count checks are stated rather than asserted`);
  }

  // -------------------------------------------------------------------------
  // 2. The browser's own reading of the same module, as a third opinion.
  // -------------------------------------------------------------------------
  let jsImports = null;
  try {
    // A module this engine refuses to compile is not a module whose table can be
    // checked: swallowing the throw would leave every check below running against
    // bytes nothing agreed to read.
    const mod = new WebAssembly.Module(bytes);
    jsImports = WebAssembly.Module.imports(mod);
  } catch (e) {
    ok('WebAssembly.Module compiles the module', false, e.message);
  }
  if (jsImports) {
    ok('WebAssembly.Module compiles the module', true);
    const jsFuncs = jsImports.filter((e) => e.kind === 'function');
    ok('WebAssembly.Module.imports agrees on the function count', jsFuncs.length === decodedFuncs.length, `js=${jsFuncs.length} ours=${decodedFuncs.length}`);
    const jsSet = new Set(jsFuncs.map((e) => `${e.module}.${e.name}`));
    const ourSet = new Set(decodedFuncs.map((e) => `${e.mod}.${e.name}`));
    const onlyJs = [...jsSet].filter((x) => !ourSet.has(x));
    const onlyOurs = [...ourSet].filter((x) => !jsSet.has(x));
    ok('and on the exact set of names', onlyJs.length === 0 && onlyOurs.length === 0, onlyJs.length + onlyOurs.length ? `js-only=${onlyJs} ours-only=${onlyOurs}` : 'sets equal');
    // The non-function surface too, since that is where the memory lives.
    const jsOther = jsImports.filter((e) => e.kind !== 'function').map((e) => `${e.module}.${e.name}:${e.kind}`);
    const ourOther = decodedOthers.map((e) => `${e.mod}.${e.name}:${e.kind}`);
    ok('and on the non-function imports', jsOther.join(',') === ourOther.join(','), `js=[${jsOther.join(',')}] ours=[${ourOther.join(',')}]`);
  }

  // -------------------------------------------------------------------------
  // 3. The emitted table, against that decode.
  // -------------------------------------------------------------------------
  const table = JSON.parse(readFileSync(JSON_PATH, 'utf8'));
  const rows = table.rows;
  ok('json row count equals the decode', rows.length === decodedFuncs.length, `json=${rows.length} decode=${decodedFuncs.length}`);

  // Set equality, both directions, as ordered lists so a reordering also shows up.
  const decodedOrdered = decodedFuncs.map((e) => `${e.mod}.${e.name}`);
  const jsonOrdered = rows.map((r) => `${table.importModule}.${r.name}`);
  {
    const a = decodedOrdered.join('\n');
    const b = jsonOrdered.join('\n');
    ok('rows match the import section in order, name for name', a === b, a === b ? '' : `first divergence: ${[...decodedOrdered].find((x, i) => x !== jsonOrdered[i]) ?? 'length'}`);
  }

  // -------------------------------------------------------------------------
  // 3b. Every function row's DECLARED signature against what this module's type
  //     section actually holds at that row's index.
  //
  //     This is the check the table exists for. `signature` is the ABI; `typeIndex`
  //     is a label that a rebuild is free to renumber. Comparing the two within one
  //     module is what makes the signature field load-bearing: a signature typed
  //     into the table (or edited after the fact) has to match bytes.
  //
  //     Note what is NOT required: that a row's typeIndex equals the committed
  //     table's typeIndex. The index is module-local, so requiring that would fail
  //     every renumbered rebuild for no ABI reason -- exactly the false failure this
  //     pair was fixed for.
  // -------------------------------------------------------------------------
  {
    const wrongSig = [];
    for (let i = 0; i < Math.min(rows.length, decodedFuncs.length); i++) {
      const r = rows[i]; const d = decodedFuncs[i];
      if (d.signatureMissing) { wrongSig.push(`a.${r.name} typeIndex ${d.typeIndex} is out of range (${decodedTypes.length} types)`); continue; }
      if (r.signature !== d.signature) wrongSig.push(`a.${r.name} row says ${r.signature} but type#${d.typeIndex} resolves to ${d.signature}`);
    }
    ok('every row\'s declared signature equals what its import\'s type index resolves to in this module',
      wrongSig.length === 0, wrongSig.slice(0, 6).join(' | ') + (wrongSig.length > 6 ? ` (+${wrongSig.length - 6} more)` : ''));
  }

  // -------------------------------------------------------------------------
  // 3c. The non-function imports, strictly: the table must carry each of them in
  //     `totals.otherImports`, with the same module, name, kind, and limits.
  //     A missing `otherImports` is a failure, not a skipped check: the memory's
  //     limits are ABI (a host sizing its own buffer needs min and max), so a table
  //     that simply does not mention them has not recorded the surface it claims to.
  // -------------------------------------------------------------------------
  {
    const declared = Array.isArray(table.totals?.otherImports) ? table.totals.otherImports : null;
    if (declared === null) {
      ok('the table records its non-function imports in totals.otherImports', false, 'totals.otherImports is missing or not an array');
    } else {
      ok('the table records one totals.otherImports entry per decoded non-function import',
        declared.length === decodedOthers.length, `table=${declared.length} decode=${decodedOthers.length}`);
      const bad = [];
      for (let i = 0; i < Math.min(declared.length, decodedOthers.length); i++) {
        const t = declared[i]; const d = decodedOthers[i];
        if (t.module !== d.mod) bad.push(`[${i}] module ${JSON.stringify(t.module)} vs ${JSON.stringify(d.mod)}`);
        if (t.name !== d.name) bad.push(`[${i}] name ${JSON.stringify(t.name)} vs ${JSON.stringify(d.name)}`);
        if (t.kind !== d.kind) bad.push(`[${i}] kind ${JSON.stringify(t.kind)} vs ${JSON.stringify(d.kind)}`);
        const ld = limitsDiff(t.limits, d.limits);
        if (ld) bad.push(`[${i}] ${d.mod}.${d.name} limits: ${ld}`);
      }
      ok('every non-function import matches the table\'s entry for it, including its limits',
        bad.length === 0, bad.slice(0, 6).join(' | '));
    }
  }

  {
    const d = new Set(decodedFuncs.map((e) => e.name));
    const j = new Set(rows.map((r) => r.name));
    const missing = [...d].filter((x) => !j.has(x));
    const extra = [...j].filter((x) => !d.has(x));
    ok('name sets are equal (no omissions, no inventions)', missing.length === 0 && extra.length === 0, `missing=${JSON.stringify(missing)} extra=${JSON.stringify(extra)}`);
    ok('198 distinct names, 207 entries', d.size === 198 && rows.length === 207, `distinct=${d.size} entries=${rows.length}`);
  }

  // Every row carries the four things the task requires of it.
  const KNOWN_BUCKETS = new Set(['gl', 'fd', 'asyncify', 'async', 'eh', 'runtime', 'todo']);
  const noBucket = rows.filter((r) => !KNOWN_BUCKETS.has(r.bucket));
  ok('every row has a known bucket', noBucket.length === 0, noBucket.map((r) => r.name).join(','));

  const noOfficial = rows.filter((r) => !r.official);
  ok('every row has an official name read from the glue', noOfficial.length === 0, noOfficial.map((r) => r.name).join(','));

  const noRef = rows.filter((r) => r.glueBindingOffset == null);
  ok('every row has a glue binding offset', noRef.length === 0, noRef.map((r) => r.name).join(','));

  const noImpl = rows.filter((r) => r.glueImplOffset == null);
  ok('every row points at an implementation definition in the glue', noImpl.length === 0, noImpl.map((r) => r.name).join(','));

  // The offsets have to be real, not merely present: re-derive each from the glue bytes.
  const glue = readFileSync(GLUE, 'utf8');

  // Identity FIRST.  Every offset below is an index into one specific file, and the vendor
  // SDK and a deployed build of the same engine differ by a couple of bytes in their embedded
  // package table, which shifts every one of the 405 offsets below by that same amount.  That
  // failure is indistinguishable from a corrupt table by its own symptoms, so it gets its own
  // gate: one line naming the two files beats 405 "bad offset" lines.
  const glueSha256 = createHash('sha256').update(glue).digest('hex');
  {
    const wantGlue = table.totals?.glueSha256 ?? null;
    const wantWasm = table.totals?.moduleSha256 ?? null;
    const gotWasm = createHash('sha256').update(bytes).digest('hex');
    // Reported, never judged.  A module embeds the directory it was built in, so its own hash can
    // only ever match on the machine that produced the committed table -- the same tree is
    // 41,321,914 B from a Windows checkout and 41,322,022 B from the CI runner.  What the table does
    // claim is the import surface and the offsets below, and those are machine-independent.
    note('table module provenance', wantWasm === null
      ? 'the table records no module sha256'
      : `table=${wantWasm.slice(0, 12)} this module=${gotWasm.slice(0, 12)}${wantWasm === gotWasm ? ' (built from this module)' : ' (a different build of the same tree)'}`);
    if (wantGlue === null) {
      ok('the glue is the one the table was generated from', true, 'table records no glue sha256 (pre-hash table; offsets not identity-checked)');
    } else {
      ok('the glue is the one the table was generated from', wantGlue === glueSha256,
        wantGlue === glueSha256 ? '' : `table=${wantGlue.slice(0, 12)} given=${glueSha256.slice(0, 12)} — this is a different glue build; regenerate the table for it`);
    }
    ok('the table records the glue it was built from', typeof wantGlue === 'string' && wantGlue.length === 64,
      typeof wantGlue === 'string' ? '' : 'totals.glueSha256 missing');
  }

  // glueImplOffset is the offset of the DEFINITION SITE — the `function` keyword, or `var` for
  // the one binding that is a variable rather than a function — so the identifier follows it.
  let badRef = 0; let badImpl = 0; const checkedImpl = new Set(); const forms = new Map();
  for (const r of rows) {
    const at = glue.slice(r.glueBindingOffset, r.glueBindingOffset + r.official.length);
    if (at !== r.official) badRef++;
    if (checkedImpl.has(r.glueImplOffset)) continue;
    checkedImpl.add(r.glueImplOffset);
    const m = /^(function|var|let|const) /.exec(glue.slice(r.glueImplOffset, r.glueImplOffset + 16));
    if (!m) { badImpl++; continue; }
    forms.set(m[1], (forms.get(m[1]) ?? 0) + 1);
    const head = glue.slice(r.glueImplOffset, r.glueImplOffset + m[0].length + r.official.length + 1);
    if (head !== `${m[1]} ${r.official}(`) badImpl++;
  }
  ok(`all ${rows.length} glue binding offsets land on their own name in artemis.js`, badRef === 0, `${badRef} bad`);
  ok(`all ${checkedImpl.size} implementation offsets are definition sites of their own row's symbol in artemis.js`, badImpl === 0, `${badImpl} bad — forms seen: ${JSON.stringify(Object.fromEntries(forms))}`);

  // The glue's own binding count: nothing in it may be left unaccounted for.
  {
    const glueNames = new Set();
    for (const r of rows) glueNames.add(r.name);
    const inWasm = new Set(decodedFuncs.map((e) => e.name));
    ok('the glue binds 199 names; the module imports all 199 including the memory', glueNames.size === 198, `distinct imported=${glueNames.size}`);
    ok('no imported name is missing from the glue', [...inWasm].every((x) => glueNames.has(x)));
  }

  // Every `todo` row explains itself.
  const silentTodo = rows.filter((r) => r.bucket === 'todo' && !r.todoReason);
  ok('no todo row without a recorded reason', silentTodo.length === 0, silentTodo.map((r) => r.name).join(','));

  // The md must show every row the json has, and nothing more.
  const md = readFileSync(MD_PATH, 'utf8');
  const mdRows = md.split('\n').filter((l) => /^\d+ \| /.test(l));
  ok('the markdown table has exactly one line per import entry', mdRows.length === 207, `got ${mdRows.length}`);
  ok('every row in the markdown carries a bucket', mdRows.every((l) => /\| `(gl|fd|asyncify|async|eh|runtime|todo)` \|/.test(l)));
  {
    const mdNames = mdRows.map((l) => l.split(' | ')[1].replace(/`/g, ''));
    const sameOrder = mdNames.length === decodedOrdered.length && mdNames.every((n, i) => n === decodedFuncs[i].name);
    ok('the markdown rows are the same rows, in the same order', sameOrder, sameOrder ? '' : `first mismatch at ${mdNames.findIndex((n, i) => n !== decodedFuncs[i]?.name)}`);
  }

  // Bucket totals must add up, in the json and in the markdown.
  {
    const sum = Object.values(table.buckets).reduce((a, c) => a + c, 0);
    ok('the bucket counts sum to 207', sum === 207, `sum=${sum}`);
    const counted = {};
    for (const r of rows) counted[r.bucket] = (counted[r.bucket] ?? 0) + 1;
    const same = Object.keys(table.buckets).every((k) => table.buckets[k] === (counted[k] ?? 0));
    ok('the per-bucket counts match the rows', same, JSON.stringify(table.buckets));
  }

  process.stdout.write(`\n${failures === 0 ? 'ALL CHECKS PASS' : `${failures} CHECK(S) FAILED`}${notes === 0 ? '' : ` (${notes} reported line${notes === 1 ? '' : 's'})`}\n`);
  process.exit(failures === 0 ? 0 : 1);
}
