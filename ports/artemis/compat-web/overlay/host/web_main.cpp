//---------------------------------------------------------------------------
// Web (Emscripten) host for artemis-compat.
//
// Modelled directly on `src/host/mac_main.mm`. The boot sequence, the queued-input model and the
// per-frame order are the same, deliberately: reset/exit gate -> boot -> drain input -> EnterFrame ->
// drain queued tags -> Step -> compositor update -> gated Present -> EndFrame. Keeping the order
// identical means a bug found on one host is a bug on the other, rather than a difference in hosts.
//
// Only the platform surface differs:
//
//   Cocoa window + legacy OpenGL 2.1   ->  a canvas + a WebGL2 context through Emscripten's GLES2
//   NSTimer at the display refresh     ->  the host calls artc_web_frame (this module has no rAF)
//   NSEvent                            ->  browser events, via the exported C entry points below
//
// The redraw gate is kept as-is. It matters more here than on the desktop: a browser compositor
// charges for every presented frame, and an ADV scene is static most of the time, so skipping
// clear+draw when the layer revision has not moved and nothing is animating is the difference
// between a warm laptop and a hot one.
//---------------------------------------------------------------------------

#include <emscripten.h>
#include <emscripten/html5.h>
// `emscripten_get_heap_size` is declared here and was only reachable transitively
// before the heap check moved out of a local helper. Included explicitly so the
// heap bound does not depend on which other Emscripten header happens to pull it in.
#include <emscripten/heap.h>

#include <GLES2/gl2.h>

#include "config/ini.h"
#include "core/engine_context.h"
#include "audio/audio_validation.h"
#include "audio/audio_web.h"
#include "log/logger.h"
#include "pack/pack_manager.h"
#include "render/compositor.h"
#include "script/asb_parser.h"
#include "script/dialog_request.h"
#include "script/iet_interpreter.h"
#include "script/lua_engine.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <cstdlib>
#include <deque>
#include <memory>
#include <string>
#include <vector>

/**
 * Bridges a `[dialog]` request to the page, **synchronously**.
 *
 * The Android host does the same thing with a poll loop (`ShowNativeInput`): it posts the request to
 * the UI thread and waits. In a browser the wait is free, because `confirm`/`prompt` block the
 * calling thread by design — so the tag's synchronous request/response contract survives with no
 * async plumbing, and the engine does not need to be resumable.
 *
 * Returns a string the caller must `free`. `stringToNewUTF8` puts it on the wasm heap, which is the
 * only way a JS string can outlive the call; a JS-side string is garbage after the call returns.
 * Returns 0 when the page installed no handler, which the caller treats as "declined".
 *
 * `kind`: 0 = message-only, 1 = yes/no confirm, 2 = text input.
 */
EM_JS(char *, artc_js_dialog, (const char *title, const char *message, int kind), {
    if (typeof window === 'undefined' || typeof window.__artcDialog !== 'function') {
        return 0;
    }
    try {
        const result = window.__artcDialog(kind, UTF8ToString(title), UTF8ToString(message));
        return stringToNewUTF8(result === null || result === undefined ? '' : String(result));
    } catch (error) {
        console.error('[artc] dialog handler threw', error);
        return 0;
    }
});

using namespace artc;

