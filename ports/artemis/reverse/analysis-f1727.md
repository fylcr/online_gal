# f1727 巨件结构分析与重写方案（供主线决策）

> 分析对象：`.ref/artemis-official/wasm/asyncify/artemis.wasm2c.c:806233–811597`（5,365 行），
> `u32 w2c_artemis_f1727(w2c_artemis* instance, u32 var_p0, u32 var_p1)`。
> 结构地图：`.ref/artemis-official/code/f1727.cpp`（5,478 行逐行转写，符号名 `art_refill_record_state`，111 条锚点注释）。
> 本文档只做分析，不改任何现有文件。行号引用：`f1727.cpp:NNN` 指转写件，`wasm2c.c:NNN` 指原文件。

---

## 1. 语义定性

**结论：f1727 是 Lua 5.1 风格词法分析器的 `llex()`——「读取并返回下一个 token」的主循环，行为与 Lua 5.1 `llex.c` 的
`llex(LexState *ls, SemInfo *seminfo)` 逐支吻合。**

签名语义：`var_p0 = LexState*`，`var_p1 = SemInfo*`，返回值 = token 枚举值（u32）。

### 1.1 定性证据

**(a) 错误文案全部是 Lua llex.c 的 lexerror 文案**（strings.tsv 核实）：

| 池址 | 文案 | Lua 5.1 llex.c 对应 |
|---|---|---|
| 175138 | `<eof>` | lexerror 的 token 名兜底 |
| 175292 | `%s:%d: %s` | `luaG_addinfo` 格式（源名:行: 消息） |
| 175302 | `%s near '%s'` | `lexerror` 结尾格式 |
| 175315 | `lexical element too long` | 缓冲超限 |
| 175340 | `invalid long string delimiter` | `[` 长括号 `skip_sep` 失败 |
| 175475 | `unfinished string` | 短/长字符串读到 EOF |
| 175493 | `escape sequence too large` | `\ddd` 转义超 255 |

**(b) 主循环 switch 分派值 = `*(var_p0+0) + 1` = 当前字符码 + 1**（EOZ = -1 → case 0 → 返回 EOS），
正是 llex 的 `switch (ls->current)` 天然布局（f1727.cpp:89–222 var_L8 主循环）。

**(c) 返回值即 Lua 5.1 token 枚举**（5.1 把 `TK_EOS..TK_DOTS` 排在 ASCII 之后；本件常量全景）：

| 常量 | token | 证据（f1727.cpp 行号） |
|---|---|---|
| 287 | TK_EOS | :99 主循环默认值、EOZ 返回；f244 的 `kStateMark` 即此值 |
| 286 | TK_STRING | :556/:584 长字符串、:3631 luaX_newstring 字符串路径 |
| 285 | TK_NAME | :4237 标识符路径 |
| 284 | TK_NUMBER | :5265（B6 数字，f5594 读完返回） |
| 283 | TK_NE `~=` | :5403（B5/B432） |
| 282 | TK_LE | :867（B15 `'<'` 支路） |
| 281 | TK_GE | :1006（B14 `'>'` 支路） |
| 280 | TK_EQ | :728（B16 `'='` 支路） |
| 278 | TK_CONCAT `..` | :4916（B7 `'.'` 支路） |
| 279 | TK_DOTS `...` | :5224（B7 `'.'` 支路） |
| 91 / 126 | `'['` / `'~'` 单字符 token = 字符本身 | B17（sep==-1 时）、B5 |

**(d) 解析类逻辑**：十进制累乘 `×10 + digit-48` 是 `\ddd` 转义累乘（短字符串 B13 内）；
34（`'"'`）判断是字符串终止判断。B6（数字）直接调 f5594(p0,p1) 读完返回 284——`read_numeral` 同型。

**(e) 与 f244（luaX_next 位）的配合**：f244 = `StepRecordResume`（tsv:12）操作 record 结构（+4/+8/+16..31/+24/+32），
调 `RefillRecordState(ctx, record, carried)` 后把返回的 token 回填写回 +16——这正是 `luaX_next` 里
`ls->t.token = llex(ls, &ls->t.seminfo)` 的形状：f1727 返回 token、经 p1（SemInfo*）写出 `seminfo->ts`/数值。

### 1.2 各 case 的语义映射（llex.c 对照）

