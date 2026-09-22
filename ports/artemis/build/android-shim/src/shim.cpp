// The web side of the Android NDK surface the engine's own NativeActivity host expects.
//
// Nothing in here is engine code: `src/jni/native_activity.cpp` and `render/renderer.cpp` compile
// unmodified against these headers, and the driver below plays the part the Android system plays —
// it fabricates an `ANativeActivity`, calls the engine's `ANativeActivity_onCreate`, then delivers a
// window and an input queue exactly the way the platform would.
//
// Why port the Android host at all rather than keep the hand-written web host: the Android host is the
// configuration where this engine is known to render correctly, and it differs from a re-implementation
// in ways that are hard to enumerate by inspection (threading, input pacing, renderer/viewport
// ownership). Reusing it removes the whole class of "the host behaves subtly differently" bugs.
#include <android/log.h>
#include <android/looper.h>
#include <android/input.h>
#include <android/native_window.h>
#include <android/native_activity.h>

#include <condition_variable>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <deque>
#include <dirent.h>
#include <emscripten/html5_webgl.h>
#include <emscripten/threading.h>
#include <EGL/egl.h>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// ---- log ------------------------------------------------------------------
extern "C" int __android_log_print(int prio, const char *tag, const char *fmt, ...) {
    (void)prio;
    std::fprintf(stderr, "[%s] ", tag ? tag : "?");
    va_list ap;
    va_start(ap, fmt);
    std::vfprintf(stderr, fmt, ap);
    va_end(ap);
    std::fputc('\n', stderr);
    return 0;
}

// ---- window ---------------------------------------------------------------
struct ANativeWindow {
    int width = 0;
    int height = 0;
    int refs = 1;
};

extern "C" void ANativeWindow_acquire(ANativeWindow *w) { if (w) ++w->refs; }
extern "C" void ANativeWindow_release(ANativeWindow *w) { if (w && --w->refs <= 0) delete w; }
extern "C" int32_t ANativeWindow_getWidth(ANativeWindow *w) { return w ? w->width : 0; }
extern "C" int32_t ANativeWindow_getHeight(ANativeWindow *w) { return w ? w->height : 0; }

// ---- input events ---------------------------------------------------------
struct AInputEvent {
    int type = 0;                        // AINPUT_EVENT_TYPE_*
    int action = 0;
    int keycode = 0;
    float x = 0, y = 0;
};

extern "C" int32_t AInputEvent_getType(const AInputEvent *e) { return e ? e->type : 0; }
extern "C" int32_t AKeyEvent_getKeyCode(const AInputEvent *e) { return e ? e->keycode : 0; }
extern "C" int32_t AKeyEvent_getAction(const AInputEvent *e) { return e ? e->action : 0; }
extern "C" int32_t AMotionEvent_getAction(const AInputEvent *e) { return e ? e->action : 0; }
extern "C" float AMotionEvent_getX(const AInputEvent *e, size_t) { return e ? e->x : 0.f; }
extern "C" float AMotionEvent_getY(const AInputEvent *e, size_t) { return e ? e->y : 0.f; }
extern "C" size_t AMotionEvent_getPointerCount(const AInputEvent *) { return 1; }

// ---- input queue + looper -------------------------------------------------
struct AInputQueue {
    std::deque<AInputEvent *> pending;
    ALooper *looper = nullptr;
    int ident = 0;
    int (*proc)(int, int, void *) = nullptr;
    void *data = nullptr;
};

struct ALooper {
    std::mutex mutex;
    std::condition_variable cv;
    std::vector<AInputQueue *> queues;
};

namespace {
thread_local ALooper *t_looper = nullptr;
std::mutex g_inject_mutex;
std::vector<AInputQueue *> g_all_queues;   // so injected input can wake every looper
} // namespace

extern "C" ALooper *ALooper_prepare(int) {
    if (!t_looper) t_looper = new ALooper();
    return t_looper;
}
extern "C" ALooper *ALooper_forThread(void) { return t_looper; }

extern "C" void AInputQueue_attachLooper(AInputQueue *q, ALooper *looper, int ident,
                                         int (*proc)(int, int, void *), void *data) {
    if (!q) return;
    q->looper = looper;
    q->ident = ident;
    q->proc = proc;
    q->data = data;
    if (looper) {
        std::lock_guard<std::mutex> lk(looper->mutex);
        looper->queues.push_back(q);
    }
    std::lock_guard<std::mutex> lk(g_inject_mutex);
    g_all_queues.push_back(q);
}

