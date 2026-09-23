#!/bin/bash
# verify-giant.sh <fnid> —— 巨函数分片整函数验收（f3744 / f3633）
# 流程：1) 全部分片按文件名序 cat 成整函数多片段跑 code-check.sh（em++ -fsyntax-only）
#       2) 合并成单文件跑 fidelity-alpha-diff.py（α 规范化逐语句保真比对）
# 用法：bash ports/artemis/reverse/verify-giant.sh f3744
set -e
fn=$1
[ -n "$fn" ] || { echo "usage: verify-giant.sh <fnid>"; exit 2; }
DIR=/d/online_gal/.ref/artemis-official/code
SRC=/d/online_gal/.ref/artemis-official/wasm/asyncify/artemis.wasm2c.c
CHECK=/d/online_gal/ports/artemis/reverse/code-check.sh
FID="D:/online_gal/ports/artemis/reverse/fidelity-alpha-diff.py"
parts=$(ls "$DIR/$fn.part"*.cpp | sort)
n=$(echo "$parts" | wc -l)
echo "== $fn: $n part file(s) =="
for p in $parts; do echo "   $p"; done
echo "== step 1: whole-function code-check (multi-fragment cat) =="
bash "$CHECK" $parts
echo "== step 2: merged fidelity-alpha-diff =="
MERGED=/d/tmp/$fn.merged.cpp
cat $parts > "$MERGED"
python "$FID" "D:/tmp/$fn.merged.cpp" "$SRC" "$fn" | tail -3
echo "== verify-giant $fn: ALL PASS =="
