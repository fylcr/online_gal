# Engine packages

`@online-gal/engines` is the aggregate entry point: one import that knows every engine the
platform ships, one call that turns a game directory into an engine choice, and one honest
answer when nothing can run the game.

It is not a new layer over the providers — the per-engine packages
(`runtime-kag`, `runtime-ons`, `runtime-siglus`, `runtime-artemis`, `runtime-tyrano`,
`runtime-web`) still own their own probes, decoders and runtimes. This package decides
*which* provider to believe and refuses to pretend.

## What it is for

Before this package, using the platform from outside `packages/core` meant importing six
runtime packages and re-deriving the registration order by hand — and getting it subtly wrong
in a way that is invisible until a game launches with the wrong engine. Consumers now have:

| need | API |
| --- | --- |
| "what engines exist, and which can actually run?" | `listEngines()` / `engineInfo(id)` |
| "what is this directory, and why can it not play?" | `probeAll(source)` |
| "give me the one engine for this game, or fail loudly" | `selectEngine(source)` |
| "start a session on whatever this game is" | `createRuntime({ source, host, options })` |
| "register the shipped engines into a registry" | `createEngineRegistry(extra?)` |
| "the raw provider list, in tie-break order" | `engineProviders()` / `ENGINE_PACKAGES` |

`createRuntime` is the one-call path: it probes, picks, and constructs, so a caller never has
to name an engine. The launcher still uses `Platform` (see `packages/core/src/platform.ts`),
which builds the same registry from the same list — the aggregate package is the import that
keeps that list in one place.

## The engines, and what each one is

Registration order is tie-break order. It is the same order `packages/core/src/platform.ts`
uses, and it is the reason a broad `web` match can never outrank a specific engine marker.

| priority | runtimeId | package | kind | inputs it recognises (`accepts`) | availability |
| --- | --- | --- | --- | --- | --- |
| 105 | `krkr2-wasm` | `runtime-kag` | engine | `.xp3`, `startup.tjs`, `Config.tjs`, `first.ks`, `System.tjs` | runnable |
| 100 | `krkr2` | `runtime-kag` | interpreter | same as `krkr2-wasm` | runnable |
| 95 | `onscripter-wasm` | `runtime-ons` | engine | `nscript.dat`, `nscript.___`, `nscr_sec.dat`, `onscript.nt2`, `onscript.nt3`, `.nsa` | runnable |
| 90 | `onscripter` | `runtime-ons` | interpreter | same as `onscripter-wasm` | runnable |
| 90 | `siglus` | `runtime-siglus` | shell | `Scene.pck`, `Gameexe.ini`, `Gameexe.dat` | **unavailable** |
| 85 | `artemis` | `runtime-artemis` | engine | `root.pfs`, `system.ini`, `boot.ini`, `system/first.iet`, `.pfs`, `.iet` | configuration-based |
| 80 | `tyrano` | `runtime-tyrano` | engine | `tyrano/`, `data/scenario/first.ks`, `tyrano/plugins/kag/kag.js` | runnable |
| 20 | `web` | `runtime-web` | engine | `index.html`, `js/rpg_core.js`, `js/rmz_core.js` | runnable |

`inputs` is each provider's own `accepts` list, exposed as `EngineDescriptor.accepts` — it is
what to check first when a game is reported as unknown. Note the two ONScripter rows and the
two KiriKiri rows: the interpreter and the engine of one title accept the *same* directory, so
`inputs` cannot distinguish them. The priority column is what breaks that tie.

`kind` is the distinction a consumer actually needs:

- **engine** — a real build of the engine, compiled to WebAssembly or running in a frame.
- **interpreter** — a TypeScript reimplementation of the engine's script interpreter. It
  recognises the same games as the engine above it and is the fallback when the engine cannot
  open them. The engine wins ties because a faithful compilation cannot drift from the
  original's behaviour the way a reimplementation can.
- **shell** — recognises its engine, reads its configuration, and has nowhere to execute it.

## The rule this package exists to enforce

**Identification is not delivery.**

Artemis and SiglusEngine probes work: a directory with `root.pfs` and `system.ini` is
recognised as Artemis with high confidence, and a Siglus directory as Siglus. Neither is
runnable out of the box, but not for the same reason, and the difference is the subject
of this repository's Artemis work:

