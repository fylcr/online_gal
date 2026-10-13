# 目标状态（Target State）

> 本文件描述**希望成立**的架构，**不是**当前状态。当前状态见 `current-state.md`。任何一条目标在被实现并通过验收前，**不得**在别处被引用为已实现。目标状态里不含"某个引擎现在能用"的断言。
>
> 迁移阶段与授权见 `migration-plan.md`；不可违反的依赖约束见 `module-dependency-rules.md`。

---

## 1. 保留下来的东西

**保留 `apps/` + `packages/` + `ports/` 三层与现有工具链**：

| 保留 | 理由（当前证据） |
| --- | --- |
| pnpm workspace（`packages/*`、`apps/*`） | `pnpm-lock.yaml` lockfileVersion 9.0、15 个 importer 与全部 workspace manifest 一致；结构本身不是瓶颈 |
| `tsc -b tsconfig.packages.json` 作包构建 | 隔离副本上 exit 0（real 6.761 s） |
| Astro 静态输出 + Svelte 组件作前端 | `astro build` 在隔离副本 exit 0；路由是 4 个静态页面 |
| `ports/<engine>/` 承载移植与原生构建配方 | `check-sources.mjs` 已把端口配方/补丁/overlay 作为门禁对象 |
| `ports/lib/engine-artifacts.json` 作产物台账 | `make-package.mjs` 已按它校验字节且无 opt-out |
| `scripts/check-layout.mjs` 与 `scripts/check-sources.mjs` 作布局/来源门禁 | 两条门禁已存在且各自有明确职责 |

**不引入** Turborepo / Nx / Bazel 等新构建器：当前包规模（13 包 + 1 app + 14 个 importer）与真实瓶颈（声明依赖边与真实 import 不一致，而非任务图规模）都不支持这笔成本。

**不自动扩散仓库**：不因为"看起来更清晰"而拆包、拆仓库、加 workspace 层级。每一次拆分都必须先有一个**真实独立消费者**（见 §4）。

---

## 2. 边界优先（boundary first）

目标不是"更多结构"，而是**让边界可被校验**：

1. **每个包的对外面固定**：`exports` 声明的入口是唯一合法入口；未声明的深路径导入视为评审阻塞项。
2. **依赖方向单向向下**：`engine-api` 在最底，`app` 在最上；`core` 与 `engines` 并列，互不导入。
3. **声明的依赖 = 真实使用的依赖**：manifest `dependencies`、包 `tsconfig.json` `references`、源码 import 三者一致（当前有三处不一致，见 `current-state.md` §2.1）。
4. **组合根唯一**：引擎 provider 的注册列表只有一处权威来源。
5. **治理面固定**：发布门禁、许可关键路径 owner、备份与恢复状态都有明确归属。

边界优先的判据是"能不能在评审阶段发现漂移"，而不是"目录看起来是否整齐"。

---

## 3. 包与 SDK 命名决策

### 3.1 现有名保持

- **`@online-gal/engine-api` 不改名**。重命名是破坏性变更，且当前没有受益者：`smoke-consumer.mjs` 断言消费方顶层只能解析出 `@online-gal/engines`，vendored 计数 10，`runtimeId` 唯一，`listEngines`/`engineProviders`/`ENGINE_PACKAGES` 各 8。
- **"引擎 SDK"作为一个契约概念存在，而不是一个新的包名**。目标语义：`engine-api` 对外承诺的稳定面 = provider 接口 + 运行时注册表 + 源抽象（`compat`/`options`/`registry`/`sources`/`types`）。凡是"引擎实现者需要遵守的东西"都属于它；实现细节不属于。

### 3.2 只有当出现真实独立引擎仓库时才新增包

若未来某个引擎（例如 Artemis compat）被独立成仓库并由外部维护者构建：

- 该仓库消费 `engine-api` **的发布产物**（`dist` + `types`），而不是深路径 import 源码；
- 宿主侧**不**因引入它而新增分支，只新增一个 catalog 条目（见 §5）；
- 该仓库自带其构建配方、pin、许可与 source offer；本仓库不复制其源码。

**只有在上述条件成立时**，才讨论"是否需要把 `engine-api` 拆成独立发布包 / SDK 包"。在此之前不拆。

