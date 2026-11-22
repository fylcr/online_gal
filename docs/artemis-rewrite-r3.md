# R3 — scene / transition owner 源码交付

2026-10-11，唯一 Sol writer；累计 **{R1,R2,R3} 源码静态review PASS、纯编译成功、独立候选供包审计最终PASS_STATIC，待用户亲测**。R1旧问题按用户判解决，R2低刷新「还是不行」仍未解决。纯编译不是测试；没有运行引擎或任何测试，没有改善/FPS/官方全render等价结论。

## 完整替换边界

仅 `ports/artemis/compat-web/overlay/render/compositor.h/.cpp` 产品改动。同文件专职 `Compositor::SceneRenderer` 独占基础/转场program、scene FBO/texture及尺寸、retained有效性、捕获/rule纹理、整数活动与事务scratch。旧Compositor资源字段与Draw/CaptureFrame/BeginTransition/DrawTransitionOverlay/InitGl/Snapshot核心方法体删除，不保留legacy Draw、并行实现或fallback。公开门面直接进入新owner。

原创事务为 **TakeSnapshot → Traverse → Submit → Overlay → Present**：
- snapshot先按保留排序比较器稳定排序，冻结有效transform、effect/coverage、素材bindings；将glyph/mesh/quad统一表达为含opacity的triangle batches，提交期间不再反读layers几何。
- traversal消费预先确定的连续子树边界，独立编排effect Begin/递归/End与继承alpha；复用现LayerShaders，不将旧Draw的lambda搬入新类。
- submit只消费冻结顶点/bindings；最终scene overlay后标retained，恢复入口FBO/viewport并提交scene quad。移除旧Draw周期diag_frames日志，该日志不是公开渲染合同。
- callback本地复制后最后调用，之后不访问Compositor或SceneRenderer状态；可重入ReleaseGl/Draw/Begin。

保留GL shader源码/CompileShader、CreateTexture、素材decode/upload、glyph atlas及text/anime活动owner、EffectiveTransform/DrawOrderLess、effect shader/helper和stage masks生命周期。RenderStageMask仅readiness及本地共享brush读接口变更；基础program由新owner唯一释放。ReleaseGl保留layer/mask/glyph/effect/scheduler原cleanup各一次，scene/program释放交owner；Shutdown保留font/callback清理，headless也清scene活动。

窄接线：Update仅原trans分支、PendingAnimationMs仅trans remaining；TransitionActive/Remaining及DescribeDrawList诊断读同owner。父代理另批准LoadImage/LoadMask/LoadFont三处 `!gl_ready_` → `!scene_renderer_.Ready()`，其余方法byte保持。Ready代表两个program已link成功，不依赖活动或捕获状态。不改build.sh/web.cmake；既有整render目录复制及同TU选择足够。

## 官方语义 → 原创设计 → 合同/未知

参考仅ignored `.ref/artemis-official/wasm/asyncify/`，未复制专有伪C或机械转译进GPL产品。

|已证依据|本轮表达|保留/未知|
|---|---|---|
|f3980 pseudo828589–829494，wasm2c2207570–2207799/2207960–2208139；整数活动593956槽2148/2149/2150=f3982/3981/3980|Armed/Running/Done/Idle；首Update定绝对start/end，now<=start赋from；中间整数截断；终值setter→Done→owner completion释放|无delta累计；compat trans没有repeat/yoyo参数，不新增；不混用581044浮点活动|
|f1475创建/注册相关段211784–214857，f3578/3579全文613242–613325；table2673/2671=f3578/3579|progress范围 -clamped vague →255+vague；shader边界消费该整数的归一化位置，rule和plain仍用保留shader合同|官方f3579→f839/f490后续未闭合，不发明VM callback ABI；f1475给定段并非声称整个函数全文|
|f3981/3982全文829499–829729|取消/析构只销毁，不冒充完成通知；终点由Advance返回Done后owner释放overlay|completion行为只本地资源所有权，不声称官方后续清场等价|
|f3933全文818580–818826|活动推进与scene事务分开，不向R1调度插入活动；完成后revision请求终帧提交|active完成移除后pending并入的官方语义未用来改R1；不新增回调注册队列|
|官方GPU front/back/dirty/commit未证|原创retained scene和入口target提交|主动保留compat合同，不叫f1615 GPU swap；动态vptr+28/+56/+92、f2672+48/VM未知不改|