extern "C" void AInputQueue_detachLooper(AInputQueue *q) {
    if (!q) return;
    if (q->looper) {
        std::lock_guard<std::mutex> lk(q->looper->mutex);
        auto &v = q->looper->queues;
        for (size_t i = 0; i < v.size(); ++i)
            if (v[i] == q) { v.erase(v.begin() + (long)i); break; }
    }
    q->looper = nullptr;
    std::lock_guard<std::mutex> lk(g_inject_mutex);
    for (size_t i = 0; i < g_all_queues.size(); ++i)
        if (g_all_queues[i] == q) { g_all_queues.erase(g_all_queues.begin() + (long)i); break; }
}

extern "C" int32_t AInputQueue_getEvent(AInputQueue *q, AInputEvent **out) {
    if (!q || q->pending.empty()) return -1;
    *out = q->pending.front();
    q->pending.pop_front();
    return 0;
}
extern "C" int32_t AInputQueue_preDispatchEvent(AInputQueue *, AInputEvent *) { return 0; }
extern "C" void AInputQueue_finishEvent(AInputQueue *, AInputEvent *e, int) { delete e; }

// The engine's looper thread calls this with -1 ("block until something happens"). Blocking forever is
// not viable in a browser, so a negative timeout is treated as "wait, but wake periodically"; the queue
// is fed from the browser's own thread.
extern "C" int ALooper_pollOnce(int timeoutMillis, int *outFd, int *outEvents, void **outData) {
    ALooper *looper = t_looper;
    if (outFd) *outFd = -1;
    if (outEvents) *outEvents = 0;
    if (outData) *outData = nullptr;
    if (!looper) return 0;

    const int wait_ms = timeoutMillis < 0 ? 40 : timeoutMillis;
    std::unique_lock<std::mutex> lk(looper->mutex);
    looper->cv.wait_for(lk, std::chrono::milliseconds(wait_ms), [&] {
        for (AInputQueue *q : looper->queues)
            if (!q->pending.empty()) return true;
        return false;
    });
    for (AInputQueue *q : looper->queues) {
        if (q->pending.empty()) continue;
        const int ident = q->ident;
        int (*proc)(int, int, void *) = q->proc;
        void *data = q->data;
        lk.unlock();
        if (proc) proc(ident, ALOOPER_EVENT_INPUT, data);
        if (outEvents) *outEvents = ALOOPER_EVENT_INPUT;
        if (outData) *outData = q;
        return ident;
    }
    return 0;
}

// ---- injection, called from the page -------------------------------------
namespace {
void Push(AInputEvent *e) {
    std::lock_guard<std::mutex> lk(g_inject_mutex);
    for (AInputQueue *q : g_all_queues) q->pending.push_back(e);
    for (AInputQueue *q : g_all_queues)
        if (q->looper) q->looper->cv.notify_all();
}
} // namespace

extern "C" void artc_shim_push_motion(int action, float x, float y) {
    auto *e = new AInputEvent();
    e->type = AINPUT_EVENT_TYPE_MOTION;
    e->action = action;
    e->x = x;
    e->y = y;
    Push(e);
}
extern "C" void artc_shim_push_key(int keycode, int action) {
    auto *e = new AInputEvent();
    e->type = AINPUT_EVENT_TYPE_KEY;
    e->keycode = keycode;
    e->action = action;
    Push(e);
}

// ---- the part the Android system plays -----------------------------------
void ANativeActivity_finish(ANativeActivity *) {}

namespace {
ANativeActivity g_activity;
ANativeActivityCallbacks g_callbacks;
ANativeWindow *g_window = nullptr;
AInputQueue g_queue;
} // namespace

// ---- boundary trace helpers -------------------------------------------------
//
// One helper shared by every probe below. The technique is `-Wl,--wrap=<symbol>`: the wrapper runs on
// the **caller's** thread, so an "enter" with no matching "leave" names the exact call the engine
// thread is stuck inside — which is the only way to see the boundary, because the engine itself prints
// nothing between `EngineContext::Open`'s last line and the renderer's "GLES2 ready" line.
//
// It only works on **link-time symbols**, which the engine's own C++ functions are. It does *not* work
// on Emscripten's EGL/GL entry points: those arrive in the wasm as imports from the JS glue
// (`import env.eglGetDisplay`) rather than as symbols the link owns, so wrapping them compiles and
// links cleanly and then logs nothing at all. Their wraps are kept anyway — they cost nothing, and
// their silence is itself evidence that the renderer is never reached.
namespace {
void Trace(const char *what, const char *phase) {
    std::fprintf(stderr, "[artc-egl] %s %s\n", what, phase);
}

// Per-frame calls (bind/teximage/update) would drown the console, so each name reports only its first
// few occurrences — enough to locate the boundary, not enough to hide everything else.
const int kGlTraceLimit = 6;
std::mutex g_gl_trace_mutex;
std::map<std::string, int> g_gl_trace_count;
void TraceCapped(const char *what, const char *phase) {
    int n;
    {
        std::lock_guard<std::mutex> lk(g_gl_trace_mutex);
        n = ++g_gl_trace_count[what];
    }
    if (n <= kGlTraceLimit) Trace(what, phase);
}
} // namespace

