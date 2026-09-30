#!/usr/bin/env python3
"""Repack an extracted Artemis game tree into multi-part `pf6` packages.

Why this exists
---------------
The plain/sync engine reads a game through an in-memory file system: the deployed
`artemis.data` is a single package that holds the whole game, and the glue materialises it
with `FS_createDataFile` before `main()` runs. That works for the SDK's 15 MB sample and
cannot work for a real title -- `root.pfs` alone is 2.7 GB and `root.pfs.000` another 3.4 GB.

Split the tree into parts of at most `--cap` bytes and each part can be registered as a
**lazy** file instead (`FS_createLazyFile`, one per part), so the engine's reads become
HTTP range requests on demand. Measured on the real game (29,224 files / 6.05 GB): booting
to the first rendered frame reads 39 MB, 0.65% of the tree.

Two details are not optional -- see `write_pf8`'s "Name encoding and order" in
`official-web-deploy.py`:

* the index must be in the packer's own order (path separator below every other byte), or
  the engine reads the index and stops without a message;
* each part gets its **own** offset space starting at zero (`root.pfs`, `root.pfs.000`,
  `root.pfs.001`, ...), exactly like the parts the publisher ships.

Payloads are streamed, so memory stays proportional to the index (~2 MB for 29k entries),
not to the tree. Version 6 is written: unkeyed, which is what the engine reads fine and
what keeps the parts verifiable by `pfs.ts`.

Usage
-----
    python ports/artemis/build/repack-parts.py <game-dir> <out-dir> [--prefix root.pfs]
                                                [--cap 1600000000]
"""
from __future__ import annotations

import argparse
import os
import struct
import sys
import time

# A path separator compares below every other byte in the official order.
_SEPARATOR_BYTES = (0x2F, 0x5C)


def official_key(name_bytes: bytes) -> bytes:
    """The packer's sort key: `\\`/`/` as if it were 0x01."""
    return bytes(1 if b in _SEPARATOR_BYTES else b for b in name_bytes)


def index_length(names: list[bytes]) -> int:
    """Byte length of the index (records + directory trailer) without computing offsets."""
    return 4 + sum(16 + len(name) for name in names) + (8 * len(names) + 16)


def build_index(names: list[bytes], offsets: list[int], sizes: list[int]) -> bytes:
    """The index blob: records, then the `count + 1` directory trailer the engine reads."""
    blob = bytearray(struct.pack("<i", len(names)))
    name_ends: list[int] = []
    cursor = 4
    for name, offset, size in zip(names, offsets, sizes):
        blob += struct.pack("<I", len(name)) + name
        cursor += 4 + len(name)
        name_ends.append(cursor)
        blob += struct.pack("<I", 0) + struct.pack("<I", offset) + struct.pack("<I", size)
        cursor += 12
    trailer = bytearray(struct.pack("<I", len(names) + 1))
    for name_end in name_ends:
        trailer += struct.pack("<II", name_end, 0)
    trailer += struct.pack("<II", 0, 0) + struct.pack("<I", cursor)
    return bytes(blob + trailer)


def collect(game: str) -> list[tuple[bytes, str, int]]:
    entries: list[tuple[bytes, str, int]] = []
    for root, _dirs, files in os.walk(game):
        for name in files:
            path = os.path.join(root, name)
            rel = os.path.relpath(path, game).replace(os.sep, "\\")
            entries.append((rel.encode("cp932"), path, os.path.getsize(path)))
    entries.sort(key=lambda item: official_key(item[0]))
    return entries


def split(entries: list[tuple[bytes, str, int]], cap: int):
    """Greedily fill parts, never exceeding `cap` for index + payload."""
    parts: list[list[tuple[bytes, str, int]]] = []
    current: list[tuple[bytes, str, int]] = []
    payload = 0
    index = 4 + 16
    for entry in entries:
        add = 16 + len(entry[0])
        if current and index + add + payload + entry[2] > cap:
            parts.append(current)
            current, payload, index = [], 0, 4 + 16
        current.append(entry)
        payload += entry[2]
        index += add
    if current:
        parts.append(current)
    return parts


def write_part(path: str, group: list[tuple[bytes, str, int]], index_len: int) -> int:
    names = [entry[0] for entry in group]
    sizes = [entry[2] for entry in group]
    offsets: list[int] = []
    cursor = 7 + index_len
    for size in sizes:
        offsets.append(cursor)
        cursor += size
    index = build_index(names, offsets, sizes)
    if len(index) != index_len:
        raise SystemExit(f"index length mismatch: {len(index)} != {index_len}")
    with open(path, "wb") as out:
        out.write(b"pf6" + struct.pack("<I", len(index)))
        out.write(index)
        for _name, src, size in group:
            with open(src, "rb") as handle:
                left = size
                while left > 0:
                    chunk = handle.read(min(1 << 22, left))
                    if not chunk:
                        raise SystemExit(f"short read: {src}")
                    out.write(chunk)
                    left -= len(chunk)
    return os.path.getsize(path)


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("game", help="extracted game tree (loose files)")
    parser.add_argument("out", help="directory to write the parts into")
    parser.add_argument("--prefix", default="root.pfs", help="first part's name (default root.pfs)")
    parser.add_argument(
        "--cap",
        type=int,
        default=1_600_000_000,
        help="maximum bytes per part, index included (default 1.6e9, below 2^31)",
    )
    args = parser.parse_args(argv)

    if not os.path.isdir(args.game):
        raise SystemExit(f"not a directory: {args.game}")
    os.makedirs(args.out, exist_ok=True)

    entries = collect(args.game)
    total = sum(entry[2] for entry in entries)
    print(f"collected {len(entries)} files / {total:,} B payload", flush=True)
    parts = split(entries, args.cap)

    written = 0
    for number, group in enumerate(parts):
        name = args.prefix if number == 0 else f"{args.prefix}.{number - 1:03d}"
        started = time.time()
        size = write_part(os.path.join(args.out, name), group, index_length([e[0] for e in group]))
        written += size
        print(
            f"  {name}: {size:,} B / {len(group)} entries / index "
            f"{index_length([e[0] for e in group]):,} B / {time.time() - started:.1f}s",
            flush=True,
        )
    print(
        f"parts={len(parts)} total={written:,} B first_entry="
        f"{parts[0][0][0].decode('cp932')!r}",
        flush=True,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
