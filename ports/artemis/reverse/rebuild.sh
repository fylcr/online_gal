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
#          The third part closes the loop: the transpiled C is linked back INTO a
#          wasm binary, and both modules are then executed export by export and
#          compared (wasm-diff.mjs). It cannot be byte-identical -- one is a clang
#          build of generated C -- so the check is behavioural instead, and it is
#          run only when emcc is on PATH. wasm-import-stubs.py and
#          wasm-export-shims.py supply the two pieces that a bare module needs:
#          definitions for the module's imports, and `export_name` wrappers so the
#          rebuilt module's export list reads like the original's.
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
# The `wasm` half additionally links the transpiled C back into a wasm binary and
# compares behaviour, which needs emcc (the emsdk) and node; that one assertion is
# skipped with a note when emcc is not on PATH.

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

# Compile the transpiled C BACK into a wasm binary and compare it against the
# original by execution. This is the assertion that turns "wasm2c output compiles"
# into "wasm2c output rebuilds a working module".
#
# The two binaries can never be byte-equal -- the original is Emscripten's release
# build, the rebuild is clang's codegen for generated C -- so the comparison is
# behavioural: instantiate both, call every export they share with identical
# arguments, and compare what comes back. wasm-diff.mjs reports a mismatch as a
# failure and a mere trap-kind difference (indirect call into a table slot that
# clang left empty) separately, since that one is a property of how full each
# module's table is rather than of the transpilation.
#
# `--export-all -sEXPORT_ALL` is load-bearing: without it Emscripten dead-strips
# everything the JS glue does not reference and the rebuild comes out 4 KB with no
# exports at all, which the differ then reports as "0 shared exports".
verify_wasm_roundtrip() {
    local flavour="$1" src="$2"
    local cdir="$OUT/$flavour"
    local stubs="$OUT/$flavour-import-stubs.c"
    local shims="$cdir/artemis-export-shims.c"
    local rebuilt="$OUT/$flavour-rebuilt.wasm"

    if [ -z "$EMCC" ] || [ -z "$NODE" ] || [ -z "$PYTHON" ]; then
        note "   skipping rebuild-to-wasm for $flavour: needs emcc, node and python3 on PATH"
        return
    fi
    [ -f "$cdir/artemis.h" ] || { bad "$flavour: $cdir/artemis.h missing -- run reverse.sh wasm first"; return; }

    "$PYTHON" "$HERE/wasm-import-stubs.py" "$cdir/artemis.h" "$stubs" >"$OUT/$flavour-stubs.log" 2>&1 || {
        bad "$flavour: wasm-import-stubs.py failed (see $OUT/$flavour-stubs.log)"
        sed 's/^/        /' "$OUT/$flavour-stubs.log" | head -5
        return
    }
    "$PYTHON" "$HERE/wasm-export-shims.py" "$cdir/artemis.h" "$shims" >"$OUT/$flavour-shims.log" 2>&1 || {
        bad "$flavour: wasm-export-shims.py failed (see $OUT/$flavour-shims.log)"
        sed 's/^/        /' "$OUT/$flavour-shims.log" | head -5
        return
    }

    note "   linking the transpiled C back into wasm with emcc"
    if ! "$EMCC" -O0 -Wno-c23-extensions -Wl,--export-all -sEXPORT_ALL \
        -I "$HERE/build" -I "$cdir" \
        "$cdir/artemis.c" "$stubs" "$shims" \
        "$HERE/build/wasm-rt-impl.c" "$HERE/build/wasm-rt-mem-impl.c" \
        -o "$OUT/$flavour-rebuilt.js" 2>"$OUT/$flavour-rebuild.log"; then
        bad "$flavour: could not link the transpiled C back into wasm (see $OUT/$flavour-rebuild.log)"
        return
    fi
    if [ ! -f "$rebuilt" ]; then
        bad "$flavour: link reported success but $rebuilt is missing"
        return
    fi

    if "$NODE" "$HERE/wasm-diff.mjs" "$src" "$rebuilt" >"$OUT/$flavour-diff.txt" 2>&1; then
        ok "$flavour: $(grep -m1 '^agreement:' "$OUT/$flavour-diff.txt")"
        # Say plainly how much of that agreement is real. An export that traps on
        # both sides under zero-value arguments has been checked for "still reaches
        # the same kind of failure" and nothing more, so a run where most exports
        # trap is weaker evidence than the number suggests and is worth flagging.
        local trappy
        trappy="$(sed -n 's/.*trap-kind-only: \([0-9]*\).*/\1/p' "$OUT/$flavour-diff.txt" | head -1)"
        if [ -n "$trappy" ] && [ "$trappy" -gt 0 ]; then
            note "        of which $trappy only match in that both trap under zero arguments"
        fi
    else
        bad "$flavour: rebuilt module does not behave like the original"
        sed -n '/^agreement:/,$p' "$OUT/$flavour-diff.txt" | sed 's/^/        /'
        grep '^  DIFFER' "$OUT/$flavour-diff.txt" | head -5 | sed 's/^/        /'
    fi

    verify_replacement_verdict "$flavour" "$rebuilt"
}

