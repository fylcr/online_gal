# ONS in a worker: byte-granular loading achieved

> **How to read this document.** It is a chronological record, so its middle chapters hold
> diagnoses that later work **superseded**. Where a claim below is contradicted, trust the later
> section, not the earlier one — and trust `docs/项目状态与恢复指南.md` above both.
>
> | Chapter | Status |
> | --- | --- |
> | `## How it works`, `## The fix that mattered`, `## What the shims cost` | current |
> | `## Input: why dispatching reaches nobody` / `## Input: SDL registers nothing at all` | **superseded diagnosis.** The conclusion "no input handler is registered anywhere" is true of the DOM route and is *why* the source shim exists; it is not the end of the story. Input is delivered through `ons_host_shim.{h,cpp}` — see the later `WEB`-build coordinate chapter and `docs/ons-source-build.md`. |
> | `## Scaling and mouse mapping`, `## Input: the WEB build's coordinate contract` | current for the diagnosis; each "still unresolved" note is the honest state (below) |
> | `## Display: filling the viewport`, `## Audio: real sound` | current |
> | `## Honest limitations` | **corrected in place** — it had gone stale against the two chapters above it |
>
> The three things that are still genuinely open, in one place (see `docs/项目状态与恢复指南.md`):
> **input** is shim-injected and the engine reacts, but "the click activates the intended control" is
> not proven; **saves** are session-only; the **audio latency counter** reads high.

## Result

The engine runs in a worker and reads its archives a window at a time. Same game,
same display, same audio — with the transfer down from ~3 GB to 16 MB:

```
## worker: 6 eager files, 2 lazy files (threshold 16 MB)
ONScripter Yuri v0.7.7beta, (Jh0.8.0,Ons20181218,NSC2.96)
Display: 793 x 446 (32 bpp)
Audio: 48000 Hz 16 bit stereo
engine exited: 1221 range reads, 16.0 MB fetched
## worker: lazy /onsyuri/onscript/arc.nsa served 0.56 MB in 636 range reads
## worker: lazy /onsyuri/onscript/arc1.nsa served 0.49 MB in 580 range reads
```

The server's transfer report agrees:

| path | before | after |
| --- | --- | --- |
| `/game/arc.nsa` (1866 MB file) | 1,866,409,698 | **749,268** |
| `/game/arc1.nsa` (1207 MB file) | 1,207,107,286 | **513,206** |
| `/game/default.ttf` | 9,753,388 | 9,753,388 |
| `/game/1.txt` | 4,237,099 | 4,237,099 |
| `/game/2.txt` | 1,675,254 | 1,675,254 |
| `/game/0.txt` | 12,225 | 12,225 |

**1780 MB → 0.71 MB, and 1151 MB → 0.49 MB**, against a 2.88 GB game that now
starts for 16 MB. The two independent counters — the engine's own and the
server's per-path tally — agree, which is why the numbers are trustworthy.

## How it works

Three pieces, each doing one job:

1. **The engine runs in a worker.** Mandatory, not a preference: byte-granular
   reads need either `Atomics.wait` or synchronous XHR, and the main thread
   forbids both (see `docs/sync-bridge-main-thread-constraint.md`).
2. **Small files go through `fetch_file`**, the host hook the build's own embedded
   library calls when it opens a path. Scripts and the font are read
   start-to-finish at start-up, so windowing them would add requests and save
   nothing.
3. **Large files get a lazy node with a ranged read.** This is where the win is,
   and it needed one specific fix.

## The fix that mattered

`FS.createLazyFile` gets the *timing* right but not the *amount*. Its loader issues
a plain `XMLHttpRequest` with **no `Range` header**, so the server answers `200`
with the entire body and the plugin copies all of it into the heap. The plugin's
own error text describes the design:

> Lazy loading should have been performed (contents set) in createLazyFile, but it
> was not. Lazy loading only works in web workers.

For a 1.8 GB archive that means the whole archive arrives — which is exactly what
the transfer report showed (1.87 GB for a file of 1.866 GB).

Emscripten dispatches file operations through `stream_ops`, so installing a `read`
that fetches only the requested window is sufficient:

```js
const node = FS.createLazyFile(directory, name, url, true, false);
node.stream_ops = {
  llseek(stream, offset, whence) { /* whence 0/1/2 */ },
  read(stream, buffer, offset, length, position) {
    const at = position ?? stream.position;
    const want = Math.min(length, Math.max(0, size - at));
    const bytes = rangeRequest(url, at, want);   // sends `Range: bytes=at-...`
    buffer.set(bytes.subarray(0, want), offset);
    return want;
  },
  write: () => -5,          // read-only
  mmap: undefined,          // force the engine through `read`
};
```

