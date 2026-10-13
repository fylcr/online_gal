# 开发工作流（Development Workflow）

> 审计戳：HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`，branch `main`。本文件记录**实际存在的**命令、它们的真实副作用，以及本轮之后要遵守的纪律。凡"拟议"的规则都标注出来。
>
> 命令台账与测试分层见 `testing.md`；阶段授权见 `../architecture/migration-plan.md`。

---

## 1. 环境

| 项 | 实际值 | 说明 |
| --- | --- | --- |
| shell | Git Bash（本机 `pwsh` 被接到 bash） | 脚本用 bash 语法 |
| Node | **本机 `v26.8.2`**；`package.json` `engines.node >= 20`；**CI pin `'22'`** | 本机版本满足下限但**不是** CI 版本 → 本机日志不是 CI 复现 |
| 包管理器 | pnpm **9.14.2** | 与 `package.json` `packageManager` 一致 |
| 工作区 | `pnpm-workspace.yaml`：`packages/*`、`apps/*` | 13 包 + 1 app |
| 依赖安装 | `node_modules/` 存在（**非**工作树保证）；`node_modules/.pnpm` 414 条目 | `node_modules` 被 gitignore，**不等于**未安装；本轮**未**做 fresh install |

**规则（拟议）**：任何"在 CI 上会过"的断言，必须在 CI pin 的环境（Node 22）下验证，或显式标注"本机版本不同、未在 CI 复现"。

## 2. 实际存在的命令（含副作用）

### 2.1 只读 / 可安全频繁运行

| 命令 | 副作用 | 备注 |
| --- | --- | --- |
| `npm run check:layout` | 只读 `git ls-files` + 读文件 | **当前在 HEAD 上 exit 1**（4 处违规，见 §3） |
| `npm run check:sources` | 读工作树、spawn `git`、读文件；不自动暂存、不触发 Actions | 快照时点**工作树** exit 0（`sources ok (worktree): 340 mapped functions, 340 product files`），附再分发复核**警告**。worktree 模式读的是**工作树**补丁表，`--staged` 模式才用 `git show`（`check-sources.mjs:216`、`:599`），因此**不等于** current-HEAD 事实 |
| `npm run test:sources` | 起临时 Git fixture 跑 `test/check-sources.test.mjs`；**不改当前索引** | |
| `git status --porcelain` / `git log` / `git ls-files` / `git show HEAD:<path>` | 只读 | 用 `--no-optional-locks` 可避免写 `.git/index` |
| `openspec list/context/doctor/status/schemas/show/validate` | 只读 | `validate` 不写文件（已验证 untracked 列表不变） |

### 2.2 会写产物（gitignored）

| 命令 | 写入位置 |
| --- | --- |
| `npm run build:packages` | `tsc -b tsconfig.packages.json` → `packages/*/dist/`、`*.tsbuildinfo`；再跑两个资产拷贝脚本（`packages/runtime-ons/scripts/copy-runtime-assets.mjs`、`packages/runtime-artemis/scripts/copy-compat-worker.mjs`） |
| `npm run build` | 上行 + `pnpm --filter @online-gal/app build`（Astro → `apps/app/dist/`、`.astro/`） |
| `npm run check` | `check:layout` + `tsc -b` + `app check`（`astro check`）；**注意第一步当前就失败** |
| `npm run dev` / `preview` | 起 Astro dev/preview（`--host 127.0.0.1 --port 4321`），长驻进程 |

**规则（已存在的事实）**：这些命令只写 gitignored 输出；**本轮全部未运行**。

### 2.3 会跑测试（可能需要工作台）

| 命令 | 行为 |
| --- | --- |
| `npm run test:portable` | `scripts/run-portable-tests.mjs`，`SUITES = ['test', 'ports/artemis/compat-web/tests']`；空套件时非零退出 |
| `npm test` | `check:layout` + `build:packages` + `node scripts/run-tests.mjs`；后者跑 gitignored 的 `.test/*.test.ts`，**`.test/` 缺失时 exit 1 并明确报 "not a pass — nothing was executed"**（防假绿） |

### 2.4 构建 / 下载（**本轮未运行**）

| 命令 | 副作用 |
| --- | --- |
| `bash ports/artemis/compat-web/RUN-BUILD.sh` / `build.sh` | emscripten 构建 compat wasm，写 `out/`；可能需要 emsdk/网络 |
| `bash ports/artemis/reverse/build-bridge-route.sh` | 编译 `bridge.wasm`，写 `ports/**/out/`、`reverse/out/`（`OUTDIR` 需绝对路径） |
| `ports/lib/fetch-engine.mjs` | 下载引擎产物到落点；**先校验大小+sha256 再改名落位**；exit 2 = 不可用、exit 3 = 校验失败（必须停止） |
| 各 `fetch-*.sh` | 下载 wabt/Zig 等工具链到 gitignored 目录 |

### 2.5 OpenSpec 写操作（**本轮未运行**）

`openspec new change` / `archive` / `sync` 会写 `openspec/**`。`openspec init` **会创建** `openspec/` 根 —— 本项目已有 root，**禁止**运行 `init`。
**`openspec/config.yaml` 不得被覆盖或改写。**

---

## 3. 已知的门禁缺口（HEAD 现状）

```
Layout violation: committed code must not depend on `.test/`.
  ports/artemis/compat-web/tests/artemis-compat-first-transition.test.mjs:14
  ports/artemis/compat-web/tests/artemis-compat-tween.test.mjs:232
  ports/artemis/compat-web/tests/artemis-compat-tween.test.mjs:233
  ports/artemis/compat-web/tests/artemis-compat-tween.test.mjs:261
```

- **4 处违规**（**checker 坐标**，即 `check-layout.mjs` 经 `stripComments` 后报的行号）；对应的**原始源码行**是 `:31`、`:238`、`:239`、`:267`。两者指**同一批**违规。
- 这两个文件**不在**基线 11 个 modified 之列，因此它们的工作树内容 == HEAD（由 porcelain 推出）。

> **时点提示**：本文件写“本轮 11 个 modified”是指**快照**；“当前”工作树是 **12 个**（多出 `.gitignore`，外部改动，作者未知）。详见 `../architecture/current-state.md` §1.2。
- 后果：`npm run check` 与 `npm test` 的**第一步**都无法通过。
- 修它属 **phase 1**（需批准）：只动测试与清单，**不动产品源码**。

---

## 4. 安全前置（preflight）与所有权

**规则（拟议，但基于已存在的事实）**：

1. **动之前先冻结比较基准。** 记录 `git rev-parse HEAD`、`git status --porcelain`、tracked/untracked 的内容哈希清单（本轮的做法见 `.test/audit-round1-ce058ba0/snapshot/`）。
2. **先确认没有在途 writer。** 同一时刻只跑一个引擎实例；收尾关掉自己开的窗口、标签页、诊断服务器与后台 job。
3. **明确写入域。** 只写自己声明的路径；不改别人的 dirty 文件。**快照时点**的 11 个 modified 与 65 个 untracked 是**他人在途工作**，不得 stage、不得格式化、不得“顺手改一下”。（当前 12 个 modified：多出的 `.gitignore` 同样是他人的。）
   **保护期望的正确说法**： “初始 dirty 集合被保留 + 外部改动被披露”，**不是**把 11 硬编码；出现不在基线内的改动时**披露而不还原**。
4. **不动冻结项**：`.github/upstream-pins.json` 的 `expected`（md5/字节数）、`docs/fetch-engine-artifacts.md` 的历史 hash、m03520 冻结的鼠标映射缺陷。
5. **解包前断言落点**：任何上游 archive / tar 解包**只允许**落到明确校验过的新建/已 guard 的输出子目录（`.test/<dir>/`、`.ref/<dir>/`、`ports/<port>/out/<dir>/`）。**绝不允许**解到仓库根。断言失败即退出非 0；不要用空变量或仓库相对路径当落点。历史上根目录的 `AGENTS.md`/`README.md`/`LICENSE`/`THIRD_PARTY_NOTICES.md`/`.gitignore` 被上游同名文件覆盖过一次。
6. **不打印 secret、不提交游戏内容。** 引擎资产与游戏数据分别由 ledger 与 `.ref/`/`.test/` 承载；后者 gitignored。

---

## 5. 提交与推送（**本轮禁止**）

**现状**：本轮**没有** stage、commit、push，也不得改写 git 历史。

**规则（拟议，适用于未来被授权的改动）**：

1. **显式 pathspec**：`git commit -F - -- <path> [<path>...]`。不带 pathspec 的 `git commit` 会把别人的 staged 删除扫进来。
2. **只 stage 自己写的东西**。不动那份 11 个 modified 的基线集合、不动别人的 untracked，也不动后续出现的未知作者改动（如 `.gitignore`）。
3. **推送只在被授权后**。push 会触发 `push:[main]` 的工作流 —— 对 ONS/KiriKiri 来说**那会直接发布 release**（见 `../governance/repository-risk.md` R-2）。因此在 push 前必须确认：这次 push 不会触发任何发布路径。
4. **提交后及时 push，并在 GitHub 上观察同一个 commit 的构建**。
5. **`docs/current-work.md` 的 checkpoint 由 supervisor 在 review 之后单独授权**，不属于任何默认动作。

**禁止**：`git add -A`、`git commit -a`、`git checkout -- <别人的文件>`、`git stash`（会动别人的工作）、`git reset --hard`、任何写历史的操作（rebase/filter-branch/amend 已推送的提交）。

---

## 6. 普通 CI 与发布分离

| 工作流 | 触发 | 是否发布 |
| --- | --- | --- |
| `build-packages.yml` | `push:[main]`(+paths)、`pull_request`(+paths)、`workflow_dispatch`（`publish` 默认 **false**） | 仅 `workflow_dispatch && publish == true` |
| `build-wasm-reusable.yml` | 仅 `workflow_call` | 继承调用方的 `publish` |
| `build-wasm-ons.yml` | `push:[main]`(+paths)、`pull_request`(+paths)、`workflow_dispatch`（`publish` 默认 **true**） | **push 即发布** |
| `build-wasm-krkr.yml` | 同上 | **push 即发布** |
| `build-wasm-artemis-compat.yml` | `push`/`pull_request`/`workflow_dispatch` | `publish: false`（硬编码） |
| `build-wasm-artemis-bridge.yml` | **仅** `workflow_dispatch` | 受 `vars.ARTEMIS_PUBLISH_ARTIFACTS == 'true'` 门控 |
| `check-upstream-drift.yml` | `schedule: '17 3 * * *'`、`workflow_dispatch` | 不发布；**不**改写 pin；派发时**不**传 `publish` |

**规则（拟议，即 Phase 4）**：普通 CI 只产工件；发布必须显式人工派发。当前 ONS/KiriKiri 是例外。

**推送前的检查清单（拟议）**：

- [ ] 这次 push 命中的 `paths` 会不会触发 `build-wasm-{ons,krkr}`？（会 → 先确认是否真的要发布）
- [ ] 是否改了 `packages/**`、`pnpm-lock.yaml`、`ports/**`、工作流文件本身？
- [ ] 是否改了任何冻结项？
- [ ] `check:layout` 与 `check:sources` 的状态是否已知（红/绿/未运行）？

---

## 7. Review 与证据

**规则（拟议）**：

1. **五结论分离**：源码证据 / 构建成功 / 构建产物 / 真实游戏可运行 / 再分发授权是**五个不同结论**。任何 review 都不得把它们混为一谈。
2. **单次运行不算判据**（引擎相关）：丢弃一次热身 + 两次一致。
3. **真实游戏测试是唯一 oracle**，且属**用户**亲做。
4. **"目录完整 / 构建成功 / 合成测试通过 / 真实游戏可用 / 再分发授权"** 分别记录，不得互相代替。
5. **不得**把官方胶水诊断、组件单测全绿、全生成掩码桥健康当作引擎可用。
6. **证据留存**：命令、cwd、版本、退出码、结论分类；原始失败的 stdout/stderr 留在可复核的位置（本轮在 `.test/audit-round1-ce058ba0/logs/`）。
7. **归因强度**：没有留存原始输出的归因标为"暂定 / 未排除项目问题"，**不得**写成"已证明"。

---

## 8. 本轮明确未做（供接手者对照）

- 未构建、未跑测试、未装依赖、未下载、未开浏览器或服务、未 stage/commit/push、未改 git 历史、未起子代理。
- 未改 `docs/current-work.md`。
- 未改 `openspec/config.yaml`。
- 未改任何产品代码或 CI。
