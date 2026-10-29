# @online-gal/engine-core-artemis-compat

Recognition, PFS utilities, and modern browser-host work for the Artemis asyncify bridge.

## Current support boundary

- [`src/index.ts`](src/index.ts) exports SHA-1, PFS, probe and provider code, not a complete host session.
- [`ArtemisRuntime.load()`](src/runtime.ts) still throws `UnsupportedEngineError`.
  Registering an `ArtemisEngineHost` does not implement that missing path; provider availability
  is evaluated when the provider object is created, not as a live getter.
- [`src/host/`](src/host/) contains an independently written host and boot/import-table components.
  Boot traces and synthetic tests below are historical diagnostics, **not real-game acceptance**.
- The shipping target is **asyncify handwritten C++ engine + independent modern host only**.
  Official SDK/minified glue is retained for reference/diagnostics, not as a shipping host.
- CI includes its `bridge.wasm` in this private package's `assets/`, but neither those bytes nor
  a return from guest `main` makes the public runtime ready to play. Host/session integration,
  handwritten cold-game acceptance and downloaded GitHub-artifact game verification remain open.
- Root GPL-3.0-or-later does not grant redistribution rights to the retained SDK or automatically
  settle the provenance of SDK-derived products. See [`THIRD_PARTY_NOTICES.md`](../../THIRD_PARTY_NOTICES.md).

Source and build navigation: [`Artemis guide`](../../ports/artemis/README.md) and
[`source layout`](../../docs/source-layout.md). The detailed measurements below are retained as
historical host-development records; they must not be read as a current end-to-end usability claim.

## Historical host implementation snapshot

The import counts and startup examples in this section describe the earlier diagnostic module,
not the ABI of every newly built bridge. The calibrated asyncify bridge used in the later cold
probe has **207 imports: 206 functions and one table**, not imported memory. Inspect the actual
artifact before adapting the examples; these notes do not establish a usable public session.

### The host, in one paragraph

`bridge.wasm` declares 208 imports: 207 functions and one memory. The
authoritative classification of those 207 is
[`ports/artemis/harness/glue/imports-map.json`](../../ports/artemis/harness/glue/imports-map.md)
(schema `online-gal/artemis-imports-map@1`), which names every short import,
its official `artemis.js` counterpart, and the bucket that decides who
implements it. Buckets land here in the order the port claims them: **`fd`
(48 rows / 41 short names)** on top of `@online-gal/vfs`, then **`runtime` (8),
`eh` (17 / 15), `asyncify` (4) and `async` (30)**, then **`gl` (99)**, then
`boot.ts`, which assembles all of them with a counted stub for every row that
is still somebody else's work. That is **197 distinct short names covering
206 of the 207 rows**; the only row left is `todo` (1) — `_system`. Of the 99
`gl` rows, 79 are real (the WebGL2 context, `glfw`, and the 11 event
registrations) and 20 are deliberate, documented refusals that still answer
their own per-call value: 19 audio rows and
`_emscripten_enter_soft_fullscreen`. The host
does not allocate the wasm memory, does not dlopen anything, and never executes
the minified glue.

## Module map

