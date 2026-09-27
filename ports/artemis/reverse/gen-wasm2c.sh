#!/usr/bin/env bash
#
# gen-wasm2c.sh -- regenerate the wasm2c half of `wasm/` from the COMMITTED vendor
# wasm, and prove the result is byte-identical to the reference.
#
# ## Why this exists
#
# `build-bridge-route.sh` needs three things that are not in the repository:
# `wasm/<flavour>/artemis.wasm2c.c`, its `.h`, and `wasm/runtime/*`. They live
# under `.ref/artemis-official/`, and `.ref/` is gitignored -- so a clean clone has
# none of them and the bridge cannot be built. This script closes that gap: the
# INPUT (the official wasm) is committed, and the generator is a pinned commit of
# wabt, so the 103 MB decompile is reproducible rather than something that has to
# be shipped as an opaque artifact.
#
# ## The one thing that must not be got wrong: the `-o` basename
#
# wasm2c writes the sibling header's name into the generated C source, as
#
#     #include "artemis.wasm2c.h"
#
# and it derives that name from the `-o` BASENAME. Pass `-o asyncify.c` and the
# line becomes `#include "asyncify.h"`: the code is otherwise identical, but the
# file is 6 bytes shorter and the md5 does not match. A 30-byte discrepancy that
# looks exactly like a wabt-version difference is really this. There is no flag
# that fixes the include independently, so the ONLY correct invocation is
#
#     wasm2c <src> -n artemis -o <dir>/artemis.wasm2c.c
#
# which is what LOOP below does. Do not "simplify" it to a name of your choosing.
#
# ## Why the tool has to be built, not downloaded
#
# `fetch-wabt.sh` pins the *release* 1.0.42 tarball, which is NOT the commit that
# produced the reference. The reference came from wabt at `1.0.42-8-g02418f72`
# (`1.0.42` plus 8 commits), and the release binary differs from it in the
# generated source. Build wabt at the pinned commit:
#
#     cmake -S "$WABT" -B "$WABT/build" -DCMAKE_BUILD_TYPE=Release \
#           -DBUILD_TESTS=OFF -DBUILD_LIBWASM=OFF
#     cmake --build "$WABT/build" --target wasm2c -j8
#
# Measured cost and the full evidence: see `wasm2c-regen.md` beside this script.
#
# ## Usage
#
#     bash gen-wasm2c.sh
#
# Environment (all optional; env-first, then the in-repo default):
#   WABT              wabt checkout        (default $REPO/.ref/wabt)
#   ARTEMIS_SDK       SDK tree with the wasm -- either the COMMITTED vendor tree
#                     or a licensed `.ref/artemis` download; both have the same
#                     layout and the same bytes (default: the vendor tree, so a
#                     clean clone needs nothing fetched)
#   ARTEMIS_OFFICIAL  output root          (default $REPO/.ref/artemis-official,
#                     which is exactly where build-bridge-route.sh looks)
#
# Output is verified against the reference md5s; any mismatch is a hard failure.

set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PORTS_ROOT="$(cd "$HERE/../.." && pwd)"
REPO_ROOT="$(cd "$PORTS_ROOT/.." && pwd)"

# House style: sibling recipes source ports/env.sh for EMSDK_DIR/REF_DIR. This one
# needs neither, but sourcing it keeps REF_DIR consistent when it is set.
if [ -z "${REF_DIR:-}" ]; then
  # shellcheck source=/dev/null
  . "$PORTS_ROOT/env.sh" 2>/dev/null || true
fi

WABT="${WABT:-${REF_DIR:-$REPO_ROOT/.ref}/wabt}"
SDK="${ARTEMIS_SDK:-$REPO_ROOT/ports/artemis/vendor/artemis-sdk}"
DEST="${ARTEMIS_OFFICIAL:-$REPO_ROOT/.ref/artemis-official}"

fail() { echo "FAIL: $*" >&2; exit 1; }
note() { printf '\n== %s\n' "$*"; }

# wasm2c.exe on Windows, wasm2c elsewhere.
W2C="$WABT/build/wasm2c.exe"
[ -x "$W2C" ] || W2C="$WABT/build/wasm2c"
[ -x "$W2C" ] || fail "no wasm2c at $WABT/build/ -- build it first (see the header of this script)"

[ -d "$SDK/WebAssembly" ] || fail "no SDK at $SDK (no $SDK/WebAssembly)"
[ -d "$WABT/wasm2c" ] || fail "no runtime sources at $WABT/wasm2c"

# The generated .c includes "wasm-rt.h", which wasm2c does NOT emit: it ships
# beside the tool in the wabt tree. Copy the set the reference runtime dir holds.
RUNTIME_FILES=(
  wasm-rt.h
  wasm-rt-impl.h
  wasm-rt-impl.c
  wasm-rt-mem-impl.c
  wasm-rt-exceptions.h
  wasm-rt-exceptions-impl.c
  wasm-rt-impl-tableops.inc
  wasm-rt-mem-impl-helper.inc
)

