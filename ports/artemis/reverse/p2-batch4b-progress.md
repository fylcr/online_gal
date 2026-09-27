# Batch 4B closeout: closure forwarders, table slots, host imports, 131 products

**Tree location:** the asyncify products these notes count have since moved from
`.ref/artemis-official/rewrite/src` to **`ports/artemis/src`** (tracked source, alongside
`ports/artemis/rewrite-names.tsv` and `ports/artemis/REWRITE.md`).  Paths below are the
ones that were true when each note was written.


Branch state at the time of this note: `.ref/artemis-official/rewrite/src` = **157 products** + `util/engine_context.h` (771 lines;
774 today).
`rewrite/rewrite-names.tsv` (now `ports/artemis/rewrite-names.tsv`) = 314 fn rows. Generator `D:/tmp/tx_gen2.py` = 280 PROD rows, METAS for 131 items, ORDER 131.

Batch 4B was decided by the user (m07012) as **route A**: write no product prose in this
round; instead make the generator's callee closure resolvable by adding inline forwarders,
so `tx_gen2.py` can run to completion and emit the 131 products itself.

## 1. What route A added

| Kind | Count | Where |
|---|---|---|
| Inline forwarders for unregistered callees | 81 (80 flat + f904 nested) | `util/engine_context.h` |
| New `CallTableT*` helpers for newly hit table slots | 35 (43 slots total) | `util/engine_context.h` |
| New host-import wraps | 4 | `util/engine_context.h` |
| PROD registrations for previously unregistered callees | 81 | `D:/tmp/tx_gen2.py` |
| TABLEMAP slot coverage | 43 | `D:/tmp/tx_gen2.py` |
| HOSTMAP import coverage | 14 | `D:/tmp/tx_gen2.py` |
| NESTED namespace qualification | made load-bearing | `D:/tmp/tx_gen2.py` |

Forwarder naming rule: a forwarder's name must match its PROD entry's first element
**verbatim**, because the generator builds call names from `PROD[fnid][0]`.

## 2. Four host-import names retired and replaced

The two pre-existing wraps `HostRegisterResource` (a_d) and `HostResolveResource` (a_h)
were wrong: they were read off the import letter as if it were a function-table arity.
Measured call sites contradict them, so both were retired.

| Import | asyncify prototype | Measured evidence | Name |
|---|---|---|---|
| `a_d` | `void (…, u32, u32)` | two-slot release, no return, always on an ownership pair | `HostReleaseSlotPair` |
| `a_h` | `u32 (…, u32, u32)` | 2nd arg is a field count (16×176, 36×151, 24×69, 32×45, 28×33, 20×26, 8×25, 4096×13), never an address | `HostFoldLayoutOffset` |
| `a_p` | `void (…, u32)` | region id (1021×120, 1019×104, 3801×20, 3704×2) | `HostMarkRegionSeen` |
| `a_r` | `u32 (…, u32)` | 126 sites all in the `!g3` arm, restores the value stored before the pending-throw store | `HostRestorePendingThrow` |

Prototype source is the **asyncify** header only — the sync build declares the same
letters with different arities (a_d 3 params, a_r returning f32), so sync prototypes must
never be used to write these wraps.

## 3. Defects found and fixed this round

1. **f783 board name stale** — board still `SetStringSlot` after the RENAME to
   `ReshapeTaggedNodeChain`; renamed at `engine_context.h:252`.
2. **f1112 / f2207 board names stale** — `ResizeTable` → `LookupOrCreateTableField` (:114),
   `ReallocateRawBlockCore` → `AllocateTaggedBlock` (:164), with the dependent call sites in
   `script/finish_table_index.cpp`, `util/reallocate_raw_block.cpp`,
   `util/allocate_tagged_block.cpp` and the PROD row for f2207.
3. **f3785 product used the two retired host names** — 1 forward declaration + 7 call sites
   in `util/dispatch_resource_step.cpp` rewritten to the new names.
