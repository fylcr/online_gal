# Sync flavour rewrite: 284 products, mechanical transcription, 18/18 agreement

**Not tracked.** The sync products live at `.ref/artemis-official/rewrite-sync/src`, which is
gitignored, and they stay there: only the asyncify flavour is committed as source
(`ports/artemis/src`, see `ports/artemis/REWRITE.md`).  This note is a record of work done,
not a description of a shipped tree.  Sync is not buildable from a clone.

Branch state at the time of this note: `.ref/artemis-official/rewrite-sync/src` = **284 products** + `util/engine_context.h` (220 lines, 50 inline helpers) = **38,191 lines**
(38,127 before the provenance block added on user request; see §8; the header is 223 lines today, its own provenance paragraph included).
`rewrite-sync/rewrite-names.tsv` = 6 comment lines + **284 fn rows**.
Generator: `D:/tmp/sygen2.py` (byte-reproducible). Verifier: `D:/tmp/syverify_f.py` (0 mismatch). Calibration: `D:/tmp/sycal6.py` (4 planted defects, all caught).

Scope was set by the user (m07912): *「现在也按照相同的思路把另一个版本的wasm（也就是D:\online_gal\.ref\artemis\WebAssembly\artemis.wasm）伪C代码转换为可读C++」* — same approach as the asyncify
flavour, applied to the second module. The acceptance number for this flavour is **`wasm-diff --strict` 18/18**, because
`.ref/artemis/WebAssembly/artemis.wasm` only exports 18 entry points; the asyncify module's 66/66 is a different criterion and
does not apply here.

## 1. What actually got built

| Kind | Count | Where |
|---|---|---|
| Transcribed products | 284 | `rewrite-sync/src/{util,script,core}` |
| Ledger rows (col5 non-`—`, file exists) | 284 / 284 | `rewrite-sync/rewrite-names.tsv` |
| Depth stamps | R0 230 · R1 28 · R2 26 | product header `// src: … ; RN transcribe-keep.` |
| Buckets | util 157 · script 54 · core 73 | mirrors the asyncify directory layout |
| `constexpr` pool constants | 328 | emitted at the top of products that use a 6+ digit literal |
| Largest products | `script/for_loop_prepare.cpp` 2,624L · `script/format_double_primary.cpp` 1,744L · `script/number_to_string.cpp` 1,180L · `util/pack_double_fields.cpp` 1,165L | — |

Generator output census, summed from `D:/tmp/sygen2-full2.log` (284 lines), and each figure cross-checks against the
independent line-by-line census taken before the generator existed:

| Stat | Count |
|---|---|
| emitted product lines | 37,907 |
| memory-macro sites rewritten to accessors | 3,377 |
| cross-product call sites rewritten to named callees | 1,036 |
| `CALL_INDIRECT` sites rewritten to `CallTableT*` | 61 |
| host-import sites rewritten to wraps | 3 |
| arithmetic builtin sites rewritten to helpers | 33 |

The closure is **284 functions / 30,787 generated-C lines / 0 missing / transitively closed**: all 283 distinct callees of
the closure are themselves inside the closure. That is the structural reason the sync side needs **no jump boards at all** —
the asyncify tree needed 216 of them, and that is the single biggest difference between the two flavours.

## 2. What makes sync different from asyncify

