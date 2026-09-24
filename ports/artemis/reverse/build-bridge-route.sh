#!/usr/bin/env bash
#
# build-bridge-route.sh -- try to make the reverse-engineered engine a *drop-in*
# replacement for the official wasm, rather than a behaviourally-equivalent module
# that the official glue cannot instantiate.
#
# ## Why
#
# `build-cxx-route.sh` answers "can the transpiled semantics be built without wabt"
# and it can.  But the artefact it produces fails three of the four gates in
# `wasm-replace-test.mjs`, and the earlier reading of those failures was that they
# were *structural* -- that a module rebuilt from wasm2c output could never satisfy
# the official glue's loader contract.  Three experiments say otherwise:
#
#   1. `__attribute__((import_module("a"), import_name("xa")))` works: clang emits a
#      real webassembly import under module "a", which is the module the glue builds
#      its import object for.  So the host ABI does not have to be stubbed in; it can
#      be imported, exactly as the official module imports it.
#   2. `-sIMPORTED_MEMORY=1 -Xlinker --import-memory=a,a` makes the module *import*
#      its linear memory as `a.a` -- again exactly what the official module does.
#   3. The wasm2c runtime reads and writes memory through `mem->data[addr]`, and
#      address 0 is the base of linear memory, so pointing `data` at address 0 makes
#      the C runtime operate on the host's memory.  Verified with a canary in both
#      directions (host writes, module reads back; module writes, host sees it).
#
# So the module can be shaped like the official one.  What is left is details: the
# table export has to be called `Lc`, and the memory accessor has to hand back the
# aliased view instead of a private `malloc`ed buffer.
#
# ## What it does differently from build-cxx-route.sh
#
#   * import stubs are generated with `--forward`, so every host function becomes a
#     real wasm import under module "a" instead of a compiled-in zero-returning copy;
#   * the memory accessor is replaced with one that aliases linear memory;
#   * the link imports its memory as `a.a`;
#   * the exported table is renamed to the name the glue looks for.
#
# ## Usage
#
#   bash build-bridge-route.sh [flavour] [outdir]
#
# Environment: EMSDK_DIR may be set; otherwise ports/env.sh finds it.

set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PORTS_ROOT="$(cd "$HERE/../.." && pwd)"
REPO_ROOT="$(cd "$PORTS_ROOT/.." && pwd)"

# A trap in the field is a bare `unreachable` unless something records which code it was.
# Off by default: `--debug-trap` (or ARTC_DEBUG_TRAP=1) links in artc-trap-probe.c, whose
# handler stores the wasm-rt trap code so the embedder can read it back.
# The flag is recognised in *any* position, because the second positional argument is the
# outdir and naming a flag there would otherwise be read as a directory.
DEBUG_TRAP="${ARTC_DEBUG_TRAP:-0}"
PROFILE="${ARTC_PROFILE:-0}"
ARGS=()
for arg in "$@"; do
  case "$arg" in
    --debug-trap) DEBUG_TRAP=1 ;;
    --profile) PROFILE=1 ;;
    *) ARGS+=("$arg") ;;
  esac
done
FLAVOUR="${ARGS[0]:-asyncify}"
case "$FLAVOUR" in
  asyncify|sync) ;;
  *) echo "unknown flavour: $FLAVOUR (expected asyncify or sync)" >&2; exit 2 ;;
esac
if [ "$FLAVOUR" = "asyncify" ]; then RDIR="asyncify"; TABLE_NAME="Lc"; else RDIR="plain"; TABLE_NAME="Ic"; fi

OFFICIAL_ROOT="${ARTEMIS_OFFICIAL:-$REPO_ROOT/.ref/artemis-official}"
GEN_C="$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.c"
GEN_H="$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm2c.h"
RUNTIME_SRC="$OFFICIAL_ROOT/wasm/runtime"
OUTDIR="${ARGS[1]:-$HERE/out/bridge-$FLAVOUR}"
RT_BUILD="$HERE/build"
SHIMS="$HERE/out/$RDIR/artemis-export-shims.c"

