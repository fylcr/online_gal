# P2 批 4 收口：A 组 23 件 + 局部类型截断根因修复

状态：**全绿**（四闸 + 双锚 + 3 连跑）
落盘态：`D:/tmp/src-fixed-b4/src`（81 .cpp）+ `D:/tmp/names-fixed-b4.tsv`（181 行）
教训入库：`REWRITE.md §7.6`

## 1. 交付物

A 组 23 件 R1 产品（全部由 `tx_gen2.py` 机械发射，人工只做命名与类型修正）：

| fnid | 文件 | 签名 | 源行 | frame |
|---|---|---|---|---|
| f205 | util/fill_bytes.cpp | `uint32_t(dst,value,length)` | 198 | 0 |
| f215 | util/unlink_list_node.cpp | `void(node,victim)` | 262 | 0 |
| f264 | script/resolve_expr_value.cpp | `uint32_t(state,tag)` | 132 | 0 |
| f341 | script/load_expr_value.cpp | `void(state,tag)` | 201 | 16 |
| f401 | util/shift_wide_digits.cpp | `void(dst,hi,lo,bytes)` | 53 | 0 |
| f493 | script/compare_numeric_literal.cpp | `uint32_t(state,lhs,rhs)` | 457 | 0 |
| f603 | util/raw_value_equal.cpp | `uint32_t(a,b)` | 53 | 0 |
| f604 | util/relocate_object_header.cpp | `void(state,source,tag)` | 29 | 0 |
| f611 | util/read_live_node_payload.cpp | `uint32_t(node)` | 34 | 0 |
| f1120 | util/bucket_log2.cpp | `uint32_t(value)` | 44 | 0 |
| f1493 | util/clear_integer_constant.cpp | `uint32_t(slot,tag)` | 38 | 0 |
| f1702 | script/compile_comparison.cpp | `uint32_t(func_state,lhs,rhs)` | 427 | 20 |
| f1709 | script/initialize_table_array.cpp | `void(func_state,table,size)` | 300 | 20 |
| f3514 | core/dispatch_class_method.cpp | `uint32_t(state,key)` | 204 | 12 |
| f4230 | util/sweep_marked_node_list.cpp | `void(pool)` | 159 | 16 |
| f4240 | util/sweep_secondary_node_list.cpp | `void(pool)` | 159 | 16 |
| f4715 | util/sync_property_vector.cpp | `void(state,index,scale,flag)` | 412 | 8 |
| f4881 | util/invoke_node_handler.cpp | `void(node,slot,scale)` | 115 | 8 |
| f4882 | util/invoke_sibling_node_handler.cpp | `void(node,slot,scale)` | 122 | 8 |
| f5511 | script/next_table_entry.cpp | `uint32_t(state,table,key)` | 429 | 16 |
| f5726 | script/set_table_array_element.cpp | `void(state,table,index)` | 173 | 12 |
| f5749 | util/compare_node_payloads.cpp | `uint32_t(a,b)` | 23 | 0 |

`core/` 目录为批 4 新建（f3514 = 327 class × 4 槽派发，属于核心门面语义）。
frame=0 的 11 件不发 `kFNNNSaveFrameBytes`（源里没有挂起点，发了就是谎报）。

## 2. 闸门记录（全部链内 `set -o pipefail`）

```
style-gate      PASS        5 处裸池址 warning（既有，非本批新增）
code-assemble   exit 0      63 fragments / 5562 defs kept
                           rw-splice: 81 product(s) spliced
                           differ: 0
                           cxx-route.wasm 41,784,481 bytes
                           wabt was not invoked
wasm-diff       exit 0      66/66, trap-kind-only: 0, differ: 0
build-bridge    exit 0      bridge.wasm
```

帧锚（`real-game-test.mjs --wait 60000`）三连跑逐字相同：

```
run  pass  rendered  ran  finalHash  lit   mean   frames  req  canvas
1    True  True      True 550f7464   97.11 247.61  3       87   2560x1398
2    True  True      True 550f7464   97.11 247.61  3       87   2560x1398
3    True  True      True 550f7464   97.11 247.61  3       87   2560x1398
```

与批 3 好签名逐字一致（`550f7464 / 97.11% / 247.61 / 3 frames / 87 files`）。

**窗口尺寸会漂移，判据不认绝对 hash**：Chrome 窗口曾从 2560x1398 缩到 1920x945，
好签名 hash 随之由 `550f7464` 变 `a2e1dcf5`、lit 由 97.11% 变 87.5%。
**好/坏判据 = 请求数（87 vs 83）+ lit 百分比 + frames 数**，
hash 只在同一 session era 内作辅助。此前的 `finalHash=='550f7464'` 过滤法已作废。

## 3. 黑屏根因（REWRITE.md §7.6 全文）

