# Tests the repair hypothesis for the Emscripten port prewarm:
#
#   tools/ports/__init__.py:447   pkgconfig_dir = cache.get_sysroot_dir('lib/pkgconfig')
#   tools/ports/__init__.py:449   Ports.write_file(filename, ...)      # no mkdir
#
# The directory is only ever created by system_libs.py:2593
#   safe_copytree(utils.path_from_root('system/lib/pkgconfig'), cache.get_sysroot_dir('lib/pkgconfig'))
# which runs as part of sysroot setup. Once that setup is considered up to date, nothing recreates the
# directory — so every port build dies writing its .pc file *after* successfully producing its library,
# and the library is then discarded.
#
# Step 1 recreates the directory from emscripten's own built-in pkgconfig sources, step 2 builds the
# smallest port (zlib) and checks both the library and the directory's survival.
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

echo "== step 1: recreate pkgconfig dir from system/lib/pkgconfig =="
mkdir -p "$PC"
cp -f "$EM"/system/lib/pkgconfig/*.pc "$PC"/ 2>/dev/null
echo "contents now: $(ls -1 "$PC" | tr '\n' ' ')"
echo

echo "== step 2: embuilder build zlib =="
embuilder build zlib 2>&1 | tail -12
echo "embuilder exit=$?"
echo

echo "== after =="
echo "pkgconfig dir exists : $([ -d "$PC" ] && echo yes || echo NO)"
echo "pkgconfig contents   : $(ls -1 "$PC" 2>/dev/null | tr '\n' ' ')"
echo "libz.a               : $([ -f "$SYSLIB/libz.a" ] && echo "OK ($(stat -c %s "$SYSLIB/libz.a") bytes)" || echo MISSING)"
