/**
 * Fidelity regression for `ports/artemis/src/render/compositor_hide.cpp`.
 *
 * Three rewrites in this hand-translated file are easy to get subtly wrong, and
 * each one was wrong at some point during the port:
 *
 *  1. A byte-sign test.  The original does `i32_load8_s` then compares `< 0`.
 *     Writing that as `(x << 24) >> 24 < 0u` in C++ is *always false*: `x` is a
 *     `uint32_t`, so the shift is unsigned and the promotion back to `int32_t`
 *     happens after the compare.  The signed low-byte test is `(x & 0x80u) != 0u`.
 *     The first half of the file keeps the legitimate *signed-narrowing* form
 *     (`static_cast<int32_t>(x << 24) >> 24 < 0`), which is a different shape and
 *     must not be touched.
 *
 *  2. An inline select feeding a call.  The original loads a slot, chooses
 *     pointer-or-inline, and calls `f220` (= `artc::SkipWhitespace`, see
 *     `ports/artemis/rewrite-names.tsv`) with that temporary.  It leaves `p2`
 *     holding the *original* address and stores the call result in the callee-saved
 *     local `l8`, not in the register the product names `word`.  Hoisting the
 *     select into `p2` and writing `word = ...` is unfaithful: `word` is the node
 *     payload register (it is stored to field 20), so clobbering it corrupts the
 *     node instead of just skipping whitespace.
 *
 *  3. Alloc gates must keep the resumable order alloc -> suspend -> store the
 *     destination, so a resumed frame cannot read an uninitialised pointer.
 *
 * The test reads the product as text.  It needs no compiler, no WASM and no game
 * data, so it runs in a fresh clone.
 */
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

const PRODUCT = fileURLToPath(
  new URL('../ports/artemis/src/render/compositor_hide.cpp', import.meta.url),
);
const source = readFileSync(PRODUCT, 'utf8');
const lines = source.split('\n');

/** Collapses whitespace so shape assertions survive reformatting. */
const flat = (text) => text.replace(/\s+/g, ' ').trim();
const flatLines = lines.map(flat);
const count = (needle) => flatLines.filter((line) => line.includes(needle)).length;

const INLINE_SELECT = 'aux = SkipWhitespace(ctx, tmp ? LoadU32(ctx, p2) : p2);';

test('the signed low-byte test never uses the unsigned-shift false negative', () => {
  const badshape = /static_cast<int32_t>\(\(\(.*<< 24\) >> 24\)\) < 0u/;
  const offenders = flatLines
    .map((line, index) => [line, index + 1])
    .filter(([line]) => badshape.test(line));
  assert.deepEqual(
    offenders,
    [],
    `always-false byte-sign tests:\n${offenders.map(([l, n]) => `  ${n}: ${l}`).join('\n')}`,
  );
});

test('exactly sixteen byte-sign tests use the signed low-byte form', () => {
  const regex = /static_cast<uint32_t>\(\((\w+) & 0x80u\) != 0u\)/;
  const found = {};
  for (const line of flatLines) {
    const match = regex.exec(line);
    if (match) found[match[1]] = (found[match[1]] ?? 0) + 1;
  }
  assert.deepEqual(found, { aux: 1, block: 2, p1: 1, p2: 5, span: 7 });
  assert.equal(Object.values(found).reduce((sum, n) => sum + n, 0), 16);
});

test('the legitimate signed-narrowing form in the first half is preserved', () => {
  // `static_cast<int32_t>(x << 24) >> 24 < 0` really does sign-extend the low byte.
  const signedNarrowing = flatLines.filter((line) =>
    /static_cast<int32_t>\(.*<< 24\) >> 24 < 0\)/.test(line));
  assert.equal(signedNarrowing.length, 53);
});

