# 当前工作检查点 — 新对话先读

> **当前摘要（2026-10-05 · compat 主线 · 本块是唯一「现状」；下文按时间倒序的历史记录原样保留，不得当现状。）**
>
> **源身份 / 原生候选**：CI pin `9f62262…`；上游 `src/host` 只有 `mac_main.mm`、CMake 只有 native
> ⇒ Web host / Web CMake / `ReadAt` / numeric 全是本地 port。原生候选 WASM `1331402` B / SHA
> `9c4a8808…`、JS `153633` B / SHA `9fdc75b6…`、源码包 `784402` B / SHA `fe2e2007…`；独立解包用自带
> 配方重编 **exit 0 且与候选逐字节同**，source / manifest 全过 —— **仍是 build 证据，不是游戏证据**。
>
> **已测事实（各条各自成立，不等于游戏可用）**：signed PF **node 3/3 + probe 30**（旧「8/8」作废）、
> read host **28/28**、worker **16/16**、`mainAudioRelay` **15/15 用 fake**（**未听见声音**）、Astro hooks
> **6/6**。音频实况见上条（relay 21/21、worker-audio 12/12、集成回归 green、portable 545/545）。
> 版本 `engine-api`/`core` `0.1.1`、`runtime-artemis` `0.1.10`、`engines` `0.1.11`，offline frozen-lock
> **exit 0**，应用 workspace link 已修。全量 `npm test` 2 个 legacy extra-unsupported 失败**根因未确定、不定责**。
>
> **存档 / 结算事实（只列仍需知道的）**：`a338631b…` / `7ea9929d…` known failed，**不再展开**；存档收尾
> 已完成 —— **36/36 exit 0、无残留 worker**，修 cleanup 与真实 restore-store-fail oracle，
> `runtimePersistQueue` 覆盖 checkpoint + 整个 await store、**不并发覆盖**、`atomic false`。
> `37edf80a` browser ready **已确认**（`aa5bb810` about:blank + observe 成功 → stop receipt accepted →
> owned list 无 active），用户**独立 foreground daemon 保持开启** ⇒ **不得再拿「ready check 未返回」判停**。
>
> **发布面（用户定调）**：**先只 Actions 工件，不自动 Release**。`8b282703` 已把 `build-packages` workflow
> 改为**仅显式 dispatch 且 `publish: true` 才发布、默认 `false`**，20/20 CI 政策测试 **exit 0**；push / PR
> **仍 build + upload**；**没有 release、没有 npm 发布**；四包版本值**不变**。
>
> **资源 / git**：旧 port `4660` 与 owned browser 已清，**无新 browser / server**，unknown PID **不 kill**；
> policy commit `ba2d7cd2…` 与 phase1 原生 commit `0877939…` **均已 push origin/main（same SHA）**，
> build-packages run `37228200311` / `37229247257` **success**、publish **skipped / no release**；旧「HEAD
> `d248224` 未 commit」**已错**，改为 policy 已 commit、其它 compat 改动**未 commit**，待**完整 candidate
> commit**。本批只改文档，未 commit / 未 push / 未跑 CI / 未起 server / 未开浏览器 / 未 build；提交一律
> **显式 pathspec**。
>
> **phase1 原生 commit + 音频**：phase1 原生 commit `087793905a14f15ee679b26e721c48666e2d7936`
> **已 push `origin/main`（same SHA）**，CI run `37229247459`（native）/ `37229247257`（build-packages）
> 均 **success**；但 **host 包仍不是最终 game 用**。音频 `74fc80f5…` **完成、scope 已释放** —— relay
> **21/21**、worker-audio **12/12**、集成回归 **green**，`build` / `check` / portable **545/545 exit 0**；
> Astro 生产 **7 files**、worker **56712** 与 package **逐字节同**；helper 折 RPC、**无第四 URL**；存档
> **36/36**；native `1331402` 与 source bundle 回编事实保持。**未实测 heap grow / 真实 restore 后
> new census，集成有缺，真实 game gate 仍必要。**
>
> **结算（只列仍需知道的）**：`a338631b…` / `7ea9929d…` known failed，**不重述**；本次 cleanup 一次
> failed（report **0** owned resource，root copy **未动生产**）。浏览器 `1b4a4ddb-b73f-4e20-8294-4eee0de6f804`
> **仅 `.test` 冻结 diagnostic active、无 game PASS**；用户 foreground bsk ready 仍开 ⇒ **禁止新并发 engine**。
>
> **下一步**：最终验证 —— **Flash 独立复验完整 build / consumer**（未知 id 写「执行代理独立复验，
> 未回前不作验收」）→ **完整 candidate commit** → **same SHA** CI（runtime-artemis-package + wasm）→
> warmup **2** 次 game 含声音 speaker ACK / save + restore + 推进 / pause / cleanup。**本批 doc 准备入
> 完整 candidate commit，准确记「未可玩」，无需 ID 长叙。**
>
> **纪律 / 文档边界**：lead 尽可能少做工作、**不逐文件审查**，闸门是执行代理的**实际端到端验证**。
> 上方 `[链接](.test/…)` 是仓库相对链接但 `.test/` 被 gitignore ⇒ **本地证据，干净 clone / CI 上不存在，
> 缺失不得当成「已验收」**。归档覆盖**已恢复**；11 文件隔离区 `.test/artemis-compact-spill-quarantine`
> **zero delete、trigger unknown、不删除历史**。下方历史记录 / OOB unknown 根因 / 历史 hash **一字不改**。

