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
# Usage: ./web-deploy.sh [game-dir]
#
# The default game dir is **not** in the repository: the packs are real commercial game data and are
# never committed. Point `ARTEMIS_GAME_DIR` (or the first argument) at wherever they were unpacked.
set -eu

# Repo root, upstream checkout and `emcc` all come from the shared environment. See `ports/env.sh`.
. "$(dirname "$0")/../../env.sh"

REPO="${REPO:-$REF_DIR/artemis-compat}"
BUILD="${BUILD:-$PORTS_ROOT/ports/artemis/build/out/web-android}"
GAME="${1:-${ARTEMIS_GAME_DIR:-$REF_DIR/artemis-game}}"

echo "== rebuilding artemis-web-android =="
cmake --build "$BUILD" --target artemis-web-android -j"$(nproc 2>/dev/null || echo 8)" 2>&1 |
  grep -E "error:|Error |Linking|Built target artemis-web-android" || true
[ -f "$BUILD/artemis-web-android.wasm" ] || { echo "no wasm built (run build-emscripten.sh web-android first)" >&2; exit 1; }

echo "== deploying to $GAME =="
mkdir -p "$GAME"
cp "$BUILD/artemis-web-android.js" "$BUILD/artemis-web-android.wasm" "$GAME/"
# The worker and the page are hand-written sources, not build output, so they come from `ports/`.
cp "$PORTS_ROOT/ports/artemis/build/artemis-web-android-worker.js" "$GAME/"
cp "$PORTS_ROOT/ports/artemis/harness/android-host.html" "$GAME/"

ls -la "$GAME/artemis-web-android.js" "$GAME/artemis-web-android.wasm" "$GAME/artemis-web-android-worker.js" "$GAME/android-host.html"
