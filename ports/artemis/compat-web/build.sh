#!/usr/bin/env bash
# Reproducible artemis-compat worker build.
# Inputs: local git object of pin 9f622629 (or a clean clone of that pin) plus
# the patches and overlay in this directory. Does not compile .ref and does not
# export EM_CONFIG. One configure, one build, -j2.
#
#   bash ports/artemis/compat-web/build.sh [-j N]
#
# Every path that is deleted or written is derived from HERE and printed before
# it is used, so a wrong working directory shows up as a wrong printed path
# instead of as a deleted directory somewhere else.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$HERE/../../.." && pwd)"
# shellcheck disable=SC1091
. "$ROOT/ports/env.sh"

JOBS=2
if [ "${1:-}" = "-j" ] && [ -n "${2:-}" ]; then JOBS="$2"; fi

PIN="${ARTC_PIN:-9f622629f595db85e6f76c45c06edde29d7d9961}"
REF="${ARTC_REF:-$ROOT/.ref/artemis-compat}"

# --- Output layout. Absolute, and checked before anything is removed. -------
# The three build directories are the only things this script deletes, and each
# one has to be a *specific* suffix under `ports/artemis/compat-web/out`. That
# check is what turns a `HERE` that is not what the caller expected (a stray
# `cd`, a copied script, a symlinked checkout) into an error message rather
# than a removed directory.
OUT="$HERE/out"
SRC="$OUT/source"
BUILD="$OUT/build-src"
OBJ="$OUT/build"
CLONE="$OUT/clone"
LOG="$OUT/build.log"
HASHES="$OUT/hashes.txt"

case "$OUT"   in */ports/artemis/compat-web/out) ;; *) echo "refusing: unexpected OUT=$OUT" >&2; exit 2 ;; esac
case "$SRC"   in "$OUT"/source)   ;; *) echo "refusing: unexpected SRC=$SRC" >&2; exit 2 ;; esac
case "$BUILD" in "$OUT"/build-src) ;; *) echo "refusing: unexpected BUILD=$BUILD" >&2; exit 2 ;; esac
case "$OBJ"   in "$OUT"/build)     ;; *) echo "refusing: unexpected OBJ=$OBJ" >&2; exit 2 ;; esac

# The log has to exist before the first `say`, and the removal has to come after
# the log is open: `: >"$LOG"` in a directory that does not exist fails under
# `set -e`, which is how the previous version died on a first run.
#
# `: >"$LOG"` truncates it: each run has to be the whole story of *that* run.
# An appended log made a stale `malformed patch` from an earlier run read as a
# failure of the current one, and the tail of a long build is where a reader
# looks first. Truncation is the only thing added here; `mkdir -p` is the same
# checked, absolute `$OUT` asserted above.
mkdir -p "$OUT"
: >"$LOG"

say() { printf '%s\n' "$*" | tee -a "$LOG"; }

say "pin=$PIN"
say "jobs=$JOBS"
say "HERE=$HERE"
say "ROOT=$ROOT"
say "OUT=$OUT"
say "SRC=$SRC"
say "BUILD=$BUILD"
say "OBJ=$OBJ"
say "LOG=$LOG"
say "REF=$REF"
say "emsdk=${EMSDK_DIR:-MISSING}"

command -v emcc >/dev/null
command -v cmake >/dev/null
command -v sha256sum >/dev/null
if [ -z "${EMSDK_DIR:-}" ]; then
  say "refusing: ports/env.sh found no emsdk (emcc is not on PATH)"
  exit 2
fi
if [ -n "${EM_CONFIG:-}" ]; then
  say "refusing: EM_CONFIG is set (${EM_CONFIG}); unset it so the cache key stays stable"
  exit 2
fi
emcc --version | head -n 1 | tee -a "$LOG"

# --- Source identity --------------------------------------------------------
# The pin must be a full 40-hex object, and it must resolve. A short or guessed
# revision is what turns "reproducible build" into "whatever was checked out",
# so both are refused rather than defaulted.
case "$PIN" in
  *[!0-9a-f]*|"") say "refusing: ARTC_PIN is not lowercase hex: $PIN"; exit 2 ;;
esac
if [ "${#PIN}" -ne 40 ]; then
  say "refusing: ARTC_PIN must be a full 40-char commit, got ${#PIN} chars: $PIN"
  exit 2
fi

# The pin is what is built. A `git archive` of a *name* would silently follow a
# moving branch, so the object has to exist locally and is archived by hash. The
# clone path is only for a machine that does not have the object, and it checks
# the checked-out revision against the pin afterwards.
if git -C "$REF" cat-file -e "$PIN^{commit}" 2>/dev/null; then
  say "source=git archive $REF $PIN"
  rm -rf "$SRC"
  mkdir -p "$SRC"
  git -C "$REF" archive "$PIN" | tar -x -C "$SRC"