### 3.3 `core` 与组合的分离（**目标，非本轮动作**）

目标形态：

- `engines` 聚合包提供**唯一的** provider 注册表与选择语义；
- `core` 的 `Platform` 消费该注册表，**不再**维护第二份手写列表；
- `apps/app` 只经过 `core`，不直接知道 `engines` 的存在；
- 保持"实时引擎优先于解释器、具体引擎优先于宽泛 web provider"的既有 tie-break 顺序。

这是一条**目标**：当前 `core/src/platform.ts:21-39` 与 `engines/src/index.ts:94-137` 各写一份，顺序相同但独立维护。收敛会改变注册来源，因此必须伴随识别回归（同一组测试游戏目录的选择结果不变）。

### 3.4 可选拆分：archive / storage SDK（**仅在有真实独立性时**）

`vfs` 的子导出 `./archives`（archive 读取）与 `IndexedDbStore`（存储）**暂不**拆包。

拆分的判据是**是否存在只想要其中一半的消费者**：

- 若出现"只要 archive 读取、不要索引/懒加载"的消费者 → 讨论 `archive-sdk`。
- 若出现"只要存储抽象、不要 archive"的消费者 → 讨论 `storage-sdk`。

当前没有：`image-codec` 只用 `vfs` 的 `extname`；`runtime-*` 用源抽象；`ports/lib/fetch-engine.mjs` 是 Node 脚本，不消费 `vfs`。因此保持单包。

---

## 4. 允许的依赖图与稳定公开 API

### 4.1 允许的层（自上而下）

```
apps/app            （应用壳，只依赖 core）
  └── core          （组合根：library/session/storage/platform）
        └── runtime-*（各引擎实现，只依赖 engine-api / vfs 及较低层）
        └── engines （聚合注册表，依赖 engine-api + 各 runtime-*；与 core 并列）
              └── player-host, image-codec, vfs, engine-api
```

**硬规则**（详见 `module-dependency-rules.md`）：

- `engine-api` 不依赖任何 workspace 包。
- `vfs` 只依赖 `engine-api`。
- 任何包**不得**依赖 `core`、`engines` 或 `apps/app`（反向导入禁止）。
- `core` 与 `engines` **互不导入**。
- `apps/app` 不得绕过 `core` 直接构造运行时注册表。

### 4.2 稳定公开 API

每个包的公开面 = `exports` 中声明的入口。目标承诺：

- **`engine-api`**：provider 接口、`RuntimeRegistry`、源抽象、选项类型、compat 契约。这是"引擎实现者"与"宿主"之间的唯一契约。
- **`vfs`**：`GameSource`/`WritableStore` 抽象、`normalisePath`（**唯一**路径规则）、`IndexedDbStore`、`./archives` 子导出。
- **各 `runtime-*`**：provider 实现 + 生命周期动词（§4.3）。引擎内部结构不承诺。
- **`core`**：`Platform`、`GameLibrary`、`Session`、`SaveStore`、`SettingsStore`、`capabilities`。
- **`engines`**：注册表与选择语义（`ENGINE_PACKAGES`、`listEngines`、`engineProviders`）。

### 4.3 生命周期契约

统一动词：**probe / load / pause / dispose**；宿主会话：**`Session.start` / `Session.stop`**。

**实测现状（`packages/core/src/session.ts`，目标描述必须与之一致）**：

- `start`（`:197-205`）：加载运行时 → 启动循环 → 首次呈现。
- `pause`（`:226-229`）：先 `this.runtime.pause()`，再 `this.loop.stop()`（不是只停循环）。
- `resume`（`:231-236`）：`this.runtime.resume()` → `this.loop.start()`。
- `stop`（`:237-243`）：`this.loop.dispose()` → `this.inputGate.reset()` → `this.detachInput()` → `this.runtime.dispose()`；**幂等**。注意顺序：先 **dispose** 循环，不是先 `loop.stop()`。
- `dispose` 释放宿主侧资源（frame / worker / canvas 上下文 / 监听 / 定时器），可重复调用。
- `pause` 只暂停时钟与输入，**不**释放加载成本高的资源。
- 引擎特有步骤通过加载选项表达，**不**引入新的生命周期动词。

---

