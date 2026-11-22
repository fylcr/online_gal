# Clean Artemis Web platform recipe

Source-only lane A; lane B platform files deliberately do not exist yet. This is
not a compiled engine, delivered candidate, or animation fix. See
`docs/artemis-compat-clean-base.md` in the project for the serial ownership board.

`source-manifest.json` is the sole upstream identity, replacement whitelist and
export list. The observed main commit is unchanged from the old pin; a fresh
base is not a newer version. Builds never read `.ref`, `D:/artemis-compat`, old
port outputs, overlays, patches 0001–0006 or R1–R4. No upstream render/script/core
file is patched or replaced. The only replacements are Audio::Impl and PF8 I/O.

After B/C source review and D authorization (not run in lane A):

```sh
ARTC_CLEAN_CANDIDATE=baseline-01 bash ports/artemis/compat-web-clean/build.sh --parallel 2
```

Each candidate is an exclusive `out/<name>/` directory. Existing output is
refused; no automatic cleanup. Only an otherwise empty, clean fixed-HEAD
`gitrepo` cache may be resumed (`ARTC_CLEAN_OFFLINE=1` refuses network). New
online caches clone/fetch the manifest commit, then archive it into a newly
created absolute guarded `source/` directory. Extraction never targets the
repository root or out root. This recipe has not been executed.

SDK discovery uses the unchanged `ports/env.sh`, or explicit `EMSDK_DIR`.
Any `EM_CONFIG`, missing SDK or SDK sysroot zlib stops the recipe; it does not
install ports or dependencies. Vendored Lua 5.1.5, core and final link use
`-fwasm-exceptions`. GLES interface selection avoids ANGLE; CLI/JNI/mac/tests
and FFmpeg are disabled. Upstream stb_truetype font rendering remains intact.
WebGL2 with FULL_ES2 supports the existing client-array renderer; growing heap,
64MiB initial / 4GiB max and 4MiB stack are independent platform settings, not
frame policy. The classic worker requires OffscreenCanvas, WebGL2, wasm EH and
cross-origin isolation for its 4MiB range SAB; no pthreads or internal rAF.

The source bundle contains the unmodified archived upstream, LICENSE and
THIRD_PARTY_NOTICES.md, complete new host/platform/audio, all recipe files,
identity and precompile byte inventories. It refuses changed source/port bytes
and does not contain only WASM. Offline CMake reconstruction instructions are
included. JS/WASM pair copying, package/factory/page/ledger wiring belongs to D,
not this recipe. No runtime or old entry point is modified here.

Project-authored code is GPL-3.0-or-later; upstream notices remain in the bundle.
Official Artemis SDK and pseudo-C have separate proprietary redistribution
risks; neither is an input or included text. Building and source delivery do
not prove game usability, animation behavior or redistribution clearance.
