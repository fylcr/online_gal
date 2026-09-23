#!/usr/bin/env python3
"""
Make the transpiled engine report *what* its bounds check refused.

## Why

When two address spaces share one linear memory -- the transpiled engine's own layout and
the emscripten runtime's -- "the module took an out-of-bounds access" is not an answer.
Which address it tried to reach is. `RANGE_CHECK` is the single macro every guest memory
access goes through, so giving it a second job (report, then trap) turns the bare
`RuntimeError: unreachable` into an address, a length, and the size it was compared
against.

The report goes to `artc_record_oob`, whose definition and exports live in
`artc-trap-probe.c`. This script only rewrites the generated C; it is called by
`build-bridge-route.sh --debug-trap` and never by a normal build.

## Usage

    instrument-oob.py <generated.cpp> --proto /path/to/artc-trap-probe.c

The prototype is emitted at the top of the file. It has to come from somewhere: the
generated file includes no project header, and `-I` alone cannot inject a declaration, so
the declaration is prepended to the one translation unit that needs it.
"""

import argparse
import sys

# The macro exactly as wasm2c emits it. Matched on the two statements rather than the whole
# macro body, because the body's other branches differ between wasm2c versions.
OLD = "    if (UNLIKELY(res > (mem)->size))             \\\n      TRAP(OOB);"

NEW = (
    "    if (UNLIKELY(res > (mem)->size)) {           \\\n"
    "      artc_record_oob(offset, len, (mem)->size);  \\\n"
    "      TRAP(OOB);                                 \\\n"
    "    }"
)

# `RANGE_CHECK` computes `offset + len` first, and *that* addition traps on overflow before
# the comparison is ever reached. Instrumenting only the comparison therefore reports
# nothing (0/0/0) for exactly the most suspicious failures -- the ones where the guest
# computed an address so far out that the sum wrapped. Recording here too covers it.
OLD_ADD = "  if (__builtin_add_overflow(a, b, &ret))\n    TRAP(OOB);"

NEW_ADD = (
    "  if (__builtin_add_overflow(a, b, &ret)) {\n"
    "    artc_record_oob(a, b, ~0ull);\n"
    "    TRAP(OOB);\n"
    "  }"
)

# The "accelerated" bounds check used for default32 memories. It has its own TRAP(OOB) and
# is what fires when guard pages are assumed but the memory is a plain imported buffer.
OLD_MEMCHECK = "  if (UNLIKELY(a + (uint64_t)sizeof(t) > mem->size))     \\\n    TRAP(OOB);"

NEW_MEMCHECK = (
    "  if (UNLIKELY(a + (uint64_t)sizeof(t) > mem->size)) {   \\\n"
    "    artc_record_oob(a, (uint64_t)sizeof(t), mem->size);  \\\n"
    "    TRAP(OOB);                                           \\\n"
    "  }"
)

PROTO = (
    "/* injected by instrument-oob.py -- records refused addresses for the trap probe */\n"
    "#ifdef __cplusplus\n"
    'extern "C" {\n'
    "#endif\n"
    "void artc_record_oob(unsigned long long offset, unsigned long long len,\n"
    "                     unsigned long long size);\n"
    "#ifdef __cplusplus\n"
    "}\n"
    "#endif\n"
)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("source", help="the generated C/C++ to rewrite in place")
    args = ap.parse_args()

    text = open(args.source, encoding="utf-8", errors="replace").read()

    hits = 0
    for old, new, what in ((OLD, NEW, "RANGE_CHECK comparison"),
                           (OLD_ADD, NEW_ADD, "RANGE_CHECK address overflow"),
                           (OLD_MEMCHECK, NEW_MEMCHECK, "MEMCHECK_DEFAULT32")):
        if old in text:
            text = text.replace(old, new, 1)
            hits += 1
            print(f"  {what}: instrumented")
        else:
            # Not every build emits every macro; say so rather than failing, because which
            # ones exist is a property of the wasm2c version and the memcheck mode.
            print(f"  {what}: pattern absent (nothing to do)")

    if hits == 0:
        sys.exit("error: none of the bounds-check patterns matched; refusing to guess")

    open(args.source, "w", encoding="utf-8", newline="\n").write(PROTO + text)
    print(f"{args.source}: {hits} site(s) instrumented")
    return 0


if __name__ == "__main__":
    sys.exit(main())
