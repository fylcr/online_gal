# R1 — tween 执行核心完整替换（源码交付）

2026-10-11。累计重写集合 **{R1 tween 执行核心}**；用户反馈 **待用户测试**。
未构建、未测试，不判已解决，不开始缩减。

## 替换与来源

正式实现位于 `ports/artemis/compat-web/overlay/render/`：
- `tween_scheduler.h` / `.cpp`：独立手写状态所有者，创建解析、collection、property FIFO、首tick启动、插值、循环/yoyo/loopdelay、完成、取消、remaining与生命周期。
- `compositor.h` / `.cpp`：整文件覆盖；职责外代码保留自 GPL artemis-compat pin `17200e64ea99681e3c27d694433c0ff78d7a7cee`，包含既有0002数值解析和0005 text clock行为。原基线来自本仓库 `out/tween-arm-trial/build-src/src/render/`，与pin的静态diff仅对应0002/0003/0005/0006，无trans-suspend。
- 新核心不调用旧QueueTween、不含旧Tween/tweens_/tween_set_/collecting_tweens_。旧0003循环delay与0006初值职责已由新owner完整承担；0005保留在非tween文本推进；0002职责外数值解析保留。patch list及各patch原字节不改。
- `build.sh:197-204` 在应用原platform patches后复制正式render overlay及同名Compositor头；`web.cmake:54-74` 从Web源列表及已声明的artemis_core SOURCES移除pin compositor.cpp，追加replacement compositor.cpp/tween_scheduler.cpp。没有0007 patch，没有运行fallback；默认build选用R1。

## 官方语义映射与兼容合同

参考是ignored `.ref/artemis-official/wasm/asyncify/artemis.wasm.pseudo.c`，必要select表达式以同目录wasm2c核对，未复制Asyncify、heap地址或伪C正文到GPL。

| 官方依据 | 查明语义 / R1表达 |
| --- | --- |
| func329:12033 | 秒/纳秒换为毫秒；继续消费host/Lua传入double毫秒，不动host时钟/ABI |
| func3654:686586、func3653（同文件前一函数） | collection单一临时owner，重复begin不清空；提交将owner交给活动调度并注册目标；R1 Begin/End。保留compat每个layer/property串行、不同property并行公开合同；官方单group总FIFO不是等价声明 |
| func3656:687549及其完整创建/注册/后续completion分支 | setter绑定、from/to、duration/delay、loop优先于yoyo、loopdelay、collection中延后注册，collection外立即setter(from)。R1 Add/Register，不用旧QueueTween补偿 |
| func3443:578231、wasm2c:1577705-1578032 | head child Update返回完成后销毁并同now继续next；R1 deque消费，不预排绝对end，长帧不会把未启动后段直接快进完 |
| func4391:915371、wasm2c:2423256起 | 首次Update建立start=now+delay/end=start+duration；now<=start输出from且不完成；零duration首tick仍保留；完成输出to并标done；loops/yoyo仅边界消耗一次，短过冲以旧end续接，长停顿以now续接，再加loopdelay；R1 Update。curve独立数学表达，最终setter为float |
| func3441/3442:578041/578136、func1433、func4390/4392、func1595:238680 | group finish/cancel逐child执行并销毁，普通析构释放callback/setter；done后不再次推进。R1 Finish跳终值并移除，Remove/Clear仅销毁无回写；删除subtree同时丢collected，不会end后复活。rename跟随活动及待提交目标 |
| compat PendingAnimationMs + lua_tags_nav.cpp:69/103/196 | 最大活动track剩余时间，串行尾部求和、独立track取max，无限循环返回infinity；仍与原transition remaining取max；动画wait改为每次IsWaiting复核owner；普通timed wait及trans后专用wait不改 |

### 保留入口/生命周期

`lua_tags_layer.cpp:71-93` TagTweenSet/TagTweenSetEnd/TagLyTween/TagLyTweenDel → 原签名Compositor → 新scheduler。Lua tag dispatch注册与普通e:tag入口不改。`Compositor::Update`由Web Tick/frame以及既有host调用 → 新scheduler；`PendingAnimationMs`的所有Lua/host消费者直接读同一owner。`DeleteLayer`两GL/headless实现 → Remove；`RenameLayer`两实现 → Rename；`ReleaseGl`两实现及Shutdown → Clear。revision继续在Compositor层更新。属性读写仍用原ReadParam/ApplyParam，单位alpha255、scale百分比等不变。

### 已知但未覆盖，不冒充官方完整动画等价

