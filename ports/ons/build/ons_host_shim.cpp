// Host event injection for the embedded ONS runtime. See ons_host_shim.h.
//
// This file is where the real SDL headers belong: it is compiled as an ordinary
// translation unit, so `-sUSE_SDL=2` is in effect and `<SDL.h>` resolves to the
// actual ported headers rather than the `fakesdl` stub.
//
// The design is deliberately minimal. SDL's queue stays the single source of
// truth, and this file only provides
//
//   * a non-blocking wait, so `runEventLoop` yields to the browser instead of
//     blocking a thread nothing can wake, and
//   * constructors for the events the host needs to deliver.
//
// Nothing here reimplements queueing: `ons_host_push_event` goes straight to
// `SDL_PushEvent`, so the engine's own `SDL_PollEvent` / `SDL_PeepEvents` calls
// observe exactly what a native SDL backend would have given them.

#include "ons_host_shim.h"

// `SDL2/SDL.h`, not `SDL.h`.
//
// The Emscripten sysroot has `include/fakesdl/SDL.h` and `include/SDL2/SDL.h` as
// siblings, and the stub comes first on the search path. Its entire body is
//
//     #error "To use the emscripten port of SDL use -sUSE_SDL or -sUSE_SDL=2"
//
// which fires even when `-sUSE_SDL=2` *is* in the compile flags — that flag selects
// the port at link time and does not rewrite this include. Naming the port
// directory explicitly is what reaches the real headers. The engine's own sources
// never hit this because they do not include SDL from a force-included header.
#include <SDL2/SDL.h>

// Kept for `emscripten_sleep`; the keepalive macros come from the shim header.
#include <emscripten.h>
#include <cstdio>

extern "C" int ons_host_push_mouse(int type, int x, int y, int button) {
    // A press is made self-contained by queueing a motion event first.
    //
    // This is what AetherKiri's ONS runtime does, and its comment states the reason
    // exactly:
    //
    // > ONScripter resolves a button press through `current_over_button`, which is
    // > refreshed by SDL_MOUSEMOTION rather than by the button event's coordinates.
    // > Godot can legitimately deliver a click without a preceding motion event (for
    // > example when a new menu appears beneath a stationary cursor), so make every
    // > press self-contained.
    //
    // > if (type == 1) { motion.type = SDL_MOUSEMOTION; motion.motion.x = device_x; ...
    // >                 PushRuntimeEvent(motion); }
    //
    // That resolves the apparent contradiction found here: `SDL_GetMouseState` returns
    // `(0,0)` no matter what is pushed or warped, yet `mouseMoveEvent` receives the real
    // coordinates. The hit test therefore only works when a motion event has primed
    // `current_over_button` beforehand, and a bare button event never does.
    //
    // `SDL_WarpMouseInWindow` was tried for this and is wrong for this build: it updates
    // SDL's internal mouse state, which is precisely the value that never becomes valid
    // here, so it cannot decide the hit test.
    if (type == SDL_MOUSEBUTTONDOWN || type == SDL_MOUSEBUTTONUP) {
        SDL_Event motion;
        SDL_zero(motion);
        motion.type = SDL_MOUSEMOTION;
        motion.motion.type = SDL_MOUSEMOTION;
        motion.motion.x = x;
        motion.motion.y = y;
        motion.motion.state = 0;
        ons_host_push_event(&motion);
    }

    SDL_Event event;
    SDL_zero(event);
    event.type = static_cast<Uint32>(type);
    if (type == SDL_MOUSEMOTION) {
        event.motion.type = static_cast<Uint32>(type);
        event.motion.x = x;
        event.motion.y = y;
    } else {
        event.button.type = static_cast<Uint32>(type);
        event.button.state = (type == SDL_MOUSEBUTTONDOWN) ? SDL_PRESSED : SDL_RELEASED;
        event.button.button = static_cast<Uint8>(button);
        event.button.clicks = 1;
        event.button.x = x;
        event.button.y = y;
    }
    return ons_host_push_event(&event);
}

extern "C" int ons_host_shim_present(void) { return 1; }

// `ons_host_button_probe` is provided by the engine side (appended to ONScripter.cpp by
// the build overlay). `ButtonLink::select_rect` is a protected member, so the probe
// cannot live here; the engine publishes it instead.
extern "C" int ons_host_button_probe(int *out, int max);
// `SDL_GetWindowFlags` is deliberately **not** overridden here.
//
// It was tried: the engine's `refreshMouseOverButton` reads
// `SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS`, and a worker's window never
// has that bit, so the cursor gets parked off-screen and no button is ever hovered.
// But SDL defines that symbol in `libSDL2.a`, so a second definition fails at link time
// with `duplicate symbol: SDL_GetWindowFlags`. The gate is patched in the engine's own
// source instead — see the build overlay — which is narrower and keeps the change
// visible where it applies.

