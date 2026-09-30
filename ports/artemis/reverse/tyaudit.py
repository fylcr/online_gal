# Audit: does any landed product declare a local with the wrong C type versus the
# generated layer's own declaration block? The f4715 defect was locals l6..l9
# declared f32 in wasm2c but uint32_t in the product, so every float value was
# truncated through a u32.
import re, os, sys, bisect

import os as _os
_HERE = _os.path.dirname(_os.path.abspath(__file__))
_REPO = _os.path.dirname(_os.path.dirname(_os.path.dirname(_HERE)))
_REF = _os.path.join(_REPO, ".ref", "artemis-official")
SRC = _os.path.join(_REF, "wasm", "asyncify", "artemis.wasm2c.c")
P = r"(?:, (?:u32|f32|f64|i32|i64|u64) var_p\d+)*"
DEF = re.compile(r"^(\w+) w2c_artemis_(f\d+)\(w2c_artemis\* instance(" + P + r")\) \{")
DECL = re.compile(r"^\s*(u32|u64|f32|f64|i32|i64)\s+((?:var_\w+\s*=\s*[^,;]+,\s*)*var_\w+\s*=\s*[^;]+);")

CXX = {"u32": "uint32_t", "u64": "uint64_t", "f32": "float", "f64": "double",
       "i32": "int32_t", "i64": "int64_t"}

lines = open(SRC, encoding="utf-8", errors="replace").read().split("\n")
# index every def start
defs = []
for i, l in enumerate(lines):
    m = DEF.match(l)
    if m:
        defs.append((i, int(m.group(2)[1:]), m.group(1), m.group(3)))
starts = [d[0] for d in defs]

names = {}
tsv = _os.path.join(_REPO, "ports", "artemis", "rewrite-names.tsv")   # the live tsv, always
for row in open(tsv, encoding="utf-8").read().split("\n"):
    c = row.split("\t")
    if len(c) >= 5 and c[0] == "fn" and c[4] not in ("—", "-", ""):
        names[int(c[1][1:])] = (c[3], c[4])

bad = []
for fnid, (target, rel) in sorted(names.items()):
    path = _os.path.join(_REPO, "ports", "artemis", "src", rel)
    if not os.path.exists(path):
        continue
    k = bisect.bisect_right(starts, 0) # placeholder
    # locate this fn in the source
    lo, hi = 0, len(starts) - 1
    pos = at = None
    while lo <= hi:
        mid = (lo + hi) // 2
        if defs[mid][1] == fnid:
            pos, at = defs[mid][0], mid
            break
        if defs[mid][1] < fnid: lo = mid + 1
        else: hi = mid - 1
    if pos is None: continue
    # the def index is sorted by fnid, not by file position, so the body ends
    # at the next def in FILE order -- take the nearest later start line.
    later = [d[0] for d in defs if d[0] > pos]
    end = (min(later) - 1) if later else len(lines)
    srclocals = {}
    for j in range(pos, min(pos + 40, end)):
        m = DECL.match(lines[j])
        if not m: continue
        cty = m.group(1)
        for part in m.group(2).split(","):
            nm = part.split("=")[0].strip()
            if nm:
                srclocals[nm[4:] if nm.startswith("var_") else nm] = cty

    prod = open(path, encoding="utf-8").read()
    # Products pack several declarations per line, so walk every
    # "<type> <name> = <init>;" pair instead of anchoring to the line start.
    for m in re.finditer(r"\b(uint32_t|uint64_t|float|double|int32_t|int64_t)\s+([A-Za-z_]\w*)\s*=", prod):
        cty, nm = m.group(1), m.group(2)
        if nm.startswith("var_"): nm = nm[4:]   # products drop the var_ prefix
        if nm not in srclocals: continue
        if CXX[srclocals[nm]] != cty:
            bad.append((fnid, rel, nm, srclocals[nm], cty))
    prodloc = len(set(re.findall(r"\b(uint32_t|uint64_t|float|double|int32_t|int64_t)\s+([A-Za-z_]\w*)\s*=", prod)))

print("products audited:", len(names))
print("wrong-type local declarations:", len(bad))
for b in bad:
    print("  f%d %s local %s : source %s -> product %s" % b)
