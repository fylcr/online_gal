# P2 batch 3 — f3785 (`artc::DispatchResourceStep`)

**Tree location:** the asyncify products these notes count have since moved from
`.ref/artemis-official/rewrite/src` to **`ports/artemis/src`** (tracked source, alongside
`ports/artemis/rewrite-names.tsv` and `ports/artemis/REWRITE.md`).  Paths below are the
ones that were true when each note was written.


批 3 只有一件：**f3785**（3,228 源行，全库最重的资源/sha1 缓存泵）。
派活前 lead 独立跑脚本核过结构数字，简报三处错认已在 § 简报纠错 里记账。

## 交付

| 项 | 值 |
| --- | --- |
| fnid | f3785 |
| 名 | `artc::DispatchResourceStep` |
| 文件 | `.ref/artemis-official/rewrite/src/util/dispatch_resource_step.cpp` |
| 源区间 | `artemis.wasm2c.c` 2136185-2139412（3,228 行） |
| 签名 | `uint32_t DispatchResourceStep(EngineContext*, uint32_t state, uint32_t slot)` |
| save frame | 56 字节（g4 -= 56u，回填 14 个字段含 p0/p1 覆写） |
| resume word | 4 字节 |
| 结构 | 66 标签 / 132 goto / 275 if / 1 return / 182 次 `w2c_g3` / 78 次 `852316u` |
| callee | `Oc_0`×37（DeallocateRawBlock）、`f202`×7（CopyBytes）、`f310`×2（StoreGlobalEntry）、`f200`×1（CompareBytes） |

**批 3 零新 callee 前向器** —— 4 个 callee 全部已有 tsv 名与产品。

## 新增 host 包装（5 条，落在 `engine_context.h` 384-417）

| 生成名 | 签名 | 包装名 | 命名证据 |
| --- | --- | --- | --- |
| `w2c_a_d` | `void (a*, u32, u32)` | `HostTableDispatch2` | 表派发族：全库 3,797 调用点 / 570 owner，f3785 内首参恒 `1286u` |
| `w2c_a_h` | `u32 (a*, u32, u32)` | `HostTableQuery2` | 1,297 点 / 399 owner，首参 `1032` 占 882 次（68%） |
| `w2c_a_s` | `void (a*, u32×6)` | `HostTableDispatch6` | 全库唯一 6 参 void，109 点 / 58 owner，f3785 内首参 `2015u` |
| `w2c_a_f` | `void (a*, u32×4)` | `HostTableDispatch4` | 1,363 点 / 258 owner；与 a_fa 同签名，只能靠 owner 面区分 |
| `w2c_a_fa` | `void (a*, u32×4)` | `HostAsyncFetchData` | 唯一真旁证：全库仅 3 调用方 = f3785 / f3822 / f3823，三者同构（帧壳 + 单次 a_fa + 重入回填，20/24/56 字节帧） |

**死路（勿重试）**：host import 真名不可考 —— 游戏目录无 js 胶水；`ports/` 内 `grep importObject|instantiateWasm` 只命中产物 `.js`；`.ref/artemis-compat` 全树零命中；wat import 段（`artemis.wasm.wat:102-160`）全是 `(import "a" "b" (func …))` 单/双字母。命名只能靠结构 + 常量语义，**禁止臆造出处**。

## 简报纠错（lead 侧脚本复核后发现的三处错认）

1. **「0 个 `var_L*` 标签」错**：实为 6 个（L50/149/229/239/244/249），且生成器写成
   `var_L50: ` —— **尾随空格、无分号**。源侧标签正则必须 `^\s*\w+:\s*;?\s*$`，
   只 grep `var_B*:;` 会数成 60 并凭空造出 6 个差异。
2. **callee 表漏 `w2c_artemis_Oc_0`**（37 处）→ `DeallocateRawBlock`。
3. **池址是十进制**：`4294967295u`×38、`4294967280u`×14（=0xFFFFFFE0，**不是**
   0xFFFFFFF0）、`2147483648u`×7。名字化正则要按十进制字面量匹配。
   附带：本函数无 `var_B4`，`var_B4 -> running_exit` 映射不适用。

