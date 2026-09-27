#!/usr/bin/env python3
"""Match the functions of two wasm2c outputs of the same program.

Problem this solves
-------------------
wasm2c numbers functions by their position in ITS OWN module, so `f205` in the
asyncify output and `f205` in the sync output are different functions (measured:
void/2 there, u32/3 here).  The two builds of the same engine therefore share no
usable function numbering, and neither do they share pool addresses: only 4,630
of ~17,000 five-to-seven-digit literals coincide, because each build lays its own
string pool out separately.

The join key
------------
What both builds agree on is the *text* of the program.  So the key is built
from two feature multisets that are address-free and numbering-free:

  * the set of string constants the function references, resolved to the TEXT
    at that address in that build's own pool (2,779 texts are unique on both
    sides -- a good anchor);
  * the multiset of functions it calls.

Neither alone identifies a function; together they do, and iterating them
propagates the match outward from the anchors (a function whose callees are all
already matched is matched by its callee signature).  Iteration is seeded by
string-anchored pairs and repeated until the assignment stops growing.

Usage:
    python tx-correspond.py <asyn.wasm2c.c> <asyn-strings.tsv> \
                            <sync.wasm2c.c> <sync-strings.tsv> <out.tsv>

Output: TAB-separated  fnid_asyn <TAB> fnid_sync <TAB> confidence <TAB> evidence
where confidence is one of `anchor` (string+shape), `iter` (propagated through
already-matched callees) or `shape` (structure only -- review these).
"""
import collections
import re
import sys

# The generated signature: <ret> w2c_artemis_fNNN(w2c_artemis* instance, <params>) {
DEF_RE = re.compile(
    r"^([a-z0-9_]+) w2c_artemis_(f\d+)\(w2c_artemis\* instance((?:, [^)]*)?)\) \{$")
# A direct call inside a body.  The generated names are the only call syntax used
# for statically-known targets; indirect ones go through CALL_INDIRECT and are
# deliberately excluded, because they name a table slot, not a function.
CALL_RE = re.compile(r"\bw2c_artemis_(f\d+)\(")
# A pool address literal.  These are bare decimals in the generated source
# (`var_i0 = 179152u;`); the `+=` form is a struct offset, not a pool address.
POOL_RE = re.compile(r"(?<![\w)])(\d{4,7})u\b")

# RTTI names and mangled C++ fragments also live in the pool, and they are stable
# across builds, so they are kept -- but they are weighted lower than real message
# text when reporting, so a match on RTTI alone is not called an anchor.
RTTI_RE = re.compile(r"^N[0-9]+[A-Za-z_]|^St\d|^[A-Za-z_]{0,3}9_|^\d+[A-Za-z_]+E$")


def read_leb(buf, i):
    result = shift = 0
    while True:
        byte = buf[i]
        i += 1
        result |= (byte & 0x7F) << shift
        if not byte & 0x80:
            return result, i
        shift += 7


def load_pool(path):
    """address -> text, from a tx-strings.py table."""
    pool = {}
    with open(path, encoding="utf-8") as fh:
        for line in fh:
            if not line.strip():
                continue
            addr, text = line.rstrip("\n").split("\t", 1)
            pool[int(addr)] = text
    return pool


def parse_flavour(path, pool):
    """fnid -> shape, string multiset and callee set for one wasm2c output."""
    funcs = {}
    fnid = None
    cur = None
    with open(path, encoding="utf-8") as fh:
        for line in fh:
            if cur is None:
                m = DEF_RE.match(line.rstrip("\n"))
                if m:
                    params = m.group(3)
                    nparams = 0 if not params.strip() else params.count(",") + 1
                    fnid = int(m.group(2)[1:])
                    cur = {"ret": m.group(1), "nparams": nparams,
                           "strings": set(), "calls": set(), "lines": 0}
                continue
            if line.startswith("}"):
                funcs[fnid] = cur
                cur, fnid = None, None
                continue
            cur["lines"] += 1
            for fn in CALL_RE.findall(line):
                cur["calls"].add(int(fn[1:]))
            for lit in POOL_RE.findall(line):
                text = pool.get(int(lit))
                if text is not None:
                    cur["strings"].add(text)
    return funcs


def shape_of(fn):
    return (fn["ret"], fn["nparams"])