## 5. 按需引擎目录与 artifact manifest（**明确的目标**）

### 5.1 目标形态

```
catalog 条目（engine id, 版本, 可获取位置）
   └── artifact manifest（名字, 字节大小, sha256, 上游 commit, 工具链版本,
                          source offer 是否可用, license 是否可用, ABI/运行前置）
          └── provider 加载（URL / 本地挂载路径）
```

- 引擎字节**不**打包进前端 bundle；通过声明式 catalog 按需加载。
- 新增引擎**不**需要改应用启动代码。
- 每个可交付产物的 manifest 至少有：**URL / sha256 / ABI（运行前置）/ source offer / license 可用性**。
- manifest 与发布产物**在同一提交**中一致；下载时按大小 + sha256 校验后才采用。

### 5.2 撤回语义

- 候选产物不存在或未过静态门禁 → provider **撤回**，**不**发布会 404 的 URL。
- 已存在但**从未经过真实游戏验收**的产物 → 文档与界面**不得**表述为可玩；发布开关保持关闭。

### 5.3 与当前状态的距离

当前已部分具备（编译期 `__ARTEMIS_ASSETS_AVAILABLE__` 门禁、KiriKiri 的 `pageUrl`、ledger 的大小+sha256 校验、`fetch-engine.mjs` 的先验后移），目标是把它们**形式化为 manifest**，而不是重写。

**必须保持的三个区分**：

1. `new URL('../assets/', import.meta.url)` **不**等于"资产已被打进 bundle"。
2. 构建成功 **不**等于引擎能运行。
3. 隔离环境里只走了"撤回分支"的构建 **不**证明"资产 emit 分支"可用。

---

## 6. 脚本沙箱与云同步：只定义接口

`scripting-permissions` 是**未来**能力。目标形态只规定边界：

- **静态部署必须始终充分**：无后端时识别/运行/存档/设置全部可用；任何后端能力都是可选增强。
- **未来浏览器内脚本（QuickJS 一类）** 必须运行在显式能力授权模型下：默认无文件系统 / 网络 / 宿主对象访问（显式拒绝，而不是默认允许）；每项能力由宿主按会话授予并可撤销。
- **受控 API 面**：文件与网络只能经宿主提供的显式受控 API 访问，脚本**不得**直接接触 DOM、IndexedDB、Worker 或原始引擎句柄。
- **资源预算**：脚本宿主必须限制内存、执行时间/CPU 与句柄/资源数量，超限时错误退出而不影响宿主其余部分。
- **插件 API 版本与来源政策**：必须声明插件 API 版本并在加载时不匹配即拒绝；必须区分官方插件与第三方插件，两者权限不同且第三方默认权限更小。
- **引擎内嵌脚本不是沙箱**：Artemis 内嵌的 Lua **不得**被复用为平台级脚本宿主，两者的隔离假设不同。
- **云同步以现有导出/导入契约为缝**：所有权、认证（authn）、授权（authz）、冲突合并与数据版本都必须先有定义的最小门禁缝；在契约确定前不引入后端框架、不改键空间。

**明确不做的**：

- 不把"实现一个脚本引擎/插件运行时"当作本变更的前置条件；没有 framework setup。
- 不因为写了接口就声称脚本执行、插件或云同步"正在做"或"快好了"；也没有任何 endpoint 已存在。
- 不在没有冲突规则与 authn/authz 定义的情况下发布同步。
- 不把 Svelte 等**构建时**依赖的存在/缺失当作"是否存在第三方前端运行时"的判据：站点资产是否包含第三方运行时需看构建产物清单（**未核**）。

---

## 7. 目标状态下的"完成"判据

一条目标只有同时满足下列条件才算达成，**缺一不可**：

1. 对应的 requirement 有可运行的验证（测试、门禁或可观测行为）；
2. 验证在**目标环境**（CI pin 的 Node 22 或明确的验收环境）下运行过，且记录在命令台账里；
3. 若涉及引擎：至少一次真实游戏验收（丢弃一次热身 + 两次一致）；
4. 文档更新，且把"未证明"的部分留在未知清单里，而不是被目标条款吸收。

**"计划写完了"不是"实现完成了"。** OpenSpec 的 4/4 planning artifacts 只说明规划存在。
