#!/usr/bin/env node
// Compares a regenerated imports-map (.json + .md) against the committed pair.
// Only these may differ; everything else, including unknown top-level fields, row
// order and every unknown nested field, must match exactly:
//   - totals.moduleBytes / totals.moduleSha256
//   - rows[*].typeIndex
//   - \btype#\d+\b inside a row's `duplicate` string (-> type#<ordinal>)
//   - in the md: the VALUE cell only of the two provenance rows "module bytes" and
//     "module sha256" -- the label and the whitespace in the row are kept verbatim,
//     so a lost space there is a difference, not a masked one -- plus the same type#N
//     rewrite, globally.
//
// Usage:
//   node compare-imports-map.mjs --committed <imports-map.json> --regenerated <imports-map.json> \
//     --committed-md <imports-map.md> --regenerated-md <imports-map.md> [--wasm <bridge.wasm>]
//
// Exit 0 = same ABI. Exit 1 = mismatch or an undecodable --wasm. Exit 2 = bad usage.
//
// An optional --wasm is decoded INDEPENDENTLY (decodeImportSection, imported from the
// sibling verify-imports-map.mjs -- deliberately not the generator's reader) and the
// decode is judged against the regenerated table: row order, names, each row's kind
// (with `func` normalized from the decode's `func`), each function row's import MODULE
// as well as its signature, and every non-function import including its limits. A
// decode that fails exits 1.
//
// The bytes are handed to WebAssembly.Module FIRST: the engine's own validation is a
// precondition, not a second opinion.  Decoding is a hand-written reader that reads
// what the bytes SAY, so on a truncated or malformed module it can return a
// plausible-looking import list for bytes no engine would run -- and the decode would
// then be the only thing standing between a corrupt module and a green gate.  Letting
// the engine reject it first keeps that reader a checker of valid modules instead of a
// parser that has to be trusted about invalid ones.  The decoder is NOT weakened and
// no masking is widened to accommodate either case.
//
// A function row's module is judged because `importModule` is one scalar for the whole
// table: with every function row importing from `a`, the scalar is the only place the
// module half of `a.foo` is recorded, so a decode whose entries all sit under some
// other module would otherwise agree with every row and with the table.
//
// Without --wasm the comparison still runs and says "COMPARE OK", but that is a
// same-ABI comparison only -- the ABI is NOT fully verified without the module.

import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';
import { pathToFileURL } from 'node:url';
import { decodeImportSection, limitsDiff } from './verify-imports-map.mjs';

// Every flag the comparer implements. An argument that starts with `--` and is not one
// of these is refused rather than ignored: a typo'd `--wasmm` would otherwise fall
// through to the same-ABI comparison and exit 0, which reads as "the module was
// verified" when nothing was ever decoded.
export const KNOWN_FLAGS = Object.freeze([
  '--committed', '--regenerated', '--committed-md', '--regenerated-md', '--wasm',
]);

const usage = (m) => {
  if (m) process.stderr.write(`compare-imports-map: ${m}\n`);
  process.stderr.write(`usage: compare-imports-map.mjs ${KNOWN_FLAGS.slice(0, 4).map((f) => `${f} <path>`).join(' ')} [--wasm <path>]\n`);
  process.exit(2);
};
const bad = (m) => { process.stderr.write(`compare-imports-map: ${m}\n`); process.exit(1); };

// Is this module actually a wasm module? The engine compiles it, or it is rejected and
// the reason is returned. `WebAssembly.Module` also enforces the version field, which
// the reader does not look at -- a \\0asm header with version 2 decodes as an import
// list and cannot be instantiated, and that difference is exactly the kind of thing a
// gate has to be told rather than infer.
export function engineRejects(bytes) {
  try { new WebAssembly.Module(bytes); return null; } catch (e) { return e.message; }
}
// The leading \b matters: `typeIndex` must NOT be masked away. Only a type#N token,
// where `type` starts at a word boundary, is a renumberable ordinal.
const TYPE_NUM = /\btype#\d+\b/g;
// Matches ONLY a well-formed two-cell provenance row: the label cell with its own
// spacing (1), the spacing between the two cells (2), the value (3), the spacing
// before the closing pipe (4) and the empty trailing cell (5). Anything else -- a
// third cell, a label with no closing pipe, a row that is not a provenance row at all
// -- does not match, is left EXACTLY as it was, and so fails the comparison below
// like any other difference rather than being masked or thrown on.
const PROVENANCE_ROW = /^(\|[ \t]*(?:module bytes|module sha256)[ \t]*\|)([ \t]*)([^|]*?)([ \t]*)(\|[ \t]*)$/;
const maskType = (s) => (typeof s === 'string' ? s.replace(TYPE_NUM, 'type#<ordinal>') : s);

