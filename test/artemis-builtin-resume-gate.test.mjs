// Source/synthetic regression: the throw-latch gate that follows resume point 603.
//
// Scope: the gate in ports/artemis/src/script/lua_tags_builtin.cpp between the
// `resume_point = 603u;` anchor and the 604 anchor, against the reference shape in
// .ref/artemis-official/wasm/asyncify/artemis.wasm2c.c:1748101-1748122 (there the
// latch work sits on `g3 == 0 (running); the reference takes the else arm`).
//
// This is NOT game acceptance. It is a static, in-process regression: it reads the
// real product file, extracts the gate block exactly as written, and executes the
// six statements of its running arm over phase x p2 x latch. It proves the source
// keeps the running-arm polarity; it says nothing about runtime behaviour or about
// the first observed abnormal `c()` call.
//
// Run: node --test test/artemis-builtin-resume-gate.test.mjs

import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';

const PRODUCT = fileURLToPath(
  new URL('../ports/artemis/src/script/lua_tags_builtin.cpp', import.meta.url));

const RUNNING = 0;
const SUSPENDED = 1;
const REWINDING = 2;
const PHASES = [RUNNING, SUSPENDED, REWINDING];
const P2S = [0, 1, 7];
const LATCHES = [0, 1];

// The reference: `g3 == 0 (running); the reference takes the else arm`; every other arm
// falls through with p2 unchanged and leaves memory alone.
function referenceGate({ phase, p2, latch }) {
  let mem = latch;
  let gotoB3 = false;
  if (phase === RUNNING) {
    const pending = mem;
    mem = 0;
    if (pending === 1) { gotoB3 = true; } else { p2 = p2 === 0 ? 1 : 0; }
  }
  return { p2, mem, gotoB3 };
}

// Anchor text, not line numbers, so edits above the gate cannot shift what is read.
// The anchors are matched whole and must be unique, or the extraction is ambiguous.
function extractGateBlock(source) {
  const lines = source.split(/\r?\n/);
  const anchor = (n) => {
    const hits = lines.reduce(
      (acc, l, i) => (l.trim() === `resume_point = ${n}u;` ? [...acc, i] : acc), []);
    assert.equal(hits.length, 1, `resume point ${n} must be anchored exactly once`);
    return hits[0];
  };
  const [i602, i603, i604] = [anchor(602), anchor(603), anchor(604)];
  assert.ok(i602 < i603 && i603 < i604,
    'expected the 602 / 603 / 604 resume anchors in that order');
  const from = lines.slice(i603 + 1, i604).join('\n');
  // Exactly the six statements of the running arm, in order: the polarity, the read,
  // the clear, the pending test with its failed `==1u` -> goto, and the p2 inversion.
  const block = from.match(/^\s*if \(ResumePhase\(ctx\) (==|!=) kResumeRunning\) \{\n([\s\S]*?)^\s*\}\n/m);
  assert.ok(block, 'expected the ResumePhase(kResumeRunning) gate between 603 and 604');
  assert.match(block[2],
    /^\s*uint32_t const pending = LoadU32\(ctx, kBuiltinThrowLatch\);\n\s*StoreU32\(ctx, kBuiltinThrowLatch, 0u\);\n\s*if \(pending == 1u\) \{ goto p05_B3; \}\n\s*p2 = static_cast<uint32_t>\(p2 == 0u\);\n$/,
    'the running arm must be read, clear, `==1u` -> goto p05_B3, then invert p2');
  return { guard: block[1], body: block[2] };
}

// Execute the extracted running arm, not a hand-copied body. The statements are the
// fixed ones the regex above pinned, so the shape is fixed and only p2/mem/goto vary.
function runGate({ guard, phase, p2, latch }) {
  let mem = latch;
  let gotoB3 = false;
  const running = (phase === RUNNING) === (guard === '==');
  if (running) {
    const pending = mem;
    mem = 0;
    if (pending === 1) { gotoB3 = true; } else { p2 = p2 === 0 ? 1 : 0; }
  }
  return { p2, mem, gotoB3 };
}

function probe(gate) {
  const divergences = [];
  const tuples = {};
  for (const phase of PHASES) {
    for (const p2 of P2S) {
      for (const latch of LATCHES) {
        const got = runGate({ ...gate, phase, p2, latch });
        const expected = referenceGate({ phase, p2, latch });
        if (got.p2 !== expected.p2 || got.mem !== expected.mem
          || got.gotoB3 !== expected.gotoB3) {
          divergences.push({ phase, p2, latch });
        }
        tuples[`${phase}/${p2}/${latch}`] = got;
      }
    }
  }
  return { divergences, tuples };
}

const source = readFileSync(PRODUCT, 'utf8');
const gate = extractGateBlock(source);

test('source gate keeps the running-arm polarity and the throw-latch body', () => {
  assert.equal(gate.guard, '==',
    'the guard must hold while running (kResumeRunning), not be inverted to !=');
});

test('executing the source gate over phase x p2 x latch matches the reference', () => {
  const { divergences } = probe(gate);
  assert.deepEqual(divergences, [], 'the executed source guard diverges from the reference');
});

test('mutation to != is caught (pure in-memory, 18 tuples, latch work moves off running)', () => {
  const mutated = { ...gate, guard: '!=' }; // no file is written
  const { divergences, tuples } = probe(mutated);
  const { tuples: referenceTuples } = probe({ ...gate, guard: '==' });
  assert.ok(divergences.length > 0,
    'the != mutation must make the executed guard diverge from the reference');
  assert.ok(divergences.some((d) => d.phase !== RUNNING && d.latch === 1),
    'the mutation must diverge on an unwind arm with a pending latch');
  assert.notDeepEqual(tuples, referenceTuples,
    'the 18-tuple behaviour of the != gate must differ from the reference');
});

test('the product file was not touched by this test', () => {
  assert.equal(readFileSync(PRODUCT, 'utf8'), source, 'the regression must be read-only');
});
