# Getting a working engine without compiling one

> **Status: implemented, published, and verified end to end.** `ports/lib/fetch-engine.mjs` downloads
> a published engine binary, checks its size and sha256 against `ports/lib/engine-artifacts.json`, and
> only then puts it where the runtime looks for it. Both halves of the point are met: someone who
> only wants to run the emulator needs no Emscripten SDK and no upstream checkout, and deploying the
> web page no longer recompiles a wasm that has not changed.
>
> The artifacts are now really published by the CI workflows in `.github/workflows/`, and the whole
> loop has been exercised against them: the tag the fetch helper asks for is the tag the build
> publishes, the bytes it downloads match the manifest, and a real browser run of a fetched engine
> renders the same frame as a locally built one. `krkr` and `ons` have releases
> (`engine-ons-0.7.7beta`, `engine-krkr-20260920`); `artemis` deliberately does not, because its
> engine is built against a proprietary SDK.

## This repository is private, so fetching needs a token

`github.com/fylcr/online_gal` is **private**, and a release asset in a private repository answers an
unauthenticated request with `404` — indistinguishable from "there is no such release". So a fetch
tries one of two ways:

1. the plain release URL, with `Authorization: Bearer <token>` when a token is set;
2. if that returns `404` **and** a token is set, the API route
   (`/repos/{owner}/{repo}/releases/tags/{tag}` → the asset id → `Accept: application/octet-stream`).

The token is read from the environment as `GITHUB_TOKEN` or `GH_TOKEN`, never from a command line:
an argument ends up in the process table, in shell history and in CI logs. With no token the plain
path still runs, so a public repository behaves exactly as before, and a private one reports the
artifact as unavailable (exit 2) and falls back to a source build rather than reporting a corrupt
download.

```bash
GITHUB_TOKEN=ghp_... node ports/lib/fetch-engine.mjs ons
```

## Why it works this way

Building an engine needs three things a stranger does not have: an Emscripten SDK, an upstream
checkout that this repository deliberately does not commit (the `.ref/` directory is gitignored, and
in the case of Artemis it is a multi-gigabyte decompiled dump), and around twenty minutes of CPU.
None of that is a reasonable price for "run the game", and re-paying it on every web deploy is worse.

The one new risk a download introduces is that an unchecked binary is worse than no cache: the
engine loads, misbehaves in a way that looks like an emulation bug, and the real cause — a truncated
download, a stale release, an artifact built from the wrong commit — is invisible. So the helper
verifies before it installs, and a download that fails verification leaves nothing behind.

**An engine is a set, not a list.** `onsyuri.js` and `onsyuri.wasm` are two halves of one module, and
a directory holding a new `.js` beside a stale `.wasm` loads and then fails in whatever the mismatched
pair happens to do — a failure that looks like an engine bug and is not one. So the fetch has two
phases: every file is downloaded and verified first, touching nothing, and only then are they moved
into place together. A failure anywhere in the list leaves the *previous* engine completely intact
rather than a half-updated one. The observable states are "all previous" and "all new", never a mix.

## The three engines

| Engine | Files | Installed to | Tracked? |
| --- | --- | --- | --- |
| `ons` | `onsyuri.html`, `onsyuri.js`, `onsyuri.wasm` | `packages/runtime-ons/assets/` | **yes** — committed source |
| `krkr` | `index.wasm`, `index.js`, `vlfs.js`, `index.html`, `shell.html`, `assets.zip` | `.ref/kirikiroid2-web/out/web/release/` | no |
| `artemis` | `artemis-web-android.wasm`, `artemis-web-android.js` | `ports/artemis/build/out/web-android/` | no |

Two of the three land in gitignored build directories, which is deliberate: a downloaded binary is a
build product and must not be committed by accident. `ons` is the exception — the engine the emulator
ships with is tracked so that it is reviewable like any other change, and a fetch there shows up in
`git status` for exactly that reason.

**The `krkr` set is the engine, not a complete web root.** Upstream's release directory also contains
`manifest.webmanifest`, `sw.js`, `robots.txt`, `sitemap.xml` and `pwa/icon-{192,512}.png`, copied
verbatim from `platforms/web/` by `CMakeLists.txt:214-251`. None of them is compiled, so none of them
belongs to the reason this list exists, and they are not published. The consequence is concrete and
bounded: pointing a server at the fetched six gets a working engine — the wasm instantiates, renders
and mounts `assets.zip` — while the page's PWA manifest and service worker 404. `sw.js` precaches the
two icons with `cache.addAll`, so a complete deploy needs all of them, and it has to place the icons
under `pwa/`: GitHub release assets are flat names, so a nested fetch destination would need its own
mechanism in both the publish step and `fetch-engine.mjs`. That work is deliberately not done here.
Anyone deploying KiriKiri needs those files from upstream `platforms/web/` alongside the fetched set.