| Aspect | asyncify | sync | Consequence for the rewrite |
|---|---|---|---|
| `struct w2c_artemis` | has `w2c_g3` / `w2c_g4` | **4 fields only**: `w2c_a_instance`, `w2c_a_a`, `w2c_g0`, `w2c_Ic` (`.ref/artemis-official/wasm/sync/artemis.wasm2c.h:30-35`) | no `ResumePhase` / `ReturnFrameSp` / `kResume*`; `EnterFrame`/`LeaveFrame` are **call-depth bookkeeping only** |
| indirect table | `w2c_Lc` | `w2c_Ic` | `CallTableT0/T2/T5/T7/T58` all index `w2c_Ic` |
| bulk-copy import | `w2c_a_yb` | `w2c_a_wb`, returns `u32` (`.h:767`) | `HostBulkCopy` returns `uint32_t` — returning `void` would have silently swallowed a value |
| frame constants | rewind-segment sizes | none | no `kF…SaveFrameBytes` at all |
| jump boards | 216 inline forwarders | **0** | closure is self-contained |
| host imports in scope | 14 letters | 3 sites (`w2c_a_wb` ×1, `w2c_a_V` ×1, `w2c_a_J` ×1) | 3 wraps: `HostBulkCopy`, `FailSlotLink`, `AssertSlotLinkSucceeded` |

Neither module carries a wasm name custom section, so the original C++ symbol names are unrecoverable; every product name
is authored from measured structure. Pool addresses are **not** reusable across flavours (only 4,630 of 16,648 addresses are
common), so `D:/tmp/strings-sync.tsv` (3,860 rows, CRLF) was built separately from the asyncify `strings.tsv`.

## 3. The three real defects found by the gates

1. **A wrapper that froze a live argument into a constant.** `AssertSlotLinkSucceeded(EngineContext* ctx, uint32_t slot)` hard-coded
   `1u` as the second import argument, but the call site `D:/tmp/syncdefs/f1078.txt:40` is `w2c_a_J(instance->w2c_a_instance, var_i0, var_i1);`
   and `w2c_a_J` is declared `void w2c_a_J(struct w2c_a*, u32, u32)` (`.h:195`). The module therefore compiled into a **different call**
   than the original. Fixed to three parameters in `util/engine_context.h:205-212`. Rule extracted: a wrapper must never pin a call-site
   argument to a literal, because that is a semantic change wearing the costume of a rename.
2. **The memory-macro regex was assembled backwards** (`MEMPREFIX` placed *before* the macro name), so all 3,377 accessor sites
   fell through untouched. It was caught by the generator's own LEFTOVER self-check, not by any later gate — the LEFTOVER token set
   had to include an uppercase-token gate (`\b[A-Z][A-Z0-9]*(?:_[A-Z0-9]+)*\b`, character class **must** include `_` or the
   arithmetic builtins read as zero sites) to see the leak.
3. **`local_types()` was failing on all 284 products.** The declaration parser only accepted single-line initialised declarations,
   so every `f64` / `f32` local would have been emitted as `uint32_t` and silently truncated through a narrow local. That is
   precisely the defect class that black-screened the real game in the asyncify batch-4 investigation, and it is why the
   continuation-aware `split_declarations()` / `parse_declaration()` pair is load-bearing rather than cosmetic.

## 4. Engineering-chain changes (each one a silent-failure guard)

| File | Change | Why it was silently wrong before |
|---|---|---|
| `code-assemble.sh:32-34` | locate the on-disk generated C via `$FLAVOUR` instead of `$RDIR` | `RDIR` is the out/ staging tree name (`plain` for sync) |
| `code-assemble.sh:55` | pass `R=$(cygpath -m "$REWRITE_TREE")` as the 4th python argument | sync assembly judged liveness against the **asyncify** ledger and treated every sync product as unlanded |
| `code-assemble.sh` | `CODE_DIR` / `REWRITE_TREE` configurables, `ARTEMIS_ALLOW_EMPTY_ARCHIVE=1` | one tree per flavour, otherwise you get a quiet wrong answer |
| `build-cxx-route.sh:54-62,131` | `-I "$REWRITE_TREE/src"` | same |
| `code-check.sh:72-73` | `REWRITE_TREE="${ARTEMIS_REWRITE:-…/rewrite}"` | 3rd `-I` was pinned to the asyncify tree |
| `build-bridge-route.sh:80,216,366,420` | one `REWRITE_TREE` for the `rw-splice.py` argument and both `-I` flags | before the fix: `rw-splice: no generated def for f202` (splice reading the asyncify ledger), then 20 errors of the form `no matching function for call to 'w2c_artemis_f219'` (f219 is an asyncify jump board) |