`llseek` has to be supplied as well: the engine seeks before it reads, and without
it the stock position tracking does not match this node. Clearing `mmap` is what
guarantees reads take the ranged path rather than a mapped fast path.

## What the shims cost, and why each was unavoidable

The engine treats most initialisation failures as unrecoverable, so every entry
below was a hard stop rather than a degradation:

| missing piece | symptom | fix |
| --- | --- | --- |
| `screen` | `_emscripten_get_screen_size` reads `screen.width` with no guard | stub with the real display size, passed from the page |
| `document` | `SDL_SetCursor` reaches for an element | minimal document; **assignment is not enough** — Chrome workers already expose a real, empty `document`, so the value must be forced |
| `getElementById('canvas')` | `Cannot set properties of undefined (setting 'cursor')` | return a canvas-like element; returning `null` is what caused the throw |
| `Module.canvas.style` | glue 773728 / 773811 set `Module["canvas"].style["cursor"]` | an `OffscreenCanvas` has no `.style`; add one before handing it to the module |
| `AudioContext` | `SDL_Init` fails with "No audio context available" and the engine aborts **before reading any archive** | a silent context |
| host UI functions | `ReferenceError: scale_full is not defined` | the set the shipped page defines: `scale_full`, `playVideo`, `flush_save`, the `toggle_*` handlers, and the `g_*` globals |

The `AudioContext` entry is worth noting: it made the archives *absent* from an
earlier transfer report, which looked like laziness working when it was actually
the engine aborting early. A transfer report shows what happened, not why.

## Input: why dispatching reaches nobody

The chain up to the engine is complete and verified:

- `host.mjs` converts DOM events and posts them, with canvas-pixel coordinates
- the worker rebuilds them as real `Event` objects and dispatches them to the
  document stub and the canvas stub
- delivery reports success: `## worker: input mousedown delivered to 2 target(s)`

It does not reach the game. Instrumenting every registration path the engine could
use gives:

```
## worker: listener registrations {"document":0,"canvas":0,"window":0}
## worker: on* handlers assigned {}
```

and clicking produces no new traffic:

```
bytesBefore: 21772639
bytesAfter:  21772639
gained:      0
```

So **no input handler is registered anywhere the engine could reach** — neither via
`addEventListener` on the stubs nor via `on<event>` property assignment.

### What this rules out

- **not a coordinate problem.** A listener that was never registered cannot care
  what the coordinates are, so the canvas-pixel conversion in `host.mjs` is not
  implicated.
- **not the missing event constructors.** `MouseEvent` and `KeyboardEvent` really
  are absent in a worker, and events are synthesised from `Event` for that reason —
  but the failure happens before any event is consumed.

### What the glue says about targets

```js
specialHTMLTargets = [0, globalThis.document ?? 0, globalThis.window ?? 0]
findEventTarget = target => specialHTMLTargets[target] || globalThis.document?.querySelector(target)
```

Two consequences, and they are the reason a wrapper saw nothing:

1. The array is built **once, when the glue loads**, from `globalThis.document`.
   It captured whatever `document` was at that moment, and it is read-only
   afterwards — so replacing `self.document` later cannot change where Emscripten
   resolves target `1`.
2. Target identifiers are resolved by **index**, and integers above the array's
   length fall through to `document.querySelector(target)`. A numeric id beyond
   `2` therefore selects nothing, silently.

The stub is installed before `importScripts`, so `globalThis.document` was already
the stub when the array was built — which points the remaining suspicion at SDL not
calling these registration helpers at all for this build, rather than at the target
resolution. That is the next thing to establish, and it is a single measurement:
count calls to `_emscripten_set_mousedown_callback_on_thread` from inside the
module.

### Diagnostic to keep

`countListeners` and `captureHandler` in `ons-worker.js`, logged as
`listener registrations` and `on* handlers assigned`. Those two lines answer the
registration question in one run and should stay in place. Note that `JSEvents`
itself is **not** reachable: it is declared inside the module factory closure, not
on the worker global, so it cannot be inspected from outside.

## Input: SDL registers nothing at all

The forwarding chain is complete — the host posts DOM events and the worker
rebuilds them as real `Event` objects — but every possible landing site reports
zero registrations:

