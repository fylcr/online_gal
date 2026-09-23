#!/usr/bin/env python3
"""
Make a rebuilt Artemis module import *only* from the module the official glue supplies.

## Why this exists

The official module and a module rebuilt from its reverse-engineered sources both need a
host, but they ask for one in different words. The official module imports every host
function from a single module named `a` -- the object the glue builds from its
`asmLibraryArg` -- so `WebAssembly.instantiate` never sees a second module name. A
rebuild linked by a current Emscripten also imports its own runtime plumbing:

    env._Unwind_RaiseException           env._emscripten_throw_longjmp
    wasi_snapshot_preview1.fd_write      ...fd_read  ...fd_close  ...fd_seek
    wasi_snapshot_preview1.clock_time_get  ...proc_exit  ...fd_fdstat_get

The official module imports **none** of these: it was built without C++ exceptions and
with its file descriptors and clock wired to the glue's own `_fd_write` / `_exit` /
`___clock_gettime`. Since the official glue has no import object for `env` or
`wasi_snapshot_preview1`, instantiation dies on the first such import -- before any engine
code runs, which is precisely what the browser showed.

## The two treatments

  * **Aliased.** The official module reaches the same facility through one of the fields
    it imports from `a`, so the rebuild can use that field instead. The correspondence is
    not guessed: it is read from the glue's own `asmLibraryArg`, where the minified field
    keys (`Ea`) are mapped to real Emscripten names (`_fd_write`). Reading it from the
    glue is what makes this correct for both flavours, whose minified keys differ
    (`_fd_write` is `Ea` in asyncify and `Ca` in plain).

  * **Defined locally.** An import with no counterpart in the glue is runtime bookkeeping
    the official module links in rather than imports. It becomes a no-op definition whose
    signature is read from the module's own type section, so the definition can never
    disagree with what the linker asked for.

Everything is done by editing the import section in place, so no wabt and no full
re-encode is involved, and no signature is ever altered -- only a module name and a field
name.

## Usage

    wasm-satisfy-imports.py <in.wasm> <out.wasm> --glue <official artemis.js>
                           [--emit-stubs FILE] [--map-out FILE] [--report FILE]

`--emit-stubs` writes C definitions for the imports the glue cannot supply, so they can be
linked in as local definitions on the next build. `--map-out` records the resolved
correspondence for inspection. `--report` writes a short summary of what was decided.
"""

import argparse
import json
import os
import re
import shutil
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

from importlib import import_module

_rn = import_module("wasm-rename-import")
read_uleb = _rn.read_uleb
write_uleb = _rn.write_uleb
read_name = _rn.read_name
skip_descriptor = _rn.skip_descriptor


def scan_imports(data):
    """Yield (module, name, kind, type_index) for every function import."""
    offset = 8
    while offset < len(data):
        section_id = data[offset]
        offset += 1
        size, offset = read_uleb(data, offset)
        start, end = offset, offset + size
        if section_id == 2:
            count, cursor = read_uleb(data, start)
            for _ in range(count):
                module, cursor = read_name(data, cursor)
                name, cursor = read_name(data, cursor)
                kind = data[cursor]
                type_index = None
                if kind == 0:
                    type_index, _ = read_uleb(data, cursor + 1)
                cursor = skip_descriptor(data, cursor, kind)
                yield (module.decode("utf-8", "replace"),
                       name.decode("utf-8", "replace"), kind, type_index)
            return
        offset = end


# The C width that matches each wasm value type, so a generated definition can be
# compiled by the same toolchain that produced the module.
VALUE_TYPES = {0x7F: "uint32_t", 0x7E: "uint64_t", 0x7D: "float", 0x7C: "double"}


def type_signatures(data):
    """Return {type_index: (param_types, result_types)} from the type section."""
    offset = 8
    while offset < len(data):
        section_id = data[offset]
        offset += 1
        size, offset = read_uleb(data, offset)
        start, end = offset, offset + size
        if section_id == 1:
            count, cursor = read_uleb(data, start)
            out = {}
            for index in range(count):
                if data[cursor] != 0x60:
                    raise ValueError("type section entry is not a function type")
                cursor += 1
                n_params, cursor = read_uleb(data, cursor)
                params = []
                for _ in range(n_params):
                    params.append(VALUE_TYPES.get(data[cursor], "uint32_t"))
                    cursor += 1
                n_results, cursor = read_uleb(data, cursor)
                results = []
                for _ in range(n_results):
                    results.append(VALUE_TYPES.get(data[cursor], "uint32_t"))
                    cursor += 1
                out[index] = (params, results)
            return out
        offset = end
    return {}


def glue_fields(glue_path):
    """Return {real C name -> the glue's field name} for module "a"."""
    text = open(glue_path, encoding="utf-8", errors="replace").read()
    match = re.search(r"var asmLibraryArg=\{([\s\S]*?)\};var asm=", text)
    if not match:
        raise SystemExit(f"error: {glue_path} has no asmLibraryArg; not an Emscripten glue?")
    out = {}
    for field, real in re.findall(r'"([^"]+)":([A-Za-z0-9_$]+)', match.group(1)):
        out.setdefault(real, field)
    return out


