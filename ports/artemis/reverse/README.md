# Reversing the official Artemis binaries

Two recipes. Both answer the same question — *is this port's description of the official
engine actually right?* — against the shipped binaries instead of against documentation.

```
fetch-wabt.sh   once: wabt 1.0.42       (wasm side)
fetch-zig.sh    once: zig 0.16.0        (so side)
reverse.sh      wasm | so [arch] | so-all
rebuild.sh      wasm | so [arch] | all   <- the checks; non-zero exit means a claim failed
build-cxx-route.sh  asyncify | sync      <- the same wasm, built without wabt at all
```

Everything lands in `out/`, which is gitignored: the dumps are hundreds of MB and are
derivative works of a proprietary SDK.

## Why there are two targets

Because there are two toolchains and they answer different questions.

| | input | output | the check |
| --- | --- | --- | --- |
| **wasm** | `artemis.wasm` (plain, 2.68 MB) and `asyncify/artemis.wasm` (6.00 MB) | `.wat` (48 MB / 180 MB), `wasm2c` C (48 MB / 108 MB) | `wat2wasm(wasm2wat(bin))` is **byte-identical**, the C **compiles**, and the C **links back into a wasm binary that behaves like the original** (with wabt, via `rebuild.sh`) **or without wabt at all** (`build-cxx-route.sh`) |
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

## The transpiled C links back into a wasm binary, and behaves like the original

Compiling is not the same as being correct, so the third assertion closes the loop:

    OK   plain: agreement: 16/18, trap-kind-only: 2 -> Yc, _c, differ: 0
             of the agreements: 15 returned equal values, 1 threw the same trap
    OK   asyncify: agreement: 20/66, trap-kind-only: 46 -> …, differ: 0
             of the agreements: 19 returned equal values, 1 threw the same trap

The rebuilt binary can never be byte-equal to the original — one is Emscripten's release
build, the other is clang's codegen for generated C — so the comparison is behavioural:
both modules are instantiated with inert zero-returning host stubs, every export they share
is called with identical arguments, and the results are compared. Two helper scripts supply
what a bare module needs:

- `wasm-import-stubs.py` writes a definition for each of the module's 195 imports. The
  memory accessor is *not* a stub — it really allocates, or the first memory read faults.
- `wasm-export-shims.py` wraps each `w2c_*` export so its original name is restored via
  `__attribute__((export_name(...)))`, which is why the differ needs no name table. The
  one exported *table* (`Ic`) is skipped and reported rather than silently dropped: the
  table lives in linear memory in the rebuild, so there is nothing to re-export.

Three things in the harness were each briefly indistinguishable from "the rebuild is
broken", and are worth knowing before touching it:

- **Fresh instances per comparison.** Exports share one linear memory and write to it, so
  reusing an instance lets earlier calls perturb later ones and the result then depends on
  the order the names sort in.
- **Exact names win over raw symbols.** The rebuild exports both the restored names and
  `w2c_artemis_*`; those un-mangle onto the same keys, and calling a raw symbol passes the
  instance pointer as its first argument — i.e. `NULL`.
- **`__wasm_call_ctors()` must be called explicitly.** Emscripten runs it from its JS glue,
  which there is none of here; without it no static initialiser runs, including the one
  backing the export shims, and every export dereferences a zeroed instance.

### What "agreement" is worth, per flavour

The plain build is the strong result: 15 of 18 exports return an equal value. The asyncify
build is weaker evidence and the script says so — 46 of 66 exports agree only in that both
throw. That is not a defect in the recipe: the two modules' indirect-call tables differ in
how full they are (official 5,884 entries, rebuild 4,472, because clang only fills the slots
reachable from the rebuilt code), so the same `call_indirect` lands on a populated slot on
one side — `function signature mismatch` — and an empty one on the other — `unreachable`.
Both trap, neither returns, and the difference is a property of the link, not of the
transpilation. `--strict` turns it back into a failure for anyone who wants that.

An export that traps on both sides has still been checked: it reaches the same kind of
failure under the same inputs, which exercises its preamble and argument handling. But it is
weaker than a value match, so the summary reports the split instead of letting the headline
number overstate the work.

### emcc is a soft requirement

The round-trip assertion needs `emcc` (the emsdk), `node` and a working Python 3; it is
skipped with a note when they are absent, so the rest of `rebuild.sh` still runs on a bare
machine. `--export-all -sEXPORT_ALL` is load-bearing in the link line: without it Emscripten
dead-strips everything the JS glue does not reference and the rebuild comes out 4 KB with no
exports, which the differ would report as "0 shared exports".

`command -v python3` is not a sufficient check on Windows: if Python came from the Store the
name resolves to an App Execution Alias that silently does nothing when run non-interactively
— no output, no error, non-zero status. The script asks each candidate for its `--version`
before trusting it.

## Building the wasm without wabt: `build-cxx-route.sh`

The three checks above all go through wabt (`wasm2wat`, `wat2wasm`, `wasm2c`). wabt is a
*conversion* tool, not part of a build, so `build-cxx-route.sh` establishes a second chain
that does not touch it:

    bash build-cxx-route.sh asyncify   # or sync

It compiles the wasm2c output as C++ and links it with `em++` in one call. Three concessions
are needed, and all three are in the script:

