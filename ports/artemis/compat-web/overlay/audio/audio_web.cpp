// Artemis compat audio backend for the web target.
//
// Compiled *instead of* `src/audio/audio.cpp` (see `overlay/cmake/web.cmake`).
// Same upstream `Audio` class and the same `PcmStream` contract; the only thing
// that changes is where the PCM goes. OpenSL ES pushes into a device queue from
// a decode-ahead thread; the browser has no such thread here, so the engine
// host *pulls* a bounded block per voice per tick and hands it to WebAudio.
//
// No std::thread, no OpenSL, no AudioContext here. The link target is
// `-sENVIRONMENT=worker --no-entry`, so a thread this module started would
// block or fail to start; the host owns the clock and calls `Pump`.
//
// Money path: every exported pull is a fixed-capacity copy with an explicit
// short return. Nothing grows a buffer to hide a stall, and no failure is
// reported as success.
#include "audio/audio.h"
#include "audio/audio_validation.h"
#include "audio/audio_web.h"
#include "audio/vorbis_stream.h"
#include "log/logger.h"
#include "pack/pack_manager.h"

#include <algorithm>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace artc {

namespace AudioWeb {
// The largest value a `size_t` can hold. A pull can never legitimately return
// it, so `ReadStereo` returns it for a bad argument or a stream that broke its
// contract. The host reads it as -1 over the CABI. See `audio_web.h`.
const size_t kReadError = static_cast<size_t>(-1);
} // namespace AudioWeb

namespace {

inline int ClampVol(int vol1000) { return std::clamp(vol1000, 0, 1000); }
inline int ClampPan(int pan1000) { return std::clamp(pan1000, -1000, 1000); }

} // namespace

// ---------------------------------------------------------------------------
// One voice. Owns its decoder and the bookkeeping the engine asks about.
// ---------------------------------------------------------------------------
struct Audio::Impl {
    PackManager* packs = nullptr;

    struct Voice {
        std::string key;
        std::unique_ptr<PcmStream> source;
        int rate = 0;
        bool paused = false;
        // Gain/pan as the engine set them. The 0..1000 / -1000..1000 ranges are
        // the upstream range; a host maps them to WebAudio itself.
        int gain = 1000;
        int pan = 0;
        // Frames the decoder has produced, and frames the speaker has consumed
        // (Acknowledge). `produced - consumed` is queued ahead of the speaker and
        // is not the clock. PlaybackMs / IsPlaying use consumed only.
        uint64_t produced = 0;
        uint64_t consumed = 0;
        // Voice handle. Assigned once when the voice is published and never
        // reused, so a handle the host kept past a `Stop` finds nothing rather
        // than a later voice.
        uint64_t cursor = 0;
        bool ended = false;
    };

    std::mutex mutex;
    std::map<std::string, std::shared_ptr<Voice>> voices;
    // Monotonic across the whole session: a handle must not be reused by a
    // later voice, or a stale handle would silently address the new one.
    uint64_t next_serial = 1;
    std::map<uint64_t, std::shared_ptr<Voice>> by_serial;
    std::vector<std::shared_ptr<Voice>> order; // enumeration order, insertion-stable
    uint32_t pump_calls = 0;
};

