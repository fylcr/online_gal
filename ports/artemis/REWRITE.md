# REWRITE.md — 可读 C++ 重写宪法（风格版）

> 本树产物必须长得像 artemis-compat（.ref/artemis-compat，GPL-3.0）的 house style，
> 而不是 wasm2c 逐句直译。语义仍以 wasm2c 为保真基线（判定顺序一字不动），
> 词汇、结构、文件落位、注释文风全部对齐 artemis-compat 的真实代码惯例。
> 本文是重写阶段的唯一规范；转写档案规范见 `.ref/artemis-official/code/TRANSCRIBE.md`（历史产物不再动）。

## 0. 先生已拍板的四个决定

1. **起点**：P0（本文 + rewrite-names.tsv + style-gate.sh + 剖析工单）→ P1 热核 18 → P2 工单头部 → P3 巨函数 → P4 长尾。
2. **注释语言**：**英文**（贴 artemis-compat 惯例）。叙事口吻抄它的：`// filename.cpp — purpose.` 开头的设计段，解释 why 的散文块，不写废话注释。
3. **长尾 3,474 个未触达函数**：进 `generated/` 隔离（生成器所有、盖 generated 戳、风格闸豁免）；工单触及时逐个升格 R0/R1。
4. **trap 层收尾**（46 trap-kind-only → --strict 归 0）：并入 P1 作旁支小任务。

## 1. 不变项（每批合入的锚点）

- 行为锚：真实游戏稳定帧**双侧像素级一致**（real-game-test.mjs --hashlist，candidate.finalHash ≡ official.hash）。
  历史记录的绝对哈希（`bef44818`、`550f7464`、`1504b2dc` 等）**随窗口尺寸漂移**，只作同 session 内的辅助，不作判据（§7.3）。
- 调用锚：剖析计数逐位相同（重写不改变调用行为）
- 模块锚：wasm-diff 66/66 不回退；`npm test` 416 绿
- 保真锚：R0/R1 产物反映射后 α-IDENTICAL（fidelity-alpha-diff.py）

## 2. 风格宪法（grep 可断言，style-gate.sh 强制）

样本实测自 artemis-compat 源码，逐条照办：

1. `namespace artc { ... }`；类 PascalCase：`IetRunner LuaEngine PackManager Compositor GlyphAtlas AutoReadTimer Pf8Reader`。
2. 方法 PascalCase；tag handler 统一签名 **`bool TagXxx(const std::string &tag, TagAttrs &m)`**（AGENT.md §3）；
   返回 false = 门控未满足的 fall-through，**不是错误**，注释里不得写成失败。
