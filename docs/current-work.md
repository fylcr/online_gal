# 当前工作检查点 — 新对话先读

> 2026-10-04 18:05 +08:00。旧EH错误入口22515/r602→21115/r616→21127/r616/tag10逐层实测修复；discard热身后两次COPY冷跑EH0，资源/IDB继续后遇到raw allocator OOB，**仍不可玩**。生产与实測COPYembedded逐字节一致，仅7ins/6del。最终新回归9tests+旧4tests **13/13 PASS**、完整portable **346/346 0skip**、包构建/TS/Astro/layout/source通过；runtime0.1.9/engines0.1.10。正显式提交push并等待同commitCI下载验证。所有代理已settled、无浏览器/server，自有BSKdaemon仍活动。

## 1. 用户要求与接手纪律

- 具体代码/细节由 **magpie/group/auto-deepseek-v4-1-flash**；主代理负责拆分、低并发（最多两个窄任务）、独立核验和最终审核。不未经明确请求建Agent Teams。
- 新对话先读本文件，再读 `docs/session-5697f1a7-handoff.md` §4.3.3/§6/§7。每阶段提交并push恢复检查点，不能依赖旧会话。
- 工作区 `/d/online_gal`，main，shell Git Bash（pwsh工具实际bash）。生产代码只在packages/apps/ports，tracked portable tests在test；.test/.ref ignored。
- 同时只跑一个引擎实例。浏览器仅BrowserSkill browser_*，已验证Chromeinstance **aa5bb810**。不用CDP/evaluate后端，不碰用户9222 chrome-mcp或用户窗口。
- 不吞r(0)/不跳trap/不造异常状态/不扩大memory盖住OOB。构建成功、合成测试、真实游戏可玩、再分发授权不同；未测就未知。
- 验收=同sourceSHA的CI产物+独立现代宿主+pristine游戏，discardwarmup后两次一致并含输入/pause/resume/save/load。当前冷跑仅证明旧EH入口不再触发，**没有draw/audio/输入/菜单存取验收**。
- 官方SDK/其转译体有专有再分发风险；不提交SDK/游戏/诊断完整TU/制品。冻结鼠标映射和历史hash不动。
- 当前goal `goal-4c733db7-f628-4026-a0ef-4b5bd99509e1` revision2，active/armed，objective为Artemis主线直到同CI真实验收；下一对话查自己的goal，不复制id。

## 2. Git、生产与当前写入范围

- 最后**引擎**提交 **77a76457bed7595c11a2320184cec8c538619ebb**：603后latch相位 `!=running→==running`，runtime0.1.8/engines0.1.9；已push/CI绿，但真实r0仍失败。
- 最新恢复文档HEAD **7d8e8ae**已push。当前主代理修改AGENTS.md/docs/current-work.md；17:44 git未见packages/ports/test diff（以现场git为准）。
- **Flash043b50b4-8e18-4dc1-88b4-f2f6f0040ead 已完成** 独占 `ports/artemis/src/script/lua_tags_builtin.cpp` + `test/artemis-builtin-control-flow.test.mjs`。已授权落入**已独立COPY差分实跑验证**的四scope+616双门修复，未授权git/全build/浏览器/版本。必须独立审diff并比对其prod文件等于reviewedCOPY embedded product，不能重复派相同写范围。
- **Flashb18cfca5-ce5b-4940-95d2-2af2bac2b604 failed、无配方文件**，不重复长任务；下一阶段OOB继续窄准备。30fc超小prep虽生成，但**写错newroot.test/writeonly.test非ignored路径，getter竟void且写counter而非读**，主代理拒绝不build，已核精确3files后搬至 `.test/artemis-rejected-oob-newroot` / `.test/artemis-rejected-oob-writeonly` 隔离，不将假探针commit；下阶段必须getter u32只读。原scope 独占 `.test/artemis-resume-oob-{prepare.mjs,plugin.mjs,server.mjs,build.sh}` + newrootTU/probe-map。基于immutable reviewedpairedTU仅在MEMCHECK失败前highbridgecounter记录addr/width/limit，随后仍原TRAP(OOB)。准备，不执行build/browser/server/git。没有guest地址非法读/写、没有修复值。未完成即failed，无文件；下一阶段从immutablepairedinput窄准备，别使用不存在配方。
- 所有其他本轮Flash已settled。oneshot subagent ids **send_message不可用**（active teammate not found），不能假装已纠正正在跑的代理。

