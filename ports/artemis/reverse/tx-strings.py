#!/usr/bin/env python3
"""Extract a linear-address -> C-string table from the official wasm data segments.

Usage: python tx-strings.py <artemis.wasm> <out.tsv>
       python tx-strings.py --fn fNNN[,fNNN...] <artemis.wasm> [artemis.wasm2c.c]

Run once per flavour; the result is a lookup aid for transcription
("what does the constant 177287u point at?").  Parses the binary directly
(the .wat is 180 MB; the .wasm is 6 MB and the data section is trivial).

--fn answers the naming question for one generated function: which strings do
its constants point at, and which products/host imports does it call.  The
string dump is byte-oriented (NUL-to-NUL span, decoded UTF-8 then CP932 then
latin-1) because the engine's own messages are Japanese and the ASCII table
above cannot see them.
"""
import os
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


def load_image(path):
    blob = open(path, "rb").read()
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

    return bytes(image), top


def deref(image, c, span=96):
    """NUL-to-NUL byte span around linear address c, decoded best-effort."""
    lo = image.rfind(b"\0", max(0, c - span), c) + 1
    hi = image.find(b"\0", c, c + span)
    if hi < 0:
        hi = min(len(image), c + span)
    if lo > c:
        return None
    raw = image[lo:hi]
    for enc in ("utf-8", "cp932", "latin-1"):
        try:
            text = raw.decode(enc)
            break
        except UnicodeDecodeError:
            continue
    else:
        text = raw.decode("latin-1", "replace")
    return c - lo, "".join(ch if ch.isprintable() else "." for ch in text)


def dump_fn(wasm, gen_c, fn_ids):
    """Per-function naming aid: data-segment constants + callee list."""
    image, top = load_image(wasm)
    src = open(gen_c, encoding="utf-8", errors="replace").read().splitlines()
    head = re.compile(r"^\w+(?: \w+)? w2c_artemis_(f\d+)\(w2c_artemis\* instance")
    starts = [(i, head.match(line).group(1)) for i, line in enumerate(src) if head.match(line)]
    spans = {fn: (i, starts[k + 1][0] if k + 1 < len(starts) else len(src))
             for k, (i, fn) in enumerate(starts)}
    const = re.compile(r"= (\d+)u;")
    for fn in fn_ids:
        if fn not in spans:
            print("// %s: not a generated function in %s" % (fn, gen_c))
            continue
        a, b = spans[fn]
        body = "\n".join(src[a:b])
        print("// ===== %s  lines %d-%d (%d) =====" % (fn, a + 1, b, b - a))
        hits, seen = [], set()
        for m in const.finditer(body):
            c = int(m.group(1))
            if c in seen or c < 1024 or c >= top:
                continue
            seen.add(c)
            got = deref(image, c)
            if not got:
                continue
            delta, text = got
            printable = sum(1 for ch in text if ch.isprintable())
            if delta > 4 or printable < 3 or printable < 0.6 * len(text):
                continue
            hits.append((c, delta, text))
        hits.sort(key=lambda row: row[2])
        for c, delta, text in hits:
            print("  const %-10d rel %-4d %s" % (c, delta, text[:120]))
        if not hits:
            print("  (no data-segment constants in range)")
        callees = sorted(set(re.findall(r"w2c_artemis_(f\d+|Oc_\d+|a_\w+)\(", body)) - {fn})
        print("  callees: %s" % (" ".join(callees) or "-"))
    return 0


def main():
    try:
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    except (AttributeError, ValueError):
        pass
    if sys.argv[1] == "--fn":
        wasm = sys.argv[3]
        gen_c = sys.argv[4] if len(sys.argv) > 4 else os.path.join(
            os.path.dirname(os.path.abspath(__file__)), "..", "..", "..",
            ".ref", "artemis-official", "wasm", "asyncify", "artemis.wasm2c.c")
        return dump_fn(wasm, os.path.normpath(gen_c), sys.argv[2].split(","))
    image, top = load_image(sys.argv[1])

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
