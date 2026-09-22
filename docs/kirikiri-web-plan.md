# KiriKiri on the web

Goal: run KiriKiri games in a browser — the real engine compiled to WebAssembly, with XP3 archives
read **byte-granularly on demand** rather than loaded whole.

## The decisive reference: kirikiroid2-web

[fenghengzhi/kirikiroid2-web](https://github.com/fenghengzhi/kirikiroid2-web), branch `web`, commit
`13dda190f8370d02b6cf59286a088529b355658c`: **a working WebAssembly port of KiriKiri2 using
Emscripten 6.0.9** — the same toolchain version installed here. It answers every open question, and
the answer is not the route this project was taking.

### It uses JSPI, not blocking threads

The threading model, quoted from `cpp/core/environ/web/VirtualLazyFS.h`:

> **主线程**: 元数据走同步 `EM_JS`；读未命中块缓存时走 `EM_ASYNC_JS`（**JSPI 挂起**，要求调用栈源自
> promising 导出 —— `main()` 或主循环 tick）。
> **pthread**: 经 emscripten proxying 系统队列投递到主线程执行，worker 在 **futex** 上阻塞等待完成。

So: metadata reads are synchronous; a cache miss **suspends the engine through JSPI**
(`EM_ASYNC_JS`) and resumes when the fetch completes. There is no synchronous XHR anywhere, and the
`platforms/web/` directory carries the integration that makes it work — `pthread_jspi.c`,
`pthread_jspi.js`, a 7.4 KB `pthread_jspi.md` write-up, and `jspi_jsc_mainloop_fix.js`.

**This is the flaw in this project's approach.** Wrapping a blocking `Read` around synchronous XHR
required `PROXY_TO_PTHREAD`, which put `main` in a worker, which made a page global invisible to the
bridge, which is the defect the ranged work was stuck on. **JSPI removes the requirement entirely** —
the read suspends instead of blocking, so the engine can stay on the main thread and no worker is
needed. Every downstream difficulty followed from that one wrong turn.

### It provides a lazy virtual filesystem, which is exactly the missing piece

`cpp/core/environ/web/VirtualLazyFS.{h,cpp}` — a C ABI — plus `platforms/web/vlfs.js` (62 KB):

> 游戏文件不再复制进 Emscripten MEMFS，而是由 JS 侧懒加载虚拟文件系统（`vlfs.js`，主线程单例）按需供数：
> 读 = **Blob 切片 / HTTP Range / ZIP stored 切片 / deflate 解压落 OPFS**
> 写 = 内存 overlay（存档级小文件），关闭时由 JS 钩子做 IDB 持久化

Four read backends, chosen per source, with an in-memory write overlay persisted to IndexedDB. The
API is a small virtual-filesystem surface — `Has`, `Stat`, `ResolveCase`, `ListDir`, `Open`, `Close`,
`Seek`, `Size`, `Read`, `ReadAsync`, `Write`, `Unlink`, `MkDir` — including **case-insensitive
resolution** and directory listing merged across VLFS and MEMFS.

### Where they hook, and how it differs here

`cpp/core/environ/web/Platform.cpp` overrides the **platform filesystem functions** rather than
patching a constructor:

```cpp
// Platform.cpp:167   create_directory
if (VLFS::Enabled()) { return VLFS::MkDir(folder.AsStdString().c_str()) == 0; }

// Platform.cpp:194   the file check
// VLFS（懒加载虚拟文件系统）优先：游戏文件不在 MEMFS 中
if (VLFS::Enabled()) { if (VLFS::Stat(name, &size, &isDir)) { ... } }

// Platform.cpp:352   directory enumeration merges VLFS (game files) and MEMFS (stray small files)
```

The decision to route storage above the backend matches this project's conclusion — the archive is
not in the filesystem, so the check must be intercepted before the filesystem lookup — but it is
implemented as a **platform layer** that upstream's build selects, not as a patch to
`_TVPCreateStream`. That is the more maintainable form of the same idea.

## Revised plan

**Abandon the synchronous-XHR ranged stream in krkrsdl2.** It is diagnosed, not merely difficult: the
blocking-read requirement forced the pthread model, and that model is what broke. Continuing to patch
around it works against the grain of the platform.

**Adopt the krkr2-on-web architecture**, which is also what the user's direction pointed at:

1. **Take kirikiroid2-web as the base** rather than krkrsdl2. It is a krkr2 engine build for the web
   that already works, with JSPI, VLFS, OPFS, and a COOP/COEP server script
   (`coi-server.py`) — and its README's prerequisites (emsdk **6.0.9**, cmake 3.31.1+, ninja, bison,
   vcpkg) are all available or in place, with 6.0.9 already installed.
2. **Build it as-is first**, reproducing the upstream recipe before changing anything:
   `embuilder` prewarm for `libpng`, `freetype`, `harfbuzz`, `sdl2`, `sdl2_ttf` (plus `-mt`
   variants), then `cmake --preset "Web Release Config"` and `cmake --build out/web/release`. Output
   is `index.html`, `index.js`, `index.wasm`, `vlfs.js`, `assets.zip`.
