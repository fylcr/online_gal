# artemis-compat web worker

Pin `9f622629f595db85e6f76c45c06edde29d7d9961` of
`Weiss-UltimateSavior/artemis-compat` (GPL-3.0), plus the patches and overlay
in this directory. Not the official SDK, and not a renamed wasm2c bridge.

```bash
bash ports/artemis/compat-web/build.sh -j2
```

`ports/env.sh` finds `/d/emsdk` (emcc 6.0.9). Do not export `EM_CONFIG`.
The script archives the pin from a local git object when that object exists,
otherwise it clones the pin (a clone left by an earlier run is reused, and its
`HEAD` is asserted equal to the pin before anything is archived). `.ref` is
read-only input and is not the build tree.

`ARTC_PIN` must be a full 40-character commit hash. The recipe refuses a short
or non-hex value rather than resolving a branch, prints every path it is about
to remove, and only removes directories it has asserted sit under
`ports/artemis/compat-web/out`. Build-time identity is recorded in
`out/build.log`: the pin, each patch's `sha256`, `web.cmake` and `web_main.cpp`
`sha256`, a digest of the extracted source tree, and a digest of the patched
tree. `out/hashes.txt` holds the artifact hashes and byte sizes.

The only source overlay is `overlay/host/web_main.cpp` and
`overlay/cmake/web.cmake`. The old reference-only driver pair
(`web_worker.js` / `web_page.html`) has been removed: they were never a build
input, and keeping a second driver beside the production `web_main.cpp` invited
copying a non-production frame loop into a host. The freeze is one tick, one
`artc_web_frame` call, no module-owned `requestAnimationFrame`.

## Frame contract

The host calls `artc_web_frame`. This module has no `requestAnimationFrame`
and no `emscripten_set_main_loop`. `artc_web_start` / `artc_web_stop` are not
exported.

## Pack bytes

`Pf8Reader::ReadAt` calls, synchronously:

```js
Module.artcReadRange(path, offset >>> 0, heapDest >>> 0, length) // exact bytes, or -1
```

Exactly one number comes back, and it is the **byte count actually written**.
The read succeeds only when that count is exactly `length` and `heapDest` is
the buffer it was given. A missing function, a throw, a short read, a longer
answer, or a `heapDest + length` that runs past `HEAPU8` is `-1` and fails the
read. There is no separate status code, no `Module.__artcReadCopied` side
channel, and no `offset`/`dest`/`length` outside unsigned 32-bit.

`offset` and `length` are checked as integers **before** the callback, and the
heap bound is checked against integer values (`length > HEAPU8.length ||
dest > HEAPU8.length - length`), never against `dest + length`, so a truncated
argument cannot pass. `HEAPU8` is read inside the callback, after any heap
growth it triggers. A zero-length read returns success without calling the
callback at all.

There is no XHR fallback and no whole-pack preload. A later host may block a
worker on a SharedArrayBuffer; this module only requires the callback to be
synchronous.

The previous contract — `0` for success plus a `Module.__artcReadCopied`
counter — was removed because it needed two agreements to hold at once: the
return code and the counter. A worker written against "return the count" and a
host written against "return 0" disagree only on the *failing* read, which is
the read that was already hard to see.

## Saved ABI

| export | returns | notes |
| --- | --- | --- |
| `artc_web_open(pack, os)` | `1` / `0` | creates WebGL2 on `#canvas`, then opens the pack. `0` sets `artc_web_last_error` |
| `artc_web_frame()` | `1` keep / `0` exit | one host tick. Order: reset/exit, boot, drain input, enter frame, drain tags, step, present |
| `artc_web_pointer(x, y, down)` | void | CSS pixels. `MapPoint` is frozen |
| `artc_web_pointer_move(x, y)` | void | CSS pixels |
| `artc_web_key(key, down)` | void | |
| `artc_web_resize(cssW, cssH)` | void | letterbox input only |
| `artc_web_started()` | `0` / `1` | |
| `artc_web_stage_width()` / `artc_web_stage_height()` | int | |
| `artc_web_revision()` / `artc_web_presented()` | int | low 32 bits |
| `artc_web_exit_requested()` | `0` / `1` | |
| `artc_web_last_error()` | string | empty until a refusal |
| `artc_web_log_sink(fn)` | void | `fn(level, message)` or null |

## Added host ABI (not accepted against a real game)

| export | returns | notes |
| --- | --- | --- |
| `artc_web_pause(paused)` | `0` / `1` | `1` pauses clock+audio, `0` resumes. `Tick` returns immediately while paused. `0` if the engine is not started |
| `artc_web_now_ms()` | double | `LuaEngine::NowMs` while paused (clock does not advance). `NaN` if not started |
| `artc_web_dispose()` | void | `Shutdown` (releases compositor GL) then destroys the WebGL context |
| `artc_web_save(file)` | `1` / `0` | `SaveSnapshot`. `0` on empty/parent path or engine refusal. Not a success if the file was not written |
| `artc_web_load(file)` | `1` / `0` | `LoadSnapshot`. Same refusal rule |

