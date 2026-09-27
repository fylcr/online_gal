#!/usr/bin/env python
"""rw-splice.py — splice the rewritten products over their generated defs.

For every rewrite product registered in rewrite-names.tsv (kind=fn rows whose
file column names a live file under <tree>/src/), the generated definition of
w2c_artemis_f<NNN> in the assembled source is replaced by:

  1. the product body, verbatim (it defines <qualified target> in artc style,
     framing itself with EnterFrame()/LeaveFrame());
  2. a pure bridge thunk keeping the original signature and name, forwarding to
     the target one-liner.

Before the first product a `namespace artc` prototype block is injected (types
spelled the way the generated layer spells them) so products can call each other
regardless of definition order. The thunk keeps every generated call site and
the `static` forward declaration valid without renaming anything in the
generated layer. instrument-profile.py counts the product's `  EnterFrame();`
line (index taken from the `// ---- rewrite: f<NNN>` marker), so call totals
match the generated bodies exactly, product-to-product calls included.

Usage: python rw-splice.py <combined.c> <rewrite-root>
"""
import os
import re
import sys

DEFRE = re.compile(r'^([A-Za-z_]\w*) (w2c_artemis_f(\d+)\(w2c_artemis\* instance)')


def read_signature(lines, i):
    """Def at lines[i] -> (ret_type, sig-without-brace, param-name list, body_end)."""
    m = DEFRE.match(lines[i])
    sig_lines = []
    j = i
    while j < len(lines):
        sig_lines.append(lines[j])
        if lines[j].rstrip('\n').rstrip().endswith('{'):
            break
        j += 1
    else:
        sys.exit('f%s: def signature never opens a body' % m.group(3))
    k = j + 1
    while k < len(lines) and not lines[k].startswith('}'):
        k += 1
    if k >= len(lines):
        sys.exit('f%s: body never closes' % m.group(3))
    sig = re.sub(r'\s+', ' ', ''.join(sig_lines)).strip()
    if not sig.endswith('{'):
        sys.exit('f%s: bad signature block %r' % (m.group(3), sig[:80]))
    sig = sig[:-1].strip()
    params = sig[sig.index('(') + 1:sig.rindex(')')]
    parts = params.split(',')
    names = [re.search(r'([A-Za-z_]\w*)\s*$', p).group(1) for p in parts]
    return m.group(1), sig, names, k


def main():
    if len(sys.argv) != 3:
        sys.exit('usage: rw-splice.py <combined.c> <rewrite-root>')
    combined, rwroot = sys.argv[1], os.path.abspath(sys.argv[2])
    tsv = os.path.join(rwroot, 'rewrite-names.tsv')
    src_dir = os.path.join(rwroot, 'src')

    # A fresh flavour starts as an empty tree: no name table, no products.  That
    # is the un-transcribed baseline, not a failure, so it is legal here (unlike
    # in code-assemble.sh, where a mistyped archive path must not pass silently).
    if not os.path.isfile(tsv):
        print('rw-splice: no %s yet -- nothing to splice into %s'
              % (os.path.relpath(tsv, rwroot), os.path.basename(combined)))
        return

    # ---- products: kind=fn rows with a live product file --------------------
    products = {}
    for raw in open(tsv, encoding='utf-8'):
        if not raw.strip() or raw.startswith('#'):
            continue
        cols = raw.rstrip('\n').split('\t')
        if len(cols) < 5 or cols[0] != 'fn':
            continue
        fnid = int(cols[1].lstrip('f'))
        target, relfile = cols[3], cols[4]
        if relfile.endswith(('.h', '.hpp')):
            continue
        if os.path.isfile(os.path.join(src_dir, relfile)):
            products[fnid] = (target, relfile)
    if not products:
        print('rw-splice: no live rewrite products under %s -- nothing to do' % src_dir)
        return

    with open(combined, encoding='utf-8', errors='surrogateescape') as fh:
        lines = fh.read().splitlines(keepends=True)

    # ---- pre-scan: signatures feed the cross-product prototype block ---------
    sig_of = {}
    i = 0
    while i < len(lines):
        m = DEFRE.match(lines[i])
        if not m or int(m.group(3)) not in products:
            i += 1
            continue
        ret_type, sig, names, k = read_signature(lines, i)
        params = sig[sig.index('(') + 1:sig.rindex(')')]
        types = [re.sub(r'\s*[A-Za-z_]\w*\s*$', '', p).strip() for p in params.split(',')]
        sig_of[int(m.group(3))] = (ret_type, types)
        i = k + 1
    if set(sig_of) != set(products):
        sys.exit('rw-splice: no generated def for f%s'
                 % ', f'.join(str(f) for f in sorted(set(products) - set(sig_of))))
    proto = ['// ---- rewrite prototypes: cross-product calls resolve here ----\n',
             'namespace artc {\n']
    for fnid in sorted(products):
        ret_type, types = sig_of[fnid]
        proto.append('  %s %s(%s);\n' % (ret_type, products[fnid][0].split('::')[-1], ', '.join(types)))
    proto.append('}\n')
    proto = ''.join(proto)

    # ---- splice: product body + pure forwarding thunk per def ----------------
    out = []
    emitted_proto = False
    spliced = 0
    i = 0
    while i < len(lines):
        m = DEFRE.match(lines[i])
        if not m or int(m.group(3)) not in products:
            out.append(lines[i])
            i += 1
            continue
        fnid = int(m.group(3))
        target, relfile = products[fnid]
        ret_type, sig, names, k = read_signature(lines, i)
        prod_path = os.path.join(src_dir, relfile)
        with open(prod_path, encoding='utf-8') as fh:
            body = fh.read()
        if target.split('::')[-1] + '(' not in body:
            sys.exit('f%d: product %s does not define %s' % (fnid, relfile, target))
        if not emitted_proto:
            out.append(proto)
            emitted_proto = True
        out.append('// ---- rewrite: f%d (%s) ----\n' % (fnid, relfile))
        out.append(body)
        if not body.endswith('\n'):
            out.append('\n')
        out.append('// ---- bridge thunk: generated callers keep the original name ----\n')
        out.append(sig + ' {\n')
        call = '%s(%s)' % (target, ', '.join(names))
        if ret_type == 'void':
            out.append('  %s;\n' % call)
        else:
            out.append('  return %s;\n' % call)
        out.append('}\n')
        i = k + 1
        spliced += 1

    if spliced != len(products):
        sys.exit('rw-splice: %d products registered but %d defs spliced'
                 % (len(products), spliced))
    with open(combined, 'w', encoding='utf-8', errors='surrogateescape', newline='') as fh:
        fh.write(''.join(out))
    print('rw-splice: %d product(s) spliced: %s'
          % (spliced, ', '.join('f%d->%s' % (f, products[f][0]) for f in sorted(products))))


if __name__ == '__main__':
    main()
