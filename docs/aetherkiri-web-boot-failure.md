# AetherKiri Web export — boot failure diagnosis

Status: **blocked in the alpha build itself, not in the environment.**

Verified in two browsers (Edge and Chrome, the latter via a clean DevTools
session with no extensions) with byte-identical results, so this is deterministic
and environment-independent.

## What works

| Check | Result |
| --- | --- |
| Page served, title becomes `Aether` | pass |
| `crossOriginIsolated` | `true` |
| `SharedArrayBuffer` / `Atomics` | `function` / `object` |
| `WebAssembly` + SIMD | supported (`WebAssembly.validate` on a SIMD module → true) |
| `hardwareConcurrency` | 20 |
| COOP / COEP response headers | `same-origin` / `require-corp` |
| Fonts (`/fonts/default.ttf`, `/fonts/symbols.ttf`) | 200, correct, Range-capable |
| All referenced assets present | pass |
| Live2D | a `Proxy` stub, cannot crash the engine |
| WebGL context | created, not lost |

## The failure

```
RuntimeError: memory access out of bounds
    at wasm://wasm/0c87b40a:wasm-function[8675]:0x88c43b
```

Captured by wrapping `WebAssembly.instantiate`, which shows the throw comes from
**module instantiation — Emscripten static constructors — not from game or
script code**:

```json
{"key":"instantiate","error":"RuntimeError: memory access out of bounds\n    at wasm://wasm/0c87b40a:wasm-function[8675]:0x88c43b"}
```

The same module hash (`0c87b40a`) appears in both browsers, so the failing module
is identical each time.

## Observable sequence

1. `[AetherKiri Live2D] bridge installed`
2. `Engine.getMissingFeatures({threads: true})` returns empty, so Godot's own
   capability gate passes
3. `setStatusMode('progress')` runs, and `engine.startGame()` is entered
4. `onProgress` fires with `current <= 0`, so the progress attributes are removed
   (removal is the code path for a non-positive total, so the boot did begin)
5. the wasm raises `memory access out of bounds`
6. the promise never settles, so the page's own failure handler never runs — the
   overlay stays visible, the canvas stays black, and no error text appears

That last point matters: **the export is silent about this failure by
construction.** The rejection happens in a separate async chain from the one the
page awaits, so `displayFailureNotice` is never reached. Nothing on screen can
indicate what went wrong.

## Conclusion

The crash is inside the prebuilt web bundle during module instantiation, and it
is **not specific to the alpha**: the latest stable release, `1.0.3`, fails in
exactly the same way.

| build | module | failing function | offset |
| --- | --- | --- | --- |
| `1.0.4-alpha.1` | `0c87b40a` | 8675 | `0x88c43b` |
| `1.0.3` (stable) | `0c87a88e` | 8677 | `0x88c48f` |

Both are `RuntimeError: memory access out of bounds` raised from
`WebAssembly.instantiate`. `index.side.wasm` (43,268,615 B) is byte-identical in
size across the two releases, as are `index.js` and `index.wasm`; only
`aether_kiri_godot.wasm` differs by about 1 KB. So this is a long-standing
upstream fault in the compiled runtime, not a regression in the newest build, and
a different release will not fix it.

Nothing about the hosting environment, headers, assets, or browser features is
implicated. This needs upstream attention, or a build produced from source with
the failure instrumented.

## What is portable regardless

The filesystem contract read out of `index.html` does not depend on this build.
It is the design worth carrying into our own host:

```js
// Per-file registration: the Blob is referenced, never copied.
fileEntry(path, file) -> { path, size, mtimeMs, fileId }
// the manifest is a list of those entries

// Synchronous ranged read — the lazy primitive.
readLocalFileSync(fileId, offset, length):
    if (FileReaderSync)                       // worker fast path
        file.slice(offset, offset + length)
    else                                      // Atomics handshake
        worker.postMessage({kind:'read', fileId, offset, length, sab})
        Atomics.wait(header, 0, 0, 30000)

// Mount points
mountPoint = "/webgames/" + id              // a game directory, or
gamePath   = mountPoint + "/" + archive     // a single archive
```

Only the requested byte range is ever read, so a 2.2 GB `data.xp3` never enters
memory whole and is never copied into the Emscripten virtual filesystem.
