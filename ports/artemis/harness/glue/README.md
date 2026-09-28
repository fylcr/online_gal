# The glue directory

Two generated files, the script that makes them, and the checker:

| file | what it is |
| --- | --- |
| `imports-map.md` | the readable table — every import the bridge module asks for, with its bucket, its signature, and two byte offsets into the official `artemis.js` |
| `imports-map.json` | the same table as data, for tooling and for diffing |
| `imports-map.mjs` | the generator; both files are its output and are not edited by hand |
| `verify-imports-map.mjs` | the independent checker; shares no code with the generator |

## Why this exists

The bridge module is a wasm2c rebuild of the official Artemis module. It asks its host
for **207 function imports** under a single module name `a`, whose field names are the
official glue's own minified keys — `$`, `Ob`, `Ea`, `xb`, and so on. Nobody can read
those names and know what a host has to build. This table is the answer: each of the
207 gets a bucket saying which modern host module decides it, and an offset saying where
in the official `artemis.js` the implementation of that call can be read.

The table is derived, not written. The import section is decoded out of the `.wasm`; the
correspondence from minified key to real Emscripten name is read out of the glue's
`asmLibraryArg` object; the nine runtime renames come from the build's own alias
manifest. Change an input and the table changes with it.

## Regenerating

```sh
ARTIFACT_DIR=<where you unpacked the artifact, see below>
node ports/artemis/harness/glue/imports-map.mjs \
  --wasm    "$ARTIFACT_DIR/dist/artemis-bridge/build/bridge.wasm" \
  --glue    ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js \
  --forward "$ARTIFACT_DIR/dist/artemis-bridge/build/imports-forward.c" \
  --aliases "$ARTIFACT_DIR/dist/artemis-bridge/build/runtime-imports.json"
```

Zero dependencies, node >= 20, ESM. Output is deterministic — no timestamps, no input
paths, nothing that depends on hash iteration order. Run it twice and `sha256sum -c` the
first run's hashes; both files compare OK.

The generator refuses to write anything rather than emit a table that looks complete but
is not. It exits non-zero if a row has no official name, no glue binding, or a `todo`
bucket with no reason; if a hand-written `todo` reason names an import that is not in
this build; if a bucket override names an import that is not in this build; or if a
classification rule matches no import at all (a dead rule means some name is riding the
fallthrough and nobody will notice).

`--forward` is optional. Without it the `fwd` column reads `-` and the classification
still stands on the wasm import section plus the glue; the C forwarders are corroboration,
not an input.

## Getting the artifacts

`bridge.wasm` is not in the repository. It comes out of the `wasm-artemis-bridge` CI
workflow, which rebuilds it from upstream `artemis-compat` and uploads it as an artifact:

```sh
gh run download <run-id> -n wasm-artemis-bridge -D "$ARTIFACT_DIR"
```

`$ARTIFACT_DIR` is any directory outside the tracked tree. The artifact is around 270 MB
(four copies of the module plus the generated 106 MB `artemis.cpp`), so it is never committed;
pick somewhere you are happy to delete.

If you are not already authenticated, or `gh` cannot reach GitHub from where you are,
the repository is private and needs a token plus a proxy:

```sh
export HTTPS_PROXY=socks5://127.0.0.1:10808
gh auth login
```

Then check what you got against the manifest the workflow wrote:

```sh
sha256sum "$ARTIFACT_DIR/dist/artemis-bridge/build/bridge.wasm"
# 2b30f5c540729d1237021a6e90160de46a48f21832cdbfe5300f5b24c11e6c96
```

`imports-map.md` records that hash in its own Provenance section, so a table and a binary
can be told apart or matched without trusting either one's filename. If the hash differs,
the table was generated from a different build and the import surface may have moved.

The glue (`artemis.js`) and the generated C (`imports-forward.c`, `runtime-imports.json`)
are side effects of the same run, unpacked next to `bridge.wasm` in
`dist/artemis-bridge/build/`.

## Checking the output

The generator checks itself against its own inputs. A separate checker re-decodes the
import section with independent code and cross-checks the emitted table against that
decode and against the browser's own `WebAssembly.Module.imports`, so an error in the
generator cannot make its own output look right:

```sh
node ports/artemis/harness/glue/verify-imports-map.mjs \
  --wasm "$ARTIFACT_DIR/dist/artemis-bridge/build/bridge.wasm" \
  --glue ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.js \
  --json ports/artemis/harness/glue/imports-map.json \
  --md   ports/artemis/harness/glue/imports-map.md
```

24 checks; all must pass. The checker is committed beside the table it checks, and it reads
the wasm and the glue itself rather than trusting anything the generator wrote.

## Reading a row

`glue ref` is where the name is bound; `impl` is where the function behind it is defined,
and is the offset you want when you go read the body. The two are different places, and
the glue is minified onto a single line, so an offset really is the only stable locator —
a line number would be 1 forever.

`todo` means "not needed yet, and here is why" — the reason is on the row. `fd` is backed
by `@online-gal/vfs`, `gl` by the real WebGL2 context and AudioContext, and `async` is
the set of Asyncify re-entry trampolines a host has to unwind through rather than call
normally. The `modern owner` column says which module under
`packages/runtime-artemis/src/host/` each bucket is heading for; none of them exist yet.
