/**
 * Portable regression for the Artemis imports-map pair and the comparator that guards it.
 *
 * The two files this exercises -- `ports/artemis/harness/glue/compare-imports-map.mjs`
 * (the comparator) and its sibling `verify-imports-map.mjs` (the independent reader) --
 * are the gate a future bridge rebuild has to pass. Nothing here needs the 41 MB
 * product module, the official 199 MB glue, a `.ref/` checkout or the `.test/`
 * workbench: every module under test is synthesized as bytes in this file's own
 * memory and handed to the real exported helpers.
 *
 * What is pinned, and why each one is a test rather than a comment:
 *
 *  - the comparator's provenance mask is VALUE-only. The two markdown provenance rows
 *    are masked in their value cell and nowhere else, so a lost label space, a third
 *    cell, a reordered row or a renamed label all still fail. A mask that widened to
 *    the whole row would hide exactly those edits.
 *  - a type-section reorder with identical resolved signatures and identical import
 *    order is NOT an ABI change: every `typeIndex` is renumbered, every signature is
 *    unchanged, and the comparison must still pass. That is the false failure the
 *    typeIndex mask exists to prevent.
 *  - name, signature, MODULE, order, memory limits and the shared flag ARE the ABI,
 *    and each mutation must fail. The module half is the one the table records as a
 *    single scalar (`importModule`) rather than per row, so it is checked per row
 *    against the decode: a module that imports the same names and signatures from
 *    somewhere else is a different ABI and must not pass. Where a mutation is NOT
 *    caught, this file says so by asserting the pass -- it never weakens the
 *    expectation to make a case green.
 *  - every synthesized module is held to the engine with `WebAssembly.validate`
 *    before it is judged by a helper. A fixture that drifted into unreadable bytes
 *    fails here rather than silently making a decode test vacuous. The cases whose
 *    whole point is bytes the ENGINE refuses are built by hand, and they assert
 *    `validate === false` explicitly so the reader is seen to reach its own verdict.
 *  - the CLI's `--wasm` path is driven end to end by REAL valid wasm fixtures whose
 *    table has the same counts and names as the committed one, and the challenges
 *    mutate the module rather than the JSON: reordered types resolving to the same
 *    ABI pass, while a changed module, a changed signature, a changed name, a changed
 *    order and changed memory limits each fail. A table pair that is byte-identical
 *    on both sides is used throughout, so nothing can pass or fail because of a JSON
 *    edit -- a mutation is only ever a mutation of the module.
 *  - invalid module bytes are rejected by the ENGINE before the decoder sees them:
 *    a bad magic, a bad version and a truncation all exit 1 through the native
 *    `WebAssembly.Module` check rather than through a hand-written reader's guess.
 *
 * The JSON offsets, alias/bucket/forwarded/owner/unknown nested fields, the markdown
 * `type#` vs `typeIndex` boundary and the real CLI subprocess behaviour are tested
 * below too. No file outside this test and the portable runner is written: the
 * subprocess tests touch only paths under a fresh `mkdtemp`.
 */
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import {
  mkdirSync, mkdtempSync, realpathSync, rmSync, writeFileSync,
} from 'node:fs';
import { tmpdir } from 'node:os';
import { basename, dirname, join, resolve } from 'node:path';
import { fileURLToPath, pathToFileURL } from 'node:url';
import test from 'node:test';
import {
  compareMaps, engineRejects, main, normalizedMap, normalizedMarkdown,
} from '../ports/artemis/harness/glue/compare-imports-map.mjs';
import {
  decodeImportSection, decodeTypeSection, limitsDiff, listSections, readName, ulebAt,
} from '../ports/artemis/harness/glue/verify-imports-map.mjs';

const COMPARE = fileURLToPath(
  new URL('../ports/artemis/harness/glue/compare-imports-map.mjs', import.meta.url),
);
const VERIFY = fileURLToPath(
  new URL('../ports/artemis/harness/glue/verify-imports-map.mjs', import.meta.url),
);

