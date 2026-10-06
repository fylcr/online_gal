# Artemis 官方 ↔ compat 对比交接（当前现状 · 新对话第 2 读）

> 本文件是**当前有效**的官方/compat 对比交接。`docs/session-5697f1a7-handoff.md` 及其前的多份 handoff 是**历史**，只作背景，**不得当现状**。
> **代码身份**始终 `73c5162a8d38548ecedafe5ca22f4fbb2c8b035c`（已 push），**绝不叫「未落盘」**；doc HEAD 以 `git rev-parse HEAD` 为准，**不是工件 SHA**。

## 零配置快速开始（本文件主体 · 照做即可）

1. **开** `http://127.0.0.1:4670/`。服务本轮已实测 live（PID 77504）；**若仍在跑，不要 restart、不要起第二份**。
2. 若页面里已有 game 实例 → 先点 **「关闭 game」**。
3. 点 **「新对比编号（fresh）」**，**只点一次**。新 run 由 `/api/new-run` 生成，runid 从 `/run.json` 读；页面**默认不启动 engine**。
4. 官方侧：点 **START**（`outer START` 只此一次），看到目标阶段（如 title）后 → **关闭 game**。
5. compat 侧：点 **START**。compat embed 自动运行，`animationTrace=true` 已由 typed preRun ENV 设好，**没有第二个内层 START**。
6. **沿同一 run 对比**。两 namespace 每次都是该侧首次使用，因此 fresh；**重复比开场要换新 id，而不是 clear 库**。

**四条硬禁令**：① **不两边并跑**（同刻只一个 engine）；② 不复用已写 save 把开场跳过；③ **不清任何 IDB**；④ 不手改 source const。

**namespace 约定**：官方 SAVEPATH 实际是 `artemis_ab_<id>`；compat URL 只带 `?ns=<id>`（原 lane prefix 会自己补），得 `artemis-anim-manual-<id>` —— 这样才**避免双 prefix**。当前 run `rn20261006a` → 官方 `artemis_ab_rn20261006a` / compat `artemis-anim-manual-rn20261006a`。

## 快速重启（唯一一条命令）

```bash
nohup bash .test/artemis-comparison-ready/launch-4670.sh > .test/artemis-comparison-ready/serve-restart.log 2>&1 &
```

launcher 本身是**前台 `exec node`**，**不声称默认 daemon**；要持久必须套 `nohup` / 现有 PowerShell `Start-Process`。**仅确证 dead 时**才恢复 4670 独立持久 Node（已装 `D:/programe/nodejs/node.exe`）；`identity` 先核；**其它 4 端口不动**。不要在 host job tied transient daemon 里跑然后说 alive —— 要记录 parent gone/已 detached 并验证 actual HTTP。

## 环境事实（本轮已实测）

- **活体身份**（`/api/identity` 只读复验）：官方 wasm `7ceabed28b84e7689fc4b629fa232e2857f15c3b6e6fa3e09fc39dba082d145e` / **5998665 B**；compat wasm `d4a54476ab2b120baae9d50d758b54d164c9c08c6eecc0688e54a6a3960ec923` / **1480956 B**，runtime **0.1.14**，code `73c5162a…`，source pin `17200e64…`。
- **served glue `"end"` 与 data 实际 length 三相一致**：metadata `end = remote_package_size = 3756`，served data 实际 **3756 B**（sha `0dc1ecd6…`）。**不只是 http 200**。
- **源文件字节未改**：源官方 JS / data 原样；本 run 只由 `make-preload.mjs` 动态派生预载配置（SAVEPATH / ASYNCIFY host）与 JS meta。alias/hash 以现 api/receipt 为准。
- **纯 engine iframe 宽高 1024×576**，无嵌套、无内 scroll、无第二 canvas、无 controls。
- **自检**：`node .test/artemis-comparison-ready/check-env.mjs` → **16/16 source contract PASS**（只 unit，**无 GPU**）。
- **工具链**：Node `D:/programe/nodejs/node.exe`；em++ = `/d/emsdk/upstream/emscripten/em++.py` + bundled python，**明确已知，下一代码 task 不先 rediscover SDK**。

## 已测 vs 未测

**已测（有真实证据）**

- 两 lane **真实完整 title 截图**已由父代理看过，**同 run `rn20261006a`**：`screenshots/official-fulltitleframe-rn20261006a.png`、`compat-fulltitleframe-rn20261006a.png`（几何见 `raw/unnested-geometry-rn20261006a.json`）。
- 输入同一性 **14/14** 文件 hash + size 相同（`PARITY.md` 含 actual scripts/titles/CG byte hash 与 dimensions）；两宿主仅差 `system.ini` 的 `ASYNCIFY_URL` / `ASYNCIFY_HASH_LIST_URL` / `SAVEPATH` —— **不是全 pack 等价**。
- 合成 **GPU fixture** 已编（em++ 6.0.9 实 `exit 0`，`.test/artemis-fade-gpu-fixture/COMPILE-RECEIPT.md`，exports `init_context` / `case_setup` / `sample` / `read_pixel`）—— **已编未在 browser 做 pixel 断言**，标「可选已有产物」，不新配置。
- **CPU fixture** `.test/artemis-capture-attachment-check/{REPORT.md,check.log}`：5 copy attachment **texture ID 不同**，alias 风险本场景**未成立**；**MOCKGL 不证 pixel mix**。
- 代码对照历史 raw `.test/artemis-official-compat-ab/{REPORT.md,compat-notes.jsonl,official-capture.json}`（actor `4a9b0e19` **已 closed**，两个 browser session 已 stop）。

**未测 / 边界（简洁）**

