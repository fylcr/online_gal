# 迁移计划（Migration Plan）

> **没有任何阶段被批准。** 除 phase 0（审计文档已交付，但验证覆盖未完成）外，phase 1–5 全部处于"待授权"状态。本文件登记每个阶段的前置条件、编辑域、影响半径、验收命令、环境需求、回滚、数据迁移风险与 go/no-go 判据；登记**不是**授权。
>
> **时点**：本文件的"11 个 modified"类描述是对**审计快照**的描述；**当前工作树为 12 个 modified**（多出 `.gitignore`，外部改动，作者未知）。详见 `current-state.md` §1.2。
>
> 与目标状态的对应见 `target-state.md`；finding 的完整证据见 `current-state.md` §5。

**图例**：`DONE` 已完成（有证据）｜`PENDING-APPROVAL` 需人工批准后才可动｜`OPTIONAL` 默认不做。

---

## Phase 0 — 只读审计基线 · **FIRST-ROUND-DOCUMENTATION-DELIVERED**（验证覆盖未完成）

| 项 | 内容 |
| --- | --- |
| 目标 | 在不改任何产品字节的前提下，产出可复核的架构/治理/测试基线 |
| 前置条件 | 用户授权"只读 + 写新文档"；确认无在途 writer |
| 编辑域 | **仅新增**：`docs/architecture/*`、`docs/governance/*`、`docs/development/*`、`openspec/changes/establish-architecture-and-release-baseline/**` |
| 影响半径 | 零产品影响；`git status` 从 65 untracked 变为 65 + **20 个新增文件**（9 文档 + 11 OpenSpec）；tracked 数量不变（800），但工作树 modified 从快照的 11 **变为 12**（多出的 `.gitignore` 是外部改动，非本写作者，见下） |
| 验收命令 | `git status --porcelain`（快照时期望 11 个 ` M`，**当前为 12**，差额已披露）；`sha256sum -c` 于 `.test/audit-round1-ce058ba0/snapshot/{tracked-hashes,untracked-hashes}.sha256`（**快照时点** 800/65 均 exit 0；今天是 799/800 + 65/65，唯一差异是 `.gitignore`）；`openspec validate … --strict`；**加上**在隔离副本上执行的 `tsc -b tsconfig.packages.json`、两个资产拷贝、`astro build`、`node --test test/*.test.mjs`（364/350/14） |
| 环境需求 | Git Bash、Node ≥ 20（本机 26.8.2；CI pin 22）、pnpm 9.14.2、OpenSpec 1.14.1（全局） |
| 回滚 | 删除新增的 20 个文件即可；无数据迁移。**不得**动 `.gitignore` 或 `MEMD.md`（不属本写作者） |
| 数据迁移风险 | 无 |
| Go/no-go | 文档已交付；**未授权**任何产品/脚本/CI/依赖改动，也**未**取得全仓验收 |

**Phase 0 的准确边界（纠正早前过度声明）**：

- **已做**：审计文档交付；**在隔离副本上执行了**构建与 node 测试（`tsc -b`、两个资产拷贝、`astro build`、`node --test test/*.test.mjs`）；只读的 `check:layout` / `check:sources` / lockfile 一致性 / openspec 只读子命令。
- **未做（不改写成"做过"）**：没有改任何产品源码；没有装依赖；没有开浏览器/服务/真游戏；没有发布重现；没有 stage/commit/push；没有改 `docs/current-work.md`；没有查远程 GitHub 状态；没有跑 6 个原生探测与 26 个工作台测试。
- 因此结论是 **FIRST-ROUND-DOCUMENTATION-DELIVERED + 验证覆盖未完成**，**不是** whole-repo acceptance complete；隔离副本的构建/测试**不是** CI 复现（Node 26.8.2 vs CI 22），且 `astro build` 只走了 **withdrawn 分支**（隔离副本故意无 `dist/artemis-compat`）。
- **不**将本阶段的结论用于任何"引擎可用"或"201 历史烟雾"的现值声明。

---

## Phase 1 — 门禁可过 · **PENDING-APPROVAL**

