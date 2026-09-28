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
| 85 | `artemis` | `runtime-artemis` | shell | `root.pfs`, `system.ini`, `boot.ini`, `system/first.iet`, `.pfs`, `.iet` | **unavailable** |
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
recognised as Artemis with high confidence, and a Siglus directory as Siglus. Both advertise
`availability: 'unavailable'`, but not for the same reason, and the difference is the subject
of this repository's Artemis work:

- **artemis** — a WebAssembly build of the engine *does* exist here (`ports/artemis/`), but the
  runtime package has no browser host registered: `runtime-artemis/src/runtime.ts` reports
  `unavailable` exactly while `artemisEngineHosts` is empty. Registering a host flips it.
- **siglus** — there is no WebAssembly build of RFVP or `siglus_rs` at all
  (`runtime-siglus/src/runtime.ts`).

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
because it is a property of the running host: when a build is registered, these values flip on
their own.

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
    "@online-gal/runtime-ons": "https://github.com/fylcr/online_gal/releases/download/engine-ons-0.7.7beta/online-gal-runtime-ons-0.1.2.tgz",
    "@online-gal/runtime-kag": "https://github.com/fylcr/online_gal/releases/download/engine-krkr-20260920/online-gal-runtime-kag-0.1.0.tgz",
    "@online-gal/runtime-artemis": "https://github.com/fylcr/online_gal/releases/download/engine-artemis-bridge-asyncify-9f62262/online-gal-runtime-artemis-0.1.0.tgz"
    // No "@online-gal/engines" entry yet, on purpose. A tarball exists only for a package whose
    // workflow sets `package-dir`, and this one is published by no workflow -- it depends on all six
    // runtime packages at once, and no engine workflow builds all six, so any tarball published
    // today would vendor five stale providers. It builds and installs correctly by hand; it just
    // has no correct trigger. See the section in docs/fetch-engine-artifacts.md.
  }
}
```

The first URL resolves against a published release today. The second and third are what the *next*
build of those workflows will publish, under the release tags those workflows already use
(`engine-krkr-20260920`, `engine-artemis-bridge-asyncify-9f62262`) — so treat them as "present after
the push that adds `package-dir`", not as something to paste into a build today. The artemis one has
a second condition on top: `build-wasm-artemis-bridge.yml` only publishes when `publish` is set or
`vars.ARTEMIS_PUBLISH_ARTIFACTS` is `true`, because that engine is deliberately withheld while its
SDK terms are unsettled. It is not cleared to publish yet.

A tarball per engine is the right default here, because the engine workflows are the thing that
actually build these packages and they build one engine each. Note that only the ONS tarball carries
its engine bytes: `@online-gal/runtime-kag` and `@online-gal/runtime-artemis` load their engine from
a URL the caller supplies, so adding `--assets-from` to those workflows would put megabytes in the
archive that no code path reads.

`@online-gal/engines` builds the same way as any other package, so `make-package.mjs --package
packages/engines` produces its tarball the moment a workflow names it in `package-dir`; until
then the supported routes are the workspace dependency above or a checkout of this repository. The
consumer still picks engines the same way — the tarball ships the compiled `dist/`, and the six
runtime packages come in as its vendored `node_modules/`, so `import { selectEngine } from
'@online-gal/engines'` works without compiling anything locally. See
`docs/fetch-engine-artifacts.md` for the tarball format, the reproducibility guarantees, and the
token a consumer needs on this private repository.

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