// ---------------------------------------------------------------- helpers (exported)
export function normalizedMap(table) {
  if (!isObj(table)) throw new TypeError('imports map is not an object');
  if (!isObj(table.totals)) throw new TypeError('imports map has no totals object');
  if (!Array.isArray(table.rows)) throw new TypeError('imports map has no rows array');
  for (const r of table.rows) if (!isObj(r)) throw new TypeError('imports map has a non-object row');
  const out = structuredClone(table);
  delete out.totals.moduleBytes;
  delete out.totals.moduleSha256;
  for (const r of out.rows) {
    delete r.typeIndex;
    if (typeof r.duplicate === 'string') r.duplicate = maskType(r.duplicate);
  }
  return out;
}
const isObj = (v) => v !== null && typeof v === 'object' && !Array.isArray(v);

export function normalizedMarkdown(text) {
  if (typeof text !== 'string') throw new TypeError('markdown is not a string');
  const lines = text.split('\n');
  for (let i = 0; i < lines.length; i++) {
    const m = PROVENANCE_ROW.exec(lines[i]);
    // Replace the value cell ONLY. Everything the row says except its own bytes and
    // its own hash is compared verbatim: the label, the whitespace around both cells
    // and the trailing cell are all captured back unchanged.
    if (m) lines[i] = `${m[1]}${m[2]}<provenance>${m[4]}${m[5]}`;
    // Every other provenance-looking row is deliberately left untouched rather than
    // thrown on: a malformed row then differs somewhere in the comparison like any
    // other edit, and the failure names the difference instead of one line.
  }
  return maskType(lines.join('\n'));
}

export function compareMaps(before, after) {
  try { return walk(normalizedMap(before), normalizedMap(after), '$'); } catch (e) { return [e.message]; }
}

function walk(a, b, at) {
  if (Array.isArray(a) || Array.isArray(b)) {
    if (!Array.isArray(a) || !Array.isArray(b)) return [`${at}: one side is not an array`];
    if (a.length !== b.length) return [`${at}: length ${a.length} vs ${b.length}`];
    const out = [];
    // In order: same length but different order is an ABI change, not a permutation.
    for (let i = 0; i < a.length; i++) out.push(...walk(a[i], b[i], `${at}[${i}]`));
    return out.slice(0, 12);
  }
  if (isObj(a) || isObj(b)) {
    if (!isObj(a) || !isObj(b)) return [`${at}: one side is not an object`];
    const ka = Object.keys(a); const kb = Object.keys(b); const out = [];
    for (const k of ka) {
      const d = walk(a[k], k in b ? b[k] : MISSING, `${at}.${k}`);
      out.push(...d);
      if (out.length > 12) break;
    }
    // Reverse direction too: a field the regenerated side gained is a difference even
    // when every field the committed side has still matches.
    for (const k of kb) if (!(k in a)) out.push(`${at}.${k}: only in the regenerated table`);
    return out.slice(0, 12);
  }
  return Object.is(a, b) ? [] : [`${at}: ${JSON.stringify(a)} vs ${JSON.stringify(b)}`];
}
const MISSING = Symbol('missing');