批 4 四闸全绿但真游戏黑屏。定位过程：

1. **受控 A/B**（同 bench 同 session）：批 3 树 3/3 `exit 0` 双侧同签名，批 4 树 0/3
   `exit 1` 逐字相同（`1920x945 / lit 0 / frames 1 / req 83`）⇒ 硬回归，与 `--profile` 无关。
2. **静态六轴全绿**：span 边界、sp 字面量、t 派发参数序、frame 常量、`LeaveFrame` 覆盖率、
   形参签名 ⇒ 机械转写本身没错。
3. **请求清单差分**（`performance.getEntriesByType('resource')` 临时打点，用完还原）：
   首个分歧在 index 64，批 4 缺 `system/{save,script,ui}.asb`、`sound/sysse/none.ogg`、
   `image/bg/white`、`pc/ja/caution`，多取 `pc/ja/title/{btn,bg}.png`
   ⇒ 走了不同标题分支。
4. **二分探针**（`D:/tmp/probe.sh`）：helpers-only 绿（四个新 CallTable 助手无罪）、
   sweep 双胞胎逐件绿、invoke×2 + dispatch 逐件绿、`sync_property_vector` 单件绿但
   画面错（`mean 129.51 / lit 50.79%`，87 请求全到）⇒ 首个内容级分叉。
5. **局部类型审计器**（`D:/tmp/tyaudit.py`）扫全部 128 件已登记产物 ⇒ **14 处错类型跨 8 产物**：

```
f205  util/fill_bytes.cpp                  l5      u64  -> uint32_t
f364  script/expression_to_register.cpp   l7      f64  -> uint32_t
f401  util/shift_wide_digits.cpp           l4      u64  -> uint32_t
f708  script/add_function_constant.cpp     l10     f64  -> uint32_t
f896  script/index_table_value.cpp         l11     u64  -> uint32_t
f910  script/expression_to_operand.cpp     l8      f64  -> uint32_t
f1550 util/unordered_check_load_factor.cpp l7      f32  -> uint32_t
                                    l8      f64  -> uint32_t
f4715 util/sync_property_vector.cpp        l6,l7,l8,l9  f32 -> uint32_t
f5511 script/next_table_entry.cpp          l6      u64  -> uint32_t
                                       l7      f64  -> uint32_t
```

源写法是 `f32 var_l6 = 0, var_l7 = 0, var_l8 = 0, var_l9 = 0;` / `u64 var_l5 = 0;`，
`tx_gen2.py` 一律发成 `uint32_t` ⇒ **每个浮点值经 u32 往返被截断**。
f4715 的 14 个 use site 全是 f 型直通（`l6 = f0` / `f0 = l6` …）⇒ 只改声明行即语义等价。

6. **f364 f708 f896 f910 f1550 是批 1–3 产物**，同样四闸全绿、同样静默截断 ⇒
   潜伏缺陷。潜伏比致命更危险，因为致命件立刻在帧锚上暴露，潜伏件会一直躺着。

## 4. 两条定案

**生成器根修**：`DECLTYPES` 原来按裸名建全局索引，而 `l6` 在 5,579 个函数里复用，
`setdefault` 留下的是**文件中第一个**出现的函数 ⇒ 改成**按函数分桶** `LOCALTYPES[fnid][name]`。
改完重跑 55 件 EMIT、0 LEFTOVER，再逐件核声明行。

**审计纪律**：审计器报 0 处错 ≠ 无错。本轮 `tyaudit.py` 自己错了三次（DECL 正则按
"逗号名单后接 `=`" 写而源是逐名带初始化器 ⇒ 零匹配；产物去 `var_` 前缀而源字典键带前缀 ⇒
恒不命中；`end = defs[lo][0] - 1` 而 `lo` 已塌缩成命中下标 ⇒ end 落在 pos 之前、窗口恒空），
三次都报"假 0"。**先证明它能报出已知错，再信它的 0。**

## 5. 残留

- 静态审计轴从 6 条增到 7 条（新增逐变量局部声明类型）。数值改写类（掩码、位宽、符号扩展、
  饱和）建议按同样办法单独开轴——它们能过全部四闸，因为四闸只验 66 个导出一致性，**不验函数内部语义**。
- 批 4 B 组 20 件待写（需新命名 46 件，~17,000 源行）。
- 批 2 残留：f560/f248 跳板（产品未落）；f427 跳板（`util/check_expr_complexity.cpp` 不存在，
  tsv 行保持跳板态）。
- 批 1 保真比较器 `D:/tmp/verify_batch1.py`：v4 的 Layer B 是可信基线，v5 不是定稿
  （`arg_filter` 吞掉了源侧 callee 名，把 28/28 绿变成全红）。
