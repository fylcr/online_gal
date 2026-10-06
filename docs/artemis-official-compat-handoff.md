# Artemis 4670 AI 接口交接（AI 优先 · 新对话读这份）

> 本文件是**当前有效**交接，主体是**给 AI 的机器接口**。旧 handoff 只作背景，**不得当现状**。
> 代码身份 `73c5162a8d38548ecedafe5ca22f4fbb2c8b035c`（已 push）**绝不说「未落盘」**；doc HEAD 以 `git rev-parse HEAD` 为准。

## 0. 单命令重启（仅确证服务 dead 时）

```bash
nohup bash .test/artemis-comparison-ready/launch-4670.sh > .test/artemis-comparison-ready/serve-restart.log 2>&1 &
```

launcher 本身是**前台 `exec node`**，不套 `nohup`/`Start-Process` 就不是 daemon。**先核 `/api/identity`** 确认真的 dead 再重启；**不新开第二个服务**。Node 用 `/d/programe/nodejs/node.exe`（PATH 里没有就写绝对路径）。

## 1. AI 快速开始（真实可用 · 照抄）

接口只在**持有 owner lock 的页面**里跑引擎。**服务自己不会启动浏览器**。

1. 读 `browser-skill`，然后：

```text
browser_session  action=start            # 不传 url 也可
browser_page     action=navigate  url=http://127.0.0.1:4670/ai
browser_inspect  action=observe          # 确认页面已连上
```

2. 此后所有动作都是 typed HTTP。最小一条（`requestId` 自选 8–64 字 `[A-Za-z0-9_-]`，**retry 必须同 id**）：

```bash
curl -s -X POST http://127.0.0.1:4670/api/ai/commands -H 'Content-Type: application/json' \
  -d '{"action":"start","engine":"compat","trace":true,"requestId":"demo00000001"}'
curl -s http://127.0.0.1:4670/api/ai/commands/demo00000001   # 直到 status=succeeded|failed
```

完整一条序列：`new-run` → `start official` → `GET commands/<id>` 等 `phase=loaded`（`title` 由引擎自报，可能是 `null`）→ `stop` → `start compat`(`trace:true`) → `capture` → `GET /api/ai/frame/<id>.png` → `stop` → `browser_session stop`。

**不想手写 curl** 就用 CLI（每次调用自动随机 id；要 retry 稳定请用 HTTP 显式 id）：

```bash
node .test/artemis-comparison-ready/agent-api-client.mjs caps|state|logs [since]
node .test/artemis-comparison-ready/agent-api-client.mjs new-run [runId]
node .test/artemis-comparison-ready/agent-api-client.mjs start official|compat [--trace]
node .test/artemis-comparison-ready/agent-api-client.mjs wait <requestId>|stop|capture
```

CLI 退出码：0 ok / 1 命令失败或未结算 / 2 用法错 / 3 **无页面接入**（先去 `/ai`）。

HITL 根页 `http://127.0.0.1:4670/` 仍可点，但**不是 AI 路径**；AI 只用 `/api/ai/*`。

## 2. 端点表

| 方法 | 路径 | 说明 |
| --- | --- | --- |
| GET | `/api/identity` | 服务/run/engine 身份与 source hash |
| GET | `/api/ai/capabilities` | 动作、引擎、phase、limits、auth、capture 说明（**schema 事实来源**） |
| GET | `/api/ai/state` | `browserConnected`/`ownerLock`/`engine`/`phase`/`lastCapture`/ledger/queue |
| GET | `/api/ai/logs?since=N&limit=M` | 环形日志；回 `entries[]` + `nextSince`，**用 `nextSince` 续读** |
| POST | `/api/ai/commands` | 提交 typed 动作，`202` + `command` |
| GET | `/api/ai/commands/<requestId>` | 查同一条命令的最终结果 |
| GET | `/api/ai/frame/<id>.png` | 取已存的 capture PNG（仅 compat 成功时才有） |
| POST | `/api/ai/release` | **只由页面 `pagehide` sendBeacon 调用**，非 AI 端点 |