# Reference hashes, from `.ref/artemis-official/MANIFEST.txt` ("## wasm2c source").
# A mismatch is the whole point of the check: it means the tool or the input moved.
REF_ASYNCIFY_C_MD5="d895a74b11ba767b06c82d722c407517"
REF_ASYNCIFY_H_MD5="306f97d1e6fa4d64ea0e5dc720dee7ab"
REF_SYNC_C_MD5="080822b6e14345d339312d1e1bcfd0aa"
REF_SYNC_H_MD5="88567995b73211a885d218514ada897e"

md5_of() {
  if command -v md5sum >/dev/null 2>&1; then
    md5sum "$1" | cut -d' ' -f1
  elif command -v md5 >/dev/null 2>&1; then
    md5 -q "$1"
  else
    fail "no md5sum or md5 on PATH; cannot verify"
  fi
}

size_of() { wc -c <"$1" | tr -d ' '; }

check() { # check <file> <expected-md5> <expected-size>
  local file="$1" want_md5="$2" want_size="$3"
  local got_md5 got_size
  got_md5="$(md5_of "$file")"
  got_size="$(size_of "$file")"
  if [ "$got_md5" != "$want_md5" ] || [ "$got_size" != "$want_size" ]; then
    printf '   MISMATCH %s\n' "$(basename "$file")" >&2
    printf '            want md5=%s size=%s\n' "$want_md5" "$want_size" >&2
    printf '            got  md5=%s size=%s\n' "$got_md5" "$got_size" >&2
    return 1
  fi
  printf '   OK  %-20s md5=%s  %s B\n' "$(basename "$file")" "$got_md5" "$want_size"
}

note "generator"
printf '  wabt:  %s\n' "$("$W2C" --version)"
if [ -d "$WABT/.git" ]; then
  printf '  commit: %s\n' "$(git -C "$WABT" rev-parse HEAD 2>/dev/null || echo '(not a git checkout)')"
fi
printf '  input SDK: %s\n' "$SDK"
printf '  output:    %s\n' "$DEST"

# Generate into a staging dir first and install only after the check passes. Writing
# straight to $DEST would leave a MISMATCHING file in place of a good one, which is the
# worst possible outcome for a script whose job is to prove the reference.
STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT

for flavour in sync asyncify; do
  if [ "$flavour" = sync ]; then
    src="$SDK/WebAssembly/artemis.wasm"
    ref_c="$REF_SYNC_C_MD5"; ref_h="$REF_SYNC_H_MD5"; size_c=47741355; size_h=35307
  else
    src="$SDK/WebAssembly/asyncify/artemis.wasm"
    ref_c="$REF_ASYNCIFY_C_MD5"; ref_h="$REF_ASYNCIFY_H_MD5"; size_c=107593501; size_h=39858
  fi
  [ -f "$src" ] || fail "no such build: $src"

  note "$flavour"
  printf '  src: %s\n' "$src"
  # The basename is load-bearing -- see the `-o` note in the header. Staging keeps it
  # because only the directory differs.
  "$W2C" "$src" -n artemis -o "$STAGE/artemis.wasm2c.c"

  check "$STAGE/artemis.wasm2c.c" "$ref_c" "$size_c" || fail "$flavour .c does not match the reference"
  check "$STAGE/artemis.wasm2c.h" "$ref_h" "$size_h" || fail "$flavour .h does not match the reference"

  mkdir -p "$DEST/wasm/$flavour"
  mv "$STAGE/artemis.wasm2c.c" "$DEST/wasm/$flavour/artemis.wasm2c.c"
  mv "$STAGE/artemis.wasm2c.h" "$DEST/wasm/$flavour/artemis.wasm2c.h"
done

note "runtime sources <- $WABT/wasm2c"
mkdir -p "$DEST/wasm/runtime"
for f in "${RUNTIME_FILES[@]}"; do
  [ -f "$WABT/wasm2c/$f" ] || fail "no $WABT/wasm2c/$f"
  cp "$WABT/wasm2c/$f" "$DEST/wasm/runtime/$f"
done
# The reference dir also carries README.md under this name.
if [ -f "$WABT/wasm2c/README.md" ]; then
  cp "$WABT/wasm2c/README.md" "$DEST/wasm/runtime/README.wasm2c.md"
fi
printf '   copied %s file(s)\n' "$(ls -1 "$DEST/wasm/runtime" | wc -l | tr -d ' ')"

note "verification"
bad=0
check "$DEST/wasm/asyncify/artemis.wasm2c.c" "$REF_ASYNCIFY_C_MD5" 107593501 || bad=1
check "$DEST/wasm/asyncify/artemis.wasm2c.h" "$REF_ASYNCIFY_H_MD5" 39858 || bad=1
check "$DEST/wasm/sync/artemis.wasm2c.c" "$REF_SYNC_C_MD5" 47741355 || bad=1
check "$DEST/wasm/sync/artemis.wasm2c.h" "$REF_SYNC_H_MD5" 35307 || bad=1
[ "$bad" = 0 ] || fail "regenerated output does not match the reference (see MISMATCH above)"

note "done -- $(printf '%s' "both flavours byte-identical to the reference")"