// ---- EGL, reimplemented on top of the pthread-aware Emscripten WebGL API ----
//
// WHY THIS EXISTS. `libegl.js` is single-threaded by construction and is the wrong layer for this
// host. `eglCreateContext` there calls `GL.createContext(Browser.getCanvas(), EGL.contextAttributes)`
// *directly*, and `eglMakeCurrent` calls `GL.makeContextCurrent(...)` directly. Both are plain
// module-scope JS variables in whichever realm they run in, and both are `__proxy: 'sync'`, so when
// the engine pthread calls them they execute on the main runtime thread: the context is created and
// bound *there*, and the calling pthread's own `GLctx` stays `undefined`. The raw `gl*` functions the
// engine calls next (`glViewport` at `src/render/renderer.cpp:66`) resolve to the OFFSCREEN_FRAMEBUFFER
// wrappers, which do `if (pthread_getspecific(currentThreadOwnsItsWebGLContext)) ... else
// emscripten_async_run_in_main_runtime_thread(...)`. Nothing ever created that TLS key, because its
// only creator is `emscripten_webgl_create_context` (`system/lib/gl/webgl1.c:75`,
// `pthread_once(&tlsInit, InitWebGLTls)`) — and libegl never calls it. Observed result: the first
// `glViewport` throws `TypeError: Cannot read properties of undefined (reading 'viewport')`.
//
// THE SANCTIONED PATH is `emscripten_webgl_*` (`system/lib/gl/webgl1.c`), which is written for exactly
// this: it creates the TLS keys, and when called from a pthread it proxies the context creation to the
// main runtime thread, then `emscripten_webgl_make_context_current` binds the context on the pthread
// *and* marks it as proxied (`_emscripten_proxied_gl_context_activated_from_main_browser_thread`,
// `src/lib/libhtml5_webgl.js:246`), which is what makes every subsequent `gl*` call dispatch correctly
// to the thread that owns the real WebGL context. `readViaOffscreenBackBuffer` is requested because
// with proxying the frame is only presented by `emscripten_webgl_commit_frame`, which blits the
// offscreen FBO (`libhtml5_webgl.js:277-287`).
//
// So the engine keeps its EGL code unchanged (it is the Android host, ported as-is), and the shim
// supplies an EGL that is a thin adapter over the pthread-aware API. Only the two calls that carry
// real semantics are more than bookkeeping: `eglCreateContext` and `eglMakeCurrent`; `eglSwapBuffers`
// becomes the commit. The rest are the constant handles `libegl.js` hands out anyway (display 62000,
// config 62002, surface 62006), so any non-zero token satisfies the engine's `!= EGL_NO_*` checks.
namespace {

// Token handles. The engine only ever compares them against EGL_NO_DISPLAY / EGL_NO_CONFIG /
// EGL_NO_SURFACE and passes them straight back, so their values are arbitrary but must be non-zero
// and must not collide with EGL's own sentinels.
void *const kPseudoDisplay = (void *)(intptr_t)0xE6400001;
void *const kPseudoConfig = (void *)(intptr_t)0xE6400002;
void *const kPseudoSurface = (void *)(intptr_t)0xE6400003;

const char kCanvasTarget[] = "#canvas";

EMSCRIPTEN_WEBGL_CONTEXT_HANDLE g_webgl_ctx = 0;
bool g_ctx_proxied = false;

} // namespace

// EGL_NOT_INITIALIZED etc. are not needed: Every call below either succeeds or reports failure via its
// return value, which is all `Renderer::Init` inspects.

