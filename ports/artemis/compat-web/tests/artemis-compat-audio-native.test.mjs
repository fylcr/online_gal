/**
 * Portable contract test for the merged Artemis audio backend.
 *
 * Scope: the web audio backend (`overlay/audio/audio_web.{h,cpp}`), its shared argument
 * validator (`overlay/audio/audio_validation.h`) and the `PcmStream` contract those two
 * depend on. NOT the engine, NOT a game run, NOT a claim that anything was heard.
 *
 * Why this file exists next to `.test/artemis-compact-audio-native-final-audcap/`:
 * that test established the ACK-driven clock contract and is the source of truth for it,
 * but it needs `out/build-src` (a completed `build.sh` run) and it *mirrored* the host's
 * argument rules into a local `FramesOk` helper, because the real ones were file-static
 * inside `web_main.cpp` and only reachable by linking the whole engine. The header is now
 * shared, so this test calls the actual functions. Its job is to be runnable in a clean
 * clone with nothing installed and to fail loudly if anything that only the toolchain
 * could otherwise prove has drifted.
 *
 * Two tiers, deliberately separated:
 *
 *   1. ALWAYS RUNS, no toolchain, no emsdk, no native build. Reads the tracked sources and
 *      asserts the contracts that are properties of the text: the ABI surface the worker
 *      binds by name, the 4096 cap being one constant rather than two, the exported-frames
 *      sentinel, and the header carrying the real rules rather than a copy of them.
 *   2. RUNS WHEN g++ IS PRESENT. Compiles the shared header plus the real backend against a
 *      synthetic in-memory stream and runs the binary. `skip` is reported explicitly when
 *      g++ is absent -- an absent toolchain is never a pass.
 *
 * The synthetic stream is generated, not vendored: no ogg, no ffmpeg, no copyrighted input.
 * The real-ogg seam is covered by the audcap test, which owns fixtures.
 */
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { existsSync, mkdtempSync, readFileSync, rmSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';
import test from 'node:test';

// This file lives at ports/artemis/compat-web/tests/, so the repository root is four levels up.
const ROOT = resolve(dirname(fileURLToPath(import.meta.url)), '..', '..', '..', '..');
const read = (file) => readFileSync(join(ROOT, file), 'utf8');

const PORT = 'ports/artemis/compat-web';
const VALIDATION = `${PORT}/overlay/audio/audio_validation.h`;
const BACKEND = `${PORT}/overlay/audio/audio_web.cpp`;
const BACKEND_H = `${PORT}/overlay/audio/audio_web.h`;
const HOST = `${PORT}/overlay/host/web_main.cpp`;
const CMAKE = `${PORT}/overlay/cmake/web.cmake`;

/** The ACK-driven contract summary, so a rename in the products is caught here. */
const AUDIO_EXPORTS = [
  'artc_web_audio_pump', 'artc_web_audio_voice_count', 'artc_web_audio_voice_handle',
  'artc_web_audio_voice_key', 'artc_web_audio_voice_rate', 'artc_web_audio_read_stereo',
  'artc_web_audio_ended', 'artc_web_audio_played_frames', 'artc_web_audio_ack',
];

// ---------------------------------------------------------------------------
// Tier 1 -- properties of the tracked sources. No toolchain.
// ---------------------------------------------------------------------------

test('the host exports the whole audio ABI the worker binds by name', () => {
  const host = read(HOST);
  const exported = new Set([...host.matchAll(/EMSCRIPTEN_KEEPALIVE[^;]*?\b(artc_web_[a-z_]+)\s*\(/g)]
    .map((match) => match[1]));
  for (const name of AUDIO_EXPORTS) {
    assert.ok(exported.has(name), `${HOST} must export ${name}; the worker cwrap()s it by name, `
      + 'so a rename is a runtime `missing _artc_web_*` rather than a link error');
  }
  // The pull entry point is the one with a buffer: it has to keep the frame cap in its signature.
  assert.match(host, /artc_web_audio_read_stereo\(double handle, uintptr_t heapDest, int frames/);
});

test('the frame cap is one constant in the shared header, not a second copy in the host', () => {
  const header = read(VALIDATION);
  assert.match(header, /kMaxReadFrames\s*=\s*4096/,
    'the 4096 cap belongs in the shared header, where the test can reach it');
  const host = read(HOST);
  // Two literals that have to agree is the shape that drifts. The host must derive its cap
  // from the header, so a change in one place moves both.
  assert.match(host, /kAudioPullFrames\s*=\s*AudioValidation::kMaxReadFrames/,
    `${HOST} must take the cap from ${VALIDATION} rather than repeating 4096`);
  assert.doesNotMatch(host, /constexpr\s+uint32_t\s+kAudioPullFrames\s*=\s*4096/,
    'the host must not re-declare the literal; that is the copy this refactor removed');
});

test('the host calls the shared validator instead of restating its rules', () => {
  const host = read(HOST);
  assert.match(host, /#include\s+"audio\/audio_validation\.h"/, `${HOST} must include the shared header`);
  assert.match(host, /ValidateAudioReadParams\(/,
    'the read entry point must go through the shared validator, or the test proves nothing '
    + 'about the function the module actually runs');
  // The refactor's whole point: the rules are callable, not mirrored. A local re-definition
  // would compile and pass while leaving the shipped path untested.
  assert.doesNotMatch(host, /bool\s+AudioHandleOk\s*\(double/,
    'AudioHandleOk must be the shared one; a local body is the duplication this removed');
  assert.doesNotMatch(host, /bool\s+AudioFramesOk\s*\(double/,
    'AudioFramesOk must be the shared one');
  // The old names stay as `using` aliases so the host reads as it did. If that ever becomes a
  // local definition the alias disappears, which is the regression this catches.
  assert.match(host, /using\s+AudioValidation::AudioHandleOk/);
  assert.match(host, /using\s+AudioValidation::AudioFramesOk/);
  // `HeapSpanOk` is the one wrapper that remains: it binds the live Emscripten heap size to the
  // shared function's explicit `heapBytes` argument, and must pass the real heap through.
  assert.match(host, /emscripten_get_heap_size\(\)/,
    'the heap bound must come from the live wasm heap, not a constant');
  assert.doesNotMatch(host, /audio_validation\.cpp|AUDIO_VALIDATION_IMPL/,
    'the header is include-only; a .cpp would be a second translation unit to link');
});

test('the shared header is free of Emscripten and of C++11-only headers', () => {
  const header = read(VALIDATION);
  // Strip `//` comments first. The header *explains* that it takes the heap size rather than
  // calling `emscripten_get_heap_size()`, and a whole-file search would flag that explanation
  // instead of a live call -- the same mistake the CI guard makes a point of avoiding.
  const code = header.split(/\r?\n/).map((line) => line.replace(/\/\/.*$/, '')).join('\n');
  // No <emscripten.h>: the heap size arrives as an argument, which is what makes the header
  // callable from a plain host compiler.
  assert.doesNotMatch(code, /#include\s*<emscripten[^>]*>/,
    'the header must not depend on Emscripten; pass the heap size in instead');
  assert.doesNotMatch(code, /\bemscripten_[a-z_]+\s*\(/,
    'no Emscripten call may leak into the portable header');
  // <cstdint>/<cmath> `#error` under `-std=c++98` on GCC; the C spellings do not.
  assert.doesNotMatch(code, /#include\s*<c(?:stdint|math)>/,
    'use <stdint.h>/<math.h>: the C++ wrappers are C++11-and-later on GCC');
  assert.match(code, /#include\s*<stdint\.h>/);
  assert.match(code, /#include\s*<math\.h>/);
  // The calls must be the C names too, or the C++98 claim is only about the includes.
  assert.match(code, /\bisfinite\(/);
  assert.match(code, /\btrunc\(/);
  assert.doesNotMatch(code, /std::isfinite|std::trunc/,
    'std::isfinite/std::trunc are C++11 declarations and are not available under -std=c++98');
  // One include guard, and it has to match the file so a second include is a no-op.
  assert.match(header, /#ifndef\s+ARTC_AUDIO_VALIDATION_H/);
  assert.match(header, /#define\s+ARTC_AUDIO_VALIDATION_H/);
  assert.match(header, /#endif/);
});

test('the backend keeps the sentinel and the short-return contracts the ABI depends on', () => {
  const backend = read(BACKEND);
  const backendH = read(BACKEND_H);
  // A decoder that writes past the caller's buffer has already overrun it. The
  // backend must not count those frames, and it must not report the read as a
  // clean empty one either: a broken stream has to be distinguishable from a
  // drained voice, so it returns the error sentinel rather than 0.
  assert.match(backendH, /extern const size_t kReadError;/,
    'the refusal sentinel belongs in the header, where a caller can name it');
  assert.match(backend, /size_t kReadError = static_cast<size_t>\(-1\);/,
    'kReadError must be the largest size_t, which the CABI turns into -1');
  assert.match(backend, /if\s*\(got\s*>\s*frames\)\s*return\s+kReadError;/,
    'ReadStereo must report a stream that returned more frames than it was given as a refusal');
  assert.doesNotMatch(backend, /if\s*\(got\s*>\s*frames\)\s*return\s+0;/,
    'a zero return is the empty answer; an over-reporting stream must not look like one');
  // A refusal never decodes: the null destination and the frame cap are settled
  // before the source is touched.
  assert.match(backend, /if\s*\(!out\s*&&\s*frames\)\s*return\s+kReadError;/);
  assert.match(backend, /if\s*\(frames\s*>\s*AudioValidation::kMaxReadFrames\)\s*return\s+kReadError;/);
  assert.match(backend, /if\s*\(!v\)\s*return\s+kReadError;/,
    'an unknown or stopped handle is a refusal, not an empty read');
  // The clock is the speaker's: PlaybackMs divides `consumed`, never `produced`.
  assert.match(backend, /1000\.0\s*\*\s*static_cast<double>\(v->consumed\)\s*\/\s*static_cast<double>\(v->rate\)/,
    'PlaybackMs must be the presented position (consumed frames over rate)');
  // Drained is EOF *and* the ACK caught up. EOF alone ends the sound early and cuts `[wait se]`.
  assert.match(backend, /v\.ended\s*&&\s*v\.consumed\s*>=\s*v\.produced/,
    'VoiceDrained must require both source EOF and consumed >= produced');
  // ACK refusal leaves the count alone: a backward or past-produced total is -1, not a clamp.
  assert.match(backend, /totalPlayed\s*>\s*v\.produced\s*\|\|\s*totalPlayed\s*<\s*v\.consumed/);
  // A zero-frame pull is an empty answer even with no destination: it writes
  // nothing, so it is accepted and yields 0, not a refusal.
  assert.match(backend, /if\s*\(frames\s*==\s*0\s*\|\|\s*v->paused\s*\|\|\s*v->ended\)\s*return\s+0;/);
});

test('the CMake target compiles the overlay instead of the pinned stub', () => {
  const cmake = read(CMAKE);
  // Exactly one Audio::Impl per link. The overlay is compiled *instead of* the pinned
  // src/audio/audio.cpp; leaving both in the glob is a duplicate-symbol link error.
  assert.match(cmake, /list\(REMOVE_ITEM ARTC_CORE_SOURCES "\$\{CMAKE_CURRENT_SOURCE_DIR\}\/src\/audio\/audio\.cpp"\)/,
    'the pinned non-Android stub must be removed from the source list');
  assert.match(cmake, /list\(APPEND ARTC_CORE_SOURCES "\$\{ARTC_AUDIO_WEB_DIR\}\/audio\/audio_web\.cpp"\)/,
    'the overlay backend must be appended in its place');
  // The overlay include dir has to be listed *before* the pinned src so `audio/audio_web.h`
  // resolves by the same relative spelling upstream headers use.
  const includeDir = cmake.indexOf('${ARTC_AUDIO_WEB_DIR}');
  const pinnedSrc = cmake.indexOf('${CMAKE_CURRENT_SOURCE_DIR}/src/render');
  assert.ok(includeDir !== -1 && pinnedSrc !== -1 && includeDir < pinnedSrc,
    'the overlay include directory must come before the pinned tree');
  // A wrong layout is a configure error, not a missing-symbol link failure 40 minutes in.
  assert.match(cmake, /FATAL_ERROR[\s\S]*?audio overlay missing/,
    'the overlay presence guard must stay a configure-time failure');
});

test('the header declares the contracts the compiled test asserts', () => {
  const header = read(VALIDATION);
  // The four names the compiled half calls. Asserted here so a rename shows up as a fast
  // text failure rather than as a compiler error on a machine that happens to have g++.
  for (const name of ['AudioHandleOk', 'AudioFramesOk', 'HeapSpanOk', 'ValidateAudioReadParams']) {
    assert.match(header, new RegExp(`inline bool ${name}\\(`),
      `${VALIDATION} must declare ${name} as an inline function`);
  }
  // ValidateAudioReadParams has to take the heap bound explicitly, or it is not portable.
  assert.match(header, /ValidateAudioReadParams\([^)]*heapBytes[^)]*\)/s);
  // The header is the seam the host and the test share; it must stay header-only.
  assert.doesNotMatch(header, /^\s*(?:extern|static)\s+(?!const|inline)/m,
    'no external or internal-linkage storage in a shared header');
});

// ---------------------------------------------------------------------------
// Tier 2 -- compiles and runs the real functions. Needs g++, not emsdk.
// ---------------------------------------------------------------------------

/**
 * The compiled probe. Written from this file so the assertions and the C++ live together and
 * cannot drift apart by editing one of them.
 *
 * It calls the *shared* header's functions directly (no local copy of the rules, which is what
 * the audcap test had to do before this header existed) and then drives the real backend
 * through `AudioWeb` over a synthetic stream. The engine symbols the backend references are
 * stubbed the same way the audcap test stubs them: this links the backend, not the engine.
 */
const PROBE = String.raw`
#include "audio/audio.h"
#include "audio/audio_validation.h"
#include "audio/audio_web.h"
#include "log/logger.h"
#include "pack/pack_manager.h"

#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <vector>

// --- pinned engine symbols this probe does not build ------------------------
namespace artc {
LogSink g_unused_sink;
void SetLogSink(LogSink sink) { g_unused_sink = sink; }
void Log(int, const std::string &) {}
bool PackManager::Read(const std::string &, std::vector<uint8_t> &) const { return false; }
}

using namespace artc;
using namespace artc::AudioValidation;

static int g_checks = 0;
static int g_failures = 0;
static void Check(bool ok, const char *what) {
    ++g_checks;
    if (!ok) { ++g_failures; std::printf("FAIL %s\n", what); }
}

// Deterministic in-memory stream: no ogg, no file, no ffmpeg. A stream that
// lies about its return value is also provided, to drive the sentinel path.
class SyntheticPcm : public PcmStream {
public:
    SyntheticPcm(int rate, uint64_t frames, bool loop, bool overreport)
        : rate_(rate), total_(frames), loop_(loop), over_(overreport) {}
    size_t ReadStereo(int16_t *out, size_t frames) override {
        if (frames == 0) return 0;
        // A lying stream writes only what fits but claims more, on the first
        // call and never again. The buffer is sized by the caller, so two
        // frames written into it is not an overrun here: it is the *claim* of
        // frames+1 that the backend has to refuse, and the check that matters
        // is that the count is not credited.
        if (over_) { out[0] = 1; out[1] = 2; return frames + 1; }
        if (!loop_ && pos_ >= total_) return 0;
        size_t n = frames;
        if (!loop_ && pos_ + n > total_) n = (size_t)(total_ - pos_);
        for (size_t i = 0; i < n; ++i) {
            const int16_t v = (int16_t)((pos_ + i) % 1000);
            out[i * 2] = v;
            out[i * 2 + 1] = (int16_t)(v + 1);
        }
        pos_ += n;
        if (loop_) pos_ %= (total_ ? total_ : 1);
        return n;
    }
    int SampleRate() const override { return rate_; }
    uint64_t FrameCount() const override { return total_; }
    bool Ended() const override { return !loop_ && pos_ >= total_; }
private:
    int rate_;
    uint64_t total_;
    bool loop_;
    bool over_;
    uint64_t pos_ = 0;
};

static uint64_t PullAll(Audio &audio, int64_t h, size_t block = 1024) {
    std::vector<int16_t> buf(block * 2);
    uint64_t total = 0;
    for (;;) {
        const size_t got = AudioWeb::ReadStereo(audio, h, buf.data(), block, nullptr, nullptr);
        if (got == 0) break;
        total += got;
    }
    return total;
}

// --- 1. Handle rules, through the real function -----------------------------
static void TestHandleRules() {
    int64_t h = 0;
    Check(AudioHandleOk(1.0, &h) && h == 1, "handle 1 is accepted");
    Check(AudioHandleOk(9007199254740991.0, &h), "2^53-1 is the largest valid handle");
    // 0 is "no such voice", not a valid handle, so it must be refused rather than passed on.
    Check(!AudioHandleOk(0.0, &h), "handle 0 is refused");
    Check(!AudioHandleOk(-1.0, &h), "a negative handle is refused");
    Check(!AudioHandleOk(1.5, &h), "a fractional handle is refused");
    Check(!AudioHandleOk(NAN, &h), "NaN is refused before any cast to int64");
    Check(!AudioHandleOk(INFINITY, &h), "Infinity is refused before any cast");
    Check(!AudioHandleOk(-INFINITY, &h), "negative Infinity is refused");
    Check(!AudioHandleOk(9007199254740992.0, &h), "2^53 is out of the exact-integer range");
}

// --- 2. Frame-count rules, through the real function ------------------------
static void TestFrameRules() {
    uint64_t n = 0;
    Check(AudioFramesOk(0.0, &n) && n == 0, "0 frames is a valid ACK total");
    Check(AudioFramesOk(4096.0, &n) && n == 4096, "4096 frames is valid");
    Check(!AudioFramesOk(-1.0, &n), "a negative total is refused");
    Check(!AudioFramesOk(0.5, &n), "a fractional total is refused");
    Check(!AudioFramesOk(NAN, &n), "NaN is refused");
    Check(!AudioFramesOk(INFINITY, &n), "Infinity is refused");
    Check(!AudioFramesOk(9007199254740992.0, &n), "2^53 is out of range");
}

// --- 3. Heap spans: alignment, bound, and the null-with-length rule ---------
static void TestHeapSpans() {
    const uintptr_t HEAP = 65536;
    // In-heap, aligned.
    Check(HeapSpanOk(1024, 4096, 2, HEAP), "an aligned in-heap span is accepted");
    // Unaligned start is refused even when the span itself fits.
    Check(!HeapSpanOk(1023, 16, 2, HEAP), "a misaligned span is refused");
    Check(HeapSpanOk(1024, 16, 4, HEAP), "an address divisible by the alignment is accepted");
    Check(!HeapSpanOk(1026, 16, 4, HEAP), "alignment 4 rejects a 2-mod-4 address");
    Check(HeapSpanOk(1024, 16, 0, HEAP), "align 0 means unaligned is acceptable");
    // The bound subtracts, so addr+bytes wrapping cannot pass.
    Check(!HeapSpanOk(70000, 8, 2, HEAP), "a start past the heap is refused");
    Check(!HeapSpanOk(HEAP, 8, 2, HEAP), "a span starting at the end is refused");
    Check(HeapSpanOk(HEAP - 4096, 4096, 2, HEAP), "a span ending exactly at the heap end fits");
    Check(!HeapSpanOk(HEAP - 4096, 4097, 2, HEAP), "one byte past the heap end is refused");
    // A zero-length span is a legal range anywhere, including at address 0: a zero-frame pull
    // writes nothing, so requiring a destination for it would refuse a legitimate call.
    Check(HeapSpanOk(0, 0, 2, HEAP), "a zero-length span at 0 is a legal no-op");
    Check(HeapSpanOk(HEAP, 0, 2, HEAP), "a zero-length span at the heap end is legal");
    // ... but a non-zero length at 0 is a bad argument.
    Check(!HeapSpanOk(0, 8, 2, HEAP), "a non-zero span at a null address is refused");
}

// --- 4. The combined read validator: every boundary the host applies --------
static void TestReadParams() {
    const uintptr_t HEAP = 65536;
    const uintptr_t DEST = 4096;
    const uintptr_t GAIN = 8192;
    const uintptr_t PAN = 12288;
    int64_t h = 0;
    uint32_t n = 0;

    Check(ValidateAudioReadParams(1, DEST, 64, 0, 0, HEAP, &h, &n) && h == 1 && n == 64,
          "a well-formed pull is accepted and reports its handle and frame count");
    Check(ValidateAudioReadParams(1, DEST, 1, 0, 0, HEAP, &h, &n) && n == 1, "one frame is accepted");
    Check(ValidateAudioReadParams(1, DEST, 4096, 0, 0, HEAP, &h, &n) && n == 4096,
          "exactly the cap is accepted");

    // The cap is a refusal, not a clamp: silently shrinking would make the caller's buffer
    // accounting wrong rather than obviously wrong.
    Check(!ValidateAudioReadParams(1, DEST, 4097, 0, 0, HEAP, &h, &n), "one past the cap is refused");
    Check(!ValidateAudioReadParams(1, DEST, 100000, 0, 0, HEAP, &h, &n), "a wildly large pull is refused");
    Check(!ValidateAudioReadParams(1, DEST, -1, 0, 0, HEAP, &h, &n), "a negative frame count is refused");
    Check(!ValidateAudioReadParams(1, DEST, NAN, 0, 0, HEAP, &h, &n), "a NaN frame count is refused");
    Check(!ValidateAudioReadParams(1, DEST, INFINITY, 0, 0, HEAP, &h, &n), "an infinite frame count is refused");
    Check(!ValidateAudioReadParams(1, DEST, 1.5, 0, 0, HEAP, &h, &n), "a fractional frame count is refused");
    Check(!ValidateAudioReadParams(1, DEST, 9007199254740992.0, 0, 0, HEAP, &h, &n),
          "a frame count past 2^53 is refused before the cast");

    // A null destination with frames > 0 is a bad argument: the span arithmetic
    // alone would call it legal, which is exactly why the validator checks it.
    Check(!ValidateAudioReadParams(1, 0, 64, 0, 0, HEAP, &h, &n),
          "a null destination with a non-zero frame count is refused");
    // A zero-frame pull is valid and needs no destination: it writes nothing.
    Check(ValidateAudioReadParams(1, 0, 0, 0, 0, HEAP, &h, &n) && n == 0,
          "a zero-frame pull with a null destination is a valid no-op");
    // A zero-sized heap with a real pull is a refusal, not a pass.
    Check(!ValidateAudioReadParams(1, DEST, 64, 0, 0, 0, &h, &n), "a pull against a zero-size heap is refused");

    // Destination alignment and bound.
    Check(!ValidateAudioReadParams(1, DEST + 1, 64, 0, 0, HEAP, &h, &n),
          "an unaligned destination is refused");
    Check(!ValidateAudioReadParams(1, HEAP, 1, 0, 0, HEAP, &h, &n),
          "a destination whose span runs past the heap is refused");
    // The last whole stereo frame must fit.
    Check(ValidateAudioReadParams(1, HEAP - 4, 1, 0, 0, HEAP, &h, &n),
          "the final stereo frame fits exactly");
    Check(!ValidateAudioReadParams(1, HEAP - 4, 2, 0, 0, HEAP, &h, &n),
          "two frames past the heap end is refused");

    // The optional out-params are checked only when present, and must be 4-aligned 4-byte spans.
    Check(ValidateAudioReadParams(1, DEST, 64, GAIN, PAN, HEAP, &h, &n),
          "well-aligned gain and pan out-params are accepted");
    Check(!ValidateAudioReadParams(1, DEST, 64, GAIN + 2, 0, HEAP, &h, &n),
          "a misaligned gain out-param is refused");
    Check(!ValidateAudioReadParams(1, DEST, 64, 0, HEAP, HEAP, &h, &n),
          "a pan out-param whose 4 bytes run past the heap is refused");
    Check(!ValidateAudioReadParams(1, DEST, 64, HEAP - 2, 0, HEAP, &h, &n),
          "a gain out-param overlapping the heap end is refused");

    // A bad handle is refused, and the out-params must not have been written through.
    h = -7; n = 99;
    Check(!ValidateAudioReadParams(0, DEST, 64, 0, 0, HEAP, &h, &n), "handle 0 is refused");
    Check(h == -7 && n == 99, "a refusal writes nothing through the out-params");
    Check(!ValidateAudioReadParams(1.5, DEST, 64, 0, 0, HEAP, &h, &n), "a fractional handle is refused");
}

// --- 5. The real backend, driven through the bridge -------------------------
static void TestBackendClock() {
    Audio audio;
    audio.Init(nullptr);
    audio.PlayStream("se", std::unique_ptr<PcmStream>(new SyntheticPcm(44100, 4410, false, false)), 1000);
    const int64_t h = AudioWeb::VoiceHandle(audio, 0);
    Check(h != 0, "a live voice has a non-zero handle");

    const uint64_t pulled = PullAll(audio, h);
    Check(pulled == 4410, "every frame is pulled exactly once");
    // The pull cursor is not the clock: decoding ahead presents nothing.
    Check(audio.PlaybackMs("se") == 0.0, "pulling the whole stream still presents 0 ms");
    Check(AudioWeb::Acknowledge(audio, h, 2205) == 1, "a partial ACK is accepted");
    Check(audio.PlaybackMs("se") == 50.0, "2205 of 4410 frames at 44.1 kHz is 50 ms");
    Check(AudioWeb::Acknowledge(audio, h, 4410) == 1, "ACKing up to produced is accepted");
    Check(AudioWeb::Ended(audio, h), "the voice drains once consumed catches produced");
    // Refusals leave the count alone.
    Check(AudioWeb::Acknowledge(audio, h, 4409) == -1, "a backward ACK is refused");
    Check(AudioWeb::Acknowledge(audio, h, 4411) == -1, "an ACK past produced is refused");
    Check(AudioWeb::PlayedFrames(audio, "se") == 4410, "no refusal changed the consumed count");
    audio.Shutdown();
}

// --- 6. A stream that over-reports must not be counted, and must be visible -
static void TestOverreportRefused() {
    Audio audio;
    audio.Init(nullptr);
    // The source claims frames+1 written into a frames-sized buffer.
    audio.PlayStream("bad", std::unique_ptr<PcmStream>(new SyntheticPcm(44100, 1000, false, true)), 1000);
    const int64_t h = AudioWeb::VoiceHandle(audio, 0);
    std::vector<int16_t> buf(64 * 2);
    const size_t got = AudioWeb::ReadStereo(audio, h, buf.data(), 64, nullptr, nullptr);
    // Not 65 (the lie), not 64 (a partial credit), not 0 (an honest empty read):
    // the sentinel, because a broken decoder and a drained voice are different
    // things and the caller has to be able to tell them apart.
    Check(got == AudioWeb::kReadError, "a stream returning more than asked is refused with the sentinel");
    Check(AudioWeb::PlayedFrames(audio, "bad") == 0, "a refused read does not advance the clock");
    Check(!AudioWeb::Ended(audio, h), "an over-reporting stream is not silently drained");
    audio.Shutdown();
}

// --- 6b. Every refusal is the sentinel, and every empty answer is 0 ---------
static void TestRefusalVsEmpty() {
    Audio audio;
    audio.Init(nullptr);
    audio.PlayStream("se", std::unique_ptr<PcmStream>(new SyntheticPcm(44100, 4410, false, false)), 1000);
    const int64_t h = AudioWeb::VoiceHandle(audio, 0);
    std::vector<int16_t> buf(64 * 2);

    // Refusals: a null destination with frames, an unknown handle, and a pull
    // above the shared cap. None of them decodes, so the clock stays at 0.
    Check(AudioWeb::ReadStereo(audio, h, nullptr, 64, nullptr, nullptr) == AudioWeb::kReadError,
          "a null destination with frames > 0 is the sentinel");
    Check(AudioWeb::ReadStereo(audio, 0, buf.data(), 64, nullptr, nullptr) == AudioWeb::kReadError,
          "handle 0 is the sentinel");
    Check(AudioWeb::ReadStereo(audio, h, buf.data(), AudioValidation::kMaxReadFrames + 1, nullptr, nullptr)
              == AudioWeb::kReadError,
          "a pull above the shared frame cap is the sentinel, unclamped");
    Check(AudioWeb::PlayedFrames(audio, "se") == 0, "no refusal decoded a single frame");

    // Empty answers: a zero-frame pull needs no destination and yields 0.
    Check(AudioWeb::ReadStereo(audio, h, nullptr, 0, nullptr, nullptr) == 0,
          "a zero-frame pull is an empty answer, not a refusal");
    Check(AudioWeb::PlayedFrames(audio, "se") == 0, "a zero-frame pull decodes nothing");

    // A normal read still works after all of that, and a short one is tiny but
    // honest: 1 frame is a full read of 1, not a refusal.
    Check(AudioWeb::ReadStereo(audio, h, buf.data(), 64, nullptr, nullptr) == 64,
          "a normal pull is unaffected by the refusals");
    Check(AudioWeb::ReadStereo(audio, h, buf.data(), 1, nullptr, nullptr) == 1,
          "a one-frame pull is a complete read of one frame");

    // EOF is an empty answer too: the source is drained but the ACK has not
    // caught up, so the voice is still enumerable and reads 0.
    const uint64_t rest = PullAll(audio, h);
    Check(rest == 4410 - 65, "the remaining frames are all pulled");
    Check(AudioWeb::ReadStereo(audio, h, buf.data(), 64, nullptr, nullptr) == 0,
          "a drained source yields 0, not the sentinel");
    Check(!AudioWeb::Ended(audio, h), "EOF alone does not drain a voice");
    audio.Shutdown();
}

// --- 7. EOF, loop and pause, end to end -------------------------------------
static void TestEofLoopPause() {
    {
        Audio audio;
        audio.Init(nullptr);
        audio.PlayStream("se", std::unique_ptr<PcmStream>(new SyntheticPcm(8000, 800, false, false)), 1000);
        const int64_t h = AudioWeb::VoiceHandle(audio, 0);
        Check(PullAll(audio, h) == 800, "the whole 800-frame stream pulls");
        // Source EOF is not the end of the sound: the ACK has not caught up.
        Check(!AudioWeb::Ended(audio, h), "EOF alone does not drain a voice");
        Check(AudioWeb::Acknowledge(audio, h, 800) == 1, "ACKing the rest drains it");
        Check(audio.PlaybackMs("se") == 100.0, "800 frames at 8 kHz presents 100 ms");
        audio.Shutdown();
    }
    {
        Audio audio;
        audio.Init(nullptr);
        audio.PlayStream("bgm", std::unique_ptr<PcmStream>(new SyntheticPcm(44100, 44100, true, false)), 1000);
        const int64_t h = AudioWeb::VoiceHandle(audio, 0);
        std::vector<int16_t> buf(2048 * 2);
        uint64_t pulled = 0;
        for (int i = 0; i < 40; ++i) {
            const size_t got = AudioWeb::ReadStereo(audio, h, buf.data(), 2048, nullptr, nullptr);
            if (got == 0) break;
            pulled += got;
        }
        Check(pulled > 44100, "a looping voice keeps producing past one segment");
        Check(AudioWeb::Acknowledge(audio, h, (uint64_t)pulled) == 1, "a loop ACK is accepted");
        Check(!AudioWeb::Ended(audio, h), "a fully ACKed loop is never drained");
        // Pause suspends output without finishing the sound.
        audio.PauseAll();
        Check(AudioWeb::ReadStereo(audio, h, buf.data(), 512, nullptr, nullptr) == 0,
              "a paused voice yields no PCM");
        Check(audio.IsPlaying("bgm"), "a paused voice still counts as playing");
        Check(!AudioWeb::Ended(audio, h), "a paused voice is not drained");
        audio.ResumeAll();
        Check(AudioWeb::ReadStereo(audio, h, buf.data(), 512, nullptr, nullptr) == 512,
              "resume restores the pull");
        audio.Shutdown();
    }
}

// --- 8. Handles are independent and never reused ---------------------------
static void TestHandles() {
    Audio audio;
    audio.Init(nullptr);
    audio.PlayStream("a", std::unique_ptr<PcmStream>(new SyntheticPcm(44100, 4410, false, false)), 1000);
    audio.PlayStream("b", std::unique_ptr<PcmStream>(new SyntheticPcm(8000, 800, false, false)), 700);
    Check(AudioWeb::VoiceCount(audio) == 2, "two voices are live");
    const int64_t ha = AudioWeb::VoiceHandle(audio, 0);
    const int64_t hb = AudioWeb::VoiceHandle(audio, 1);
    Check(ha != 0 && hb != 0 && ha != hb, "each voice has its own handle");
    Check(AudioWeb::VoiceSampleRate(audio, ha) == 44100, "voice a reports its own rate");
    Check(AudioWeb::VoiceSampleRate(audio, hb) == 8000, "voice b reports its own rate");
    audio.Stop("a");
    Check(AudioWeb::VoiceCount(audio) == 1, "Stop removes exactly one voice");
    Check(AudioWeb::Acknowledge(audio, ha, 10) == -1, "a stale handle cannot ACK after Stop");
    Check(AudioWeb::VoiceKey(audio, ha) == nullptr, "a stale handle has no key");
    // The survivor keeps its own identity across the Stop.
    Check(std::strcmp(AudioWeb::VoiceKey(audio, hb), "b") == 0, "voice b keeps its key");
    audio.Shutdown();
}

int main() {
    TestHandleRules();
    TestFrameRules();
    TestHeapSpans();
    TestReadParams();
    TestBackendClock();
    TestOverreportRefused();
    TestRefusalVsEmpty();
    TestEofLoopPause();
    TestHandles();
    std::printf("%d checks, %d failures\n", g_checks, g_failures);
    return g_failures == 0 ? 0 : 1;
}
`;

/** Where the pinned `vorbis_stream.cpp` lives: a completed recipe run, not a checkout.
 *  `ARTC_AUDIO_SRC` points at a guarded build-src (the tree that contains `src/`),
 *  so a second pin can be probed without rewriting the default path. */
function pinnedBackendSources() {
  const buildSrc = process.env.ARTC_AUDIO_SRC
    ? join(process.env.ARTC_AUDIO_SRC, 'src')
    : join(ROOT, PORT, 'out', 'build-src', 'src');
  return {
    buildSrc,
    files: [join(buildSrc, 'audio', 'vorbis_stream.cpp'), join(buildSrc, 'audio', 'audio_channels.cpp')],
  };
}

function findCompiler() {
  for (const candidate of [process.env.CXX, 'g++', 'clang++']) {
    if (!candidate) continue;
    const probe = spawnSync(candidate, ['--version'], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    if (!probe.error && probe.status === 0) return candidate;
  }
  return null;
}

test('the shared validator and the real backend compile and pass when g++ is present', (t) => {
  const cxx = findCompiler();
  const pinned = pinnedBackendSources();
  const missing = pinned.files.filter((file) => !existsSync(file));
  // Two independent reasons to skip, reported as themselves. Neither is a pass: an absent
  // toolchain proves nothing about the code, which is the whole point of saying so out loud.
  if (!cxx) {
    t.skip('no C++ compiler on PATH (set CXX to override); the compiled half did not run');
    return;
  }
  if (missing.length) {
    t.skip(`no pinned backend sources at ${pinned.buildSrc}; run `
      + `\`bash ${PORT}/build.sh -j2\` first. Missing: ${missing.map((f) => f.slice(ROOT.length + 1)).join(', ')}`);
    return;
  }

  const dir = mkdtempSync(join(tmpdir(), 'artc-audio-portable-'));
  try {
    const probe = join(dir, 'probe.cpp');
    const binary = join(dir, process.platform === 'win32' ? 'probe.exe' : 'probe');
    writeFileSync(probe, PROBE);

    // The overlay backend plus the two pinned files it actually needs. Not the engine, not
    // the compositor, not Lua: a failure here is an audio failure.
    const sources = [
      probe,
      join(ROOT, BACKEND),
      ...pinned.files,
    ];
    const compile = spawnSync(cxx, [
      '-std=c++17', '-Wall', '-Wextra', '-O1', '-o', binary,
      ...sources,
      `-I${join(ROOT, PORT, 'overlay')}`,
      `-I${join(pinned.buildSrc)}`,
    ], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    assert.ok(!compile.error, `compiler failed to start: ${compile.error?.message}`);
    assert.equal(compile.status, 0,
      `the audio probe must compile against the real backend:\n${compile.stderr ?? ''}`);

    const run = spawnSync(binary, [], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
    assert.ok(!run.error, `probe failed to start: ${run.error?.message}`);
    const output = `${run.stdout ?? ''}${run.stderr ?? ''}`;
    assert.equal(run.status, 0, `the audio probe must pass every check:\n${output}`);
    // The count is reported so a silently-skipping probe cannot read as a pass.
    const summary = output.match(/(\d+) checks, (\d+) failures/);
    assert.ok(summary, `the probe must report its check count; got:\n${output}`);
    assert.ok(Number(summary[1]) >= 60,
      `the probe must actually run its checks (reported ${summary[1]}); a truncated run is not a pass`);
    assert.equal(Number(summary[2]), 0);
  } finally {
    rmSync(dir, { recursive: true, force: true });
  }
});

test('the header compiles under C++98 as well as C++17 when a compiler is present', (t) => {
  const cxx = findCompiler();
  if (!cxx) {
    t.skip('no C++ compiler on PATH; the standard-conformance check did not run');
    return;
  }
  const dir = mkdtempSync(join(tmpdir(), 'artc-audio-std-'));
  try {
    const probe = join(dir, 'std.cpp');
    // Include only: the claim is that the *header* is portable, and a TU that includes it
    // under -std=c++98 is the proof. The C names it uses are declared by <math.h> there.
    writeFileSync(probe, '#include "audio/audio_validation.h"\nint main() { return 0; }\n');
    for (const std of ['c++98', 'c++11', 'c++17']) {
      const compile = spawnSync(cxx, ['-std=' + std, '-Wall', '-Wextra', '-c', '-o',
        join(dir, `std-${std.replace('++', 'pp')}.o`), probe,
        `-I${join(ROOT, PORT, 'overlay')}`,
      ], { encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] });
      assert.equal(compile.status, 0,
        `${VALIDATION} must compile under -std=${std}:\n${compile.stderr ?? ''}`);
    }
  } finally {
    rmSync(dir, { recursive: true, force: true });
  }
});
