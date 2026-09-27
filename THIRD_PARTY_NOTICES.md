# Third-party notices

This project is distributed under **GPL-3.0-or-later** (see [`LICENSE`](LICENSE)).
The following third-party material is included or reproduced, with the licence
each part is used under.

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
WebAssembly build, and `ports/krkr/kirikiroid2/run-build.sh` drives its CMake
preset. The build recipe carries two idempotent in-place patches to those two
files; no upstream source is redistributed from this repository, because the
build consumes a `.ref/` checkout that is gitignored.

**Licence:** the upstream `README.md` says "MIT License", but the repository's
`LICENSE` file is the BSD-style Kirikiri/W.Dee text quoted above for
Kirikiroid2, followed by the notices of the libraries it bundles. The two are
not in agreement, so the more restrictive file is the one treated as
authoritative here. Both are permissive and compatible with this project's
GPL-3.0-or-later.

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
(commit `a4f8a257cf87cb8f65ab4e3f83501ab327a55424`, pinned in `ports/README.md`)

No upstream source is redistributed here, and no file is copied: `ports/artemis/src/`
is hand-written C++ that was checked against that project's conventions rather than
lifted from it. What the rewrite takes from artemis-compat is **naming and shape** —
the file-per-concept layout, the `Class::Method` vocabulary, the `// file.cpp — purpose.`
header form, and the specific class names it already uses for the subsystems this
port needed. `ports/artemis/REWRITE.md` calls this "对齐 house style" and states
explicitly that no "Adapted from artemis-compat" attribution is written into the
sources.

Both projects are GPL-3.0, so the alignment raises no combination question. The
notice is recorded because the naming is a deliberate derivation and a reader
comparing the two trees should find the reason stated rather than inferred.

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

## Artemis Engine SDK — **proprietary, committed deliberately**

**Files:** `ports/artemis/vendor/artemis-sdk/` (9 files, 24 MB)

| File | Bytes |
| --- | --- |
| `Copyright.txt` | 20,717 |
| `WebAssembly/artemis.{html,js,wasm}` | 3,912 / 264,298 / 2,682,787 |
| `WebAssembly/asyncify/artemis.{html,js,wasm}` | 3,912 / 276,695 / 5,998,665 |
| `Android/arm64-v8a/libartemis.so` | 8,149,024 |
| `Android/armeabi-v7a/libartemis.so` | 7,395,052 |

**Upstream:** the official Artemis Engine SDK, by Mikage. Its own `Copyright.txt`
is reproduced verbatim at `ports/artemis/vendor/artemis-sdk/Copyright.txt`. It is
**not** a free licence: it grants use only to parties who obtained permission from
the author, and it charges a fee for commercial use. It is incompatible with this
repository's GPL-3.0-or-later.

**This is a known, accepted licence violation, not an oversight.** The material is
included because the port's central claims cannot be checked without it: the
official-wasm deployment route (`ports/artemis/build/official-web-deploy.py`)
reads these exact bytes as a drop-in for a licensed `.ref/artemis`, and the
reverse-engineering recipes under `ports/artemis/reverse/` assert byte-identity
and export-surface equality *against these files*. Removing them would turn those
assertions into unverifiable statements.

The decision and its reasoning are recorded in
[`docs/artemis-web-port.md`](docs/artemis-web-port.md) §15. A licensed
`.ref/artemis` takes precedence whenever one is present, so a properly licensed
user still builds against their own tree. Anyone redistributing this repository
should remove `ports/artemis/vendor/` unless they hold a licence from the author.

---

## Artemis Engine — `ports/artemis/src/` (rewritten from the official decompile)

**Files:** `ports/artemis/src/` — 159 files, ~71,000 lines of hand-written C++
(`namespace artc`), beside its style constitution `ports/artemis/REWRITE.md`
and its rename ledger `ports/artemis/rewrite-names.tsv`

**Derived from:** the proprietary SDK's WebAssembly builds, via the `wasm2c`
output in `.ref/artemis-official/`. Each product records its source range in its
own header, e.g. `// src: artemis.wasm2c.c 780308-780308 (f1700); R1 transcribe-keep.`

This is a **descent question, not a copying question**, and it is recorded here
rather than left implicit. Every file in that tree reproduces the control flow of
a function the official SDK compiled, so the provenance of the *behaviour* is the
proprietary binary above. What the rewrite removes is the artefact: it is
hand-written C++ in `artemis-compat`'s house style, renamed per the ledger,
with the generated driver's goto graph and pool addresses replaced, held to
`ports/artemis/REWRITE.md` and checked by `ports/artemis/reverse/style-gate.sh`.

The practical consequence is the same as for the SDK itself: **this tree cannot
be relicensed by this project, and it inherits the SDK's licensing risk.**
`ports/artemis/vendor/artemis-sdk/` and `ports/artemis/src/` stand or fall
together — redistributing one without the other does not remove the problem.
`REWRITE.md` §4 states the position taken at the time (the products are
re-expressed original code, not a transliteration of pseudocode, and no
"Adapted from artemis-compat" attribution is written into the sources); it also
records that the GPL-3.0 attribution question is left open.

The reverse-engineering recipes that produce and verify this tree live in
`ports/artemis/reverse/`; §16 of
[`docs/artemis-web-port.md`](docs/artemis-web-port.md) describes what each one
asserts.

