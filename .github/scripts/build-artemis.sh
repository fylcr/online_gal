#!/usr/bin/env bash
# Build the Artemis web host (artemis-web-android.wasm + .js) on a Linux CI runner.
#
# WHAT THIS BUILDS, AND WHAT IT DOES NOT
#
# This is the *third-party* port: `ports/artemis/build/build-emscripten.sh web-android` compiles the
# `artemis-compat` engine with a host layer transcribed from its Android `NativeActivity`. It is the
# artifact `ports/lib/fetch-engine.mjs` calls `artemis`, and the one a web deploy loads.
#
# It is NOT the official Artemis SDK build -- that is `official-web-deploy.py`, which deploys the
# proprietary binaries rather than compiling anything, and it is not an engine artifact.
#
# It is also NOT the rewrite bridge (`ports/artemis/src` + the wasm2c transpile), which is its own
# workflow. Those two produce different modules from different sources and only one of them is a
# drop-in for the official module; conflating them is how a "the bridge builds" run would get
# mistaken for "the host builds".
#
# THE PATCH IS THE BUILD
#
# `ports/artemis/engine/0001-web-host.patch` is where the entire browser host lives: `web_main.cpp`,
# the page, the worker, and the engine-side changes they need. Upstream `artemis-compat` has no web
# target, so without this patch the `web-android` stage has nothing to configure. Two consequences
# that make the CI version different from a dev machine:
#
#   * The patch must be applied to a clean checkout. A dev machine usually has it applied already,
#     so `build-emscripten.sh` "just works" there; on a fresh clone it has to be applied first, and
#     re-applying it over an already-patched tree fails -- hence the reverse-check.
#   * The patch adds files (`web_main.cpp`, `web_page.html`, `web_worker.js`) that `git apply
#     --reverse` cannot reach if they are absent, so the check below is on the whole patch and the
#     fallback is `ports/README.md`'s documented recipe: restore tracked files, delete the three
#     added files, apply forward.
set -eu

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

: "${UPSTREAM_PATH:=.ref/artemis-compat}"
: "${EMSDK_DIR:=${EMSDK:-$REPO_ROOT/.ref/emsdk}}"
export EMSDK_DIR
export REPO="$REPO_ROOT/$UPSTREAM_PATH"
PATCH="$REPO_ROOT/ports/artemis/engine/0001-web-host.patch"

echo "## repo:        $REPO_ROOT"
echo "## upstream:    $REPO"
echo "## EMSDK_DIR:   $EMSDK_DIR"
echo "## patch:       $PATCH"

[ -d "$REPO/src" ] || { echo "::error::no upstream checkout at $REPO"; exit 1; }
[ -f "$PATCH" ] || { echo "::error::missing patch $PATCH"; exit 1; }
[ -d "$REPO_ROOT/ports/artemis/build/android-shim" ] || {
  echo "::error::missing ports/artemis/build/android-shim (the web-android stage needs it)"
  exit 1
}

# --- bring the checkout to the patched state --------------------------------
if git -C "$REPO" apply --check --reverse "$PATCH" >/dev/null 2>&1; then
  echo "## patch already applied"
else
  echo "## applying the web-host patch"
  git -C "$REPO" checkout -- .
  rm -f "$REPO/src/host/web_main.cpp" "$REPO/src/host/web_page.html" "$REPO/src/host/web_worker.js"
  if ! git -C "$REPO" apply "$PATCH"; then
    echo "::error::the web-host patch does not apply to $UPSTREAM_PATH at this commit"
    echo "## upstream HEAD: $(git -C "$REPO" rev-parse HEAD)"
    exit 1
  fi
fi

# `--check --reverse` succeeding above is the assertion that the tree is in the state the build
# expects. Re-assert it here so a partially applied patch cannot slip through into a compile that
# then fails with a confusing missing-symbol error.
git -C "$REPO" apply --check --reverse "$PATCH" >/dev/null 2>&1 || {
  echo "::error::the checkout is not in the fully-patched state"
  exit 1
}

# --- prewarm the Emscripten ports this build needs ---------------------------
#
# `artemis-compat` does `find_package(ZLIB REQUIRED)` (CMakeLists.txt:36) against the **Emscripten
# sysroot**, and Emscripten does not ship a built zlib: it ships zlib as a *port* that `embuilder`
# has to build into `cache/sysroot/` first. Without that step CMake fails with
#
#     Could NOT find ZLIB (missing: ZLIB_LIBRARY ZLIB_INCLUDE_DIR)
#
# which is what this script hit on its first real run. A dev machine never notices, because the
# sysroot already has `zlib.h`/`libz.a` from an earlier `libpng` prewarm -- so the missing dependency
# only exists on a clean machine, which is to say only in CI. It is spelled out here rather than left
# to the caller for that reason: an engine build must not depend on another engine's side effects.
#
# `EMSDK`/`EMSDK_HOME` are exported alongside `EMSDK_DIR` because `embuilder` looks for its SDK
# through them, and the retry loop matches `build-krkr.sh`: the first run of a port downloads its
# sources from the emscripten-ports bucket, which is the flakiest network step in any of these builds.
export EMSDK="${EMSDK_DIR}"
export EMSDK_HOME="${EMSDK_DIR}"

echo
echo "== prewarming Emscripten ports (zlib) =="
prewarm_port() {
  local port="$1" attempt
  for attempt in 1 2 3; do
    if embuilder build "$port" >"/tmp/embuilder-$port.log" 2>&1; then
      echo "## embuilder $port ok"
      return 0
    fi
    echo "## embuilder $port attempt $attempt failed; tail:"
    tail -8 "/tmp/embuilder-$port.log" | sed 's/^/     /' || true
    sleep $((attempt * 5))
  done
  echo "::error::embuilder build $port failed after 3 attempts"
  return 1
}
prewarm_port zlib

[ -f "$EMSDK_DIR/upstream/emscripten/cache/sysroot/include/zlib.h" ] || {
  echo "::error::embuilder did not produce the zlib sysroot headers; find_package(ZLIB) will fail"
  exit 1
}

# --- build ------------------------------------------------------------------
#
# `build-emscripten.sh` sources `ports/env.sh`, which honours `EMSDK_DIR` and `REPO` from the
# environment, so the stage builds the CI checkout with the runner's SDK. The stage's output goes to
# `ports/artemis/build/out/web-android/`.
echo
echo "== building the Artemis web-android stage =="
bash ports/artemis/build/build-emscripten.sh web-android

SRC_DIR="$REPO_ROOT/ports/artemis/build/out/web-android"

# --- collect ----------------------------------------------------------------
#
# Copied into `dist/artemis/` because the artifact path belongs to the workflow, not to the build
# script's `BUILD_ROOT` default. Only the two module files are publishable; the `liblua51.a` and
# CMake droppings beside them are intermediates.
OUT_DIR="$REPO_ROOT/dist/artemis"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

for name in artemis-web-android.wasm artemis-web-android.js; do
  src="$SRC_DIR/$name"
  if [ ! -s "$src" ]; then
    echo "::error::missing build output $src"
    ls -la "$SRC_DIR" 2>&1 | head -30 || true
    exit 1
  fi
  cp "$src" "$OUT_DIR/$name"
done

echo
echo "== artifacts =="
ls -la "$OUT_DIR"