// ---------------------------------------------------------------------------
// A tiny wasm encoder. Every module here is built from these functions and lives in
// source memory -- no `.wasm` file is read, committed or written.
// ---------------------------------------------------------------------------
const i32 = (value) => {
  const out = [];
  let remaining = value | 0;
  do {
    let byte = remaining & 0x7f;
    remaining >>>= 7;
    if (remaining !== 0) byte |= 0x80;
    out.push(byte);
  } while (remaining !== 0);
  return out;
};
const bytes = (...parts) => parts.flat(Infinity);
const section = (id, payload) => bytes(id, i32(payload.length), payload);
const count = (n) => i32(n);
const name = (text) => bytes(i32(text.length), [...text].map((ch) => ch.charCodeAt(0)));
const HEADER = [0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00];
const FUNC_TYPE = (params, results) => bytes(0x60, count(params.length), params, count(results.length), results);
// The flag byte's low bit 0x01 is the "a max follows" flag, bit 0x02 is shared. The
// spec only permits `shared` alongside a max, so a shared memory always carries 0x03.
const MEM_TYPE = (flags, min, max) => bytes(flags, i32(min), ...(flags & 1 ? i32(max) : []));

/** One import encoded in the import section's own entry shape. */
const impFunc = (mod, field, typeIndex) => bytes(name(mod), name(field), 0x00, i32(typeIndex));
const impMemory = (mod, field, flags, min, max) => bytes(name(mod), name(field), 0x02, MEM_TYPE(flags, min, max));

/**
 * Assembles a module. Types/imports/memory are given as raw payload parts so a test
 * can reorder the type section or drift a type index without re-encoding a table.
 */
const moduleBytes = ({ types, imports, memory }) => new Uint8Array(bytes(
  HEADER,
  section(1, bytes(count(types.length), types)),
  section(2, bytes(count(imports.length + (memory ? 1 : 0)), imports, memory ?? [])),
));

/** The engine's own verdict, asserted so no fixture can rot into unreadable bytes. */
const validated = (wasm) => {
  assert.equal(WebAssembly.validate(wasm), true, 'synthesized fixture must be a valid wasm module');
  new WebAssembly.Module(wasm);
  return wasm;
};

// ---------------------------------------------------------------------------
// The fixture surface. `t0` is (i32)->i32, `t1` is ()->i32 and `t2` is
// (i32,i32)->i32; the third type exists so a reorder has something to permute.
// ---------------------------------------------------------------------------
const T_I32_I32 = FUNC_TYPE([0x7f], [0x7f]);
const T_VOID_I32 = FUNC_TYPE([], [0x7f]);
const T_TWO_I32_I32 = FUNC_TYPE([0x7f, 0x7f], [0x7f]);
const MEMORY_IMPORT = impMemory('a', 'memory', 0x01, 3072, 65536);
const FUNC_IMPORTS = [
  impFunc('a', '$', 0),
  impFunc('a', 'Ob', 0),
  impFunc('a', 'Ea', 1),
  impFunc('a', 'V', 2),
];
const BASE = validated(moduleBytes({
  types: [T_I32_I32, T_VOID_I32, T_TWO_I32_I32],
  imports: FUNC_IMPORTS,
  memory: MEMORY_IMPORT,
}));

const decodeOf = (wasm) => decodeImportSection(Buffer.from(wasm));

/** The table the generator would have emitted for a decoded module. */
const tableFor = (decode) => {
  const funcs = decode.imports.filter((e) => e.kind === 'func');
  const others = decode.imports.filter((e) => e.kind !== 'func');
  return {
    schema: 1,
    importModule: 'a',
    totals: {
      importEntries: decode.imports.length,
      functionImports: funcs.length,
      distinctNames: new Set(funcs.map((e) => e.name)).size,
      // Reported, never judged: the comparator masks both of these.
      moduleBytes: 1234,
      moduleSha256: 'not-the-real-hash',
      glueBytes: 5,
      glueSha256: 'glue-hash-used-only-as-identity',
      otherImports: others.map((e) => ({ module: e.mod, name: e.name, kind: e.kind, limits: e.limits })),
    },
    buckets: { gl: 2, eh: 1, fd: 1 },
    bucketMeaning: { gl: 'webgl', eh: 'exceptions', fd: 'file descriptors' },
    runtimeAliases: [{ from: 'env.sleep', to: 'a.$' }],
    rows: funcs.map((e, index) => ({
      index,
      name: e.name,
      signature: e.signature,
      typeIndex: e.typeIndex,
      official: `_official_${index}`,
      glueBindingOffset: 100 + index,
      glueImplOffset: 200 + index,
      alias: null,
      forwarded: '-',
      bucket: index % 2 === 0 ? 'gl' : 'fd',
      rule: '^x',
      todoReason: null,
      owner: `landed: module-${index}.ts`,
      duplicate: null,
    })),
  };
};