```
## worker: listener registrations {"document":0,"canvas":0,"window":0,"self":0}
## worker: last listener null
## worker: on* handlers assigned {}
```

Four targets were instrumented: the document stub, the canvas stub, the worker
global (where `window` resolves in a worker, and thus `specialHTMLTargets[2]`),
and the `on<event>` property path. Nothing was ever registered, and clicking
produces no new traffic (`gained: 0`).

### What this rules out

- **not a coordinate problem** — a listener that was never registered cannot care
  what the coordinates are;
- **not the missing event constructors** — `MouseEvent`/`KeyboardEvent` really are
  absent in a worker, and events are synthesised from `Event` for that reason, but
  the failure happens before anything consumes an event;
- **not a wrong-target problem** — the target would have to be one of these four,
  and all four were counted.

### What the glue says

```js
specialHTMLTargets = [0, globalThis.document ?? 0, globalThis.window ?? 0]
findEventTarget = target => specialHTMLTargets[target] || globalThis.document?.querySelector(target)
```

The array is built **once, when the glue loads**, and is read-only afterwards, so
replacing `self.document` later cannot change where target `1` resolves. Integers
above the array's length fall through to `document.querySelector(target)`, where a
numeric id selects nothing and the failure is silent. Even so, the stub is
installed before `importScripts`, so the array should hold the stub — which leaves
SDL not calling the registration helpers at all.

### Why the prebuilt binary has no API to inject through

Emscripten exports its runtime symbols minified, so the build's 58 exports are
`he`, `ie`, `je`, … with no readable event surface:

```
total exports: 58
  memory   he
  function ie
  function je
  ...
```

There is therefore no exported `SDL_PushEvent`-style entry to call. This is exactly
the gap AetherKiri's build closes differently: it **patches the engine's source** at
configure time so input arrives through a host-supplied function rather than
through SDL's DOM registration (see the comparison below). A prebuilt binary cannot
be patched that way, which is why this route is blocked and the next step is to
find the internal function rather than to keep looking for a listener.

> **This is where the story was left at the time, and it is now closed.** Building
> OnscripterYuri **from source** gives back the ability to patch — which is what
> `ports/ons/build/ons_host_shim.{h,cpp}` does, replacing `SDL_WaitEvent` with
> `ons_host_wait_event` and exporting `ons_host_push_key` / `ons_host_push_mouse` /
> `ons_host_push_event`. So the conclusion above holds only for the **prebuilt**
> binary route; the source route (and therefore the engine this project ships) does
> deliver input. See `docs/ons-source-build.md` and the later `WEB`-build coordinate
> chapter in this file. What remains unproven is narrower: that a click activates the
> *intended control*.

### Diagnostic to keep

`countListeners` (all four targets) and `captureHandler` in `ons-worker.js`, logged
as `listener registrations`, `last listener` and `on* handlers assigned`. Note that
`JSEvents` is **not** reachable — it is declared inside the module factory closure,
not on the worker global — so it cannot be inspected from outside.

## How AetherKiri does ONS, and why it is not a simulation

This is worth stating precisely, because it explains why its input path works and
the prebuilt one does not.

`bridge/onscripter_runtime/CMakeLists.txt` compiles the **real engine**:

```cmake
set(ONSCRIPTER_YURI_SOURCE_DIR "${CMAKE_SOURCE_DIR}/packages/OnscripterYuri/src/onsyuri")
if(NOT EXISTS "${ONSCRIPTER_YURI_SOURCE_DIR}/ONScripter.cpp")
    message(FATAL_ERROR "OnscripterYuri submodule is unavailable...")
```

— `ONScripter.cpp`, `ScriptHandler.cpp`, `NsaReader.cpp`, `SarReader.cpp`,
`DirectReader.cpp`, `LuaHandler.cpp`, and the rest, linked against SDL2 and its
image/mixer/ttf siblings. There is no ONS interpreter of its own; the "ONS
support" is the upstream C++ engine plus a host adapter.

That adapter is what matters. `src/onscripter_host_shim.h` declares host functions
and redefines engine calls to reach them:

```c
extern "C" int aetherkiri_onscripter_wait_event(SDL_Event *event);
extern "C" int aetherkiri_onscripter_poll_host_key_release(SDL_Event *event);
extern "C" void aetherkiri_onscripter_publish_frame(SDL_Surface *surface, const SDL_Rect *dirty_rect);
#define exit(code) aetherkiri_onscripter_host_exit((code), __FILE__, __LINE__)
#define SDL_FreeSurface aetherkiri_onscripter_free_surface
```

