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
if [ "$FLAVOUR" = "asyncify" ]; then RDIR="asyncify"; else RDIR="plain"; fi

GEN_C="$REPO_ROOT/.ref/artemis-official/wasm/$RDIR/artemis.wasm2c.c"
CODE_DIR="$REPO_ROOT/.ref/artemis-official/code"
OUTDIR="${2:-$HERE/out/code-$FLAVOUR}"
SPLICE="$OUTDIR/combined.c"
mkdir -p "$OUTDIR"

# python on this box only takes native Windows paths (see extract-tag-map notes)
G=$(cygpath -m "$GEN_C"); C=$(cygpath -m "$CODE_DIR"); S=$(cygpath -m "$SPLICE")
python - "$G" "$C" "$S" <<'EOF'
import os, re, sys

gen_c, code_dir, out_path = sys.argv[1], sys.argv[2], sys.argv[3]

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
    sys.exit("no fragments found in %s" % code_dir)

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

with open(out_path, "w", encoding="utf-8", errors="surrogateescape", newline="") as fh:
    fh.write(text)
print("spliced: %s  (%d fragments, %d defs kept)" % (out_path, sum(len(v) for v in frags.values()), len(defs) - len(frags)))
EOF
[ $? -eq 0 ] || exit 1

bash "$HERE/build-cxx-route.sh" "$FLAVOUR" "$SPLICE" "$OUTDIR"
