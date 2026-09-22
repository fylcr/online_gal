# Verifies the overlay-install loop that run-build.sh now uses: extracted verbatim so the glob,
# basename and destination naming are exercised for real, twice, to prove it is idempotent.
set -u

. "$(dirname "$0")/../../env.sh"

REPO="$REF_DIR/kirikiroid2-web"
OVERLAY_PORTS_DIR="$PORTS_ROOT/ports/krkr/kirikiroid2/overlay-ports"

echo "== bash -n run-build.sh =="
if bash -n "$PORTS_ROOT/ports/krkr/kirikiroid2/run-build.sh"; then echo "syntax OK"; else echo "SYNTAX FAIL"; exit 1; fi

run_loop() {
  for _overlay_port in "$OVERLAY_PORTS_DIR"/*/; do
      [ -d "$_overlay_port" ] || continue
      _overlay_name="$(basename "$_overlay_port")"
      rm -rf "$REPO/vcpkg/ports/$_overlay_name"
      cp -r "$_overlay_port" "$REPO/vcpkg/ports/$_overlay_name" || exit 1
      echo "## overlay port: $_overlay_name -> $REPO/vcpkg/ports/$_overlay_name"
  done
}

echo
echo "== loop run 1 =="
run_loop
echo "== loop run 2 =="
run_loop

echo
echo "== resulting overlay ports in \$REPO/vcpkg/ports =="
ls -1 "$REPO/vcpkg/ports" | sort | tr '\n' ' '
echo
echo "libexif host-triplet line count : $(grep -c 'VCPKG_MAKE_BUILD_TRIPLET' "$REPO/vcpkg/ports/libexif/portfile.cmake")   (expect 1)"
echo "gettext patch entry count       : $(grep -c 'fix-mingw-gcc15' "$REPO/vcpkg/ports/gettext/portfile.cmake")   (expect 1)"
echo "libexif vcpkg.json present      : $([ -f "$REPO/vcpkg/ports/libexif/vcpkg.json" ] && echo yes || echo NO)"
echo "nested dirs from double run     : $(ls -d "$REPO/vcpkg/ports/gettext/gettext" "$REPO/vcpkg/ports/libexif/libexif" 2>/dev/null | wc -l)   (expect 0)"
