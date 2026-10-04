# AGENTS.md — D:\online_gal

浏览器端多引擎视觉小说（galgame）模拟器：把**真实引擎**编译成 WebAssembly 在浏览器里跑，前端固定 **Astro**，宿主是 Canvas 2D + Web Audio + TypeScript。许可证 **GPL-3.0-or-later**，仓库 private，包不发布 npm。

## 接手先读（按顺序）

1. **`docs/current-work.md`** — 滚动工作检查点：当前任务、已测证据/未知点、未提交改动、代理写入范围、进程清理与下一步。**每次新对话先从这里接手，随阶段更新，不依赖旧对话恢复。** 文首「当前摘要」块是**唯一现状**，其下按时间倒序的历史记录不得当现状。
2. **`docs/session-5697f1a7-handoff.md`** — 上一开发会话的交接总结（2026-09-17 → 2026-10-04，266 轮）：历史现状、**已被推翻的结论**、构建/验收命令与证据标准。不要重读原会话。
3. `docs/项目状态与恢复指南.md` — 更早的能力矩阵与实测数据。
4. `README.md` — 支持范围与「可运行 / 仅识别」的真实边界，不要读成「全都可用」。
5. `docs/source-layout.md` — 目录职责、上传清单与检查方式。
6. `ports/artemis/REWRITE.md` — 手写 Artemis 重写的风格宪法与「必须不变」的契约。

## 构建、检查、测试

```bash
npm run build:packages   # tsc -b tsconfig.packages.json（+ runtime-ons 资产拷贝）
npm run build            # 上面的 + Astro 前端
npm run check            # check:layout + 包 TS + 前端 check
npm run check:layout     # 违规判定：packages|apps|ports 里引用 .test/
npm run check:sources    # 上传清单 / 忽略规则 / Artemis 台账守卫
npm test                 # check:layout + build:packages + scripts/run-tests.mjs
npm run test:portable
```

要求 Node >= 20 与 pnpm 9.14.2；本机 shell 是 Git Bash（`pwsh` 被接到 bash，写脚本用 bash 语法）。

## 硬约定

- **布局**：所有模拟/移植代码放 `packages/`、`apps/`、`ports/`（必须入库）；只有测试、真实游戏、一次性脚本、截图放 `.test/`，参考检出与官方 SDK 放 `.ref/`（两者都 gitignored）。`npm run check:layout` 会拦住漂移 —— 历史上正是这条被违反，导致整个 WASM 端口一度无法构建。**任何上游 archive / tar 解包只允许落到已明确校验的新建 / 已 guard 的输出子目录** —— `.test/<dir>/`、`.ref/<dir>/`，或 `ports/<port>/out/<dir>/` 这类**事先断言过路径的构建输出目录**（正式 ports 的 `out/` 构建输出因此照常允许），**绝不允许解到仓库根**；根目录的 `AGENTS.md`/`README.md`/`LICENSE`/`THIRD_PARTY_NOTICES.md`/`.gitignore` 是本仓库的，被上游同名文件覆盖过一次。解包前必须断言目标绝对路径落在上述前缀之下，断言失败即退出非 0；不要让空变量或仓库相对路径变成落点。
- **提交**：用显式 pathspec（`git commit -F -` 不带 pathspec 会把别人的 staged 删除扫进来）；提交后**及时 push**，并在 GitHub 上观察**同一个 commit** 的构建。
- **证据纪律**：目录完整、构建成功、合成测试通过、真实游戏可用、再分发授权是**五个不同结论**。真实游戏测试是唯一 oracle，且**单次运行不算判据**（丢弃一次热身 + 两次一致）。不得把官方胶水诊断、组件单测全绿、全生成掩码桥健康当作引擎可用。
- **许可**：自有代码 GPL-3.0-or-later；官方 Artemis SDK 及其转译体**专有、有独立再分发风险**，改名或附 GPL 都不算解决，不要提交进仓库。
- **浏览器/游戏实测**：同一时刻只跑一个引擎实例；收尾关掉自己开的窗口、标签页、诊断服务器与后台 job（窗口堆多了会把机器卡死）。
- **子代理与主代理分工（用户明确定调）**：**主代理不承担任何具体工作** —— 编码、文档、构建、浏览器实测、测试、清理**全部由子代理执行**；主代理只负责任务**拆分顺序 / 证据审核 / 最后验收**。以前安排给 lead 亲跑的 next steps **必须改分配执行，不得倒退**。具体代码与细节默认由 `magpie/group/auto-deepseek-v4-1-flash` 承担；**Flash 不能胜任的复杂工作交给 `avemujica-grok/grok-4.7`**，其他模型按需分配；不确定模型擅长什么时先搜索或做小测试，不编造专长。发现当前路由做不成时，把已测证据 / 失败原因 / 待办交接给升级模型，**不对同一路由反复无效重试**。低并发、明确写入范围。每阶段更新 `docs/current-work.md`，让新对话立即接手。**主代理尽可能少做工作，不逐文件审查；最终闸门是执行代理的实际正常工作 / 端到端验证，不是 lead 逐文件读代码。**
- **不要动被用户冻结的东西**：例如 m03520 冻结的鼠标映射缺陷；`docs/fetch-engine-artifacts.md` 里的历史 hash 不得改写。

