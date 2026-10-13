# 测试与兼容性（Testing）

> 审计戳：快照 HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`，branch `main`（**当前工作树 12 modified**，多出 `.gitignore` 外部改动，作者未知 —— 见 `../architecture/current-state.md` §1.2）。**本轮没有新增任何测试**。本文件记录的是**已有的**测试面、本轮的实际运行结果（含原始失败片段与日志相对路径），以及**未被覆盖**的东西。
>
> **Phase 0 准确性**：审计**文档交付**完成，且**在隔离副本上执行过** `tsc -b`、两个资产拷贝脚本、`astro build`、`node --test test/*.test.mjs`（见 §3）；但真游戏/浏览器/原生探测/发布重现/fresh install **未跑**。结论是 **第一轮文档已交付 + 验证覆盖未完成**，**不是**全仓验收完成。
>
> 原始证据留在 `.test/audit-round1-ce058ba0/`（`snapshot/`、`sandbox/`、`logs/`）。**这是本机、不可移植的证据**；它不是 CI 产物，也不是发布工件。

---

## 1. 测试面清单（**数量 ≠ 覆盖率**）

| 层 | 文件数 | 位置 | 能否在纯 Node 下跑 |
| --- | ---: | --- | --- |
| tracked，node-only 套件 | **29** | `test/*.test.mjs` | 是（但见 §3 的环境阻塞） |
| tracked，原生探测 | **6** | `ports/artemis/compat-web/tests/*.test.mjs` | 否：需要 `g++`、`zlib.h`、`tar`、`nm`、`--wrap` 链接与 `.ref/artemis-compat` 检出于 pin `17200e64…` |
| tracked，fixtures | 4 | `gl2.h`、`glstub.h`、`probe.cpp`、`tween_probe.cpp` | — |
| untracked，fixture | 1 | `ports/artemis/compat-web/tests/fixtures/animation-observe/path-string.h` | — |
| **tracked 测试文件合计** | **35** | 29 + 6 | |
| gitignored 工作台 | **26** 个顶层 `*.test.ts` + 252 个顶层子目录 | `.test/` | 否：需要 `.ref/` 与真实游戏数据（约 12 GB 级） |

**必须区分**：`35` / `29` / `6` / `26` 都是**工具/文件计数**，**不是**被验证行为的度量。没有任何一个数字可以当覆盖率读。

### 1.1 node-only 套件的 29 个文件名（索引，非覆盖声明）

`artemis-assets`、`artemis-audio`、`artemis-boot`、`artemis-browser-services`、`artemis-builtin-control-flow`、`artemis-builtin-resume-gate`、`artemis-compat-app-assets`、`artemis-compat-audio-integrated`、`artemis-compat-audio-relay`、`artemis-compat-ci`、`artemis-compat-compact-audio-restore-check`、`artemis-compat-frame-platform`、`artemis-compat-read-host`、`artemis-compat-runtime`、`artemis-compat-save`、`artemis-compat-worker-audio`、`artemis-compat-worker`、`artemis-download`、`artemis-eh-abi`、`artemis-fd-idb`、`artemis-gl-lifecycle`、`artemis-hide-fidelity`、`artemis-import-map`、`artemis-module-loading`、`artemis-public-integration`、`artemis-session-core`、`artemis-suspend`、`build-packages-release-policy`、`check-sources`。

### 1.2 原生探测的 6 个文件名与各自的结局口径

`animation-observe`、`audio-native`、`first-transition`、`pf8-readat-bounds`、`source-offer`、`tween`。

**三种结局必须分开写**（来源：源码行号，本轮未运行）：

| 文件 | 缺工具时的行为 | 行号 |
| --- | --- | --- |
| `artemis-compat-first-transition.test.mjs` | **硬失败** | `:470,472` |
| `artemis-compat-tween.test.mjs` | **硬失败** | `:234,236` |
| `artemis-compat-animation-observe.test.mjs` | **硬失败**（注释："a missing compiler is not a skip"） | `:72,74` |
| `artemis-compat-audio-native.test.mjs` | `t.skip(...)`（注释："Two independent reasons to skip, reported as themselves. Neither is a pass"） | `:601,605,651` |
| `artemis-compat-pf8-readat-bounds.test.mjs` | `t.skip(...)` | `:122,310` |
| `artemis-compat-source-offer.test.mjs` | `console.log('SKIP …')` + `return`（**部分**控制台跳过） | `:36,73` |

**规则**：3 个硬失败 / 2 个跳过 / 1 个部分跳过 **不能**被概括为"6 个失败"或"6 个跳过"。本轮这 6 个**全部 NOT RUN** —— 缺工具**不等于**自动失败。

---

## 2. 按构造分类（观察，不是推断覆盖率）

- **unit / contract**（纯进程内）：`artemis-builtin-control-flow`、`artemis-hide-fidelity`、`artemis-compat-worker-audio`、`build-packages-release-policy`、`artemis-compat-app-assets`。
- **integration / process**（spawn `process.execPath` 对编译后的 `dist`）：`artemis-download`、`artemis-compat-save`、`artemis-compat-runtime`、`artemis-public-integration`。
- **workflow / contract**（解析 `.github/workflows/*`）：`artemis-compat-ci`。
- **native compile / WASM-adjacent**：`ports/artemis/compat-web/tests/*`（g++/cmake/zlib/tar/nm）。
- **real-game**：`test/real-game.test.ts`（在 gitignored 工作台里）。
- **security / offer**：`artemis-compat-source-offer`、`check-sources`。

---

## 3. 命令台账（本轮实际结果）

**证据根**：`D:/online_gal/.test/audit-round1-ce058ba0/`（本机、不可移植）。**不是** CI 产物。

| 命令 | cwd | 版本 | 退出码 | 结论 | 日志 |
| --- | --- | --- | --- | --- | --- |
| `node scripts/check-layout.mjs` | `D:/online_gal` | Node v26.8.2 | **1** | **FAIL**（4 处，checker 坐标） | `logs/check-layout.err`（`.out` 为空） |
| `node scripts/check-sources.mjs` | `D:/online_gal` | Node v26.8.2 | **0** | **PASS（快照时点的工作树）**，附再分发警告；**不是** current-HEAD 事实 | `logs/check-sources.{out,err}` |
| lockfile 一致性（自定义只读脚本） | sandbox | Node v26.8.2 | 0 | **PASS**（`lockfile consistent with all workspace package.json manifests`） | `logs/lockfile-consistency.out` |
| `tsc -b tsconfig.packages.json` | sandbox repo | tsc 5.9.3 | **0** | **PASS**（real 6.761 s） | `logs/03-tsc-build.out` |
| `node packages/runtime-ons/scripts/copy-runtime-assets.mjs` | sandbox repo | Node v26.8.2 | **0** | **PASS**（`copied 4 harness file(s) into dist/`） | `logs/04-copy-runtime-assets.out` |
| `node packages/runtime-artemis/scripts/copy-compat-worker.mjs` | sandbox repo | Node v26.8.2 | **0** | **PASS** | `logs/05-copy-compat-worker.out` |
| `node node_modules/astro/astro.js build` | sandbox `apps/app` | Astro 5.18.2 | **0** | **PASS** | `logs/06-astro-build.out` |
| `node --test test/*.test.mjs` | sandbox repo | Node v26.8.2 | **1** | **FAIL**（350/364） | `logs/08-portable-test-suite.out` |
| preservation re-check | 原树 | Node v26.8.2 | **0** | **PASS** | `logs/09-verify-preserved.out` |
| `openspec` 只读探测（context/list/schemas/doctor/status/validate/templates + help） | 原树 | OpenSpec 1.14.1 | 0 | **PASS**（未创建任何文件） | `logs/openspec_*.out/.json` |
| `npm test` | — | — | — | **NOT RUN** | 无日志（需 `.test/` + `.ref/`） |
| `npm run check` | — | — | — | **NOT RUN**（第一步 `check:layout` 在 HEAD 就失败） | 无日志 |
| `pnpm --filter @online-gal/app check`（`astro check`） | — | — | — | **NOT RUN**（从未运行过） | 无日志 |
| `npm run test:portable`（整体） | — | — | — | **NOT RUN** | 无日志 |
| fresh install / `pnpm install --frozen-lockfile` | — | — | — | **NOT TESTED** | 无日志 |
| 6 个原生探测 | — | — | — | **NOT RUN** | 无日志 |
| 26 个工作台 `*.test.ts` | — | — | — | **NOT RUN** | 无日志 |
| 任何 WASM 构建 / 发布重现 / 浏览器 / 真游戏 | — | — | — | **NOT RUN** | 无日志 |

**Astro 计时口径（勘误）**：`logs/06-astro-build.out` 里 `✓ built in 1.86s`（`:39`）是 **Vite 客户端打包**那一趟；Astro 自己是 `[build] 4 page(s) built in 7.15s`（`:53`）；`real 0m32.358s`（`:56`）是整条命令的 wall。**引用时三者都要带上限定词**。

**隔离副本的构建分支（重要）**：隔离副本**故意**没有 `dist/artemis-compat`，所以 `astro build` 只走了 **withdrawn 分支**（`apps/app/scripts/artemis-assets.mjs:187-192` 的 `logger.warn('Artemis withdrawn: …')`）。**真实工作树里有** `D:/online_gal/dist/artemis-compat/`（`artemis-compat.wasm`、`.js`、`-source.tar.gz(.sha256)`、`LICENSE`、`THIRD_PARTY_NOTICES.md`）。因此这次 PASS **不**证明资产 emit 分支可用，也**不**等于 production 等价验证。

### 3.1 `node --test test/*.test.mjs` 的原始结果

官方 summary（`logs/08-portable-test-suite.out:631-638`）：

```
tests 364
pass 350
fail 14
duration_ms 152449
```

0 cancelled / 0 skipped / 0 todo。**13 个文件失败、16 通过**。

原始失败（样例，逐字）：

```
Error [ERR_MODULE_NOT_FOUND]: Cannot find package '@online-gal/engine-api' imported from
C:\Users\ASUS\AppData\Local\Temp\<test>-<rnd>\out\resolve-probe.mjs
```

```
AssertionError [ERR_ASSERTION]: Expected values to be strictly deep-equal
  at test/artemis-compat-ci.test.mjs:339
  at test/artemis-compat-ci.test.mjs:587
```

（上段是**日志原文**，未改写；那两行是 `test()` 起始行。真正比较 patch 名的 `assert.deepEqual` 在 `:357` 与 `:601`。）

**分组**：

| 组 | 文件数 | 失败原因 | 结论 |
| --- | ---: | --- | --- |
| 环境阻塞 | **12** | `artemis-assets`、`artemis-boot`、`artemis-compat-audio-integrated`、`artemis-compat-compact-audio-restore-check`、`artemis-compat-frame-platform`、`artemis-compat-runtime`、`artemis-compat-save`、`artemis-compat-worker`、`artemis-fd-idb`、`artemis-module-loading`、`artemis-public-integration`、`artemis-session-core` —— 这些测试把包编译到临时目录，再 `symlinkSync(<repo>/packages/<pkg>/node_modules, ..., 'junction')` 后 import `@online-gal/*`；隔离副本里 `@online-gal/*` 是**相对** symlink（`sandbox/02-link-node-modules.py:64` 用 `os.path.relpath`） | **BLOCKED — 归因暂定，未排除项目问题** |
| 真实失败 | **1** | `artemis-compat-ci.test.mjs`，2 个断言（`assert.deepEqual` 在 `:357`、`:601`） | **REAL — 在 HEAD 上也失败**（见 §4） |

**归因纪律（必须保留）**：12 个文件的"隔离副本造成"这一结论**只有机制上的合理性**。绝对链接探测与 `g++`/`zlib.h` 探测的 stdout **没有**留在 `logs/` 里（只留在散文里）。因此：**不得**写成"已证明是 sandbox-only"；准确表述是"归因暂定：由隔离副本的相对链接构造推出，缺少可复核的原始输出"。

**写入范围纪律**：`node --test` 还会在**系统临时目录**写文件（`C:\Users\ASUS\AppData\Local\Temp\<test>-<rnd>\out\...`）。因此**不能**声称"唯一写入域是 sandbox"；准确表述是"没有写到仓库内 sandbox 之外的地方"。

---

## 4. 在 HEAD 上真实失败的两件事

### 4.1 `check:layout`（P1，可复现，无环境依赖）

`node scripts/check-layout.mjs` exit 1，**4 处**。下面引用保留自 `.test/audit-round1-ce058ba0/logs/check-layout.err`（**不改写日志**）；它们是 **checker 坐标**（`check-layout.mjs` 先做 `stripComments`，多行块注释折成一个空格，所以行号与原始 `grep` 不同）：

```
ports/artemis/compat-web/tests/artemis-compat-first-transition.test.mjs:14: const WORK = join(ROOT, '.test/artemis-first-transition-fix');
ports/artemis/compat-web/tests/artemis-compat-tween.test.mjs:232: mkdirSync(join(ROOT, '.test/artemis-tween-loopdelay-fix'), { recursive: true });
ports/artemis/compat-web/tests/artemis-compat-tween.test.mjs:233: const work = assertExtractDir(ROOT, join(ROOT, '.test/artemis-tween-loopdelay-fix/managed'));
ports/artemis/compat-web/tests/artemis-compat-tween.test.mjs:261: writeFileSync(join(ROOT, '.test/artemis-tween-loopdelay-fix/managed.log'),
```

**同一批违规的原始源码坐标（另记，不是 checker 坐标）**：`git --no-optional-locks show HEAD:<file> | grep -n '\.test/'` 给出 `artemis-compat-first-transition.test.mjs:31`、`artemis-compat-tween.test.mjs:238`、`:239`、`:267`。两者**不是矛盾**：checker 在 `stripComments` 后计数。复现 `check-layout.mjs` 的 `stripComments` 语义可精确重现 `14 / 232 / 233 / 261`（本轮 09 最终评审独立复现）。

这两个文件**不在** 11 个基线 modified 之列 → 内容 == HEAD（由 porcelain 推出；原始 `git diff --stat` 未留存）。
**后果**：`npm run check` / `npm test` 的**第一步**就无法通过。

### 4.2 `test/artemis-compat-ci.test.mjs`（P1，patch 清单契约漂移）

- 断言实际值：工作树有 **6** 个 patch 名（`0001…0006`）。
- 测试字面量期望：**4**；错误堆栈里的**测试行**是 `:357` 与 `:601`（两个 `assert.deepEqual(names, [...4 names])`）。早前把 `:339`/`:587` 当成断言行是错的（那是 `test()` 起始行）。
- HEAD 的清单：`git --no-optional-locks show HEAD:ports/artemis/compat-web/PLATFORM-PATCHES.txt` 列出 **5** 项（`0001…0005`）—— **由写作者与后续 scout 两次独立只读查证，不是推断**。
- 同一个 4 字面量也被 `ports/artemis/compat-web/tests/artemis-compat-animation-observe.test.mjs:91` 断言（`assert.equal(names.length, 4)`）。
- **三个数字分开写**：HEAD = **5**、工作树 = **6**、测试字面量 = **4**。**不得**用工作树的 6 冒充 HEAD 内容。

---

## 5. 什么证据能证明什么（分层）

| 结论 | 由什么证明 | 本轮状态 |
| --- | --- | --- |
| 源码里存在某条链 | 静态阅读（`path:line`） | 有（部分链） |
| 构建能过 | `tsc -b` / `astro build` exit 0 | 沙箱内 PASS（但只走了 withdrawn 分支） |
| 产物被正确生成 | 打包/emit 步骤 + 校验步骤 | **未验证**（emit 分支未跑） |
| 引擎在浏览器中能运行 | **真实浏览器 / 真游戏** | **未验证** |
| 再分发授权 | 书面许可 | **未解决** |

**五个结论互不代替**（见 `../governance/repository-risk.md` 与 `workflow.md` §7）。

### 5.1 真游戏 oracle 纪律

- **真实游戏测试是唯一 oracle**，且属**用户**亲做。
- **单次运行不算判据**：丢弃一次热身 + 两次一致。
- 用户上报的 clock-ms-02「还是不行」**无步骤、无测量、无重复次数** → 记为**未解决信号**，**不**推断根因。
- 官方 WebGL 产物 `preserveDrawingBuffer=false` → 截图可能 blank；要观测绘制必须用浏览器工具**在真实运行时**截图（静态阅读不行）。

---

## 6. 未覆盖 / 未运行的测面向（**必须保留**）

| 面向 | 为什么未覆盖 | 需要什么前置 |
| --- | --- | --- |
| 发布产物生成（release/publish 路径） | 未授权 + 需要人工验收 | 显式派发 + 真游戏验收 |
| C++ 原生探测（6 个） | 本机缺 `zlib.h`（`g++ 15.2.0` 在，但 `#include <zlib.h>` 报 `fatal error: zlib.h: No such file or directory`）；隔离副本不含 `.ref/` | `g++`、`libz-dev`、`tar`、`nm`、`.ref/artemis-compat` 检出于 pin |
| IndexedDB 配额 | 未读实现，未运行 | 浏览器 + 配额填充 |
| 跨标签写冲突 | 未运行（`db.ts` 有 versionchange 处理，`IndexedDbStore.open` **没有**同等处理） | 多标签浏览器环境 |
| archive 峰值内存 | 只有源码约束（整成员缓冲），**未测量** | 构造大成员 + 测量工具 |
| 安全 / 权限（`ensurePermission` 持久化） | 未运行 | 浏览器 + File System Access |
| 插件机制 | **未实现**（tracked 无相关代码） | 尚无需求 |
| QuickJS 脚本沙箱 | **未实现**（tracked `git grep -i quickjs` = 0 命中） | 尚无实现；只有 `scripting-permissions` 的边界条款 |
| 云同步后端 | **未实现**（tracked `git grep -i cloud.backend\|cloudBackend` = 0 命中） | 尚无实现 |
| 全仓 import AST / 动态 import | 本轮只有 comment-stripped 正则 | 完整 TS AST 或运行时追踪 |
| `.env` / secret 值审计 | 只做了**键名**扫描（`process.env.*`） | **不是** secret 审计；需要单独工作 |
| 本机 Node 26 与 CI Node 22 的差异 | 未在 CI 环境复现 | 在 pin 环境跑一遍 |

**注意**："tracked grep 0 命中"**不等于**"需求没有规划"。用户的未来目标包含脚本执行与云同步；这里记录的是**代码里没有实现**，不是"没人想做"。

**“未实现 → 没有测试目标”清单（去掉歧义）**：上表中标为**未实现**的面向（插件机制、QuickJS 脚本沙箱、云同步后端）**当前连测试目标都没有**：tracked 代码里无实现，也无对应测试文件。它们在本轮只作为 `scripting-permissions` 的**边界条款**被登记，**不**当作待实现工作。发布产物生成、C++ 原生探测、IDB 配额、跨标签、archive 峰值、权限持久化则是"**有代码但未验证**"，两类不要混同。

---

## 7. 运行测试时的环境陷阱（已观察）

1. **工作台路径**：`.test/` 是 gitignored 的本地工作台；把它写进 tracked 文件会在 `check:layout` 上失败（见 §4.1）。
2. **临时目录**：node 测试会写 `%TEMP%`；报告写入范围时要说清"仓库内 vs 仓库外"。
3. **symlink 链接方式**：测试用 `symlinkSync(..., 'junction')` 造 `node_modules` 链接；**相对**链接在某些临时目录布局下无法解析 `@online-gal/*`。这是本机/隔离环境的坑，**不是**仓库缺陷——但必须有原始输出才能这么断言。
4. **`npm test` 的假绿防护**：`.test/` 缺失时 `scripts/run-tests.mjs` exit 1 并明确报 "**not a pass — nothing was executed**"。这是好的设计，应保留。
5. **原生探测的硬失败设计**：`first-transition`/`tween`/`animation-observe` 在缺编译器时**硬失败**而不是跳过 —— 这是有意的；不要为了"绿灯"把它们改成跳过。
6. **CI 与本地不同**：CI pin Node 22 并在 `build-packages.yml:176-189` 装 `g++ libz-dev` 并 shallow-clone pin；本地缺这些 → 本地 NOT RUN 不等于 CI 会失败。

---

## 8. 本轮没有做的事

- **没有新增任何测试**（本轮只做文档修正，不动测试文件）。
- 没有运行 `npm test` / `npm run check` / `astro check` / `test:portable` / 任何原生探测 / 任何 WASM 构建 / 任何浏览器或真游戏。（已执行的沙箱 `tsc -b` / 资产拷贝 / `astro build` / `node --test test/*.test.mjs` 见 §3；本轮**修正轮**没有重跑它们。）
- 没有安装或更新依赖。
- 没有把"静态 PASS"写成"引擎可用"。
