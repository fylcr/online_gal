#!/usr/bin/env python3
# tx-auto-part.py —— 巨函数分片的探针式自做生成器（tx-gen 的自动化前端）
# 自动完成：探针扫描（标签判别点/分支块标签/boost 断言/字符串池装载）-> 生成锚点与页眉 -> 调 tx-gen 产分片 -> 自检。
# 用法：python tx-auto-part.py --fn f3633 --start 1706835 --end 1716668 --part 2 --name art_builtin_dispatch --note "本片定位"
import argparse, json, re, subprocess, sys

SRC = r'D:/online_gal/.ref/artemis-official/wasm/asyncify/artemis.wasm2c.c'
TAGMAP = r'D:/online_gal/.ref/artemis-official/wasm/asyncify/tag-map.json'
STRINGS = r'D:/online_gal/.ref/artemis-official/code/strings.tsv'
TXGEN = r'D:/online_gal/ports/artemis/reverse/tx-gen.py'
OUTDIR = r'D:/online_gal/.ref/artemis-official/code'
AUXDIR = r'D:/tmp'

ap = argparse.ArgumentParser()
ap.add_argument('--fn', required=True)
ap.add_argument('--start', type=int, required=True)
ap.add_argument('--end', type=int, required=True)
ap.add_argument('--part', type=int, required=True)
ap.add_argument('--last', action='store_true')
ap.add_argument('--name', required=True)
ap.add_argument('--note', default='')
a = ap.parse_args()

d = json.load(open(TAGMAP, encoding='utf-8'))
owned = set(d.get('tag_owner', {}).get('w2c_artemis_' + a.fn, []))
addr2 = {}
for t, al in d.get('tag_addr', {}).items():
    for x in (al if isinstance(al, list) else [al]):
        addr2.setdefault(int(x), ('tag:' + t) if t in owned else ('pool:' + t))
strs = {}
for ln in open(STRINGS, encoding='utf-8', errors='replace'):
    p = ln.rstrip('\n').split('\t')
    if len(p) == 2 and p[0].strip().isdigit():
        strs[int(p[0])] = p[1]

anchors = []
with open(SRC, encoding='utf-8', errors='replace') as f:
    for n, ln in enumerate(f, 1):
        if n < a.start:
            continue
        if n > a.end:
            break
        s = ln.strip()
        m = re.match(r'var_i\d+ = (\d+)u;$', s)
        if m:
            x = int(m.group(1))
            if x in addr2:
                v = addr2[x]
                txt = v[4:]
                anchors.append((n, '标签判别点：%s（字符串池 %du 装载参与比较）' % (txt, x)) if v.startswith('tag:') else (n, '同池串 %s（池 %du，他函数认领的标签名）' % (txt, x)))
            elif x in strs:
                anchors.append((n, '字符串池 %du：%s' % (x, strs[x])))
        if re.match(r'var_[BL]\d+:', s):
            anchors.append((n, '分支块边界 %s（wasm2c 展平控制流的汇流标签）' % s.rstrip(':; ')))
        if 'w2c_a_e(' in s:
            anchors.append((n, 'boost 断言调用点（宿主导入 w2c_a_e）'))
        if 'CALL_INDIRECT' in s:
            anchors.append((n, '间接表调用点（函数指针分派）'))
anchors = sorted(set(anchors))[:600]

hdr = '// src: %d-%d\n// %s part %02d：%s——机械层逐行 verbatim 零改写、全程零改名\n' % (a.start, a.end, a.fn, a.part, a.note or '分片切片')
hp = '%s/%s.part%02d.header.txt' % (AUXDIR, a.fn, a.part)
ap_path = '%s/%s.part%02d.anchors.txt' % (AUXDIR, a.fn, a.part)
out = '%s/%s.part%02d.cpp' % (OUTDIR, a.fn, a.part)
open(hp, 'w', encoding='utf-8').write(hdr)
with open(ap_path, 'w', encoding='utf-8') as f:
    for n, c in anchors:
        f.write('L%d\t%s\n' % (n, c))

cmd = [sys.executable, TXGEN, '--src', SRC, '--fn', a.fn, '--start', str(a.start), '--end', str(a.end),
       '--name', a.name, '--part', str(a.part)] + (['--last'] if a.last else []) + \
      ['--anchors', ap_path, '--header', hp, '--out', out]
r = subprocess.run(cmd, capture_output=True, text=True)
print(r.stdout.strip() or r.stderr.strip())

src_nb = 0
with open(SRC, encoding='utf-8', errors='replace') as f:
    for n, ln in enumerate(f, 1):
        if n < a.start:
            continue
        if n > a.end:
            break
        if ln.strip():
            src_nb += 1
lines = open(out, encoding='utf-8').read().splitlines()
code_nb = sum(1 for l in lines if l.strip() and not l.strip().startswith('//'))
raw = open(out, 'rb').read()
print('anchors=%d code_nonblank=%d src_nonblank=%d MATCH=%s NUL=%d' %
      (len(anchors), code_nb, src_nb, code_nb == src_nb, raw.count(bytes([0]))))
print('first=%r last=%r' % (lines[0], lines[-1]))
sys.exit(0 if code_nb == src_nb and raw.count(bytes([0])) == 0 else 1)