## 当前状态（2026-10-05 · compat 主线）

- **旧 OOB 主线已暂停**：Allocator 越界根因仍 **unknown**（不扩 memory、不吞 r(0)、不放宽检查）。**Artemis 旧路线仍不可用**。
- **新主线 = artemis-compat 出货线**（`.github/workflows/build-wasm-artemis-compat.yml` → `.github/scripts/build-artemis-compat.sh` → `ports/artemis/compat-web/build.sh`，pin `9f62262…`，`publish: false`；发布面见下方「发布面」条）。上游 pin 的 `src/host` 只有 `mac_main.mm`、CMake 只有 native targets ⇒ **Web host / Web CMake / `ReadAt` / numeric 全是本地 port**。
- **原生最终候选（最新可确认，build 证据，不是游戏证据）**：WASM `1331402` B / SHA `9c4a8808…`，JS `153633` B / SHA `9fdc75b6…`；源码包 `784402` B / SHA `fe2e2007…`，独立解包用自带配方重编 **exit 0** 且与候选**逐字节同**，source / manifest 完整通过。
- **已测（测试）**：signed PF **node 3/3 + probe 30 checks**（旧写「8/8」是错的）、read host **28/28**、worker **16/16**、`mainAudioRelay` **15/15 用 fake**（**未听见声音**）、Astro hooks **6/6**（**实际 Astro build 未跑**）。版本 `engine-api`/`core` `0.1.1`、`runtime-artemis` `0.1.10`、`engines` `0.1.11`；offline frozen-lock **exit 0**；应用 workspace link 已修。全量 `npm test` 2 个 legacy extra-unsupported 失败**根因未确定**，**不定责**。
- **phase1 原生 commit + 音频**：phase1 原生 commit `087793905a14f15ee679b26e721c48666e2d7936` **已 push `origin/main`（same SHA）**，CI run `37229247459`（native）/ `37229247257`（build-packages）均 **success**；但 **host 包仍不是最终 game 用**。音频 `74fc80f5-3fdd-4a98-807d-7f3eaff0cf75` **完成、scope 已释放**：relay **21/21**、worker-audio **12/12**、集成回归 green；`build` / `check` / portable **545/545 exit 0**，Astro 生产 **7 files**、worker **56712** 与 package **逐字节同**，helper 折 RPC **无第四 URL**；存档 **36/36**。**未实测 heap grow / 真实 restore 后 new census，集成有缺，真实 game gate 仍必要。**
- **结算现状**：`a338631b…` / `7ea9929d…` **known failed**，**不再展开叙述**；本次 cleanup 一次 failed（report **0** owned resource，root copy **未动生产**）。浏览器 `1b4a4ddb-b73f-4e20-8294-4eee0de6f804` **仅 `.test` 冻结 diagnostic active、无 game PASS**；用户 foreground bsk ready 仍开 ⇒ **禁止新并发 engine**。`37edf80a` browser ready **已确认**（`aa5bb810` about:blank + observe 成功 → stop receipt accepted → owned list 无 active），用户**独立 foreground daemon 保持开启** ⇒ **不能再以「ready check 未返回」为由**处理/判停。存档 **36/36** 与原生 WASM `1331402` / 源码包回编**事实保留**。
- **发布面（用户 2026-10-05 定调）**：**先只 Actions 工件，不自动 Release**。`8b282703` 已把 `build-packages` workflow 改为**仅显式 dispatch 且 `publish: true` 才发布，默认 `false`**，20/20 CI 政策测试 **exit 0**；push / PR **仍 build + upload artifact**；**没有 release、没有 npm 发布**。四包版本值**不变**。
- **资源与 git 实况**：狭义旧 port `4660` / owned browser 已清，**无新 browser / server**，unknown PID **不 kill**。policy commit `ba2d7cd2…` 与 phase1 原生 commit `0877939…` **均已 push `origin/main`（same SHA）**，build-packages run `37228200311` / `37229247257` **success**，publish **skipped / no release**；旧「HEAD `d248224` 未 commit」**已错** —— 现为 policy 与 phase1 已 commit，其它 compat 改动**仍未 commit**，待**完整 candidate commit**。本批只改文档，**未 commit / 未 push / 未跑 CI / 未起 server / 未开浏览器 / 未 build**。提交一律**显式 pathspec**。原生候选 WASM `1331402` / source bundle 回编同、versions **原样保留**。
- **下一步**：**Flash 独立复验完整 build / consumer**（执行代理独立复验，未回前不作验收）⇒ **完整 candidate commit** ⇒ **same SHA** CI（runtime-artemis-package + wasm）⇒ 真 game 丢弃 warmup 后 **2 次一致**，含声音 speaker ACK / save + restore + 推进 / pause / cleanup。文档**不假称已可玩、不假称已听到 audio**。
- **证据路径**：`docs/current-work.md` 里的 `[](.test/…)` 链接是仓库相对链接，但 `.test/` 被 gitignore ⇒ **本地证据，干净 clone / CI 上不存在，缺失不得当成「已验收」**。隔离区是 `.test/artemis-compact-spill-quarantine/`（非 `.test/spill-quarantine/`）。下游 pending 见该文件文首摘要块。