| Module | What it owns |
| --- | --- |
| `host/errno.ts` | The musl errno table, `ArtemisErrnoError`, `ArtemisExit`, `ArtemisHostError`. |
| `host/memory.ts` | `ArtemisMemory` — typed views re-derived after every `grow`, `assertRange`, UTF-8 helpers. |
| `host/fs.ts` | `SyncByteSource`, `FsNode`, `ARTEMIS_O`, `MutableByteSource`, `mountTree`. |
| `host/tree.ts` | `ArtemisFs` — the mount table, open/read/write/seek/close, dup2/dup3, chdir, rename, readdir, stat. |
| `host/layout.ts` | `writeStat64` (96 B, i64 as lo/hi words), `writeDirent64` (stride 280), `doReadv` / `doWritev` (advancing, as the glue is). |
| `host/timefmt.ts` | `struct tm` (44 B), `localtime_r` / `gmtime_r` / `mktime`, `strftime`, `clock_gettime`, `TimeZoneNames`. |
| `host/fd.ts` | `createArtemisFdImports(host)` — the 41-key import table. |
| `host/bridge.ts` | `createArtemisFdTable`, `createBridgeMemory`, `mountArtemisTree`, `entriesFromPfs`, `initArtemisStandardStreams`, `ARTEMIS_FD_IMPORTS`. |
| `host/asyncify.ts` | `createArtemisAsyncifyImports(host)` — 4 keys: the `wget_data` family and `memcpy_big`. |
| `host/runtime.ts` | `createArtemisRuntimeImports(host)` — 8 keys: `TempRet0`, `asm_const_int`, `run_script_string`, `resize_heap`, the mutexattr trio. `ArtemisTempRet0` lives here and is **shared with `eh`**. |
| `host/eh.ts` | `createArtemisEhImports(host)` — 15 keys over 17 rows: the C++ exception ABI (`allocate_exception` … `rethrow`) plus `assert_fail`, `longjmp`, `_abort` / `_Unwind_RaiseException`. Raises `ArtemisCppThrow` / `ArtemisLongjmp` / `ArtemisExit`. |
| `host/async.ts` | `createArtemisAsyncImports(host)` — the 30 `invoke_*` re-entry trampolines, generated from one factory. `ARTEMIS_ASYNC_FORWARDS` / `ARTEMIS_ASYNC_HELPERS` name the guest exports they dispatch through. |
| `host/boot.ts` | `createArtemisBootTable(setup)` — merges the five real buckets, stubs the rest, and returns `{ imports, trace, temp, attachExports, boot }`. `boot(instance, args)` runs the official startup sequence; `formatBootTrace` renders the per-bucket measurement. |

Zero runtime dependencies beyond `@online-gal/vfs`.

## Booting `bridge.wasm` with no glue

The control this is measured against is the official glue's own startup
sequence, read out of `ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js`
rather than assumed:

```
preRun([])
  initRuntime()      FS.init()  →  createStandardStreams
                     TTY.init() →  (a no-op)
                     SOCKFS.root = FS.mount(SOCKFS, {}, null)
                     callRuntimeCallbacks(__ATINIT__)
  preMain()          FS.ignorePermissions = false
                     callRuntimeCallbacks(__ATMAIN__)
  callMain(args)     argv built on the guest's own stackAlloc; _main(argc, argv)
  postRun([])
```

Four things about that sequence are not guessable and each cost real debugging:

1. **The constructor pass is the export `Mc`, not `__wasm_call_ctors`.** The
   export literally named `__wasm_call_ctors` is **inert** — it makes zero
   import calls. `Mc` is the real pass: five calls, `Fb Eb Bb Ab Aa`. It also
   runs *inside* `initRuntime`, because it is the one member of `__ATINIT__` —
   not before it, which is the order you would write by hand.
   `__ATMAIN__` is **empty**, so nothing runs between it and `main`.
2. **`attachExports` resolves the SHORT names.** `$c` and `ad` are the
   `___cxa_can_catch` and `___cxa_is_pointer_type` *exports*; the long names are
   the *imports* and are absent from the export list, so `eh.ts` keeps
   `canCatch` / `isPointerType` mutable. The `async` bucket resolves `Xc`
   (`stackSave`), `Yc` (`stackRestore`), `_c` (`_setThrew`) and the 30 dynCalls
   the same way.
3. **One `WebAssembly.Memory`, wrapped once.** The memory *import* demands the
   raw object; the wrapper's views come from the same object. Constructing two
   buffers for one memory detaches them from each other on grow.
4. **Overrides installed after `new WebAssembly.Instance` reach nothing.** V8
   captures import functions at instantiate time. An override patched in
   afterwards silently never runs — which reads as "the trampoline dispatched 0".

```ts
import { createArtemisBootTable } from './host/boot.js';
import { ArtemisMemory } from './host/memory.js';
import { ArtemisFs } from './host/tree.js';

const wasmMemory = new WebAssembly.Memory({ initial: 3072, maximum: 65536 });
const memory = new ArtemisMemory({ memory: wasmMemory, label: 'HEAP' });
const boot = createArtemisBootTable({ spec, memory, fs: new ArtemisFs() });

const instance = new WebAssembly.Instance(module, { a: { ...boot.imports, a: wasmMemory } });
boot.attachExports(instance);
const result = boot.boot(instance, []);   // the sequence above
console.log(result.returned);             // -1 in the real package, with no gl bucket
```