/**
 * Diagnostic: reports what `SDL_GetMouseState` returns.
 *
 * The engine hit-tests buttons through that call rather than through the event it is
 * handling, so whether a pushed motion event actually updates SDL's internal mouse
 * position decides whether clicks can land. Reading it back from the host turns that
 * question into a measurement instead of an inference.
 *
 * Returns `x | (y << 16)`; `-1` when the call is unavailable.
 */
extern "C" int ons_host_mouse_state(void) {
    int x = -1;
    int y = -1;
    Uint32 state = SDL_GetMouseState(&x, &y);
    (void)state;
    return (x & 0xffff) | ((y & 0xffff) << 16);
}

extern "C" int ons_host_push_event(const SDL_Event *event) {
    if (!event) return 0;
    SDL_Event copy = *event;
    return SDL_PushEvent(&copy) == 1 ? 1 : 0;
}

// Probe the engine's hit test from inside its own loop.
//
// Nothing scheduled from the worker can do this: `callMain` occupies the thread, and
// `emscripten_sleep` yields the engine rather than the JavaScript event loop, so a
// `setInterval` registered before it fires zero times. This function is called by the
// engine's loop while the game is live, which is the only point where the buttons and the
// pointer state exist.
//
// The result is reported with `emscripten_run_script`, which runs in the page context.
// That avoids every blocked route at once: no return value to ferry, no engine memory to
// read (`getValue` is not exported and the HEAP views are not on `Module`), and no export
// list involved.
int ons_host_probe_summary(void);
extern "C" int ons_host_pointer_sweep(int cols, int rows);
// Length and address of the probe buffer, so the in-engine hit report can read it.
// Defined by the build overlay in ONScripter.cpp; nothing in JavaScript can reach the
// buffer, because `getValue` is not exported and the HEAP views are not on `Module`.
extern "C" int ons_host_pointer_count(void);
extern "C" const int *ons_host_pointer_values(void);

static int ons_wait_calls = 0;

/// Calls per probe. Frequent enough to catch a live frame, rare enough not to flood.
static const int ONS_PROBE_EVERY = 240;

extern "C" int ons_host_wait_event(SDL_Event *event) {
    if (!event) return 0;

    // Confirms from the log that the probe fires **during** the game rather than after it
    // exits. It runs here because this is the only code that executes inside the engine's
    // loop while the game is live: `callMain` otherwise occupies the thread, and
    // `emscripten_sleep` yields the engine rather than the JavaScript event loop, so a
    // `setInterval` registered before `callMain` fires zero times.
    //
    // The readings come from the worker-request path, which posts the `values` array the
    // host decodes. This path deliberately posts no probe data of its own: two differently
    // shaped messages with one decoder is what produced out-of-range numbers earlier.
    if (++ons_wait_calls % ONS_PROBE_EVERY == 0) {
        ons_host_pointer_sweep(12, 8);
        emscripten_run_script("self.postMessage({kind:'probeTick',fromLoop:true});");
    }

    // Prefer whatever is already queued — including the engine's own synthetic
    // timer, BGM and sound events, which arrive through SDL_PushEvent.
    if (SDL_PollEvent(event)) return 1;

    // Nothing pending. Returning 0 would end `runEventLoop`'s while-loop and
    // therefore the game, so the call site yields and retries instead. The yield is
    // also what keeps the page responsive: without it this loop would spin and the
    // browser would never paint. `emscripten_sleep` requires ASYNCIFY, which the
    // web target already enables.
    emscripten_sleep(0);
    return SDL_PollEvent(event) ? 1 : 0;
}

extern "C" int ons_host_push_key(int pressed, int scancode, int keycode, int modifiers) {
    SDL_Event event;
    SDL_zero(event);
    event.type = pressed ? SDL_KEYDOWN : SDL_KEYUP;
    event.key.type = event.type;
    event.key.state = pressed ? SDL_PRESSED : SDL_RELEASED;
    event.key.repeat = 0;
    event.key.keysym.scancode = static_cast<SDL_Scancode>(scancode);
    event.key.keysym.sym = static_cast<SDL_Keycode>(keycode);
    event.key.keysym.mod = static_cast<SDL_Keymod>(modifiers);
    return ons_host_push_event(&event);
}


