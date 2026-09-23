#!/usr/bin/env python3
"""
Rename import entries in a linked WebAssembly module, in place.

## Why this exists

The official Artemis module and the module rebuilt from its reverse-engineered
sources both need a host, but they ask for one in different words. The official
module imports **every** host function from a single module named `a`, because
Emscripten's `asmLibraryArg` object is what the glue builds and the engine was
minified onto short field names. A rebuild linked by a current Emscripten puts its
own runtime plumbing (`env`, `wasi_snapshot_preview1`) in separate modules, which
the official glue has no import object for -- so `WebAssembly.instantiate` fails on
import #196 before a single engine function runs.

Renaming an import is a local edit to the import section, so it can be done on the
binary directly, the same way `wasm-rename-export.py` fixes an export name. That
keeps the build chain free of wabt: there is no need to round-trip the whole module
through `.wat` to change two strings.

## What it does and does not touch

Only the import section (id 2) is rewritten, and only entries whose (`module`,
`name`) pair matches a mapping. The kind byte and the whole descriptor after it --
type index, table type, memory limits, global type -- are copied through verbatim for
every entry, so a rename cannot change what is imported, only where it is looked up.

Every import that is *not* renamed is still re-emitted byte-for-byte, so a mapping
that matches nothing is detectable: the script fails rather than writing a file whose
only difference is a fresh section header.

## Usage

    wasm-rename-import.py <in.wasm> <out.wasm> <map.json>

The map is JSON, either nested or flat:

    {"env": {"_abort_js": "c"}, "wasi_snapshot_preview1": {"fd_write": {"module": "a", "name": "Ea"}}}

A value that is a string renames only the field, keeping the module. A value that is
an object renames both. A `"*"` key applies to every unmatched import in that module
and is only accepted when every one of them is meant to change module without caring
which field it becomes -- use it sparingly, and prefer explicit pairs.
"""

import argparse
import json
import sys


def read_uleb(data, offset):
    """Decode one unsigned LEB128 value. Returns (value, next_offset)."""
    result = 0
    shift = 0
    while True:
        byte = data[offset]
        offset += 1
        result |= (byte & 0x7F) << shift
        if not byte & 0x80:
            return result, offset
        shift += 7


def write_uleb(value):
    """Encode one unsigned LEB128 value."""
    out = bytearray()
    while True:
        byte = value & 0x7F
        value >>= 7
        if value:
            out.append(byte | 0x80)
        else:
            out.append(byte)
            return bytes(out)


def read_name(data, offset):
    """Read one length-prefixed name. Returns (bytes, next_offset)."""
    length, offset = read_uleb(data, offset)
    return data[offset:offset + length], offset + length


def skip_descriptor(data, offset, kind):
    """
    Return the offset just past one import descriptor.

    `offset` points at the kind byte, which is consumed here.  The descriptor shapes
    are fixed by the spec, so this walks them rather than looking for a pattern:
      0 function -> type index (LEB128)
      1 table    -> reference type, then limits
      2 memory   -> limits
      3 global   -> value type, then mutability
    `limits` is a flag byte followed by one or two LEB128 sizes.

    Getting this wrong is not a subtle off-by-one: consuming the kind byte as if it
    were the descriptor shifts every later entry, and the next entry's "module length"
    is read from arbitrary payload, so the walk degenerates into garbage (the symptom
    is an "unknown import kind" in the double digits).
    """
    offset += 1                          # the kind byte itself

    if kind == 0:
        _, offset = read_uleb(data, offset)
        return offset
    if kind == 1:
        offset += 1                      # reference type
    elif kind == 3:
        offset += 1                      # value type
        offset += 1                      # mutability
        return offset
    elif kind != 2:
        raise ValueError(f"unknown import kind {kind}")

    flags = data[offset]
    offset += 1
    _, offset = read_uleb(data, offset)
    if flags & 1:
        _, offset = read_uleb(data, offset)
    return offset


