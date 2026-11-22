# R2 用户反馈与批准边界（2026-10-11）

## 当前实施状态（2026-10-11 · Sol fresh writer）

用户选择「1」允许avemujica-promax/gpt-6.1-sol接手，覆盖下方渠道pending；Opus两次403无工具不再重试，原retained合同没有暗改。**累计{R1,R2}源码已实施，R1完整保留；R2静态review通过、纯编译与独立供包完成；未运行未测试，独立artifact审计及恢复补证无静态阻塞，待用户反馈。**低刷新未量化、未证明改善，全部效果测试由用户执行。

### 完整职责替换及装配

仅产品文件 `ports/artemis/compat-web/overlay/host/web_main.cpp`：Tick/Step/DrainQueued/Present整段独立手写，新增同TU `artc::WebFrameState`持有暂停、待提交、已提交revision及present计数。旧方法体/旧drawn_rev/paused/presented帧状态移除，不转调旧core，无fallback。初始化/Boot/输入/音频及其他host职责保留；删除旧文件头中不再真实的“与mac逐步相同”声明。

Tick先pause/reset/exit/boot有效性，再采Lua引擎double毫秒、R1 Update一次→DrainInput→RunEnterFrame→Step→完整批条件下Present→EndFrame。没有delta累加/平台新时钟/尾部补Update。Step将queue与runner编排为一个有限事务：先排队标签，wait或runner停机完成批，剩余预算耗尽续到下帧且不自动提交；jump/call仍调用原runner，其他tag仍DispatchTag(false)。4096保留为host响应上限，现包括queued tag与instruction，不称官方预算/FPS规则。没有去重、跳wait或改VM内部。

Present每次实际提交读取canvas backing尺寸，保留无效尺寸fallback、默认FBO/GL禁用状态/全黑清屏/等比居中viewport/Draw和累计计数；不改Draw内部/显式flip。reset/boot/dispose使提交状态失效，pause冻结仍由Lua Clock/Audio原接口承担，暂停不处理reset/boot/推进。dispose仍清boot/exit且保留原累计present计数及输入队列行为，不扩全Shutdown。

正式build.sh:162已将该overlay复制到build-src/src/host/web_main.cpp，:213记录hash；web.cmake的add_executable直接编译此文件。**无需新装配文件或patch**；build.sh/web.cmake/PLATFORM-PATCHES/0006原字节不变。不是改D:/artemis-compat外部树。

### 官方映射与裁定

已连续读 `.ref/artemis-official/wasm/asyncify/artemis.wasm.pseudo.c` f3551:594808-601766、f2890:435216-437204及f2801/f3544/f3933/f2892完整正文；pseudo丢失select/indirect实参不得据此命名槽。官方原文不入tracked。

| 官方符号 | 可证语义 → host表达 / 保留合同 |
| --- | --- |
| f2801:425469、f329 | 正常分支毫秒采样→f3551→f3544；host保留Lua NowMs毫秒ABI，不搬asyncify，不用Qc初始化作frame锚 |
| f3551首调f3933，f3933:818580 | active逐项Update、done销毁移除、pending并入；host在事件/脚本poll之前调用保留R1 Update一次，不重写R1注册/析构 |
| f3551状态循环与queued分支 | 循环推进状态/队列直到阻塞/返回；host独立Step/DrainQueued编排，但具体Lua/VM语义与host有限响应预算为主动保留compat合同，非官方逐分支等价 |
| f3544:592612→f2890，f2892:437280 | 状态128/48/132与多对象间接生命周期；未命名槽及提交闭包**未覆盖**，不说等于IsWaiting/Draw，不声称官方Present顺序 |

live父代理明确批准：采用可证“活动动画推进先于当帧事件/脚本poll”，不要求本轮补齐f2890间接闭包；Lua/VM等待、scene-commit gate与Draw保留。新脚本创建动画本帧不再尾部推进，下帧才首Update，这是明确替换边界而非补偿遗漏。

### 静态机制与review重点（不是执行测试）