- **artemis** — the provider is **configuration-based**, not empty. `packages/runtime-artemis`
  provides an independent asyncify session/host path: it instantiates a caller-configured asyncify
  WebAssembly bridge itself and renders into the host canvas, with the browser service layer
  (`runtime-artemis/src/browser-services.ts`) supplying the `EM_ASM` / `syncfs` / IndexedDB /
  video-element seams the guest reaches for, and real WebAudio for the OpenAL rows. Its
  `availability` getter (`runtime-artemis/src/runtime.ts:1613`) reads
  `configuredWasm === null ? 'unavailable' : 'runnable'` — so it is `unavailable` until a
  bridge is supplied, and becomes `runnable` the moment one is. Supply the deployment-wide
  fallback with `configureArtemisWasm({ wasm })`, or a per-game `options.gameConfig.wasm` /
  `wasmUrl`, which takes precedence for that one game. A registered
  `artemisEngineHosts` entry is the *legacy* injection seam for an embedder that already owns
  a bridge: the array is published compatibility surface and registering into it flips
  nothing, because availability is read from the configured binary and a session never routes
  through that list.
- **siglus** — there is no WebAssembly build of RFVP or `siglus_rs` at all
  (`runtime-siglus/src/runtime.ts`), so there is no configuration that makes it run.

Two things the Artemis row must **not** be read as claiming. The official SDK JavaScript is a
**diagnostic reference only, not the shipping path**: this runtime does not load, and does not
need, the SDK page, an iframe, `eval`, or the vendored SDK glue — the vendored bytes exist for
reverse-engineering and import-map checks. And Artemis is **not acceptance-tested**: no specific
title and no cold game from a downloaded artifact has been played through end to end, which is
why `describe()` reports `coldGameUnverified: true` and why the provider description says the
engine's compatibility with any particular title is untested. Configuration-based availability
means "this deployment supplied a bridge", never "this game is known to work".

`probeAll` therefore separates three things that are easy to conflate:

- `matches` — every engine that recognised the game, best first, including the ones that
  cannot run it. Hidden matches would be a lie in the other direction: the user should be told
  what their game was identified as.
- `best` — the highest-scoring match, whatever its availability. This is what a UI shows as
  "detected as X".
- `selected` — the best match that can *actually run*. `null` when there is none, together with
  a `blockedReason` the UI can display verbatim.

So a Siglus directory returns a confident Siglus match and a `selected: null`, not a
launch that fails on the first frame. `selectEngine` and `createRuntime` throw
`UnsupportedEngineError` in that case, carrying the recognised `runtimeId` and the runner-up
candidates, so a caller can offer an override without re-probing.

The rule holds in both directions:

- a `runtimeId` override for an engine that declares itself `unavailable` is refused, so a
  caller cannot launder an unrunnable engine into a session;
- a directory nothing recognises is reported as unknown, never as a low-confidence guess.

Availability is read from the provider on every call rather than cached in the descriptor,
because it is a property of the running deployment: the Artemis getter reads the live
`configuredWasm` binding, so calling `configureArtemisWasm()` (or withdrawing it with `null`)
flips these values on its own.

## Choosing the engine

Ranking is the registry's own chain — score, then priority, then registration order — so this
package never invents a second way to pick an engine and cannot disagree with `RuntimeRegistry`.

One invariant worth stating, because it is easy to get wrong: **the engine is chosen from
exactly the provider set the session will be created from.** `createRuntime` probes
`registry.list()`, not the shipped `ENGINE_PACKAGES`, so a caller that registered a replacement
provider gets that one, and can never be handed a runtime id its registry cannot construct.

```ts
import { createEngineRegistry, createRuntime, probeAll } from '@online-gal/engines';

const report = await probeAll(source);
if (!report.selected) {
  showReport(report.best?.displayName ?? 'unknown', report.blockedReason);
  return;
}

const started = await createRuntime({
  source,
  host,
  options: { surface, mode: 'new' },
  registry: createEngineRegistry(),
});
await started.instance.load();
```

`probeAll` accepts a `ProbeContext`, so a caller that already has one (a hot directory, a
re-probe after a failed launch) reuses the cache instead of re-reading the game.

