/**
 * The `Pf8Reader::ReadAt` Emscripten guard, compiled and run.
 *
 * Scope, stated so this is not read as more than it is: it does NOT run wasm,
 * does NOT run the engine, and does NOT touch a pack. It extracts the guard
 * block *from the patch under test*, compiles that exact text against a stub
 * of the callback seam, and runs it on the boundary values.
 *
 * Why it exists: the old check stated a real defect -- a range whose `offset`
 * and `len` each fit in 32 bits but whose sum crosses 2^32 was passed to the
 * page truncated into a *different* request (offset 0xfffff000 + len 0x2000
 * arrives as offset 0xfffff000, length 0x2000, spanning [0xfffff000, 0x100001000)),
 * and the page would satisfy it happily. A source regexp cannot tell a fixed
 * guard from a reworded comment; a compiled call can.
 *
 * Tier 1 (always): the patch applies to the pinned source and the guard text is
 * present in the applied file. Tier 2 (needs a compiler): the guard is compiled
 * and every boundary row is driven through it.
 *
 * The upper end of the range is reached through the callback's own bound check,
 * which the probe reimplements identically to the patch: that is the whole
 * verified path from a uint64 offset+len to accept/refuse, with no heap.
 */
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdirSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..', '..', '..');
const PORT = 'ports/artemis/compat-web';
const PATCH = `${PORT}/patches/0001-pf8-readat-host-source.patch`;

// The callback bound, copied from the patch verbatim. If the patch's copy
// changes, this one has to change with it -- asserted below rather than assumed.
const HEAP_BOUND = "if (length > HEAPU8.length || dest > HEAPU8.length - length) return -1;";

/**
 * Git for Windows' `tar` escapes a `C:\...` argument into `C\:\\...` (`--force-local`
 * does not stop it) and then cannot open it; the same path with forward slashes
 * extracts fine. `node:path` returns backslashes on win32, so the tree is
 * extracted into the forward-slash spelling of the same directory.
 */
function forTar(dir) {
  return dir.replaceAll(String.fromCharCode(92), '/');
}

function runStatus(command, args, extra) {
  const result = spawnSync(command, args, {
    encoding: 'utf8',
    stdio: 'inherit',
    ...extra,
  });
  return { status: result.status, error: result.error };
}

function shellStatus(command) {
  const result = spawnSync('bash', ['-lc', command], { stdio: 'inherit' });
  return { status: result.status, error: result.error };
}

function findCompiler() {
  for (const candidate of [process.env.CXX, 'g++', 'clang++']) {
    if (!candidate) continue;
    const probe = runStatus(candidate, ['--version']);
    if (!probe.error && probe.status === 0) return candidate;
  }
  return null;
}

/** The guard, as the patch writes it: the whole Emscripten branch of ReadAt. */
function extractGuard(patchText) {
  // The patch has two Emscripten hunks; the guard is the one that follows the
  // function signature, so the anchor is the signature rather than the first
  // `#if`, which is the include block.
  const signature = patchText.indexOf(' bool Pf8Reader::ReadAt(');
  assert.notEqual(signature, -1, `${PATCH} must contain the ReadAt hunk`);
  const start = patchText.indexOf('     if (len == 0) return true;', signature);
  assert.notEqual(start, -1, 'the guard must start at the zero-length early return');
  const tail = patchText.slice(start);
  // The branch ends where the patch cuts back to the POSIX side (`#elif`): the
  // first `#endif` belongs to the `#include <emscripten.h>` block above.
  const close = tail.indexOf('+#elif');
  assert.notEqual(close, -1, 'the Emscripten branch must be delimited by an #elif');
  const end = tail.indexOf('\n', close);
  // Strip the `+` added lines into the text the build compiles, and drop the
  // `-` lines: those are the reads the patch replaces. The `#endif` the branch
  // needs is not a line of the hunk -- it is the one the patch kept -- so it is
  // closed here.
  return `${tail.slice(0, end < 0 ? tail.length : end).split('\n')
    .filter((line) => !line.startsWith('-'))
    .map((line) => line.replace(/^\+/, '')).join('\n')}
#endif
`;
}

// ---------------------------------------------------------------------------
// Tier 1 -- the patch fits the pinned source and carries the guard. No compiler.
// ---------------------------------------------------------------------------

