#!/usr/bin/env bash
# Rebuild the official Artemis module from its transpiled form, on top of the hand-written engine,
# and check that the result is a drop-in replacement for the official one.
#
# WHY THIS IS THE ONLY ARTEMIS BUILD NOW
#
# An earlier script, `build-artemis.sh`, compiled the third-party `artemis-compat` engine -- `artc`
# code from `.ref/artemis-compat` plus a browser host -- into a web deploy artifact. It is retired,
# with `build-wasm-artemis.yml`, `ports/artemis/build/web-deploy.sh` and
# `ports/artemis/engine/0001-web-host.patch`; this one is the only Artemis form that ships.
#
# This one is different in kind from that anyway. It takes the *official* module, transpiles it to C
# with wasm2c, compiles that C, and lets the hand-written rewrite in `ports/artemis/src` (namespace
# `artc`) replace the transpiled bodies function by function. The result has the official module's
# import and export faces -- that is the whole point -- with an engine inside it that can be read and
# edited. That is why its acceptance test is `wasm-replace-test.mjs`'s four gates rather than "it
# compiled".
#
# THE 103 MB INPUT IS GENERATED, NOT DOWNLOADED
#
# `build-bridge-route.sh` needs `wasm/<flavour>/artemis.wasm2c.c` (+ its `.h`, + `wasm/runtime/`).
# Those are not in the repository -- between them they are 160 MB of machine-written C -- and they
# used to live only in a gitignored `.ref/artemis-official/`. They do not have to: wasm2c is
# deterministic, so regenerating them from the vendored module with the pinned wabt produces the
# same bytes. The md5s in `.github/upstream-pins.json` are the reference values, and they are
# asserted below, because a silently different transpile is the one failure mode that would still
# make every gate pass.
#
# ONE DETAIL THAT IS LOAD-BEARING
#
# wasm2c derives the emitted `#include "artemis.wasm2c.h"` from the `-o` BASENAME. Naming the output
# anything else produces a file that differs by ~30 bytes (the include) and then does not compile.
# The output name is therefore not a preference: it must be `artemis.wasm2c.c`.
#
# WHERE IT DIFFERS FROM THE DEV-MACHINE ROUTE
#
#   * wabt is built from source at the pinned commit instead of using the release tarball
#     `fetch-wabt.sh` downloads. The committed `ports/artemis/build/wabt/` tree is a Windows build
#     (`.exe` binaries) and is gitignored anyway.
#   * `build-bridge-route.sh` calls bare `python` throughout; a `python -> python3` shim goes on PATH
#     for this job, exactly as in the ONS build.
set -eu

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

: "${EMSDK_DIR:=${EMSDK:-$REPO_ROOT/.ref/emsdk}}"
export EMSDK_DIR
WABT_COMMIT="02418f72a70466b45805891843f114c7f602eb34"
FLAVOUR="${BRIDGE_FLAVOUR:-asyncify}"
# `case` rather than an `if`, because anything that is not one of the two names is an error here.
# `build-bridge-route.sh` rejects an unknown flavour too, but only at its own line 67 -- i.e. after
# this script has spent ten minutes building wabt and transpiling the module. And a *blank*
# BRIDGE_FLAVOUR must fall back to the default rather than reach the route script as "".
case "$FLAVOUR" in
  asyncify|sync) ;;
  *) echo "::error::unknown BRIDGE_FLAVOUR '$FLAVOUR' (expected asyncify or sync)"; exit 1 ;;
esac
TARGET="ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm"
[ "$FLAVOUR" = "sync" ] && TARGET="ports/artemis/vendor/artemis-sdk/WebAssembly/artemis.wasm"
# `build-bridge-route.sh` calls the non-asyncify flavour "sync" but names its output directory
# "plain" (see its own RDIR/`build-bridge-route.sh:70`), and the shims directory below follows that
# spelling rather than the flavour's. The two must not be conflated: the directory is `plain`.
if [ "$FLAVOUR" = "asyncify" ]; then RDIR="asyncify"; else RDIR="plain"; fi

# `$WORK` is scratch, not output: it holds a wabt checkout, the regenerated transpile and the
# runtime headers. It is deliberately placed under the runner's own temp directory rather than in
# the checkout, because the checkout's `build/` is NOT gitignored (only `dist/`, `.ref/` and the
# per-port `out/` are), so writing there would leave untracked files behind for any later step that
# inspects the tree. RUNNER_TEMP is set on Actions runners; the `/tmp` fallback is for running this
# locally.
WORK="${RUNNER_TEMP:-/tmp}/artemis-bridge"
OFFICIAL_ROOT="$WORK/official"
WABT_SRC="$WORK/wabt"
GEN_C="$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.c"
GEN_H="$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.h"