namespace {

std::shared_ptr<Audio::Impl::Voice> FindVoice(Audio::Impl& impl, const std::string& key) {
    const auto it = impl.voices.find(key);
    return it == impl.voices.end() ? nullptr : it->second;
}

std::shared_ptr<Audio::Impl::Voice> FindHandle(Audio::Impl& impl, int64_t handle) {
    if (handle <= 0) return nullptr;
    const auto it = impl.by_serial.find(static_cast<uint64_t>(handle));
    return it == impl.by_serial.end() ? nullptr : it->second;
}

// True when this voice has no PCM left to give *and* the host has consumed
// everything already produced. A looping voice never ends here, which is what
// makes `[wait se]` behave.
bool VoiceDrained(const Audio::Impl::Voice& v) {
    return v.ended && v.consumed >= v.produced;
}

int AckVoice(Audio::Impl::Voice& v, uint64_t totalPlayed) {
    if (totalPlayed > v.produced || totalPlayed < v.consumed) return -1;
    v.consumed = totalPlayed;
    return 1;
}

// --- Reaching `Impl` from the bridge --------------------------------------
//
// `Audio::impl_` is private and `audio/audio.h` is upstream: frozen, not ours to
// widen. A free function in `AudioWeb` therefore cannot name the pointer — and
// no friend, member-pointer or static-back-pointer trick changes that, because
// all of them need an access expression written where `impl_` is visible.
//
// `Audio`'s own members *are* visible to it, and this file defines them (it is
// compiled instead of `src/audio/audio.cpp`). So the constructor and destructor
// — the two members that already touch `impl_` — publish the association here,
// keyed by the address of the `Audio` instance. Per-instance, so two engines in
// one process do not collide, and no upstream header changes.
std::mutex g_registry_mutex;
std::map<const Audio*, Audio::Impl*> g_registry;

void Register(Audio& audio, Audio::Impl* impl) {
    std::lock_guard<std::mutex> lk(g_registry_mutex);
    g_registry[&audio] = impl;
}
void Unregister(Audio& audio) {
    std::lock_guard<std::mutex> lk(g_registry_mutex);
    g_registry.erase(&audio);
}
// Null for an `Audio` this translation unit did not construct, which is exactly
// the "not our backend" case the bridge has to refuse rather than guess at.
Audio::Impl* Lookup(const Audio& audio) {
    std::lock_guard<std::mutex> lk(g_registry_mutex);
    const auto it = g_registry.find(&audio);
    return it == g_registry.end() ? nullptr : it->second;
}

} // namespace

Audio::Audio() : impl_(new Impl) { Register(*this, impl_); }
Audio::~Audio() { Unregister(*this); StopAll(); delete impl_; }

void Audio::Init(PackManager* packs) {
    impl_->packs = packs;
    Log(kLogInfo, "audio(web): pull backend ready");
}

void Audio::Shutdown() {
    StopAll();
    impl_->packs = nullptr;
}

// Play decodes from the pack chain, exactly like the Android backend, so a
// script's `[playbgm]` works without the host knowing about Vorbis.
bool Audio::Play(const std::string& key, const std::string& file, bool loop, int vol) {
    if (!impl_->packs) return false;
    auto source = std::make_unique<VorbisStream>();
    if (!source->Open(
            [this](const std::string& name, std::vector<uint8_t>& bytes) {
                return impl_->packs->Read(name, bytes);
            },
            file, loop)) {
        return false;
    }
    return PlayStream(key, std::move(source), vol);
}

bool Audio::PlayStream(const std::string& key, std::unique_ptr<PcmStream> pcm, int vol) {
    if (!pcm) return false;
    const int rate = pcm->SampleRate();
    if (rate <= 0) return false;

    auto voice = std::make_shared<Impl::Voice>();
    voice->key = key;
    voice->source = std::move(pcm);
    voice->rate = rate;
    voice->gain = ClampVol(vol);
    voice->paused = false;

    std::lock_guard<std::mutex> lk(impl_->mutex);
    // Replacing a key is a replacement, not a second voice: the map key is the
    // engine's identity and two voices under it would double-play. Drop the old
    // one from every index before publishing the new one.
    if (const auto old = FindVoice(*impl_, key)) {
        for (auto o = impl_->order.begin(); o != impl_->order.end(); ++o) {
            if (o->get() == old.get()) { impl_->order.erase(o); break; }
        }
        for (auto s = impl_->by_serial.begin(); s != impl_->by_serial.end();) {
            s = s->second == old ? impl_->by_serial.erase(s) : std::next(s);
        }
        impl_->voices.erase(key);
    }
    // The handle is the serial, kept on the voice so a caller's handle can be
    // checked against a live voice rather than trusted to address one.
    voice->cursor = impl_->next_serial++;
    impl_->voices[key] = voice;
    impl_->by_serial[voice->cursor] = voice;
    impl_->order.push_back(voice);
    Log(kLogInfo, "audio(web): play " + key + " rate=" + std::to_string(rate));
    return true;
}

