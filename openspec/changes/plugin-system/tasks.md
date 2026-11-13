# Tasks: plugin-system

把插件系统做成本地服务器：一个 Service Worker 内核接管本源的请求，从已安装的插件中应答；官方插件来自 Cloudflare Pages，第三方插件来自 GitHub 标签；管理器由 Astro 构建后打包成插件。`packages/plugin-host` 保留为插件规则层。范围以 `design.md` 的 Non-Goals 为界。

## 1. 插件规则层：契约与包骨架（`packages/plugin-host`）

- [x] 1.1 新建 `packages/plugin-host`（`package.json`、`tsconfig.json`），只依赖 `@online-gal/engine-api`（`GameSource`/`WritableStore`），加入 `tsconfig.packages.json` 与 pnpm workspace
- [x] 1.2 `src/errors.ts`：`PluginErrorCode` 联合与 `PluginError`，`asPluginError()` 把任意抛出值收敛为带码错误
- [x] 1.3 `src/manifest.ts`：`PluginManifest`、`parseManifest()`（拒绝未知字段/坏 id/非 semver/未知 capability/重复声明，返回冻结对象）、`assertApiCompatible()`（精确匹配 `PLUGIN_API_VERSION`）
- [x] 1.4 `src/capabilities.ts`：`PLUGIN_CAPABILITIES`、`DEFAULT_GRANTS`（`official: vfs.read+ui.notify`，`third-party: []`）、`GrantLedger`（只授予已声明的、每次调用 `require`、`close()` 清空、事件历史）

## 2. 插件规则层：受控表面与预算

- [x] 2.1 `src/budget.ts`：`ResourceBudget { memoryBytes, timeMs }`、`DEFAULT_BUDGETS`、`BudgetMeter`（窗口、`checkpoint()`、分配前检查的 `chargeMemory()`）
- [x] 2.2 `src/host-api.ts`：`HostApi` 构造冻结的 `PluginContext`（`vfs`/`store`/`net`/`notify`/`log`/`now`），每个方法都 `grants.require` + `budget.checkpoint`
- [x] 2.3 路径闸门 `assertRelativePath()`：拒绝盘符、UNC、前导分隔符、NUL、含 `..` 段、超长路径，且在校验前不触碰规范化
- [x] 2.4 `store` 前缀隔离：落盘到 `plugins/<pluginId>/`，`list()` 按目录边界过滤并剥离私有根
- [x] 2.5 未配置的宿主能力（`net`/`notify`）抛 `capability-unavailable`，绝不回退到 `fetch` 或全局对象

## 3. 插件规则层：宿主、启动与服务图

- [x] 3.1 `PluginHost.register()`/`boot(entries)`：全部闸门通过后才调 `load()`；结果按输入顺序、不抛；`register(entry)` = `boot([entry])`
- [x] 3.2 `provides`/`requires`：依赖可满足性、拓扑序、环检测；同一依赖层并行 `load()`，`setup` 仍按拓扑序；`setup` 返回值与 `provides` 严格一致；`ctx.use` 只给已声明的
- [x] 3.3 特权能力 `plugins.manage`：第三方不可声明、不进默认、仅按 id 批准、不可授出
- [x] 3.4 `grant`/`revoke`/`unload`/`dispose`；停止顺序为消费者先于提供者、`dispose()` 逆序；失败与卸载都先 `ledger.close()` 再 `dispose()`
- [x] 3.5 `dispatch(hook, payload)` 逐插件隔离；`plugins` 检查视图（状态、授权、拒绝次数、预算、失败原因）
- [x] 3.6 manifest 新增 `role: "manager"`；删除 `ui.mount` 与页面根（管理器就是站点本身，不再挂载）

## 4. 插件规则层：测试（本地工作台 `.test/`，gitignored）

- [x] 4.1 `.test/plugin-host.test.ts` 25 项：manifest 拒绝、版本闸门、默认授权、撤销与卸载、受控表面与路径闸门、预算、失败隔离
- [x] 4.2 `.test/plugin-kernel.test.ts` 18 项：启动顺序、服务图、并行加载、特权能力、停止顺序、`role`

## 5. ~~内核化：页面先启动内核，再加载管理器插件~~（已被第 6–9 节取代）