## 3. 原故障、CI输入与真实首c证据

### 同提交77a7645下载桥

- bridge23667751B，sha **57944740c1d09468c199efa6f1ec22c5b0a5fc5698f4a8e6409c202ac687c0d4**。
- 下载完整TU98937232B，sha **849098ce06a417c8f220c54b6e18ce80958bb4d8c21e819d1807c2258660c418**，路径 `.test/github-77a7645/artemis-wasm/dist/artemis-bridge/build/artemis.cpp`。
- 手写产品1321222B，sha **911669ded12a066c1cf1f713d6b38f8e9df5ae31dc96ea9c65addf56ec8a401b**，marker `// ---- rewrite: f3633 (script/lua_tags_builtin.cpp) ----`唯一、其后完整字节==product。
- Host `.test/github-77a7645/packages-artemis-extracted/package` vendored closure；tgzsha f348d47263386994b693b2acda1c06fbf2bed8dc59788cf8bccf680115081e53。游戏 `.test/artemis-official-async/game-chs`，system.ini3618B/shade3e548de1b5115d815a2ea4ce421f3076e6e1393f22039bead2b0bdbadf20f7。
- 原冷跑load865ms、settle后2696ms r0 failure。GET58/HEAD62/760225B、clear1/draw0/audio0。
- `.test/artemis-resume-newbridge-cold01-final-capture.json`547269B，run dff5663129b25，7/7Probechunks；不能用截断单行Probe。

### 新桥stackonly与旧source-site定位

- 新bridge前置栈capture `.test/artemis-resume-stackonly-cold01-final-capture.json`604124B，run df0d9f41a9d17，原reader42/42EHchunks：firstcseq1302、last0/uncaught0/caught[]/temp4/phase0。
- 原生 **5962:0x15d9559←BuiltinDispatch873:0x3a4f35**；该精确新binary字节 **10 ca ae 80 80 00**=call5962，thin5962→import102=a.c。rawindices0based包括207imports，raw−207只bodyordinal，**不是officialfNNN**。
- 复用旧d2诊断 `.test/artemis-builtin-csite-observation01/bridge.wasm` sha e3261e54f16fbaaadd780cd72260188120286b21c20eb8bd7bb41c7bd06d9b46；不是新77验收源，旧603!=不能混当current。
- `.test/artemis-resume-existing-csite-cold01-final-capture.json`597327B，run d5091b5635a5f，38/38EHchunks：firstC **22515/phase0/r602/p1=0/p2=0/latch0**。
- 实际路径33188×4→20606→20619→20627→20632(p1=1)→20640(p1=0)→20661→20670(p2=0)→22515；false20670跳过整scope到22513，**没经过22493/22494**。sourceLine是counter注入字面原产品编号，不是插入后TU行号。

## 4. 已独立核验的作用域与616分支缺陷

comment/string-aware既有scanner + 正式官方文本核验，不靠缩进/调用多重集。

| 产品门 | 原close | 正式官方open→close / 应有产品close |
| --- | --- | --- |
| 20670 p2门 | 22513 | 1748142→1749242 / 21146 |
| 20781链门（真门不能删） | 21146 | 1748465→1748724 / 20892 |
| 20791running比较初始化 | 20892 | 1748497→1748600 / 20836 |
| 21828字符串门 | 22492 | 1750791→1750836 / 21851 |
| 24477outer | 24834 | 1756791→1757629 / 24835 |
| 24773running719尾 | 24827 | 1757511→1757628 / 24834 |