## Consuming it, including the CI tarballs

Inside the monorepo, add the workspace dependency and import it:

```jsonc
// apps/app/package.json
"dependencies": { "@online-gal/engines": "workspace:*" }
```

Outside it, the CI-produced tarballs are the interface. `.github/scripts/make-package.mjs`
stages any package into a standard npm tarball with its dependency closure vendored in, and the
release URL is the dependency:

```jsonc
{
  "dependencies": {
    "@online-gal/runtime-ons": "https://github.com/fylcr/online_gal/releases/download/engine-ons-0.7.7beta/online-gal-runtime-ons-0.1.3.tgz",
    "@online-gal/runtime-kag": "https://github.com/fylcr/online_gal/releases/download/engine-krkr-20260920/online-gal-runtime-kag-0.1.1.tgz",
    "@online-gal/runtime-artemis": "https://github.com/fylcr/online_gal/releases/download/engine-artemis-bridge-asyncify-9f62262/online-gal-runtime-artemis-0.1.5.tgz",
    // The aggregate, from the workflow that builds packages and nothing else. It carries all six
    // runtimes at once, which is why it needs its own trigger: no engine workflow builds all six,
    // and the packer drops `dependencies`, so a stale vendored provider would be invisible to the
    // consumer. `build-packages.yml` builds every package from source, so its ten vendored
    // directories are all rebuilt in the same job that packs them.
    "@online-gal/engines": "https://github.com/fylcr/online_gal/releases/download/engine-packages-0.1.6/online-gal-engines-0.1.6.tgz"
  }
}
```

Those are the URLs `packages/engines/package.json` (0.1.6) and `packages/runtime-artemis/package.json`
(0.1.5) would publish on the next release — the live build to target, not a claim that these exact
URLs resolve yet. Three earlier versions *are* published today: `runtime-ons` 0.1.3 under
`engine-ons-0.7.7beta`, `runtime-kag` 0.1.1 under `engine-krkr-20260920`, and the aggregate **0.1.5**
under `engine-packages-0.1.5` — which is the tag `build-packages.yml` derives from the package
version, so a version bump is what publishes it. So the aggregate line above describes the *next*
release: bumping `packages/engines` to 0.1.6 and pushing is what creates `engine-packages-0.1.6` and
makes that URL live. The artemis URL is the exception in a second way: it is what the
*next* build of `build-wasm-artemis-bridge.yml` will publish, under the release tag that workflow
already uses (`engine-artemis-bridge-asyncify-9f62262`), and that workflow only publishes when
`publish` is set or `vars.ARTEMIS_PUBLISH_ARTIFACTS` is `true`, because that engine is deliberately
withheld while its SDK terms are unsettled. It is not cleared to publish yet, so treat that one as
"present after the push that sets `package-dir` and the flag", not as something to paste into a
build today. The aggregate's own 0.1.0 was published with the LF checkout form of `onsyuri.html` and
then withdrawn, because the aggregate is the one tarball that vendors the committed copy rather than
a fresh build (see the note on `packages/runtime-ons/assets/**` in `.gitattributes`).

That withdrawal is why the packer now checks the engine ledger itself. `ports/lib/engine-artifacts.json`
pins every engine artefact by size and sha256, and those hashes check the bytes that ship — but they
only mean something if something compares them, and for a while nothing did. So `make-package.mjs`
verifies every ledger entry that is committed anywhere under the engine-asset directories before it
stages anything, and exits `2` naming the file, the two digests, the CR count and the fix. The rule
it enforces is worth stating once: **the ledger is over the committed bytes, not the worktree's**, so
`packages/runtime-ons/assets/**` is marked `-text -diff` in `.gitattributes` — an engine artefact is a
build output whose hash-pinned bytes *are* the substance, and `* text=auto eol=lf` must not get to
decide them. If you change them, commit the bytes **and** bump the version of every package that
vendors them. The check is skipped for ledger entries that exist only as build output (the six krkr
files), and `--json` reports `ledgerChecked` so a run that checked nothing is visible rather than
looking like a run that checked everything.

