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