- **中间 1s fade pixel 未抓**；**CG 播放对比没执行**。
- brand 之前 state 旧 AB 与现 fresh ready **不同**；最终截图是 official title 入口 menu，**尚在动画期**，不能拿 static position 差当 root。
- **官方用户授权** = 正常 oracle 对应场景**接受**；环境 title 终态**已实测**，**不代表** middle fade pixel 测试通过。
- `Detected invalid hash` / 字体缺失是**静态遗留**：**未证无害、也未 cleanup**，但**无需再让 user loop retry**。

## 已否定 / 禁止重试

- **候选 6f**（`unconditionalDraw`-before-`Capture`）**已撤**，不再建议；旧 `dt` / dirty fps / DPR inactive / CG pixel swap oracle **一并作废**。
- **不从历史 `ROOT_CAUSE.md` 的未测结论开始。**
- 旧 AB 报告 pc=160 重复 **8 次** / pc=162 重复 **5 次**、190 条 dispatch START **不等**「没有 mix 成功」——**该 observer 本来只有 dispatch**，不能从缺 mix 断失败；official **无 trace**，也不能断「官方不重复」。**只交可定位状态，不本轮分析代码。**
- 前 20 条 static IF **不作为 root**；用户未亲自否定，只能说「lead 审核未支持」，**别错误归成用户断言**。
- **复杂 C++ 修复全部交新会话**（opus-5-5）；default Flash / browser Grok 可安排但**未授权**，不做 Agent Teams / workflow。

## 证据导航

- [current-work.md](current-work.md) — 滚动检查点，文首摘要即唯一现状
- [FINAL-ENVIRONMENT-PASS.md](../.test/artemis-comparison-ready/FINAL-ENVIRONMENT-PASS.md) · [environment.json](../.test/artemis-comparison-ready/environment.json) · [REUSABLE-READY.md](../.test/artemis-comparison-ready/REUSABLE-READY.md) · [check-env.mjs](../.test/artemis-comparison-ready/check-env.mjs) — **当前环境**
- [unnested-geometry-rn20261006a.json](../.test/artemis-comparison-ready/raw/unnested-geometry-rn20261006a.json) · [capture-real-new-run.json](../.test/artemis-comparison-ready/raw/capture-real-new-run.json) — 几何与真 run capture
- [official-fulltitleframe-rn20261006a.png](../.test/artemis-comparison-ready/screenshots/official-fulltitleframe-rn20261006a.png) · [compat-fulltitleframe-rn20261006a.png](../.test/artemis-comparison-ready/screenshots/compat-fulltitleframe-rn20261006a.png) — 两 title
- [REPORT.md](../.test/artemis-official-compat-ab/REPORT.md) · [compat-notes.jsonl](../.test/artemis-official-compat-ab/compat-notes.jsonl) · [official-capture.json](../.test/artemis-official-compat-ab/official-capture.json) — **代码对照历史**
- [PARITY.md](../.test/artemis-official-compat-parity/PARITY.md) — 输入同一性 14/14
- [artemis-fade-gpu-fixture/COMPILE-RECEIPT.md](../.test/artemis-fade-gpu-fixture/COMPILE-RECEIPT.md) · [artemis-capture-attachment-check/REPORT.md](../.test/artemis-capture-attachment-check/REPORT.md) — 可选既有产物
- [session-5697f1a7-handoff.md](session-5697f1a7-handoff.md) — **历史**，非现状

> 上文多数 `.test/**` 证据是 **local & gitignored**：本机同 workspace 继续才 0 配置，**不是 clean clone 的现成**。

## 未提交 & 写域

- **本轮只 commit 3 个文档路径**；`ports/artemis/compat-web/tests/fixtures/animation-observe/path-string.h` **untracked、不 stage**；`docs/session-5697f1a7-handoff.md` **modified、不 stage**。
- **零在途 writer**：所有 subagent 已 closed，无 active writer。**不要继续「等待其 REPORT」**。
- 官方 SDK / 商业 script / 产物 / `.test` / `.ref` **不 commit**。

## 进程与规则

- **单 4670 / PID 77504 为主**；4661/41648、4662/2612、4663/58184、4664/71972 **有意保留**，**不新 dialogue cleanup kill**。
- 所有自己开的 browser 已 **stop、none active**；**本轮不 browser list、不再重复 play**。
- 真实链：`*game_start → brand_logo → *title → title_init`。
- 四 active patch：`0001 readat` / `0002 numeric` / `0003 tween loopdelay` / `0004 default-off observer`。
- `pack` 读取 **≤ 4 MiB**；unsigned **> 2 GiB 合法**。
- **freeze / 安全**：不加内存、不 OOB 绕错、不统一 duration、不恢复自动三轮；不改 `releaseArtifacts-only` / `publish false` / `npm private`；**不得抄官方专有 code 进 GPL**。
- 提交用**显式 pathspec**；不 `git add .` / `-A`；不 stash / reset / rebase / force push；他人 staged 保持不动。
- **lead 只协调 / 审核 / 验收，实作出 subagent**；复杂 C++ 用 opus-5-5。

## 新对话如何继续

1. 读 [current-work.md](current-work.md) 文首摘要 → 再读本文件，即得现状。
2. 用户说「**继续**」即**直接处理代码问题**（fade / title / CG），**不重配环境**（环境已验通、4670 已 live）。
3. 复现开场按上面「零配置快速开始」；**不新开第二服务、不重跑已 closed 的 AB**。
4. 修复面：中间 1s fade pixel 与 CG 播放是**未测边界**，先建**真实 GL recorder / 真实 browser pixel** 证据再谈红 → 绿。