## Fetching

```bash
node ports/lib/fetch-engine.mjs ons          # one engine
node ports/lib/fetch-engine.mjs krkr --json  # machine-readable result on stdout
node ports/lib/fetch-engine.mjs artemis --out /tmp/elsewhere
```

It is idempotent: a file that is already present and matches the manifest is left alone, and every
file is reported as either `verified` (was already good) or `downloaded` (was missing or wrong).
`--force` re-downloads even when the copy on disk is good.

Exit codes, because scripts branch on them rather than parsing output:

| Code | Meaning |
| --- | --- |
| `0` | the artifact is in place and verified |
| `1` | bad usage — unknown engine, unknown flag, missing argument |
| `2` | the artifact is unavailable: not present locally and not downloadable |
| `3` | verification failed — a size or sha256 mismatch; **the engine was left exactly as it was** |
| `4` | the manifest is missing, unreadable, or has no entry for this engine |

Code 2 is the expected outcome for `artemis`, whose engine is not published, and for any engine on a
machine with no network. The message names the file, the URL it tried, and the command that builds it
from source instead. There is no stack trace and no silent success: a fetch that cannot deliver says
so and exits non-zero.

## Where the bytes come from

`ports/lib/engine-artifacts.json` records, per engine, the upstream repository and commit the
artifact was built from, the Emscripten version, and each file's exact size and sha256. The download
URL is `<repository>/releases/download/<releaseTag>/<name>`, with the tag convention
`engine-<engine>-<version>`. A future release can also publish its own manifest and be pointed at
directly, which is how the bytes' location changes without a code change here:

```bash
node ports/lib/fetch-engine.mjs ons --manifest https://example.invalid/engine-artifacts.json
```

Every hash in that file was computed from the bytes of a real, tested build of the pinned upstream
commit. It is generated, not hand-maintained: **if a file is rebuilt, its hash changes and the entry
must be regenerated**, never guessed. A mismatch is the mechanism working, not a false alarm.

**Which build the hashes describe matters, and it is the producer's.** Emscripten's `DETERMINISTIC`
setting no longer functions in the 6.0.9 SDK this project uses, so a build's function order, its
data-segment order and the JS `dynCall_*` ordering all depend on the host that compiled it. Real
numbers: the same ONS sources give `onsyuri.wasm` at 4,660,610 bytes on a Linux runner and 4,660,612
bytes on a Windows machine, with a different function-index permutation — behaviourally identical
(same rendered frame, same shim state in a real browser) but a different hash. Four independent
runner builds were byte-identical to each other, so the runner is the reproducible party and the
manifest follows it. Pinning a developer's machine instead would make every consumer's verification
fail and silently fall back to a source build, which is exactly what this feature exists to avoid.

`krkr` is the same decision with a bigger spread, for a second reason. Its Windows recipe configures
vcpkg with `-DVCPKG_HOST_TRIPLET=x64-mingw-static` and the runner takes the Linux default, so
host-side ports compile differently and the runner's module carries two more functions — about 17 KB
more code and data out of 22.6 MB. What decides behaviour is identical either way: `vlfs.js`, which
carries the host byte-source seam, is byte-identical to the tested Windows build; `index.html` and
`shell.html` become identical once CRLF and the version string are normalised; and the seam patcher's
two probes are asserted during the CI build. Line endings are worth naming here because they are the
first thing that looks like a real difference and is not one: a Windows checkout writes CRLF, so
`index.html` differs by exactly its 21 carriage returns until you normalise.

The same reasoning puts a constraint on the *build*: any input that varies per run makes the bytes
unpinnable, and there turned out to be two such inputs, not one. The first was
`KRKR2_BUILD_VERSION` — a run id, and upstream’s own default of a timestamp. It changes the `?v=`
cache-buster in `index.html` and the version compiled into the module on every run, so
`.github/scripts/build-krkr.sh` passes the checkout’s own commit instead. The second was the wall
clock, and it survived the first fix: two consecutive runs over identical sources produced
`index.wasm` and `assets.zip` with identical sizes and different sha256. The wasm differed in 17
bytes, all inside the data segment — OpenCV’s build-info `Timestamp:` (vcpkg compiles OpenCV from
source during configure) and `__DATE__`/`__TIME__` in `cpp/core/base/MsgIntf.cpp` and
`cpp/core/tjs2/tjs.cpp` — and the zip differed only in its 75 entry timestamps, because
`cmake/CreateStoredZip.py` encodes each source file’s mtime and a fresh clone’s mtimes are all the
checkout time. The script now exports `SOURCE_DATE_EPOCH` (the upstream commit’s own time) along
with `TZ=UTC` and `VCPKG_KEEP_ENV_VARS=SOURCE_DATE_EPOCH`, and normalises the UI assets’ mtimes.
Only `krkr` needs any of this: it is the one engine whose tree carries a `string(TIMESTAMP)` or a
`__DATE__`, the one that ships a zipped asset bundle, and the one whose wasm embeds a port that
vcpkg builds at configure time.