## 越权改动（lead 处置）

- **回退**：`HostAssertFail` 返回类型被子代理 void→uint32_t，但函数体丢弃
  `w2c_a_q(...)` 结果无 return。生成层 `artemis.wasm2c.h:679` 是 `void`。
  三个调用点（`util/unordered_check_load_factor.cpp:337`、
  `util/unordered_find_entry.cpp:240`、`util/unordered_prepare_slot.cpp:226`）
  都是独立语句丢弃返回值 → 无行为影响，但头文件说谎。已改回 `inline void`。
- **保留**：`void wasm_rt_segue_write_base(uint8_t*)` 前置声明换成
  `#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE` 守卫的 `SyncMemoryBase` 实现。
  核证 `artemis.wasm2c.c:71` 是 `static inline`（file-scope static），产品无法链接。
  旧声明版本见 `D:/tmp/src-backup-batch2/util/engine_context.h:337`；批 2 基线已有
  2 个产品（`script/pump_event_queue.cpp`、`util/reallocate_block.cpp`）在调
  `SyncMemoryBase` → 判定为批 3 之前**潜伏的链接缺陷**，本次被消除。
  lead 逐处回溯 f3785 的 103 处 `SyncMemoryBase`，103/103 在守卫内。

## Lead verification (not taken on trust)

四闸全部链内 `set -o pipefail`：

- `style-gate.sh` → `== style-gate: PASS ==` exit 0
  （warning 仅 `allocate_string_buffer.cpp:37,45,46,47` + `dispatch_resource_step.cpp:37` 裸池址）
- `code-assemble.sh asyncify` → `cxx-route.wasm 41790336 bytes`、
  `63 fragments, 5562 defs kept`、`rw-splice: 59 product(s) spliced`（58→59）、
  `agreement: 66/66, trap-kind-only: 0, differ: 0`、
  `19 returned equal values, 47 threw the same trap`、`ASM_EXIT=0`、
  `built by em++ alone; wabt was not invoked.`
- `wasm-diff.mjs --strict` → `STRICT_EXIT=0`
- `build-bridge-route.sh` → exit 0
  （`renamed export '__indirect_function_table' -> 'Lc' (41,802,957 -> 41,802,934 bytes)`）

结构保真（多重集，先 strip_comments）：goto 132/132、labels 66/66、if 275/275、
return 1/1；16 个 `kF3785*` 常量全部有使用。

## 🔴 深锚红项与定案：基线文件认错了父

首跑深锚报两处红：**f805 117.6K→760（155×）**、**f3785 只 2 次**（池位测算记 29,226）。

**定案：引擎无回归，红项全部是基线错误。**

**分裂定位**（按 tsv 产品集把两次 profile 拆层）：

- 产品层 6,209,996 → 4,551,487（**-26.71%**）
- 生成层 1,704,899 → 1,693,459（-0.67%）
- 零产品增加、41 掉、45 不变

测量机制健全性核查：b3 树里 `EnterFrame();` 锚点戳 = 59，`FUNC_PROLOGUE;` 锚点戳 = 5,520，
无第三类；而 `EnterFrame()` 就是 `inline void EnterFrame() { FUNC_PROLOGUE; }`
（`engine_context.h:43`），两路对称。

**根因**：`D:/tmp/batch2-frame-anchor.json`（mtime 11:53:22）内含 f707=73,982、
f1119=28,130、f346=54,588、f1125=37,155 等**批 2 产品**的值；但它旁边 mtime 10:45 的
`out/bridge-asyncify-profile/artemis.cpp` 只有 53 个批 1 标记（b3-only = [346,707,1119,1125,3785,4082]）。
→ **该 JSON 来自一个已被覆盖的更晚构建，不是它旁边那台引擎的产物。**
总调用 7,914,895 → 6,271,651；>3000 差异的 20 个 fnid 几乎全是产品层
（f202 452,862→102,263、f201 424,658→44,808、f463 212,983→8,466、f805 117,611→760、
f1042 29,234→10、f3785 29,226→2）。30 最热 ratio 分布 min=0.23 / **median=1.00** / max=1.00
⇒ 纯尾部产品层塌陷，非全局漂移。

