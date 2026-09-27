#!/usr/bin/env python
"""patch-native-traps.py -- trap-text parity for call_indirect failures.

The wasm2c preamble pre-checks every indirect call (func_types_eq_slowpath) and
traps with wasm_rt_trap(WASM_RT_TRAP_CALL_INDIRECT), which in an emcc build
collapses to abort() -> `unreachable`.  The official module instead lets the
engine's own call_indirect raise its native trap text.  Measured on this box's
V8 (ports/artemis/reverse/build/trap-exp/):

    type mismatch   -> "function signature mismatch"
    null funcref    -> "function signature mismatch"
    table OOB       -> "table index is out of bounds"

Fix: a bounds failure issues a synthetic call_indirect through an index no real
table can contain (the engine then reports its native OOB text); type and null
failures return "eq" so the real call_indirect in DO_CALL_INDIRECT raises the
native type-mismatch text at exactly the point the official module does.
Successful calls are untouched: this only re-surfaces paths that were already
going to trap, so `differ: 0` cannot regress.

Usage: python patch-native-traps.py <src.cpp>   (idempotent)
"""
import re
import sys

path = sys.argv[1]
with open(path, encoding="utf-8", errors="surrogateescape") as fh:
    text = fh.read()

if "trap_oob" in text:
    print("patch-native-traps: already patched")
    sys.exit(0)

NEW = """W2C_COLD_FUNC static bool func_types_eq_slowpath(
    const wasm_rt_funcref_table_t* table,
    const wasm_rt_func_type_t expected_type,
    uint32_t index) {
  (void)expected_type;
  if (index >= table->size) {
    /* Table index out of bounds.  Reproduce the engine's native trap text by
       issuing a call_indirect through an index no real table can contain. */
    void (*volatile trap_oob)(void) = (void (*)(void))(uintptr_t)0xffffffffu;
    trap_oob();
    return false;
  }
  /* Type and null checks are deliberately deferred to the real call_indirect
     in DO_CALL_INDIRECT: the engine raises its native trap text ("function
     signature mismatch") exactly where the official module does. */
  return true;
}"""

pat = re.compile(r"W2C_COLD_FUNC static bool func_types_eq_slowpath\(.*?\n\}", re.S)
text, n = pat.subn(NEW, text, count=1)
if n != 1:
    sys.exit("patch-native-traps: slowpath not found (wasm2c preamble changed shape?)")
with open(path, "w", encoding="utf-8", errors="surrogateescape", newline="") as fh:
    fh.write(text)
print("patch-native-traps: call_indirect failures now defer to engine traps")
