#!/usr/bin/env bash
# Purpose: CI entry point for the SHIPPING Artemis build. It runs the tracked recipe
# `ports/artemis/compat-web/build.sh` -- which compiles the artemis-compat engine source at the
# pinned commit, plus that directory's patch set and overlays -- and stages the two publishable
# modules where the workflow expects them.
#
# This is not the SDK route. No official Artemis module, glue or Android binary is an input here,
# and no wasm2c transpile is involved; see `.github/scripts/build-artemis-bridge.sh` for the
# legacy diagnostic build that does use them.
#
# Scope: it produces and stages the modules and copies their source offer. It does not boot a
# game, does not run the import-map gate (that gate is the bridge's: it compares against a
# committed table for a different module), and a green run is not evidence that the engine is
# playable.
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$REPO_ROOT"

# The recipe archives the pinned commit from a local object when one exists, and clones the pin
# when it does not. A CI runner has no `.ref/`, so the clone is the normal path there.
#
# The reusable workflow's own upstream checkout -- `upstream-path: .ref/artemis-compat` -- is what
# makes a local object exist, so ARTC_REF and that path must not be allowed to drift apart: the
# clone reads the engine source out of ITS OWN checkout, and a mismatch would silently build the
# wrong tree. Asserted, not assumed, because both values are supplied by the workflow.
: "${EMSDK_DIR:=${EMSDK:?EMSDK_DIR must point at an Emscripten SDK with a upstream/emscripten/emcc}}"
: "${ARTC_REF:=.ref/artemis-compat}"
: "${ARTC_PIN:?ARTC_PIN must be the pinned artemis-compat commit; the workflow supplies it}"
: "${UPSTREAM_PATH:?UPSTREAM_PATH must name the checkout ARTC_REF points at}"

# ONE pin for the run, written here so nothing downstream can disagree with it. The workflow's
# `upstream-commit` and its `build-env: ARTC_PIN=...` are the same literal, but they are two
# separate copies of that literal: if only one of them moved, the workflow would fetch one commit
# while the recipe archived the other, and the artifact would carry a manifest, a version tag and
# a PIN file naming a commit whose source is not in it. A provenance record that names a commit
# nobody built is worse than no record, so it is refused rather than reconciled.
UPSTREAM_COMMIT="${UPSTREAM_COMMIT:?UPSTREAM_COMMIT must be the commit the workflow checked out; the reusable workflow exports it}"
if [ "$UPSTREAM_COMMIT" != "$ARTC_PIN" ]; then
  echo "::error::the workflow checked out UPSTREAM_COMMIT=$UPSTREAM_COMMIT but passes ARTC_PIN=$ARTC_PIN. Rewrite the single literal in .github/workflows/build-wasm-artemis-compat.yml so both name the same reviewed commit."
  exit 1
fi

echo "## repo:        $REPO_ROOT"
echo "## EMSDK_DIR:   $EMSDK_DIR"
echo "## ARTC_REF:    $ARTC_REF"
echo "## UPSTREAM_PATH: $UPSTREAM_PATH"
echo "## UPSTREAM_COMMIT: $UPSTREAM_COMMIT"
echo "## ARTC_PIN:    $ARTC_PIN"

[ "$ARTC_REF" = "$UPSTREAM_PATH" ] || {
  echo "::error::ARTC_REF=$ARTC_REF but the workflow checked out UPSTREAM_PATH=$UPSTREAM_PATH; the recipe would clone into a different path than the one the workflow fetched"
  exit 1
}
[ -x "$EMSDK_DIR/upstream/emscripten/emcc" ] || {
  echo "::error::no emcc at $EMSDK_DIR/upstream/emscripten/emcc -- is EMSDK_DIR right?"
  exit 1
}
[ -f ports/artemis/compat-web/build.sh ] || {
  echo "::error::missing ports/artemis/compat-web/build.sh (the tracked recipe)"
  exit 1
}
[ -f ports/artemis/compat-web/overlay/host/web_main.cpp ] || {
  echo "::error::missing the compat-web host overlay; this is not the compat-web tree"
  exit 1
}

