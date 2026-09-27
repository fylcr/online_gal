# P2 批 1 名表（f265 族 28 件 · 外置防身档案）

**Tree location:** the asyncify products these notes count have since moved from
`.ref/artemis-official/rewrite/src` to **`ports/artemis/src`** (tracked source, alongside
`ports/artemis/rewrite-names.tsv` and `ports/artemis/REWRITE.md`).  Paths below are the
ones that were true when each note was written — the 落位 column's `util/…`/`script/…`
values are relative to that tree, now `ports/artemis/src/`.

> 用途：批 1 全部 fnid→名→落位→行数 的唯一权威表。任何上下文截断/击杀后，从本文件续跑。
> 产物形态：R1（判定序一字不动、asyncify 相位机只注释、池址数值不改、机械层词汇走 engine_context.h）。
> 生成方式：D:/tmp/tx_gen2.py（通用转换器，meta 同文件）；生成后 code-check 逐件 + style-gate + 全链 differ:0 + wasm-diff --strict + 计数锚（双侧同跑）。
> 纪律：产品落地同刻补 tsv 行（注册行先交）+ 本表打勾；f223 落地即删其 engine_context.h 转发表。
> 行数 = artemis.wasm2c.c 定义体行数（def 行至收尾 `}`）；起始行见括号。

## 产品 28 件（call-count 降序执行）

| # | fnid | 名 | 落位（原 rewrite/src/） | 行数（起始行） | 状态 |
|---|------|----|--------------------|--------------|------|
| 1 | f265 | AppendStringText | util/append_string_text.cpp | 152 (128731) | 产品✓ tsv✓ 件检✓ |
| 2 | f5721 | ResizeRawBlock | util/resize_raw_block.cpp | 24 (2756784) | 产品✓ tsv✓ 件检✓ |
| 3 | f910 | ExpressionToOperand | script/expression_to_operand.cpp | 520 (359973) | 产品✓ tsv✓ 件检✓ |
| 4 | f708 | AddFunctionConstant | script/add_function_constant.cpp | 317 (271843) | 产品✓ tsv✓ 件检✓ |
| 5 | f364 | ExpressionToRegister | script/expression_to_register.cpp | 396 (161104) | 产品✓ tsv✓ 件检✓ |
| 6 | f645 | EncodeConstant | script/encode_constant.cpp | 119 (252887) | 产品✓ tsv✓ 件检✓ |
| 7 | f223 | ReallocateBlock | util/reallocate_block.cpp | 187 (116561) | 产品✓ tsv✓ 件检✓ |
| 8 | f476 | EmitConditionJump | script/emit_condition_jump.cpp | 341 (196722) | 产品✓ tsv✓ 件检✓ |
| 9 | f4579 | AllocateGuardedBuffer | util/allocate_guarded_buffer.cpp | 365 (2512105) | 产品✓ tsv✓ 件检✓ |
| 10 | f399 | ReallocateRawBlock | util/reallocate_raw_block.cpp | 93 (175827) | 产品✓ tsv✓ 件检✓ |
| 11 | f1116 | AppendWithLimit | script/append_with_limit.cpp | 598 (481796) | 产品✓ tsv✓ 件检✓ |
| 12 | f204 | ReleaseNodeTree | util/release_node_tree.cpp | 45 (113039) | 产品✓ tsv✓ 件检✓ |
| 13 | f207 | StringLength | util/string_length.cpp | 83 (113410) | 产品✓ tsv✓ 件检✓ |
| 14 | f414 | EmitOpInstruction | script/emit_op_instruction.cpp | 128 (179653) | 产品✓ tsv✓ 件检✓ |
| 15 | f970 | AppendTextBytes | util/append_text_bytes.cpp | 247 (387582) | 产品✓ tsv✓ 件检✓ |
| 16 | f303 | ClearFlagsRecursive | util/clear_flags_recursive.cpp | 135 (136350) | 产品✓ tsv✓ 件检✓ |
| 17 | f1233 | SetStringBytes | util/set_string_bytes.cpp | 240 (531541) | 产品✓ tsv✓ 件检✓ |
| 18 | f592 | UnorderedFindEntry | util/unordered_find_entry.cpp | 292 (232347) | 产品✓ tsv✓ 件检✓ |
| 19 | f226 | SetStringFromSource | util/set_string_from_source.cpp | 155 (116989) | 产品✓ tsv✓ 件检✓ |
| 20 | f1550 | UnorderedCheckLoadFactor | util/unordered_check_load_factor.cpp | 349 (689991) | 产品✓ tsv✓ 件检✓ |
| 21 | f2801 | PumpEventQueue | script/pump_event_queue.cpp | 519 (1199229) | 产品✓ tsv✓ 件检✓ |
| 22 | f1071 | UnorderedPrepareSlot | util/unordered_prepare_slot.cpp | 248 (459903) | 产品✓ tsv✓ 件检✓ |
| 23 | f3784 | UnorderedInsert | util/unordered_insert.cpp | 308 (2135876) | 产品✓ tsv✓ 件检✓ |
| 24 | f902 | InclineNumber | script/incline_number.cpp | 278 (355751) | 产品✓ tsv✓ 件检✓ |
| 25 | f221 | PrepareStringBuffer | util/prepare_string_buffer.cpp | 155 (116393) | 产品✓ tsv✓ 件检✓ |
| 26 | f896 | IndexTableValue | script/index_table_value.cpp | 457 (347488) | 产品✓ tsv✓ 件检✓ |
| 27 | f220 | SkipWhitespace | util/skip_whitespace.cpp | 90 (116302) | 产品✓ tsv✓ 件检✓ |
| 28 | f209 | SetStackTop | script/set_stack_top.cpp | 121 (113626) | 产品✓ tsv✓ 件检✓ |