`saveDir` is the pack's parent directory (`EngineContext::Open` when `save_dir`
is empty). `SaveSnapshot` writes the ARCV variable bank **plus an ARCL v1 scene
trailer** (`lyc` / `lyprop` / `lyevent` / `lytween` / `anime`) appended after the
bank, and depends on the game's `onSave` / `onLoad`. The bank alone was never a
restored scene — with it the background did not come back; the trailer is what
restores one. Not a BOWS export, and not the official snapshot format. A missing
`onSave`, a path that escapes `saveDir`, or a write error returns `0`.

Both new-file halves need matching versions: an older native build still writes
the plain ARCV prefix, and a host checks the trailer. An old host reading a new
trailer is not covered.

## Link

`-sENVIRONMENT=worker --no-entry -sINVOKE_RUN=0 -sASSERTIONS=1`
`-sINITIAL_MEMORY=67108864 -sALLOW_MEMORY_GROWTH=1 -sMAXIMUM_MEMORY=4294967296`
`-sSTACK_SIZE=4194304 -sUSE_WEBGL2=1 -sFULL_ES2=1 -sFORCE_FILESYSTEM=1`
`-sEXPORTED_RUNTIME_METHODS=ccall,cwrap,HEAPU8,FS,UTF8ToString`
`-sEXPORTED_FUNCTIONS=_malloc,_free`
`-fwasm-exceptions` (exceptions stay on for `native_save` / PSB `throw`/`catch`).

`FS` and `_malloc`/`_free` are exported for the page that has to place the pack
into the Emscripten filesystem before `artc_web_open` can read it: `FS` to write
the file, `ccall`/`cwrap` to call the C ABI with marshalled strings,
`UTF8ToString` for `artc_web_last_error`, and the allocator pair for a path
string that has to outlive the call that created it.

`-fwasm-exceptions` is a compile-and-link flag, so it has to be on the C++
target's `target_compile_options` as well as its link options. Put it on
`artemis_deps` (an `INTERFACE` library) instead and it lands **before** the link
libraries, where it emits no exception handling: the linker then drops
`liblua51.a` for resolving `__cpp_exception`. `lua51` is compiled with the same
flag, because Lua's `setjmp`/`longjmp` and the C++ `catch` have to share one
personality. The emitted format is the wasm exception-handling feature
(`__cpp_exception`), not `-fexceptions`, which is JS longjmp emulation and is
what Emscripten's own zlib is not built against.

Exports are the functions above. This is not the 207-import SDK bridge, and the
export count is deliberately not asserted here — read the wasm export section of
a real build instead of trusting a number in prose.

## Audio (web pull backend — not accepted against a real game)

`overlay/audio/audio_web.cpp` is compiled **instead of** the pinned
`src/audio/audio.cpp` (see `overlay/cmake/web.cmake`), the same one-`Audio::Impl`
rule the pinned CMake already applies for the OHOS backend. The pinned
`src/audio/audio.h` is **not modified**: it is byte-identical to the upstream pin
(`sha256 853970b48b59cd28990e97b6fec2d1d01b68f73aab716bd0bd1b164fb5a81a8d`),
verified against `git show <pin>:src/audio/audio.h`. `Audio::Impl` is reached
through the registry the backend's own `Audio` constructor publishes, because
`impl_` is private and widening the frozen header was not an option.

Why a pull instead of the Android push: the worker link is
`-sENVIRONMENT=worker --no-entry`, so there is no decode-ahead thread and no
OpenSL device queue. The engine host calls `AudioWeb::Pump` once per tick; the
page pulls interleaved stereo int16 per voice and feeds WebAudio itself. **No
`AudioContext`, no `AudioWorklet`, no `std::thread` in native code.**

`PlaybackMs` is the **presented position**: frames *handed to the host* over the
source rate, per the upstream contract "excluding PCM buffered ahead of the
speaker". The clock therefore advances **only where PCM is actually pulled** —
an unpulled voice reads 0, which is what `render/video_player.cpp` advances a clip
against. The pull is acknowledged: the host's consumed-frame feed moves the
presented position, so EOF is reported before the final ACK rather than on the
last pull. `IsPlaying` stays true while a paused or looping voice still has audio;
it goes false once a non-looping voice is drained, which is what ends `[wait se]`.

| export | returns | notes |
| --- | --- | --- |
| `artc_web_audio_pump()` | `1` / `0` | one tick: reaps voices whose PCM ran out **and** whose speaker ACK caught up. `0` if not started |
| `artc_web_audio_voice_count()` | int | voices the engine holds; `0` when there is nothing to pull |
| `artc_web_audio_voice_handle(index)` | double | stable handle, never reused. `0` = out of range |
| `artc_web_audio_voice_key(handle)` | string | engine key; `""` for an unknown handle |
| `artc_web_audio_voice_rate(handle)` | int | source sample rate; `0` for an unknown handle |
| `artc_web_audio_read_stereo(handle, dest, frames, outGain, outPan)` | frames written | `0` = nothing left, `-1` = bad argument (null `dest`, negative `frames`). Never writes more than `frames` |
| `artc_web_audio_ack(handle, cumulativeFrames)` | `1` / `-1` | the speaker's ACK: **cumulative** frames consumed, not a delta. Moves the presented position; `-1` on unknown handle, backward, or past produced |
| `artc_web_audio_ended(handle)` | `0` / `1` | `1` when drained **and** ACKed. An unknown handle is `1` |
| `artc_web_audio_played_frames(key)` | double | presented position in frames — advanced by the ACK, not by the pull cursor and not by wall time |