3. **Only then** adapt: the integration surface is `cpp/core/environ/web/` (three files, ~39 KB) and
   `platforms/web/` — small, and written to be a platform layer.
4. **Check the licensing** before reusing anything: kirikiroid2-web's own LICENSE, and the krkr2
   origin it forked. This project ships GPL-3.0-or-later and KiriKiri2 is BSD-style, so the
   combination is likely fine, but it must be recorded in `THIRD_PARTY_NOTICES.md`.

**What carries over from the krkrsdl2 work:** the build driver and patcher discipline, the launcher
concept, the range-request accounting and the `/__diag` channel, the COOP/COEP server, and —
most usefully — the list of traps in "Problems found" below, most of which are Emscripten-wide.

**What to drop:** `tTVPHttpRangeStream`, the `_TVPCreateStream` intercept, the `--js-library` bridge,
and the `-pthread` requirement for blocking reads.

## The upstream build recipe, from CI

Read from `.github/workflows/build-web.yml` on the `web` branch, so it is the configuration that
actually produces releases rather than an inferred one.

**Prerequisites**, with local status:

| step | CI does | here |
| --- | --- | --- |
| system deps | `apt-get install bison ninja-build python3 autoconf automake libtool` | **bison missing**; ninja's Python module is installed but no `ninja.exe` on PATH |
| Emscripten | `emsdk install 6.0.9` (pinned in `env: EMSDK_VERSION`) | **6.0.9 already installed** |
| vcpkg | `lukka/run-vcpkg@v11` at `vcpkgGitCommitId: b1e15efef6758eaa0beb0a8732cfa66f6a68a81d` | **cloned to `.ref/vcpkg` at that exact commit, bootstrapped** |
| submodules | `submodules: recursive` | `.ref/kirikiroid2-web` has `cpp/plugins/layerExImage` only |

**The build**, in order:

```bash
# 1. Emscripten ports must be prewarmed before configuring; vcpkg compiles against these headers.
#    The explicit variants are needed because "the 6.0.9 libpng/FreeType ports pass True as a
#    boolean flag" — a quirk of this exact SDK version.
embuilder build libpng libpng-mt libpng-legacysjlj libpng-mt-legacysjlj
embuilder build freetype freetype-legacysjlj
embuilder build harfbuzz harfbuzz-mt
embuilder build sdl2 sdl2-mt
embuilder build sdl2_ttf sdl2_ttf-mt

# 2. Configure. CMake drives vcpkg from inside this step, and CI retries three times to ride out
#    transient source-download failures.
cmake --preset "Web Release Config"

# 3. Build.
cmake --build out/web/release
```

Environment the build expects: `VCPKG_ROOT`, `EMSDK`, `EM_CONFIG`, and `EMSDK_NODE` pointing at the
node binary inside the SDK. CI also puts `$EMSDK`, `$EMSDK/upstream/emscripten`, and the directory
holding `node` on `PATH`.

**Outputs** in `out/web/release/`: `index.html`, `index.js`, `index.wasm`, `vlfs.js`, `assets.zip`,
plus PWA files. `index.worker.js` appears depending on SDK and options.

**Why this order matters:** the ports must be built *before* configuring, because vcpkg compiles
against those headers. Doing it the other way produces failures that look like missing headers rather
than a sequencing mistake.

## Swapping in our file system: a JavaScript change, not a wasm rebuild

The requirement is to use this project's own byte-granular file system rather than loading whole
files. The important thing found here is **where that decision actually lives**, because it changes
the size of the work considerably.

**The wasm does not know how bytes arrive.** It calls the VLFS C ABI (`VirtualLazyFS.h`: `Has`,
`Stat`, `ResolveCase`, `ListDir`, `Open`, `Seek`, `Size`, `Read`, `ReadAsync`, …) and
`VirtualLazyFS.cpp` forwards to JavaScript. **All fetching happens in `platforms/web/vlfs.js`** — 62 KB
of JavaScript, with no engine rebuild involved.

Its data sources, from its own header comment and code:

```
vlfs.js:9    Blob when available (download package / dragged File / ZIP entry slice, off-heap)
vlfs.js:11   OPFS for decompressed output, validated by resource URL + strong ETag, reused across pages
vlfs.js:145  var response = await fetch(url, { ... })
vlfs.js:218  OPFS directory handling
```

So **the byte-granular swap is: replace `vlfs.js`'s byte source with `@online-gal/vfs`.** This
project already has exactly that machinery, and it is verified against a real archive:

| piece | what it provides |
| --- | --- |
| `packages/vfs/src/lazy.ts` | `LazyFileSystem` implementing `GameSource`, `readAt`, `readRange`, `manifest()`, `stats()` |
| `packages/vfs/src/sync-bridge.ts` | `rangeFetch`, `createBridgeWorker`, `SyncFileBridge` — synchronous ranged reads for a blocking caller |
| `packages/vfs/src/archives/{nsa,xp3,zip}.ts` | archive readers over the same byte source |

The shape matches: `vlfs.js` is already a **main-thread singleton that serves reads on demand and
proxies to a worker when needed**, which is what `sync-bridge.ts` does, and the ONS port measured the
payoff of that arrangement — **0.56 MB of ranged reads serving a 1.8 GB archive.**

