# 模块依赖规则（Module Dependency Rules）

> 本文件分两部分：**§1–§3 是当前实测事实**，**§4–§7 是拟议规则**（只有 `openspec/changes/establish-architecture-and-release-baseline/specs/system-architecture/spec.md` 被接受并实现后才生效）。事实部分带 `路径:行号`；拟议部分标注为"拟议"。
>
> 证据等级：**已观察** / **已确认约束** / **推断** / **未验证假设**。全文件不把"静态图"称为"运行时图"。

---

## 1. 事实：契约层与依赖方向

`@online-gal/engine-api` 是最底层：**没有任何 workspace 依赖**（`packages/engine-api/package.json` 的 `dependencies` 为空，`tsconfig.json` 无 references）。

实测的 workspace 依赖边（manifest `dependencies`，全部 `workspace:*`）：

| 包 | 依赖 |
| --- | --- |
| `engine-api` | *(无)* |
| `vfs` | engine-api |
| `image-codec` | vfs |
| `player-host` | engine-api, vfs, image-codec |
| `runtime-web` | engine-api, vfs |
| `runtime-tyrano` | engine-api, vfs, runtime-web |
| `runtime-siglus` | engine-api, vfs |
| `runtime-artemis` | engine-api, vfs |
| `runtime-kag` | engine-api, vfs, image-codec, player-host |
| `runtime-ons` | engine-api, vfs, image-codec, player-host |
| `tjs2-bytecode` | engine-api, vfs, runtime-kag |
| `core` | engine-api, vfs, image-codec, player-host, runtime-{kag,ons,siglus,artemis,tyrano,web} |
| `engines` | engine-api, runtime-{kag,ons,siglus,artemis,tyrano,web} |
| `app` | core, engine-api, image-codec, player-host, runtime-{artemis,kag,ons,tyrano,web}, vfs |

**已确认约束**：`core` 与 `engines` **互不导入**（两者都消费 runtime，但彼此之间没有边）。

## 2. 事实：值边 vs 类型边

`import type` 只产生类型边，**不构成值依赖**，也就**不构成值环**。已观察到的类型边至少三条：

- `packages/runtime-tyrano/src/index.ts:1-2`：对 `engine-api` 是 `import type`；对 `runtime-web` 是值导入。因此 `runtime-tyrano → engine-api` 这条 manifest 边**不是**运行时值边。
- `packages/vfs/src/sources.ts:1`：`import type { GameSource, WritableStore } from '@online-gal/engine-api'`。因此 `vfs → engine-api` 也是**类型边**。
- `packages/tjs2-bytecode/src/analyse.ts:1-10`：类型导入 engine-api，值导入本包模块。

**未使用的 manifest 声明（不是依赖）**：`@online-gal/runtime-siglus` 在 `packages/runtime-siglus/package.json:20` 声明 `vfs`，但 `packages/runtime-siglus/src/**` 无任何 `@online-gal/vfs` import —— 归为**声明但未使用的潜在依赖**。

已观察到的值边（抽样，非全仓扫描）：

- `packages/core/src/platform.ts:1-8`、`packages/core/src/session.ts:1-11`、`packages/core/src/storage.ts:1`、`packages/core/src/library.ts:1-16`
- `packages/engines/src/index.ts:14-22`（且 `export * from '@online-gal/engine-api'` 是**值再导出**）
- `packages/image-codec/src/index.ts:1`
- `packages/tjs2-bytecode/src/script-bytes.ts:10-12`
- `packages/player-host/src/assets.ts:1-3`
- `packages/runtime-web/src/runtime.ts:1-12`
- `packages/runtime-kag/src/wasm-provider.ts:11-21`、`packages/runtime-ons/src/wasm-provider.ts:12-16`
- `packages/runtime-siglus/src/runtime.ts:1-16`、`packages/runtime-artemis/src/runtime.ts:1-29`
- `apps/app/src/lib/platform.ts:7-11`（只值导入 `core`、`runtime-kag`、`runtime-artemis` 三包）

**app 的其余 manifest 边**（`apps/app/package.json:12-23` 的另外 7 项：engine-api、image-codec、player-host、runtime-ons、runtime-tyrano、runtime-web、vfs）是**声明边**：manifest 有、启动链未用。**不算启动依赖，也不算运行时依赖**。

## 3. 事实：静态图无环，但静态图不是运行时图

**已确认约束（有界结论）**：由 manifest `dependencies` + 包 `tsconfig.json` `references` + comment-stripped 静态 import 正则（覆盖 `import` / `export … from` / `import(...)`）三者交叉核对，**每个包自成一个 singleton，不存在任何大小的静态环**。包级 manifest 与 references 在包级一致。

**被推翻的旧结论（记录以免复发）**：早期报告提出的 `runtime-kag ↔ tjs2-bytecode` 环**不成立**。