`RDIR="plain"` is the out/ staging tree name and must **not** be renamed.

## 5. Gate results (all four re-run together, `set -o pipefail` inside each chain)

| Gate | Result |
|---|---|
| `style-gate.sh rewrite-sync` | **PASS**, EXIT 0, 284 files in scope, 5 non-fatal pool-literal warnings |
| `code-assemble.sh sync` | EXIT 0 — `284 product(s) spliced`, `cast sites fixed : 195`, `cxx-route.wasm 18,112,652 bytes`, `agreement: 18/18, trap-kind-only: 0, differ: 0`, `built by em++ alone; wabt was not invoked.` |
| `wasm-diff.mjs --strict` | **EXIT 0, 18/18** — 15 equal returns + 3 identical traps (`Yc(3)`, `_c(5)` → `RuntimeError: function signature mismatch`; `Zc(1) = 0`) |
| `build-bridge-route.sh sync` | EXIT 0 — `renamed export '__indirect_function_table' -> 'Ic'`, `OK: sync -> out/bridge-sync/bridge.wasm` |
| splice / bridge-thunk markers in `out/code-sync/artemis.cpp` | 284 / 284 |

No wabt anywhere in this chain; trap text comes from `patch-native-traps.py`, which is why the three trapping exports agree
on *message text* and not merely on trap kind.

## 6. Fidelity verification

`D:/tmp/syverify_f.py` reads each product back, reconstructs the generated-layer statement sequence, and compares it against
`D:/tmp/syncdefs/fNNNN.txt` (the offline cache of the 47 MB generated C, which itself is the ground truth here). Result:
**`checked 284 product(s), 0 mismatch`**.

Two properties make that number worth something:

* the renaming maps come only from the data tables (`sync-params.tsv` for parameter names, the ledger for file and depth),
  never from a regex guess — a wrong rename therefore cannot be normalised away into a false green;
* declarations are stripped from the statement stream and compared **independently as a `(type, bare name)` set**, so a `f64`
  local emitted as `u32` shows up even though the statement text would fold to the same spelling.

`D:/tmp/sycal6.py` is the calibration, and it must pass before the zero is believed: it first asserts the two baseline products
are clean, then plants four known wrongs into two products (a value change, a local widened to the wrong width, a dropped
statement, and a `f64` local narrowed to `u32`) and asserts each is caught, then restores and re-verifies:

```
baseline: f10246 and f905 both 0 mismatch (exit 0)
value change         caught=True
local type widened   caught=True
dropped statement    caught=True
f64 local narrowed   caught=True
calibration passed: 4 planted defects caught, both products restored
```

Reproducibility: a second full generator run concatenated over the whole tree compares **byte-identical** against the first
run snapshot `D:/tmp/sync-snap1/` for all 284 `.cpp` files. The only differing file is `util/engine_context.h`, because defect 1
above was fixed after the snapshot was taken.

## 7. Conventions worth carrying to the next round

* The uppercase-token census regex must include `_` in its character class. It was wrong twice this round: `load8_u` hid the
  narrow macros (12 kinds became 22), and `INFINITY` hid itself until the class was widened to `(?:_[A-Z0-9]+)*`.
* An auditor reporting zero proves nothing until it is shown reporting non-zero for a defect that is really there. This round
  produced four false zeros before the verifier itself was trustworthy.
* When a probe contradicts the artefacts, read the source and the product first. Twice this round a probe was wrong and the
  products were right.
* An auditor that can be fooled by a rename is worse than no auditor: parameter names, label names, helper names and file
  names must come from tables, not from patterns.
* `goto` and labels stay exactly where the generated C put them. 506 label/goto pairs sit at different brace depths, and C++
  cannot have a jump into a scope with a non-trivial initialisation — reordering would not compile and rewriting it as
  `if`/`while` would change the frame accounting.
