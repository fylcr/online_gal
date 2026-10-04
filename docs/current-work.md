# 当前工作检查点 — 新对话先读

> 本文件是滚动恢复入口，不替代历史交接。每完成一个阶段、改变任务归属、启动/关闭实测资源或准备提交时更新。
> 最新检查点：2026-10-04 13:17 +08:00，恢复入口已提交推送、三类基线通过；前两份异常定位报告因观测器/映射错误未接受，新最小探针仍在审核。发现602/603区域静态相位门差异，但根因尚未实测。

## 1. 用户要求与目标

- 继续 Artemis 主线；具体代码和细节由 **magpie/group/auto-deepseek-v4-1-flash** 承担，主代理负责拆分、独立证据核验、代码审核和验收。
- 低并发（当前最多 2 个细任务），浏览器/游戏同一时刻只运行一个实例；代理未获主代理授权不得自行开游戏或构建桥。
- 任何新对话须能从此文件直接恢复，不能依赖旧会话压缩或未落盘的代理结果。
- 完成目标仍是：对应提交的 CI 产物在独立现代宿主中通过真实游戏冷启动、输入、pause/resume、save/load，丢弃一次热身后取得两次一致结果；授权边界独立判断。
- 当前 DSH goal：`goal-4c733db7-f628-4026-a0ef-4b5bd99509e1` revision 1，active/armed。新对话先查询自己的 goal，不要复制旧 goal id 或声称它仍活动；获得用户继续指令后按工具要求恢复。

## 2. 已直接核对的仓库状态

- 工作区 `/d/online_gal`，shell 是 Git Bash（`pwsh` 工具实际 bash）；分支 `main`，恢复源码基线 `d2b1ad0`。恢复入口文档已提交并 push：`65beaca`；该提交没有生产代码变动。当前检查点后续资源状态有滚动未提交更新。
- 本次开始前已有改动：`docs/项目状态与恢复指南.md` modified，`AGENTS.md` 与 `docs/session-5697f1a7-handoff.md` untracked。**不再沿用历史文档的“工作树干净”说法**；不覆盖、不还原用户改动。
- 本轮新增恢复入口为本文件，已更新根 AGENTS 和恢复指南导航；603 latch相位门唯一一行已审核，新增portable回归test（4test），runtime-artemis0.1.8 / engines0.1.9版本已审。准备显式提交此修复与检查点；没有修改其它生产源码。
- 历史交接：[`session-5697f1a7-handoff.md`](session-5697f1a7-handoff.md)，以 §4.3.3 / §6 / §7 为准。旧 README/状态指南含已被后续接线推翻的句子，生产现状须实读核对。

## 3. 当前卡点与禁止跳过的核验

历史最新记录：GitHub bridge 在独立自写宿主启动约 3 秒调用异常查找 `c()`，该调用开始时 lastException 已为 0；同宿主官方 WASM 对照 50 秒未触发，观测恢复点 602。**根因未知**。

本轮第一件事：把观测前置到每个 EH 查找 import 之前，获取最早调用的原生 wasm 栈、参数、异常事件顺序、恢复状态，然后映射到准确源码位置。不得通过忽略 `r(0)` 或伪造异常状态使测试假通过。

主代理已实读到 `lua_tags_builtin.cpp:20619-20624` 的 **guest resume_point=602**（system字段 HostInvokeViiiiii1959），不是首个assert恢复点0；`:20654-20659` 紧接603的 latch 处理是 `!= kResumeRunning`，正式官方 `wasm2c.c:1748101-1748119` 是running臂才读/清latch并取反p2。这是静态真实差异，**是否导致首次c/崩溃未测**，已委托窄任务合成差分+控制流核验。
`eh.ts:384-386` 才是 last0返回0，第二个报告引用404错误。`__setThrew`正式官方体 `wasm2c.c:1161638-1161655` 写852316及852320，g3仍仅相位；需新pre探针测852316/20。

独立只读旁线：比较 `FinishTableIndex` 与官方 f643，静态本体差分未发现直接语义差异，调用方未覆盖。不要把“最可能”当发现，也不要把未触发路径列为根因；不接受报告中“trap记录已污染”“必归因宿主”“结构化重写opcode必须逐条相同”的未测推论。

本轮包基线已通过：EH 的 CatchInfo.base / 16B header / real native signatures 和 boot deferred/silent-neutral honoured 检查都已经在当前 HEAD 测试通过。历史交接把它们列作未完成，但不能不读现代码就重新修；这不等于真实游戏 EH 路径健康。

603相位门修复唯一一行diff已现场核验；portable新测试最终精简为4/4通过（18组phase×p2×latch+内存mutation旧!=反例），running=g3==0注释已纠正。全树style-gate exit1，首项为未改动的compositor_effect.cpp等w2c_调用残留；不假报全绿，不在本轮扩大风格返工。style日志 `.test/artemis-resume-style.log`。