4. **Two host wraps referenced an undeclared identifier** — the bodies passed `a` but the
   parameter is `count`; em++ reported `use of undeclared identifier 'a'` at header:404
   and :413.
5. **Three boards defined names that already had products** — f1112 / f2207 / f783
   produced `redefinition of 'X'`. Boards deleted once the products were confirmed on disk
   by grep of their fnid stamp.
6. **`NESTED` was dead code** — defined at `tx_gen2.py:309` and never referenced, so f904's
   call was emitted unqualified (`i0 = Call(ctx, …)`) while its forwarder lives inside
   `namespace LuaEngine`, giving `undefined symbol: artc::Call(...)`. `crep` now qualifies a
   call only when the PROD name is unqualified, which leaves the already-qualified f3551 and
   f1042 names untouched.
7. **`D:/tmp/tyaudit.py` audited a stale tsv** — it read `names-live-b4.tsv`, so it reported
   128 products while 157 were on disk. Repointed at the live tsv; it now audits 204 rows
   and finds 0 wrong-type locals.

## 4. Gate results

```
style-gate      PASS        5 pre-existing bare-pool-address warnings (named constants)
code-assemble   exit 0      63 fragments, 5562 defs kept
                           rw-splice: 157 product(s) spliced
                           differ: 0
                           cxx-route.wasm 41,744,792 bytes
                           built by em++ alone; wabt was not invoked
wasm-diff       exit 0      agreement 66/66, trap-kind-only: 0, differ: 0
                           19 equal values, 47 identical traps
build-bridge    exit 0      bridge.wasm 41,757,381 -> 41,757,358 bytes
```

Registry audits:

```
b4b_prodcheck   280 PROD rows, all agree with the generated signatures
b4b_crosscheck  CLEAN (0 problems); 50 tsv names still without PROD (forwarder-only)
tyaudit         204 rows audited, 0 wrong-type local declarations
tx_gen2.py      exit 0, 131 EMIT, 0 LEFTOVER, byte-stable on re-run
```

## 5. Real-game frame anchor

Bench: `--game .test/artemis-official-async/game`. Note that `.test/inifix` only holds
`system.ini` and makes even the **official** build fail to render, so nothing is established
there.

| Field | Official | Candidate run 1 | run 2 | run 3 |
|---|---|---|---|---|
| lit | 97.11 | 97.11 | 97.11 | 97.11 |
| frames | 3 | 3 | 3 | 3 |
| gameRequests | 87 | 87 | 87 | 87 |
| canvas | 2560x1398 | 2560x1398 | 2560x1398 | 2560x1398 |
| finalHash | 550f7464 | 550f7464 | 550f7464 | 550f7464 |
| calledRun / trapCode | True / None | True / None | True / None | True / None |
| mean | 246.95 | 246.71 | 247.61 | 246.75 |
| pixelIdentical | — | False | **True** | **True** |

`mean` is the only field that moves between runs, on the official side as well, so it is
timing noise and not a criterion. Run 1's `pixelIdentical: False` is that same noise: the
two following runs are pixel-identical and every behavioural field is constant.

The hash `550f7464` matches the batch-3/batch-4 good signature, but per the standing rule
the hash is never itself the criterion — request count, lit fraction and frame count are.

## 6. Standing lessons

- A forwarder name must equal the PROD first element verbatim; a rename on either side alone
  produces `undefined symbol` or, worse, a stale board that shadows the real product.
- A forwarder must be deleted the moment its product lands, otherwise the product and the
  board collide as `redefinition of`.
- Never read a host-import's meaning from its import letter. Read it from measured call
  sites, and never mix sync-build prototypes into an asyncify build.
- Never point an audit script at a snapshot tsv. A green audit over a stale registry is
  worse than no audit, because it reports a smaller population than exists.
- `NESTED` in the generator existed but was never wired into the call-name rewrite, so a
  declared feature produced unqualified calls. Check that a registry is actually read, not
  merely present.
