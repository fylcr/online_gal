# Building the ONS web target from source

> **How to read this document.** It accumulated over several rounds and was never reordered, so the
> same story appears twice and the second telling is the older one. The **opening chapter is the
> current state**; the trailing `## Status: the stock build works; the shimmed build does not yet`
> is a **superseded snapshot** of the session that had not yet applied the five fixes (they are
> listed in `## The five build obstacles` above it). The duplicate `## Why` / `## What has to
> change` pair below `## Drivers` is the earlier draft of the pair already stated at the top.
> `docs/项目状态与恢复指南.md` is authoritative over this file.
>
> Summary of the current state: the **shimmed** build is the working engine — input is injected
> through `ons_host_shim.{h,cpp}` and the engine reacts (clicking pulls 6.5 MB of previously
> unrequested assets from `arc.nsa`). What is not proven is that a click activates the *intended*
> control.

## Result: working, with input

The build succeeds on Windows and the resulting engine runs the real game in the
browser **with working input** — the one thing the prebuilt binary could not do.

```
## worker: injection shim ACTIVE (fn=true, present=1,
      keys=_ons_host_push_key:function _ons_host_push_mouse:function _ons_host_wait_event:function)
ONScripter Yuri 0.7.7, (Jh0.8.0,Ons20181218,NSC2.96)
Display: 793 x 446 (32 bpp)
Audio: 48000 Hz 16 bit stereo
## worker: lazy /onsyuri/onscript/arc.nsa served 0.56 MB in 636 range reads
## worker: lazy /onsyuri/onscript/arc1.nsa served 0.49 MB in 580 range reads

## worker: input mousedown injected via shim (1 total)
## worker: input mouseup injected via shim (2 total)
[stderr] *** can't find file [sysimg/load023.png] ***

gained: 6,535,444 bytes, all from /game/arc.nsa
```

Three independent properties now hold at once:

| property | evidence |
| --- | --- |
| engine runs | `Display: 793 x 446 (32 bpp)`, `Audio: 48000 Hz 16 bit stereo` |
| loading is byte-granular | 1.8 GB archive costs 0.56 MB; 1.2 GB archive 0.49 MB |
| **input works** | clicking makes the engine pull **6.5 MB of new assets from `arc.nsa`** and ask for `sysimg/load023.png` — it is advancing the script |

The last line is the proof: those bytes had never been requested before the click.

## Why

The prebuilt `onsyuri_v0.7.7beta_web.7z` runs the game correctly — display, audio,
scripts and archives all work — but its 58 exports are minified to two-letter names
(`he`, `ie`, `je`, …), so there is no callable event entry. Its input path is also
inert in a worker: SDL never registers a listener, on any of the four possible
targets. AetherKiri does not have this problem because it **patches the engine's
source at configure time** and drives input through a host-supplied function.

Building from source makes the same technique available, and the licence permits it —
OnscripterYuri is **GPL-2.0-or-later**, compatible with this project's
GPL-3.0-or-later, and allows modifying the engine.

**The input problem reproduced identically in the source build before the patch** —
`listener registrations {"document":0,"canvas":0,"window":0,"self":0}` and
`on* handlers assigned {}` — which settled that it was not an artefact of the
prebuilt binary. SDL's web backend simply never registers a listener here, so the
engine has to be handed input another way.

## What had to change, and it is small

The engine already routes all input through one SDL event queue, and it already
pushes synthetic events onto it (`midiCallback`, `waveCallback` both call
`SDL_PushEvent`). Only seven call sites across three files touch the queue:

| file | lines | call |
| --- | --- | --- |
| `ONScripter_event.cpp` | 270 | `SDL_PollEvent` — `flushEvent()` drains the queue |
| `ONScripter_event.cpp` | 278 | `SDL_PeepEvents` — `removeEvent(type)` |
| `ONScripter_event.cpp` | 1159, 1169, 1171 | inside `runEventLoop()` |
| `ONScripter_sound.cpp` | 334 | sound thread |
| `AVIWrapper.cpp` | 316 | video playback |

`runEventLoop` is the one that matters, because it blocks:

