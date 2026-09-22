#!/usr/bin/env python3
"""
Parses an Artemis pf8 index, using **GARbro's** layout as the specification and checking it against
the real archive bytes.

Why this exists: `ArcFormats/Artemis/ArcPFS.cs` (GARbro, morkt) is the battle-tested description of
this format, and `artemis-compat` has its own reader. Where two independent implementations agree, the
format is settled; where they disagree, the bytes decide. This script does the deciding, and its output
is the reference the TypeScript reader in `packages/vfs/src/archives/pf8.ts` will be written against.

Layout, from `ArcPFS.cs:OpenPf` (versions 6 and 8) — offsets are absolute in the file:

    0  "pf"                     2 bytes ASCII
    2  version                  1 byte, ASCII digit ('6' or '8' here; '2' takes the OpenPf2 path)
    3  index_size               u32 LE
    7  count                    i32 LE
    7  index                    index_size bytes  (the entries begin at +4 within this blob)

    per entry, starting at index_offset = 4:
        name_length   i32 LE
        name          name_length bytes, encoded
        <one u32, skipped by GARbro: index_offset += name_length + 8>
        offset        u32 LE
        size          u32 LE
        index_offset += 8

Version 2 (`OpenPf2`) differs in three ways, which is why it is treated separately: `count` sits at
0xB instead of 7, entries start at index_offset = 8 instead of 4, and the stride is `name_length + 0x10`
instead of `+8` — i.e. two skipped u32s rather than one.

Key derivation (GARbro, for versions 4, 5, 8 and 9 — note 6 is *not* keyed):

    key = SHA1(index)

`SHA1(file[7 : 7+index_size])` in artemis-compat's notation is the same bytes, and a 20-byte SHA1 digest
used as a repeating XOR key is the "20-byte periodic XOR" that project describes. The two agree.
"""
import hashlib
import os
import struct
import sys

# The pack chain is real game data and is never committed. `ARTEMIS_GAME_DIR` points at it; the
# fallback stays beside the other reference checkouts, under the gitignored `.ref/`.
GAME = os.environ.get('ARTEMIS_GAME_DIR') or os.path.join(
    os.path.dirname(os.path.abspath(__file__)), '..', '..', '..', '.ref', 'artemis-game')
PATH = sys.argv[1] if len(sys.argv) > 1 else os.path.join(GAME, 'root.pfs')
SHOW = int(sys.argv[2]) if len(sys.argv) > 2 else 8


def parse_index_entries(index, count, encoding, base):
    """Parse `count` entries using GARbro's version-6/8 layout. Returns (entries, end_offset)."""
    entries = []
    off = base
    for i in range(count):
        if off + 4 > len(index):
            raise ValueError(f'index truncated reading name_length of entry {i} at {off}')
        name_length = struct.unpack_from('<i', index, off)[0]
        if name_length < 0 or off + 4 + name_length + 12 > len(index):
            raise ValueError(
                f'implausible name_length {name_length} for entry {i} at {off} '
                f'(index is {len(index)} bytes)')
        raw = index[off + 4: off + 4 + name_length]
        try:
            name = raw.decode(encoding)
        except UnicodeDecodeError as e:
            name = f'<{encoding} decode failed: {e}>'
        skipped = struct.unpack_from('<I', index, off + 4 + name_length)[0]
        file_offset = struct.unpack_from('<I', index, off + 4 + name_length + 4)[0]
        size = struct.unpack_from('<I', index, off + 4 + name_length + 8)[0]
        entries.append({
            'name': name, 'skipped': skipped, 'offset': file_offset, 'size': size,
        })
        # The stride is `name_length + 16`, and getting it wrong is easy because GARbro splits the
        # advance across two statements:
        #
        #     index_offset += name_length + 8;   // moves past the 4-byte field AND the name AND a 4-byte gap
        #     ... read offset, size ...
        #     index_offset += 8;                 // moves past offset and size
        #
        # so the record is: [u32 name_length][name][u32 gap][u32 offset][u32 size]
        #                 = 4 + name_length + 4 + 4 + 4 = name_length + 16
        #
        # Reading the two steps as "name_length + 16 plus 4 more" gives name_length + 20 and walks off
        # the record, which then reads a *name* where a length is expected. The symptom is memorable:
        # the "length" comes back as 0x746E6F66, which is the ASCII text "font".
        off += name_length + 16
    return entries, off


print(f'file: {PATH}')
with open(PATH, 'rb') as fh:
    head = fh.read(11)
    magic = head[0:2]
    version_ch = head[2:3]
    index_size = struct.unpack_from('<I', head, 3)[0]
    count = struct.unpack_from('<i', head, 7)[0]

    print(f'  magic:      {magic!r}   (GARbro requires "pf")')
    print(f'  version:    {version_ch.decode()}  -> GARbro {"OpenPf" if version_ch in (b"6", b"8") else "OpenPf2" if version_ch == b"2" else "unsupported"}')
    print(f'  index_size: {index_size:,}')
    print(f'  count:      {count:,}')

    fh.seek(7)
    index = fh.read(index_size)
    print(f'  index read: {len(index):,} bytes  {"OK" if len(index) == index_size else "SHORT"}')

    key = hashlib.sha1(index).digest()
    print(f'  key (SHA1 of index): {key.hex()}')
    print(f'  keyed?      version {version_ch.decode()} is {"keyed" if version_ch in (b"4", b"5", b"8", b"9") else "NOT keyed"} per ArcPFS.cs')

    print()
    # Encoding: GARbro defaults to the PFSEncodingCP setting, falling back between cp932 and UTF-8.
    for enc in ('cp932', 'utf-8'):
        try:
            entries, end = parse_index_entries(index, count, enc, 4)
            print(f'  parsed OK with {enc}: {len(entries)} entries, index consumed to {end:,} of {len(index):,}')
            print()
            print(f'  first {SHOW} entries:')
            for e in entries[:SHOW]:
                print(f"    off={e['offset']:>12,}  size={e['size']:>10,}  skip={e['skipped']:>10,}  {e['name']}")
            print()
            # Structural checks that do not depend on the encoding being right.
            bad = [e for e in entries if e['offset'] + e['size'] > 2690454559]
            print(f'  entries past EOF (vs file size 2,690,454,559): {len(bad)}')
            if bad:
                print(f"    e.g. {bad[0]['name']} off={bad[0]['offset']:,} size={bad[0]['size']:,}")
            total = sum(e['size'] for e in entries)
            print(f'  sum of entry sizes: {total:,}')
            print(f'  distinct `skipped` values (first 6): {sorted({e["skipped"] for e in entries})[:6]}')
            break
        except (ValueError, UnicodeDecodeError) as err:
            print(f'  {enc}: FAILED — {err}')
