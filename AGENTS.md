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

## 当前状态（2026-10-05 · doc 收尾 · 源码 73 不动 · 游戏未验）

- 产品 writer 已 closing。代码 `73c49188f26f0fbf13b71dc5549f37affa479974` parent `8e9fed8` 已 push，origin/main 同 SHA。fresh clone pin `172` ≠ repo SHA。source 纯 git archive；两 active patch；最小 WASM。runtime `0.1.13` / engines `0.1.14`。
- native CI `37265688704` / packages `37265688380` success。`.test/artemis-compat-refresh-final-ci/proof.json` status verified。CI actual 569 tests / pass 567 / skip 2 / fail 0（local 此前 568/568，source 新 case +1）。不重 build/native。
- 下载冻结 CI WASM `1477363` sha256 `5aa135ecfa0d36acb267d23e0a95e6f0dae42aae57c8cabf4259cfe4facbfc67` / JS `1538318` `fc9…`。Windows 与 CI 不同不能混。完整纯 source offer 平台 hash 不同，只记 actual proof，不硬 copy `65f`。source 对应源码自带独立 RUN 本地 0：same JS/WASM 实通过；音频算法及冻 MapPoint 未改。
- 新版 `http://127.0.0.1:4662/` PID `2612` 与旧 `http://127.0.0.1:4661/` PID `41648` 各 GET `/` **200**，有意留给用户亲测。不自动 3-run。旧声正常不以旧结果证明新版动画已修。仅旧 scene 非平台扩展退役；现 basic ARCV checkpoint 非 full scene。用户 START 与真实游戏效果待确认。
- Artifacts-only；default publish false；无 GH Release；npm/SDK 不入库。goal paused 不 resume。port/build/CI/入口完成，**不说全游戏验收 passed**。本批只这两 doc。
