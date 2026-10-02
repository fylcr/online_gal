# Third-party notices

Project-authored code is declared **GPL-3.0-or-later** (see [`LICENSE`](LICENSE)).
The following records third-party provenance, applicable notices and unresolved
permissions. The root licence does not replace those notices or grant rights to
material whose redistribution permission has not been established.

---

## Kirikiroid2 — TJS2 opcode and bytecode format tables

**Files:** `packages/tjs2-bytecode/src/opcodes.ts`,
`packages/tjs2-bytecode/src/container.ts`

**Upstream:** <https://github.com/zeas2/Kirikiroid2> — `src/core/tjs2/`

The TJS2 VM opcode enumeration, the per-opcode instruction widths, the bytecode
container layout (file/data/objects tags and the constant-array encoding) and
the version tag are **transcribed** from that project's
`tjsInterCodeGen.h`, `tjsInterCodeGen.cpp` and `tjsByteCodeLoader.cpp`.

**Licence:** BSD-style. The upstream notice is reproduced below.

```text
Copyright (c), W.Dee and contributors All rights reserved.
Contributors
 Go Watanabe, Kenjo, Kiyobee, Kouhei Yanagita, mey, MIK, Takenori Imoto, yun
Kirikiri Z Project Contributors
 W.Dee, casper, 有限会社MCF, Biscrat, 青猫, nagai, ニャー, 高籔 雅之, 永劫,
 ぴょんぴょん探偵, ぷりんぷりん（今は魔がぁの星）, AZ-UME, 浜, 秋人,
 Katsumasa Tsuneyoshi, 小池潤, miahmie, ソールギア, メガネ, ゆっくり, yun,
 株式会社, 有限会社エルザ, Takenori Imoto

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

Because this material is included in a GPL-3.0-or-later work, the combined
distribution is governed by the GPL; the BSD notice above is retained as
required and applies to the transcribed portions in their original form.

---

## Kirikiroid2-web — architecture and two patched web-platform files

**Files:** `ports/krkr/kirikiroid2/patch-vlfs-host-source.mjs` and the two
upstream files it patches in place, `platforms/web/vlfs.js` and
`platforms/web/shell.html`

**Upstream:** <https://github.com/fenghengzhi/kirikiroid2-web> (`web` branch, commit
`13dda190` — pinned in `docs/kirikiri-web-plan.md`)

Its architecture (JSPI + VLFS) is the basis of this project's KiriKiri
WebAssembly build. The local [`run-build.sh`](ports/krkr/kirikiroid2/run-build.sh)
is Windows-specific; Linux CI uses [`build-krkr.sh`](.github/scripts/build-krkr.sh).
Both apply the host-source seam to an upstream checkout. Not committing the full
checkout does **not** remove the obligations of distributing the resulting wasm,
patched web-platform files or bundled libraries.

**Licence:** the upstream README says "MIT License", but the actual LICENSE is
the Japanese BSD-style Kirikiri/W.Dee text plus individual dependency notices.
Use those applicable texts, not the README label or a blanket MIT assumption.
The Japanese conditions include retaining the copyright/conditions in source and
binary distributions and not using contributor names for endorsement without
permission. The abbreviated English notice above is not a replacement for the
full upstream LICENSE or dependency notices. Preserve them with the actual
source and binary distributions and review each bundled component separately.

---

## vcpkg overlay ports — gettext and libexif

**Files:** `ports/krkr/kirikiroid2/overlay-ports/gettext/**`,
`ports/krkr/kirikiroid2/overlay-ports/libexif/**`

These are committed patch files and `portfile.cmake` overrides that make two
vcpkg ports build under Emscripten / MinGW on this machine, so that the
KiriKiri build is reproducible from a clone. They are not modified upstream
source bundles — each overlay points at the upstream tarball and applies the
patches listed beside it.

| Overlay | Upstream licence (declared in its `vcpkg.json`) | Why it exists |
| --- | --- | --- |
| `gettext` | `GPL-3.0-only` | Upstream's Windows build fails on a real source defect in `gnulib-lib/localtime.c`; the local `fix-mingw-gcc15-localtime-env-loop.patch` is this machine's fix. See the gettext section of [`docs/项目状态与恢复指南.md`](docs/项目状态与恢复指南.md). |
| `libexif` | `LGPL-2.1-or-later` | Under an Emscripten target `vcpkg_configure_make` passes no `--host`, so autoconf treats the build as native and runs conftest binaries it cannot run. The overlay sets `VCPKG_MAKE_BUILD_TRIPLET "--host=wasm32-unknown-emscripten"`. |

The patches themselves are trivial build fixes and are distributed under the
same terms as the ports they modify; the upstream notices remain in the
upstream tarballs that vcpkg downloads.

---

## AetherKiri — architecture and adapter-layer knowledge

**Upstream:** <https://github.com/AetherKiri/AetherKiri> — **GPL-3.0-or-later**

No source files are copied from that project. Its `bridge/` directory informed
the design of `@online-gal/engine-api` (provider registration, the scored probe
and dispatch order, the option key names, and the input-queue and save-storage
semantics), and the interfaces here are deliberate TypeScript counterparts of
its C ABI. The option **key strings** in `engine-api/src/options.ts` are
reproduced verbatim so that per-game tuning notes remain meaningful across both
codebases.

## OnscripterYuri — save-file conventions

**Upstream:** <https://github.com/YuriSizuku/OnscripterYuri> — GPL-2.0-or-later

The ONScripter save-file naming rules in `runtime-ons/src/save-storage.ts` are
transcribed from AetherKiri's `bridge/onscripter_runtime/src/onscripter_save_storage.cpp`.
OnscripterYuri's own `GPL-2.0-or-later` grant is what makes folding that layer
into a GPL-3.0-or-later project permissible.

---

## artemis-compat — engine structure and house style

**Upstream:** <https://github.com/Weiss-UltimateSavior/artemis-compat> — **GPL-3.0**
(reference commit `9f622629f595db85e6f76c45c06edde29d7d9961`, recorded in the
[`bridge workflow`](.github/workflows/build-wasm-artemis-bridge.yml)).

The current bridge CI skips that upstream checkout; it does not build the complete
artemis-compat engine. The project's C++ products use it as a **naming and structural
reference**: file-per-concept organization, class/method vocabulary and purpose
comments. [`REWRITE.md`](ports/artemis/REWRITE.md) records the intended house style,
not proof that the whole current product tree conforms to it.

This reference explains the deliberate naming relationship. Its GPL declaration
cannot grant rights to the official Artemis SDK or automatically resolve the
provenance of products checked against SDK-generated code. Those materials and
remaining generated function bodies are addressed separately below.

---

## garbro — NScripter archive format reference

**Files:** `packages/vfs/src/archives/nsa.ts`

**Upstream:** <https://github.com/morkt/garbro> — `ArcFormats/NScripter/ArcNSA.cs`

The NSA / NS2 / SAR header layout, the big-endian integer order, the compression
codes and the record field order were taken from that project's NScripter
handler, which both reads and writes the format. No code is copied; the reader is
an independent TypeScript implementation of the same structure, verified against
a real 1.8 GB archive.

**Licence:** MIT.

```text
Copyright (C) 2014-2015 by morkt

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

---

## Test fixtures

`.test/fixtures/aetherkiri-startup.tjs` is copied verbatim from AetherKiri's
bundled KAG3 demo (`demos/aetherkiri-kag3/data/startup.tjs`, GPL-3.0-or-later).
It is a plain TJS source file used to exercise the script-classification path
against a real shipping artefact instead of a hand-written fixture.

---

## Reference-only material (no code included)

| Project | Licence | Use |
| --- | --- | --- |
| [Tyranor-Next](https://github.com/Weiss-UltimateSavior/Tyranor-Next) | **GPL-2.0-or-later** | Engine marker names and file-layout facts only — no code is copied. Its `GPL-2.0-or-later` grant is compatible with this repository's GPL-3.0-or-later, so folding it in is permissible; the engine fingerprint table (`root.pfs`, `startup.tjs`, `nscript.dat`, `tyrano/`, `js/rmmz_core.js` …) is what is actually used, and the scored probe over it is an independent TypeScript implementation. |
| [siglus_rs](https://github.com/xmoezzz/siglus_rs) | MPL-2.0 | Not included. Referenced for the Siglus capability list; MPL-2.0 §3.3 would permit combination if it is ever ported, subject to retaining its notices. |

---

## Artemis Engine SDK — retained reference inputs, redistribution unresolved

**Files:** `ports/artemis/vendor/artemis-sdk/` (9 files, 24 MB)

| File | Bytes |
| --- | --- |
| `Copyright.txt` | 20,717 |
| `WebAssembly/artemis.{html,js,wasm}` | 3,912 / 264,298 / 2,682,787 |
| `WebAssembly/asyncify/artemis.{html,js,wasm}` | 3,912 / 276,695 / 5,998,665 |
| `Android/arm64-v8a/libartemis.so` | 8,149,024 |
| `Android/armeabi-v7a/libartemis.so` | 7,395,052 |

**Upstream:** the official Artemis Engine SDK, by Mikage. The retained
[`Copyright.txt`](ports/artemis/vendor/artemis-sdk/Copyright.txt) begins with:

> Artemis Engine — Copyright (C)2009 Daiju Hanaoka (Mikage) All rights reserved.

The remainder includes bundled-library notices. Those grants are **not a grant to
redistribute Artemis itself**. This file does not establish the earlier claims of
commercial fees or a particular permission procedure; those claims are withdrawn.
No verified Artemis redistribution grant is recorded here. The root GPL declaration
cannot supply that missing permission, and retaining these bytes is not approval to publish them.

**Current use:** the asyncify wasm is the tracked input to
[`build-artemis-bridge.sh`](.github/scripts/build-artemis-bridge.sh), which regenerates
wasm2c source and combines it with handwritten products. Official HTML / minified JS
and Android binaries remain reference or historical diagnostic material, **not the
shipping browser host**. This organization pass preserves their bytes and notices;
it does not remove them, claim authorization, or release an SDK-derived artifact.

Historical decisions remain in [`artemis-web-port.md`](docs/artemis-web-port.md) §15,
but that history is not evidence of an applicable grant. Before redistribution,
review the SDK input, remaining generated bodies and resulting products together
and establish the necessary rights or replace the affected material. Removing only
the vendor directory does not by itself resolve provenance in already derived outputs.

---

## Artemis Engine — `ports/artemis/src/` (rewritten from the official decompile)

**Files:** [`ports/artemis/src/`](ports/artemis/src/) — the current worktree ledger
maps **340 functions to 340 C++ product files** in `namespace artc`. Four new products
are not yet in the index; this count is not a claim about the committed snapshot.
The rules and ledger are [`REWRITE.md`](ports/artemis/REWRITE.md) and
[`rewrite-names.tsv`](ports/artemis/rewrite-names.tsv).

**Provenance:** the products re-express behaviour checked against the official SDK's
wasm2c output in `.ref/artemis-official/`. They are not a build of the complete
artemis-compat upstream source tree. That project supplies naming and structural
reference; it does not grant rights to the official SDK or settle the provenance
of this rewrite. Existing source-range annotations and historical records are preserved.

The asyncify bridge still contains **unreplaced SDK-generated bodies** alongside
these products. Renaming functions or writing new C++ does not establish that all
resulting code is independently redistributable. Conversely, this notice does not
make a categorical legal finding that every product has identical licensing status:
that boundary requires separate review of the actual material and applicable rights.
The project's GPL declaration cannot substitute for missing third-party permission.

[`REWRITE.md`](ports/artemis/REWRITE.md) records the rewriting policy, not proof of
completed compliance. Generated-like labels, numeric pool references and style debt
remain; the full handwritten cold-game gate and modern-host public session are not
accepted. The earlier claims of a fully replaced goto/address graph, passed full-tree
style gate and complete engine semantics are withdrawn.

Recipes under [`ports/artemis/reverse/`](ports/artemis/reverse/) and historical
[`artemis-web-port.md`](docs/artemis-web-port.md) §16 explain the diagnostic work.
Synthetic or generated-body success does not establish game usability or a
redistribution grant. This organization pass changes navigation and upload checks,
not the engine, third-party copyright text, or authorization status.

