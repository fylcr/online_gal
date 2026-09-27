#!/usr/bin/env bash
# Build the KiriKiri2 web engine (out/web/release/index.wasm + friends) on a Linux CI runner.
#
# WHY THIS MIRRORS UPSTREAM INSTEAD OF CALLING THE REPO'S OWN SCRIPT
#
# `ports/krkr/kirikiroid2/run-build.sh` is the Windows recipe: it downloads WinGet packages, passes
# `-DVCPKG_HOST_TRIPLET=x64-mingw-static`, points `CMAKE_MAKE_PROGRAM` at a MinGW `ninja.exe` and
# shells out through `cmd //c`. None of that exists on a Linux runner, and half of it would actively
# mis-configure the build. Upstream's own `.github/workflows/build-web.yml` is the Linux recipe, and
# this file reproduces its load-bearing steps.
#
# WHERE IT DIFFERS FROM THAT SCRIPT, AND WHY IT HAS TO
#
#   1. `ports/krkr/kirikiroid2/patch-emsdk-port-flags.mjs` hardcodes the SDK path
#      (`const PORTS = 'D:/emsdk/upstream/emscripten/tools/ports'`) and has no override, so on a
#      runner it prints "not present, skipped" for both files and leaves the bug it exists to fix in
#      place. The bug is real in a stock 6.0.9 install: `tools/ports/libpng.py` and `freetype.py`
#      interpolate `settings.WASM_LEGACY_EXCEPTIONS` (a boolean, so it serialises as the string
#      `True`) into an emcc flag, and emcc rejects `-sWASM_LEGACY_EXCEPTIONS=True` with
#      "use 1/0 to set boolean settings". That kills the `embuilder` prewarm below. It is fixed
#      here with an in-place `int(...)` substitution against the SDK the runner actually has.
#   2. The port overlays under `ports/krkr/kirikiroid2/overlay-ports/` are copied into the upstream
#      checkout's `vcpkg/ports/`, which is where `vcpkg-configuration.json` declares its overlay to
#      be. vcpkg reads overlays from the filesystem, so this is the only way to apply them without
#      editing a tracked upstream file.
#   3. The `embuilder` prewarm is explicit about variants. The `-mt` and `-legacysjlj` builds are
#      separate artifacts, and building only the plain name makes CMake's later port fetch
#      re-satisfy them -- slowly, and (for the legacysjlj pair) through the broken flag above.
#
# ORDER IS LOAD-BEARING: emsdk activation, then the port-flag fix, then `embuilder`, then cmake.
# Running `embuilder` before the fix caches the failure.
set -eu

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

: "${UPSTREAM_PATH:=.ref/kirikiroid2-web}"
: "${EMSDK_DIR:=${EMSDK:-$REPO_ROOT/.ref/emsdk}}"
: "${VCPKG_ROOT:=$REPO_ROOT/.vcpkg}"
export EMSDK_DIR
export VCPKG_ROOT
export REPO="$REPO_ROOT/$UPSTREAM_PATH"

echo "## repo:        $REPO_ROOT"
echo "## upstream:    $REPO"
echo "## EMSDK_DIR:   $EMSDK_DIR"
echo "## VCPKG_ROOT:  $VCPKG_ROOT"

[ -d "$REPO" ] || { echo "::error::no upstream checkout at $REPO"; exit 1; }
[ -d "$REPO/vcpkg/ports" ] || { echo "::error::$REPO/vcpkg/ports missing (overlay target)"; exit 1; }

# --- the emsdk port-flag fix ------------------------------------------------
#
# Applied unconditionally to the SDK the runner has. `grep -q` first so a re-run (or an SDK that
# already carries the fix) is a no-op rather than a double substitution.
EMPORTS="$EMSDK_DIR/upstream/emscripten/tools/ports"
[ -d "$EMPORTS" ] || { echo "::error::no port scripts at $EMPORTS -- is the SDK installed?"; exit 1; }
for port in libpng freetype; do
  file="$EMPORTS/$port.py"
  [ -f "$file" ] || { echo "::error::missing $file"; exit 1; }
  if grep -q 'settings.WASM_LEGACY_EXCEPTIONS}' "$file"; then
    sed -i 's/-sWASM_LEGACY_EXCEPTIONS={settings\.WASM_LEGACY_EXCEPTIONS}/-sWASM_LEGACY_EXCEPTIONS={int(settings.WASM_LEGACY_EXCEPTIONS)}/g' "$file"
    echo "## patched $port.py"
  else
    echo "## $port.py already carries the fix"
  fi
  grep -n 'WASM_LEGACY_EXCEPTIONS=' "$file" | sed 's/^/     /'
done