and `cmake/PrepareOnscripterYuriSources.cmake` rewrites `ONScripter.cpp` and the
event and sound sources in place, **failing the configure step with `FATAL_ERROR`
if an anchor string has moved** — so the patches cannot rot silently.

The resulting interface is frame-oriented rather than window-oriented
(`include/onscripter_runtime.h`):

```cpp
bool open_game(const std::string &game_root_path);
bool tick();                                    // host drives the loop
bool send_pointer_event(int type, int pointer_id, double x, double y, ...);
bool send_key_event(bool pressed, int key_code, int modifiers, int unicode_codepoint);
bool read_frame(Frame &frame);                  // RGBA out
```

**Input is injected, not registered.** `send_key_event`/`send_pointer_event` hand
events to the host-supplied wait function, so the engine never needs a DOM listener
— which is precisely why AetherKiri's approach is unaffected by the problem the
prebuilt build has.

The same table applies to our own work: we run the same engine, but as the
**upstream prebuilt WASM**, whose source we cannot patch. Either the internal event
function is reached some other way, or the build has to be produced from source
with the same overlay AetherKiri uses.

## Scaling and mouse mapping

Two bugs were reported together — the scaling was wrong and mouse input landed in
the wrong place — and they shared a root cause, then the layout fix exposed a third.

### Root cause: SDL never resizes `Module.canvas`

Measured directly: the transferred `OffscreenCanvas` stays at the HTML default of
`300x150` for the whole session while the engine renders `793x446`. The picture was
therefore upscaled from a 300-pixel buffer, and the element's CSS box and the
drawing's intrinsic size disagreed, which broke the mapping.

SDL does not resize it because the engine sizes the canvas **itself**, by calling a
function the page must provide:

```cpp
// ONScripter.cpp:295
let canvas = document.getElementById("canvas");
if(scale_full && canvas) scale_full(canvas, self.g_screen_width/self.g_screen_height);
```

`scale_full` was stubbed as a no-op — it had to exist or the engine threw
`ReferenceError`, and doing nothing looked harmless. It is the engine's canvas-sizing
hook.

### The three fixes

1. **`scale_full` reports what the engine passes it** rather than swallowing it
   (`canvas currently 1189x669, screen 793x446`).