extern "C" void *__wrap_eglGetDisplay(void *d) {
    (void)d;
    return kPseudoDisplay;
}
extern "C" unsigned __wrap_eglInitialize(void *d, int *major, int *minor) {
    if (d != kPseudoDisplay) return 0;
    if (major) *major = 1;
    if (minor) *minor = 4;
    return 1;
}
extern "C" unsigned __wrap_eglChooseConfig(void *d, const int *attrs, void **cfg, int n, int *num) {
    (void)attrs;
    if (d != kPseudoDisplay) return 0;
    // The engine asks for exactly one config and only checks `num >= 1`; the token is opaque to it.
    if (cfg && n > 0) cfg[0] = kPseudoConfig;
    if (num) *num = 1;
    return 1;
}
extern "C" void *__wrap_eglCreateContext(void *d, void *cfg, void *share, const int *attrs) {
    (void)share;
    if (d != kPseudoDisplay || cfg != kPseudoConfig) return nullptr;

    EmscriptenWebGLContextAttributes wa;
    emscripten_webgl_init_context_attributes(&wa);
    // `Renderer::Init` passes `{EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE}`. The build sets
    // MIN_WEBGL_VERSION=2 / MAX_WEBGL_VERSION=2, so WebGL2 is the only possible answer and the
    // requested value can only ever be 2.
    (void)attrs;
    wa.majorVersion = 2;
    wa.minorVersion = 0;
    wa.alpha = false;
    wa.depth = false;
    wa.stencil = false;
    wa.antialias = false;
    // Proxying is requested unconditionally: this host has no OffscreenCanvas of its own on the
    // engine thread (the page transferred the canvas to the worker, i.e. to the main runtime thread),
    // so a locally-owned context is impossible by construction.
    wa.proxyContextToMainThread = EMSCRIPTEN_WEBGL_CONTEXT_PROXY_ALWAYS;
    // With proxying, frames reach the canvas only through `emscripten_webgl_do_commit_frame`, which
    // blits an offscreen FBO and does nothing at all unless one exists. See the block comment above.
    wa.renderViaOffscreenBackBuffer = true;
    wa.preserveDrawingBuffer = true;

    // Re-init (window swap) calls `Renderer::Shutdown` first; that path reaches `eglDestroyContext`,
    // which is harmless here, so release the old context explicitly rather than leaking it.
    if (g_webgl_ctx) {
        emscripten_webgl_make_context_current(0);
        emscripten_webgl_destroy_context(g_webgl_ctx);
        g_webgl_ctx = 0;
    }

    g_webgl_ctx = emscripten_webgl_create_context(kCanvasTarget, &wa);
    if (!g_webgl_ctx) {
        Trace("eglCreateContext", "failed (emscripten_webgl_create_context returned 0)");
        return nullptr;
    }
    g_ctx_proxied = !emscripten_is_main_runtime_thread();
    Trace("eglCreateContext", g_ctx_proxied ? "created (proxied to main runtime thread)"
                                            : "created (local)");
    return (void *)g_webgl_ctx;
}
extern "C" void *__wrap_eglCreateWindowSurface(void *d, void *cfg, void *win, const int *attrs) {
    (void)win;
    (void)attrs;
    if (d != kPseudoDisplay || cfg != kPseudoConfig) return nullptr;
    return kPseudoSurface;
}
extern "C" unsigned __wrap_eglMakeCurrent(void *d, void *draw, void *read, void *ctx) {
    (void)draw;
    (void)read;
    if (d != kPseudoDisplay) return 0;
    // ctx == EGL_NO_CONTEXT (0) is `Renderer::Shutdown`'s unbind, and is a genuine deactivate.
    const EMSCRIPTEN_WEBGL_CONTEXT_HANDLE handle = ctx ? (EMSCRIPTEN_WEBGL_CONTEXT_HANDLE)ctx : 0;
    if (handle && handle != g_webgl_ctx) return 0;
    const EMSCRIPTEN_RESULT r = emscripten_webgl_make_context_current(handle);
    if (r != EMSCRIPTEN_RESULT_SUCCESS) {
        Trace("eglMakeCurrent", "emscripten_webgl_make_context_current failed");
        return 0;
    }
    Trace("eglMakeCurrent", handle ? "bound" : "unbound");
    return 1;
}
namespace {
// Swap interval requested by the engine. `Renderer::Init` asks for 1, and on a real device
// `eglSwapInterval(1)` succeeds: `Renderer::SwapPaced()` then returns true and
// `native_activity.cpp:598` skips its own `sleep_for(16ms)` because the *swap itself* blocks until
// the next vsync. Reporting the request as rejected (what this shim used to do) leaves the host
// adding a fixed 16 ms sleep on top of a ~6-10 ms frame, which lands the loop at ~32 Hz instead of
// the display's 60 Hz — the judder/frame-pacing half of the reported animation defect.
//
// So the request is honoured, and `__wrap_eglSwapBuffers` becomes the vsync: it blocks until the
// next frame boundary. `std::this_thread::sleep_for` on an Emscripten pthread lowers to
// `emscripten_thread_sleep`, which services the queued-call queue and futex-waits, so the main
// runtime thread keeps draining proxied GL work while the engine thread is parked here.
int g_swap_interval = 0;
std::chrono::steady_clock::time_point g_next_swap{};
constexpr int kSwapHz = 60;
} // namespace

