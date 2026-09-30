# -*- coding: utf-8 -*-
"""frame.py -- derive the asyncify save-frame size of one generated definition.

The generated body restores the frame pointer as
    var_i0 = instance->w2c_g4;
    var_i0 = i32_load_default32(..., (u64)(var_i0), 0);
    var_i2 = <N>u;
    var_i0 -= var_i2;
so the size is the literal in that subtraction.  Calibrate against the landed
products, whose constant is already named kF<nnn>SaveFrameBytes.
"""
import os, re, sys

import os as _os, os.path as _p
_HERE = _p.dirname(_p.abspath(__file__))
_REPO = _p.dirname(_p.dirname(_p.dirname(_HERE)))
GEN = _os.environ.get('ARTEMIS_GEN_C') or _p.join(_REPO, '.ref', 'artemis-official', 'wasm', 'asyncify', 'artemis.wasm2c.c')
SRC = _p.join(_REPO, 'ports', 'artemis', 'src')
DEFRE = re.compile(r'^\w+(?: \w+)? w2c_artemis_(f\d+)\(w2c_artemis\* instance(.*)\) \{')
SUB = re.compile(r'^\s*(\w+) -= (\w+);\s*$')
LIT = re.compile(r'^\s*(\w+) = (\d+)u;\s*$')

lines = open(GEN, encoding='utf-8', errors='replace').read().split('\n')
starts = {}
owner = [None] * (len(lines) + 2)
cur = None
for i, ln in enumerate(lines):
    m = DEFRE.match(ln)
    if m:
        cur = m.group(1)
        starts[cur] = i
    owner[i] = cur
frames = {}
for i, ln in enumerate(lines):
    m = SUB.match(ln)
    if not m:
        continue
    j = i - 1
    if j < 0 or not LIT.match(lines[j]):
        continue
    k = int(LIT.match(lines[j]).group(2))
    # the minuend must have been loaded from the frame pointer a few lines up
    win = '\n'.join(lines[max(0, i - 5):i + 1])
    if 'w2c_g4' in win and k > 4:
        frames.setdefault(owner[i], k)

if len(sys.argv) > 1:
    for fnid in sys.argv[1:]:
        print('%-7s frame=%s  span=%d' % (fnid, frames.get(fnid, 0), starts.get(fnid, -1) + 1))
else:
    # calibrate against landed products
    known = {}
    for root, _d, fs in os.walk(SRC):
        for f in fs:
            p = os.path.join(root, f)
            t = open(p, encoding='utf-8', errors='replace').read()
            mm = re.search(r'kF(\d+)SaveFrameBytes', t)
            if mm:
                known['f' + mm.group(1)] = int(re.search(r'kF%sSaveFrameBytes = (\d+)u' % mm.group(1), t).group(1))
    ok = bad = miss = 0
    for fnid, want in sorted(known.items()):
        got = frames.get(fnid)
        if got is None:
            miss += 1; print('MISS %-7s want=%d' % (fnid, want))
        elif got == want:
            ok += 1
        else:
            bad += 1; print('BAD  %-7s want=%d got=%s' % (fnid, want, got))
    print('calibration: ok %d / bad %d / not-derived %d (known %d)' % (ok, bad, miss, len(known)))
