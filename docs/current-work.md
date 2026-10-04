# 当前工作检查点 — 新对话先读

> 更新：2026-10-04 14:33 +08:00。Artemis 仍不可用；已修一个真实的源码相位门缺陷，但下载 CI 产物真游戏仍失败。首次 `c()` 的前置原生栈已捕获，正在映射到准确源码。不要把“CI 绿”读成可玩。

## 1. 用户要求与恢复纪律

- 具体代码和细节由 **magpie/group/auto-deepseek-v4-1-flash** 承担，主代理拆分、低并发（最多两个细任务）、独立核验和最终审核。没有用户明确要求，不创建 Agent Teams。
- 新对话先读本文件，再读 [`session-5697f1a7-handoff.md`](session-5697f1a7-handoff.md) §4.3.3 / §6 / §7；无需重读原会话。每阶段更新此检查点。
- 游戏/浏览器实测同一时刻仅一个引擎实例；构建与游戏不要争资源；清理自己开的窗口、标签、服务器和后台 job，不动用户浏览器/9222 chrome-mcp。
- 不吞 `r(0)`，不伪造异常状态；未测根因就说未知。真实验收仍要求对应提交的 CI 产物、独立现代宿主、pristine 游戏，丢弃一次热身后两次一致，含输入、pause/resume、save/load。
- 本会话 goal `goal-4c733db7-f628-4026-a0ef-4b5bd99509e1` revision 1：最后现场查询为 **phase active / activation disarmed**，resume 工具拒绝（需要直接 top-level human turn）。不能声称正在自动续跑；新对话查询自己的 goal，用户明确继续后按工具要求恢复，不复制旧 id。

## 2. 已提交的源码与版本

- 工作区 `/d/online_gal`（Git Bash，pwsh 工具实际 bash），分支 `main`。
- 恢复入口文档提交 **65beaca** 已推送：根 AGENTS、历史交接、滚动检查点、恢复指南导航。
- 引擎修复提交 **77a76457bed7595c11a2320184cec8c538619ebb**（77a7645）已推送。
- 唯一生产逻辑改动：`ports/artemis/src/script/lua_tags_builtin.cpp:20654`，603 后的 latch 门由 `!= kResumeRunning` 改为 `== kResumeRunning`。正式官方参考 `w2c_artemis_f3633` 的 `artemis.wasm2c.c:1748101-1748119` 在 g3==0 的 else 臂才读/清 852316 并取反 p2。非运行态 p2 自赋无额外行为，因此不造三元/新框架。
- 新 portable 回归：`test/artemis-builtin-resume-gate.test.mjs`，128 行 / 4 tests，读实际源码，唯一定位 602/603/604、精确绑定 latch 小块，对 phase×p2×latch 18 组及内存字符串 != 变异验证。仅 source/synthetic，不是游戏验收。
- runtime-artemis **0.1.8**、engines **0.1.9**。没有修改其他生产逻辑、冻结鼠标映射或历史 release hash，没有提交游戏、参考输入或新 SDK 文件。
- 最后现场工作树仅本检查点滚动修改；生产源码已冻结在77a7645。收尾会单独提交检查点，后续文档提交不改变引擎产物的来源 SHA。

## 3. 实测结论与当前卡点

### 已确证的边界