extern "C" unsigned __wrap_eglSwapBuffers(void *d, void *surface) {
    (void)surface;
    if (d != kPseudoDisplay) return 0;
    // `Present()` is the engine's only presentation point, so this is where the frame is committed.
    // `emscripten_webgl_commit_frame` runs the offscreen blit on the main runtime thread.
    if (!g_webgl_ctx) return 0;
    const EMSCRIPTEN_RESULT r = emscripten_webgl_commit_frame();
    if (r != EMSCRIPTEN_RESULT_SUCCESS && r != EMSCRIPTEN_RESULT_INVALID_TARGET) {
        TraceCapped("eglSwapBuffers", "commit_frame failed");
        return 0;
    }
    // The vsync back-pressure the device gets from `eglSwapInterval(1)`.
    //
    // Paced against an absolute schedule laid down one period at a time, not by sleeping a fixed
    // period after each frame: the sleep is only the *remainder* of the current period, so the
    // engine's own work happens inside the period rather than being added to it. Sleeping a whole
    // period after the work instead pins the loop at `work + period` (14 ms of work + 16.7 ms of
    // sleep = 30.6 ms, i.e. 32 Hz), which is precisely the judder this emulation exists to remove.
    //
    // A frame more than one period late — a stall, a tab switch, a slow scene load — restarts the
    // schedule from now rather than trying to catch up, because catching up would present a burst of
    // back-to-back frames and read as a stutter.
    if (g_swap_interval > 0) {
        const auto period = std::chrono::microseconds(1000000 / kSwapHz);
        const auto now = std::chrono::steady_clock::now();
        if (g_next_swap.time_since_epoch().count() == 0 || now > g_next_swap + period) {
            g_next_swap = now + period;
        }
        std::this_thread::sleep_for(g_next_swap - now);
        g_next_swap += period;
    }
    return 1;
}
extern "C" unsigned __wrap_eglSwapInterval(void *d, int interval) {
    if (d != kPseudoDisplay) return 0;
    g_swap_interval = interval;
    Trace("eglSwapInterval", interval > 0 ? "applied (swap paces to 60 Hz)" : "disabled");
    return 1;
}

// ---- GL boundary trace ------------------------------------------------------
//
// The EGL trace above stayed silent, which means the engine thread never reaches `Renderer::Init`.
// The next GL-touching statement in the host is `ctx->compositor().ReleaseGl()` (native_activity.cpp:418),
// which runs *before* the first `Start()` has created the compositor. These wraps bracket the GL entry
// points that `Compositor::ReleaseGl` / `Shaders::ReleaseGl` / `Compositor::Draw` call, so a trace here
// tells us whether the thread reached GL at all and which call it entered without leaving.
//
// `glViewport`/`glClear*` are per-frame in steady state and therefore not wrapped; the one-shot
// resource calls are what the boot path uses.
extern "C" void __real_glDeleteTextures(int, const unsigned *);
extern "C" void __real_glDeleteProgram(unsigned);
extern "C" void __real_glDeleteFramebuffers(int, const unsigned *);
extern "C" void __real_glGenTextures(int, unsigned *);
extern "C" unsigned __real_glCreateProgram(void);
extern "C" unsigned __real_glCreateShader(unsigned);
extern "C" void __real_glBindFramebuffer(unsigned, unsigned);
extern "C" void __real_glBindTexture(unsigned, unsigned);
extern "C" void __real_glTexImage2D(unsigned, int, int, int, int, int, unsigned, unsigned, const void *);