fail() { echo "FAIL: $*" >&2; exit 1; }
note() { printf '\n== %s\n' "$*"; }

for f in "$GEN_C" "$GEN_H" "$SHIMS" "$RT_BUILD/wasm-rt-impl.c" "$RT_BUILD/wasm-rt-mem-impl.c"; do
  [ -e "$f" ] || fail "missing input: $f"
done

if [ -z "${EMSDK_DIR:-}" ] || [ ! -x "${EMSDK_DIR:-}/upstream/emscripten/em++" ]; then
  # shellcheck source=/dev/null
  . "$PORTS_ROOT/env.sh" 2>/dev/null || true
fi
EMXX="$EMSDK_DIR/upstream/emscripten/em++"
EMCC="$EMSDK_DIR/upstream/emscripten/emcc"
[ -x "$EMXX" ] || fail "em++ not found at $EMXX (set EMSDK_DIR)"
[ -x "$EMCC" ] || fail "emcc not found at $EMCC (set EMSDK_DIR)"

echo "flavour     : $FLAVOUR"
echo "table name  : $TABLE_NAME"
echo "outdir      : $OUTDIR"
mkdir -p "$OUTDIR"

# --- 1. the C++ that em++ can compile ---------------------------------------
# The flags matter more than they look.  The official module imports its runtime
# plumbing -- `_fd_write`, `_exit`, `___clock_gettime`, `_abort` -- from the same
# module "a" the glue builds, and it imports *none* of the WASI module's functions or
# the exception/memory-growth hooks a modern default build adds.  So the goal is not
# "standalone", it is "shaped like the official one":
#
#   * `-sALLOW_MEMORY_GROWTH=0`: growth is what makes Emscripten import
#     `emscripten_notify_memory_growth`, which the glue does not supply.  The *declared
#     maximum* still has to match the official one, though, because the glue hands over
#     a `WebAssembly.Memory` with maximum 65536 pages and the link checks the import
#     against the module's own declaration -- declare 256 and instantiation fails with
#     "memory import has a larger maximum size 65536 than the module's declared maximum
#     256".  `--max-memory` sets that ceiling without asking for growth, so the module
#     declares the official 256/65536 shape while importing no growth hook.
#   * `-fno-exceptions -fno-rtti -sSUPPORT_LONGJMP=0`: these are what introduce
#     `_Unwind_RaiseException` and `_emscripten_throw_longjmp`.  The official imports
#     neither, i.e. it was built without C++ exceptions.
#   * no `-sSTANDALONE_WASM`: that is what moves the file descriptors and clock onto
#     `wasi_snapshot_preview1`, a module the glue has no import object for.
#
# Anything still left over is folded into module "a" and given one of the glue's own
# field names by wasm-satisfy-imports.py, using runtime-imports.json.
# One more clash has to be designed around, and it is the subtle one. The transpiled
# engine is a *guest* of this module: its data segments are written by `init_memories`
# at fixed addresses (1032 up to about 821 KB) and it treats address 0 as the base of
# the linear memory. But emcc's default layout puts **our** data and stack at the bottom
# of that same memory -- `--stack-first` starts the stack at 0. So the guest's startup
# writes straight through the stack and data this module is running on, and `ctors`
# traps with `memory access out of bounds` before anything else can happen. The fix is
# to move our own layout above everything the guest owns:
#
#   --global-base=<high>   put our statics above the guest's data
#   --no-stack-first       and our stack after them, not at address 0
#
# The guest owns two regions, and both have to be cleared:
#
#   * its data segments, written by `init_memories` at fixed addresses;
#   * its **stack**, whose top is the initial value of its first global (6100320 in the
#     asyncify build). The guest's stack grows down from there, so everything below that
#     address belongs to it.
#
# Sizing against the data segments alone leaves our statics sitting inside the guest's
# stack, and the first ctor that uses more than a few hundred bytes of stack writes
# through it -- which is what the browser showed: `unreachable` raised from inside
# `Mc`, reached from `__wasm_call_ctors`, with no import error at all. So the base is
# taken past whichever of the two is higher.
#
# The *initial* memory must not be raised, though: the glue allocates exactly 256 pages,
# and a module declaring more cannot be linked against it. Moving our own layout to the
# top of that 16 MiB and giving the stack a modest slice leaves plenty of room -- the
# guest tops out at 6.1 MB and the relocation only needs a few hundred KB.
GUEST_DATA_TOP=$(grep -oE 'LOAD_DATA\(\(\*instance->w2c_a_a\), [0-9]+u' "$GEN_C" \
  | grep -oE '[0-9]+u' | tr -d u | sort -n | tail -1)
