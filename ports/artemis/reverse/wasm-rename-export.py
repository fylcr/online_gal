#!/usr/bin/env python3
"""
Rename an export in a linked WebAssembly module, in place.

## Why this exists

The official Artemis module does not export its function table under the name the
Emscripten toolchain picks (`__indirect_function_table`); it exports it as `Lc`,
and the official JS glue is *hard-wired* to that name:

    wasmTable=Module["asm"]["Lc"]

So a module rebuilt from the reverse-engineered sources is not usable as a drop-in
replacement until that one name matches, even though the table itself is the same
thing. Aiming the whole build at a text round-trip would fix it -- `wasm2wat` and
back -- but that puts wabt back into the build chain, which is exactly what route 3
(`C++ -> em++ -> wasm`, see build-cxx-route.sh) exists to avoid.

Renaming one export is a local edit to a single section, so it can be done directly
on the binary: parse the section list, rewrite the one name, fix up the lengths
(the name length and the section payload size are both LEB128), and leave every
other byte alone. No converter, no re-encoding, no dependency.

## What it does and does not touch

Only the *export section* (id 7) is rewritten, and only the single entry whose
current name matches. The entry's kind and index are preserved, so the rename cannot
change what is exported -- only what it is called. Everything before the export
section is copied verbatim; everything after it is copied verbatim.

## Usage

    wasm-rename-export.py <in.wasm> <out.wasm> <old-name> <new-name>
    wasm-rename-export.py module.wasm module.fixed.wasm __indirect_function_table Lc

Exit status is 0 on success. If the old name is absent the script fails rather than
writing an unchanged file, because a silent no-op here would look like a successfully
patched module and only show up much later as an undefined `Module["asm"]["X"]`.
"""

import argparse
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


def sections(data):
    """
    Yield (id, payload_start, payload_end) for each section after the 8-byte header.

    Section sizes are not trusted for slicing blindly: this walks the real sizes so
    that a malformed tail is reported instead of silently truncated.
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
        yield section_id, start, end
        offset = end


def rename_export(data, old_name, new_name):
    """Return a new module with the export `old_name` renamed to `new_name`."""
    if len(new_name.encode("utf-8")) == 0:
        raise ValueError("new name must not be empty")
    old_bytes = old_name.encode("utf-8")
    new_bytes = new_name.encode("utf-8")

    for section_id, start, end in sections(data):
        if section_id != 7:
            continue

        count, cursor = read_uleb(data, start)
        rebuilt = bytearray(write_uleb(count))
        hit = 0
        for _ in range(count):
            name_len, cursor = read_uleb(data, cursor)
            name = data[cursor:cursor + name_len]
            cursor += name_len
            # kind is a single byte (0 func, 1 table, 2 memory, 3 global); index is LEB.
            kind = data[cursor]
            cursor += 1
            _index, cursor = read_uleb(data, cursor)

            if name == old_bytes:
                hit += 1
                name = new_bytes
            rebuilt += write_uleb(len(name))
            rebuilt += name
            rebuilt.append(kind)
            rebuilt += write_uleb(_index)

        if cursor != end:
            raise ValueError(
                f"export section parsed to {cursor} but the section ends at {end}; "
                "refusing to write a module that would be torn"
            )
        if hit == 0:
            raise SystemExit(f"error: no export named {old_name!r} in {args.input}")
        if hit > 1:
            raise SystemExit(f"error: {hit} exports named {old_name!r}; refusing an ambiguous rename")

        head = data[:start - len(write_uleb(end - start))]
        tail = data[end:]
        return head + write_uleb(len(rebuilt)) + bytes(rebuilt) + tail

    raise SystemExit(f"error: module has no export section, so {old_name!r} cannot exist")


def main():
    global args
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("input")
    ap.add_argument("output")
    ap.add_argument("old_name")
    ap.add_argument("new_name")
    args = ap.parse_args()

    data = open(args.input, "rb").read()
    out = rename_export(data, args.old_name, args.new_name)
    with open(args.output, "wb") as handle:
        handle.write(out)
    print(f"{args.output}: renamed export {args.old_name!r} -> {args.new_name!r} "
          f"({len(data):,} -> {len(out):,} bytes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