const TABLE = tableFor(decodeOf(BASE));

/** A deep-structural copy so a mutation never leaks into a sibling test. */
const clone = (value) => JSON.parse(JSON.stringify(value));

/** Two provenance rows and one unrelated row, so the mask's selectivity is visible. */
const md = (rows = ['| module sha256 | `aaaa` |']) => [
  '| module bytes | 1234 |',
  ...rows,
  '| label | other |',
  '',
].join('\n');

/** A one-import module, for cases that need a hand-tuned import surface. */
const wasmWith = ({ fields = ['$'], mod = 'a', typeIndex = 0 } = {}) => validated(moduleBytes({
  types: [T_I32_I32, T_VOID_I32, T_TWO_I32_I32],
  imports: fields.map((f) => impFunc(mod, f, typeIndex)),
}));

// ---------------------------------------------------------------------------
// 1. The exports the comparator is contracted to provide.
// ---------------------------------------------------------------------------
test('the comparator exports the four helpers the table is judged with', () => {
  for (const [helper, fn] of Object.entries({
    normalizedMap, normalizedMarkdown, compareMaps, main, engineRejects,
  })) {
    assert.equal(typeof fn, 'function', `${helper} must be exported as a function`);
  }
  for (const [helper, fn] of Object.entries({
    decodeImportSection, decodeTypeSection, limitsDiff, listSections, readName, ulebAt,
  })) {
    assert.equal(typeof fn, 'function', `the reader must export ${helper}`);
  }
});

// ---------------------------------------------------------------------------
// 2. Masking. What is masked, and -- more importantly -- what is not.
// ---------------------------------------------------------------------------
test('normalizedMap masks moduleBytes/moduleSha256, every typeIndex and type#N in duplicate', () => {
  const source = clone(TABLE);
  source.rows[0].duplicate = '_(x2: 1 of 2: _fd_write type#2 (i32,i32) -> i32)_';
  const masked = normalizedMap(source);
  assert.equal('moduleBytes' in masked.totals, false);
  assert.equal('moduleSha256' in masked.totals, false);
  for (const row of masked.rows) assert.equal('typeIndex' in row, false);
  assert.equal(masked.rows[0].duplicate, '_(x2: 1 of 2: _fd_write type#<ordinal> (i32,i32) -> i32)_');
  assert.equal(masked.rows[1].duplicate, null, 'a null duplicate stays null, it is not stringified');
  // The input is cloned, never edited in place.
  assert.equal(TABLE.rows[0].typeIndex, 0);
  assert.equal('moduleBytes' in TABLE.totals, true);
});

test('a two-digit type#N is masked as one ordinal, not just its first digit', () => {
  assert.equal(normalizedMarkdown('| a | _(x2: 2 of 3: _x type#123 (i32) -> i32)_ |'),
    '| a | _(x2: 2 of 3: _x type#<ordinal> (i32) -> i32)_ |');
});

test('typeIndex is NOT masked by the type# rule -- the leading word boundary protects it', () => {
  // `\btype#\d+\b` must not fire inside `typeIndex`, or a renumbered column would be
  // silently invisible in a markdown comparison too.
  assert.equal(normalizedMarkdown('typeIndex 7'), 'typeIndex 7');
  assert.equal(normalizedMarkdown('the type#7 ordinal'), 'the type#<ordinal> ordinal');
});

test('normalizedMarkdown masks the VALUE of a two-cell provenance row and keeps label whitespace', () => {
  // The label, its padding, the padding before the value and the padding before the
  // closing pipe are all captured back verbatim -- only the value cell is replaced.
  assert.equal(normalizedMarkdown('| module bytes | 41757498 |'), '| module bytes | <provenance> |');
  assert.equal(normalizedMarkdown('|  module sha256  |  `abc`  |'), '|  module sha256  |  <provenance>  |');
  assert.equal(normalizedMarkdown('| module bytes |41757498|'), '| module bytes |<provenance>|');
  // The label whitespace really is load-bearing: a lost space survives the mask, so a
  // normalizer that widened to the whole row would hide exactly this edit.
  assert.notEqual(
    normalizedMarkdown('| module  bytes | 41757498 |'),
    normalizedMarkdown('| module bytes | 41757498 |'),
  );
});

