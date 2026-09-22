#!/usr/bin/env python3
"""
Writes a minimal Artemis `pf8` package.

Why this exists: the web host has to be exercised in a browser, and the only real pack to hand is
2.69 GB — far past anything `--preload-file` can embed, and there is no pf8 writer in
`artemis-compat` (`src/` has only the reader). A tiny well-formed pack with the fields the host
actually reads lets the browser-side chain be checked for real: module load -> WebGL2 context ->
Emscripten filesystem -> pf8 parse -> stage size.

It is **not** a playable game. It carries `system.ini` (so `EngineContext::Open` can size the stage)
and nothing else, so `BootFramework()` is expected to fail for want of `system/init.lua`. That
distinction is the point: a run that reports a stage size and then "boot failed" has proved the host
and the container, and has not pretended to prove the framework.

Format (see docs/artemis-pfs-format.md), matching both GARbro's `ArcPFS.cs` and artemis-compat:

    0   "pf"         2 bytes
    2   version      1 byte ASCII digit; 6 is the unkeyed variant
    3   index_size   u32 LE
    7   count       i32 LE          <- the first 4 bytes of the index blob
    7   index       index_size bytes

    record: [u32 name_length][name][u32 pad][u32 offset][u32 size]   stride = 16 + name_length
"""
import struct
import sys

OUT = sys.argv[1] if len(sys.argv) > 1 else '/tmp/artemis-min/root.pfs'
# Version 6 keeps the payloads unkeyed, so the file stays readable with a hex editor while debugging.
VERSION = sys.argv[2] if len(sys.argv) > 2 else '6'

SYSTEM_INI = """\
; Minimal Artemis configuration for a host smoke test.
; `boot.ini` points the engine at the entry script; the web host reads WIDTH/HEIGHT from here.
[Global]
Title=artc web host smoke test
[WINDOW]
WIDTH=800
HEIGHT=600
"""

FILES = [
    ('system.ini', SYSTEM_INI.encode('utf-8')),
]


def build(files, version):
    # The table is a flat run of records; offsets are absolute in the finished file, so the header
    # plus table has to be sized before the payload offsets are known. Two passes.
    def build_table(offsets):
        out = bytearray()
        out += struct.pack('<i', len(files))
        for (name, payload), offset in zip(files, offsets):
            raw = name.encode('cp932')
            out += struct.pack('<I', len(raw))
            out += raw
            out += struct.pack('<I', 0)                 # the field both references ignore
            out += struct.pack('<I', offset)
            out += struct.pack('<I', len(payload))
        return bytes(out)

    table = build_table([0] * len(files))
    header_size = 7
    offsets = []
    cursor = header_size + len(table)
    for _, payload in files:
        offsets.append(cursor)
        cursor += len(payload)
    table = build_table(offsets)

    # `index_size` is the length of the **index blob**, and the blob's first 4 bytes are the count —
    # so it is exactly `len(table)`, where `build_table` already emitted the count first. Writing
    # `len(table) + 4` (the count once more) makes the recorded index overlap the first payload, which
    # still parses because the reader stops after `count` records, but leaves the file internally
    # inconsistent and the engine reading 4 bytes into the first file.
    out = bytearray()
    out += b'pf' + version.encode('ascii')
    out += struct.pack('<I', len(table))
    out += table
    for _, payload in files:
        out += payload
    return bytes(out)


data = build(FILES, VERSION)
with open(OUT, 'wb') as fh:
    fh.write(data)

index_size = struct.unpack_from('<I', data, 3)[0]
count = struct.unpack_from('<i', data, 7)[0]
print(f'wrote {OUT}  ({len(data):,} bytes)')
print(f'  version={VERSION}  index_size={index_size:,}  count={count}')
for name, payload in FILES:
    print(f'  {name}  {len(payload):,} bytes')
