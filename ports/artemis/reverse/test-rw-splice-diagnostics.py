#!/usr/bin/env python
"""Regression tests for the two diagnostic defects fixed in rw-splice.py.

Scope: these are *diagnostic fixtures*, not a rebuild.  They drive the splicer
over a synthetic generated layer and assert on the text it emits, so a
regression is caught in milliseconds and ports/artemis/src is never touched.
A real compiler is used only where a compiler is the thing under test (the
`volatile` sentinel probe); when none is present that one test reports a
genuine skip.

Defect 1 -- mask sentinel folded away at higher optimisation
    `static uint32_t artc_pt_mask[]` read only by ARTC_PT_OFF tests is constant
    folded, so no sentinel reaches the data segment and the runtime bisect mask
    cannot be patched at all.  The declaration must be `static volatile uint32_t`.

Defect 2 -- void forwarder selects BOTH bodies
    The runtime forwarder emitted `gen(...)` for a void product and then fell
    through to `_impl(...)`, so a set mask bit ran the generated body *and* the
    rewritten body.  The selected arm must be `gen(...); return;`.  The
    value-returning arm (`return gen(...)`) and the thunk that routes the
    generated layer to the chooser must both stay as they are.
"""
import os
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest

HERE = os.path.dirname(os.path.abspath(__file__))
SPLICE = os.path.join(HERE, 'rw-splice.py')

# ---- synthetic generated layer -------------------------------------------
# Two products: f101 returns u32, f102 returns void.  The defs are spelled the
# way wasm2c spells them (no `static`, which DEFRE requires to be absent), so
# read_signature() takes its real path.  The parameter types are wasm2c types
# (`u32`), because the cross-product prototype block copies the generated
# spelling verbatim rather than the product's own C++ spelling.
COMBINED = '''\
// synthetic generated layer (wasm2c shape)
// Counters let the compiled run tell the generated body and the product body
// apart; the product files bump the matching `p_ctr_*`.
unsigned g_ctr_value, g_ctr_void;
static u32 w2c_g1;
u32 w2c_artemis_f101(w2c_artemis* instance, u32 var_p0) {
  u32 var_i0 = 0;
  FUNC_PROLOGUE;
  g_ctr_value++;
  return var_i0;
}
void w2c_artemis_f102(w2c_artemis* instance, u32 var_p0) {
  FUNC_PROLOGUE;
  g_ctr_void++;
  return;
}
'''

TSV = (
    '# synthetic rewrite-names.tsv\n'
    'fn\tf101\tgen_value\tartc::ValueProduct\tvalue/product.cpp\tR1\ttest\n'
    'fn\tf102\tgen_void\tartc::VoidProduct\tvoid/product.cpp\tR1\ttest\n'
)

VALUE_PRODUCT = '''\
// value/product.cpp -- synthetic; no codegen is inferred from a real product.
#include "util/engine_context.h"

extern unsigned g_ctr_value, g_ctr_void;
extern unsigned p_ctr_value, p_ctr_void;

namespace artc {

uint32_t ValueProduct(EngineContext* ctx, uint32_t p0) {
  (void)ctx;
  p_ctr_value++;
  return p0 + 1u;
}

}  // namespace artc
'''

VOID_PRODUCT = '''\
// void/product.cpp -- synthetic; no codegen is inferred from a real product.
#include "util/engine_context.h"

extern unsigned g_ctr_value, g_ctr_void;
extern unsigned p_ctr_value, p_ctr_void;

namespace artc {

void VoidProduct(EngineContext* ctx, uint32_t p0) {
  (void)ctx;
  (void)p0;
  p_ctr_void++;
}

}  // namespace artc
'''

# The one header the fixture products include.  It must supply exactly what the
# spliced text assumes: the generated layer's scalar spelling, `EngineContext` as
# an alias of `w2c_artemis`, and a `FUNC_PROLOGUE` that is valid outside a real
# generated body.
ENGINE_CONTEXT_H = '''\
// util/engine_context.h -- synthetic stand-in for the port's real header.
#ifndef UTIL_ENGINE_CONTEXT_H_
#define UTIL_ENGINE_CONTEXT_H_
#include <stdint.h>
typedef uint32_t u32;
typedef struct w2c_artemis w2c_artemis;
typedef w2c_artemis EngineContext;
#define FUNC_PROLOGUE do { } while (0)
#endif
'''


