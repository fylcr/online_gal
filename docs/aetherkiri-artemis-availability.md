# Is AetherKiri's Artemis runtime usable as our Artemis emulation?

Question asked: emulate Artemis using AetherKiri's Artemis emulation code,
instead of the self-built `artemis-compat` runtime.

**Answer: no — the code is private, and the public binary that contains it
cannot be loaded. But AetherKiri's public *host-side input mapping* is
available, and it explains the pointer defect we froze.**

All claims below are measured on this machine. Nothing is inferred from
README prose.

## 1. The Artemis runtime is a private submodule

The runtime's own namespace is `aetherinternal::artemis::{runtime,script,storage}`.
`aetherinternal` belongs to `packages/AetherInternal`, declared in `.gitmodules`:

```
[submodule "packages/AetherInternal"]
	path = packages/AetherInternal
	url = git@github.com:AetherKiri/AetherInternal.git
```

That SSH URL is not an accident of configuration — the repository is private,
proven twice over:

| probe | result |
| --- | --- |
| `git ls-remote https://github.com/AetherKiri/AetherInternal.git` | `remote: Repository not found.` |
| `git ls-remote git@github.com:AetherKiri/AetherInternal.git` | `ERROR: Repository not found.` |
| `api.github.com/orgs/AetherKiri/repos` | 6 public repos, no `AetherInternal` |

The SSH probe ran under an authenticated key (`Hi fylcr! You've successfully
authenticated`), and still got "Repository not found" — so this is an access
denial, not a missing key. No `gh` login, no `~/.git-credentials`, no
`GITHUB_TOKEN` in the environment.

The upstream project treats that boundary as a protected asset on purpose:
`tools/verify_internal_protection.py:34` asserts
`owned = "/packages/AetherInternal/src/" in source`, and `:43` raises
`SystemExit("no AetherInternal owned compilation units were found")` when it
cannot find them.

## 2. The public source contains none of it

Checked against a full clone of the real repository
(`git clone --depth 1 https://github.com/AetherKiri/AetherKiri.git`), because
the earlier local `.ref/ak` copy is a 145-file subset missing `cpp/`,
`packages/`, `rust/`, `tests/`, `tools/`, `cmake/`, `third_party/` and `web/`.

Every Artemis runtime class name is absent from the entire public tree:

| class | public files |
| --- | --- |
| `PfArchive` | 0 |
| `IetDocument` / `IetNode` / `IetAttribute` | 0 |
| `LuaHost` / `LuaRuntime` / `VariableStore` | 0 |
| `ArtemisRenderer` / `ArtemisConfig` | 0 |
| `VfsStream` / `ScriptFrameInfo` / `EmoteLayerOptions` | 0 |
| `ArtemisCommandSink` / `ArtemisAudioEngine` | 0 |

`grep -rln "namespace aetherinternal"` over the public tree returns nothing.
The only `aetherinternal` hits are CMake *calls* — `aetherinternal_extend_artemis(engine_api)`
at `bridge/engine_api/CMakeLists.txt:80-81`, and the API-version gate at
`CMakeLists.txt:125-140` — never definitions. The gate degrades rather than
fails when the package is missing, which is how public CI builds without it.

The public Artemis-facing surface is host-side only: the Lua symbol prefix
header, `ENGINE_OPTION_ARTEMIS_MAX_VISUAL_DELTA_MS`, the provider registry
gate, and game detection.

## 3. The public artifact that does carry the runtime cannot be loaded

The published web exports do contain the compiled runtime — the gate string
`"available only in internal Debug builds"` is absent from both builds, so
`AETHERKIRI_ENABLE_ARTEMIS_RUNTIME` was defined and the runtime was linked in:

| build | `aether_kiri_godot.wasm` | `artemis` strings | `AetherInternalRegisterArtemisRuntime` |
| --- | --- | --- | --- |
| 1.0.4-alpha.1 (`.ref/ak-web/`) | 52,555,010 B | 2179 | present |
| 1.0.3 stable (`.ref/ak-web-1.0.3/`) | 52,554,275 B | 2179 | present |

1,316 exported names carry "artemis"; `engine_*` exports total 110.

Loading it fails at the module's **start function**, which the section table
names as function index **8675** — the same index the browser reports:

```
RuntimeError: memory access out of bounds
    at wasm://wasm/0c87b40a:wasm-function[8675]:0x88c43b
```

That start function is where Emscripten's dlink loader applies data
relocations. `.test/wasm-probe/ak-load.mjs` reproduces the whole thing in Node
and then separates the two candidate causes: it stubs all 32,679 imports with
correctly-typed zero values and instantiates.