| switch case（=字符+1） | 字符 | 标签 | llex.c 对应 |
|---|---|---|---|
| 0 | EOZ | →B4 | 返回 TK_EOS(287) |
| 11 / 14 | `\n` / `\r` | →B20 | `inclinenumber` 后续循环 |
| 35 / 40 | `"` / `'` | →B13 | 短字符串 `read_string`（含 `\ddd` 转义） |
| 46 | `-` | →B18 | 注释 `--`；`--[=*[` 长注释走 f901/f1726（B12） |
| 47 | `.` | →B7 | `save_and_next`+`check_next`：`..`→278、`...`→279、数字前缀 |
| 61 / 62 / 63 | `<` / `=` / `>` | →B15/B16/B14 | 双字符比较 token（`<=` 282、`==` 280、`>=` 281） |
| 92 | `[` | →B17 | 长字符串：f901=skip_sep，sep≥0 → f1726(p0,p1,sep) 返 286；sep==-1 返 91；否则 f397 报 175340 |
| default | `~` 等 | →B11→B5 | `'~'`/`'~='`→283（f285 两次取字符，挂起点 85/86） |
| 其余 | 空白 9-13/32 | →B10 | 跳过续循环 |
| 其余 | `0`-`9` | →B6 | f5594(p0,p1) 读数字 → 284 |
| 其余 | 字母/下划线 | →var_L312 | 标识符累积循环 → luaX_newstring 组合 → 285/保留字 |

**luaX_newstring 组合**（f1727.cpp:3500–3635 与 :4133–4211 两处同型）：
`f247(L, buf, len)`（InternString，驻留）→ `f778(L, table, ts)`（GetTableFieldByString 查保留字表）→
miss 建槽 `f363(L)` → `store(p1, l6)`（`seminfo->ts = ts`）→ 字符串路径返 286（:3631）/ 标识符路径返 285（:4237，
保留字命中返回表中 token 值 257..277）。

**错误链**（buff 超限 `>=2147483646` 或语法错，样本 f1727.cpp:1011–1215、:4307–4420）：
1. `f312(dst=l10+208, src=*(p0+64)+16, 80)`——`p0+64` = `TString* source`（+16 = getstr 偏移），
   疑似 `luaO_chunkid`（源名→80 字节短 id 缓冲，落在帧局部区 +208）。
2. 栈上三字向量（+192 向量指针、+196 `linenumber = *(p0+4)`、+200 池串地址）。
3. `f219(L, 175292u "%s:%d: %s", vec)`——`luaO_pushfstring` 的 wasm 化（L, fmt, 变参向量）。
4. `f236(L, 3)`——`luaD_throw(L, 3)`，3 = LUA_ERRSYNTAX。
5. buff 扩容 `f223(L, block, old, new)`（ReallocateBlock）→ 超限 `f250(L)`（RaiseBlockTooBig = luaM_toobig）。

---

## 2. 相位机结构图

### 2.1 恢复序言帧布局（f1727.cpp:12–76，与挂起压帧 :5417–5474 严格对称）

asyncify 保存帧 **44 字节 / 11 个保存字**（g3==2 rewinding 时先弹 44 字节，再弹 4 字节挂起编号）：

| 帧偏移 | 恢复到 | 变量 | 语义（llex） |
|---|---|---|---|
| +0 | var_p0 | var_p0 | LexState*（实参 0） |
| +4 | var_p1 | var_p1 | SemInfo*（实参 1） |
| +8 | var_l2 | var_l2 | 返回值 / 当前 token |
| +12 | var_l3 | var_l3 | switch 分派值（字符+1） |
| +16 | var_l4 | var_l4 | 临时 |
| +20 | var_l6 | var_l6 | 临时 |
| +24 | var_l7 | var_l7 | 临时 |
| +28 | var_l8 | var_l8 | 临时（B7 路径复用） |
| +32 | var_l10 | var_l10 | 本地帧基址（g0-=288） |
| +36 | var_l11 | var_l11 | 临时 |
| +40 | var_l12 | var_l12 | 临时 |

外加 **4 字节帧**：挂起编号 → var_l9（挂起时先 `*g4 = 编号; g4 += 4`，恢复时先弹 44 再弹 4）。
本地帧：`g0 -= 288`（:79–81），出口 `g0 += 288`（B4，:5408 起）。
两个保存帧的保存/恢复代码逐字互为镜像，R1 时不得改动任何一边（§8 禁区）。

### 2.2 挂起点清单

**共 87 个挂起点，编号 0–86 连续各一次**，与 87 个 `goto var_B1` 一一对应（99 个调用点中 12 个同步不挂起）。