1. **198 casts (195 for sync).** `instance_ptr` is `void**`, so `*instance_ptr` is `void*`,
   while the callee wants `struct w2c_a*`. That is legal C and illegal C++. The fix is
   `*(struct w2c_a**)instance_ptr`, and every site is in a `wasm_tailcall_w2c_a_*` wrapper.
   **clang's `-fpermissive` does not downgrade this the way GCC's does**, so it has to be an
   actual edit: em++ reports `cannot convert argument of incomplete type 'void *' to
   'struct w2c_a *'` and stops. `g++ -fpermissive` would quietly accept the file, which is
   exactly why the acceptance tool has to be em++.
2. **The runtime stays C.** `build/wasm-rt-impl.c` and `wasm-rt-mem-impl.c` each have two
   errors as C++, at `wasm-rt-impl-tableops.inc:54` and `:80` (`table->data = calloc(...)`
   and `= new_data`, `void*` into `void**`). Compile them with `emcc -c -x c` rather than
   editing wabt's runtime.
3. **A one-line include bridge.** `out/*-import-stubs.c` includes `"artemis.h"`; the
   generated header is `artemis.wasm2c.h`. The script writes `inc/artemis.h` rather than
   touching a generated file.

Both flavours build and behave as before:

    asyncify: agreement: 20/66, trap-kind-only: 46, differ: 0   -> 41 852 136 B
    sync:     agreement: 16/18, trap-kind-only:  2, differ: 0   -> 18 182 343 B

The numbers are identical to the wasm2c route, so going through C++ costs no behaviour.
Sizes stay ~7x the official module because both are clang codegen for generated C, not an
Emscripten release build. The script runs the behavioural diff itself and exits non-zero
unless it reports `differ: 0` — which is what makes it the acceptance tool for readable-C++
work: a transcription must not change behaviour, and this is how that is checked.

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

## Making the rebuilt engine a *drop-in* replacement

`build-cxx-route.sh` answers "can the transpiled semantics be built without wabt". It can.
But the artefact fails three of the four gates in `wasm-replace-test.mjs`, and the natural
reading of that is "a module rebuilt from wasm2c output can never satisfy the official
glue". `build-bridge-route.sh` shows that reading is wrong: the contract is reachable, and
the rebuilt engine now renders the real game through the official glue.

    bash build-bridge-route.sh asyncify          # -> out/bridge-asyncify/bridge.wasm
    bash build-bridge-route.sh sync              # -> out/bridge-sync/bridge.wasm

Both flavours pass all four gates:

| | asyncify | plain |
| --- | --- | --- |
| Gate 1 import face | 198/198 consumed, memory imported | 195/195, memory imported |
| Gate 2 export face | 67/67 | 19/19 |
| Gate 3 behaviour | agreement 20/66, differ 0 | 16/18, differ 0 |
| Gate 4 host wiring | 207 imports, GL 65/65 | 204, GL 65/65 |

Four things had to be true at once, and each one looked like a dead end first:

1. **The host ABI can be imported rather than stubbed.** `__attribute__((import_module("a"),
   import_name("xa")))` emits a real webassembly import under the module the glue builds its
   import object for, so `wasm-import-stubs.py --forward` turns all 198 stubs into
   forwarders.

2. **The memory is shared, not owned.** `-sIMPORTED_MEMORY -Xlinker --import-memory=a,a`
   makes the module import `a.a`, and the wasm2c runtime addresses memory as
   `&mem->data[addr]` -- so pointing `data` at address 0 makes the C runtime operate on the
   host's linear memory. The catch is that `(uint8_t*)0` is a null pointer constant and clang
   folds dereferences of it away; the value has to pass through a `volatile` to stay opaque.

3. **Two tenants need distinguishable halves.** The guest's own allocator reads the memory
   descriptor's `size` as its heap ceiling, so returning the whole buffer tells it that
   everything (including this module's statics and stack) belongs to it. It allocates
   straight through them. The accessor therefore reports a *fence*, and the layout sits above
   it. Fencing inside the official 16 MiB starves the guest -- it ran to within four bytes of
   the fence -- so the memory is enlarged and the page is told via
   `Module["INITIAL_MEMORY"]` (the deployer patches that automatically for a candidate).

4. **`Mc` is not just an export.** The glue maps `Mc` to `___wasm_call_ctors`
   (`Module["___wasm_call_ctors"] = Module["asm"]["Mc"]`) and the transpiled module has a
   guest function of that name. So the module's start-up runs *that shim*, not the
   constructor that instantiates the wasm2c instance. Every shim now initialises lazily;
   without that the instance pointer stayed null and the first access trapped reading
   `(*NULL).size`.

Everything else is small: the exported table has to be named `Lc`/`Ic`
(`wasm-rename-export.py`), the leftover `env`/`wasi_snapshot_preview1` imports have to be
folded onto names the glue supplies (`wasm-satisfy-imports.py`), and the imported memory's
declared maximum has to match (`wasm-rename-import.py --memory-max`). All three are in-place
edits to the binary's sections, so no wabt and no re-encode is involved.

### Diagnosing a trap in the field

A trap inside the built module surfaces as `RuntimeError: unreachable` with no information.
`--debug-trap` links in `artc-trap-probe.c`, which records the wasm-rt trap code, and
`instrument-oob.py` additionally rewrites the generated bounds checks to record the address
they refused. Between them, "it traps" becomes "it trapped at address N, length L, against a
memory of size S", which is what identified the two-tenant overlap.

### Serving the real game to a candidate

`ports/artemis/build/official-web-deploy.py --engine-wasm <candidate>` deploys the on-demand
(asyncify) route with a rebuilt module in place of the official one, so the real game is the
test bench. It prints the tester's verdict before deploying and raises `INITIAL_MEMORY` in the
page when the candidate needs more than the official buffer.