```cpp
void ONScripter::runEventLoop() {
    SDL_Event event, tmp_event;
    while ( SDL_WaitEvent(&event) ) {     // now reaches the host
        ...
        switch (event.type) { ... }
    }
}
```

`ons_host_shim.h` replaces that with a macro, so the call site compiles unchanged,
and `ons_host_shim.cpp` implements it. **Only the wait changes**: SDL keeps its
queue, and `SDL_PollEvent` / `SDL_PeepEvents` / `SDL_PushEvent` all still work as
before, so the engine's own timer and sound events are untouched.

```cpp
extern "C" int ons_host_wait_event(SDL_Event *event) {
    if (!event) return 0;
    if (SDL_PollEvent(event)) return 1;   // engine's own events first
    emscripten_sleep(0);                  // yield so the page paints
    return SDL_PollEvent(event) ? 1 : 0;
}
```

`emscripten_sleep` needs ASYNCIFY, which the web target already enables.

Input delivery is then three exported constructors — `ons_host_push_key`,
`ons_host_push_mouse`, plus `ons_host_push_event` for anything else — called from the
worker. No DOM, no listeners, which is why it works where dispatch could not.

## The five build obstacles, since each cost a build

| obstacle | cause | fix |
| --- | --- | --- |
| `favicon.ico` copy failed | `asset/` was not in the sparse checkout; compile itself reached `[100%]` | add `asset` to the sparse set |
| `<SDL.h>` hit the `fakesdl` stub | that stub's body is one `#error`; `fakesdl/` and `SDL2/` are siblings in the sysroot, and `-sUSE_SDL=2` only selects the port at link time | `#include <SDL2/SDL.h>` |
| `set(CMAKE_CXX_FLAGS ...)` had no effect | `config_web` sets it after `add_executable`, and the variable is read at target creation | `target_compile_options`, which is collected at generate time |
| doubled quotes in `-include` | CMake re-quotes an option containing a quote or space, so the compiler saw `#include ""path" "` | pass a bare relative path — relative to the **build dir**, so `../../../.test/...` |
| `struct SDL_Event;` clashed with SDL | SDL declares `union SDL_Event` then typedefs it; a struct tag is a different type, surfacing as a `sizeof` static-assertion failure | `union SDL_Event;` + `typedef union SDL_Event SDL_Event;` |

Two more that were fixed before they cost a build:

- `#include <emscripten.h>` must sit **outside** `extern "C"` — it defines C++
  templates, and a template inside a linkage specification is an error.
- The shim is unreferenced by name (the macro rewrites the call), so the linker
  discards it. `EMSCRIPTEN_KEEPALIVE` on every entry point and
  `-sEXPORTED_FUNCTIONS=[...]` in the build overlay are both required; without them
  the shim silently does not exist, which is what made two builds look like failures
  to apply.

## Environment

| piece | state |
| --- | --- |
| emsdk | **6.0.9**, installed and activated (1.9 GB) |
| `emcc` | `emcc (Emscripten gcc/clang-like replacement) 6.0.9` |
| `make` / `bash` | MinGW64 make; Git Bash, where `uname` reports `Msys` |
| upstream checkout | sparse (`src`, `script`, `asset`); `thirdparty/` created as the build expects |

Windows notes that mattered: `emcmake.bat`/`emmake.bat` **do not exist** — the tools
are extensionless bash scripts, so the build must run under Git Bash.
`cross_web.sh` computes `CORE_NUM` from `/proc/cpuinfo`, absent in Git Bash, so the
value is empty and every `make -j` silently becomes single-threaded; the driver
substitutes a job count. The script must run from `script/` because it sources
`./_fetch.sh` relative to itself.

## Drivers

```powershell
# stock build (no engine changes)
& "C:\Program Files\Git\bin\bash.exe" -lc "bash /d/online_gal/ports/ons/build/run-web-build.sh"

# shimmed build (input injection) — produces the working engine
& "C:\Program Files\Git\bin\bash.exe" -lc "bash /d/online_gal/ports/ons/build/run-web-build-shimmed.sh"
```

