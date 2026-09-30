# -*- coding: utf-8 -*-
"""tx-plan.py -- what is left of the rewrite, and what unblocks each row.

Joins the plan (ports/artemis/rewrite-names.tsv) with the generated C's call
graph.  tx-gen2.py can emit a row only when every callee that row makes already
has a name in the generator's PROD registry, so the gap between the plan and
the registry is exactly the remaining closure work.

Usage:
  python ports/artemis/reverse/tx-plan.py            # every unlanded row
  python ports/artemis/reverse/tx-plan.py --ready    # only the emittable ones
"""
import os, re, sys

_HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.dirname(os.path.dirname(os.path.dirname(_HERE)))
REF = os.path.join(REPO, '.ref', 'artemis-official')
GEN = os.environ.get('ARTEMIS_GEN_C') or os.path.join(REF, 'wasm', 'asyncify', 'artemis.wasm2c.c')
TSV = os.path.join(REPO, 'ports', 'artemis', 'rewrite-names.tsv')
SRC = os.path.join(REPO, 'ports', 'artemis', 'src')

# ---- the generator's declarative head: registries only, no emission ----
_g = {'__file__': os.path.join(_HERE, 'tx-gen2.py'), '__name__': 'txgen-head'}
_lines = open(os.path.join(_HERE, 'tx-gen2.py'), encoding='utf-8').read().split('\n')
_cut = next(i for i, l in enumerate(_lines) if l.startswith('def transform('))
exec(compile('\n'.join(_lines[:_cut]), 'txgen-head', 'exec'), _g)
PROD = _g['PROD']
HOSTMAP, TABLEMAP = _g['HOSTMAP'], _g['TABLEMAP']

# ---- generated C: one pass -> per-definition line count and callee set ----
DEF = re.compile(r'^(\w+) w2c_artemis_(f\d+)\(w2c_artemis\* instance')
CALL = re.compile(r'w2c_artemis_(f\d+|[A-Za-z]\w*_\d+)\(instance')
HOST = re.compile(r'w2c_a_(\w+)\(instance->w2c_a_instance')
TABLE = re.compile(r'CALL_INDIRECT\(instance->w2c_Lc, [^\n]*?, (w2c_artemis_(t\d+)), ')
start, lines, callees, hosts, tables = {}, {}, {}, {}, {}
cur = None
with open(GEN, encoding='utf-8', errors='replace') as fh:
    for n, ln in enumerate(fh, 1):
        if cur:
            for c in CALL.findall(ln):
                callees.setdefault(cur, set()).add(c)
            for h in HOST.findall(ln):
                hosts.setdefault(cur, set()).add(h)
            for t in TABLE.findall(ln):
                tables.setdefault(cur, set()).add(t[1])
        m = DEF.match(ln)
        if m:
            cur = m.group(2)
            start[cur] = n
            lines[cur] = 0
        elif cur:
            lines[cur] += 1

rows = []
for ln in open(TSV, encoding='utf-8'):
    p = ln.rstrip('\n').split('\t')
    if len(p) < 6 or p[0] != 'fn':
        continue
    fnid, target, rel, depth = p[1], p[3], p[4], p[5]
    landed = rel not in ('—', '') and os.path.exists(os.path.join(SRC, rel))
    rows.append((fnid, target, rel, depth, landed))

ready = blocked = land = 0
out = []
for fnid, target, rel, depth, landed in rows:
    if landed:
        land += 1
        continue
    missing = sorted(c for c in callees.get(fnid, ()) if c not in PROD and c != fnid)
    miss_h = sorted(h for h in hosts.get(fnid, ()) if h not in HOSTMAP)
    miss_t = sorted(t for t in tables.get(fnid, ()) if t not in TABLEMAP)
    kind = 'READY' if not (missing or miss_h or miss_t) else 'BLOCKED'
    ready += kind == 'READY'
    blocked += kind == 'BLOCKED'
    out.append((fnid, target, rel, depth, kind, missing, miss_h, miss_t))

out.sort(key=lambda r: -lines.get(r[0], 0))
only_ready = '--ready' in sys.argv
print('rows: landed %d / ready %d / blocked %d' % (land, ready, blocked))
for fnid, target, rel, depth, kind, missing, miss_h, miss_t in out:
    if only_ready and kind != 'READY':
        continue
    print('%-7s %-6s %-8s %-7s %-40s %s' % (fnid, depth, kind, lines.get(fnid, 0), target or '—', rel if rel != '—' else ''))
    if missing:
        print('        missing callee: %s' % ' '.join(missing))
    if miss_h:
        print('        missing host:   %s' % ' '.join('a_' + h for h in miss_h))
    if miss_t:
        print('        missing table:  %s' % ' '.join(miss_t))
