# Getting a working engine without compiling one

> **Status: implemented and verified on this machine.** `ports/lib/fetch-engine.mjs` downloads a
> published engine binary, checks its size and sha256 against `ports/lib/engine-artifacts.json`, and
> only then puts it where the runtime looks for it. Both halves of the point are met: someone who
> only wants to run the emulator needs no Emscripten SDK and no upstream checkout, and deploying the
> web page no longer recompiles a wasm that has not changed.
>
> The published artifacts themselves do **not** exist yet — there is no release and no tag on
> `github.com/fylcr/online_gal`. Until one is published, a fetch reports the artifact as unavailable
> (exit 2) and every caller falls back to building from source, which is the same behaviour as
> before this existed. The manifest is ready for the release; the release is what is missing.

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

Code 2 is the expected outcome today, and the message names the file, the URL it tried, and the
command that builds it from source instead. There is no stack trace and no silent success: a fetch
that cannot deliver says so and exits non-zero.

## Where the bytes come from

`ports/lib/engine-artifacts.json` records, per engine, the upstream repository and commit the
artifact was built from, the Emscripten version, and each file's exact size and sha256. The download
URL is `<repository>/releases/download/<releaseTag>/<name>`, with the tag convention
`engine-<engine>-<version>`. A future release can also publish its own manifest and be pointed at
directly, which is how the bytes' location changes without a code change here:

```bash
node ports/lib/fetch-engine.mjs ons --manifest https://example.invalid/engine-artifacts.json
```

Every hash in that file was computed from the bytes actually built and tested on this machine. It is
generated, not hand-maintained: **if a file is rebuilt, its hash changes and the entry must be
regenerated**, never guessed. A mismatch is the mechanism working, not a false alarm.

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
