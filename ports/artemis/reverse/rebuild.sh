#!/usr/bin/env bash
# Rebuild the official Artemis binaries from their reversed form, and prove it.
#
# "Reverse-engineer it and rebuild the same binary" is a claim that needs a check,
# not a demonstration. This script runs the check for both targets.
#
#   wasm   wasm2wat emits a .wat from the shipped .wasm; wat2wasm takes it back.
#          The result is compared byte-for-byte against the original. That this
#          comes out identical is not luck: the SDK's build is a stock
#          Emscripten release build, so its section layout is already canonical
#          and wabt's writer reproduces it exactly. The check is still worth
#          running, because the moment either side is compiled with different
#          flags the round-trip stops being identical and the script says so
#          (it exits non-zero) instead of quietly passing.
#
#          The second half of the wasm check is that the transpiled C builds:
#          wasm2c's output is ordinary C, and gcc has to accept ~1.2M lines of it.
#          Compiling is what makes the pseudo-C usable as source rather than just
#          as something to read.
#
#   so     A shared library cannot be rebuilt byte-identically from decompiled
#          pseudo-C -- decompilation loses types, and no compiler reproduces an
#          NDK release build's exact instruction selection. What *can* be checked
#          is the part that matters for interoperation: the dynamic symbol
#          surface. build-so-stub.sh emits a translation unit with the same
#          exported names and compiles it into a .so for the right ABI, and this
#          script diffs the two export lists. Equal lists mean a caller linked
#          against the official binary resolves every symbol against ours too.
#
# Usage:
#   rebuild.sh wasm          reverse (if needed) then verify both wasm builds
#   rebuild.sh so [arch]     verify the export surface of one Android .so
#   rebuild.sh all           everything
#
# Requirements: fetch-wabt.sh and fetch-zig.sh once each; gcc on PATH for the
# wasm half (MinGW works). No NDK is needed -- see the note above zig_target().

set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO="$(cd "$HERE/../../.." && pwd)"
# Same resolution as reverse.sh: the licensed `.ref/artemis` if present, otherwise
# the vendored tree, which keeps only the two libraries and the two wasm builds.
if [ -z "${ARTC_SDK:-}" ]; then
    if [ -d "$REPO/.ref/artemis/WebAssembly" ]; then
        SDK="$REPO/.ref/artemis"
    else
        SDK="$REPO/ports/artemis/vendor/artemis-sdk"
    fi
else
    SDK="$ARTC_SDK"
fi
so_path() {
    local abi="$1"
    if [ -f "$SDK/Android/app/src/main/jniLibs/$abi/libartemis.so" ]; then
        echo "$SDK/Android/app/src/main/jniLibs/$abi/libartemis.so"
    else
        echo "$SDK/Android/$abi/libartemis.so"
    fi
}
OUT="$HERE/out"
WABT="$HERE/../build/wabt/wabt-1.0.42/bin"
CC="${CC:-gcc}"

# The `so` half needs a cross-compiler, and this is the part of the recipe that
# is easiest to get wrong. MinGW's gcc cannot do it: it always links x86-64 COFF,
# so `-shared -fPIC` produces a file with no dynamic symbol table at all and the
# export diff then fails for a reason that has nothing to do with the recipe. It
# also renames every weak symbol to `.weak.<name>.<anchor>`. The emsdk's clang
# cannot do it either -- its LLVM is built with `wasm32 wasm64 x86 x86-64` only,
# so `--target=aarch64-linux-android` dies with "unable to create target".
#
# Zig is used instead: it bundles libc and a linker for both Android ABIs and
# needs no NDK. `-nostdlib` keeps the output to just the translation unit, which
# is all the export surface is about.
#
# fetch-zig.sh downloads the pinned release.
ZIG="$HERE/build/zig-x86_64-windows-0.16.0/zig.exe"
[ -x "$ZIG" ] || ZIG="$HERE/build/zig/zig"
zig_target() {
    case "$1" in
    arm64-v8a) echo aarch64-linux-gnu ;;
    armeabi-v7a) echo arm-linux-gnueabihf ;;
    *) echo "" ;;
    esac
}

pass=0
fail=0
note() { printf '%s\n' "$*"; }
ok() { pass=$((pass + 1)); note "   OK   $*"; }
bad() { fail=$((fail + 1)); note "   FAIL $*"; }

