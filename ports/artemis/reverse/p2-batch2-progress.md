# P2 batch 2 — 5 product functions landed

**Tree location:** the asyncify products these notes count have since moved from
`.ref/artemis-official/rewrite/src` to **`ports/artemis/src`** (tracked source, alongside
`ports/artemis/rewrite-names.tsv` and `ports/artemis/REWRITE.md`).  Paths below are the
ones that were true when each note was written.

Status: all four gates green. `agreement: 66/66, trap-kind-only: 0, differ: 0`, exit 0.

## Products (R1)

| fnid | product | file | lines | ret | frame |
|---|---|---|---|---|---|
| f707 | `artc::CompileExpression` | `script/compile_expression.cpp` | 1750 | void | 68 |
| f1119 | `artc::CheckMatch` | `script/check_match.cpp` | 755 | void | 28 |
| f346 | `artc::CheckStatementLimits` | `script/check_statement_limits.cpp` | 963 | uint32_t | 32 |
| f1125 | `artc::CloseFunctionScope` | `script/close_function_scope.cpp` | 1109 | uint32_t | 32 |
| f4082 | `artc::CleanupPendingResources` | `util/cleanup_pending_resources.cpp` | 1302 | void | 48 |

`rw-splice` count moved 53 → 58.

## Vocabulary boundary (`src/util/engine_context.h`, 302 → 383 lines)

- +20 batch-2 forwarders: ExpectToken (f1115), MatchKeyword (f1713), ParseArguments
  (f1714), EmitMoveSlot (f1745), FormatCharValue (f347), CheckExprComplexity (f427),
  CheckClosingMatch (f547), EmitJumpIf (f5675), ParseParamList (f1118), CompileMainBody
  (f1717), EmitExpression (f5671), EmitCondition (f5672), EmitStatement (f5673),
  ResizeTableArray (f1114), ClearLocalFlags (f1731), ClearUpvalueFlags (f1732),
  InvokeMetaField (f1734), LoadProtoSlot (f782), SetStringSlot (f783),
  StoreGlobalEntry (f310).
- +1 host wrap: `HostDispatchInvoke` for `w2c_a_m` — the generic function-table
  trampoline (`invoke_iiii` in the emscripten JS). f4082 calls it with table slot
  860 as its first argument. Real names land with `host/host_api`.
- −1: the f707 call-through `AppendFunctionCode`; the product takes over the same
  signature and carries its own forward declaration.
- f1114 is now `ResizeTableArray`. `ResizeStringTable` survives as a thin alias so
  `script/intern_string.cpp` (its only caller) needed no edit. The alias must be
  deleted when intern_string takes the new name.
- f244 needed a forwarder too (f1119 and f346 both call it); it reuses the existing
  tsv name `artc::StepRecordResume`, which already has a product in
  `script/step_record_resume.cpp`.

## `rewrite-names.tsv` (136 → 161 lines)

+19 R0 rows for the new forwarders, 1 row rewritten in place (f1114 →
`artc::ResizeTableArray`, PROVISIONAL dropped), +5 R1 rows for the products.

## Generator (`D:/tmp/tx_gen2.py`)

- `PROD['f303']` return corrected to `void` (the generated f303 is `void`).
- Emit-stage constants now carry the fnid prefix (`kF<NNN>Msg*`, `kF<NNN>Const%X`,
  `kF<NNN>SaveFrameBytes`, `kF<NNN>ResumeWordBytes`). All 33 products land in one
  translation unit, so un-prefixed names would collide across anonymous namespaces.
- The `// src:` header now prints the true inclusive span (was off by one at the end).
- `'m': 'HostDispatchInvoke'` added to HOSTMAP.

## Latent bug found and fixed (not in the batch spec)

Regenerating rewrote the 28 batch-1 products too, which exposed **9 stale return
types in the generator tables** — PROD and METAS both said `uint32_t` where the
generated layer is `void`: f204 ReleaseNodeTree, f209 SetStackTop, f303
ClearFlagsRecursive, f896 IndexTableValue, f910 ExpressionToOperand, f1550
UnorderedCheckLoadFactor, f2801 PumpEventQueue, f3784 UnorderedInsert, f4579
AllocateGuardedBuffer. They only surfaced as link errors
("functions that differ only in their return type cannot be overloaded") because
rw-splice's injected prototype block takes its return types from the generated
signatures, which are authoritative. All 96 PROD and all 33 METAS entries are now
audited against `.ref/artemis-official/wasm/asyncify/artemis.wasm2c.c`: 0 mismatches.
**Any future batch that regenerates must re-run that audit.**