---

> **2026-10-05（compat CI 返工批 · 极窄 · 只修 lead 明确阻塞，未重写 300 行 guard）。**
> 本批**未 commit / 未 push / 未跑 GitHub Actions / 未起 server / 未开浏览器 / 未碰 native build**。
> 证据日志 `.test/artemis-compact-ci-final-check.log`（gitignored）；staging `.test/artemis-compact-ci-final-stage/`。
>
> **本批实际修的真实缺陷（每条都有实跑证据，不是设计意图）**：
> 1. **假 provenance（必修）**：workflow 有 `upstream_commit` 任意 dispatch override，而 `build-env`
>    的 `ARTC_PIN` 是固定 `9f62262…`。两者可被拆开 ⇒ checkout 一个 commit、recipe 归档另一个，
>    run 仍然绿。**已删 override input**，`upstream-commit` 改为字面量 `'9f62262…'`；
>    wrapper 新增 `UPSTREAM_COMMIT != ARTC_PIN` **运行期硬失败**（`UPSTREAM_COMMIT` 由 reusable
>    workflow 导出，已核实）；`test`/guard 对应改为**要求字面量、禁止表达式**，不再要求 override。
> 2. **`sha256sum ./*` 清单**：glob 写出 `./LICENSE` 而非 artifact 自身文件名，且会连带覆盖多出的
>    第七个文件。改为**显式六文件**（不含自己），并新增 `sha256sum --check --quiet` 自检。
> 3. **`rm -rf "$DIST"` 无校验**：改为 `DIST_ABS` **解析成绝对路径 + 精确根断言**
>    （`== "$REPO_ROOT/dist/artemis-compat"`）后才删，且先于创建。
> 4. **`nproc` 巨大 -j**：改为固定 `JOBS=2`（可 `ARTC_JOBS` 覆盖）。
> 5. **wrapper `set -eu` → `set -euo pipefail`**；脚本注释删掉不实说法（旧 bridge「不影响源」是假话）。
> 6. **source bundle 缺 build recipe（GPL 必修）**：上游 pin 的树**没有 `build.sh`、没有 `ports/`、
>    也没有 web CMake target**（`overlay/cmake/web.cmake` 是被 recipe **append** 进上游
>    `CMakeLists.txt` 的）。只给 patches+overlays = 一堆没有回编方法文件。现 bundle 携带
>    `build.sh`（逐字节）、`NOTICE`（列出相对上游新增了哪些文件）、`upstream-root.diff`（`diff -ruN` 自证）、
>    `LICENSE`（上游 GPL-3.0 原文）与 `third_party/lua-5.1.5/COPYRIGHT`（Lua MIT 原文）。
> 7. **Lua 路径错误**：`THIRD_PARTY_NOTICES.md` 与任何 `third_party/lua51/` 引用**均错**——
>    上游目录是 **`third_party/lua-5.1.5/`**，`lua51` 只是 CMake target 名。已改正，并在 guard
>    里禁止再写错路径（错路径＝静默不产出通知，因为 copy 失败不是 build 失败）。
> 8. **tar 绝对 Windows 路径（本批新发现并实测）**：GNU tar 把绝对路径开头的 `D:` 当**远程主机**
>    （`tar: Cannot connect to D: resolve failed`，exit 2）。generator 传给 tar 的路径必须是
>    **repo 相对 POSIX**形式；已修，否则任何机器上都是 exit 2。
> 9. **child_process 管道**：sandbox 下 EPERM 表现为 `status=null`/`error`。generator 的外呼改为
>    `stdio: ['ignore','inherit','inherit']`（不 capture），`tar -9n` 与 shell tar 行为不变。
>
> **本批验证（全部实跑，退出码见日志）**：
> - `bash -n .github/scripts/build-artemis-compat.sh` **ok**；
> - `node --test test/check-sources.test.mjs` **86/86 pass**；
> - `node --test test/artemis-compat-ci.test.mjs` **15/15 pass**（新增 fake：固定 pin、
>   workflow/recipe/pins 三处一致、sha 清单、rm guard、`nproc` 禁止、bundle 必带 recipe、
>   tar 路径、错 Lua 路径）；
> - `node scripts/check-sources.mjs` worktree **ok**（340 products；pending 仅因本批未 commit）；
> - **source bundle 真实跑两次**，同一输入 **字节相同** sha256 `0b668ebe94be…67a5`（772,440 B），
>   **不是构造断言**；tar 条目核 **无 SDK / 无 game / 无 `.ref`**，含 `build.sh`/`LICENSE`/
>   `lua-5.1.5/COPYRIGHT`/`NOTICE`/`upstream-root.diff`；
> - **bundle 单独可重编（本批最强证据）**：把 bundle 解到空目录、按 `NOTICE` 重组布局后跑
>   **bundle 自带的 `build.sh`**：从 GitHub clone pin 成功（`HEAD matches pin`）、两个 patch 与两个
>   overlay 应用、`archive-source-sha256=8e94b699…5ae3`（**与仓库内 build.log 逐字节同**）、
>   **`exit=0`**，产出 `artemis-compat.js` 150,186 B + `artemis-compat.wasm` 1,267,898 B。
>   ⚠ 但 recipe **未为 archive 重写**：它 `. "$ROOT/ports/env.sh"` 且 `OUT` 有
>   `*/ports/artemis/compat-web/out` 断言，**不能原地直接跑**，需按 `NOTICE` 重组布局
>   （NOTICE 与 `THIRD_PARTY_NOTICES.md` 已**诚实**写明这是已知限制，不吹成一条命令可编）。
>
> **接手（未做，本批不能做）**：显式 pathspec 提交 → 同 SHA 看 CI；真实游戏验收（含 SAB worker、
> audio、save/load）仍未过门；`publish: false` 保持不变。**下文「native clean build / CABI worker 13
> passed」等旧记录是历史证据**，不得当作本批验收；旧 malformed 记录是 past SHA，不引用为当前失败。