- COPY仅blank原产品 **20824/21829/24827/31185**；保留20781/24835。全文件原/改均balanced，但归属不同，四blank修回20632→22492/19604→22513等。完整TU4eb85eb512c97fdd2908e3d66c7f8c07ee72608c7cb6c3855418d6adbddac749，主代理完整prefix+candidate+tail bytes assert，原下载/产品不变。
- COPY4blankplainbridge sha64fa07b69d137ce70a9d1bc7bff0d598e7b1f73dfbd3a544eeeb8351628dcce6，构建/ABI通过，但真实仍r0（不能说四blank完整解法）。
- 4blank source-site诊断bridge shaffefbd885d1260c90837a789138863ef08d8e691a0928750f8a7736a8ed7868a；capture `.test/artemis-resume-candidate-csite-cold01-final-capture.json`596372B / run d82890016e4b2，原reader **37/37EHchunks**：firstC **21115/phase0/r616/p1=6099136/p2=0/latch0**。已越过原22515/r602入口。
- 616产品21178 `pending!=1→B3721`把**正常无throw**接进EH。正式1749309..1749360：正常pending!=1进入field数据，pending1 skipouter1749328→1749907再1749910gotoB4。
- 先只flip21178!=→== onebyte，plainbridge sha9d42782b4cd1fa33b6617192b685f6d760dd9da58f31913de07450b499ab44da，实跑仍r0。加只读source计数后的bridge sha42768813ec180aaeb961e22f5ac2a7bdaed54ec9cdde2708c867fd9258abedc9；capture `.test/artemis-resume-gate616-csite-cold01-final-capture.json`594424B / run de102fdbea6b9，**37/37EHchunks**：firstC **21127/phase0/r616/p1=0/p2=0/value10/latch0**。
- 产品21189tagfalse(10!=8)→B3720也是错EH。正式1749360false只skip617inner至1749415，再正常filenamecleanup1749418，不goto前fieldcatch。
- 最终reviewedpaired差分=四blank +616pending1→B4（不是B3721）+tagfalse→新`p04_B616_lookup_done`+标签在filenamecleanup前。617调用与pendingB3不变，无p2zeroEHguard、无直接B11 bypass。
- reviewed完整TU **98937137B shae94b2b6fd901f46d049c8d9d56173ae9c66456f3ea87b270dab88f4e2e32db4f**，路径 `.test/artemis-resume-gate616-paired-build-reviewed/artemis.cpp`；主代理完整三个unique替换字节assert，不只首尾4096比较。
- reviewedplainbridge **23666735B sha7b45bced53db1f1811c85e283b68aabea459a4e6d945f7d7b7d9a60aa5bb1188**，构建exit0、独立207import ABI ALLPASS。

## 5. discardwarmup + 两次一致的真实COPY差分

同下载CIhost、pristine游戏、每次新origin/savepath，单实例串行，无观察host变换：

| 跑次 | run / capture | load | EH调用 | 资源 | 新故障 |
| --- | --- | --- | --- | --- | --- |
| cold01热身 | d32e66b995bcc / paired-cold01-final-capture.json567850B | 1126ms | 0 | GET60HEAD65 /1013918B /IDB114 | allocator OOB |
| cold02量测 | db2065ef67d5b / paired-cold02-final-capture.json567765B | 894ms | 0 | 同上 | 同native10frames |
| cold03量测 | d92701a82db11 / paired-cold03-final-capture.json567808B | 800ms | 0 | 同上 | 同native10frames |

- capture全名都 `.test/artemis-resume-gate616-paired-<coldXX>-final-capture.json`，**各7/7Probechunks完整/no droppedconsole**；原reader logs同前缀lead-read.log。主代理三probe字段独立重拼比对，EH0/GLclear3draw0/audio0/storewrite1一致。
- 消除了这条真实路径的r0入口并进入后续资源/IDB流程；**仍unavailable**，不能叫boot可玩/save功能验收，也不能说内存损坏根因已定。

## 6. 新trap的已测身份与未知

- 原生stack **6149:0x15db0d1→6203:0x15db476→6125:0x15da986→212:0x42a6b→213:0x44894→4324:0x10f806b→885:0x48981c→883:0x484eb4→3673:0xdc6610→4145:0x106bd6e**。三次完全同。
- 主代理用existing正确decodeImportSection核207imports、section10body skip验证10offset全IN。exports直接按raw索引查：6149 `_abort_js` /6203 `abort` /6125 `wasm_rt_trap` /213 `artc::AllocateStringBuffer` /883 `artc::BacklogView::DispatchTag`。
- 212 exactcallbyte `41 01 10 ed af 80 80 00`=const1→call6125；runtime wasm-rt.h enum1=**OOB**。`AllocateRawBlock` engine_context.h:90→generated`w2c_artemis_Nc_0`，当前候选TU117516起原rawmalloc，不是BuiltinDispatch的explicittrap。
- **未知：**实际越界addr/width、allocator metadata被谁污染、哪个原语句的memorycheck、是否更早内存写/恢复frame错误。下一轮用窄失败前counter测address，不去扩memory/放宽检查。
- Flashtrap-map `.test/artemis-resume-paired-trap-map.{mjs,json}` 直接export-name事实已独立核验；其“official=raw+imports”“runtimewrapper就是rootcause”等**拒绝**。raw本身已含imports；anonymous不可猜officialfNNN。