# The initial value of the guest's global 0 is its stack top. The digits have to be taken
# from *after* the `=`, because the signal name itself ends in a digit (`w2c_g0`) and
# pulling every number out of the line yields that trailing 0 as the stack top.
GUEST_STACK_TOP=$(grep -A6 'static void init_globals' "$GEN_C" \
  | grep -oE 'w2c_g0 = [0-9]+u' | grep -oE '[0-9]+u' | tr -d u | head -1)
[ -n "$GUEST_DATA_TOP" ] || fail "could not read the guest's data top from $GEN_C"
[ -n "$GUEST_STACK_TOP" ] || fail "could not read the guest's stack top from $GEN_C"
GUEST_TOP=$(( GUEST_DATA_TOP > GUEST_STACK_TOP ? GUEST_DATA_TOP : GUEST_STACK_TOP ))

# Two tenants share this one linear memory: the transpiled engine and the Emscripten
# runtime this module links. They must be given non-overlapping halves, and the guest's own
# allocator is what decides where its half ends -- it reads the memory descriptor's `size`
# as its heap ceiling. Left at the full 16 MiB the guest sees everything as its own and
# allocates straight over this module's statics and stack; the first visible symptom was a
# trap whose recorded address was `offset=-4, size=0`, i.e. the descriptor itself had been
# overwritten with heap data.
#
# So the layout is: the guest keeps [0, LAYOUT_BASE) and this module takes the rest.
# LAYOUT_BASE is taken off the *top* rather than placed just above the guest's known data
# and stack regions, because the guest's heap base is its stack top and grows upward
# without bound -- anything in the middle of the address space is in its way.
# The guest is not a small tenant: under the official 16 MiB its own working set runs up to
# within a few hundred bytes of the top, so fencing it below a modest reservation starves it
# -- it keeps allocating and runs straight through the fence. The last trap recorded was
# `offset=14680100, len=4, size=14680064`: four bytes past the fence, doing exactly that.
# Sharing 16 MiB between two engines is simply not enough.
#
# So the memory is enlarged rather than divided. The glue sizes its buffer from
# `Module["INITIAL_MEMORY"]`, which the page can set before the glue loads, and the module
# declares a matching minimum; the guest still sees a ceiling through the memory descriptor,
# just a generous one, with this module's layout above it.
TOTAL=${ART_C_MEMORY_BYTES:-$(( 192 * 1024 * 1024 ))}
RESERVE=$(( 4 * 1024 * 1024 ))
STACK_SIZE=$(( 2 * 1024 * 1024 ))
LAYOUT_BASE=$(( (TOTAL - RESERVE) / 65536 * 65536 ))
[ "$LAYOUT_BASE" -gt "$GUEST_TOP" ] || fail "no room: guest owns up to $GUEST_TOP, layout base $LAYOUT_BASE"
export ART_C_GUEST_CEILING="$LAYOUT_BASE"
export ART_C_MEMORY_BYTES="$TOTAL"
echo "     guest owns below $GUEST_TOP (data $GUEST_DATA_TOP, stack $GUEST_STACK_TOP)"
echo "     guest ceiling (what it sees as memory size): $LAYOUT_BASE"
echo "     our layout base: $LAYOUT_BASE (page $((LAYOUT_BASE/65536)) of $((TOTAL/65536))), reserve $((RESERVE/1024/1024)) MiB, stack $((STACK_SIZE/1024/1024)) MiB"


