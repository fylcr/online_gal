# Tasks

> 全部任务保持未勾选（**34 条，`openspec list` 记 0/34**）。**没有任何一条实现工作被授权**。
>
> **Phase 0 的实际范围**：审计**文档交付**完成，并且**在隔离副本上执行过** `tsc -b tsconfig.packages.json`、两个资产拷贝脚本、`astro build`、`node --test test/*.test.mjs`（364/350/14）。这些**不是** CI 复现（本机 Node 26.8.2 vs CI pin 22），也**不**等于全仓验收。因此本变更标为 **FIRST-ROUND-DOCUMENTATION-DELIVERED（验证覆盖未完成）**，**不是** whole-repo acceptance complete。4/4 planning artifacts 完成只说明"计划存在"，与"实现完成"是两个结论。
>
> **保护时点**：基线 800 tracked + 65 untracked + **11 个 ` M`**；**当前** 12 个 ` M`（多出 `.gitignore`，外部改动，作者未知）。详见 `docs/architecture/current-state.md` §1.2。

## 1. Phase 0 — 审计基线与持久文档（本轮交付，不含实现）

- [ ] 1.1 确认 9 份新文档存在且不含产品行为改动：`docs/architecture/{current-state,target-state,migration-plan,module-dependency-rules}.md`、`docs/governance/{third-party-inventory,repository-risk,ci-release-policy}.md`、`docs/development/{workflow,testing}.md`；验证方式：`ls` 上述路径并抽查每份文件顶部的审计戳与证据等级图例。
- [ ] 1.2 确认每份文档的 `path:line` 引用可在当前 HEAD 上复读；验证方式：随机抽取不少于 10 条引用逐一 `read`/`grep` 核对。
- [ ] 1.3 确认保护范围：对 `.test/audit-round1-ce058ba0/snapshot/{tracked-hashes,untracked-hashes}.sha256` 覆盖的既有文件做 after 复核；验证方式：`sha256sum -c`（800+65 项，**快照时点**全 exit 0），并核对 `git status --porcelain` 为 "**初始 dirty 集合被保留 + 已披露的外部改动**"（当前是 12 个 ` M`：11 基线 + `.gitignore`），而不是把 11 硬编码。**不得**因出现外部改动而还原它。
- [ ] 1.4 确认 `openspec/config.yaml` 与 `openspec/specs/` 未被本次变更写入；验证方式：`git status --porcelain` 中不出现对 `openspec/config.yaml` 的修改，`openspec/specs/` 仍只有 `.gitkeep`。

## 2. Phase 1 — 门禁可过（需另行批准；不改产品行为）

- [ ] 2.1 取得单独批准后再动，前置：确认 4 处 `check-layout` 违规的 **checker 坐标**为 `artemis-compat-first-transition.test.mjs:14`、`artemis-compat-tween.test.mjs:232`、`:233`、`:261`（`stripComments` 后；原始源码坐标为 `:31`、`:238`、`:239`、`:267`），且这两个文件在 HEAD 上未被修改；验证方式：`node scripts/check-layout.mjs` 输出与上述 checker 坐标一致、`git status --porcelain` 中不含这两个文件。
- [ ] 2.2 把 tracked 测试文件里的 `.test/` 工作台路径改为运行期注入或工作台外的临时目录，使跟踪文件不再引用 `.test/`；验证方式：`node scripts/check-layout.mjs` exit 0，且这两个测试文件在原生探测环境下的行为变化被记录（可用/跳过/失败）。
- [ ] 2.3 复核 patch 清单契约漂移：`PLATFORM-PATCHES.txt` 在 HEAD 上列出 `0001…0005` 共 5 项（已由 `git show` 只读查证），而 `test/artemis-compat-ci.test.mjs:357`/`:601` 与 `ports/artemis/compat-web/tests/artemis-compat-animation-observe.test.mjs:91` 的字面量期望 4 项（工作树是 6）；验证方式：`git --no-optional-locks show HEAD:ports/artemis/compat-web/PLATFORM-PATCHES.txt` 与两个测试断言并排核对，再 `node --test test/artemis-compat-ci.test.mjs` 记录实际/期望（HEAD 5 / 工作树 6 / 测试 4，三者分开写）。
- [ ] 2.4 修正 2.3 的契约（改测试字面量或补清单声明，二者取一）使断言与 HEAD 一致；验证方式：`node --test test/artemis-compat-ci.test.mjs` 通过，且 `node scripts/check-sources.mjs` 仍 exit 0（它按本文件双向校验补丁表）。
- [ ] 2.5 记录 phase 1 后的门禁状态：`npm run check` 的第一步与 TS/app 步骤分别在何种环境下通过；验证方式：在干净副本上跑 `node scripts/check-layout.mjs`、`tsc -b tsconfig.packages.json`、`pnpm --filter @online-gal/app check` 并留存退出码（`astro check` 当前从未运行过，必须记录）。