// ------------------------------------------------------------------------------ run
export function main(argv) {
  // A flag the comparer does not implement is a usage error (exit 2), not something to
  // walk past: the whole point of naming these five is that an unrecognised one cannot
  // silently disable the only check that reads the module.
  const unknown = argv.filter((a) => a.startsWith('--') && !KNOWN_FLAGS.includes(a));
  if (unknown.length > 0) usage(`unknown option ${unknown[0]}`);
  const at = (k) => { const i = argv.indexOf(k); return i < 0 ? null : argv[i + 1]; };
  // An empty string is a missing value, not a path: `resolve('')` is the cwd, which
  // would read a directory. It is checked before resolving.
  const arg = (k) => {
    const v = at(k);
    return v === null || v === undefined || v === '' || v.startsWith('--') ? null : resolve(v);
  };
  const CLI = arg('--committed');
  const REG = arg('--regenerated');
  const CLI_MD = arg('--committed-md');
  const REG_MD = arg('--regenerated-md');
  const WASM = arg('--wasm');
  // A flag that is present but names no file is a usage error (exit 2): the empty
  // string, a value that is really the next option, and a flag at the very end of
  // argv all leave the comparer with nothing to read, which is not a mismatch.
  for (const flag of KNOWN_FLAGS) {
    if (argv.includes(flag) && arg(flag) === null) usage(`${flag} needs a path`);
  }
  if (!CLI || !REG || !CLI_MD || !REG_MD) usage();

  const read = (p, who) => { try { return readFileSync(p, 'utf8'); } catch (e) { bad(`cannot read the ${who}: ${e.message}`); } };
  const a = read(CLI, 'committed table');
  const b = read(REG, 'regenerated table');
  let aDoc; let bDoc;
  try { aDoc = JSON.parse(a); } catch (e) { bad(`committed table is not valid JSON: ${e.message}`); }
  try { bDoc = JSON.parse(b); } catch (e) { bad(`regenerated table is not valid JSON: ${e.message}`); }
  let aNorm; let bNorm;
  try { aNorm = normalizedMap(aDoc); bNorm = normalizedMap(bDoc); } catch (e) { bad(`${e.message}`); }
  // A markdown that is not a string, or a normalizer that throws, is a bad input, not
  // a crash: exit 1 with the reason, the same as any other unusable input.
  let aMd; let bMd;
  try { aMd = normalizedMarkdown(read(CLI_MD, 'committed md')); bMd = normalizedMarkdown(read(REG_MD, 'regenerated md')); } catch (e) { bad(`cannot normalize the markdown: ${e.message}`); }

  let failures = 0;
  const ok = (label, pass, detail = '') => {
    process.stdout.write(`${pass ? 'PASS' : 'FAIL'}  ${label}${detail ? `  — ${detail}` : ''}\n`);
    if (!pass) failures++;
  };

  const diffs = compareMaps(aNorm, bNorm);
  ok('the json maps are identical after masking', diffs.length === 0, diffs.slice(0, 6).join(' | '));
  for (const d of diffs) process.stdout.write(`      ${d}\n`);
  ok('the markdown is identical after masking', aMd === bMd,
    aMd === bMd ? '' : 'normalized md differs');

  // ------------------------------------------------------- optional --wasm decode
  // The import section is decoded INDEPENDENTLY, by the sibling verifier's reader --
  // not by the generator's -- and the decode is what the table is judged against.
  // `rows.length` can never be compared to the decoded entry count: the rows are the
  // function imports only, and the memory is not a function, so any check written that
  // way would pass by never looking at the memory at all.
  if (WASM) {
    let bytes;
    try { bytes = readFileSync(WASM); } catch (e) { bad(`cannot read --wasm: ${e.message}`); }
    // The engine's verdict comes FIRST, on the bytes as they sit on disk. A module
    // whose header, version or body the engine refuses is not a module the ABI of
    // which can be checked: the decode below would be reading bytes nothing agreed to
    // be a valid module, and a table that matched such a decode would be a table for
    // something that cannot be run.
    const rejected = engineRejects(bytes);
    if (rejected !== null) bad(`--wasm is not a valid module: ${rejected}`);
    let decoded;
    try { decoded = decodeImportSection(bytes).imports; } catch (e) { bad(`--wasm does not decode: ${e.message}`); }
    const dFuncs = decoded.filter((d) => d.kind === 'func');
    const dOthers = decoded.filter((d) => d.kind !== 'func');
    const rows = bDoc.rows ?? [];

    ok('--wasm: one decode entry per function row, in import order', rows.length === dFuncs.length,
      `rows=${rows.length} decoded functions=${dFuncs.length}`);
    {
      const bads = [];
      for (let i = 0; i < Math.min(rows.length, dFuncs.length); i++) {
        const d = dFuncs[i]; const r = rows[i];
        // The decode says `func`; the table spells the function kind `function`.
        // Normalize that one spelling so the comparison is about the kind, not a word.
        const dk = d.kind === 'func' ? 'function' : d.kind;
        if (r.name !== d.name) bads.push(`[${i}] name ${JSON.stringify(r.name)} vs ${JSON.stringify(d.name)}`);
        if (r.signature !== d.signature) bads.push(`[${i}] ${d.name} signature ${JSON.stringify(r.signature)} vs ${JSON.stringify(d.signature)}`);
        if (r.kind !== undefined && r.kind !== dk) bads.push(`[${i}] ${d.name} kind ${JSON.stringify(r.kind)} vs ${JSON.stringify(dk)}`);
        // The module half of the import is judged per row, against the table's own
        // scalar. The table records the module once (`importModule`) and every row
        // inherits it, so the scalar has to be the thing this is compared to -- and a
        // decode that put these entries under a different module is a different ABI,
        // not a spelling difference.
        const declaredModule = typeof bDoc.importModule === 'string' ? bDoc.importModule : null;
        if (declaredModule === null) {
          bads.push(`[${i}] the table records no importModule, so ${JSON.stringify(d.mod)}.${d.name} cannot be checked against it`);
        } else if (declaredModule !== d.mod) {
          bads.push(`[${i}] module ${JSON.stringify(declaredModule)} vs decoded ${JSON.stringify(d.mod)}`);
        }
      }
      ok('--wasm: every function row matches the decoded import (module, name, signature, kind)', bads.length === 0,
        bads.slice(0, 6).join(' | '));
    }
    {
      const declared = Array.isArray(bDoc.totals?.otherImports) ? bDoc.totals.otherImports : null;
      if (declared === null) {
        ok('--wasm: the table records its non-function imports in totals.otherImports', false,
          'totals.otherImports is missing or not an array');
      } else {
        const bads = [];
        if (declared.length !== dOthers.length) bads.push(`count ${declared.length} vs ${dOthers.length}`);
        for (let i = 0; i < Math.min(declared.length, dOthers.length); i++) {
          const t = declared[i]; const d = dOthers[i];
          if (t.module !== d.mod) bads.push(`[${i}] module ${JSON.stringify(t.module)} vs ${JSON.stringify(d.mod)}`);
          if (t.name !== d.name) bads.push(`[${i}] name ${JSON.stringify(t.name)} vs ${JSON.stringify(d.name)}`);
          if (t.kind !== d.kind) bads.push(`[${i}] kind ${JSON.stringify(t.kind)} vs ${JSON.stringify(d.kind)}`);
          const ld = limitsDiff(t.limits, d.limits);
          if (ld) bads.push(`[${i}] ${d.mod}.${d.name} limits: ${ld}`);
        }
        ok('--wasm: every non-function import matches totals.otherImports, limits included', bads.length === 0,
          bads.slice(0, 6).join(' | '));
      }
    }
  } else {
    process.stdout.write('NOTE  --wasm not given: nothing was decoded, so no row was checked against the module.\n');
  }

  process.stdout.write(`\n${failures === 0
    ? (WASM
      ? 'COMPARE OK — same ABI (masked: module bytes/sha256, typeIndex, type#N) and the module decode agrees with the table'
      : 'COMPARE OK — same ABI (masked: module bytes/sha256, typeIndex, type#N). The ABI is NOT fully verified without --wasm.')
    : `${failures} COMPARISON(S) FAILED`}\n`);
  process.exitCode = failures === 0 ? 0 : 1;
}

const invokedDirectly =
  process.argv[1] !== undefined &&
  import.meta.url === pathToFileURL(resolve(process.argv[1])).href;
if (invokedDirectly) main(process.argv.slice(2));