R1显式from创建时写初态，collection延后注册，首Update建立start=now+delay；新建动画的TweensActive保证零duration/尚未started也能请求首帧提交，Lua activity gate仍阻塞。host采样先Update会同步Compositor now_ms_，本帧新transition的clock基准继续消费该毫秒；Lua内部另采时钟接口未改。

活动完成时Compositor::Update返回changed并增revision，待提交latch保留完成最终值，即使后置TweensActive=false也不遗漏；脚本半批导致延迟时latch留到完整批，不提前Draw破坏trans旧图。下一帧onEnterFrame仍按原每帧事件合同运行，不当成continuation重复旧事件。pause/resume保留冻结时钟，reset boot黑底Capture合同不改。

review重点：结束帧/zero与implicit/collection新动画首帧、时钟基准、reset/pause/dispose有效性、queue预算yield与wait side effects、半批+显式flip。R1 completion回调/素材anime/完整VMDraw/rollover及runtime跨线程调度仍未知/未覆盖。

### 身份与阶段禁止项

新guard `.ref/artemis-rewrite-r2-baseline/source-sol/`绝对parent断言+不存在后创建；保存15身份原字节、三doc原字节、status/full diff/cached与protected-identities.json、after-identities.json。初次路径字符串断言因Windows分隔符失败（写前），修为Path.parent等值；随后Python默认GBK读doc失败，guard已完整保存，改显式UTF-8，未改CLI/模型或产品补偿。两个失败没有覆盖旧baseline。

host before `d64890c45c5b6564c2f9f56d0f946f4fad63c838747cb1b83d2e585c7e1d9705`；after `4d00e8d2a2e4812d23cda410b9e3e6d0a7e241ca2dccb3f4e3fcec232535f6b4`。其余14保护身份一致，包括R1七源码、build/web.cmake/patchlist/runtime/page/server。R1 JS `1f7f7963…`、WASM `1644f3c2…`历史身份不改；详细完整源码hash见R1账本与guard，不套为R2 binary身份。

仅文本源检查/gitdiff/hash，不运行任何build/test/check/typecheck/contract/benchmark/WASM/browser/game/HTTP/API/curl或服务探测启停；未stage/commit/push/reset/stash/clean。许可风险未解决，独立手写不自动授权。

下一阶段由父代理先另委托fresh只读review，再纯编译 `ARTC_OUT=rewrite-r2 bash ports/artemis/compat-web/build.sh -j 2`（未执行，先guard新out并禁网络/测试）。供包须另授权最小独立rewrite-r2-trial assets/manifest、page显式trial与hash白名单，原runtime/旧trial资产不动；当前无新JS/WASM或可测入口声明。

---

## 历史批准前现状与基线

用户最新反馈：「现在将之前的问题标记为解决，但是刷新率太低了 还是需要重写」。旧问题按用户判定记解决；不虚构具体问题清单/FPS/重复次数。新问题=刷新率低（主观观察、未计量）。**R1完整保留，不缩减；R2边界已批准但尚未实施**。已实施累计{R1}，计划累计{R1,R2}不冒充已完成。两份current摘要及R1账本已记录反馈。