# `build-bridge-route.sh` reads its inputs from `$ARTEMIS_OFFICIAL`, whose dev-machine default is
# the gitignored `.ref/artemis-official/`. On a clean runner that directory does not exist, and it
# does not need to: everything it held that this script needs is generated into `$OFFICIAL_ROOT`
# above -- the transpile, its header, and `wasm/runtime/`. `ARTEMIS_OFFICIAL` is exported to point
# there, so the route script and the shim generator both read the tree this run just produced
# instead of a developer's. Nothing else under `.ref/` is required.
#
# Exported rather than passed as an argument because `build-bridge-route.sh` resolves this path
# itself (`build-bridge-route.sh:72`) and takes only the flavour and the output directory.
export ARTEMIS_OFFICIAL="$OFFICIAL_ROOT"

echo "## repo:        $REPO_ROOT"
echo "## flavour:     $FLAVOUR"
echo "## module:      $TARGET"
echo "## EMSDK_DIR:   $EMSDK_DIR"
echo "## scratch:     $WORK"
echo "## official:    $OFFICIAL_ROOT  (generated; not .ref/artemis-official)"

[ -s "$TARGET" ] || { echo "::error::missing vendored module $TARGET"; exit 1; }
[ -d ports/artemis/src ] || { echo "::error::missing ports/artemis/src (the rewritten engine)"; exit 1; }
# Checked before the wabt build rather than after: `build-bridge-route.sh` also tests for em++
# (build-bridge-route.sh:102-109) but it is invoked at the very end, so without this a wrong
# EMSDK_DIR would first cost ten minutes of wabt compilation and a full transpile.
[ -x "${EMSDK_DIR:-}/upstream/emscripten/em++" ] || {
  echo "::error::no em++ at ${EMSDK_DIR:-<unset>}/upstream/emscripten/em++ -- is EMSDK_DIR right?"
  exit 1
}

# --- python shim ------------------------------------------------------------
#
# `build-bridge-route.sh` and the helpers it calls invoke bare `python` throughout. Ubuntu ships
# only `python3`, so a shim goes first on PATH. Resolved explicitly rather than with
# `ln -s "$(command -v python3)"`, which under `set -e` aborts the whole script with no message if
# the name is missing, and prefers an interpreter that actually runs (the Windows Store alias
# answers to `python3` and then fails every invocation -- see rebuild.sh's own find_python()).
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

# --- build wabt's wasm2c at the pinned commit ------------------------------
#
# `-DBUILD_TESTS=OFF -DBUILD_LIBWASM=OFF` keeps this to the tools; `wasm2c` is the only target built.
#
# wabt is NOT submodule-free, contrary to what this comment used to claim: `src/sha256.cc` includes
# `third_party/picosha2/picosha2.h`, and without it the build stops at
# `fatal error: picosha2.h: No such file or directory`. Only that one submodule is needed -- with
# tests and libwasm off, the other six (gtest, ply, simde, testsuite, uvwasi, wasm-c-api) are not
# compiled, and a tree reduced to picosha2 alone produces the same bytes. So this is `--init
# third_party/picosha2` and deliberately not `--recursive`, which would fetch ~6 unused checkouts.
#
# The clone is shallow but must be re-fetched at the exact commit: a shallow `--branch main` drifts,
# and a wabt that is merely "the same version" does not reproduce the reference decompile -- tag
# 1.0.42 itself differs by 24 bytes (see the header of ports/artemis/reverse/gen-wasm2c.sh).
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

# --- transpile --------------------------------------------------------------
#
# The output basename is fixed (see the header). `-n artemis` sets the module prefix the generated C
# and its `.h` use, and the build-bridge-route script's symbol references depend on it.
echo
echo "== transpiling $FLAVOUR =="
mkdir -p "$(dirname "$GEN_C")" "$OFFICIAL_ROOT/wasm/runtime"
"$WASM2C" "$TARGET" -n artemis -o "$GEN_C"
[ -f "$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.h" ] || {
  echo "::error::wasm2c did not emit the sibling .h beside $GEN_C"
  exit 1
}

# The `.h` does `#include "wasm-rt.h"`, which wasm2c does NOT emit: it ships in wabt's own
# `wasm2c/` directory. Copying them here is what lets the generated pair compile without reaching
# back into the wabt checkout.
for header in wasm-rt.h wasm-rt-impl.h wasm-rt-impl.c wasm-rt-mem-impl.c \
              wasm-rt-exceptions.h wasm-rt-exceptions-impl.c \
              wasm-rt-impl-tableops.inc wasm-rt-mem-impl-helper.inc; do
  cp "$WABT_SRC/wasm2c/$header" "$OFFICIAL_ROOT/wasm/runtime/$header"
