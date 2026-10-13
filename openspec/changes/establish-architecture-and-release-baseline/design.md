# Design: establish-architecture-and-release-baseline

动机见 `proposal.md`。规范和条款见 `specs/*/spec.md`。本文件只写"怎么做"与设计取舍。

## Context

第一轮只读审计的事实底座在 `docs/architecture/current-state.md`（审计戳、证据等级、finding registry）与 `docs/development/testing.md`（命令台账）。设计只需知道以下约束：

- **审计戳（快照）**：HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`，branch `main`，与 `origin/main` 同点；工作区 **11 个 tracked 文件 modified**（index 为空，无 staged），**65 个 untracked non-ignored 条目**。
- **当前工作树（本变更编写时）**：**12 个 ` M`**、85 个 untracked（`-uall`，= 65 基线 + 20 个本变更新增）。多出的第 12 个 modified 是 **`.gitignore`**（快照后出现，新增一行 `MEMD.md`，作者**未知**），与本轮写作者无关，**只披露、不还原**。详见 `docs/architecture/current-state.md` §1.2。
- **仓库规模**：800 tracked；`ports/` 559、`packages/` 144、`test/` 29、`docs/` 19、`apps/` 17、`.github/` 16、`scripts/` 4、根文件 12。
- **门禁现状**：`node scripts/check-layout.mjs` 在 HEAD 上 **exit 1**（**4 处**违规，2 个 tracked 测试文件，未 modified；坐标是 checker 坐标（`stripComments` 后）`14/232/233/261`，原始源码行为 `31/238/239/267`）；`node scripts/check-sources.mjs` 在**快照时点的工作树** exit 0（附一条再分发复核警告；worktree 模式读工作树补丁表，**不等于** HEAD 事实）。`npm run check` 第一步即 `check:layout`，因此**在当前 HEAD 上不可能通过**。
- **工具链**：Node 本机 `v26.8.2`（CI pin `'22'`，见 `.github/workflows/build-packages.yml:160`、`build-wasm-artemis-bridge.yml:118`）；pnpm `9.14.2`；tsc `5.9.3`；Astro `5.18.2`；OpenSpec `1.14.1`（全局安装，非仓库依赖）。
- **依赖图**：静态扫描（manifest + tsconfig references + comment-stripped import 正则）结果是**每个包自成一个 singleton，无任何大小的环**；manifest 与 references 在包级一致。该结论只由静态边证明，不能排除运行时 service 环。**注意**：`apps/app` 的 manifest 有 10 项依赖，但启动链只值导入 3 包（`core`、`runtime-kag`、`runtime-artemis`）；`vfs→engine-api` 与 `runtime-tyrano→engine-api` 是 `import type`。完整边表见 `docs/architecture/current-state.md` §3。
- **OpenSpec 存量**：`openspec/specs/` 为空，本轮 7 项全部是新增 capability。

与本变更并列但**不在本轮执行**的输入：`docs/current-work.md` 的 Art 兼容线现状、用户对 clock-ms-02「还是不行」的实测反馈。它们属于 phase 1 之后的实现议题，不作为本变更的验收依据。

## Goals / Non-Goals

**Goals:**

- 把"应当成立"的边界写成可静态校验或可评审的条款（7 个 capability），使得以后任何一处的漂移都能被定位到某条 requirement。
- 建立一份**持久基线**：`docs/architecture/current-state.md` 之后作为架构事实的唯一入口，替代散落在 `README.md`、`docs/*.md` 与口头约定里的架构描述。
- 把第一轮审计的 finding 登记成稳定 ID + `路径:行号` + 建议范围 + 回滚 + 人工确认需求，使后续修复可以逐条授权。
- 明确"未证明"的部分：远程状态、真实引擎可用性、许可再分发权一律留在未知清单，不被目标条款吸收。

**Non-Goals:**

- 不实现任何一条 requirement。本变更只产出规划产物与文档。
- 不改 `packages/**`、`apps/**`、`ports/**`、`.github/**`、`package.json`、`pnpm-lock.yaml`、`README.md`、`LICENSE`、`THIRD_PARTY_NOTICES.md`。
- 不引入新构建器（Turborepo 一类）、不新增仓库、不拆分 package。
- 不修改 `openspec/config.yaml`，不 `apply`，不 `sync`，不 `archive`，不在 `openspec/specs/` 直接写入（该目录在 archive 前由 CLI 管理）。
- 不重写冻结的历史哈希（`docs/fetch-engine-artifacts.md`、`.github/upstream-pins.json` 的 `expected` md5）。

## Decisions

### D1 — 边界优先，不换构建器

**决定**：保留 `apps/app` + `packages/*` + `ports/*` 的 pnpm workspace 与 `tsc -b` 项目引用模型。

**理由**：`tsc -b tsconfig.packages.json` 在隔离副本上 exit 0（real 6.761s），Astro 静态构建 exit 0（Astro 自报 7.15s / Vite client 1.86s / wall 32.358s）。构建器本身不是当前瓶颈；当前瓶颈是**声明的依赖边与真实 import 不一致**，那不需要换工具就能修。

**备选**：Turborepo / Nx（引入新依赖与缓存语义，收益只在任务图规模，当前 14 个 importer 不值得）；把 13 个包合并（会丢掉公开发布边界，与 `ports/lib/engine-artifacts.json` 的消费契约冲突）。

### D2 — 依赖方向：单向，向下

**决定**：确立 `engine-api` 为最底层契约包；`vfs` 依赖 `engine-api`；`runtime-*` 依赖 `engine-api`/`vfs` 及其需要的较低层；`core` 与 `engines` 是**并列的**上层消费者，互不导入；`apps/app` 只经过 `core`。

**理由**：静态扫描已证明当前没有反向边（`runtime-kag/src/runtime.ts:40` 对 `tjs2-bytecode` 只是注释）。把这条现状升格为条款，成本为零，收益是禁止未来反向 import。

**备选**：允许 `engines` 聚合包反向依赖 `core`（会形成 core↔engines 的潜在环）；让 `core` 直接构造 provider 列表（现状，导致双注册，见 D3）。

### D3 — 组合根收敛到一处（目标，非本轮动作）

**决定**：`packages/engines/src/index.ts` 的 `ENGINE_PACKAGES` 与 `packages/core/src/platform.ts` 的 `BUILTIN_PROVIDERS` 描述同一组 8 个 provider；目标是让它们有单一权威来源（`core` 消费 `engines` 的注册表，或抽共享常量）。**本轮不改**。

**理由**：`apps/app` 只导入 `@online-gal/core`，不依赖 `@online-gal/engines`（`apps/app/package.json` 依赖里没有 engines）。因此 `engines` 目前没有产品调用者，是独立入口；两处手写列表随时可能漂移（顺序影响 tie-break，进而影响识别结果）。

**风险**：改变 provider 列表来源会改变注册顺序，进而改变引擎选择。所以列为 phase 3、需要人工批准与识别回归。

### D4 — 包命名：`engine-api` 保持不变，SDK 概念作为"边界"而非新包

**决定**：不重命名现有 `@online-gal/engine-api`；把"引擎 SDK"理解为**该包对外承诺的稳定契约面**（provider、registry、sources、compat、options、types）。只有当出现**真实独立消费者**（例如独立引擎仓库）时才讨论新包。

**理由**：重命名是破坏性变更且没有当前受益者；`smoke-consumer.mjs` 断言消费方顶层只能解析出 `@online-gal/engines`（vendored 计数 10、`runtimeId` 唯一、provider 各 8），说明发布面已经是隐藏实现细节的单一聚合包。

**备选**：现在就拆 `@online-gal/engine-sdk`（无消费者，纯增加维护面）；把 archive/storage 单独成包（见 D5）。

### D5 — 可选拆分只在出现真实独立性时做

**决定**：`vfs` 的 archive 读取（`./archives` 子导出）与 storage（`IndexedDbStore`）**暂不**拆包。判据是"是否存在只想要其中一半的消费者"。

**理由**：`image-codec` 只需要 `vfs` 的 `extname`；`runtime-*` 需要 `vfs` 的源抽象；`ports/lib/fetch-engine.mjs` 是 Node 侧脚本，不消费 `vfs`。当前没有独立消费者，拆包只会增加版本与发布矩阵。

### D6 — 引擎目录与 artifact manifest 作为目标形态

**决定**：目标是把"引擎字节按 URL 提供"升格为声明式 catalog + manifest（名字、大小、sha256、上游 commit、工具链、source offer/license 可用性），并要求缺失时撤回 provider。

**理由**：现状已经部分如此（`platform.ts` 用 `__ARTEMIS_ASSETS_AVAILABLE__` 编译期门禁、KiriKiri 用 `pageUrl`、ledger 校验大小+sha256）——目标是把它形式化，而不是重写。

**关键区分**：`new URL('../assets/', import.meta.url)` **不**等于"所有资产被打进 bundle"；`build` 成功也**不**证明引擎能运行。文档必须保持这三层结论分开。

### D7 — `scripting-permissions` 只定义边界

**决定**：QuickJS 一类脚本宿主与云同步后端在本变更中**只有接口与权限条款**，没有实现任务前置。

**理由**：tracked 全仓 grep 对 `quickjs` 与 cloud backend 均 0 命中；"没有实现"不等于"没有规划"——用户已明确这是未来目标。把边界先写下来，可以在实现时不必重开架构讨论。

**明确排除**：Artemis 内嵌的 Lua **不能**作为平台级脚本沙箱的替代（隔离假设不同）。这条写进 spec 的 `Engine-embedded scripting is not a sandbox` requirement。

### D8 — 文档语言与结构

**决定**：持久文档用中文（与 `AGENTS.md` 及既有 `docs/*.md` 一致），技术标识符、路径、命令、`path:line` 保留英文原样；每个事实附证据等级。

**理由**：本仓库的接手文档是中文；混排英文标识符保证 `grep` 与 `path:line` 可复核。

### D9 — 迁移按阶段授权，本变更不授权任何实现

**决定**：phase 0 的审计文档已交付但验证覆盖未完成（FIRST-ROUND-DOCUMENTATION-DELIVERED）；phase 1 起每一阶段都需要 supervisor/用户单独批准。本变更的 `tasks.md` 全部是 `- [ ]`（34 条，0 完成），粒度为"需要批准的动作 + 验证方式"。**没有** architecture migration 或产品改动被授权。

### D10 — 七个 capability delta 覆盖用户要求的七个面

用户提出的七个面与选定 delta 的对应关系（毋需额外 capability）：

| 用户面 | 对应 capability |
| --- | --- |
| 架构/边界/组合根 | `system-architecture` |
| VFS / archive / 存档 | `vfs-save-management` |
| 引擎 SDK 生命周期 | `engine-sdk-lifecycle` |
| 引擎目录与产物 manifest | `engine-catalog-loading` |
| 脚本沙箱权限 + 未来云同步 | `scripting-permissions` |
| 第三方/许可/SDK 与发布治理 | `third-party-release-governance` |
| 测试与兼容性证据 | `testing-compatibility` |

新增的 requirement 依然写成**未来时**（SHALL/MUST/MUST NOT）；当前观测继续隔离在 `docs/architecture/current-state.md`。


## Risks / Trade-offs

- **[规范写了但没人维护]** → 每条 requirement 绑定到 `current-state.md` 的 finding ID；finding 关闭时同步更新条款状态。文档结构固定，更新=追加证据而不是重写。
- **[把"目标"读成"现状"]** → 每份文档顶部标注证据等级图例；`proposal.md` 保留"当前观测 vs 目标 vs 假设"表；`tasks.md` 全部未勾选。
- **[组合根收敛改变识别结果]** → phase 3 才做，要求识别回归（同一组游戏目录的 provider 选择不变化）后才合并。
- **[依赖方向条款冻结了未来的合理重构]** → 条款只约束"反向 import"与"跨包深路径导入"，不约束包内结构；新增层级需要新 requirement 而不是绕过。
- **[manifest/ledger 与真实产物漂移]** → `make-package.mjs` 已在打包前按 ledger 校验引擎字节且无 opt-out；条款要求保持该行为并把"手动更新台账"事实写进文档。
- **[文档本身成为新的"陈旧真相"来源]** → `current-state.md` 带审计戳并在每次修订时保留历史修正段；`AGENTS.md` 的 2026-10-06 状态块就是反例，被明确标为历史。
- **[许可条款被读成法律结论]** → `third-party-inventory.md` 顶部声明"这是仓库自身声明的记录，不是法律意见"，未解决项保持未解决。

## Migration Plan

阶段划分、每阶段的前置条件/编辑域/影响半径/验收命令/环境需求/回滚/数据迁移风险/人工 go-no-go 见 `docs/architecture/migration-plan.md`。要点：

- **Phase 0（DONE）**：只读审计。产出 `docs/**` 新文档与 OpenSpec 规划产物。回滚 = 删除新增文件。
- **Phase 1（需批准）**：修 `check:layout` 的 4 处 HEAD 违规（把 tracked 测试文件里的 `.test/` 工作台路径移出跟踪范围或改成运行期注入），使 `npm run check` 的第一步可过。**这一步不改任何产品行为。**
- **Phase 2（需批准）**：数据契约。修 `SaveStore.fileStore` 的硬编码 DB 名、`GameLibrary.remove/moveStoredFiles` 的双前缀、`remove` 不回收 overlay，以及两套 path normalizer。**先加不运行引擎的键空间测试，再改实现，再考虑是否需要迁移已有数据。**
- **Phase 3（需批准）**：组合根与 SDK 边界收敛、catalog/manifest 形式化。
- **Phase 4（需批准）**：发布治理（SHA pin、发布门禁统一、CODEOWNERS、备份与恢复记录）。
- **Phase 5（可选，需批准）**：插件/后端等未来能力。

**回滚策略**：phase 1–3 都是小范围源代码改动，逐条 revert 即可；phase 2 如果引入了键迁移，回滚必须先回滚迁移脚本再回滚代码，并保留迁移前的导出备份。phase 4 的 workflow 改动逐行 revert；**不得**通过改写冻结哈希来"回滚"。

## Open Questions

1. `runtime-kag` 的 manifest/references 声明了 `image-codec`，但 `runtime-kag/src` 无任何 import（factual gate 结论）。这是有意重导出还是漂移？需要 owner 裁定——它只影响 D2 条款的"声明但未使用"分支，不改变条款本身。
2. `docs/current-work.md` 的 checkpoint 更新时机由 supervisor 在 review 后单独授权；本变更不含该编辑。
3. 独立引擎仓库是否要建（版权/维护者/构建/权利四问）见 `migration-plan.md` 的决策选项；本轮不做决定。
4. 远程 branch protection、Actions allow-list、`ARTEMIS_PUBLISH_ARTIFACTS` 变量值与发布资产是否存在：**未查询**，保持未知。
