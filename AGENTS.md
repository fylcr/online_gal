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
## 当前状态（2026-10-06 · 环境已验通 · 代码交新对话 · goal-93d83444 rev2 environment-only 完成交接）

- **环境已验通**：单实例对比环境 `http://127.0.0.1:4670/`，run `rn20261006a`，PID **77504**（node，父 `nohup.exe` 74200 仍存活 = 真 detached 持久进程）。宽度 1024×576、无嵌套 iframe、无内 scroll；两 lane 真实完整 title 截图已实测。证据 `.test/artemis-comparison-ready/FINAL-ENVIRONMENT-PASS.md` + `environment.json` + `raw/unnested-geometry-rn20261006a.json`。收尾自检 `check-env.mjs` **16/16 PASS**（只 unit，无 GPU）。
- **零配置入口**：开 4670 → 若实例运行先「关闭 game」→ 点**新对比编号（fresh）**只一次 → 官方 START 看到目标阶段 → 关 game → compat START，沿同 run 比。**不两边并跑、不复用已写 save 跳过开场、不清任何 IDB**；不手改 source const。快速重启只一条：`.test/artemis-comparison-ready/launch-4670.sh`（配现有 PowerShell `Start-Process` 或 `nohup` 才持久；前台 `exec node` 本身不是 daemon）。
- **代码交新对话**：未测边界 = 中间 1s fade pixel 未抓、CG 播放对比未执行；品牌前 state 与旧 AB 不同，当前为 fresh ready。旧 AB 报告 `.test/artemis-official-compat-ab/REPORT.md`（actor `4a9b0e19` 已 closed）仅记 pc=160 重复 8 次 / 162 重复 5 次与 190 条 dispatch START，**observer 只有 dispatch、不能从缺 mix 断失败**；official 无 trace 不能断「官方不重复」。**本轮只交可定位状态，不分析代码。**
- **冻结不动**：单 4670 / 当前 PID 77504；原四服务 4661/41648、4662/2612、4663/58184、4664/71972 有意保留，不 cleanup kill。官方 WASM `7ceabed2…`、compat WASM `d4a54476…`、源胶水 `78929c4c…`/源 data `7f9b94c9…` 原样未改。用户 IDB / asset / `.ref` / 测试 harness 不动。
- **未动 & 非本轮**：源码产品 noDiff；`docs/session-5697f1a7-handoff.md` modified 与 `ports/.../path-string.h` untracked **保持不 stage**。已撤候选 6f、旧 dt/dirty fps/DPR/CG pixelswap oracle **不再建议**；`Detected invalid hash` / 字体缺失属静态遗留，**未证无害也未 cleanup**。设计/取用纪律见「硬约定」两节（模型分工、证据纪律）。
