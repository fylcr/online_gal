#!/usr/bin/env bash
#
# code-assemble.sh -- splice every transcribed fragment back into the generated
# file, rename the touched functions, then build + behaviour-check via
# build-cxx-route.sh (em++ only, no wabt).  The accept gate for any batch:
# it must end with "differ: 0".
#
# Fragment contract: first line is `// tx: f<NNN> -> <new_name>`.  The original
# definition of each transcribed f<NNN> is cut from the generated file and the
# fragment takes its place; `w2c_artemis_f<NNN>` is then renamed to <new_name>
# everywhere (call sites, forward declarations).  A function too big to
# transcribe in one go is split across files f<NNN>.partNN.cpp (NN zero-padded
# to two digits), concatenated in part order into that one replacement body.
# The `// tx:` line lives only in part01, on its first line; part02+ start with
# a `// tx-part: f<NNN> part <NN>` marker instead (kept in the output), and each
# part may carry its own `// src: <start>-<end>` provenance comment.  Every part
# number from 01 to the last must be present.
#
# Usage: bash code-assemble.sh [flavour] [outdir]

set -uo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PORTS_ROOT="$(cd "$HERE/../.." && pwd)"
REPO_ROOT="$(cd "$PORTS_ROOT/.." && pwd)"

FLAVOUR="${1:-asyncify}"
case "$FLAVOUR" in
  asyncify|sync) ;;
  *) echo "unknown flavour: $FLAVOUR" >&2; exit 2 ;;
esac
# Two directory names, and conflating them is how the sync flavour used to be
# unreachable: reverse.sh calls the sync module "plain" in its out/ scratch tree,
# while the official SDK keeps the generated sources under wasm/sync/.  RDIR is the
# scratch-tree name (used for out/ paths), FLAVOUR is the generated-source name.
if [ "$FLAVOUR" = "asyncify" ]; then RDIR="asyncify"; else RDIR="plain"; fi

GEN_C="$REPO_ROOT/.ref/artemis-official/wasm/$FLAVOUR/artemis.wasm2c.c"
# The transcription archive and the product tree are per-flavour: wasm2c numbers
# functions by their position in ITS OWN module, so fNNN in the asyncify tree
# names a different function in the sync tree (measured: f205 is void/2 there
# and u32/3 here).  asyncify's products are tracked source inside the port;
# sync's stay a local, untracked tree.
CODE_DIR="${ARTEMIS_CODE_DIR:-$REPO_ROOT/.ref/artemis-official/code}"
REWRITE_TREE="${ARTEMIS_REWRITE:-}"
if [ -z "$REWRITE_TREE" ]; then
  if [ "$FLAVOUR" = "asyncify" ]; then
    REWRITE_TREE="$REPO_ROOT/ports/artemis"
  else
    REWRITE_TREE="$REPO_ROOT/.ref/artemis-official/rewrite-sync"
  fi
fi
OUTDIR="${2:-$HERE/out/code-$FLAVOUR}"
SPLICE="$OUTDIR/combined.c"
mkdir -p "$OUTDIR"

echo "flavour        : $FLAVOUR"
echo "generated C    : $GEN_C"
echo "archive dir    : $CODE_DIR"
echo "product tree   : $REWRITE_TREE"

# python on this box only takes native Windows paths (see extract-tag-map notes)
G=$(cygpath -m "$GEN_C"); C=$(cygpath -m "$CODE_DIR"); S=$(cygpath -m "$SPLICE")
R=$(cygpath -m "$REWRITE_TREE")
python - "$G" "$C" "$S" "$R" <<'EOF'
import os, re, sys

gen_c, code_dir, out_path, rw_root = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]