| 项 | 内容 |
| --- | --- |
| 目标 | 让 `npm run check` 的第一步（`check:layout`）在 HEAD 上可过；让 patch 清单契约自洽 |
| 前置条件 | ① 用户/supervisor 明确批准；② 记录 phase 1 前的保护哈希（工作树会变，必须先冻结比较基准）；③ 确认 `ports/artemis/compat-web/tests/*` 的两个文件在 HEAD 上未被修改 |
| 编辑域 | `ports/artemis/compat-web/tests/artemis-compat-first-transition.test.mjs`、`artemis-compat-tween.test.mjs`（把 `.test/` 工作台路径改为运行期注入或工作台外临时目录）；`test/artemis-compat-ci.test.mjs` 或 `ports/artemis/compat-web/PLATFORM-PATCHES.txt`（二者取一，修 patch 字面量漂移） |
| 影响半径 | 测试文件与清单；**不改**任何产品行为、不改构建产物、不改 workflow。两个原生探测测试的行为可能从"运行"变为"跳过"（若工作台不可用），必须记录 |
| 验收命令 | `node scripts/check-layout.mjs` → exit 0；`node --test test/artemis-compat-ci.test.mjs` → 通过（当前断言在 `:357`/`:601`）；`node scripts/check-sources.mjs` → exit 0（它按 `PLATFORM-PATCHES.txt` 双向校验补丁表，worktree 模式读工作树、`--staged` 模式用 `git show`）；`git status --porcelain` 复核改动范围 |
| 环境需求 | Node ≥ 20（CI 为 22）；原生探测另需 `g++`、`zlib.h`、`tar`、`nm` 与 `.ref/artemis-compat` 检出于 pin `17200e64…` —— **本机 `zlib.h` 缺失**，因此原生探测在本机 **BLOCKED**，不得据此判失败 |
| 回滚 | 逐文件 revert |
| 数据迁移风险 | 无 |
| Go/no-go | 需要人工确认：是否要让 `check` 在 HEAD 上可过；patch 清单应"期望 5"还是"应保持 4" |
| 风险 | 把工作台路径改到 `.test/` 之外会改变测试的落盘位置；必须确认测试仍能运行，而不是变成静默跳过 |

**关键约束**：phase 1 只碰测试与清单，**不碰** `packages/**` 的产品源码。

---

## Phase 2 — 数据契约与删除安全 · **PENDING-APPROVAL**

| 项 | 内容 |
| --- | --- |
| 目标 | 让存档/覆层/导入的键空间与删除/迁移语义自洽；消除两套 path 规范化的分歧 |
| 前置条件 | ① phase 1 完成（或至少门禁状态已知）；② 批准；③ **先产出数据迁移与回滚方案**，并且先加**不运行引擎**的键空间测试（测试必须在当前实现上失败，才算"复现"） |
| 编辑域 | `packages/core/src/storage.ts`（`SaveStore.fileStore` 的 `DB_NAME` 硬编码）；`packages/core/src/library.ts`（`remove`/`moveStoredFiles` 的键前缀、overlay 回收、多步事务）；`packages/vfs/src/path.ts` 与 `packages/engine-api/src/sources.ts`（统一 normalizer）；测试文件 |
| 影响半径 | **用户可见**：删除游戏会真正回收字节；改名/移动会搬运字节；如果键形状改变，**已有存档可能读不到** |
| 验收命令 | 新增的键空间测试（不启动引擎）；`node --test test/*.test.mjs` 的 node-only 子集；`tsc -b tsconfig.packages.json`；**另需**手动：用临时 DB 写入三类键 → `remove` → 核对剩余键 |
| 环境需求 | Node ≥ 20（CI 22）；浏览器/IndexedDB 行为的人工核对（`node --test` 用 jsdom 或显式 mock，需确认哪一层真正覆盖 IDB） |
| 回滚 | **顺序关键**：先回滚迁移脚本/键迁移，再回滚代码；迁移前必须留导出备份 |
| 数据迁移风险 | **高**。若改键形状，既有用户存档需要迁移；若只改删除匹配条件，旧数据会被漏删。两者都必须先测试后改，且必须先有备份方案 |
| Go/no-go | 需要人工确认：是否接受改变键形状；是否接受为已存在的残留数据提供清理路径 |
| 风险 | 改动删除语义可能**看似**清理了数据但实际扩大了删除范围。必须用一个只含测试数据的临时 DB 验证 |

**明确不做**：不先改 schema；不在没有测试的情况下"顺手"统一两个 normalizer。

---

