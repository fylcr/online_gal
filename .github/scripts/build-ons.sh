#!/usr/bin/env bash
# Build the ONS web engine (onsyuri.wasm + onsyuri.js + onsyuri.html) on a Linux CI runner.
#
# WHY THIS IS A SCRIPT AND NOT A `run:` BLOCK
#
# Everything here is engine-specific and non-obvious, and all of it had to be proved on a dev
# machine before it could be trusted in CI. A shell script can be run by hand against the same
# checkout; a multi-line `run:` block can only be run by pushing.
#
# WHAT THIS WORKFLOW DOES *NOT* DO
#
# Upstream's own `script/cross_web.sh` does the build, unchanged: this wrapper only puts the tree in
# the shape that script expects. That is deliberate -- the moment the port starts compiling the
# engine itself there are two build recipes to keep in step, and the upstream one is the only one
# that can track upstream.
#
# THE THREE THINGS THAT BREAK ON LINUX, AND WHY
#
#   1. `run-web-build-shimmed.sh` calls bare `python` in two places (it renders a relative path and
#      runs an inline patcher). Ubuntu images ship `python3` and often no `python`, so a `python ->
#      python3` shim goes on PATH ahead of everything else. Fixing it with a shim rather than by
#      editing the script is the point: that script is tracked and also drives the Windows build,
#      and it is out of this workflow's write scope.
#   2. The shimmed wrapper resolves the upstream checkout from `ports/env.sh`, which honours
#      `REPO`/`REF_DIR`/`EMSDK_DIR` from the environment. All three are set below so the runner uses
#      the CI checkout and NOT the `/d/emsdk` and `.ref/` paths a Windows dev box has.
#   3. `EMSDK_HOME` must be the emsdk ROOT (that is where `emsdk_env.sh` lives), while
#      `EMSDK_DIR` is what `ports/env.sh` reads. Both are exported.
#
# THE HOST SHIM
#
# The shimmed variant is used, not the plain one, because it force-includes `ons_host_shim.h` ahead
# of `SDL_WaitEvent`: without it the engine's main loop blocks in a synchronous wait that never
# returns under Emscripten, so the page loads and then shows a frozen first frame. This is the same
# artifact the local build produces and the one whose hashes are recorded in
# `ports/lib/engine-artifacts.json`.
set -eu

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

: "${UPSTREAM_PATH:=.ref/onscripter-yuri}"
: "${EMSDK_DIR:=${EMSDK:-$REPO_ROOT/.ref/emsdk}}"
export EMSDK_DIR
export REPO="$REPO_ROOT/$UPSTREAM_PATH"
export JOBS="${JOBS:-$(nproc 2>/dev/null || echo 4)}"
export BUILD_TYPE="${BUILD_TYPE:-Release}"

echo "## repo:        $REPO_ROOT"
echo "## upstream:    $REPO"
echo "## EMSDK_DIR:   $EMSDK_DIR"
echo "## jobs:        $JOBS"

[ -d "$REPO" ] || { echo "::error::no upstream checkout at $REPO"; exit 1; }
[ -x "$EMSDK_DIR/upstream/emscripten/emcc" ] || {
  echo "::error::no emcc at $EMSDK_DIR/upstream/emscripten/emcc"
  exit 1
}

# --- the `python` shim ------------------------------------------------------
#
# Prepended to PATH so `python` resolves for the duration of the build. A shim directory rather
# than a system-wide `update-alternatives` call, so nothing outside this job is touched.
SHIM_BIN="$(mktemp -d)"
ln -s "$(command -v python3)" "$SHIM_BIN/python"
export PATH="$SHIM_BIN:$PATH"
echo "## python:      $(command -v python) -> $(python --version 2>&1)"

# --- build ------------------------------------------------------------------
#
# `run-web-build-shimmed.sh` patches tracked upstream files and restores them from git on exit, so
# the checkout is left pristine and a re-run starts from a clean tree.
#
# It is invoked with NO arguments, deliberately. The wrapper forwards its arguments to upstream's
# `script/cross_web.sh`, where they become `TARGETS=$@` -- make targets, not flags. `BUILD_TYPE` and
# the core count are taken from the environment instead (`BUILD_TYPE` is exported above, and the
# wrapper rewrites `CORE_NUM=` from `JOBS`).
echo
echo "== building ONS web target =="
bash ports/ons/build/run-web-build-shimmed.sh

# --- collect ----------------------------------------------------------------
#
# The outputs land in `$REPO/build_web/` (`cross_web.sh` sets `BUILD_PATH=$(pwd)/../build_web` from
# the `script/` directory). They are copied into `dist/` because the artifact path the reusable
# workflow is given must not depend on where an upstream script decided to write.
OUT_DIR="$REPO_ROOT/dist/ons"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

for name in onsyuri.wasm onsyuri.js onsyuri.html; do
  src="$REPO/build_web/$name"
  if [ ! -s "$src" ]; then
    echo "::error::missing build output $src"
    echo "## build_web contains:"
    ls -la "$REPO/build_web" 2>&1 | head -30 || true
    exit 1
  fi
  cp "$src" "$OUT_DIR/$name"
done

echo
echo "== artifacts =="
ls -la "$OUT_DIR"