def run_splice(tmpdir, env_extra):
    """Run the splicer over the fixture; return the produced source text."""
    combined = os.path.join(tmpdir, 'combined.c')
    with open(combined, 'w', encoding='utf-8') as fh:
        fh.write(COMBINED)
    env = dict(os.environ)
    env.pop('RW_PASSTHROUGH', None)
    env.pop('RW_PT_RUNTIME', None)
    env.update(env_extra)
    proc = subprocess.run([sys.executable, SPLICE, combined, tmpdir],
                          capture_output=True, text=True, env=env)
    if proc.returncode != 0:
        raise AssertionError('rw-splice failed: %s%s' % (proc.stdout, proc.stderr))
    with open(combined, encoding='utf-8') as fh:
        return fh.read()


class Fixture(object):
    def __init__(self):
        self.dir = os.path.realpath(tempfile.mkdtemp(prefix='rw-splice-test-'))

    def build(self):
        src = os.path.join(self.dir, 'src')
        for rel, body in (('value/product.cpp', VALUE_PRODUCT),
                          ('void/product.cpp', VOID_PRODUCT)):
            path = os.path.join(src, rel)
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, 'w', encoding='utf-8') as fh:
                fh.write(body)
        with open(os.path.join(self.dir, 'rewrite-names.tsv'), 'w', encoding='utf-8') as fh:
            fh.write(TSV)
        return self.dir

    def header(self, rel, body):
        """Write a support file the *emitted* text includes (engine_context.h)."""
        path = os.path.join(self.dir, rel)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, 'w', encoding='utf-8') as fh:
            fh.write(body)
        return path

    def close(self):
        # Only ever remove a directory this fixture created, under the system
        # temp dir, with our own prefix: never a computed or relative path.
        d = self.dir
        assert os.path.isabs(d), d
        assert os.path.dirname(d) == os.path.realpath(tempfile.gettempdir()), d
        assert os.path.basename(d).startswith('rw-splice-test-'), d
        shutil.rmtree(d, ignore_errors=True)


def body_of(text, name):
    """Extract `... name(...) { ... }` by brace matching; None if absent."""
    m = re.search(r'(?m)^[ \t]*[A-Za-z_][\w:<>*&\s]*\b%s\s*\([^)]*\)\s*\{' % re.escape(name),
                  text)
    if not m:
        return None
    depth, j = 0, text.index('{', m.start())
    start = m.start()
    while j < len(text):
        if text[j] == '{':
            depth += 1
        elif text[j] == '}':
            depth -= 1
            if depth == 0:
                return text[start:j + 1]
        j += 1
    return None


def compile_obj(cc, tmpdir, name, prog, opt='-O1'):
    """Compile prog at -O1/-O0; return the object path.

    A compile failure is a real failure: the emitted probe is valid C, so the
    only reason to skip is a missing compiler, which the caller checks.
    """
    cfile = os.path.join(tmpdir, name + '.c')
    obj = os.path.join(tmpdir, name + '.o')
    with open(cfile, 'w') as fh:
        fh.write(prog)
    proc = subprocess.run([cc, opt, '-c', cfile, '-o', obj],
                         capture_output=True, text=True)
    if proc.returncode != 0:
        raise AssertionError('%s failed on the emitted probe:\n%s'
                             % (opt, proc.stderr[:2000]))
    return obj