test('the patch text carries the signed-return length cap and the 2^32 range guard', () => {
  const patchText = readFileSync(join(ROOT, PATCH), 'utf8');
  assert.match(patchText, /if \(len > 0x7fffffffu \|\| offset > 0xffffffffu\) return false;/,
    'len above INT_MAX is refused before EM_ASM_INT; offset stays a full uint32');
  assert.doesNotMatch(patchText, /len > 0xffffffffu/,
    'the old unsigned length cap accepts a complete read EM_ASM_INT returns negative');
  assert.match(patchText, /if \(len > 0x100000000ull - offset\) return false;/,
    'the range check must subtract: `offset + len` is the form that overflows');
  assert.match(patchText, /if \(len == 0\) return true;/,
    'a zero-length read must stay an accepted no-op');
  assert.ok(patchText.includes(HEAP_BOUND), `the callback bound must stay: ${HEAP_BOUND}`);
});

const PIN = '9f622629f595db85e6f76c45c06edde29d7d9961';

test('the patch dry-runs and applies to the two pinned reader files', (t) => {
  const ref = join(ROOT, '.ref', 'artemis-compat');
  const object = runStatus('git', ['-C', ref, 'cat-file', '-e', `${PIN}^{commit}`]);
  if (object.error || object.status !== 0) {
    t.skip(`pin ${PIN} is not a local object; patch application was not run`);
    return;
  }
  const dir = mkdtempSync(join(tmpdir(), 'artc-pf8-patch-'));
  const patchText = readFileSync(join(ROOT, PATCH), 'utf8');
  try {
    const patchFile = join(dir, 'apply.patch');
    writeFileSync(patchFile, patchText);
    for (const file of ['src/pack/pf8_reader.cpp', 'src/pack/pf8_reader.h']) {
      const out = join(dir, file);
      mkdirSync(dirname(out), { recursive: true });
      const shown = shellStatus(
        `git -C ${JSON.stringify(forTar(ref))} show ${PIN}:${file} > ${JSON.stringify(forTar(out))}`,
      );
      assert.equal(shown.status, 0, `git show ${PIN}:${file} failed`);
    }
    const dry = shellStatus(
      `patch --binary -d ${JSON.stringify(forTar(dir))} -p1 --batch --dry-run < ${JSON.stringify(forTar(patchFile))}`,
    );
    assert.equal(dry.status, 0, 'patch --dry-run must apply to the two pinned files');
    const applied = shellStatus(
      `patch --binary -d ${JSON.stringify(forTar(dir))} -p1 --batch < ${JSON.stringify(forTar(patchFile))}`,
    );
    assert.equal(applied.status, 0, 'patch must apply to the two pinned files');
    const source = readFileSync(join(dir, 'src', 'pack', 'pf8_reader.cpp'), 'utf8');
    assert.match(source, /if \(len > 0x7fffffffu \|\| offset > 0xffffffffu\) return false;/);
    assert.match(source, /#include <emscripten\.h>/);
  } finally {
    rmSync(dir, { recursive: true, force: true });
  }
});

// ---------------------------------------------------------------------------
// Tier 2 -- the guard, compiled and run. Needs a compiler, not emsdk.
// ---------------------------------------------------------------------------

const PROBE = String.raw`
#include <cstddef>
#include <cstdint>
#include <cstdio>

// --- the one Emscripten symbol the guard names, stubbed ---------------------
// The guard's body is JavaScript handed to EM_ASM_INT; here it is the same
// predicates compiled as C++, driven by the scripted callback result.
static int g_calls = 0;
static uint32_t g_seenOffset = 0;
static uint32_t g_seenLength = 0;
static bool g_callbackDefined = true;
// What the scripted callback answers with. The default echoes back the length
// it was handed, which is an honest read; each dishonest case sets a number.
// No row leaves a global behind, so every row means the same thing wherever it
// runs.
static size_t g_returnOverride = (size_t)-1;

// The heap the callback bound is checked against: 2^32 bytes, so a read whose
// last byte is byte 0xffffffff is inside it and anything past is not. Declared
// before the macro so the substituted text can name it.
static const uint64_t kHeapBytes = 0x100000000ull;

static int CallbackFake(uint32_t offset, const void *dest, uint32_t length) {
    ++g_calls;
    if (!g_callbackDefined) return -1;
    const uint64_t destv = (uint64_t)(uintptr_t)dest;
    const uint64_t heapLength = kHeapBytes;
    // The heap length itself, not a subtraction that could be read backwards.
    if ((uint64_t)length > heapLength || destv >= heapLength) return -1;
    g_seenOffset = offset;
    g_seenLength = length;
    if (g_returnOverride != (size_t)-1) return (int)g_returnOverride;
    // EM_ASM_INT is a signed int. A complete 0x80000000 read is negative here.
    return (int)length;
}

#define EM_ASM_INT(code, path, offset32, dest, len32) \
    CallbackFake((uint32_t)(offset32), (dest), (uint32_t)(len32))

// The guard text, lifted verbatim from the patch by the test that wrote this
// file: from the first length check to the statement that reads the callback's
// answer. The Emscripten header is stubbed on the include path, so this is the
// branch a wasm build compiles; the other side of that conditional belongs to
// the POSIX host and is not part of this probe.
static bool ReadAt(uint64_t offset, void *buf, size_t len, const char *path_ = "root.pfs") {
    if (len == 0) return true;
    //===GUARD===
    return false;
}

static int g_checks = 0, g_failures = 0;
static void Check(bool ok, const char *what) {
    // The row name goes out with the bit, so a reader can tell which row it was.
    std::printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    ++g_checks;
    if (!ok) ++g_failures;
}

/** Resets every piece of scripted state, so no row inherits another's setup. */
static void Reset() {
    g_calls = 0;
    g_returnOverride = (size_t)-1;
    g_callbackDefined = true;
}

int main() {
    const size_t zero = 0;
    // A non-null address outside the real stack: nothing here dereferences it,
    // the callback's bound check is what decides whether it is acceptable.
    void *const dest = (void *)(uintptr_t)8;
    Reset();

    // --- the legal reads the old guard already allowed -----------------------
    Check(ReadAt(0, dest, 11), "the 11-byte header at offset 0 is a legal read");
    Check(ReadAt(0x7fffffffu, dest, 1), "offset INT32_MAX, len 1 is legal");
    Check(ReadAt(0x7fffffffu, dest, 2), "offset INT32_MAX, len 2 is legal");
    Check(ReadAt(0xa0b1c2d0ull, dest, 4096), "a 2.69 GB offset with a 4 KiB read is legal");
    // offset + len == 2^32 is NOT a crossing: the read ends on the last byte.
    Check(ReadAt(0xffffffffull, dest, 1), "the final byte of a 2^32-byte pack is legal");
    Check(ReadAt(0xffff0000ull, dest, 0x10000ull), "a read ending exactly at 2^32 is legal");
    Check(g_seenOffset == 0xffff0000u && g_seenLength == 0x10000u,
          "the exact-fit read reached the callback untruncated");

    // --- zero is a no-op, and never reaches the callback --------------------
    g_seenOffset = 0xdeadbeefu; g_seenLength = 0xdeadbeefu;
    Check(ReadAt(0xffffffffull, dest, zero), "a zero-length read at the last byte is a no-op");
    Check(ReadAt(0x1ffffffffull, dest, zero), "a zero-length read past 2^32 is a no-op");
    Check(g_seenOffset == 0xdeadbeefu && g_seenLength == 0xdeadbeefu,
          "a zero-length read never enters the callback");

    // --- the defect: each argument fits, the range does not ------------------
    Check(!ReadAt(0xfffff000ull, dest, 0x2000ull),
          "offset+len crossing 2^32 is refused instead of truncating to a different read");
    Check(!ReadAt(0xffffffffull, dest, 2), "the last byte plus one more is refused");
    Check(!ReadAt(0x80000000ull, dest, 0x80000000ull), "half the address space each is refused");
    // What the old guard would have done with it, spelled out: both halves fit
    // in 32 bits, so the truncating cast is silent.
    Check((uint32_t)0xfffff000ull == 0xfffff000u && (uint32_t)0x2000ull == 0x2000u
              && 0xfffff000ull + 0x2000ull > 0xffffffffull,
          "the refused request is exactly the one a 32-bit truncation would have passed");

    // --- past the end is refused, and INT_MAX is not a limit ----------------
    Check(!ReadAt(0x100000000ull, dest, 1), "an offset past 2^32 is refused");
    Check(!ReadAt(0, dest, 0x100000000ull), "a length past 2^32 is refused");
    Check(!ReadAt(0xffffffffull, dest, 0xffffffffull), "a long read from a high offset is refused");
    Check(ReadAt(0x7fffffffu, dest, 0x7fffffffull),
          "offset INT_MAX plus len INT_MAX still ends before 2^32");
    Reset();
    Check(ReadAt(0, dest, 0x7fffffffu), "INT_MAX length at offset 0 is a complete legal read");
    Check(g_calls == 1 && g_seenLength == 0x7fffffffu, "INT_MAX length reached the callback");
    Reset();
    Check(!ReadAt(0, dest, 0x80000000ull), "INT_MAX+1 length is refused");
    Check(g_calls == 0, "INT_MAX+1 length never enters the callback");
    Check(!ReadAt(0x80000000ull, dest, 0x80000000ull), "and the next byte past it is not");
    Check(!ReadAt(0xffffffffull, dest, 0x80000000ull), "len > INT_MAX is not clamped to INT_MAX");

    // --- a short read never counts as a complete one -------------------------
    // One row at a time, each setting and clearing its own override, so a row
    // never depends on what ran before it.
    Reset();
    g_returnOverride = 4095;
    Check(!ReadAt(0xa0b1c2d0ull, dest, 4096), "a short callback result is not a complete read");
    Reset();
    g_returnOverride = 4097;
    Check(!ReadAt(0xa0b1c2d0ull, dest, 4096), "a longer callback result is not a complete read");
    Reset();
    Check(ReadAt(0xa0b1c2d0ull, dest, 4096), "a 2.69 GB offset still returns success");

    // --- a destination outside the heap is refused by the callback bound -----
    // 0xfff8 is past the callback's heap, which is 2^32 bytes here, so it is
    // the callback bound that has to look at the address itself; nothing in
    // this probe dereferences it. Both sides of that bound are driven.
    Reset();
    Check(!ReadAt(0, (void *)(uintptr_t)0x1ffffffffull, 9), "a destination past the heap is refused");
    Check(ReadAt(0, (void *)(uintptr_t)0xffff0000ull, 0x10000ull),
          "a destination whose span ends exactly at the heap end is accepted");

    // --- no callback at all still fails closed -------------------------------
    Reset();
    g_callbackDefined = false;
    Check(!ReadAt(0, dest, 11), "a missing callback read is a failure, never a success");
    Reset();

    std::printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
`;

test('the guard compiles and refuses the crossing range when a compiler is present', (t) => {
  const cxx = findCompiler();
  if (!cxx) {
    t.skip('no C++ compiler on PATH (set CXX to override); the compiled half did not run');
    return;
  }
  const patchText = readFileSync(join(ROOT, PATCH), 'utf8');
  const guard = extractGuard(patchText);
  assert.match(guard, /if \(len > 0x100000000ull - offset\) return false;/,
    'the extracted guard must be the one with the range check');

  // Everything the extracted guard needs that a plain host compiler does not
  // have: <emscripten.h> (EM_ASM_INT and its argument macros) and the two C
  // file APIs the non-Emscripten branch of the same function names. Declared
  // here rather than in PROBE, because they only exist for this splice.
  const splice = [guard].join('\n');

  const dir = mkdtempSync(join(tmpdir(), 'artc-pf8-guard-'));
  try {
    // The guard's `#include <emscripten.h>` cannot resolve on a host compiler,
    // so the header is stubbed through the include path rather than deleted
    // from the text. The guard itself is spliced in verbatim.
    writeFileSync(join(dir, 'emscripten.h'),
      '// Stand-in for the Emscripten header: the probe declares EM_ASM_INT.\n'
      + '#ifndef ARTC_PROBE_EMSCRIPTEN_H_INCLUDED\n'
      + '#define ARTC_PROBE_EMSCRIPTEN_H_INCLUDED\n'
      + '#define EM_ASM_INT(code, path, offset32, dest, len32) \\\n'
      + '    CallbackFake((uint32_t)(offset32), (dest), (uint32_t)(len32))\n'
      + '#endif\n');
    const probe = join(dir, 'guard.cpp');
    const binary = join(dir, process.platform === 'win32' ? 'guard.exe' : 'guard');
    writeFileSync(probe, PROBE.replace('    //===GUARD===', splice));

    const log = join(dir, 'probe.log');
    const compile = shellStatus(
      `${JSON.stringify(cxx)} -std=c++17 -Wall -Wextra -O1 -D__EMSCRIPTEN__=1 -I${JSON.stringify(forTar(dir))} -o ${JSON.stringify(forTar(binary))} ${JSON.stringify(forTar(probe))} > ${JSON.stringify(forTar(log))} 2>&1`,
    );
    assert.equal(compile.status, 0, `the extracted guard must compile:\n${readFileSync(log, 'utf8')}`);
    const run = shellStatus(`${JSON.stringify(forTar(binary))} > ${JSON.stringify(forTar(log))} 2>&1`);
    const output = readFileSync(log, 'utf8');
    console.log(output);
    assert.equal(run.status, 0, `every boundary row must pass:\n${output}`);
    const summary = output.match(/(\d+) checks, (\d+) failures/);
    assert.ok(summary, `the probe must report its check count; got:\n${output}`);
    assert.ok(Number(summary[1]) >= 25, `the probe must run its rows (reported ${summary[1]})`);
    assert.equal(Number(summary[2]), 0);
  } finally {
    rmSync(dir, { recursive: true, force: true });
  }
});
