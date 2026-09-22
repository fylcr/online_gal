#!/usr/bin/env bash
#
# Builds artemis-compat for the browser, in stages.
#
# ## Why Emscripten first, and not a native build
#
# The reference project has no WASM target: its three targets are the CLI (`artc`), an Android JNI
# library (`libartemis.so`) and a macOS Cocoa host (`artemis-mac`). So a web host has to be written.
#
# A native build would have been the cheaper sanity check, but it needs `find_package(ZLIB REQUIRED)`
# and this machine's MinGW has no zlib — whereas **Emscripten ships zlib as a port and the sysroot
# already has `zlib.h` / `libz.a`** (built earlier when libpng was prewarmed). So the web build is
# both the goal and the path of least resistance to a first compile.
#
# ## What the codebase gives us
#
# Three seams make this tractable rather than a rewrite:
#
#   * `artemis_core` builds **without GL** — that is what the CLI and the regression suite link. Only
#     `ARTC_HAS_GLES=1` pulls in the GLES2 compositor and layer shaders.
#   * `artemis-mac` is the template for a new host: recompile `${ARTC_CORE_SOURCES}` with a different
#     `main` and `ARTC_HAS_GLES=1`.
#   * `Pf8Reader::ReadAt(uint64_t offset, void *buf, size_t len)` is the single byte-read primitive,
#     which is where an on-demand byte source attaches later.
#
# ## Stages
#
#   core   - `artemis_core` only, no GL: proves the 30k lines compile under Emscripten at all.
#   cli    - add the CLI (`artc`), still GL-free.
#   cli-node - the CLI with `NODERAWFS`, so it can read host paths (used for the pack-layer probes).
#   web-android - the real host: the Android JNI activity + the host shim, compiled as an
#                 Emscripten pthread/GLES2 module. This is the one the browser actually runs.
set -u

# Repo root, upstream checkout and `emcc` all come from the shared environment. See `ports/env.sh`.
. "$(dirname "$0")/../../env.sh"

REPO="${REPO:-$REF_DIR/artemis-compat}"
BUILD_ROOT="${BUILD_ROOT:-$PORTS_ROOT/ports/artemis/build/out}"
STAGE="${1:-core}"

echo "## emcc:  $(command -v emcc || echo MISSING)"
emcc --version 2>&1 | head -1
echo "## stage: $STAGE"
echo "## repo:  $REPO"
echo

[ -d "$REPO/src" ] || { echo "## source tree missing; did the sparse checkout run?" >&2; exit 1; }

# `-DARTC_BUILD_JNI=OFF` matters even though it defaults off: this is an Android-only target and
# would look for EGL/GLESv2/OpenSLES.
#
# `-DARTC_ENABLE_FFMPEG=OFF` avoids a pkg-config hunt for FFmpeg, which is not present. The decoder
# compiles to a stub in that case, which is the documented behaviour rather than a workaround.
#
# `-DARTC_BUILD_MAC_HOST=OFF` is implied off on non-Apple hosts, but is passed explicitly so the
# intent is visible: a web host is going to replace it, not sit beside it.
#
# Warnings: the project builds with `-Wall -Wextra` and does not set `-Werror`, so warnings are
# expected and not fatal. Left enabled, because a wall of new warnings under Emscripten is itself
# useful signal about which code paths are being compiled differently.
BUILD=
TARGET=artemis_core
CLI=OFF
EXTRA_LINK=""
EXTRA_CMAKE=""