用户否定了"每页都加载一次插件系统"（见 `proposal.md`）。原 5.1–5.7 中仍成立的部分已并入第 3 节；页面根、`ui.mount`、构建期内联插件 chunk 与 `modulepreload` 方案作废。

## 6. 插件系统：Service Worker 内核（`packages/plugin-system`）

- [x] 6.1 新建 `packages/plugin-system`，依赖 `@online-gal/plugin-host`，devDependency `esbuild@0.27.7`；加入 `tsconfig.packages.json`
- [x] 6.2 `src/format.ts`：`plugin.json` format 1 的 `parsePackage()`（复用 `parseManifest`/`assertApiCompatible`）、安全相对路径、上限（5000 文件 / 单文件 256 MiB / 合计 1 GiB）、管理器保留路径、注册表 `parseRegistry()`、`sha256Hex()`
- [x] 6.3 `src/sources.ts`：`official:<id>` 与 `github:<owner>/<repo>@<tag>` 的解析与 URL 构造；来源决定 `provenance`，第三方不得使用 `online-gal.` 前缀
- [x] 6.4 `src/kernel.ts` 路由：同步的 `routeRequest()`；`/sw.js`、`/registry/**` 放行；`/__kernel/**`、`/__plugins/<id>/**`、其余导航与子资源由内核应答；所有应答带 COOP/COEP/nosniff
- [x] 6.5 静态托管语义：目录索引、`/a` → `/a/` 的 308（保留查询串）、`404.html`、HEAD、单段 Range（206/416）、`Cache-Control: no-cache`；解码失败为 400
- [x] 6.6 状态：`og-kernel` 缓存中的 JSON（`STATE_FORMAT = 1`），启动时读一次；先写状态再切换内存；改变状态的操作排队执行；格式不认识时从空状态开始
- [x] 6.7 安装：新一代写入独立缓存，6 路并发下载，逐文件校验大小与 SHA-256，全部通过且状态写入成功后才切换；失败删除新缓存；同包且文件完好时不重新下载
- [x] 6.8 更新与清理：保留最多一代旧文件供已打开页面使用，没有客户端时清除；一小时后清理无主缓存；管理器文件丢失时从原来源 `repair`
- [x] 6.9 管理器选择：没有可用管理器时先换用其他已安装管理器，再安装默认 `online-gal.manager`；Service Worker 安装阶段预先执行，失败不阻止启用
- [x] 6.10 官方后台更新：导航时 `waitUntil` 检查注册表，间隔至少 6 小时，只更新摘要变化了的官方插件；首次安装默认管理器时顺带记一次检查，首次导航不再重复取注册表（首次访问的服务器请求 12 个降到 11 个，FCP 无可测变化）
- [x] 6.11 内核 API `/__kernel/api/*`（`state`/`registry`/`install`/`uninstall`/`activate`/`update`/`reset`）：JSON 对象请求、拒绝导航式请求、统一响应与按错误码映射 HTTP 状态；`postMessage` 通知打开的页面
- [x] 6.12 `/__kernel/` 内核页与导航失败时的恢复页，不依赖任何插件
- [x] 6.13 `src/sw.ts`：把 `install`/`activate`/`fetch` 接到 `Kernel`；`skipWaiting` + `clients.claim`
- [x] 6.14 `format.ts`：文件条目可选 `lazy: true`（非布尔拒绝；至少一个非 lazy 文件）；格式版本仍为 1
- [x] 6.15 安装只下载非 lazy 文件，安装记录保存 lazy 清单与 `base`；请求时 `fetchLazy` 下载并校验，并发请求共用一次下载；完整性检查与修复只看非 lazy 文件；上一代同样保留 lazy 清单
- [x] 6.16 lazy 文件以 `fetch-failed` 失败时立即 `refreshOfficial`（忽略更新间隔，仅官方插件），有新版本就改从新版本应答

## 7. 构建与分发

