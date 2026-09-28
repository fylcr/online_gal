# @online-gal/runtime-artemis

Runtime support for the Artemis (舰队Collection / 苍翼) engine bridge.

This package now holds two things: the **PFS reader** that the existing
`src/index.ts` exports, and the **modern host** under `src/host/`, which replaces
the official emscripten glue for the 48 `fd`-bucket imports of
`bridge.wasm`.

## The host, in one paragraph

`bridge.wasm` declares 208 imports: 207 functions and one memory. The
authoritative classification of those 207 is
[`ports/artemis/harness/glue/imports-map.json`](../../ports/artemis/harness/glue/imports-map.md)
(schema `online-gal/artemis-imports-map@1`), which names every short import,
its official `artemis.js` counterpart, and the bucket that decides who
implements it. This package implements the **`fd` bucket — 48 rows over 41
distinct short names** — as ordinary TypeScript on top of `@online-gal/vfs`.
It does not allocate the wasm memory, does not dlopen anything, and never
executes the minified glue.

## Module map

| Module | Lines | What it owns |
| --- | ---: | --- |
| `host/errno.ts` | 117 | The musl errno table, `ArtemisErrnoError`, `ArtemisExit`, `ArtemisHostError`. |
| `host/memory.ts` | 166 | `ArtemisMemory` — typed views re-derived after every `grow`, `assertRange`, UTF-8 helpers. |
| `host/fs.ts` | 340 | `SyncByteSource`, `FsNode`, `ARTEMIS_O`, `MutableByteSource`, `mountTree`. |
| `host/tree.ts` | 741 | `ArtemisFs` — the mount table, open/read/write/seek/close, dup2/dup3, chdir, rename, readdir, stat. |
| `host/layout.ts` | 229 | `writeStat64` (96 B, i64 as lo/hi words), `writeDirent64` (stride 280), `doPreadv` / `doPwritev`. |
| `host/timefmt.ts` | 607 | `struct tm` (44 B), `localtime_r` / `gmtime_r` / `mktime`, `strftime`, `clock_gettime`, `TimeZoneNames`. |
| `host/fd.ts` | 1071 | `createArtemisFdImports(host)` — the 41-key import table. |
| `host/bridge.ts` | 222 | `createArtemisFdTable`, `createBridgeMemory`, `mountArtemisTree`, `entriesFromPfs`, `ARTEMIS_FD_IMPORTS`. |

3,493 lines total, zero runtime dependencies beyond `@online-gal/vfs`.

## Using it

```ts
import {
  createArtemisFdTable,
  createBridgeMemory,
  mountArtemisTree,
} from './host/bridge.js';
import { ArtemisFs } from './host/tree.js';
import { entriesFromPfs } from './host/bridge.js';

const fs = new ArtemisFs();
const memory = createBridgeMemory();            // 3072 pages, max 65536
const archive = new Uint8Array(await readFile('root.pfs'));   // or a byte source

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
ABI while the real persistence work stays where it belongs.

**`Xb` (`_system`) is the only `todo`, and it lives in the `fd` bucket** as a
row that always fails. A browser tab cannot fork. A call must fail loudly
rather than silently pretend.

## Two shape facts the port keeps re-learning

Both are recorded in the source where they bite, and both were found by a
verification failure rather than by reading the glue:

1. **`glueImplOffset` in the spec points at the `function` keyword**, with the
   identifier 9 bytes later — it is the definition site, not the name.
2. **The glue's `intArrayFromString(pattern, false)` keeps the terminator**, so
   `strftime`'s bound test and return value are both shifted by one relative to
   an array of visible bytes. The correct port refuses a string whose *visible*
   length merely equals `maxsize`, and returns the visible count.

## Verification

Three independent surfaces, all reproducible from the repo root:

```bash
npx tsc -b tsconfig.packages.json                  # EXIT=0
node .test/team-artemis/reconcile-fd-imports.mjs   # ALL CHECKS PASS
node .test/team-artemis/verify-fd-real.mjs         # 98 PASS, 0 FAIL
node --test .test/team-artemis/fd-host.test.ts     # 14 tests, 0 failures
```

`reconcile-fd-imports.mjs` proves the 48 spec rows resolve to real exports, that
`WebAssembly.Module.imports` agrees, and that the module **links** with a
stub for every non-`fd` import — a check that can fail, which an earlier version
of it could not (it counted "0 unresolved imports" because V8 reports them in
double quotes and the regex used single).

`verify-fd-real.mjs` drives the 41-key table against the real
`.test/test-games/Artemis/root.pfs` (2,690,454,559 B — larger than 2 GiB, so
`readFileSync` refuses it and the range reader is mandatory) and checks 98
behaviours: a 2.69 GB size that survives the lo/hi `stat64` split exactly,
byte-identical reads of `system/adv/adv.lua`, a resumable `getdents64` cursor,
`strftime` producing `2026-01-01 08:00:00 UTC` with its length matching the
string, and the six rows that return **positive** errno where every other row
returns negative.

`fd-host.test.ts` is a real `node:test` suite, not another bespoke script.
`scripts/run-tests.mjs` runs `node --test ".test/*.test.ts"`, and it **fails
loudly when that glob matches nothing** — a false green would be worse than no
test at all, so a bespoke `.mjs` verifier is not a substitute for a file in that
shape. It pins the 41 keys exactly, the byte-identical read of a real file at
offset 2,688,647,132, the >2 GiB `stat64` size, `SEEK_END`/`SEEK_CUR`, the
`getdents64` cursor, `strftime`, EROFS, ENOTTY, and the `_exit` control flow.

Two API facts that test file had to learn the hard way, both of which cost real
debugging and are worth writing down for the next bucket:

- **`gameSourceByteSource` is async.** `size()` and `readRange()` return
  promises, so `assert.equal(source.size(), n)` compares a pending promise.
- **The real `pf8` container is keyed.** `root.pfs` payloads are XOR-encrypted
  (`PfsArchive.keyed`), so guest bytes must be compared against *decoded* bytes,
  never against the raw file.

All four scripts live under `.test/`, which is gitignored — they are local
verification, not shipped tests.

## Public surface

`src/index.ts` still re-exports only `./sha1.js ./pfs.js ./probe.js
./runtime.js`. `host/` is **not** re-exported. That is deliberate for now: the
host surface is still moving, and widening the package's public API is a
decision for the Lead rather than something to slip in with a feature commit.
Import from `./host/bridge.js` until that changes.