**Consequence for the build order:** reproducing the full upstream build is only needed to *change the
engine*. For the file-system requirement it is **not** on the critical path, because the swap is on
the JavaScript side of a fixed ABI. A working upstream build could be used as-is and the storage
replaced underneath it.

**But the engine build is still worth having**, because the storage swap needs something to run
against and because the engine is where a game is actually simulated. The prerequisites are now
mostly in place: Emscripten 6.0.9 is installed and matching, vcpkg is cloned at the pinned commit,
and what remains is `bison` and getting `ninja.exe` onto `PATH`.



The requirement is to use this project's own byte-granular file system rather than loading whole
files. The important thing found here is **where that decision actually lives**, because it changes
the size of the work considerably.

**The wasm does not know how bytes arrive.** It calls the VLFS C ABI (`VirtualLazyFS.h`: `Has`,
`Stat`, `ResolveCase`, `ListDir`, `Open`, `Seek`, `Size`, `Read`, `ReadAsync`, …) and
`VirtualLazyFS.cpp` forwards to JavaScript. **All fetching happens in `platforms/web/vlfs.js`** — 62 KB
of JavaScript, with no engine rebuild involved.

Its data sources, from its own header comment and code:

```
vlfs.js:9    Blob when available (download package / dragged File / ZIP entry slice, off-heap)
vlfs.js:11   OPFS for decompressed output, validated by resource URL + strong ETag, reused across pages
vlfs.js:145  var response = await fetch(url, { ... })
vlfs.js:218  OPFS directory handling
```

So **the byte-granular swap is: replace `vlfs.js`'s byte source with `@online-gal/vfs`.** This
project already has exactly that machinery, and it is verified against a real archive:

| piece | what it provides |
| --- | --- |
| `packages/vfs/src/lazy.ts` | `LazyFileSystem` implementing `GameSource`, `readAt`, `readRange`, `manifest()`, `stats()` |
| `packages/vfs/src/sync-bridge.ts` | `rangeFetch`, `createBridgeWorker`, `SyncFileBridge` — synchronous ranged reads for a blocking caller |
| `packages/vfs/src/archives/{nsa,xp3,zip}.ts` | archive readers over the same byte source |

The shape matches: `vlfs.js` is already a **main-thread singleton that serves reads on demand and
proxies to a worker when needed**, which is what `sync-bridge.ts` does, and the ONS port measured the
payoff of that arrangement — **0.56 MB of ranged reads serving a 1.8 GB archive.**

**Consequence for the build order:** reproducing the full upstream build (vcpkg, Emscripten ports,
JSPI toolchain hooks, `Emscripten 6.0.9` exactly, enforced by a version check in `CMakeLists.txt`) is
only needed to *change the engine*. For the file-system requirement it is **not** on the critical
path, because the swap is on the JavaScript side of a fixed ABI. A working upstream build could be
used as-is and the storage replaced underneath it.

### Build prerequisites, checked

| | |
| --- | --- |
| Emscripten | **6.0.9 — exact, enforced**: `if(NOT EMSCRIPTEN_VERSION VERSION_EQUAL "6.0.9")` warns. Installed here at 6.0.9 |
| cmake | 4.3.1 present |
| ninja | the Python module is installed (`python -m ninja`), but **no `ninja.exe` on PATH** |
| vcpkg | **not present**; the build requires `VCPKG_ROOT` |
| bison | **not present** |

`ninja` is trivial (`pip install ninja` put the module in place; the executable needs its Scripts
directory on PATH). `vcpkg` and `bison` are real downloads. **But per the finding above, none of them
block the file-system work** — they block rebuilding the engine, which the requirement does not
currently demand.



Builds, runs, renders the KAG3 demo, threads on, ranged stream compiled and linked, **range requests
zero**. The selection is confirmed present in the patched sources and in the wasm, yet the entry
diagnostic never fires — so the intercept is not reached at all, which is consistent with the engine
failing earlier or the storage path differing from the assumption. **Do not continue debugging this**;
the architecture above supersedes it.

## Problems found, recorded because each cost real time

They share a shape: **the failure pointed somewhere other than its cause.**