verify_wasm() {
    local flavour="$1" src="$2"
    local wat="$OUT/$flavour.wat" rt="$OUT/$flavour.rt.wasm"

    [ -f "$wat" ] || { bad "$flavour: $wat missing -- run reverse.sh wasm first"; return; }
    note "== $flavour =="

    "$WABT/wat2wasm.exe" "$wat" -o "$rt"
    if cmp -s "$rt" "$src"; then
        ok "$flavour: wat2wasm(wasm2wat(bin)) == bin ($(wc -c <"$src") bytes, sha256 $(sha256sum "$src" | cut -c1-16)…)"
    else
        bad "$flavour: round-trip differs from the original"
        note "        original $(sha256sum "$src" | cut -d' ' -f1)"
        note "        rebuilt  $(sha256sum "$rt" | cut -d' ' -f1)"
    fi

    # wasm2c's C, compiled. A failure here is a real one: the generated code is
    # what a reader would port from, so it has to be a valid C translation unit.
    local cdir="$OUT/$flavour"
    if [ -f "$cdir/artemis.c" ]; then
        note "   compiling $flavour/artemis.c with $CC (this takes minutes on the asyncify build)"
        if "$CC" -c -O0 -I "$HERE/build" -I "$cdir" "$cdir/artemis.c" -o "$OUT/$flavour-artemis.o" 2>"$OUT/$flavour-compile.log"; then
            ok "$flavour: wasm2c output compiles ($(wc -c <"$OUT/$flavour-artemis.o") byte object)"
        else
            bad "$flavour: wasm2c output failed to compile (see $OUT/$flavour-compile.log)"
        fi
    else
        bad "$flavour: $cdir/artemis.c missing -- run reverse.sh wasm first"
    fi
}

verify_so() {
    local arch="$1"
    local src
    src="$(so_path "$arch")"
    local stub="$OUT/export-stub-$arch.c"
    local mine="$OUT/libartemis-$arch-rebuilt.so"
    local theirs="$OUT/export-names-$arch.official.txt"
    local ours="$OUT/export-names-$arch.rebuilt.txt"

    [ -f "$src" ] || { bad "$arch: no such native: $src"; return; }
    note "== $arch =="

    # The official export list, straight from the shipped ELF. llvm-nm is used
    # rather than readelf because it prints one `addr type name` line per symbol
    # and needs no column arithmetic.
    #
    # `A` (absolute) symbols are dropped from both sides: `__bss_start`, `_end`
    # and the rest are synthesised by the linker's script, so no translation unit
    # can define them and their absence says nothing about the recipe. They are
    # the sole difference between the full 13,058 and the comparable list below.
    local nm="$REPO/../emsdk/upstream/bin/llvm-nm.exe"
    [ -x "$nm" ] || nm="/d/emsdk/upstream/bin/llvm-nm.exe"
    [ -x "$nm" ] || { bad "$arch: llvm-nm not found"; return; }
    "$nm" -D --defined-only "$src" |
        awk '$2 ~ /^[TWVDRB]$/ { print $3 }' | sort -u >"$theirs"
    note "   official dynamic symbols: $(wc -l <"$theirs") (excluding linker-synthesised absolute symbols)"

    bash "$HERE/build-so-stub.sh" "$arch" "$stub" >/dev/null
    local zt
    zt="$(zig_target "$arch")"
    if [ -z "$zt" ]; then
        bad "$arch: no cross target known for this ABI"
        return
    fi
    if [ ! -x "$ZIG" ]; then
        bad "$arch: zig not found at $ZIG -- run fetch-zig.sh first"
        return
    fi
    if ! "$ZIG" cc --target="$zt" -shared -fPIC -O0 -nostdlib "$stub" -o "$mine" 2>"$OUT/stub-compile-$arch.log"; then
        bad "$arch: export stub failed to compile (see $OUT/stub-compile-$arch.log)"
        return
    fi
    "$nm" -D --defined-only "$mine" |
        awk '$2 ~ /^[TWVDRB]$/ { print $3 }' | sort -u >"$ours"
    note "   rebuilt dynamic symbols:  $(wc -l <"$ours")"

    # Both directions matter and they fail for different reasons: a name only in
    # `theirs` is a symbol a caller would fail to resolve, and one only in `ours`
    # is a symbol this recipe invented (which a caller could then bind to by
    # accident and get a stub instead of the real implementation).
    #
    # Weak symbols are compared loosely on the `W`/`V` side: a name that is weak
    # in the official library and strong in ours still satisfies every caller, so
    # only the *set* of names is checked, not the binding strength.
    local missing extra
    missing="$(comm -23 "$theirs" "$ours" | wc -l)"
    extra="$(comm -13 "$theirs" "$ours" | wc -l)"
    if [ "$missing" -eq 0 ] && [ "$extra" -eq 0 ]; then
        ok "$arch: rebuilt export surface is identical ($(wc -l <"$ours") symbols)"
    else
        bad "$arch: $missing official symbols missing, $extra extra"
        comm -23 "$theirs" "$ours" | head -5 | sed 's/^/        missing: /'
        comm -13 "$theirs" "$ours" | head -5 | sed 's/^/        extra:   /'
    fi
}

target="${1:-all}"
arch="${2:-arm64-v8a}"

case "$target" in
wasm)
    verify_wasm plain "$SDK/WebAssembly/artemis.wasm"
    verify_wasm asyncify "$SDK/WebAssembly/asyncify/artemis.wasm"
    ;;
so) verify_so "$arch" ;;
all)
    verify_wasm plain "$SDK/WebAssembly/artemis.wasm"
    verify_wasm asyncify "$SDK/WebAssembly/asyncify/artemis.wasm"
    verify_so arm64-v8a
    verify_so armeabi-v7a
    ;;
*)
    echo "usage: rebuild.sh {wasm|so [arch]|all}" >&2
    exit 2
    ;;
esac

note ""
note "== $pass passed, $fail failed =="
[ "$fail" -eq 0 ]