extern "C" void __wrap_glDeleteTextures(int n, const unsigned *t) {
    TraceCapped("glDeleteTextures", "enter");
    __real_glDeleteTextures(n, t);
    TraceCapped("glDeleteTextures", "leave");
}
extern "C" void __wrap_glDeleteProgram(unsigned p) {
    TraceCapped("glDeleteProgram", "enter");
    __real_glDeleteProgram(p);
    TraceCapped("glDeleteProgram", "leave");
}
extern "C" void __wrap_glDeleteFramebuffers(int n, const unsigned *f) {
    TraceCapped("glDeleteFramebuffers", "enter");
    __real_glDeleteFramebuffers(n, f);
    TraceCapped("glDeleteFramebuffers", "leave");
}
extern "C" void __wrap_glGenTextures(int n, unsigned *t) {
    TraceCapped("glGenTextures", "enter");
    __real_glGenTextures(n, t);
    TraceCapped("glGenTextures", "leave");
}
extern "C" unsigned __wrap_glCreateProgram(void) {
    TraceCapped("glCreateProgram", "enter");
    unsigned r = __real_glCreateProgram();
    TraceCapped("glCreateProgram", "leave");
    return r;
}
extern "C" unsigned __wrap_glCreateShader(unsigned s) {
    TraceCapped("glCreateShader", "enter");
    unsigned r = __real_glCreateShader(s);
    TraceCapped("glCreateShader", "leave");
    return r;
}
extern "C" void __wrap_glBindFramebuffer(unsigned a, unsigned b) {
    TraceCapped("glBindFramebuffer", "enter");
    __real_glBindFramebuffer(a, b);
    TraceCapped("glBindFramebuffer", "leave");
}
extern "C" void __wrap_glBindTexture(unsigned a, unsigned b) {
    TraceCapped("glBindTexture", "enter");
    __real_glBindTexture(a, b);
    TraceCapped("glBindTexture", "leave");
}
extern "C" void __wrap_glTexImage2D(unsigned a, int b, int c, int d, int e, int f, unsigned g, unsigned h, const void *i) {
    TraceCapped("glTexImage2D", "enter");
    __real_glTexImage2D(a, b, c, d, e, f, g, h, i);
    TraceCapped("glTexImage2D", "leave");
}

// ---- client-side vertex arrays: `glDrawArrays` must not be async --------------
//
// THE rendering defect, and it is in the GL interface, not in the engine. The engine draws every quad
// from **client-side vertex arrays** — it never creates a VBO (`glBindBuffer` appears nowhere in
// src/render/`) — and the vertex pointer is passed straight through to `glVertexAttribPointer`:
//
//   src/render/compositor.cpp:1739   float verts[16] = {...};            // a *stack local*
//                                    glVertexAttribPointer(prog_.a_pos, 2, GL_FLOAT, GL_FALSE, 16, verts);
//                                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//
// Emscripten emulates client arrays by *deferring*: `_emscripten_glVertexAttribPointer` stores the raw
// pointer in `GL.currentContext.clientBuffers[i].ptr`, and the actual upload happens at draw time, in
// `GL.preDrawHandleClientVertexAttribBindings`, which reads that pointer (glue:
// `webglBufSubData(34962,0,size,cb.ptr)`). So the pointer stays live until the draw executes — and
// which thread reads it matters:
//
//   * `-sOFFSCREEN_FRAMEBUFFER` compiles `glDrawArrays` as ASYNC_GL_FUNCTION
//     (system/lib/gl/webgl1.c:206 — the one draw call with no FULL_ES2 guard, unlike
//     glVertexAttribPointer at :584 and glDrawElements at :209). An async proxy is fire-and-forget:
//     the engine thread returns from `glDrawArrays` immediately and runs on.
//   * The engine therefore keeps computing in the very stack frame that owns `verts`, and reuses that
//     address for the next layer's quad, while the main thread has not drawn anything yet.
//   * When the main thread finally executes the queued draw it reads `cb.ptr` from the shared heap —
//     now belonging to a *later* quad. The geometry is real but from the wrong moment, which is exactly
//     the reported symptom: correct layers, correct rects (`draw[...] drawn=46` is right in the log)
//     and yet everything sheared and flying around on screen.
//
// The fix is to make the draw synchronous, so the read happens while the data is still valid (this is
// what FULL_ES2's VOID_SYNC form does for the two sibling draw calls). `--wrap=glDrawArrays` redirects
// only the *engine's* references; `__real_glDrawArrays` is still the SDK implementation, so nothing
// else in the GL layer changes. Ordering is preserved because async and sync dispatches share one FIFO
// queue, so every queued `glVertexAttribPointer` ahead of the draw has already been applied to
// `cb.ptr` by the time this runs.
// `emscripten_glDrawArrays` is the SDK's raw GL entry point (system/lib/gl/webgl1.c) — the same symbol
// the async macro would call, minus the proxying decision. Both paths below call it directly on
// purpose: the macro's own guard is `pthread_getspecific(currentThreadOwnsItsWebGLContext)`, and on the
// *main runtime thread* that flag is never set to 1 (it is only set on the thread the context was
// created from), so going through `__real_glDrawArrays` there would take the else-branch and
// re-dispatch to itself. Calling the entry point is unambiguous and is exactly "execute the GL call
// inline, on this thread, now".
extern "C" void emscripten_glDrawArrays(unsigned mode, int first, int count);

