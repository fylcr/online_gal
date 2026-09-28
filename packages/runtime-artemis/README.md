# @online-gal/runtime-artemis

Runtime support for the Artemis (舰队Collection / 苍翼) engine bridge.

This package holds the **PFS reader** that the existing `src/index.ts` exports,
the **modern host** under `src/host/` that replaces the official emscripten glue
for the `fd` bucket, and — since the `runtime` / `eh` / `asyncify` buckets — the
first host that **boots `bridge.wasm` with no official glue at all**.

## The host, in one paragraph

`bridge.wasm` declares 208 imports: 207 functions and one memory. The
authoritative classification of those 207 is
[`ports/artemis/harness/glue/imports-map.json`](../../ports/artemis/harness/glue/imports-map.md)
(schema `online-gal/artemis-imports-map@1`), which names every short import,
its official `artemis.js` counterpart, and the bucket that decides who
implements it. Buckets land here in the order the port claims them: **`fd`
(48 rows / 41 short names)** on top of `@online-gal/vfs`, then **`runtime` (8),
`eh` (17 / 15) and `asyncify` (4)**, then `boot.ts`, which assembles all of them
with a counted stub for every row that is still somebody else's work. The host
does not allocate the wasm memory, does not dlopen anything, and never executes
the minified glue.

## Module map

| Module | What it owns |
| --- | --- |
| `host/errno.ts` | The musl errno table, `ArtemisErrnoError`, `ArtemisExit`, `ArtemisHostError`. |
| `host/memory.ts` | `ArtemisMemory` — typed views re-derived after every `grow`, `assertRange`, UTF-8 helpers. |
| `host/fs.ts` | `SyncByteSource`, `FsNode`, `ARTEMIS_O`, `MutableByteSource`, `mountTree`. |
| `host/tree.ts` | `ArtemisFs` — the mount table, open/read/write/seek/close, dup2/dup3, chdir, rename, readdir, stat. |
| `host/layout.ts` | `writeStat64` (96 B, i64 as lo/hi words), `writeDirent64` (stride 280), `doPreadv` / `doPwritev`. |
| `host/timefmt.ts` | `struct tm` (44 B), `localtime_r` / `gmtime_r` / `mktime`, `strftime`, `clock_gettime`, `TimeZoneNames`. |
| `host/fd.ts` | `createArtemisFdImports(host)` — the 41-key import table. |
| `host/bridge.ts` | `createArtemisFdTable`, `createBridgeMemory`, `mountArtemisTree`, `entriesFromPfs`, `ARTEMIS_FD_IMPORTS`. |
| `host/asyncify.ts` | `createArtemisAsyncifyImports(host)` — 4 keys: the `wget_data` family and `memcpy_big`. |
| `host/runtime.ts` | `createArtemisRuntimeImports(host)` — 8 keys: `TempRet0`, `asm_const_int`, `run_script_string`, `resize_heap`, the mutexattr trio. `ArtemisTempRet0` lives here and is **shared with `eh`**. |
| `host/eh.ts` | `createArtemisEhImports(host)` — 15 keys over 17 rows: the C++ exception ABI (`allocate_exception` … `rethrow`) plus `assert_fail`, `longjmp`, `_abort` / `_Unwind_RaiseException`. Raises `ArtemisCppThrow` / `ArtemisLongjmp` / `ArtemisExit`. |
| `host/boot.ts` | `createArtemisBootTable(setup)` — merges the four real buckets, stubs the rest, and returns `{ imports, trace, temp, attachExports }`. `formatBootTrace` renders the per-bucket measurement. |

Zero runtime dependencies beyond `@online-gal/vfs`.

## Booting `bridge.wasm` with no glue

```ts
import { createArtemisBootTable } from './host/boot.js';
import { ArtemisMemory } from './host/memory.js';
import { ArtemisFs } from './host/tree.js';

const wasmMemory = new WebAssembly.Memory({ initial: 3072, maximum: 65536 });
const memory = new ArtemisMemory({ memory: wasmMemory, label: 'HEAP' });
const boot = createArtemisBootTable({ spec, memory, fs: new ArtemisFs() });

const instance = new WebAssembly.Instance(module, { a: { ...boot.imports, a: wasmMemory } });
boot.attachExports(instance);
```

Five things about that sequence are not guessable and each cost real debugging:

1. **The constructor pass is the export `Mc`, not `__wasm_call_ctors`.** The
   export literally named `__wasm_call_ctors` is **inert** — it makes zero
   import calls. `Mc` is the real pass: five calls, `Fb Eb Bb Ab Aa`.