`boot()` builds argv the way `callMain` does, on the guest's own `Zc`
`stackAlloc`: `argc + 1` slots, `argv[0] = thisProgram` (`"./this.program"`),
`argv[argc] = 0`. Nothing here is emscripten's own allocator.

One deliberate omission: `SOCKFS`. The glue mounts a socket filesystem at the
root, but no row in the spec reaches it — `O` (`___sys_socketcall`) is the only
socket seam, and it is the `fd` bucket's business. Mounting a filesystem nothing
can reach would be a decoration, not a port.

### The `async` bucket is 30 forwarders, not 30 call sites

Every `invoke_*` glue body is the same five lines — `stackSave` / `dynCall_*` /
`stackRestore`-on-throw / re-raise-unless-integer-or-`"longjmp"` / `_setThrew(1,0)`
— and the `dynCall_*` it calls is a **named export forwarder**: the glue writes
`Module["dynCall_iii"] = Module["asm"]["cd"]`, and `cd` is an export of the
bridge. So the host hands the arguments to the guest's own dispatcher, which
does the table lookup, the argument marshalling and the `__THREW__` bookkeeping
inside the module. Nothing here reimplements a guest function table, and no
`Lc` access is needed.

The mapping is not derivable and is recorded in `ARTEMIS_ASYNC_FORWARDS`
(`Ba→Nd Ca→Md D→md P→Od Ra→qd … x→xd y→pd`, 30 rows, 1:1). **Arity does not
identify the target** — none of the 30 has a same-arity export — and the wasm
type barely does (only `Ca→Md`, `ba→Pd`, `rc→Gd`, `t→td`, `tc→Ed` are unique).
The glue has to be read, which is why the table is a committed artefact rather
than something re-derived at boot.

### Where it stops now: the `gl` bucket, which is honest

`main` is reached and **returns `-1`**. It does not trap, abort, or throw: the
trampolines dispatch into the guest, the guest parses its own configuration, sets
up its save directory, asks for a window, and declines to continue without one.
The last import of the run is `oc = _glfwInit ()` returning 0 — which is
glfw's own success value, so the host is not lying to the guest; the guest is
deciding for itself that there is no display.

Measured on the real package (`root.pfs` mounted lazily at `/`, 2.69 GB):

| step | what the guest did |
| --- | --- |
| `_environ_sizes_get`, `_environ_get "USER=web_user"` | environment block |
| `Mc` | 5 constructor-pass calls (`Fb Eb Bb Ab Aa`) |
| `___sys_open "/root.pfs"` → fd 6, read, close | mounted package |
| `___sys_open "system.ini"` → fd 7, `_fd_seek` ×5, `_fd_read` | reads the **real 3,618 B** `system.ini` |
| `_emscripten_memcpy_big` ×2 | moves those 3,618 bytes |
| `___sys_mkdir "/savedata_sync"`, `open "dummy"`, `unlink`, `chdir` | sets up its own save dir |
| `___sys_ioctl(6, 21523)` → `-59` (`ENOTTY`) | asks the save fd for terminal attributes |
| `_glfwInit ()` → 0, then **`return -1`** | no display, so no game |

Running the identical sequence with an **empty** package (no archive mounted)
produces the same 16 calls and the same `-1` — the exits are the guest's own
"no display" decision in both cases, not a value the host supplied. What the
real package adds is everything between: 1,175 dispatched `async` re-entries
instead of 16, and a `system.ini` read out of a real archive.

What is left is the audio seam, and it is honest about it: 19 of the 99 `gl`
rows are `_al*`/`_alc*` calls with no `AudioContext` in this host, and each one
answers OpenAL's own "no device, no context" value rather than a blanket 0
(`_alGetError` returns `AL_INVALID_OPERATION`, 40964, because that is what
OpenAL reports with no current context). Four of
the guest's own 14 `__syscall_<name>` gaps are worth naming here as well — the
guest prints `warning: unsupported syscall: __syscall_<name>` and **continues**,
so these are the concrete syscalls this port has no route for: `mprotect`,
`acct`, `socketpair`, `mremap`, `munlockall`, `mlockall`, `munlock`, `mlock`,
`madvise`, `mincore`, `getrusage`, `wait4`, `prlimit64`, `getresgid32`.

### The control: what the official path actually does

