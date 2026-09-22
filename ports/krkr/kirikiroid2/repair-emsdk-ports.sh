# Repairs the Emscripten port prewarm that run-build.sh attempts and that has been silently failing.
#
# The prewarm loop only tails 3 lines of embuilder output, so its failure was invisible: `embuilder`
# could not write `sysroot/lib/pkgconfig/zlib.pc` because that directory did not exist, which aborted
# the install step of every port. The result is a sysroot with the port *headers* (zlib.h is present)
# but none of the port *libraries* — `sysroot/lib/wasm32-emscripten/` held only the built-in system
# libs, and nothing anywhere in the cache contained libz.a / libpng.a / libSDL2.a. The web build links
# cocos2d-x against exactly these ports, so this would have failed at link time.
#
# This script only drives embuilder with the same environment run-build.sh sets up. It changes no SDK
# version and no build flags.
set -u

EMSDK_DIR=/d/emsdk
export EMSDK="$EMSDK_DIR"
# `EM_CONFIG` is deliberately NOT exported here.
#
# Spelling it as a POSIX path (`$EMSDK_DIR/.emscripten` with EMSDK_DIR=/d/emsdk) makes Emscripten's
# cache sanity string `6.0.9|D:/emsdk/upstream/bin`; leaving it unset gives
# `6.0.9|D:\emsdk/upstream/bin`. The two differ, so Emscripten answers with
# "config changed, clearing cache" and **erases cache/sysroot — every port library included**.
# Emscripten finds `$EMSDK_DIR/.emscripten` by itself; pinning the spelling is what causes the wipe.
export PATH="$EMSDK_DIR/upstream/emscripten:$EMSDK_DIR/upstream/bin:$EMSDK_DIR:$PATH"

if [ -f "$EMSDK_DIR/emsdk_env.sh" ]; then
  # shellcheck disable=SC1091
  . "$EMSDK_DIR/emsdk_env.sh" >/dev/null 2>&1 || true
fi

SYSLIB="$EMSDK_DIR/upstream/emscripten/cache/sysroot/lib/wasm32-emscripten"
PKGCONFIG="$EMSDK_DIR/upstream/emscripten/cache/sysroot/lib/pkgconfig"

echo "== ensure pkgconfig dir exists (the missing directory that aborted every port install) =="
mkdir -p "$PKGCONFIG" && echo "ok: $PKGCONFIG"

echo
echo "== embuilder per port (full output, not just the tail) =="
for ports in \
  "libpng libpng-mt libpng-legacysjlj libpng-mt-legacysjlj" \
  "freetype freetype-legacysjlj" \
  "harfbuzz harfbuzz-mt" \
  "sdl2 sdl2-mt" \
  "sdl2_ttf sdl2_ttf-mt" ; do
  for p in $ports; do
    echo "--- embuilder build $p"
    embuilder build "$p" 2>&1 | tail -6
  done
done

echo
echo "== resulting port libraries in the sysroot =="
for lib in libz.a libpng.a libfreetype.a libharfbuzz.a libSDL2.a libSDL2_ttf.a; do
  if [ -f "$SYSLIB/$lib" ]; then echo "  OK      $lib ($(stat -c %s "$SYSLIB/$lib") bytes)"; else echo "  MISSING $lib"; fi
done

echo
echo "== pkgconfig entries =="
ls -1 "$PKGCONFIG" 2>/dev/null | tr '\n' ' '
echo