double Audio::PlaybackMs(const std::string& key) const {
    std::lock_guard<std::mutex> lk(const_cast<Impl*>(impl_)->mutex);
    const auto v = FindVoice(*impl_, key);
    if (!v || v->rate <= 0) return -1;
    // Presented position, per the upstream contract: the frames the speaker has
    // actually been handed. `produced - handed` is the opposite quantity (PCM
    // decoded and still buffered), so it is never the clock. A voice the page
    // has not pulled from has presented nothing and so reads 0 ms -- which is
    // what the video-sync caller in `render/video_player.cpp` expects, since it
    // advances the clip together with this number.
    return 1000.0 * static_cast<double>(v->consumed) / static_cast<double>(v->rate);
}

void Audio::Stop(const std::string& key) {
    std::lock_guard<std::mutex> lk(impl_->mutex);
    const auto it = impl_->voices.find(key);
    if (it == impl_->voices.end()) return;
    for (auto o = impl_->order.begin(); o != impl_->order.end(); ++o) {
        if (o->get() == it->second.get()) { impl_->order.erase(o); break; }
    }
    for (auto s = impl_->by_serial.begin(); s != impl_->by_serial.end();) {
        s = s->second == it->second ? impl_->by_serial.erase(s) : std::next(s);
    }
    impl_->voices.erase(it);
}

void Audio::StopAll() {
    std::lock_guard<std::mutex> lk(impl_->mutex);
    impl_->voices.clear();
    impl_->by_serial.clear();
    impl_->order.clear();
}

bool Audio::IsPlaying(const std::string& key) const {
    std::lock_guard<std::mutex> lk(const_cast<Impl*>(impl_)->mutex);
    const auto v = FindVoice(*impl_, key);
    if (!v) return false;
    // A paused voice is still playing: PauseAll suspends output, it does not
    // finish the sound, and `[wait se]` must not fall through on a pause.
    return !VoiceDrained(*v);
}

void Audio::SetVolume(const std::string& key, int vol) {
    std::lock_guard<std::mutex> lk(impl_->mutex);
    if (const auto v = FindVoice(*impl_, key)) v->gain = ClampVol(vol);
}

void Audio::SetPan(const std::string& key, int pan) {
    std::lock_guard<std::mutex> lk(impl_->mutex);
    if (const auto v = FindVoice(*impl_, key)) v->pan = ClampPan(pan);
}

void Audio::PauseAll() {
    std::lock_guard<std::mutex> lk(impl_->mutex);
    for (const auto& kv : impl_->voices) kv.second->paused = true;
}

void Audio::ResumeAll() {
    std::lock_guard<std::mutex> lk(impl_->mutex);
    for (const auto& kv : impl_->voices) kv.second->paused = false;
}

// ---------------------------------------------------------------------------
// Bridge. Every entry point resolves the instance first and refuses when there
// is none: a null `Impl` is a caller that is not using this backend.
// ---------------------------------------------------------------------------
void AudioWeb::Init(Audio&) {}
void AudioWeb::Shutdown(Audio&) {}

void AudioWeb::Pump(Audio& audio, double, uint32_t onceFrameCap) {
    Audio::Impl* p = Lookup(audio);
    if (!p || onceFrameCap == 0) return;
    Audio::Impl& impl = *p;

    std::lock_guard<std::mutex> lk(impl.mutex);
    // Decode-free. ReadStereo is the only consumer, so the clock advances only
    // where PCM is consumed. Pump only reaps voices whose PCM ran out.
    for (auto it = impl.order.begin(); it != impl.order.end();) {
        if (VoiceDrained(**it)) {
            it = impl.order.erase(it);
        } else {
            ++it;
        }
    }
    ++impl.pump_calls;
}