test('the three SkipWhitespace select sites pass the temporary inline', () => {
  assert.equal(count(INLINE_SELECT), 3);
  // No select may be hoisted into p2 ahead of the call.
  const hoisted = flatLines.filter((line) =>
    line === 'p2 = (tmp != 0u) ? LoadU32(ctx, p2) : p2;');
  assert.equal(
    hoisted.length,
    1,
    'exactly one hoisted select remains, and it is the UnorderedLookupNumber path, ' +
      'not a SkipWhitespace path',
  );
});

test('the SkipWhitespace result is never written to the node payload register', () => {
  const clobber = flatLines.filter((line) => /word = SkipWhitespace\(/.test(line));
  assert.deepEqual(clobber, []);
});

test('the select sites keep the operand addresses live', () => {
  const selects = flatLines
    .map((line, index) => [line, index])
    .filter(([line]) => line === INLINE_SELECT);
  assert.equal(selects.length, 3);
  for (const [line, index] of selects) {
    assert.match(line, /\? LoadU32\(ctx, p2\) : p2\)/,
      `line ${index + 1} lost its memory operand`);
    // The address register is rebuilt from memory, and the byte-sign test that
    // decides the select sits immediately before it.
    assert.equal(flatLines[index - 2], 'p2 = tmp + 28u;',
      `line ${index + 1} no longer rebuilds the address from tmp + 28u`);
    assert.equal(
      flatLines[index - 1],
      'tmp = static_cast<uint32_t>(static_cast<int32_t>(LoadS8(ctx, tmp + 39u)) <= -1);',
      `line ${index + 1} lost its byte-sign discriminator`,
    );
  }
});

test('the alloc gates keep alloc -> suspend -> destination order', () => {
  const alloc = flatLines.filter((line) => /^result = AllocateStringBuffer\(ctx, .*\);$/.test(line));
  assert.equal(alloc.length, 9);

  // Eight of the nine are resumable gates.  The original keeps one destination
  // register live across the suspend (f201's result overwrites p0 inside the
  // resumable arm), so the resumed frame reads an initialised pointer.
  const gates = flatLines
    .map((line, index) => [line, index])
    .filter(([line]) => line === 'result = AllocateStringBuffer(ctx, p0);');
  assert.equal(gates.length, 8);
  for (const [line, index] of gates) {
    const window = flatLines.slice(index, index + 12).join(' ');
    assert.match(window, /resume_point = \d+u;/,
      `alloc at line ${index + 1} has no resume point`);
    assert.match(window, /if \(ResumePhase\(ctx\) == kResumeSuspended\) \{ goto suspend_point; \}/,
      `alloc at line ${index + 1} has no suspend check`);
    assert.match(window, /if \(ResumePhase\(ctx\) == kResumeSuspended\) \{ goto suspend_point; \} p0 = result;/,
      `alloc at line ${index + 1} does not bind the result to p0 inside the guard, ` +
        'so a resumed frame reads an uninitialised pointer');
  }
  // No stranded self-assignment survives the rewrite, and the one non-resumable
  // alloc stores its result directly instead.
  assert.equal(count('p0 = p0;'), 0);
  assert.equal(count('block = result;'), 1);
});

test('the six CompareBytes and the index fold keep their product counts', () => {
  assert.equal(count('= CompareBytes(ctx,'), 6);
  const fold = flatLines.filter((line) =>
    line.includes('static_cast<int32_t>(span) / static_cast<int32_t>(36u)'));
  assert.equal(fold.length, 8);
  // The 32-byte pair walk became a 36-byte node walk, so no scaling still uses 32.
  const scalesBy32 = flatLines.filter((line) => /[*/] 32u\b/.test(line));
  assert.deepEqual(scalesBy32, []);
  // Every scaling that multiplies a node index is by 36.
  assert.equal(count('* 36u'), 32);
});

test('the product stays a single well-formed translation unit', () => {
  assert.equal(count('EnterFrame();'), 1);
  assert.equal(count('LeaveFrame();'), 1);
  assert.ok(source.endsWith('\n'), 'product must end with a newline');
  assert.equal(/\r/.test(source), false, 'product must not contain CR');
});
