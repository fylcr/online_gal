#!/usr/bin/env bash
# Fixed upstream archive + separate Web platform. Never patches the engine tree.
set -euo pipefail
PORT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
die() { echo "compat-web-clean: $*" >&2; exit 1; }
CANDIDATE="${ARTC_CLEAN_CANDIDATE:?set ARTC_CLEAN_CANDIDATE to a new isolated name}"
[[ "$CANDIDATE" =~ ^[A-Za-z0-9][A-Za-z0-9_-]*$ ]] || die 'invalid candidate'
CLOCK="${ARTC_CLEAN_CLOCK:-}"
MANIFEST="$PORT/source-manifest.json"
case "$CLOCK" in
  '') ;;
  clock-ms-01) MANIFEST="$PORT/candidates/clock-ms-01/manifest.json" ;;
  *) die 'invalid ARTC_CLEAN_CLOCK; only clock-ms-01 is supported' ;;
esac
mkdir -p "$PORT/out"
OUT="$(cd "$PORT/out" && pwd -P)"
[[ "$OUT" == "$PORT/out" ]] || die 'out must not be a symlink'
RUN="$OUT/$CANDIDATE"
if [[ -e "$RUN" ]]; then
  [[ -d "$RUN" && ! -L "$RUN" ]] || die 'invalid existing candidate'
  [[ "$(cd "$RUN" && pwd -P)" == "$RUN" ]] || die 'candidate escaped out'
  # Only an otherwise empty git cache may be resumed. Never overwrite prior output.
  [[ "$(find "$RUN" -mindepth 1 -maxdepth 1 -printf '%f\n')" == gitrepo ]] || die 'candidate already has output; choose a new name'
else
  mkdir "$RUN"
