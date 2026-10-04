// Source regression: resume-scope closure and the 616/617/618 two-gate branch targets, read from the
// real product with a comment/string-aware brace scanner. Asserts each resumed scope around the
// 603/609/616/617/719 sites closes before the statement that follows it, that the 616 throw gate
// targets p05_B4 while a type-false result falls through to the new p04_B616_lookup_done label
// ahead of the filename cleanup, and that the 617 lookup window (617..618) keeps its own call and
// pending->p05_B3 arm scoped to that window only. Anchors are statements, never line numbers;
// mutations are in memory only. NOT game acceptance.
// Run: node --test test/artemis-builtin-control-flow.test.mjs

import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';

const PRODUCT = fileURLToPath(new URL('../ports/artemis/src/script/lua_tags_builtin.cpp', import.meta.url));
const RUN = 'if (ResumePhase(ctx) == kResumeRunning) {';
const P2 = 'if (p2 != 0u || ResumePhase(ctx) == kResumeRewinding) {';
const P0 = 'if (p0 != 0u || ResumePhase(ctx) == kResumeRewinding) {';
const CALL617 = 'ctx, 1354u, p1, 0u, 0xFFFFFFFFu, 743830u, 8u);';
const CLEANUP = 'The embedded "filename" bytes are released once they are no longer used.';
const B3_ARM = 'if (pending == 1u) { goto p05_B3; }';

// open line -> closing line, skipping //, /* */, "..." and '...'. A close with no open is a structural defect, not a key swallowed as `undefined`; it throws, as does an unclosed open.
function pair(src) {
  const closers = new Map(), opens = [];
  let line = 1, state = 'code', end = '';
  for (let i = 0; i < src.length; i++) {
    const c = src[i], nx = src[i + 1] || '';
    if (c === '\n') { if (state === 'line') state = 'code'; line++; continue; } if (state === 'line') continue;
    if (state === 'block') { if (c === '*' && nx === '/') { state = 'code'; i++; } continue; } if (state === 'str') { if (c === '\\') i++; else if (c === end) state = 'code'; continue; }
    if (c === '/' && nx === '/') { state = 'line'; i++; continue; } if (c === '/' && nx === '*') { state = 'block'; i++; continue; }
    if (c === '"' || c === "'") { state = 'str'; end = c; continue; }
    if (c === '{') opens.push(line);
    else if (c === '}') { assert.ok(opens.length, `unmatched close at line ${line}`); closers.set(opens.pop(), line); }
  }
  assert.equal(opens.length, 0, `unclosed open at line ${opens[0]}`); return closers;
}
// Anchor statements must be unique; a duplicate makes the extraction ambiguous.
function anchor(L, needle) {
  const hits = L.flatMap((t, i) => (t.includes(needle) ? [i + 1] : []));
  assert.equal(hits.length, 1, `anchor not unique (${hits.length} hits): ${needle}`); return hits[0];
}
// Nearest whole-line match at or before `from` when dir=-1, at or after when dir=1.
function find(L, text, from, dir = 1) {
  for (let i = from - 1; i >= 0 && i < L.length; i += dir) if (L[i].trim() === text) return i + 1; return 0;
}
// First non-empty, trimmed line strictly after line `from`; 0 when there is none.
function nextNonEmpty(L, from) {
  for (let i = from; i < L.length; i++) if (L[i].trim()) return { line: i + 1, text: L[i].trim() }; return { line: 0, text: '' };
}
// The 617 lookup window: g617's guard line through the line before g618's guard. Scoped so that a
// B3 arm elsewhere in the (thousands-of-lines) 617..719 span cannot satisfy the invariant.
function window617(L, g617, g618) { return L.slice(g617 - 1, g618 - 1); }
// Throws on any violated invariant; returns the names of the invariants that held.
function validate(src) {
  const L = src.split('\n'), closer = pair(src), held = [], A = (n) => anchor(L, n), at = (l) => closer.get(l);
  const g603 = A('resume_point == 603u'), g609 = A('resume_point == 609u'), g616 = A('resume_point == 616u');
  const g617 = A('resume_point == 617u'), g618 = A('resume_point == 618u'), g632 = A('resume_point == 632u'), g719 = A('resume_point == 719u');
  const lab = A('p04_L3728:;'), b3724 = A('p04_B3724:;'), b3736 = A('p04_B3736:;');
  const b4367 = A('p04_B4367:;'), b2011 = A('p05_B2011:;');
  const ok = (name, cond, detail) => (assert.ok(cond, `${name} (${detail})`), held.push(name));
  // 1-3: each resumed scope closes before the statement that follows it. 4: the 719 cleanup body
  // still sits inside its own running scope. 5: after the B2011 guard close exactly one outer
  // close follows. 6: the 632 StringLength region carries no doubled running-guard open.
  const outer = find(L, P2, g603), chain = find(L, P2, lab, -1), runChain = find(L, RUN, b3736, -1);
  ok('p2-closed-before-616', at(outer) < g616, `${outer}->${at(outer)} vs ${g616}`);
  ok('chain-p2-closed-before-B3724', at(chain) < b3724, `${chain}->${at(chain)} vs ${b3724}`);
  ok('chain-running-closed-before-609', at(runChain) < g609, `${runChain}->${at(runChain)} vs ${g609}`);
  const run719 = find(L, RUN, g719), f224 = A('StoreU32(ctx, frame + 224u, LoadU32(ctx, frame + 224u) + 1u);');
  const gb11 = find(L, 'goto p05_B11;', b4367);
  ok('719-body-inside-running-scope', at(run719) > gb11 && at(run719) > f224, `${run719}->${at(run719)} vs B11@${gb11}/224@${f224}`);
  const gclose = at(find(L, RUN, b2011)); let k = 0;
  for (let i = gclose; i < L.length; i++) { const t = L[i].trim(); if (!t) continue; if (t !== '}') break; k++; }
  ok('one-outer-close-after-B2011', k === 1, `${k} further closes from ${gclose}`);
  const p0outer = find(L, P0, g632, -1); let dup = 0;
  for (let i = p0outer; i < g632; i++) if (L[i].trim() === RUN && L[i + 1].trim() === RUN) dup = i + 1;
  ok('no-doubled-running-open-at-632', dup === 0, dup ? `doubled ${dup}/${dup + 1}` : 'none');
  // 616: the throw gate retargets to the p05_B4 merge; the type-false arm reaches the new normal
  // label, and that label precedes the filename cleanup, whose own running guard is the very next
  // non-empty statement. 617 owns a window that ends where 618's guard begins.
  const seg = L.slice(g616 - 1, g617 - 1).join('\n'), win617 = window617(L, g617, g618);
  ok('616-pending1-goto-B4', seg.includes('if (pending == 1u) { goto p05_B4; }')
    && !seg.includes('if (pending != 1u) { goto p04_B3721; }'), 'running arm targets p05_B4');
  ok('616-typefalse-new-label', seg.includes('goto p04_B616_lookup_done;') && !seg.includes('goto p04_B3720;'),
    'type-false falls to the normal label, not B3720');
  const label616 = A('p04_B616_lookup_done:;'), nxt = nextNonEmpty(L, label616);
  ok('616-label-precedes-filename-cleanup', label616 < A(CLEANUP), 'label before cleanup');
  // 12-13: the label is not just before the cleanup somewhere: its next non-empty statement is the
  // running guard, and the cleanup comment sits within the following three lines of that guard.
  ok('616-label-next-nonempty-is-running-guard', nxt.text === RUN, `next@${nxt.line}=${nxt.text.slice(0, 32)}`);
  ok('616-cleanup-comment-follows-that-guard', A(CLEANUP) - nxt.line <= 2, `cleanup@${A(CLEANUP)} vs guard@${nxt.line}`);
  // 13: the scoped 617 window carries exactly one B3 arm and it is the one in its own pending gate;
  // a B3 arm anywhere else in the 617..719 span does not satisfy this.
  const b3 = win617.flatMap((t, i) => (t.trim() === B3_ARM ? [g617 + i] : []));
  ok('617-window-single-pending-B3-arm', b3.length === 1
    && win617.some((t) => t.includes('uint32_t const pending = LoadU32(ctx, kBuiltinThrowLatch);')),
    b3.length ? `arms at ${b3.join(',')}` : 'no B3 arm in 617..618 window');
  ok('617-call-retained', win617.join('\n').includes(CALL617), 'file_crc lookup call retained');
  ok('616-no-zero-pointer-catch-shortcut',
    !/if \(p2 == 0u\) \{ goto p05_B4; \}/.test(L.slice(g616 - 1, g618 - 1).join('\n')),
    'normal cleanup must not manufacture an exception on a zero result');
  return held;
}

