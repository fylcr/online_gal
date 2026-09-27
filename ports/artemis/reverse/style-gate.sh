#!/bin/bash
# style-gate.sh — assert the rewritten engine's hand-written code carries no
# wasm2c fingerprints and follows the artemis-compat house style
# (ports/artemis/REWRITE.md §2/§3).
#
# Scan scope: hand-written code under <tree>/src ONLY.  Docs and mappings
# (REWRITE.md, rewrite-names.tsv) name mechanical symbols by definition — the
# clear-list and the rename table must show the old names — so they are out of
# scope.  engine_context.h is the designated vocabulary boundary (the one code
# file allowed to spell mechanical names) and generated/ is generator-owned;
# both are pruned below.
#
# usage: bash ports/artemis/reverse/style-gate.sh [tree]
#        tree default: <repo>/ports/artemis (its src/ is the product tree)
# exit 0 = pass; 1 = fingerprint/dependency failure; warnings do not fail
# unless STYLE_GATE_STRICT=1 (bare pool-ish literals, pragma once misses).
set -u
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# Resolved from this script, not hardcoded: every other recipe in ports/ works
# wherever the clone lands, and a baked-in absolute path silently gates the
# wrong tree (or nothing at all) on any other machine.
TREE="${1:-$(cd "$HERE/.." && pwd)}"
SRC="$TREE/src"
fail=0
echo "== style-gate: $TREE"

# hand-written code files in scope (file list from find: grep's --include /
# --exclude globs proved unreliable on this box's grep)
FILES=()
if [ -d "$SRC" ]; then
  while IFS= read -r f; do FILES+=("$f"); done < <(
    find "$SRC" \( -name generated -o -name engine_context.h \) -prune -o \
      \( -name '*.h' -o -name '*.cpp' -o -name '*.hpp' \) -print 2>/dev/null)
fi
echo "     ${#FILES[@]} hand-written file(s) in scope"

# An empty scope is a FAILURE, not a pass.  Every check below would report "ok"
# against zero files, so a mistyped tree, a moved tree or a wrong default would
# come back green while checking nothing -- the same failure mode that let a
# product audit report zero defects three times before it found a real one (see
# ports/artemis/REWRITE.md §7.6).  A gate that cannot say "I found nothing to
# look at" is worse than no gate, because it is believed.
if [ "${#FILES[@]}" -eq 0 ]; then
  echo "FAIL  nothing to scan under $SRC"
  echo "      the tree argument is wrong, or the products moved"
  exit 1
fi

scan() {  # scan <grep-pattern>; prints hits, returns 1 on hit
  local hits
  hits=$(grep -InE "$1" "${FILES[@]}" 2>/dev/null)
  if [ -n "$hits" ]; then
    echo "FAIL  fingerprint [$1]:"; echo "$hits" | head -5 | sed 's/^/      /'
    return 1
  fi
  echo "ok    [$1]"
  return 0
}

PATS=(
  'var_[ilpB][0-9]'
  'w2c_'
  'FUNC_PROLOGUE|FUNC_EPILOGUE'
  'goto[[:space:]]+var_B'
  'i32_load|i64_load|f32_load|f64_load|i32_store|_default32'
  '\bUNREACHABLE\b'
  'TRAP\('
  'wasm_rt_'
)
for p in "${PATS[@]}"; do scan "$p" || fail=1; done

# dependency bans (AGENT.md §2): render/ must not include script/; script/*.h must
# not include render/; only host|jni may include core/.
depcheck() {  # depcheck <label> <grep-pattern> <file...>
  local label="$1" pat="$2"; shift 2
  [ "$#" -gt 0 ] || { echo "ok    [dep] $label"; return 0; }
  local bad
  bad=$(grep -InE "$pat" "$@" 2>/dev/null)
  if [ -n "$bad" ]; then
    echo "FAIL  [dep] $label:"; echo "$bad" | head -5 | sed 's/^/      /'; fail=1
  else
    echo "ok    [dep] $label"
  fi
}
in_dir() { for f in "${FILES[@]}"; do case "$f" in "$SRC/$1"/*) echo "$f";; esac; done; }
depcheck 'render -> script' '#include *"script/' $(in_dir render)
depcheck 'script.h -> render' '#include *"render/' $(in_dir script | while read -r f; do case "$f" in *.h) echo "$f";; esac; done)
for d in script render audio pack config util input save; do
  depcheck "src/$d -> core (host|jni only)" '#include *"core/' $(in_dir "$d")
done

# must-hit: namespace artc somewhere in hand-written code
if [ "${#FILES[@]}" -gt 0 ]; then
  grep -lIn 'namespace artc' "${FILES[@]}" 2>/dev/null | head -1 >/dev/null \
    && echo "ok    [must] namespace artc present" \
    || { echo "FAIL  no 'namespace artc' in tree"; fail=1; }
fi

# warnings (STYLE_GATE_STRICT=1 to fail)
warn=0
if [ "${#FILES[@]}" -gt 0 ]; then
  w=$(grep -InE '(^|[^0-9A-Za-z_])[0-9]{6,}u([^0-9A-Za-z_]|$)' "${FILES[@]}" 2>/dev/null)
  if [ -n "$w" ]; then
    echo "WARN  bare pool-ish literal (name it):"; echo "$w" | head -5 | sed 's/^/      /'; warn=1
  fi
  for h in "${FILES[@]}"; do
    case "$h" in *.h) grep -q '#pragma once' "$h" 2>/dev/null || { echo "WARN  no #pragma once: $h"; warn=1; } ;; esac
  done
fi
[ "$warn" = 1 ] && [ "${STYLE_GATE_STRICT:-0}" = 1 ] && fail=1

if [ "$fail" = 0 ]; then echo "== style-gate: PASS =="; exit 0; fi
echo "== style-gate: FAIL =="; exit 1