2. **`attachExports` resolves the SHORT names `$c` and `ad`.** Those are the
   `___cxa_can_catch` and `___cxa_is_pointer_type` *exports*; the long names are
   the *imports* and are absent from the export list. `eh.ts` keeps
   `canCatch` / `isPointerType` mutable precisely because they back wasm
   exports.
3. **One `WebAssembly.Memory`, wrapped once.** The memory *import* demands the
   raw object; the wrapper's views come from the same object. Constructing two
   buffers for one memory detaches them from each other on grow.
4. **Overrides installed after `new WebAssembly.Instance` reach nothing.** V8
   captures import functions at instantiate time. An override patched in
   afterwards silently never runs — which reads as "the trampoline dispatched 0".
5. **The `async` bucket is a table dispatch, not 30 call sites.** Every
   `invoke_*` glue body is the same five lines (`stackSave` / `dynCall_*` /
   `stackRestore`-on-throw / re-raise-unless-integer-or-longjmp / `_setThrew`).
   Guest table export `Lc` holds 3613 entries and the element section is a bare
   `funcidx` vector with base 1, so the table's own slots, not the body, decide
   where a trampoline lands.

### Where it stops, and why that is honest

`_main` (`Qc`) is reached and then traps:

```
_main(10224) → w2c_artemis_Qc(656) → main(660) → wasm_rt_trap(10286)
             → __assert_fail(10313) → abort(10369) → _abort_js(10315, unreachable)
```

**`wasm_rt_trap` is called by `main` directly**, and the firing site is main's
**site 2, insn 492 @560873, `i32.const 5`** — trap code 5, "Unreachable
instruction executed", which is a per-argument **type-tag check** in the guest's
own generated code. (Main's site 1 is the 500-frame depth guard and cannot fire:
main entered once and the cell read 0 at entry, against a max of 4 at import
boundaries. Function 657 is a 91 B wrapper with **zero** trap sites.)

This is not a host failure, and it is not stub-return values. A pass in which
**every** stubbed name threw its own name instead of answering 0 reached the
first trampoline and trapped *inside* it, with no further import calls at all.
Table slot 2011 resolves to function 5326, whose direct-call graph is 31
functions with zero self-calls and zero cycles, so there is no runaway loop
either. bridge.wasm is stripped (no `name` section), so the diagnosis is a code
-section walk, not a symbol lookup.

Four of the guest's own 14 `__syscall_<name>` gaps are worth naming here: the
guest prints `warning: unsupported syscall: __syscall_<name>` and **continues**,
so these are the concrete syscalls this port has no route for —
`mprotect`, `acct`, `socketpair`, `mremap`, `munlockall`, `mlockall`, `munlock`,
`mlock`, `madvise`, `mincore`, `getrusage`, `wait4`, `prlimit64`, `getresgid32`.

## The per-bucket measurement

`formatBootTrace(boot.trace)` is the deliverable the Lead asked for — a table of
short name → call count per bucket, so "did the guest actually reach our code" is
a measurement rather than a hope. From the current run:

| bucket | calls | short names | called |
| --- | ---: | ---: | ---: |
| `async` | 7 | 30 | 6 |
| `asyncify` | 0 | 4 | 0 |
| `eh` | 0 | 15 | 0 |
| `fd` | 2 | 41 | 2 |
| `gl` | 0 | 99 | 0 |
| `runtime` | 3 | 8 | 3 |
| `todo` | 0 | 1 | 0 |

The seven `async` calls are the measurement, not a failure: the guest asked for
seven Asyncify re-entries (`invoke_iii` twice, then `invoke_vi`, `invoke_ii`,
`invoke_iiii`, `invoke_viii`, `invoke_viiii`) and tripped its type-tag check
right after. That bucket is not this task's work, and a stub that *counts* is the
honest state of the port — a silent one would hide it. The three `runtime` calls
are `Aa` / `Ab` / `Bb` from the `Mc` constructor pass.

`counted()` increments the counter **before** the body runs, so a row that
unwinds (`Ta` aborts, `I` longjmps, `q` fails an assert) still appears in the
table instead of vanishing into the exception.

## Using the `fd` bucket on its own

