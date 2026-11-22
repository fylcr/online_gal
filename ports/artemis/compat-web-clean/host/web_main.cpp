#include "host/web_abi.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/heap.h>
#include <GLES2/gl2.h>
#include "audio/audio.h"
#include "audio/audio_validation.h"
#include "audio/audio_web.h"
#include "core/engine_context.h"
#include "log/logger.h"
#include "render/compositor.h"
#include "script/asb_parser.h"
#include "script/dialog_request.h"
#include "script/lua_engine.h"
#include <algorithm>
#include <cmath>
#include <deque>
#include <exception>
#include <limits>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>

using namespace artc;
namespace {
constexpr const char *canvas_target = "#canvas";
struct Input {
    float x = 0, y = 0;
    int key = 1;
    bool keyboard = false, move = false, down = false;
};
struct Host {
    std::unique_ptr<EngineContext> ctx;
    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE gl = 0;
    int stage_w = 1280, stage_h = 720, css_w = 1280, css_h = 720;
    bool booted = false, paused = false, exit_requested = false;
    bool output_invalidated = false;
    uint64_t drawn_rev = ~0ull;
    unsigned int presented = 0;
    std::string error;
    std::deque<Input> events;

    bool Started() const { return ctx && ctx->Started() && booted; }
    bool Current() {
        if (!gl || emscripten_is_webgl_context_lost(gl) ||
            emscripten_webgl_make_context_current(gl) != EMSCRIPTEN_RESULT_SUCCESS) {
            error = "WebGL context unavailable or lost";
            return false;
        }
        return true;
    }
    void Release() {
        // Even a lost WebGL context must remain selected while the graph drops
        // its GL objects; deletion on a lost context is a WebGL no-op.
        if (gl) emscripten_webgl_make_context_current(gl);
        if (ctx) ctx->Shutdown();
        ctx.reset();
        if (gl) emscripten_webgl_destroy_context(gl);
        gl = 0;
        booted = paused = false;
        output_invalidated = false;
        events.clear();
    }
    int Fail(const std::string &why) {
        error = why;
        exit_requested = true;
        Release();
        Log(kLogError, "clean web host: " + error);
        return 0;
    }
    int Open(const char *pack, const char *os_id) {
        Release();
        error.clear();
        exit_requested = false;
        drawn_rev = ~0ull;
        presented = 0;
        stage_w = css_w = 1280;
        stage_h = css_h = 720;
        if (!pack || !*pack) return Fail("empty pack path");
        EmscriptenWebGLContextAttributes attrs;
        emscripten_webgl_init_context_attributes(&attrs);
        attrs.alpha = attrs.depth = attrs.antialias = false;
        attrs.stencil = true;
        attrs.premultipliedAlpha = true;
        attrs.preserveDrawingBuffer = false;
        attrs.majorVersion = 2;
        attrs.minorVersion = 0;
        attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
        attrs.failIfMajorPerformanceCaveat = false;
        gl = emscripten_webgl_create_context(canvas_target, &attrs);
        if (!Current()) return Fail("cannot create/current WebGL2 on #canvas");
        ctx = std::make_unique<EngineContext>();
        if (!ctx->Open(pack, os_id && *os_id ? os_id : "windows"))
            return Fail(std::string("cannot open pack: ") + pack);
        stage_w = ctx->stageW();
        stage_h = ctx->stageH();
        css_w = stage_w;
        css_h = stage_h;
        return 1;
    }
    bool Boot() {
        if (!ctx || !ctx->Start()) { error = "engine Start failed"; return false; }
        ctx->lua().SetDialogHandler([](DialogRequest &req) {
            // This target is a headless worker: there is no window dialog.
            req.accepted = false;
            Log(kLogWarn, "clean web host: window dialog unavailable");
            return false;
        });
        if (!ctx->BootFramework()) { error = "BootFramework failed"; return false; }
        booted = true;
        return true;
    }
    void Queue(const Input &event) {
        if (!ctx || exit_requested) return;
        events.push_back(event);
        if (events.size() > 512) events.pop_front();
    }
    void MapPoint(float px, float py, float *sx, float *sy) const {
        const float s = std::min((float)css_w / (float)stage_w, (float)css_h / (float)stage_h);
        const float vw = (float)stage_w * s, vh = (float)stage_h * s;
        const float vx = ((float)css_w - vw) * 0.5f, vy = ((float)css_h - vh) * 0.5f;
        *sx = (px - vx) / vw * (float)stage_w;
        *sy = (py - vy) / vh * (float)stage_h;
    }