extern "C" void __wrap_glDrawArrays(unsigned mode, int first, int count) {
    if (emscripten_is_main_runtime_thread()) {
        emscripten_glDrawArrays(mode, first, count);
        return;
    }
    // Dispatch the bare GL call and wait for it. The engine thread stays parked inside this call until
    // the vertices have been read, so `cb.ptr` cannot go stale.
    emscripten_sync_run_in_main_runtime_thread(EM_FUNC_SIG_VIII, &emscripten_glDrawArrays, mode, first, count);
}

// ---- host-loop boundary trace ------------------------------------------------
//
// `--wrap=` also accepts C++ mangled names, and these three brackets bound the host's frame loop
// (src/jni/native_activity.cpp:416-455): the first iteration calls `renderer.Shutdown()` and
// `ctx->compositor().ReleaseGl()` *before* any `Start()`, then `renderer.Init(window)`, and only once
// the renderer is ready does `compositor().Update(...)` run per frame. A silent engine thread can
// therefore be located exactly: no `Renderer::Init enter` means the loop never got past the window
// swap, and `Init enter` without `leave` means it is blocked inside EGL.
//
// The `asm()` labels are the C-identifier aliases the wrappers need; the mangled spellings come from
// `llvm-nm` on the object files.
extern "C" bool real_renderer_init(void *, void *) __asm__("__real__ZN4artc8Renderer4InitEPv");
extern "C" bool wrap_renderer_init(void *, void *) __asm__("__wrap__ZN4artc8Renderer4InitEPv");
extern "C" void real_compositor_release_gl(void *) __asm__("__real__ZN4artc10Compositor9ReleaseGlEv");
extern "C" void wrap_compositor_release_gl(void *) __asm__("__wrap__ZN4artc10Compositor9ReleaseGlEv");
extern "C" void real_compositor_draw(void *) __asm__("__real__ZN4artc10Compositor4DrawEv");
extern "C" void wrap_compositor_draw(void *) __asm__("__wrap__ZN4artc10Compositor4DrawEv");

extern "C" bool wrap_renderer_init(void *self, void *window) {
    Trace("Renderer::Init", "enter");
    bool r = real_renderer_init(self, window);
    Trace("Renderer::Init", "leave");
    return r;
}
extern "C" void wrap_compositor_release_gl(void *self) {
    // CONFIRMED ENGINE BUG, guarded here rather than patched in the engine: the host's frame loop
    // calls `ctx->compositor().ReleaseGl()` (src/jni/native_activity.cpp:418) on its very first
    // iteration, *before* `EngineContext::Start()` has had a chance to create the compositor.
    // `EngineContext::compositor()` is `return *compositor_;` on a `std::unique_ptr` that is still
    // null there, so `this` arrives as 0 and the callee reads a `Compositor` laid over address 0 —
    // inside Emscripten's linear memory that is readable zeroed data, so it does *not* trap; it walks
    // whatever garbage container headers happen to live in the first page, and the engine thread
    // never returns (observed: an "enter" with no "leave", then total silence from the thread).
    //
    // The intent of that line is "the previous GL context is gone, so its textures die with it".
    // When there is no compositor there is nothing to release, so treating it as a no-op is exactly
    // the intended semantics — and it is what the Android build effectively gets by luck of a
    // different memory layout. Nothing else is altered: a non-null `this` still calls the real thing.
    if (!self) {
        Trace("Compositor::ReleaseGl", "skipped (compositor not created yet)");
        return;
    }
    Trace("Compositor::ReleaseGl", "enter");
    real_compositor_release_gl(self);
    Trace("Compositor::ReleaseGl", "leave");
}
extern "C" void wrap_compositor_draw(void *self) {
    // Per-frame once the engine is running: capped so the console stays readable.
    TraceCapped("Compositor::Draw", "enter");
    real_compositor_draw(self);
    TraceCapped("Compositor::Draw", "leave");
}