父代理已明确批准：只替换compat现有tween执行核心，**不发明VM callback ABI**。
- 官方func3656解析sync，并将唤醒/续跑functor或scenario/file派发functor绑定到child completion；func4391完成先写终值/标done，再调用非空completion；func4390强制完成也可通知；func4392释放绑定functor。compat AddTween现有实现没有等价completion桥，Lua现改为owner活动状态复核的动画wait，Remaining只作reason展示/诊断估算。R1保留其他公开合同，不放fake/no-op callback。完成通知/脚本续跑是后续高优先候选，具体scenario/file加载/恢复闭包未实现，不能声称与卡顿无关。
- SetAnimeFrame/AdvanceAnime有独立anime_素材帧播放状态，不共用tween集合；原字节逻辑保留，未重写，未擅自清理/rename anime_。
- 原text easing helpers只供glyph text绘制；layer tween曲线由新Curve负责，不是旧tween fallback。
- 官方uint32 rollover/零start哨兵与host double时钟没有做全宽等价；保持compat时间ABI。官方back/elastic附加幅值/周期参数不在原compat tween合同，本轮不新增；所有curve数值及completion未实测。
- TagTrans/transition、渲染/素材解码、VM/scenecommit、Web帧预算、鼠标、API/IDB/schema未改。

## 身份与撤回证据