---

> **2026-10-05（compat 出货线接线）· 独立 CI 执行批已完成，只等 lead 审核后显式 pathspec 提交。**
> 本批**未 commit / 未 push**，也**未跑 native build / 未起 server / 未开浏览器**。
> 证据日志：`.test/artemis-compact-ci-check.log`（gitignored）。详细交接写给 lead，
> 见本批次子代理回报；下面只记接手所需的事实。
>
> **本批落地的事实（均已实际跑过守卫，不是设计意图）**：
> - **出货构建 = 编译上游源码**，不是 SDK 路线：
>   `.github/workflows/build-wasm-artemis-compat.yml` → `.github/scripts/build-artemis-compat.sh`
>   → `ports/artemis/compat-web/build.sh`，pin `9f622629f595db85e6f76c45c06edde29d7d9961`，
>   产出 `dist/artemis-compat/`：`artemis-compat.js`、`artemis-compat.wasm`、
>   `artemis-compat-source.tar.gz`(+`.sha256`)、`LICENSE`、`THIRD_PARTY_NOTICES.md`。
>   **`publish: false`**；**`glue-import-map` 留空**（那张表是 207-import SDK 桥的导入面）；
>   `package-dir: packages/runtime-artemis` + `package-engine-assets: true`。
> - **pin 的确传递方式已核实**：workflow 检出 pin 到 `.ref/artemis-compat`
>   （`skip-upstream-checkout: false`），并把 `ARTC_PIN=<pin>` 经 `build-env` 传给脚本；
>   脚本断言 `ARTC_REF == UPSTREAM_PATH` 后才归档。配方自身的默认 pin 与 pins 记录同值。
> - **旧 bridge 已改为仅手动**：`build-wasm-artemis-bridge.yml` 移除 `push` / `pull_request`
>   触发（旧产物与"它检出的源码内容"无关：`skip-upstream-checkout: true` 下唯一源码身份只是
>   `upstream_commit`）。**旧 code / sdk / 历史 hash 未删未改**；守卫改为禁止它重新拿回自动触发。
> - **pins 新增兄弟键 `artemis-compat`**（`dispatch: false`、`git-archive: true`、
>   `workflow`/`build-script`/`recipe`/`artifact-dir`/`files` 精确列表）；**没有**加进 `engines`，
>   因为 `engines` 会被 drift check 自动 dispatch，而上游漂移会把未审查的引擎修订与已审查的
>   补丁集配在一起。`artemis-bridge` 记录保留为 historical/diagnostic，**不是 shipping 声明**。
> - **守卫语义已从旧 bridge 改为新 shipping**：`scripts/check-sources.mjs` 现在要求 compat 的
>   pin/构建文件/workflow 契约/源码 URL/精确资产白名单，并对旧 ledger 只做存在性检查、
>   标注 legacy（340 产品**不是** shipping 要求；没有为过检禁止它）。
> - **文档已更新**：`ports/artemis/README.md`、`docs/source-layout.md`、`README.md`、
>   `THIRD_PARTY_NOTICES.md` 说明出货 compat 路线 + SAB worker；明确 **checkpoint / audio 未验，
>   不标"可玩"**；REWRITE 宪法未改，只标注旧 bridge 为 legacy scope。通知文件记录新路线使用
>   的**实际 GPL 完整源**（+ 捆绑 Lua MIT / stb / zlib 原文），**不**把 SDK notice 当成新源授权。
>
> **本批验证（全部实跑，退出码见日志）**：`bash -n` wrapper、JSON parse、
> `check-sources.mjs`（worktree **ok**；`--staged` 待新文件入索引后 ok）、
> `check:layout` ok、`node --test test/check-sources.test.mjs` **84/84**、
> `node --test test/artemis-compat-ci.test.mjs` **9/9**、YAML 用本机已装 pyyaml 解析
> （不装依赖，未跑 Actions）。**未跑** full build / native / browser。
>
> **lead 接手（未做，且本批不能做）**：
> 1. 显式 pathspec 提交本批（不 `git add .`；本批文件清单见日志），**同 SHA 观察 CI**；
> 2. native build 尚未在本批执行（配方已 `cp` 到 `.github` 侧可 create，但按批不跑）；
> 3. 真实游戏验收（含 SAB worker、音频、save/load）仍是未过门，不得宣布可玩。
> 4. **注意范围**：`packages/runtime-artemis/src/compat/**` 与 `packages/runtime-artemis/scripts/`
>    属另一 Flash（runtime Flash `a1cf67`）的在跑写入范围；本批只读引用，未改其文件。

