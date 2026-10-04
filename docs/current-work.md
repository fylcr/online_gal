# 当前工作检查点 — 新对话先读

> 更新：2026-10-04 16:05 +08:00。用户已再次“继续”，goal armed。Artemis 仍不可用；真实 first-c 已由已有源码站点诊断定位 **22515 / phase0 / resume602 / latch0**。主代理核验两额外 if-open 导致作用域错位，正在**只编译下载完整 TU 的四行候选副本**，生产源码仍冻结。不要把静态缺陷、构建成功或官方诊断读成游戏验收。

## 1. 用户要求与恢复纪律

- 具体代码/细节由 **magpie/group/auto-deepseek-v4-1-flash** 承担；主代理拆分、低并发（最多两个细任务）、独立核验/审查/最终验收。未经明确请求不建 Agent Teams。
- 新对话先读本文件，再读 [`session-5697f1a7-handoff.md`](session-5697f1a7-handoff.md) §4.3.3/§6/§7；每阶段维护检查点，不依赖旧会话。
- 同时只有一个引擎实例；构建与游戏不要争资源。收尾关闭自有窗口/标签/服务器/jobs，不动用户浏览器和9222 chrome-mcp。
- 未测根因就说未知；不吞r(0)、不改冻结鼠标映射/历史制品hash。真实验收=同提交CI产物+独立现代宿主+pristine游戏，丢弃热身后两次一致，含输入、pause/resume、save/load。
- 用户约15:16明确继续，本会话goal `goal-4c733db7-f628-4026-a0ef-4b5bd99509e1` **revision2 active/armed**。新对话查询自己的goal，不复制旧id。

## 2. 仓库、已交付修复与冻结边界

- `/d/online_gal`，Git Bash（pwsh工具实际bash），`main`。
- 引擎源码最后修复 **77a76457bed7595c11a2320184cec8c538619ebb**：仅 `lua_tags_builtin.cpp:20654` 的603后latch门 `!=running → ==running`，正式官方 `artemis.wasm2c.c:1748101-1748119` 在g3==0才做latch操作；非运行态p2自赋无额外行为。
- tracked source/synthetic回归 `test/artemis-builtin-resume-gate.test.mjs`：4tests /18 tuples /旧!=内存变异检查，不是游戏验收。runtime-artemis0.1.8、engines0.1.9。
- 引擎修复已push；恢复文档最后提交 **d9195b0**已push，本阶段开始工作树干净。现在仅本检查点滚动修改，**生产packages/ports/test无新增diff**。
- 603缺陷已修但**同提交下载桥仍r(0)失败**，不是完整崩溃解法。SDK/转译余留再分发风险独立未解决，没有新提交参考/游戏/SDK。

## 3. 已实测的故障与first-c定位

### 77a7645未观察变换CI桥

- 新origin/savepath独立冷跑：load **865ms**；settle后 **2696ms** `ArtemisEhFailure: cxa_begin_catch: the catch record at 0 carries no base pointer`；nonBlack0、GLclear1/draw0、audio0；58GET/62HEAD/760225B。
- capture `.test/artemis-resume-newbridge-cold01-final-capture.json`，547269B，run `dff5663129b25`，dropped_console0。Probe必须用 **7/7同at chunks**（单行2048字符截断不能用）。Flash evidence脚本/result `.test/artemis-resume-newbridge-cold01-evidence.{mjs,json}`已实读，身份手填字段不替代hash/CI回执。

### 新CI桥stackonly前置栈

- 原已工作EH观察3files仅+13行 preCallStack，capture `.test/artemis-resume-stackonly-cold01-final-capture.json`，604124B，run `df0d9f41a9d17`，主代理原reader独立 **42/42 chunks**完整，console dropped18/observer ringDropped1092。
- 首 `eh.enter c` seq1302（import.enter1301），last=0/uncaught=0/caught=[]/temp4/phase0。原生 **func5962:0x15d9559 ← BuiltinDispatch func873:0x3a4f35**。精确新wasm字节 **10 ca ae 80 80 00** 解为call5962，再thin5962→import102=a.c。之后c返0→i(0)→r(0)。
- firstc-map `.test/artemis-resume-firstc-map.{mjs,json,md}`接受chunk/原生符号/真实call事实，不接受“首defined208/objdump1-based”“寄存器重装必是重播”推论。索引0-based，首defined207；raw-207仅可索code body，不能当官方fNNN。