## Phase 3 — 组合根、SDK 边界与引擎目录 · **PENDING-APPROVAL**

| 项 | 内容 |
| --- | --- |
| 目标 | 组合根唯一；依赖声明与真实 import 一致；引擎产物形式化为 catalog + manifest |
| 前置条件 | ① phase 2 完成或明确解耦；② 批准；③ 识别回归基线（同一组测试游戏目录当前的 provider 选择结果）已记录 |
| 编辑域 | `packages/core/src/platform.ts` 与 `packages/engines/src/index.ts`（注册表收敛）；`packages/core/tsconfig.json`、`packages/image-codec/tsconfig.json`（补 references）；`packages/runtime-kag`（裁定 `image-codec` 声明）；引擎 manifest/catalog 相关脚本；`apps/app/**` 仅在必要时 |
| 影响半径 | **引擎选择结果可能变化**（注册顺序 → tie-break → 识别）；构建顺序变化；catalog 形式化会触及产物加载路径 |
| 验收命令 | 识别回归：对固定的一组测试游戏目录，改动前后 provider 选择一致；`tsc -b tsconfig.packages.json` exit 0，且清理 `dist/` + `*.tsbuildinfo` 后 `core` 构建顺序落在两个 runtime 之后；`node .github/scripts/smoke-consumer.mjs`（校验消费方顶层集合、vendored 计数、`runtimeId` 唯一） |
| 环境需求 | Node ≥ 20（CI 22）；pnpm（smoke-consumer 需要 `pnpm add --offline`）；构建产物目录可写 |
| 回滚 | 逐文件 revert；注册表收敛要能退回两处手写（保留旧列表直到回归通过） |
| 数据迁移风险 | 低（不涉及用户数据）；但识别变化会让原本可运行的游戏走不同 provider |
| Go/no-go | 需要人工确认：是否接受收敛后注册顺序的任何变化；`runtime-kag` 的 `image-codec` 是有意还是漂移 |
| 风险 | 收敛可能让某个"本来能识别的游戏"改走更弱的 provider。回归必须在**真实游戏目录集**上做，不能只靠单元测试 |

---

## Phase 4 — 发布治理与恢复 · **PENDING-APPROVAL**

| 项 | 内容 |
| --- | --- |
| 目标 | 发布只由显式人工触发；外部 Action 固定到 commit SHA；pin 记录一致；许可关键路径有 owner；备份与恢复状态被记录 |
| 前置条件 | ① phase 1–3 的产物稳定；② 批准；③ **owner 必须先裁定 Artemis pin 冲突**（`9f622629…` vs `17200e64…`）与 ONS/krkr 的 `push` 发布是否为有意 |
| 编辑域 | `.github/workflows/**`（`uses:` 固定 SHA、publish 表达式、`real-game` 作业权限与 `puppeteer` 安装）；`.github/upstream-pins.json`（**仅 `$comment`**，不动 `commit`/`expected`）；`THIRD_PARTY_NOTICES.md` 与 `ports/README.md` 的措辞；新增 `.github/CODEOWNERS`、可能的 `.github/dependabot.yml` 与 `SECURITY.md` |
| 影响半径 | 发布路径、供应链、文档口径；**不影响**运行时行为 |
| 验收命令 | `grep -n "uses:" .github/workflows/*.yml` 全部为 40-hex + 注释；三个 publish 表达式并排核对；一次 push 只上传工件不建 release；`scripts/check-sources.mjs` 仍 exit 0；`build-artemis-compat.sh` 的 `UPSTREAM_COMMIT == ARTC_PIN` 断言仍存在且 `.sha256` 自校验可复现 |
| 环境需求 | 需要**远程**访问才能验证的项（branch protection、Actions allow-list、release 资产、`ARTEMIS_PUBLISH_ARTIFACTS`）必须单独授权后查询；本地只能做出 workflow 语法与静态核对 |
| 回滚 | workflow 逐行 revert；CODEOWNERS 直接删除；**不得**通过改写冻结哈希来"回滚" |
| 数据迁移风险 | 无 |
| Go/no-go | **必须人工确认**：pin 裁定的结论；发布是否改为仅显式派发；SHA pin 是否接受手动 bump |
| 风险 | 把 `uses:` 固定到 SHA 后，安全更新变成手动流程；必须同时决定更新通道（Dependabot 或定期人工） |

