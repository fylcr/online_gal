#!/usr/bin/env bash
# Purpose: rebuild the official Artemis module from its transpiled form on top of the hand-written
# engine in `ports/artemis/src`, assert the transpile is the reference one, and assert the rebuilt
# module is a drop-in replacement. This is the only Artemis build.
# Scope: asserts the module's bytes and its agreement with the official module. It does not measure
# how much of the rewrite is in place, and it does not boot a game.
set -eu

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

: "${EMSDK_DIR:=${EMSDK:-$REPO_ROOT/.ref/emsdk}}"
export EMSDK_DIR
WABT_COMMIT="02418f72a70466b45805891843f114c7f602eb34"
# Reject an unsupported flavour here, before the wabt build and the 103 MB transpile are paid for.
# `sync` is not a variant of this build: the route reads its rewrite from an untracked local tree, so
# no clean clone can produce it. A blank value falls back to the default.
FLAVOUR="${BRIDGE_FLAVOUR:-asyncify}"
case "$FLAVOUR" in
  asyncify) ;;
  sync)
    echo "::error::BRIDGE_FLAVOUR=sync is no longer supported: this build is asyncify-only."
    echo "::error::a sync bridge needs .ref/artemis-official/rewrite-sync (an untracked local tree), so no clean clone can produce it."
    exit 1
    ;;
  *)
    echo "::error::unknown BRIDGE_FLAVOUR '$FLAVOUR' (expected: asyncify)"
    exit 1
    ;;
esac
# The one input module and the route directory `build-bridge-route.sh` names after it.
TARGET="ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm"
RDIR="asyncify"

# `$WORK` is scratch: the wabt checkout, the regenerated transpile and the runtime headers. It sits
# under the runner temp dir because the checkout's `build/` is not gitignored, so writing there would
# leave untracked files behind. RUNNER_TEMP is set on Actions; the `/tmp` fallback is for local runs.
WORK="${RUNNER_TEMP:-/tmp}/artemis-bridge"
OFFICIAL_ROOT="$WORK/official"
WABT_SRC="$WORK/wabt"
GEN_C="$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.c"
GEN_H="$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.h"

# `build-bridge-route.sh` resolves `$ARTEMIS_OFFICIAL` itself, so point it at the tree this run just
# generated instead of its dev-machine default under `.ref/`.
export ARTEMIS_OFFICIAL="$OFFICIAL_ROOT"

echo "## repo:        $REPO_ROOT"
echo "## flavour:     $FLAVOUR"
echo "## module:      $TARGET"
echo "## EMSDK_DIR:   $EMSDK_DIR"
echo "## scratch:     $WORK"
echo "## official:    $OFFICIAL_ROOT  (generated; not .ref/artemis-official)"

[ -s "$TARGET" ] || { echo "::error::missing vendored module $TARGET"; exit 1; }
[ -d ports/artemis/src ] || { echo "::error::missing ports/artemis/src (the rewritten engine)"; exit 1; }
# Checked before the wabt build, since `build-bridge-route.sh` tests for em++ only at the very end.
[ -x "${EMSDK_DIR:-}/upstream/emscripten/em++" ] || {
  echo "::error::no em++ at ${EMSDK_DIR:-<unset>}/upstream/emscripten/em++ -- is EMSDK_DIR right?"
  exit 1
}

# The route and its helpers invoke bare `python`; Ubuntu ships only `python3`, so shim it onto PATH.
PYTHON_BIN=""
for cand in python3 python; do
  if command -v "$cand" >/dev/null 2>&1 && "$cand" --version >/dev/null 2>&1; then
    PYTHON_BIN="$(command -v "$cand")"
    break
  fi
done
[ -n "$PYTHON_BIN" ] || { echo "::error::no working python3/python on PATH"; exit 1; }
SHIM_BIN="$(mktemp -d)"
ln -s "$PYTHON_BIN" "$SHIM_BIN/python"
export PATH="$SHIM_BIN:$PATH"
echo "## python:      $PYTHON_BIN (shimmed as 'python' for the reverse-engineering scripts)"

# Build wasm2c from wabt source at the pinned commit -- a release tarball does not reproduce the
# reference transpile. Only `third_party/picosha2` is initialised (wabt does not compile without it);
# `--recursive` would fetch six submodules this build never compiles.
echo
echo "== building wabt (wasm2c) =="
if [ ! -x "$WABT_SRC/build/wasm2c" ]; then
  mkdir -p "$WORK"
  [ -d "$WABT_SRC/.git" ] || git clone https://github.com/WebAssembly/wabt.git "$WABT_SRC"
  git -C "$WABT_SRC" fetch --depth 1 origin "$WABT_COMMIT"
  git -C "$WABT_SRC" checkout --detach "$WABT_COMMIT"
  [ "$(git -C "$WABT_SRC" rev-parse HEAD)" = "$WABT_COMMIT" ] || {
    echo "::error::wabt checkout is not at $WABT_COMMIT"
    exit 1
  }
  git -C "$WABT_SRC" submodule update --init --depth 1 third_party/picosha2
  [ -f "$WABT_SRC/third_party/picosha2/picosha2.h" ] || {
    echo "::error::third_party/picosha2 is missing; wasm2c will not compile without it"
    exit 1
  }
  cmake -S "$WABT_SRC" -B "$WABT_SRC/build" \
    -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=OFF -DBUILD_LIBWASM=OFF
  cmake --build "$WABT_SRC/build" --target wasm2c -j"$(nproc 2>/dev/null || echo 4)"
fi
WASM2C="$WABT_SRC/build/wasm2c"
[ -x "$WASM2C" ] || { echo "::error::wasm2c was not built at $WASM2C"; exit 1; }
"$WASM2C" --version 2>&1 | sed 's/^/## wasm2c: /'

