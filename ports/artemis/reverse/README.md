# Reversing the official Artemis binaries

Two recipes. Both answer the same question — *is this port's description of the official
engine actually right?* — against the shipped binaries instead of against documentation.

```
fetch-wabt.sh   once: wabt 1.0.42       (wasm side)
fetch-zig.sh    once: zig 0.16.0        (so side)
reverse.sh      wasm | so [arch] | so-all
rebuild.sh      wasm | so [arch] | all   <- the checks; non-zero exit means a claim failed
```

Everything lands in `out/`, which is gitignored: the dumps are hundreds of MB and are
derivative works of a proprietary SDK.

## Why there are two targets

Because there are two toolchains and they answer different questions.

| | input | output | the check |
| --- | --- | --- | --- |
| **wasm** | `artemis.wasm` (plain, 2.68 MB) and `asyncify/artemis.wasm` (6.00 MB) | `.wat` (48 MB / 180 MB), `wasm2c` C (48 MB / 108 MB) | `wat2wasm(wasm2wat(bin))` is **byte-identical**, and the C **compiles** |
| **so** | `libartemis.so` arm64-v8a (8.15 MB) and armeabi-v7a (7.40 MB) | 9042 / 9175 one-function `.c` files | the rebuilt **export surface is identical** |

The wasm build has no name section, so its functions are `a.b`, `a.c`, `a.aa`… The Android
natives have 13k real dynamic symbols, which makes them the oracle for those names —
that is the second reason both targets exist.

## The wasm round-trip is exact, and that is not luck

    OK   plain:    wat2wasm(wasm2wat(bin)) == bin (2682787 bytes, sha256 f05ae1cb25eaaff8…)
    OK   asyncify: wat2wasm(wasm2wat(bin)) == bin (5998665 bytes, sha256 7ceabed28b84e768…)

The SDK ships a stock Emscripten release build, so its section layout is already
canonical and wabt's writer reproduces it exactly. The check is still worth running: the
moment either side is built differently the round-trip stops matching and the script says
so, rather than quietly passing.

**wabt is pinned (1.0.42) for this reason.** `.wat` is not a canonical form, so a different
wabt may emit an equally valid but differently spelled module and `wat2wasm` would then not
give back the original bytes — a failure that says nothing about the SDK.

The second half of the check is that the transpiled C is real C: gcc has to accept 1.19M
lines of it (plain) and 2.5M lines (asyncify). It does, producing 18.5 MB and 37.1 MB
objects. That is what makes the pseudo-C usable as source rather than just as reading.

## The `so` export surface is rebuilt name-for-name

    OK   arm64-v8a:   rebuilt export surface is identical (13051 symbols)
    OK   armeabi-v7a: rebuilt export surface is identical (13196 symbols)

A shared library cannot be rebuilt *byte*-identically from decompiled pseudo-C —
decompilation loses types and no compiler reproduces an NDK release build's instruction
selection. What can be checked is the part interoperation depends on: `build-so-stub.sh`
emits one translation unit with the same exported names and the same weak/strong binding,
compiles it for the target ABI, and `rebuild.sh` diffs the two `llvm-nm -D` lists in both
directions.

Both directions matter and they fail differently. A name only on the official side is one a
caller would fail to resolve; a name only on our side is one this recipe invented, which a
caller could then bind to by accident and get a stub instead of the real implementation.

`A` (absolute) symbols are dropped from both sides. They are `__bss_start`, `_end`,
`__bss_end__` and the rest — synthesised by the linker script, so no translation unit can
define them and their absence says nothing about the recipe. They are the entire difference
between the raw 13,058 and the comparable 13,051. The same 7 names explain why
armeabi-v7a counts 13,252 raw and 13,196 comparable.

Weak (`W`) and guard-variable (`V`) names are compared as names only, not as binding
strength: a name that is weak officially and strong here still satisfies every caller.

## The cross-compiler, and the two that do not work

MinGW's gcc cannot produce the `.so`: it always links x86-64 COFF, so `-shared -fPIC`
yields a file with no dynamic symbol table at all, and it rewrites every weak symbol to
`.weak.<name>.<anchor>`. Both failure modes look like a broken recipe and are not.

The emsdk's clang cannot either: its LLVM is built for `wasm32 wasm64 x86 x86-64` only, so
`--target=aarch64-linux-android` dies with `unable to create target`. The tree also has no
`ld.lld.exe` and no `llvm-mc`.

Zig is used instead — one download, both ABIs, no NDK:

    zig cc --target=aarch64-linux-gnu  -shared -fPIC -O0 -nostdlib stub.c -o out.so
    zig cc --target=arm-linux-gnueabihf -shared -fPIC -O0 -nostdlib stub.c -o out.so

`-nostdlib` keeps the output to just the translation unit, which is all the export surface
is about. `fetch-zig.sh` pins the release for the same reason `fetch-wabt.sh` pins wabt.

## Ghidra notes

`ghidra/DecompileAll.java` writes one `.c` per function plus `index.tsv` (name, address,
size, file) and `summary.txt`. One file per function rather than one 100 MB blob, so
callers of a symbol stay greppable.

Two things bite when running it headless:

- **Ghidra 12.1.4 removed `DecompInterface.getDefaultOptions()`.** Calling it fails to
  compile with `cannot find symbol`, and the visible symptom is
  `ERROR REPORT SCRIPT ERROR: DecompileAll.java : The class could not be found`.
- **Headless will not create the project directory.** It aborts with
  `Directory not found: …\out\ghidra-arm64`; `reverse.sh` makes it first. Paths also must
  not contain a `.`-prefixed component.

Result: 9039 ok / 3 failed and 9173 ok / 2 failed. Named functions come out with real
signatures, e.g. `CommandTag__0060f8c0.c` → `void __thiscall
artemis::CArtemis::CommandTag(CArtemis *this, CScriptBlock *param_1, bool param_2)`, whose
body is the tag registry itself.

`Windows/Artemis.exe` is packed — Ghidra finds 131 functions and 0 instructions — so the
`.so` files are the authority, not the Windows build.

## Which SDK tree it reads

`reverse.sh` and `rebuild.sh` resolve the SDK the same way: `$ARTC_SDK` if set, else the
licensed `.ref/artemis` if it is present, else the vendored copy under
`ports/artemis/vendor/artemis-sdk/`. The native lives in a different place in each — the
full SDK has `Android/app/src/main/jniLibs/<abi>/libartemis.so`, the vendored tree has
`Android/<abi>/libartemis.so` — so `so_path()` tries both.

Both recipes were run against both layouts; the results are identical.
