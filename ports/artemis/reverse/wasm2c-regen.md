# Can the wasm2c decompile be regenerated? — yes, byte-identically

**Verdict: regenerable, byte-identically, from a clean clone.** All four reference files
reproduce exactly from the **committed** vendor wasm using wabt built at the pinned commit.
`.ref/` is not needed for any of it.

The in-repo recipe is `gen-wasm2c.sh` beside this note. What it exists to close: a clean clone
has no `.ref/artemis-official/wasm/`, so `build-bridge-route.sh` has no `artemis.wasm2c.c`,
no `.h`, and no `wasm/runtime/`. It turns out none of that has to be shipped — the input is
already committed and the generator is pinned.

## The measurement

Generator, with its commit proven rather than assumed:

```
$ .ref/wabt/build/wasm2c.exe --version
1.0.42 (git~1.0.42-8-g02418f72)
$ git -C .ref/wabt rev-parse HEAD
02418f72a70466b45805891843f114c7f602eb34
```

Input is the committed vendor wasm (both files are `git ls-files` members, so a clean clone
has them):

```
$ md5sum ports/artemis/vendor/artemis-sdk/WebAssembly/artemis.wasm \
         ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm
1f23bf7c7fcc26362340bd18a78e121b  .../WebAssembly/artemis.wasm
5d31853df6ad7a209abbe2508d9c6c60  .../WebAssembly/asyncify/artemis.wasm
```

Those are the same bytes the licensed SDK carries — `.ref/artemis/WebAssembly/artemis.wasm`
and `.../asyncify/artemis.wasm` hash identically — so the committed copy is a faithful
substitute for the download the reference was made from.

Generated, and compared against the four values at `.ref/artemis-official/MANIFEST.txt:21-24`:

| file | size | md5 | reference |
| --- | --- | --- | --- |
| `asyncify/artemis.wasm2c.c` | 107,593,501 | `d895a74b11ba767b06c82d722c407517` | ✅ exact |
| `asyncify/artemis.wasm2c.h` | 39,858 | `306f97d1e6fa4d64ea0e5dc720dee7ab` | ✅ exact |
| `sync/artemis.wasm2c.c` | 47,741,355 | `080822b6e14345d339312d1e1bcfd0aa` | ✅ exact |
| `sync/artemis.wasm2c.h` | 35,307 | `88567995b73211a885d218514ada897e` | ✅ exact |

The 9 files of `wasm/runtime/` come from `<wabt>/wasm2c/` and are **md5-identical** to the
reference dir (`wasm-rt.h`, `wasm-rt-impl.h`, `wasm-rt-impl.c`, `wasm-rt-mem-impl.c`,
`wasm-rt-exceptions.h`, `wasm-rt-exceptions-impl.c`, `wasm-rt-impl-tableops.inc`,
`wasm-rt-mem-impl-helper.inc`, plus `README.md` → `README.wasm2c.md`). wasm2c does not emit
`wasm-rt.h`; it ships next to the tool, which is why the copy step is part of the recipe.

Determinism: running the same binary twice on the same input gives byte-identical output.

### It is not a compiler or host artifact

The same pinned tree built on **Linux** (Alpine, `g++ 13.2.1`, `cmake 3.27.8`) produced all four
identical md5s. So the output does not depend on the host compiler, the CMake version, or the
OS — only on the wabt commit and the input bytes. That matters because the CI runner is not
this machine.

## The `-o` basename trap — it breaks two files, not one

wasm2c derives names from the `-o` **basename**:

- the `.c` gets `#include "artemis.wasm2c.h"`; `-o asyncify.c` writes `#include "asyncify.h"`
  (−6 B);
- the `.h` gets its include guard from the same stem: `ARTEMIS_WASM2C_H_GENERATED_` (27 B)
  becomes `ASYNCIFY_H_GENERATED_` (21 B) at **three** places — `#ifndef`, `#define`, `#endif`
  — for −18 B.

**The only correct invocation is `-o <dir>/artemis.wasm2c.c`.** A wrong basename silently
changes both files while every check except a hash comparison still passes, which is exactly
how this cost time.

### The 30-byte gap was two different things

Regenerating with the **release tag** `1.0.42` (`ff0ef7e0009402740c805a9744c09b05be063e48`)
separates them:

```
reference        (pinned 02418f72, -o artemis.wasm2c.c)   107,593,501
release 1.0.42                , -o artemis.wasm2c.c        107,593,477   →  −24 B
release 1.0.42                , -o asyncify.c              107,593,471   →  −30 B
```