def main():
    asyn_c, asyn_pool_p, sync_c, sync_pool_p, out_path = sys.argv[1:6]
    apool, spool = load_pool(asyn_pool_p), load_pool(sync_pool_p)
    A = parse_flavour(asyn_c, apool)
    S = parse_flavour(sync_c, spool)
    sys.stderr.write("parsed: asyncify %d, sync %d\n" % (len(A), len(S)))

    # ---- seed: functions whose resolved string set is unique on both sides ----
    astr = collections.defaultdict(list)
    sstr = collections.defaultdict(list)
    for f, fn in A.items():
        for t in fn["strings"]:
            astr[t].append(f)
    for f, fn in S.items():
        for t in fn["strings"]:
            sstr[t].append(f)

    def usable(t):
        # RTTI/mangled names are stable but weak: they are shared by whole class
        # hierarchies, so a function matching only on those is not anchored.
        return not RTTI_RE.match(t)

    anchors = collections.defaultdict(list)   # asyn fnid -> [sync fnid]
    for t in set(astr) & set(sstr):
        if not usable(t):
            continue
        if len(astr[t]) == 1 and len(sstr[t]) == 1:
            anchors[astr[t][0]].append(sstr[t][0])
    # A function referenced by several distinct strings is better identified
    # than one referenced by a single string; require two independent hits.
    pair_votes = collections.Counter()
    for t in set(astr) & set(sstr):
        if not usable(t):
            continue
        if len(astr[t]) == 1 and len(sstr[t]) == 1:
            pair_votes[(astr[t][0], sstr[t][0])] += 1

    match = {}       # asyn fnid -> sync fnid
    how = {}         # asyn fnid -> confidence
    evidence = {}    # asyn fnid -> string
    for (a, s), votes in sorted(pair_votes.items(), key=lambda kv: -kv[1]):
        if votes >= 2 and a not in match and s not in match.values():
            match[a] = s
            how[a] = "anchor"
            evidence[a] = "%d shared strings" % votes
    sys.stderr.write("seeded %d anchor pair(s)\n" % len(match))

    # ---- propagate: a function whose callees are all matched is matched by the
    # multiset of its matched callees, which is numbering-free once mapped. ----
    inv = {s: a for a, s in match.items()}
    for _ in range(12):
        grew = 0
        for a, fn in A.items():
            if a in match:
                continue
            if not fn["calls"]:
                continue
            mapped = [inv[c] for c in fn["calls"] if c in inv]
            if not mapped or len(mapped) < max(2, len(fn["calls"]) // 2):
                continue
            # Find sync functions whose callee multiset contains the same mapped
            # set (after mapping their callees back through inv, where known).
            want = collections.Counter(mapped)
            for s, sfn in S.items():
                if s in inv:
                    continue
                got = collections.Counter(
                    inv[c] for c in sfn["calls"] if c in inv)
                if not got:
                    continue
                if shape_of(sfn) != shape_of(fn):
                    continue
                common = sum((want & got).values())
                if common and common >= max(2, sum(want.values()) // 2):
                    if common >= 3 or (fn["strings"] and fn["strings"] == sfn["strings"]):
                        match[a] = s
                        how[a] = "iter"
                        evidence[a] = "%d/%d callees agree" % (common, sum(want.values()))
                        inv[s] = a
                        grew += 1
                        break
        if not grew:
            break
        sys.stderr.write("iterated: +%d pair(s), total %d\n" % (grew, len(match)))

    # ---- shape-only remainder: same signature and similar size ----------------
    for a, fn in A.items():
        if a in match:
            continue
        cands = [s for s, sfn in S.items()
                 if s not in inv and shape_of(sfn) == shape_of(fn)]
        if not cands:
            continue
        best = min(cands, key=lambda s: abs(S[s]["lines"] - fn["lines"]))
        match[a] = best
        how[a] = "shape"
        evidence[a] = "%s/%d, %d vs %d lines" % (fn["ret"], fn["nparams"],
                                                fn["lines"], S[best]["lines"])
        inv[best] = a

    with open(out_path, "w", encoding="utf-8") as out:
        out.write("# asyn\tsync\tconfidence\tevidence\n")
        for a in sorted(match):
            out.write("f%d\tf%d\t%s\t%s\n" % (a, match[a], how[a], evidence[a]))

    tally = collections.Counter(how.values())
    sys.stderr.write("matched %d/%d asyncify functions (anchor=%d iter=%d shape=%d)\n"
                     % (len(match), len(A), tally["anchor"], tally["iter"],
                        tally["shape"]))
    print("wrote %s: %d pair(s)" % (out_path, len(match)))


if __name__ == "__main__":
    main()