实时main/HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`；cached空。7 tracked既存修改及全部untracked保护，新guard `.ref/artemis-rewrite-r2-baseline/`保存status/cached/diff/hash、dirty原字节、R1七源、host、runtime源码、页面/server参考身份；不覆盖R1基线。host SHA256 `d64890c45c5b6564c2f9f56d0f946f4fad63c838747cb1b83d2e585c7e1d9705`；patch list `a4195f6df1a9fbfb4441809f9e13d0dd493b626e51c261806c5bdca893a882a0`；0006 `3cb3e92ca3b3bd6341f78923500351ef0227e052b6cf85838bc0b3fa73575b8f`。其余精确hash见guard/hashes.txt。session历史dirty/path-string.h/鼠标原样，不stage/reset/stash/clean/commit/push。

## 静态链与证据，不是低刷新根因

- `ports/artemis/compat-web/overlay/host/web_main.cpp:300-353` Tick：pause早退，reset/exit/boot，DrainInput→RunEnterFrame→DrainQueued→Step→R1 Update(now)→revision/animation与batch_complete gate→Present→EndFrame。`:424-446` Step有限4096脚本批至wait/stop，未完整批不自动Present；`:448-468` Present绑定默认FBO/清理GL状态/letterbox后Draw，计presented。frame C ABI入口`:543-547`不自调度。既存scene-commit dirty保证半批不覆盖trans旧图，不能抹回HEAD。
- `overlay/render/compositor.cpp:627-641` R1 Update，`:1511-1769` Draw内部scene FBO、effect递归、transition overlay、回原target/viewport后提交quad，显式flip可直接Draw。它是帧事务调用合同，本批准不改Draw内部/素材/effect/转场。
- 正式build.sh复制host overlay；web.cmake编译该host，与R1新render owner/三Lua覆盖并存，不是外部检出。
- 实际R1供包runtime0.1.14保持：page compat-index.html:230-238 rAF tick；dist/compat/runtime.js:258-263仍1000/60累计余数门；源码runtime.ts:283-309同职责；worker-host.ts:218-240单inflight、pause/checkpoint busy门；compat-worker.js:1134-1166调用_frame并frame-ok返回计数/audio。**普通frame不RGBA读回/传输**，Canvas已transfer worker直接WebGL绘制；runtime.ts:315-321 render空是合同。不能把读回当已测瓶颈，60Hz门/4096/画面提交gate都只是可能限制，未计量因果。

## 官方可核查锚与未知

唯一依据 `.ref/artemis-official/wasm/asyncify/artemis.wasm.pseudo.c` +同目录符号/wasm2c辅助：
- `Qc:423767`对应symbols index2800导出初始化，**不是frame函数**。内部初始化/注册链，不拿它冒充tick语义。
- `func2801:425469`正常分支平台调用a.pc、初始化/退出状态检查、`func329()`毫秒采样→`func3551`→`func3544`。完整函数已读，g3/g4恢复层不搬入产品。
- `func3551:594808`大型状态调度首调`func3933(now)`，随后多个状态/事件/脚本分支；约6958行，仅提取调用定位，**尚未逐分支读全/命名**。
- `func3933:818580`完整读：active注册集合逐项间接Update，done析构移除，pending注册并入active，临时推进状态切换。这是帧动画推进锚，不同于只拿f4391当frame。
- `func3544:592612`完整读：从上下文获取对象转`func2890:435216`。后者state128/48/132等等待/调度分支和对象间接槽，约1990行，当前只读前段；`func2892:437280`多目标间接生命周期调用。**完整VM/Draw/平台提交间接目标闭包尚未知**，不编造槽名字/帧预算或官方Present次序。

研究摘录仅guard ignored .ref。下一Opus写前须读全f3551/f2890及该批准单元必要间接目标，建立符号→职责→保留合同表。若未知直接影响host时序/等待/状态推进，须具体报父代理，不猜/不退回参数补丁；不能自行扩大到Draw/全VM。

## 父代理批准（live coordination已回复）

批准较小完整单元：**WebEngine Tick/Step/DrainQueued/Present及该事务直接关联reset/pause/dispose帧状态**。正式写域：`ports/artemis/compat-web/overlay/host/web_main.cpp`、必要同目录专职host组件、正式build选择replacement的最小接线。现host overlay完整替换这些职责，不顺带改初始化/素材/音频/其他host职责，不造泛用框架，旧frame core不得继续执行或fallback。

外部合同：C导出签名、delta-free引擎毫秒时钟、pause冻结/resume与dispose对外行为，输入/冻结MapPoint、音频、framebuffer尺寸/letterbox/显式flip与保留scene提交合同。生命周期仅事务自有状态；R1 scheduler/Lua activity gate、Draw内部、VM语义不改。runtime/page是独立跨线程seam，**本轮排除**；不是统一多层状态所有者，不能借低刷新自动改它们。

许可风险未解决，独立手写不等于授权；不得机械转译/上传官方参考。R2覆盖host原dirty要保存其精确原字节；R1不动。后续仅用户明确授权缩减时，按guard原字节+构建选择独立撤去R2，不reset回HEAD，不本轮缩减。

## 下一交付与禁止项

本阶段仅文档/静态取证，无产品代码、构建、测试/check/typecheck/benchmark、浏览器/game/WASM/HTTP/API/curl、服务探测/启停或清理。未另起代理。下一独立Opus唯一产品writer只源码实施/静态自查，之后父代理静态审核再委托纯编译。建议未来独立 `ARTC_OUT=rewrite-r2 bash ports/artemis/compat-web/build.sh -j 2`（本轮未运行；先guard新out，禁止网络/测试分支）。产物source digest/JS-WASM配对hash→独立rewrite-r2-trial assets/manifest及显式白名单，保持R1/旧trial资产；runtime不改。供包页面写域另授权，不能现写或套R1 identity。效果待用户亲测，不称刷新率改善/根因/FPS/官方全等价。

### 批准原文摘录

「批准你提出的较小完整R2 host单帧事务单元，不再等待症状确认。批准范围：WebEngine的Tick/Step/DrainQueued/Present及与该事务直接关联的reset/pause/dispose帧状态。产品写域为ports/artemis/compat-web/overlay/host/web_main.cpp、必要的同目录专职host组件，以及让正式build选择该替换实现的最小构建接线。默认在现有host overlay完整替换该职责块，不顺带重写整个host的初始化/素材/其他职责；避免为此新建泛用框架。对应旧frame core不得继续执行或有fallback。」

「Opus在写入前读全f3551/f2890及本职责必要的间接目标，建立官方符号→帧事务职责→保留合同的可核查表；Qc=f2800仅初始化，不能冒充frame证据。」

## 恢复故障检查点

- **恢复检查点**：scope反馈/批准边界已完成记录；workflow c67bb392-e67e-4315-ae88-c017139564a3收尾emit失败：`emit.outputPathMapping must be a JSON value; received undefined.`，后续Opus/Sol lane未launch（父代理提供事实）。R2产品未实施；恢复前dirty diff/名单/前后文档字节/hash与host/R1身份已捕获到 `.ref/artemis-rewrite-r2-baseline/recovery-c67bb392/`。同协议去掉emit后准备由Opus接手，无需清洁工作树；边界不变。

## Opus首请求403故障检查点

- **最新403恢复检查点**：workflow `17c548d2-c993-49d5-84b4-f4e658c1bec6` 的首次Opus child `682c284a-dae8-46f8-b580-f01864b706c6` 首请求403渠道过载（1 turn、无工具；review未启动，父代理提供事实）。历史首请求检查点已被下方最终阻塞状态覆盖：Opus首请求403 + 单次原合同恢复再次403，已停止同路由重试；R2批准但未实施，R2未build/review/供包/测试，待用户决定渠道恢复或批准其他允许模型实施C++。恢复取证保存 `.ref/artemis-rewrite-r2-baseline/recovery-17c548d2/`，15项受保护身份与上次快照相等；原scope/批准边界不变，R1保留，旧问题用户已确认解决、新低刷新未量化且待用户亲测。

原错误：`403: {"code":null,"message":"avemujica-aws: 当前渠道负载过高，请稍后再试 (request id: 202610110241364686000238268d9d6LltTgqmd)","param":null,"type":"permission_error"}`

## 最终阻塞检查点（覆盖历史pending）

Opus首请求403 + 单次原合同恢复再次403，已停止同路由重试；R2批准但未实施，R2未build/review/供包/测试，待用户决定渠道恢复或批准其他允许模型实施C++。

workflow `c22a2e93-4edf-40da-adbe-3fe87cc51b7e`：前置snapshot complete，Opus单次原合同resume child `6f1241e8-7265-46ea-8f5a-79fd6e553844` 使用claude-opus-5.5，1turn无工具再次403；r2-host-review未launch。原错误：`403: {"code":null,"message":"avemujica-aws: 当前渠道负载过高，请稍后再试 (request id: 202610110244496376472258268d9d6kyKQXALS)","param":null,"type":"permission_error"}`。首次17c548d2与request id保留上方。此为父代理inspect报告，不是产品故障或能力/许可结论。

最终取证 `.ref/artemis-rewrite-r2-baseline/blocked-c22a2e93/`；覆盖绑定artifact前全文存pre-final-report.md，原scope快照不覆盖。R2批准范围不变、产品尚未实施；R1保留，新低刷新仍未量化待用户亲测。不继续retry/另起代理，等owner决定。

## R2纯编译与供包（2026-10-11）

独立review `85254823-87c3-4d60-8a6d-ca06faac8423_reviewer_output.md`：OK with notes / static blocker NO，父代理批准继续。冻结源码/正式build接线无改，无编译局部修正。新guard delivery-sol/绝对parent+不存在断言，保存161文件及git状态/diff；out/rewrite-r2、新trial先断言不存在。

`ARTC_OUT=rewrite-r2 bash ports/artemis/compat-web/build.sh -j 2` EXIT0；本地pin对象存在，走git archive，无下载/安装；脚本解包至已guard out/rewrite-r2/source，再复制build-src，未解仓库根。Emscripten6.0.9（4e4223852a0835923411059a3929907d7df1232e）、CMake4.3.1、dos2unix7.5.2。ARTC_BUILD_TESTS=OFF；仅正常CMake能力探测/编译链接，无引擎或测试执行。warnings保留build.log，console和exit保存delivery-sol。

批准overlay raw30402B SHA256 `4d00e8d2a2e4812d23cda410b9e3e6d0a7e241ca2dccb3f4e3fcec232535f6b4`；原build的dos2unix令实际compiled host29680B SHA256 `edc167a1a3ca4391621d3aad007df1a78113624eb246bc49461ddd614e114914`。已向父代理报差异并获批准：只以actual==approved raw.replace(CRLF,LF)严格字节关系继续，未strip/编码/BOM/其他忽略；expected LF同29680B/edc167a1完整hash，byteEqual=true。R1七源均raw==actual，无归一。compiled-source-identities.json逐项保存；objects1.rsp/link.txt/build.make证明host及R1 compositor/tween/Lua源码入链接，旧src/render/compositor.cpp.o排除。

archive-source digest `813037cd43055edd2d5413429e4bf56b190f4121f51b610b24ead06b68b39ca0`；真实build-src patched digest `186e7cb8b960742afe830b86313b4f05aba849c9ba842395457273aa38c18477`。JS153843B SHA256 `1f7f7963125000660e8e39ed5f96b577edbd68e24831eea5725fd8d9b9b36d45`，WASM1503668B SHA256 `08a426da6bb54c59ed8e097a6dec277e8225fc923369d42d98081cd8a8a1ad9a`。JS与R1相同是实际编译结果，不制造差异；新WASM独立。pair逐字节复制至ignored assets/rewrite-r2-trial，manifest含cumulative R1+R2、pin/head/review hash、source与normalization/runtime身份、tested:false/effect:not-tested。

page仅增加rewrite-r2完整WASM白名单与独立JS hash校验，旧分支/默认不改，unknown或hash mismatch仍fail closed。ENGINE_BASE依既有逻辑为/compat/pkg/assets/rewrite-r2-trial/；configure.js拼matching JS/WASM，runtime将URL送worker，worker importScripts及locateFile消费同pair。serve.js既有按请求stat/stream的page/package路由足够，server/runtime未改。用户入口 `http://127.0.0.1:4670/compat/index.html?trial=rewrite-r2&ns=r2-08a426da`；无embed不自动boot，可达/加载未知。