- **−24 B is a real wabt-version difference, and it is the decisive one.** It is a single line,
  appearing once in each `.c` (`artemis.wasm2c.c:214`), and `diff` between the release-tag
  output and the reference is exactly that one line plus its counterpart:
  `#elif defined(__GNUC__) && WASM_RT_MEMCHECK_GUARD_PAGES` (55 B) vs
  `#elif (defined(__GNUC__) || defined(__clang__)) && WASM_RT_MEMCHECK_GUARD_PAGES` (79 B).
  It comes from commit `7da43f24` — *"Keep guard-page bounds checking enabled under clang-cl.
  (#2863)"* — which is inside `1.0.42..02418f72` and changes
  `src/template/wasm2c.declarations.c` line 194. Confirmed by reading that template at both
  commits, and by building the tag.
  **So the release binary genuinely cannot produce the reference even with the right basename.**
  The pinned build is required, not merely tidier. The same 24 B appears in the sync `.c`.
- **−6 B was the basename bug** in the `.c`, plus −18 B in the `.h` (the guard, 3×6). Those are
  the bytes that vanish from the `.h`: `39858 → 39840`.

Both effects are inert *once the basename is correct and the commit is pinned*, because the
output is then byte-identical and the question of tolerance never arises. For the record, if a
future divergence did appear, `rw-splice.py:27` anchors on markers, not line numbers —
`DEFRE = re.compile(r'^([A-Za-z_]\w*) (w2c_artemis_f(\d+)\(w2c_artemis\* instance)')` — so
splicing is robust to line shifts but *not* to a changed guard-pages line surviving into the
rebuilt module. The md5 check in the recipe is what converts that from silent to loud.

## Cost on a runner, and the one non-obvious prerequisite

**wabt needs the `third_party/picosha2` submodule.** Without it the build dies at
`src/sha256.cc:22: fatal error: picosha2.h: No such file or directory`. I checked whether the
other six submodules (`gtest`, `ply`, `simde`, `testsuite`, `uvwasi`, `wasm-c-api`) are needed:
with `-DBUILD_TESTS=OFF -DBUILD_LIBWASM=OFF` they are not — a tree reduced to `picosha2` alone
builds `wasm2c` and produces the correct hash. So a CI clone needs
`git submodule update --init third_party/picosha2`, not `--recursive`.

Measured (this machine, both figures are wall clock for `--target wasm2c`):

| host | cores / `-j` | configure | build | total |
| --- | --- | --- | --- | --- |
| Windows, MinGW `c++` 15.2.0 | 20 / `-j8` | ~18 s | 28.6 s | **50.4 s** clean configure+build |
| Linux, Alpine `g++` 13.2.1 | 2 / `-j2` | 3.2 s | 94.2 s | **97 s** |

The CLI is only for MinGW; on a Linux runner the `MinGW Makefiles` generator is dropped. The
whole thing is a CMake build of one static library plus one executable, so it caches normally
(a `ccache`/build-dir cache removes almost all of the 50–97 s).

Running `gen-wasm2c.sh` itself, once wabt exists, is cheap — 2.4 s for asyncify and 1.6 s for
sync, dominated by writing 155 MB.

## Answer

**A CI runner can build the Artemis bridge from a clean clone**, because the only input the
decompile needs is the committed vendor wasm and the generator is a ~50–97 s CMake build of
wabt at `02418f72` (requiring the `picosha2` submodule) — so no prebuilt 103 MB artifact and
no `.ref/` checkout are required.

## Reproducing

```
# once: wabt at the pinned commit
git clone https://github.com/WebAssembly/wabt .ref/wabt
git -C .ref/wabt checkout 02418f72a70466b45805891843f114c7f602eb34
git -C .ref/wabt submodule update --init third_party/picosha2
cmake -S .ref/wabt -B .ref/wabt/build -DCMAKE_BUILD_TYPE=Release \
      -DBUILD_TESTS=OFF -DBUILD_LIBWASM=OFF
cmake --build .ref/wabt/build --target wasm2c -j8

# then, from the repo root
bash ports/artemis/reverse/gen-wasm2c.sh
```

Both steps are what the script's own header documents; the script verifies all four md5s and
exits non-zero on any mismatch. It generates into a staging directory and installs only after
the check passes, so a mismatch can never overwrite a good reference file with a bad one.

### Running it

```
$ bash ports/artemis/reverse/gen-wasm2c.sh

== generator
  wabt:  1.0.42 (git~1.0.42-8-g02418f72)
  commit: 02418f72a70466b45805891843f114c7f602eb34
  input SDK: /d/online_gal/ports/artemis/vendor/artemis-sdk
  output:    /d/online_gal/.ref/artemis-official

== sync
  src: .../WebAssembly/artemis.wasm
   OK  artemis.wasm2c.c     md5=080822b6e14345d339312d1e1bcfd0aa  47741355 B
   OK  artemis.wasm2c.h     md5=88567995b73211a885d218514ada897e  35307 B

== asyncify
  src: .../WebAssembly/asyncify/artemis.wasm
   OK  artemis.wasm2c.c     md5=d895a74b11ba767b06c82d722c407517  107593501 B
   OK  artemis.wasm2c.h     md5=306f97d1e6fa4d64ea0e5dc720dee7ab  39858 B

== runtime sources <- /d/online_gal/.ref/wabt/wasm2c
   copied 9 file(s)

== verification
   OK  artemis.wasm2c.c     md5=d895a74b11ba767b06c82d722c407517  107593501 B
   OK  artemis.wasm2c.h     md5=306f97d1e6fa4d64ea0e5dc720dee7ab  39858 B
   OK  artemis.wasm2c.c     md5=080822b6e14345d339312d1e1bcfd0aa  47741355 B
   OK  artemis.wasm2c.h     md5=88567995b73211a885d218514ada897e  35307 B

== done -- both flavours byte-identical to the reference
```

That is the verbatim output of the run performed in this session. The fail path was also
exercised (with one expected md5 deliberately corrupted): it prints the MISMATCH block, exits
1, and leaves the destination untouched.

### Measurements that are mine vs. inherited

Everything above was measured in this session on this machine, except: the reference sizes and
md5s are read from `.ref/artemis-official/MANIFEST.txt:21-24`, and the claim that the
*release* 1.0.42 binary fails to reproduce was first observed by the Lead. I reproduced and
then explained that failure (see the decomposition above), including the fact that only 6 of
the 30 bytes was the basename bug — the other 24 are a real version difference.

One artifact of the investigation, for completeness: `.ref/wabt` is a git checkout and the
`git worktree` I used to build the release tag was removed again (`git worktree list` shows only
the main tree, still at `02418f72`). `git status` in that checkout reports six
`src/prebuilt/*.cc` files as modified, but `git diff` is empty — that is Git's CRLF stat-cache
on a Windows working copy, not a content change, and the pinned tree is unmodified.