### 已有源码站点诊断（旧d2b1ad0输入，只作定位）

- 复用 `.test/artemis-builtin-csite-observation01/bridge.wasm`，hash **e3261e54f16fbaaadd780cd72260188120286b21c20eb8bd7bb41c7bd06d9b46**，map origin为旧d2TU，hash **1dff07566bb0a29cd0be8efbb0da090134d0eb947036512326536686c3566751**已重算匹配，ABI通过。
- capture `.test/artemis-resume-existing-csite-cold01-final-capture.json`，597327B，run `d5091b5635a5f`，主代理原reader **38/38 chunks**完整（log `.test/artemis-resume-existing-csite-lead-read.log`）。计数器内`sourceLine`是注入字面原产品行号，**不是插入后TU行号**。
- firstC **[22515,0,602,6110372,0,0,6098896,11455776,1701995379,0,0,0,0]**，fields=line/phase/resumePoint/p0/p1/p2/frame/result/node/value/flag/ok/latch。
- preEvents：33188×4 →20606 →20619(r578) →20627(r602) →20632(p1=1,p2=0) →20640(p1=0) →20661 →20670(p2=0) →22515。没有经过21828，也没有22493分支见证。
- `22515 HostRet0`发生在20670外层块结束后的正常跌落，不是“22493错落到22494”；后者是Flash错误推论。旧诊断源有旧603!=，所以**不能当77a7645的source-line动态证明**，但结构对应仍可核验。

## 4. 主代理已核验的作用域差分与候选

已实读正式官方段+既有comment/string-aware scanner，不按缩进、不用调用多重集定罪。

| 真实门/语句 | 原产品配对 | 官方对应/应有闭合（保留原行号） |
| --- | --- | --- |
| 20670 p2门 | closes22513 | official1748142→1749242 / 应21146 |
| 20781 format链门（真门，不能删） | closes21146 | official1748465→1748724 / 应20892 |
| 20791 running比较/初始化 | closes20892 | official1748497→1748600 / 应20836 |
| 21828超长字符串门 | closes22492 | official1750791→1750836 / 应21851 |
| 24477 collector outer | closes24834 | official1756791→1757629 / 应24835 |
| 24773 running719清理/loop tail | closes24827 | official1757511→1757628 / 应24834 |

额外20824running-open和21829duplicate-open造成闭合链错位；24827running-close过早；B2011后31185/31186两close，官方1771445(B2011end)+1771446只有一outerclose。

COPY candidate仅**blank原产品20824 /21829 /24827 /31185**，保留行数；保留真20781和24835。主代理IN-MEMORY4blank测全局balanced、闭合20791→20836、20781→20892、20670→21146、20632→22492、19604→22513、24773→24834、24477→24835。**静态差分真实；是否解决崩溃必须受控运行证实**。

## 5. 当前本地实验（生产0diff）

- 输入完整下载TU `.test/github-77a7645/artemis-wasm/dist/artemis-bridge/build/artemis.cpp`，**98,937,232B sha849098ce06a417c8f220c54b6e18ce80958bb4d8c21e819d1807c2258660c418**。
- product1321222B sha911669ded12a066c1cf1f713d6b38f8e9df5ae31dc96ea9c65addf56ec8a401b，marker唯一 `// ---- rewrite: f3633 (script/lua_tags_builtin.cpp) ----`之后完整字节==product。
- COPY完整TU `.test/artemis-resume-fourline-build-new/artemis.cpp`，**98,937,090B sha4eb85eb512c97fdd2908e3d66c7f8c07ee72608c7cb6c3855418d6adbddac749**。主代理独立字节assert prefix+4blankproduct+tail完整，只有4行差异/−142B；不是1.3MBbareproduct。
- **当前编译job pwsh-1085 活动**：`bash .test/artemis-resume-fourline-build-new/build.sh`，log在同root/build.log；源码已compile产生历史warning，无error（16:03检查），还未出bridge。下载runtime .o、FWD、原exportshims、-O1/192MiB/globalbase184549376/stack2MiB，fold/memory/table同原。
- 注意**不要运行旧 `.test/artemis-resume-fourline-build.sh` 顶层错误配方**（SRC仍bareproduct）。真正已审运行配方是build-new/build.sh。新root首次build目标此前不存在；不重复执行跑第二份。
- `prepare.mjs`/metadata作来源证据；worker越界写keys/newrootrecipe、删除错误barecopy已报告，生产/输入TUhash没有改变。不授权继续任意删除；所有源码更改先读审。
- baseline配方Flash **f8140298-9976-4c59-b384-7998b537b301** 准备 `.test/artemis-resume-fourline-baseline-build.sh/plan.md`，仅写不build，用相同工具链/flags/原未改TU消除CIvs本机构建混杂；待审核。只能等candidate结束后再串行构建。
- portable草稿Flash **8515ddfb-efc6-4ec9-b9c0-aec9b9f55e71** 仅 `.test/artemis-resume-scope-regression-draft.mjs`，旧源应fail/COPY4blank应pass，按uniqueanchor+scopeownership，不修改production；待报告。
- 其它本阶段Flash已结束/failed。拒绝e9df的SyncMemoryBase必占opcode空间、e302的经22494路径、0e7a的删20781门、af394的旧source“污染”故事；不要拿它们的自动断言数当发现。

