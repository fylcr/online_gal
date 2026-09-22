# Builds the Emscripten port variants the web build actually resolves, which run-build.sh's prewarm
# list does not cover.
#
# Observed failure inside `cmake --build`:
#   emcc -c .../SDL_ttf.c ... -sUSE_SDL=2 -sUSE_FREETYPE -sUSE_HARFBUZZ -pthread
#     -> File "tools/ports/freetype.py", line 111, in get  (building a *port* while compiling)
#     -> AssertionError: attempt to lock the cache while a parent process is holding the lock
#        (sysroot\lib\wasm32-emscripten\libfreetype.a)
#
# Two causes:
#   1. `-pthread` selects the `-mt` port variant. The prewarm list in run-build.sh covers
#      "freetype freetype-legacysjlj" but never `freetype-mt`, so emcc had to build freetype *nested*
#      inside the sdl2_ttf port build — which emscripten forbids (cache lock assertion).
#   2. `tools/ports/__init__.py:447` writes `<sysroot>/lib/pkgconfig/<name>.pc` without creating the
#      directory, so any port build on a sysroot whose pkgconfig dir is absent aborts *after* producing
#      its library, discarding it. The directory is only created as a side effect of the sysroot
#      header install, so it can be missing while ports are being built. Recreate it first.
set -u

EMSDK_DIR=/d/emsdk
EM="$EMSDK_DIR/upstream/emscripten"
export EMSDK="$EMSDK_DIR"
# `EM_CONFIG` is deliberately NOT exported here.
#
# Spelling it as a POSIX path (`$EMSDK_DIR/.emscripten` with EMSDK_DIR=/d/emsdk) makes Emscripten's
# cache sanity string `6.0.9|D:/emsdk/upstream/bin`; leaving it unset gives
# `6.0.9|D:\emsdk/upstream/bin`. The two differ, so Emscripten answers with
# "config changed, clearing cache" and **erases cache/sysroot — every port library included**.
# Emscripten finds `$EMSDK_DIR/.emscripten` by itself; pinning the spelling is what causes the wipe.
export PATH="$EM:$EM/upstream/bin:$EMSDK_DIR:$PATH"
if [ -f "$EMSDK_DIR/emsdk_env.sh" ]; then
  . "$EMSDK_DIR/emsdk_env.sh" >/dev/null 2>&1 || true
fi

PC="$EM/cache/sysroot/lib/pkgconfig"
SYSLIB="$EM/cache/sysroot/lib/wasm32-emscripten"

echo "== recreate pkgconfig dir (see note 2 above) =="
mkdir -p "$PC"
cp -f "$EM"/system/lib/pkgconfig/*.pc "$PC"/ 2>/dev/null
echo "contents: $(ls -1 "$PC" | tr '\n' ' ')"

echo
echo "== build the variants the build resolves (incl. the missing -mt freetype) =="
for p in freetype-mt sdl2_ttf-mt freetype-mt-legacysjlj sdl2_ttf-mt-legacysjlj \
         libpng-mt libpng-mt-legacysjlj harfbuzz-mt sdl2-mt zlib ; do
  echo "--- embuilder build $p"
  embuilder build "$p" 2>&1 | tail -3
done

echo
echo "== port libraries now present =="
for lib in libz.a libpng.a libpng-mt.a libpng-legacysjlj.a libpng-mt-legacysjlj.a \
           libfreetype.a libfreetype-mt.a libfreetype-legacysjlj.a libfreetype-mt-legacysjlj.a \
           libharfbuzz.a libharfbuzz-mt.a libSDL2.a libSDL2-mt.a \
           libSDL2_ttf.a libSDL2_ttf-mt.a ; do
  if [ -f "$SYSLIB/$lib" ]; then echo "  OK      $lib"; else echo "  MISSING $lib"; fi
done
