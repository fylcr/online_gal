#!/usr/bin/env python3
"""tx-gen.py — wasm2c 生成 C → 可读 C++ 脚本辅助转写生成器（机械保真）。

方法：逐语句复制源码（机械层零改写，保真由构造保证），只做两件事：
  1) 标识符改名（词边界正则，def 符号 w2c_artemis_fNNN 自动改新名）；
  2) 按锚点插入整行 // 注释。
产物首行固定 `// tx: f<NNN> -> <新名>`，全文件该标记恰好出现一次（code-assemble.sh 装配契约）。

用法：
  python tx-gen.py --src <artemis.wasm2c.c> --fn f3698 --start 1984534 --end 1987441 \
      --name art_video_dispatch --out <code/f3698.cpp> \
      [--rename var_p0=props ...] [--anchors f3698.anchors.txt] [--header f3698.header.txt]

anchors.txt 每行：`<spec>\t<comment>`，在目标行前插入注释。
  spec = L<绝对行号>  或  R:<正则（首个匹配行）>
  comment 中字面反斜杠+n 两字符序列 → 拆成多行注释。
header.txt：普通注释行（不要含装配标记），插在标记行之后。
"""
import argparse
import re
import sys


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--src', required=True)
    ap.add_argument('--fn', required=True, help='fNNN')
    ap.add_argument('--start', type=int, required=True, help='源定义首行（1-based）')
    ap.add_argument('--end', type=int, required=True, help='源定义末行（含）')
    ap.add_argument('--name', required=True, help='新函数名')
    ap.add_argument('--out', required=True)
    ap.add_argument('--rename', action='append', default=[], help='old=new，可多次')
    ap.add_argument('--anchors')
    ap.add_argument('--header')
    args = ap.parse_args()

    with open(args.src, encoding='utf-8') as f:
        lines = f.read().splitlines()
    body = lines[args.start - 1:args.end]
    if not body:
        sys.exit('empty range')
    sym = 'w2c_artemis_' + args.fn
    if sym + '(' not in body[0]:
        sys.exit('first line mismatch: %r' % body[0])
    while body and not body[-1].strip():
        body.pop()
    if not body or body[-1] != '}':
        sys.exit('last non-blank line not a lone closing brace: %r' % (body[-3:],))

    ren = [(sym, args.name)]
    for spec in args.rename:
        old, new = spec.split('=', 1)
        ren.append((old, new))
    ren.sort(key=lambda kv: -len(kv[0]))
    counts = dict((old, 0) for old, _ in ren)
    out_body = []
    for ln in body:
        for old, new in ren:
            ln, n = re.subn(r'(?<!\w)' + re.escape(old) + r'(?!\w)', new, ln)
            counts[old] += n
        out_body.append(ln)

    ins = {}
    if args.anchors:
        for raw in open(args.anchors, encoding='utf-8'):
            raw = raw.rstrip('\n')
            if not raw or raw.startswith('#'):
                continue
            spec, comment = raw.split('\t', 1)
            idx = None
            if spec.startswith('R:'):
                pat = re.compile(spec[2:])
                for i, ln in enumerate(body):
                    if pat.search(ln):
                        idx = i
                        break
            elif spec.startswith('L'):
                idx = int(spec[1:]) - args.start
                if not 0 <= idx < len(body):
                    sys.exit('anchor %s out of range' % spec)
            else:
                sys.exit('bad anchor spec: %r' % spec)
            if idx is None:
                sys.exit('anchor not matched: %s' % spec)
            parts = comment.split('\\n')
            ins.setdefault(idx, []).extend('//' + (' ' + p if p else '') for p in parts)

    out_lines = ['// tx: %s -> %s' % (args.fn, args.name)]
    if args.header:
        out_lines += [l.rstrip('\n') for l in open(args.header, encoding='utf-8')]
    for i, ln in enumerate(out_body):
        out_lines.extend(ins.get(i, []))
        out_lines.append(ln)
    text = '\n'.join(out_lines) + '\n'
    if '\x00' in text:
        sys.exit('generated NUL byte')
    if text.count('// tx:') != 1:
        sys.exit('assembly marker count != 1')
    with open(args.out, 'w', encoding='utf-8', newline='\n') as f:
        f.write(text)
    print('OK %s: body %d lines -> out %d lines, anchors %d'
          % (args.out, len(body), len(out_lines), len(ins)))
    for old, new in ren:
        print('  rename %s -> %s : %d' % (old, new, counts[old]))


if __name__ == '__main__':
    main()