* Cross-flavour structure (ledger format, `rw-splice.py` contract, `style-gate` ban list, the four-gate chain) is shared; the
  struct layout, the table name, the import letters and the frame semantics are not.

## 8. Product description rewrite (user request m10180)

The user asked for both trees' products to describe themselves as *「基于artemis-compact，并参考了官方的wasm反编译代码进行编写」*.
Two things were settled by asking rather than assuming:

* **The name is `artemis-compat`, not `artemis-compact`.** `.ref/artemis-compat` is the only such directory in the repo and its
  README calls it a *clean-room* Artemis-compatible engine. `artemis-compact` appears exactly once in the whole repo
  (`docs/artemis-web-port.md:1281`) and has no directory behind it.
* **Scope = every product's header comment in both trees, plus both `engine_context.h` headers.** The ledger's `note` column was
  left alone and no tree-level document was invented.

The wording deliberately says *written against* and *with … as the reference*, never *adapted from* / *derived from*.
`ports/artemis/REWRITE.md:61` (was `rewrite/REWRITE.md:61` before the tree moved) forbids fabricated provenance claims, and a claim of derivation is exactly that.

Sync (284 products) went through the generator, so one edit to the head block in `D:/tmp/sygen2.py` changed all 284 at once. It
also removed a factual error the old boilerplate carried: it promised *jump board calls*, but the sync closure is
transitively closed and needs **zero** jump boards.

Asyncify (157 products) could not be edited by text search — their header comments are heterogeneous (99 share one paragraph,
32 another, ~26 are hand-written prose), and the old prose is *accurate* there (asyncify really does have frames, resume code and
a jump board). So the provenance paragraph was **prepended** at the structural seam instead: the end of the leading `//` block,
immediately above the `#include`. Inserters: `D:/tmp/prov-asyncify.py` then `D:/tmp/prov-asyncify2.py` (the second pass only
adds the missing `//` paragraph separator, which the first pass had left out).

### Proof that only comments moved

A pre-change snapshot of all 157 files is at `D:/tmp/asyncify-snap1/`. Comparing every file against it line by line:

| Check | Result |
|---|---|
| Original lines still present, verbatim and in original order | **157 / 157** |
| Total line delta | **785 = 5 × 157** (5 comment lines per file) |
| Files whose exact 5-line provenance block is present | **157 / 157** |
| Files with a doubled separator or a missing separator | 0 |

Sync side: `sygen2.py` re-run ⇒ `emitted 284 product(s), 38191 lines`, 0 LEFTOVER, site census unchanged at
mem 3377 / call 1036 / ind 61 / host 3 / intr 33, and the three-directory concatenation `cmp`s byte-identical to the
pre-comment snapshot. `syverify_f.py` ⇒ `checked 284 product(s), 0 mismatch`; `sycal6.py` ⇒ EXIT 0 with all 4 planted defects caught.

### Gates after the description rewrite — all eight green

| Gate | asyncify | sync |
|---|---|---|
| `style-gate.sh` | PASS | PASS |
| `code-assemble.sh` | 66/66, differ 0, 157 products spliced, wabt not invoked | 18/18, differ 0, 284 products spliced, wabt not invoked |
| `wasm-diff.mjs --strict` | **EXIT 0, 66/66** | **EXIT 0, 18/18** |
| `build-bridge-route.sh` | EXIT 0, table renamed to `Lc` | EXIT 0, table renamed to `Ic` |

Note for the next session: the asyncify baseline is `.ref/artemis/WebAssembly/**asyncify**/artemis.wasm`, not the sync path
`.ref/artemis/WebAssembly/artemis.wasm`. Guessing the latter produces a `FileNotFoundError`, not a wrong answer.

## 9. Still owed

Nothing on the sync side. The asyncify backlog is untouched by this round: the remaining batch-4B product bodies (28,417 source
lines; 3 landed), the f3744 split, the P4 long tail of ~3,474 functions into `generated/`, and a `tyaudit.py` re-run after each
new product lands.