## 新增转发表（engine_context.h Entry-point forwarders 块 · 查重已过）

| 符号 | 名 | 签名 |
|------|----|------|
| Oc_0 | DeallocateRawBlock | void(ctx, block) |
| f2207 | ReallocateRawBlockCore | uint32_t(ctx, header, size) |
| f518 | StoreListEntry | void(ctx, list, index, value) |
| f523 | CopyStringBytes | void(ctx, text, bytes, length) |
| f707 | AppendFunctionCode | void(ctx, func_state, code, line) |
| f218 | RaiseAllocLimitError | void(ctx, state) |
| f253 | RaiseAllocatorAssert | void(ctx, state) |
| f635 | UnorderedRehash | void(ctx, map, buckets) |
| f4180 | UnorderedUpdateLoadFactor | void(ctx, map, count) |
| f329 | PollEventKind | uint32_t(ctx) |
| f3544 | RunEventStep | void(ctx, state) |
| f1042 | BacklogView::DispatchAux | void(ctx, p0, p1) · 嵌套命名空间 |
| f3551 | AutoReadTimer::Dispatch | void(ctx, p0, p1) · 嵌套命名空间 |

- f223 产品落地即删其既有转发表（既定纪律）。
- 在册签名逐字对齐（前向声明用）：CompareBytes/AllocateStringBuffer/CopyBytes/FillBytes/InternString/GetTableFieldByString/ResolveTableField/EmitInstruction/DischargeExpression/CopyIfNonEmpty/GrowStringAndReplace/ResolveTableField/GrowArray/RaiseSyntaxError/RaiseTypeError/RaiseScriptError/RaiseBlockTooBig/RaiseStringLengthError/DeallocateStringBuffer/GrowValueStack/FetchMetamethod/FetchObjectMetamethod/CallValue/AllocateRawBlock(Nc_0)/Panic。
- 通用已知名：NextCharSlow/SkipSep/InclineNumber(本批)/ReadLongString/LexError/FormatPushString/ThrowScriptError/GrowTableSlot/ReadNumeral/FormatSourceId（f1727 批已入 engine_context.h）。

## 批末闸门（全部绿才交批）