uint64_t AudioWeb::PlayedFrames(const Audio& audio, const char* key) {
    if (!key) return 0;
    Audio::Impl* p = Lookup(audio);
    if (!p) return 0;
    Audio::Impl& impl = *p;
    std::lock_guard<std::mutex> lk(impl.mutex);
    const auto v = FindVoice(impl, key);
    // Frames handed to the host, i.e. how far the voice has been presented.
    // This is the same quantity as `PlaybackMs` before the rate division, and
    // is what the page uses to feed its played-frame feedback.
    return v ? v->consumed : 0;
}

int AudioWeb::VoiceCount(const Audio& audio) {
    Audio::Impl* p = Lookup(audio);
    if (!p) return 0;
    Audio::Impl& impl = *p;
    std::lock_guard<std::mutex> lk(impl.mutex);
    return static_cast<int>(impl.order.size());
}

int64_t AudioWeb::VoiceHandle(const Audio& audio, int index) {
    Audio::Impl* p = Lookup(audio);
    if (!p) return 0;
    Audio::Impl& impl = *p;
    std::lock_guard<std::mutex> lk(impl.mutex);
    if (index < 0 || static_cast<size_t>(index) >= impl.order.size()) return 0;
    return static_cast<int64_t>(impl.order[static_cast<size_t>(index)]->cursor);
}

const char* AudioWeb::VoiceKey(const Audio& audio, int64_t handle) {
    Audio::Impl* p = Lookup(audio);
    if (!p) return nullptr;
    Audio::Impl& impl = *p;
    std::lock_guard<std::mutex> lk(impl.mutex);
    const auto v = FindHandle(impl, handle);
    return v ? v->key.c_str() : nullptr;
}

int AudioWeb::VoiceSampleRate(const Audio& audio, int64_t handle) {
    Audio::Impl* p = Lookup(audio);
    if (!p) return 0;
    Audio::Impl& impl = *p;
    std::lock_guard<std::mutex> lk(impl.mutex);
    const auto v = FindHandle(impl, handle);
    return v ? v->rate : 0;
}

size_t AudioWeb::ReadStereo(Audio& audio, int64_t handle, int16_t* out, size_t frames,
                            int* gain1000, int* pan1000) {
    // Bad arguments are the exception sentinel, not a zero: `0` is also the
    // honest "this voice has no PCM left" answer, and the host refused to tell
    // a broken decoder from an empty one. Every refusal below happens before
    // any decode, so `produced`/`consumed` never move.
    if (!out && frames) return kReadError;
    if (frames > AudioValidation::kMaxReadFrames) return kReadError;
    Audio::Impl* p = Lookup(audio);
    if (!p) return kReadError;
    Audio::Impl& impl = *p;
    std::lock_guard<std::mutex> lk(impl.mutex);
    const auto v = FindHandle(impl, handle);
    if (!v) return kReadError;
    if (gain1000) *gain1000 = v->gain;
    if (pan1000) *pan1000 = v->pan;
    // A zero-frame pull, a paused voice and a drained voice all yield nothing.
    // Those are legitimate empty answers, not refusals.
    if (frames == 0 || v->paused || v->ended) return 0;

    const size_t got = v->source->ReadStereo(out, frames);
    // A stream that wrote more frames than the buffer holds has already
    // overrun it. Those frames are not counted, and the caller is told the
    // read failed instead of being handed a plausible length.
    if (got > frames) return kReadError;
    v->produced += got;
    if (v->source->Ended()) v->ended = true;
    return got;
}

bool AudioWeb::Ended(const Audio& audio, int64_t handle) {
    Audio::Impl* p = Lookup(audio);
    if (!p) return true;
    Audio::Impl& impl = *p;
    std::lock_guard<std::mutex> lk(impl.mutex);
    const auto v = FindHandle(impl, handle);
    return v ? VoiceDrained(*v) : true;
}

int AudioWeb::Acknowledge(Audio& audio, int64_t handle, uint64_t totalPlayedFrames) {
    Audio::Impl* p = Lookup(audio);
    if (!p) return -1;
    Audio::Impl& impl = *p;
    std::lock_guard<std::mutex> lk(impl.mutex);
    const auto v = FindHandle(impl, handle);
    if (!v) return -1;
    return AckVoice(*v, totalPlayedFrames);
}

} // namespace artc
