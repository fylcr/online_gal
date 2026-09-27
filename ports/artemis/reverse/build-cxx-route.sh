#!/usr/bin/env bash
#
# build-cxx-route.sh -- build a wasm from the reverse-engineered code with em++
# alone.  No wabt anywhere in the chain: not wasm2wat, not wat2wasm, not wasm2c.
#
# This is route 3 of docs/artemis-wasm-transcription-handoff.md: the generated
# C is treated as C++, the three hand-fixes below are applied, and em++ links
# the result into a wasm.  The point is to prove the reverse-engineered
# semantics can be built without wabt -- wabt is a *conversion* tool, not part
# of a build.
#
# The generated body needs exactly three concessions to become C++:
#
#   1. 198 uses of `*instance_ptr` in the tail-call wrappers.  instance_ptr is
#      `void**` (see the WEAK_FUNC_DECL macro in the generated .c), so
#      `*instance_ptr` is `void*`, and the callee wants `struct w2c_a*`.
#      Illegal in C++, and clang's -fpermissive does NOT downgrade it (only GCC
#      does), so it must actually be fixed.
#   2. The runtime files in build/ are C.  Compile them with `emcc -x c`; do not
#      "fix" wabt's runtime to be C++ -- it has 4 of its own void* assignments.
#   3. The import stubs include "artemis.h" while the generated header is
#      "artemis.wasm2c.h".  Bridge it with a one-line header rather than
#      editing a generated file.
#
# Usage:
#   bash build-cxx-route.sh [flavour] [wasm2c-c] [outdir]
#
#   flavour    asyncify (default) or sync
#   wasm2c-c   the generated C; defaults to the matching artemis-official file
#   outdir     where the intermediates and the wasm land
#
# Environment: EMSDK_DIR may be set; otherwise ports/env.sh finds it.

set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PORTS_ROOT="$(cd "$HERE/../.." && pwd)"
REPO_ROOT="$(cd "$PORTS_ROOT/.." && pwd)"

FLAVOUR="${1:-asyncify}"
case "$FLAVOUR" in
  asyncify|sync) ;;
  *) echo "unknown flavour: $FLAVOUR (expected asyncify or sync)" >&2; exit 2 ;;
esac

# reverse.sh calls the sync module "plain" in its scratch tree, and the official
# SDK keeps it one directory up from the asyncify build.
if [ "$FLAVOUR" = "asyncify" ]; then
  RDIR="asyncify"; OFFICIAL_SUB="asyncify/artemis.wasm"
else
  RDIR="plain";    OFFICIAL_SUB="artemis.wasm"
fi

OFFICIAL_ROOT="${ARTEMIS_OFFICIAL:-$REPO_ROOT/.ref/artemis-official}"
GEN_C="${2:-$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.c}"
GEN_H="$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.h"
RUNTIME_SRC="$OFFICIAL_ROOT/wasm/runtime"
# The product tree is per-flavour: engine_context.h is the vocabulary boundary and
# its forwarders spell generated names, which are numbered per module.  asyncify's
# products are tracked source inside the port; sync's stay a local, untracked tree.
# Callers (code-assemble.sh) pass ARTEMIS_REWRITE to override the default.
REWRITE_TREE="${ARTEMIS_REWRITE:-}"
if [ -z "$REWRITE_TREE" ]; then
  REWRITE_TREE="$REPO_ROOT/ports/artemis"
  [ "$FLAVOUR" = "asyncify" ] || REWRITE_TREE="$OFFICIAL_ROOT/rewrite-sync"
fi
OUTDIR="${3:-$HERE/out/cxx-route-$FLAVOUR}"

STUBS="$HERE/out/$RDIR-import-stubs.c"
SHIMS="$HERE/out/$RDIR/artemis-export-shims.c"
RT_BUILD="$HERE/build"
OFFICIAL_WASM="${ARTEMIS_SDK:-$REPO_ROOT/.ref/artemis}/WebAssembly/$OFFICIAL_SUB"

fail() { echo "FAIL: $*" >&2; exit 1; }
note() { printf '\n== %s\n' "$*"; }

for f in "$GEN_C" "$GEN_H" "$STUBS" "$SHIMS" \
         "$RT_BUILD/wasm-rt-impl.c" "$RT_BUILD/wasm-rt-mem-impl.c"; do
  [ -e "$f" ] || fail "missing input: $f"
done

# --- toolchain -------------------------------------------------------------
if [ -z "${EMSDK_DIR:-}" ] || [ ! -x "${EMSDK_DIR:-}/upstream/emscripten/em++" ]; then
  # shellcheck source=/dev/null
  . "$PORTS_ROOT/env.sh" 2>/dev/null || true
fi
EMXX="$EMSDK_DIR/upstream/emscripten/em++"
EMCC="$EMSDK_DIR/upstream/emscripten/emcc"
[ -x "$EMXX" ] || fail "em++ not found at $EMXX (set EMSDK_DIR)"
[ -x "$EMCC" ] || fail "emcc not found at $EMCC (set EMSDK_DIR)"