note "1/6  turning the generated C into compilable C++"
SRC="$OUTDIR/artemis.cpp"
{
  echo "// Generated from $(basename "$GEN_C") by build-bridge-route.sh."
  sed 's/\*instance_ptr/*(struct w2c_a**)instance_ptr/g' "$GEN_C"
} > "$SRC"
LEFT=$(grep -v '^//' "$SRC" | grep -c '\*instance_ptr' || true)
[ "$LEFT" = "0" ] || fail "$LEFT uncast *instance_ptr remain"

# --- 1b. optional: record what the guest's bounds checks refuse ---------------
# With two address spaces sharing one linear memory, "an out-of-bounds access" is not an
# answer -- which address the guest tried to reach is. Every guest access goes through
# RANGE_CHECK, so with the probe enabled that macro is given a second job: report its
# arguments before it traps. See instrument-oob.py for why the rewrite lives in a script.
if [ "$DEBUG_TRAP" = "1" ]; then
  python "$HERE/instrument-oob.py" "$SRC" || fail "could not instrument RANGE_CHECK"
fi

# --- 1c. optional: count which functions a real game actually reaches --------
# A hit counter per function, written into the body by a script for the same reason the
# OOB probe is: this is a textual edit to 100 MB of machine-written C++, and the anchor
# (`FUNC_PROLOGUE;`) is a literal that only appears as a first statement.  The point is
# triage for the readable rewrite: ~5,600 unnamed functions is not a transcription job,
# it is a question of which few hundred a game actually uses.
PROFILE_MAP=""
if [ "$PROFILE" = "1" ]; then
  PROFILE_MAP="$OUTDIR/profile-map.tsv"
  NAME_TABLE=""
  for candidate in "$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm.functions.tsv" \
                   "$OFFICIAL_ROOT/wasm/$FLAVOUR/artemis.wasm.warden-index.tsv"; do
    [ -f "$candidate" ] && NAME_TABLE="$candidate" && break
  done
  if [ -n "$NAME_TABLE" ]; then
    python "$HERE/instrument-profile.py" "$SRC" --map "$NAME_TABLE" \
      --out-map "$PROFILE_MAP" || fail "could not instrument function entry"
  else
    python "$HERE/instrument-profile.py" "$SRC" --out-map "$PROFILE_MAP" \
      || fail "could not instrument function entry"
  fi
fi

# --- 2. host imports are forwarded, not stubbed -----------------------------
note "2/6  forwarders for the 198 host functions"
FWD="$OUTDIR/imports-forward.c"
python "$HERE/wasm-import-stubs.py" "$GEN_H" "$FWD" --forward || fail "stub generator failed"
FORWARDED=$(grep -c 'import_module("a")' "$FWD" || true)
echo "     $FORWARDED forwarders"

# --- 3. the memory accessor aliases the host's linear memory ----------------
note "3/6  re-pointing the wasm2c memory accessor at address 0"
python - "$FWD" "$ART_C_GUEST_CEILING" <<'PYEOF'
import re, sys
path = sys.argv[1]
ceiling = sys.argv[2]
text = open(path, encoding="utf-8").read()
# The generated accessor allocates a private buffer with wasm_rt_allocate_memory.
# Every memory read and write inside the module goes through the pointer it returns,
# so for the module to operate on the host's memory this has to return a view of
# linear memory -- whose base is address 0.
#
# The `size` field is the subtle part. The guest's allocator reads it as its own *heap
# ceiling* (`w2c_artemis_f2719` and friends seed the heap from it), so returning the whole
# 16 MiB tells the guest that everything up to the top belongs to it -- and it will happily
# allocate over this module's own statics and stack, which is what the first attempts did.
# Reporting the ceiling instead fences the guest below this module's region: it sees a
# memory that ends where our layout begins, which is exactly the truth inside the guest's
# address space.
new_body = '''wasm_rt_memory_t* w2c_a_a(struct w2c_a* instance) {
  (void)instance;
  /* Address 0 IS the base of linear memory, so `data` has to be 0 -- but a literal
   * `(uint8_t*)0` is a null pointer constant and clang folds any dereference of it
   * to nothing at every optimisation level.  Routing the value through a volatile
   * keeps the pointer opaque without changing which address it denotes. */
  static uint8_t *volatile base;
  static wasm_rt_memory_t view;
  base = (uint8_t*)0;
  view.data = base;
  view.pages = __builtin_wasm_memory_size(0);
  view.max_pages = 65536;
  /* The guest's half ends where this module's layout begins. */
  view.size = __GUEST_CEILING__ull;
  view.page_size = 65536;
  view.is64 = false;
  view.data_end = view.data + view.size;
  return &view;
}'''
pat = re.compile(r"wasm_rt_memory_t\* w2c_a_a\(struct w2c_a\* instance\) \{.*?\n\}", re.S)
if not pat.search(text):
    sys.exit("could not find the w2c_a_a definition to replace")
