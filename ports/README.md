# ports

Build recipes and browser harnesses for the three emulated engines.

Everything here drives an **upstream** engine checkout plus an Emscripten SDK, and both live
outside this repository. `ports/env.sh` (shell) and `ports/lib/ref.mjs` (JavaScript) resolve
them, so no recipe contains a hardcoded absolute path: a clone builds wherever it lands.

| directory | engine | upstream | status |
| --- | --- | --- | --- |
| [`ons/`](ons/) | ONScripter | [onscripter-yuri](https://github.com/YuriSizuku/OnscripterYuri) | runs real games in the browser |
| [`krkr/`](krkr/) | KiriKiri | [kirikiroid2-web](https://github.com/fenghengzhi/kirikiroid2-web) | runs real XP3 games in the browser |
| [`artemis/`](artemis/) | Artemis | [artemis-compat](https://github.com/Weiss-UltimateSavior/artemis-compat) | runs real PFS games in the browser |

`krkr/krkrsdl2/` is the earlier KiriKiri attempt (krkrsdl2 + krkrz). It builds and runs but was
superseded by `krkr/kirikiroid2/`; it is kept because its diagnosis of why the synchronous-XHR
ranged stream fails is still the reference for that problem — see
[`docs/kirikiri-web-plan.md`](../docs/kirikiri-web-plan.md).

## Layout

    ports/
      env.sh                        shared shell environment (repo root, .ref, emsdk, path helpers)
      lib/ref.mjs                   shared ES-module equivalent, for the *.mjs patchers
      <engine>/
        build/                      the build recipe: scripts, patchers, shims
        harness/                    the page + worker that drive the built engine in a browser
        engine/                     patches to apply to the upstream checkout (artemis only)

## Getting the upstream checkouts

They are **gitignored** — not ours to redistribute, and several GB in total. Fetch them into
`.ref/` (or anywhere; every script takes an override):

| path | repository | pinned commit |
| --- | --- | --- |
| `.ref/onscripter-yuri` | `https://github.com/YuriSizuku/OnscripterYuri` | see `docs/ons-source-build.md` |
| `.ref/kirikiroid2-web` | `https://github.com/fenghengzhi/kirikiroid2-web` (`web` branch) | see `docs/kirikiri-web-plan.md` |
| `.ref/krkrsdl2` | `https://github.com/ammaraskar/krkrsdl2` | see `docs/kirikiri-web-plan.md` |
| `.ref/artemis-compat` | `https://github.com/Weiss-UltimateSavior/artemis-compat` | `a4f8a257cf87cb8f65ab4e3f83501ab327a55424` |
| `.ref/vcpkg` | `https://github.com/microsoft/vcpkg` | KiriKiri only; the CI pins it |

Artemis additionally needs its engine patch applied — the upstream project has no web target at
all, so `ports/artemis/engine/0001-web-host.patch` adds the CMake target, the pthread/GLES2 host,
and four real bug fixes:

    cd .ref/artemis-compat
    git checkout -- . && rm -f src/host/web_{main.cpp,page.html,worker.js}
    git apply /path/to/ports/artemis/engine/0001-web-host.patch

## The Emscripten SDK

`ports/env.sh` finds it by looking for the `emcc` driver, in this order: `EMSDK_DIR`, `/d/emsdk`,
`.ref/emsdk`, `~/emsdk`, `C:/emsdk`. A directory that merely *exists* is not enough — an
interrupted `emsdk install` leaves a tree with no compiler in it, and the search skips those.

emmcripten 6.0.9 is the version the KiriKiri build enforces with an exact check.

## Game data

Never committed. Real game files are pointed at by environment variable (`ARTEMIS_GAME_DIR`,
`ONS_GAME_DIR`, `KRKR_GAME_ROOT`, …) or passed as an argument; each recipe's default keeps them
under `.ref/`. The Windows-form path is preserved where a tool needs it — see `ports_win` in
[`env.sh`](env.sh).

## Licences

This repository is GPL-3.0-or-later. What the ports may and may not take from each upstream is
tabulated in the root [`README.md`](../README.md) and [`THIRD_PARTY_NOTICES.md`](../THIRD_PARTY_NOTICES.md);
the short version is that upstream GPL-3.0-or-later sources may be reused, GPL-2.0-**or-later**
sources (OnscripterYuri, Tyranor-Next) fold into GPL-3.0 as well, and MPL-2.0 components
(siglus_rs) may be combined with their notice kept.
