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

    # ---- optional passthrough: keep the generated body for chosen products ----
    # RW_PASSTHROUGH=all | f101,f102 | util/foo.cpp,...  Bisecting a defect needs
    # a build where some products are compiled but their behaviour is still the
    # generated one; nothing may go undefined, so this never drops a product.
    # RW_PT_RUNTIME=1 keeps both bodies for every product and picks between them
    # at run time from a mask word in linear memory.  One build then answers a
    # whole bisect: patch the mask (ptmask.py) and re-run, no rebuild.  The mask
    # is a plain global array, so the css-style editing below is only in C.
    runtime = os.environ.get('RW_PT_RUNTIME', '').strip() not in ('', '0')
    passthrough = set()
    want = os.environ.get('RW_PASSTHROUGH', '')
    if runtime and not want.strip():
        want = 'all'
    if want.strip():
        by_rel = {rel: f for f, (_t, rel) in products.items()}
        for tok in want.replace(' ', '').split(','):
            if not tok:
                continue
            if tok == 'all':
                passthrough |= set(products)
            elif tok in by_rel:
                passthrough.add(by_rel[tok])
            elif re.fullmatch(r'f\d+', tok) and int(tok[1:]) in products:
                passthrough.add(int(tok[1:]))
            else:
                # A name that no product owns today is a no-op, not an error:
                # bisect lists are written once and reused as products come and go.
                sys.stderr.write('rw-splice: RW_PASSTHROUGH %r is not a live '
                                 'product, skipping\n' % tok)

    # Bit order is fixed by the product list, so a mask written for one build
    # still means the same thing in the next one.
    bits = {fnid: n for n, fnid in enumerate(
        sorted(products, key=lambda f: products[f][1]))}
    pt_words = (len(products) + 31) // 32
    pt_head = ''
    if runtime:
        # `volatile` is load-bearing: without it a table read only by
        # `ARTC_PT_OFF` tests is constant-folded away, so the sentinel words the
        # patcher searches for never reach the data segment and the mask cannot
        # be patched at all.  The table was `static` and non-const.
        pt_head = ('// ---- runtime bisect: a set bit keeps the generated body ----\n'
                   'static volatile uint32_t artc_pt_mask[%d] = {%s};\n'
                   '#define ARTC_PT_OFF(b) (artc_pt_mask[(b) >> 5] & (1u << ((b) & 31u)))\n\n'
                   % (pt_words, ', '.join(['0x9E3779B9u'] * pt_words)))
        with open(os.path.join(os.path.dirname(os.path.abspath(combined)),
                               'pt-map.tsv'), 'w', encoding='utf-8') as fh:
            fh.write('# words\t%d\n' % pt_words)
            for fnid in sorted(bits, key=lambda f: bits[f]):
                fh.write('%d\tf%d\t%s\n'
                         % (bits[fnid], fnid, products[fnid][1]))

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
    # A nested target (`artc::LuaEngine::Call`) has to be declared inside its own
    # namespace: the thunk below calls it by its full name and every product that
    # spells the callee the way the generator's NESTED table does (`LuaEngine::Call`)
    # needs the namespace open, not a bare `Call` in artc.
    groups = {}
    for fnid in sorted(products):
        parts = products[fnid][0].split('::')
        groups.setdefault(tuple(parts[1:-1]), []).append((fnid, parts[-1]))
    proto = ['// ---- rewrite prototypes: cross-product calls resolve here ----\n',
             'namespace artc {\n']
    for qual in sorted(groups):
        for depth, name in enumerate(qual):
            proto.append('%snamespace %s {\n' % ('  ' * (depth + 1), name))
        for fnid, short in groups[qual]:
            ret_type, types = sig_of[fnid]
            proto.append('%s%s %s(%s);\n'
                         % ('  ' * (len(qual) + 1), ret_type, short, ', '.join(types)))
        for depth in reversed(range(len(qual))):
            proto.append('%s}  // namespace %s\n' % ('  ' * (depth + 1), qual[depth]))
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
            out.append(pt_head)
            emitted_proto = True
        short = target.split('::')[-1]
        parts = target.split('::')
        if fnid in passthrough:
            # Keep the generated body under a fresh name, then route BOTH the
            # generated layer and the artc callers to it.  The product file stays
            # compiled (under a probe name) so the helper names its siblings call
            # keep resolving -- nothing may become undefined.
            gen = 'w2c_artemis_f%d_wasm' % fnid
            out.append('// ---- passthrough: f%d keeps the generated body ----\n' % fnid)
            out.extend(re.sub(r'\bw2c_artemis_f%d\b' % fnid, gen, line)
                       for line in lines[i:k + 1])
            out.append('// ---- passthrough: %s (%s) stays compiled for its helpers ----\n'
                       % (target, relfile))
            m_def = re.search(r'(?m)^[A-Za-z_][A-Za-z0-9_:<>\*&\s]*\b'
                              + re.escape(short) + r'\s*\(', body)
            if m_def is None:
                m_def = re.search(re.escape(short) + r'\s*\(', body)
            if m_def is None:
                sys.stderr.write('   passthrough: %s does not define %s, '
                                 'splice kept\n' % (relfile, short))
                continue
            # The forwarder has to spell the parameters exactly the way the
            # product's own definition does, or the two overloads turn every
            # converted call site ambiguous.
            open_i = body.index('(', m_def.start())
            depth, j = 0, open_i
            while j < len(body):
                if body[j] == '(':
                    depth += 1
                elif body[j] == ')':
                    depth -= 1
                    if depth == 0:
                        break
                j += 1
            params = body[open_i + 1:j]
            line_start = body.rfind('\n', 0, m_def.start()) + 1
            ret_prod = body[line_start:m_def.start()].strip() or ret_type
            pnames = [re.search(r'([A-Za-z_]\w*)\s*$', p).group(1)
                      for p in params.split(',')] if params.strip() else []
            call = '%s(%s)' % (gen, ', '.join(pnames))
            indent = '  ' * (len(parts) - 2)
            ret_kw = '' if ret_prod == 'void' else 'return '
            if runtime:
                # The switch sits in front of the product body, so both the
                # generated layer (through the thunk) and the artc callers meet
                # the same decision.  A void product's selected arm must
                # *return*: a bare call would run `gen(...)` and then fall
                # through into `_impl(...)`, making one mask bit select BOTH
                # bodies.  A value-returning product keeps the call as the tail
                # expression (`return gen(...)`), which already returns.
                impl_name = short + '_impl'
                arm = 'return %s;' % call if ret_kw else '%s; return;' % call
                forwarder = ('%s%s %s_impl(%s);\n'
                             '%s%s %s(%s) {\n'
                             '%s  if (ARTC_PT_OFF(%du)) {\n%s    %s\n%s  }\n'
                             '%s  %s%s(%s);\n%s}\n\n'
                             % (indent, ret_prod, short, params,
                                indent, ret_prod, short, params,
                                indent, bits[fnid], indent, arm, indent,
                                indent, ret_kw, impl_name, ', '.join(pnames), indent))
            else:
                impl_name = short + '_disabled_probe'
                forwarder = ('%s%s %s(%s) {\n%s  %s%s;\n%s}\n\n'
                             % (indent, ret_prod, short, params, indent,
                                ret_kw, call, indent))
            # The product opens and closes its own namespaces, so the forwarder
            # goes on the definition's own line-start: same namespace, and
            # before the rename shifts nothing.
            out.append(body[:line_start] + forwarder
                       + body[line_start:m_def.start()]
                       + body[m_def.start():open_i + 1]
                       .replace(short, impl_name, 1)
                       + body[open_i + 1:])
            if not out[-1].endswith('\n'):
                out.append('\n')
            # The generated layer calls the bridge name, not the product name, so
            # the thunk it used to find has to exist.  Under the runtime switch it
            # has to reach the *chooser*, otherwise the generated layer would keep
            # calling the generated body and the mask would only ever move the
            # calls our own products make -- which never enter the engine on
            # their own.
            thunk_call = '%s(%s)' % (target if runtime else gen, ', '.join(names))
            out.append(sig + ' {\n')
            if ret_type == 'void':
                out.append('  %s;\n' % thunk_call)
            else:
                out.append('  return %s;\n' % thunk_call)
            out.append('}\n')
        else:
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