namespace {

constexpr int kKeyTap = 1;

/**
 * How the GL canvas is named to Emscripten.
 *
 * Every `emscripten_*` entry point that takes a canvas takes a **target string**, and resolution is
 * `document.querySelector(target)` — so the target has to be a selector the host can answer. The canvas
 * is `#canvas` in the page, and the worker's `document` shim answers the same selector with the
 * `OffscreenCanvas` it was handed, which keeps one name working on both hosts.
 *
 * Worth stating because the obvious spelling is wrong: `0` is not "the default canvas" here.
 * Emscripten looks a numeric target up in `specialHTMLTargets`, whose slot 0 is the literal `0` (the
 * document sits in slot 1), so a `0` target falls through to `document.querySelector(0)` and resolves
 * nothing — on the main thread as much as in a worker. That produced a `create_context` failure with no
 * GL error and no explanation, because the call never reached WebGL.
 */
constexpr const char *kCanvasTarget = "#canvas";

// Frames a page may pull from one voice in one call. The value and the cap it
// enforces both live in the shared validation header, so the host and the audio
// test read one constant instead of two copies that can drift.
constexpr uint32_t kAudioPullFrames = AudioValidation::kMaxReadFrames;

// The three argument checks are `AudioValidation::*` in
// `overlay/audio/audio_validation.h`. They used to be file-static here, which
// left the audio test asserting a *copy* of the rules; the header is includable
// without Emscripten, so the test now calls these exact functions. The names
// below are kept because call sites in this file read as host-side prose
// (`AudioHandleOk` on a handle the page passed), not because the logic is local.
//
// `HeapSpanOk` binds the live wasm heap: the shared function takes the heap size
// as an argument so it stays testable off-Emscripten, and this is the only place
// that supplies the real one.
using AudioValidation::AudioFramesOk;
using AudioValidation::AudioHandleOk;

bool HeapSpanOk(uintptr_t addr, uint64_t bytes, uintptr_t align) {
    return AudioValidation::HeapSpanOk(addr, bytes, align,
                                       static_cast<uintptr_t>(emscripten_get_heap_size()));
}

struct RawInput {
    bool is_key = false;
    bool is_move = false;
    bool down = false;
    int key = 0;
    float x = 0, y = 0;
};

class WebEngine {
public:
    bool Open(const std::string &game_path, const std::string &os) {
        ctx = std::make_unique<EngineContext>();
        if (!ctx->Open(game_path, os)) {
            ctx.reset();
            return false;
        }
        stage_w = ctx->stageW();
        stage_h = ctx->stageH();
        Log(kLogInfo, "web host: stage=" + std::to_string(stage_w) + "x" + std::to_string(stage_h));
        return true;
    }

    bool Boot() {
        if (!ctx || !ctx->Start()) return false;

        // The `[dialog]` host box.
        //
        // The Android host (`src/jni/native_activity.cpp`, `ShowNativeInput`) answers these
        // **synchronously**: it posts the request to the UI thread and polls until the box is
        // dismissed, with the engine paused meanwhile. The browser can do the same thing more
        // directly, because `window.confirm` and `window.prompt` are themselves synchronous and block
        // the calling thread — so the tag's request/response contract is preserved with no async
        // plumbing at all.
        //
        // Answering with a fabricated value would be worse than refusing: accepting a `[confirm]` the
        // player never saw tells the game to take a branch on its own.
        ctx->lua().SetDialogHandler([](DialogRequest &req) -> bool {
            const int kind = req.has_input ? 2 : (req.has_result ? 1 : 0);
            char *answer = artc_js_dialog(req.title.c_str(), req.message.c_str(), kind);
            if (!answer) {
                req.accepted = false;
                return false;
            }
            const std::string text = answer;
            free(answer);
            if (kind == 2) {
                // A prompt returns the typed text, or null when cancelled; the page encodes cancel as
                // an empty answer, which is also a legitimate empty entry, so acceptance is decided by
                // the page and reported through `accepted`.
                req.accepted = !text.empty() || req.has_input;
                req.text = text;
                if (req.textfieldsize > 0 && req.text.size() > (size_t)req.textfieldsize) {
                    req.text.resize(req.textfieldsize);
                }
                return req.accepted;
            }
            req.accepted = text == "1";
            return req.accepted;
        });

        // Establish the boot base frame **before the first script tag runs**.
        //
        // Why: `Compositor::BeginTransition` calls `CaptureFrame`, and `CaptureFrame`
        // returns immediately while `scene_fbo_ == 0`. The scene FBO only comes into
        // existence on the first `Draw`, and the boot chain (`system/first.iet` plus the
        // queued boot tags) runs its opening `[trans]` before any Draw has happened --
        // so the capture is skipped, `trans_have_frame_` stays false, `BeginTransition`
        // returns false, and the opening fade never activates. Drawing once here, with
        // no layers yet, allocates the FBO and clears it black, which makes the legal
        // old black frame that the opening transition captures.
        //
        // It must stay before `BootFramework()`: drawing afterwards would capture the
        // new picture the boot chain just staged and fade it into itself. The call is
        // skipped when there is no GL context for the same reason `Start()` failed.
        ctx->compositor().Draw();

        if (!ctx->BootFramework()) return false;
        booted = true;
        return true;
    }

    void Enqueue(const RawInput &ev) {
        events.push_back(ev);
        if (events.size() > 512) events.pop_front();
    }