| Problem | Symptom | Cause |
| --- | --- | --- |
| `data` parameter rejected | stuck "Downloading...", 404, canvas at 300x150 — reads like an engine failure | the generated page validates `?data=` against `/^[0-9a-z._-]+$/i` and throws on any `/` |
| `MODULARIZE=1` | no error, one log line, blank canvas | loading `krkrsdl2.js` defines a factory and starts nothing; the factory must be called |
| IDBFS sync ordering | `Fatal Error: Use path delimiter '>' or '/'` | the `syncfs` restore must be gated on a run dependency |
| C++ standard conflict | `bind.h` needs C++17, `tjsArray.cpp` needs C++14 (`std::binary_function`, removed in 17) | one file gets `-std=c++17` **and** `SKIP_PRECOMPILE_HEADERS`, since the PCH is built at the target's standard |
| `EXPORTED_FUNCTIONS` | `callMain` vanished from the glue | a later `-sEXPORTED_FUNCTIONS=` **replaces** rather than merges |
| `EM_JS` JavaScript | `expected ')'` at the `EM_JS` line, macro "undefined" — points at the preprocessor | `EM_JS` holds JS in a C string and the patcher added a template literal: **an apostrophe or backtick in a comment terminates one of them** |
| `--pre-js` for a symbol | `undefined symbol` | a pre-js runs in a different scope; symbols come from **`--js-library`** |
| js-library top-level assignment | `Internal compiler error JS compiler` | assignments belong inside the library object |
| target-property `--js-library` | symbol still undefined, indistinguishable from a wrong path | pass it via `-DCMAKE_EXE_LINKER_FLAGS` |
| patchers reverting each other | `anchor not found` from a patcher whose input was wiped | each restored from git at start; the driver now restores once |
| banner split by insertion | `// tTVPLocalFileStream#if defined(...)` — swallowing the `#if` | an insertion walk off by one line |
| CRLF | a visibly present three-line anchor never matched | the anchor used `\n`; anchored on one line |
| submodule restore | `pathspec ... did not match any file(s) known to git` | `external/krkrz` is a submodule: `git -C external/krkrz` |

**The meta-lesson, repeated often enough to state plainly:** when a class of failure recurs, suspect
the **approach**, not the current instance. The `EM_JS` quoting problem was diagnosed correctly by the
second failure and then worked around three more times. Likewise the blocking-read requirement should
have been questioned as soon as it demanded a worker — a reference implementation that avoids the
requirement was one search away.

## Build progress on kirikiroid2-web

Prerequisites are **all satisfied now**, and the configure step is the one thing left failing.

| | |
| --- | --- |
| emcc 6.0.9 | present, matching what `CMakeLists.txt` enforces |
| cmake 4.3.1 | present |
| ninja | `C:\Users\ASUS\AppData\Roaming\Python\Python314\Scripts\ninja.exe`; passed as `-DCMAKE_MAKE_PROGRAM` because CMake would not resolve it from `PATH` |
| bison | **installed** this session — `winget install WinFlexBison.win_flex_bison`, giving `win_bison.exe`, which the CMake's `find_program(... win_bison.exe bison.exe bison)` accepts |
| vcpkg | `.ref/vcpkg` at the CI's pinned `b1e15efe`, **full clone** and bootstrapped |
| Emscripten ports | prewarmed (`embuilder` for libpng/libpng-mt/libpng-legacysjlj/libpng-mt-legacysjlj, freetype/freetype-legacysjlj, harfbuzz/harfbuzz-mt, sdl2/sdl2-mt, sdl2_ttf/sdl2_ttf-mt) |

### What is blocking

`cmake --preset "Web Release Config"` fails inside vcpkg's manifest install:

```
error: in triplet x64-windows: Unable to find a valid Visual Studio instance
Could not locate a complete Visual Studio instance
The following paths were examined for Visual Studio instances:
  D:\programe\msvc\VC\Auxiliary/Build/vcvarsall.bat
```

**vcpkg is using `x64-windows` instead of `wasm32-emscripten`.** The preset does specify the right
triplet:

```json
// CMakePresets.json, "Web Config" (hidden, inherited by "Web Release Config")
"cacheVariables": { "VCPKG_TARGET_TRIPLET": "wasm32-emscripten", ... },
"toolchainFile": "$env{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
```

### What was already tried

| attempt | result |
| --- | --- |
| `-DVCPKG_TARGET_TRIPLET=wasm32-emscripten` on the command line | still `x64-windows` |
| `export VCPKG_TARGET_TRIPLET=wasm32-emscripten` in the environment | still `x64-windows` |
| deleting `out/` so no stale cache could carry the old triplet | still `x64-windows` |
| adding `vcpkg/` (the overlay ports and triplets) to the sparse checkout | fixed a *different* error — `Overlay path ".../vcpkg/ports" must be an existing directory` |
| `-DCMAKE_MAKE_PROGRAM=<abs path to ninja.exe>` | fixed a *different* error — Ninja not found |
| full (non-shallow) vcpkg clone | fixed a *different* error — `vcpkg was cloned as a shallow repository` during `read-tree` of a pinned port |

So the overlay, ninja and shallow-clone problems are resolved; this is the one remaining, and it is
specifically about **when** vcpkg's manifest install runs relative to when the triplet is known.

### Where to look next

The call stack is `CMakeDetermineSystem.cmake:146` ← `project()` at `CMakeLists.txt:38`, so the
install happens **during `project()`**. The relevant ordering in `CMakeLists.txt` is:

```cmake
elseif(VCPKG_TARGET_TRIPLET STREQUAL "wasm32-emscripten" OR EMSCRIPTEN)   # line 24
    set(WEB ON)
    set(VCPKG_TARGET_TRIPLET "wasm32-emscripten")
    if(CMAKE_TOOLCHAIN_FILE AND NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
        set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${CMAKE_TOOLCHAIN_FILE}")     # lines 28-30
    endif()
    set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)  # line 31
```