## 4. 当前任务与写入范围

| 任务 | 归属/标识 | 范围与状态 |
| --- | --- | --- |
| 首次异常调用定位 | Flash subagent `1936d4a4-c4fa-4163-8e0b-1a4bf9c0b2f3`，已结束 | 写入 `.test/artemis-resume-exception-*`；主代理拒绝报告/探针：instantiate Promise当instance、banner HTTP引用本地文件、读错导入memory、表slot当func index、firstSearch过滤错误、dump时机缺失、扫描器错import编号/函数映射。报告"全部Lc表调用/各结果唯一解释"未经测量且与原生反汇编矛盾；这些文件不能作为判据运行 |
| FinishTableIndex trap-diff | Flash subagent `4580aac6-5a64-4a5b-812f-cbb41bdf2367`，已结束 | f643 本体静态逐行比对未发现语义差异；调用方与动态路径未覆盖，不代表排除本函数；报告的“trap记录已污染/归因宿主/字节码必相同”推论未测量，不接受为发现 |
| 异常候选源码核验 | Flash subagent `e99d3cd8-5b7c-45c8-96cd-a1d66563dca7`，已结束 | 仅接受 func873符号→BuiltinDispatch(f3633)、func5986→a.i薄转发的实读映射；报告混用官方f873与候选func873、错EH行号/last0分支、把恢复点0当602，主代理拒绝其排除结论 |
| 第二版前置探针 | Flash subagent `8a09fd92-b743-4054-b2b5-38ab438b9c84`，已结束 | `.test/artemis-resume-precall-*` 的selfcheck通过但主代理仍拒绝：__pcWatched try内const被catch外读，memoryRef/lastSeen未安装/未写，header observer两份覆盖global，instantiate Promise误认、dump按钮/失败hook缺失、索引假标签；selftest没执行真实变换wrapper，不能作为判据运行 |
| 最终前置探针 | Flash `77c68aec-509c-496e-beb4-3dbfa54a1eae`，已结束 | `.test/artemis-resume-finalpre-*` 实际变换eh nativeWASM自检17项主代理重跑PASS，但仍有monkeypatch/Promise误认/堆来源冗余/栈全局修改，待最后窄修正Flash（id待记）去掉多余挂钩、直接closure传入memory、临时栈depth恢复；未获浏览器/服务器权限 |
| 602/603门静态差分 | Flash subagent `ef3d1fe8-90ca-404d-80e6-84d2e425b878`，已结束 | `.test/artemis-resume-602-audit.{md,mjs}` 对phase×p2×latch12例差异、flip零残差；非游戏根因。报告'bareflip不等价'与自有模型矛盾不接受 |
| 已证实603门最小修复 | Flash `2b504467-8162-4ba4-9198-25b0c28ff5ee` / 精简 `cdf3de30-6d6c-453c-9923-348a196cd432`，均结束 | 唯一生产行 `lua_tags_builtin.cpp:20654` !=→==已独立核验；test缩为128行/4test，guard/body绑定，版本runtime0.1.8/engines0.1.9。错误注释g3!=0=running已由Flash `2dbeb361-409f-4e12-9583-9e6a82d3156f` 校正，仅两注释行；真实running为g3==0。不接受早期worker真实源临时mutation作为证据。 |
| 恢复入口、基线与独立审核 | 主代理 | 本文件、AGENTS、指南入口；build:packages + portable 333/333已通过；基线测试和最终验证由主代理掌控 |

这些代理 id 只属于本次会话。新对话不能假定可继续向它们发消息：先核验活动任务/资源，再重派同一窄任务；禁止与旧任务重复写同一文件或同时跑引擎。

## 5. 可复用诊断入口

- `.test/artemis-independent-host-server.mjs`：下载 package + 指定 wasm + pristine loose game；只读游戏、随机空闲端口、同源 Range，打印 `HARNESS-READY`。
- `.test/artemis-independent-host-entry.mjs`：浏览器端真正 runtime host、IndexedDB、音频和画面计数；独立宿主，不是官方 glue。
- `.test/artemis-d2b1ad0-abi-independent.mjs`：解码真实 wasm section 并与 native import/export 交叉验证，**不代表游戏验收**。
- 已有 d2b1ad0 CI bridge：`.test/github-d2b1ad0/artemis-wasm/dist/artemis-bridge/build/bridge.wasm`，本轮现场 sha256=`f6e87c2301c73284d604b3ba5e023b277d32367a501611e80738f2f5051433e8`，与 manifest/历史 provenance 一致。
- `gh run list` 本轮核验同提交成功 run：bridge `37142404767` / packages `37142404507`（`d2b1ad0bad09661f38c2aac7bac5ce9fbd23c747`）；pristine game-chs/system.ini 现场 hash=`de3e548de1b5115d815a2ea4ce421f3076e6e1393f22039bead2b0bdbadf20f7`。
- `.test/artemis-d2b1ad0-eh-observation-{server,plugin,browser}.mjs`：历史观察专用 esbuild 变换（不改下载包文件、返回值或异常），可复用；`.test/artemis-d2b1ad0-eh-observation01-evidence.json:2321` 保留首次 c 的 last=0，但没有 pre-call stack，需要新探针。
- 官方参考（只读诊断，不可新增入库）：`ports/artemis/vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm` 和 `.ref/artemis-official/wasm/asyncify/artemis.wasm2c.c`。
- wasm 栈 `wasm-function[N]` 为含 import 的索引，不能减 207；不要打印原始 wasm 大块字节。