# Substituted rather than interpolated: the body is a C function full of braces, so an
# f-string would have to escape every one of them.
new_body = new_body.replace("__GUEST_CEILING__", ceiling)
text = pat.sub(new_body.replace("\\", "\\\\"), text, count=1)
open(path, "w", encoding="utf-8", newline="\n").write(text)
print("     accessor replaced")
PYEOF
[ $? -eq 0 ] || fail "memory accessor patch failed"
grep -q '__builtin_wasm_memory_size' "$FWD" || fail "accessor patch did not take"

# --- 4. include bridge + runtime -------------------------------------------
note "4/6  include bridge and the wasm-rt runtime"
# A trap in the field is a bare `unreachable` unless something records which code it was,
# so `--debug-trap` links in a handler that stores it. Off by default: it is a diagnostic.
TRAP_DEFS=""
TRAP_OBJ=""
if [ "$DEBUG_TRAP" = "1" ]; then
  TRAP_DEFS='-DWASM_RT_TRAP_HANDLER=artc_trap_handler'
  # The transpiled engine has a much deeper *C* call tree than the wasm had frames:
  # asyncify splits every function into numbered parts, and each part carries its own
  # FUNC_PROLOGUE, so one wasm frame can add several to the depth counter. The stock
  # limit of 500 is sized for plain wasm frames and trips on a chain the original
  # module runs happily. Raised here (trap code 1 is EXHAUSTION as well as OOB, so
  # this is the same guard that was firing).
  TRAP_DEFS="$TRAP_DEFS -DWASM_RT_MAX_CALL_STACK_DEPTH=100000"
  TRAP_OBJ="$HERE/artc-trap-probe.c"
  # Record the refused address, not just the trap code: with two address spaces in
  # one linear memory, *which* address was out of range is the whole question.
  echo "     trap probe: on"
fi

# The hit counter itself, separate from the trap probe so either can be enabled alone.
# It is only useful in a real run, so `--profile` implies nothing about `--debug-trap`.
PROFILE_OBJ=""
if [ "$PROFILE" = "1" ]; then
  PROFILE_OBJ="$HERE/artc-profile.c"
  echo "     function profiler: on"
fi
INCDIR="$OUTDIR/inc"
mkdir -p "$INCDIR"
printf '#include "artemis.wasm2c.h"\n' > "$INCDIR/artemis.h"
for f in wasm-rt-impl wasm-rt-mem-impl; do
  # -fno-exceptions matters: the runtime is what drags in `_Unwind_RaiseException` and
  # `_emscripten_throw_longjmp`, and the official module imports neither, i.e. it was
  # built with exceptions off.  Compiling the runtime the same way is what makes the
  # rebuilt module's import list match.
  "$EMCC" -c -O0 -x c -fno-exceptions -sDISABLE_EXCEPTION_CATCHING=1 $TRAP_DEFS -I "$RUNTIME_SRC" \
    "$RT_BUILD/$f.c" -o "$OUTDIR/$f.o" \
    2>> "$OUTDIR/runtime.log" || fail "emcc failed on $f.c (see $OUTDIR/runtime.log)"
