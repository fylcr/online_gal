#pragma once

// Host event injection for the embedded ONS runtime.
//
// ## Why this exists
//
// OnscripterYuri is an application: `runEventLoop()` blocks in
// `SDL_WaitEvent(&event)` until SDL hands it something. In a browser that wait can
// never be satisfied the way SDL expects, because SDL's web backend registers its
// listeners through Emscripten's DOM helpers and, in a worker, none of them find a
// target — measured directly, with zero registrations on all four possible targets
// (document, canvas, `self`, and the `on<event>` property path). The engine then
// renders and never responds to input.
//
// AetherKiri solves this by patching the engine at configure time to call a
// host-supplied wait and pushing events in from the host. This is the same
// technique, small enough to state in full.
//
// ## What is replaced, and what is not
//
// Only the *wait* changes. SDL keeps its event queue, and the engine keeps calling
// `SDL_PollEvent`, `SDL_PeepEvents` and `SDL_PushEvent` exactly as before — its own
// timer and sound callbacks already push synthetic events, and those paths are
// untouched. The patch therefore cannot desynchronise internal state; it only
// decides when the loop gets control back.
//
// ## Why this header does not include SDL
//
// It is force-included with `-include`, which the compiler processes *before* the
// command line's `-D`/`-s` options take effect. Pulling in `<SDL.h>` at that point
// selects Emscripten's `fakesdl/SDL.h`, which exists only to say:
//
//     #error "To use the emscripten port of SDL use -sUSE_SDL or -sUSE_SDL=2"
//
// So the header stays self-contained: `SDL_Event` is forward-declared and passed
// through as an opaque pointer, and `SDL_WaitEvent` is replaced by a macro. The
// engine's own sources include the real SDL headers themselves, so everything that
// needs the real types still gets them.

// `<emscripten.h>` must be included *outside* the `extern "C"` block below. It
// defines C++ templates (`__em_asm_sig` among them), and a template inside a
// linkage specification is an error: "templates must have C++ linkage".
#include <emscripten.h>

#ifdef __cplusplus
extern "C" {
#endif

// SDL declares `union SDL_Event` and then `typedef union SDL_Event SDL_Event`, so
// the forward declaration has to name the **union**. Writing `struct SDL_Event;`
// introduces a different type, and the clash surfaces much later as
//
//     SDL_events.h:655:9: error: use of 'SDL_Event' with tag type that does not
//     match previous declaration
//
// followed by a failed `sizeof` static assertion. Naming the union and then
// typedef-ing it reproduces SDL's own arrangement without including its headers.
union SDL_Event;
typedef union SDL_Event SDL_Event;

// Every entry point is marked `EMSCRIPTEN_KEEPALIVE`, and the same names are added
// to `-sEXPORTED_FUNCTIONS` in the build overlay.
//
// This is not belt-and-braces. The engine never references these functions by name
// — the macro at the bottom of this file rewrites its `SDL_WaitEvent` call — so
// from the linker's point of view nothing uses them, and it discards them along
// with the rest of the unreferenced code. Without the keepalive and the explicit
// export list the shim silently does not exist in the output, which is exactly the
// failure that took two builds to notice.

// Returns 1 and fills `event` when one is available; 0 when the caller should
// yield to the browser and try again.
EMSCRIPTEN_KEEPALIVE int ons_host_wait_event(SDL_Event *event);

// Queues an event for the engine, from the host. Returns 1 on success.
EMSCRIPTEN_KEEPALIVE int ons_host_push_event(const SDL_Event *event);

// Convenience constructors, so the host does not have to agree with this build on
// the exact SDL_Event layout to deliver the common cases. `type` is an SDL event
// type value as SDL defines it (e.g. 1025 for SDL_MOUSEBUTTONDOWN).
EMSCRIPTEN_KEEPALIVE int ons_host_push_key(int pressed, int scancode, int keycode, int modifiers);
EMSCRIPTEN_KEEPALIVE int ons_host_push_mouse(int type, int x, int y, int button);

// Self-test, used to confirm from the host that the shim is actually compiled in.
// Returns 1 if this build replaced SDL's wait.
EMSCRIPTEN_KEEPALIVE int ons_host_shim_present(void);

// Diagnostic: `SDL_GetMouseState`'s x and y packed as `x | (y << 16)`.
//
// The engine hit-tests buttons through that call, not through the event it is
// handling, so whether a pushed motion event updates it decides whether clicks land.
// Exposing it makes that measurable from the host.
EMSCRIPTEN_KEEPALIVE int ons_host_mouse_state(void);

// Writes up to `max` button rectangles as `(x, y, w, h)` quads and returns the count.
//
// The engine provides this (appended to `ONScripter.cpp` by the build overlay) because
// `ButtonLink::select_rect` is a protected member. It exists to answer a question that
// reasoning could not: where the buttons actually are. Without it, the only observable
// was "did the engine fetch new assets", and the first click on a title screen loads a
// whole scene anyway — so that signal cannot tell a correct hit from an incidental
// advance.
EMSCRIPTEN_KEEPALIVE int ons_host_button_probe(int *out, int max);

#ifdef __cplusplus
}
#endif

// The engine's call site compiles unchanged: `SDL_WaitEvent(&event)` now reaches
// the host. Redefining the name is deliberate — patching call sites individually
// would leave any missed one silently blocking.
#define SDL_WaitEvent(event) ons_host_wait_event(event)

// Reports the focus bits as set, because a worker's window never has them.
//
// `refreshMouseOverButton` does this:
//
// ```cpp
// SDL_GetMouseState( &mx, &my );
// if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)) {
//     mx = screen_device_width;   // parks the cursor past the bottom-right corner
//     my = screen_device_height;
// }
// mx = mx * screen_scale_ratio1;
// my = my * screen_scale_ratio2;
// mouseOverCheck( mx, my );
// ```
//
// There is no real window in a worker, so SDL reports no focus, the branch is taken
// on every call, and the hit test runs against a point outside the screen — so no
// button is ever hovered and clicks do nothing, no matter how correct the
// coordinates are.
//
// Only the two focus bits are forced on; every other flag is passed through.
//
// This is a **function definition**, not a macro. A macro was tried first and cannot
// work: `<SDL2/SDL_video.h>` declares
//
//     extern DECLSPEC Uint32 SDLCALL SDL_GetWindowFlags(SDL_Window * window);
//
// and the force-include makes this header precede that declaration, so the macro
// expands inside it and mangles it — `expected ')'`, whatever the macro's parameter is
// called. Defining the symbol instead leaves the declaration alone and simply
// replaces the implementation at link time.
//
// Reason for the override: `refreshMouseOverButton` does
//
// ```cpp
// if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)) {
//     mx = screen_device_width;   // parks the cursor past the bottom-right corner
//     my = screen_device_height;
// }
// ```
//
// A worker has no real window, so SDL reports no focus, that branch is taken on every
// call, and the hit test runs against a point outside the screen — no button is ever
// hovered and clicks do nothing however correct the coordinates are.
//
// `struct SDL_Window` and `SDL_GetWindowFlags` are declared inside the `extern "C"`
// block above, because SDL's own declaration has C linkage.