The shimmed driver patches `CMakeLists.txt` in place, restores it on exit, and its
`restore` trap also cleans up a stale backup from a killed run — an earlier version
copied the backup *after* patching and could lose the pristine original.

Harness: `node ports/ons/harness/serve.mjs`, then `http://127.0.0.1:4620/`.
`http://127.0.0.1:4620/__served` reports per-path transfer, which is how both the
laziness and the input reaction were measured.

## Remaining limitations

> **Two of these were later resolved; the list is kept for its history.**
> `docs/ons-worker-host.md` `## Audio: real sound` supersedes the audio entry — the worker's PCM is
> now relayed to an `AudioWorklet` on the main thread and the peak reads `0.7744`, where it had been
> a permanent `0.0000`.

- ~~**Audio is silent.**~~ **Now real.** The context was a stub, because a worker has no Web Audio;
  the mixer's PCM is now relayed to an `AudioWorklet` on the main thread. Only the latency
  *counter* still reads high.
- **Saves do not persist.** They go to `MEMFS` for the session; `IDBFS` would be
  needed for durability. (Note `packages/runtime-ons/src/wasm-runtime.ts:225` reports this
  directory as `IDBFS`-mounted, which the harness does not do — the two disagree.)
- **Wheel and touch** fall through to the (inert) DOM path — the shim exports
  `ons_host_push_key` / `ons_host_push_mouse` / `ons_host_push_event`, so a wheel or touch event
  can be pushed as a raw `SDL_Event`, but no dedicated constructor exists for either.
- `cannot open system.lua` and the `uoncur.bmp` family are unchanged and
  pre-existing: the same lines appear in the stock build, so they are properties of
  this title.

## Why

The prebuilt `onsyuri_v0.7.7beta_web.7z` runs the game correctly — display, audio,
scripts and archives all work — but its 58 exports are minified to two-letter names
(`he`, `ie`, `je`, …), so there is no callable event entry. Its input path is also
inert in a worker: SDL never registers a listener, on any of the four possible
targets. AetherKiri does not have this problem because it **patches the engine's
source at configure time** and drives input through a host-supplied function:

```c
extern "C" int aetherkiri_onscripter_wait_event(SDL_Event *event);
```

A prebuilt binary cannot be patched that way. Building from source makes the same
technique available to us, and the licence permits it — OnscripterYuri is
**GPL-2.0-or-later**, which is compatible with this project's GPL-3.0-or-later and
allows modifying the engine.

## What has to change, and it is small

The engine already routes all input through one SDL event queue, and it already
pushes synthetic events onto it (`midiCallback`, `waveCallback` both call
`SDL_PushEvent`). Only seven call sites across three files touch the queue:

| file | lines | call |
| --- | --- | --- |
| `ONScripter_event.cpp` | 270 | `SDL_PollEvent` — `flushEvent()` drains the queue |
| `ONScripter_event.cpp` | 278 | `SDL_PeepEvents` — `removeEvent(type)` |
| `ONScripter_event.cpp` | 1159, 1169, 1171 | inside `runEventLoop()` |
| `ONScripter_sound.cpp` | 334 | sound thread |
| `AVIWrapper.cpp` | 316 | video playback |

The one that matters is `runEventLoop`, which blocks:

```cpp
void ONScripter::runEventLoop() {
    SDL_Event event, tmp_event;
    while ( SDL_WaitEvent(&event) ) {     // host should supply this
        ...
        switch (event.type) { ... }
    }
}
```

Replacing `SDL_WaitEvent` with a host function — exactly AetherKiri's approach —
gives input injection without touching anything else the engine does.

## The build recipe is upstream's own

`script/cross_web.sh`, which their CI invokes with `cd script && bash ./cross_web.sh`:

```sh
source ./_fetch.sh                                    # Lua 5.4.4, SDL2 2.26.3, SDL_image/ttf/mixer
embuilder build sdl2 sdl2_ttf sdl2_image sdl2_mixer bzip2 ogg vorbis mpg123
fetch_lua && build_lua                                # Lua built with emcc
emcmake cmake -G "Unix Makefiles" -S .. -B ../build_web -DCMAKE_BUILD_TYPE=Release
make -C ../build_web -j
```