Every tarball published before `8482272` also carried a few `tsconfig.tsbuildinfo` files — `tsc -b`'s
incremental build cache, which the packer now excludes. It records the sha256 of each source file
*as the compiler read it*, i.e. before CRLF is normalized to LF, so a CRLF checkout produced caches
describing text that was no longer in the archive, and no normalization pass could repair a digest of
bytes that were already gone. A CRLF and an LF checkout of the same commit therefore gave tarballs
of identical length that still differed in those 2–10 files and in nothing else. They are gone: the
current *published* releases (`runtime-ons` 0.1.3, `runtime-kag` 0.1.1, `engines` 0.1.5) each contain **0** of
them, and a CRLF checkout of `8482272` now packs to the same `2,553,631` B /
`e674dfd15b7768f0…` as the LF one. Their sizes and digests are in `fetch-engine-artifacts.md`.

A tarball per engine is the right default here, because the engine workflows are the thing that
actually build these packages and they build one engine each. Note that only the ONS tarball carries
its engine bytes: `@online-gal/runtime-kag` and `@online-gal/runtime-artemis` load their engine from
a URL the caller supplies, so adding `--assets-from` to those workflows would put megabytes in the
archive that no code path reads.

`@online-gal/engines` gets its own workflow, `.github/workflows/build-packages.yml`, and that file
is the whole reason the aggregate can be published at all. It is not another engine workflow: it
installs dependencies, runs `pnpm run build:packages`, and packs — no Emscripten, no vcpkg, no
upstream checkout, well inside three minutes. What makes that correct rather than merely fast is
that it rebuilds *all* the packages the aggregate vendors, in the same job, before packing, so the
closure cannot be stale. It publishes only `online-gal-engines-<version>.tgz`; the per-engine
tarballs stay as workflow artifacts, because publishing those from here too would put two
byte-different archives of one version in two different releases. The consumer still picks engines
the same way — the tarball ships the compiled `dist/`, and the ten vendored packages come in as
its `node_modules/`, so `import { selectEngine } from '@online-gal/engines'` works without
compiling anything locally. See `docs/fetch-engine-artifacts.md` for the tarball format, the
reproducibility guarantees, and the token a consumer needs on this private repository.

One consequence of that vendoring is worth knowing before it surprises someone. The aggregate
package's copy of `runtime-ons` is the one committed in this repository, engine bytes included, and
those four asset files are byte-identical to what `ports/lib/engine-artifacts.json` records today
(`onsyuri.html` 24,399 B, `onsyuri.js` 198,947 B, `onsyuri.wasm` 4,660,610 B). So the two packages
agree as things stand. They take the bytes from different places, though: the aggregate has no
`--assets-from`, so it takes whatever is committed, while `@online-gal/runtime-ons` takes the fresh
build. Rebuild ONS and then either commit the new assets or bump both packages together — otherwise
the aggregate would keep shipping the previous engine under a version that claims otherwise.

That rule generalises. `build-packages.yml` freezes ten vendored directories, so **any change to a
vendored package, or to ONS's `assets/`, has to bump `packages/engines`'s version in the same
commit.** The version is what a consumer's `pnpm add` line points at, so a change to any part of
the closure without a bump is a lie about identity: the URL would still serve the previous engine.
The workflow carries the same rule in its comment, and the first release is 0.1.0.

### The aggregate is installed before it is published

Everything above is a promise about a tarball. A tar step and a digest cannot tell whether the
archive is *installable*, because a perfectly reproducible archive can still be unusable: a package
missing from the vendored closure, an import that only resolves because this monorepo happens to be
sitting next to it, a `dist/` that was never compiled. So `build-packages.yml` runs
`.github/scripts/smoke-consumer.mjs` on the packed `.tgz` — after the pack step, before the upload
and before the release — and the step fails the job if any of it does not hold:

```
node .github/scripts/smoke-consumer.mjs dist/packages/online-gal-engines-0.1.6.tgz
```

The script creates a consumer directory **outside the repository** (under `RUNNER_TEMP` on a
runner, `os.tmpdir()` elsewhere), writes a minimal `package.json`, runs
`pnpm add file:… --offline --ignore-scripts`, and then asserts that the installed package is the
thing this document claims it is:

- the consumer's top level has exactly one `@online-gal/` package, `engines`;
- the installed manifest declares no `dependencies`, so nothing can be satisfied from outside the
  archive;