    void Pointer(float x, float y, bool move, bool down) {
        RawInput ev;
        ev.x = x;
        ev.y = y;
        ev.is_move = move;
        ev.down = down;
        ev.key = kKeyTap;
        Enqueue(ev);
    }

    void Key(int key, bool down) {
        RawInput ev;
        ev.is_key = true;
        ev.key = key;
        ev.down = down;
        Enqueue(ev);
    }

    void Resize(int w, int h) {
        css_w = w;
        css_h = h;
    }

    bool ExitRequested() const { return exit_requested; }
    bool Started() const { return ctx && ctx->Started(); }
    int StageWidth() const { return stage_w; }
    int StageHeight() const { return stage_h; }
    unsigned long long Revision() const {
        return (ctx && ctx->Started()) ? ctx->compositor().Revision() : 0ull;
    }
    unsigned long long Presented() const { return presented; }
    const char *LastError() const { return last_error.c_str(); }

    /**
     * Record why a host entry point refused.
     *
     * `artc_web_open` has two independent reasons to fail — no GL context, or no pack — and the page
     * can only see the combined answer. Without a reason string here both collapse into the same
     * "open failed" with no detail, which is indistinguishable from the boot chain having died.
     */
    void SetLastError(const std::string &message) { last_error = message; }

    // PauseClock/PauseAudio are idempotent. Tick bails before any of them while paused.
    bool Pause(bool on) {
        if (!ctx || !ctx->Started()) return false;
        if (on) {
            ctx->lua().PauseClock();
            ctx->lua().PauseAudio();
        } else {
            ctx->lua().ResumeClock();
            ctx->lua().ResumeAudio();
        }
        paused = on;
        return true;
    }

    double NowMs() const {
        if (!ctx || !ctx->Started()) return std::numeric_limits<double>::quiet_NaN();
        return ctx->lua().NowMs();
    }

    // Audio PCM pull: the engine's Audio, so voices are the engine's. Callers
    // must have checked Started() -- ctx->audio() is only valid after Open().
    artc::Audio &Audio() { return ctx->audio(); }

    // file is a slot name under saveDir (the pack parent). SaveSnapshot/LoadSnapshot
    // already reject an empty path, system.dat, and a missing onSave/onLoad.
    bool Save(const char *file) {
        if (!ctx || !ctx->Started() || !file || !*file) return false;
        return ctx->lua().SaveSnapshot(file);
    }
    bool Load(const char *file) {
        if (!ctx || !ctx->Started() || !file || !*file) return false;
        return ctx->lua().LoadSnapshot(file);
    }

    void Dispose() {
        paused = false;
        if (ctx) ctx->Shutdown();
        ctx.reset();
        booted = false;
        exit_requested = false;
    }

    /**
     * One frame. Mirrors `mac_main.mm`'s `Tick` step for step.
     *
     * The backing-store size is read every frame rather than cached: a browser resizes a canvas by
     * changing its attributes, and a stale viewport shows up as a stretched or cropped picture rather
     * than as an error.
     */
    void Tick() {
        int fb_w = stage_w, fb_h = stage_h;
        emscripten_get_canvas_element_size(kCanvasTarget, &fb_w, &fb_h);
        if (fb_w <= 0 || fb_h <= 0) {
            fb_w = stage_w;
            fb_h = stage_h;
        }
        // Host owns the clock. A paused tick does not boot, step, or present.
        if (paused) return;

        if (ctx && ctx->Started() && ctx->lua().ConsumeResetRequest()) {
            Log(kLogInfo, "web host: reset requested; re-running the boot chain");
            ctx->ResetSession();
            booted = false;
            drawn_rev = ~0ull;
        }
        if (ctx && ctx->Started() && ctx->lua().ConsumeExitRequest()) {
            Log(kLogInfo, "web host: exit requested");
            exit_requested = true;
            return;
        }
        if (!booted && !Boot()) {
            last_error = "boot failed";
            exit_requested = true;
            return;
        }

        DrainInput();
        if (ctx && ctx->Started()) {
            ctx->lua().RunEnterFrame();
            DrainQueued();
        }
        Step();
        if (ctx && ctx->Started()) {
            // CRITICAL FIX: Call NowMs() only ONCE to ensure consistent timestamp
            // across Update() and animation checks. Multiple calls can return different
            // values in WASM, causing animation skips and frame stuttering.
            const double now = ctx->lua().NowMs();
            ctx->compositor().Update(now);
            const bool animating = ctx->compositor().PendingAnimationMs(now) > 0 ||
                                   ctx->compositor().TransitionActive() ||
                                   ctx->compositor().PendingTextMs(now) > 0;
            if (drawn_rev != ctx->compositor().Revision() || animating) {
                Present((float)fb_w, (float)fb_h);
                drawn_rev = ctx->compositor().Revision();
            }
            ctx->lua().EndFrame();
        }
    }

private:
    void MapPoint(float px, float py, float *sx, float *sy) const {
        const float s = std::min((float)css_w / (float)stage_w, (float)css_h / (float)stage_h);
        const float vw = (float)stage_w * s, vh = (float)stage_h * s;
        const float vx = ((float)css_w - vw) * 0.5f, vy = ((float)css_h - vh) * 0.5f;
        *sx = (px - vx) / vw * (float)stage_w;
        *sy = (py - vy) / vh * (float)stage_h;
    }