test('an unknown provenance-looking cell is left alone rather than masked or thrown on', () => {
  assert.equal(normalizedMarkdown('| module bytes | 1 | 2 |'), '| module bytes | 1 | 2 |',
    'a third cell is not the documented row shape and is not masked');
  assert.equal(normalizedMarkdown('| module size | 1 |'), '| module size | 1 |',
    'a label that is not one of the two is not masked');
  assert.equal(normalizedMarkdown('| module bytes | 1'), '| module bytes | 1',
    'a row with no closing pipe is not masked');
  assert.equal(normalizedMarkdown('|module bytes|1|'), '|module bytes|<provenance>|',
    'the documented shape matches even without padding');
});

test('normalizedMarkdown rejects a non-string instead of coercing it', () => {
  for (const bad of [null, undefined, 7, {}, []]) {
    assert.throws(() => normalizedMarkdown(bad), TypeError, `expected TypeError for ${String(bad)}`);
  }
});

test('normalizedMap rejects a table that is not shaped like one', () => {
  assert.throws(() => normalizedMap(null), /not an object/);
  assert.throws(() => normalizedMap({ rows: [] }), /no totals object/);
  assert.throws(() => normalizedMap({ totals: {} }), /no rows array/);
  assert.throws(() => normalizedMap({ totals: {}, rows: [null] }), /non-object row/);
});

// ---------------------------------------------------------------------------
// 3. The comparison. Same ABI must pass; a different ABI must fail.
// ---------------------------------------------------------------------------
test('an identical table compares clean', () => {
  assert.deepEqual(compareMaps(TABLE, clone(TABLE)), []);
});

test('the renumbering pair compares clean: different module provenance and type indices only', () => {
  // This is the whole reason the mask exists. A rebuild moves every type index and
  // rewrites the module's own bytes and hash; not one signature changed.
  const committed = clone(TABLE);
  committed.totals.moduleBytes = 41757498;
  committed.totals.moduleSha256 = '2b30f5c540';
  // Swap t0 and t1: (i32)->i32 <-> ()->i32. Every reference moves with them.
  const swapped = validated(moduleBytes({
    types: [T_VOID_I32, T_I32_I32, T_TWO_I32_I32],
    imports: [impFunc('a', '$', 1), impFunc('a', 'Ob', 1), impFunc('a', 'Ea', 0), impFunc('a', 'V', 2)],
    memory: MEMORY_IMPORT,
  }));
  const after = tableFor(decodeOf(swapped));
  assert.notEqual(after.rows[0].typeIndex, committed.rows[0].typeIndex, 'the reorder must renumber');
  assert.equal(after.rows[0].signature, committed.rows[0].signature, 'and must not change a signature');
  assert.deepEqual(compareMaps(committed, after), []);
});

test('a renumbered typeIndex is not a difference, but a lost signature is', () => {
  const before = clone(TABLE);
  const reindexed = clone(TABLE);
  reindexed.rows[0].typeIndex = 99;
  assert.deepEqual(compareMaps(before, reindexed), [], 'typeIndex is module-local and masked');

  const resigned = clone(TABLE);
  resigned.rows[0].signature = '() -> void';
  const diffs = compareMaps(before, resigned);
  assert.equal(diffs.length, 1);
  assert.match(diffs[0], /rows\[0\]\.signature/);
});

test('a renamed, re-moduled or reordered import fails', () => {
  const base = clone(TABLE);
  const renamed = clone(TABLE);
  renamed.rows[1].name = 'Xy';
  assert.ok(compareMaps(base, renamed).some((d) => /rows\[1\]\.name/.test(d)));

  const remoduled = clone(TABLE);
  remoduled.importModule = 'env';
  assert.ok(compareMaps(base, remoduled).some((d) => /importModule/.test(d)));

  const reordered = clone(TABLE);
  [reordered.rows[0], reordered.rows[1]] = [reordered.rows[1], reordered.rows[0]];
  assert.ok(compareMaps(base, reordered).length > 0, 'a permutation is an ABI change, not a set');
});