2. **The engine's resolution is read from its own banner.** `Display: 793 x 446
   (32 bpp)` is the only place the render resolution appears on this side, and the
   worker sets the canvas to that many pixels before the engine commits.
3. **The canvas element's CSS box is set to the game's aspect ratio**, so the box
   *is* the drawing rectangle.

### Why the third fix was needed

The layout went through three wrong versions, all failing the same way — the
element's box and the drawing disagreed, so something had to reconcile them:

| version | box | game | result |
| --- | --- | --- | --- |
| `width/height: 100%` + `object-fit: contain` | 1036×652 (1.589) | 793×446 (1.778) | game letterboxed *inside* a box that was too tall — the off-centre picture reported |
| `max-width: 100%` | resolved against the 300×150 HTML default | — | display collapsed to 300 px wide |
| host sets `width`/`height` to the drawn size | 793×446 (1.778) | 793×446 (1.778) | one rectangle, flex-centred, no reconciliation needed |

`object-fit`, `position: absolute` and the `translate(-50%, -50%)` centring were all
removed: with the box matching the aspect ratio they were solving a problem that no
longer exists, and each was itself a source of misalignment.

### Verified

```
canvasRect:   793x446 at (122,103)
canvasAspect: 1.7780  ==  engineAspect 1.7780
offset from box centre: { x: 0, y: 0 }
quarter click -> (198,112)   expected (198,112)
```

The element is centred in its container, its aspect ratio matches the engine's
exactly, and the mouse mapping is exact at the probe points.

### Input: coordinates exact, button correspondence still wrong

After the canvas was matched to the engine's surface, the delivered coordinates were
verified pixel-exact:

```
canvasPixel:  1189x669      elementRect: 1189x669
click topLeft     -> arrived at (2,2)
click bottomRight -> arrived at (1187,667)
```

The canvas **is** the engine's device space, so no conversion is needed and the
host's mapping is correct. That settles two earlier suspicions: it is not a scaling
error and not a letterbox offset.

Three faults were found and fixed on the way, and untangling them took several rounds
because each masked the next:

1. **The canvas stayed at the HTML default of 300x150** while the engine drew
   793x446, because `scale_full` — the function the engine calls to size the canvas —
   was stubbed as a no-op. It has to exist or the engine throws `ReferenceError`, and
   doing nothing looked harmless. It is the engine's canvas-sizing hook.
2. **The canvas was then forced to the banner's `Display: 793x446`**, which is the
   *display* size, not the rendering surface. The surface is 1189x669 and SDL scales
   it into whatever the canvas is, so a smaller canvas **clips** rather than scales:
   the visible region was magnified by exactly `1036/793 = 1.31`.
3. **The engine hit-tests through `SDL_GetMouseState`**, not through the event being
   handled:

   ```cpp
   void ONScripter::refreshMouseOverButton() {
       SDL_GetMouseState( &mx, &my );
       mx = mx * screen_scale_ratio1;
       my = my * screen_scale_ratio2;
       mouseOverCheck( mx, my );
   }
   ```

   SDL updates that position while *processing* a motion event, so a pushed button
   event left it reporting the origin. The shim now pushes a motion event before every
   button event — what SDL's own web backend does, for the same reason.

**Still wrong:** the engine reacts to clicks (a click pulls new assets), but the
**button correspondence is inverted** — clicking empty space produced the reaction and
clicking the `Start` control did not.

The root cause of that was then found, and it is not the coordinates:

```cpp
// ONScripter.cpp:1197
void ONScripter::refreshMouseOverButton() {
    int mx, my;
    SDL_GetMouseState( &mx, &my );
    if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)) {
        mx = screen_device_width;    // parks the cursor past the bottom-right corner
        my = screen_device_height;
    }
    mx = mx * screen_scale_ratio1;
    my = my * screen_scale_ratio2;
    mouseOverCheck( mx, my );
}
```

Two independent blockers, both measured:

1. **The window never has mouse focus in a worker**, so the branch is taken on every
   call and the hit test runs against a point outside the screen. No button can be
   hovered however correct the coordinates are.
2. **`SDL_GetMouseState` always returns `(0,0)`.** Probed directly through an exported
   `ons_host_mouse_state()`: `(0,0)` before a motion event and `(0,0)` after. Pushing a
   motion event does not update SDL's internal position, and neither does
   `SDL_WarpMouseInWindow` — which the engine itself uses in `warpMouse`, so the
   mechanism it relies on is not functioning in this build.

### Attempts that did not work, and why

- **Pushing a motion event before each button event.** SDL does not update its mouse
  state from a pushed event; the probe stayed at `(0,0)`.
- **`SDL_WarpMouseInWindow` before each button event.** The function the engine uses
  for the same purpose; also had no effect on the state.
- **Overriding `SDL_GetWindowFlags` from the shim to force the focus bits.** SDL defines
  that symbol in `libSDL2.a`, so a second definition fails at link time with
  `duplicate symbol: SDL_GetWindowFlags`. A macro cannot be used either: the
  force-include puts this header ahead of `<SDL2/SDL_video.h>`, so the macro expands
  inside SDL's own declaration and mangles it.
- **Patching the engine's focus gate in its own source.** This *succeeded* — the build
  overlay rewrites the branch under `#if defined(WEB)`, verified by the build reporting
  `## patched ONScripter.cpp mouse-focus gate for WEB`. It removed blocker 1, but
  blocker 2 remains, so the verdict is unchanged.

### Next step

Blocker 2 is the one to solve, since the engine's hit test depends on it entirely.

**The likely mechanism, found but not confirmed:** SDL's `SDL_WarpMouseInWindow` bails
out when no window holds mouse focus. In SDL's `SDL_mouse.c` it forwards to
`SDL_SendMouseMotion(SDL_GetMouseFocus(), ...)`, and `SDL_GetMouseFocus()` is set from
DOM mouse events on the canvas — which a worker does not have. So the warp does
nothing, the state stays `(0,0)`, and patching the engine's focus gate cannot help
because the position it falls back to is still zero.

### The verification tool: built up, not finished

Reasoning about where a click lands has been guesswork, because the only observable was
"did the engine fetch new assets" — and the first click on a title screen legitimately
loads a whole scene, so **that signal cannot distinguish a correct hit from an
incidental advance**. The conclusion labelled "button correspondence inverted" earlier in
this document therefore rests on a weak proxy and should not be trusted as stated.

The ground truth is the engine's own hit test, so a probe was started for it:

- the constructor records the instance in a file-static pointer under `#if defined(WEB)`;
- `ons_host_pointer_probe(int *out, int max, int cols, int rows)` reports
  `current_button_state` followed by a grid of `mouseOverCheck` results.