保护比较：161前态中只page许可变更，其余160保持（后续三doc允许更新另列）；R1原JS/WASM/manifest、所有旧assets及package/dist/runtime/configure/worker/input、冻结源/历史dirty保持。无HTTP/API/curl/browser/START/WASM/game/截图/FPS/服务进程探测启停，无test/check/typecheck/contract/benchmark/CI，无stage/commit/push/reset/stash/clean。累计保留，下一fresh只读artifact audit后用户亲测。编译成功不证明低刷新改善；原unknown与许可风险不变。

## Doc-only审计收口（2026-10-11）

fresh源码review `85254823-87c3-4d60-8a6d-ca06faac8423` OK with notes / static blocker NO；真实纯编译EXIT0见上方日志。独立artifact审计 `f328f2c1-d62c-4213-b5dd-c792f4a0778e`后，GBK读取阻塞经UTF-8恢复补证，最新报告 `C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/b876b9cb-1a8a-46dc-8e99-0f9ec0d44b7a/rewrite/r2-artifact-audit-recovery-utf8.md`（output所属run目录b876b9cb-1a8a-46dc-8e99-0f9ec0d44b7a；未另提供execution run ID）。**STATIC_BLOCKER=NO**。page完整diff两个hunk、9新增行无删除，R1完整JS检查与旧分支保留；原式source/build-src digest均匹配历史记录。前次自拼stream漏binary标记*导致差异，并非源码漂移；实际stream LF，不推断Windows必CRLF。

