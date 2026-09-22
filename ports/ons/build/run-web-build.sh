#!/usr/bin/env bash
# Drives upstream's `script/cross_web.sh` under Git Bash on Windows.
#
# Upstream's CI runs this on Linux, but the script already handles MSYS
# (`uname -a | grep Msys`), so only a few things need supplying here:
#
#   * `EMSDK_HOME` — `cross_web.sh` sources `$EMSDK_HOME/emsdk_env.sh`, and that
#     file lives at the emsdk **root** (`<emsdk>/emsdk_env.sh`), alongside
#     `emsdk.py`. Pointing it at `upstream/emscripten` looks natural but is wrong.
#   * `CORE_NUM` is read from `/proc/cpuinfo`, which does not exist under Git
#     Bash, and the empty value silently makes every `make -j` single-threaded.
#
# The patched script is written into `script/` rather than `/tmp`, because
# `cross_web.sh` sources `./_fetch.sh` relative to its own location and so must
# run from there. A distinct name keeps the original file untouched.
set -eu

# Repo root, upstream checkout and `emcc` all come from the shared environment, which derives
# the root from its own location and searches for the SDK. See `ports/env.sh`.
. "$(dirname "$0")/../../env.sh"

REPO="${REPO:-$REF_DIR/onscripter-yuri}"
JOBS="${JOBS:-12}"
BUILD_TYPE="${BUILD_TYPE:-Release}"

# `env.sh` leaves `EMSDK_DIR` in whatever form it found; `cross_web.sh` needs the Unix one.
EMSDK_UNIX="$(ports_unix "$EMSDK_DIR")"
export EMSDK_HOME="$EMSDK_UNIX"
export BUILD_TYPE

for candidate in "$EMSDK_HOME/emsdk_env.sh" "$EMSDK_HOME/upstream/emscripten/emsdk_env.sh"; do
  if [ -f "$candidate" ]; then
    ENV_SCRIPT="$candidate"
    break
  fi
done
if [ -z "${ENV_SCRIPT:-}" ]; then
  echo "## ERROR: emsdk_env.sh not found under $EMSDK_HOME" >&2
  ls -la "$EMSDK_HOME" 2>&1 | head -20 >&2
  exit 1
fi

echo "## repo:       $REPO"
echo "## EMSDK_HOME: $EMSDK_HOME"
echo "## env script: $ENV_SCRIPT"
echo "## BUILD_TYPE: $BUILD_TYPE"
echo "## CORE_NUM:   $JOBS"
echo

# Emscripten's environment (emcc, emmake, emcmake, embuilder) on PATH. `set -u` is
# relaxed while sourcing, because the script reads variables it does not define.
set +u
# shellcheck disable=SC1090
source "$ENV_SCRIPT" >/dev/null 2>&1 || source "$ENV_SCRIPT"
set -u

# The source above did not actually add the tools to PATH in this environment —
# `command -v emcc` came back empty afterwards — so the directory is set directly
# rather than trusted. `emcc`, `emcmake` and friends are extensionless shell
# scripts here, so they only work from a bash shell.
EMSDK_BIN="$EMSDK_HOME/upstream/emscripten"
case ":$PATH:" in
  *":$EMSDK_BIN:"*) ;;
  *) PATH="$EMSDK_BIN:$PATH" ;;
esac
export PATH

if ! command -v emcc >/dev/null 2>&1; then
  echo "## ERROR: emcc still not on PATH after adding $EMSDK_BIN" >&2
  ls "$EMSDK_BIN" 2>&1 | head -20 >&2
  exit 1
fi
echo "## emcc: $(command -v emcc) — $(emcc --version 2>/dev/null | head -1)"

cd "$REPO"
mkdir -p thirdparty/port thirdparty/build/arch_wasm

PATCHED="script/cross_web_generated.sh"
sed "s|^CORE_NUM=.*|CORE_NUM=${JOBS}|" script/cross_web.sh > "$PATCHED"
chmod +x "$PATCHED"
trap 'rm -f "$REPO/$PATCHED"' EXIT

cd script
exec bash "./cross_web_generated.sh" "$@"
