"""Stage the reference runtime, dropping one check the bridge cannot satisfy.

The generated instantiate allocates the engine's own funcref table -- 4278 slots, a
`calloc` of 34 KB -- out of this module's libc heap.  In the bridge that heap is the
stretch of memory between our statics and the stack, the request is refused, and the
runtime calls `abort()`, which kills the module before the first frame.  The engine never
reads that table: an indirect call in the bridge goes through the table this module
exports to the page.  Dropping the check leaves the pointer null and the rest of the
runtime untouched.

The runtime sources are copied out of the reference tree in the repository and the one
edit is applied to the copy, so the module CI builds and the module a developer builds
come from the same source.  A staging directory that was edited by hand once made those
two differ silently.  The guard must be found, so this cannot quietly stop matching after
an upstream update.

Usage: patch-runtime-alloc.py <reference-runtime-dir> <staging-dir>
"""

import os
import shutil
import sys

TEMPLATE = "wasm-rt-impl-tableops.inc"

GUARD = b"""  if (table->size != 0 && !table->data) {
    abort();
  }
"""

GUARD_CRLF = GUARD.replace(b"\n", b"\r\n")


def main(argv):
    if len(argv) != 3:
        sys.stderr.write(__doc__)
        return 2
    src_dir, stage_dir = argv[1], argv[2]
    os.makedirs(stage_dir, exist_ok=True)
    for name in sorted(os.listdir(src_dir)):
        source = os.path.join(src_dir, name)
        if os.path.isfile(source):
            shutil.copyfile(source, os.path.join(stage_dir, name))

    target = os.path.join(stage_dir, TEMPLATE)
    with open(target, "rb") as handle:
        text = handle.read()
    if text.count(GUARD) == 1:
        edited = text.replace(GUARD, b"")
    elif text.count(GUARD_CRLF) == 1:
        edited = text.replace(GUARD_CRLF, b"")
    else:
        sys.stderr.write(
            "patch-runtime-alloc: %s does not hold the table guard exactly once\n" % target
        )
        return 1
    with open(target, "wb") as handle:
        handle.write(edited)
    print("     table guard dropped from %s" % TEMPLATE)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