有限认证：补证仅关闭page diff与digest两缺口，沿用前轮161保护字节/pair/输入/objects证据，不重算全repo；恢复报告中文替换字符未知仍保留。host raw/compiled仅严格CRLF→LF，已有父代理批准及独立byte证明；R1/旧assets/runtime/server未改。不是运行、服务健康或低刷新效果认证。

当前累计R1+R2，未缩减、未R3；旧问题用户已判解决，新低刷新未量化。待测入口 `http://127.0.0.1:4670/compat/index.html?trial=rewrite-r2&ns=r2-08a426da`，无embed、不自动boot；新WASM `08a426da6bb54c59ed8e097a6dec277e8225fc923369d42d98081cd8a8a1ad9a`/1503668B，JS `1f7f7963125000660e8e39ed5f96b577edbd68e24831eea5725fd8d9b9b36d45`/153843B与R1同胶水实情。仅用户先停其他引擎、开R2手动START测试反馈；未反馈前代理不自测/擅重写/缩减。

本轮仅两当前摘要和本账本写入，静态hash/gitdiff确认冻结产品/page/newpair/manifest本轮前后身份保持，cached空。未build/供包/test/check/typecheck/contract/benchmark/CI/WASM/browser/game/HTTP/API/curl/服务进程探测启停清理，无stage/commit/push/reset/stash/clean/新代理。入口仅静态配置，f2890闭包/runtime调度/completion/完整VMDraw/materialanime/rollover/许可及真实效果仍未知。