3. 成员 snake_case + 尾下划线：`packs_ lua_ stopped_ seen_tags_ compositor_`；局部 snake_case。
4. 类型用标准面目：`uint32_t std::string std::vector`（机械层的 u32/u64 进 R1 时换）；头文件 `#pragma once`。
5. 目录镜像 artemis-compat：`src/{pack,script,render,audio,save,util,config,core,input,log,host}`；
   依赖三禁令同 AGENT.md §2（render 不 include script；script/*.h 不 include render；core/ 仅 host/jni 可 include）。
6. 共享类型下沉同 AGENT.md §2.4：文本 → `util/text.h`；输入 → `input/input_state.h`；存档 → `save/`。
7. **asyncify 词汇**：`w2c_g3` 称 **resume phase**（0 running / 1 suspended / 2 rewinding），
   `w2c_g4` 称 **event return-frame sp**——照它 README「可重入 runner + 事件返回帧 + 脚本栈」的原生叙事；
   R2 可把三态提成 `enum class ResumeState`（runtime_state.h 的 LoadPhase 是现成范式）。
8. 错误文案保留日文原文（「ファイルが見つかりません。」等），那是行为的一部分。

## 3. 露馅点清除表（style-gate.sh 的禁词根）

| 逆向指纹 | 换成 |
|---|---|
| `var_i0 / var_l3 / var_p0 / var_B12` | 语义 snake_case 局部名；goto 标签 → 结构化控制流 |
| `instance->w2c_g3 / w2c_g4` | `resume_phase_ / return_frame_sp_`（见 §2.7） |
| `w2c_artemis_fNNN / w2c_a_b…` | `LuaEngine::TagXxx / HostApi::…`（rewrite-names.tsv 全表） |
| `FUNC_PROLOGUE / FUNC_EPILOGUE / UNREACHABLE / TRAP(x)` | EnterFrame/LeaveFrame 级真实调用或结构化消除 |
| `i32_load_default32 / i32_store8…` | 真实访问器（memory_base 上的具名 load/store helper） |
| 裸池址 `718223u / 852316u` | 命名常量 + 真实字面量（`"print"` / `kPendingThrowAddr`） |
| 中文行内注释 | 英文设计注释（§2.2 口吻） |

## 4. 词汇映射

全表在 **rewrite-names.tsv**（kind=fn 314 行现状，其中 **157 行已落产品文件** + kind=rt 11 行运行时面 + kind=host 4 行宿主导入族）。
原则：

- 能对上 artemis-compat 现成类/文件的**直接用它的**：f3744→`LuaEngine::TagDispatch`（照 l_tag 双阶段
  RawTagTable+TagTable 骨架）、f3650/51→`TagGenericDelOn/TagGenericSetOn`（AGENT.md 点名模式）、
  f3551→`AutoReadTimer`、f3548 rp=翻页（AGENT.md 点名）、reset 家族→`LoadPhase::ResetEphemeral` 语义族。
- 对不上的按它起名法新造（类归属 + PascalCase 动词），note 列标「provisional」，R1 时按真语义定终名。
- 已知撞名 8 例（f3688 真身 scetween 等）：**逐句语义为准**，tag 认领只是线索。
- 严禁伪造出处声明（不写 "Adapted from artemis-compat" 类字样）；若未来以 fork 名义发布，
  GPL-3.0 的署名与开源义务由先生届时定夺。降险本质 = 产物是**重表达的原创代码**，非伪代码直译。

## 5. 工序三档

- **R0 气味清洗**：机械改名（var_*→语义名、池址→常量、宏→真调用），语句序不动。长尾用。
- **R1 纯搬运风格化**：R0 + 类型/签名/文件落位 + 英文设计注释。**判定顺序一字不动**——
  这正是 artemis-compat AGENT.md 的「纯搬运」纪律，两头同源。
- **R2 结构化改写**：R1 + goto 汤→控制流、asyncify→事件返回帧/ResumeState、提取函数与类。热核与分发器用。

单函数子代理流程：读四源（wasm2c / wasm-decompile / artemis-compat 对应模块 / tag 文档+strings.tsv）
→ 对齐 rewrite-names.tsv 定名 → 按档产出 → 自检（style-gate + code-check + α 或行为差分）→ 父代理合入验收。

## 6. 验收闸（每批合入）

```bash
bash ports/artemis/reverse/style-gate.sh              # 风格闸（本文 §2/§3，默认扫 ports/artemis/src）
node ports/artemis/tools/real-game-test.mjs --game .test/artemis-official-async/game-chs \
  --engine <构建产物> --hashlist                                       # 帧锚 == 同 run 的 official.hash
node ports/artemis/reverse/wasm-diff.mjs <official> <构建产物>           # 66/66 不回退
bash ports/artemis/reverse/fidelity-alpha-diff.py <产物> <wasm2c> <fnid> # R0/R1 保真
```

跑闸链务必 `set -o pipefail`：`code-assemble.sh | tail` 的管道会吞非零退出，构建 FAIL 后
strict 差分对着陈旧产物假绿（实测教训）。style-gate 的裸池址告警按 §3 改写为命名常量或等价折叠算术。

**词汇边界扩展（asyncify 家族，2026-09-25 定案）**：util/engine_context.h 提供
`ResumePhase(ctx)`／`ReturnFrameSp(ctx)`（返回 uint32_t& 引用、可直接读写赋值）、常量
`kResumeRunning/kResumeSuspended/kResumeRewinding`（0/1/2）、`TrapUnreachable()`。定论：
访问器方案；`#define resume_phase_ w2c_g3` 宏映射否决（污染整个 src/）。UNREACHABLE 真实
定义 = `#define UNREACHABLE TRAP(UNREACHABLE)` = `(wasm_rt_trap(WASM_RT_TRAP_UNREACHABLE), 0)`
（artemis.wasm2c.c:93/:107）——TrapUnreachable 只转写宏本体，trap 文案与生成层逐字同。
tsv rt 行目标拼写已对齐 `ResumePhase()` / `ReturnFrameSp()`。

**跨产品调用与 code-check 单文件模式**：code-check 不注入别家产品原型，跨产品调用假 FAIL
（实测 push_table_field.cpp→LookupValue、copy_if_nonempty.cpp→CopyBytes，产品本身无错）。
政策：产品可在 namespace artc 顶部带前向声明——已注册 callee 签名与 tsv 注册目标完全一致，
未注册 callee 用 PROVISIONAL 名，注释 `// Forward declaration — defined in <相对路径>.`；
禁止修改依赖产品的文件。

**未重写 callee 的调用跳板（2026-09-25 起）**：前向声明只过语法闸、链接必挂——产品调用
尚未重写的函数一律经 util/engine_context.h 的 thin call-through（词汇边界内拼
w2c_artemis_fNNN，产品照常只写 artc 名 + 同型前向声明）；tsv 记 `fn fNNN — artc::Name — —`
行标 PROVISIONAL，对应产品落地时删跳板并集中对名。另有一段 PROVISIONAL **嵌套命名空间**跳板
（engine_context.h `// PROVISIONAL nested-namespace forwarders`，2026-10-01 记录）：f1042
`BacklogView::DispatchAux` 与 f904 `LuaEngine::Call` 已随批 5 落地删除，现只剩 f3551
`AutoReadTimer::Dispatch`；**哪个产品的定义进了树，就必须同步删掉它在这一段的 inline 跳板**，
否则 em++ 直接 `error: redefinition`（跳板与产品同名同签名，且产品落地后 `w2c_artemis_fNNN` 已被
rw-splice 改名，跳板也再无被调用的可能）。在册：f603 RawValueEqual（2×u32）/
f643 FinishTableIndex（3×u32）/ f805 GrowStringAndReplace（void, 8×u32）/ f1727
RefillRecordState（2×u32）/ f313 RaiseScriptError（void, 3×u32；勿与 f199 artc::Panic
混淆）。f898 定名 GetTableFieldByNumber（与 GetTableFieldByString 成对；f326 曾以
FindOrCreateValue 前向调用，已中央对名）。f777 侧新增五条：f517 FetchMetamethod（u32,
3×u32）/ f333 FetchObjectMetamethod（u32, 3×u32）/ f289 GrowValueStack（void, 2×u32）/
f372 CallValue（void, 3×u32）/ f907 RaiseTypeError（void, 3×u32）。
f805 侧新增三条：f201 AllocateStringBuffer（u32, 1×u32）/ f560 DeallocateStringBuffer（void,
3×u32）/ f248 RaiseStringLengthError（void, 1×u32）。f201 语义已确证并落地 R1 产品
（util/allocate_string_buffer.cpp，跳板已删）：可挂起分配器（libc 簇 f200/f202 邻座），判断序
=rewind 两段弹帧（先 8B {bytes,slot} 后 4B resume word，不合并）→ 归一 `bytes = g3 ? bytes :
(bytes ? bytes : 1)`（仅 running 0→1）→ 重试环 raw alloc（w2c_artemis_Nc_0）成功即返；失败取
GC 步进索引槽 856940u 作 t6 间接调（CallCollectorStep，仅 module_instance 参，可挂起）→ 调后
resume word 归 0 再查 suspended（var_B1 挂起点压 4B word + 8B 帧，返 0）→ running 重试；OOM 路
（running）w2c_a_u 异常对象双 typeinfo 词（798992u/798956u 同偏移覆盖写）→ w2c_a_w 抛
bad_alloc（799044u/2270u）。早期「asyncify 事件泵」标签系对其恢复序言+GC 回调的误读。g0 访问器统一名 `GuestStackSp`（可挂起体必须保 g0 帧流量
——实参块经帧传址且驱动 unwind/rewind；纯叶净零帧方可局部数组替代）。

**删跳板的范围与做法（2026-10-01 批 7 实测）**：engine_context.h 里转发到 `w2c_artemis_fNNN` 的
inline 跳板共四种形态——入口跳板清单、批 4B 闭包段、PROVISIONAL 嵌套命名空间段、以及缩进两格的
**单行**跳板清单。产品落地时按 **fnid** 反查、逐块删（不是按名字）：取调用行 → 向上找 `inline `
签名行（单行形态签名与闭括号同行，要单独判）→ 向下找第一行 `}`；块首再把连续 `//` 注释吸收进来，
遇段标题（含 forwarders / PROVISIONAL / Batch-）即停。批 7 一次落地 86 个产品 ⇒ 删 86 块 / 155 行。
两处坑：① 只按名字、或只按「最近一条 inline 行」删，会把段标题连同 `namespace X {` 一起吃掉，留下
孤儿 `}` 把 `namespace artc` 提前闭合，报的是 `error: unknown type name 'EngineContext'`（真因是
括号失衡，不是类型没声明）；删完必须核 `{` 与 `}` 计数相等。② `tx-gen2.py` 的 METAS `purpose` 必须
是 `"<产品名> -- 一句话."` 形态——`emit()` 按 `' -- '` 切给 banner，缺前缀直接
`IndexError: list index out of range`。
批 3（p 代）产品侧新增八条：f250 RaiseBlockTooBig（u32, 1×u32）/ f223 ReallocateBlock（u32,
4×u32，块分配器）/ f1114 ResizeStringTable（void, 2×u32，字符串表 rehash）/ f243
RaiseSyntaxError（void, 2×u32，解析器报错）/ f704 GrowArray（u32, 6×u32，解析器数组增长器）/
f205 FillBytes（u32, 3×u32）/ f1120 BucketLog2（u32, 1×u32）/ f1112 ResizeTable（void,
4×u32）。批 3 落地四品：f428 DischargeExpression、f247 InternString、f340 EmitInstruction、
f643 FinishTableIndex（名字早定，产品落位）；f643 自调×2 递归可挂起。callee 对号经源调用序↔
产品调用序逐位对齐钉死（九位吻合），f643 前四位 f205/f1120×2/f1112 由数字包容正则补齐。
f4551 产品（artc::ReleaseContainerElements，容器析构，11 挂起点）落地：新增五条跳板——
Panic（f199，util/panic.cpp 落地后删跳板）/ HostFailAssertion（w2c_a_e 断言桥 5×u32）/
FindMatchingCatch（w2c_a_g）/ HostTempRet0（w2c_a_b）/ CallTableFunction（w2c_Lc t1 表分派，
CALL_INDIRECT 原生宏薄包保 trap 文案）。产品内 kPendingThrowAddr（=852316）等 6 常量为匿名
namespace 私有副本；util/panic.cpp 落地时收敛为共享定义并删私有副本（防 kIndex* 式撞名复发）。

**剖析计数锚按「会话时代」对比，不跨时代比**：浏览器冷缓存时代 404 重试风暴驱动脚本簇
（f203/f206/f210/f5508/f4832 等）计数高 ~35%，暖缓存时代回落；生成代码未改一字计数也整体
平移（r1/r2 ≈171K 时代 vs r3-r6 ≈128K 时代，同代码态双向复现过）。可靠不变量 = 钉死簇逐位
稳定（428/393/244/1727/340/805/778）+ f206≡f5508 结构等值 + 帧锚像素级一致；改写嫌疑用
「同代码态新会话复测」裁决（r5/r6 双探针实测全部产品无罪）。

## 7. 构建集成架构

**装配时全域改名**：code-assemble.sh 现有的 rename 层（已做 fNNN→名）扩展为全词汇表
（w2c_* / g3 / g4 / wasm_rt_* / 裸池址 → artc 词汇），生成层与手写层共用同一张表；
手写重写产物直接按 artc 词汇书写、装配时零改名落位。
产物结构 = `ports/artemis/src/**`（手写，风格闸管辖）+ `ports/artemis/generated/**`（生成器所有，盖戳豁免）
→ 装配 → build-bridge-route.sh（宿主 ABI 原样导入：import_module "a" + imported memory a.a + 表导出 Lc）。

## 7.1 帧与计数架构（装配集成定案）

产品体自带帧：入口 `EnterFrame();`（2 空格缩进首句）、每处 return 前 `LeaveFrame();`
（词汇边界 util/engine_context.h 映射到 FUNC_PROLOGUE/FUNC_EPILOGUE）；桥接 thunk 纯转发不带帧；
产品互调直接用 artc 名（rw-splice 在首个产品前注入 `namespace artc` 原型块保证跨序可见性）。
instrument-profile.py 计数锚 = 产品体 `  EnterFrame();`（索引取自 rw-splice 的 `// ---- rewrite: fNNN` 标记），
生成函数锚 `  FUNC_PROLOGUE;` 不变——两路合计与原版逐位相同，产品互调也计入。

## 7.2 单 TU 拼接的跨产品纪律（P2 批 1 实测）

所有产品被 splice 进**同一个 TU**，各文件匿名 namespace 合并 ⇒ 跨产品同名 `constexpr` 直接
`redefinition of 'kXxx'`；且返回类型不同的同名函数在 C++ **不能靠返回类型重载**。

- **常量名必须全局唯一**：生成器按 fnid 加前缀（`kSaveFrameBytes` → `kF708SaveFrameBytes`，
  167 处 / 25 件）。值不动。语义冲突实例：`kSaveFrameBytes` 在库内有 9 种不同值（8u…44u）、
  `kMsgBuckets` 三件两值（0x8AE58u / 0x8FAFCu）——绝不可"合并成一份"。
- **返回类型必须与生成层一致**：以 `.ref/artemis-official/wasm/asyncify/artemis.wasm2c.c` 的
  `static? <type> w2c_artemis_fNNN(` 原型为准（5,609 条可解析）。src=void 的 9 件产品曾写
  `uint32_t`，生成层 void 原型先声明 → `functions that differ only in their return type
  cannot be overloaded`。
- **改返回类型必须名锚定本产品函数名**：`^uint32_t\s+<Name>\s*\(\s*EngineContext\*`。禁止
  "文件内首个匹配"——会误改邻居产品的前置声明（批 1 首轮 5 例：EmitInstruction /
  ResolveTableField / PollEventKind / AllocateStringBuffer ×2）。
- 审计脚本口径：以 tsv 为权威 file→fnid 映射，取文件内**最后一个带 `{` 的定义**；前置声明不是定义。

## 7.3 剖析/帧锚运行要点（工具坑）

- `ARTC_PROFILE=1 bash ports/artemis/reverse/build-bridge-route.sh asyncify <OUTDIR>` 的 OUTDIR
  **必须绝对路径**；传相对路径报 `OSError: [Errno 22] Invalid argument` / `FAIL: native-trap patch failed`。
- `node ports/artemis/tools/real-game-test.mjs` **不接位置参数**：
  `--game .test/artemis-official-async/game-chs --engine <wasm> --port 4710 --hashlist --json-out <json>`。
- 帧锚判定 = **双侧像素级一致**（candidate.finalHash ≡ official.hash），不是死记绝对值；
  rafTicks 不同属采样窗漂移；控制台 404 `[error] Download file pc/ja/caution failed.` 良性。
- 计数锚比较器：解析 worklist 的 `| # | index | calls | first-run order |` 行为主键；钉死簇逐位同
  （calls + first-run order 双同）才算过。
- **统计计数前必须按 settle 筛 run**：bench 双侧都会 flake —— 实测 6 跑里 2 跑 official
  未渲染（`FAIL: the official build did not render on this bench`）、2 跑 candidate stalled
  （`frames 1 settled stalled`）。**只有「双侧都 rendered 且 settled」的 run 才带可用计数** —— 判据
  是请求数 + lit% + frames（§1、§7.3），不是 hash：`550f7464` 只是当时那个窗口尺寸下的值，
  窗口一变就漂（实测缩到 1920x945 后变 `a2e1dcf5`）。拿死 hash 过滤会把好 run 也筛掉。
- 🔴 **profile JSON 必须和它实测的那台 `bridge.wasm` 同目录、同批次产出**。单看 mtime 相邻
  会认错父。判据：JSON 里出现的批内产品计数，必须在同目录 `artemis.cpp` 里数得出对应数量的
  `^// ---- rewrite: fNNN ` 标记；数不上就是孤儿产物。
  批 3 教训：`D:/tmp/batch2-frame-anchor.json`（11:53）内含批 2 产品的值，而它旁边 mtime
  10:45 的树只有 53 个标记（批 1 态）→ 拿它当基线，f805/f3785 报出 155× 与 29,226→2 的假红。

## 7.4 f3785 转写实录（`util/dispatch_resource_step.cpp`）

- **是什么**：28 臂资源/缓存步进分发器。`g4` 回溯帧里存 56 字节帧（14 × u32 槽），另存一个
  4 字节 resume word；重入时按 resume word 与 1u..28u 逐一比对选臂，每臂在 `g0` 上分配
  112 字节 scratch frame、重建一个 key、调一个 host 槽，末了抽干 asyncify 挂起槽 852316。
  非自递归：再入靠挂起/恢复，不靠自调用。
- **前置侦察的坑（务必复述）**：brief 说"0 个 `var_L*` 标签"是**错的**——本函数有 6 个
  （L50/149/229/239/244/249），生成器把它们写成 `var_L50: `（**尾随空格、无分号**），
  所以任何 `var_B*:;` 形态的 grep 都会漏掉它们。另 brief 漏了 `w2c_artemis_Oc_0`
  这一个被调方（本函数 37 处 → `DeallocateRawBlock`）。教训：标签数对不上时，grep 的
  **终止符**要放宽到 `:;` 和 `:` 两种再数一遍。
- **Pool 常量以十进制原样出现**：`4294967295u`×38、`4294967280u`×14（=0xFFFFFFE0，**不是**
  0xFFFFFFF0）、`2147483648u`×7。名字化的正则要用十进制字面量匹配；若常数定义写十六进制而
  匹配用十进制（或反过来），那三个常量会静默留在函数体里不被命名。
- **`SyncMemoryBase` 必须带 `#if WASM_RT_USE_SEGUE_FOR_THIS_MODULE` 守卫**：
  `wasm_rt_segue_write_base` 在 `artemis.wasm2c.c:71` 是 `static inline`，产品里**无声明可链接**；
  且守卫必须与 batch-mates（`util/reallocate_block.cpp`、`script/pump_event_queue.cpp`）一致，
  否则 link 报 `undefined symbol: wasm_rt_segue_write_base`。
- **五个 host import 此前无 wrap**：a_d/a_f/a_h/a_s 是**表派发族**（全库调用数 3797/1363/1297/109），
  a_fa 才是 async host fetch `_emscripten_wget_data(url, pbuffer, pnum, perror)`（仅 3 个调用方）。
  wrap 加在 `engine_context.h`（唯一豁免文件），名字一律 `Host*` + 槽位语义，`w2c_a_*` 只允许出现在那里。
- **多集比较器要接受两种标签终止符**：源侧 `^\s*\w+:\s*;?\s*$` 才数得出 66（本函数
  goto 132 / 标签 66 / if 275 / return 1，源与产品全等）。只数 `:;` 会得 60，凭空造出 6 个差异。
- **越权改动裁决口径**：子代理顺手改了 `engine_context.h` 里别的条目（批 3 改了
  `HostAssertFail` 返回类型 void→uint32_t）——判据是**以生成层 `artemis.wasm2c.h` 为准**
  + 三个调用点是否真的消费返回值，都不消费就改回（头文件不许说谎）。
  同一次改动里另一处（`SyncMemoryBase` 守卫化）是**必要修复**，保留。

## 7.5 剖析计数丢失的排查法（P2 批 3 实录，5,000+ token 的教训）

症状：批 3 四闸全绿，但深锚计数锚报 f805 117.6K→760、f3785 29,226→2。

1. **先分层，别先怀疑代码**。按 tsv 产品集把两次 profile 拆成产品层 / 生成层：
   批 3 是 -26.71% vs -0.67% → **问题在产品层，不在生成层**。
2. **验证测量机制本身健全**：b3 树里 `EnterFrame();` 锚点戳 59、`FUNC_PROLOGUE;` 锚点戳 5,520，
   无第三类；`EnterFrame()` 就是 `inline void EnterFrame() { FUNC_PROLOGUE; }`（`engine_context.h:43`），
   两路对称 → 计数器没坏。**零产品增加、41 掉、45 不变**这个形状本身就是「换了台引擎」。
3. **重建正确基线再比**：从 `D:/tmp/src-backup-batch2` 还原树、删 tsv 的 f3785 行、
   重跑 `build-bridge-route.sh asyncify --profile "D:/tmp/pre-b3-profile"` → 58 products。
   pre/post 两边的 settle 运行**逐位一致**（钉死簇 7/7、f805 同为 760、f3785 同为 2）。
4. **认错父的判据**：profile JSON 里的批内产品计数，必须在同目录 `artemis.cpp` 里数得出
   对应数量的 `^// ---- rewrite: fNNN ` 标记。`D:/tmp/batch2-frame-anchor.json` 内含批 2 产品的
   f707=73,982 等值，而旁边 10:45 的树只有 53 个标记 → 那是**一个已被覆盖的更晚构建**的产物。
5. **分布指纹确认**：30 最热 fnid 的 ratio min=0.23 / **median=1.00** / max=1.00
   → 纯尾部塌陷，不是全局 session 漂移。全局漂移的指纹是**全体同向小幅偏移**。
6. **备份必须早于派活，派活后要 diff 验证**：批 3 的 `src-backup-batch3` 报 "backup ok: 61 files"，
   但 `diff -u` 无输出 —— 备份是子代理改完之后的快照。**diff 无输出才是危险信号，不是"干净"。**

## 7.6 局部变量类型截断（批 4 黑屏根因，5 件潜伏 + 1 件致命）

症状：批 4 四闸全绿、`wasm-diff --strict` 66/66 differ 0，**真游戏却黑屏**——候选侧
`mean 0 / lit 0% / frames 1 / 83 files`（好签名是 87 files），且 3 连跑逐字相同 ⇒ 硬回归非 flake。
受控 A/B：批 3 树 3/3 exit 0 双侧同签名，批 4 树 0/3。**批 4 相对批 3 的变更面只有 22 新件 +
8 处纯 head 注释 + `engine_context.h` 删 3 跳板 / 增 4 个 CallTable 助手**。

1. **根因**：`tx_gen2.py` 的局部声明发射器把所有 `var_l*` 一律发成 `uint32_t`，忽略源声明块的
   逐变量类型。源写法是 `f32 var_l6 = 0, var_l7 = 0, var_l8 = 0, var_l9 = 0;` 与 `u64 var_l5 = 0;`
   ⇒ 浮点局部被截断成整数位，**值经 u32 往返后全错**。f4715 四个 use site 全是 f 型直通
   （`l6 = f0` / `f0 = l6` …），所以只改声明行即语义等价，14 处一处 use site 都不用动。
2. **致命件 vs 潜伏件**：f4715（SyncPropertyVector）直接进渲染路径 ⇒ 画面错但 87 请求全到
   （`mean 129.51 / lit 50.79%`）；另 5 件（f205 f364 f401 f708 f896 f910 f1550 f5511）当时未在
   渲染路径上触发，**同样四闸全绿、同样静默截断** ⇒ 潜伏缺陷比致命缺陷更危险。
3. **静态审计轴全绿也不代表保真**：本轮 span 边界、sp 字面量、t 派发参数序、frame 常量、
   `LeaveFrame` 覆盖率、形参签名 6 条轴**逐件核过全绿**，唯一漏的正是**局部变量类型**。
   新增第 7 条轴：**逐变量局部声明类型**。
4. **帧锚的判据是请求清单，不只是画面**：83 vs 87 files 的差集（缺 `system/*.asb`、
   `sound/sysse/none.ogg`、`image/bg/white`、`pc/ja/caution`）直接指到"走了不同标题分支"，
   比逐帧像素更快定位。取清单要在 bench 里临时打一行 `performance.getEntriesByType('resource')`，
   **用完必须还原**（本轮已 `diff -q` 确认与备份逐字相同）。
5. **审计脚本自身错了三次才首次报真值**，且三次都报"假 0"：`DECL` 正则按"逗号名单后接 `=`"写
   而源是逐名带初始化器（零匹配）；产物去 `var_` 前缀而源字典键带前缀（恒不命中）；
   **`end = defs[lo][0] - 1` 而 `lo` 已塌缩成命中下标 ⇒ end 落在 pos 之前，窗口恒空**。
   教训：**审计器报 0 处错 ≠ 无错**，先证明它能报出已知错（对着 14 处真值验），再信它的 0。
6. **手工修完必须重跑生成器**：9 处手改被下一次 `tx_gen2.py` 全量覆盖回 `uint32_t`，
   因为 `DECLTYPES` 按裸名建索引而 `l6` 在 5,579 个函数里复用，`setdefault` 留下的是
   **文件中第一个**出现的函数。定案：**局部类型表按函数分桶**（`LOCALTYPES[fnid][name]`）。

## 8. 已知风险与禁区

- **asyncify 状态机是唯一禁区**：g3/g4、852316 挂起槽、挂起/恢复语义 R1 只许注释不许动；R2 动它必须双锚（帧哈希+剖析计数）全绿。
- f3219 式 5 行错译教训：α 闸是 R0/R1 的防线；R2 靠双锚。
- 池位数坑：6 位 u 是常态（7 位是我此前口误）；strings.tsv 未收的直接解析 data 段镜像核字节。
- 生成器产物不许手改（改生成器重跑）；委托 ≤8 并发、单函数任务、2 次不成即自做。
- **同类静默缺陷**：类型截断（`uint32_t` 收 `f32`/`f64`/`u64`）能过全部四闸、四闸只验 66 个导出
  的一致性、**不验函数内部语义**。任何数值改写（掩码、位宽、符号扩展、饱和）都要单独开审计轴。
- **`--only=` 会静默跳过没登记全的产品（2026-10-01 批 8A 实测）**：一个产品要落盘必须**同时**登记
  三处——`tx-gen2.py` 的 `PROD`（元组形 `'fNNN': ('Qual::Name', 'ret', ['ctx','p0', …])`）、
  `METAS`（`dict(ret=…, params=['a', …], file=…, purpose="Name -- 一句话.", loc={}, frame=N)`）、
  `ORDER` 列表；**只有 tsv 行不够**（`rewrite-names.tsv` 是命名表，不是落盘清单）。缺任一处，
  `python tx-gen2.py --only=fNNN` 会 rc=0、**一行 `EMIT` 都不打、文件也不发**，日志里只剩别的
  函数的 `PROBLEM … leftovers: ['UNREACHABLE']` 噪声，极易误判成「函数太大发不出来」。发完必须用
  `--check` 核对 `identical N / differing 0 / absent 0`。frame 从伪 C 的 `mem[g4] - N` 取。
- **横向前置 callee 的登记形**：多个产品共用的 callee 先只登记名字（tsv 的 `file` 列写 `—`、
  `PROD` 元组、`engine_context.h` 一行 `inline` 转发），实体留在生成层等它自己的批次。这类行会
  出现在 `tx-plan --ready` 输出里，但永远不会被 splice（`rw-splice.py` 只筛真实存在的 `src/<file>`）。
- **单 TU 内的常量重名是硬错（批 17 实测）**：resume 码常量原按 `kResume<Callee><NN>` 命名（不带 fnid），
  `render/layer_events.cpp`（f3661）与 `text/babel_encode_scorer.cpp`（f1674）都经 `Oc_0`（DeallocateRawBlock）
  以码 1 重入 ⇒ 两文件各印一份 `constexpr uint32_t kResumeDeallocateRawBlock01 = 1u;`，而单 TU 拼装所有产品 ⇒
  `error: redefinition of 'kResumeDeallocateRawBlock01'`（码 01 是唯一撞名；layer_events 另有 41/51/54/118、
  babel 另有 00，当时不撞）。已修：`tx-gen2.py` 的 resume 常量名带 fnid（`kF<nnnn>Resume<Callee><NN>`，
  与既有的 `kF<nnnn>ResumeWordBytes`/`kF<nnnn>SaveFrameBytes` 同形）。**规矩：凡印进产品文件作用域的常量、
  枚举或类型名都必须带 fnid**；动了这类命名要跑一次全量构建，不能只看本批产品。
- **改名不追溯，全量 `--check` 是唯一的漂移探针（批 17 实测）**：`--only=` 只核对本批产品；callee 改名
  （`GrowTableSlot`→`EnsureTableArrayRoom`）与占位参数改名（`a0..a5`→`a..f`）不会回头改已落产品的原型行与呼叫点。
  批 17 首次全量 `python tx-gen2.py --check` 报 `identical 235 / differing 58 / absent 0`，逐件 diff 证实 58 件
  全是陈旧名字（无一行逻辑差异），全量重发后 `identical 293 / differing 0 / absent 0`。
  **规矩：改 `PROD`/`METAS` 里任何名字后，跑全量 `--check`，把 DIFF 行全量重发**（`differing` 必须是 0 才算收工）。
- **fread 家族曾整族反向命名（批 13–16 遗留，批 17 依证据改正）**：`util/copy_bytes.cpp` 的
  `CopyBytes(ctx, dst, src, n)` **首参是 dst 且返回 dst**（官方 C `.ref/artemis-official/wasm/asyncify/artemis.wasm2c.c:112723-112983`
  尾部 `var_i0 = var_p0; … return var_i0;`）；f962 第 226 行是 `CopyBytes(..., mem[流对象+20], p0, p1)`，数据**写进**流对象，
  且与 musl FILE 布局吻合（wend@16 / wpos@20，容量检查 `mem[p2+16] - mem[p2+20]`，写满先走 `__towrite` 式分支）
  ⇒ f962 = `FwriteBuffered`（≈`__fwritex`）、f1210 = `PrepareStreamWrite`（≈`__towrite`）、f366 = `Fwrite`（≈`fwrite`，
  `l = size*nmemb` / 返回 `k==l ? nmemb : k/size`）。旧名 `ReadInputLine` / `ResetInputLineState` / `Fread` 已 `git mv`
  改正为 `util/{fwrite_buffered,prepare_stream_write,fwrite}.cpp`。教训：**方向性命名要拿 CopyBytes 的首参（dst）
  或 memcpy 的 dst 语义钉死**，别按“看起来像读”猜；调用侧（`host/fetch_manager_command.cpp` 里 `Fwrite(ctx, b, 1u, c, l4)`
  把下载数据写文件）是第二道佐证。