# EMSDK_DIR is exported rather than left in this shell only: the recipe does not read it (it uses
# ports/env.sh, and the workflow puts emcc on PATH), but a caller that inherits it gets the same
# SDK this run asserted, instead of a dev-machine default.
export EMSDK_DIR ARTC_REF ARTC_PIN

echo
echo "== running the tracked recipe =="
# -j2, fixed. On a CI runner the host core count is in the dozens, and this link step allocates
# per job; over-subscribing it is how the run dies with an OOM that later reads as "flaky". The
# recipe defaults to 2 as well -- the value is passed explicitly so the number that ran is in this
# log rather than only in the recipe.
JOBS="${ARTC_JOBS:-2}"
bash ports/artemis/compat-web/build.sh -j "$JOBS"

# The recipe asserts these paths in its own header (`$OUT/build`); re-checking here is what makes
# a recipe that started writing somewhere else a failed step rather than a manifest with no bytes
# behind it. `make-manifest.mjs` would catch a missing file too, but it would do so after the
# package step had already run against an empty directory.
OUT_DIR="ports/artemis/compat-web/out"
JS="$OUT_DIR/build/artemis-compat.js"
WASM="$OUT_DIR/build/artemis-compat.wasm"
for f in "$JS" "$WASM"; do
  [ -s "$f" ] || { echo "::error::the compat-web recipe produced no $f"; exit 1; }
done

DIST="dist/artemis-compat"
echo
echo "== staging the publishable set =="
# The one recursive delete, and it happens BEFORE the directory is created, so a wrong path here
# is a deleted directory rather than a cleaned one. `dist/artemis-compat` is written relative to
# $REPO_ROOT (the workflow runs in the checkout root, but that is a property of the runner, not of
# this script), so it is resolved to an absolute path first and then held to the exact root it is
# allowed to be. A computed or empty value cannot survive both checks, which is the whole point:
# an unverified `rm -rf` is indistinguishable from a correct one until the day it is not.
DIST_ABS="$REPO_ROOT/$DIST"
[ "$DIST_ABS" = "$REPO_ROOT/dist/artemis-compat" ] || {
  echo "::error::refusing to remove $DIST_ABS: not the checkout's own dist/artemis-compat"
  exit 1
}
echo "## removing:  $DIST_ABS"
rm -rf "$DIST_ABS"
mkdir -p "$DIST_ABS"
cp "$JS" "$WASM" "$DIST/"
# The license pair and the source offer ride in the answer to the upstream GPL. These are the
# repository's own root files, copied rather than regenerated, so the notice that ships beside
# the binary is the one under review.
cp LICENSE THIRD_PARTY_NOTICES.md "$DIST/"

# --source-bundle writes `<out>/artemis-compat-source.tar.gz`: the exact archive the recipe
# extracted (upstream `git archive` at ARTC_PIN, server-side generated) plus the patch set, the
# two overlays and the build recipe that were applied to it, so a recipient can rebuild this
# binary from what is in the artifact. Its `--work` is a scratch tree; it is removed below.
node ports/artemis/compat-web/make-source-bundle.mjs \
  --out "$DIST" \
  --work "${RUNNER_TEMP:-/tmp}/artemis-compat-source-bundle" \
  --repo "$REPO_ROOT/$ARTC_REF"

# The manifest is the six published files by name, hashed from inside $DIST so the recorded paths
# are the artifact's own names. It deliberately does NOT hash a shell glob of the directory: a
# glob records `./LICENSE` rather than `LICENSE`, which is not the name in the artifact, and it
# would silently cover a seventh file that rode into dist. Naming the six keeps the manifest
# usable exactly as published, and the artifact directory holds nothing else to omit.
MANIFEST="artemis-compat-source.tar.gz.sha256"
(
  cd "$DIST"
  sha256sum artemis-compat.js artemis-compat.wasm artemis-compat-source.tar.gz \
    LICENSE THIRD_PARTY_NOTICES.md >"$MANIFEST"
  # Checked here, not just written: this is the file the workflow publishes and every consumer
  # verifies against, so a manifest that cannot verify its own siblings stops the run.
  sha256sum --check --quiet "$MANIFEST"
)

echo
echo "== artifacts =="
ls -l "$DIST"
cat "$DIST/$MANIFEST"
echo "## exit=0"