Three things blocked it, each with a known fix:

1. **`current_button_state`, `current_over_button`, `screen_width/height` and
   `root_button_link` are all private** — not protected, as assumed. A free function
   reaches none of them. `ButtonState` is also a *nested* private type, so even the
   parameter type is unreachable.
   *Fix:* put the whole sweep inside a `#if defined(WEB)` **accessor method** on
   `ONScripter`, which can read every one of those members, and have the `extern "C"`
   function call it. An accessor rather than a friend declaration keeps the change in
   one place.
2. **The probe must not require the host to allocate.** `_malloc`/`HEAP32` are not in
   `EXPORTED_RUNTIME_METHODS`, so the caller cannot pass a pointer in.
   *Fix:* the accessor fills a `static int[]` and returns a pointer to it; the host reads
   it with `Module.getValue`.
3. **`mouseOverCheck` is private** (the header's `public:` at line 442 is followed by
   `private:` at 448, and `mouseOverCheck` sits after it). Fix 1 covers this too.

The build script's restore logic was also fixed: it now restores every tracked file it
modifies **from git** before patching. Copying a `.onsbak` was not safe — a run killed
before its trap leaves the live file patched, the next run copies the patched file over
itself as the "backup", and the pristine original is lost. That happened once and
produced `redefinition of 'embed_host_instance'`, so every build result before this fix
should be treated as suspect.

Only after the button rectangles and the resolved button are known does the coordinate
space become a question worth asking (`screen_scale_ratio` suggests the hit test expects
display space, not the device space the events carry).

## Input: the `WEB` build's coordinate contract

The position offset is explained by the engine's own source, and it contradicts what the
first four attempts assumed. In `runEventLoop`, both mouse cases are guarded:

```cpp
// ONScripter_event.cpp:1241
#if !defined(ANDROID) && !defined(IOS) && !defined(WINRT)
          case SDL_MOUSEMOTION:
#if !defined(WEB)
            event.button.x = (event.button.x - render_view_rect.x) * screen_scale_ratio1;
            event.button.y = (event.button.y - render_view_rect.y) * screen_scale_ratio2;
#endif
            if (mouseMoveEvent( &event.motion )) return;

// ONScripter_event.cpp:1273
#if !defined(WEB)
            event.button.x = (event.button.x - render_view_rect.x) * screen_scale_ratio1;
            event.button.y = (event.button.y - render_view_rect.y) * screen_scale_ratio2;
#endif
            ret = mousePressEvent( &event.button );
```

**A `WEB` build skips the conversion.** `mousePressEvent` then copies the coordinates
through unscaled:

```cpp
current_button_state.x = event->x;
current_button_state.y = event->y;
```

and `mouseOverCheck` compares against **screen** space — the `Display: 793 x 446` from the
banner, not the 1189x669 canvas. On other platforms SDL supplies window pixels and the
engine converts them down, so on `WEB` the **host** has to supply screen coordinates.

### The consequence, and the correction

The host must divide canvas pixels by `screen_scale_ratio` (1189/793 = 1.5). Three earlier
positions on this were wrong, including an intermediate "revert" to unconverted pixels that
was itself the regression:

| coordinates forwarded | result |
| --- | --- |
| unconverted canvas pixels (1189x669 space) | every click 1.5x too far right and down — the offset reported |
| divided by 1.5 | **screen space, which is what `WEB` expects** |

Verified: a click at canvas `(452,521)` now arrives as screen `(301,347)`, and the engine's
own handler reports screen-space values.

### The second requirement, from AetherKiri

The press must queue a motion event first. AetherKiri's ONS runtime states why:

```cpp
// ONScripter resolves a button press through `current_over_button`,
// which is refreshed by SDL_MOUSEMOTION rather than by the button
// event's coordinates. Make every press self-contained.
if (type == 1) { motion.type = SDL_MOUSEMOTION; ... PushRuntimeEvent(motion); }
```

Its coordinate transform also confirms the direction:

```cpp
input_device_scale_x.store(1.0f / ons->screen_scale_ratio1);
input_device_offset_x.store(ons->render_view_rect.x);
const int device_x = script_x * input_device_scale_x + input_device_offset_x;
```

### What is still unresolved

`SDL_GetMouseState` continues to report the window size rather than the pointer — the motion
handler logs `ev=(793,446)`, which is `screen_device_width/height`, i.e. the value SDL
produces when its internal mouse position was never set. The injected motion event's real
coordinates are therefore not what that handler reads.