Note that the preset sets `toolchainFile` to **Emscripten's**, and line 31 then **replaces** it with
vcpkg's, having preserved the original as `VCPKG_CHAINLOAD_TOOLCHAIN_FILE`. Two things are worth
checking, in this order:

1. **Whether `EMSDK` is visible when the preset's `$env{EMSDK}` is expanded.** A preset whose
   `toolchainFile` expands to an empty or wrong path would leave CMake without the Emscripten
   toolchain, and vcpkg would then fall back to a host triplet. This is the cheapest thing to verify
   and it is consistent with every symptom.
2. **Whether `EMSCRIPTEN` is set at line 24**, since that branch is the one that both sets the triplet
   and swaps in vcpkg's toolchain. If it is not, execution falls to the `else()` at line 33, which sets
   vcpkg's toolchain **without** setting the triplet at all — which would produce exactly this error.

The second reading is the more likely one: `EMSCRIPTEN` is normally set by the Emscripten toolchain,
but the preset's toolchain file is replaced at line 31 during the same configure run, so on a **cold**
configure the variable may not yet be defined when line 24 is evaluated. The upstream CI runs from a
warm cache (it caches `.emsdk` and ccache) whereas this is a first configure — which is consistent with
the build working in CI and not here.

The project's own `CLAUDE.md` adds a Windows-specific note worth heeding rather than guessing:

> Windows/vcpkg 构建应显式导出 `EMSDK_PYTHON`，避免 vcpkg 误选不兼容的旧 Python

and it also notes that after changing `CMakeLists.txt` one must re-run `cmake --preset` before
building.

### The triplet problem was solved — `VCPKG_HOST_TRIPLET`

Four attempts failed before the right variable was found, and the reason it was hard to see is worth
recording: **the error named a triplet that was never the target one.**

| attempt | result |
| --- | --- |
| `-DVCPKG_TARGET_TRIPLET=wasm32-emscripten` | still `x64-windows`, though `CMakeCache.txt` recorded the right value |
| `export VCPKG_TARGET_TRIPLET=...` | still `x64-windows` |
| `export VCPKG_DEFAULT_TRIPLET=...` | still `x64-windows` — this sets the *target* default, not the host one |
| **`-DVCPKG_HOST_TRIPLET=wasm32-emscripten`** | **fixed** |

From `vcpkg/scripts/buildsystems/vcpkg.cmake`:

```cmake
# lines 492-493
if(DEFINED VCPKG_HOST_TRIPLET AND NOT VCPKG_HOST_TRIPLET STREQUAL "")
    list(APPEND Z_VCPKG_ADDITIONAL_MANIFEST_PARAMS "--host-triplet=${VCPKG_HOST_TRIPLET}")
```

vcpkg builds **host tools** for a triplet it chooses separately, deriving it from
`CMAKE_HOST_SYSTEM_PROCESSOR` (from line 314 onward). On Linux that yields `x64-linux`, which needs
nothing; on this machine it yields `x64-windows`, whose toolchain demands Visual Studio. **That is why
CI never hit it, and why the target-side fixes made no difference** — the two are independent.

### The next failure, also precisely identified

With the triplet fixed, vcpkg genuinely built for `wasm32-emscripten` and got as far as `cocos2dx`,
which failed compiling **libpng**:

```
emcc: error: attempt to set `WASM_LEGACY_EXCEPTIONS` to `True`; use 1/0 to set boolean settings
cmdline: emcc -c .../cache/ports/libpng/libpng-1.6.58/example.c -o ... \
           -sUSE_ZLIB -pthread -sSUPPORT_LONGJMP=wasm -sWASM_LEGACY_EXCEPTIONS=True
```

This is the quirk the CI comment referred to, seen from the other side. The port definitions in this
emsdk already use integers:

```python
# emsdk/upstream/emscripten/tools/ports/libpng.py:16-17
'libpng-wasmsjlj':    {'SUPPORT_LONGJMP': 'wasm', 'WASM_LEGACY_EXCEPTIONS': 0},
'libpng-mt-wasmsjlj': {'PTHREADS': 1, 'SUPPORT_LONGJMP': 'wasm', 'WASM_LEGACY_EXCEPTIONS': 1},
```

so the `True` is arriving as a **boolean** from the cached port build rather than from these literals —
`emcc` rejects it because the setting must be an integer. The likely cause is a **stale cached port
build**: this build directory was reused across several SDK-adjacent experiments, and the CI note is
explicit that after an SDK change one must use a fresh build directory and recompile all dependencies,
because *Emscripten does not guarantee ABI compatibility across versions*.

### Next step, in order

1. **Clear the Emscripten port cache and rebuild the ports cleanly**, in a fresh tree:
   remove `D:/emsdk/upstream/emscripten/cache/build` (and `cache/ports` if in doubt) and re-run
   `embuilder` for the same five groups.
2. If the boolean recurs, **drop the `-legacysjlj` variants** from the prewarm and let vcpkg build the
   libpng it needs — those variants exist for TJS2's `setjmp`/`longjmp` use, and if the engine needs
   them the error will reappear at link time with a clearer message than a port build failure.