    void DrainInput() {
        std::deque<RawInput> batch;
        batch.swap(events);
        if (!ctx || !ctx->Started()) return;

        LuaEngine *lua = &ctx->lua();
        int touch = 0;
        bool tapped = false;
        float tap_x = 0, tap_y = 0;
        for (const auto &ev : batch) {
            if (ev.is_key) {
                if (ev.down) lua->PushKeyDown(ev.key);
                else lua->PushKeyUp(ev.key);
                continue;
            }
            float sx = 0, sy = 0;
            MapPoint(ev.x, ev.y, &sx, &sy);
            if (ev.is_move) {
                lua->SetMousePoint(sx, sy);
                lua->DragMove(sx, sy);
                continue;
            }
            lua->SetMousePoint(sx, sy);
            if (ev.down) {
                lua->PushKeyDown(kKeyTap);
                touch = 1;
                lua->BeginDrag(sx, sy);
            } else {
                lua->PushKeyUp(kKeyTap);
                touch = 0;
                const bool dragging = lua->DragMoved();
                lua->EndDrag();
                if (!dragging) {
                    tapped = true;
                    tap_x = sx;
                    tap_y = sy;
                }
            }
            lua->SetTouchCount(touch);
        }
        if (tapped) lua->ClickAt(tap_x, tap_y);
    }

    void DrainQueued() {
        if (!ctx || !ctx->Started()) return;
        LuaEngine &lua = ctx->lua();
        AsbRunner &runner = ctx->runner();
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
            } else {
                lua.DispatchTag(name, attrs, false);
            }
        }
    }

    void Step() {
        if (!ctx || !ctx->Started()) return;
        LuaEngine &lua = ctx->lua();
        AsbRunner &runner = ctx->runner();
        if (runner.Loaded() && !runner.Halted() && !lua.IsWaiting()) {
            for (int i = 0; i < 4 && runner.Loaded() && !runner.Halted(); ++i) {
                runner.ExecuteLine(lua);
                DrainQueued();
                if (lua.IsWaiting()) break;
            }
        } else if (!lua.IsWaiting() && lua.HasQueuedTag()) {
            DrainQueued();
        }
    }

    void Present(float fb_w, float fb_h) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glViewport(0, 0, (GLsizei)fb_w, (GLsizei)fb_h);
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Letterbox: fit the stage into the canvas and centre it. The stage never stretches, which is
        // what keeps circles round when the window's aspect ratio differs.
        const float s = std::min(fb_w / (float)stage_w, fb_h / (float)stage_h);
        const int vw = (int)((float)stage_w * s), vh = (int)((float)stage_h * s);
        const int vx = ((int)fb_w - vw) / 2, vy = ((int)fb_h - vh) / 2;
        glViewport(vx, vy, vw, vh);

        ctx->compositor().Draw();
        presented += 1;
    }

    std::unique_ptr<EngineContext> ctx;
    int stage_w = 1280, stage_h = 720;
    int css_w = 1280, css_h = 720;
    bool booted = false;
    bool exit_requested = false;
    bool paused = false;
    unsigned long long drawn_rev = ~0ull;
    unsigned long long presented = 0;
    std::string last_error;
    std::deque<RawInput> events;
};

