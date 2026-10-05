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

## 当前状态（2026-10-05 · doc-only · 未 commit · 仅此 2 文件）

- **prod writers 全 released**。`df13cc46` 已 closing：`8e9fed8f5c385a4ff5f3085ad0d34ad459a0d236` parent `1dc4006`（21 paths；pin `17200` active；2 PlatformPatches；runtime `0.1.13` / engines `0.1.14`）。non-force push origin/main actual same 0/0。真 index check 0；退休两 scene files 已 tracked 删除。root 115 / source offer 3 / staged source 0。新 native `1477363` / JS `153831` / source `65f` 纯 src 独立 RUN 0 cmp byte-equal。
- **`11b2250d` / `b83444de` 已 closing**（full app；portable 568/568；source 340；pkg `2437785` SHA `2757…`；offline consumer 0）。post-doc 中途改动不是 block，不当 active code writer。
- **已回未过**：`b0c94a06` closing。exact `8e9fed8` CI native `37263314120` **FAILURE**（原生编译 exit 0：JS `8fc9` / WASM `1477363`；Linux log hash `5aa` 仅日志、工件未确认）。失败点是 GPL source offer：staging 用 `.test` fresh ref 目录，CI checkout `.ref/artemis-compat`，sourcegen 默认 `.ref/artemis-compat-refresh/src`，双 path 传错，不是 native 算法错。package `37263313647` **SUCCESS** 只是聚合工件，不能代 native（native artifacts 0；新独立包未验，不得写 CI 全过）。`3c70ac0d` 仍仅 `.test` 新手测 prepared、未 closing、无任何 port/browser。
- **冻结**：`:4661` PID `41648` freeze `9a` 用户手测保留，不 stop/restart/操作。CI 后另起 `:4662`；不造 fake proof、源 SHA 不当 repo。goal paused；直接执行；无自动 3-run。语音 normal、MapPoint、audio hash 不变；不动态修动画。无 GH Release / npm；旧 fetch 历史 hash 不动。
- **next**：`9053f6fe` 是唯一 source/CI path writer：wrapper 传已核实 REF，或走 generator 已有 param，最短修 source offer + compat CI case。未返不能写 fixed。原音频 math / 新 172 source 逻辑不改。实测 path case 0 → small fix commit → 新 exact CI 校 native assets 0 / manifest / source offer / offline consumer → 新 manual `:4662`，留旧 `:4661`。本地 source `65f` / RUN 0 / 568 / 44 仍真实，身份与 CI 不同，不抹。goal paused，直接执行，无自动 3-run。
