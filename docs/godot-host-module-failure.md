# AetherKiri web export: which module fails, and why

## The export ships three WebAssembly modules, not two

| file | size | what it is |
| --- | --- | --- |
| `index.wasm` | 1,601,069 | Emscripten main module (dlink runtime) — provides `env` and 357 `GOT.func` imports |
| `index.side.wasm` | 43,268,615 | Godot engine side module — exports `godot_web_main`, `OS_Web::fs_sync_callback` |
| `aether_kiri_godot.wasm` | 52,554,275 | **the GDExtension**: 26,292 × `TJS`, 130 × `XP3`, `Kirikiri` |

Identity established by string markers in the raw bytes, which needs no section
parsing and cannot drift:

```
=== side (index.side.wasm) ===        === godot (aether_kiri_godot.wasm) ===
      1  godot_web_main                     26292  TJS
      3  OS_Web                               130  XP3
     14  Godot Engine                          2  Kirikiri
```

## Which one the loader loads, and which one fails

`GODOT_CONFIG` in `index.html` names the third module explicitly:

```json
{"executable":"index",
 "fileSizes":{"index.pck":59096168,"index.wasm":1601069},
 "gdextensionLibs":["aether_kiri_godot.wasm"],
 "godotPoolSize":8,"emscriptenPoolSize":32,"ensureCrossOriginIsolationHeaders":true}
```

So Godot loads `aether_kiri_godot.wasm` as a **GDExtension dlink library** during
engine start-up. That is where the failure occurs: the exception was captured from
`WebAssembly.instantiate` while the page reported only `[AetherKiri Live2D] bridge
installed` and never reached a game.

This is consistent with everything measured earlier:

- the page title becomes `Aether`, so **Godot itself starts** — `index.wasm` and
  `index.side.wasm` are fine;
- the export's own capability gate passes (`getMissingFeatures` returns nothing),
  so COOP/COEP and `SharedArrayBuffer` are satisfied;
- the crash is a `RuntimeError: memory access out of bounds` from instantiation,
  which is what a **dlink ABI mismatch** looks like: the module instantiates, a
  start function runs, and a pointer computed from a mismatched layout goes out of
  bounds.

Both the alpha and the stable release fail identically (`0c87b40a` vs `0c87a88e`),
and `index.side.wasm` is byte-identical in size across both, so this is not a
regression in a newer build.

## What this means for the Godot plan

The plan is sound and this does not contradict it: Godot is the intended host for
all three engines, and the export proves the architecture — one Godot shell, one
`AetherRuntimePlayer`, providers for KiriRuntime, OnsRuntime and the Artemis
runtime. The blocker is narrower than "Godot does not work":

**the prebuilt GDExtension binary cannot be instantiated in this browser setup.**

Two ways forward, and they are genuinely different in cost:

1. **Build the web export from source.** A dlink side module has to be produced
   against the same Emscripten and Godot versions as the main module; a mismatched
   pair is exactly the failure above. The toolchain is emsdk + Godot 4.7 with the
   dlink web templates + vcpkg with 51 wasm dependencies (including ffmpeg,
   opencv4 and full boost), which is a multi-hour, ~10 GB build. The advantage is
   that it also gives us the ability to change things — which the prebuilt binary
   does not.

2. **Try a different published build first, cheaply.** The failure is in the
   GDExtension, and `gdextensionLibs` names one file, so a release exported with a
   matching toolchain would work. Before committing to a build, it is worth
   checking whether any release ships a *different* `aether_kiri_godot.wasm` — the
   hash would say so immediately, and it costs one download.

Option 2 is worth doing because it is minutes, whereas option 1 is hours; and if
option 2 fails, it also removes the last uncertainty before starting option 1.

## What is already established and independent of this

- **Byte-granular loading works**, in the ONS host built earlier: a 1.8 GB archive
  costs 749 KB and a 1.2 GB archive 513 KB, with the engine unaffected
  (`Display: 793 x 446`, `Audio: 48000 Hz`). See `docs/ons-worker-host.md`.
- **The unified filesystem contract is known** and implemented in
  `packages/vfs/src/lazy.ts` and `sync-bridge.ts`.
- **The XP3 and NSA formats are settled** from authoritative sources
  (KiriKiri2's own `XP3Archive.cpp`, garbro's `ArcNSA.cs`).
- **Which engines AetherKiri hosts**: one Godot 4.7 shell with
  `AetherRuntimePlayer` dispatching to KiriRuntime, OnsRuntime, the Artemis
  runtime and Siglus — so "one Godot core for these engines" is its design, not a
  proposal.

## Licensing note that affects how this work proceeds

AetherKiri is GPL-3.0-or-later, and its code may be reused. **Tyranor-Next is
GPL-2.0-only and is therefore incompatible with GPL-3.0** — no code from it may be
copied, and the existing notices already record this correctly at
`THIRD_PARTY_NOTICES.md`. Facts taken from it (engine marker names, directory
layouts, the fingerprint table) are not copyrightable expression, but its
implementation is off limits. AetherKiri is the reference to build on.

## Reproducing the diagnosis

- `node .test/wasm-identify.mjs` — identifies each module by its string markers
- `node .test/ak-web-modules.mjs` — prints `GODOT_CONFIG` and the module list
- `node .ref/ak-web/serve.mjs`, then `http://127.0.0.1:4600/` — the crashing page