WebEngine g_engine;
EMSCRIPTEN_WEBGL_CONTEXT_HANDLE g_gl = 0;

bool CreateGlContext() {
    EmscriptenWebGLContextAttributes attrs;
    emscripten_webgl_init_context_attributes(&attrs);
    attrs.alpha = false;
    attrs.depth = false;
    // Stencil stays on: the compositor masks layers, and a missing stencil buffer shows up as
    // masks that simply do not apply rather than as a GL error.
    attrs.stencil = true;
    attrs.antialias = false;
    attrs.premultipliedAlpha = true;
    attrs.preserveDrawingBuffer = false;
    attrs.majorVersion = 2; // WebGL2. The engine targets GLES2; ES2 calls are a subset of ES3.
    attrs.minorVersion = 0;
    attrs.powerPreference = EM_WEBGL_POWER_PREFERENCE_HIGH_PERFORMANCE;
    attrs.failIfMajorPerformanceCaveat = false;

    g_gl = emscripten_webgl_create_context(kCanvasTarget, &attrs);
    if (!g_gl) return false;
    emscripten_webgl_make_context_current(g_gl);
    // A tainted or lost context is silent otherwise: every later GL call becomes a no-op and the
    // canvas stays black, which reads like "the engine drew nothing".
    Log(kLogInfo, std::string("web host: GL ") + (const char *)glGetString(GL_VERSION));
    return true;
}

} // namespace

//---------------------------------------------------------------------------
// Entry points for the page.
//
// Everything the page needs is an exported C function rather than a raw `ccall` into C++: the
// signature stays stable, and a mistake in the page surfaces as a failed call instead of as a silent
// no-op.
//---------------------------------------------------------------------------