class MaskSentinel(unittest.TestCase):
    """Defect 1: the patchable sentinel must be volatile, so -O1 keeps it."""

    def setUp(self):
        self.fx = Fixture()
        self.fx.build()

    def tearDown(self):
        self.fx.close()

    def test_volatile_declaration_and_sentinel(self):
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        decl = re.search(r'static\s+(volatile\s+)?uint32_t\s+artc_pt_mask\[(\d+)\]\s*=\s*\{([^}]*)\}',
                         text)
        self.assertIsNotNone(decl, 'artc_pt_mask declaration not emitted')
        self.assertEqual(decl.group(1), 'volatile ',
                         'mask must be `static volatile uint32_t` or the optimiser '
                         'folds the sentinel out of the data segment and the mask '
                         'cannot be patched')
        words = int(decl.group(2))
        self.assertEqual(words, 1, 'two products must occupy one mask word')
        inits = [w.strip() for w in decl.group(3).split(',')]
        self.assertEqual(inits, ['0x9E3779B9u'] * words,
                         'every word must carry the sentinel the patcher searches for')

    def test_optimiser_keeps_the_sentinel(self):
        """The point of `volatile`: prove it against a real compiler, not by grep.

        Compiles the emitted mask + one reader at -O1 and reads the words back out
        of the object's data section.  Skipped only when no C compiler is available.
        """
        cc = shutil.which('cc') or shutil.which('clang') or shutil.which('gcc')
        if not cc:
            self.skipTest('no C compiler available to prove the -O1 behaviour')
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        # The declaration AND the ARTC_PT_OFF macro that consumes it: the macro
        # tests the mask, so taking only the declaration would leave the reader
        # undefined (and the host test would fail on an implicit function).
        snippet = re.search(r'static[^\n]*artc_pt_mask\[[^\n]*\n'
                            r'#define ARTC_PT_OFF[^\n]*\n', text).group(0)
        prog = ('#include <stdint.h>\n' + snippet +
                'uint32_t probe(void) { return ARTC_PT_OFF(0u) ? 1u : 0u; }\n')
        obj = compile_obj(cc, self.fx.dir, 'probe', prog, '-O1')
        with open(obj, 'rb') as fh:
            blob = fh.read()
        self.assertIn(struct.pack('<I', 0x9E3779B9), blob,
                      'sentinel words are absent from the -O1 object: the mask '
                      'would be un-patchable in the shipped module')