```
import kinds: { func: 8672, global: 24003, tag: 2, memory: 1, table: 1 }
memory imports: [ 'env.memory min=337 max=65536 flags=3' ]
table imports: [ 'env.__indirect_function_table' min=34891 ]
RESULT: instantiated OK
exports: 58434
has Artemis register: true
```

So the module is not intrinsically corrupt: it instantiates when the host
supplies the imports. But calling into it still traps, and the reason is
structural:

```
data segments: 5 | by flags: {1: 5} | total bytes 9240630
  flags=1 means PASSIVE: the host must place it
engine_get_runtime_api_version()       = THREW RuntimeError: memory access out of bounds
engine_get_runtime_provider_count()    = 0
AetherInternalRegisterArtemisRuntime() = THREW RuntimeError: memory access out of bounds
```

All 5 data segments (9.24 MB) are **passive**. A side module cannot place its
own passive segments — the dlink host must call `__wasm_apply_data_relocs`
after instantiation. The start function traps under Godot's loader before that
can happen, so the 9.24 MB of relocated data is never mapped and every
subsequent call reads unmapped memory.

`docs/godot-host-module-failure.md` reaches the same place from the other
direction: `gdextensionLibs` names one file, `index.side.wasm` is size-identical
across releases, and both alpha and stable fail at the same function with a
different module hash (`0c87b40a` vs `0c87a88e`). This is an upstream
dlink/ABI pairing fault, not something a different download fixes. There is no
newer artifact to try: the repository publishes **no releases and no tags**, and
`aetherkiri.github.io` is a marketing page with no export download.

Its 32,679 imports are also mostly Godot's: 12,190 `GOT.mem` and 11,810
`GOT.func` globals including `_ZNK5godot7Wrapped19_is_extension_classEv`. The
module is built against Godot's dlink ABI and cannot be driven by a host of our
own without reimplementing that ABI.

## 4. What *is* available, and why it matters

`apps/godot_app/scripts/game_input_mapping.gd` is public, 151 lines, and is
exactly the layer our pointer mapping lives in. Two comments state the rule
that our frozen defect violates:

```gdscript
# Artemis maps that surface to the logical frame with an aspect-fit viewport
# (including letterbox offsets), then maps input back through the same viewport.
# Mirror that transform here so the provider does not apply a second, different
# scale when it receives the event.
```

`map_point_to_surface()` performs two chained aspect-fit inversions — window →
content, then content → surface — because after a frame is presented, the
requested surface size can differ from the game's native size:

```gdscript
var surface_scale := minf(surface_size.x / content_size.x, surface_size.y / content_size.y)
var presented_size := content_size * surface_scale
var surface_offset := (surface_size - presented_size) * 0.5
return Vector2(surface_offset.x + content_point.x * surface_scale,
               surface_offset.y + content_point.y * surface_scale)
```

`map_point()` also documents why the requested size must not be reused after a
frame exists: "a runtime may accept a requested surface size before open and
then replace it with the game's native size while booting."

Two further functions encode mobile-host discipline worth having:
`stable_tap_point()` collapses sub-pixel drift back to the down point (so a
hover-driven runtime cannot read a tap as cursor motion), and
`touch_drag_release_is_cancelled()` is deliberately `catsystem2`-only, because
"Artemis owns richer layer-drag semantics internally."

**This is the transferable result of the investigation.** It converts our
frozen mouse-mapping symptom into a specific, named mechanism — a second scale
applied after the aspect-fit viewport — rather than leaving it as a mystery.

## 5. Conclusion and route

AetherKiri's Artemis emulation cannot replace `artemis-compat`:

- the runtime implementation is private, and access is denied to this machine
  under an authenticated identity that has no grant;
- the public binary carries it but cannot be loaded, because its passive data
  segments are never relocated under the shipped dlink pairing;
- no newer build exists to try.

`artemis-compat` therefore remains our Artemis runtime, and it already boots,
renders and plays the real game in the browser. The reusable finding is
AetherKiri's public input mapping, which names the mechanism behind the frozen
pointer defect.

## Reproducing

```
# which module fails, and at which function
python -c "..."   # see docs/godot-host-module-failure.md
# instantiate outside Godot and call into it
node --stack-size=4000 .test/wasm-probe/ak-load.mjs        # instantiate
CALL=1 node --stack-size=4000 .test/wasm-probe/ak-load.mjs # call in, observe traps
# the public export (serve from the repository root; serve.mjs resolves '.ref/ak-web' from cwd)
node .ref/ak-web/serve.mjs   # then http://127.0.0.1:4600/
# the public host-side input mapping
#   <clone>/apps/godot_app/scripts/game_input_mapping.gd
```