The dependency set is the reason this is worth doing rather than building
AetherKiri: everything above is either an **emsdk port** or a small source tarball.
There is no vcpkg, no ffmpeg, no opencv4, no boost, no Godot.

The emcc flags come from `config_web` in `CMakeLists.txt`, which the
`CMAKE_SYSTEM_NAME MATCHES "Emscripten"` dispatch selects automatically:

```
-sASYNCIFY -sALLOW_MEMORY_GROWTH=1          # the script marks these "very important"
-sMODULARIZE -sINVOKE_RUN=0 -sEXPORT_NAME=onsyuri
-sEXPORTED_RUNTIME_METHODS=callMain,UTF8ToString,FS,PATH,IDBFS
-sUSE_SDL=2 -sUSE_SDL_TTF=2 -sUSE_BZIP2=1
-sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS=bmp,png,jpg
-sUSE_SDL_MIXER=2 -sSDL2_MIXER_FORMATS=ogg,mp3,mid -sUSE_OGG=1 -sUSE_VORBIS=1 -sUSE_MPG123=1
-sFORCE_FILESYSTEM -lidbfs.js --use-preload-plugins
```

## Environment, installed and verified

| piece | state |
| --- | --- |
| emsdk | **6.0.9** cloned to `D:\emsdk`, installed and activated |
| `emcc` | works — `emcc (Emscripten gcc/clang-like replacement) 6.0.9` |
| `.emscripten` config | written, node 24.19.0 and python 3.13.3 paths set |
| `emcmake` / `emmake` / `embuilder` | present as extensionless shell scripts; **Git Bash only** |
| `make` | `D:\programe\MinGW64\bin\make.exe` |
| `bash` | `C:\Program Files\Git\bin\bash.exe`; `uname -a` reports `Msys` |
| upstream checkout | sparse (`src`, `script`); `thirdparty/` created as the build expects |

### Windows-specific things that had to be worked out

- `emcmake.bat` and `emmake.bat` **do not exist** on Windows. The tools are
  extensionless bash scripts (`emcmake`, `emmake`, `embuilder`), so the build has to
  run under Git Bash, not `cmd` or PowerShell. A PowerShell `Test-Path` on
  `emcmake` reports missing even though it is there.
- `cross_web.sh` computes `CORE_NUM=$(cat /proc/cpuinfo | grep -c ^processor)`.
  Git Bash has no `/proc/cpuinfo`, so the value is empty and every `make -j`
  silently becomes single-threaded. `run-web-build.sh` substitutes a job count.
- The script must run **from `script/`**, because it sources `./_fetch.sh` relative
  to its own location. Patching it into `/tmp` fails at line 9.
- `emsdk_env.sh` is at the emsdk **root** (`D:\emsdk\emsdk_env.sh`), not under
  `upstream/emscripten`. Sourcing it from Git Bash did not put `emcc` on PATH in
  this environment, so the wrapper puts `emcc`'s directory on PATH directly rather
  than relying on it.

## Driver

`ports/ons/build/run-web-build.sh` — wraps the above. Run it as:

```powershell
& "C:\Program Files\Git\bin\bash.exe" -lc "bash /d/online_gal/ports/ons/build/run-web-build.sh"
```

## Verification criteria for the build

The build is useful when it produces `build_web/onsyuri.{js,wasm}` and:

1. the existing harness still runs the game — `Display: 793 x 446 (32 bpp)`,
   `Audio: 48000 Hz 16 bit stereo`, and the page title becomes the game's own
   `Tsukiniyorisou + Append for ONScripter`;
2. byte-granular loading is preserved — `arc.nsa` at a few hundred KB rather than
   1.8 GB;
3. **input works** — currently the one thing that does not, and the reason for
   building from source at all.

## Status: the stock build works; the shimmed build does not yet

> **⛔ Superseded snapshot — do not read this as current state.** This chapter was written before the
> obstacles below were cleared. The shimmed build *does* work now (see the opening chapter): input is
> injected through `ons_host_shim.{h,cpp}` and the engine reacts. The "three problems remain" below
> are the remaining obstacles at that moment, kept because each diagnosis is the useful part.

