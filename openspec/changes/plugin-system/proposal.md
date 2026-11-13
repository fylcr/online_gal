# Proposal

## Why

用户要开始做插件系统。仓库里对"插件"只有**边界预留**、没有实现：

- `openspec/changes/establish-architecture-and-release-baseline/specs/scripting-permissions/spec.md:115-129` 的 `Plugin API version and provenance policy`：插件必须声明 API 版本，不匹配即拒绝加载；必须区分官方/第三方，第三方默认最小权限；任何插件不得绕过宿主授权模型。
- 同一变更的 `tasks.md:52-54` 把插件列为 Phase 5、只在出现真实需求时启动。现在用户明确要求启动。

用户的设定（按时间顺序，后者覆盖前者）：

1. "所有功能都作为插件使用"；界面在构建时直接产出为插件，改名叫**管理器（manager）**。
2. 插件系统不是"每页都加载一次的内核"，而是**本地的小型服务器**："插件系统更像是一个 worker 拦截了请求以方便在本地提供更自由的页面和功能"。请求链路是 `浏览器 → 插件系统 → 管理器 → 插件系统 → 浏览器`；换成第三方管理器就是 `浏览器 → 插件系统 → 第三方管理器 → 插件系统 → 浏览器`。真正的服务器只是插件的来源。
3. 分成两个产物：管理器仍用 Astro 构建，再打包成插件；插件系统只要 Node.js 能构建出来即可，**性能和稳定性优先**。
4. 官方插件由 **Cloudflare Pages** 分发；第三方插件来自 **GitHub 仓库的标签**。

用户表示详细设定之后补齐，所以本变更只做不依赖细节就必须成立的部分：插件包格式与校验、两种来源、安装/更新/切换/卸载、把当前管理器当作站点在本地提供、以及出错时仍然能恢复。

## What Changes

- 新增包 `packages/plugin-system`（`@online-gal/plugin-system`）：一个 Service Worker 内核，构建为站点根的 `/sw.js`（esbuild，classic script）。它接管本源的所有请求，从已安装的插件里应答；只在安装或更新插件时访问网络。
- 新增插件包格式 `plugin.json`（format 1）：manifest + 每个文件的路径、大小、SHA-256。内核逐字节校验，不认识或对不上的一律拒绝。
- 新增两种来源：`official:<id>`（本站 `/registry/`，Cloudflare Pages）与 `github:<owner>/<repo>@<tag>`（`raw.githubusercontent.com` 上的标签）。信任程度由**来源**决定，不由 manifest 自称。
- 新增构建脚本：`packages/plugin-system/scripts/pack-plugin.mjs`（把任意目录打成插件包，第三方作者也用它）与 `packages/plugin-system/scripts/build-site.mjs`（产出 Cloudflare Pages 目录 `dist/site/`：站点根的官方管理器静态页（带 `load` 之后注册 `sw.js` 的脚本）、`sw.js`、`_headers`、官方注册表）。根 `package.json` 新增 `build:site`。
- `apps/app` 新增 `plugin.manifest.json`：官方管理器 `online-gal.manager`，`role: "manager"`。Astro 构建产物原样打包为该插件，不改管理器代码。
- `packages/plugin-host` 保留为插件规则层（manifest 校验、版本闸门、来源分级授权、受控表面、预算、服务图）。manifest 新增 `role`；去掉不再成立的 `ui.mount` 与页面根。插件系统复用它的 `parseManifest`/`assertApiCompatible`。
- 插件包的文件条目新增可选的 `lazy: true`：安装时不下载，第一次被请求时再下载并校验；格式版本仍是 1。官方管理器把 `engines/**` 与 `**/*.map` 标为 lazy，安装下载量从 3.1 MiB 降到约 0.1 MiB。
- 新增 `.test/plugin-system.test.ts`（本地工作台，gitignored）：在 Node 里用内存 CacheStorage 与假网络驱动内核。

## 待用户补齐的设定

- (a) 非管理器插件做什么：现在只能作为静态文件挂在 `/__plugins/<id>/`，没有执行模型（何时运行、在哪运行、能调什么）。`plugin-host` 的 `boot`/服务图/授权是为这一步准备的，尚无调用方。
- (b) 第三方管理器的信任：它和官方管理器同源运行，能读写本站全部存储（存档、游戏库）并调用内核 API。目前的处理是"由用户主动安装即视为信任"，是否需要确认界面、警告或权限分级由用户决定。
- (c) 引擎是否拆成独立插件：现在引擎随官方管理器一起打包。拆分会改变管理器与引擎之间的接口。
- (d) 更新策略：官方插件每 6 小时最多后台检查一次并自动更新；第三方插件只在用户要求时更换标签。是否需要提示、是否允许关闭自动更新未定。
- (e) 插件管理界面：现在只有内核自带的极简页面 `/__kernel/`；管理器里的安装、切换、卸载界面未做（API 已就绪）。

## Capabilities

### New Capabilities

- `plugin-system`: Service Worker 内核——插件包格式与校验、官方/GitHub 两种来源、安装/更新/切换/卸载、把当前管理器作为站点本地提供、恢复页与重置。
- `plugin-host`: 插件规则层——版本化 manifest、来源分级默认权限、会话级授予与撤销、受控 VFS/网络/私有存储、资源预算、故障隔离、服务图。

### Modified Capabilities

- 无。`scripting-permissions` 仍是未归档的边界 delta，本变更不修改它。

## Impact

- 新增 `packages/plugin-system/**`、`packages/plugin-host/**`、`apps/app/plugin.manifest.json`；`tsconfig.packages.json` 两条 reference；`pnpm-lock.yaml` 两个 importer（`plugin-system` 新增 devDependency `esbuild@0.27.7`，仓库已有该版本）。
- 根 `package.json` 新增 `build:site`；`build`/`dev`/`test` 不变，因此 CI 与本地开发不受影响。
- **部署改变**：Cloudflare Pages 的构建命令要改为 `npm run build:site`，输出目录改为 `dist/site`。不改的话线上仍是旧的 Astro 静态站，插件系统不生效。
- 不改管理器源码、不改任何引擎包、`ports/`、`.github/**`、许可文件。