    void DrainInput() {
        std::deque<Input> batch;
        batch.swap(events);
        auto &lua = ctx->lua();
        int touch = 0;
        bool tapped = false;
        float tap_x = 0, tap_y = 0;
        for (const auto &ev : batch) {
            if (ev.keyboard) {
                if (ev.down) lua.PushKeyDown(ev.key);
                else lua.PushKeyUp(ev.key);
                continue;
            }
            float sx = 0, sy = 0;
            MapPoint(ev.x, ev.y, &sx, &sy);
            lua.SetMousePoint(sx, sy);
            if (ev.move) { lua.DragMove(sx, sy); continue; }
            if (ev.down) {
                lua.PushKeyDown(1);
                touch = 1;
                lua.BeginDrag(sx, sy);
            } else {
                lua.PushKeyUp(1);
                touch = 0;
                const bool dragging = lua.DragMoved();
                lua.EndDrag();
                if (!dragging) { tapped = true; tap_x = sx; tap_y = sy; }
            }
            lua.SetTouchCount(touch);
        }
        if (tapped) lua.ClickAt(tap_x, tap_y);
    }
    void DrainQueued() {
        if (!ctx || !ctx->Started()) return;
        auto &lua = ctx->lua();
        auto &runner = ctx->runner();
        while (!lua.IsWaiting() && lua.HasQueuedTag()) {
            std::string name;
            std::vector<std::pair<std::string, std::string>> attrs;
            if (!lua.PopQueuedTag(&name, &attrs)) break;
            if (name == "jump" || name == "call") {
                std::string file, label;
                for (const auto &kv : attrs) {
                    if (kv.first == "file") file = kv.second;
                    else if (kv.first == "label") label = kv.second;
                }
                if (name == "call") runner.Call(file, label);
                else runner.Jump(file, label);
            } else lua.DispatchTag(name, attrs, false);
        }
    }
    void Step() {
        if (!ctx || !ctx->Started()) return;
        auto &lua = ctx->lua();
        auto &runner = ctx->runner();
        if (runner.Loaded() && !runner.Halted() && !lua.IsWaiting()) {
            for (int i = 0; i < 4 && runner.Loaded() && !runner.Halted(); ++i) {
                runner.ExecuteLine(lua);
                DrainQueued();
                if (lua.IsWaiting()) break;
            }
        } else if (!lua.IsWaiting() && lua.HasQueuedTag()) DrainQueued();
    }
    void Present() {
        int width = 0, height = 0;
        if (emscripten_get_canvas_element_size(canvas_target, &width, &height) !=
                EMSCRIPTEN_RESULT_SUCCESS || width <= 0 || height <= 0)
            throw std::runtime_error("invalid canvas backing size");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glViewport(0, 0, width, height);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        const float s = std::min(float(width) / stage_w, float(height) / stage_h);
        const int vw = int(stage_w * s), vh = int(stage_h * s);
        glViewport((width - vw) / 2, (height - vh) / 2, vw, vh);
        ctx->compositor().Draw();
        ++presented;
    }
    int Frame() {
        if (exit_requested) return 0;
        if (!ctx) return Fail("frame before open");
        if (!Current()) return Fail(error);
        if (paused) return 1;
        // Clean mac_main.mm policy, including its native redraw gate and
        // separate NowMs reads. No boot Draw, wait gate or script transaction.
        if (ctx->Started() && ctx->lua().ConsumeResetRequest()) {
            ctx->ResetSession();
            booted = false;
            drawn_rev = ~0ull;
        }
        if (ctx->Started() && ctx->lua().ConsumeExitRequest()) {
            exit_requested = true;
            Release();
            return 0;
        }
        if (!booted && !Boot()) return Fail(error);
        DrainInput();
        ctx->lua().RunEnterFrame();
        DrainQueued();
        Step();
        ctx->compositor().Update(ctx->lua().NowMs());
        const double now = ctx->lua().NowMs();
        const bool animating = ctx->compositor().PendingAnimationMs(now) > 0 ||
            ctx->compositor().TransitionActive() || ctx->compositor().PendingTextMs(now) > 0;
        if (output_invalidated || drawn_rev != ctx->compositor().Revision() || animating) {
            Present();
            output_invalidated = false;
            drawn_rev = ctx->compositor().Revision();
        }
        ctx->lua().EndFrame();
        return 1;
    }
};
Host host;
// C++ exceptions cannot cross the exported lifecycle operations unnoticed.
template<class F> int Guard(F operation) {
    try { return operation(); }
    catch (const std::exception &e) { return host.Fail(e.what()); }
    catch (...) { return host.Fail("unknown native exception"); }
}
bool Ready() { return host.Started() && !host.exit_requested; }
}

