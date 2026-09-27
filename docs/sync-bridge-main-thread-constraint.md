# Synchronous byte-range reads: why the obvious bridge cannot work

The goal is to let a compiled engine read a multi-gigabyte archive on demand
instead of materialising it. `packages/vfs/src/lazy.ts` provides the ranged end —
`RangedLazyFile` over a `Blob` or HTTP source, `ArchiveEntryLazyFile` for entries
inside an archive — and `packages/vfs/src/sync-bridge.ts` was written to be the
synchronous facade on top of it.

**The facade cannot run on the page's main thread.** This document records the
measurement, because the conclusion changes the architecture.

## What was measured

In Chrome, cross-origin isolated (`crossOriginIsolated: true`,
`SharedArrayBuffer: function`), the bridge's first `readSync` threw:

```
TypeError: Atomics.wait cannot be called in this context
    at SyncFileBridge.readChunk (dist/sync-bridge.js:159:32)
    at SyncFileBridge.readSync  (dist/sync-bridge.js:125:33)
```

This is not a missing capability. `Atomics.wait` is **specified to throw on the
main thread** — browsers forbid blocking the thread that runs the event loop. No
header, flag, or permission enables it. (Node allows it on its main thread, which
is why the same code cannot be validated there either: see the deadlock below.)

## A second, independent obstacle

The in-process test harness surfaced a related trap. With the worker half driven
by `postMessage` on the same thread, the block starves the very work it waits for:

```
[main]   posted read seq=1
[main]   result: timed out
[worker] finish state=1 count=64      <- only ran after the main thread yielded
```

`Atomics.wait` blocks the thread, so a worker on that same thread can never
finish. A real `Worker` runs elsewhere and avoids this — but it also means the
protocol cannot be unit-tested in-process, which is why the verification lives in
`.test/bridge-check/` behind a browser instead.

## Consequences for the design

The engine must not run on the main thread. Three candidate shapes:

### A. Run the engine inside the Worker (the correct answer)

The engine, its WASM, and the synchronous FS all live in a worker. There,
`Atomics.wait` is legal, so `readSync` works as written.

The presentation path has to move too:

```text
worker:  game.wasm -> OffscreenCanvas (canvas.transferControlToOffscreen())
main:    DOM events -> postMessage -> worker
```

This is the standard shape for threaded WASM applications, and it is why the
AetherKiri web build ships threaded WASM with `SharedArrayBuffer` requirements.
Audio is unaffected: an `AudioWorklet` already runs on its own thread.

The cost is that the UI, input and canvas setup have to be restructured around a
worker boundary, and the engine's canvas becomes an `OffscreenCanvas`.

### B. Synchronous XHR with a `Range` header

`XMLHttpRequest` with `{ async: false }` still blocks legally on the main thread,
and with `Range: bytes=a-b` it fetches only the requested window. This is exactly
what Emscripten's own `FS.createLazyFile` does.

It is deprecated, it blocks the UI, and `SharedArrayBuffer` means option A is
available — so this is a fallback, not the plan. It is worth knowing because it is
what the upstream tooling actually relies on.

### C. Keep deferring whole files (the status quo)

This is what OnscripterYuri's `mount_game` does today, and it is the behaviour the
byte-granularity work exists to remove: a `lazyload` file is fetched with
`fetch(url).then(r => r.arrayBuffer())` on first touch, so `arc.nsa` arrives as
one 1.8 GB allocation. Measured totals for that boot are in
`docs/ons-emulation-baseline.md`.

## What survives unchanged

`sync-bridge.ts` is not wasted. It is the correct worker-side implementation, and
its protocol carries two fixes worth keeping regardless of where it runs:

- **The sequence-based handshake.** Waiting on "state is idle" loses a wakeup
  whenever the worker finishes before the caller reaches `Atomics.wait`, because
  `postMessage` is asynchronous and a notification delivered to nobody is gone.
  Waiting on "the sequence number is still the one I sent" removes the race.
- **Chunked transfers.** A read wider than the window is served in successive
  passes into the caller's buffer, so the window never has to scale with the
  engine's largest request.

Both properties are asserted by the tests that can run without threads.

## What was actually chosen

The constraint held, and the three candidate shapes were resolved in two different ways:

- **Option A is what the ONS port does.** The engine, its WASM and its FS live in a worker; the
  canvas crosses as an `OffscreenCanvas` and input crosses as `postMessage`. There is no synchronous
  range bridge in the path at all — the archive is read through a ranged `stream_ops.read` on an
  `FS.createLazyFile` node, which works precisely *because* it runs in the worker
  (`docs/ons-worker-host.md`, `## The fix that mattered`).
- **Option C is gone from the shipped engine.** The 1.8 GB archive is served from a few hundred KB of
  ranged reads. `mount_game` still exists in the upstream page kept at
  `packages/runtime-ons/assets/onsyuri.html` — it is what this constraint was about — but it is **not**
  the mounted path: the provider loads `wasm/host.mjs` + `wasm/ons-worker.js`, and the worker does the
  mounting itself through `FS.createLazyFile`.
- **KiriKiri found a third way: it does not need the facade either.** JSPI suspends the engine on a
  cache miss instead of blocking it, so the engine stays on the **main thread** with no worker
  boundary and no synchronous read (`docs/kirikiri-web-plan.md`, `## The decisive reference` and the
  status block at its top). That is the outcome that superseded this project's own
  synchronous-XHR-ranges attempt.

So `sync-bridge.ts` remains the correct *worker-side* implementation of option A, and it is not the
route either shipped engine takes. Its own constraint is documented here so the next attempt does not
rediscover it.

## Verification harness

`.test/bridge-check/` — a page that imports the package's compiled
`sync-bridge.js` directly, a worker that wires `createBridgeWorker` to a range
fetch, and a server that generates a 64 MB synthetic file by formula behind
`Range` support. A 128-byte read of that file must fetch 128 bytes; anything
larger is visible immediately.

Run it with `node .test/bridge-check/serve.mjs` and open
`http://127.0.0.1:4610/`.
