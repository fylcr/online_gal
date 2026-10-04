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
- **新主线 = artemis-compat 出货线**（`.github/workflows/build-wasm-artemis-compat.yml` → `.github/scripts/build-artemis-compat.sh` → `ports/artemis/compat-web/build.sh`，pin `9f62262…`，`publish: false`）。上游 pin 的 `src/host` 只有 `mac_main.mm`、CMake 只有 native targets ⇒ **Web host / Web CMake / `ReadAt` / numeric 全是本地 port**。
- **CI（exact `c609667`）**：native `37237267223` + package `37237267118` **completed / success**；**555 / pass 553 / skip 2 / fail 0**。**artifact API verified**：package `7a576718a9630187c16cbad622047b4151f73d29c63686421e98a0d221e9d581`、**manifest 6/6**；**source PIN `upstream ≠ repo`**；**SDK game 0**；**offline consumer 2 entry / 0**。native `5f934…` / **1331402 B**、JS `9fd…` **同旧** ⇒ **new pkg 只 6 条目变（version + worker-host）**。
- **CI 回归纳管**：**HEAD = `origin/main` = `c609667e61c2f1b4a378b4f85cd542370c4aaa9e`**（parent `a4b6ef3`，runtime `0.1.11` / aggregate `0.1.12`）**已 push same origin/main**；**code candidate = `c609667`**（旧 `93` 只是历史证据）。**⚠ 绝不拿旧 `93` game 自动覆盖 ACK 版本。**
- **Astro / HTTP**：preview **上游限制**；**static 正常**。**不 custom preview 框架**。
- **game / browser（`a5b4f6ea` 已正式 closing · NOTPASS）**：旧 `93` 包实验 `wyaq` 热身真 START 入 ADV、save ARCV **`26959 B`**、推进**不同对白**；`restore await` 日志 `layers=0`、对白**回首句**但**卧室背景黑**，再次推进**对白正常背景仍黑**。**`runA/B` 未跑 ⇒ 不能一致验收**；report `.test/artemis-compact-ci-final-game/report.json`。**owner 已明确 cleanup accepted**（stop `wyaq`、capture `dd6909d4f8733` export、job `pwsh` `5004` pid `26052` killed、`4661` no LISTEN），**scope 释放**；**现场无 active owned instance，不再 wait 旧关闭**。**INFO 的 draw/save 误落 fatal 槽 ≠ native error**（**不重 frozen Mapping failed**）；`bfa4diag` 已实 START `140,136` → real ADV、**MapPoint untouch**、**diag closed clean**。新 **`c609`** 仍是 **candidate，不是可玩**。
- **恢复候选准备入库，game 尚未过；所有具体 writers 释放**：`337196c4`（根 source guard + `check-sources.test` fail-closed **closing**，**88/88 工作树、staged 0、scope 释放**）、`289cc11a`（宿主 `worker-host` / `compat-worker` + save/format 测试兼容新 **ARCV + ARCL 分段 CRC**，**closing**，**不改 native / mapping**）、`653d0c64`（ports patch / recipes / native probe / guarded `out/dist`，**closing**）、`bd9dc6c1`（ONLY `.test` 探针，**closing**）、`081edf3f`（受管 compat-ci case，**closing**）**全部已释放**。**唯一未 release = `34cec20f` 的 `.test`-only final harness prepared —— 不碰、不等待。**
- **504 最终稳定 full gates**（`5045f18e` · **首次 mid-write 作废，不用旧 logs**）：`.test/artemis-compact-scene-candidate/logs/` 真实 `build` 0 / `check` 0 / `test:portable` **564 pass / 0 fail** / `check:sources` **89 pass / 89**；独立 source **RUN-BUILD exit 0**、**same both 测过**。两 manifest 各 bump：runtime **0.1.12** / engines **0.1.13**（private + GPL，无 npm publish）。
- **native 证据（`653` 新 body）**：baseline fails **2 → 14 tests PASS**；**WASM** `ac3c3bdd7f9e9cd359fcd4aca2454cfdb68ad0de5e689a8d16340b58fcb13790` / **JS** `799a61330a553e9816c9ef92b93532e4337d05c94d6f6840f99006b4e18dd7f8`。**最新 SOURCE** `371b4334c1aca6dc6cc3c1c5bc721a62c47a28027697a4baa46db3fcac35f7a9` **与 native `653` 初 `de16` 不同** ⇒ 只 record **最新源 generator + patch / overlay 真实齐备**，**不冒 copy 旧 hash**。`504` staging `dist` 原旧 native 已换 `out/build` 新 body 并重跑 `source.manifest` verify，**不 commit `dist`**。**RPC new** `08f0d2071a46a9a69f746e195f504e5e77e8f40a711e98b46912b91ecb679bd6`：`app` / `package` / `dist` **byte-equal**。runtime package **0.1.12** local `2294390` sha `e80299df2abc689d7b0e432d36645c4802195c9dbf899fa59fcd16758123a41c`：**离线 fresh root / compat import 0 / 3 URL offer 齐**。**这是本地 candidate：未真实 game，未可玩。**
- **next**：**candidate commit（显式 pathspec）→ same SHA CI → same SHA artifact download → browser 热身 ×2 → 真实背景恢复 / 继续**。**目标未完成**（game 尚未过）。**不生产源码实现调整 / 版本再增 / 全构建 native 重编 / 服务器 / 浏览器**。**不新造 custom framework / node cache 全 audit**，**不把旧 `500` start 失败重写成当前阻塞**。**旧真实 game 正式 NOTPASS（背景黑）**；owner cleanup scope 已释放，**0 active / port free**。
- **发布面**：**用户 only —— Artifacts only、`publish` 默认 `false` 保持**；**no GH Release / no npm publish**；**source hash 历史 divider 不动**。**unknown PID / bsk leave**，**user foreground 不 kill**。`.test/` gitignored ⇒ 本地证据，缺失不得当「已验收」；**doc 文件 dirty only ⇒ 别 claim whole clean**。
