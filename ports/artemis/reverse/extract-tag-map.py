#!/usr/bin/env python3
"""Extract the tag-name oracle for the Artemis wasm: which functions implement
which scenario tag.

Why this exists
---------------
The official wasm ships with **no name section** -- function names are gone, and
the only exports are toolchain symbols (`_malloc`, `_free`, `dynCall_*`, the
asyncify pair). So there is no symbol table to read.

But the SDK also ships `Docs/tag/` -- **186 official tag documents**, one per
scenario tag the engine understands -- and the engine keeps those tag names as
**NUL-terminated string literals in its data segments**. A tag name found at
linear address A is a name for whatever code compares against A. That is the
naming oracle: 153 of the 186 tags resolve, binding to 44 functions.

Output (`--json`): tag -> addresses, and function -> tags, plus function sizes.

Usage:
    python ports/artemis/reverse/extract-tag-map.py \
        --wasm2c  .ref/artemis-official/wasm/asyncify/artemis.wasm2c.c \
        --tagdir  .ref/artemis/Docs/tag \
        -o        .ref/artemis-official/wasm/asyncify/tag-map.json

Notes
-----
* Pass NATIVE paths on Windows. Python here does not resolve MSYS `/d/...`
  paths, and a wrong path fails silently as "0 tags found" rather than as an
  error, so this script asserts a non-zero tag count before writing.
* Reads the wasm2c C rather than the wasm directly because that form has both
  the data-segment bytes and the function bodies in one file, already parsed.
* Data segments only reach ~821 KB of linear memory; the much hotter
  `mem[852316]` (47 601 references) is heap/BSS, not a string pool.
"""

from __future__ import annotations

import argparse
import json
import os
import re
import sys

# wasm2c emits each data segment as a byte array, then a LOAD_DATA that places it.
SEG_DEF = re.compile(r"^static const u8 (data_segment_data_\w+)\[\] = \{")
SEG_LOAD = re.compile(
    r"LOAD_DATA\(\(\*instance->w2c_a_a\), (\d+)u, (data_segment_data_\w+), (\d+)\)"
)
# Forward declarations carry `static`; definitions do not. Matching the `static`
# form silently binds every address to the wrong (empty) span -- so require the
# `instance` parameter, which only definitions have.
FUNC_DEF = re.compile(r"^[a-z0-9_]+ (w2c_artemis_f\d+)\(w2c_artemis\* instance")
ADDR_CONST = re.compile(r"\b(\d{5,6})u\b")


def read_data_segments(lines: list[str]) -> dict[int, bytes]:
    """segment name -> raw bytes, from the array literals."""
    arrays: dict[str, bytes] = {}
    name: str | None = None
    buf: list[str] = []
    for line in lines:
        m = SEG_DEF.match(line)
        if m:
            name, buf = m.group(1), []
            continue
        if name is None:
            continue
        if line.startswith("};"):
            arrays[name] = bytes(
                int(x, 16) for x in re.findall(r"0x([0-9a-fA-F]{2})", " ".join(buf))
            )
            name = None
        else:
            buf.append(line)
    return arrays


def placed_segments(lines: list[str], arrays: dict[str, bytes]) -> dict[int, bytes]:
    """linear address -> bytes actually copied there."""
    out: dict[int, bytes] = {}
    for line in lines:
        m = SEG_LOAD.search(line)
        if m:
            addr, seg, length = int(m.group(1)), m.group(2), int(m.group(3))
            out[addr] = arrays.get(seg, b"")[:length]
    return out


def function_spans(lines: list[str]) -> dict[str, tuple[int, int]]:
    starts = [(m.group(1), i) for i, l in enumerate(lines) if (m := FUNC_DEF.match(l))]
    return {
        name: (start, starts[i + 1][1] if i + 1 < len(starts) else len(lines))
        for i, (name, start) in enumerate(starts)
    }


def load_tags(tagdir: str) -> list[str]:
    tags = set()
    for dirpath, _, filenames in os.walk(tagdir):
        for fn in filenames:
            if fn.endswith(".txt") and fn != "readme.txt":
                tags.add(fn[:-4])
    return sorted(tags)


def main(argv: list[str]) -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--wasm2c", required=True, help="generated artemis.wasm2c.c")
    ap.add_argument("--tagdir", required=True, help="SDK Docs/tag directory")
    ap.add_argument("-o", "--json", help="write the map here (default: stdout summary only)")
    args = ap.parse_args(argv[1:])

    for label, path in (("--wasm2c", args.wasm2c), ("--tagdir", args.tagdir)):
        if not os.path.exists(path):
            print(f"{label}: no such path: {path}", file=sys.stderr)
            print("(on Windows pass native paths like D:/... -- MSYS /d/... does not resolve)",
                  file=sys.stderr)
            return 2

    tags = load_tags(args.tagdir)
    if not tags:
        print(f"no tag documents under {args.tagdir}", file=sys.stderr)
        return 2

    lines = open(args.wasm2c, encoding="utf-8", errors="replace").read().split("\n")
    arrays = read_data_segments(lines)
    placed = placed_segments(lines, arrays)
    spans = function_spans(lines)
    print(
        f"data segments: {len(arrays)} parsed, {len(placed)} placed; "
        f"functions: {len(spans)}",
        file=sys.stderr,
    )

    # Collect ALL placements, not just the first. A tag string is often copied
    # (e.g. `lyprop` sits at both 718417 and 771729), and every copy is a real
    # comparison site, so keeping them all widens the oracle. The cost is
    # ambiguity: one tag can then name several functions, which the report
    # surfaces rather than hides.
    tag_addr: dict[str, list[int]] = {}
    for tag in tags:
        needle = b"\x00" + tag.encode() + b"\x00"
        for base, blob in placed.items():
            start = 0
            while (i := blob.find(needle, start)) >= 0:
                tag_addr.setdefault(tag, []).append(base + i + 1)
                start = i + 1

    # address -> owning function, then invert to function -> tags
    addr_owner: dict[int, set[str]] = {}
    for name, (start, end) in spans.items():
        for a in set(ADDR_CONST.findall("\n".join(lines[start:end]))):
            addr_owner.setdefault(int(a), set()).add(name)

    tag_owner: dict[str, list[str]] = {}
    for tag, addrs in tag_addr.items():
        for a in addrs:
            for fn in addr_owner.get(a, ()):
                tag_owner.setdefault(fn, []).append(tag)
    tag_owner = {k: sorted(set(v)) for k, v in tag_owner.items()}

    sizes = {n: e - s for n, (s, e) in spans.items()}
    unlocated = [t for t in tags if t not in tag_addr]
    print(
        f"tags: {len(tags)} total, {len(tag_addr)} located, {len(unlocated)} unlocated",
        file=sys.stderr,
    )
    print(f"functions owning >=1 tag: {len(tag_owner)}", file=sys.stderr)
    for fn, ts in sorted(tag_owner.items(), key=lambda x: -len(x[1]))[:10]:
        print(f"  {fn}: {sizes[fn]:>7} lines, {len(ts):>3} tags: {', '.join(ts[:6])}",
              file=sys.stderr)

    if args.json:
        # sort_keys so the artifact is byte-reproducible across runs: the sets
        # above iterate in hash-seed order, which varies per process and would
        # otherwise make two runs of the same input differ in key order only.
        payload = {
            "tag_addr": tag_addr,
            "tag_owner": tag_owner,
            "sizes": sizes,
            "unlocated": unlocated,
        }
        with open(args.json, "w", encoding="utf-8") as fh:
            json.dump(payload, fh, indent=1, sort_keys=True)
            fh.write("\n")
        print(f"wrote {args.json}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