test('a changed memory limit or shared flag fails, through limitsDiff and through the map', () => {
  const base = clone(TABLE);
  const roomier = clone(TABLE);
  roomier.totals.otherImports[0].limits = { min: 3072, max: 131072, shared: false };
  assert.ok(compareMaps(base, roomier).some((d) => /otherImports\[0\]\.limits\.max/.test(d)));

  const shared = clone(TABLE);
  shared.totals.otherImports[0].limits = { min: 3072, max: 65536, shared: true };
  assert.ok(compareMaps(base, shared).some((d) => /otherImports\[0\]\.limits\.shared/.test(d)));

  const smaller = clone(TABLE);
  smaller.totals.otherImports[0].limits = { min: 1, max: 65536, shared: false };
  assert.ok(compareMaps(base, smaller).some((d) => /otherImports\[0\]\.limits\.min/.test(d)));

  // "no difference" is a falsy return, and it must be falsy for equal-and-present
  // limits: the string form is reserved for a side that has NO limits at all.
  assert.equal(limitsDiff({ min: 1, max: 2, shared: false }, { min: 1, max: 2, shared: false }), null);
  assert.equal(limitsDiff({ min: 1, max: null, shared: false }, { min: 1, max: null, shared: false }), null);
  assert.match(limitsDiff({ min: 1, max: 2, shared: false }, null), /no limits/);
  assert.match(limitsDiff(null, { min: 1, max: 2, shared: false }), /no limits/);
  // A side that IS an object but carries no limit fields reports the field diff: the
  // reader never produces one, but the helper does not pretend it is absent either.
  assert.match(limitsDiff({ min: 1, max: 2, shared: false }, {}), /min 1 vs undefined/);
  assert.match(limitsDiff({ min: 1, max: 2, shared: false }, { min: 1, max: 2, shared: true }), /shared/);
});

test('a strict nested comparison: alias/bucket/forwarded/owner/unknown fields and added keys', () => {
  const base = clone(TABLE);
  const mutations = {
    alias: (t) => { t.rows[0].alias = { from: 'env.sleep', to: 'a.$' }; },
    bucket: (t) => { t.rows[0].bucket = 'runtime'; },
    forwarded: (t) => { t.rows[0].forwarded = 'yes'; },
    owner: (t) => { t.rows[0].owner = 'landed: elsewhere.ts'; },
    official: (t) => { t.rows[0].official = '_something_else'; },
    glueBindingOffset: (t) => { t.rows[0].glueBindingOffset += 1; },
    glueImplOffset: (t) => { t.rows[0].glueImplOffset += 1; },
    rule: (t) => { t.rows[0].rule = '^y'; },
    todoReason: (t) => { t.rows[0].todoReason = 'deferred'; },
    'a nested field nothing knows': (t) => { t.rows[0].seam = { depth: 1 }; },
    'a row index': (t) => { t.rows[0].index = 99; },
  };
  for (const [label, patch] of Object.entries(mutations)) {
    const changed = clone(TABLE);
    patch(changed);
    assert.ok(compareMaps(base, changed).length > 0, `${label} must be a difference`);
  }
  // A field the regenerated side GAINED is a difference even when every field the
  // committed side has still matches -- a silently added column is an ABI claim.
  const grown = clone(TABLE);
  grown.totals.newField = 1;
  assert.ok(compareMaps(base, grown).some((d) => /only in the regenerated table/.test(d)));
  const droppedBucket = clone(TABLE);
  delete droppedBucket.buckets.eh;
  assert.ok(compareMaps(base, droppedBucket).length > 0);
  const extraRow = clone(TABLE);
  extraRow.rows.push(clone(TABLE.rows[0]));
  assert.ok(compareMaps(base, extraRow).some((d) => /length/.test(d)), 'a row count change fails');
});

test('compareMaps reports a non-table input rather than throwing', () => {
  // The guard is a message on the result list, so compareMaps is safe on anything.
  assert.match(compareMaps(null, TABLE)[0], /not an object/);
  assert.match(compareMaps(TABLE, null)[0], /not an object/);
  assert.match(compareMaps({ rows: [] }, { rows: [] })[0], /no totals object/);
  // An empty-but-well-shaped pair is genuinely compared, not short-circuited: with no
  // shared keys and no rows there is nothing to differ on, and a gained key shows up.
  assert.deepEqual(compareMaps({ totals: {}, rows: [] }, { totals: {}, rows: [] }), []);
  assert.ok(compareMaps({ totals: {}, rows: [] }, { totals: { added: 1 }, rows: [] }).length > 0);
});

// ---------------------------------------------------------------------------
// 4. The independent reader. Its decode is what the table is judged against, so its
//    own primitives are pinned here.
// ---------------------------------------------------------------------------
test('the reader decodes the synthesized module exactly', () => {
  const decode = decodeOf(BASE);
  assert.deepEqual(decode.types, ['(i32) -> i32', '() -> i32', '(i32,i32) -> i32']);
  assert.deepEqual(decode.imports.map((e) => `${e.mod}.${e.name}:${e.kind}`),
    ['a.$:func', 'a.Ob:func', 'a.Ea:func', 'a.V:func', 'a.memory:memory']);
  assert.deepEqual(decode.imports[0], {
    mod: 'a', name: '$', kind: 'func', typeIndex: 0, signature: '(i32) -> i32', signatureMissing: false,
  });
  assert.deepEqual(decode.imports[4].limits, { min: 3072, max: 65536, shared: false });
  assert.equal(listSections(Buffer.from(BASE)).map((s) => s.id).join(','), '1,2');
});

