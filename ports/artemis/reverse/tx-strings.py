#!/usr/bin/env python3
"""Extract a linear-address -> C-string table from the official wasm data segments.

Usage: python tx-strings.py <artemis.wasm> <out.tsv>

Run once per flavour; the result is a lookup aid for transcription
("what does the constant 177287u point at?").  Parses the binary directly
(the .wat is 180 MB; the .wasm is 6 MB and the data section is trivial).
"""
import re
import sys


def read_leb(buf, i):
    result = shift = 0
    while True:
        byte = buf[i]
        i += 1
        result |= (byte & 0x7F) << shift
        if not byte & 0x80:
            return result, i
        shift += 7


def main():
    blob = open(sys.argv[1], "rb").read()
    assert blob[:4] == b"\0asm", "not a wasm module"
    off = 8
    segments = []
    while off < len(blob):
        sec_id = blob[off]
        off += 1
        size, off = read_leb(blob, off)
        end = off + size
        if sec_id == 11:  # Data
            count, off = read_leb(blob, off)
            for _ in range(count):
                flags, off = read_leb(blob, off)
                assert flags == 0, "passive segment — no address to map"
                assert blob[off] == 0x41, "offset is not i32.const"
                off += 1
                addr, off = read_leb(blob, off)
                assert blob[off] == 0x0B, "offset expr not terminated"
                off += 1
                length, off = read_leb(blob, off)
                segments.append((addr, blob[off:off + length]))
                off += length
        off = end

    top = max(a + len(b) for a, b in segments)
    image = bytearray(top)
    for addr, payload in segments:
        image[addr:addr + len(payload)] = payload

    rows = []
    i = 0
    while i < top:
        if 0x20 <= image[i] <= 0x7E:
            j = i
            while j < top and 0x20 <= image[j] <= 0x7E:
                j += 1
            if j - i >= 4 and j < top and image[j] == 0:
                s = image[i:j].decode("ascii").replace("\t", "\\t")
                rows.append("%d\t%s" % (i, s))
            i = max(j, i + 1)
        else:
            i += 1

    with open(sys.argv[2], "w", encoding="utf-8") as out:
        for row in rows:
            out.write(row + "\n")
    print("strings: %d  image top: %d" % (len(rows), top))


if __name__ == "__main__":
    main()