echo "flavour : $FLAVOUR"
echo "generated C : $GEN_C"
echo "outdir  : $OUTDIR"
mkdir -p "$OUTDIR"

# --- 1. the C++ that em++ can actually compile ------------------------------
note "1/4  turning the generated C into compilable C++"
SRC="$OUTDIR/artemis.cpp"
{
  echo "// Generated from $(basename "$GEN_C") by build-cxx-route.sh."
  echo "// The only source-level change is a cast in the tail-call wrappers:"
  echo "// each deref of the void** instance pointer now carries the struct type."
  echo "// see the header comment of build-cxx-route.sh for why."
  sed 's/\*instance_ptr/*(struct w2c_a**)instance_ptr/g' "$GEN_C"
} > "$SRC"
FIXED=$(grep -c '(struct w2c_a\*\*)instance_ptr' "$SRC" || true)
# ignore the leading // comment block, which mentions the pattern in prose
LEFT=$(grep -v '^//' "$SRC" | grep -c '\*instance_ptr' || true)
echo "     cast sites fixed : $FIXED"
[ "$LEFT" = "0" ] || fail "$LEFT uncast *instance_ptr remain"
[ "$FIXED" -gt 0 ] || fail "no cast sites found -- pattern changed?"

# The second documented concession (trap-text parity): call_indirect failures
# must surface as the engine's own traps ("function signature mismatch" / "table
# index is out of bounds"), not as abort() -> `unreachable`.  The pre-check in
# func_types_eq_slowpath defers to the real call_indirect instead of trapping;
# see patch-native-traps.py and build/trap-exp/ for the measured texts.
python "$HERE/patch-native-traps.py" "$SRC" || fail "native-trap patch failed"

# --- 2. the include bridge -------------------------------------------------
note "2/4  include bridge for the import stubs"
INCDIR="$OUTDIR/inc"
mkdir -p "$INCDIR"
printf '#include "artemis.wasm2c.h"\n' > "$INCDIR/artemis.h"
echo "     $INCDIR/artemis.h -> artemis.wasm2c.h"

# --- 3. the runtime stays C ------------------------------------------------
note "3/4  compiling the wasm-rt runtime as C"
for f in wasm-rt-impl wasm-rt-mem-impl; do
  "$EMCC" -c -O0 -x c -I "$RUNTIME_SRC" "$RT_BUILD/$f.c" -o "$OUTDIR/$f.o" \
    2>> "$OUTDIR/runtime.log" || fail "emcc failed on $f.c (see $OUTDIR/runtime.log)"
  echo "     $f.o"
done

# --- 4. link ---------------------------------------------------------------
note "4/4  linking with em++ (no wabt in this chain)"
"$EMXX" -O0 -Wno-c23-extensions \
  -Wl,--export-all -sEXPORT_ALL \
  -I "$INCDIR" -I "$RUNTIME_SRC" -I "$(dirname "$GEN_H")" -I "$REWRITE_TREE/src" \
  "$SRC" "$STUBS" "$SHIMS" \
  "$OUTDIR/wasm-rt-impl.o" "$OUTDIR/wasm-rt-mem-impl.o" \
  -o "$OUTDIR/cxx-route.js" 2> "$OUTDIR/link.log"
LINK_RC=$?
if [ "$LINK_RC" -ne 0 ] || [ ! -f "$OUTDIR/cxx-route.wasm" ]; then
  tail -30 "$OUTDIR/link.log" >&2
  fail "em++ link failed (see $OUTDIR/link.log)"
fi
echo "     $OUTDIR/cxx-route.wasm  $(stat -c%s "$OUTDIR/cxx-route.wasm" 2>/dev/null || echo '?') bytes"

# --- behavioural check -----------------------------------------------------
DIFF="$HERE/wasm-diff.mjs"
NODE="$(command -v node || true)"
if [ -n "$NODE" ] && [ -f "$DIFF" ] && [ -f "$OFFICIAL_WASM" ]; then
  note "behavioural diff vs the official module"
  "$NODE" "$DIFF" "$OFFICIAL_WASM" "$OUTDIR/cxx-route.wasm" \
    2>&1 | tee "$OUTDIR/diff.txt" | tail -6
  grep -q 'differ: 0' "$OUTDIR/diff.txt" \
    || fail "behaviour differs -- see $OUTDIR/diff.txt"
  echo "     differ: 0  (trap-kind-only differences are indirect-table fullness, see doc)"
else
  echo
  echo "NOTE: skipped the behavioural diff (need node, wasm-diff.mjs and the official wasm)"
fi

echo
echo "OK: $FLAVOUR  ->  $OUTDIR/cxx-route.wasm"
echo "    built by em++ alone; wabt was not invoked."