case "$STAGE" in
  core)
    BUILD="$BUILD_ROOT/core"
    ;;
  cli)
    # The CLI links the GL-free core, so it needs no GL: it is the fastest way to run the engine's
    # own stack (pack layer, Lua, .iet interpreter, script runner) without a renderer.
    TARGET=artc
    CLI=ON
    BUILD="$BUILD_ROOT/cli"
    ;;
  cli-node)
    # `NODERAWFS` makes Emscripten's filesystem pass straight through to Node's, so the wasm binary can
    # open **host** paths. Without it the module only sees MEMFS: `artc list root.pfs` would report a
    # missing file, which reads like a broken reader rather than a filesystem that is not mounted.
    #
    # The alternative, `--preload-file`, embeds the archive in the module — impossible here, since the
    # sample pack alone is 2.69 GB.
    #
    # Memory settings are not optional. Emscripten 6.x leaves `ALLOW_MEMORY_GROWTH` **off**, so the heap
    # is whatever `INITIAL_MEMORY` says (16 MB by default) and the first sizeable allocation aborts:
    #
    #   Aborted(OOM)
    #     at abortOnCannotGrowMemory (artc.cjs)
    #     at _emscripten_resize_heap
    #
    # That is exactly what happened booting the real game — the trace reached
    # `lyc {id=1.0.-1, file=:bg/black}` (the engine creating the first background layer, and therefore
    # loading a real asset) and then died. Growth is enabled with a sane initial size, and the ceiling
    # is left at wasm32's 4 GB.
    TARGET=artc
    CLI=ON
    BUILD="$BUILD_ROOT/cli-node"
    EXTRA_LINK="-sNODERAWFS=1 -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=64MB -sMAXIMUM_MEMORY=4GB"
    ;;
  web-android)
    # The real host: the upstream Android JNI entry point (`ANativeActivity_onCreate`) plus the
    # Android shim in `ports/artemis/build/android-shim/`, compiled with pthreads and GLES2.
    #
    # `ARTC_ANDROID_SHIM` points at the shim directory in the **repository**, not at `.test/`, which
    # is what makes this build reproducible from a clone. Everything else about the target
    # (`-sOFFSCREEN_FRAMEBUFFER`, the `--wrap` list, the exported functions) lives in the
    # `artemis-web-android` block of the engine's `CMakeLists.txt`, applied by
    # `ports/artemis/engine/0001-web-host.patch`.
    TARGET=artemis-web-android
    BUILD="$BUILD_ROOT/web-android"
    EXTRA_CMAKE="-DARTC_BUILD_WEB_ANDROID_HOST=ON -DARTC_ANDROID_SHIM=$(cygpath -m "$PORTS_ROOT/ports/artemis/build/android-shim" 2>/dev/null || echo "$PORTS_ROOT/ports/artemis/build/android-shim")"
    ;;
  *)
    echo "## unknown stage '$STAGE' (expected core, cli, cli-node or web-android)" >&2
    exit 2
    ;;
esac

mkdir -p "$BUILD"
emcmake cmake -S "$REPO" -B "$BUILD" \
  -DCMAKE_BUILD_TYPE=Release \
  -DARTC_BUILD_CLI=$CLI \
  -DARTC_BUILD_JNI=OFF \
  -DARTC_BUILD_TESTS=OFF \
  -DARTC_BUILD_MAC_HOST=OFF \
  -DARTC_ENABLE_FFMPEG=OFF \
  ${EXTRA_CMAKE} \
  ${EXTRA_LINK:+-DCMAKE_EXE_LINKER_FLAGS="$EXTRA_LINK"} \
  > /tmp/artemis-configure.log 2>&1
STATUS=$?
echo "## configure exit: $STATUS"
[ "$STATUS" -eq 0 ] || { echo "--- configure tail ---"; tail -25 /tmp/artemis-configure.log; exit "$STATUS"; }
echo "## target:  $TARGET   build dir: $BUILD   extra linker flags: ${EXTRA_LINK:-none}"

# zlib resolution is the one external dependency; report what CMake found so a failure here is
# identifiable rather than surfacing later as an undefined symbol.
grep -iE "^ZLIB|ZLIB_(LIBRARY|INCLUDE)" "$BUILD/CMakeCache.txt" 2>/dev/null | sed 's/^/  /' | head -5

echo
echo "== building $TARGET =="
cmake --build "$BUILD" --target "$TARGET" -j"$(nproc 2>/dev/null || echo 8)" \
  > /tmp/artemis-build.log 2>&1
STATUS=$?
echo "## build exit: $STATUS"
if [ "$STATUS" -ne 0 ]; then
  echo "--- first errors ---"
  grep -E "error:|Error " /tmp/artemis-build.log | head -20
  exit "$STATUS"
fi

echo
echo "== artefact =="
if [ "$TARGET" = "artc" ]; then
  ls -la "$BUILD"/artc.wasm "$BUILD"/artc.js 2>/dev/null
elif [ "$TARGET" = "artemis-web-android" ]; then
  ls -la "$BUILD"/artemis-web-android.js "$BUILD"/artemis-web-android.wasm 2>/dev/null
else
  ls -la "$BUILD"/libartemis_core.a 2>/dev/null ||
    find "$BUILD" -name "*.a" -maxdepth 2 -exec ls -la {} \; 2>/dev/null | head -5
fi
echo
echo "--- warning summary ---"
grep -c "warning:" /tmp/artemis-build.log 2>/dev/null | sed 's/^/  warnings: /'
grep -oE "warning: [^\[]*" /tmp/artemis-build.log 2>/dev/null | sort | uniq -c | sort -rn | head -8 | sed 's/^/  /'
