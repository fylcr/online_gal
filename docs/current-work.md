# 当前工作检查点 — 新对话先读

> 本文件是滚动恢复入口，不替代历史交接。每完成一个阶段、改变任务归属、启动/关闭实测资源或准备提交时更新。
> 最新检查点：2026-10-04 12:54 +08:00，恢复入口已建立、构建和 portable 基线通过，首次异常探针准备中；游戏历史结论尚未复测。

## 1. 用户要求与目标

- 继续 Artemis 主线；具体代码和细节由 **magpie/group/auto-deepseek-v4-1-flash** 承担，主代理负责拆分、独立证据核验、代码审核和验收。
- 低并发（当前最多 2 个细任务），浏览器/游戏同一时刻只运行一个实例；代理未获主代理授权不得自行开游戏或构建桥。
- 任何新对话须能从此文件直接恢复，不能依赖旧会话压缩或未落盘的代理结果。
- 完成目标仍是：对应提交的 CI 产物在独立现代宿主中通过真实游戏冷启动、输入、pause/resume、save/load，丢弃一次热身后取得两次一致结果；授权边界独立判断。
- 当前 DSH goal：`goal-4c733db7-f628-4026-a0ef-4b5bd99509e1` revision 1，active/armed。新对话先查询自己的 goal，不要复制旧 goal id 或声称它仍活动；获得用户继续指令后按工具要求恢复。

## 2. 已直接核对的仓库状态

- 工作区 `/d/online_gal`，shell 是 Git Bash（`pwsh` 工具实际 bash）；分支 `main`，HEAD `d2b1ad0`。
- 本次开始前已有改动：`docs/项目状态与恢复指南.md` modified，`AGENTS.md` 与 `docs/session-5697f1a7-handoff.md` untracked。**不再沿用历史文档的“工作树干净”说法**；不覆盖、不还原用户改动。
- 本轮新增恢复入口为本文件，随后更新根 AGENTS 和恢复指南导航。生产源码尚未修改。
- 历史交接：[`session-5697f1a7-handoff.md`](session-5697f1a7-handoff.md)，以 §4.3.3 / §6 / §7 为准。旧 README/状态指南含已被后续接线推翻的句子，生产现状须实读核对。

## 3. 当前卡点与禁止跳过的核验

历史最新记录：GitHub bridge 在独立自写宿主启动约 3 秒调用异常查找 `c()`，该调用开始时 lastException 已为 0；同宿主官方 WASM 对照 50 秒未触发，观测恢复点 602。**根因未知**。

本轮第一件事：把观测前置到每个 EH 查找 import 之前，获取最早调用的原生 wasm 栈、参数、异常事件顺序、恢复状态，然后映射到准确源码位置。不得通过忽略 `r(0)` 或伪造异常状态使测试假通过。

独立只读旁线：比较 `FinishTableIndex` 与官方 f643，静态本体差分未发现直接语义差异，调用方未覆盖。不要把“最可能”当发现，也不要把未触发路径列为根因；不接受报告中“trap记录已污染”“必归因宿主”“结构化重写opcode必须逐条相同”的未测推论。

本轮包基线已通过：EH 的 CatchInfo.base / 16B header / real native signatures 和 boot deferred/silent-neutral honoured 检查都已经在当前 HEAD 测试通过。历史交接把它们列作未完成，但不能不读现代码就重新修；这不等于真实游戏 EH 路径健康。

## 4. 当前任务与写入范围

| 任务 | 归属/标识 | 范围与状态 |
| --- | --- | --- |
| 首次异常调用定位 | Flash subagent `1936d4a4-c4fa-4163-8e0b-1a4bf9c0b2f3` | 只可创建/编辑 `.test/artemis-resume-exception-*`，读取生产源码与现有 CI 产物；准备探针，尚未授权构建、浏览器或服务器 |
| FinishTableIndex trap-diff | Flash subagent `4580aac6-5a64-4a5b-812f-cbb41bdf2367`，已结束 | f643 本体静态逐行比对未发现语义差异；调用方与动态路径未覆盖，不代表排除本函数；报告的“trap记录已污染/归因宿主/字节码必相同”推论未测量，不接受为发现 |
| 异常候选源码核验 | Flash subagent `e99d3cd8-5b7c-45c8-96cd-a1d66563dca7` | 查历史 i(0) 栈→BuiltinDispatch(f3633) 的 guard、852316共享槽与恢复点602，未获任何写入/构建/浏览器权限 |
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

- 当前会话未启动浏览器、服务器或桥构建。
- 基线后台 job `pwsh-177` 已收集，exit 0：`npm run build:packages && npm run test:portable`，当前 **333/333 pass，0 skip**，日志 `.test/artemis-resume-baseline.log`；不是游戏验收。
- source/layout 检查已通过，340 mapped functions / 340 product files；许可 warning 仍存在。
- 两个 Flash 是一次性 subagent，不是 Agent Teams；`send_message` 向首个 id 补充约束报 active teammate not found，**不证明任务停止**。等其结果后审核，不依此重派造成重复写入。
- 不终止用户浏览器或 9222 chrome-mcp 进程；只关闭本轮创建的标签/窗口/专用服务器。

## 7. 下一步（按顺序）

1. 读本文件、历史交接最新节、根约定；`pwd; git status --short; git log -3 --oneline` 验证真实状态。
2. 收集并审核两个 Flash 报告；探针审查前不运行引擎，不接受无实测根因。
3. 核对当前 EH/boot 与 d2b1ad0 修复的实际状态，运行必要组件基线，明确失败测试与已修项目。
4. 用下载 CI bridge 在独立宿主复现，串行同宿主官方对照以测定位线索；这一步仅诊断，不算产品验收。
5. 定位后委托窄范围最小修复，主代理审 diff 与独立回归；继续 honoured/GL/audio/browser-services 遗留。
6. 冻结源码，质量门、显式 pathspec 提交并及时 push，观察同一提交 CI；下载该 commit 产物冷启动真实游戏验收。
7. 每个阶段回写本文件；记录已测/未知、提交/CI/产物对应关系与清理状态，避免只在对话中保存进度。
