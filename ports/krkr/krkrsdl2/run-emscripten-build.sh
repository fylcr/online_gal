# Builds krkrsdl2 for the web with CMake and Emscripten.
#
# **CMake, not Meson.** The repository's own `meson.build` opens with
# `message("The Meson build system is deprecated; please use the CMake build system
# instead")`, and that message is not merely advisory: as written it uses double quotes,
# which Meson rejects outright (`ERROR: Double quotes are not supported`). So the Meson
# route cannot even configure on a current Meson. CMake is the supported path.
#
# `emcc.exe` lives in the Emscripten root rather than `upstream/bin`, so both directories go
# on PATH; adding only the conventional one leaves `emcc` unresolvable.
set -u

# Repo root, upstream checkout and `emcc` all come from the shared environment. See `ports/env.sh`.
. "$(dirname "$0")/../../env.sh"

REPO="${REPO:-$REF_DIR/krkrsdl2}"
BUILDDIR="${BUILDDIR:-$REPO/build_web}"

echo "## emcc:      $(command -v emcc || echo MISSING)"
emcc --version 2>&1 | head -1
echo "## cmake:     $(cmake --version 2>&1 | head -1)"
echo "## build dir: $BUILDDIR"
echo

cd "$REPO" || exit 1

# Reconfigure from scratch: a partially configured build directory reports success on the
# next run while still missing whatever the earlier failure skipped.
if [ -d "$BUILDDIR" ]; then
  echo "## removing previous build dir"
  rm -rf "$BUILDDIR"
fi
mkdir -p "$BUILDDIR"

# The Emscripten-only plugin `src/plugins/kremscripten.cpp` includes `<emscripten/bind.h>`,
# which needs C++17, while the project configures C++14 — so the stock tree cannot compile
# that translation unit. The standard is raised for Emscripten targets only; see the patch
# script for the reasoning. Applied after the directory reset so a failed run cannot leave the
# file patched twice.
# Every file the patchers touch is restored from git exactly once, here, before any of them run.
#
# This is the driver's job rather than each patcher's. When each patcher restored as well, the
# second one to run reverted the first one's edit and only the last patch survived — observed as
# `anchor not found` from a patcher whose input an earlier patcher had wiped. One restore, in one
# place, before the sequence starts.
#
# Restoring from git rather than from a copy is deliberate: a run killed before its cleanup leaves
# the files patched, and a copy-based backup would then be a patched file pretending to be the
# original — the mistake that broke the ONScripter overlay earlier.
echo "== restoring sources from git =="
git checkout -- CMakeLists.txt \
  src/core/base/sdl2/StorageImpl.h \
  src/core/base/sdl2/StorageImpl.cpp || {
  echo "## could not restore patched sources; they may not be tracked" >&2
  exit 1
}

# `external/krkrz` is a **submodule**, so its files belong to a different repository and the
# checkout above cannot see them:
#   error: pathspec 'external/krkrz/base/StorageIntf.cpp' did not match any file(s) known to git
# Restored through the submodule itself instead.
git -C external/krkrz checkout -- base/StorageIntf.cpp || {
  echo "## could not restore external/krkrz/base/StorageIntf.cpp" >&2
  exit 1
}
echo

echo "== applying port fixes =="
PATCHER_DIR="$PORTS_ROOT/ports/krkr/krkrsdl2"
node "$PATCHER_DIR/patch-cxx-standard.mjs" || exit 1
node "$PATCHER_DIR/patch-pthread.mjs" || exit 1

# The ranged-read storage is **not applied by default yet**.
#
# It is written but does not compile, and the reason is an interaction that is easy to get wrong and
# was got wrong repeatedly: `EM_JS` holds JavaScript inside a C string literal, that literal is
# produced by a JavaScript template literal, and a stray apostrophe or backtick in a *comment* on
# either layer terminates it. The symptom is a syntax error at the `EM_JS` line with the macro
# apparently undefined, which points at the preprocessor rather than at the comment responsible.
#
# Gating it keeps the baseline buildable and the failure isolated, so the next session starts from a
# working build rather than a broken one. Set KRKR_RANGED_STORAGE=1 to include it.
if [ "${KRKR_RANGED_STORAGE:-0}" = "1" ]; then
  echo "## including the ranged-read storage (KRKR_RANGED_STORAGE=1)"
  # Writes krkr-range.js as well as patching the sources, and must run before CMake configures
  # because the link line names that file.
  node "$PATCHER_DIR/patch-ranged-storage.mjs" || exit 1
  node "$PATCHER_DIR/patch-prejs.mjs" || exit 1
else
  echo "## ranged-read storage NOT applied (set KRKR_RANGED_STORAGE=1 to include it)"
fi
echo

# `emcmake` is the supported entry point: it sets the system name and toolchain for CMake so
# the Emscripten platform module is selected without naming it by hand.
#
# Threads are **on**, and that is a requirement rather than a tuning choice: the ranged-read
# stream blocks on synchronous XHR, which is forbidden on the main thread for an arraybuffer
# response. `PROXY_TO_PTHREAD` puts `main` on a pthread where it is legal. The ranged path is
# compiled out without it, so leaving threads off would silently restore whole-file loading.
echo "== cmake configure =="
#
# `--js-library` is passed here rather than through a target property in CMakeLists.txt.
#
# The property approach was tried and `wasm-ld` still reported the symbol undefined, which is the
# kind of failure that is hard to tell apart from a wrong file path. Passing it as an exe linker flag
# applies it to every link in the project, so there is nothing to get wrong about which target
# carries it.
RANGED_LINK_FLAGS=""
if [ "${KRKR_RANGED_STORAGE:-0}" = "1" ]; then
  RANGED_LINK_FLAGS="--js-library=$REPO/krkr-range.js"
fi

emcmake cmake -S "$REPO" -B "$BUILDDIR" \
  -DCMAKE_BUILD_TYPE=Release \
  -DKRKRSDL2_EMSCRIPTEN_ENABLE_THREADS=ON \
  -DCMAKE_EXE_LINKER_FLAGS="$RANGED_LINK_FLAGS" 2>&1 | tail -45
STATUS=${PIPESTATUS[0]}
echo "## cmake configure exit: $STATUS"
[ "$STATUS" -eq 0 ] || exit "$STATUS"

echo
echo "== cmake build =="
cmake --build "$BUILDDIR" --parallel 2>&1 | tail -35
STATUS=${PIPESTATUS[0]}
echo "## cmake build exit: $STATUS"

echo
echo "== artefacts =="
find "$BUILDDIR" -maxdepth 3 -type f \( -name '*.js' -o -name '*.wasm' -o -name '*.html' \) \
  -printf '  %10s  %p\n' 2>/dev/null | head -20
exit "$STATUS"