A second ordering bug was mine: `ResizeStringTable` called `ResizeTableArray` before
its declaration. Fixed by moving the f1114 forwarder above the alias.

## Note on gate (a)

`code-check.sh` requires at least one fragment path and exits 2 with a usage message
when run bare. It syntax-checks `code/` transcriptions, not the rewrite products
(that tree was `rewrite/src` when this was written; it is `ports/artemis/src` now),
so it has no role in a product-only batch. Gate (c) compiles every product in the
real build, which is the stronger check here.

## Lead verification (not taken on trust)

Re-ran every gate myself after the subagent reported green, and added a
structural fidelity comparison that the subagent did not perform.

Gates, all run in-chain with `set -o pipefail`:

- `style-gate.sh` -> `== style-gate: PASS ==`, exit 0 (4 warnings, bare pool
  addresses at allocate_string_buffer.cpp:37,45,46,47).
- `code-assemble.sh asyncify` -> `rw-splice: 58 product(s) spliced`,
  `differ: 0`, exit 0; artifact out/code-asyncify/cxx-route.wasm 41,794,267
  bytes; `wabt was not invoked`.
- `wasm-diff.mjs --strict` -> `agreement: 66/66, trap-kind-only: 0, differ: 0`,
  exit 0 (19 returned equal values + 47 threw the same trap).
- `build-bridge-route.sh` -> exit 0, export rename
  `__indirect_function_table` -> `Lc` (41,857,743 -> 41,857,720 bytes).

Deep anchors:

- Frame anchor: `real-game-test.mjs` needs `--game DIR --engine FILE`, it takes
  no positional args; a bare run exits 2 with a usage message. The first real
  run failed on the bench, not the candidate: the official build itself
  rendered 0% (lit 0, frames 2, 404 on saveg.dat) and the tool said so itself.
  Re-run on port 4730 with `--wait 60000` and the profiling build ->
  `PASS: the candidate rendered the real game, and its settled frame is
  pixel-identical to the official build's`.
- Count anchor: `worklist-r12.md`, 2105/5579 entered (37.7%). The pinned
  cluster is bit-identical to r11 on calls (f428 294.7K, f393 279.0K,
  f244 218.4K, f1727 218.4K, f340 125.2K, f805 117.6K, f778 108.9K) and
  f206 stays equal to f5508. Every batch 1 and batch 2 product is entered with
  a non-zero count; first-run order moved by a uniform +1, which is a session
  shift, not per-function drift.

Structural fidelity (`/tmp/b2_final2.py`, multiset comparison per file after
the systematic label rename): goto, label, if and return counts match exactly
on all five products. The generated label names map to product names as
`var_B1` -> `spill_frame`, `var_Bfunc` -> `func_exit`, `var_B4` ->
`running_exit`, `var_BN` -> `block_N`, `var_LN` -> `loop_N`. Memory-op counts
halve by design: one `i32_load_default32(...)` becomes one `LoadU32(...)`.

Four bugs in my own comparator, worth keeping as rules:

1. A label regex that assumes column 0 and forgets `var_L*` labels.
2. Comparing generated callee counts against all `artc::` call counts, which
   are not the same population.
3. A false positive from the header prose "the goto graph is the generated
   driver's own flattening" -- strip comments before counting.
4. Special names `var_B1` / `var_Bfunc` swallowed by the generic `var_[BL]`
   branch when the map was ordered wrong.

Compare multisets, not positional sequences. Strip comments first. The rename
map must cover the systematic rule, not only the special cases.

Backup of the landed tree: /tmp/src-backup-batch2.

## Batch 2 close

Five products, twenty forwarders, one host wrapper. Nothing outstanding.
Next is batch 3: f3785, 3228 lines, the first genuinely giant single function
outside the f3744 facade work.