extern "C" {
int artc_web_open(const char *pack, const char *os_id) {
    return Guard([&] { return host.Open(pack, os_id); });
}
int artc_web_frame(void) { return Guard([] { return host.Frame(); }); }
void artc_web_pointer(float x, float y, int down) {
    if (std::isfinite(x) && std::isfinite(y)) host.Queue({x, y, 1, false, false, down != 0});
}
void artc_web_pointer_move(float x, float y) {
    if (std::isfinite(x) && std::isfinite(y)) host.Queue({x, y, 1, false, true, false});
}
void artc_web_key(int key, int down) { host.Queue({0, 0, key, true, false, down != 0}); }
void artc_web_resize(int css_w, int css_h) {
    if (css_w > 0 && css_h > 0) {
        host.css_w = css_w;
        host.css_h = css_h;
        // A valid notification may follow a backing reset even with identical CSS.
        host.output_invalidated = true;
    }
}
int artc_web_started(void) { return Ready() ? 1 : 0; }
int artc_web_stage_width(void) { return host.stage_w; }
int artc_web_stage_height(void) { return host.stage_h; }
int artc_web_revision(void) { return Ready() ? int(host.ctx->compositor().Revision()) : 0; }
int artc_web_presented(void) { return int(host.presented); }
int artc_web_exit_requested(void) { return host.exit_requested ? 1 : 0; }
const char *artc_web_last_error(void) { return host.error.c_str(); }
int artc_web_pause(int paused) {
    return Guard([&] {
        if (!Ready()) { host.error = "pause before start"; return 0; }
        if (paused) { host.ctx->lua().PauseClock(); host.ctx->lua().PauseAudio(); }
        else { host.ctx->lua().ResumeClock(); host.ctx->lua().ResumeAudio(); }
        host.paused = paused != 0;
        return 1;
    });
}
double artc_web_now_ms(void) {
    return Ready() ? host.ctx->lua().NowMs() : std::numeric_limits<double>::quiet_NaN();
}
void artc_web_dispose(void) {
    Guard([] { host.Release(); return 1; });
    host.exit_requested = false;
    host.presented = 0;
    host.drawn_rev = ~0ull;
}
int artc_web_save(const char *file) {
    return Guard([&] {
        if (!Ready() || !file || !*file || !host.Current()) { host.error = "save unavailable"; return 0; }
        const bool ok = host.ctx->lua().SaveSnapshot(file);
        if (!ok) host.error = "SaveSnapshot refused";
        return ok ? 1 : 0;
    });
}
int artc_web_load(const char *file) {
    return Guard([&] {
        if (!Ready() || !file || !*file || !host.Current()) { host.error = "load unavailable"; return 0; }
        const bool ok = host.ctx->lua().LoadSnapshot(file);
        if (!ok) host.error = "LoadSnapshot refused";
        return ok ? 1 : 0;
    });
}
void artc_web_log_sink(void (*sink)(int level, const char *message)) {
    if (!sink) SetLogSink(LogSink{});
    else SetLogSink([sink](int level, const std::string &message) { sink(level, message.c_str()); });
}
int artc_web_audio_pump(void) {
    return Guard([] {
        if (!Ready()) return 0;
        AudioWeb::Pump(host.ctx->audio(), host.ctx->lua().NowMs(), AudioValidation::kMaxReadFrames);
        return 1;
    });
}
int artc_web_audio_voice_count(void) { return Ready() ? AudioWeb::VoiceCount(host.ctx->audio()) : 0; }
double artc_web_audio_voice_handle(int index) {
    return Ready() ? double(AudioWeb::VoiceHandle(host.ctx->audio(), index)) : 0;
}
const char *artc_web_audio_voice_key(double handle) {
    int64_t h = 0;
    if (!Ready() || !AudioValidation::AudioHandleOk(handle, &h)) return "";
    const char *key = AudioWeb::VoiceKey(host.ctx->audio(), h);
    return key ? key : "";
}
int artc_web_audio_voice_rate(double handle) {
    int64_t h = 0;
    return Ready() && AudioValidation::AudioHandleOk(handle, &h) ?
        AudioWeb::VoiceSampleRate(host.ctx->audio(), h) : 0;
}
int artc_web_audio_read_stereo(double handle, uintptr_t heapDest, int frames,
                              intptr_t outGain, intptr_t outPan) {
    if (!Ready()) return -1;
    int64_t h = 0;
    uint32_t n = 0;
    if (!AudioValidation::ValidateAudioReadParams(handle, heapDest, frames,
            uintptr_t(outGain), uintptr_t(outPan), emscripten_get_heap_size(), &h, &n)) return -1;
    try {
        int gain = 0, pan = 0;
        const size_t got = AudioWeb::ReadStereo(host.ctx->audio(), h,
            reinterpret_cast<int16_t *>(heapDest), n, &gain, &pan);
        if (got > n) return -1;
        if (outGain) *reinterpret_cast<int *>(outGain) = gain;
        if (outPan) *reinterpret_cast<int *>(outPan) = pan;
        return int(got);
    } catch (const std::exception &e) {
        host.Fail(e.what());
    } catch (...) {
        host.Fail("audio decoder exception");
    }
    return -1;
}
int artc_web_audio_ended(double handle) {
    int64_t h = 0;
    if (!Ready() || !AudioValidation::AudioHandleOk(handle, &h)) return 1;
    return AudioWeb::Ended(host.ctx->audio(), h) ? 1 : 0;
}
double artc_web_audio_played_frames(const char *key) {
    return Ready() && key ? double(AudioWeb::PlayedFrames(host.ctx->audio(), key)) : 0;
}
int artc_web_audio_ack(double handle, double cumulativeFrames) {
    int64_t h = 0;
    uint64_t n = 0;
    if (!Ready() || !AudioValidation::AudioHandleOk(handle, &h) ||
        !AudioValidation::AudioFramesOk(cumulativeFrames, &n)) return -1;
    return AudioWeb::Acknowledge(host.ctx->audio(), h, n);
}
}