## 6. 资源账本

- 当前会话未启动游戏、服务器或桥构建。`browser_session list` 报一个待cleanup start，stop 报 BSK daemon 无法从当前 Job Object 脱离；按工具建议使用持久后台 job 启动 `bsk daemon start --foreground`，启动后核验并重试原cleanup，不能换浏览器后端规避。
- 基线后台 job `pwsh-177` 已收集，exit 0：`npm run build:packages && npm run test:portable`，当前 **333/333 pass，0 skip**，日志 `.test/artemis-resume-baseline.log`；不是游戏验收。
- source/layout 检查已通过，340 mapped functions / 340 product files；许可 warning 仍存在。
- 603修复后主代理质量门 job `pwsh-489` 已收集 exit0：build:packages + portable **338/338** + source/layout；最终精简4test版本的主代理质量门 `pwsh-541` 已收集exit0，build:packages + portable **337/337**，日志 `.test/artemis-resume-final-portable.log`。
- 文档显式 pathspec commit+push 后台 job `pwsh-253` 已收集 exit 0：`65beaca` 已推送 origin/main，范围仅 AGENTS、本检查点、历史交接和指南入口；非引擎代码/产物变更，不触发引擎workflow是预期。
- BSK foreground daemon job `pwsh-254` 活动，Windows PID `39780`；`BSK_AUTO_START=0 bsk status --json` 独立核验 ready，Chrome实例 `aa5bb810` / Edge实例 `7e2c2b96`，无session。原待cleanup请求 `1791074559377:70d3e0fa-334c-4ff8-b3b7-aab410756d2b`（gtip）已通过原stop完成。仅用 browser_* 操作页面，daemon CLI仅基础设施启停；用完关自有sessions后停本job。
- 本地完整基线 job `pwsh-293` 已收集 exit 0：`npm test`，439 tests / 434 pass / 5 skip，日志 `.test/artemis-resume-local-tests.log`；5 skip 为既有遗留，不等于全部实现。
- 包TS+Astro检查 job `pwsh-307` 已收集 exit 0：Astro 9 files / 0 errors / 0 warnings / 0 hints，日志 `.test/artemis-resume-check.log`。
- 本轮 browser_* 空白 session `xwoy` 已明确 stop成功（长空闲时daemon曾自动stop，工具list仍残留记录，activity报session stopped；最终原session stop收口）。无本轮活动浏览器窗口/标签；可信探针就绪后重新start，不复用失效session。
- 两个 Flash 是一次性 subagent，不是 Agent Teams；`send_message` 向首个 id 补充约束报 active teammate not found，**不证明任务停止**。等其结果后审核，不依此重派造成重复写入。
- 不终止用户浏览器或 9222 chrome-mcp 进程；只关闭本轮创建的标签/窗口/专用服务器。

## 7. 下一步（按顺序）

1. 读本文件、历史交接最新节、根约定；`pwd; git status --short; git log -3 --oneline` 验证真实状态。
2. 收集并审核两个 Flash 报告；探针审查前不运行引擎，不接受无实测根因。
3. 核对当前 EH/boot 与 d2b1ad0 修复的实际状态，运行必要组件基线，明确失败测试与已修项目。
4. 用下载 CI bridge 在独立宿主复现，串行同宿主官方对照以测定位线索；这一步仅诊断，不算产品验收。
5. 定位后委托窄范围最小修复，主代理审 diff 与独立回归；继续 honoured/GL/audio/browser-services 遗留。
6. 603门最小修复已独立审核、质量门337/337；立即显式pathspec提交生产一行+test+两版本+本检查点并push，等同commit bridge/packages CI；下载该commit产物先对旧基线串行诊断再真实冷启动验收。全树style既有exit1单独记录，不伪造过门。
7. 每个阶段回写本文件；记录已测/未知、提交/CI/产物对应关系与清理状态，避免只在对话中保存进度。