// ---- virtual pack directory ------------------------------------------------
//
// The Android host discovers its pack chain the platform way: `ListPacks(dir)` opens the app data
// directory and keeps every `*.pfs` entry (src/jni/native_activity.cpp:219). On web there is no such
// directory, and the listing happens on the **engine's own pthread**, where Emscripten's MEMFS is a
// separate, empty instance — so populating a filesystem from the page cannot reach it.
//
// Rather than edit the engine, the three directory calls are wrapped at link time
// (`-Wl,--wrap=opendir,readdir,closedir`). The registry below lives in the wasm linear memory, which
// *is* shared across threads, so the page fills it before boot and the engine thread reads it.
//
// Only the one mount point is virtual; every other path falls through to the real libc call, so this
// cannot change how anything else on the filesystem behaves.
namespace {
std::mutex g_pack_mutex;
std::vector<std::string> g_pack_names;   // basenames, e.g. "root.pfs"
const char *const kVirtualDir = "/game";

bool IsVirtualDir(const char *path) { return path && std::strcmp(path, kVirtualDir) == 0; }

// A DIR* the engine only ever hands back to readdir/closedir, so its contents are ours to define.
// Held in a registry rather than recognised by a magic field: `DIR` is opaque, and a real libc DIR
// must never be reinterpreted as one of these.
struct FakeDir {
    size_t next;
    struct dirent entry;
};
std::vector<FakeDir *> g_fake_dirs;

// Caller must hold g_pack_mutex.
FakeDir *FindFakeDirLocked(DIR *dir) {
    for (FakeDir *d : g_fake_dirs) {
        if (reinterpret_cast<DIR *>(d) == dir) return d;
    }
    return nullptr;
}
} // namespace

extern "C" DIR *__real_opendir(const char *);
extern "C" struct dirent *__real_readdir(DIR *);
extern "C" int __real_closedir(DIR *);

/** Registers one pack basename. Called from the page, before `artc_android_boot`. */
extern "C" void artc_shim_add_pack(const char *name) {
    if (!name || !*name) return;
    std::lock_guard<std::mutex> lk(g_pack_mutex);
    g_pack_names.emplace_back(name);
}

extern "C" DIR *__wrap_opendir(const char *path) {
    if (!IsVirtualDir(path)) return __real_opendir(path);
    auto *d = new FakeDir();
    std::memset(d, 0, sizeof(*d));
    std::lock_guard<std::mutex> lk(g_pack_mutex);
    g_fake_dirs.push_back(d);
    return reinterpret_cast<DIR *>(d);
}

extern "C" struct dirent *__wrap_readdir(DIR *dir) {
    std::lock_guard<std::mutex> lk(g_pack_mutex);
    FakeDir *d = FindFakeDirLocked(dir);
    if (!d) return __real_readdir(dir);
    if (d->next >= g_pack_names.size()) return nullptr;
    std::memset(&d->entry, 0, sizeof(d->entry));
    std::snprintf(d->entry.d_name, sizeof(d->entry.d_name), "%s", g_pack_names[d->next].c_str());
    d->entry.d_type = DT_REG;
    d->next++;
    return &d->entry;
}

extern "C" int __wrap_closedir(DIR *dir) {
    std::lock_guard<std::mutex> lk(g_pack_mutex);
    FakeDir *d = FindFakeDirLocked(dir);
    if (!d) return __real_closedir(dir);
    for (auto it = g_fake_dirs.begin(); it != g_fake_dirs.end(); ++it) {
        if (*it == d) { g_fake_dirs.erase(it); break; }
    }
    delete d;
    return 0;
}

// Defined by the engine (src/jni/native_activity.cpp).
extern "C" void ANativeActivity_onCreate(ANativeActivity *, void *, size_t);

/** Boots the engine's own Android host. Called once from the page. */
extern "C" int artc_android_boot(int width, int height) {
    g_window = new ANativeWindow();
    g_window->width = width;
    g_window->height = height;

    std::memset(&g_activity, 0, sizeof(g_activity));
    std::memset(&g_callbacks, 0, sizeof(g_callbacks));
    g_activity.callbacks = &g_callbacks;
    // A null `clazz` is deliberate: the engine's only JNI use is guarded by `if (activity->clazz)`.
    g_activity.clazz = nullptr;
    // The Android host resolves its pack by LISTING a directory (`ListPacks` → opendir/readdir + a
    // `.pfs` suffix test), because on Android these two fields are directories, not files. `/game` is
    // that directory here, served from the in-memory registry the page fills before boot — see the
    // `__wrap_opendir` block above for why it cannot be a real filesystem.
    g_activity.internalDataPath = "/game";
    g_activity.externalDataPath = "/game";
    g_activity.sdkVersion = 30;
    g_activity.instance = &g_activity;

    ANativeActivity_onCreate(&g_activity, nullptr, 0);

    // What the platform does next, in order.
    if (g_callbacks.onNativeWindowCreated)
        g_callbacks.onNativeWindowCreated(&g_activity, g_window);
    if (g_callbacks.onInputQueueCreated)
        g_callbacks.onInputQueueCreated(&g_activity, &g_queue);
    if (g_callbacks.onStart) g_callbacks.onStart(&g_activity);
    if (g_callbacks.onResume) g_callbacks.onResume(&g_activity);
    return 1;
}