**动作**：`new-run{runId?}` · `start{engine,trace}` · `stop{}` · `readState{}` · `capture{}` · `scene{engine,runId?}`。
`compat` 必须 `trace:true`。**没有 UI 点击、没有 script/eval/文件/进程路由。**

## 3. 一次只一个 owner

不同 `ownerId` **即使 `takeover:true` 也是 `409`**：租约新鲜 → `owner-mismatch`；租约过期 → `owner-state-unknown`。
**过期只表示服务没听到页面，不等于引擎停了，也不清 `engine` 或锁。**
锁只在该页 dispose 后 `sendBeacon /api/ai/release` 时清；此后 `browserConnected=false`，新页可接入。**被强杀、没走到 `pagehide` 的页面会留下 unknown 锁**——此时若原页确已关闭、无法 release，**确认无实际 engine 后按第 0 节重启本服务一次**恢复；**禁止任意 caller 用伪造 ACK takeover**。原 owner 可重连并 `stop`。

## 4. 边界与限制（如实）

| 限制 | 值 | 超出 |
| --- | --- | --- |
| JSON 响应 | 8 KiB（按字节） | 裁 `notes/errors/resources/result`；`version/phase/requestId/status/error/source/seq/nextSince` 保留 |
| 请求体 | 16 KiB | `413` |
| 队列 | 16 | `429` |
| 命令账本 | 512 / 进程生命周期 | `429 command-ledger-full`；结算过的 id 仍在，**retry 不会重复执行**；重启进程才清，`new-run` 不清 |
| owner 租约 | 15 s | 标 unknown，不释放锁 |
| 命令超时 | 120 s | 命令失败 |
| 日志 | 4000 条，单行有上限 | 被裁条目**仍推进** `nextSince` |

- **capture**：official 用 WebGL 且 `preserveDrawingBuffer=false` → canvas 读可能 blank，命令 **failed**（`blank:true`）且**不存 PNG**；请用 browser 工具截图。compat 画到 2D surface，真实读是**非空 PNG**（实测 1024×576 / 25220 B / sha `fbd69fa5…`，91 色）。
- `runtime+canvas` 只证 `loaded`/`rendered`，**不自动证 title**；`title=null` 是诚实值，**不得用浏览器文案代替引擎自报**。
- `new-run` 生成 fresh namespace `artemis_ab_<id>` / `artemis-anim-manual-<id>`，**不删库**；fresh run 只在 idle 时，**开新 run 前必须 stop**。
- **未测**：中间 1s fade pixel、CG 播放、official PNG 可用性。
- **仅本机 workspace / zero-config**；`.test/**`、`.ref/**` 是 gitignored，**不是 clean clone 的现成**。不装 SDK、不开新端口、不用 CDP eval。

## 5. 证据与规则

- 证据：`.test/artemis-comparison-ready/AI-API.md`、`FINAL-AI-RECEIPT.md`、`agent-api-client.mjs`；`.test/artemis-ai-api-review/FINAL-VERIFICATION.md`；`.test/artemis-test-services-consolidation/STOP-RECEIPT.md`。
- 红→绿：异 owner takeover 409 / 8 KiB 裁剪保字段 / 账本 512 硬顶；contract **25/25** + 独立 review **3/3** 均 EXIT 0（source-only，**无 GPU claim**）。
- 冻结：official `7ceabed2…`/5998665 B、compat `d4a54476…`/`0.1.14`/code `73c5162a…`/pin `17200e64…`；源胶水 `78929c4c…`、源 data `7f9b94c9…`。
- 提交用**显式 pathspec**；不 `add -A`/`reset`/`stash`/`rebase`/force。官方 SDK / 商业资源 / `.test` / `.ref` **不 commit、不发布**。
- **复杂 C++ 交 `opus-5-5`**；默认 Flash，困难用 Grok；**无 Teams 除非用户明示**。
