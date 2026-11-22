# R4 — session-generation frame owner（源码阶段）

唯一Sol writer，累计R1–R4保留未缩减；R1旧问题用户认解决，R2/R3用户反馈低刷新仍不行。R4 SOURCE_REVIEW PASS；隔离纯TS实际emit EXIT0，独立供包最终PASS_STATIC，候选待用户亲测，全部测试由用户执行。效果/GPU/FPS/可达/根因/官方完全等价/许可未知。

独立供包审计 runId `bd86e4fe-c34c-4305-b8ff-03db873c8976`，outputReference `C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/a9ab56e2-14f4-420a-a04e-657f15c99ee9/rewrite/r4-delivery-audit.md`：**PASS_STATIC**，无P0/P1 blocker；核定63输入SHA、204 emit bytecmp、211候选文件身份、worker配方/R3 pair及152保护身份，不是重跑编译或测试。

## 静态复审P1修正

父代理接受review `214c4056-244f-4b5f-a98a-43cb1911defc`唯一P1：worker-host.ts frame continuation前置running===false已throw，后置重复比较导致TS收窄冲突。本次仅后置 `reply.type !== 'frame-ok' || reply.running === false` → `reply.type !== 'frame-ok'`，保留前置error/false拒绝、generation/credit核定及全部owner行为。无其他源码改动；现有after身份/diff/status更新，before/original不动。未运行typecheck或构建，无R4候选；待局部独立静态复审。

## 直接替换

正式三个文件`packages/runtime-artemis/src/compat/{runtime.ts,worker-host.ts,compat-worker.js}`。无新helper、接口factory、flags或依赖。runtime tick不使用dt/60Hz accumulator，不再持有frameFailure与pause epoch；worker-host直接持有generation、单调credit、单flight Promise与一个布尔latest需求。公开frame()并发join不产生需求；真实tick在途时仅合并一次，ACK清flight后最多补发一次，无新tick不会自循环。旧inflight/sendFrame实现删除，非外包旧算法。

serialize及facade持久化队列入口同步revoke并busy，先等已有frame，再操作；finally释放屏障，不从屏障前tick复活需求。pause/resume调用顺序排队，busy期间丢需求，resume无异步清pause continuation；native/relay失败终止。load ready后显式首frame和started=1检查保持，dispose等待过程中不会由ready/settle复活。过期成功ACK不写counter/audio/补帧；错误/false native结果仍安全终止，不复活旧需求。worker对generation/credit验证后调用无参_frame并原样返回native counters/now/audio，加内部ACK字段，不改外部协议version、存档格式或IDB。

worker-host处理RPC timeout、post同步throw、worker error/messageerror及fatal一次stop，回调通知runtime后dispose；公开frame Promise保留拒绝且owner有拒绝处理。原30s RPC与1s dispose fallback保持，无新timer/RAF。audioBusy/audioEpoch专用状态、native时钟、input/resize/mount/import/read/save算法原样依赖，仅必要帧等待和终态边界变更。shared GameLoop与测试页loop不改。

## 官方证据与原创策略

`.ref/artemis-official/wasm/asyncify/artemis.wasm.pseudo.c:12033-12041` f329整数毫秒；425469-425676 f2801可见329→3551→3544。R2账本保留其余frame/wait闭包。实际reference/artemis.asyncify.js的_clock_gettime clock4使用_emscripten_get_now，单loop generation、runIter后scheduler有静态依据；实际官方main fps注册未确证，不声称官方用了worker ACK。latest-credit为本轮原创platform设计，不改变Lua毫秒采样、不给native塞dt，不宣称完整VM/浏览器策略等价。

## 身份/证据

main HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`，cached空。新绝对guard `.ref/artemis-rewrite-r4-baseline/source-sol/`目标不存在才建立，无解包。original保存三源及两个当前docs原bytes；before-identities.json含155身份：R1五源、R2 host、R3 compositor、旧runtime dist、R1R2R3 matching pair/manifest、page/server/copy/package及共享caller。after-identities.json与actual-source.diff提供交接；职责外名单逐项rawSHA相等，不冒称整个共享file职责外冻结。源LF/BOM状态与原bytes一致，strict UTF8。首baseline脚本parse-time引号SyntaxError零写入，准确停报；父批准一次Path.as_posix纠正后成功，非源码缺陷/测试证据。

旧session历史/R1R2R3账本、package/lock/build脚本/dist/page/serve/ports/.test资产不写。无stage/commit/push/reset/stash/clean，无测试/check/typecheck/contract/benchmark/JS/WASM/browser/game/HTTP/API或进程操作。许可风险单列：官方SDK/伪C专有，参考留ignored .ref；独立手写不自动解决再分发授权。

## 下一阶段装配蓝图（未执行）

fresh独立静态复审通过后，另授权guard新`.ref/artemis-rewrite-r4-candidate/`源快照与独立输出，候选身份`0.1.15-r4`仅该独立package，不原地改0.1.14。纯TS编译独立runtime-artemis以及engine-api/vfs完整依赖闭包，配置outDir/tsbuildinfo均落独立目录，禁止调用会写正式dist的现有build脚本。将现copy recipe的worker-audio原bytes + LF + R4 compat-worker原bytes装入候选dist/compat；全部import闭包（audio-relay/read-host/configure等）及node_modules/importmap所需依赖必须指向独立树，不能暗回旧runtime。

建议独立交付`.test/artemis-rewrite-r4-delivery/package/`，独立trial路由/importmap与workerUrl，pair复制现rewrite-r3-trial JS `859b2179e5fcdae043230bb725e99ff2d708551caebaa831aedf9e9a63fc8113`、WASM `ea38ff9ba4a1fcec452a683cbd44706ca66099248f82e24d6c0b1674af15489a`，不重编C++。manifest列source/dist/全部依赖/pair rawSHA，旧供包与R1R2R3入口保持。路径/路由接线需下一阶段另授权，当前未创建可测试入口。

## 实际TS emit与独立交付（后续阶段）

SOURCE_REVIEW PASS：独立复审c8bc92c5-73aa-4f20-b1b0-0e716145212f确认P1消除；正式三源冻结。纯TS实际emit `node node_modules/typescript/bin/tsc -p .test/artemis-rewrite-r4-delivery/configs/tsconfig.json` EXIT0，所有输出/tsbuildinfo隔离，未运行测试或产物。
独立R4候选已静态装配于旧PKG的assets/rewrite-r4-trial/，package0.1.15-r4与engine-api/vfs完整闭包；worker按原concat配方。runtime SHA7ece321d…、worker SHA2b47bcc4…，R3 JS859b2179…/WASMea38ff9b…逐byte复用；不是只用WASM身份冒认R4。
显式入口 `http://127.0.0.1:4670/compat/index.html?trial=rewrite-r4&ns=r4-7ece321d` 仅静态配置，无HTTP/浏览器确认。page仅R4分支隔离importmap/worker/base与完整manifest身份准入；旧路由与serve/候选冻结。独立供包最终PASS_STATIC，待用户亲测；效果/GPU/FPS/可达/根因/官方完全等价/许可未知。

新guard `.test/artemis-rewrite-r4-delivery/`与served `assets/rewrite-r4-trial/`均创建前绝对parent断言且不存在；delivery-pre保存page原bytes与关键库存，不覆盖source-sol。63项input SHA含三个包完整src、实际tsconfig/package/copy/compiler身份；input原bytes快照、隔离configs、compile.log/compile.exit、actualdeliverydiff.patch、protected-after.json保存。单配置继承实际base strict emit选项，以paths解析正式engine-api/vfs源码，rootDir=packages，outDir=新build-stage，tsbuildinfo新configs；没有综合npm build或noEmit检查。211候选文件全hash，manifest自身SHA cd4d66c637f2bb790cc9169c7795c07609b9cf74b6a7745529ad33db33d68f05。

新runtime完整SHA `7ece321da95f090dc4c6b8b773aa09ac3187b1895a2a5adb285cac0c76374ac9`；worker `2b47bcc4c6646326900d8bc93d20985c2912d9f907a240602aebeeebfc69966f`。demo-input原bytes复制进候选独立dist，所有literal JS相对/包import静态解析存在。page R4专用动态importmap在module前选择独立闭包，manifest固定SHA后逐项核bytes/hash（供用户运行，代理未执行）；unknown/mismatch仍failclosed。UI/loop/API不重写，daisyUI技能已读但无视觉改动。

formal源/旧dist/0.1.14/serve/R1R2R3候选/native身份结束相等，page原CRLF保持。TS emit不是测试，未执行任何候选JS/WASM/浏览器/HTTP/服务操作，未重编C++。旧阶段禁止项及下一阶段蓝图为历史授权，当前实际结果以上方为准。