Everything above is a claim about **our** host, so the only way to believe it
is to ask the official path the same questions. The control is the official
module under the official glue in a real browser, serving
`.test/artemis-official-async/deploy-ctrl` (the probe is
`.test/team-artemis/a3-ctrl.mjs`; it is gitignored with the rest of `.test/`).

It patches **no import**. The channel is page-side: the glue's own `callMain`
does `var entryFunction = Module["_main"]` — a property lookup at call time, not
a captured binding — so wrapping `Module._main` from the page intercepts the
real call with the real `argc`/`argv` and the real outcome. The wrapper has to be
armed on a timer, because the glue publishes `_main` only together with the wasm
instance, hundreds of milliseconds after the page's own 8-key `Module` object
appears, and enters `main` a millisecond or two later.

| question | official | ours |
| --- | --- | --- |
| `argc` / `argv` | `1` / `["./this.program"]` | same |
| `thisProgram` | `"./this.program"` | same |
| callbacks after `__ATMAIN__` | none — `__ATMAIN__` is empty | same |
| does `_main` return? | **no** — it throws the string `"unwind"` | returns `-1` |

The first three match, which is what the startup sequence above predicted. The
fourth is the interesting one, and it corrects the shape of the work rather than
the diagnosis:

**The official `main` never returns.** It throws `"unwind"`, which is the
Asyncify unwind — the guest parks mid-run waiting for an async event (a
finished download, a timer) and is resumed later by the glue's rewind. The
glue's `callMain` catches exactly that case and returns cleanly
(`else if (e == "unwind") { noExitRuntime = true; return }`), which is why the
page survives and keeps drawing. Confirmed on the live page: `calledRun: true`,
a **non-black frame** (`mean 173.38`, `max 255`, 87% lit), 24 game requests and
18,240 bytes fetched, the guest loading `index.lua`, `trial.ini`,
`save9999.dat`, `index.dat` and then decoding real PNGs.

So `-1` is not the shape a finished run has. It is the shape a run has when the
guest asks for a display, is told there is none, and gives up — which is exactly
what our run is, and the only difference between the two is the one import:
`oc = _glfwInit ()`. The glue's `_glfwInit` returns 1 and the guest proceeds;
ours returns 0 — glfw's own `GLFW_FALSE` — and the guest declines. Returning 0 is
the honest answer for a host with no display, but it means the `gl` bucket is
not a formality to be filled in afterwards: **a host with a real canvas must
return 1 there, and a correct `gl` will not make `main` return either — it will
make it park in `"unwind"` and resume.** Building `gl` against the wrong
success shape is the trap here.

Two harness facts are worth keeping, because both first looked like a guest that
stops early:

- The control's data package (`deploy-ctrl/web/artemis.data`, a `pf66` file)
  carries a **hardcoded** `ASYNCIFY_URL = http://127.0.0.1:4702/game/`. The
  guest fetches its assets from that port, not from the page's own origin. Serve
  on any other port and every asset dies with `ERR_CONNECTION_REFUSED`, which
  reads exactly like a guest that gives up and is nothing of the sort.
- `argv[i]` is a pointer to NUL-terminated **UTF-8 bytes**, not a UTF-32 string.
  Reading those bytes as 32-bit words returns confident, plausible mojibake
  (`"⼮獩潲浡ᕀ"`) with no error at all. A mis-specified width in a memory reader
  does not fail; it lies.

## The per-bucket measurement

`formatBootTrace(boot.trace)` is the deliverable the Lead asked for — a table of
short name → call count per bucket, so "did the guest actually reach our code" is
a measurement rather than a hope. From the current run, with the real package
mounted and the `async` bucket dispatching:

| bucket | calls | short names | called |
| --- | ---: | ---: | ---: |
| `async` | 1175 | 30 | 12 |
| `asyncify` | 2 | 4 | 1 |
| `eh` | 0 | 15 | 0 |
| `fd` | 19 | 41 | 10 |
| `gl` | 1 | 99 | 1 |
| `runtime` | 3 | 8 | 3 |
| `todo` | 0 | 1 | 0 |

The `async` count is the headline: 1,175 re-entries into the guest's own code
from a host that implements no guest logic of its own. The single `gl` call is
`_glfwInit`, and it is the last import of the run. The three `runtime` calls are
`Aa` / `Ab` / `Bb` from the `Mc` constructor pass.