done
echo "     runtime objects built"

# --- 5. link: import the host's memory, exactly as the official module does --
note "5/6  linking with em++ (importing memory as a.a)"
"$EMXX" -O0 -Wno-c23-extensions --no-entry -sINVOKE_RUN=0 \
  -fno-exceptions -fno-rtti -sDISABLE_EXCEPTION_CATCHING=1 -sSUPPORT_LONGJMP=0 \
  -Wl,--export-all -sEXPORT_ALL -sSTACK_SIZE=$STACK_SIZE \
  -sIMPORTED_MEMORY=1 -Xlinker --import-memory=a,a \
  $TRAP_DEFS \
  -Xlinker --global-base="$LAYOUT_BASE" -Xlinker --no-stack-first \
  -Xlinker --initial-memory=$TOTAL \
  -sALLOW_UNIMPLEMENTED_SYSCALLS=1 \
  -I "$INCDIR" -I "$RUNTIME_SRC" -I "$(dirname "$GEN_H")" \
  "$SRC" "$FWD" "$SHIMS" \
  "$OUTDIR/wasm-rt-impl.o" "$OUTDIR/wasm-rt-mem-impl.o" $TRAP_OBJ $PROFILE_OBJ \
  -o "$OUTDIR/bridge.raw.wasm" 2> "$OUTDIR/link.log"
LINK_RC=$?
if [ "$LINK_RC" -ne 0 ] || [ ! -f "$OUTDIR/bridge.raw.wasm" ]; then
  tail -25 "$OUTDIR/link.log" >&2
  fail "em++ link failed (see $OUTDIR/link.log)"
fi
ls -l "$OUTDIR/bridge.raw.wasm" | awk '{print "     bridge.raw.wasm "$5" bytes"}'

# --- 6. fold the runtime's leftover imports into the glue's module ----------
# Whatever Emscripten still imports beyond the engine's own host ABI has to be given a
# name the glue actually supplies -- `WebAssembly.instantiate` resolves imports by
# (module, field) and the glue only has module "a".  The correspondence table lives in
# runtime-imports.json and was read off the glue's own asmLibraryArg, so it is data
# that can be checked rather than a guess buried in the build.
note "6/7  folding leftover runtime imports into module \"a\""
GLUE="${ARTEMIS_GLUE:-}"
if [ -z "$GLUE" ]; then
  if [ "$FLAVOUR" = asyncify ]; then
    GLUE="$REPO_ROOT/.ref/artemis/WebAssembly/asyncify/artemis.js"
  else
    GLUE="$REPO_ROOT/.ref/artemis/WebAssembly/artemis.js"
  fi
fi
[ -f "$GLUE" ] || GLUE="$REPO_ROOT/ports/artemis/vendor/artemis-sdk/WebAssembly/$([ "$FLAVOUR" = asyncify ] && echo asyncify/ || echo '')artemis.js"
[ -f "$GLUE" ] || fail "no official glue to read the host ABI from"
echo "     contract read from: $GLUE"

# Two passes.  The first resolves what the glue can supply and writes a local stub for
# whatever remains; those stubs are compiled into the module, so the second link has no
# unresolved runtime symbols left to import.  Only then is the import section rewritten,
# because a module that still *imports* a name cannot also define it.
note "6a/7  deriving the runtime stub set"
python "$HERE/wasm-satisfy-imports.py" "$OUTDIR/bridge.raw.wasm" "$OUTDIR/bridge.aliased.wasm" \
  --glue "$GLUE" --map-out "$OUTDIR/runtime-imports.json" --report "$OUTDIR/fold.txt" \
  --emit-stubs "$OUTDIR/runtime-stubs.c" || fail "import folding failed"
sed 's/^/     /' "$OUTDIR/fold.txt"