# ---- collect fragments: fnid -> [(part, new_name, filename), ...] ------------
# A whole fragment is fNNN.cpp (`// tx:` header).  A split function is
# fNNN.partNN.cpp, 01..N contiguous: part01 carries the `// tx:` header like a
# whole fragment, part02+ carry a `// tx-part: f<NNN> part <NN>` marker instead.
frag_re = re.compile(r"^f(\d+)(?:\.part(\d+))?\.cpp$")
tx_re = re.compile(r"// tx: f(\d+) -> (art_[a-z0-9_]+)")
part_re = re.compile(r"// tx-part: f(\d+) part (\d+)")
frags, names = {}, {}
for fname in sorted(os.listdir(code_dir)):
    m = frag_re.match(fname)
    if not m:
        continue
    fnid = int(m.group(1))
    part = 0 if m.group(2) is None else int(m.group(2))
    if m.group(2) is not None and part == 0:
        sys.exit("bad part number in %s: parts are numbered from 01" % fname)
    with open(os.path.join(code_dir, fname), encoding="utf-8") as fh:
        head = fh.readline()
    if part <= 1:
        # whole fragment, or part01: the `// tx:` header must be the first line
        tx = tx_re.match(head)
        if not tx:
            sys.exit("bad fragment header: %s -> %r" % (fname, head))
        if int(tx.group(1)) != fnid:
            sys.exit("fragment header ids disagree: %s says f%s, filename says f%d"
                     % (fname, tx.group(1), fnid))
        new_name = tx.group(2)
        if names.setdefault(new_name, fnid) != fnid:
            sys.exit("duplicate new name: %s" % new_name)
    else:
        # part02+: first line is the part marker, not a `// tx:` header
        tp = part_re.match(head)
        if not tp:
            sys.exit("bad part header: %s -> %r" % (fname, head))
        if int(tp.group(1)) != fnid or int(tp.group(2)) != part:
            sys.exit("part header ids disagree: %s -> %r" % (fname, head))
        new_name = None
    frags.setdefault(fnid, []).append((part, new_name, fname))
if not frags:
    # An empty archive is the legitimate starting state of a fresh flavour: the
    # assembly then keeps every generated definition untouched, which is exactly
    # the baseline that proves the route is reachable before any product exists.
    # It stays an error by default so a mistyped CODE_DIR cannot masquerade as one.
    if os.environ.get("ARTEMIS_ALLOW_EMPTY_ARCHIVE") != "1":
        sys.exit("no fragments found in %s (set ARTEMIS_ALLOW_EMPTY_ARCHIVE=1 to "
                 "assemble the un-transcribed baseline)" % code_dir)
    print("no fragments in %s: keeping all %s definitions as the generated baseline"
          % (code_dir, "generated"))

# ---- products take precedence over their code/ transcriptions ---------------
# A fnid whose rewrite product is live（a kind=fn row in rewrite-names.tsv naming
# a file that exists under rewrite/src）is assembled from the product instead of
# its code/ transcription; the transcription stays on disk as the fidelity
# archive.  Dropping the fnid here skips the fragment, the rename layer and the
# alias block for it in one stroke.  The live-product test mirrors rw-splice.py
# exactly, so both sides always agree on who owns a definition.
live = set()
with open(os.path.join(rw_root, "rewrite-names.tsv"), encoding="utf-8") as fh:
    for raw in fh:
        if not raw.strip() or raw.startswith("#"):
            continue
        cols = raw.rstrip("\n").split("\t")
        if len(cols) < 5 or cols[0] != "fn":
            continue
        relfile = cols[4]
        if relfile.endswith((".h", ".hpp")):
            continue
        if os.path.isfile(os.path.join(rw_root, "src", relfile)):
            live.add(int(cols[1].lstrip("f")))
for fnid in sorted(set(frags) & live):
    print("f%-5d -> product takes over; code/ transcription skipped" % fnid)
    del frags[fnid]

# ---- validate each group: part01 present, 01..N contiguous, one `// tx:` ------
# `// tx:` must sit on part01's first line; a group without it is an error, and a
# second `// tx:` anywhere else in the group is an error too.
for fnid in sorted(frags):
    group = sorted(frags[fnid])
    parts = [p for p, _, _ in group]
    files = [f for _, _, f in group]
    if 0 in parts and len(parts) > 1:
        sys.exit("f%d: mixed whole fragment and parts: %s" % (fnid, files))
    if parts[0] == 0:
        want = [0]                     # unsplit fragment: unchanged behaviour
    else:
        if 1 not in parts:
            sys.exit("f%d: part01 of the group is missing (the `// tx:` header sits "
                     "in part01): %s" % (fnid, files))
        want = list(range(1, len(parts) + 1))
    if parts != want:
        sys.exit("f%d: part numbers not contiguous 01..%02d: %s"
                 % (fnid, len(parts), files))
    if parts == [0]:
        # whole fragment: exactly the old behaviour (the header was checked above)
        frags[fnid] = [(n, f) for _, n, f in group]
        continue
    hits = 0
    for _, _, fn in group:
        with open(os.path.join(code_dir, fn), encoding="utf-8") as fh:
            hits += len(tx_re.findall(fh.read()))
    if hits != 1:
        sys.exit("f%d: %d `// tx:` headers in the group -- exactly one (in part01) "
                 "required: %s" % (fnid, hits, files))
    frags[fnid] = [(n, f) for _, n, f in group]

