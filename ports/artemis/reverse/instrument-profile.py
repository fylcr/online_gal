#!/usr/bin/env python3
"""
instrument-profile.py -- add a hit counter to every function of the transpiled engine.

The generated C++ looks like this, thousands of times:

    u32 w2c_artemis_f200(w2c_artemis* instance, u32 var_p0, u32 var_p1, u32 var_p2) {
      u32 var_l3 = 0, var_l4 = 0, var_l5 = 0;
      FUNC_PROLOGUE;
      uint8_t* const wasm_rt_local_memory_base = (instance->w2c_a_a)->data;
      ...

The counter goes immediately after `FUNC_PROLOGUE;`, which every generated body has and which
is already the first statement that runs. Putting it there rather than after the body's own
declarations keeps the insertion a single-line edit at a fixed anchor, and it stays correct
when a body declares locals before the prologue, as the asyncify parts do.

What it writes, per function index N:

    artc_profile_hit(N);

and nothing else. The counter itself lives in `artc-profile.c`, which the build links in. The
edit is textual on purpose: the alternative is parsing 100 MB of machine-written C++, and the
anchor is a literal string that only appears as a statement.

This is a *measurement* tool. It runs only behind `build-bridge-route.sh --profile`, so the
shipped build carries none of it.

Usage:
    python instrument-profile.py <generated.cpp> [--map <funcs.tsv>] [--out-map <file>]

`--map` takes a `functions.tsv` / `warden-index.tsv`-style table so the profile can be printed
with whatever names are known; `--out-map` writes the index -> name table actually used, which
is what the report needs to be readable.

Exits non-zero if it instruments nothing, because an instrumented build that silently has no
counters is worse than one that fails: the profile would come back all zeros and read like a
finding about the game.
"""

import argparse
import re
import sys

# A definition, not a forward declaration or a call: the return type and name start the line
# and a `{` ends it. `.*?` rather than `[^)]*` because a generated signature can be long but
# never spans a line.
DEFINITION = re.compile(r"^[A-Za-z_][A-Za-z0-9_ \t*]*\bw2c_artemis_f(\d+)\s*\(.*\)\s*\{")
PROLOGUE = "  FUNC_PROLOGUE;"

# Prepended once, not written at every call site: 5,579 prototypes would cost more to compile
# than the calls themselves. `extern "C"` because this file is compiled as C++ while the
# counter is C, and without it the two name-mangle differently.
PROTOTYPE = """/* Injected by instrument-profile.py: artc_profile_hit lives in artc-profile.c. */
#ifdef __cplusplus
extern "C" void artc_profile_hit(unsigned int index);
#else
void artc_profile_hit(unsigned int index);
#endif
"""


def load_names(path):
    """
    Read an index -> name table.

    Two shapes are accepted and both are produced elsewhere in this tree:
      * `functions.tsv`:  `<index>\\t<name>`  (possibly sparse)
      * `warden-index.tsv`: `# ...` header then `<index>\\t<name>\\t<provenance>\\t...`
    Blank names are dropped, so an un-named function simply has no entry.
    """
    names = {}
    with open(path, encoding="utf-8", errors="replace") as handle:
        for line in handle:
            if line.startswith("#") or not line.strip():
                continue
            parts = line.rstrip("\n").split("\t")
            if len(parts) < 2 or not parts[0].strip().isdigit():
                continue
            name = parts[1].strip()
            if name:
                names[int(parts[0])] = name
    return names


def instrument(lines):
    """
    Yield instrumented lines, and report what was seen.

    Returns (lines, instrumented_count, highest_index, indices_seen). Kept separate from main
    so the transform can be tested without a 100 MB input.
    """
    out = []
    current = None
    instrumented = 0
    highest = -1
    seen = set()

    for line in lines:
        match = DEFINITION.match(line)
        if match:
            current = int(match.group(1))
            highest = max(highest, current)
        out.append(line)
        # The anchor only counts once we are inside a function, so a stray `FUNC_PROLOGUE;`
        # elsewhere in the file cannot pick up a bogus index.
        if current is not None and line.rstrip("\n") == PROLOGUE:
            if current in seen:
                # A function split across parts (asyncify) is still one function to the
                # profile: counting each part separately would make the totals incomparable
                # with the plain flavour, where the same code is one function.
                current = None
                continue
            seen.add(current)
            out.append(f"  artc_profile_hit({current}u);\n")
            instrumented += 1
            current = None

    return out, instrumented, highest, seen


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("source", help="the generated .cpp to instrument")
    parser.add_argument("--map", dest="map_path", default=None,
                        help="index -> name table to copy names from")
    parser.add_argument("--out-map", dest="out_map", default=None,
                        help="write the resulting index -> name table here")
    parser.add_argument("--report", action="store_true",
                        help="print what was instrumented")
    args = parser.parse_args()

    names = load_names(args.map_path) if args.map_path else {}

    with open(args.source, encoding="utf-8", errors="surrogateescape") as handle:
        lines = handle.readlines()

    out, instrumented, highest, seen = instrument(lines)

    if instrumented == 0:
        print("instrument-profile.py: no function bodies found -- did the generator change?",
              file=sys.stderr)
        return 1

    with open(args.source, "w", encoding="utf-8", errors="surrogateescape", newline="\n") as handle:
        handle.write(PROTOTYPE)
        handle.writelines(out)

    if args.out_map:
        with open(args.out_map, "w", encoding="utf-8", newline="\n") as handle:
            handle.write("# func_index\tname\n")
            for index in sorted(seen):
                handle.write(f"{index}\t{names.get(index, '')}\n")

    print(f"  profile: {instrumented} functions instrumented, highest index {highest}")
    if highest >= 8192:
        print(f"  profile: index {highest} exceeds ARTC_PROFILE_MAX (8192) -- raise it in "
              f"artc-profile.c", file=sys.stderr)
        return 1
    if args.report:
        for index in sorted(seen):
            print(f"    {index}\t{names.get(index, '')}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