## 7. 本轮其它控制、拒绝配方、quality边界

- 官方samehost84秒对照：`.test/artemis-resume-official-control-new01-final-capture.json`724909B /run d8797a32243d0、10/10Probechunks，load709ms/EH0/drawElements121507/audio34started/source24864733B。gridnonBlack0/无完整input/save验收，**不算官方可玩**。
- same本机未改TUbaselinebridge23667751B sha7b46b1a24f05a9c3954598e85ea71af37089cb2fe990f283d9d732feddc0f578（与CI同size但不同bytes；sections6/10/11diff，不能说只custom）。真实baseline load837ms/r0同症状/GET58HEAD62B760225，capture `.test/artemis-resume-localbaseline-cold01-final-capture.json`547267B /run d739ebbc35a5a、7/7Probechunks。
- 77aCI bridge[37180317084](https://github.com/fylcr/online_gal/actions/runs/37180317084) /packages[37180316882](https://github.com/fylcr/online_gal/actions/runs/37180316882)success，real-game/publish skipped；不能当后续sourceSHA验收。
- 历史portable337/337、local434pass5skip、build/typecheck/Astro/source/layout通过；本阶段新增trackedtests尚未完成。全树stylegate既有exit1，不能假报全绿。
- 拒绝：e302经22494故事；0e7a删真实20781门；af394旧d2“污染”故事；e9df缺SyncMemoryBaseopcode即可断不可达；7ae声称官方无tag8/无c+a.b/无latchclear（正式原文均有）。不要引用它们报告里的PASS数字。
- 拒绝旧fourline顶层build.sh（bareproduct当完整TU），只build-new/build.sh；旧paired-build-new含擅加p2zeroEHguard，**只能用paired-build-reviewed**。
- 单门observerprep68a曾eachbyte读99MB源，主代理精确cmd/PID36944停止，failed收口后仅缓存输入/wholeTUcount两处review修正，当前诊断已compile/run。不再运行原runaway版本。043b生产已完成并独立审完，7a6b收紧617window、主代理补14th anti-EHshortcut与内存变异，最终13tests通过；不用旧toy21PASS草稿（tag8错618/无真实phase2）。

## 8. 资源与下一步

- 所有本轮game sessions **zjbz/hnqp/iwyj/upgk/louh/tcbm/saio/tkjf/xara已stop**，服务器jobs1041/1129/1262/1320/1324/1360/1402/1476/1488已kill并收集。所有COPY编译1085/1140/1231/1314/1341/1366已完成收集。
- **BSK foreground daemon pwsh-834 PID60668仍活动**（owned，只为后续诊断；收尾用bsk daemon stop再collect，不碰其它服务）。browser_session list无活动。
- 所有代理已settled，生产/测试/版本三范围已独立审。oneshot ids不能send_message，别拿旧失败probe当完成品。
- 最终qualityjobs **pwsh1635/pwsh1644/pwsh1666**已exit0并收集，346portable0skip，TS/Astro0error，layout/source通过；没有全树style假绿。
- 下一步1：显式pathspec commit/push最小source+test+versions+恢复docs，观察**同commit**bridge/packagesCI，下载artifact解包身份核验/ABI/真实fresh量測，不能拿77旧artifact代替。
- 下一步2：未接受任何OOBprobe制品；下轮先读quarantinedprep，修成只读u32getter和正确.test路径，再最小三literaltransform/失败前counter测addr/width/limit；不改变TRAP，不扩大guestmemory。
- 下一步3：新allocator坏addr按源/官方逐层查，输入/pause/save完整验收仍未到达。维护本文件与AGENTS，不宣布goal完成/blocked（仍有可推进工作）。