---

> **2026-10-04（新阶段）· 路线切换：旧 OOB 主线已被当前请求暂停，旧数据原样保留。**
> 用户新请求 = **把 Artemis 的 WASM 换成「从 artemis-compact 改造出的 WASM」，参考现有 WASM 代码**。
> 现场调查已完成（只读，**未 build / 未 server / 未浏览器 / 未改生产代码**），报告：
> `.test/artemis-compact-migration-plan.md`（gitignored）。
>
> **核事实（不推断；2026-10-05 主代理审核后校正）**：
> - **命名边界**：只有「**本仓库未找到 `artemis-compact`**」（glob/grep 全树，含 `.ref/` 0 命中）。
>   **不得**说"互联网/世界上不存在 `artemis-compact`"。历史目标名 = **`artemis-compat`**
>   （`Weiss-UltimateSavior/artemis-compat`, GPL-3.0, C++17 ≈30K 行）。
> - **源身份（只读实测）**：`.ref/artemis-compat` HEAD = `a4f8a257cf87cb8f65ab4e3f83501ab327a55424`，
>   worktree **dirty**（12 tracked changed + 3 untracked：`web_main.cpp` / `web_page.html` /
>   `web_worker.js`）；CI pin = `9f622629f595db85e6f76c45c06edde29d7d9961`
>   （`.github/workflows/build-wasm-artemis-bridge.yml:99`）。`a4f8a25` 是 pin 的**前一个**点
>   ⇒ **任何构建必须按 pin 重取源码，不得在 `.ref` 上改或 build。**
> - **上游不自带 Web 宿主**：`git ls-tree 9f62262… src/host` **只有 `mac_main.mm`**；pin 的
>   `CMakeLists.txt` **只有 native targets**。⇒ **Web host / Web CMake / `ReadAt` / numeric 全部是
>   本地 port 产物**，不是上游自带。宿主三文件是 **untracked 本地文件**，不是上游文件。
> - **历史证据仅历史**：`docs/artemis-web-port.md` §9.18 的长跑与 7 张截图
>   （`frames 961 presented 553`、按需读 0.41%）**只是当年的历史证据**，
>   **不保证**当前新源码或新 CI 候选已可用。
> - **退役根因仍 unknown**：文档原文「compat 这条线的**输入缺陷长期收敛不掉**」，
>   其**精确根因现有文档不足以回答 = 本任务最大已知未知**。**「编译一次就能消解退役原因」是错误推断**
>   —— 编译只证明**能编**，不证明输入缺陷被收敛。须以真实游戏实测为准，不得靠读文档推断。
> - 环境（只读实测）：`/d/emsdk` emcc **6.0.9**（与 CI 一致）、cmake 4.3.1、node v26.8.2、
>   pnpm 9.14.2；`netstat` 在 9222/4700/8080/3000/4321/5173 **无 LISTEN**；
>   `git status --short` / `--cached --stat` **均空**；HEAD `d248224`。
> - 旧 `42181662` 任务与现场 node/chrome PIDs **均未确认**（该 id 非本会话子代理，`job_list` 空）
>   ⇒ **不假定在跑也不假定已停，不写成已 cleanup**。
>
> **接缝审计结论（`.test/artemis-compact-seams-review.md`，静态只读、零运行证据）必须按其原文边界引用**：
> - 该报告**无任何运行证据**，不得当验收。
> - `SaveSnapshot` 写的 **ARCV 变量银行是检查点**，依赖游戏自己的 `onSave`/`onLoad` 脚本框架，
>   **并非 BOWS 导出**；**完整恢复必须真实游戏实测**。
> - **audio 在非 Android 分支是 stub**（emscripten 走 stub，`PlayStream` 直接 false）⇒ **必须新增**，
>   不是接线。
> - **`GameSource.readRange` 是 async，不能与同步 XHR「天然等价」**；compat 引擎侧要求同步读，
>   方向相反。可行方向是 **worker 阻塞 + SAB 镜像**，且需要**宿主供数**；`SyncFileBridge` 方向相反，
>   不能原样复用。
>
> **最终宿主帧契约（主代理定，不得退让）**：仍由 **host tick 驱动既有的 `artc_web_frame`**；
> **不使用** `web_worker.js` 自己的 rAF；**也不照旧 runtime 的 no-op 错误继续跑第二个时钟**。
>
> **冻结项不变**：`MapPoint` 数学、`docs/fetch-engine-artifacts.md` 历史 hash，
> 以及**源码 / 构建 / 合成 / 真实游戏 / 许可五层证据**分离判据。
>
> **pending（下一对话直接接手）**：native build + 正确源 pin 补丁身份 → 小 CABI freeze →
> Flash adapter 与独立测试 → **必要升级 Grok** → **CI 同 SHA 真实热身 + 两次一致验收**。
> 本会话新起的 **2 个调查代理已 settled，无 job / 无 browser**；总领**只审核、不执行细活**。

