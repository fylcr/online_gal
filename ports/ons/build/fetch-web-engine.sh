#!/usr/bin/env bash
# Gets the ONS web engine into `packages/engine/core/onscripter-yuri/assets/`, compiling only when it must.
#
# Two ways to obtain the same three files (`onsyuri.html`, `onsyuri.js`, `onsyuri.wasm`):
#
#   1. **Fetch** the published, hash-verified artifact (`ports/lib/fetch-engine.mjs ons`). This is
#      the default, and it is what makes a fresh clone usable without an Emscripten SDK, an
#      OnscripterYuri checkout, or a twenty-minute build.
#   2. **Build** from source, through the shimmed recipe, and copy the result into `assets/`.
#
# The fetch is tried first and the build is the *fallback*, not the other way round, because the
# published artifact is the one that was verified; the local build is merely the one that is
# available right now. `--from-source` skips straight to the build when that is what you want.
#
# Usage:
#
#     bash ports/ons/build/fetch-web-engine.sh                # fetch, else build
#     bash ports/ons/build/fetch-web-engine.sh --from-source  # always build
#
# `assets/` is **tracked source**, unlike the other engines' build directories, so a fetch or a
# build here rewrites files that `git status` will show. That is deliberate: the engine the emulator
# ships with should be reviewable like any other change, and the sha256 in
# `ports/lib/engine-artifacts.json` is what identifies it.
set -u

# Repo root, upstream checkout and `emcc` all come from the shared environment. See `ports/env.sh`.
. "$(dirname "$0")/../../env.sh"

FROM_SOURCE=0
for argument in "$@"; do
  case "$argument" in
    --from-source|--rebuild) FROM_SOURCE=1 ;;
    -h|--help)
      # Print the leading comment block: the first line is the shebang, and the block ends at the
      # first line that is not a comment.
      awk 'NR > 1 { if ($0 !~ /^#/) exit; sub(/^# ?/, ""); print }' "$0"
      exit 0
      ;;
    *)
      echo "unknown argument: $argument" >&2
      echo "usage: bash ports/ons/build/fetch-web-engine.sh [--from-source]" >&2
      exit 1
      ;;
  esac
done

FETCH="$PORTS_ROOT/ports/lib/fetch-engine.mjs"
MANIFEST="$PORTS_ROOT/ports/lib/engine-artifacts.json"
REPO="${REPO:-$REF_DIR/onscripter-yuri}"
BUILD_WEB="$REPO/build_web"
ASSETS="$PORTS_ROOT/packages/engine/core/onscripter-yuri/assets"
FILES="onsyuri.html onsyuri.js onsyuri.wasm"

# --- 1. the cheap path: a published artifact ------------------------------------------------

if [ "$FROM_SOURCE" -eq 0 ]; then
  echo "== ons: looking for a published engine artifact =="
  # `--json` is deliberately not used: this wrapper only needs the exit code, and the human lines
  # are what a reader of the build log wants.
  #
  # The status is captured from the command DIRECTLY, not from `if node ...; then` followed by `$?`.
  # That older form read the exit code of the `if` compound, which is 0 whenever the condition is
  # false -- so every failure arrived here as a 0 and the exit-3 guard below could never fire. A
  # corrupted artifact therefore fell through to a source build and replaced it, which is precisely
  # the outcome that guard exists to prevent (see the comment on 3 below).
  node "$FETCH" ons
  status=$?

  if [ "$status" -eq 0 ]; then
    echo "== ons: using the fetched artifact (no compile) =="
    exit 0
  fi

  # Every other code is a reason the fetch did not produce a file; which one decides whether
  # falling back is right:
  #
  #   2 (unavailable) — no release published yet, or the network is down. Expected on a fresh
  #                     clone, and building from source is exactly the right answer.
  #   4 (bad manifest) — the publisher and this script disagree. Still no artifact, so still a
  #                     fallback; the message above says which it was.
  #   3 (verification failed) — the bytes downloaded did not match the manifest. Falling back here
  #                     would *hide* a corrupted or substituted artifact, so this one stops.
  if [ "$status" -eq 3 ]; then
    echo "== ons: the artifact failed sha256 verification; not building over it silently ==" >&2
    echo "   re-run to retry the download, or pass --from-source to build locally" >&2
    exit 3
  fi

  echo "== ons: no verified artifact available (fetch exited $status) — building from source =="
fi

# --- 2. the expensive path: build it here ---------------------------------------------------

if [ -z "${EMSDK_DIR:-}" ]; then
  echo "== ons: no Emscripten SDK found, so the source build cannot run ==" >&2
  echo "   set EMSDK_DIR, or install the SDK where ports/env.sh looks (see its header)" >&2
  echo "   alternatively, wait for a published artifact and re-run without --from-source" >&2
  exit 1
fi

if [ ! -d "$REPO" ]; then
  echo "== ons: upstream checkout not found at $REPO ==" >&2
  echo "   the build recipe needs it; see the root .gitignore for where it comes from" >&2
  exit 1
fi

echo "== ons: building from source through the shimmed recipe =="
echo "   repo:  $REPO"
echo "   emsdk: $EMSDK_DIR"
# `run-web-build-shimmed.sh` is the real entry point: it patches `CMakeLists.txt` so the host
# event-injection shim is compiled in, runs upstream's recipe, and restores the checkout on exit.
# A plain `run-web-build.sh` build would produce an engine without working input.
bash "$PORTS_ROOT/ports/ons/build/run-web-build-shimmed.sh" || {
  echo "== ons: source build failed ==" >&2
  exit 1
}

# --- 3. place the result where the runtime reads it -----------------------------------------

missing=0
for name in $FILES; do
  [ -f "$BUILD_WEB/$name" ] || { echo "== ons: build produced no $BUILD_WEB/$name ==" >&2; missing=1; }
done
[ "$missing" -eq 0 ] || exit 1

mkdir -p "$ASSETS"
for name in $FILES; do
  cp "$BUILD_WEB/$name" "$ASSETS/$name"
  echo "   copied $name -> $ASSETS/$name"
done

# The manifest is keyed by hash, so a fresh build makes it stale by definition. Printing the new
# values rather than writing them is intentional: an artifact becomes publishable only after it has
# been *tested*, and silently rewriting the manifest here would let an untested build claim a
# verified hash.
echo
echo "== ons: built from source and installed into packages/engine/core/onscripter-yuri/assets/ =="
echo "   this build is not yet in the manifest; after testing it, update"
echo "   ports/lib/engine-artifacts.json with the values below and publish a release."
echo
for name in $FILES; do
  size=$(stat -c %s "$ASSETS/$name" 2>/dev/null || wc -c < "$ASSETS/$name" | tr -d ' ')
  hash=$(sha256sum "$ASSETS/$name" | cut -d' ' -f1)
  printf '   { "name": "%s", "size": %s, "sha256": "%s" },\n' "$name" "$size" "$hash"
done