统一模式（每点三件套）：
- **选择守卫**：`var_i0 = (var_l9 == N); var_i1 = 1; var_i2 = g3; var_i0 = var_i2 ? var_i0 : var_i1`——
  running 直通、rewinding 仅当挂起编号命中才重放该调用；
- **重放调用**（f285/f901/f1726/…，返回值经 var_l5 中转）；
- **挂起写编号**：`var_i0 = N; if (g3 == 1) goto var_B1`——suspended 时写编号并转 B1 压帧。

按 callee 分布（代表点；f1727.cpp 行号）：

| 编号 | callee | 位置/语境 |
|---|---|---|
| 0 | f902 | :212 主循环尾（换行 inclinenumber） |
| 1–4… | f285 | 主循环/字符串读取各次取字符（NextCharSlow 兜底，共 24 调用点的大部分） |
| 5 | f901 | :513 B17（长括号 skip_sep） |
| 6 | f1726 | :545（ReadLongString sep≥0） |
| 7 | f397 | :585（LexError 报 175340） |
| 8–13 | f285/f902/… | 字符串与注释路径 |
| 14 | f219 | 错误链（FormatPushString） |
| 15 | f236 | 错误链（luaD_throw） |
| 16 | f223 | 错误链（ReallocateBlock 扩容） |
| 17 | f250 | 错误链（RaiseBlockTooBig） |
| … | … | … |
| 55 | f247 | :3537 InternString（字符串路径） |
| 56 | f778 | :3566 GetTableFieldByString |
| 57 | f363 | :3615 建槽 |
| 58 | f1726 | B12 长注释（`f1726(p0, 0, sep)`） |
| 59 | f285 | … |
| 84 | f5594 | :5254 B6 ReadNumeral |
| 85 / 86 | f285 | B5 `'~'` 两次取字符（→126 / →283） |

完整编号表可用 `grep -B4 'goto var_B1' f1727.cpp` 复核（96 命中中含少量守卫误抓，如 10u/9u/4294967295u 为比较值）。

### 2.3 主循环 var_L8（f1727.cpp:89–222）

- **进入条件**（g3==0，running 首入）：`l6 = *(p0+0)`（取当前字符）、`l3 = l6 + 1`、`l2 = 287`（默认 TK_EOS）→ switch(l3)。
- **续循环**：多数 case 块处理完 `goto var_L8`。
- **出口 B4**（:5408）：`g0 += 288; return var_l2`（返回 token）；EOZ → case 0 → B4 返回 287。
- **挂起出口 B1**（:5420）：g3==1 时压 4 字节挂起编号 + 44 字节保存帧，返回 0。

### 2.4 switch case→标签映射表

| case 值 | 源字符 | 目标标签 | 处理 |
|---|---|---|---|
| 0 | EOZ | B4 | 返回 287 |
| 11 | `\n` | B20 | inclinenumber |
| 14 | `\r` | B20 | inclinenumber |
| 35 | `"` | B13 | read_string |
| 40 | `'` | B13 | read_string |
| 46 | `-` | B18 | 注释/长注释 |
| 47 | `.` | B7 | `..`/`...`/数字前缀 |
| 61 | `<` | B15 | `<=`→282 / `<` |
| 62 | `=` | B16 | `==`→280 / `=` |
| 63 | `>` | B14 | `>=`→281 / `>` |
| 92 | `[` | B17 | 长字符串（f901/f1726/f397） |
| default | — | B11 | `'~'` 特判 → B5 |
| 其余 | 空白/数字/字母 | B10 / B6 / var_L312 | 见 §1.2 |

其他定位：var_B12 :3636（长注释 `f1726(p0,0,sep)` + buff 复位）、var_B10 :3677、var_B11 :3666、
var_B5 :5270、var_B6 :5244、var_B432 :5395、var_B1 :5420。

---

## 3. callee 清单

签名列格式：`u32/void (instance, …)`；行号为 artemis.wasm2c.c 定义行。