---

> 2026-10-04 19:05 +08:00（**以下为旧 OOB 主线内容，已被上面的新请求暂停；数据全部保留**）。**主代理不承担任何具体工作**（用户明确定调）：编码/文档/构建/浏览器/测试/清理**全部子代理**，主代理只做**任务拆分顺序 / 证据审核 / 最后验收**；以前安排 lead 亲跑的 next steps **必须改分配执行，不得倒退**（已记入 AGENTS.md 硬约定）。**子代理默认 Flash，Flash 不能胜任的复杂工作交 Grok 4.7，其他模型按需；交接已测证据/失败原因/待办，不对同路由反复无效重试**（用户明确定调，已记入 AGENTS.md + 本文件§1）。旧EH入口22515/r602→21115/r616→21127/r616/tag10逐层实测修复；discard热身后两次COPY冷跑EH0，资源/IDB继续后遇raw allocator OOB，**仍不可玩**。**bf014fa已push，同SHA bridge[37194261586]/packages[37194261493]均success**；本地portable **346/346**+checks通过。下载bridge sha **e16c8aa203391dee16ac10141e228b0e16544992e6a0ddabc419437ae2012690**（23,666,735B）manifest与磁盘一致；下载完整TU sha **e94b2b6f...32db4f与本地reviewedTU逐字节同**；host 0.1.9 tgz sha958377...de129已解包 `github-bf014fa/packages-artemis-extracted/package`。**本轮OOB配方**：5c615完成correctu32getterprep、46df完成插件/server/build，**未compile**；产物 root `.test/artemis-resume-oob-reviewed-build-new/`，TU sha **331516aa22d08d9c7bbad714bae5bc23cff1ccdb0a6bcb45986c5d0d2aa7a036**；主代理**独立静态审核通过**（findings `.test/artemis-oob-final-review.md`，gitignored），**无返工缺陷**。
> **review verdict：root cause 仍 unknown；docs 可显式 pathspec commit（仅两 docs）；不宣布引擎可玩。** 另一低并发 Flash 正做 bf014fa CI unmodified host **3 cold run**，只写 `.test/artemis-bf014fa-ci-*`，**不写 doc、不要碰它**；本轮**不起任何 build / game server / browser**以免资源冲突。
>
> **执行交接（Grok 未完成；现场未知，不编清理）**：
> - Flash 执行 `865498f5-7a63-4a0d-966c-d8cd96e3943b` **failed、无 closing**；文档更新那次 Flash 调用也 **runfailed、无落盘 receipt**。
> - 已升级执行子代理 `42181662-c350-42f5-b11e-3dc9923f91ba`（magpie/avemujica-grok/grok-4.7，high）**正在跑，不得写成已完成**。
> - 它先确认失败任务可能残留的 jobs/browser/captures（**是否残留未知，须核 ownership**），再对 bf014fa 已下载 CI bridge/runtime + pristine 游戏做热身 1 + 冷跑 2 + cleanup，只写 `.test/artemis-bf014fa-ci-*`。
> - **同 SHA CI 实际复测尚未验收。** 已测仍只是本地 COPY：旧 EH0、新 allocator OOB；**Artemis unavailable 不变。**
> - 上文「另一 Flash 正做 3 cold run」已被这次 failed 取代，不要再当它在跑。本检查点写入方不读该子代理工具状态、不终止进程。