extern "C" {

/** Creates the GL context and opens `pack` (a path inside the Emscripten filesystem). */
EMSCRIPTEN_KEEPALIVE int artc_web_open(const char *pack, const char *os_id) {
    if (!CreateGlContext()) {
        // Naming the target is not decoration: resolution goes through `querySelector(kCanvasTarget)`,
        // so "no context" here usually means the host could not answer for that selector — which has
        // nothing to do with WebGL being available.
        const std::string why = std::string("could not create a WebGL2 context on ") + kCanvasTarget +
                                " (the host must be able to resolve that selector to its canvas)";
        Log(kLogError, "web host: " + why);
        g_engine.SetLastError(why);
        return 0;
    }
    const std::string os = (os_id && *os_id) ? os_id : "windows";
    if (!g_engine.Open(pack ? pack : "", os)) {
        const std::string why = std::string("could not open pack: ") + (pack ? pack : "(null)");
        Log(kLogError, "web host: " + why);
        g_engine.SetLastError(why);
        return 0;
    }
    return 1;
}

/**
 * One frame. The host calls this from its own tick. This module does not schedule rAF.
 * Returns 1 to keep running, 0 when the engine has asked to exit.
 */
EMSCRIPTEN_KEEPALIVE int artc_web_frame(void) {
    g_engine.Tick();
    return g_engine.ExitRequested() ? 0 : 1;
}

/** Pointer input in **CSS pixels** relative to the canvas; the host letterboxes them to the stage. */
EMSCRIPTEN_KEEPALIVE void artc_web_pointer(float x, float y, int down) {
    g_engine.Pointer(x, y, 0, down != 0);
}

EMSCRIPTEN_KEEPALIVE void artc_web_pointer_move(float x, float y) {
    g_engine.Pointer(x, y, 1, 0);
}

EMSCRIPTEN_KEEPALIVE void artc_web_key(int key, int down) {
    g_engine.Key(key, down != 0);
}

EMSCRIPTEN_KEEPALIVE void artc_web_resize(int css_w, int css_h) {
    g_engine.Resize(css_w, css_h);
}

EMSCRIPTEN_KEEPALIVE int artc_web_started(void) { return g_engine.Started() ? 1 : 0; }
EMSCRIPTEN_KEEPALIVE int artc_web_stage_width(void) { return g_engine.StageWidth(); }
EMSCRIPTEN_KEEPALIVE int artc_web_stage_height(void) { return g_engine.StageHeight(); }
EMSCRIPTEN_KEEPALIVE int artc_web_revision(void) { return (int)g_engine.Revision(); }
EMSCRIPTEN_KEEPALIVE int artc_web_presented(void) { return (int)g_engine.Presented(); }
EMSCRIPTEN_KEEPALIVE int artc_web_exit_requested(void) { return g_engine.ExitRequested() ? 1 : 0; }
EMSCRIPTEN_KEEPALIVE const char *artc_web_last_error(void) { return g_engine.LastError(); }

/**
 * Log sink so page-side code can surface engine output where it likes.
 *
 * `SetLogSink` takes a `std::function<void(int, const std::string &)>`, so the C callback is wrapped
 * rather than bound directly — passing a raw function pointer into a `std::function` by signature
 * alone does not compile, and the wrapper is where the `const char *` becomes a `std::string`.
 */
/** 1 pauses LuaEngine clock+audio; 0 resumes. Tick is a no-op while paused. 0 if not started. */
EMSCRIPTEN_KEEPALIVE int artc_web_pause(int paused) {
    return g_engine.Pause(paused != 0) ? 1 : 0;
}

/** LuaEngine::NowMs. Stays put across a pause. NaN if the engine is not started. */
EMSCRIPTEN_KEEPALIVE double artc_web_now_ms(void) { return g_engine.NowMs(); }

/** Shutdown and destroy the WebGL context. Safe to call twice. */
EMSCRIPTEN_KEEPALIVE void artc_web_dispose(void) {
    g_engine.Dispose();
    if (g_gl) {
        emscripten_webgl_destroy_context(g_gl);
        g_gl = 0;
    }
}

/**
 * SaveSnapshot / LoadSnapshot. 0 unless the engine accepted the slot.
 * ARCV variable bank via onSave/onLoad — not a BOWS scene export.
 * saveDir is the pack parent. A path that escapes it is rejected inside SavePath.
 */
EMSCRIPTEN_KEEPALIVE int artc_web_save(const char *file) { return g_engine.Save(file) ? 1 : 0; }
EMSCRIPTEN_KEEPALIVE int artc_web_load(const char *file) { return g_engine.Load(file) ? 1 : 0; }

EMSCRIPTEN_KEEPALIVE void artc_web_log_sink(void (*sink)(int level, const char *message)) {
    if (!sink) {
        artc::SetLogSink(artc::LogSink{});
        return;
    }
    artc::SetLogSink([sink](int level, const std::string &message) { sink(level, message.c_str()); });
}

//---------------------------------------------------------------------------
// Audio PCM pull.
//
// The engine creates voices (a script's [playbgm] is what makes one), and the
// page pulls PCM out of them to feed WebAudio. Nothing here creates an
// AudioContext, an AudioWorklet, or a thread: the page owns the output, this
// side owns the decode. The bridge lives in `overlay/audio/audio_web.cpp`,
// which is compiled instead of the pinned `src/audio/audio.cpp`.
//
// ABI rules, applied at every entry point below:
//   * an unknown handle, a null buffer, or an engine that is not started is a
//     refusal (0 or -1), never a silent success;
//   * `ReadStereo` writes at most the `frames` asked for -- there is no growth
//     path that could turn a short buffer into a heap overwrite;
//   * the string return is never null for a valid handle.
//---------------------------------------------------------------------------

/** Engine clock, in ms, for the very first pump. Not the page's wall clock. */
EMSCRIPTEN_KEEPALIVE int artc_web_audio_pump(void) {
    if (!g_engine.Started()) return 0;
    artc::AudioWeb::Pump(g_engine.Audio(), g_engine.NowMs(), kAudioPullFrames);
    return 1;
}

/** Voices the engine currently holds. 0 when there is no audio to pull. */
EMSCRIPTEN_KEEPALIVE int artc_web_audio_voice_count(void) {
    if (!g_engine.Started()) return 0;
    return artc::AudioWeb::VoiceCount(g_engine.Audio());
}

/** Handle for voice `index`, or 0 when the index is out of range. */
EMSCRIPTEN_KEEPALIVE double artc_web_audio_voice_handle(int index) {
    if (!g_engine.Started()) return 0;
    return (double)artc::AudioWeb::VoiceHandle(g_engine.Audio(), index);
}

/** Engine key for a handle. Empty string for an unknown handle. */
EMSCRIPTEN_KEEPALIVE const char *artc_web_audio_voice_key(double handle) {
    if (!g_engine.Started()) return "";
    int64_t h = 0;
    if (!AudioHandleOk(handle, &h)) return "";
    const char *key = artc::AudioWeb::VoiceKey(g_engine.Audio(), h);
    return key ? key : "";
}

/** Source sample rate. 0 for an unknown handle. */
EMSCRIPTEN_KEEPALIVE int artc_web_audio_voice_rate(double handle) {
    if (!g_engine.Started()) return 0;
    int64_t h = 0;
    if (!AudioHandleOk(handle, &h)) return 0;
    return artc::AudioWeb::VoiceSampleRate(g_engine.Audio(), h);
}

/**
 * Pull up to `frames` stereo int16 into `heapDest` (2-byte aligned, inside the
 * wasm heap, frames <= kAudioPullFrames). `-1` on a bad argument and on a
 * decoder that broke its contract by returning more frames than it was given;
 * `0` when the handle has no PCM left (EOF, paused, ended, or a zero-frame
 * pull). outGain/outPan, when non-zero, must be 4-aligned 4-byte spans inside
 * the heap.
 */
EMSCRIPTEN_KEEPALIVE int artc_web_audio_read_stereo(double handle, uintptr_t heapDest, int frames,
                                                     intptr_t outGain, intptr_t outPan) {
    if (!g_engine.Started()) return 0;
    // One shared validator rather than a sequence of inline checks: the audio
    // test calls this same function, so a rule that changes here changes the
    // thing under test. `frames` arrives as an int from the cabi boundary; the
    // validator owns the range, the 4096 cap, the null-destination rule and the
    // heap spans.
    int64_t h = 0;
    uint32_t n = 0;
    const uintptr_t heap = static_cast<uintptr_t>(emscripten_get_heap_size());
    if (!AudioValidation::ValidateAudioReadParams(handle, heapDest, static_cast<double>(frames),
                                                  static_cast<uintptr_t>(outGain),
                                                  static_cast<uintptr_t>(outPan), heap, &h, &n)) {
        return -1;
    }
    // A zero-frame pull is a valid no-op: there is nothing to write, so the
    // decoder is not entered and no destination is required.
    if (n == 0) return 0;
    int gain = 0, pan = 0;
    const size_t got = artc::AudioWeb::ReadStereo(
        g_engine.Audio(), h, reinterpret_cast<int16_t *>(heapDest), static_cast<size_t>(n), &gain, &pan);
    // The backend reports a refusal as `kReadError`, the largest `size_t`, and
    // that comes back through this `int` return as -1 -- the same value the
    // argument refusals above use. Anything at or above the requested count is
    // a broken decoder rather than a length.
    if (got > static_cast<size_t>(n)) return -1;
    if (outGain) *reinterpret_cast<int *>(outGain) = gain;
    if (outPan) *reinterpret_cast<int *>(outPan) = pan;
    return static_cast<int>(got);
}

/** 1 when source EOF and the speaker ACK has caught produced. Unknown is 1. */
EMSCRIPTEN_KEEPALIVE int artc_web_audio_ended(double handle) {
    if (!g_engine.Started()) return 1;
    int64_t h = 0;
    if (!AudioHandleOk(handle, &h)) return 1;
    return artc::AudioWeb::Ended(g_engine.Audio(), h) ? 1 : 0;
}

/** Speaker-consumed frames for `key`. 0 if unknown or not yet acknowledged. */
EMSCRIPTEN_KEEPALIVE double artc_web_audio_played_frames(const char *key) {
    if (!g_engine.Started() || !key) return 0;
    return (double)artc::AudioWeb::PlayedFrames(g_engine.Audio(), key);
}

/**
 * Cumulative speaker frames for `handle`. 1 accepted, -1 refused (unknown,
 * non-finite, non-integer, backward, or past produced). Does not decode.
 */
EMSCRIPTEN_KEEPALIVE int artc_web_audio_ack(double handle, double cumulativeFrames) {
    if (!g_engine.Started()) return -1;
    int64_t h = 0;
    uint64_t n = 0;
    if (!AudioHandleOk(handle, &h) || !AudioFramesOk(cumulativeFrames, &n)) return -1;
    return artc::AudioWeb::Acknowledge(g_engine.Audio(), h, n);
}

} // extern "C"
