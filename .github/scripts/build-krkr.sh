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
#   4. `ports/krkr/kirikiroid2/patch-vlfs-host-source.mjs` is applied here because the Windows
#      recipe applies it (`run-build.sh:319`) and a fresh clone has not. It is what installs the
#      `window.__onlineGalVfsSource` seam in `platforms/web/vlfs.js` + `shell.html`, which is how
#      this project feeds the engine game bytes from its own VFS instead of letting the page fetch
#      them. Without it the build succeeds and the engine runs -- but the host seam is simply
#      absent, so the deployed engine cannot load a game the way this project loads one. That is a
#      silent, functional difference, not a cosmetic one, so the seam is asserted after patching
#      and again in the built output.
#
# ORDER IS LOAD-BEARING: emsdk activation, then the port-flag fix, then `embuilder`, then cmake.
# Running `embuilder` before the fix caches the failure.
#
# DETERMINISM: `KRKR2_BUILD_VERSION` is passed the *upstream pin*, never a timestamp or a run id.
# Upstream defaults it to `string(TIMESTAMP ...)` (CMakeLists.txt:201-202), and an earlier version
# of this script passed `${GITHUB_SHA}-${GITHUB_RUN_ID}`; both change the `?v=` cache-buster in
# `index.html` and the `BUILD_VERSION` compiled into the module on every single run, which makes
# the published bytes impossible to pin in `ports/lib/engine-artifacts.json` and turns every
# consumer's checksum verification into a spurious failure.
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

# --- vlfs host byte-source seam --------------------------------------------
#
# Same patch the Windows recipe applies, for the same reason. The patcher restores each target from
# git before re-applying its insertions, so running it over an already-patched tree is a no-op that
# reproduces the same bytes (verified: two consecutive runs give identical md5).
#
# `refPath()` in `ports/lib/ref.mjs` resolves the checkout from `KIRIKIROID2_WEB_DIR` first, then
# `<repo-root>/.ref/kirikiroid2-web`. The checkout here is `$REPO`, which is normally that same
# path, but it is passed explicitly so an overridden `UPSTREAM_PATH` cannot silently patch a
# different tree than the one about to be compiled.
echo
echo "== vlfs host byte-source seam =="
node ports/krkr/kirikiroid2/patch-vlfs-host-source.mjs "$REPO" || {
  echo "::error::patch-vlfs-host-source.mjs failed"
  exit 1
}

# Assert the seam is actually in the sources now. The patcher is idempotent and prints its anchors,
# but a patch that silently matched nothing would leave a *working* build that quietly lacks the
# integration -- exactly the failure mode this step exists to prevent.
for probe in "platforms/web/vlfs.js:__onlineGalVfsSource" "platforms/web/shell.html:hostSourceStat"; do
  file="${probe%%:*}"; needle="${probe#*:}"
  grep -q "$needle" "$REPO/$file" || {
    echo "::error::$file is missing '$needle' after patching -- the host seam did not land"
    exit 1
  }
  echo "## seam present in $file ($needle)"
done

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
# The version string is derived from the checkout's own commit, so the same sources always produce
# the same bytes. `KRKR2_BUILD_VERSION` is sanitised upstream (`CMakeLists.txt:204` allows only
# `[A-Za-z0-9._-]`), and a 40-char hex sha satisfies that.
KRKR2_VERSION="$(git -C "$REPO" rev-parse --short=12 HEAD)"
echo "## KRKR2_BUILD_VERSION: $KRKR2_VERSION"
STATUS=1
for attempt in 1 2 3; do
  echo "  attempt $attempt/3"
  if EMSDK="$EMSDK_DIR" VCPKG_ROOT="$VCPKG_ROOT" \
     cmake --preset "Web Release Config" \
       -DEMSCRIPTEN=1 \
       -DVCPKG_TARGET_TRIPLET=wasm32-emscripten \
       -DKRKR2_BUILD_VERSION="$KRKR2_VERSION" \
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
