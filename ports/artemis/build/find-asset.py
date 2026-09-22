#!/usr/bin/env python3
"""Finds entries in a pf8 archive by substring, printing name/offset/size.

Why a separate script: `parse-pf8-index.py` validates the header and the first N entries, which is what
it was written for, but it cannot answer "does this archive contain a `z`-prefixed sibling of X" — and
that is the question. Format per the verified spec: header `0 "pf" 2 version 3 index_size u32LE 7 count
i32LE`, entries from offset 11, records `[u32 name_len][name][u32 pad][u32 offset][u32 size]`, and the
key is `SHA1(file[7, 7+index_size))` cycled every 20 bytes with **phase 0 at the entry start**.

Usage: find-asset.py <pack> <substring> [max]
"""
import hashlib
import struct
import sys

path = sys.argv[1]
needle = sys.argv[2].lower()
limit = int(sys.argv[3]) if len(sys.argv) > 3 else 40
# `--dump` prints the first match's bytes, decrypted. Payloads (unlike the index table) are keyed with
# SHA1(index) cycled every 20 bytes, and the phase is taken from the entry's own file offset so a
# sub-range read starts where the full read would have.
dump = '--dump' in sys.argv

with open(path, 'rb') as handle:
    header = handle.read(7)
    if header[:2] != b'pf':
        raise SystemExit(f'not a pf8 archive: {path}')
    version = header[2:3]
    index_size = struct.unpack_from('<I', header, 3)[0]
    # The index region is file[7, 7+index_size) — the hashed region and the key's source are the same
    # bytes. Reading it from 11 (where the entries begin) loses the first 4 bytes, which corrupts both
    # the SHA-1 and every record's phase.
    raw = handle.read(index_size)
    if len(raw) != index_size:
        raise SystemExit(f'short index read: {len(raw)} != {index_size}')
    # The 4-byte entry count sits at index[0:4] and is not encrypted; entries begin at index[4].
    count = struct.unpack_from('<i', header, 7)[0] if False else struct.unpack_from('<i', raw, 0)[0]

key = hashlib.sha1(raw).digest()  # decrypts the entry <i>payloads</i>; the index table itself is plain
data = bytearray(raw)

pos = 4
matches = []
names = 0
for _ in range(count):
    if pos + 12 > len(data):
        break
    name_len = struct.unpack_from('<I', data, pos)[0]
    if name_len == 0 or name_len > 4096 or pos + name_len + 16 > len(data):
        break
    name = data[pos + 4:pos + 4 + name_len].decode('cp932', 'replace')
    offset = struct.unpack_from('<I', data, pos + 4 + name_len + 4)[0]
    size = struct.unpack_from('<I', data, pos + 4 + name_len + 8)[0]
    pos += name_len + 16
    names += 1
    if needle in name.lower():
        matches.append((name, offset, size))

print(f'{path}  version={version.decode()} entries={count} parsed={names}')
for name, offset, size in matches[:limit]:
    print(f'  {offset:>12}  {size:>10}  {name}')
print(f'  matches: {len(matches)}')

if dump and matches:
    name, offset, size = matches[0]
    with open(path, 'rb') as handle:
        handle.seek(offset)
        payload = bytearray(handle.read(int(size)))
    for i in range(len(payload)):
        payload[i] ^= key[i % 20]
    print(f'--- payload of {name} ({size} bytes) ---')
    print(payload[:4000].decode('cp932', 'replace'))
