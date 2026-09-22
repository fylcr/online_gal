#!/usr/bin/env bash
# Build the OnscripterYuri web target.
#
# This is upstream's own recipe (`script/cross_web.sh`), driven from Git Bash on
# Windows. Two things differ from a Unix run and both are handled here:
#
#   * `cross_web.sh` reads `/proc/cpuinfo`, which Git Bash does not have, so the
#     job count is supplied explicitly.
#   * it sources `$EMSDK_HOME/emsdk_env.sh`, so EMSDK_HOME must be set to a
#     Unix-style path; Windows paths are converted with cygpath where needed.
#
# The output is `build_web/onsyuri.js` + `onsyuri.wasm`, which is the same pair the
# shipped prebuilt bundle contains — except built from source, so the engine can
# be modified (the input path in `ONScripter_event.cpp` is the reason to do this).
set -euo pipefail

EMSDK_WIN="${EMSDK_DIR:-D:/emsdk}"
export EMSDK_HOME="$(cygpath -u "$EMSDK_WIN" 2>/dev/null || echo "$EMSDK_WIN")"
export BUILD_TYPE="${BUILD_TYPE:-Release}"
JOBS="${JOBS:-8}"

cd "$(dirname "$0")"
echo "## emsdk: $EMSDK_HOME"
echo "## build type: $BUILD_TYPE, jobs: $JOBS"

# `cross_web.sh` sets BUILD_PATH relative to the script directory, so run from
# there. It also calls `make` with the CPU count read from /proc; provide a
# CPU-count shim by exporting the variable it would otherwise compute.
shopt -s expand_aliases || true

# Upstream reads: CORE_NUM=$(cat /proc/cpuinfo | grep -c ^processor)
# which fails here, so a fake /proc entry is not an option; instead the script is
# invoked with the file patched in a copy, keeping the original pristine.
TMP_SCRIPT="$(mktemp -d)/cross_web.sh"
sed "s|CORE_NUM=\$(cat /proc/cpuinfo .*|CORE_NUM=${JOBS}|" script/cross_web.sh > "$TMP_SCRIPT"
chmod +x "$TMP_SCRIPT"

echo "## running patched cross_web.sh"
bash "$TMP_SCRIPT" "$@"