Whether clicks now activate the intended control is **unverified**: every measurement taken
so far ran on a game state that had already been advanced past its buttons by the page's own
self-test. That self-test is now disabled (`RUN_SELF_TEST = false` in `index.html`), because
it invalidated several readings — a probe reporting `hits: 0` was repeatedly read as "no
buttons exist" when the clicks had simply moved past the screen that had them.

The next measurement must be taken on a freshly loaded page before anything clicks.

## Display: filling the viewport, and the coordinate identity

Two problems were reported together — the picture did not fill the window, and the pointer
was offset. Both had the same cause, and it was the canvas size.

### The engine fits, it never stretches

`calcRenderRect()` takes its device size from the renderer and then constrains it:

```cpp
// ONScripter.cpp:71
inline void alignAspectRatio(const int &sw, const int &sh, int &dw, int &dh) {
    int swdh = sw * dh;
    int dwsh = dw * sh;
    if (swdh == dwsh) return;
    if (swdh > dwsh) dh = (int)ceil(sh * ((float)dw / sw));
    else             dw = (int)ceil(sw * ((float)dh / sh));
}

// ONScripter.cpp:78
void ONScripter::calcRenderRect() {
    SDL_GetRendererOutputSize(renderer, &width, &height);
    screen_device_width = width;
    screen_device_height = height;
    alignAspectRatio(screen_width, screen_height, screen_device_width, screen_device_height);
```

`alignAspectRatio` **fits inside and never stretches** — it only ever shrinks one dimension.
So a canvas whose aspect ratio differs from the script's gets letterboxed *inside itself*,
which is why the picture did not fill the window however the page was styled. No amount of
CSS could fix it while the canvas disagreed with the engine.

### The fix, and the identity it produces

The canvas is now pre-sized to the game's own resolution, `793x446` — the same number the
engine prints as `Display:` — and the page stretches it to fill with `object-fit: fill`:

```
canvasPixel:   793x446
elementRect:   1920x680     ← fills the viewport box exactly
drawn:         1920x680     ← drawing area == element == viewport
scaleFactor:   1.000
scale_full:    canvas currently 793x446, screen 793x446
```

Because the canvas and the engine's screen space are now **the same rectangle**,
`screen_scale_ratio` is exactly 1. That makes the coordinate mapping an identity — a click
at client `960` (half of 1920) reaches the engine as `397`, exactly half of 793 — and it
removes the whole class of scale errors that four earlier attempts were fighting. It also
supersedes the `#if !defined(WEB)` conversion discussed above: with a 1.0 ratio, whether the
host divides or not makes no difference.

### Still open

The engine's own hit test reports no buttons (`hits: 0` at both 192 and 1344 grid cells),
while `pointer.button` carries `-5`/`-4` — engine sentinels, not button numbers. So whether a
click activates a control cannot yet be confirmed on this scene. Either the current scene has
no buttons, or its button list is built at a point the probe does not reach. The mapping and
the display are no longer implicated: both are verified exact.

### Two scaling modes

The canvas is stretched to fill by default, and a second mode preserves the aspect ratio for
whenever that is preferred. `F2` toggles between them, and `window.__host.setScaleMode()`
does the same from a harness.

```
fill:     elementRect 1920x680   drawn 1920x680   object-fit: fill
contain:  elementRect 1920x680   drawn 1210x680   object-fit: contain
```

The distinction only matters as a choice because the canvas now matches the game's
resolution: with both at 1.778 the two modes are arithmetically identical, and `fill` cannot
distort. `contain` becomes useful if the engine is ever started at a different resolution —
`--window` can change it — at which point stretching would distort and letterboxing would not.

The mapping stays correct in both. `drawnRect` computes the drawing sub-rectangle for the
contain case and returns the whole element for fill, and in contain mode the drawn centre
still maps to the engine's centre:

```
containCentreMapsTo: { x: 397, y: 223 }
expectedCentre:      { x: 397, y: 223 }
```

That is the calculation two earlier rounds got wrong while `object-fit: contain` was the only
mode, which is why the mode is now explicit rather than implied by the stylesheet.

## Audio: real sound

The engine's audio now plays on the main thread:

```
## worker: input mousedown injected via shim (1 total)
audio: context running at 48000 Hz
audio: started (user gesture received)
## worker: audio 1350 callbacks, 5524800 frames relayed, peak 0.7682
## worker: audio 1850 callbacks, 7573440 frames relayed, peak 0.7744

stats: { started: true, state: "running", blocks: 2441, peak: 0.7744 }
```

