# Captures the FULL embuilder traceback for one port, to identify which emscripten code path writes
# the port .pc file and why its parent directory is missing at that moment.
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
  . "$EMSDK_DIR/emsdk_env.sh" >/dev/null 2>&1 || true
fi

PC="$EMSDK_DIR/upstream/emscripten/cache/sysroot/lib/pkgconfig"
LOG=/tmp/embuilder-libpng-full.log

echo "== pkgconfig before =="
ls -1 "$PC" 2>&1 | head -20
echo

echo "== embuilder build libpng (full output -> $LOG) =="
embuilder build libpng > "$LOG" 2>&1
echo "exit=$?"
echo
echo "== first 60 lines of the failure =="
head -60 "$LOG"
echo

echo "== pkgconfig after =="
ls -1 "$PC" 2>&1 | head -20
echo

echo "== where does emscripten expect to write port .pc files? =="
grep -rn "pkgconfig" "$EMSDK_DIR/upstream/emscripten/tools/ports/__init__.py" 2>/dev/null | head -20
echo "--- system_libs.py ---"
grep -rn "pkgconfig" "$EMSDK_DIR/upstream/emscripten/tools/system_libs.py" 2>/dev/null | head -20