1. **603 源码缺陷真实存在，已修，但不是本次崩溃的完整解法。** 新/旧桥在同一个逐字节一致的宿主下，串行全新 origin/savepath 冷跑都仍 `r(0)` 失败。
2. 新77a7645桥，未做观察变换的下载宿主，pristine game-chs：load **865ms** 成功；settle 后 **2696ms** 发生 `ArtemisEhFailure: cxa_begin_catch: the catch record at 0 carries no base pointer`。画面 nonBlack=0、GL clear=1/draw=0、audio context=0；58 GET / 62 HEAD / 760225 B。
3. 新桥 unmodified cold01 capture：`.test/artemis-resume-newbridge-cold01-final-capture.json`，547269 B，run `dff5663129b25`，dropped_console=0。Probe 大单行截断，必须用同 at 的 **7/7 HARNESS-PROBE-CHUNK** 重组，不能用2048字符单行。证据脚本/结果为 `.test/artemis-resume-newbridge-cold01-evidence.{mjs,json}`（Flash解析，主代理已实读）。脚本手填的身份字段不替代现场 hash/CI 回执；请求采集器 HEAD 状态也不能直接推断网络错误根因。
4. 观察型 stackonly 冷跑：新桥 + 同下载宿主 + pristine game-chs，捕获**首次异常查找前**的原生栈：`c()` / seq1302，last=0、uncaught=0、caught=[]、temp=4；boot import 相位为0。原生栈：**5962:0x15d9559 ← BuiltinDispatch func873:0x3a4f35 ←3761/4145/3783/3782/258…**。随后 c返回0、temp=0、i(0)重抛，最终r(0)失败。
5. stackonly capture：`.test/artemis-resume-stackonly-cold01-final-capture.json`，604124 B，run `df0d9f41a9d17`，console dropped18。关键 **42个 EH-OBS-CHUNK 必须完整核验**；目前工具已直接看到20–25块含首次c栈，Flash正在完整重组与源码映射。console丢弃不等于EH事件未执行。
6. 两次新桥运行（unmodified与观察变换）都失败；它们是诊断/首跑，不是完成“丢弃热身+两次一致”的健康验收。没有证据支持把异常归因于某个宿主模块、404、内存 arena 或完整某个函数。

### 未解决

- 精确定位首次c对应哪个源码 merge/guard、为何没有真实throw却进入该分支，以及最早的控制流分歧。
- 最新一次性 Flash **714f1291-234a-4d16-b71b-453452f47a69** 正在完成 firstc 映射；只允许写 `.test/artemis-resume-firstc-map.{mjs,json,md}`。不能写生产、构建桥或开浏览器。新对话不能假定可继续向旧id发消息；先检查文件/任务状态，避免重复写。
- 当前只读 FinishTableIndex/f643 本体差分未发现直接差异；调用方未覆盖，不是“已排除”。
- 官方 WASM 同宿主健康对照50秒是**历史记录**，本轮未重跑；需要针对已定位分歧再串行对照。

## 4. CI 与下载产物对应关系

