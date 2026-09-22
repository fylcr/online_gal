#!/usr/bin/env bash
# Fetch the pinned Zig release that rebuild.sh uses for its `so` check.
#
# The `so` half of the reverse-engineering claim is "the export surface can be
# rebuilt", and that needs a compiler which can emit an ELF shared object for
# both Android ABIs. This machine has no AArch64 toolchain and no NDK, and the
# two obvious substitutes do not work:
#
#   * MinGW's gcc always links x86-64 COFF. `-shared -fPIC` there produces a file
#     with no dynamic symbol table, and it rewrites weak symbols to
#     `.weak.<name>.<anchor>`, so the export diff fails for reasons unrelated to
#     the recipe.
#   * The emsdk's clang has LLVM built for `wasm32 wasm64 x86 x86-64` only.
#     `--target=aarch64-linux-android` fails with "unable to create target".
#
# Zig bundles libc and a linker for both ABIs, so one download removes the
# dependency on an NDK install. The version is pinned for the same reason wabt's
# is: the check compares against a fixed expected symbol count, so the tool that
# produces the candidate list should not drift underneath it.

set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DEST="$HERE/build"
VERSION="0.16.0"

if [ -x "$DEST/zig-x86_64-windows-$VERSION/zig.exe" ]; then
    echo "zig $VERSION already present at $DEST/zig-x86_64-windows-$VERSION"
    exit 0
fi

mkdir -p "$DEST"
cd "$DEST"

case "$(uname -s)" in
MINGW* | MSYS* | CYGWIN*) asset="zig-x86_64-windows-$VERSION.zip" ;;
Darwin)
    case "$(uname -m)" in
    arm64) asset="zig-aarch64-macos-$VERSION.tar.xz" ;;
    *) asset="zig-x86_64-macos-$VERSION.tar.xz" ;;
    esac
    ;;
*)
    case "$(uname -m)" in
    aarch64) asset="zig-aarch64-linux-$VERSION.tar.xz" ;;
    *) asset="zig-x86_64-linux-$VERSION.tar.xz" ;;
    esac
    ;;
esac

url="https://ziglang.org/download/$VERSION/$asset"
echo "downloading $url"
curl -fsSL -o "$asset" "$url"
case "$asset" in
*.zip) unzip -q -o "$asset" ;;
*) tar xf "$asset" ;;
esac
rm -f "$asset"
ls -d "$DEST"/zig-*
