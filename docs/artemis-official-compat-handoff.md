# Artemis 官方 ↔ compat 对比交接（当前现状 · 新对话第 2 读）

> 本文件是**当前有效**的官方/compat 对比交接。`docs/session-5697f1a7-handoff.md` 及其前的多份 handoff 是**历史**，只作背景，**不得当现状**，也不要用它逐句覆盖旧记录。
> 本文件写于文档提交前，**状态：committed** —— 已随 `f6cca83a655689ed3b49272a0d7c00f8cd4dcc0c` 提交并 push（见文末「提交记录」）。

## 目标

让本仓库 compat（`ports/artemis/compat-web`）的**淡入淡出 / title / CG** 三个片段恢复到与**官方 4664** 行为一致。
官方已获用户授权作为该游戏这些片段的**行为 oracle**，因此可以正式做官方 vs compat 对比。

## 立即下一步

1. **先收在途 A/B 报告**：浏览器代理（one-shot，`4a9b0e19-b13e-4ca8-8bb2-2979814d1a7c`，provider `magpie/model-avemujica-grok/grok-4.7`）写域 `.test/artemis-official-compat-ab/**`。先查有无完成 notice / `REPORT*`；无产物即**未完成**，**不得**从 dirty mtime 判已释放，**不得**重跑双引擎（防重复跑）。**此 one-shot 不可 steer**：`send_message` 会报 active teammate not found，不要当 teammate 转。
2. 报告到手后，**结合静态风险**校验（只读），确认哪些片段真已可比。
3. **红**之后才派**唯一**复杂 C++ writer（opus-5-5）。
4. 收尾：真实浏览器**验收 + cleanup**，源码 offer / commit / CI 通过后再交用户。

## 已测 vs 未知

**已测（有真实证据）**

- **官方 4664 可以正常播放**，独立 16:9 视口正常；用户最终明确「**都是正常的，可以进行对比了**」。范围是**淡入淡出 / title / CG**，不是只有尺寸。
- **输入同一性**：`PARITY.md` / `inputs.json` / `parity-raw.json`，`parity.mjs` 真用 runtime 包 PF8 reader：**14/14 文件 hash + size 相同**。两 PNG **1280×720 / 2560×1440 皆实同**。
- compat **CI 真绿**：581 tests / 579 pass / 0 fail / 2 skip（skip 因缺旧 audio 构建源与 old 9f 对象）。
- runtime **0.1.14** / engines repo **0.1.15**；source pin **`17200e64ea99681e3c27d694433c0ff78d7a7cee`**（独立源）。

**未知 / 仍未过门**

- **compat 的 fade / title / CG 是否仍不正常** —— A/B 报告未回。
- **4663 engineBase 曾漏 `/pkg` 403 已同包 fix**；但 **15:33 用户 trace true + 真实 GL + ready + frames 而 fade 无效** 仍未解释。
- 首 frame / tween / resize 已闭包，**不代表**用户 fade / title / CG 正常。
- `Detected invalid hash entry` 每行 `engine.html` 每行都报，**未证无害**，属静态遗留（不新开调查）。

**已否定 / 禁止重试**

- `unconditionalDraw`-before-`Capture` 候选 6f **未经测**，只改 out/source 已撤，同 `clean.sha 92cb0897`；`.test/artemis-opus-fade-render/ACCEPTANCE.md`。不构建、不发包。
- 复杂 C++ 代理（opus5.5，`20de7c14-dfc9-4ded-a46f-77f13564b928`）**已正式 closed**，只读静态分析、**未编译 strict checker**、未留 scope 报告文件。它声明的 `CaptureFrame` feedback-loop「确定」**不成立**：只有 `IF last_frame_tex_ 是 scene_fbo_ATTACHMENT` 才冲突，**未证明**实际 attachment 两纹理同名。**禁止**在交接里写「根因已定位 / 已红」。
  - 下一 C++ 必须：真实 GL recorder 严格 tracking framebuffer attachment、copy destination IDs，或真实 browser pixel 证明，**再实际红 → 绿**。
- `renderHz 39 / dirty rev 64` **不是两个 clock**；`dt` 节流 / native absolute now **不改音频**；legacy host `gl.js` DPR **不在** compat active path。
- 不同 CG pixel 换图导致 logical size 变化属正常；`lyprop` 没 file / w-h，旧 lyc clip 继承 oracle **误**。
- 官方 main 未返回 **≠ 永久 block**；diagnostic raw 236 ≠ 3763，且最终 NUL 使 WIDTH 0 **已真 fix**。
- **不从历史 `ROOT_CAUSE.md` 的未测结论开始。**

## 身份与入口表

| 对象 | 入口 / 身份 | 说明 |
|---|---|---|
| 官方母页 | `.test/artemis-official-user-manual/page/index.html` | child `engine.html`，经 `/?view=engine` |
| 官方 WASM | 原 5998665 B，SHA `7ceabed28b84e7689fc4b629fa232e2857f15c3b6e6fa3e09fc39dba082d145e` | **原 byte**；胶水本地副本仅 data 长度 metadata `236 → 3762` |
| 官方胶水 | 预载 config 去 WIDTH 前 NUL | 旧 `236/3763` **作废**；值以现 serve 路由原文件与 CURRENT/IFRAME 收据为准 |
| 官方资源 | `.test/artemis-official-async/game` | 已有 loose **29224**；与原 pack 全量等价**未测**，但关键场景资源已实 matched |
| compat | 源码 `73c5162a8d38548ecedafe5ca22f4fbb2c8b035c` 已 push | current git HEAD 见本文件提交后回写 |
| 对比证据 | `.test/artemis-official-compat-parity/` | `PARITY.md` / `inputs.json` / `parity-raw.json` / `parity.mjs` |
| A/B 在途 | `.test/artemis-official-compat-ab/**` | 写域属在途 one-shot 浏览器代理 |
| 静态风险 | `.test/artemis-opus-fade-render/ACCEPTANCE.md` | 候选 6f，已撤 |