# The round-trip above answers "is the transpiled code the same code?".  It does not
# answer "could this file be dropped where the official one is?", and those are genuinely
# different questions -- a module can be faithfully transpiled and still be unusable,
# because the official module is only half an engine.  The other half is the JS glue:
# it supplies 195-198 host functions through an import named `a` and reads a fixed set
# of short names back off the instance.  wasm-replace-test.mjs walks that loader contract
# in four gates and reports both questions separately.
#
# The expected verdict for a rebuilt module is the one thing that must not drift: it is
# *supposed* to be "faithful but not a replacement", because the transpile compiles the
# host ABI in as zero-returning stubs rather than importing it.  So this assertion fails
# if the verdict changes in either direction -- if a rebuild starts claiming to be
# drop-in that is a bug in the tester, and if the transpile stops being faithful that is
# a real regression.  It is checked, not assumed.
verify_replacement_verdict() {
    local flavour="$1" rebuilt="$2"
    local json="$OUT/$flavour-replace.json"

    # The contract has to come from the *matching* official build. The two flavours have
    # different loader contracts -- plain exports its table as `Ic` and has 196 imports,
    # asyncify as `Lc` with 199 -- so pointing both at one of them makes the other's
    # rebuilt module look like it stopped being faithful when nothing changed but the
    # yardstick. That mistake is what the plain control caught.
    local sdk wasm js
    sdk="$SDK"
    if [ "$flavour" = plain ]; then
        wasm="$sdk/WebAssembly/artemis.wasm"
        js="$sdk/WebAssembly/artemis.js"
    else
        wasm="$sdk/WebAssembly/asyncify/artemis.wasm"
        js="$sdk/WebAssembly/asyncify/artemis.js"
    fi
    if [ ! -f "$wasm" ] || [ ! -f "$js" ]; then
        bad "$flavour: no official $flavour build under $sdk to take the contract from"
        return
    fi

    "$NODE" "$HERE/wasm-replace-test.mjs" --json --official "$wasm" --glue "$js" \
        "$rebuilt" >"$json" 2>"$OUT/$flavour-replace.log" || true

    local read verdict faithful dropin
    read=$(sed -n 's/.*"logicFaithful": *\(true\|false\).*/\1/p' "$json" | head -1)
    dropin=$(sed -n 's/.*"dropIn": *\(true\|false\).*/\1/p' "$json" | head -1)
    if [ -z "$read" ] || [ -z "$dropin" ]; then
        bad "$flavour: wasm-replace-test.mjs produced no verdict (see $OUT/$flavour-replace.log)"
        sed 's/^/        /' "$OUT/$flavour-replace.log" | head -5
        return
    fi

    if [ "$read" = true ] && [ "$dropin" = false ]; then
        ok "$flavour: not a drop-in replacement, as expected -- engine logic faithful, loader contract not met"
    elif [ "$read" = false ]; then
        bad "$flavour: the rebuilt module is no longer even logic-faithful"
        sed -n 's/.*"g3": *\(.*\)/\1/p' "$json" | head -1 | sed 's/^/        /'
    else
        bad "$flavour: wasm-replace-test.mjs now reports it as a drop-in replacement -- the tester or the build changed"
    fi
}

# The emsdk and node are only needed by the round-trip assertion above; resolve
# them once. `emcc` is on PATH only after the user has sourced the emsdk, so this
# is a soft requirement: the assertion is skipped rather than failed when absent.
EMCC="$(command -v emcc || true)"
NODE="$(command -v node || true)"

# `command -v python3` is not enough on Windows: if Python came from the Store, the
# name resolves to an App Execution Alias under WindowsApps that does nothing at all
# when run non-interactively -- no output, no error, non-zero status. It passes
# `command -v` and then fails every invocation, so each candidate is asked for its
# version as well.
find_python() {
    local cand
    for cand in python3 python; do
        if command -v "$cand" >/dev/null 2>&1 && "$cand" --version >/dev/null 2>&1; then
            command -v "$cand"
            return 0
        fi
    done
    return 1
}
PYTHON="$(find_python || true)"

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

    verify_wasm_roundtrip "$flavour" "$src"
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