class VoidSelection(unittest.TestCase):
    """Defect 2: a set bit must select exactly one body, for void and for value."""

    def setUp(self):
        self.fx = Fixture()
        self.fx.build()

    def tearDown(self):
        self.fx.close()

    def test_runtime_void_arm_returns(self):
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        fwd = body_of(text, 'VoidProduct')
        self.assertIsNotNone(fwd, 'runtime forwarder for the void product not emitted')
        arm = re.search(r'if\s*\(ARTC_PT_OFF\((\d+)u\)\)\s*\{(.*?)\n\s*\}', fwd, re.S)
        self.assertIsNotNone(arm, 'no mask test in the void forwarder')
        self.assertRegex(arm.group(2), r'w2c_artemis_f102_wasm\s*\([^;]*\)\s*;',
                         'the generated body must be called in the selected arm')
        self.assertRegex(arm.group(2), r'\breturn\s*;',
                         'the void arm must return, otherwise it falls through and '
                         'runs _impl as well: one bit would select both bodies')
        # the unselected path stays the rewritten body, exactly once
        self.assertEqual(len(re.findall(r'VoidProduct_impl\s*\([^;]*\)\s*;', fwd)), 1,
                         'the _impl body must be reached exactly once')

    def test_runtime_value_arm_returns_the_call(self):
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        fwd = body_of(text, 'ValueProduct')
        self.assertIsNotNone(fwd, 'runtime forwarder for the value product not emitted')
        arm = re.search(r'if\s*\(ARTC_PT_OFF\((\d+)u\)\)\s*\{(.*?)\n\s*\}', fwd, re.S)
        self.assertIsNotNone(arm, 'no mask test in the value forwarder')
        self.assertRegex(arm.group(2), r'return\s+w2c_artemis_f101_wasm\s*\(',
                         'the value arm must `return` the generated call')
        self.assertNotRegex(arm.group(2), r'return\s*;\s*\n\s*\}',
                            'the value arm must not be turned into a bare return')

    def test_each_chooser_reaches_exactly_one_body(self):
        """No implicit double-body: every path through the chooser runs one body."""
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        for name, gen, impl in (('ValueProduct', 'w2c_artemis_f101_wasm', 'ValueProduct_impl'),
                                ('VoidProduct', 'w2c_artemis_f102_wasm', 'VoidProduct_impl')):
            fwd = body_of(text, name)
            self.assertIsNotNone(fwd, '%s forwarder missing' % name)
            self.assertEqual(fwd.count(gen + '('), 1,
                             '%s: generated body called %d times' % (name, fwd.count(gen + '(')))
            self.assertEqual(fwd.count(impl + '('), 1,
                             '%s: rewritten body called %d times' % (name, fwd.count(impl + '(')))

    def test_generated_layer_thunk_reaches_the_chooser(self):
        """Under RW_PT_RUNTIME the bridge name must call the product, not gen()."""
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        for fnid, target in ((101, 'ValueProduct'), (102, 'VoidProduct')):
            thunk = body_of(text, 'w2c_artemis_f%d' % fnid)
            self.assertIsNotNone(thunk, 'bridge thunk f%d missing' % fnid)
            self.assertIn(target + '(', thunk,
                          'f%d: the generated layer must meet the chooser, else the mask '
                          'only moves our own products\' calls' % fnid)
            self.assertNotIn('w2c_artemis_f%d_wasm(' % fnid, thunk,
                             'f%d: the thunk must not bypass the chooser' % fnid)

    def test_forward_declarations_present(self):
        """The crossproto block declares every product, and each chooser declares
        its own `_impl` before calling it.

        Types are spelled the way the *generated* layer spells them (`u32`), not
        the way the product's own C++ spells them -- the block is copied from the
        generated signature.
        """
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        for decl in ('u32 ValueProduct(w2c_artemis*, u32);',
                     'void VoidProduct(w2c_artemis*, u32);'):
            self.assertIn(decl, text, 'missing cross-product prototype: %s' % decl)
        # Each `_impl` forward declaration must precede its chooser's definition,
        # or the chooser calls an undeclared function.
        for fwd_decl, chooser in (('u32 ValueProduct_impl(EngineContext* ctx, uint32_t p0);',
                                   'u32 ValueProduct(EngineContext* ctx, uint32_t p0) {'),
                                  ('void VoidProduct_impl(EngineContext* ctx, uint32_t p0);',
                                   'void VoidProduct(EngineContext* ctx, uint32_t p0) {')):
            di = text.find(fwd_decl)
            ci = text.find(chooser)
            self.assertNotEqual(di, -1, 'missing _impl forward declaration: %s' % fwd_decl)
            self.assertNotEqual(ci, -1, 'missing chooser definition: %s' % chooser)
            self.assertLess(di, ci,
                            'the _impl forward declaration must precede the chooser '
                            'that calls it')

    def test_non_runtime_keeps_the_simple_thunk(self):
        """Without the mask there is no chooser: the forwarder calls the generated body.

        This is the `RW_PASSTHROUGH` *diagnostic* mode, not shipping.  The
        product file is still compiled -- renamed to `<name>_disabled_probe` -- so
        the helper names its siblings call keep resolving; the forwarder under the
        product's own name calls the renamed generated body instead.  So the mode
        emits a probe definition, but neither `_impl` nor any mask.
        (Shipping is env{}: the product *replaces* the generated body.)
        """
        text = run_splice(self.fx.dir, {'RW_PASSTHROUGH': 'all'})
        self.assertNotIn('ARTC_PT_OFF', text,
                         'no mask should be emitted without RW_PT_RUNTIME')
        self.assertNotIn('artc_pt_mask', text)
        self.assertNotIn('_impl', text,
                         'the mask chooser is runtime-mode only')
        for fnid, gen, probe in ((101, 'w2c_artemis_f101_wasm', 'ValueProduct_disabled_probe'),
                                 (102, 'w2c_artemis_f102_wasm', 'VoidProduct_disabled_probe')):
            thunk = body_of(text, 'w2c_artemis_f%d' % fnid)
            self.assertIsNotNone(thunk, 'bridge thunk f%d missing' % fnid)
            self.assertIn(gen + '(', thunk,
                          'f%d: non-runtime passthrough thunk must call the generated '
                          'body' % fnid)
            # the product body survives, compiled but renamed so it cannot be reached
            self.assertIsNotNone(
                body_of(text, probe),
                '%s must stay compiled so the product\'s helper names keep resolving'
                % probe)
            # the renamed generated def exists exactly once
            self.assertEqual(
                len(re.findall(r'(?m)^\S[^\n]*\b%s\s*\(' % gen, text)), 1,
                '%s must have exactly one definition' % gen)

    def test_no_mask_normal_shipping_has_no_doubled_bodies(self):
        """Shipping shape: one body per product, no `_wasm`/probe artefact.

        This is the distinct no-mask case: with neither RW_PT_RUNTIME nor
        RW_PASSTHROUGH the product replaces the generated body outright, so the
        thunk must call the product and nothing may reference a generated `_wasm`
        def or a `_disabled_probe`.
        """
        text = run_splice(self.fx.dir, {})
        self.assertNotIn('ARTC_PT_OFF', text)
        self.assertNotIn('artc_pt_mask', text)
        self.assertNotIn('_wasm', text,
                         'no generated `_wasm` body may survive the plain splice')
        self.assertNotIn('_disabled_probe', text)
        for fnid, target in ((101, 'artc::ValueProduct'), (102, 'artc::VoidProduct')):
            thunk = body_of(text, 'w2c_artemis_f%d' % fnid)
            self.assertIsNotNone(thunk, 'bridge thunk f%d missing' % fnid)
            self.assertIn(target + '(', thunk,
                          'f%d: the thunk must call the product' % fnid)
        # the product body is spliced verbatim, exactly once, under its own name.
        # A definition opens a brace; the crossproto block's forward declaration
        # ends in `;` and must not be counted as a second definition.
        self.assertEqual(len(re.findall(r'(?m)^\s*uint32_t ValueProduct\([^)]*\)\s*\{', text)), 1)
        self.assertEqual(len(re.findall(r'(?m)^\s*void VoidProduct\([^)]*\)\s*\{', text)), 1)

    def test_mask_selects_exactly_one_body_by_compilation(self):
        """Compile the *emitted* spliced text and prove the mask picks one body.

        No reimplementation: the whole fixture emitted by rw-splice.py under
        RW_PT_RUNTIME is compiled as C++ against a stub util/engine_context.h, and
        main() drives the real bridge thunks.  Each generated body and each
        product body bumps its own counter, so a 0 bit must run only the product
        (impl counters) and a 1 bit only the generated bodies (gen counters) --
        for the void product AND the valued one.  A compile failure is a real
        failure; only a missing compiler skips.
        """
        cc = shutil.which('g++') or shutil.which('clang++') or shutil.which('c++')
        if not cc:
            self.skipTest('no C++ compiler available to prove mask selection')
        self.fx.header('util/engine_context.h', ENGINE_CONTEXT_H)
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        # Only the counter definitions and main() are added: every arm under test
        # is the splicer's own emitted text, verbatim.  main() comes after it so
        # it sees the mask and the thunks.
        prog = ('#include "util/engine_context.h"\n') + text + (
            '\nunsigned p_ctr_value, p_ctr_void;\n'
            'static void reset(void) { g_ctr_value = g_ctr_void = 0;'
            ' p_ctr_value = p_ctr_void = 0; }\n'
            'int main() {\n'
            '  int bad = 0;\n'
            '  reset();\n'
            '  artc_pt_mask[0] = 0u;\n'
            '  w2c_artemis_f101((w2c_artemis*)0, 1u);\n'
            '  w2c_artemis_f102((w2c_artemis*)0, 0u);\n'
            '  if (g_ctr_value || g_ctr_void\n'
            '      || p_ctr_value != 1u || p_ctr_void != 1u) bad = 1;\n'
            '  reset();\n'
            '  artc_pt_mask[0] = 0xFFFFFFFFu;\n'
            '  w2c_artemis_f101((w2c_artemis*)0, 1u);\n'
            '  w2c_artemis_f102((w2c_artemis*)0, 0u);\n'
            '  if (g_ctr_value != 1u || g_ctr_void != 1u\n'
            '      || p_ctr_value || p_ctr_void) bad = 2;\n'
            '  return bad;\n'
            '}\n')
        cfile = os.path.join(self.fx.dir, 'emitted.cpp')
        exe = os.path.join(self.fx.dir, 'emitted.exe')
        with open(cfile, 'w', encoding='utf-8') as fh:
            fh.write(prog)
        proc = subprocess.run([cc, '-O1', '-I', self.fx.dir, '-o', exe, cfile],
                              capture_output=True, text=True)
        if proc.returncode != 0:
            raise AssertionError('the emitted spliced text did not compile:\n%s'
                                 % proc.stderr[:2000])
        run = subprocess.run([exe], capture_output=True, text=True)
        self.assertEqual(run.returncode, 0,
                         'mask selection wrong (rc=%d: 1 = bit 0 ran a generated body '
                         'or skipped a product body, 2 = bit 1 ran a product body or '
                         'skipped a generated body)' % run.returncode)

    def test_void_product_definitions_unchanged_by_the_fix(self):
        """The product body itself is spliced verbatim; only the arm changed."""
        text = run_splice(self.fx.dir, {'RW_PT_RUNTIME': '1'})
        self.assertIn('(void)ctx;', text, 'the product body must be spliced verbatim')


if __name__ == '__main__':
    unittest.main(verbosity=2)
