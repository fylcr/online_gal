# CI 与发布政策（拟议）

> **本文件是拟议规范，不是已强制的事实。** 每一条都带 `当前状态` 字段：**已强制**（有代码/工作流证据）、**部分**（机制存在但不覆盖全部）、**未强制**（只有散文约定或根本不存在）。
>
> 这是 Phase 4 的目标（见 `../architecture/migration-plan.md`）。本文件**不授权**任何改动；**没有任何一条**已经落地。
>
> 审计戳：HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`。

---

## 0. 现状 vs 拟议（一页速览）

| 主题 | 当前状态 | 拟议 |
| --- | --- | --- |
| 普通 CI（构建/检查）与发布分离 | **部分**：`build-packages.yml` 分离（只有 `workflow_dispatch && publish==true` 才发布）；ONS/krkr **未分离**（push 即发布）；compat 硬编码 `publish: false` | 统一为"普通 CI 只产工件；发布必须显式人工派发" |
| 外部 Action 固定 | **未强制**：全部可变 tag | 全部固定到 40-hex commit SHA |
| 工作流权限 | **部分**：`check-upstream-drift` 是 `contents: read` + `actions: write`；其余构建/发布为 `contents: write` | 每个作业只取所需权限 |
| 路径检查 | **已强制**（`check:sources.mjs` 双向校验补丁表与配方/overlay）；`check:layout.mjs` 已存在但**在 HEAD 上失败** | 保留并让它在 HEAD 上通过 |
| SDK / consumer / native fixtures 的分层检查 | **已强制**：`smoke-consumer.mjs` 断言消费方顶层解析集合、vendored 计数、`runtimeId` 唯一；原生探测有 env 前置 | 保留；把"环境前置缺失"与"硬失败"的分界写进政策 |
| 引擎字节与 ledger 一致性 | **已强制**：`make-package.mjs` 打包前按 ledger 校验，**无 opt-out** | 保留，并明确"台账手工更新"的责任人与频率 |
| 发布人工批准 | **部分**：packages 与 compat 有人工门；ONS/krkr 无 | 全部引擎发布都需显式人工批准 |
| source offer | **部分**：compat 有自校验 offer；bridge 无 | 每个已发布产物都有可自校验的 source offer |
| 二进制与 commit 一致 | **部分**：`build-artemis-compat.sh:32-42` 硬断言 `UPSTREAM_COMMIT == ARTC_PIN`；bridge 有 `skip-upstream-checkout: true` | 所有引擎构建都断言"构建的 commit == 记录的 pin" |
| SBOM / 依赖清单 | **未强制**：无 SBOM，`pnpm-lock.yaml` 不记录 license | 产出可复现的依赖清单（含许可状态） |
| 备份与恢复 | **未强制 / 未知**：远程状态未查询 | 记录并演练恢复；区分"仓库可重建"与"只在单机" |
| CODEOWNERS | **未强制**：`.github/CODEOWNERS` 探测为 ENOENT（仅该路径） | 为许可关键路径指定 owner 并要求评审 |
| 分支保护 | **未知**（未查询） | 明确记录并配置 |

---

## 1. 拟议规范

### PR-1 普通 CI 与发布分离

**规范**：普通 CI（push / pull_request）**只**产出工件，**不得**创建或修改 release。只有显式的 `workflow_dispatch` 且 `publish == true` 才能发布。

**理由**：仓库自己的 `build-packages.yml` 已写明"a release is a claim that these bytes were accepted, and the acceptance oracle is a real game booting them — which no runner can do"。ONS/krkr 的 push 发布是这条口径的例外。

**当前状态**：**部分**。

### PR-2 外部 Action 固定到 commit SHA

**规范**：所有 `uses:` 引用固定到完整的 40 位 commit SHA，人类可读版本号保留为尾注释。建立更新通道（Dependabot for `github-actions`，或定期人工 bump 并评审）。

**当前状态**：**未强制**。6 个外部 Action 全部为 `@v4` / `@v11`。

**风险**：pin 之后安全更新变手动 —— 所以规范要求**同时**建立更新通道，否则会把 R-1 换成"永不更新"。

### PR-3 最小权限

**规范**：每个作业的 `permissions:` 只包含它实际需要的范围；不使用 workflow 级 `contents: write` 作为默认。

**当前状态**：**部分**。例外：`build-wasm-artemis-bridge.yml` 的 `real-game` 作业继承写 token，并执行未固定的 `npm install --no-save puppeteer`。另外 `check:sources.mjs` 的 **exit 0 是快照时点的工作树证据**（`sources ok (worktree): 340 mapped functions, 340 product files`），**不是** current-HEAD 事实（它读工作树里的补丁表，而工作树现在有 6 项）；`check:layout.mjs` 的坐标是 **checker 坐标（stripComments 后）**，与原始 `grep` 行号不同。

### PR-4 路径与来源检查（保留并加强）

**规范**：三层检查同时存在且各有归属：

| 检查 | 负责什么 | 当前状态 |
| --- | --- | --- |
| `scripts/check-layout.mjs` | `packages/`/`apps/`/`ports/` 不得引用 `.test/` | **已存在但在 HEAD 上 exit 1**（4 处违规） |
| `scripts/check-sources.mjs` | 端口入口/导航/台账/包声明；补丁表与配方/overlay **双向**一致 | **已强制**（HEAD exit 0；附再分发复核警告） |
| `.github/scripts/smoke-consumer.mjs` | 消费方顶层解析集合、vendored 计数、`runtimeId` 唯一、workflow/lockfile 未被改动 | **已强制** |

**规范**：任何一处检查失败**不得**被绕过；警告**不得**被当作许可结论。

### PR-5 SDK / consumer / compat / native / fixture / environment 的分层如实报告

**规范**：结果必须按层报告，且**环境缺失不等于失败**：

- **SDK 层**：专有字节的存在与许可状态（见 `third-party-inventory.md`）。
- **consumer 层**：打包后消费方能否按声明解析（`smoke-consumer.mjs`）。
- **compat 层**：compat 产物与 source offer 的一致性。
- **native 层**：需要 `g++`、`zlib.h`、`tar`、`nm`、`.ref/artemis-compat` 检出的探测。**按文件**区分：**3 个硬失败**（缺编译器**不是**跳过：`first-transition:470,472`、`tween:234,236`、`animation-observe:72,74`）、**跳过**（`audio-native:601,605,651`、`pf8-readat-bounds:122,310`）、**部分控制台 SKIP**（`source-offer:36,73`）。
- **fixture 层**：`.ref/` 与 `.test/` 内容**不**上传；CI **会** clone `.ref/artemis-compat`（`build-packages.yml:176-189`）供原生探测使用，但**不下载游戏 fixture**。不得写"没有工作流检出 `.ref`"。
- **environment 层**：CI pin `node-version: '22'`（`build-packages.yml:160`、`build-wasm-artemis-bridge.yml:118`）；本机 v26.8.2 的日志**不是** CI 复现。

**当前状态**：机制多在，但**报告口径**尚未成为规范（"全部 fail"或"全部 skip"的错误表述曾出现）。

### PR-6 引擎字节与 ledger 一致

**规范**：任何打包/发布引擎字节之前，按 `ports/lib/engine-artifacts.json` 校验大小 + sha256，**无 opt-out**；台账更新必须有明确责任人，且"陈旧条目静默退化为源码构建"这一行为必须被文档化而不是被容忍。

**当前状态**：**已强制**（`make-package.mjs:329-364`）。台账**手工**更新是已知事实。

### PR-7 发布需显式人工批准

**规范**：发布必须由知道"验收 oracle 是真游戏启动"的人显式触发；发布前检查清单至少包含：许可状态、source offer、二进制与 commit 一致、产物与 ledger 一致。

**当前状态**：**部分**（见 PR-1）。

### PR-8 source offer 与二进制 commit 一致

**规范**：每个已发布产物都有可自校验的 source offer（pinned 归档 + patches + overlays + 构建脚本 + 说明 + sha256），且构建期间断言"使用的 commit == 记录的 pin"，不一致即失败。

**当前状态**：**部分**（compat 已具备；bridge 缺）。

### PR-9 许可与 SBOM / 依赖清单

**规范**：产出可复现的依赖清单，至少包含直接依赖的名字、声明范围、解析版本与许可**状态**；随仓交付的第三方字节另有清单。清单**不得**把"声明字段"当作授权。

**当前状态**：**未强制**。无 SBOM；`pnpm-lock.yaml` **不**记录 license 字段。**直接外部依赖**（9 行 / 8 个唯一包）的声明许可与 LICENSE 文件路径已读（MIT ×7、Apache-2.0 ×1，见 `third-party-inventory.md` §1）；**传递依赖**许可仍未枚举，未做法律结论。清单要求把“**声明字段 / 文件存在 / 已读 / 未枚举 / 法律未知**”分开写，且不把 devDependency 事实当作站点无第三方运行时的证明（站点包含需构建产物清单，**未核**）。

**插件/脚本来源政策（拟定，与 PR-9 同层）**：官方插件与第三方插件必须分开登记，权限不同且第三方默认权限更小；插件 API 版本不匹配即拒绝加载。当前无插件实现（tracked grep 0 命中），本项只是预先定好的治理条款。

### PR-10 备份与恢复

**规范**：区分"仓库可重建"与"只存在于单机"的内容，为后者写下并**演练**重建命令；远程备份状态必须被**查询并记录**，而不是默认。

**当前状态**：**未强制 / 未知**。见 `repository-risk.md` §9。

### PR-11 CODEOWNERS 与分支保护

**规范**：为许可关键路径指定 CODEOWNERS（`ports/artemis/vendor/**`、`ports/artemis/compat-web/**`、`packages/runtime-ons/assets/**`），并要求评审；分支保护状态被记录（已查证或未知）。

**当前状态**：**未强制**。`.github/CODEOWNERS` 探测 ENOENT（**仅该路径**）；远程保护状态**未知**。

---

## 2. 政策如何被验证（不加新工具）

拟议规范都应当能用**现有**命令或工作流本身验证：

| 规范 | 验证方式 |
| --- | --- |
| PR-1 | 三个引擎工作流的 publish 表达式并排核对；一次 push 只上传工件 |
| PR-2 | `grep -n "uses:" .github/workflows/*.yml` 全部为 40-hex |
| PR-3 | 逐作业读 `permissions:` 并与其步骤比对 |
| PR-4 | `node scripts/check-layout.mjs` exit 0；`node scripts/check-sources.mjs` exit 0（注意：后者的 exit 0 在 worktree 模式下读的是**工作树**，不等于 HEAD 事实） |
| PR-5 | 报告按层分节，且 native 层按文件列出三种结局 |
| PR-6 | 打包一次，确认 ledger 校验步骤存在且无跳过开关 |
| PR-7/PR-8 | 发布清单存在；`build-artemis-compat.sh` 的断言与 `.sha256` 自校验可复现 |
| PR-9 | 清单文件存在且含许可**状态**列，并区分声明/文件/已读/未枚举/法律未知 |
| PR-10 | 恢复命令存在且被演练过（有记录） |
| PR-11 | CODEOWNERS 条目与路径匹配；远程保护状态被记录 |

**不引入新工具**：本轮**没有**发现 lint/format 门禁，也**不**建议凭空发明一个（"no lint invent"）。如果未来要加，必须有明确规则与现有代码基线，而不是为了"看起来很规范"。

---

## 3. 边界与免责

- 本文件是**拟议**。除明确标注"已强制"的条目外，**没有任何一条**已经在仓库中生效。
- 本文件**不授权**任何 workflow 改动；phase 1–5 都是待批准状态（见 `../architecture/migration-plan.md`）。
- 本文件**不**提供任何规避平台策略或执法的做法；它要求的是把来源、许可与责任人**记录清楚**。
- 远程侧事实（branch protection、Actions allow-list、release 资产、`ARTEMIS_PUBLISH_ARTIFACTS` 变量值）**未查询**，因此**未知**，不是"不存在"。
- 冻结记录（`.github/upstream-pins.json` 的 `expected`、`docs/fetch-engine-artifacts.md` 的历史 hash）**不得**被本政策的任何落地动作改写。

## 4. ONS/KRKR 已实现的发布条件（2026-10-14）

以上 PR-1 仍是未批准的全仓拟议。以下只描述本次已经写入 ONS 与 KRKR workflow 的行为，不改写前文，也不把 Artemis 算进去。

- 自动：`main` push 的精确 `before..after` 只有在碰到该引擎自己的发布输入时，才先做昂贵 native/WASM 构建，成功后发布一个新的不可变版本。ONS 输入是 `packages/runtime-ons/src/**`、`scripts/**`、`package.json`、`tsconfig.json`；KRKR 输入是 `packages/runtime-kag/src/**`、`package.json`、`tsconfig.json`。
- 自动版本取该包 tracked version 的 `major.minor.patch`，形式为 `BASE-beta-YYYYMMDDHHmm`，时钟是构建开始时一次读取的 `Asia/Shanghai`。不回写 tracked `package.json`。
- 手动：必须显式 `publish=true` 加一个完整 SemVer；缺省、false、空白和非法都在昂贵构建前拒绝。
- 碰撞按完整版本字符串判断。`1.0.0+a` 与 `1.0.0+b` 不同。已有 tag、release 或 asset 则保留原字节。创建成功后上传失败以非 0 退出，不 clobber、不删除。
- drift 只报告。compat 仍是字面 `publish: false`；bridge 仍是原来的手动 opt-in 与 clobber 语义。旧 catalog pin `engine-ons-0.7.7beta` 与 `engine-krkr-20260920` 不随本次发布改变。
- 这一节是源码契约，不是一次实际 GitHub Release 或 native 构建已经跑过的证明。