| fn | 签名 | 调用× | 作用推断（证据） | 建议命名 | tsv 状态 |
|---|---|---|---|---|---|
| f285 | u32(instance, 1×u32) :132109 | 24 | 取下一字符慢路径（ZIO fill；体内内联 zgetc 快路径 count--/ptr++ 后兜底调用） | `artc::NextCharSlow`（或 `FillInput`） | 待定名 |
| f219 | u32(instance, 3×u32) :116173 | 13 | `luaO_pushfstring` 的 wasm 化（L, fmt, 变参向量→新串） | `artc::FormatPushString` | 待定名 |
| f312 | void(instance, 3×u32) :138109 | 12 | 疑似 `luaO_chunkid`：源名(TString*+16) → 80 字节短 id 缓冲（帧 +208） | `artc::FormatSourceId` | 待定名 |
| f236 | void(instance, 2×u32) :121438 | 12 | `luaD_throw(L, errcode)`（errcode=3=LUA_ERRSYNTAX） | `artc::ThrowScriptError` | 待定名 |
| f250 | u32(instance, 1×u32) :124567 | 11 | `luaM_toobig` 抛「block too big」 | `artc::RaiseBlockTooBig` | **已知名沿用** |
| f223 | u32(instance, 4×u32) :116561 | 11 | `luaM_realloc_(L, block, old, new)` | `artc::ReallocateBlock` | **已知名沿用** |
| f397 | void(instance, 3×u32) :175102 | 3 | `lexerror(ls, msg, token)`（3 调用点=3 种错误文案） | `artc::LexError` | 待定名 |
| f902 | void(instance, 1×u32) :355751 | 2 | `inclinenumber`（换行计数/行号累加） | `artc::InclineNumber` | 待定名 |
| f901 | u32(instance, 1×u32) :355009 | 2 | `skip_sep`（长括号 `=` 计数，返回 sep） | `artc::SkipSep` | 待定名 |
| f778 | u32(instance, 3×u32) :298928 | 2 | 被驻留字符串的 table 取场（保留字表查询） | `artc::GetTableFieldByString` | **已知名沿用**（tsv:13） |
| f363 | void(instance, 1×u32) :160907 | 2 | 表插入/增长单参(L)（luaH_newkey/luaH_resize 形状，intern miss 建槽） | `artc::GrowTableSlot`（候选，待 R1 对照） | 待定名 |
| f247 | u32(instance, 3×u32) :123983 | 2 | 字符串驻留 `luaS_newlstr(L, buf, len)` | `artc::InternString` | **已知名沿用**（tsv:32） |
| f1726 | void(instance, 3×u32) :804159 | 2 | `read_long_string(ls, seminfo, sep)`（B12 长注释时 seminfo=0） | `artc::ReadLongString` | 待定名 |
| f5594 | void(instance, 2×u32) :2720258 | 1 | `read_numeral(ls, seminfo)`（B6 数字，返回 284） | `artc::ReadNumeral` | 待定名 |
| f1727 | 自递归 | 1 | 自身（goto 式自递归或挂起重入） | `artc::RefillRecordState` | tsv:42，PROVISIONAL |

未定名 10 个（f285/f219/f312/f236/f397/f902/f901/f363/f1726/f5594）命名依据均为「Lua 5.1 llex.c 同位函数名
PascalCase 化」，与 tsv 既有风格（动词开头 PascalCase，如 ReallocateBlock/RaiseBlockTooBig）一致。

---

## 4. R1/R2 方案建议

### 4.1 落位与签名

- **文件**：`rewrite/src/script/refill_record_state.cpp`（f244 的前置声明 :21–27 已按此路径预注，直接对齐）。
- **签名**：`uint32_t RefillRecordState(EngineContext* ctx, uint32_t record, uint32_t arg);`
  （与 step_record_resume.cpp:24 的声明、engine_context.h 跳板原型逐字一致，装配器交叉原型块零改动）。
- **实参语义命名**（函数体内可起别名注释）：
  `var_p0` = **LexState 指针**（f244 语境叫 `record`；偏移族 +0 当前字符、+4 linenumber、+48 保留字表、
  +52 lua_State*、+56 Zio、+60 Mbuffer、+64 TString* source）；
  `var_p1` = **SemInfo 指针**（f244 语境叫 `carried`；`seminfo->ts` 在 :3631/:4237 前被写出）。
  参数名建议保持 `record` / `arg` 以对齐调用方与装配器，语义进注释。
- **匿名 namespace 常量候选**（池址裸值命名，§8 要求；文义经 strings.tsv 核实）：
  - `kTokenEos = 287u`（兼 f244 `kStateMark` 语义）、`kTokenString = 286u`、`kTokenName = 285u`、
    `kTokenNumber = 284u`、`kTokenNe = 283u`、`kTokenLe = 282u`、`kTokenGe = 281u`、`kTokenEq = 280u`、
    `kTokenDots = 279u`、`kTokenConcat = 278u`；
  - `kMsgEof = 175138u`、`kMsgAddInfoFmt = 175292u`、`kMsgNearFmt = 175302u`、`kMsgElementTooLong = 175315u`、
    `kMsgBadLongDelim = 175340u`、`kMsgUnfinishedString = 175475u`、`kMsgEscapeTooLarge = 175493u`；
  - `kMaxBufferSize = 2147483646u`、`kErrSyntax = 3u`、`kLocalFrameBytes = 288u`、`kSaveFrameBytes = 44u`。