## 6. 本轮同宿主官方对照已实跑

- 官方5998665B，sha7ceabed28b84e7689fc4b629fa232e2857f15c3b6e6fa3e09fc39dba082d145e；same77downloadhost+stackonlyobserver+pristinegame，新origin61557/savepathresume77-official-control-new01。
- capture `.test/artemis-resume-official-control-new01-final-capture.json`，724909B，run `d8797a32243d0`，主代理原reader **10/10 Probe chunks**完整；日志 `.test/artemis-resume-official-control-new01-lead-read.log`。
- load709ms/gameElapsed **83878ms**/hasFailure=false/EHbucket0；drawElements121507/audio34started/source109paths/24,864,733B。跨过桥2.7秒故障窗口，但canvasgridnonBlack0、未完整截图/input/save验收，**不是官方可玩/生产验收**。console dropped296，worker未采。
- xxbj/zjbz sessions已明确stop；source-site/server **pwsh-835**、官方server **pwsh-1041**都kill并收集。自有BSK foreground **pwsh-834** PID60668当前活动，无活动browser session；编译时不再跑游戏。
- capture整理Flash28726任务failed无交付，现直接用原接受reader提取；不能用其未完成脚本。

## 7. CI基线与质量门

- 77a7645 bridge run[37180317084](https://github.com/fylcr/online_gal/actions/runs/37180317084) / packages run[37180316882](https://github.com/fylcr/online_gal/actions/runs/37180316882)均success，real-game/publish skipped。
- 下载bridge `.test/github-77a7645/artemis-wasm/dist/artemis-bridge/build/bridge.wasm` **23667751B sha57944740c1d09468c199efa6f1ec22c5b0a5fc5698f4a8e6409c202ac687c0d4**，manifest/磁盘匹配。
- 下载host `.test/github-77a7645/packages-artemis-extracted/package`（vendored closure），与d2host eh/boot字节cmp一致；tgzsha f348d47263386994b693b2acda1c06fbf2bed8dc59788cf8bccf680115081e53。
- pristine game `.test/artemis-official-async/game-chs`，system.ini3618B hashde3e548de1b5115d815a2ea4ce421f3076e6e1393f22039bead2b0bdbadf20f7。
- 最终portable337/337、0skip，buildpackages/typecheck/Astro/source/layout通过；本地npmtest基线434pass/5skip。ABI独立verify-imports-map通过。全树stylegate既有exit1，不假报全绿。
- **不要运行**exception/precall/finalpre大探针（已拒/残留语法错），只接受原independent harness、原EHreader与stackonly/csite复用模式；不再造WASM解码框架。

## 8. 下一步（按顺序）

1. 收集 **pwsh-1085**（别复制运行），查完整build退出/产物、独立ABI/hash/原输入不变，构建失败先定位，不把失败叫PASS。
2. 如果candidate构建成功，启动单一independent host（可stackonly诊断）跑COPYbridge，先确认是否仍22515/c/r0；export后及时stop/kill。本地bridge只是诊断，不替代CI验收。
3. 同本机工具链baseline串行运行与candidate配对，丢弃热身+重复一致，核对scope修复是否实际消除原故障。测到前不说根因已定。
4. 有受控证据才委托Flash最小生产4行修复/portable回归/版本更新；主代理审diff、质量门、显式commit/push，同commit CI下载重复真实游戏、输入/pause/save验收。
5. 随阶段回写/提交本文件；新对话查询git/jobs/当前goal，不能假定旧代理id可用，当前仍活动的写任务先收口再重派。
