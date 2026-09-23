#!/usr/bin/env python3
"""
Emit C definitions for every host function that a `wasm2c` output imports.

`wasm2c` turns a WebAssembly module into a C translation unit plus a header.  The
header declares, for every imported function, both the callable signature and the
slot the instance will hold it in:

    /* import: 'a' '$' */
    u32 w2c_a_0x24(struct w2c_a*, u32, u32, u32, u32, u32, u32, u32, u32);

Nothing defines those functions -- in a real embedder they are supplied by the
host.  This script writes a translation unit of do-nothing definitions so that the
module can be *linked*, which is what makes "the transpiled C compiles back into a
wasm binary" a statement that can be checked rather than asserted.

Two details matter:

  * The memory accessor (`w2c_a_a`) is not a stub.  `init_instance_import` calls it
    to fill `instance->w2c_a_a`, and every memory read in the module goes through
    that pointer, so a stub returning nothing would fault on the first load.  It
    gets a real `wasm_rt_allocate_memory` instead.
  * Non-void stubs return zero, and void stubs do nothing.  Zero is what a
    differential harness must give both sides: the original module is exercised
    with JavaScript stubs that also return zero, so the two can be fed identical
    inputs and compared.  Trapping instead would abort on the first host call and
    make the comparison vacuous.

`--forward` keeps the parsing and the file's skeleton but changes what the body
does.  Instead of defining the host side, each function declares a real wasm
import under the module and name the header recorded, and the wasm2c definition
becomes a thin call into it.  The rebuilt module then *consumes* the host ABI from
the host, exactly as the official module does, rather than carrying a private
zero-returning copy of it -- which is what makes it a faithful rebuild instead of
a module whose host functions were replaced by constants.  The imports are named
by a sanitised C identifier so that a host name which is not a C identifier (`$`,
`_c`) still gets one; the original spelling rides along in `import_name`.

Usage: wasm-import-stubs.py <artemis.h> <out.c> [--forward] [--memory-initial N] [--memory-max N]
"""

import argparse
import os
import re
import sys

# Matches a declaration the header emits for an imported function.  The return type
# is either a core type alias (`u32`, `f64`, ...), a pointer to a memory struct, or
# `void`.  The second group is the name, the third the parameter list verbatim.
DECL = re.compile(
    r"^(?P<ret>(?:void|f32|f64|u32|s32|u64|s64|u8|s8|u16|s16|wasm_rt_memory_t\s*\*))"
    r"\s+(?P<name>w2c_a_[A-Za-z0-9_]+)\s*\((?P<args>[^)]*)\)\s*;",
    re.MULTILINE,
)

# The `/* import: 'MODULE' 'NAME' */` comment and the declaration it introduces.
# The module and the ORIGINAL name exist nowhere else -- wasm2c has already mangled
# the name into `w2c_...` by the time it reaches a C identifier -- so `--forward`
# has to read them back out of the comment to re-declare the import faithfully.
# The accessor's own `/* import: 'a' 'a' */` does not match: a struct member has no
# parameter list, and the declaration is what this requires.
IMPORT = re.compile(
    r"/\* import: '(?P<module>[^']*)' '(?P<orig>[^']*)' \*/\s*"
    r"(?P<ret>(?:void|f32|f64|u32|s32|u64|s64|u8|s8|u16|s16|wasm_rt_memory_t\s*\*))"
    r"\s+(?P<name>w2c_a_[A-Za-z0-9_]+)\s*\((?P<args>[^)]*)\)\s*;",
    re.MULTILINE,
)