- `runtime-kag` 的 manifest 与该包 `tsconfig.json` 的 references 都不含 `tjs2-bytecode`。
- `packages/runtime-kag/src/runtime.ts:40` 对 `tjs2-bytecode` 的唯一提及是**注释**（"The check is inlined rather than imported from `@online-gal/tjs2-bytecode`"）。
- `packages/runtime-kag/src/compiled.ts:10` 只值导入 `@online-gal/vfs/archives`；`loadScriptBytes`/`decodeCompiledScript`/`sniffScriptFormat`/`DecodedScript` 正是在 `compiled.ts` **定义**的，由 `runtime-kag/src/index.ts:2` 再导出，`tjs2-bytecode/src/script-bytes.ts:11-12` 再把它转出去。**依赖只有 `tjs2-bytecode → runtime-kag` 单向。**

**正则的限制（必须随结论一起引用）**：该扫描**不**解析 `require()` 字符串字面量、生成代码、非字面量动态 `import()`，因此它**证明不了**"不存在运行时 service 环"。它证明的是"不存在声明或静态 import 的边"。任何把它读成"运行时无环"的引用都是过度解读。

**未核**：包内 cycle、Worker import、跨包相对路径 import、动态 `import()` 的全仓清单。

## 4. 事实：三处声明/使用不一致

| # | 不一致 | 证据 |
| --- | --- | --- |
| 1 | `core` 依赖但 tsconfig 未引用两个 runtime | `packages/core/package.json:16-27` vs `packages/core/tsconfig.json:7-16`（只有 8 项）vs `packages/core/src/platform.ts:5-6`（值导入） |
| 2 | `image-codec` 依赖 `vfs` 但 tsconfig 无 reference | `packages/image-codec/package.json` `dependencies.vfs` vs `packages/image-codec/tsconfig.json`（**无** `references` 字段）vs `packages/image-codec/src/index.ts:1`（值导入 `extname`） |
| 3 | `runtime-kag` 声明 `image-codec` 但源码无 import | `packages/runtime-kag/package.json` 与 `tsconfig.json` 声明/引用；`runtime-kag/src` 无命中 |

**已确认约束**：`tsc -b` 的构建顺序由 references 决定，因此 #1/#2 意味着这两条真实依赖**不在** build graph 里。**潜在风险，不是已发生的构建失败**：隔离副本上 `tsc -b tsconfig.packages.json` 是 **exit 0**（real 6.761 s，见 `../development/testing.md` §3）。**未验证**：在清理 `dist/` 与 `*.tsbuildinfo` 的拓扑顺序下，`core` 是否真的落在两个 runtime 之后。

## 5. 拟议：禁止的反向导入

以下规则在 spec 被接受后生效（当前状态**不违反**它们，因为静态图已是单向）：

1. **禁止反向导入**：任何包**不得**依赖 `@online-gal/core`、`@online-gal/engines` 或 `apps/app`。
2. **禁止跨包深路径导入**：不得导入另一个包的 `src/**` 内部模块；只能用该包 `exports` 声明的入口（`.` 或已声明的子路径，例如 `@online-gal/vfs/archives`、`@online-gal/runtime-artemis/compat`）。
3. **`core` 与 `engines` 互不导入**：两者是并列的上层消费者。
4. **`apps/app` 只经过 `core`**：不得直接构造运行时注册表或读 IndexedDB。
5. **`engine-api` 保持零 workspace 依赖**：它是契约层；任何想放进它的实现细节必须下沉到别处。
6. **新增包必须回答"谁是消费者"**：没有当前或明确的未来消费者的包，不新增。

## 6. 拟议：共享类型，不共享业务

- **允许共享**：纯类型、接口、常量枚举、`runtimeId` 字面量、选项 schema。
- **不允许共享**：任何带行为的实现——注册逻辑、IO、格式解析、状态机。

判据：如果一段代码在 import 时会产生副作用或需要运行时环境，它**不是**共享类型，放在 `engine-api` 之外。

`engine-api` 当前承载 `types`/`options`/`registry`/`sources`/`compat`。`registry` 与 `sources` 含实现（`RuntimeRegistry`、`MemoryGameSource`、`OverlayGameSource`），它们是**契约的一部分**而不是"业务逻辑"，因此留在 `engine-api`；这一判断在新增内容时需要重新论证。

## 7. 拟议：消费者兼容性需求

新增或修改任何包的公开面时，必须同时检查：

1. **消费方顶层解析集合**：`smoke-consumer.mjs` 断言消费方顶层只能解析出 `@online-gal/engines`；vendored 计数 10；`listEngines`/`engineProviders`/`ENGINE_PACKAGES` 各 8；`runtimeId` 唯一。
2. **深路径导入**：只允许 `exports` 中声明的子路径。
3. **类型入口分叉**：`types`/`development` → `src`，`default` → `dist`。改动这个分叉等于改动**开发时**与**发布时**看到的代码，必须显式评审。
4. **engine 选择顺序**：任何触及 provider 顺序或 `kind` 的改动都需要识别回归（对固定游戏目录集，改动前后选择一致）。

## 8. 本文件不声明的事

- **不声明"已验证 SCC"**：本轮只有"无静态环"这一有界结论（见 §3）。除非有完整的 TS AST 或运行时追踪证据，否则不得把任何图结论写成"已验证的 SCC 分解"。
- **不声明运行时无环**：见 §3 的限制段落。
- **不声明包内无环**：包内部结构未扫。
- **不声明 `runtime-kag` 的 `image-codec` 是缺陷**：它是"声明但未使用"，意图未知（见 §4 #3）。
