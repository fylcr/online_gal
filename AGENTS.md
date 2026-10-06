# AGENTS.md — D:\online_gal

浏览器端多引擎视觉小说（galgame）模拟器：把**真实引擎**编译成 WebAssembly 在浏览器里跑，前端固定 **Astro**，宿主是 Canvas 2D + Web Audio + TypeScript。许可证 **GPL-3.0-or-later**，仓库 private，包不发布 npm。

## 接手先读（按顺序）

1. **`docs/current-work.md`** — 滚动工作检查点：当前任务、已测证据/未知点、未提交改动、代理写入范围、进程清理与下一步。**每次新对话先从这里接手，随阶段更新，不依赖旧对话恢复。** 文首「当前摘要」块是**唯一现状**，其下按时间倒序的历史记录不得当现状。
2. **`docs/artemis-official-compat-handoff.md`** — 当前官方 4664 ↔ compat 对比交接：目标、立即下一步、已测 vs 未知、身份与入口表、在途写域与规则。**新对话读它，不读旧交接当现状。**
3. `docs/session-5697f1a7-handoff.md` — **历史**交接总结（2026-09-17 → 2026-10-04，266 轮）：历史现状、**已被推翻的结论**、构建/验收命令与证据标准。不要重读原会话，也不要逐句覆盖旧记录。**该文件当前为 modified 未提交状态，本轮不 stage。**
4. `docs/项目状态与恢复指南.md` — 更早的能力矩阵与实测数据。
5. `README.md` — 支持范围与「可运行 / 仅识别」的真实边界，不要读成「全都可用」。
6. `docs/source-layout.md` — 目录职责、上传清单与检查方式。
7. `ports/artemis/REWRITE.md` — 手写 Artemis 重写的风格宪法与「必须不变」的契约。

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
- **子代理与主代理分工（用户明确定调）**：**主代理不承担任何具体工作** —— 编码、文档、构建、浏览器实测、测试、清理**全部由子代理执行**；主代理只负责任务**拆分顺序 / 证据审核 / 最后验收**。以前安排给 lead 亲跑的 next steps **必须改分配执行，不得倒退**。具体代码与细节默认由 `magpie/group/auto-deepseek-v4-1-flash` 承担；**Flash 不能胜任的复杂工作交给 `avemujica-grok/grok-4.7`**；**复杂 C++ 任务交给 `opus-5-5`（`magpie/avemujica-aws/claude-opus-5-5`），此规则优先于一般复杂任务升级 Grok。** 其他模型按需分配；不确定模型擅长什么时先搜索或做小测试，不编造专长。发现当前路由做不成时，把已测证据 / 失败原因 / 待办交接给升级模型，**不对同一路由反复无效重试**。低并发、明确写入范围。每阶段更新 `docs/current-work.md`，让新对话立即接手。**主代理尽可能少做工作，不逐文件审查；最终闸门是执行代理的实际正常工作 / 端到端验证，不是 lead 逐文件读代码。**
- **不要动被用户冻结的东西**：例如 m03520 冻结的鼠标映射缺陷；`docs/fetch-engine-artifacts.md` 里的历史 hash 不得改写。
## 当前状态（2026-10-06 · 官方 4664 已授权作对比 oracle · 对比进行中）

- **官方 4664 显示已收口**：START 前 `#stage` 即预留 1024×576 黑框，START 后不生长；宽 1280 / 窄 800 两窗口下 canvas 都同比例填满 frame（宽 1024×576、窄 1019×573），无溢出/剪边/黑屏。方案=两处最小 CSS 改（16:9 框从 `#game` 移到 `#stage`；`engine.html` canvas `100%!important`），**无** buffer/descriptor/getContext/GL viewport 写。
- **用户已授权对比（关键更新）**：用户确认官方 4664 **能正常播放**、独立 16:9 视口正常，并最终明确「**都是正常的，可以进行对比了**」——范围是**淡入淡出 / title / CG**（不只是尺寸）。因此官方可作为该游戏这些片段的**行为 oracle**，official-vs-compat 对比**已授权、进行中**。
- **在途**：唯一 A/B 浏览器代理（`4a9b0e19`，**尚未 closing，不得当 failed / released**）写域 `.test/artemis-official-compat-ab/**`，报告未回。**用户此前亲测 15:33 fade 实际无效（异常已确认）**；**本轮同输入的 A/B 对照结果与具体原因仍未确认**，不得先写「根因已定位」。用户**已否定** static-IF 反馈当根因（需真实 attachment / destination ID 或实际 pixels 红，本轮不新 probe）。
- **不要做**：不新开动画调查 / 测试 / 实现 / 浏览器 / 服务；不重跑双引擎 A/B；不用历史未测结论（含候选 6f、`ROOT_CAUSE.md`）当起点。
- 接手顺序：`docs/current-work.md` 文首摘要 → `docs/artemis-official-compat-handoff.md` → 历史交接（仅背景）。当前三份有效文档合计不足 100 行即可，不追长状态史。
