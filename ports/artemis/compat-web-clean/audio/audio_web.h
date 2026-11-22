// Web bridge for the Artemis audio backend.
//
// `audio/audio.h` is upstream and frozen: it does not expose `Impl`, and the
// web host needs engine-time voices (a voice is created by the engine, not by
// the page) plus a pull of PCM. So this header is the seam instead of a change
// to the upstream class. The implementation lives in `audio_web.cpp`, which is
// compiled *instead of* `src/audio/audio.cpp` for the web target.
//
// The free functions below are additive: `Audio::Impl` stays private and
// `Audio::Audio` still owns it. Only the web host includes this header.
#ifndef ARTC_AUDIO_WEB_H
#define ARTC_AUDIO_WEB_H

#include <cstddef>
#include <cstdint>

namespace artc {

class Audio;

namespace AudioWeb {

// Owner thread only. Safe before `Audio::Init` (nothing to do) and after
// `Shutdown` (no voices, pump stopped).
void Init(Audio& audio);
void Shutdown(Audio& audio);

// --- The pump -------------------------------------------------------------
// The engine host calls `Pump` once per tick. It is decode-free on purpose:
// `ReadStereo` only decodes; the playback clock advances only where the page
// acknowledges speaker consumption. The pump only reaps voices whose PCM ran
// out *and* whose speaker ACK has caught the decoded length. `onceFrameCap`
// bounds the pull the host is allowed to ask for.
//
// `now_ms` is the engine clock (LuaEngine::NowMs), not wall time.
void Pump(Audio& audio, double now_ms, uint32_t onceFrameCap);

// Frames the speaker has consumed, i.e. its presented position in frames.
// This, not the pull cursor and not wall time, is what makes `PlaybackMs` and
// the page's queue agree. Returns 0 for an unknown voice or one not yet ACKed.
uint64_t PlayedFrames(const Audio& audio, const char* key);

// --- Voice enumeration, for a host that pulls PCM per voice ---------------
// A handle identifies one voice for its whole lifetime and is never reused, so
// a handle the host kept past a `Stop` resolves to nothing rather than to a
// later voice. 0 means "no such voice", and is never a valid handle.
int VoiceCount(const Audio& audio);
int64_t VoiceHandle(const Audio& audio, int index);
const char* VoiceKey(const Audio& audio, int64_t handle);
int VoiceSampleRate(const Audio& audio, int64_t handle);

// The largest value a `size_t` can hold. No pull can legitimately return it, so
// `ReadStereo` returns it for a refusal. The host returns it as -1 over the
// CABI, which is why a caller compares against this name rather than spelling
// out `(size_t)-1`.
extern const size_t kReadError;

// Copies the next `frames` stereo frames of one voice into `out`. Returns the
// frames written. `0` means this voice has nothing left to give: source EOF, a
// paused voice, an ended voice, or a zero-frame pull -- all of them legitimate
// empty answers that decode nothing and advance nothing. Source EOF does not
// remove the voice: it stays enumerable until Acknowledge catches `produced`.
//
// `kReadError` means the call was refused, not that it was empty: a null
// destination with `frames > 0`, an unknown or stopped handle, a frame count
// above `AudioValidation::kMaxReadFrames`, or a stream that returned more
// frames than it was given. A refusal never decodes and never moves `produced`,
// so the clock is untouched. `0` and `kReadError` are distinct on purpose: a
// host that conflates them cannot tell a drained voice from a broken decoder,
// and a broken decoder is the one failure that must not read as a clean end.

// `gain1000` and `pan1000` come back as the *current* values for this block, so
// a host pulls gain/pan changes rather than duplicating the engine's ramp math.
// Either pointer may be null.
size_t ReadStereo(Audio& audio, int64_t handle, int16_t* out, size_t frames,
                  int* gain1000, int* pan1000);

bool Ended(const Audio& audio, int64_t handle);

// Speaker consumed `totalPlayedFrames` of this voice (cumulative, not a delta).
// 1 accepted, -1 refused (unknown handle, backward, or past produced). Does not
// move the decode cursor. A voice stays enumerable until this catches produced
// after source EOF; Stop/Shutdown remove immediately.
int Acknowledge(Audio& audio, int64_t handle, uint64_t totalPlayedFrames);

} // namespace AudioWeb
} // namespace artc

#endif // ARTC_AUDIO_WEB_H
