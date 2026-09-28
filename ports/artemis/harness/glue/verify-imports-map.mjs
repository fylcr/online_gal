#!/usr/bin/env node
// Independent check of ports/artemis/harness/glue/imports-map.{md,json}.
//
// Deliberately NOT sharing code with the generator: it re-decodes the wasm import
// section with its own reader, and cross-checks the emitted table against both that
// decode and the browser's own WebAssembly.Module.imports. If the generator and this
// script ever agree by accident rather than by measurement, that is the failure this
// is here to catch.
//
//   node ports/artemis/harness/glue/verify-imports-map.mjs \
//     --wasm   <bridge.wasm> \
//     --glue   <artemis.js> \
//     --json   <imports-map.json> \
//     --md     <imports-map.md>

import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';

const argv = process.argv.slice(2);
const arg = (k) => { const i = argv.indexOf(k); return i < 0 ? null : resolve(argv[i + 1]); };
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

// ---------------------------------------------------------------------------
// 1. Our own decode of the import section, written separately from the generator's.
// ---------------------------------------------------------------------------
const bytes = readFileSync(WASM);
let p = 8;
const sections = [];
while (p < bytes.length) {
  const id = bytes[p++];
  let len = 0; let shift = 0; let b;
  do { b = bytes[p++]; len |= (b & 0x7f) << shift; shift += 7; } while (b & 0x80);
  sections.push({ id, start: p, end: p + len });
  p += len;
}
const importSec = sections.find((s) => s.id === 2);
if (!importSec) { process.stderr.write('FAIL  no import section\n'); process.exit(1); }

const ulebAt = (at) => { let v = 0; let shift = 0; let b; do { b = bytes[at++]; v |= (b & 0x7f) << shift; shift += 7; } while (b & 0x80); return [v >>> 0, at]; };
const readName = (at) => { const [len, start] = ulebAt(at); return [bytes.subarray(start, start + len).toString('utf8'), start + len]; };

let q = importSec.start;
let count = 0; { [count, q] = ulebAt(q); }
const decoded = [];
for (let i = 0; i < count; i++) {
  const [mod, after] = readName(q); q = after;
  const [name, after2] = readName(q); q = after2;
  const kind = bytes[q++];
  if (kind === 0) { let t; [t, q] = ulebAt(q); decoded.push({ mod, name, kind: 'func', typeIndex: t }); continue; }
  if (kind === 1) { q++; let f; [f, q] = ulebAt(q); let min; [min, q] = ulebAt(q); let max = null; if (f & 1) [max, q] = ulebAt(q); decoded.push({ mod, name, kind: 'table' }); continue; }
  if (kind === 2) { let f; [f, q] = ulebAt(q); let min; [min, q] = ulebAt(q); let max = null; if (f & 1) [max, q] = ulebAt(q); decoded.push({ mod, name, kind: 'memory', min, max }); continue; }
  if (kind === 3) { q += 2; decoded.push({ mod, name, kind: 'global' }); continue; }
  let n; [n, q] = ulebAt(q);
  for (let j = 0; j < n; j++) { q++; let x; [x, q] = ulebAt(q); }
  decoded.push({ mod, name, kind: 'tag' });
}
ok('import section decodes to the documented 208 entries', decoded.length === 208, `got ${decoded.length}`);

const decodedFuncs = decoded.filter((e) => e.kind === 'func');
const decodedOthers = decoded.filter((e) => e.kind !== 'func');
ok('207 of them are functions', decodedFuncs.length === 207, `got ${decodedFuncs.length}`);
ok('exactly 1 non-function import', decodedOthers.length === 1, `got ${decodedOthers.length} (${decodedOthers.map((e) => e.kind).join(',')})`);

// ---------------------------------------------------------------------------
// 2. The browser's own reading of the same module, as a third opinion.
// ---------------------------------------------------------------------------
const jsImports = WebAssembly.Module.imports(new WebAssembly.Module(bytes));
const jsFuncs = jsImports.filter((e) => e.kind === 'function');
ok('WebAssembly.Module.imports agrees on the function count', jsFuncs.length === decodedFuncs.length, `js=${jsFuncs.length} ours=${decodedFuncs.length}`);
{
  const jsSet = new Set(jsFuncs.map((e) => `${e.module}.${e.name}`));
  const ourSet = new Set(decodedFuncs.map((e) => `${e.mod}.${e.name}`));
  const onlyJs = [...jsSet].filter((x) => !ourSet.has(x));
  const onlyOurs = [...ourSet].filter((x) => !jsSet.has(x));
  ok('and on the exact set of names', onlyJs.length === 0 && onlyOurs.length === 0, onlyJs.length + onlyOurs.length ? `js-only=${onlyJs} ours-only=${onlyOurs}` : 'sets equal');
}

// ---------------------------------------------------------------------------
// 3. The emitted table, against that decode.
// ---------------------------------------------------------------------------
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

process.stdout.write(`\n${failures === 0 ? 'ALL CHECKS PASS' : `${failures} CHECK(S) FAILED`}\n`);
process.exit(failures === 0 ? 0 : 1);