Gain/pan are handed back on the pull in the upstream `0..1000` / `-1000..1000`
ranges; the page maps them to WebAudio. Handles are serials: a handle kept past
a `Stop` resolves to nothing rather than to a later voice. Unknown handles,
null buffers and a non-started engine are refusals, never silent successes.

### Argument rules live in `overlay/audio/audio_validation.h`

The page's numbers are JS `number`s that become `int64`/`uintptr_t` at the CABI
boundary, so every one of them is checked before it reaches the heap or the
decoder. Those checks used to be file-static helpers in `web_main.cpp`, which
left the audio test asserting a *copy* of the rules — it could only link them by
building the whole engine. They are now shared:

- `AudioHandleOk` — finite integer in `1 .. 2^53-1`. No cast first: `NaN`/`Inf`
  become implementation-defined when truncated, and `0` means "no such voice".
- `AudioFramesOk` — finite integer in `0 .. 2^53-1`.
- `HeapSpanOk(addr, bytes, align, heapBytes)` — the span fits and is aligned.
  Subtracts, so a wrapping `addr + bytes` cannot pass. A **zero-length span is
  legal anywhere** (including at `0` and at the heap end): a zero-frame pull
  writes nothing. `addr == 0` with a non-zero length is refused.
- `ValidateAudioReadParams(...)` — the composition the read entry point uses:
  handle, then frame count (finite, integral, non-negative, `<= 4096`), then the
  destination span when `frames > 0`, then the optional gain/pan spans when
  present. A refusal writes nothing through its out-params.

`4096` is `kMaxReadFrames` in that header, and `web_main.cpp` derives its
`kAudioPullFrames` from it — one constant, not two that can drift. The header
includes no `<emscripten.h>`: the heap size is an argument, which is what makes
it callable from a plain host compiler. It uses `<stdint.h>`/`<math.h>` and the
C `isfinite`/`trunc`, so it compiles under `-std=c++98` as well as `c++17`.

`web_main.cpp` keeps `AudioHandleOk`/`AudioFramesOk` as `using` aliases so its
call sites read as before; `HeapSpanOk` remains a one-line wrapper that supplies
the live `emscripten_get_heap_size()` to the shared function. **`MapPoint` is not
touched by any of this.**

The rules are exercised by `tests/artemis-compat-audio-native.test.mjs`, which
calls these functions directly (no mirror) and drives the real backend through
`AudioWeb` over a synthetic stream. It needs no emsdk and no engine build; the
compiled half runs when a C++ compiler is present and reports `skip` — never a
pass — when one is not.

Audio on this target was previously the upstream non-Android stub
(`PlayStream` returned false). **A green link is still not a playable game** —
this backend has not been accepted against a real game, on a real page, with a
real `AudioContext`.

## Building from the source bundle

`make-source-bundle.mjs` writes a GPL-3.0 source offer that is also a buildable
tree. It carries the pinned `src/`, all three patches (`0003-scene-checkpoint.patch`
is the save/restore one), every overlay — including the
three audio files above, which exist nowhere upstream — the tracked `build.sh`,
and `RUN-BUILD.sh`.

`build.sh` is included for its *build logic* and is not rewritten for the
archive: it sources `ports/env.sh` and refuses any path not ending in
`ports/artemis/compat-web/out`, because that check is what stops a stray `cd`
from becoming a deleted directory. The archive has neither, so `RUN-BUILD.sh` is
the same build without those two assumptions:

```bash
tar -xzf artemis-compat-source.tar.gz
cd artemis-compat-9f622629f595
EMSDK_DIR=/path/to/emsdk bash RUN-BUILD.sh -j2
```

Verified: two runs of the generator produce a byte-identical tarball, and `RUN-BUILD.sh`
**needs no repository and no network** — a build from the unpacked archive, executed
standalone, reproduces `artemis-compat.js` and `artemis-compat.wasm` **bit-for-bit**
against the in-repository build. That is the core evidence for this claim, and it lives in
the local receipt tree (`.test/…`, gitignored): it is **local evidence only**, and its
absence on a clean clone or in CI must not be read as "already accepted".

## Tests

```bash
node --test ports/artemis/compat-web/tests/artemis-compat-audio-native.test.mjs
```

The ACK-driven clock contract (EOF before ACK, ACK monotonicity, loop and pause,
independent handles) plus the real-ogg seam lives in
`.test/artemis-compact-audio-native-final-audcap/`, which is the local workbench
and needs `out/build-src` from a completed `build.sh` run.