# Names whose Emscripten counterpart cannot be found by prefix matching, because the
# toolchain renamed them on the way to the glue (`clock_time_get` is `___clock_gettime`,
# `proc_exit` is `_exit`).  These are real correspondences, read off the official
# module's own import list -- the official module reaches the same facilities through
# exactly these names -- so they are aliased rather than stubbed.
KNOWN_ALIASES = {
    "clock_time_get": "___clock_gettime",
    "proc_exit": "_exit",
    # The official build reaches stat through the syscall layer, not the WASI fd layer.
    "fd_fdstat_get": "___sys_stat64",
    # Exact counterpart: the official module imports `_emscripten_longjmp`.
    "_emscripten_throw_longjmp": "_emscripten_longjmp",
    # No unwinding exists in a build without C++ exceptions, so reaching this at all
    # means something has gone unrecoverably wrong; the official module's own answer for
    # that is `_abort`, and it imports exactly that.
    "_Unwind_RaiseException": "_abort",
}


def decide(data, glue_path):
    """
    Split the non-"a" imports into what can be aliased and what must be defined locally.

    A name is aliasable when the glue supplies an import with that name -- with or without
    a leading underscore, because the module's own spelling and the glue's symbol both
    come from the same C name and differ only in whether the toolchain kept the prefix.
    """
    fields = glue_fields(glue_path)
    mapping = {}
    local = []
    for module, name, kind, type_index in scan_imports(data):
        if module == "a" or kind != 0:
            continue
        real = KNOWN_ALIASES.get(name)
        if real is None:
            real = name if name in fields else ("_" + name if "_" + name in fields else name)
        field = fields.get(real)
        if field is None and real.startswith("_"):
            field = fields.get(real[1:])
        if field:
            mapping.setdefault(module, {})[name] = {"module": "a", "name": field}
        else:
            local.append((module, name, type_index))
    return mapping, local


def c_ident(name):
    """
    The C identifier that resolves a given wasm *import* name.

    It has to be the import name verbatim -- not a prefixed variant.  A local definition
    only satisfies an unresolved reference if its linker symbol matches, and the symbol
    the object files reference is exactly the name the module ended up importing.  An
    earlier version prefixed these with `artc_local_`, which produced a file that
    compiled cleanly, linked cleanly, and changed nothing: the import was still there,
    because defining `artc_local_fd_fdstat_get` says nothing about `fd_fdstat_get`.
    """
    return name


def write_stub_c(path, local, types):
    """Write a definition for each import the glue cannot supply."""
    lines = [
        "/* Generated by wasm-satisfy-imports.py -- do not edit.",
        " *",
        " * Local definitions for runtime imports the official module links in rather than",
        " * importing.  Signatures are read from the module's own type section, so they",
        " * always match what the linker asked for.",
        " */",
        "#include <stdint.h>",
        "",
    ]
    made = set()
    for module, name, type_index in local:
        ident = c_ident(name)
        if ident in made or type_index is None:
            continue
        made.add(ident)
        params, results = types.get(type_index, ([], []))
        args = ", ".join(f"{t} p{i}" for i, t in enumerate(params)) or "void"
        ret = results[0] if results else "void"
        lines.append(f"/* was the runtime import {module}.{name} */")
        lines.append(f"{ret} {ident}({args}) {{")
        for i in range(len(params)):
            lines.append(f"  (void)p{i};")
        if results:
            lines.append("  return 0;")
        else:
            lines.append("  (void)0;")
        lines.append("}")
        lines.append("")
    open(path, "w", encoding="utf-8", newline="\n").write("\n".join(lines))
    return sorted(made)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("input")
    ap.add_argument("output")
    ap.add_argument("--glue", required=True, help="the official artemis.js (the host ABI)")
    ap.add_argument("--emit-stubs", default=None, help="write C for imports the glue lacks")
    ap.add_argument("--map-out", default=None, help="write the resolved correspondence")
    ap.add_argument("--report", default=None, help="write a short account of the decisions")
    args = ap.parse_args()

    data = open(args.input, "rb").read()
    mapping, local = decide(data, args.glue)

    stubs = []
    if args.emit_stubs:
        stubs = write_stub_c(args.emit_stubs, local, type_signatures(data))
    if args.map_out:
        with open(args.map_out, "w", encoding="utf-8", newline="\n") as handle:
            handle.write(json.dumps(mapping, indent=2) + "\n")

    aliased = 0
    if mapping:
        out, aliased = _rn.rename_imports(data, mapping)
        with open(args.output, "wb") as handle:
            handle.write(out)
    else:
        shutil.copy2(args.input, args.output)

    lines = [
        f"aliased into module \"a\": {aliased}",
        f"left for local definition : {len(local)}",
    ]
    for module, name, _ in local:
        lines.append(f"    {module}.{name} -> {c_ident(name)}")
    text = "\n".join(lines)
    print(text)
    if args.report:
        open(args.report, "w", encoding="utf-8", newline="\n").write(text + "\n")
    return 0


if __name__ == "__main__":
    sys.exit(main())