## R2用户最新反馈（2026-10-11 · 覆盖等待首次反馈状态）

用户对R2反馈「还是不行」，指低刷新仍未解决，不是撤销R1旧问题已判解决；未测量FPS，不虚构数值或根因。此前静态review通过、纯编译EXIT0和独立artifact审计无静态阻塞记录继续成立，但不是效果成功；上方待首次反馈/效果未测表述仅为反馈前历史。

累计R1+R2保留，不回退、不缩减。下一步开始R3选域/边界调查，在累计重写上继续以官方 `.ref/artemis-official/wasm/asyncify` 伪C指导逐块独立手写直接替换compat旧职责，不打差异patch；失败记录后继续，解决后才逐步缩减。尚未R3实施，本轮不新建R3产品或文档设计。全部测试用户亲自执行，代理不得代测。

R2入口及新WASM/JS身份沿用上方原记录，本轮未访问或改动入口/产物；source-sol、delivery-sol guard不覆盖。仅更新两份当前摘要并在本账本追加反馈，保留全部历史hash与既存dirty/untracked，不stage/commit/push/reset/stash/clean；未构建、测试/check/typecheck/contract/benchmark、浏览器/game/WASM/HTTP/API/curl/FPS/截图、服务或进程探测启停清理，未另起代理。