3. Then re-run `cmake --preset "Web Release Config"` with the four variables that are now known to be
   required: `CMAKE_MAKE_PROGRAM`, `EMSCRIPTEN=1`, `VCPKG_TARGET_TRIPLET`, `VCPKG_HOST_TRIPLET`, plus
   `EMSDK`, `VCPKG_ROOT` and `EMSDK_PYTHON` in Windows form.

`cocos2dx` is a large port and a slow one; it is also **only the renderer**, so a failure here says
nothing about whether the storage swap is viable.

## Engine sources, and what each actually is

| Source | Contains | Verdict |
| --- | --- | --- |
| `.ref/kirikiroid2-web` | krkr2 core + **working web port** (JSPI, VLFS, OPFS) | **the new base** |
| `.ref/krkrsdl2` | krkrsdl2 + krkrz, Emscripten target, builds and runs | superseded, keep for reference |
| `.ref/krkr2-next` | krkr2 core + versioned C ABI, **no web support**, heavy deps (Boost×4, bullet3, ANGLE, OpenMP) | not the cheap path |
| `.ref/tynext` | app layer only, **zero C/C++** | nothing to reuse |
| `.ref/tynext-git` | `engine/`: 3 JNI files + prebuilt `libkrkrsdl3.so` | kernel is a binary |
| `.ref/kirikiroid2` | third port, tjs2 only | incomplete |

Kernel names are not interchangeable: `kirikiroid2` (that port), `krkrsdl2` (this project's earlier
base), `krkrsdl3` (Tyranor-Next's, binary only), `krkr2` (KrKr2-Next's sources, and what
kirikiroid2-web ports).

## Deliverables so far

```
ports/krkr/krkrsdl2/
  run-emscripten-build.sh     build driver; KRKR_RANGED_STORAGE=1 enables the ranged read
  patch-cxx-standard.mjs      one file at C++17, PCH skipped for it
  patch-pthread.mjs           -pthread, which actually defines __EMSCRIPTEN_PTHREADS__
  patch-ranged-storage.mjs    ranged stream, _TVPCreateStream intercept, js-library, /__diag
  patch-prejs.mjs             links krkr-range.js with --js-library
  launcher.html               takes ?archive=<url>, no copy into the web root
  serve.mjs                   port 4630; wasm MIME, COOP/COEP, ranges, all-request logging, /__diag
docs/kirikiri-web-plan.md     this file
```

Reference checkouts live under `.ref/` (gitignored). `krkrsdl2` and its `krkrz` submodule are returned
to pristine after each build and the patchers reapply from pinned revisions.

| | |
| --- | --- |
| krkrsdl2 builds for the web | **yes** — CMake + `emcmake`, emcc 6.0.9, threaded, 9 MB wasm |
| The stock engine runs in a browser | **yes** — renders the KAG3 demo |
| Ranged-read storage | **compiles and links**, selection wired |
| Range requests observed | **zero** — two candidate causes, undistinguished |
| KrKr2-Next | sources checked out, assessed, **not started** |

Pinned revisions:

| | |
| --- | --- |
| krkrsdl2 | `dccbdf6a14829ffee778123350dfc954f919d835` |
| krkrz (submodule) | `b11c43a07543756fcb943a60ff0a949a7f470f10` |
| KrKr2-Next | `1abd1ed4e8aec7abd5d2524c3a9ad7886880602f` |
| Tyranor-Next | app layer only; `engine/` separately cloned |
| Emscripten | 6.0.9 · CMake 4.3.1 · meson 1.12.0 |

## Engine sources: what each reference actually is

Checked rather than assumed, because the names are genuinely confusable — there are three distinct
kernels in play, and two of the references turned out not to contain engine code at all.

| Source | Contains | Verdict |
| --- | --- | --- |
| `.ref/krkrsdl2` | krkrsdl2 + `krkrz` submodule, full sources, Emscripten is a first-class target | **the working base** |
| `.ref/krkr2-next` | full KiriKiri2 core sources **plus** a versioned C ABI; **no web support** | better interface, heavier port |
| `.ref/tynext` | app layer only — **zero C/C++ files** | nothing to reuse |
| `.ref/tynext-git` | `engine/`: 3 JNI files + prebuilt `libkrkrsdl3.so` (28 MB) | kernel is a binary, not sources |
| `.ref/kirikiroid2` | a third port, tjs2 only | incomplete |

The three kernel names are not interchangeable:

| name | what it is | status here |
| --- | --- | --- |
| `kirikiroid2` | separate cross-platform port | `.ref/kirikiroid2`, tjs2 only |
| `krkrsdl2` | what this project builds | builds and runs |
| `krkrsdl3` | what Tyranor-Next uses | prebuilt `.so` only |
| `krkr2` | what KrKr2-Next vendors | full sources, no web target |

### KrKr2-Next: why it was considered, and why it is not the first path

**For it:** it vendors the complete KiriKiri2 core as sources (`XP3Archive.cpp`, `ZIPArchive.cpp`,
`7zArchive.cpp`, `TARArchive.cpp` — a wider archive surface than krkrz alone), and it exposes a
**versioned C ABI** instead of requiring internal patching:

