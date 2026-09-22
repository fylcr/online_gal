#!/usr/bin/env bash
# Reverse the official Artemis binaries to pseudo-C.
#
# Two independent targets, because they are two different toolchains:
#
#   wasm   the SDK's own WebAssembly builds (plain + asyncify) are reversed with
#          wabt: wasm2wat for a readable .wat and wasm2c for compilable C. Both
#          round-trip back to a byte-identical .wasm with wat2wasm, which is what
#          makes "we can rebuild the same binary" a checked claim rather than a
#          hope. See rebuild.sh for that half.
#
#   so     the Android natives (arm64-v8a and armeabi-v7a libartemis.so) carry
#          real dynamic symbols -- 13k on arm64 -- so they are the oracle for the
#          wasm build's minified a.b/a.c names. Ghidra headless decompiles every
#          function to one .c file each.
#
# Usage:
#   reverse.sh wasm          disassemble + transpile both official wasm builds
#   reverse.sh so [arch]     Ghidra-decompile one Android .so (default arm64-v8a)
#   reverse.sh so-all        both Android .so files
#
# Outputs land in ports/artemis/reverse/out/ (gitignored: the SDK is proprietary
# and its reverse-engineered dumps are derivative works of it).

set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/../../.." && pwd)"
# The vendored copy is the tracked one, so a fresh clone can run this. `.ref/artemis`
# is the licensed drop-in and wins if it happens to be present.
if [ -z "${ARTC_SDK:-}" ]; then
    if [ -d "$REPO/.ref/artemis/WebAssembly" ]; then
        SDK="$REPO/.ref/artemis"
    else
        SDK="$REPO/ports/artemis/vendor/artemis-sdk"
    fi
else
    SDK="$ARTC_SDK"
fi
OUT="$HERE/out"

# wabt is fetched by fetch-wabt.sh; the version matters because the .wat it emits
# is what wat2wasm has to accept back byte-for-byte.
WABT="$HERE/../build/wabt/wabt-1.0.42/bin"

GHIDRA="${GHIDRA_HOME:-/d/programe/ghidra}"
JAVA_HOME="${JAVA_HOME:-/c/Program Files/Java/jdk-21}"

target="${1:-wasm}"
arch="${2:-arm64-v8a}"

need_wabt() {
    for t in wasm2wat wat2wasm wasm2c wasm-objdump; do
        [ -x "$WABT/$t.exe" ] || [ -x "$WABT/$t" ] || {
            echo "missing $WABT/$t -- run ports/artemis/reverse/fetch-wabt.sh" >&2
            exit 1
        }
    done
}

# The native lives in a different place in each tree: the SDK ships a full Android
# project (`Android/app/src/main/jniLibs/<abi>/`), while the vendored copy keeps only
# the two libraries (`Android/<abi>/`). Resolve whichever is present.
so_path() {
    local abi="$1"
    if [ -f "$SDK/Android/app/src/main/jniLibs/$abi/libartemis.so" ]; then
        echo "$SDK/Android/app/src/main/jniLibs/$abi/libartemis.so"
    else
        echo "$SDK/Android/$abi/libartemis.so"
    fi
}

case "$target" in
wasm)
    need_wabt
    mkdir -p "$OUT"
    for flavour in plain asyncify; do
        if [ "$flavour" = plain ]; then
            src="$SDK/WebAssembly/artemis.wasm"
        else
            src="$SDK/WebAssembly/asyncify/artemis.wasm"
        fi
        [ -f "$src" ] || { echo "no such official build: $src" >&2; exit 1; }
        echo "== $flavour: $(stat -c%s "$src" 2>/dev/null || wc -c <"$src") bytes =="

        echo "   wasm2wat -> out/$flavour.wat"
        "$WABT/wasm2wat.exe" "$src" -o "$OUT/$flavour.wat"

        echo "   wasm2c  -> out/$flavour/artemis.c"
        mkdir -p "$OUT/$flavour"
        "$WABT/wasm2c.exe" "$src" -o "$OUT/$flavour/artemis.c"

        echo "   wasm-objdump -> out/$flavour.objdump.txt"
        "$WABT/wasm-objdump.exe" -x "$src" >"$OUT/$flavour.objdump.txt"

        echo "   round-trip check"
        "$WABT/wat2wasm.exe" "$OUT/$flavour.wat" -o "$OUT/$flavour.rt.wasm"
        if cmp -s "$OUT/$flavour.rt.wasm" "$src"; then
            echo "   OK: wat2wasm(wasm2wat(bin)) is byte-identical"
        else
            echo "   FAIL: round-trip differs from the original" >&2
            exit 1
        fi
    done
    echo "== done =="
    ;;

so)
    src="$(so_path "$arch")"
    [ -f "$src" ] || { echo "no such native: $src" >&2; exit 1; }
    # Ghidra refuses project paths with a dot-prefixed component, and the MCP
    # server holds its own project open, so headless gets its own.
    proj="$OUT/ghidra-$arch"
    mkdir -p "$proj" "$OUT/pseudo-c/$arch"
    export JAVA_HOME
    export PATH="$JAVA_HOME/bin:$PATH"
    echo "== $arch: Ghidra headless decompile of $(basename "$src") =="
    "$GHIDRA/support/analyzeHeadless" "$proj" artemis \
        -import "$src" \
        -scriptPath "$HERE/ghidra" \
        -postScript DecompileAll.java "$OUT/pseudo-c/$arch" \
        -deleteProject
    echo "== done: $OUT/pseudo-c/$arch =="
    ;;

so-all)
    "$0" so arm64-v8a
    "$0" so armeabi-v7a
    ;;

*)
    echo "usage: reverse.sh {wasm|so [arch]|so-all}" >&2
    exit 2
    ;;
esac