# Transpile the vendored module. The output basename is load-bearing: wasm2c derives its emitted
# `#include` from it, so naming the output anything but `artemis.wasm2c.c` changes the bytes.
echo
echo "== transpiling $FLAVOUR =="
mkdir -p "$(dirname "$GEN_C")" "$OFFICIAL_ROOT/wasm/runtime"
"$WASM2C" "$TARGET" -n artemis -o "$GEN_C"
[ -f "$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.h" ] || {
  echo "::error::wasm2c did not emit the sibling .h beside $GEN_C"
  exit 1
}

# wasm2c writes no `wasm-rt.h` even though the generated `.h` includes it, so copy wabt's runtime
# headers beside the generated pair to let it compile without reaching back into the wabt checkout.
for header in wasm-rt.h wasm-rt-impl.h wasm-rt-impl.c wasm-rt-mem-impl.c \
              wasm-rt-exceptions.h wasm-rt-exceptions-impl.c \
              wasm-rt-impl-tableops.inc wasm-rt-mem-impl-helper.inc; do
  cp "$WABT_SRC/wasm2c/$header" "$OFFICIAL_ROOT/wasm/runtime/$header"
done
cp "$WABT_SRC/wasm2c/README.md" "$OFFICIAL_ROOT/wasm/runtime/README.wasm2c.md"

# Assert the transpile is the reference one, byte for byte: wasm2c is deterministic for a fixed
# input module and wabt commit, so any difference means the input or the tool moved. Both the `.c` and
# `.h` are hashed, because `build-bridge-route.sh` takes the export surface from the `.h`. The values
# match `.github/upstream-pins.json` under `artemis-bridge.expected`.
echo
echo "== verifying the transpile =="
# One set of constants, because there is one input module.
EXPECTED_BYTES=107593501
EXPECTED_MD5=d895a74b11ba767b06c82d722c407517
EXPECTED_H_MD5=306f97d1e6fa4d64ea0e5dc720dee7ab
ACTUAL_BYTES="$(stat -c%s "$GEN_C")"
ACTUAL_MD5="$(md5sum "$GEN_C" | cut -d' ' -f1)"
ACTUAL_H_MD5="$(md5sum "$GEN_H" | cut -d' ' -f1)"
echo "## $FLAVOUR: $ACTUAL_BYTES B, md5 $ACTUAL_MD5"
echo "## header:   md5 $ACTUAL_H_MD5"
echo "## expected: $EXPECTED_BYTES B, md5 $EXPECTED_MD5 / header $EXPECTED_H_MD5"
if [ "$ACTUAL_BYTES" != "$EXPECTED_BYTES" ] || [ "$ACTUAL_MD5" != "$EXPECTED_MD5" ]; then
  echo "::error::the transpile differs from the reference -- refusing to publish a module built from it"
  exit 1
fi
if [ "$ACTUAL_H_MD5" != "$EXPECTED_H_MD5" ]; then
  echo "::error::the transpiled header differs from the reference -- the export surface it describes is not the measured one"
  exit 1
fi

# Generate the export shims `build-bridge-route.sh` expects at
# `ports/artemis/reverse/out/<flavour>/`: they re-expose the wasm2c exports under the original
# module's names. `rebuild.sh wasm`, the dev-machine step that usually makes them, cannot run here
# (its wat2wasm and `so` halves are Windows-only), so the same generator is invoked directly.
echo
echo "== generating the export shims =="
SHIMS_DIR="$REPO_ROOT/ports/artemis/reverse/out/$RDIR"
mkdir -p "$SHIMS_DIR"
# The generator's status is read directly rather than through a pipe, so a partial write cannot
# pass the size check below as if it were a complete shim set.
python ports/artemis/reverse/wasm-export-shims.py "$GEN_H" "$SHIMS_DIR/artemis-export-shims.c" \
  > "$WORK/shims.log" 2>&1 || {
  echo "::error::wasm-export-shims.py failed on $GEN_H"
  tail -20 "$WORK/shims.log"
  exit 1
}
sed 's/^/     /' "$WORK/shims.log"
[ -s "$SHIMS_DIR/artemis-export-shims.c" ] || {
  echo "::error::wasm-export-shims.py produced no shims from $GEN_H"
  exit 1
}

echo
echo "== building the bridge =="
# An absolute output path is required, and the directory is cleared so a stale bridge from an earlier
# run cannot be mistaken for this run's output.
OUT_DIR="$REPO_ROOT/dist/artemis-bridge"
rm -rf "$OUT_DIR/build"
mkdir -p "$OUT_DIR/build"
bash ports/artemis/reverse/build-bridge-route.sh "$FLAVOUR" "$OUT_DIR/build"

BRIDGE="$OUT_DIR/build/bridge.wasm"
[ -s "$BRIDGE" ] || {
  echo "::error::no bridge.wasm produced"
  ls -la "$OUT_DIR/build" 2>&1 | head -30 || true
  exit 1
}

# `build-bridge-route.sh` reports the export-by-export agreement while it builds, but its exit status
# reflects the build, not the verdict, so re-run the differ here. Its status is read directly and its
# output kept in a file: piping into `tail` would return tail's status and hide a differ.
echo
echo "== asserting the rebuilt module still behaves like the official one =="
node ports/artemis/reverse/wasm-diff.mjs "$TARGET" "$BRIDGE" > "$OUT_DIR/build/wasm-diff.txt" 2>&1 || {
  echo "::error::the rebuilt module no longer behaves like the official one"
  tail -30 "$OUT_DIR/build/wasm-diff.txt"
  exit 1
}
tail -4 "$OUT_DIR/build/wasm-diff.txt"

echo
echo "== artifacts =="
ls -la "$OUT_DIR/build"
