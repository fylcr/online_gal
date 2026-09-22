# Independently verifies the vlfs.js host-byte-source patcher and its effect.
# Uses drive-letter paths (D:/...) for `node`, because the native Windows node does not resolve
# MSYS `/d/...` paths (it turns them into `D:\d\...`).
set -u

. "$(dirname "$0")/../../env.sh"

PATCHER="$(ports_win "$PORTS_ROOT/ports/krkr/kirikiroid2/patch-vlfs-host-source.mjs")"
REPO="$(ports_win "$REF_DIR/kirikiroid2-web")"
VLFS="$REPO/platforms/web/vlfs.js"
GIT="$REF_DIR/kirikiroid2-web"

echo "== 1. patcher syntax =="
node --check "$PATCHER"; echo "node --check patcher exit=$?"

echo
echo "== 2. restore clean base, then apply once =="
git -C "$GIT" checkout -- platforms/web/vlfs.js
echo "clean base md5: $(md5sum "$VLFS" | cut -d' ' -f1)"
node "$PATCHER"; echo "apply exit=$?"
node --check "$VLFS"; echo "node --check vlfs exit=$?"
echo "marker counts: __onlineGalVfsSource=$(grep -c '__onlineGalVfsSource' "$VLFS")  hostByteSource=$(grep -c 'function hostByteSource() {' "$VLFS")"

echo
echo "== 3. idempotency: apply a second time, compare md5 =="
md5_before=$(md5sum "$VLFS" | cut -d' ' -f1)
node "$PATCHER"; echo "second apply exit=$?"
md5_after=$(md5sum "$VLFS" | cut -d' ' -f1)
echo "md5 before=$md5_before"
echo "md5 after =$md5_after"
[ "$md5_before" = "$md5_after" ] && echo "IDEMPOTENT: identical" || echo "NOT IDEMPOTENT"

echo
echo "== 4. the diff must be exactly the four insertions =="
git -C "$GIT" diff --stat -- platforms/web/vlfs.js
git -C "$GIT" diff -- platforms/web/vlfs.js

echo
echo "== 5. no other file in the repo touched =="
git -C "$GIT" status --porcelain