## 1. 用户要求与接手纪律

- **主代理不承担任何具体工作（用户明确定调）**：编码、文档、构建、浏览器实测、测试、清理**全部由子代理执行**；主代理只负责**任务拆分顺序 / 证据审核 / 最后验收**。以前安排 lead 亲跑的 next steps **必须改分配执行，不得倒退**。
- **子代理模型分工（用户明确定调）**：具体代码/细节默认 **magpie/group/auto-deepseek-v4-1-flash**；**Flash 不能胜任的复杂工作交给 avemujica/grok-4.7**（supported reasoning effort low/medium/high/xhigh），其他模型按需分配。Lead 已用 `list_subagent_models` 核验这两个 route 真实可用（route 事实，不是能力排名，不据此编造专长）。不确定模型擅长什么时先搜索或小测试。发现当前路由做不成时，把**已测证据 / 失败原因 / 待办**交接给升级模型，**不对同一路由反复无效重试**。低并发（最多两个窄任务）。不未经明确请求建 Agent Teams。
- 新对话先读本文件，再读 `docs/session-5697f1a7-handoff.md` §4.3.3/§6/§7。每阶段提交并push恢复检查点，不能依赖旧会话。
- 工作区 `/d/online_gal`，main，shell Git Bash（pwsh工具实际bash）。生产代码只在packages/apps/ports，tracked portable tests在test；.test/.ref ignored。
- 同时只跑一个引擎实例。浏览器仅BrowserSkill browser_*，已验证Chromeinstance **aa5bb810**。不用CDP/evaluate后端，不碰用户9222 chrome-mcp或用户窗口。
- 不吞r(0)/不跳trap/不造异常状态/不扩大memory盖住OOB。构建成功、合成测试、真实游戏可玩、再分发授权不同；未测就未知。
- 验收=同sourceSHA的CI产物+独立现代宿主+pristine游戏，discardwarmup后两次一致并含输入/pause/resume/save/load。当前冷跑仅证明旧EH入口不再触发，**没有draw/audio/输入/菜单存取验收**。
- 官方SDK/其转译体有专有再分发风险；不提交SDK/游戏/诊断完整TU/制品。冻结鼠标映射和历史hash不动。
- 当前goal `goal-4c733db7-f628-4026-a0ef-4b5bd99509e1` revision2，active/armed，objective为Artemis主线直到同CI真实验收；下一对话查自己的goal，不复制id。
- **提交纪律**：文档可用**显式 pathspec** commit/push 两个 docs（`AGENTS.md` + `docs/current-work.md`）；**不带代码/SDK/本地 probe** commit。提交前先 `git status` / 读 staged，**不能带无关 staged 内容**。
- **BSK daemon pwsh-834**（owner）只在**末阶段**做 delegated cleanup。

