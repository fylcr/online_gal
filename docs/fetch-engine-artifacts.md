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
> (`engine-ons-0.7.7beta`, `engine-krkr-20260920`). Artemis has no artifact at all any more: its
> host build is retired, and the bridge that replaced it is not published either.

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

## The engines

| Engine | Files | Installed to | Tracked? |
| --- | --- | --- | --- |
| `ons` | `onsyuri.html`, `onsyuri.js`, `onsyuri.wasm` | `packages/runtime-ons/assets/` | **yes** — committed source |
| `krkr` | `index.wasm`, `index.js`, `vlfs.js`, `index.html`, `shell.html`, `assets.zip` | `.ref/kirikiroid2-web/out/web/release/` | no |

Both of the two land in gitignored build directories, which is deliberate: a downloaded binary is a
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

**The fetched `krkr` set is not only hash-checked; it boots a real game.** The six files were taken from
the artifact of `workflow_dispatch` run 36367138923 (`publish=false`, so no release was touched), hashed
against `ports/lib/engine-artifacts.json` — all six match — then served from
`ports/artemis/build/serve-range.mjs` beside a hardlink to the shipping game's `data.xp3`
(2,312,777,571 B) and driven in Edge by `ports/krkr/kirikiroid2/browser-test/kiri-shot.mjs` at
`?xp3=/data.xp3`. The page reported
`{"title":"krkr2","canvas":"756x488","body":"Kirikiroid2 Web Running..."}`, and the canvas was not blank:
87.09% of pixels above luminance 24, 50.17% above 200, mean colour `r174.67 g161.10 b156.41`,
signature `f007553e`. `"Running..."` alone is equally consistent with an engine that booted and drew
nothing, which is why the numbers are recorded rather than the status text. The server's log shows the
load the port exists for: `206` responses in 262,144-byte chunks at scattered offsets (511 MB, 982 MB
and 1.6 GB into a 2.3 GB file), so the game arrived on demand instead of as a whole-file download.

## Fetching