- `listEngines()`, `engineProviders()` and `ENGINE_PACKAGES` each report all eight runtimes, with
  no duplicate runtime id;
- all ten vendored packages resolve *and* `import()` **from inside the archive**, while the
  consumer's own top level cannot resolve any of them (`MODULE_NOT_FOUND`) — the executable form of
  "a vendored copy is the only copy";
- `@online-gal/runtime-artemis`'s `dist/host/{boot,eh,runtime,asyncify,fd,bridge}.js` all
  deep-import, which is the part of the public surface a flat `exports` map does not cover;
- the repository's `pnpm-lock.yaml` is unchanged afterwards.

A vendored copy is the *source* directory, so it keeps its own manifest: `scripts.build`, and a
`dependencies` block whose specs are `workspace:*`. That is not an oversight — resolution inside the
archive is by directory, because the archive already contains the whole `node_modules` tree, so
nothing ever asks a registry to satisfy those specs. It does mean an install run *inside* an
extracted aggregate is not supported: the paths are laid out, not resolvable. `engines`' own shipped
manifest is the one the packer rewrites (its `dependencies` and `scripts` are dropped), because that
is the manifest a consumer's installer reads.

Every assertion prints the value it actually saw, and the run ends with a `N/M assertions held` line
and a non-zero exit if any of them did not hold — so a green step says what it checked and a red one
says what it found, instead of a digest that only proves the bytes did not change.

`--offline` and `--ignore-scripts` are deliberate, not defensive. The aggregate has no
`dependencies` and its whole closure is vendored, so a correct archive must install with no registry
reachable — a version that starts reaching for the network is the regression. And a published
package's install must not be able to run code, so if a future dependency needs a build step that
has to be a reviewed change rather than something a consumer finds out about.

The lockfile assertion is not ceremony. A `pnpm install` below this workspace root is treated as a
workspace member and **rewrites the repository's `pnpm-lock.yaml`**, which is why the consumer
directory has to be outside the repository. To reproduce the hazard and watch the guard fire:

```
node .github/scripts/smoke-consumer.mjs dist/packages/online-gal-engines-0.1.6.tgz \
  --allow-in-repo-temp --temp-dir .test/consumer-check
# ::error::the repository lockfile is not dirty: got  M pnpm-lock.yaml; expected (empty)
git checkout -- pnpm-lock.yaml
```

If you see that error without meaning to reproduce it, `git checkout -- pnpm-lock.yaml` and make sure
the consumer directory is not inside the checkout. The flag exists so this can be demonstrated; it
is not a supported mode.

### GPL-3.0-or-later

Every package in the workspace — including this one — is `private: true` and licensed
`GPL-3.0-or-later`, matching the repository `LICENSE`. That is not bookkeeping: the engines
whose binaries ship in these tarballs are themselves derived works, and a project that installs
one is a combined work.

So a consumer that installs any of these packages or tarballs must:

- keep its own release under GPL-3.0-or-later, and
- ship the corresponding source for the engine artifacts it redistributes.

`make-package.mjs` preserves the `license` field verbatim in the shipped manifest rather than
rewriting it, so the obligation travels with the tarball. Nothing in this package narrows it:
the aggregate package adds no engine code of its own, it only orders the ones that already
carry their own obligations.

## Testing

`.test/engines.test.ts` covers the contract rather than any single provider's probe —
many providers matching one directory, the ranking chain, unknown directories, recognise-only
engines, the refusal of an unavailable override, and the registry's duplicate-id check. The
per-engine probes are covered in their own suites (`.test/dispatch.test.ts`,
`.test/siglus.test.ts`, `.test/artemis.test.ts`, `.test/ons-wasm-provider.test.ts`,
`.test/kiri-wasm-provider.test.ts`).

The suite runs against `packages/engines/dist/`, so build the package first:

```
npx tsc -b packages/engines/tsconfig.json
node --test .test/engines.test.ts
```

Note when writing tests against this package: import everything from
`packages/engines/dist/index.js`, which re-exports `@online-gal/engine-api`. Importing the two
entries by separate relative paths loads two copies of the module, and `instanceof` against
`RuntimeError` / `UnsupportedEngineError` then fails for a reason that has nothing to do with
the code under test.