## 2. Git、生产与当前写入范围

- 最新**引擎修复 bf014fa0ddc4862ccfb4c76eca1198bae8769289 已commit/push**：scope+616双门，runtime0.1.9/engines0.1.10、portable346/346；之前77a7645的603相位修复保留。Artemis仍unavailable（paired重复OOB），不是完全修复。
- 同SHACI**bridge[37194261586] / packages[37194261493]**均**success**。下载bridge23,666,735B sha**e16c8aa203391dee16ac10141e228b0e16544992e6a0ddabc419437ae2012690**，manifest与磁盘一致；下载完整TUsha **e94b2b6f...32db4f与本地reviewedTU逐字节相同**；host 0.1.9 tgz sha958377...de129已解包 `.test/github-bf014fa/packages-artemis-extracted/package`。**必须此SHA新制品再量測，不能用77旧**。当前trackedsource已commit，仅滚动docs更新（以现场git为准）。
- **本轮OOB配方进度（未compile）**：`5c615`完成correct u32 getter prep；`46df`完成 plugin/server/build。产物 root `.test/artemis-resume-oob-reviewed-build-new/`，TU sha **331516aa22d08d9c7bbad714bae5bc23cff1ccdb0a6bcb45986c5d0d2aa7a036**，记录6word addrLo/addrHi/width/limitLo/limitHi/count。失败前record、随后仍原`TRAP(OOB)`；predicate/guest业务字节不改；getter只读返回u32。
- **主代理独立静态审核通过**（findings只写 `.test/artemis-oob-final-review.md`，gitignored不入库），三项PASS：①制品身份+**逐字节字面量反演证明**（输出=输入+三处字面量编辑，delta595；从两份TU提取原文证明而非手打转义）；②getter只读/无副作用/两处TRAP(OOB)保留/谓词逐字不变/无额外EHguard/无非法地址读/getter不写counter；③host接线与顺序（reviewed plugin包delegate onLoad，故once()作用于delegate编辑后文本；对真实下载host锚点各计1；`__eo`在suspend.js作用域内；fail dump在driver throw捕获RuntimeError前读counter，原throw保留且只once）。**无返工缺陷**；仍**未compile/未link/未跑host/未跑browser**。
- **另一CI caller pending task**，**id未知**，先按责任描述记（不编造task id）；**owner BSK daemon pwsh-834 只在末阶段做 delegated cleanup**。
- **Flash043b50b4-8e18-4dc1-88b4-f2f6f0040ead 已完成** 独占 `ports/artemis/src/script/lua_tags_builtin.cpp` + `test/artemis-builtin-control-flow.test.mjs`。已授权落入**已独立COPY差分实跑验证**的四scope+616双门修复，未授权git/全build/浏览器/版本。必须独立审diff并比对其prod文件等于reviewedCOPY embedded product，不能重复派相同写范围。
- **Flashb18cfca5-ce5b-4940-95d2-2af2bac2b604 failed、无配方文件**，不重复长任务；下一阶段OOB继续窄准备。30fc超小prep虽生成，但**写错newroot.test/writeonly.test非ignored路径，getter竟void且写counter而非读**，主代理拒绝不build，已核精确3files后搬至 `.test/artemis-rejected-oob-newroot` / `.test/artemis-rejected-oob-writeonly` 隔离，不将假探针commit；下阶段必须getter u32只读。最新极小修正Flash **5c615cd7-2fda-4bcf-a206-4436ec9cdba6** 仅 `.test/artemis-resume-oob-reviewed-prepare.mjs` + newrootTUmap，复制隔离错误配方只修6wordvolatileu32/read-onlyexportgetter/正确.testpath，不build/plugin/server/git生产；node生成后主代理独立审，原rejected文件不跑。原scope 独占 `.test/artemis-resume-oob-{prepare.mjs,plugin.mjs,server.mjs,build.sh}` + newrootTU/probe-map。基于immutable reviewedpairedTU仅在MEMCHECK失败前highbridgecounter记录addr/width/limit，随后仍原TRAP(OOB)。准备，不执行build/browser/server/git。没有guest地址非法读/写、没有修复值。未完成即failed，无文件；下一阶段从immutablepairedinput窄准备，别使用不存在配方。
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
- **BSK foreground daemon pwsh-834 PID60668仍活动**（owned，只为后续诊断；**只在末阶段**由 owner 做 delegated cleanup：用bsk daemon stop再collect，不碰其它服务）。browser_session list无活动。
- **资源冲突隔离**：另一低并发 Flash 正在 bf014fa CI unmodified host 做 **3 cold run**，只写 `.test/artemis-bf014fa-ci-*`，**不写 doc、不要碰它的文件**。本轮**不起任何 build / game server / browser**，避免与它抢资源。该句已被文首交接取代：该 Flash 已 failed。
> - **下一步 0（挡住并行）**：Grok `42181662-c350-42f5-b11e-3dc9923f91ba` 未完成前，**不要另起引擎实例，也不要开 OOB build**；不碰它的 `.test/artemis-bf014fa-ci-*`、capture、server。残留是否存在**未知**，未核 ownership 前不编已清理。
> - **验收门未过**：CI 实际复测没有落盘证据。不得把本地 COPY 的旧 EH0 / 新 allocator OOB 写成 CI 复验通过；Artemis 仍不可用。
- 所有代理已settled，生产/测试/版本三范围已独立审。oneshot ids不能send_message，别拿旧失败probe当完成品。
- 最终qualityjobs **pwsh1635/pwsh1644/pwsh1666**已exit0并收集，346portable0skip，TS/Astro0error，layout/source通过；没有全树style假绿。
- 下一步1（**改由子代理执行，主代理只审核证据**）：显式pathspec commit/push两个docs（`AGENTS.md`+`docs/current-work.md`，不带代码/SDK/本地probe；先`git status`/读staged，不能带无关staged），观察**同commit**CI，下载artifact解包身份核验/ABI/真实fresh量測，不能拿77旧artifact代替。
- 下一步2（**改由子代理执行**）：OOB reviewed配方已静态审核通过，进入compile+run+真实冷跑量测；下轮从immutable paired input窄准备（不使用不存在配方）；**只测**失败前addr/width/limit，不改变TRAP、不扩大guestmemory、不吞r(0)。如发现缺陷**只report、不修改任何OOB文件**；lead验收后再派implementation。
- 下一步3：新allocator坏addr按源/官方逐层查，输入/pause/save完整验收仍未到达。维护本文件与AGENTS，不宣布goal完成/blocked（仍有可推进工作）。