- [x] 7.1 `apps/app/plugin.manifest.json`：官方管理器 `online-gal.manager`，`role: "manager"`；构建时版本追加内容哈希
- [x] 7.2 `packages/plugin-system/scripts/pack-plugin.mjs`：把任意目录打成插件包（第三方作者用它生成 `plugin.json`），使用与内核相同的 `parsePackage`；`--lazy <glob>`（可重复，`!` 开头的 glob 把匹配的文件留作立即下载）标记按需下载的文件，标记计入内容哈希
- [x] 7.3 `packages/plugin-system/scripts/build-site.mjs`：产出 `dist/site/`——站点根的官方管理器静态页（原样复制，每个 `.html` 在 `</body>` 前注入 `load` 之后注册 `/sw.js` 的脚本；无顶层 `404.html`）、`sw.js`（esbuild classic、压缩，约 31.5 KiB）、`_headers`、`/registry/index.json` 与带内容哈希的 `/registry/p/<id>/<hash>/`
- [x] 7.4 `_headers`：全站 COOP/COEP/nosniff；`/sw.js` 与注册表索引 `no-cache`；`/registry/p/*` `immutable`
- [x] 7.5 根 `package.json` 新增 `build:site`；`build`/`dev`/`test` 不变
- [x] 7.6 构建脚本给官方管理器的 lazy 配置是 `engines/**` 与 `**/*.map`：24 个文件共 3.1 MiB，安装时下载 14 个、约 0.1 MiB（10 个 lazy）。曾试过除入口页外全部 lazy，站点根提供静态页之后没有收益，已改回
- [ ] 7.7 Cloudflare Pages 项目设置：构建命令 `npm run build:site`，输出目录 `dist/site`（需要用户在 Cloudflare 控制台操作）

## 8. 测试与验证

- [x] 8.1 `.test/plugin-system.test.ts` 34 项（其中 lazy 文件 10 项）：内存 CacheStorage 与注入的 `fetch`，覆盖路由、静态托管语义、编码路径、包与注册表校验、两种来源、原子安装与回滚、上一代、修复、管理器切换与回退、后台更新、内核 API
- [x] 8.2 三套测试合计 77 项全绿；`npx tsc -b tsconfig.packages.json` 通过
- [x] 8.3 端到端（本地 Chromium + CDP，仿 Cloudflare Pages 的静态服务器 + 假 GitHub raw 服务器）：14/14 通过；80 ms 服务器延迟下深链首次访问 FCP/LCP 约 430–540 ms（3 次：427、470、536 ms；上一版先装内核再显示约 900–1000 ms；样本少）；安装时不下载引擎与 sourcemap，引擎文件在第一次请求时才下载一次且字节一致，第二次由本地应答；首次访问与刷新后的页面都跨源隔离（首访检查的是生成的 `_headers` 内容，测试服务器按它加响应头，并没有测 Cloudflare 本身）；刷新时服务器收到 0 个请求（约 306 ms）；离线可用；内核页可用；从 GitHub 标签安装第三方管理器并接管站点；切回官方并卸载第三方
- [x] 8.4 `openspec validate plugin-system --strict` 通过

## 9. 待用户补齐后展开（不在本变更）

- [ ] 9.1 非管理器插件的执行模型：在哪运行（页面 / Worker / 沙箱 iframe）、钩子表与服务表、调用时机；届时接上 `plugin-host` 的 `boot`/服务图/授权
- [ ] 9.2 第三方管理器的信任：确认界面、警告或权限分级
- [ ] 9.3 引擎是否拆成独立插件，以及管理器与引擎之间的接口
- [ ] 9.4 更新策略：是否提示、能否关闭自动更新、第三方是否检查新标签
- [ ] 9.5 管理器内的插件管理界面（安装、切换、卸载、失败展示；内核 API 已就绪）
- [ ] 9.6 与 `Session`/运行时接线：插件在游戏启动、存档、脚本执行中的调用点
- [ ] 9.7 lazy 文件的预取策略（例如空闲时预取引擎）与离线提示：现在没取过的引擎文件离线时会返回 503
- [ ] 9.8 站点根的注册脚本在 Service Worker 已激活却没有接管时 `reload` 一次（10 秒内最多一次）：这条硬刷新路径没有测试覆盖；用户切到第三方管理器后硬刷新会先看到官方管理器
- [ ] 9.9 Service Worker 接管前后对未知路径的行为不一致：接管前 Cloudflare 回退到根 `index.html`，接管后内核对没有 `404.html` 的管理器返回纯文本 404；管理器在站点上存两份（根目录与 `registry/p/`）
