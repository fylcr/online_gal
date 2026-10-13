# 仓库风险台账（Repository Risk）

> 审计戳：HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`，branch `main`。本文件的每一条都给出**触发条件**（什么情况下会真的发生）与**当前证据强度**。
>
> **不是**法律意见；**不含**任何规避平台策略或执法的建议。目标恰恰相反：让来源、许可与责任人**可被如实追溯**。
>
> 证据等级：**已观察** / **已确认约束** / **推断** / **未验证假设** / **未知**。带"未知"的项**不得**被读成"不存在"。

---

## 0. 风险登记摘要

| ID | 风险 | 等级 | 证据强度 | 需要人工决定 |
| --- | --- | --- | --- | --- |
| R-1 | 外部 Action 可变 tag + 写 token | **高** | 已观察 | 否（技术上） |
| R-2 | `push` 直接发布引擎 release，绕过验收口径 | **高** | 已观察 | **是** |
| R-3 | Artemis pin 记录冲突 → 假 provenance | **高** | 已观察 | **是** |
| R-4 | 未 pin 的 `puppeteer` 装进写 token 作业 | 中 | 已观察 | 否（技术）/ 是（是否接受） |
| R-5 | 专有 SDK 字节的再分发权未解决 | 中 | 已观察 | **是（发布前强制）** |
| R-6 | `check:layout` 在 HEAD 上失败，门禁被绕过或长期红 | 中 | 已观察 | **是** |
| R-7 | 存档/覆层删除不回收字节（source-level） | 中 | 已观察（未复现） | **是** |
| R-8 | 无 CODEOWNERS / 无依赖更新通道 / 无安全报告通道 | 中 | 部分（ENOENT） | **是** |
| R-9 | 远程保护、备份、发布资产状态**未知** | — | 未知 | **是（需授权查询）** |
| R-10 | 本地标签 vs 远程 release 的一致性**未知** | — | 未知 | 否 |
| R-11 | 上游检出来源与 source offer 的完整性 | 中 | 部分 | **是** |
| R-12 | 基线后出现未知作者的工作树改动（`.gitignore` + 100 B 的 `MEMD.md`） | 低（文档层面）/ 中（可追溯性） | 已观察 | **是（需 owner 认领）** |

---

## 1. 供应链：外部 Action 与写 token（R-1）

- **证据**：7 个工作流中的全部外部 `uses:` 都是可变 tag（`@v4` / `@v11`），**无** 40-hex SHA；`build-wasm-reusable.yml:167-168` 与其余构建/发布工作流为 workflow 级 `contents: write`。
- **触发条件**：任何命中 `paths` 过滤的 push 都会运行这些 action。一旦某个 tag 被移动、或上游 action 仓库被攻陷，可在持有写 token 的 runner 上执行任意代码。
- **后果链**（推断）：写 token 可创建/修改 release → 可替换 `ports/lib/engine-artifacts.json` 所 pin 的引擎字节 → 消费者按 ledger 校验时下载的是被替换的字节（若攻击者在同一窗口也改了 ledger，则校验会"通过"）。
- **当前未观测到任何利用**。
- **缓解方向**：全部固定到完整 commit SHA（版本号留注释）；建立依赖更新通道（否则 SHA pin 会让安全更新停摆）；该风险**不需要**平台规避手段。

## 2. 发布门禁：`push` 即发布（R-2）

- **证据**：`build-wasm-ons.yml` 与 `build-wasm-krkr.yml` 的 publish 表达式含 `github.event_name == 'push'`；`build-packages.yml` 用 `event == 'workflow_dispatch' && inputs.publish == true` 并写明只有显式派发才发布、"a release is a claim that these bytes were accepted, and the acceptance oracle is a real game booting them — which no runner can do"；compat 工作流硬编码 `publish: false`。
- **触发条件**：合并一个触及 `ports/ons/build/**`、`ports/krkr/kirikiroid2/**`、`packages/runtime-ons/**`、`pnpm-lock.yaml`、`LICENSE` 等的 PR。
- **后果**：`engine-ons-0.7.7beta` / `engine-krkr-20260920` 的资产可在**无人工验收**下被创建或覆盖（`gh release upload --clobber`）；ledger 的 sha256 是**手工**更新，陈旧条目的失配会静默退化为"源码构建"。
- **当前影响面**：仓库是 private，因此这是**消费者完整性与分发**暴露，不是公开泄露。
- **需要人工决定**：这是否有意（ONS/krkr 是唯二有真游戏证据的引擎）。

## 3. Pin 与 provenance 冲突（R-3）

- **证据**：
  - `.github/upstream-pins.json` → `artemis-compat.commit = 17200e64ea99681e3c27d694433c0ff78d7a7cee`，其 `$comment` 称 bridge 与 compat 是"SAME commit"。
  - `build-wasm-artemis-bridge.yml:78` → `upstream-commit: ${{ inputs.upstream_commit || '9f622629f595db85e6f76c45c06edde29d7d9961' }}`（**不同的** commit），且 `skip-upstream-checkout: true`。
  - `THIRD_PARTY_NOTICES.md:135` → 把 **compat** 的上游 pin 写成 `9f622629…`。
- **触发条件**：任何一次 bridge 派发。
- **后果**：产物、`version`、release tag 与 manifest 都指名一个仓库另有记录的 commit；`THIRD_PARTY_NOTICES.md` 给出的"compat pin 出处"是错的，会削弱接收方依赖的 source-offer 声明。**这正是 compat 工作流自己注释禁止的"false provenance"失败模式。**
- **需要人工决定**：哪一个 commit 对应既有的 transpile 证据。
- **禁止**：改写 `.github/upstream-pins.json` 的 `expected` md5/字节数（冻结记录）。

## 4. 未 pin 的运行时安装（R-4）

- **证据**：`build-wasm-artemis-bridge.yml` 的 `real-game` 作业执行 `npm install --no-save puppeteer`（无版本、无 lockfile、无 `--frozen-lockfile`），并继承 workflow 级 `contents: write`。
- **触发条件**：手动派发且 `game_tree` 非空。
- **后果**（推断）：来自 registry 的任意未固定代码在持有写 token 的 runner 上执行，且该作业同时处理专有 SDK 派生的模块。
- **缓解方向**：固定版本或移入受评审的 devDependency；该作业最小权限。**是否接受为已知风险**属 owner 决定（该作业手动且诊断用）。

## 5. 专有 SDK 与许可边界（R-5）

- **证据**：`ports/artemis/vendor/artemis-sdk/Copyright.txt:1-2` "All rights reserved"；`THIRD_PARTY_NOTICES.md` §"Artemis Engine SDK" 三处声明"没有可核实的再分发授权"、"根 GPL 声明不能提供该缺失许可"、"保留这些字节不等于批准发布"；`.gitignore:70-76` 的注释 "this exception is NOT a redistribution grant"；`build-wasm-artemis-bridge.yml` **可以**在 `vars.ARTEMIS_PUBLISH_ARTIFACTS == 'true'` 时发布该 SDK 派生模块（变量值**未知**）。
- **触发条件**：任何发布 SDK 派生字节的动作。
- **后果**：再分发状态**未解决**；这是**人工复核项**，不是自动缺陷。
- **重要**：删除 vendor 目录**不**解决已派生输出的来源问题，因此"直接删"**不是**缓解。
- **需要人工决定**：**发布前强制**。

## 6. 门禁在 HEAD 上就失败（R-6）

- **证据**：`node scripts/check-layout.mjs` exit 1，4 处违规（两个 tracked 测试文件，其中一个文件 3 处）；`npm run check` 第一步即该命令。
- **触发条件**：任何人在当前 HEAD 上跑 `npm run check` 或 `npm test`。
- **后果**：门禁长期为红 → 要么被绕过、要么没人再跑；同时 `check:sources` 的再分发警告也会与失败混淆，削弱其信号。
- **缓解方向**：phase 1（见 `../architecture/migration-plan.md`）只修测试与清单，不动产品。

## 7. 存档与覆层删除不回收（R-7）

- **证据**：`packages/core/src/library.ts:119-120`（`list(id)` 已带 id 前缀却再拼 `${id}/`）、`:261-265`、`:369-370` 写入 `saves:<id>\0userfs/...` 而 `:122-127` 只删 `` `${id}\u0000`` 前缀；对照正确的 `:282-283`。
- **触发条件**：从 UI 删除一个游戏（`apps/app/src/components/Library.svelte:136` 承诺"其存档也会一并移除"），或改名/移动。
- **后果**：已导入的文件索引与引擎可写覆层字节残留。**未复现**，因此**不声称已发生数据损失**。
- **需要人工决定**：是否接受键形状变化（牵涉数据迁移）。

## 8. 治理面缺失（R-8）

- **证据**：`.github/CODEOWNERS`、`.github/dependabot.yml`、`SECURITY.md`、`CONTRIBUTING.md`、`.github/PULL_REQUEST_TEMPLATE.md` 探测为 ENOENT。
- **限制**：ENOENT **只**证明那些**路径**不存在，**不**证明没有同义别名文件。
- **后果**：许可关键路径（`ports/artemis/vendor/**`、`ports/artemis/compat-web/**`、`packages/runtime-ons/assets/**`）无声明 owner；无自动依赖更新通道（与 R-1 的 SHA pin 直接相关）；无安全报告通道；无 PR 检查清单来承载"不要上传 `.ref`/`.test`、证据纪律"这些目前只写在 `AGENTS.md` 散文里的规则。
- **需要人工决定**：CODEOWNERS 需要真实用户名/团队；安全报告是否走私有渠道。

## 9. 备份与恢复（R-9）

**当前状态：未知。** 本轮**未查询**任何远程侧信息。

**必须区分的两类内容**：

| 类别 | 例子 | 能否从仓库重建 |
| --- | --- | --- |
| 仓库内 | 源码、脚本、workflow、配方、patch、overlay、`ports/lib/engine-artifacts.json` | **能**（clone 即可） |
| 只存在于单机 | 被 gitignore 的构建产物（`dist/`、`ports/**/out/`）、`.ref/` 上游检出（约 12 GB 级）、`.test/` 工作台与真实游戏数据、本地下载的工具链 | **不能**（需按配方重新获取/重建） |
| 发布产物 | release 资产（若存在） | **未知**（未查询） |

**"未查询"不等于"没有备份"。** 本文件**不**声称备份不存在，也不**不**声称备份存在。

**建议的合法防护（不涉及规避任何策略或执法）**：

- 在授权的托管平台上启用保护与备份，并按平台提供的正规方式导出重要内容；
- 为"只存在于单机"的类别写下**重建命令**（本文件与 `migration-plan.md` 已把配方路径列出）；
- 记录恢复演练的**实际结果**，而不是把"理论上能重建"当作"已验证可恢复"。

## 10. 本地标签 vs 远程 release（R-10）

- **已观察**：本地存在 `engine-krkr-20260920`、`engine-ons-0.7.7beta`、`engine-packages-0.1.0`…（0.1.0、0.1.1、0.1.3–0.1.10 中的若干）。**没有**任何本地 `engine-artemis-*` 标签。
- **未知**：`origin` 上是否存在同名的 release 资产，其内容是否与本地标签一致。
- **触发条件**：消费者按 ledger 的 URL 下载。
- **后果**：若远程资产缺失或与 ledger 不符，`fetch-engine.mjs` 会以 exit 2（不可用）或 exit 3（校验失败）终止 —— 这是**设计上正确**的行为，但意味着"台账里写了"不等于"下载得到"。
- **本条不是缺陷**；是**未采集**的事实。

## 11. 上游检出与 source offer 的完整性（R-11）

- **已观察**：CI 在 `build-packages.yml:176-189` 会 clone `.ref/artemis-compat` 并安装 `g++ libz-dev`（供 `test:portable` 的原生探测使用）；**游戏 fixture 不被下载**。
- **因此**：不能写"没有工作流检出 `.ref`" —— 那是错的。
- **source offer**：compat 有自校验的 `-source.tar.gz(.sha256)`；bridge 由 workflow 产出 `manifest` 与 artifact，但**没有**源码 offer 的自校验链。
- **触发条件**：外部接收方依赖 source offer 重建产物。
- **后果**：若 offer 与产物不同源（见 R-3），重建得到的不是被发布的字节。
- **缓解方向**：保持 `build-artemis-compat.sh` 的 `UPSTREAM_COMMIT == ARTC_PIN` 硬失败断言；为桥接路线单独裁定 provenance。

---

## 11.1 保护异常：基线后出现的未知作者改动（R-12）

- **证据**：基线快照（`.test/audit-round1-ce058ba0/snapshot/`）记录的 `git status` 是 **11 个 ` M` + 65 untracked**。当前工作树是 **12 个 ` M` + 85 untracked（`-uall`）**。多出的是：
  - `.gitignore`：快照 `c1168c427761fc950de9d225f829a34128c011ee23e3272e189e720273b504c4`（4265 B，等于 `HEAD:.gitignore`）→ 当前 `9434290335af8e88c71f34e65d88bac5b4d1989b67735e56d7166ef808a95c4f`（4273 B）；`git diff -- .gitignore` 只新增一行 `MEMD.md`。
  - 根 `MEMD.md`（100 B，untracked，被上述规则隐藏）。
- **触发条件**：无特定触发；它是**已经发生**的落盘事实。
- **后果**：① 若今后的验收脚本硬编码“11 个 modified”，会误报失败；② `MEMD.md` 一旦被 ignore，它不会被 untracked 清单看到，可能成为隐形状态。
- **归因与处置**：作者**未知**；**不**归因于本轮文档写作者；**不**改写、**不**还原、**不**删除（那是别人的工作）。只**披露**。
- **需要人工决定**：由 owner 认领这两项。
- **不做的**：不建议任何规避平台策略或隐藏痕迹的做法；此处只需要**如实记录**。

---

## 12. 明确不做的建议

- **不**提供任何规避平台策略、托管服务条款或执法的做法。
- **不**建议把许可风险"通过改名或附 GPL"来消除：文档已明确这不算解决。
- **不**把"删除字节"当作许可或 provenance 的缓解。
- **不**在未查询的情况下声称远程保护/备份/发布资产不存在。

---

## 12.1 关于 R-12 的额外说明

R-12 不是缺陷，而是**可追溯性缺口**：一个不在基线内的 ignore 规则 + 一个被它隐藏的 100 B 根文件。它的危害是让未来的“工作树未变”验收变成假阳性。处理方式是**提升可见性**（owner 认领、验收脚本改成“初始集合 + 披露变更”），**不是**删除或掩盖。

---

## 13. 关闭这些风险各需要什么

| ID | 关闭条件 |
| --- | --- |
| R-1 | `uses:` 全部 40-hex SHA + 依赖更新通道（Dependabot 或定期人工） |
| R-2 | 三个工作流的发布判据一致；owner 确认意图；一次 push 只上传工件 |
| R-3 | 三个记录一致，且裁定既有证据对应的 commit；**不改冻结哈希** |
| R-4 | `puppeteer` 固定版本或移入受评审依赖；该作业最小权限 |
| R-5 | 书面再分发许可，或明确的人工保留决定 |
| R-6 | phase 1 落地且 `check:layout` 在 HEAD 上 exit 0 |
| R-7 | phase 2 的键空间测试 + 修复 + 迁移方案 |
| R-8 | 新增 CODEOWNERS/dependabot/SECURITY，并记录远程保护状态 |
| R-9 | 授权后查询远程；为"只存在于单机"的类别写出并**演练**重建命令 |
| R-10 | 授权后比对本地标签与远程 release 资产 |
| R-11 | bridge 路线的 provenance 裁定；source offer 覆盖全部已发布产物 |
| R-12 | owner 认领 `.gitignore` 改动与 `MEMD.md`；验收脚本改成“初始 dirty 集合 + 披露的外部改动”而非硬编码 11 |