def import_ident(orig: str) -> str:
    """The C identifier a wasm import is declared under.

    Identifier characters survive and every other byte becomes `_HH`, so the result
    is always a legal C identifier and, because the escape is injective, two
    different wasm names can never share one.  Writing the name through unescaped
    would collide `$` with a literal `_24`, silently pointing one import's
    forwarder at the other import's host function.
    """
    return "artc_import_" + re.sub(r"[^A-Za-z0-9_]", lambda m: "_%02X" % ord(m.group(0)), orig)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("header", help="the wasm2c-generated .h")
    ap.add_argument("out", help="the .c to write")
    ap.add_argument(
        "--forward",
        action="store_true",
        help="emit forwarders to real wasm imports instead of zero-returning stubs",
    )
    ap.add_argument("--memory-initial", type=int, default=256)
    ap.add_argument("--memory-max", type=int, default=65536)
    args = ap.parse_args()

    text = open(args.header, encoding="utf-8", errors="replace").read()

    # `w2c_a_a` is the memory accessor, declared with `extern` above the imports.
    # It is not one of the stubs: it has to hand back a live memory.
    decls = []
    seen = set()
    for m in DECL.finditer(text):
        name = m.group("name")
        if name in seen or name == "w2c_a_a":
            continue
        seen.add(name)
        decls.append((m.group("ret"), name, m.group("args").strip()))

    module = os.path.splitext(os.path.basename(args.header))[0]

    # With `--forward` each stub is preceded by the import comment and the import
    # declaration it forwards to.  The block comes straight out of the header, in
    # header order, so the emitted file reads like the import section it rebuilds;
    # the `struct w2c_a*` type it mentions comes from the same header.
    blocks = []
    if args.forward:
        seen = set()
        for m in IMPORT.finditer(text):
            name = m.group("name")
            if name in seen or name == "w2c_a_a":
                continue
            seen.add(name)
            blocks.append(
                (m.group("module"), m.group("orig"), m.group("ret"), name, m.group("args").strip())
            )

    with open(args.out, "w", encoding="utf-8", newline="\n") as f:
        if args.forward:
            f.write(
                "/* Generated by wasm-import-stubs.py -- do not edit.\n"
                " *\n"
                " * Forwarders for every host function this module imports, so that the\n"
                " * rebuilt module consumes the host ABI from the host rather than carrying\n"
                " * a private zero-returning copy of it.  Each forwarder declares a real wasm\n"
                " * import under the module and name the original module used and calls it;\n"
                " * see the script for why the name is sanitised, and why the memory accessor\n"
                " * is handled separately.\n"
                " */\n"
            )
        else:
            f.write(
                "/* Generated by wasm-import-stubs.py -- do not edit.\n"
                " *\n"
                " * Zero-returning definitions for every host function this module imports,\n"
                " * so that the wasm2c output can be linked back into a wasm binary.  See\n"
                " * the script for why they return zero rather than trapping, and why the\n"
                " * memory accessor is handled separately.\n"
                " */\n"
            )
        f.write(
            f'#include "{os.path.basename(args.header)}"\n'
            "#include \"wasm-rt.h\"\n"
            "\n"
            "/* The module imports its memory, so this is the host's.  The size is the\n"
            " * one the official module declares. */\n"
            "static wasm_rt_memory_t g_memory;\n"
            "\n"
            "wasm_rt_memory_t* w2c_a_a(struct w2c_a* instance) {\n"
            "  (void)instance;\n"
            "  if (!g_memory.data) {\n"
            f"    wasm_rt_allocate_memory(&g_memory, {args.memory_initial}, {args.memory_max}, false, WASM_DEFAULT_PAGE_SIZE);\n"
            "  }\n"
            "  return &g_memory;\n"
            "}\n"
            "\n"
            f"/* {len(decls)} imports. */\n"
        )
        if args.forward:
            first = True
            for module_name, orig, ret, name, params in blocks:
                types = [p.strip() for p in params.split(",")]
                # The first declared parameter is the instance pointer the header
                # leaves unnamed; the host import does not take one, so it is named
                # `instance` for the definition and dropped from both the declaration
                # and the call.  The host is not told about the instance.
                assert types and types[0].startswith("struct w2c_a"), (orig, params)
                rest = types[1:]
                named = ", ".join(f"{t} p{i}" for i, t in enumerate(rest))
                call = ", ".join(f"p{i}" for i in range(len(rest)))
                # Blank line between pairs, so each import reads as one block: the
                # comment it came from, the import it declares, and the forwarding
                # definition -- grouped the way the header groups them.
                if first:
                    first = False
                else:
                    f.write("\n")
                f.write(f"/* import: '{module_name}' '{orig}' */\n")
                f.write(f'__attribute__((import_module("{module_name}"), import_name("{orig}")))\n')
                # The declaration is a prototype, so an empty parameter list means an
                # empty parameter list; the definition needs the instance parameter to
                # be the only one, and `void` is only writable as the SOLE parameter.
                f.write(f"{ret} {import_ident(orig)}({named or 'void'});\n")
                f.write(f"{ret} {name}(struct w2c_a* instance{', ' + named if named else ''}) {{\n")
                f.write("  (void)instance;\n")
                if ret == "void":
                    f.write(f"  {import_ident(orig)}({call});\n")
                else:
                    f.write(f"  return {import_ident(orig)}({call});\n")
                f.write("}\n")
        else:
            for ret, name, params in decls:
                f.write(f"{ret} {name}({params}) {{\n")
                if ret == "void":
                    f.write("  (void)0;\n")
                else:
                    # Same value the JS side returns, so the two runs are comparable.
                    f.write("  return 0;\n")
                f.write("}\n")

    print(f"{os.path.basename(args.out)}: {len(decls)} import stubs for {module}")


if __name__ == "__main__":
    sys.exit(main())