```bash
node ports/lib/fetch-engine.mjs ons          # one engine
node ports/lib/fetch-engine.mjs krkr --json  # machine-readable result on stdout
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

Code 2 is the expected outcome for any engine on a machine with no network. The message names the
file, the URL it tried, and the command that builds it from source instead. There is no stack trace
and no silent success: a fetch that cannot deliver says so and exits non-zero.

Code 4 — "the manifest has no usable entry" — is a different thing, and it is what `artemis` now
returns. The name is still accepted, so the argument parses, but the ledger has no `artemis` group
because the artifact it used to describe is no longer produced. Both outcomes mean "no bytes for you
here"; 4 is the honest one for an engine that has been retired rather than merely unpublished.

## Installing a runtime package instead of fetching loose files

`ports/lib/fetch-engine.mjs` is the right interface *inside this repository* — it hashes against
`ports/lib/engine-artifacts.json` and writes into a fixed directory. A downstream project wants
something else: a dependency it can declare, install and upgrade like any other, without knowing the
engine's file names, a tag, or where a hash manifest lives. So the ONS build now also publishes a
**standard npm tarball** — the CI output *is* the package.

```bash
pnpm add https://github.com/fylcr/online_gal/releases/download/engine-ons-0.7.7beta/online-gal-runtime-ons-0.1.3.tgz
```

```jsonc
// package.json
{
  "dependencies": {
    "@online-gal/runtime-ons": "https://github.com/fylcr/online_gal/releases/download/engine-ons-0.7.7beta/online-gal-runtime-ons-0.1.3.tgz"
  }
}
```

Then the runtime is an ordinary import, and the engine wasm it needs travels with it:

```js
import { OnscripterRuntime } from '@online-gal/runtime-ons';
```

The loose release assets are **unchanged** and still published. `fetch-engine.mjs` downloads them by
name, knows nothing about packages, and keeps working exactly as before — the tarball is an
additional asset, not a replacement.

### The repository is private, so a tarball URL needs a token too

The `404`-on-unauthenticated problem from the section above applies identically: a release asset in a
private repository answers an anonymous request with `404`, which a package manager reports as a
transient or generic network error rather than "you need a token". Put the token in
`.npmrc` (or `NPM_TOKEN` in CI) rather than on the command line, where it lands in shell history and
in CI logs:

```ini
# .npmrc
//github.com/fylcr/online_gal/:_authToken=${NPM_TOKEN}
always-auth=true
```

### Why the tarball has its dependencies inside it

The archive is not a repackaged `dist/`. `packages/runtime-ons/package.json` declares its
dependencies as `workspace:*`, and that is a hard blocker for a distributed tarball — pnpm rejects
it with `ERR_PNPM_WORKSPACE_PKG_NOT_FOUND` and npm with `EUNSUPPORTEDPROTOCOL`, both *before* the
archive is even extracted. Rewriting the protocol does not help either, because those packages are
`private: true` and were never published (`ERR_PNPM_FETCH_404` on the registry), and a `file:`
rewrite fails differently: pnpm resolves `file:` relative to the **consumer's** `node_modules`, not
to the package declaring it.

So `.github/scripts/make-package.mjs` vendors the dependency closure into
`package/node_modules/@online-gal/*` as real directories and drops the `dependencies` map from the
shipped manifest. That is what makes the install work offline, against a private monorepo, with no
registry. The cost is honest and worth stating: the closure is frozen into the archive, so a
consumer upgrading the runtime also picks up new copies of `engine-api`, `vfs`, `image-codec` and
`player-host`. The version in the tarball name is the single thing that should be bumped when those
change.

Vendoring the closure *flat* — once each, at the top level — is not just tidiness. Each workspace
package's own `node_modules` contains pnpm's links back to its siblings, so a naive recursive copy
reproduces the whole closure inside every member of it: 13 copies of 4 packages and 4.0 MB where
1.2 MB is honest. Node resolves upward, so the nested copies would be dead weight.

### The tarball also carries the sources and the licence

`package/src/**` ships alongside `package/dist/**`, for two independent reasons.

The mechanical one: every package here routes types to the source — the exports map is
`{"types": "./src/index.ts", "default": "./dist/index.js"}` — and TypeScript resolves `types` first.
A tarball without `src/` gives a TypeScript consumer a types condition pointing at a file that is
not there. The vendored siblings need `src/` for exactly this reason, so shipping it for only the
package under test would be the inconsistency.

The licensing one, which is not negotiable: this is GPL-3.0-or-later, and the GPL requires that
recipients of the covered work receive the corresponding source. `dist/*.js` alone is not that.
Shipping `src/` is how a single tarball satisfies that obligation without the consumer having to
know to go back to the repository.

And `package/LICENSE` and `package/THIRD_PARTY_NOTICES.md` ship for the same underlying reason: §5(a)
asks for a copy of the licence to accompany the work, and §5(b) for the source. Together with the
`"license"` field in the manifest, all three halves travel, and a recipient never has to come back to
the repository to comply. They are copied byte for byte, so `sha256sum package/LICENSE` in an
extracted tarball equals `sha256sum LICENSE` in a clone of the project.

### The tarball is reproducible

Two runs over the same inputs produce byte-identical archives (`sha256` equal), because the build
pins the seven things that vary by default: every entry's mtime is `--mtime=@498073600` (1985-10-26,
the earliest a zip can represent), entries are `--sort=name` so filesystem readdir order cannot leak
in, owner/group are forced to `0`, file and directory modes are normalized to `0644`/`0755` before
`tar` reads them, text files are normalized to LF before `tar` reads them, and the gzip header has
both its timestamp (`mtime: 0`) and its OS byte (3) pinned.

The last four are not theoretical, and each of them was a real mismatch between a build on a
laptop and the same build on a runner. The first two builds of this package were byte-identical on
a laptop and **one byte apart** on a runner: `actions/upload-artifact` preserves the mode of the file
as it sits on the runner, so the `--assets-from` copy of `assets/onsyuri.wasm` arrived as `0755`
while the same bytes fetched by curl locally are `0644`. The extracted trees were identical — 209
files, byte for byte — and the entire difference was the mode field in one tar header. The gzip OS
byte leaked the same way, because zlib writes the *host's* OS there (0x03 on Unix, 0x0a on Windows
Node) and nothing in the archive depends on it. So the modes are taken from nowhere: the staged tree
is normalized before `tar` runs, and the umask of whoever ran the build never reaches the archive.

Line endings leaked last, and it was the only one of the four that `git status` cannot see. This
repository is `* text=auto eol=lf` with `core.autocrlf=true`, so the blob in git is LF while a
developer's worktree is CRLF — and git reports that worktree as *clean*, because it compares the
blob after applying the same conversion. Copying a working-tree file into the archive therefore
copies the **developer's** line endings, not the repository's. The published
`@online-gal/runtime-ons` 0.1.2 and a local build of the same commit disagreed by 186 bytes, and the
only file that differed was the licence: 35,149 B on CI, 35,823 B locally, exactly 674 bytes larger,
and 674 is that file's line count. One CR per line, nothing else. Both archives listed the same 233
entries, so it was diagnosable — but every machine that happened to check out with CRLF would have
produced a different `.tgz` for a repository state that had not changed at all.

So text files are normalized to LF on the same principle the modes are: LF is the repository's
canonical form, and a worktree's line endings are a property of the machine, not an input. Two
guards keep the rule from eating data. `package/assets/` is skipped entirely, because those engine
artefacts are committed CRLF and their exact bytes are pinned by hash in `ports/lib/engine-artifacts.json`.
A file containing a NUL byte is treated as binary and left alone. `--json` reports how many files a
given run rewrote as `normalizedLineEndings`: non-zero on a Windows worktree, zero in CI, same
archive either way — and a non-zero value *in CI* means a CRLF file got committed.

Verified locally:

```bash
node .github/scripts/make-package.mjs --package packages/runtime-ons --out /tmp/a
touch packages/runtime-ons/dist/index.js          # mtime changes, content does not
node .github/scripts/make-package.mjs --package packages/runtime-ons --out /tmp/b
cmp /tmp/a/online-gal-runtime-ons-0.1.3.tgz /tmp/b/online-gal-runtime-ons-0.1.3.tgz   # identical
```

There is one limit worth stating plainly, because it is the difference between a content change and
a toolchain change. A `.tgz` is produced by two programs: GNU tar writes the headers, and whatever
zlib the host links to produces the deflate stream. Pinning the seven inputs above removes the ambient
variation — clock, readdir order, uid, umask, checkout line endings, host OS — but it cannot pin a
future zlib's match search. So the script also reports the sha256 of the **uncompressed tar stream**
(`tarSha256` via `--json`), and that is the number to compare across machines. A `tarSha256` mismatch
is a content change. A `tarSha256` match with a differing `.tgz` sha256 is a toolchain difference,
not a regression.

Two claims, and it is worth keeping them apart. **Same checkout, different toolchains**: the same
script on Windows/Node 26 and on Linux/Node 22.14.0 with GNU tar 1.35 produced the same `.tgz`. That
was measured, and the mode and OS-byte pins are what made it possible — with `normalizeModes`
disabled the same two runs differ, and on a filesystem that reports `777` for every file (a 9p
mount, for instance) the tarball picks up `-rwxrwxrwx` on 205 of its 209 files. **Different
checkouts, different machines**: this is the claim the line-ending rule buys, and until that rule
existed it was silently false — the earlier cross-toolchain run read the *same* CRLF worktree through
WSL, so it could never have detected the difference. The anchor for the second claim is the
published artifact, built from CI's LF checkout: `online-gal-runtime-ons-0.1.3.tgz` is
**2,136,169 B**, sha256 `df2d59c9453ee02db8f3e5b9eef04a79cbc6aa7a4767f34b5a161a8223a7fa40`, and its
uncompressed tar stream is **6,318,080 B**, sha256
`00f4255d9050a4cccdf618bdbb3c2edef45cc0512a0dbe3dcd06405ec8cf70e3`. A local build on a CRLF
worktree now reproduces both numbers exactly.

The aggregate makes the same claim, and it is the one that was actually proved: `build-packages.yml`
packs `@online-gal/engines` from CI's LF checkout, and the released
`online-gal-engines-0.1.4.tgz` is **2,578,765 B**, sha256
`492cd9603c32c363d727df5c6256376e6535b4444afffdf943f02849b13fa92d`, uncompressed tar stream
**8,263,680 B**, sha256 `bb49f5778e8b511ac1d3fd722a7b3f9e7a365f6039ef18a625f8682fa54bd06e`. A pack
of the same commit in a *fresh Windows worktree* — CRLF engine assets, Node 26, a full `pnpm install`
and a full `build:packages` — reproduces all four numbers, `.tgz` sha256 included: the two archives
`cmp` byte-identical, and the extracted trees `diff -rq` clean. Two checkouts, two operating systems,
one byte.

0.1.4 rather than 0.1.3 only because the artemis host's own bytes changed in `f5938bb` (the async
bucket), and a vendored runtime's bytes are part of the aggregate's contract; the claim above is
about the checkout, not the version.

### Three things are checked, not pinned

Pinning governs the bytes between `tar` and the file on disk. Two inputs are wrong *before* that, and
no normalization can tell a good archive from a bad one, so `make-package.mjs` refuses instead. A
third input is neither wrong nor pinnable — it is simply not the packer's to keep.

**The build must be fresh.** A `dist/` compiled from an older `src/` archives cleanly and looks
perfect, so the packer compares the newest `src/**` mtime against the newest build witness of the
package *and of every package in the vendored closure* — an aggregate's bytes include its siblings'
`dist/`, so a stale `vfs` is as wrong as a stale `engines`. The witness is the newer of the newest
`dist/` file and `tsconfig.tsbuildinfo`, because `tsc -b` is incremental: a `git checkout` or a
`git pull` moves a source file's mtime without changing its content, so a rebuild correctly writes
nothing and `dist/` stays behind by an amount no rebuild repairs. A failure is exit 2, and
`--skip-freshness` overrides it for the case where packing a deliberately stale tree is the point.
`--json` reports `freshness.checked`.

**The committed engine bytes must match the ledger.** `ports/lib/engine-artifacts.json` pins every
engine artefact by size and sha256, and those hashes are only a claim until something checks them.
For a while nothing did, and `packages/runtime-ons/assets/onsyuri.html` was committed as LF
**23,817 B** where the ledger says **24,399 B** — the 582 CRs missing, `git status` clean the whole
time because the *worktree* held the right bytes and the *blob* did not. The aggregate vendors the
repository's copy, so `@online-gal/engines` 0.1.0 published 23,817 B of engine while the ledger and
the ONS release both said 24,399 B, and it surfaced only when somebody downloaded the published
tarball and compared it against the ledger. Two developers had compared their worktrees; both were
right and the repository was wrong.

The packer now verifies every ledger entry that is committed anywhere under the engine-asset
directories, before it stages anything, and exits 2 naming the file, both digests, the CR count, and
the fix. There is deliberately **no flag to skip it**: `--skip-freshness` exists because a stale
`dist/` is a local condition a rebuild fixes in a second, while a committed file that disagrees with
the ledger is a repository fact, and a flag would be a way to publish the defect on purpose.
`--json` reports `ledgerChecked` (3 here — the ons trio; the six krkr files are build outputs that
exist only on a runner, so they are skipped, and a `0` is reported rather than hidden).

The fix is `.gitattributes`, not a re-save: `packages/runtime-ons/assets/**` is marked
`-text -diff` (as `ports/artemis/vendor/**` already is), because an engine artefact is a **build
output** whose sha256-pinned bytes *are* the substance, and `* text=auto eol=lf` must not get to
decide them. After changing them, commit the bytes **and** bump the version of every package that
vendors them — `@online-gal/engines` at minimum, since its ten vendored directories are frozen into
its tarball and its version is what distinguishes a stale aggregate from a new one.

### The build cache is not source

`tsconfig.tsbuildinfo` used to ship in every tarball — 2 copies in `runtime-artemis`, 4 in
`runtime-kag` and `runtime-ons`, 10 in the `engines` aggregate. It is `tsc -b`'s incremental cache,
and it breaks reproducibility in a way that is easy to misread, so the mechanism is worth stating.

The cache records the **sha256 of every source file as the compiler read it** — that is, *before* the
packer normalizes CRLF to LF. A CRLF checkout therefore produces a cache whose embedded digests
describe text that is no longer in the archive, and no normalization pass can fix it, because there
is no line ending left to normalize: the contaminated value is a hash *of* the pre-normalized bytes,
stored as a 64-character string. Measured, not argued: building this repository from LF sources and
from a CRLF checkout gave tarballs of **identical length** (8,130,560 B) whose only differing files
were exactly the 10 `tsconfig.tsbuildinfo` entries in the vendored closure — 819 B larger in total,
tar sha256 `be179a378c0673…` against `68c172fef93c…`. The other 406 files were byte-identical, which
is what makes this so easy to attribute to something else. Those two digests and that length describe
the cache-carrying tarball of `@online-gal/engines` 0.1.1, which has since been withdrawn; they are
kept here as the record of the defect, and the table below gives the published replacement.

So the packer excludes that one filename, for the package under test and every vendored sibling,
through a single shared predicate. It is deliberately not *normalized*: rewriting hashes inside the
file would ship a cache that is **wrong** about the tree it sits in, and a consumer's `tsc` trusts
it. A cache is not source, and the archive already ships what a cache exists to accelerate — `src/**`
and `dist/**` together rebuild in seconds, whereas a stale cache inside an archive is worse than none.

This also retires the caveat on the numbers above. **Every tarball published before the cache was
excluded contains the `tsconfig.tsbuildinfo` files counted here**, so the sizes and digests quoted
earlier in that paragraph describe archives that still carry the cache, and the published set is a
version higher in every case — and the aggregate has since moved once more, to 0.1.4, when the
artemis host's bytes changed. The published archives, measured rather than remembered:

| package | release | `.tgz` bytes | `.tgz` sha256 | tar stream bytes | tar sha256 | files |
| --- | --- | --- | --- | --- | --- | --- |
| `@online-gal/engines` 0.1.4 | `engine-packages-0.1.4` | 2,578,765 | `492cd9603c32c363…` | 8,263,680 | `bb49f5778e8b511a…` | 431 |
| `@online-gal/runtime-ons` 0.1.3 | `engine-ons-0.7.7beta` | 2,136,169 | `df2d59c9453ee02d…` | 6,318,080 | `00f4255d9050a4cc…` | 207 |
| `@online-gal/runtime-kag` 0.1.1 | `engine-krkr-20260920` | 367,243 | `ac3ffb26f1bc1cfab…` | 1,720,320 | `7f77eb8b3ee03b58…` | 236 |
| `@online-gal/runtime-artemis` 0.1.3 | *not published* | 363,638 | `ecdce36b7d421df4…` | 1,546,240 | `b7d5bc160bef2f28…` | 197 |

The first three rows were each re-measured from the released asset and reproduced byte-for-byte by a
local pack of the same commit — `.tgz` sha256 included — which is the cross-checkout claim this
document previously could only make for the aggregate; for the 0.1.4 row that check went as far as
`cmp` on the two archives and `diff -rq` on the two extracted trees. The aggregate is the one that
carries a committed engine, so it is the one that can be rebuilt against a *different* line-ending
checkout without changing the engine bytes: a worktree of `8482272` converted wholesale to CRLF,
installed and rebuilt, packed to the same `2,553,631` B and the same tar sha256 `434c98ff3ef1517b…`
as the LF one, with 109 files normalized on the way in. That measurement is on 0.1.3 and is left as
it was taken; 0.1.4 is a different archive (the artemis host changed), so it does not inherit the
number — what it inherits is the rule that produced it. `tsconfig.tsbuildinfo` appears **0** times in
all four archives. The artemis row is a local build: that engine is deliberately not published, so it
has no release to measure and the number is here only to show what a fourth row would look like.

Against the pre-cache published tarballs the only change is the removed cache: `runtime-ons`
211 → 207 files and `runtime-kag` 240 → 236, with **0 files differing in content**. Every tarball
was then installed with `pnpm add` into a directory outside this repository and every
`@online-gal/*` specifier inside it was imported (10/10 for the aggregate, 4/4 for `runtime-ons` and
`runtime-kag`, 2/2 for `runtime-artemis`), with `tsconfig.tsbuildinfo` still present **0** times in
the installed tree.

### Licensing: GPL-3.0 propagates

These packages are `private: true`, are never published to npm, and are licensed
**GPL-3.0-or-later** — the same license as this repository. That is not a formality to be noted once
and forgotten:

- **A project that installs one of these tarballs is a combined work, so it must also be
  GPL-3.0-or-later**, and must distribute its own source. Linking `@online-gal/runtime-ons` into a
  proprietary product and shipping it is a licence violation, not a grey area.
- **Redistribution needs the GPL text and the corresponding source**, and the notices must survive.
  So every tarball ships them literally: `package/LICENSE` and `package/THIRD_PARTY_NOTICES.md` sit
  at the top of the archive, byte-identical to the repository's own files, and can be checked with
  `sha256sum`. A `"license": "GPL-3.0-or-later"` field in the manifest is the machine-readable half
  of the obligation — it is what a scanner reads without opening the archive — and `src/**` is the
  other half; neither is a substitute for the text itself, which is why all three ship. The engine
  bytes under `assets/` carry their own upstream terms and must keep their own notices alongside.
  The notice file is the repository-wide one, shipped whole rather than narrowed to one package: the
  vendored closure means a recipient of one package receives all four packages' worth of code, and
  the only safe hand-picked subset of obligations is "everything".
- The upstream engines keep **their own** licenses, and they are not all GPL. OnscripterYuri ships
  under its own terms; the KiriKiroid2 and Artemis lines are separately licensed (Artemis is built
  against a proprietary SDK and is deliberately not published at all). Publishing a *package* does
  not change the engine's license, and where the two differ the engine's own terms apply. Check the
  engine's license before redistributing a package containing it.

### Which engines get a package, and who builds the aggregate

Four tarballs are published. Three come from the engine workflows, one per engine, each carrying
that engine's own compiled `dist/`; the fourth is the cross-engine registry, built by a workflow
that exists only to build packages.

| package | built by | engine bytes in the tarball? |
| --- | --- | --- |
| `@online-gal/runtime-ons` | `build-wasm-ons.yml` | yes — `assets/`, because `dist/wasm-runtime.js` resolves `new URL('../assets/', import.meta.url)` |
| `@online-gal/runtime-kag` | `build-wasm-krkr.yml` | no — the wasm is a URL-served page, not a file beside the module |
| `@online-gal/runtime-artemis` | `build-wasm-artemis-bridge.yml` | no — the module is loaded by URL from a caller-supplied location |
| `@online-gal/engines` | `build-packages.yml` | yes, but only the **committed** ONS `assets/` — this workflow builds no engine |

The "no" rows are deliberate, and passing `--assets-from` anyway would be actively misleading: the
artifacts would land in the archive as `assets/`, where **no code path in those packages reads
them**, producing a tarball that looks self-contained while being wrong about what it is. For krkr
specifically, `configureKirikiriWasm({ pageUrl })` decides where the engine comes from and
`findKiriMount()` then mounts `?xp3=` or `?game=` against it — the engine is wherever the URL says, so
it cannot be baked into the package. The engine half of those releases keeps the route it already
had: the run's `wasm-krkr` artifact and the `engine-krkr-20260920` release.

`@online-gal/engines` depends on all six runtime packages at once, and no engine workflow builds all
six — ONS's job has ONS alone, krkr's has krkr alone, the artemis bridge's has artemis alone.
Publishing it from any of them would mean shipping a tarball whose other five providers are vendored
from whatever stale `dist/` happens to be on that runner, which is a worse failure than not
publishing it: the vendoring design deliberately drops `dependencies` because nothing outside the
monorepo can resolve `workspace:*`, so a stale copy is invisible to the consumer and the failure
surfaces as a provider silently missing at runtime. Its tarball builds and installs correctly by
hand — 2.5 MB, all ten vendored dependencies resolving inside the package — so this was a
trigger-graph problem, not a packaging limitation, and the answer was a workflow rather than an
input.

`.github/workflows/build-packages.yml` is that answer. Its single job checks out, installs with
`--frozen-lockfile`, runs `pnpm run build:packages`, and packs — no Emscripten, no vcpkg, no
upstream checkout, so it finishes in a couple of minutes. Rebuilding *all* the packages in the same
job that packs them is what makes the vendored closure correct rather than merely plausible. It
packs the six runtimes and the aggregate (and **not** `packages/core`, the app shell, nor
`packages/tjs2-bytecode`, a bytecode library that is in no runtime's closure), and it publishes
**only** `online-gal-engines-<version>.tgz`; the per-engine tarballs remain workflow artifacts,
because publishing them from here as well would put two byte-different archives of one version in
two different releases. The tag is `engine-packages-<version>`, so a consumer's `pnpm add` line
never needs editing.

**The version rule.** That aggregate freezes ten vendored directories, so **any change to a
vendored package, or to ONS's `assets/`, requires bumping `packages/engines`'s version in the same
commit.** The version is what a consumer's dependency line points at, so a change to any part of
the closure without a bump is a false claim about identity: that URL would still serve the previous
engine. The first release is 0.1.0. Note the asymmetry that makes this more than bookkeeping — the
aggregate has no `--assets-from`, so its `runtime-ons` carries the `assets/` **committed in this
repository**, byte-identical to `ports/lib/engine-artifacts.json` today; only `@online-gal/runtime-ons`
itself takes the fresh engine build. After an ONS rebuild, commit the new assets or bump both.

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
bash ports/krkr/kirikiroid2/run-build.sh                 # KiriKiroid2
```

`ports/ons/build/fetch-web-engine.sh` is the ONS entry point and its default is fetch-then-build, so
it works on a machine with nothing and on a machine with everything — the two scripts are the model
the CI job follows.

There is no Artemis line here any more. The browser host this section used to name
(`ports/artemis/build/web-deploy.sh`) is retired along with the `artemis-web-android` artifact it
installed; see "Relationship to the rest of the tree" above for what the engine looks like now.

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

**Where Artemis stands.** Artemis is a **wasm rewrite of the third-party `artemis-compat` engine
(GPL-3.0)**; the rewrite's sources are `ports/artemis/src/`. The only form of it that ships today is
the **bridge module** (`.github/workflows/build-wasm-artemis-bridge.yml`), which links that rewrite
against the official Artemis SDK so the emulator can drive a real game. There is **no modern host**
for it yet — no `packages/runtime-artemis` engine that a downstream app can depend on — so
`packages/engines` reports `artemis` as `unavailable` and the emulator refuses it rather than
half-running. Accordingly this manifest describes two engines, not three.

The two files under `ports/artemis/build/` that a build or a benchmark needs —
`official-web-deploy.py` and the `ports/artemis/tools/real-game-test.mjs` that drives it — are
**not** an Artemis shipping route. They are the **yardstick**: the real-game benchmark's control
tree is deliberately "the official module plus the official glue", so our rewrite is measured
against the engine it replaces rather than against itself. A failure there means the rewrite is
wrong, not that the harness is.

**What that yardstick can and cannot measure (measured 2026-09-29).** On the loose tree it measures
**ABI-level equivalence**: both sides run the same import trace and produce the same output for the
same inputs. It does **not** measure rendering fidelity, because that deployment supplies no game
bytes at all — the engine asks for extension-less names (`image/bg/black`, `debug/lua/index.lua`)
while every one of the tree's 29,224 files has an extension, and `serve-range.mjs` has no
extension-less fallback, so all 24 asset requests are 404 (`netCounts {200:3, 404:24}`,
`gameBytesByStatus {404:0}`, 24× `Download file <name> failed.`, frames at `lit 87.5/87.39%` and
`distinct16 16` — placeholder content, not the title screen). Rendering is measured on **packs**
instead: `node ports/artemis/build/serve-range.mjs <dir with root.pfs> 4650`, where `/__packs`
lists `root.pfs` first and the page's byte source only accepts `206`; the retired Android-shim host
page in the same directory is the known-good reference for that path. See
`docs/artemis-web-port.md` §18.4.

- `ports/lib/ref.mjs` is the shared path helper; the fetch script resolves the repository root
  through it rather than counting `..` segments of its own.
- `packages/runtime-ons/scripts/copy-runtime-assets.mjs` is the only thing that moves ONS assets into
  `dist/`; a fetch writes the tracked files in `assets/` and the existing build step does the rest.
- `apps/app/astro.config.mjs` serves the KiriKiroid2 build directory through middleware rather than
  copying it into `public/`, so a fetch into `.ref/kirikiroid2-web/out/web/release/` is picked up by
  the next dev-server request with no rebuild.
