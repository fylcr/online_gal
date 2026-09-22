# Verifies the new gettext port patch the way vcpkg applies it: `git apply -p1` from the source root.
# Kept as a script rather than an inline shell string so the output is not mangled by nested quoting.
set -u

. "$(dirname "$0")/../../env.sh"

D="$REF_DIR/vcpkg"
P="$D/ports/gettext/fix-mingw-gcc15-localtime-env-loop.patch"
SRC="$D/buildtrees/gettext/src/gettext-0-5736b92587.clean"

echo "== patch: $P"
echo "== CR bytes in patch: $(tr -cd '\r' < "$P" | wc -c) (expect 0)"
echo "== PATCHES references in portfile: $(grep -c 'fix-mingw-gcc15-localtime-env-loop.patch' "$D/ports/gettext/portfile.cmake") (expect 1)"
echo "== last PATCHES entry:"
grep -n -B1 -A1 'fix-mingw-gcc15-localtime-env-loop.patch' "$D/ports/gettext/portfile.cmake"

rm -rf /tmp/gpcheck
mkdir -p /tmp/gpcheck/gettext-tools/gnulib-lib
cp "$SRC/gettext-tools/gnulib-lib/localtime.c" /tmp/gpcheck/gettext-tools/gnulib-lib/
cd /tmp/gpcheck || exit 1

echo
echo "== git apply --check -p1 (non-repo dir, same operation vcpkg performs) =="
git apply -p1 --check -v "$P" 2>&1
echo "check exit=$?"

echo
echo "== git apply -p1 =="
git apply -p1 "$P" 2>&1
echo "apply exit=$?"

echo
echo "== fixed lines present =="
grep -n 'char \*\*s = env' gettext-tools/gnulib-lib/localtime.c
grep -n 'wchar_t \*\*ws = wenv' gettext-tools/gnulib-lib/localtime.c
grep -n '(\*s)\[0\]' gettext-tools/gnulib-lib/localtime.c
grep -n '(\*ws)\[0\]' gettext-tools/gnulib-lib/localtime.c

echo
echo "== old (buggy) lines remaining — expect no output =="
grep -n 'for (char \*s = env\|for (wchar_t \*ws = wenv\|if (s\[0\]\|if (ws\[0\]' gettext-tools/gnulib-lib/localtime.c

echo
echo "== line count before/after (expect 77 both) =="
wc -l "$SRC/gettext-tools/gnulib-lib/localtime.c" /tmp/gpcheck/gettext-tools/gnulib-lib/localtime.c