test('the reader reads primitives the way the wasm format says', () => {
  assert.deepEqual(ulebAt(Buffer.from([0x00]), 0), [0, 1]);
  assert.deepEqual(ulebAt(Buffer.from([0x7f]), 0), [127, 1]);
  assert.deepEqual(ulebAt(Buffer.from([0x80, 0x01]), 0), [128, 2]);
  assert.deepEqual(ulebAt(Buffer.from([0xe5, 0x8e, 0x26]), 0), [624485, 3]);
  assert.throws(() => ulebAt(Buffer.from([0x80]), 0), /past the end/);
  assert.deepEqual(readName(Buffer.from([0x02, 0x24, 0x61]), 0), ['$a', 3]);
  assert.throws(() => readName(Buffer.from([0x05, 0x61]), 0), /past the end/);
});

test('the reader fails closed on a non-module and on a module shorter than a header', () => {
  assert.throws(() => decodeImportSection(Buffer.from('not wasm at all, not even close!!')), /magic/);
  assert.throws(() => decodeImportSection(Buffer.from([0, 0x61, 0x73, 0x6d])), /shorter than/);
});

test('an out-of-range type index is reported missing rather than passed silently', () => {
  // The ENGINE refuses this module (a type index past the section is a validation
  // error), so it is built by hand rather than through `validated()`: the reader has
  // to reach its own verdict on those bytes without borrowing the engine's.
  const drifted = moduleBytes({
    types: [T_I32_I32],
    imports: [impFunc('a', '$', 0), impFunc('a', 'Ob', 3)],
  });
  assert.equal(WebAssembly.validate(drifted), false, 'the engine refuses it, as expected');
  const decode = decodeOf(drifted);
  assert.equal(decode.imports[1].signature, undefined);
  assert.equal(decode.imports[1].signatureMissing, true);
  assert.equal(decode.imports[0].signatureMissing, false, 'the in-range row is unaffected');
});

test('the reader decodes every non-function import kind with its exact limits', () => {
  const withTable = validated(moduleBytes({
    types: [T_I32_I32],
    imports: [
      impFunc('a', '$', 0),
      bytes(name('a'), name('tbl'), 0x01, 0x70, 0x01, i32(2), i32(4)),
      impMemory('a', 'memory', 0x01, 3072, 65536),
      impMemory('env', 'shared_mem', 0x03, 1, 8),
    ],
  }));
  const limits = decodeOf(withTable).imports.map((e) => `${e.name}:${e.kind}:${JSON.stringify(e.limits ?? null)}`);
  assert.deepEqual(limits, [
    '$:func:null',
    'tbl:table:{"min":2,"max":4,"shared":false}',
    'memory:memory:{"min":3072,"max":65536,"shared":false}',
    'shared_mem:memory:{"min":1,"max":8,"shared":true}',
  ]);
});

test('a module with no max byte reads max as null, not as zero', () => {
  const noMax = validated(moduleBytes({
    types: [T_I32_I32], imports: [], memory: impMemory('a', 'm', 0x00, 1),
  }));
  const [mem] = decodeOf(noMax).imports;
  assert.deepEqual(mem.limits, { min: 1, max: null, shared: false });
});

test('the reader fails closed on an unknown import kind', () => {
  // An unknown kind is also a module the ENGINE refuses (0x05 is not in the
  // import-descriptor grammar), so this fixture is deliberately not validated.
  const unknownKind = moduleBytes({
    types: [T_I32_I32],
    imports: [bytes(name('a'), name('weird'), 0x05)],
  });
  assert.equal(WebAssembly.validate(unknownKind), false, 'the engine refuses it, as expected');
  assert.throws(() => decodeImportSection(unknownKind), /unknown kind 0x5/);
});