`peak 0.7744` is the decisive number: the engine's mixer is producing real audio and
it is crossing the thread boundary. Before, the peak was permanently `0.0000`.

### Why it has to cross a thread boundary

A worker has no `AudioContext` — it is a window API — so the engine cannot play
anything itself. The engine mixes PCM into buffers the worker owns (SDL's callback
writes into them), and each block is transferred to the page, where a real
`AudioContext` and an `AudioWorklet` play it.

### The bug that made it silent

The stub context handed out a **new** `Float32Array` on every `getChannelData()`
call:

```js
getChannelData: () => new Float32Array(frames)   // wrong
```

so everything the engine wrote went into an array that was immediately discarded.
The audio was not missing because nothing fed the context — it was missing because
the samples had nowhere to land. Allocating the channel buffers once per processor
and reusing them fixed it, and the peak counter is what exposed it: without that
number, silent-but-connected and silent-because-broken look identical.

### The pacing problem, and flow control

SDL requests 4096 frames per callback, and in a worker that callback is driven by a
timer rather than by an audio clock — so a 20 ms timer produces roughly **four times
real time**. Posting that straight to the page flooded it and the relay trimmed the
queue constantly.

The engine's output is now accumulated and released in exact real-time blocks (960
frames per 20 ms tick, matching 48 kHz), so production matches playback. Trimming
fell from many per second to one, and underruns are rare. The counters stay in the
log because the producer and the audio clock are independent and will drift.

### What remains

The queue-tracking counter only increments on push and is not decremented by the
worklet's consumption, so the reported latency reads high (it showed `0.82 s`
against a `0.25 s` target). Feedback from the worklet — reporting frames consumed —
would make it exact and turn the trimmer into properly informed flow control.

## Honest limitations

**Corrected: this list had gone stale.** It still described the state before the two chapters
directly above it, and three of its four entries were no longer true. What actually stands:

- **Audio is real, not silent.** The stub-context era ended with `## Audio: real sound`: the
  worker's PCM is relayed to an `AudioWorklet` on the main thread and the peak reads `0.7744`
  (it was a permanent `0.0000` while the context was a stub). What is still imperfect is the
  **latency counter**, which only increments on push and so reports high (`0.82 s` against a
  `0.25 s` target) — feedback from the worklet would make it exact.
- **Saves are session-only.** This one is still true as stated, and it is narrow. `IDBFS` *is*
  linked and exported (the build's export list carries `callMain,UTF8ToString,FS,PATH,IDBFS`), but
  the worker mounts the `--save-dir` on **`MEMFS`** (`ons-worker.js`, "Saves go to MEMFS:
  persistent storage would need IDBFS"), so it lives for the session only and import/export is not
  wired. Note also that `packages/runtime-ons/src/wasm-runtime.ts:225` describes the same directory
  as `IDBFS`-mounted and `save()` reports `persistence: 'idbfs'` — so the **provider's save contract
  and the harness's mount disagree**. That mismatch is the open item, not a missing link.
- **Input is delivered through the source shim, and its last step is unproven.** The DOM route
  really does reach nobody (the two chapters above are right about *that*), which is why the
  engine is patched at build time: `ons_host_shim.h` replaces `SDL_WaitEvent` with
  `ons_host_wait_event`, and `ons_host_push_key` / `ons_host_push_mouse` / `ons_host_push_event`
  inject events. The engine reacts — a click pulls **6.5 MB of previously unrequested assets
  from `arc.nsa`** and asks for `sysimg/load023.png`, i.e. it advances the script. But the
  engine's own hit test has not confirmed that a click **activates the intended control**: the
  running-game sweep (`runPointerProbe(12, 8)` = 96 cells) read 1 hit that was never where the
  click was, the smaller/larger sweeps in the display chapter read 0, and every measurement so
  far ran on a state already advanced past its buttons.
- **`cannot open system.lua`** and the `uoncur.bmp` family are unchanged and pre-existing — the
  same lines appear in the shipped main-thread build, so they are properties of this title, not
  of the worker.

## Harness

- `node ports/ons/harness/serve.mjs`, then `http://127.0.0.1:4620/`
- `?trace` enables a cursor trap that reports the assignment site
- `http://127.0.0.1:4620/__served` — per-path transfer report, the guard every
  change in this work was checked against
- `ports/ons/harness/ons-worker.js` — engine host, shims, ranged lazy reads
- `ports/ons/harness/host.mjs` — main-thread half: canvas transfer, input relay
