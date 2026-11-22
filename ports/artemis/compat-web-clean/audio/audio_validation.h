// Host-side argument validation for the web audio CABI.
//
// These three checks used to live as file-static helpers inside
// `overlay/host/web_main.cpp`, where the only way to test them was to link the
// whole engine -- so the audio test asserted a *copy* of the rules instead of
// the rules. Moving them here makes the real functions callable from a small
// test that links nothing but this header, which is the difference between
// testing the rule and testing a restatement of it.
//
// Deliberately no <emscripten.h>: the heap bound is passed in as a `heapBytes`
// argument rather than read from `emscripten_get_heap_size()`, so the same
// header compiles under a plain host compiler and under em++.
// `web_main.cpp` passes the Emscripten heap size; a test passes whatever it
// wants to prove the bound with.
//
// `<stdint.h>` and `<math.h>` rather than `<cstdint>`/`<cmath>`: on GCC the C++
// wrappers are C++11-and-later (they `#error` under `-std=c++98`), and this
// header is meant to be includable by the oldest compiler in reach. The C names
// are declared in both C++98 and C++11, so one spelling covers the whole range
// the audio test compiles this header under. Verified with
// `g++ -std=c++98|c++11|c++17`; all three agree on every rule below.
#ifndef ARTC_AUDIO_VALIDATION_H
#define ARTC_AUDIO_VALIDATION_H

#include <math.h>
#include <stdint.h>

namespace artc {
namespace AudioValidation {

// Frames a page may pull from one voice in one call. The cap is enforced by
// `ValidateAudioReadParams`, not by the caller remembering to compare.
static const uint32_t kMaxReadFrames = 4096;

// Largest integer a JS `number` can hold exactly (2^53 - 1). A value above it
// has already lost the low bits, so it is not a frame count any more.
static const double kMaxExactInteger = 9007199254740991.0;

// A JS number that is a finite integer handle in 1 .. 2^53-1. No cast first:
// NaN/Inf become implementation-defined when truncated to int64, and handle 0
// is "no such voice" rather than a valid one.
inline bool AudioHandleOk(double h, int64_t *out) {
    if (!isfinite(h) || h < 1.0 || h > kMaxExactInteger) return false;
    const double t = trunc(h);
    if (t != h) return false;
    *out = static_cast<int64_t>(t);
    return true;
}

// Cumulative speaker frames: finite integer in 0 .. 2^53-1.
inline bool AudioFramesOk(double n, uint64_t *out) {
    if (!isfinite(n) || n < 0.0 || n > kMaxExactInteger) return false;
    const double t = trunc(n);
    if (t != n) return false;
    *out = static_cast<uint64_t>(t);
    return true;
}

// `bytes` at `addr` inside a heap of `heapBytes`, `align`-aligned. Subtracts so
// a wrapping addr+bytes cannot pass: on a 64-bit host `addr + bytes` can wrap,
// and `addr > heap - addr` is the form that stays correct.
//
// A zero-length span is a legal memory range anywhere (including at the end of
// the heap), so it is accepted; `addr == 0` with a non-zero length is not.
inline bool HeapSpanOk(uintptr_t addr, uint64_t bytes, uintptr_t align, uintptr_t heapBytes) {
    if (align && (addr % align) != 0) return false;
    if (addr > heapBytes || bytes > static_cast<uint64_t>(heapBytes - addr)) return false;
    if (addr == 0 && bytes != 0) return false;
    return true;
}

// Everything `artc_web_audio_read_stereo` has to accept before it touches the
// heap or the decoder, in the order it checks them.
//
// `dest`/`outGain`/`outPan` are raw wasm addresses (0 meaning "absent" for the
// two optional out-params). The rules, and why each one exists:
//
//   * a bad handle or a non-finite/out-of-range `frames` is a refusal, never a
//     silent success -- the page would otherwise read uninitialised memory and
//     play it;
//   * `frames > kMaxReadFrames` is a refusal. The cap bounds one call's work;
//     the backend would otherwise decode an arbitrarily large block a page
//     asked for;
//   * `frames == 0` is a *valid* no-op and is accepted with no buffer check at
//     all -- a zero-length pull writes nothing, and requiring a destination for
//     it would turn a legitimate call into an error;
//   * `frames > 0` requires `dest != 0` and a 2-byte-aligned span of `frames*4`
//     bytes inside the heap. A null destination with a non-zero frame count is
//     a bad argument even though the span arithmetic itself is fine;
//   * the optional gain/pan out-params, when present, must each be a 4-aligned
//     4-byte span inside the heap.
//
// Returns true and writes nothing when the arguments are acceptable.
inline bool ValidateAudioReadParams(double handle, uintptr_t dest, double frames, uintptr_t outGain,
                                    uintptr_t outPan, uintptr_t heapBytes, int64_t *outHandle,
                                    uint32_t *outFrames) {
    int64_t h = 0;
    if (!AudioHandleOk(handle, &h)) return false;
    // A negative, fractional, NaN or Infinity frame count is refused before any
    // cast; only then is the 4096 cap applied.
    if (!isfinite(frames) || frames < 0.0 || frames > kMaxExactInteger) return false;
    const double t = trunc(frames);
    if (t != frames) return false;
    if (t > static_cast<double>(kMaxReadFrames)) return false;
    const uint32_t n = static_cast<uint32_t>(t);

    if (n != 0) {
        // Stereo int16: two channels, two bytes each.
        if (dest == 0) return false;
        if (!HeapSpanOk(dest, static_cast<uint64_t>(n) * 4u, 2, heapBytes)) return false;
    }
    if (outGain && !HeapSpanOk(outGain, 4, 4, heapBytes)) return false;
    if (outPan && !HeapSpanOk(outPan, 4, 4, heapBytes)) return false;

    if (outHandle) *outHandle = h;
    if (outFrames) *outFrames = n;
    return true;
}

} // namespace AudioValidation
} // namespace artc

#endif // ARTC_AUDIO_VALIDATION_H