else
  say "source=clone pin (local object missing)"
  say "clone=$CLONE"
  if [ -d "$CLONE/.git" ]; then
    # Reusable across runs: the previous clone is kept, and the fetch is what
    # proves the pin is reachable. No `|| true` anywhere in the pipeline.
    git -C "$CLONE" fetch --depth 1 origin "$PIN"
  else
    rm -rf "$CLONE"
    git clone --filter=blob:none --no-checkout \
      "https://github.com/Weiss-UltimateSavior/artemis-compat.git" "$CLONE"
    git -C "$CLONE" fetch --depth 1 origin "$PIN"
  fi
  git -C "$CLONE" checkout --detach "$PIN"
  HAVE="$(git -C "$CLONE" rev-parse HEAD)"
  if [ "$HAVE" != "$PIN" ]; then
    say "refusing: clone HEAD=$HAVE but PIN=$PIN"
    exit 2
  fi
  say "clone HEAD=$HAVE (matches pin)"
  # `git archive PIN`, not `HEAD`: the pin is the input, the checked-out
  # revision is only an assertion that the clone can supply it.
  rm -rf "$SRC"
  mkdir -p "$SRC"
  git -C "$CLONE" archive "$PIN" | tar -x -C "$SRC"
fi

say "archive-source-sha256=$(cd "$SRC" && find . -type f -print0 | sort -z | xargs -0 sha256sum | sha256sum | cut -d' ' -f1)"

# --- Patches and overlay ----------------------------------------------------
# Both trees are `rm -rf` of a checked path followed by `mkdir -p`: after
# `set -u` resolved HERE, neither can be empty, and both are asserted above to
# sit under OUT. `cp -a "$SRC/."` copies the *contents* and never renames SRC,
# so the archive the source-identity digest was taken from is still on disk.
rm -rf "$BUILD" "$OBJ"
mkdir -p "$BUILD" "$OBJ"
cp -a "$SRC/." "$BUILD/"
cp "$HERE/overlay/host/web_main.cpp" "$BUILD/src/host/web_main.cpp"
# The audio backend overlay is compiled *instead of* the pinned
# `src/audio/audio.cpp`, so it has to be inside the build tree: `web.cmake`
# points `ARTC_AUDIO_WEB_DIR` at `$BUILD/overlay` and refuses to configure
# without it. Copied whole, not merged, so nothing lands in the pinned tree.
rm -rf "$BUILD/overlay"
cp -a "$HERE/overlay/audio" "$BUILD/overlay.tmp"
mkdir -p "$BUILD/overlay"
mv "$BUILD/overlay.tmp" "$BUILD/overlay/audio"
for p in "$HERE"/patches/0001-pf8-readat-host-source.patch \
         "$HERE"/patches/0002-numeric-strto.patch; do
  say "patch $(basename "$p") sha256=$(sha256sum "$p" | cut -d' ' -f1)"
  # No `--forward`: a patch that is already applied is a recipe that no longer
  # matches its source, and that has to stop the build.
  patch --binary -d "$BUILD" -p1 --batch <"$p" >>"$LOG" 2>&1
done
cat "$HERE/overlay/cmake/web.cmake" >>"$BUILD/CMakeLists.txt"
say "web.cmake sha256=$(sha256sum "$HERE/overlay/cmake/web.cmake" | cut -d' ' -f1)"
say "web_main.cpp sha256=$(sha256sum "$HERE/overlay/host/web_main.cpp" | cut -d' ' -f1)"

# The generated tree is the build input; its digest goes into the report so a
# build can be tied to the exact patched source, not just to the pin.
say "patched-source-sha256=$(cd "$BUILD" && find . -type f -print0 | sort -z | xargs -0 sha256sum | sha256sum | cut -d' ' -f1)"

say "configure"
emcmake cmake -S "$BUILD" -B "$OBJ" \
  -DARTC_BUILD_CLI=OFF \
  -DARTC_BUILD_JNI=OFF \
  -DARTC_BUILD_MAC_HOST=OFF \
  -DARTC_BUILD_TESTS=OFF \
  -DARTC_ENABLE_FFMPEG=OFF \
  -DARTC_BUILD_WEB=ON \
  -DCMAKE_BUILD_TYPE=Release >>"$LOG" 2>&1

say "build (-j$JOBS)"
cmake --build "$OBJ" --target artemis-compat -j"$JOBS" >>"$LOG" 2>&1

say "artifacts"
ls -l "$OBJ/artemis-compat.js" "$OBJ/artemis-compat.wasm" | tee -a "$LOG"
sha256sum "$OBJ/artemis-compat.js" "$OBJ/artemis-compat.wasm" | tee -a "$LOG"
{
  sha256sum "$OBJ/artemis-compat.js" "$OBJ/artemis-compat.wasm"
  wc -c "$OBJ/artemis-compat.js" "$OBJ/artemis-compat.wasm"
} >"$HASHES"
say "hashes written to $HASHES"
say "exit=0"