# Count from the generator's own marker rather than from a guessed identifier shape:
# the definitions carry the *import's* name, which has no common prefix to grep for.
STUB_COUNT=$(grep -c '^/\* was the runtime import ' "$OUTDIR/runtime-stubs.c" 2>/dev/null || true)
STUB_COUNT=${STUB_COUNT:-0}
if [ "$STUB_COUNT" -gt 0 ]; then
  note "6b/7  relinking with $STUB_COUNT local runtime definition(s)"
  "$EMCC" -c -O0 -x c -fno-exceptions -I "$RUNTIME_SRC" "$OUTDIR/runtime-stubs.c" \
    -o "$OUTDIR/runtime-stubs.o" 2>> "$OUTDIR/runtime.log" \
    || fail "compiling the runtime stubs failed (see $OUTDIR/runtime.log)"
  "$EMXX" -O0 -Wno-c23-extensions --no-entry -sINVOKE_RUN=0 \
    -fno-exceptions -fno-rtti -sDISABLE_EXCEPTION_CATCHING=1 -sSUPPORT_LONGJMP=0 \
    -Wl,--export-all -sEXPORT_ALL \
    -sIMPORTED_MEMORY=1 -Xlinker --import-memory=a,a \
    -sALLOW_UNIMPLEMENTED_SYSCALLS=1 \
    -I "$INCDIR" -I "$RUNTIME_SRC" -I "$(dirname "$GEN_H")" \
    "$SRC" "$FWD" "$SHIMS" "$OUTDIR/runtime-stubs.c" \
    "$OUTDIR/wasm-rt-impl.o" "$OUTDIR/wasm-rt-mem-impl.o" \
    -o "$OUTDIR/bridge.raw2.wasm" 2>> "$OUTDIR/link.log" \
    || fail "the second link failed (see $OUTDIR/link.log)"
  mv "$OUTDIR/bridge.raw2.wasm" "$OUTDIR/bridge.raw.wasm"
  python "$HERE/wasm-satisfy-imports.py" "$OUTDIR/bridge.raw.wasm" "$OUTDIR/bridge.aliased.wasm" \
    --glue "$GLUE" --map-out "$OUTDIR/runtime-imports.json" --report "$OUTDIR/fold.txt" \
    || fail "the second folding pass failed"
  sed 's/^/     /' "$OUTDIR/fold.txt"
fi

BEFORE=$(node -e "const fs=require('fs');WebAssembly.compile(fs.readFileSync('$OUTDIR/bridge.raw.wasm')).then(m=>console.log([...new Set(WebAssembly.Module.imports(m).filter(i=>i.module!=='a').map(i=>i.module+'.'+i.name))].join(' ')))")
AFTER=$(node -e "const fs=require('fs');WebAssembly.compile(fs.readFileSync('$OUTDIR/bridge.aliased.wasm')).then(m=>console.log([...new Set(WebAssembly.Module.imports(m).filter(i=>i.module!=='a').map(i=>i.module+'.'+i.name))].join(' ')))")
echo "     non-\"a\" imports before: ${BEFORE:-none}"
echo "     non-\"a\" imports after : ${AFTER:-none}"
[ -z "$AFTER" ] || fail "imports are still outside module \"a\": $AFTER"

# --- 7. match the official memory declaration and table name ----------------
# The declared maximum is checked exactly when linking the imported memory, and wasm-ld
# refuses to raise it while growth is off, so the declaration is corrected on the binary
# -- the same in-place section edit used for the names.
note "7/8  raising the imported memory maximum to the official 65536 pages"
python "$HERE/wasm-rename-import.py" "$OUTDIR/bridge.aliased.wasm" "$OUTDIR/bridge.mem.wasm" \
  --memory-max 65536 || fail "memory maximum fixup failed"

note "8/8  renaming the exported table to $TABLE_NAME"
python "$HERE/wasm-rename-export.py" "$OUTDIR/bridge.mem.wasm" "$OUTDIR/bridge.wasm" \
  __indirect_function_table "$TABLE_NAME" || fail "table rename failed"

echo
echo "OK: $FLAVOUR -> $OUTDIR/bridge.wasm"
echo "    now run: node $HERE/wasm-replace-test.mjs $OUTDIR/bridge.wasm"