test('decodeTypeSection refuses a section whose entry is not a func type', () => {
  assert.deepEqual(decodeTypeSection(Buffer.from(BASE)), ['(i32) -> i32', '() -> i32', '(i32,i32) -> i32']);
  const corrupt = new Uint8Array(Buffer.from(BASE));
  corrupt[corrupt.indexOf(0x60)] = 0x61;
  assert.throws(() => decodeTypeSection(corrupt), /not a func type/);
});

// ---------------------------------------------------------------------------
// 4b. The engine's own verdict, which the CLI asks for before it decodes. It is what
//     makes the decoder a checker of valid modules rather than a parser that has to be
//     trusted about invalid ones -- and it catches the two things the reader does NOT
//     look at, the magic and the version, plus any truncation inside a section.
// ---------------------------------------------------------------------------
test('engineRejects accepts a real module and refuses a bad magic, version or truncation', () => {
  assert.equal(engineRejects(Buffer.from(BASE)), null, 'a valid module passes the engine');

  // The version field is the point: this reader never reads bytes 4..8, so a module
  // that says version 2 decodes as an ordinary import list and cannot be instantiated.
  const badVersion = new Uint8Array(Buffer.from(BASE));
  badVersion[4] = 0x02;
  assert.equal(WebAssembly.validate(badVersion), false, 'the engine refuses version 2');
  assert.match(engineRejects(badVersion), /version/i);

  const badMagic = new Uint8Array(Buffer.from(BASE));
  badMagic[1] = 0x62;
  assert.equal(WebAssembly.validate(badMagic), false, 'the engine refuses a bad magic');
  assert.ok(engineRejects(badMagic) !== null, 'and engineRejects reports it');

  const truncated = Buffer.from(BASE).subarray(0, BASE.length - 3);
  assert.equal(WebAssembly.validate(truncated), false, 'the engine refuses a truncated module');
  assert.ok(engineRejects(truncated) !== null);
});

// ---------------------------------------------------------------------------
// 5. The real CLI, run as a subprocess. stdio is inherited or swallowed to a file --
//    never a captured pipe, which the sandbox refuses.
// ---------------------------------------------------------------------------
const tempRoots = [];
test.after(() => {
  for (const root of tempRoots.splice(0)) rmSync(root, { recursive: true, force: true });
});

function fixtureRoot() {
  const parent = realpathSync(tmpdir());
  const root = realpathSync(mkdtempSync(join(parent, 'online-gal-import-map-')));
  tempRoots.push(root);
  const put = (file, content) => {
    const target = resolve(root, file);
    assert.ok(target.startsWith(root + (process.platform === 'win32' ? '\\' : '/')),
      'a fixture path must stay inside its own mkdtemp root');
    mkdirSync(dirname(target), { recursive: true });
    writeFileSync(target, content);
    return target;
  };
  // Never delete a computed path: the resolved absolute target is checked first.
  const verifyCleanable = () => {
    assert.equal(dirname(root), parent);
    assert.match(basename(root), /^online-gal-import-map-/);
    assert.equal(realpathSync(root), root);
  };
  return { root, put, verifyCleanable };
}

function runCli(script, args) {
  const result = spawnSync(process.execPath, [script, ...args], {
    cwd: process.cwd(), encoding: 'utf8', stdio: 'inherit',
  });
  if (result.error) throw result.error;
  return result;
}

test('the CLI accepts a matching quartet and exits 0, and --wasm agrees with the table', (t) => {
  const { root, put, verifyCleanable } = fixtureRoot();
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const json = put('imports-map.json', JSON.stringify(TABLE));
  const reg = put('imports-map.regenerated.json', JSON.stringify(TABLE));
  const mdPath = put('imports-map.md', md());
  const regMd = put('imports-map.regenerated.md', md());
  const wasmPath = put('bridge.wasm', Buffer.from(BASE));
  verifyCleanable();

  assert.equal(runCli(COMPARE, [
    '--committed', json, '--regenerated', reg, '--committed-md', mdPath, '--regenerated-md', regMd,
  ]).status, 0);
  assert.equal(runCli(COMPARE, [
    '--committed', json, '--regenerated', reg, '--committed-md', mdPath, '--regenerated-md', regMd,
    '--wasm', wasmPath,
  ]).status, 0, 'the decode must agree with the regenerated table');
});

