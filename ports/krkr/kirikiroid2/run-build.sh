# Builds kirikiroid2-web for the browser, following the upstream CI recipe.
#
# The order is from `.github/workflows/build-web.yml` on the `web` branch, and the sequencing matters:
# the Emscripten ports must be prewarmed **before** CMake configures, because vcpkg compiles against
# those headers. Doing it the other way round produces failures that look like missing headers rather
# than a step out of order.
#
# Prerequisites, all verified present on this machine:
#
#   emcc 6.0.9      the version CMakeLists.txt enforces with an exact version check
#   vcpkg           .ref/vcpkg at the CI's pinned commit b1e15efe, bootstrapped
#   cmake, ninja    ninja.exe comes from the pip install and needs its Scripts dir on PATH
#   win_bison        WinFlexBison, which the CMake finds as win_bison.exe
#
# `win_bison` is also exposed as `bison`: the CMake looks for both names, but `bison` is the one a
# bare `bison` invocation would need, and a link is cheaper than remembering the prefix.
set -u

# Repo root, upstream checkouts and `emcc` all come from the shared environment. See `ports/env.sh`.
. "$(dirname "$0")/../../env.sh"

REPO="${REPO:-$REF_DIR/kirikiroid2-web}"
VCPKG_DIR="${VCPKG_DIR:-$REF_DIR/vcpkg}"
OVERLAY_PORTS_DIR="${OVERLAY_PORTS_DIR:-$PORTS_ROOT/ports/krkr/kirikiroid2/overlay-ports}"
PATCHER_DIR="$PORTS_ROOT/ports/krkr/kirikiroid2"
# `$USER` is not set in this Git Bash, and `set -u` turns that into an immediate failure — so the
# account name is taken from the environment variables Windows does provide, with a fallback.
WIN_USER="${USERNAME:-${USER:-ASUS}}"
WIN_BISON_DIR="/c/Users/$WIN_USER/AppData/Local/Microsoft/WinGet/Links"
NINJA_DIR="/c/Users/$WIN_USER/AppData/Roaming/Python/Python314/Scripts"

export EMSDK="$EMSDK_DIR"
# `EM_CONFIG` is deliberately NOT set here; see the note at the prewarm below, and `ports/env.sh`.
export VCPKG_ROOT="$VCPKG_DIR"
export EMSDK_NODE="$(ls "$EMSDK_DIR"/node/*/node.exe 2>/dev/null | head -1)"

# MinGW on PATH, because vcpkg must build **host tools natively**.
#
# This is the resolution of the whole `x64-windows` family of failures. vcpkg builds host tools —
# `pkgconf` and friends — for a host triplet that is separate from the target. Setting the host
# triplet to the wasm triplet, which was the earlier workaround, made those tools unbuildable as
# executables and therefore unrunnable: the install tree ended up with `pkgconf.js` and
# `pkgconf.wasm`, and ffmpeg's portfile then died trying to *execute* pkg-config:
#
#   execute_process failed command indexes:
#     1: "Abnormal exit with child return code: no such file or directory"
#
# The correct arrangement is a **native** host triplet. `x64-windows` needs Visual Studio, which is
# not completely installed here (`D:\programe\msvc` exists but has no
# `VC/Auxiliary/Build/vcvarsall.bat`), so MinGW is used instead: `g++` is present at
# `D:\programe\MinGW64\bin`, and vcpkg ships the `x64-mingw-static` community triplet.
MINGW_BIN=/d/programe/MinGW64/bin
export VCPKG_HOST_TRIPLET=x64-mingw-static
echo "## host triplet: $VCPKG_HOST_TRIPLET"

# Git's `usr/bin` goes **ahead of** MinGW's, because both ship a `patch` and only one is usable.
#
# vcpkg's bundled meson probes `patch --version` while importing and aborts if the call fails:
#
#   mesonbuild.utils.core.MesonException: Command
#   'D:\programe\MinGW64\bin\patch.EXE' --version' failed with status 1.
#
# MinGW's `patch.exe` is **BusyBox** (v1.38.0), which has no `--version` and exits non-zero. Git's is
# **GNU patch 2.7.6**, which satisfies the `>=2.6.1` check meson performs. Order is the entire fix:
# with MinGW first — which is what a plain `PATH="$MINGW_BIN:$PATH"` produces — the wrong `patch` wins
# and the host-tool build of `pkgconf` dies before anything downstream can be attempted.
export PATH="/usr/bin:$MINGW_BIN:$PATH"
echo "## g++:          $(command -v g++ || echo MISSING)"
echo "## patch:        $(command -v patch || echo MISSING) - $(patch --version 2>&1 | head -1)"

