#!/usr/bin/env bash
# Fetch the pinned wabt release that reverse.sh and rebuild.sh use.
#
# wabt is the only external tool the wasm side needs. The version is pinned
# because `.wat` text is not a canonical form: a different wabt may pick a
# different (equally valid) spelling of the same module, and then wat2wasm would
# no longer reproduce the original bytes even though nothing is wrong. Pinning it
# is what makes the byte-identity check in reverse.sh a meaningful assertion.

set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEST="$HERE/../build/wabt"
VERSION="1.0.42"

if [ -x "$DEST/wabt-$VERSION/bin/wasm2wat.exe" ] || [ -x "$DEST/wabt-$VERSION/bin/wasm2wat" ]; then
    echo "wabt $VERSION already present at $DEST/wabt-$VERSION"
    exit 0
fi

mkdir -p "$DEST"
cd "$DEST"

case "$(uname -s)" in
MINGW* | MSYS* | CYGWIN*) asset="wabt-$VERSION-windows-x64.tar.gz" ;;
Darwin)
    case "$(uname -m)" in
    arm64) asset="wabt-$VERSION-macos-arm64.tar.gz" ;;
    *) asset="wabt-$VERSION-macos-x64.tar.gz" ;;
    esac
    ;;
*)
    case "$(uname -m)" in
    aarch64) asset="wabt-$VERSION-linux-arm64.tar.gz" ;;
    *) asset="wabt-$VERSION-linux-x64.tar.gz" ;;
    esac
    ;;
esac

url="https://github.com/WebAssembly/wabt/releases/download/$VERSION/$asset"
echo "downloading $url"
curl -fsSL -o "$asset" "$url"
tar xzf "$asset"
rm -f "$asset"
ls -la "$DEST/wabt-$VERSION/bin"