test('the CLI exits 1 when the module disagrees with the table it is judged against', (t) => {
  const { root, put, verifyCleanable } = fixtureRoot();
  t.after(() => rmSync(root, { recursive: true, force: true }));
  // The module imports a.$ under t1, so its signature is () -> i32 while the table
  // still claims (i32) -> i32 -- the exact "a signature is a claim about bytes" case.
  const json = put('map.json', JSON.stringify(TABLE));
  const reg = put('map.regenerated.json', JSON.stringify(TABLE));
  const mdPath = put('map.md', md());
  const regMd = put('map.regenerated.md', md());
  const wasmPath = put('drifted.wasm', Buffer.from(wasmWith({ fields: ['$'], typeIndex: 1 })));
  verifyCleanable();

  assert.equal(runCli(COMPARE, [
    '--committed', json, '--regenerated', reg, '--committed-md', mdPath, '--regenerated-md', regMd,
    '--wasm', wasmPath,
  ]).status, 1, 'a module that contradicts the table is exit 1');
});

test('the CLI exits 1 on a markdown difference and 2 on missing or empty inputs', (t) => {
  const { root, put, verifyCleanable } = fixtureRoot();
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const json = put('map.json', JSON.stringify(TABLE));
  const mdPath = put('map.md', md());
  const regMd = put('map.regenerated.md', md(['| module sha256 | `aaaa` | ']));
  verifyCleanable();

  // A trailing space inside a provenance row is a difference, not a formatting detail.
  assert.equal(runCli(COMPARE, [
    '--committed', json, '--regenerated', json, '--committed-md', mdPath, '--regenerated-md', regMd,
  ]).status, 1);

  assert.equal(runCli(COMPARE, []).status, 2, 'no inputs is a usage error');
  assert.equal(runCli(COMPARE, [
    '--committed', json, '--regenerated', json, '--committed-md', mdPath, '--regenerated-md', '',
  ]).status, 2, 'an empty path is a usage error');
  assert.equal(runCli(COMPARE, [
    '--committed', json, '--regenerated', json, '--committed-md', mdPath, '--regenerated-md', mdPath,
    '--wasm', '',
  ]).status, 2, '--wasm present with an empty value is a usage error');
  assert.equal(runCli(COMPARE, [
    '--committed', json, '--regenerated', json, '--committed-md', mdPath, '--regenerated-md', mdPath,
    '--wasm', '--committed',
  ]).status, 2, '--wasm followed by another option names no module');
  assert.equal(runCli(COMPARE, ['--committed', json]).status, 2);
});

test('the CLI exits 1 on unreadable input and on an undecodable module', (t) => {
  const { root, put, verifyCleanable } = fixtureRoot();
  t.after(() => rmSync(root, { recursive: true, force: true }));
  const json = put('map.json', JSON.stringify(TABLE));
  const mdPath = put('map.md', md());
  const garbage = put('garbage.wasm', Buffer.from('this is not a wasm module at all, truly'));
  const badJson = put('bad.json', '{ not json');
  verifyCleanable();

  assert.equal(runCli(COMPARE, [
    '--committed', join(root, 'does-not-exist.json'), '--regenerated', json,
    '--committed-md', mdPath, '--regenerated-md', mdPath,
  ]).status, 1, 'a file that cannot be read is a mismatch, not a crash');
  assert.equal(runCli(COMPARE, [
    '--committed', badJson, '--regenerated', json, '--committed-md', mdPath, '--regenerated-md', mdPath,
  ]).status, 1, 'a table that is not JSON is exit 1');
  assert.equal(runCli(COMPARE, [
    '--committed', json, '--regenerated', json, '--committed-md', mdPath, '--regenerated-md', mdPath,
    '--wasm', garbage,
  ]).status, 1, 'a module that does not decode is exit 1');
});

test('importing the helpers has no CLI side effects', () => {
  // `main` is only called when the file is the process entry point. If that guard
  // broke, every import of the comparator would try to parse argv and exit.
  const probe = spawnSync(process.execPath, ['--input-type=module', '-e', `
    const m = await import(${JSON.stringify(pathToFileURL(COMPARE).href)});
    if (typeof m.compareMaps !== 'function') throw new Error('helpers missing');
    if (typeof m.main !== 'function') throw new Error('main missing');
    process.stdout.write('imported-clean\\n');
  `], { encoding: 'utf8' });
  assert.equal(probe.status, 0, probe.stderr);
  assert.match(probe.stdout, /imported-clean/);
});

test('the verifier CLI refuses a bad invocation without reading anything', () => {
  const result = spawnSync(process.execPath, [VERIFY], { encoding: 'utf8' });
  assert.equal(result.status, 2, 'no arguments is a usage error');
  assert.match(result.stderr, /usage:/);
});