# Windows-form paths for the two variables the **preset** and **vcpkg** consume.
#
# The preset expands
# `"toolchainFile": "$env{EMSDK}/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"`, and
# `CMakePresets.json` is parsed by CMake itself on Windows, which does not understand a Git Bash
# `/d/emsdk` path — so the Emscripten toolchain would silently fail to load and `CMakeLists.txt`
# would fall to its `else()` branch, which installs vcpkg's toolchain without setting the triplet.
EMSDK_DIR_WIN="$(ports_win "$EMSDK_DIR")"
VCPKG_DIR_WIN="$(ports_win "$VCPKG_DIR")"
export EMSDK_PYTHON="$EMSDK_DIR/python/3.13.3_64bit/python.exe"

echo "## EMSDK:        $EMSDK"
echo "## EMSDK(win):   $EMSDK_DIR_WIN"
echo "## VCPKG_ROOT:   $VCPKG_ROOT"
echo "## VCPKG_ROOTwin:$VCPKG_DIR_WIN"
echo "## preset toolchain file exists: $([ -f "$EMSDK_DIR/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" ] && echo yes || echo NO)"

# Emscripten's own directories first, then the two Windows tools whose locations are not conventional.
export PATH="$EMSDK_DIR/upstream/emscripten:$EMSDK_DIR/upstream/bin:$EMSDK_DIR:$WIN_BISON_DIR:$NINJA_DIR:$PATH"

echo "## emcc:      $(command -v emcc || echo MISSING)"
emcc --version 2>&1 | head -1
echo "## bison:     $(command -v bison || command -v win_bison || echo MISSING)"
echo "## ninja:     $(command -v ninja || echo MISSING)"
echo "## vcpkg:     $([ -x "$VCPKG_DIR/vcpkg.exe" ] && echo present || echo MISSING)"