## 重要证据导航

- [current-work.md](current-work.md) — 滚动检查点，文首摘要是唯一现状
- [artemis-official-compat-parity/PARITY.md](../.test/artemis-official-compat-parity/PARITY.md) — 输入同一性报告
- [artemis-official-user-manual/FINAL-IFRAME-ACCEPTANCE.md](../.test/artemis-official-user-manual/FINAL-IFRAME-ACCEPTANCE.md) — 官方视口数值表
- [artemis-animation-manual/proof.json](../.test/artemis-animation-manual/proof.json) — API verified
- [artemis-opus-fade-render/ACCEPTANCE.md](../.test/artemis-opus-fade-render/ACCEPTANCE.md) — 已撤候选
- [artemis-testbed-manifest-recovery/RECEIPT.txt](../.test/artemis-testbed-manifest-recovery/RECEIPT.txt) — 脏树/incident 证据
- [session-5697f1a7-handoff.md](session-5697f1a7-handoff.md) — **历史**，非现状

## 未提交 & 在途写域

- **未提交**：本轮只提交三个文档路径；`ports/artemis/compat-web/tests/fixtures/animation-observe/path-string.h` 为 **untracked**，**不得 stage**。
- **在途写域**：`.test/artemis-official-compat-ab/**` 属在途浏览器代理，本文件不碰。
- **b85 incident**：曾误改共享 `.test/package.json`，原文 unknown 未可恢复（证据见上）；后续 `npm install` **必须** own isolated dir。

## 测试与进程

- **四服务器有意保留**（read-only `netstat`/command line 可一次确认；值若 not alive 只注明最近 receipt，**不凭猜 kill / restart**）：4661 PID 41648、4662 PID 2612、4663 PID 58184、4664 **PID 71972**。
- 用户 tab / IDB **不碰**；**每次只一 engine**；只用 `browser_*`。
- 所有测试 browser session 结束需 stop，**原用户页不动**（上一 session `ugku` 已 stop、`list` 为空）。
- 四 active patch：`0001 readat` / `0002 numeric` / `0003 tween loopdelay` / `0004 default-off observer`。
- `pack` 读取 **≤ 4 MiB**；unsigned **> 2 GiB 合法** —— 旧文档建议的 `offset < 2GiB` 是**错的**。
- 真实链：`*game_start → brand_logo → *title → title_init`。

## 规则

- **lead 只协调 / 审核 / 验收，所有实作出 subagent**；默认 **Flash 4.1**，复杂 C++ 用 **opus-5-5**（`magpie/avemujica-aws/claude-opus-5-5`）；**未明确授权不做 Agent Teams / workflow**。
- `system.ini` **只**预期 `ASYNCIFY_URL` / `ASYNCIFY_HASH_LIST_URL` / `SAVEPATH` 宿主差异；剧情选项**未删**。
- **freeze / 安全**：语音已正常、MapPoint 冻结；不加内存、不 OOB 绕错、不统一 duration 延长、不 no-agent 自动三轮；`releaseArtifacts-only` / `publish false` / `npm private`。
- 官方专有 WASM / SDK 本机 **readonly**，**不 Git、不发布复制**；动态对比 capture 是行为数据，不是抄 GPL 实现。原官方 asset **不可** present 打包（专有本机参考）。
- 环境：本机 Git Bash；emsdk **6.0.9** 已装在 `/d/emsdk`，要 `source emsdk_env.sh`（**不是**未装）；Node fs 用 Windows cwd 派生防 `/d` 变字面 `D:\d`；**不改** SDK / cache / `EM_CONFIG`。
- 提交用显式 pathspec；不 `git add .` / `-A`；不 stash / reset / force push；他人 staged 保持不动。

## 新对话如何继续

1. 读 [current-work.md](current-work.md) 文首摘要 → 再读本文件，即得现状。
2. 查在途 A/B（`4a9b0e19-…`）是否有 REPORT：有则审核；**无则视作未完成**，不重跑双引擎。
3. **未授权不新开**动画调查 / 测试 / 实现 / 浏览器 / 服务；不新开修复任务（用户当前只要求整理）。
4. Goal：`goal-93d83444-18c2-4843-9802-1297d85d4070` rev1 **active armed**。旧 `goal-bb…` rev14 只完成了 canvas 显示修复，**不继续**；当前 active goal 新会话 resume / disarmed 按 `get_goal` 与 host 规则处理。

## 提交记录

- **状态：committed** —— 本文件随文档提交 `f6cca83a655689ed3b49272a0d7c00f8cd4dcc0c`（docs-only）一同 push 到 `origin/main`，remote `ls-remote` 与此 SHA 一致，ahead/behind 0/0。该 SHA 是**文档提交**，**不是** compat 工件 / 源码身份（源码身份仍是 `73c5162a8d38548ecedafe5ca22f4fbb2c8b035c`）。
