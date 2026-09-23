#!/usr/bin/env python3
"""tx-gen.py — wasm2c 生成 C → 可读 C++ 脚本辅助转写生成器（机械保真）v2（支持分片）。

方法：逐语句复制源码（机械层零改写，保真由构造保证），只做两件事：
  1) 标识符改名（词边界正则，def 符号 w2c_artemis_fNNN 自动改新名）；
  2) 按锚点插入整行 // 注释。

整函数模式（默认）：产物首行 `// tx: f<NNN> -> <新名>`，全文件该标记恰好 1 次。
分片模式（巨函数拆 part）：`--part N` 指定片号（1 起），`--last` 标记末片。
  part01 首行仍是 `// tx: f<NNN> -> <新名>` 且含 def 行；part02+ 首行
  `// tx-part: f<NNN> part <NN>`（两位零填充，与文件名一致）；末片以列 0 `}` 收口，
  非末片不许落在 `}` 行。文件名约定 f<NNN>.part<NN>.cpp，装配器按文件名升序拼接。

用法（整函数）：
  python tx-gen.py --src <artemis.wasm2c.c> --fn f3698 --start 1984534 --end 1987441 \
      --name art_video_dispatch --out <code/f3698.cpp> \
      [--rename var_p0=props ...] [--anchors a.txt] [--header h.txt]
用法（分片）：同上加 `--part N [--last]`，--start/--end 为该片的源行区间（绝对行号）。

anchors.txt 每行：`<spec>\t<comment>`，在目标行前插入注释。
  spec = L<绝对行号>  或  R:<正则（首个匹配行）>；未命中/越界的锚点警告后跳过，不致命。
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
    ap.add_argument('--start', type=int, required=True, help='源切片首行（1-based，绝对行号）')
    ap.add_argument('--end', type=int, required=True, help='源切片末行（含）')
    ap.add_argument('--name', required=True, help='新函数名')
    ap.add_argument('--out', required=True)
    ap.add_argument('--rename', action='append', default=[], help='old=new，可多次')
    ap.add_argument('--anchors')
    ap.add_argument('--header')
    ap.add_argument('--part', type=int, default=0, help='分片片号，1 起；0=整函数模式')
    ap.add_argument('--last', action='store_true', help='分片模式：这是末片（以 } 收口）')
    args = ap.parse_args()

    with open(args.src, encoding='utf-8') as f:
        lines = f.read().splitlines()
    body = lines[args.start - 1:args.end]
    if not body:
        sys.exit('empty range')
    sym = 'w2c_artemis_' + args.fn
    first_part = (args.part == 0 or args.part == 1)
    is_last = (args.part == 0) or args.last

    if first_part and sym + '(' not in body[0]:
        sys.exit('first line mismatch: %r' % body[0])
    while body and not body[-1].strip():
        body.pop()
    if is_last:
        if not body or body[-1] != '}':
            sys.exit('last non-blank line not a lone closing brace: %r' % (body[-3:],))
    elif body and body[-1] == '}':
        sys.exit('non-last part must not end at the closing brace')

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
                    print('WARN: anchor out of range, skipped: %s' % spec)
                    continue
            else:
                sys.exit('bad anchor spec: %r' % spec)
            if idx is None:
                print('WARN: anchor not matched, skipped: %s' % spec)
                continue
            parts = comment.split('\\n')
            ins.setdefault(idx, []).extend('//' + (' ' + p if p else '') for p in parts)

    if args.part and args.part > 1:
        marker = '// tx-part: %s part %02d' % (args.fn, args.part)
    else:
        marker = '// tx: %s -> %s' % (args.fn, args.name)
    out_lines = [marker]
    if args.header:
        out_lines += [l.rstrip('\n') for l in open(args.header, encoding='utf-8')]
    for i, ln in enumerate(out_body):
        out_lines.extend(ins.get(i, []))
        out_lines.append(ln)
    text = '\n'.join(out_lines) + '\n'
    if '\x00' in text:
        sys.exit('generated NUL byte')
    if args.part and args.part > 1:
        marker_ok = text.count('// tx-part:') == 1 and text.count('// tx:') == 0
    else:
        marker_ok = text.count('// tx:') == 1 and text.count('// tx-part:') == 0
    if not marker_ok:
        sys.exit('assembly marker count wrong')
    with open(args.out, 'w', encoding='utf-8', newline='\n') as f:
        f.write(text)
    print('OK %s: body %d lines -> out %d lines, anchors %d, marker %r'
          % (args.out, len(body), len(out_lines), len(ins), marker))
    for old, new in ren:
        print('  rename %s -> %s : %d' % (old, new, counts[old]))


if __name__ == '__main__':
    main()