**绝不能做**：改写 `.github/upstream-pins.json` 的 `expected` md5/字节数，或 `docs/fetch-engine-artifacts.md` 的历史 hash——它们是冻结的证据记录。

---

## Phase 5 — 可选：插件、脚本沙箱与后端 · **OPTIONAL**

| 项 | 内容 |
| --- | --- |
| 目标 | 若真有需求：浏览器内脚本宿主（能力授权模型）与可选的云同步（以导出/导入契约为缝） |
| 前置条件 | ① 用户明确提出该需求；② 已有沙箱能力授权模型与威胁评审；③ 冲突合并规则先于传输实现 |
| 编辑域 | 尚未确定；届时新开变更（本变更只提供 `scripting-permissions` 的边界条款） |
| 影响半径 | 高：引入新的执行环境与（可能）网络传输 |
| 验收命令 | 届时定义；最低要求是 `scripting-permissions` 的 4 条 requirement 各有对应测试，且"无后端仍完整可用"有断网验证 |
| 环境需求 | 浏览器；若涉及 QuickJS，需确认 WASM 构建线与之兼容 |
| 回滚 | 后端能力必须可关；关闭后静态部署仍完整可用 |
| 数据迁移风险 | 高（同步会碰存档） |
| Go/no-go | 需要人工确认 |
| 风险 | 把引擎内嵌的 Lua 误当作沙箱（**已明确禁止**，见 `target-state.md` §6） |

---

## Phase 6 — 独立引擎仓库 · **决策选项（本轮不决定）**

是否把某个引擎（最可能是 Artemis compat）独立成仓库、由外部维护者构建并发布？**本轮不做决定**，只登记需要回答的四个问题：

| 问题 | 需要什么才能回答 | 选项 |
| --- | --- | --- |
| **版权 / 许可** | Artemis SDK 的再分发许可是否存在书面grant；compat 路线（编译上游源码 + 补丁 + overlay）与其转译体的许可状态 | (a) 取得书面许可后独立发布；(b) 保持 `publish: false` 直到许可明确；(c) 完全不独立 |
| **维护者** | 谁对该仓库的 pin、补丁 rebase 与发布负责 | (a) 本仓库 owner 兼管；(b) 指定外部维护者 + CODEOWNERS；(c) 无人 → 不独立 |
| **构建** | 独立仓库能否自洽地重建产物（emsdk 6.0.9、补丁集、overlay、source offer 自校验） | (a) 复用现有 workflow 的可复用部分；(b) 独立 CI；(c) 无法自洽 → 不独立 |
| **权利 / 治理** | 独立发布是否会把许可风险转移给消费者 | (a) 独立发布并各自声明许可；(b) 只在私有渠道交付；(c) 不发布 |

**约束**：本文件的任何步骤都**不得**用于规避平台策略或执法；目标是让字节的来源、许可与责任人可被如实记录，而不是让它们更难被追溯。

---

## 跨阶段规则

1. **每个阶段独立授权**。批准一个阶段**不**等于批准下一个。本文件的存在不代表任何授权。
2. **每阶段先冻结比较基准**（工作树的哈希快照），再动；动后复核。**保护期望是"初始 dirty 集合被保留 + 外部改动被披露"，不是把 11 永远硬编码**：本轮就出现了这种情况（`.gitignore` 在快照后变为第 12 个 modified，作者未知，只披露不还原，见 `current-state.md` §1.2.1 与 `../governance/repository-risk.md` R-12）。
3. **命令台账**：每阶段的验收命令连同 cwd、Node/pnpm 版本、退出码与结论分类（通过 / 失败 / 阻塞 / 未运行）记录在 `../development/testing.md` 的口径下。
4. **五结论分离**：源码证据 / 构建成功 / 构建产物 / 真实游戏可运行 / 再分发授权是五个不同结论，任何阶段的报告都不得把它们混为一谈。
5. **不改冻结项**：`.github/upstream-pins.json` 的 `expected`、`docs/fetch-engine-artifacts.md` 的历史 hash、m03520 冻结的鼠标映射缺陷。也**不**更新受保护的 `.test/audit-round1-ce058ba0/snapshot/` 台账。
6. **`docs/current-work.md` 的 checkpoint** 由 supervisor 在 review 之后单独授权，不属于任何阶段的默认动作（本轮仍未写入）。