**正确基线（重新建立，作后续基线用）**：用 `D:/tmp/src-backup-batch2`（11:57 批 2 收官态，
engine_context.h 19,319 B）还原树 + 删 tsv 的 f3785 行 →
`bash ports/artemis/reverse/build-bridge-route.sh asyncify --profile "D:/tmp/pre-b3-profile"`
→ 58 products。**pre-b3 与 post-b3 的 settle 运行逐位一致**：

| fnid | post-b3（4 跑均值） | pre-b3（2 跑均值） | 判定 |
| --- | --- | --- | --- |
| f428 | 294,732 | 294,732 | OK |
| f393 | 279,041 | 279,041 | OK |
| f244 | 218,441 | 218,441 | OK |
| f1727 | 218,441 | 218,441 | OK |
| f340 | 125,199 | 125,199 | OK |
| f805 | 760 | 760 | OK（红项消解） |
| f778 | 108,938 | 108,938 | OK |
| f3785 | 2 | 2 | 一致（红项消解） |

`f206 ≡ f5508` = 668,392 双方成立。产品层 +0.29%、生成层 +0.66%（正常 session 漂移）。
帧锚 PASS：official/candidate 均 `canvas 2560x1398 lit 97.11% frames 3 settled 550f7464`
（candidate mean 247.61 / official 246.88）。

## 排除项（都已实测，勿重试）

1. f3785 表项 `{RefFunc, w2c_artemis_t3, wrap_w2c_artemis_f3785, {NULL}, 0}` @:105995
   在两引擎**逐字节相同** → 间接调用路径无差异。
2. `#define WASM_RT_USE_SEGUE_FOR_THIS_MODULE` 两引擎同为 1/0 分支，宏解析为 0 →
   `SyncMemoryBase` 守卫不解释计数损失（batch3 树多 103 处调用是**新增**调用，非丢失）。
3. f3785 在 batch-2 引擎里也不是产品（同样只经表 wrapper 到达）。

## 台面坑（复现必读）

- `ports/artemis/tools/real-game-test.mjs` **不接 `--outdir`**（unknown argument, exit 2）；
  必接 `--game DIR --engine FILE`，可选 `--wait MS --port N --json-out FILE --cdp-port`；
  裸跑 exit 2 打 usage。解析表 @`:72-92`。
- 报告 `control.profile` 恒 `null`（官方侧无剖析）；`candidate.profile` 才是
  `[fnid, calls, firstrun_order]` 三元组数组，**按 fnid 数字索引，不是名字**。
- **bench 双侧都会 flake**：实测 6 跑里 2 跑 official 未渲染
  （`FAIL: the official build did not render on this bench`）、2 跑 candidate stalled
  （`frames 1 settled stalled`）→ **只有 `finalHash=='550f7464'` 的 run 才带可用计数**。
- 剖析版构建形态 = `build-bridge-route.sh asyncify --profile "OUTDIR"`（第二位置参是 OUTDIR，`:76`）。
- 良性 404 噪声（`pc/ja/caution`、`font/extra_12.rft.rft` 等）。

## Batch 3 close

- 落地 1 件 R1 产品 + 5 条 host 包装；`rewrite/src` 59 cpp（今 `ports/artemis/src`）；
  `rewrite-names.tsv` 141 行 landed。
- 批 1+2+3 = 39 件全部有计数、无一件为 0。
- **备份教训**：批 3 那次 `cp -r rewrite/src D:/tmp/src-backup-batch3`（当时树名）报 "backup ok: 61 files"，
  但事后 `diff -u` 无输出 —— 备份是子代理改完之后的快照。可用基线是
  `D:/tmp/src-backup-batch2`；批 3 收官态是 `D:/tmp/src-backup-batch3-good`
  （engine_context.h 21,114 B）。
- **批 4 待办**：34 件 ≥10K（见 REWRITE.md 与本文件上文池位表）；
  巨件 f899（5,386 行，lparser `forbody`）、f255（654 行，boost shared_ptr 异常展开）；
  39 个待命名 callee（f219/f397 已是前向器）。