```c
#define ENGINE_API_VERSION 0x01000000u
typedef struct engine_create_desc_t { uint32_t struct_size; uint32_t api_version; ... };
engine_create, engine_destroy, engine_open_game, engine_open_game_async,
engine_get_startup_state, engine_tick, engine_pause, engine_resume, ...
```

That is the `probe`/`create`/lifecycle shape this project's `RuntimeProvider` already uses, and
`engine_open_game_async` with `engine_get_startup_state` is a **poll-based** startup — what an
Emscripten host wants.

**Against it, and this decided the order:**

- **No web support whatsoever.** `__EMSCRIPTEN__`, `emscripten` and `wasm` appear nowhere in
  `cpp/`, `bridge/`, `cmake/` or `platforms/`; the only hits are substring matches inside
  `ShowWindowAsModal`.
- **Heavy dependencies**, much of it not web-portable: ANGLE (EGL + GLES2 with Metal/Vulkan
  features), **bullet3**, four Boost libraries (`iostreams`, `locale`, `phoenix`, `spirit`), 7zip,
  minizip, tinyxml2, and `find_package(OpenMP REQUIRED)` on non-Apple platforms.
- krkrsdl2 **already is** an Emscripten target and already runs in a browser. Its remaining problem
  is one identified defect; KrKr2-Next's would be an entire platform port.

The storage insertion point is equivalent in both — `tTVPFileMedia::Open` returns
`new tTVPLocalFileStream(...)`, the same single-class change as in krkrsdl2 — so the cleaner interface
buys little against having to build a web platform layer from nothing.

**Plan of record**, following the user's direction: use krkr2-compatible engines first, and reach for
krkrsdl3 or krkrz only for a game that cannot be simulated otherwise. KrKr2-Next remains the krkr2
source of record for later. The sample game is already krkr2-compatible, so the krkrsdl2 route can be
validated against it now.

### On nihui/nikki

`nikki` provides Vulkan-on-WebGPU. It is **not needed here**: KrKr2-Next renders GLES 2.0 through
ANGLE, and GLES 2.0 maps onto Emscripten's WebGL2 directly. It stays a fallback for a kernel that
genuinely insists on Vulkan.

## The krkrsdl2 port

### What works

