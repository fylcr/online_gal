#pragma once
#include <stdint.h>
#include <EGL/egl.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct ANativeWindow ANativeWindow;
void ANativeWindow_acquire(ANativeWindow *window);
void ANativeWindow_release(ANativeWindow *window);
int32_t ANativeWindow_getWidth(ANativeWindow *window);
int32_t ANativeWindow_getHeight(ANativeWindow *window);
#ifdef __cplusplus
}
#endif

/* Emscripten vs Android disagree only about the *type* of the native window: Emscripten declares
 * `EGLNativeWindowType` as `int`, Android as `ANativeWindow*`. Emscripten's implementation ignores the
 * argument completely — `eglCreateWindowSurface` validates the display and config and returns its one
 * magic "default surface" id — so the bridge is a cast, and it is safe because the value is never read.
 *
 * The macro is defined **after** the real function so the body below still calls the original. */
static inline EGLSurface artcEglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
                                                    const ANativeWindow *win,
                                                    const EGLint *attrib_list) {
    return eglCreateWindowSurface(dpy, config, (EGLNativeWindowType)(intptr_t)win, attrib_list);
}
#define eglCreateWindowSurface artcEglCreateWindowSurface