fi
readarray -t ID < <(node -e 'const m=require(process.argv[1]); console.log(m.origin); console.log(m.commit); console.log(m.tree)' "$MANIFEST")
[[ ${#ID[@]} == 3 ]] || die 'invalid source manifest'
ORIGIN="${ID[0]}"; COMMIT="${ID[1]}"; TREE="${ID[2]}"
[[ "$COMMIT" =~ ^[0-9a-f]{40}$ && "$TREE" =~ ^[0-9a-f]{40}$ ]] || die 'invalid git identity'
# Fail before fetching/configuring when the next writer has not supplied the platform.
node -e 'const fs=require("node:fs"),p=require("node:path"),crypto=require("node:crypto"),r=process.argv[1],m=require(process.argv[2]); for(const f of m.platformFiles) if(!fs.statSync(p.join(r,f),{throwIfNoEntry:false})?.isFile()) throw Error("Lane B platform missing: "+f); if(m.clockCandidate && crypto.createHash("sha256").update(fs.readFileSync(p.join(r,m.clockCandidate.replacementPath))).digest("hex")!==m.clockCandidate.replacementSha256) throw Error("Clock replacement hash mismatch")' "$PORT" "$MANIFEST"
[[ -z "${EM_CONFIG:-}" ]] || die 'EM_CONFIG is set; unset it to avoid SDK cache identity changes'
. "$PORT/../../env.sh"
[[ -n "$EMSDK_DIR" ]] || die 'SDK missing; installation is not part of this recipe'
SDK="$(cd "$(ports_unix "$EMSDK_DIR")" && pwd -P)"
EM="$SDK/upstream/emscripten"
[[ -f "$EM/emcc" && -f "$EM/emcmake" && -f "$SDK/.emscripten" ]] || die 'incomplete SDK'
# Emscripten requires an absolute EM_CACHE; use the same root for ZLIB.
if [[ -n "${EM_CACHE:-}" ]]; then
  case "$EM_CACHE" in
    /*|[A-Za-z]:/*|[A-Za-z]:\\*) ;;
    *) die 'EM_CACHE must be an absolute cache path' ;;
  esac
  CACHE_ROOT="$(ports_unix "$EM_CACHE")"
  [[ -d "$CACHE_ROOT" ]] || die 'EM_CACHE directory missing; no automatic installation'
  CACHE_ROOT="$(cd "$CACHE_ROOT" && pwd -P)"
  export EM_CACHE="$(ports_win "$CACHE_ROOT")"
else
  # The SDK config parser does not accept an explicitly empty EM_CACHE.
  unset EM_CACHE
  CACHE_ROOT="$EM/cache"
fi
SYSROOT="$CACHE_ROOT/sysroot"
[[ -f "$SYSROOT/include/zlib.h" ]] || die "zlib headers missing in effective cache: $SYSROOT"
ZLIB="$SYSROOT/lib/wasm32-emscripten/libz.a"
[[ -f "$ZLIB" ]] || die "zlib library missing in effective cache: $SYSROOT; no automatic installation"
CACHE="$RUN/gitrepo"
if [[ ! -e "$CACHE" ]]; then
  [[ "${ARTC_CLEAN_OFFLINE:-0}" != 1 ]] || die 'offline cache missing'
  git clone --no-checkout "$ORIGIN" "$CACHE"
  git -C "$CACHE" fetch origin "$COMMIT"
  git -C "$CACHE" checkout --detach "$COMMIT"
fi
[[ ! -L "$CACHE" && "$(cd "$CACHE" && pwd -P)" == "$RUN/gitrepo" ]] || die 'git cache escaped candidate'
CACHE_TOP="$(git -C "$CACHE" rev-parse --show-toplevel)"
[[ "$(cd "$(ports_unix "$CACHE_TOP")" && pwd -P)" == "$CACHE" ]] || die 'not dedicated git cache'
[[ "$(git -C "$CACHE" remote get-url origin)" == "$ORIGIN" ]] || die 'cache origin mismatch'
[[ -z "$(git -C "$CACHE" status --porcelain --untracked-files=all)" ]] || die 'dirty git cache'
[[ "$(git -C "$CACHE" rev-parse HEAD)" == "$COMMIT" ]] || die 'cache HEAD mismatch'
if ! git -C "$CACHE" cat-file -e "$COMMIT^{commit}"; then
  [[ "${ARTC_CLEAN_OFFLINE:-0}" != 1 ]] || die 'offline commit missing'
  git -C "$CACHE" fetch origin "$COMMIT"
fi
[[ "$(git -C "$CACHE" rev-parse "$COMMIT^{tree}")" == "$TREE" ]] || die 'tree mismatch'
# All archive extraction happens in a newly created, absolute, guarded child.
SOURCE="$RUN/source"
[[ ! -e "$SOURCE" ]] || die 'source exists'
mkdir "$SOURCE"
[[ "$(cd "$SOURCE" && pwd -P)" == "$OUT/$CANDIDATE/source" ]] || die 'archive destination escaped out'
git -C "$CACHE" archive --format=tar "$COMMIT" > "$RUN/upstream.tar"
tar -xf "$RUN/upstream.tar" -C "$SOURCE"
cp "$MANIFEST" "$RUN/source-manifest.json"
node - "$PORT" "$RUN/port-hashes.json" "$MANIFEST" <<'JS'
const fs=require('node:fs'),path=require('node:path'),crypto=require('node:crypto');
const root=process.argv[2],m=require(process.argv[4]),hashes={};
function walk(p) { const s=fs.lstatSync(path.join(root,p)); if(s.isDirectory()) for(const f of fs.readdirSync(path.join(root,p))) walk(path.join(p,f)); else if(s.isFile()) hashes[p.split(path.sep).join('/')]=crypto.createHash('sha256').update(fs.readFileSync(path.join(root,p))).digest('hex'); else throw Error('Unsupported port entry: '+p); }
for(const p of [...m.recipeFiles,'host','platform','audio',...(m.clockCandidate ? ['candidates/clock-ms-01/lua_engine.cpp'] : [])]) walk(p);
fs.writeFileSync(process.argv[3],JSON.stringify(hashes,null,2)+'\n');
JS
# Byte inventory accounts for git archive attributes, rather than checkout line endings.
node --input-type=module - "$SOURCE" "$RUN/source-hashes.json" <<'JS'
import fs from 'node:fs'; import path from 'node:path'; import crypto from 'node:crypto';
const root=process.argv[2], hashes={};
function walk(dir) { for(const e of fs.readdirSync(dir,{withFileTypes:true})) { const p=path.join(dir,e.name); if(e.isDirectory()) walk(p); else if(e.isFile()) hashes[path.relative(root,p).split(path.sep).join('/')]=crypto.createHash('sha256').update(fs.readFileSync(p)).digest('hex'); else throw Error('unsupported archive entry: '+p); } }
walk(root); fs.writeFileSync(process.argv[3],JSON.stringify(hashes,null,2)+'\n');
JS
"$EM/emcmake" cmake -S "$(ports_win "$PORT")" -B "$(ports_win "$RUN/build")" \
  -DARTC_CLEAN_SOURCE="$(ports_win "$SOURCE")" -DARTC_CLEAN_CLOCK="$CLOCK" \
  -DZLIB_INCLUDE_DIR="$(ports_win "$SYSROOT/include")" -DZLIB_LIBRARY="$(ports_win "$ZLIB")" \
  -DCMAKE_BUILD_TYPE=Release
cmake --build "$(ports_win "$RUN/build")" "$@"
node "$PORT/make-source-bundle.mjs" "$CANDIDATE"
echo "Isolated output: $RUN/build (no product copying or runtime rewiring)"