done
cp "$WABT_SRC/wasm2c/README.md" "$OFFICIAL_ROOT/wasm/runtime/README.wasm2c.md"

# --- assert the transpile is the reference one ------------------------------
#
# Byte-identical, not merely similar: wasm2c is deterministic for a fixed input module and wabt
# commit, so any difference means the input or the tool moved, and the manifest must not describe a
# module built from a transpile nobody has measured.
#
# Both the `.c` and the `.h` are checked. They are separate artifacts with separate hashes, and the
# `.h` is the one `build-bridge-route.sh` takes the export surface from -- so a matching `.c` with a
# moved `.h` would still be a build nobody measured. The values are the same four constants
# `ports/artemis/reverse/gen-wasm2c.sh:103-106` verifies against (`REF_ASYNCIFY_C_MD5` and
# friends); they are duplicated here rather than sourced because that script is a local
# walkthrough which also writes into a `.ref/` destination this job must not touch.
echo
echo "== verifying the transpile =="
EXPECTED_BYTES=107593501
EXPECTED_MD5=d895a74b11ba767b06c82d722c407517
EXPECTED_H_MD5=306f97d1e6fa4d64ea0e5dc720dee7ab
if [ "$FLAVOUR" = "sync" ]; then
  EXPECTED_BYTES=47741355
  EXPECTED_MD5=080822b6e14345d339312d1e1bcfd0aa
  EXPECTED_H_MD5=88567995b73211a885d218514ada897e
fi
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

# --- the two gitignored inputs `build-bridge-route.sh` insists on ------------
#
# It reads its runtime translation units from `ports/artemis/reverse/build/` (its `RT_BUILD`), which
# is gitignored and empty on a clean clone, and its export shims from
# `ports/artemis/reverse/out/<flavour>/artemis-export-shims.c`, likewise gitignored. Both are
# products of the dev-machine walkthrough, not of anything in the repository, so they are made here.
#
# The runtime files must be wabt's originals: `build-bridge-route.sh` compiles them with `-x c`, and
# that is load-bearing -- em++ as C++ rejects four pointer casts in `wasm-rt-impl-tableops.inc`
# (`table->data = calloc(...)` and `= new_data`), which is exactly why the runtime is kept C rather
# than being folded into the C++ build.
echo
echo "== staging the wasm-rt runtime sources where build-bridge-route.sh looks =="
RT_BUILD="$REPO_ROOT/ports/artemis/reverse/build"
mkdir -p "$RT_BUILD"
for f in wasm-rt-impl.c wasm-rt-mem-impl.c; do
  cp "$WABT_SRC/wasm2c/$f" "$RT_BUILD/$f"
  echo "   $RT_BUILD/$f"
done

# The export shims re-expose the wasm2c exports under the original module's names, so the rebuilt
# module's export surface reads like the official one instead of like `w2c_artemis_*`.
#
# `rebuild.sh wasm` is NOT called to make these, although it is the dev-machine step that does.
# Its wasm half invokes `"$WABT/wat2wasm.exe"` unconditionally at rebuild.sh:275, so it cannot run
# on this runner at all, and its `so` half is Windows-only too (a zig Windows build and
# `llvm-nm.exe`). Rather than depend on it, the shims are generated directly with the same script it
# would have used -- `wasm-export-shims.py`, fed the `.h` just generated. The only difference in the
# output is the `#include` line, which names the header by basename (`artemis.wasm2c.h` here,
# `artemis.h` when rebuild.sh runs) and resolves either way: `build-bridge-route.sh` puts both the
# generated header's directory and its own `inc/` on the include path.
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
# An absolute output path is required: the script writes ~100 MB of intermediates and a relative
# path has failed on this project before (a Windows `Errno 22` recorded in
# ports/artemis/reverse/p2-batch1-names.md). The directory is cleared first so a stale bridge from a
# different flavour cannot be mistaken for this run's output.
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

# --- the drop-in assertion, independently of what build-bridge-route.sh printed ---
#
# That script already reports the compare-every-export agreement while it builds, but its exit
# status reflects the build, not the verdict. Re-running the differ here is what makes the workflow
# fail on a module that assembled cleanly and stopped behaving like the original.
#
# `wasm-diff.mjs` signals the verdict through its exit status (non-zero on any differ, on an
# incomplete export surface, or on any trap-kind difference under strict), so its status is read
# directly and its output is kept in a file rather than piped: `node ... | tail` would return
# *tail's* status, and a differ would then pass unnoticed.
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