- Builds via CMake + `emcmake`; the repository's own `meson.build` cannot configure at all (`ERROR:
  Double quotes are not supported` around its own deprecation notice), so CMake is the supported path.
- **Threads on**, which the ranged read requires: glue grew 560,592 → 634,392 B and contains PThread
  code. Upstream enables threads with the deprecated `-sUSE_PTHREADS=1`, which Emscripten 6.x accepts
  **silently while producing a single-threaded build**; `-pthread` is the current spelling.
- The stock engine renders the KAG3 demo: canvas 2560x1398, Japanese text correct.
- The dev server answers ranges with `206` against a real XP3, verified byte-for-byte.
- `launcher.html` takes `?archive=<url>` and does **not** write the archive into the Emscripten
  filesystem — the behaviour that made a multi-gigabyte XP3 unopenable at 128 MB initial memory.

### The seam, confirmed statically

The engine opens files in **exactly one place**: `tTVPLocalFileStream`'s constructor at
`src/core/base/sdl2/StorageImpl.cpp:941`, `SDL_RWFromFile`. **Everything routes through
`TVPCreateStream`** in `base/StorageIntf.cpp`, which is platform-independent; there is no `fopen`,
`ifstream` or `SDL_RWFromFile` in the platform-independent base layer. And `XP3Archive.cpp` already
treats the archive as a **seekable stream** (`:270, :340, :554`), never assuming it is resident.

So ranged loading is a change to `Seek` and `Read` in that one class, covering the archive and every
other game file at once — and it settles that `packages/vfs/src/archives/xp3.ts` is needed only for
the **library UI**, not for playback, so the `0x80` redirect and 5 skipped tests are off the critical
path for running a game.

### Why the ranged read must block, and what that costs

`tTJSBinaryStream::Read`/`Seek` are blocking by contract and are called from inside the script
interpreter, where nothing can be awaited. So the ranged read blocks on synchronous XHR — forbidden
on the main thread for an `arraybuffer` response
(`InvalidAccessError: ... synchronous requests made from a document`), legal in a worker. Hence
`PROXY_TO_PTHREAD`. `SDLApplication.cpp` branches on `__EMSCRIPTEN_PTHREADS__` in eight places, so a
threaded build is supported rather than novel.

### Where it stopped

Everything is compiled and linked, and **range requests are zero**:

```
krkr_try_ranged_stream   in wasm          krkr_js_archive_info   in wasm and glue
krkr_js_range_read       in glue          __krkrRangeRead        installed
```

**The defect that made it dead code once, now fixed:** a first version declared and implemented
`tTVPHttpRangeStream` and **never constructed it**, so the build succeeded, linked, and did nothing.
The selection now lives in `_TVPCreateStream`, **before** `TVPGetPlacedPath` — which performs a real
filesystem lookup and would throw for an archive that is deliberately not in the filesystem.

**Two candidate causes remain, neither distinguished:**

1. The page never reaches the storage call, because startup fails earlier.
2. `Module.__krkrArchiveSize` returns ≤ 0; its `HEAD` for `Content-Length` is the one step not yet
   observed.

The server now logs **all** requests rather than only ranged ones, precisely because logging only
ranges made those identical — both produced a count of zero. **A `HEAD` with no following `GET`**
means the probe ran and was rejected; **no requests at all** means the page did not load.

**Most likely, and unchecked:** `PROXY_TO_PTHREAD` runs the engine in a worker, where the global
object is the `WorkerGlobalScope`. A global set on the page as `self.__krkrArchiveUrl` does not exist
on the thread the engine runs on. The bridge has since been changed to read `Module.__krkrArchiveUrl`
first (`Module` is shared with the worker) and the launcher sets both, but **that change has not yet
been observed to produce a range request** — the last test predates it reaching a rebuilt page.

## Problems found, recorded because each cost real time

They share a shape: **the failure pointed somewhere other than its cause.**

| Problem | Symptom | Cause |
| --- | --- | --- |
| `data` parameter rejected | stuck "Downloading...", 404, canvas at 300x150 — reads like an engine failure | the generated page validates `?data=` against `/^[0-9a-z._-]+$/i` and throws on any `/` |
| `MODULARIZE=1` | no error, one log line, blank canvas | loading `krkrsdl2.js` defines a factory and starts nothing; `instantiate_krkrsdl2_module` must be called |
| IDBFS sync ordering | `Fatal Error: Use path delimiter '>' or '/'` | the `syncfs` restore must be gated on a run dependency, as the generated page does |
| C++ standard conflict | `bind.h` needs C++17, `tjsArray.cpp` needs C++14 (`std::binary_function`, removed in 17) | one file gets `-std=c++17` **and** `SKIP_PRECOMPILE_HEADERS`, because the PCH is built at the target's standard |
| `EXPORTED_FUNCTIONS` | `callMain` vanished from the glue | a later `-sEXPORTED_FUNCTIONS=` **replaces** rather than merges; upstream's list must be repeated |
| `EM_JS` JavaScript | `expected ')'` at the `EM_JS` line, macro "undefined" — points at the preprocessor | `EM_JS` holds JS in a C string, and the patcher's template literal is another layer: **an apostrophe or backtick in a comment terminates one of them** |
| `--pre-js` for a symbol | `undefined symbol: krkr_js_range_read` | a pre-js runs in a different scope; symbols come from **`--js-library`** |
| js-library top-level assignment | `Internal compiler error JS compiler` | assignments belong inside the library object; `$krkrInstallHelpers` |
| target-property `--js-library` | symbol still undefined, indistinguishable from a wrong path | pass it via `-DCMAKE_EXE_LINKER_FLAGS` |
| patchers reverting each other | `anchor not found` from a patcher whose input was wiped | each patcher restored from git at start; the **driver** now restores once, before the sequence |
| banner split by insertion | `// tTVPLocalFileStream#if defined(...)` — swallowing the `#if` | an insertion walk was off by one line; insert at the constructor's own line |
| CRLF | a visibly present three-line anchor never matched | the anchor used `\n`; anchored on one line |
| submodule restore | `pathspec ... did not match any file(s) known to git` | `external/krkrz` is a submodule: `git -C external/krkrz` |

**The meta-lesson, from having repeated it:** when a class of failure recurs, suspect the **tool
choice**, not the current instance. The `EM_JS` quoting problem was diagnosed correctly by the second
failure and then worked around three more times instead of being removed.

## Deliverables

```
ports/krkr/krkrsdl2/
  run-emscripten-build.sh     build driver; KRKR_RANGED_STORAGE=1 enables the ranged read
  patch-cxx-standard.mjs      one file at C++17, PCH skipped for it
  patch-pthread.mjs           -pthread, which actually defines __EMSCRIPTEN_PTHREADS__
  patch-ranged-storage.mjs    ranged stream, selection in _TVPCreateStream, js-library
  patch-prejs.mjs             links krkr-range.js with --js-library
  launcher.html               takes ?archive=<url>, no copy into the web root
  serve.mjs                   port 4630; wasm MIME, COOP/COEP, ranges, request logging
docs/kirikiri-web-plan.md     this file
```

`.ref/krkrsdl2` and its `krkrz` submodule are returned to pristine at the end of each build, and the
patchers reapply from a pinned revision, so no state carries between runs. Submodules total ~700 MB.
`.ref/krkr2-next` is a metadata clone with a sparse checkout of `cpp/`, `bridge/`, `cmake/`,
`platforms/` (11.3 MB).

## Next

1. **Read the server log after loading the page.** One glance distinguishes "the page reached the
   archive probe" from "the page never loaded", and either way names the next fix. This is the
   cheapest possible step and it is the one that has not been taken.
2. If the probe runs and is rejected, check the `HEAD` response's `Content-Length` — the server
   answers it, and `tTVPHttpRangeStream` rejects a zero size.
3. Only then measure: range counts and bytes against the 17.5 MB demo, and against a real multi-GB
   archive, where the ONS port's ratio (0.56 MB of reads serving 1.8 GB) is the figure to compare.
