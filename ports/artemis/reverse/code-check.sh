#!/usr/bin/env bash
#
# code-check.sh -- syntax-check one transcribed fragment, or a group of parts.
#
# A fragment (see .ref/artemis-official/code/TRANSCRIBE.md) is a bare function
# definition with no includes.  This prepends the generated file's own preamble
# (includes, macros, forward declarations — everything before the first
# function definition) and hands the pair to em++ -fsyntax-only.
#
# A function split for transcription is a group of parts f<NNN>.partNN.cpp (see
# the "巨函数分片（part）规则" section of TRANSCRIBE.md).  Give all of them in
# order and they are concatenated first, then checked as that one function:
#
#   bash code-check.sh f3744.part01.cpp f3744.part02.cpp f3744.part03.cpp
#
# Single-fragment behaviour is unchanged.  A whole fragment may itself contain
# everything (its own `// tx:` header included) — the `// tx:` / `// tx-part:`
# comment lines are just comments to the compiler.
#
# Usage: bash code-check.sh <fragment.cpp> [<fragment2.cpp> ...] [flavour]
#        (default flavour asyncify; may also be given as FLAVOUR=... in the env)

set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PORTS_ROOT="$(cd "$HERE/../.." && pwd)"
REPO_ROOT="$(cd "$PORTS_ROOT/.." && pwd)"

# Positional flavour is still accepted for the legacy `... <frag> [flavour]`
# form; any argument that is exactly a flavour name is not a fragment path.
FLAVOUR="${FLAVOUR:-asyncify}"
FRAGS=()
for arg in "$@"; do
  case "$arg" in
    asyncify|sync) FLAVOUR="$arg" ;;
    *) FRAGS+=("$arg") ;;
  esac
done
[ "${#FRAGS[@]}" -ge 1 ] || {
  echo "usage: code-check.sh <fragment.cpp> [<fragment2.cpp> ...] [flavour]" >&2
  exit 2
}

GEN_ROOT="$REPO_ROOT/.ref/artemis-official/wasm"
[ "$FLAVOUR" = "sync" ] && RDIR="plain" || RDIR="$FLAVOUR"
GEN_C="$GEN_ROOT/$RDIR/artemis.wasm2c.c"
GEN_H="$GEN_ROOT/$RDIR/artemis.wasm2c.h"
RUNTIME_SRC="$GEN_ROOT/runtime"
[ -e "$GEN_C" ] || { echo "missing: $GEN_C" >&2; exit 1; }
for f in "${FRAGS[@]}"; do
  [ -e "$f" ] || { echo "missing: $f" >&2; exit 1; }
done

if [ -z "${EMSDK_DIR:-}" ] || [ ! -x "${EMSDK_DIR:-}/upstream/emscripten/em++" ]; then
  # shellcheck source=/dev/null
  . "$PORTS_ROOT/env.sh" 2>/dev/null || true
fi
EMXX="$EMSDK_DIR/upstream/emscripten/em++"
[ -x "$EMXX" ] || { echo "em++ not found (set EMSDK_DIR)" >&2; exit 1; }

# everything up to (but excluding) the first function definition = preamble
FIRST_DEF=$(grep -n -m1 '^[a-z0-9_]\+ w2c_artemis_f[0-9]\+(w2c_artemis\* instance' "$GEN_C" | cut -d: -f1)
[ -n "$FIRST_DEF" ] || { echo "cannot locate first definition in $GEN_C" >&2; exit 1; }

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
{
  sed 's/\*instance_ptr/*(struct w2c_a**)instance_ptr/g' "$GEN_C" | head -n "$((FIRST_DEF - 1))"
  cat "${FRAGS[@]}"
} > "$TMP/t.cpp"

# The rewritten engine supplies engine_context.h, so its src/ must be on the
# include path.  Each flavour has its own tree (asyncify under this port's src/,
# sync local and untracked), because wasm2c numbers functions per module and one
# fNNN names a different function in each.  Pointing this at the wrong tree is
# silent -- the check then never sees the header the products include -- so
# ARTEMIS_REWRITE is the only place the tree is named.
REWRITE_TREE="${ARTEMIS_REWRITE:-}"
if [ -z "$REWRITE_TREE" ]; then
  if [ "$FLAVOUR" = "asyncify" ]; then
    REWRITE_TREE="$REPO_ROOT/ports/artemis"
  else
    REWRITE_TREE="$REPO_ROOT/.ref/artemis-official/rewrite-sync"
  fi
fi

"$EMXX" -fsyntax-only -x c++ -Wno-c23-extensions -Wno-undefined-internal \
  -I "$RUNTIME_SRC" -I "$(dirname "$GEN_H")" -I "$REWRITE_TREE/src" "$TMP/t.cpp"
RC=$?
[ "$RC" -eq 0 ] && echo "check OK: ${FRAGS[*]}"
exit "$RC"