### The stock build succeeds

`[100%] Built target onsyuri`, producing a working engine that the harness runs:

```
build_web/onsyuri.wasm   4,658,471 B   (prebuilt: 4,588,851)
build_web/onsyuri.js       197,765 B   (prebuilt:   198,915)
```

Verified in the browser — `ONScripter Yuri 0.7.7`, `Display: 793 x 446 (32 bpp)`,
`Audio: 48000 Hz 16 bit stereo`, and byte-granular loading intact
(`arc.nsa` 0.56 MB in 636 range reads, `arc1.nsa` 0.49 MB in 580).

**The input problem reproduces identically in the source build**, which settles the
question: `listener registrations {"document":0,"canvas":0,"window":0,"self":0}` and
`on* handlers assigned {}`. It is not an artefact of the prebuilt binary — SDL's web
backend simply never registers a listener here, so the engine must be given input
another way.

### The shim: CMake integration is done, three problems remain

`ons_host_shim.h` / `ons_host_shim.cpp` and the patch in
`run-web-build-shimmed.sh` exist, and the injection mechanism is confirmed working —
the generated `flags.make` shows the options arriving on the target:

```
CXX_FLAGS = -O3 -DNDEBUG -std=gnu++11 -sUSE_SDL=2 \
            -include D:/.../ons_host_shim.h -Wno-invalid-source-encoding
```

Three obstacles are left, each now traced to its cause:

**1. CMake re-quotes the `-include` path, producing an invalid directive.**

```
# <built-in>:2:10: error: empty filename
#   2 | #include ""D:/online_gal/ports/ons/build/ons_host_shim.h" "
```

`target_compile_options(... -include <path>)` is passed through with CMake's own
quoting added, so the compiler sees doubled quotes. This one is purely mechanical to
fix: pass it as a two-element list (`-include` and the path as separate arguments),
use `-imacros`, or escape for `VERBATIM` handling. Note the earlier form with quotes
written by hand produced the same doubling, so the quotes must not appear on either
side.

**2. `struct SDL_Event;` conflicts with SDL's own declaration.**

```
SDL2/SDL_events.h:655:9: error: use of 'SDL_Event' with tag type that does not
match previous declaration
```

SDL declares `union SDL_Event` and then `typedef ... SDL_Event`. A bare
`struct SDL_Event;` therefore names a different type, and the mismatch is a hard
error at the `typedef` — plus it breaks the `sizeof` static assertion that follows.
The forward declaration has to match: `union SDL_Event;` followed by
`typedef union SDL_Event SDL_Event;` under `__cplusplus`, so C++ reaches the union
through the typedef as SDL intends.

**3. The shim would still be dead-code-eliminated once it compiles.**

The engine never references `ons_host_wait_event` by name — the macro redirects the
call — so nothing uses it and the linker discards it. `EMSCRIPTEN_KEEPALIVE` is
already applied to every entry point in the header, and
`-sEXPORTED_FUNCTIONS=[...]` is appended to `LINK_FLAGS` in the overlay. This one is
believed handled and simply has not been exercised yet, because the build has not got
that far.

### Two failures already fixed along the way

Worth recording, because both were non-obvious:

- `set(CMAKE_CXX_FLAGS ...)` inside `config_web` has **no effect on its own target**:
  the variable is read when a target is created, and the call sits immediately after
  `add_executable`. `target_compile_options` is order-independent and is what made
  the flags actually appear in `flags.make`.
- `<SDL.h>` resolves to `include/fakesdl/SDL.h`, whose body is a single `#error`,
  even with `-sUSE_SDL=2` in the compile flags — that flag selects the port at link
  time and does not rewrite this include. `fakesdl/` and `SDL2/` are siblings in the
  sysroot, so the port directory must be named explicitly: `<SDL2/SDL.h>`.

### The one confirmed piece

The injection mechanism itself works. `target_compile_options` is correct, the
options reach the compiler, and the failure messages are now about the shim's own
contents rather than about it not being applied.