const source = readFileSync(PRODUCT, 'utf8');
test('product control flow closes every resumed scope and retargets the 616 gates', () => {
  assert.equal(validate(source).length, 14, 'every invariant is exercised');
});
// Each mutation edits an in-memory copy only; validate() must throw.
const mutations = {
  'duplicate running open before the p04_B3733 statements': (s) => s.replace('p04_B3733:;\n', `p04_B3733:;\n                      ${RUN}\n`),
  'old close restored above the B4367 merge point': (s) => s.replace('              // Merge point p04_B4367.', '              }\n              // Merge point p04_B4367.'),
  '616 throw gate flipped back to != p04_B3721': (s) => s.replace('if (pending == 1u) { goto p05_B4; }', 'if (pending != 1u) { goto p04_B3721; }'),
  '616 type-false fallback reverted to p04_B3720': (s) => s.replace('goto p04_B616_lookup_done;', 'goto p04_B3720;'),
  'zero result incorrectly forced into the catch path': (s) => s.replace(
    '                  // The embedded "filename" bytes are released once they are no longer used.',
    '                  if (p2 == 0u) { goto p05_B4; }\n                  // The embedded "filename" bytes are released once they are no longer used.'),
  'label separated from the filename cleanup by an extra arm': (s) => {
    const i = s.indexOf('p04_B616_lookup_done:;\n');
    assert.notEqual(i, -1, 'the label must exist');
    return s.slice(0, i + 'p04_B616_lookup_done:;\n'.length)
      + '                  p2 = LoadU32(ctx, frame + 240u);\n' + s.slice(i + 'p04_B616_lookup_done:;\n'.length);
  },
  // The 617 pending arm is replaced by an early-exit guard in its own window only: the B3 arm is
  // gone from 617..618, which the scoped count rejects even though the same text survives elsewhere.
  '617 pending arm retargeted inside the 617 window': (s) => {
    const i = s.indexOf('p04_B616_lookup_done:;'), j = s.lastIndexOf(B3_ARM, i);
    assert.ok(j > -1 && j < i, 'the 617 B3 arm must precede the label');
    return s.slice(0, j) + 'if (p2 == 0u) { goto p04_B617_slow; }' + s.slice(j + B3_ARM.length);
  },
};
for (const [name, mutate] of Object.entries(mutations)) {
  test(`mutation is caught: ${name}`, () => {
    const mutated = mutate(source);
    assert.notEqual(mutated, source, 'the mutation must actually change the source');
    assert.throws(() => validate(mutated), 'validate must reject the mutated source'); });
}
test('the product file was not touched by this test', () => {
  assert.equal(readFileSync(PRODUCT, 'utf8'), source, 'the regression must be read-only'); });
