#!/usr/bin/env python3
"""alpha-renaming fidelity diff for artemis wasm2c transcriptions.

Verifies a transcription product (.cpp) is statement-identical to its wasm2c
source function (in artemis.wasm2c.c) modulo renames and comments:
  1. locate the source def `^[A-Za-z_][A-Za-z0-9_]* w2c_artemis_<fnid>(w2c_artemis* ...) {`
     and the product def (same shape, any function name) through the first
     column-0 `}` line;
  2. strip // and /* */ comments (quote-aware) and blank lines;
  3. replace every identifier positionally (first-appearance order -> v1, v2, ...)
     in both, collapse whitespace;
  4. unified-diff the two token-line lists.

usage: fidelity-alpha-diff.py <product.cpp> <artemis.wasm2c.c> <fnid>
exit 0 = IDENTICAL, 1 = DIVERGENCE (diff printed), 2 = usage / def not found.

Windows python: pass native D:/... paths (MSYS /d/... silently matches nothing).
"""
import difflib
import re
import sys

DEF_SRC_TMPL = r'^[A-Za-z_][A-Za-z0-9_]*\s+w2c_artemis_%s\(w2c_artemis\s*\*.*\{\s*$'
DEF_PROD = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*\s+[A-Za-z_][A-Za-z0-9_]*\(w2c_artemis\s*\*.*\{\s*$')
IDENT = re.compile(r'[A-Za-z_][A-Za-z0-9_]*')


def strip_comments(line, state):
    """Remove // and /* */ comments quote-aware. state: 0 normal, 1 block comment."""
    out = []
    i, n = 0, len(line)
    while i < n:
        c = line[i]
        if state == 1:
            if c == '*' and i + 1 < n and line[i + 1] == '/':
                state = 0
                i += 2
            else:
                i += 1
        elif c == '/' and i + 1 < n and line[i + 1] == '/':
            break
        elif c == '/' and i + 1 < n and line[i + 1] == '*':
            state = 1
            i += 2
        elif c in '"\'':
            q = c
            out.append(c)
            i += 1
            while i < n:
                if line[i] == '\\':
                    out.append(line[i:i + 2])
                    i += 2
                    continue
                out.append(line[i])
                if line[i] == q:
                    i += 1
                    break
                i += 1
        else:
            out.append(c)
            i += 1
    return ''.join(out), state


def body_lines(path, def_re):
    """Return stripped non-blank body lines from the matched def through first column-0 `}`."""
    start = None
    with open(path, encoding='utf-8', errors='replace') as f:
        for idx, ln in enumerate(f):
            if def_re.match(ln.rstrip('\n')):
                start = idx
                break
        if start is None:
            return None, None
        out = []
        st = 0
        for ln in f:
            raw = ln.rstrip('\n')
            s, st = strip_comments(raw, st)
            s = s.strip()
            if s:
                out.append(s)
            if raw == '}' and st == 0:
                return out, start + 1
    return out, start + 1


def normalize(lines):
    mp = {}
    out = []
    for ln in lines:
        ln = IDENT.sub(lambda m: mp.setdefault(m.group(0), 'v%d' % (len(mp) + 1)), ln)
        out.append(' '.join(ln.split()))
    return out


def main():
    if len(sys.argv) != 4:
        print(__doc__)
        return 2
    try:
        sys.stdout.reconfigure(encoding='utf-8', errors='replace')
    except AttributeError:
        pass
    prod, src, fnid = sys.argv[1], sys.argv[2], sys.argv[3]
    def_src = re.compile(DEF_SRC_TMPL % re.escape(fnid))
    src_body, src_ln = body_lines(src, def_src)
    if src_body is None:
        print('FAIL: source def for %s not found in %s' % (fnid, src))
        return 2
    prod_body, prod_ln = body_lines(prod, DEF_PROD)
    if prod_body is None:
        print('FAIL: product def not found in %s' % prod)
        return 2
    ns, np_ = normalize(src_body), normalize(prod_body)
    print('%s: src body %d lines (from %d), product body %d lines (from %d)'
          % (fnid, len(ns), src_ln, len(np_), prod_ln))
    if ns == np_:
        print('IDENTICAL (%d normalized lines)' % len(ns))
        return 0
    diff = list(difflib.unified_diff(ns, np_, 'src', 'product', lineterm=''))
    minus = sum(1 for d in diff if d.startswith('-') and not d.startswith('---'))
    plus = sum(1 for d in diff if d.startswith('+') and not d.startswith('+++'))
    print('DIVERGENCE: -%d/+%d diff lines' % (minus, plus))
    for d in diff[:60]:
        print(d)
    return 1


if __name__ == '__main__':
    sys.exit(main())