### 4.2 挂起点与帧形状

- **挂起点数量：87**（编号 0–86），帧形状 = 44 字节 11 保存字 + 4 字节挂起编号（§2.1 表逐字保留）。
  R1 把三件套（选择守卫/重放调用/挂起写编号）原样搬运并加注释；R2 可按 release_container_elements.cpp
  的先例（494 行 11 挂起点）做**每挂起点一个 EnterFrame/LeaveFrame 帧纪律标注** + 注释统一模板。
- f244 侧约定必须写进头注释：返回 287（TK_EOS/`kStateMark`）是「自身结局之一」（step_record_resume.cpp:33）；
  挂起时返回值清 0 由 f244 压回。

### 4.3 R1/R2 分界

| 部分 | 工序 | 说明 |
|---|---|---|
| asyncify 状态机（g3 门、B1 压帧、恢复序言、87 个三件套） | **R1 禁区**（§8） | 只注释不动，判定顺序一字不动 |
| 主循环 switch、case 块、goto 汤（82 标签 / 332 goto） | R2 结构化 | 逐步换 while/switch/if；双锚（帧哈希+剖析计数）全绿才许动 |
| 错误链五连、luaX_newstring 组合 | R2 可提 helper | 但「判定顺序原封」（allocate_string_buffer.cpp 先例）；f312/f219/f236 三连可提匿名 namespace 内联函数 |
| 池址/常量 | R1 起命名 | 见 §4.1 |

### 4.4 体量与风险

- 体量估计：R1 约 5,500–6,500 行（转写 5,478 行 + 注释/常量），R2 结构化后约 2,500–3,500 行。
- **风险点**：
  1. 87 个挂起点任何一处顺序/编号错位 → 恢复错位灾难（帧哈希必须逐点比对）；
  2. f312 语义（luaO_chunkid 形状）未坐实，80 字节长度与 5.1 的 LUA_IDSIZE=60 不符（此 fork 可能改 80），
     R1 期间保持原序、命名可暂用中性 `CopyErrorContext`/`FormatSourceId` 二选一；
  3. f363 单参(L) 语义未坐实（luaH_newkey/luaH_resize 候选），命名标 PROVISIONAL；
  4. 保留字命中路径返回表值 token（257..277）的分支在标识符段（:4237 附近），R2 结构化时勿与 285 默认值互换；
  5. trap 文案对齐：错误链最终走 f236(L,3) 抛 LUA_ERRSYNTAX，文案经 175292/175302 格式化，wasm-diff --strict
     的 trap-kind-only 断言依赖这些文案原文，池址常量化不得改动数值。

---

## 5. 建议落地顺序与前置条件

1. **产品落地** `rewrite/src/script/refill_record_state.cpp`（R1 优先：搬运 + 注释 + 常量命名）。
   前置：本方案定稿；装配器交叉原型块核对 `uint32_t(EngineContext*, uint32_t, uint32_t)`；
   参照 release_container_elements.cpp 头注释文风。
2. **删跳板** `engine_context.h:78–80` 的 `inline RefillRecordState`。
   前置：步骤 1 进入装配链且 `code-assemble.sh asyncify` differ: 0（跳板与产品定义同名共存会撞）；
   f244 的前置声明（step_record_resume.cpp:21–27）同时去掉 Provisional 注。
3. **tsv 行 42 集中对名**：`f1727 — artc::RefillRecordState script/refill_record_state.cpp R2`（或 R1），
   并补记 10 个待定名 callee 的候选名（§3 表），一次性对齐。
   前置：步骤 1/2 全绿；f312/f363 语义在 R1 对照中确认或标 PROVISIONAL。
4. **P1 收官**：全链 `bash ports/artemis/reverse/code-assemble.sh asyncify`（differ: 0）+
   `node ports/artemis/reverse/wasm-diff.mjs --strict …`（66/66、trap-kind-only 0、exit 0，链内 pipefail）+
   批末深锚（帧哈希 + 剖析计数双锚全绿）。
   前置：步骤 3 完成；R2 结构化部分（若有）双锚数据归档；87 挂起点逐点核对清单留档于本目录。

---

*分析人：space-bunny；只读分析，未改动任何现有文件。*