# --- vcpkg port overlays ----------------------------------------------------
#
# `vcpkg-configuration.json` declares `overlay-ports: ["./vcpkg/ports"]`, so a port is overridden by
# placing a directory of that name inside it. The upstream tree is a checkout, not a submodule of
# ours, so this edit is disposable -- it lives and dies with the runner.
if [ -d "ports/krkr/kirikiroid2/overlay-ports" ]; then
  for overlay in ports/krkr/kirikiroid2/overlay-ports/*/; do
    [ -d "$overlay" ] || continue
    name="$(basename "$overlay")"
    rm -rf "$REPO/vcpkg/ports/$name"
    cp -r "$overlay" "$REPO/vcpkg/ports/$name"
    echo "## overlay port: $name -> $REPO/vcpkg/ports/$name"
  done
fi

# --- emsdk ports prewarm ----------------------------------------------------
#
# The variants are explicit and the order matters: `freetype` depends on `libpng`, and the sjlj
# variants are what the wasm32-emscripten triplet asks for. Each `embuilder` run is retried because
# the first one downloads sources from Google's emscripten-ports bucket, which is the flakiest
# network dependency in this build.
#
# `EMSDK_DIR` has to be exported for `embuilder` to find the SDK it belongs to.
export EMSDK_DIR
export EMSDK="${EMSDK_DIR}"
export EMSDK_HOME="${EMSDK_DIR}"

embuild() {
  local port="$1" attempt status
  for attempt in 1 2 3; do
    if embuilder build "$port" >"/tmp/embuilder-$port.log" 2>&1; then
      echo "## embuilder $port ok"
      return 0
    fi
    echo "## embuilder $port attempt $attempt failed; tail:"
    tail -8 "/tmp/embuilder-$port.log" | sed 's/^/     /'
    sleep $((attempt * 5))
  done
  echo "::error::embuilder build $port failed after 3 attempts"
  return 1
}

for port in libpng libpng-mt libpng-legacysjlj libpng-mt-legacysjlj; do embuild "$port"; done
for port in freetype freetype-legacysjlj; do embuild "$port"; done
for port in harfbuzz harfbuzz-mt; do embuild "$port"; done
for port in sdl2 sdl2-mt; do embuild "$port"; done
for port in sdl2_ttf sdl2_ttf-mt; do embuild "$port"; done

# --- configure + build ------------------------------------------------------
#
# `cmake --preset` resolves `CMakePresets.json` from the CURRENT DIRECTORY upward, and that file
# lives in the upstream checkout -- not in this repository. So the configure and build steps run
# from `$REPO`, which is also where upstream's own CI runs them, and why the preset names
# `binaryDir: ${sourceDir}/out/web/release` resolve to the upstream tree.
#
# Configure is retried three times, as upstream does, because vcpkg resolves and builds its
# dependencies during configure and a single flaky download otherwise fails the whole run.
#
# `EMSDK` is the variable the preset's `$env{EMSDK}` toolchainFile reference reads, so it must be
# the absolute path to the emsdk root and must be exported for cmake's own environment.
BUILD_DIR="$REPO/out/web/release"
echo
echo "== vcpkg bootstrap =="
[ -f "$VCPKG_ROOT/bootstrap-vcpkg.sh" ] || {
  echo "::error::no vcpkg at $VCPKG_ROOT -- the reusable workflow sets it up via lukka/run-vcpkg"
  exit 1
}
if [ ! -x "$VCPKG_ROOT/vcpkg" ]; then
  "$VCPKG_ROOT/bootstrap-vcpkg.sh" -disableMetrics
fi

cd "$REPO"
echo
echo "== cmake configure (from $REPO) =="
STATUS=1
for attempt in 1 2 3; do
  echo "  attempt $attempt/3"
  if EMSDK="$EMSDK_DIR" VCPKG_ROOT="$VCPKG_ROOT" \
     cmake --preset "Web Release Config" \
       -DEMSCRIPTEN=1 \
       -DVCPKG_TARGET_TRIPLET=wasm32-emscripten \
       -DKRKR2_BUILD_VERSION="${GITHUB_SHA:-local}-${GITHUB_RUN_ID:-0}" \
       > /tmp/krkr2-configure.log 2>&1; then
    STATUS=0
    break
  fi
  STATUS=$?
  echo "  configure failed; tail:"
  tail -20 /tmp/krkr2-configure.log | sed 's/^/     /'
  sleep $((attempt * 15))
done
echo "## cmake configure exit: $STATUS"
[ "$STATUS" -eq 0 ] || exit "$STATUS"

echo
echo "== cmake build =="
cmake --build out/web/release > /tmp/krkr2-build.log 2>&1 || {
  tail -30 /tmp/krkr2-build.log | sed 's/^/     /'
  exit 1
}

# --- upstream's own release tests -------------------------------------------
#
# These are upstream's assertions about the release tree (the asset pair is complete, the vlfs zip
# cache behaves). They exist at the pinned commit, and running them here is what makes "the same
# recipe as upstream" checkable rather than merely claimed.
if [ -f "$REPO/tests/web/vlfs_zip_cache.test.mjs" ]; then
  echo
  echo "== upstream web tests =="
  node "$REPO/tests/web/vlfs_zip_cache.test.mjs"
  node "$REPO/tests/web/web_release_asset_pair.test.mjs" "$BUILD_DIR"
fi

# Back to the repo root: the collection step below uses repo-relative paths.
cd "$REPO_ROOT"

# --- collect ----------------------------------------------------------------
#
# The publishable set is copied into `dist/krkr/` so the artifact path is ours and does not depend
# on the preset's `binaryDir`. `shell.html` is included because `index.html` is generated from it
# and a deploy that regenerates the page needs the source of truth beside it.
OUT_DIR="$REPO_ROOT/dist/krkr"
rm -rf "$OUT_DIR"
mkdir -p "$OUT_DIR"

for name in index.wasm index.js vlfs.js index.html shell.html assets.zip; do
  src="$BUILD_DIR/$name"
  if [ ! -s "$src" ]; then
    echo "::error::missing build output $src"
    ls -la "$BUILD_DIR" 2>&1 | head -30 || true
    exit 1
  fi
  cp "$src" "$OUT_DIR/$name"
done

echo
echo "== artifacts =="
ls -la "$OUT_DIR"