## 3. Phase 2 — 数据契约与删除安全（需另行批准）

- [ ] 3.1 先加不运行引擎的键空间测试：覆盖 `SaveStore.fileStore`、`GameLibrary.openSource` overlay、`GameLibrary.remove`/`moveStoredFiles` 的写入键形状；验证方式：新增测试在**当前实现**上失败，且失败信息打印实际键与期望键（这是"缺陷已复现"的判据，而不是运行游戏）。
- [ ] 3.2 修 `SaveStore.fileStore` 的硬编码数据库名，使 `dbName` 参数在记录存储与引擎可写文件存储之间一致；验证方式：以非默认 `dbName` 构造后断言两类键落在同一 `IDBDatabase`，且 3.1 的对应用例转绿。
- [ ] 3.3 修 `GameLibrary.remove`/`moveStoredFiles` 的键前缀处理（`list()` 已返回 id 前缀，调用方不得再次拼接）；验证方式：3.1 的 remove/move 用例转绿，且删除后该 id 下无残留键。
- [ ] 3.4 让删除回收引擎可写覆层（`saves:<id>` 前缀）字节；验证方式：写入覆层文件后调用删除，断言覆层键不再存在，且其他 game id 的键保持不变。
- [ ] 3.5 补 `importAll` 与库删除的多步失败语义（要么包进单事务，要么定义可恢复的中间态）；验证方式：注入中途失败后断言残留状态符合文档化定义。
- [ ] 3.6 统一路径规范化：选定 `vfs` 的 `normalisePath` 语义为唯一规则，让 `engine-api` 的 `MemoryGameSource`/`OverlayGameSource` 与之对齐；验证方式：同一输入（含 `..`、`\`、前导 `/`、空段）在两层的输出逐例相等，且差异测试在改动前先失败。
- [ ] 3.7 在改动任何键形状之前先产出数据迁移与回滚方案（含导出备份步骤、迁移脚本、回滚顺序），并经人工 go/no-go；验证方式：方案文档存在、包含"回滚必须先回滚迁移脚本"的顺序，且在被批准前 3.2–3.6 不合并。

## 4. Phase 3 — 组合根、SDK 边界与引擎目录（需另行批准）

- [ ] 4.1 收敛 provider 注册的唯一权威来源，使 `packages/core/src/platform.ts` 不再维护与 `packages/engines/src/index.ts` 重复的手写列表；验证方式：注册顺序在改动前后对同一组测试游戏目录产生相同选择（识别回归），且重复 `runtimeId` 仍在加载期报错。
- [ ] 4.2 消除 manifest/references 与源码 import 的不一致：补 `packages/core/tsconfig.json` 缺失的 `runtime-siglus`、`runtime-artemis` 引用，补 `packages/image-codec/tsconfig.json` 缺失的 `vfs` 引用；验证方式：`tsc -b tsconfig.packages.json` exit 0，且在清理 `dist/` 与 `*.tsbuildinfo` 后 core 的构建顺序落在两个 runtime 之后。
- [ ] 4.3 裁定 `runtime-kag` 声明但未 import 的 `image-codec` 边（有意重导出或删除声明）；验证方式：`grep` 确认 `runtime-kag/src` 无 import 后由 owner 给出结论，并按结论二选一落地，且 `tsc -b` 仍 exit 0。
- [ ] 4.4 把引擎目录形式化为声明式 catalog + artifact manifest（名字/大小/sha256/上游 commit/工具链/source offer 与 license 可用性）；验证方式：`ports/lib/fetch-engine.mjs` 的校验行为不变（大小+sha256 先校验后落位、exit 2/3 语义保持），且缺失产物时 provider 撤回而不是发布 404 URL。
- [ ] 4.5 把"资产 emit 路径"从"撤回分支"中分离验证：在存在候选产物的部署下走 emit 分支；验证方式：构建输出目录出现被复制的引擎资产且日志记录 emitted 数量（本轮隔离构建只覆盖了 withdrawn 分支，emit 路径仍未被证明）。

## 5. Phase 4 — 发布治理与恢复（需另行批准）

- [ ] 5.1 把 7 个 workflow 中所有外部 `uses:` 固定到完整 40 位 commit SHA（版本号保留为注释）；验证方式：`grep -n "uses:"` 输出的每一项都是 40-hex + 注释，且工作流语法仍被 GitHub 接受（dry-run 或 lint）。
- [ ] 5.2 统一发布门禁：使 ONS/KiriKiri 与 `build-packages.yml` 采用同一"仅显式派发且 `publish==true` 才发布"的判据；验证方式：三个 workflow 的 publish 表达式并排核对，且一次 push 只上传工件不创建 release（需人工确认这是否是 owner 的意图）。
- [ ] 5.3 消除 Artemis pin 记录冲突：文档所称 compat pin 与实际 workflow 使用的 commit 必须一致；验证方式：`.github/upstream-pins.json`、`THIRD_PARTY_NOTICES.md`、`build-wasm-artemis-compat.yml` 三处并排核对，且**不改写**冻结的历史 md5/expected 值。
- [ ] 5.4 收紧最弱权限：`build-wasm-artemis-bridge.yml` 的 `real-game` 作业只用所需权限，且其中未固定的 `npm install --no-save puppeteer` 被固定或移入受评审的依赖；验证方式：该作业的 `permissions:` 与安装命令核对，且安装不再无版本约束。
- [ ] 5.5 指定许可关键路径的 CODEOWNERS 并要求评审（`ports/artemis/vendor/**`、`ports/artemis/compat-web/**`、`packages/runtime-ons/assets/**`）；验证方式：CODEOWNERS 条目与实际路径匹配，且远程 branch protection 状态被记录为"已查证"或"未知"而不是"不存在"。
- [ ] 5.6 记录 source offer 与二进制的同提交一致性证据（pinned `git archive` + patches + overlays + 校验和自校验）；验证方式：`build-artemis-compat.sh` 的 `UPSTREAM_COMMIT == ARTC_PIN` 断言仍在，且 `.sha256` 自校验步骤在本地可复现。
- [ ] 5.7 产出备份与恢复记录：区分仓库内可重建（源码、脚本、workflow）与只存在于单机的部分（被 gitignore 的构建产物、`.ref/` 检出、`.test/` 工作台、真实游戏数据）；验证方式：记录每一类的重建命令或明确标为不可重建，且远程侧状态标为未知直到实际查询。

## 6. Phase 5 — 可选：插件/脚本沙箱与后端（默认不做）

- [ ] 6.1 只在出现真实需求时启动；若启动，先产出沙箱能力授权模型（**默认拒绝**，逐会话授予与撤销；文件/网络只能经宿主的**受控 API**；内存/时间/资源预算；插件 API 版本 + 官方与第三方插件的**权限分级**政策）与威胁评审；验证方式：设计文档存在且 `scripting-permissions` 的对应 requirement（含 Permission denial、Controlled file and network API、Resource budget、Plugin API version and provenance policy）各有测试计划。
- [ ] 6.2 用现有导出/导入契约作为同步缝，先定**所有权 / authn / authz / 冲突规则 / 数据版本**再谈传输；验证方式：导出→导入 round-trip 测试通过，且冲突与鉴权用例有定义好的期望结果。**不引入后端框架，也不声称已有 endpoint**。
- [ ] 6.3 保持静态部署充分：任何后端能力上线后，无后端路径仍必须完整可用；验证方式：断网/无后端环境下启动、运行、存档、设置全通过。

## 7. 跨阶段验证（集成检查）

- [ ] 7.1 每个 phase 合并前在干净副本上跑一次验收命令并留存台账（命令、cwd、版本、退出码、结论分类）；验证方式：台账文件存在且包含 Node/pnpm 版本与 CI pin 版本（`'22'`）的差异标注。
- [ ] 7.2 每次改动后复核 `npm run check:sources` 的再分发警告与 `check:layout` 的违规计数；验证方式：两者退出码与输出被记录，且警告不被当作许可结论。
- [ ] 7.3 保持"源码证据 / 构建成功 / 构建产物 / 真实游戏可运行 / 再分发授权"五结论分离；验证方式：任何阶段报告中不得把前四项之一表述为后一项。

## Workflow follow-up

- 本变更的 7 个 capability delta 在 `openspec/specs/` 落地前只存在于 `openspec/changes/`；archive 需要在该变更被接受后单独执行。
- `docs/current-work.md` 的 checkpoint 更新由 supervisor 在 review 之后单独授权，本变更不包含该编辑。
- 第一轮审计的未知项（远程发布/保护状态、真实引擎可用性、许可再分发权）在获得实际证据前保持未知。