**live父裁定**：为保持现Lua trans专用wait deadline，在Begin内用传入同一Lua now调用新活动一次首Update；后续仅既有Compositor Update推进。explicit Phase避免now==0重复初始化。此为compat等待合同适配，不宣称官方调用时机等价，不另采clock、不尾部Update、不额外Draw/Present。

Begin先取消上次活动并释放其capture/rule，再捕获最近完成scene（含该次overlay，不重读当前layers）。duration<=0/非有限now/无retained/capture失败返回false且不留活动；重入Begin取消旧owner但不修改retained画面；缺/非法rule或上传失败降为既有plain合同。终帧赋to并Done后释放overlay，Update仍返回changed一次；Reset/Release取消且释放全部scene/trans/program资源。headless保留原无GPU定时活动合同，Snapshot/Draw无GL行为，不当作运行fallback。

boot空层Draw仍黑底；R2半批gate完全不动，显式Lua flip仍直接Draw。Snapshot只读同retained scene，保存/恢复FBO和PACK_ALIGNMENT并翻行。scene尺寸变化释放旧目标/捕获，FBO失败释放新资源；Draw所有失败出口恢复入口FBO/viewport，不硬绑0；letterbox入口viewport原样。未声称保存全部GL program/texture/blend/attrib状态。

## 原字节身份与保护证据

main HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`，cached空。基线 **`D:/online_gal/.ref/artemis-rewrite-r3-baseline/source-sol/`**：先绝对root/parent equality断言，确认新target不存在再mkdir，无解包。guard.txt、before-identities.json、原source/docs、status.txt/tracked.diff/cached.diff、compositor.h.diff/compositor.cpp.diff、protected-methods.txt提供静态审查。未stage/commit/push/reset/stash/clean；历史session dirty/path-string.h不动。

|文件|before SHA256|after SHA256|
|---|---|---|
|compositor.h|1315b6a71f3f432cfeba6e271d51b786fd00e05448bf21f7e0cb20153c880b64|4d9af003d594019e0ecc7ee6d421f777deb8e2af9d1dbe201cdc79877d5ec5de|
|compositor.cpp|14762aae18deafcb47c658f9b03db30e2a6c7050f6f968a20a1f26d149788e1f|83b9763d0be03656d62ba40946849311f44fe640688aa38417801a748550713c|

R2 host完整raw SHA `4d00e8d2a2e4812d23cda410b9e3e6d0a7e241ca2dccb3f4e3fcec232535f6b4`不变；R1 Lua三文件/scheduler两文件与R2 source-sol after-identities逐项一致。55个职责外Compositor方法byte相等，三个readiness方法归一化单表达式后相等，scheduler调用表达式/顺序保留；Update/Pending仅trans seam，header scheduler字段/入口保留。原CRLF保持：h 355→382行，cpp2099→2157行（每行仍CRLF），无全文件fmt/BOM变更。产品基线diff h+62/-35、cpp+466/-408，不用行数证明重写。build/cmake/runtime/page/server及其他保存身份不变。

official rebuilt-wabt wasm SHA `7ceabed28b84e7689fc4b629fa232e2857f15c3b6e6fa3e09fc39dba082d145e`；pseudo `db6e038cb5d1e05abc8771cddc746b30f8a697a9c9ae1723adc2023b88196f70`；wasm2c `90559646e150372a0a10316d9474d6417116984493a5e4a1502a92e99c9460ed`实核。工具默认GBK一次读失败（源码未写），停报获准严格UTF8；之后仅原生edit改源码。一次edit非唯一匹配拒绝未写，增加实际上下文后成功；一次静态scheduler“整行存在”guard被合法headless scene清理接线触发，改核表达式/顺序及允许seam，未以此修改源码。

## 阶段与下步

两current摘要已标源码完成待复审/编译/用户亲测。执行仅read/sed/grep、git只读diff/status、SHA256和纯文本guard；git diff --check无问题，但render为既有untracked，另有raw基线diff与手读。未增加测试，未运行build/test/check/typecheck/contract/benchmark/CI/引擎/browser/game/HTTP/API/服务探测启停清理，未碰R1/R2候选资产或47xx服务。

必须fresh reviewer核资源失败/resize、callback重入、整数终点/等待、glyph/mesh/effect嵌套及mask合同；编译/link及像素均未知。事务snapshot新顶点批会分配，性能影响未测，不宣称优化；无skip/cache/FPS参数试改。许可/再分发风险独立保留。当前无已知需要writer后续修复的静态阻塞；review发现或编译错误需另授权writer followup，不能主代理自行编码。

## 纯编译与独立供包（2026-10-11，覆盖上方历史待编译状态）

fresh源码review静态PASS无阻塞，报告 `C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/8313979f-6f8a-4206-b346-69a0af1b18c9/rewrite/r3-scene-owner-static-review.md` 已读；review未独立重算身份，本阶段重算source/input，最终独立供包审计已PASS_STATIC，引用见本节末；不是运行或效果认证。

真实执行 `ARTC_OUT=rewrite-r3 bash ports/artemis/compat-web/build.sh -j 2`，recipe成功EXIT0（原 `out/rewrite-r3/build.log` 末行exit=0），Emscripten6.0.9/CMake4.3.1，local pin17200e64ea99681e3c27d694433c0ff78d7a7cee存在，走git archive无下载。ARTC_BUILD_TESTS:BOOL=OFF；仅编译链接与configure能力探测，不执行引擎/WASM。实际90条compiler warning: + 3条CMake Warning = 93，保存delivery-pre/warnings.txt；109为含warning字串行数。包括CMake shared-library版本警告和unused/GCC-compat等编译警告，不称零警告。

**恢复事故如实记录**：初次组合命令的Python guard中反斜杠字符串被传输成非法字符串，SyntaxError: unterminated string literal；Python零写入，但外层没有failfast，后续build已运行。末尾delivery-pre/build-exit.txt写失败（目录不存在）；不把整组合命令称EXIT0，不伪造原exit记录。编译仅recipe自身绝对OUT/SRC guard生效。停报获父批准，同工具set -euo pipefail独立短命令恢复；新delivery-pre及asset绝对parent/不存在断言后创建，不重编译、不覆盖out、不换CLI。**delivery-pre库存是postcompile/pre-delivery，不是编译前baseline**。

新guard `.ref/artemis-rewrite-r3-baseline/delivery-pre/` 保存199身份原bytes、inventory/status/partialdiff、原build.log.saved、compiled-source-identities.json、实际build.make/objects1.rsp/link.txt、page.diff。source-sol保护证据原样。除page许可新增9行/零删除外，198库存身份保持；R1五源、R2 host与历史after-identities逐项一致。共享h/cpp与已审raw SHA一致，本轮产品零修改。render四源与Lua三源实际输入raw byte相等，src/render/compositor.h也相等；host仅原recipe CRLF→LF，actual严格等于raw.replace(CRLF,LF)，compiled29680B SHA edc167a1a3ca4391621d3aad007df1a78113624eb246bc49461ddd614e114914。objects/link/build.make证新overlay compositor/R1 scheduler/Lua cpp/R2 host入编译链接，旧src/render/compositor.cpp.o排除。header输入由复制和编译依赖接线证明，不将.h声称为独立TU。

archive-source SHA 813037cd43055edd2d5413429e4bf56b190f4121f51b610b24ead06b68b39ca0；patched-source SHA 1548ddc8e275ad7aa26c83e3d2f9a616f0139cef3951bef4929104de257bd67f（recipe原式digest）。JS153843B SHA **859b2179e5fcdae043230bb725e99ff2d708551caebaa831aedf9e9a63fc8113**；WASM1504392B SHA **ea38ff9ba4a1fcec452a683cbd44706ca66099248f82e24d6c0b1674af15489a**。逐byte复制新 `.test/artemis-animation-delivery-ci/package/unpacked/package/assets/rewrite-r3-trial/`，manifest.json记录cumulative R1/R2/R3、tested:false、pin/head/input/digest/runtime身份与库存时点。旧候选/default/runtime0.1.14/serve.js/official/source guard未改。

page仅新增rewrite-r3完整WASM白名单和专用完整JS SHA校验；ENGINE_BASE既有拼接得到 `/compat/pkg/assets/rewrite-r3-trial/`；unknown/hash mismatch仍failclosed，R1/R2/default分支保持。configure resolve生成同namespace pair→runtime→worker importScripts及locateFile指定WASM，serve现有stat/stream按请求供文件，无需重启。用户手动入口 **http://127.0.0.1:4670/compat/index.html?trial=rewrite-r3&ns=r3-ea38ff9b**；无embed不自动boot，先停其他引擎再手动START，同刻一个实例。入口仅静态配置，未请求可达/加载。

两current摘要同步候选状态。未tests/check/typecheck/contract/benchmark/CI/engine/browser/game/HTTP/API/curl/FPS/截图/进程探测启停清理，未stage/commit/push/reset/stash/clean，无新代理。最终独立供包审计 **VERDICT: PASS_STATIC**，runId `785953d1-fba6-41c1-bb6f-1331dbbdaa6d`，outputReference `C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/3507794e-ec66-48a6-8ece-3490cb3f7dac/rewrite/r3-releasegl-review.md`。199库存195冻结、pair/manifest/input及装配接线继承通过；唯一ReleaseGl缺口以直接read闭合：headless原revision/layers/scheduler清理保留，旧trans/gl字段清理替换为新owner取消活动、清ready/retained及scratch；无GL调用或completion新增。此前输出截断、GBK读取、比较helper漏旧trans/gl末尾导致AssertionError的三次中断，不是源码缺陷，也不记为PASS。只有最终报告为PASS_STATIC。

本次仅三份阶段文档精确收口，源码/page/pair/manifest/runtime/serve/official/基线及历史dirty冻结，无构建、测试或任何运行/请求/进程操作。R1旧问题用户认解决，R2低刷新未解决，累计R1+R2+R3保留未缩减，全部测试用户亲自做；候选待用户亲测，不认定R3改善。入口仅静态配置，同刻一个引擎并手动START；可达/GPU/FPS/效果/根因/官方全render等价未知，许可独立风险保持。原guard事故/组合命令失败及postcompile/pre-delivery库存时点原样保留，不倒填。

## R3用户最新反馈（2026-10-11 · 覆盖待首次测试状态）

用户最新反馈（m00532）R3「还是不行」，指低刷新仍未解决；不撤回R1旧问题已认解决。上方源码静态PASS、纯编译recipe EXIT0与独立供包最终PASS_STATIC真实记录保留，但不等于效果成功，不再称R3待首次测试。无量化FPS、根因或代理自动测试结论，全部测试用户亲自做。

累计R1+R2+R3保留未缩减。R4主线程↔worker帧驱动/时钟owner正在静态取证，尚未写产品/构建候选，不预认根因、不新增R4 ledger；继续基于官方pseudo逐块原创直接替换、累计扩展，用户确认当前低刷新解决后才缩减。R3入口、完整SHA、93警告口径、guard事故/postcompile库存及许可风险沿用上方，不改候选或历史证据。本轮仅三文档更新，源码/page/pair/manifest/runtime/serve/official/所有旧候选和既存dirty冻结；无构建、测试、运行/HTTP请求或进程操作，不stage/commit/push。