```ts
import {
  createArtemisFdTable,
  createBridgeMemory,
  mountArtemisTree,
} from './host/bridge.js';
import { ArtemisFs } from './host/tree.js';

const fs = new ArtemisFs();
const memory = createBridgeMemory();            // 3072 pages, max 65536

mountArtemisTree(fs, '/game', entriesFromPfs(gameSource, 'root.pfs'));
const imports = createArtemisFdTable({ memory, fs, malloc, errnoLocation });

const { instance } = await WebAssembly.instantiate(bytes, { a: imports });
```

**Memory is injected, never allocated for you.** `createBridgeMemory()` is a
convenience; a test can pass a two-page `createArtemisMemory(2)` instead. The
bridge declares `initial: 3072` / `maximum: 65536` pages, but nothing in this
package requires you to hand it 192 MiB up front.

**A wasm `i32` arrives in JavaScript as a signed number.** A pointer above
2^31 comes through negative, so every argument-derived index inside `fd.ts` is
`| 0`-coerced at the point of use. This is the single most common source of
"works until the heap grows past 2 GiB" bugs and it is not optional.

## Design decisions worth knowing

These are judgement calls, not derivations. Each one is argued in the source
at the site it affects.

**41 export keys, not 48.** Seven of the 48 rows repeat a name already
present — `Ea Ub Q Ob Jb V xb` — under a second type index. wasm links by
module and name, not by type, so one implementation satisfies both entries. The
two type indices differ in how the *caller* marshals arguments, not in what the
body does. A 48-key table would be impossible to build correctly anyway, because
JavaScript objects cannot carry two functions under one key.

**`Ta` answers both `_abort` and `_Unwind_RaiseException`.** Same shape: the
module lists the name twice, both rows carry `official: "_abort"`, and the
second identity survives only in the `duplicate` and `alias` fields. One
implementation, `throw new ArtemisExit(1, …)`, serves both — an unwind out of a
module compiled `-fno-exceptions` has no other correct behaviour.

**The `TempRet0` cell is shared between `runtime` and `eh`.** The glue's
`___cxa_find_matching_catch_*` publishes the *matched type* into that cell, so
the two tables must read and write one `ArtemisTempRet0` instance. Constructing
one per table is the bug this shared class exists to prevent.

**`_exit` throws `ArtemisExit`; it does not call `exit()`.** The glue's
`exit()` unwinds the emscripten runtime. There is no equivalent here, and a
status code returned as an `int` would be indistinguishable from a syscall
result. `ArtemisExit` is a control-flow signal, distinguishable from
`ArtemisHostError`, which is a real failure the caller must not swallow.

**`___sys__newselect` never sleeps on `timeout`.** It copies the glue exactly:
`DEFAULT_POLLMASK` is literally `5`, which is `POLLIN | POLLOUT` and does *not*
contain `POLLPRI` (`5 & 2 === 0`) — so an fd named only in `exceptfds` never
comes back set. The timeout is read and then ignored, because blocking the
frame thread is worse than the semantic gap, and because the recorded glue
behaviour is the contract. The doc comment on the `Ib` body carries the mask
arithmetic so the next reader does not "fix" it.

**`___sys_ioctl` returns `ENOTTY` where the glue calls `abort()`.** The glue's
default arm is `abort("bad ioctl syscall " + op)`. A browser host has no
stderr and no business killing the process because a game asked for a
terminal ioctl it will never get.

**The eight `_emscripten_idb_*` rows get a synchronous in-memory store.** They
are required imports: omitting them makes instantiation fail outright. They
belong to the `asyncify` bucket, so what this host supplies is a
`WritableStore`-shaped `Map` — honest, synchronous, and enough to satisfy the
ABI while the real persistence work stays where it belongs. **The `async` slice
must replace this with a real persistent store**; the idb rows are that bucket's
semantics and are only parked here to make instantiation possible.

**`Xb` (`_system`) is the only `todo`, and it lives in the `fd` bucket** as a
row that always fails. A browser tab cannot fork. A call must fail loudly
rather than silently pretend — so it writes `ENOSYS` (52) into the guest's
`errno` slot via `___errno_location`, reports, and returns `-1`. A host that
returned 0 here would fork.

**`new CatchInfo` does not push onto the caught stack.** Only
`___cxa_begin_catch` and `___cxa_rethrow` do (two sites in the glue). The
port was right and the first draft of the test was wrong: `find_matching_catch`
lends a catch-info block to the `begin_catch`/`end_catch` pair, it does not own
one.

## Two shape facts the port keeps re-learning

Both are recorded in the source where they bite, and both were found by a
verification failure rather than by reading the glue:

1. **`glueImplOffset` in the spec points at the `function` keyword**, with the
   identifier 9 bytes later — it is the definition site, not the name.