# ---- split the generated file at every function definition ------------------
with open(gen_c, encoding="utf-8", errors="surrogateescape") as fh:
    src = fh.read().splitlines(keepends=True)
defre = re.compile(r"^[a-z0-9_]+ w2c_artemis_f(\d+)\(w2c_artemis\* instance")
defs = []  # (fnid, start, end) 1-based inclusive
for i, line in enumerate(src, 1):
    m = defre.match(line)
    if m:
        if defs:
            defs[-1] = (defs[-1][0], defs[-1][1], i - 1)
        defs.append((int(m.group(1)), i, None))
for k, (fnid, start, end) in enumerate(defs):
    if end is None:  # last def: closing brace = last non-blank line
        j = len(src)
        while src[j - 1].strip() == "":
            j -= 1
        assert src[j - 1].rstrip("\r\n") == "}", "def f%d has no closing brace" % fnid
        defs[k] = (fnid, start, j)

first_start = defs[0][1]
cut = {fnid: (start, end) for fnid, start, end in defs if fnid in frags}
missing = sorted(set(frags) - set(cut))
if missing:
    sys.exit("no definition found for: %s" % missing)

# ---- emit: preamble, then definitions in order (fragments in place) ---------
chunks = ["".join(src[: first_start - 1])]
by_start = {start: fnid for fnid, start, _ in defs}
for fnid, start, end in defs:
    if fnid in frags:
        for _, fname in frags[fnid]:
            chunks.append("\n// ---- transcribed: f%d (%s) ----\n" % (fnid, fname))
            with open(os.path.join(code_dir, fname), encoding="utf-8") as fh:
                chunks.append(fh.read())
    else:
        chunks.append("".join(src[start - 1 : end]))
text = "".join(chunks)

# ---- rename transcribed functions everywhere (call sites + decls) -----------
for fnid, parts in sorted(frags.items()):
    new_name = parts[0][0]
    text, n = re.subn(r"\bw2c_artemis_f%d\b" % fnid, new_name, text)
    print("f%-5d -> %-28s rename hits: %d" % (fnid, new_name, n))

# ---- route compat aliases ---------------------------------------------------
# engine_context.h call-throughs spell the generated name (w2c_artemis_fNNN);
# the rename above has just stamped the transcription's symbol into the body.
# Alias every generated spelling to its new one so the vocabulary boundary
# compiles in both this route and the bridge route (which never renames).
alias = ""
for fnid, parts in sorted(frags.items()):
    new_name = parts[0][0]
    if new_name != "w2c_artemis_f%d" % fnid:
        alias += "#define w2c_artemis_f%d %s\n" % (fnid, new_name)
if alias:
    text = alias + text

with open(out_path, "w", encoding="utf-8", errors="surrogateescape", newline="") as fh:
    fh.write(text)
print("spliced: %s  (%d fragments, %d defs kept)" % (out_path, sum(len(v) for v in frags.values()), len(defs) - len(frags)))
EOF
[ $? -eq 0 ] || exit 1

# rewrite/src products take over their generated defs; a bridge thunk keeps the
# original name, signature and FUNC_PROLOGUE frame so generated callers, the
# static forward decls and the profiler anchor all stay valid untouched.
python "$HERE/rw-splice.py" "$SPLICE" "$REWRITE_TREE" || exit 1

bash "$HERE/build-cxx-route.sh" "$FLAVOUR" "$SPLICE" "$OUTDIR"
