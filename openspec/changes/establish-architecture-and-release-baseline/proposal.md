# Proposal: establish-architecture-and-release-baseline

## Why

第一轮只读审计（2026-10-13，HEAD `9dd18ec`）发现：仓库的架构边界、包依赖方向、存档数据契约、引擎交付与发布治理目前**只存在于散落的散文与口头约定里**，没有可校验的规范，也没有一个"当前状态"基线把「已证明 / 未证明 / 未知」分开。结果是同一批事实在不同文档里互相矛盾（Artemis 出货路线、上游 pin、发布门禁），构建/测试门禁在 HEAD 上失败却无人拥有修复授权，而 `check:layout` 的失败点正是把 `.test/` 工作台路径写进了 tracked 测试文件。本变更把这些事实固化为可校验的规范与按阶段授权的迁移计划，**不实现任何修复**。

## What Changes

- **新增 7 个 capability delta**，把"应当成立"的边界写成可校验条款（SHALL/MUST + Scenario），覆盖：模块依赖方向与组合根、VFS/存档数据契约、引擎生命周期、按需引擎目录加载、脚本沙箱与权限边界、第三方与发布治理、测试与兼容性证据。
- **新增 9 份持久审计文档**（`docs/architecture/**`、`docs/governance/**`、`docs/development/**`），承载第一轮的实测事实、证据等级、finding registry 与未知项；这些文档是规范的事实底座，不是新的产品行为。
- **不修改任何产品代码、脚本、CI、依赖、README、许可证**。不构建、不测试、不发布。
- **迁移按阶段授权**：phase 0（审计）已完成；phase 1（门禁修复）起每一阶段都需要单独的人工批准，本变更只登记阶段、前置条件、编辑域、验收命令与回滚，不执行。

### 当前观测 vs 目标 vs 假设

| 主题 | 当前观测（有证据） | 本变更的目标 | 明确标为假设/未知 |
| --- | --- | --- | --- |
| 模块依赖方向 | 13 包 + app，manifest 依赖图无环（每个包是 singleton）；`core`/`engines` 各自注册同一组 provider | 依赖方向成为可校验条款；重复注册收敛到一个组合根 | "无环"只由静态 import/manifest 扫描证明，**不能**排除运行时 service 环 |
| TS project references | `core/tsconfig.json` 缺 `runtime-siglus`、`runtime-artemis`；`image-codec/tsconfig.json` 缺 `vfs` | references 与真实 import 一致 | 是否曾导致过期产物尚未实测 |
| 存档数据契约 | `SaveStore.fileStore` 硬编码 `DB_NAME`；`GameLibrary.remove/moveStoredFiles` 双前缀；`remove` 不回收 overlay | 删除/迁移必须回收该 game id 下的全部键 | 是否有真实数据损失**未复现**（缺陷为 source-level contract defect） |
| 引擎生命周期 | `load/pause/dispose` + `Session.start/stop` 已存在，但未写成契约 | 生命周期与资源释放成为规范 | 各引擎 dispose 的真实释放效果未逐引擎实测 |
| 引擎目录/交付 | 引擎字节按 URL 提供，**未**打包进 bundle；Artemis absent 时 provider 必须撤回 | 按需目录 + artifact manifest（URL/sha256/ABI/source offer/license）成为目标 | 远程 release 是否存在**未知** |
| 脚本权限 | 产品代码无 QuickJS、无 cloud backend（tracked grep 0 命中） | 只为未来脚本/云同步定义最小接口与权限边界 | 是否要建、何时建是**用户未来目标**，不是本轮需求 |
| 发布治理 | 7 个 workflow 的 `uses:` 全是可变 tag；ONS/krkr 在 `push` 上发布；`publish:false` 只硬编码在 compat | 发布门禁、SHA pin、source offer 一致性成为规范 | 远程 branch protection / 备份 / Actions allow-list **未查证** |

## Capabilities

### New Capabilities

- `system-architecture`: 定义本仓库允许的模块划分（`apps/`、`packages/`、`ports/`）、单一组合根、公开 API 与依赖方向，以及文档化基线的持久位置。
- `vfs-save-management`: 定义游戏只读层 + 可写覆层、存档槽、IndexedDB 键空间与删除/迁移的完整回收契约，以及唯一路径规范化规则。
- `engine-sdk-lifecycle`: 定义引擎 provider 与宿主会话的稳定生命周期契约（probe / load / pause / dispose 与 `Session.start/stop`）及资源释放责任。
- `engine-catalog-loading`: 定义引擎产物按需加载（catalog 条目、artifact manifest 的 URL/sha256/ABI、source offer 与 license 可用性）以及缺失时必须撤回而非给出 404 URL。
- `scripting-permissions`: 定义未来浏览器内脚本执行（QuickJS 一类）与云同步后端的最小接口与权限/隔离边界；**仅接口，不含实现前置**。
- `third-party-release-governance`: 定义第三方（含专有 SDK）字节的清单、许可状态记录、source offer 与二进制一致性，以及发布门禁。
- `testing-compatibility`: 定义测试分层（源码级 vs 浏览器/真游戏 oracle）、证据纪律（丢弃一次热身 + 两次一致）、命令台账与可移植性边界。

### Modified Capabilities

（无。`openspec/specs/` 当前为空，7 项全部是新增 capability。）

## Impact

- **文档轮只新增文件**：`openspec/changes/establish-architecture-and-release-baseline/**`、`docs/architecture/{current-state,target-state,migration-plan,module-dependency-rules}.md`、`docs/governance/{third-party-inventory,repository-risk,ci-release-policy}.md`、`docs/development/{workflow,testing}.md`（共 **20 个**：9 文档 + 11 OpenSpec）。
- **保护时点（不是“零改动”）**：基线台账是 800 tracked + 65 untracked、**11 个 ` M`**；本轮的**当前**工作树是 **12 个 ` M`** —— 多出的 `.gitignore` 是**快照后出现的外部改动**（新增一行 `MEMD.md`，作者未知），**不**由本轮写作者产生，**不**改写、**不**还原。另新增 20 个 untracked 文件。详见 `docs/architecture/current-state.md` §1.2。
- **不改**：产品代码、脚本、`.github/**`、`package.json`、`pnpm-lock.yaml`、依赖、`README.md`、`LICENSE`、`THIRD_PARTY_NOTICES.md`、`docs/fetch-engine-artifacts.md`（历史 hash 冻结）、`openspec/config.yaml`、`.gitignore`、`MEMD.md`。
- **不改** `docs/current-work.md`：最终 checkpoint 编辑由 supervisor 在 review 之后单独授权。
- **受影响系统（未来，需逐阶段批准）**：`packages/core`（组合根与 storage）、`packages/vfs`（键空间与 path 规范化）、`packages/runtime-*`（生命周期）、`.github/workflows/**`（发布门禁与 SHA pin）、`.github/CODEOWNERS`（新建）。
- **判断依据（事实底座）**：`docs/architecture/current-state.md` 的 finding registry 与证据等级表；`docs/development/testing.md` 的命令台账。本变更的所有条款都以那些 `path:line` 为依据，未列出的部分标为未知。