def rename_imports(data, mapping):
    """Return (new_module, renamed_count) with every mapped import renamed."""
    if data[:4] != b"\x00asm":
        raise ValueError("not a WebAssembly module (bad magic)")

    offset = 8
    while offset < len(data):
        section_id = data[offset]
        offset += 1
        size, offset = read_uleb(data, offset)
        start = offset
        end = start + size
        if end > len(data):
            raise ValueError(f"section {section_id} claims {size} bytes but the file ends early")

        if section_id != 2:
            offset = end
            continue

        count, cursor = read_uleb(data, start)
        rebuilt = bytearray(write_uleb(count))
        hits = 0
        for _ in range(count):
            module, cursor = read_name(data, cursor)
            name, cursor = read_name(data, cursor)
            kind = data[cursor]
            desc_end = skip_descriptor(data, cursor, kind)

            new_module, new_name = module, name
            spec = mapping.get(module.decode("utf-8", "replace"))
            if spec is not None:
                value = spec.get(name.decode("utf-8", "replace"))
                if value is None:
                    value = spec.get("*")
                if value is not None:
                    if isinstance(value, str):
                        new_name = value.encode("utf-8")
                    else:
                        new_module = value.get("module", module).encode("utf-8")
                        new_name = value.get("name", name).encode("utf-8")
            if new_module != module or new_name != name:
                hits += 1

            rebuilt += write_uleb(len(new_module)) + new_module
            rebuilt += write_uleb(len(new_name)) + new_name
            rebuilt += data[cursor:desc_end]
            cursor = desc_end

        if cursor != end:
            raise ValueError(
                f"import section parsed to {cursor} but the section ends at {end}; "
                "refusing to write a module that would be torn"
            )
        if hits == 0:
            raise SystemExit(
                "error: no import matched the mapping; refusing to write an unchanged module"
            )

        # Rebuild the section header, because the payload length changes whenever a
        # name changes length.
        head = data[:start - len(write_uleb(size))]
        tail = data[end:]
        return head + write_uleb(len(rebuilt)) + bytes(rebuilt) + tail, hits

    raise SystemExit("error: module has no import section")


def set_memory_max(data, minimum):
    """
    Raise every *imported* memory's declared maximum to at least `minimum` pages.

    Import matching is exact in both directions: the glue supplies a
    `WebAssembly.Memory` with maximum 65536 pages, and a module that declares a smaller
    maximum cannot be linked against it --

        Import #0 "a" "a": memory import has a larger maximum size 65536
        than the module's declared maximum 256

    wasm-ld refuses to set a large maximum while growth is off (`--max-memory is
    incompatible with --no-growable-memory`), so the declaration has to be corrected
    after the fact. Only the maximum is touched: the initial size is what the engine
    sizes its heap against, and the flags byte is left alone so a shared or 64-bit
    memory stays whatever it was.

    Returns (new_module, count_changed).
    """
    if data[:4] != b"\x00asm":
        raise ValueError("not a WebAssembly module (bad magic)")

    offset = 8
    while offset < len(data):
        section_id = data[offset]
        offset += 1
        size, offset = read_uleb(data, offset)
        start = offset
        end = start + size
        if end > len(data):
            raise ValueError(f"section {section_id} claims {size} bytes but the file ends early")

        if section_id != 2:
            offset = end
            continue

        count, cursor = read_uleb(data, start)
        rebuilt = bytearray(write_uleb(count))
        changed = 0
        for _ in range(count):
            module, cursor = read_name(data, cursor)
            name, cursor = read_name(data, cursor)
            kind = data[cursor]

            rebuilt += write_uleb(len(module)) + module
            rebuilt += write_uleb(len(name)) + name
            rebuilt.append(kind)

            if kind != 2:
                rebuilt += data[cursor + 1:skip_descriptor(data, cursor, kind)]
                cursor = skip_descriptor(data, cursor, kind)
                continue

            # memory: flags, initial, optional maximum.
            cursor += 1
            flags = data[cursor]
            cursor += 1
            initial, cursor = read_uleb(data, cursor)
            if flags & 1:
                current, cursor = read_uleb(data, cursor)
                if current < minimum:
                    current = minimum
                    changed += 1
            rebuilt.append(flags)
            rebuilt += write_uleb(initial)
            rebuilt += write_uleb(current)

        if cursor != end:
            raise ValueError(
                f"import section parsed to {cursor} but the section ends at {end}; "
                "refusing to write a module that would be torn"
            )
        if changed == 0:
            raise SystemExit("error: no imported memory needed its maximum raised")

        head = data[:start - len(write_uleb(size))]
        tail = data[end:]
        return head + write_uleb(len(rebuilt)) + bytes(rebuilt) + tail, changed

    raise SystemExit("error: module has no import section")


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("input")
    ap.add_argument("output")
    ap.add_argument("map", nargs="?", help="JSON mapping file, see the docstring for the shape")
    ap.add_argument("--memory-max", type=int, default=None,
                    help="raise every imported memory's maximum to at least this many pages")
    args = ap.parse_args()

    data = open(args.input, "rb").read()

    if args.memory_max is not None:
        out, changed = set_memory_max(data, args.memory_max)
        with open(args.output, "wb") as handle:
            handle.write(out)
        print(f"{args.output}: raised {changed} imported memory maximum to "
              f"{args.memory_max} pages ({len(data):,} -> {len(out):,} bytes)")
        return 0

    if not args.map:
        raise SystemExit("error: give a mapping file, or --memory-max")
    mapping = json.load(open(args.map, encoding="utf-8"))
    out, hits = rename_imports(data, mapping)
    with open(args.output, "wb") as handle:
        handle.write(out)
    print(f"{args.output}: renamed {hits} import(s) "
          f"({len(data):,} -> {len(out):,} bytes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
