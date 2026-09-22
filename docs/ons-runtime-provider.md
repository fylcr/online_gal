# The ONS runtime as a platform provider

The ONScripter engine is no longer only a test harness. It is packaged as
`@online-gal/runtime-ons`'s second provider, `onscripter-wasm`, and registered ahead of the
TypeScript interpreter that used to be the only implementation.

## Where the files live

| Path | What it is |
| --- | --- |
| `packages/runtime-ons/src/wasm/ons-worker.js` | The engine host: the worker that runs the WebAssembly engine |
| `packages/runtime-ons/src/wasm/host.mjs` | The main-thread half: worker startup, audio relay, coordinate mapping |
| `packages/runtime-ons/src/wasm/audio-relay.mjs` | PCM playback through an `AudioWorklet` |
| `packages/runtime-ons/src/wasm/host.d.mts` | The declarations for the above — `.d.mts` because a `.mjs` import needs that name |
| `packages/runtime-ons/src/wasm-runtime.ts` | The `RuntimeInstance` adapter |
| `packages/runtime-ons/src/wasm-provider.ts` | The `RuntimeProvider` registration |
| `packages/runtime-ons/assets/onsyuri.{js,wasm}` | The engine, built from OnscripterYuri source |
| `packages/runtime-ons/scripts/copy-runtime-assets.mjs` | Copies the harness and assets into `dist/` at build time |

The three harness files are **copied verbatim** from `ports/ons/harness/`, deliberately. They
are the engine host, not application code, and they are the version that was verified working
end to end — assets loaded in 0.56 MB of ranged reads out of a 1.8 GB archive, audio, input,
and the picture filling the viewport. Rewriting them for packaging would mean re-verifying all
of that to gain nothing.

## Why a copy step is needed

`tsc` compiles TypeScript and nothing else, so the `.mjs` harness would be absent from `dist/`
and every import of the package outside the dev resolver would fail with
`ERR_MODULE_NOT_FOUND`. That is not hypothetical — it broke six test files until
`scripts/copy-runtime-assets.mjs` was added to the build. The engine binaries under `assets/`
need the same treatment, because the adapter resolves them relative to its own module URL.

## The adapter, and what it does not do

`OnsWasmRuntime` translates between the harness's interface and the contract the launcher
expects. It is small because the two models agree on the important points: both are
canvas-based, both take normalised coordinates in the engine's own space, and both are driven
externally by load/tick/input.

Two methods are no-ops **on purpose**:

- `tick()` — the engine owns its own loop inside the worker and yields through
  `emscripten_sleep`. A second clock stepping it would fight that, which is also why the
  harness needed no animation-frame driver.
- `render()` — the engine draws straight into the transferred `OffscreenCanvas`, so there is
  no frame for the host to blit.

`save()` reports the engine's **own** save directory rather than serialising engine state.
ONScripter writes saves through its `--save-dir`, which the worker mounts on `IDBFS`;
inventing a second representation would duplicate it and risk the two diverging.

## Configuration

The runtime needs two things, read from the per-game config first because that is the channel
the launcher actually uses (`RuntimeSession` forwards the library record's config into
`create()`):

| key | meaning | fallback |
| --- | --- | --- |
| `indexUrl` | the index describing the game's files | `./onsyuri_index.json` |
| `engineBase` | where `onsyuri.js` / `onsyuri.wasm` live | the package's `assets/` |

`load()` waits for the engine to report its resolution rather than sleeping a fixed interval —
that message is the first proof the engine is up and drawing. A timeout turns a game that
cannot start into an error instead of a hang, which matters because the failure mode of a
translation bug here is a freeze, not a crash.

## Registration order

```
 100  krkr2
  95  onscripter-wasm   ← the engine
  90  onscripter        ← the TypeScript interpreter
  90  siglus            (unavailable)
  85  artemis           (unavailable)
  80  tyrano
  20  web
```

Both ONScripter providers recognise the same games — which games those are is a property of
the engine family, not the execution strategy, so they **share one probe** rather than keeping
two in step. The choice is made by probe score and then by priority; the engine's 95 puts it
ahead wherever it can run, and the interpreter stays as the fallback for games the engine
cannot open.

## What the tests cover, and what they cannot

`.test/ons-wasm-provider.test.ts` covers the provider's registration, its precedence over the
interpreter, and the adapter's refusal to half-start without a canvas. It does **not** run the
engine: that is a 4.6 MB WebAssembly binary needing a worker and a transferred canvas, which
is what `ports/ons/harness/` and its server on port 4620 are for.

The split is deliberate. A translation mistake in the adapter would not crash — it would show
up as freezes and dropped input, which is exactly the class of failure the browser harness
catches and Node cannot.