| 项 | 现场核验 |
| --- | --- |
| 引擎源码 SHA | 77a76457bed7595c11a2320184cec8c538619ebb |
| bridge workflow | [37180317084](https://github.com/fylcr/online_gal/actions/runs/37180317084)，success；real-game/publish skipped，不是验收 |
| packages workflow | [37180316882](https://github.com/fylcr/online_gal/actions/runs/37180316882)，success |
| bridge artifact | id11295212540，ZIP digest `b34e15f5ce7d0fe07c4948620ee705f29a017d12f075fcda756121752dc11108`，API head_sha一致 |
| packages artifact | id11294139547，ZIP digest `d2d52f53e4dd9a3eefbd6340ee2621c903955f44c179a8d7793eaf28a35fa01a`，API head_sha一致 |
| 下载bridge | `.test/github-77a7645/artemis-wasm/dist/artemis-bridge/build/bridge.wasm`，**23667751B**，sha256 **57944740c1d09468c199efa6f1ec22c5b0a5fc5698f4a8e6409c202ac687c0d4**，manifest/磁盘一致 |
| 下载runtime | `.test/github-77a7645/packages/online-gal-runtime-artemis-0.1.8.tgz`，sha256 `f348d47263386994b693b2acda1c06fbf2bed8dc59788cf8bccf680115081e53` |
| 解包host | `.test/github-77a7645/packages-artemis-extracted/package`，含vendored闭包，不是workspace symlink |
| 旧d2b1ad0桥 | `.test/github-d2b1ad0/artemis-wasm/dist/artemis-bridge/build/bridge.wasm`，23667750B，sha256 `f6e87c2301c73284d604b3ba5e023b277d32367a501611e80738f2f5051433e8` |
| pristine game | `.test/artemis-official-async/game-chs`，system.ini 3618B sha256 `de3e548de1b5115d815a2ea4ce421f3076e6e1393f22039bead2b0bdbadf20f7`，现场再次核验未改 |

新/旧 CI host eh.js/boot.js `cmp` **逐字节一致**；hash分别 `ca0d728e3093d11a2a4d2a265797f95bff65a012a4cae1e785e4f8fc680e173a` / `6a2d3b44bc24f3b29f31713e92ab67d7b9728c11b5208cb5ff914369f66d4631`。因此新桥实验没有同时偷换EH宿主。

## 5. 已跑质量门（不替代游戏）

- `build:packages`通过；最终 portable **337/337、0skip**（基线333+新test4），日志 `.test/artemis-resume-final-portable.log`。
- 本地完整 `npm test` 基线439 tests / 434 pass / 5skip；既有5skip不代表已实现，日志 `.test/artemis-resume-local-tests.log`。
- `npm run check`通过，Astro9files / 0error / 0warning / 0hint，日志 `.test/artemis-resume-check.log`。
- worktree/index source/layout检查通过，340 mapped functions / 340 products；许可warning保留。
- 下载新桥独立ABI核验 `verify-imports-map.mjs`通过，207函数+1memory、198 distinct；日志 `.test/artemis-resume-77-abi.log`。台账历史module hash只report，**不得改写历史hash**。
- 全树 `style-gate.sh` **exit1**：既有未改动 compositor_effect等 `w2c_` 调用指纹/裸常量，不假报全绿、不扩大本轮风格返工。

## 6. 只使用接受的诊断入口

- `.test/artemis-independent-host-server.mjs` / entry：未改下载包的独立宿主验收入口。
- `.test/artemis-resume-stackonly-{server,plugin,browser}.mjs`：在**已工作**的原EH观察3files上只新增preCallStack/firstSearch，不monkeypatchWebAssembly；12anchor fullharness实际build通过，主代理已跑并出证据。只用于定位，不作出货验收。
- **不要运行** `.test/artemis-resume-exception-*`、`.test/artemis-resume-precall-*`、`.test/artemis-resume-finalpre-*`：多版虽曾selfcheckPASS，主代理审核发现Promise误认、表/函数编号混淆、未装内存/状态、catch作用域/缺import、fixture假验证或残留语法错，均已拒绝；部分任务failed，没有可靠最终版本。
- 原生 `wasm-function[N]` 包含import，**不能减207得到官方fNNN**；桥func873 = mangled BuiltinDispatch = 官方f3633。旧桥disasm `0x3a4f34` 是call5962，新桥实测0x3a4f35；新旧偏移必须字节核对，不能拿旧disasm当新动态事实。
- 852316/852320是guest throw latch/value，host.temp是另一个共享单元，g3是相位，resume_point是恢复站点，四者不能混同。

## 7. 清理状态与下一步

- 所有本轮browser_* sessions **xwoy/vzop/oiuv/freg 已明确stop成功**；browser_session list最后为无活动session。
- 三个诊断服务器 jobs **pwsh-618 / pwsh-652 / pwsh-699 已kill并收集**。50065端口清理后无HTTP响应（curl exit28，不能由此猜进程/防火墙根因）。
- 自有BSK foreground daemon job **pwsh-254** 通过`bsk daemon stop`停止，已收集exit1；独立 `BSK_AUTO_START=0 bsk status --json`报告daemon不运行。这是已请求停止后的回执，不是引擎测试失败。下次按工具建议持久job foreground启动，用同用户/BSK_HOME，不尝试Job Object detached启动；页面仅browser_*。
- 基线、commit/push、CIwatch、下载jobs都已收集，无仍相关后台job。仅firstc-map Flash细任务待收口，不能与新写任务重复。

下一步顺序：

1. `pwd; git status --short; git log -3 --oneline`核验现场，读本文件。
2. 读/重跑firstc-map解析，assert **42/42 EH chunks**，核对firstSearch/preCallStack原始记录、新桥具体call偏移和源码位置；主代理独立审核，不接受假fNNN映射。
3. 从该源码站点读前置门/merge及正式官方f3633对应段，必要时在查找前的guest站点加窄观测；同宿主官方对照和重复冷跑验证，测到分歧才定根因。
4. 细活仍Flash窄范围写，主代理审查、合成回归/质量门、显式pathspec提交push、下载同提交产物再真游戏验证。禁止绕过r0。
5. 游戏能越过此点后再审GL/audio/browser-services与真实菜单存取；当前组件已绿的不盲目重修，仍不能把它们当可玩证据。