2. **The glue's `intArrayFromString(pattern, false)` keeps the terminator**, so
   `strftime`'s bound test and return value are both shifted by one relative to
   an array of visible bytes. The correct port refuses a string whose *visible*
   length merely equals `maxsize`, and returns the visible count.

And one from the second half of the port: **a bootstrap allocator of `() => 0`
is not a neutral stub.** It makes every allocation-dependent row report 0 bytes
and every errno assertion read the heap base, which reads as a host bug rather
than a missing allocator. The harnesses here bump-allocate for real.

## Verification

Every gate below is reproducible from the repo root, and all of them are green:

```bash
npx tsc -b tsconfig.packages.json                  # EXIT=0
node scripts/check-layout.mjs                      # layout ok
node .test/team-artemis/reconcile-fd-imports.mjs   # ALL CHECKS PASS
node .test/team-artemis/verify-fd-real.mjs         # 98 PASS, 0 FAIL
node --test .test/team-artemis/fd-host.test.ts     # 14 tests, 0 failures
node .test/team-artemis/reconcile-t17-imports.mjs  # ALL 51 CHECKS PASS
node --test .test/team-artemis/t17-host.test.ts     # 46 tests, 0 failures
node .test/team-artemis/boot-bridge.mjs            # the per-bucket table
```

`reconcile-t17-imports.mjs` proves the 29 task-17 rows are 27 short names with
exactly `I` and `Ta` doubled, that each compiled factory's `Object.keys` equals
its spec names (4 / 8 / 15), that the merged table is 27 and all 29 rows resolve,
that `Ta` is one callable answering both `()` and `(1)`, that bridge.wasm's
sha256 matches `spec.totals.moduleSha256`, and that the module **links** with
`exports 1163`.

`t17-host.test.ts` is seven suites: the spec rows, then asyncify / runtime / eh
semantics, then the boot table, then the real bridge.wasm, and finally a suite
that puts the three tables in front of the **real package** — `check.tbl` read
byte-identically through the guest `fd` ABI, `root.pfs` stat'd at 2,690,454,559 B,
`adv.lua` (19,399 B) served to the guest by the asyncify resolver, and
`system.ini` (3,618 B) moved by `memcpy_big` with every `eh` row asserted not to
have raised.

The size assertion there is worth spelling out, because 2,690,454,559 B is past
2 GiB but **under 2³²**: the `stat64` lo word is `-1604512737` as an i32 and only
reads back as 2,690,454,559 when it is read **unsigned**, while the hi word is
legitimately 0. A sibling case mounts a source that merely *reports* 5 GiB + 7 to
reach the second word, so the pair is proven rather than assumed.

`verify-fd-real.mjs` drives the 41-key table against the real
`.test/test-games/Artemis/root.pfs` (2,690,454,559 B — larger than 2 GiB, so
`readFileSync` refuses it and the range reader is mandatory) and checks 98
behaviours: a 2.69 GB size that survives the lo/hi `stat64` split exactly,
byte-identical reads of `system/adv/adv.lua`, a resumable `getdents64` cursor,
`strftime` producing `2026-01-01 08:00:00 UTC` with its length matching the
string, and the six rows that return **positive** errno where every other row
returns negative.

`fd-host.test.ts` and `t17-host.test.ts` are real `node:test` suites, not
another bespoke script. `scripts/run-tests.mjs` runs `node --test
".test/*.test.ts"`, and it **fails loudly when that glob matches nothing** — a
false green would be worse than no test at all, so a bespoke `.mjs` verifier is
not a substitute for a file in that shape.

Two API facts those test files had to learn the hard way, both of which cost
real debugging and are worth writing down for the next bucket:

- **`gameSourceByteSource` is async.** `size()` and `readRange()` return
  promises, so `assert.equal(source.size(), n)` compares a pending promise.
- **The real `pf8` container is keyed.** `root.pfs` payloads are XOR-encrypted
  (`PfsArchive.keyed`), so guest bytes must be compared against *decoded* bytes,
  never against the raw file.

All these scripts live under `.test/`, which is gitignored — they are local
verification, not shipped tests.

## Public surface

`src/index.ts` still re-exports only `./sha1.js ./pfs.js ./probe.js
./runtime.js`. `host/` is **not** re-exported. That is deliberate and settled:
the public surface is added only once the host can do one complete thing, and
`bridge.ts` still stubs 159 of the 207 imports. Import from `./host/bridge.js`
or `./host/boot.js` until that changes.
