# Two control experiments that decide how to make a gettext port patch actually take effect.
#
#  1. pkgconf: is the documented local edit to .ref/vcpkg/ports/pkgconf/portfile.cmake effective,
#     or did vcpkg hash the git blob instead? (same question as for our new gettext patch)
#  2. ffmpeg: do the ports under .ref/kirikiroid2-web/vcpkg/ports (VCPKG_OVERLAY_PORTS) actually win?
#     If yes, an overlay port is a proven route for gettext.
set -u
. "$(dirname "$0")/../../env.sh"
V="$REF_DIR/vcpkg"
O="$REF_DIR/kirikiroid2-web/vcpkg/ports"
cd "$V" || exit 1

abi_portfile() {   # $1 = port, $2 = triplet
  grep '^portfile.cmake ' "buildtrees/$1/$2.vcpkg_abi_info.txt" 2>/dev/null | awk '{print $2}'
}

echo "== 1. pkgconf (builtin port, locally modified, uncommitted) =="
echo "on-disk            : $(sha256sum ports/pkgconf/portfile.cmake | cut -d' ' -f1)"
echo "git HEAD blob      : $(git cat-file blob HEAD:ports/pkgconf/portfile.cmake | sha256sum | cut -d' ' -f1)"
echo "ABI recorded (mingw): $(abi_portfile pkgconf x64-mingw-static)"
for t in $(ls buildtrees/pkgconf/*.vcpkg_abi_info.txt 2>/dev/null | xargs -n1 basename 2>/dev/null | sed 's/\.vcpkg_abi_info\.txt//'); do
  echo "ABI recorded ($t): $(abi_portfile pkgconf "$t")"
done
echo
echo "== 2. ffmpeg: overlay port vs builtin port =="
echo "overlay on-disk    : $(sha256sum "$O/ffmpeg/portfile.cmake" | cut -d' ' -f1)"
echo "builtin on-disk    : $(sha256sum ports/ffmpeg/portfile.cmake | cut -d' ' -f1)"
echo "builtin HEAD blob  : $(git cat-file blob HEAD:ports/ffmpeg/portfile.cmake | sha256sum | cut -d' ' -f1)"
for t in wasm32-emscripten x64-mingw-static; do
  echo "ABI recorded ($t): $(abi_portfile ffmpeg "$t")"
done
echo
echo "== 3. glib: overlay port (emscripten-specific patch) =="
echo "overlay on-disk    : $(sha256sum "$O/glib/portfile.cmake" | cut -d' ' -f1)"
echo "builtin HEAD blob  : $(git cat-file blob HEAD:ports/glib/portfile.cmake | sha256sum | cut -d' ' -f1)"
for t in wasm32-emscripten x64-mingw-static; do
  echo "ABI recorded ($t): $(abi_portfile glib "$t")"
done
