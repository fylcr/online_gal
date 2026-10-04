#!/usr/bin/env bash
# Standalone build for the artemis-compat source bundle.
#
# This is NOT the recipe CI runs. The recipe CI runs is `build.sh`, which is
# included beside this file for its *build logic* (patch order, the appended
# CMake target, every emcc/link flag) and which cannot run here: it sources
# `ports/env.sh` and refuses any path not ending in `ports/artemis/compat-web/out`,
# and this archive contains neither. So this file is the smallest thing that
# turns the bundled sources into the bundled artifacts, with no repository and
# no network.
#
#   EMSDK_DIR=/path/to/emsdk bash RUN-BUILD.sh [-j2]
#
# Layout expected, which is what the archive unpacks to:
#   src/        the pinned upstream tree (with patches/overlay NOT yet applied)
#   patches/    the two tracked patches, applied here in this order
#   overlay/    the host + audio + cmake overlays, copied in here
#   build.sh    the CI recipe, kept as the record of what CI ran
# Output: out/artemis-compat.js and out/artemis-compat.wasm
#
# Only one thing is removed, and its path is derived from this script's own
# location and asserted before use, so a wrong working directory is a wrong
# printed path rather than a deleted directory somewhere else.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SRC="$HERE/src"
BUILD="$HERE/out/source"
OBJ="$HERE/out/build"
LOG="$HERE/out/build.log"

# Refuse before removing anything: this script is meant to run inside an
# unpacked bundle, so a HERE that does not look like one is a mistake, not a
# layout to adapt to.
for f in "$SRC" "$HERE/overlay/audio/audio_web.cpp" "$HERE/overlay/cmake/web.cmake" \
         "$HERE/patches/0001-pf8-readat-host-source.patch" \
         "$HERE/patches/0002-numeric-strto.patch" \
         "$HERE/patches/0003-scene-checkpoint.patch"; do
  [ -e "$f" ] || { echo "refusing: $f is missing; run this in an unpacked bundle" >&2; exit 2; }
done
case "$HERE" in
  */artemis-compat-*) ;;
  *) echo "refusing: unexpected bundle root $HERE" >&2; exit 2 ;;
esac

JOBS=2
[ "${1:-}" = "-j" ] && [ -n "${2:-}" ] && JOBS="$2"

# The SDK is the one external input, and it is external on purpose: it is not
# ours to redistribute. A missing one is a refusal, never a silent fallback.
if [ -z "${EMSDK_DIR:-}" ]; then
  for c in /d/emsdk "$HOME/emsdk" /opt/emsdk; do
    [ -x "$c/upstream/emscripten/emcc" ] && EMSDK_DIR="$c" && break
  done
fi
if [ -z "${EMSDK_DIR:-}" ]; then
  echo "refusing: no emsdk found; set EMSDK_DIR to a built emsdk (emcc must exist)" >&2
  exit 2
fi
# Same reason as ports/env.sh: a POSIX EM_CONFIG makes the cache key alternate
# between two spellings and wipes the cache (including the sysroot zlib) each run.
[ -n "${EM_CONFIG:-}" ] && { echo "refusing: EM_CONFIG is set; unset it" >&2; exit 2; }
export EMSDK="$EMSDK_DIR"
export PATH="$EMSDK_DIR/upstream/emscripten:$EMSDK_DIR/upstream/bin:$PATH"

mkdir -p "$HERE/out"
: >"$LOG"
say() { printf '%s\n' "$*" | tee -a "$LOG"; }

say "bundle=$HERE"
say "emsdk=$EMSDK_DIR"
emcc --version | head -n 1 | tee -a "$LOG"

# The patched tree. `cp -a src/.` copies the contents and keeps the archive's
# own src/ intact, so the failure mode is a leftover directory rather than a
# modified source offer.
rm -rf "$BUILD" "$OBJ"
mkdir -p "$BUILD" "$OBJ"
cp -a "$SRC/." "$BUILD/"

# The host overlay replaces the pinned file outright, exactly as build.sh does.
cp "$HERE/overlay/host/web_main.cpp" "$BUILD/src/host/web_main.cpp"

# The audio overlay is compiled *instead of* the pinned src/audio/audio.cpp, and
# web.cmake points ARTC_AUDIO_WEB_DIR at $BUILD/overlay, so it has to be inside
# the build tree at that path. Copied whole rather than merged.
rm -rf "$BUILD/overlay"
cp -a "$HERE/overlay" "$BUILD/overlay"

for p in "$HERE"/patches/0001-pf8-readat-host-source.patch \
         "$HERE"/patches/0002-numeric-strto.patch \
         "$HERE"/patches/0003-scene-checkpoint.patch; do
  say "patch $(basename "$p")"
  # No --forward: an already-applied patch means the recipe no longer matches
  # its source, and that has to stop the build rather than pass quietly.
  patch --binary -d "$BUILD" -p1 --batch <"$p" >>"$LOG" 2>&1
done

# The web target. The pinned CMakeLists.txt has native targets only, so the
# target is appended here from the overlay -- this is the step that makes the
# bundle a web build rather than a description of one.
cat "$HERE/overlay/cmake/web.cmake" >>"$BUILD/CMakeLists.txt"

say "configure"
emcmake cmake -S "$BUILD" -B "$OBJ" \
  -DARTC_BUILD_CLI=OFF -DARTC_BUILD_JNI=OFF -DARTC_BUILD_MAC_HOST=OFF \
  -DARTC_BUILD_TESTS=OFF -DARTC_ENABLE_FFMPEG=OFF -DARTC_BUILD_WEB=ON \
  -DCMAKE_BUILD_TYPE=Release >>"$LOG" 2>&1

say "build (-j$JOBS)"
cmake --build "$OBJ" --target artemis-compat -j"$JOBS" >>"$LOG" 2>&1

say "artifacts"
sha256sum "$OBJ/artemis-compat.js" "$OBJ/artemis-compat.wasm" | tee -a "$LOG"
say "exit=0"