# vcpkg must be bootstrapped, and it must **not** be a shallow clone.
#
# Both were learned the hard way. A shallow clone fails during version resolution because vcpkg reads
# historical tree objects for the pinned ports:
#
#   error: "git" ... read-tree 718d272c... failed with exit code 128
#   note: vcpkg was cloned as a shallow repository. Try again with a full vcpkg clone.
#
# The bootstrap is run here rather than assumed, so a fresh checkout of this repository's tooling
# works without an undocumented manual step.
if [ ! -x "$VCPKG_DIR/vcpkg.exe" ]; then
  echo "## bootstrapping vcpkg"
  ( cd "$VCPKG_DIR" && cmd //c bootstrap-vcpkg.bat -disableMetrics 2>&1 | tail -3 )
fi
if [ -f "$VCPKG_DIR/.git/shallow" ]; then
  echo "## WARNING: vcpkg is a shallow clone; version resolution will fail." >&2
  echo "##          re-clone it in full: git clone https://github.com/microsoft/vcpkg.git" >&2
  exit 1
fi
echo "## EMSDK_NODE: $EMSDK_NODE"
echo "## source:    $REPO"
echo

cd "$REPO" || exit 1

# --- vcpkg port fixes: none needed here --------------------------------------
#
# Both port problems this build used to hit are solved by mechanisms **other than** editing vcpkg's
# own port tree, and that is not an accident — writing to `.ref/vcpkg/ports/` does not work for
# builtin-registry ports:
#
# > vcpkg's builtin registry reads port files from the **git baseline commit's object store**, not
# > from the working tree. Verified for `pkgconf`: `buildtrees/pkgconf/x64-mingw-static.vcpkg_abi_info.txt`
# > records `portfile.cmake 50e854f5…`, which is the sha256 of `git cat-file blob HEAD:ports/pkgconf/portfile.cmake`
# > — while the edited working-tree file hashed to `bd8b9423…`. The edit was never read.
#
# So the two fixes live where vcpkg actually looks:
#
#   1. `pkgconf`'s tool-copy step — **no fix needed**. It failed because the host triplet had been set
#      to `wasm32-emscripten`, so the tools were built as `bomtool.js`/`pkgconf.js`, which
#      `vcpkg_copy_tools` cannot place. With a real native host triplet (`x64-mingw-static`) they are
#      built as executables and the copy succeeds. The earlier patch attempt for this was **dead
#      code** and has been removed.
#
#   2. `gettext`'s `localtime.c` GCC-15 error — an **overlay port**
#      (`overlay-ports/gettext/fix-mingw-gcc15-localtime-env-loop.patch`). Overlay ports are read from
#      the **filesystem**, so this one is effective. It is installed by the overlay-copy loop further
#      down, which is why nothing is applied at this point.
echo "== vcpkg port fixes: none at this stage (overlays applied below) =="
echo

# `bison` as a name, not `win_bison`: the CMake accepts either, but a plain `bison` call in any other
# context would otherwise be unresolved. Removed on exit so the checkout is not quietly modified.
BISON_SHIM="$(dirname "$WIN_BISON_DIR")/bison"
cleanup() {
  [ -n "${BISON_SHIM_CREATED:-}" ] && rm -f "$BISON_SHIM" "$BISON_SHIM.exe"
  return 0
}
trap cleanup EXIT

if ! command -v bison >/dev/null 2>&1 && [ -x "$WIN_BISON_DIR/win_bison.exe" ]; then
  cp "$WIN_BISON_DIR/win_bison.exe" "$BISON_SHIM.exe" 2>/dev/null && BISON_SHIM_CREATED=1
  export PATH="$(dirname "$BISON_SHIM"):$PATH"
  echo "## exposed win_bison as bison for this run"
fi

# WinFlexBison resolves its skeleton and m4 data relative to the directory of the *invoked* executable.
# WinGet exposes it as a symlink under `...\WinGet\Links`, so CMake invokes
# `...\WinGet\Links\win_bison.exe` and bison then looks for `...\WinGet\Links\data\m4sugar\m4sugar.m4`,
# which does not exist. Every bison step then fails during the build with:
#
#   win_bison.exe: C:\...\WinGet\Links\data/m4sugar/m4sugar.m4: cannot open: No such file or directory
#
# The real package directory does contain `data/`, and pointing BISON_PKGDATADIR at it fixes the lookup
# (verified: `win_bison <tjs.y>` then exits 0 and emits tjs.tab.cpp / tjs.tab.hpp). The value must be a
# Windows-form path because win_bison.exe is a native binary: an MSYS `/c/...` path is not resolvable
# by it, hence `cygpath -m`.
WIN_BISON_DATA=""
for _candidate in /c/Users/"$WIN_USER"/AppData/Local/Microsoft/WinGet/Packages/WinFlexBison*/data; do
  [ -d "$_candidate" ] && WIN_BISON_DATA="$_candidate" && break
done
if [ -z "$WIN_BISON_DATA" ]; then
  _real="$(readlink -f "$WIN_BISON_DIR/win_bison.exe" 2>/dev/null || true)"
  [ -n "$_real" ] && [ -d "$(dirname "$_real")/data" ] && WIN_BISON_DATA="$(dirname "$_real")/data"
fi
if [ -n "$WIN_BISON_DATA" ]; then
  export BISON_PKGDATADIR="$(cygpath -m "$WIN_BISON_DATA")"
  echo "## BISON_PKGDATADIR: $BISON_PKGDATADIR"
else
  echo "## WARNING: could not locate WinFlexBison's data directory; bison steps will fail" >&2
fi

# `embuilder` refuses to run from an unactivated SDK, so the SDK's env script is sourced when present.
if [ -f "$EMSDK_DIR/emsdk_env.sh" ]; then
  # shellcheck disable=SC1091
  . "$EMSDK_DIR/emsdk_env.sh" >/dev/null 2>&1 || true
fi

# Pin ONE spelling of the SDK path for every emcc/embuilder invocation in this script.
#
# `tools/shared.py` computes its cache sanity string as `generate_sanity()` = "<version>|<LLVM_ROOT>",
# and `LLVM_ROOT` is expanded from `$CFGDIR` at load time. When the stored `cache/sanity.txt` differs
# from that value, `check_sanity()` does not warn and continue — it calls `cache.erase()`
# (tools/shared.py:430-435, "(Emscripten: config changed, clearing cache)") and deletes the entire
# cache, `cache/sysroot` and every port library in it.
#
# Measured on this machine, with the SDK at `D:\emsdk`:
#
#   EMSDK=D:/emsdk, EM_CONFIG unset              -> 6.0.9|D:\emsdk/upstream/bin   (cache survives)
#   EMSDK=D:/emsdk, EM_CONFIG=D:/emsdk/.emscripten -> 6.0.9|D:/emsdk/upstream/bin (cache WIPED)
#   EMSDK=/d/emsdk, EM_CONFIG unset              -> 6.0.9|D:\emsdk/upstream/bin   (cache survives)
#
# So it is **setting `EM_CONFIG` at all** that flips the slash direction — not the API form of
# `EMSDK`. Emscripten locates `$EMSDK_DIR/.emscripten` by itself, so nothing is lost by leaving the
# variable unset. Earlier this script exported it twice (POSIX form for the prewarm, Windows form for
# CMake); both spellings produced the wiped string, so each phase erased the other's cache: the port
# libraries the prewarm had just built vanished before the build ran, the build then had to rebuild
# ports mid-compile, and that nested port build tripped emscripten's cache-lock assertion
# ("attempt to lock the cache while a parent process is holding the lock").
#
# The visible symptom is not the wipe but its consequence: CMake failing with
# `Could NOT find ZLIB (missing: ZLIB_LIBRARY ZLIB_INCLUDE_DIR)`.
export EMSDK="$EMSDK_DIR_WIN"

# `tools/ports/__init__.py:447` writes each port's `<sysroot>/lib/pkgconfig/<name>.pc` without creating
# the directory first (`Ports.write_file(filename, ...)`), and the directory is only ever created as a
# side effect of the sysroot header install (`tools/system_libs.py:2593`). On a cache that has just
# been erased, ports are built before that install runs, so every port build aborts *after* producing
# its library and the library is discarded:
#   FileNotFoundError: ... cache\sysroot\lib\pkgconfig\zlib.pc
# Recreating it from emscripten's own built-in pkgconfig sources removes that failure.
EM_SYSROOT_PC="$EMSDK_DIR_WIN/upstream/emscripten/cache/sysroot/lib/pkgconfig"
mkdir -p "$EM_SYSROOT_PC"
cp -f "$EMSDK_DIR_WIN"/upstream/emscripten/system/lib/pkgconfig/*.pc "$EM_SYSROOT_PC"/ 2>/dev/null || true
echo "## emscripten pkgconfig dir: $EM_SYSROOT_PC ($(ls -1 "$EM_SYSROOT_PC" 2>/dev/null | wc -l) entries)"

echo "== prewarming Emscripten ports =="

# Emscripten's own port scripts must be fixed first, because the prewarm builds the port variants that
# hit the bug.
#
# `tools/ports/libpng.py` (and `freetype.py`) interpolate a Python value straight into a compiler flag:
#
#   flags.append(f'-sWASM_LEGACY_EXCEPTIONS={settings.WASM_LEGACY_EXCEPTIONS}')
#
# When that setting is the boolean `True` it emits `...=True`, and `emcc` rejects it:
#
#   emcc: error: attempt to set `WASM_LEGACY_EXCEPTIONS` to `True`; use 1/0 to set boolean settings
#
# The `-legacysjlj` variants below are exactly the ones that set it, so without this the prewarm fails.
# Reapplied on every run because it edits the **toolchain**, which lives outside this repository — a
# fresh emsdk install would otherwise reintroduce the failure.
node "$PATCHER_DIR/patch-emsdk-port-flags.mjs" || exit 1

# The explicit `-mt` and `-legacysjlj` variants are required: as the CI notes, the 6.0.9 libpng and
# FreeType ports otherwise pass `True` as a boolean flag and fail.
for port in \
  "libpng libpng-mt libpng-legacysjlj libpng-mt-legacysjlj" \
  "freetype freetype-legacysjlj" \
  "harfbuzz harfbuzz-mt" \
  "sdl2 sdl2-mt" \
  "sdl2_ttf sdl2_ttf-mt" ; do
  echo "  embuilder: $port"
  embuilder build $port 2>&1 | tail -20
done
echo "## prewarm done"
echo

echo "== cmake configure =="
# `CMAKE_MAKE_PROGRAM` names ninja explicitly.
#
# Passing it through the environment was not enough: CMake reported "unable to find a build program
# corresponding to Ninja" while `ninja` was resolvable on PATH, because the preset selects Ninja and
# CMake resolves the generator before consulting how the process was launched. An absolute path
# removes the question.
# `-DEMSCRIPTEN=1` is what selects the Web branch, and it is passed explicitly.
#
# `CMakeLists.txt` decides the target platform at line 24:
#
#   elseif(VCPKG_TARGET_TRIPLET STREQUAL "wasm32-emscripten" OR EMSCRIPTEN)
#       set(VCPKG_TARGET_TRIPLET "wasm32-emscripten")
#       ...
#   else()
#       set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)   # no triplet!
#   endif()
#
# On a cold configure neither condition held: the preset's `VCPKG_TARGET_TRIPLET` is a cache variable
# that is not yet visible when this line is evaluated, and `EMSCRIPTEN` is normally set by the
# Emscripten toolchain, which line 31 replaces with vcpkg's during the same run. Execution therefore
# reached `else()`, which installs vcpkg's toolchain **without setting the triplet at all** — and vcpkg
# fell back to its host default, `x64-windows`, then failed looking for Visual Studio.
#
# Passing `-DEMSCRIPTEN=1` makes the branch deterministic instead of dependent on ordering, and it also
# restores the `VCPKG_CHAINLOAD_TOOLCHAIN_FILE` that the branch sets.
#
# CI does not hit this because it caches `.emsdk` and ccache, so its configure is warm; this is a first
# configure.
NINJA_EXE="$NINJA_DIR/ninja.exe"
echo "## ninja:     $NINJA_EXE ($([ -x "$NINJA_EXE" ] && echo present || echo MISSING))"

# Overlay ports are the only mechanism that actually reaches vcpkg here, because the
# builtin registry serves port files from the git object database at the pinned baseline
# commit (b1e15efe), not from this working tree — a `git checkout` of these gettext edits
# is therefore inert: the ABI-recorded portfile.cmake hash equals `git cat-file blob
# HEAD:ports/gettext/portfile.cmake` (the pre-edit value), and the new .patch file is not
# an ABI input at all. `VCPKG_OVERLAY_PORTS` as an environment variable is *not* read by
# vcpkg.cmake either (it only honours the CMake variable / -D cache entry, vcpkg.cmake
# lines 112, 496-500); the effective wiring is the `overlay-ports` entry in this repo's
# `$REPO/vcpkg-configuration.json`. So the patched port must be copied into `$REPO/vcpkg/ports`
# (the directory already declared as the overlay) at runtime, never left as an edit under
# `$VCPKG_DIR/ports`.
for _overlay_port in "$OVERLAY_PORTS_DIR"/*/; do
    [ -d "$_overlay_port" ] || continue
    _overlay_name="$(basename "$_overlay_port")"
    rm -rf "$REPO/vcpkg/ports/$_overlay_name"
    cp -r "$_overlay_port" "$REPO/vcpkg/ports/$_overlay_name" || exit 1
    echo "## overlay port: $_overlay_name -> $REPO/vcpkg/ports/$_overlay_name"
done

# vlfs.js 的取数来源接缝：幂等补丁——脚本自己先 `git checkout` 还原
# platforms/web/vlfs.js 再打接缝，所以重复执行是安全的。接缝让嵌入页面可以用平台
# 自己的 @online-gal/vfs 供数（按需加载）；页面不装宿主对象时行为与参考实现一致。
VLFS_PATCHER="$PATCHER_DIR/patch-vlfs-host-source.mjs"
NODE_BIN="$(command -v node || true)"
[ -n "$NODE_BIN" ] || NODE_BIN="$EMSDK_NODE"
"$NODE_BIN" "$VLFS_PATCHER" || exit 1
echo "## vlfs host byte-source seam: patched"

# The target triplet is set here, in the environment, as well as on the CMake command line.
#
# `CMakeLists.txt` does set `VCPKG_TARGET_TRIPLET` for wasm32-emscripten, but vcpkg runs during
# `project()` — via `CMakeDetermineSystem` — and its **manifest install happens before that line
# executes**. Without this it defaulted to `x64-windows` and failed looking for Visual Studio:
#
#   error: in triplet x64-windows: Unable to find a valid Visual Studio instance
#
# Setting it in the environment means vcpkg sees the right triplet from its first invocation.
export VCPKG_TARGET_TRIPLET=wasm32-emscripten
export VCPKG_OVERLAY_TRIPLETS="$REPO/vcpkg/triplets"
# NOTE: `VCPKG_OVERLAY_PORTS` as an env var is NOT read by vcpkg.cmake (it only honours
# the CMake variable / -D cache entry, vcpkg.cmake lines 112, 496-500). The effective
# wiring is `overlay-ports` in `$REPO/vcpkg-configuration.json`, which is why the patched
# gettext port is copied into `$REPO/vcpkg/ports` above.
export VCPKG_OVERLAY_PORTS="$REPO/vcpkg/ports"
# `VCPKG_HOST_TRIPLET` is the variable that was actually missing, and it is distinct from the target
# triplet in a way that made this hard to see.
#
# From `vcpkg/scripts/buildsystems/vcpkg.cmake`:
#
#   line 492-493:
#     if(DEFINED VCPKG_HOST_TRIPLET AND NOT VCPKG_HOST_TRIPLET STREQUAL "")
#         list(APPEND Z_VCPKG_ADDITIONAL_MANIFEST_PARAMS "--host-triplet=${VCPKG_HOST_TRIPLET}")
#
# vcpkg builds **host tools** for a triplet of its own choosing, separate from `--triplet`. It derives
# that from `CMAKE_HOST_SYSTEM_PROCESSOR` (line 314 onward), which on this machine yields
# `x64-windows` — and `x64-windows`'s toolchain then requires a Visual Studio instance, which is the
# error. On Linux the same derivation yields `x64-linux`, which needs nothing, so CI never saw it.
#
# This is why the earlier attempts failed even though they fixed the target side: `CMakeCache.txt`
# recorded `VCPKG_TARGET_TRIPLET:STRING=wasm32-emscripten` while vcpkg still reported
# `in triplet x64-windows`. The symptom named a triplet that was never the target one.
#
# `VCPKG_DEFAULT_TRIPLET` does not help — it sets the *target* default, not the host one.
echo "## triplet:   target=wasm32-emscripten host=x64-mingw-static (both passed to cmake)"

# CI retries this three times to ride out transient source downloads inside vcpkg.
STATUS=1
for attempt in 1 2 3; do
  echo "  attempt $attempt/3"
  # The Windows-form variables are set only for this invocation, so `embuilder` earlier in the script
  # still sees the Git Bash paths it needs.
  EMSDK="$EMSDK_DIR_WIN" \
  VCPKG_ROOT="$VCPKG_DIR_WIN" \
  cmake --preset "Web Release Config" \
    -DCMAKE_MAKE_PROGRAM="$NINJA_EXE" \
    -DEMSCRIPTEN=1 \
    -DVCPKG_TARGET_TRIPLET=wasm32-emscripten \
    -DVCPKG_HOST_TRIPLET=x64-mingw-static > /tmp/krkr2-configure.log 2>&1
  STATUS=$?
  [ "$STATUS" -eq 0 ] && break
  echo "  configure failed; tail:"
  tail -12 /tmp/krkr2-configure.log
done
echo "## cmake configure exit: $STATUS"
[ "$STATUS" -eq 0 ] || exit "$STATUS"

echo
echo "== cmake build =="
cmake --build out/web/release > /tmp/krkr2-build.log 2>&1
STATUS=$?
echo "## cmake build exit: $STATUS"
[ "$STATUS" -eq 0 ] || { tail -20 /tmp/krkr2-build.log; exit "$STATUS"; }

echo
echo "== artefacts =="
find out/web/release -maxdepth 1 -type f -printf '  %10s  %f\n' 2>/dev/null | sort -k2
exit 0