接手 main / HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`；5 tracked修改、18 untracked顶层条目，cached无改动。guarded ignored `.ref/artemis-rewrite-r1-baseline/` 保存接手status、全部dirty tracked原字节、build.sh/web.cmake/0006原字节、hashes.txt、pin与实际patched Compositor原字节。无reset/stash/clean/stage/commit/push。

| 当前文件 | SHA256 |
| --- | --- |
| `ports/artemis/compat-web/overlay/render/compositor.cpp` | `14762aae18deafcb47c658f9b03db30e2a6c7050f6f968a20a1f26d149788e1f` |
| `ports/artemis/compat-web/overlay/render/compositor.h` | `1315b6a71f3f432cfeba6e271d51b786fd00e05448bf21f7e0cb20153c880b64` |
| `ports/artemis/compat-web/overlay/render/tween_scheduler.cpp` | `1b148048403a05656aff17c7d8d30bbe45928d1eebb4c799f1330e962fbcf009` |
| `ports/artemis/compat-web/overlay/render/tween_scheduler.h` | `6b0f27fc0a4ce916eb9652909db50b3e4a46c214d6ce624cb1a813f67772b61f` |
| `ports/artemis/compat-web/build.sh` | `f1721ef223fdb139899b95680aa4f2b8a244fe18131b850717413448762fb087` |
| `ports/artemis/compat-web/overlay/cmake/web.cmake` | `3e228c05af0edc3cd31813f7d69a778608dd902f075dfbd2433c534e9927a247` |
| `ports/artemis/compat-web/overlay/script/lua_engine.cpp` | `f6306c4248de8d93c15fa30342b8afd33e76a5c4560340ceef4a55ac63a6b780` |
| `ports/artemis/compat-web/overlay/script/lua_engine.h` | `e89f4a4c6e1f24d76b43030d3e4f40a6c6325d75987414d43fc3f4c474f69b47` |
| `ports/artemis/compat-web/overlay/script/lua_tags_nav.cpp` | `6026e315d8781b3ce73dad908fe7c484d2e4681498dd8845fadbd92858a5b165` |

host `d64890c45c5b6564c2f9f56d0f946f4fad63c838747cb1b83d2e585c7e1d9705`、patch list `a4195f6df1a9fbfb4441809f9e13d0dd493b626e51c261806c5bdca893a882a0`、0006 `3cb3e92ca3b3bd6341f78923500351ef0227e052b6cf85838bc0b3fa73575b8f`保持。
独立撤回只能经后续显式授权用这份dirty原字节恢复本轮build接线/摘要，并移除本轮新增四源/账本；不能恢复HEAD扫掉其他dirty。当前没有撤回授权，累计R1保留。

## 静态自查与下阶段交付

只读git diff/hash及source inspection：旧成员/QueueTween不在replacement，公开签名不变，职责外normalized diff仅生命周期与tween桥，CMake两目标排除旧TU，保护既存dirty。`git diff --check`无输出；未运行任何测试/check脚本、编译器或构建。未生成测试脚手架。新文件尚untracked，静态diff --check不覆盖它们；已源码手读。

后续父代理另委托纯编译（本阶段未执行）：

```bash
ARTC_OUT=rewrite-r1 bash ports/artemis/compat-web/build.sh -j 2
```

输出应为 `ports/artemis/compat-web/out/rewrite-r1/build/artemis-compat.{js,wasm}`，build.log/hashes.txt绑定pin、render overlay、patched-source digest、JS/WASM配对hash与大小。当前JS/WASM **不存在新身份**，编译/link风险未排除。

旧4670 page仅白名单scene-commit/tween-arm固定hash，且 `/compat/pkg/` 从 `.test/artemis-animation-delivery-ci/package/unpacked/package`供应，不读取新out。未来供包必须独立R1 assets namespace、matching JS/WASM与runtime package配对、显式增加R1 trial路径/hash白名单及候选身份，不覆盖旧有效资产；先由父代理委托装配，再交用户刷新/START测试。本轮不修改服务、不探测存活、不声称入口可测。

许可：保留的上游render代码来自GPL artemis-compat；官方SDK/伪C为专有参考，独立手写不自动取得授权或解决再分发风险。研究摘录只在ignored .ref，不上传伪C/转译体。


## R1 静态审查后必要等待接线（2026-10-11）

父代理接受静态审查Finding1 P1、Finding2 P2，授权在累计{R1}上修正，不撤回核心。此发现是**静态推导，不是运行失败**；本次仍未构建/未测试，用户反馈待测试。

- **P1**：新增Compositor::TweensActive（scheduler Count只含已提交活动FIFO，包含尚未首tick和zero child）。LuaEngine::SetAnimationWait/AnimationWaitActive将原动画wait标记为timed reason，但IsWaiting不按初次deadline释放，改复核TweensActive、原transition时间条件、scenario分支原有text条件。Remaining只用于getScriptWaitReason的time字段展示，非放行依据。
- 所有原PendingAnimationMs等待入口：TagWt、TagWait scenario分支、TagWait无显式time/se分支均接新gate。trans后专用分支仍只设原transition deadline，不等待装饰tween；普通time/se/click/choice/input不自动继承动画gate。允许input跳过的分支仍通过SetWaiting(false)清gate；TagWt不接受input跳过。
- WaitState尾部新增animation/animation_text（默认false），SuspendWait保存并清理，RestoreWait恢复。AsbRunner现有WaitActive看waiting/timed即可，不需要修改：动画gate必置两者。事件/嵌套恢复后下一IsWaiting重新查询当前owner，不恢复陈旧预测deadline。SetTimedWait明确清动画标志；SetWaiting(false)在原OUT通知之后清gate，reason仍可被通知读取。load的ResetEphemeral已有SuspendWait、DiscardFlow、ReleaseGl；Dispose销毁Lua和Compositor，新gate没有指针注册或外部回调。
- **P2**：Endpoint使用专用strtof，errno ERANGE/解析失败/非有限值回退0，与既存ToF ERANGE/fallback合同一致并额外拒绝非有限值；不再将有限1e100 double强转float。duration/delay/loopdelay仍用double Number。合法端点及implicit property采样保留，未改ReadParam。
- 新正式script overlay为完整pin lua_engine.h/.cpp、lua_tags_nav.cpp，仅上述少数方法和等待数据接线，不计整个Lua VM重写。build.sh应用原patch后用完整script overlay覆盖构建树同名源/头；原GLOB两个目标均编译这些覆盖源，旧同名文件没有并列fallback。不新增C ABI/0007 patch，不改host顺序。

### 静态时间推导（没有执行）

首段100ms+尾段100ms：tick0启动首段；tick150完成首段并以150启动第二段，end250。即使Remaining原预测200，tick200在host Step先于Update时及更新后，Count均非0，IsWaiting不能放行。tick250 Update移除末段后下次poll才放行（可晚一poll，绝不提前）。若注册后的首次Update延迟到150，尚未started段已经占Count，wait仍阻塞直到真实末段完成。

zero段Count非0，首tick输出from且不完成；后续严格now>start写to并移除，下一poll释放。有限repeat保持Count直到最后真实完成；infinite-repeat不会因INT_MAX截止时间到达释放，但Finish/Remove/Clear后Count清空，下次poll释放，不变成无法取消的永久deadline。DeleteTweens/删除subtree/reset同理，collected未提交不算活动wait（保留原集合未提交不运行合同）。trans-only时间与text依据原消费定义，普通显式time仍按ClockNow deadline；普通click/choice仍走原输入/auto流程。取消/reset解除的是动画原因；若scenario同时有原text/transition原因，保留这些原因而非错误全放行。

### 本次备份、未知与交付

pre-fix保存于原guard下新 `.ref/artemis-rewrite-r1-baseline/wait-fix-pre/`，不覆盖R1前基线：本次status、hashes.txt、全部tracked dirty、四render/账本原字节及三份pin-script。原R1 source hash保存在该hashes.txt，不改旧历史hash。当前hash见上表。

本次仅源码阅读、静态diff/hash；未跑测试/check脚本/typecheck/contract/合成/单元/浏览器/游戏/API/curl、编译器/构建、联网安装、解包、服务启停/引擎启动/清理资源。未stage/reset/stash/clean/commit/push。官方completion/脚本续跑和素材anime仍未重写；无授权结论。Remaining无法预测未来tick间隔，仍是估算，gate与它脱钩。

下步另一只读review复审，再由父代理另委托纯编译，命令/out路径不变。新增script overlay也会进入patched-source digest并在build.log记录hash；无新JS/WASM身份。4670供包/显式R1 trial白名单需求不变，本轮不改服务/页面。

## R1 纯编译与独立供包（2026-10-11）

- targeted静态复审 `r1-wait-rereview.md`：OK with notes / 静态阻塞NO；不是动态验收。
- 命令 `ARTC_OUT=rewrite-r1 bash ports/artemis/compat-web/build.sh -j 2` **EXIT 0**。先确认新out不存在、绝对路径guard、pin本地对象存在；因此使用git archive本地分支，无联网下载。Emscripten6.0.9（4e422385…）、CMake4.3.1、dos2unix7.5.2。configure仅编译器能力检查；ARTC_BUILD_TESTS=OFF，未运行引擎/WASM/测试。
- 日志 `ports/artemis/compat-web/out/rewrite-r1/build.log`、`hashes.txt`；archive-source digest `813037cd43055edd2d5413429e4bf56b190f4121f51b610b24ead06b68b39ca0`，patched-source digest `4de6a5b2b1849d0e2428b9c67c12e39fc7f4efcfe7723c428b54d6fd50231045`。
- JS **153843B** SHA256 `1f7f7963125000660e8e39ed5f96b577edbd68e24831eea5725fd8d9b9b36d45`；WASM **1503645B** SHA256 `1644f3c269ad1ae28e49decde8a6cb62734d59d5159f7068b6cea6d40e7b7a1d`，位于out/rewrite-r1/build/artemis-compat.{js,wasm}，逐字节复制到独立 `.test/artemis-animation-delivery-ci/package/unpacked/package/assets/rewrite-r1-trial/`，附manifest.json。
- objects1.rsp/link.txt/build.make与build.log证明overlay/render/compositor.cpp、tween_scheduler.cpp编译链接；无src/render/compositor.cpp旧对象。三Lua覆盖与render四文件构建输入hash全部等于正式overlay。**无编译局部修正**；警告保留日志，不伪称零warning。
- 父代理追加授权最小修改 `.test/artemis-comparison-ready/page/compat-index.html`，R1完整WASM hash白名单及R1 JS完整hash验证；未知trial/身份不匹配fail closed。默认/旧trial不改。页面原字节、serve.js身份、所有既存assets hash及源码/docs前态保存 `.ref/artemis-rewrite-r1-baseline/delivery-pre/`；旧有效assets逐hash均未改。
- **用户入口**：`http://127.0.0.1:4670/compat/index.html?trial=rewrite-r1&ns=r1-1644f3c2`（直接页面，无embed，不自动boot）。用户先停已有引擎，再打开此URL，确认R1身份，手动START并操作测试，反馈结果；同刻只一个实例。
- ENGINE_BASE明确 `/compat/pkg/assets/rewrite-r1-trial/`；现有runtime configure.js将engineJsUrl/wasmUrl拼为新pair，原compat-worker.js locateFile返回请求wasmUrl。runtime/package0.1.14保持原字节，不替代调度；manifest记录runtime及demo-input hashes。
- OS netstat只读确认127.0.0.1:4670 LISTEN PID37368；wmic不可用，未获得commandline，未尝试启停。serve.js:277每请求stat/stream页面，:402/404既有page/package路由足够，**无server改动/重启/HTTP/API/curl请求**。入口已静态配置，可达/候选实际加载未验，不称API健康或游戏可用。
- 累计{R1}保留，不缩减/R2；所有测试由用户，**待用户反馈**。官方completion/VM续跑、素材anime未重写，许可风险未解决。下步独立只读artifact/接线audit（不测试），再由用户运行。

## 最新用户反馈（2026-10-11）

用户确认之前的问题标记为解决，另反馈刷新率太低仍要求重写。没有具体已解决问题列表、FPS或重复次数，不虚构。累计{R1}完整保留，不缩减；R2每帧职责优先静态查界，当前未实施；所有测试继续由用户。原R1构建/供包身份和历史反馈原样保留。