An earlier run, before the `async` bucket dispatched, put `main` into a
`wasm_rt_trap` 3 instructions after a stubbed trampoline returned — and the
diagnosis was wrong in a way worth recording. The site was a pure **memory**
access through a struct pointer (`call_indirect: 0` in the whole window), so it
could not be a table-slot problem, and the "500-frame recursion" reading was a
block-depth misreading: the `br_if` *exits* the block and lands on the
`i32.const 5 / call wasm_rt_trap` pair. What actually fixed it was giving the
trampolines the module's own `dynCall_*` exports, so the guest's real function
table and its `__THREW__` bookkeeping run instead of a zero.

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

## The glue is an input, not a constant

The spec this package dispatches against was generated from **one specific
glue file**, and the two glues in the tree are **not the same bytes**:

| glue | size | sha256 (head) |
| --- | ---: | --- |
| `ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js` | 276,695 B | `a3e65b92` |
| `.test/artemis-official-async/deploy/web/artemis.js` | 276,697 B | `f759f8b3` |

The first difference is at **byte 4495** — an embedded `adPackage` table reads
`end:236` in one and `end:3817` in the other — which shifts every later function
by a uniform **+2 bytes**. So a table generated from the vendor glue places every
`impl` offset after byte 4495 two bytes early against the deploy build, and a
2×2 cross check (vendor↔vendor, deploy↔deploy, and both cross pairs) fails both
cross pairs on exactly that. The table records `totals.moduleSha256` and
`totals.glueSha256`; the checker **refuses** a table whose glue is not the one it was
made from, and only **reports** the module hash — a module embeds its own build
directory (41,321,914 B with `D:/online_gal/...` from a Windows checkout against
41,322,022 B with `/home/runner/work/...` from the runner), so its hash can only match
on the machine that built it. The ABI rows are what is judged, and the CI step masks
those two provenance lines before comparing.

Two consequences for the work in this package:

- **Instrument the browser path with the table generated from the DEPLOY glue.**
  The control run (official module + official glue + a real game in a browser)
  is what the deploy table describes. The vendor table is for the vendor binary.
  A control whose numbers were read off the wrong table would be measuring
  nothing.
- **Regenerate for the inputs you actually have.** The committed pair came out
  of the bare command, with **no `--forward`**:

  ```bash
  node ports/artemis/harness/glue/imports-map.mjs \
    --wasm   <this build's bridge.wasm> \
    --glue   ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js \
    --aliases <this build's runtime-imports.json> \
    --out-dir <dir>
  ```

  `imports-forward.c` is generated by the build and is **not tracked**
  (`git ls-files` finds no such file), so a clean checkout cannot produce it —
  and passing it flips every row's `forwarded` from `-` to `yes`, about 200
  lines of diff that record *how* a seam was implemented in that build, while
  every other column records *what the module needs*. This table records the
  ABI. `--forward` is still accepted as corroboration, not as an input.

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
node .test/team-artemis/count-coverage.mjs          # 197 names / 206 of 207 rows / 1 stub
bash .test/team-artemis/gate-step.sh                # GATE EXIT 0
```

`count-coverage.mjs` is the check that keeps this README honest: it reads
`imports-map.json`, imports the **compiled** factories, unions their
`Object.keys`, and reconciles against the spec rows. It is how the numbers in
this file were established rather than counted by hand.

`gate-step.sh` is the body of the CI step *Gate the committed import map against
this build* (`.github/workflows/build-wasm-reusable.yml`) run locally against
the CI bridge build. It regenerates the pair with **no `--forward`**, `cmp`s
both files against the committed ones (byte-identical), and re-runs
`verify-imports-map.mjs` — `ALL CHECKS PASS`. That is the same proof CI runs on
every build, so the table in this repo cannot silently drift from the binary.

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
the one remaining row is `todo` (`_system`). Of the 99 `gl` rows, 79 are real
implementations and 20 are documented refusals (19 audio rows plus
`_emscripten_enter_soft_fullscreen`); the 11 event rows count as real, because
answering `EMSCRIPTEN_RESULT_INVALID_TARGET` with no document *is* an answer.
Import from `./host/bridge.js` or `./host/boot.js` until that changes.
