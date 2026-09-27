#!/usr/bin/env bash
#
# Builds the Artemis web host and deploys it beside the test game's pack files.
#
# The game mount is a directory of real `.pfs` archives (several GB) that the page must be served
# from the same origin as. Build output is copied in at run time rather than kept there, so the
# mount only ever holds game data plus whatever the last run deployed. A stale `artemis-web-android.wasm`
# sitting next to a pack is a trap: it makes "the engine did not change" indistinguishable from
# "the deploy did not happen", and that costs more time than this script.
#
# Usage: ./web-deploy.sh [game-dir] [--from-source]
#
# The engine comes from a verified published artifact by default; `--from-source` rebuilds it
# instead. Deploying a web page should not recompile the engine — the wasm only changes when the
# engine source does, and the artifact is the copy that was tested. A rebuild is still one flag
# away, and it happens automatically when no artifact is available and none can be downloaded.
#
# The default game dir is **not** in the repository: the packs are real commercial game data and are
# never committed. Point `ARTEMIS_GAME_DIR` (or the first argument) at wherever they were unpacked.
set -eu

# Repo root, upstream checkout and `emcc` all come from the shared environment. See `ports/env.sh`.
. "$(dirname "$0")/../../env.sh"

BUILD="${BUILD:-$PORTS_ROOT/ports/artemis/build/out/web-android}"
FROM_SOURCE=0

# The game dir is positional, so the flag can arrive in either position and both are accepted.
for argument in "$@"; do
  case "$argument" in
    --from-source|--rebuild) FROM_SOURCE=1 ;;
    -h|--help) sed -n '2,17p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
  esac
done

GAME="${ARTEMIS_GAME_DIR:-$REF_DIR/artemis-game}"
# `$1` is the game dir unless it was consumed as the flag, in which case `$2` is.
if [ "${1:-}" = "--from-source" ] || [ "${1:-}" = "--rebuild" ]; then
  GAME="${2:-$GAME}"
elif [ -n "${1:-}" ]; then
  GAME="$1"
fi

ENGINE_WASM="$BUILD/artemis-web-android.wasm"
ENGINE_JS="$BUILD/artemis-web-android.js"

# --- which engine path? ---------------------------------------------------------------------
#
# The helper is asked first, and its exit code decides the path:
#
#   0 verified artifact in place  -> deploy it, do not compile
#   2 unavailable, 4 bad manifest -> nothing to be had; build from source
#   3 verification failed         -> stop. Building here would silently replace a corrupted
#                                   artifact with a local one and hide the corruption.
#
# `--from-source` skips the helper entirely, which is what a developer changing the engine wants:
# the artifact is by definition older than their edit.
engine_source="fetched artifact"
run_build=0

if [ "$FROM_SOURCE" -eq 1 ]; then
  engine_source="source build (--from-source)"
  run_build=1
  echo "== engine: rebuilding from source (--from-source) =="
else
  set +e
  node "$PORTS_ROOT/ports/lib/fetch-engine.mjs" artemis --out "$BUILD"
  fetch_status=$?
  set -e

  if [ "$fetch_status" -eq 3 ]; then
    echo "== engine: the published artifact failed sha256 verification — stopping ==" >&2
    echo "   re-run to retry the download, or pass --from-source to build locally" >&2
    exit 3
  elif [ "$fetch_status" -ne 0 ]; then
    echo "== engine: no verified artifact available (fetch exited $fetch_status) — building from source =="
    run_build=1
  else
    echo "== engine: using the verified published artifact (no compile) =="
  fi
fi

if [ "$run_build" -eq 1 ]; then
  echo "== rebuilding artemis-web-android =="
  cmake --build "$BUILD" --target artemis-web-android -j"$(nproc 2>/dev/null || echo 8)" 2>&1 |
    grep -E "error:|Error |Linking|Built target artemis-web-android" || true
  [ "$FROM_SOURCE" -eq 1 ] || engine_source="source build (no artifact available)"
fi

[ -f "$ENGINE_WASM" ] || {
  echo "no wasm available at $ENGINE_WASM" >&2
  echo "  fetch it: node ports/lib/fetch-engine.mjs artemis" >&2
  echo "  or build: bash ports/artemis/build/build-emscripten.sh web-android" >&2
  exit 1
}

echo "== deploying ($engine_source) to $GAME =="
mkdir -p "$GAME"
cp "$ENGINE_JS" "$ENGINE_WASM" "$GAME/"
# The worker and the page are hand-written sources, not build output, so they come from `ports/`.
cp "$PORTS_ROOT/ports/artemis/build/artemis-web-android-worker.js" "$GAME/"
cp "$PORTS_ROOT/ports/artemis/harness/android-host.html" "$GAME/"

ls -la "$GAME/artemis-web-android.js" "$GAME/artemis-web-android.wasm" "$GAME/artemis-web-android-worker.js" "$GAME/android-host.html"