**The vcpkg binary cache is back, and it is safe for a reason that was measured rather than argued.**
Building `krkr` used to cost about an hour on a runner, and most of it was vcpkg compiling OpenCV
from source — 38 of the 48 minutes, in the run that measured this. It was deferred once, for the
reason above: OpenCV writes its `Timestamp:` when OpenCV
itself is configured, so a cached `libopencv_core4.a` carries the timestamp it had when it was
cached, and a miss or an eviction could then change `index.wasm`’s hash while every source stayed
identical. What reopened the question was `lukka/run-vcpkg`’s own cache turning out to store nothing
at all: the repository’s cache listing held exactly one entry, the emsdk tree, after every run, and
the Build step’s environment listed neither `ACTIONS_CACHE_URL` nor `ACTIONS_RUNTIME_TOKEN`. So
`build-krkr.sh` now points `VCPKG_BINARY_SOURCES` at a `files` directory that
`build-wasm-reusable.yml` saves with `actions/cache`, and two runs over the same commit bracket the
change: the first went in with `0` entries, spent 38m01s in configure and left 117 behind; the
second restored those 117 and finished configure in 12s. The six published files were identical by
sha256 on both sides, and identical to the run from before the cache existed — a hit and a miss
really do produce the same bytes.

Then the key itself changed, and the bracket was re-run over the key the repository actually ships
rather than assumed to still hold. Run `36369745639` reported `Cache not found for input keys:
…-13dda190f837…`, entered configure with `0` entries, took 3022s and saved 117; run `36374373996`,
the same commit dispatched by hand, restored that key, reported `## vcpkg binary cache entries on
entry: 117` and finished configure in **12s**. Both produced the same six sha256 values as each other
and as this manifest. So the extra key component costs exactly one cold rebuild and changes no byte.

The key names the vcpkg commit, the upstream commit and the hash of the dependency manifests. The
upstream commit is in there because it is what `SOURCE_DATE_EPOCH` is derived from, and the cached
archives embed that timestamp: a moved pin starts from an empty directory and rebuilds under the new
epoch instead of restoring archives that carry the previous one. That is the precondition this
paragraph used to ask for, met by the key rather than left to vcpkg’s own ABI hash.

## Forcing a build from source

The published artifact is the verified copy; a local build is merely the copy that is available right
now. When you are changing the engine, the artifact is by definition older than your edit, so build:

```bash
bash ports/ons/build/fetch-web-engine.sh --from-source   # ONS
bash ports/artemis/build/web-deploy.sh --from-source     # Artemis
bash ports/krkr/kirikiroid2/run-build.sh                 # KiriKiroid2
```

`ports/ons/build/fetch-web-engine.sh` is the ONS entry point and its default is fetch-then-build, so
it works on a machine with nothing and on a machine with everything. The Artemis equivalent is
`ports/artemis/build/web-deploy.sh`, which installs the verified artifact beside the game's pack
files and only compiles when no artifact is available — the two scripts are the model the CI job
follows.

One asymmetry worth knowing when you rebuild ONS by hand: `ports/ons/build/run-web-build-shimmed.sh`
is the real build recipe, not upstream's. It force-includes the host shim that makes input injection
work and restores the patched checkout via `trap ... EXIT`. A plain upstream build produces an engine
whose input does not function.

After a source build the manifest is stale by construction, so the wrapper prints the new sizes and
hashes rather than writing them. Updating the manifest is a separate, deliberate step taken **after**
the build has been tested — silently rewriting it would let an untested build claim a verified hash.

## Verifying a fetch

```bash
node ports/lib/fetch-engine.mjs ons --json | grep -A3 '"action": "downloaded"'
```

To check the manifest against the files on disk without network access, run the helper for an engine
whose files are already present: every file reports `already present and verified`, and a mismatch
would be replaced or reported as a failure rather than passed over.

## Relationship to the rest of the tree

- `ports/lib/ref.mjs` is the shared path helper; the fetch script resolves the repository root
  through it rather than counting `..` segments of its own.
- `packages/runtime-ons/scripts/copy-runtime-assets.mjs` is the only thing that moves ONS assets into
  `dist/`; a fetch writes the tracked files in `assets/` and the existing build step does the rest.
- `apps/app/astro.config.mjs` serves the KiriKiroid2 build directory through middleware rather than
  copying it into `public/`, so a fetch into `.ref/kirikiroid2-web/out/web/release/` is picked up by
  the next dev-server request with no rebuild.