1. code-check 逐件（preamble+产品 → em++ -fsyntax-only）
2. style-gate（禁 var_/w2c_/FUNC_/goto var_B/i32_load/_default32/UNREACHABLE/TRAP(/wasm_rt_；裸池址 WARN 按房规 hex 命名常量）
3. 全链 `bash ports/artemis/reverse/code-assemble.sh asyncify` → differ: 0
4. `node ports/artemis/reverse/wasm-diff.mjs --strict .ref/artemis/WebAssembly/asyncify/artemis.wasm ports/artemis/reverse/out/code-asyncify/cxx-route.wasm` → 66/66、trap-kind-only 0、exit 0
5. 计数锚：钉死簇 f428/f393/f244/f1727/f340/f805/f778 + 批内 fn 双侧同跑逐位稳定（工具 ports/artemis/reverse/ 与 ports/artemis/tools/ 既有件；帧锚双侧像素级一致为准，§136 环境漂移豁免）

## 进度记录

- 名表建立。产品 28/28 落盘（D:/tmp/tx_gen2.py 生成）、tsv 28 行登记、f223 旧转发表已删、engine_context.h 增 11 转发表+2 嵌套+9 词汇包装（PROVISIONAL 注明）。
- 闸门首跑：① code-check 28/28 过 ✓ ② style-gate PASS ✓ ③ 全链红（em++ 21 errors，-ferror-limit=20 截断）。根因两类：(a) 拼接单 TU 内跨产品同名匿名 namespace 常量重定义 16 条（kSaveFrameBytes×5、kResumeWordBytes×7、kConstFFFFFFFF×3、kConstFFFFFFC0×1）；(b) 9 件 src=void 但产品写 uint32_t（f910 f896 f2801 f209 f4579 f303 f204 f1550 f3784），生成层 void 原型先声明 → "functions that differ only in their return type cannot be overloaded"。④⑤ 阻塞于③。
- lead 修复（D:/tmp/fix_batch1_v2.py）：回滚 9 文件后幂等重施常量前缀（167 处 / 25 文件，kSaveFrameBytes→kF708SaveFrameBytes 形态，值不动）；从生成层 5,609 条 `static? type w2c_artemis_fNNN(` 原型反查 53 件产品定义返回类型，mismatch 恰 9 件，全部**名锚定**改为 void，repaired 9 definitions 无附带损伤。
- 教训（务必固化）：改返回类型必须锚定本产品函数名 `^uint32_t\s+Name\s*\(\s*EngineContext\*`，不能用「文件内首个匹配」——首轮 count=1 误改了 5 个邻居产品的前置声明（EmitInstruction/ResolveTableField/PollEventKind/AllocateStringBuffer ×2），制造 5 对新的 return-type 冲突。生成层静态前置声明与真定义返回类型必须一致。
- 闸门终跑：① code-check 28/28 ✓ ② style-gate PASS（STYLE=0；warning 仍是 allocate_string_buffer.cpp:37,45,46,47 裸池址）③ code-assemble ASM_EXIT=0：63 fragments / 5,562 defs kept / **rw-splice: 53 product(s) spliced** / agreement 66/66、trap-kind-only 0、differ 0 ④ **STRICT EXIT=0**（19 returned equal values + 47 threw the same trap）⑤ 深锚全绿：
  - 帧锚：candidate `finalHash=1504b2dc` ≡ 官方 `hash=1504b2dc`（像素级一致，canvas 2260x1365，finalMax 255，candidateRendered=True，pass=True）。rafTicks 官方 2062 / 候选 2528 属采样窗漂移。
  - 计数锚 worklist-r11.md：5579 instrumented 中 2,103 entered (37.7%)。**钉死簇 7/7 逐位同**（calls 与 first-run order 全等）f428 294.7K/1104、f393 279.0K/976、f244 218.4K/1085、f1727 218.4K/1086、f340 125.2K/1105、f805 117.6K/608、f778 108.9K/1088。
  - f206≡f5508 仍成立：r11 138.4K/824 ≡ 138.4K/825（两两相等，first-run 差 1）。绝对值 127.8→138.4 属 session-era 采样漂移，不影响等价判定。
  - f201 424.3K→424.4K/419（first-run 不变）；f4551 41.3K→65.1K/1167（批 1 兄弟件改走 artc 前向器，量级变化可解释，first-run 仍 1167）。
  - 批内 28 件全部有计数、无一件为 0：f265 83.6K/477、f5721 83.5K/746、f910 76.0K/1110、f708 74.4K/1092、f364 69.6K/1114、f645 66.6K/1091、f223 64.2K/751、f476 62.1K/1103、f399 60.2K/747、f4579 59.3K/904、f1116 50.6K/1090、f204 49.4K/573、f207 37.5K/448、f414 36.0K/1107、f970 34.3K/463、f303 31.1K/804、f1233 31.1K/451、f592 30.5K/691、f226 30.5K/450、f1550 30.2K/692、f2801 29.9K/891、f1071 29.3K/955、f3784 29.3K/954、f209 29.7K/768、f902 28.6K/1087、f896 28.5K/770、f221 27.8K/1645、f220 27.6K/593。
- 剖析构建坑：`ARTC_PROFILE=1 bash ports/artemis/reverse/build-bridge-route.sh asyncify out/bridge-asyncify-profile`（相对路径）报 `OSError: [Errno 22] Invalid argument: 'out/.../artemis.cpp'` / `FAIL: native-trap patch failed`；**必须传绝对路径** `... asyncify ports/artemis/reverse/out/bridge-asyncify-profile`。
- real-game-test.mjs 不接受位置参数：`--game .test/artemis-official-async/game-chs --engine <wasm> --port 4710 --hashlist --json-out <json>`（用法见 ports/artemis/reverse/README.md:307-311）。控制台 404 `[error] Download file pc/ja/caution failed.` 属良性。
- **批 1 收官：28 件产品 + 28 行 tsv + 11 转发表全部合入，53 件在册产品全链绿。** 遗留：比对器 `D:/tmp/verify_batch1.py` v5 未定稿（Layer B 在 v4 已 28/28 绿，v5 重写 arg_filter 时把源侧 callee 名吞掉导致全红，教训=改归一化器必须保住已验证绿的那一层）；批 2（f1119 f346 f1125 f4082 f707）与批 3 巨件 f3785（3,229 行）未启动。
