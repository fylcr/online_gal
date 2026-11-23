# Tasks: cf-account-admin

用 cf 自己的身份登录 Cloudflare，凭据只放在插件系统内核里并加密存放；把 cf CLI 的生成命令放进浏览器；拥有本站的账号可以进入 admin。顺序按用户要求：代理 → cf 浏览器构建 → 登录 → 设置页 → admin。范围以 `design.md` 的 Non-Goals 为界。

## 0. 可行性验证（先于对应阶段）

- [x] 0.1（先于 1）用 `wrangler pages dev` 实测 `functions/__cf/[[path]].ts` 的路由、`_routes.json` 同时包含 `/__ca/*` 与 `/__cf/*`、请求体与响应体的流式透传。结果见 design.md A2。
- [x] 0.2（先于 2）在 `.ref/cf` 检出上试打包：yargs 18 在浏览器里可用；生成命令只需要七个 `#lib/*` 替换；统计 `node:*` 与第三方包的引入；记下分组数量与产物体积
- [x] 0.3（先于 3）确认 `@cloudflare/workers-auth@0.18.4` 的 `/cf` 入口能读出 client id、默认权限列表和端点，以及被排除的权限（cf 构建产物里的 `Le`）；读不出就确定从产物中提取的方式，记入 design.md A1

## 1. 同源代理

- [x] 1.1 `functions/__cf/[[path]].ts`：按 design.md A2 实现映射、方法限制、请求头白名单、去掉 `set-cookie`、`no-store`、同源检查、流式透传，不记录日志
- [x] 1.2 `packages/plugin-system/scripts/build-site.mjs`：`_routes.json` 的 `include` 加 `/__cf/*`，RESERVED 加 `__cf`，并把站点主机名 `SITE_HOST`（默认 `onlinegal.cc.cd`）写入内核
- [x] 1.3 `.test/cf-proxy.test.ts`：假上游覆盖映射、404、方法限制、Cookie 不转发、`set-cookie` 被去掉、跨站 403、查询串保留

## 2. 浏览器版 cf（`packages/cf-browser`）

- [x] 2.1 包骨架：`package.json`（`@online-gal/cf-browser`，private；开发依赖 `yargs@18.2.0`、`@cloudflare/workers-auth@0.18.4`）、`tsconfig.json`，加入 pnpm workspace；`upstream.json` 记录固定 commit。类型检查用 `scripts/typecheck.mjs`，不加入 `tsconfig.packages.json`（依赖 `.ref/cf` 检出，见 design.md A7）
- [x] 2.2 `scripts/fetch-upstream.mjs`：把 `cloudflare/cf` 稀疏克隆到 `.ref/cf`（只取 `packages/cli`、根许可文件），核对 commit，落点必须在 `.ref/cf/` 下
- [x] 2.3 `src/lib/`：`cli-types`、`telemetry/index`（空实现）、`auth`（基础地址 `/__cf/api`、不带 token、`CLOUDFLARE_ACCOUNT_ID` 的缺省规则，见 design.md A7）、`dry-run`、`local`、`output`、`progress` 的浏览器实现
- [x] 2.4 `scripts/build.mjs`（esbuild）：解析插件（`#lib/*` → `src/lib`，`#api`、`#operations/*` → 上游）、按顶层分组拆包、入口 `runCf(argv, io)`、白名单闸门（遇到 `node:*` 或白名单外的包就失败）、许可文件与 chunk 头部的版权声明
- [x] 2.5 浏览器版 `auth` 分组：`login`、`logout`、`whoami` 走内核账号 API
- [x] 2.6 `.test/cf-browser.test.ts`：闸门（构造一个引入 `node:fs` 的模块，构建必须失败）、`runCf` 用假 `fetch` 执行若干生成命令（基础地址、方法、查询串、请求里没有 `Authorization`）、账号缺省规则、取消

## 3. 内核账号模块（`packages/plugin-system/src/account/`）

- [x] 3.1 `cf-identity.generated.ts` 的生成脚本（按 0.3 的结论），版本不一致时构建失败
- [x] 3.2 `vault.ts`：`VaultStore` 接口，IndexedDB 实现（`online-gal-vault` v1，`keys`/`credentials`）与内存实现；AES-GCM-256、不可导出密钥、每次写入换新 iv、附加认证数据、解密失败就删除
- [x] 3.3 `cloudflare.ts`：设备码登录（`login`、`poll`，含 `slow_down`/`expired`/`access_denied`）、刷新（单飞，`invalid_grant`/`invalid_client` 清除凭据）、退出（尽力吊销 + 一定清除）、状态（不含 token）、站点管理员判定（分页、主机名比对、localhost 改用 `SITE_HOST`、10 分钟缓存与清空规则）
- [x] 3.4 `kernel.ts` 路由：`/__kernel/api/account/<provider>/{status,login,poll,logout}`；`/__cf/**` 所有方法按 A3 核对调用方、丢弃页面的 `Authorization`、补 token 后转发；同步改 `openspec/changes/plugin-system` 的路由要求与保留路径
- [x] 3.5 `.test/plugin-account.test.ts`：保管库里没有明文 token、解密失败即删除、只放行官方管理器（第三方管理器、插件页、没有 clientId 时 403）、页面 `Authorization` 被替换、刷新单飞、`invalid_grant` 清除与网络错误不清除、离线退出、设备码流程各结果、站点管理员判定与缓存；跑现有插件测试确认没有回归；记下 `sw.js` 增加的体积（压缩后 +20.9 KiB：身份 9.9、`cloudflare.ts` 8.3、`vault.ts` 2.7；`IndexedDbVaultStore` 在 Node 下没有测试，靠真实浏览器验证）

## 4. 设置"账号"分区（`apps/app`）

- [x] 4.1 `src/lib/settings-sections.ts` 加 `account`；`src/pages/settings/[section].astro` 对 `account` 渲染 `AccountPanel.svelte`
- [x] 4.2 `src/components/account/AccountPanel.svelte`：未就绪、被拒、未登录、登录中（验证码、链接、倒计时、轮询）、已登录（用户、权限数量、到期时间、退出、站点管理员入口）
- [x] 4.3 `npx astro check` 与 `npx astro build`

## 5. admin 页（`apps/app`）

- [x] 5.1 `src/pages/admin.astro`：不是站点管理员时说明原因并链接到账号设置
- [x] 5.2 `src/components/admin/ProjectPanel.svelte`：项目名、账号、生产分支、最近部署、域名
- [x] 5.3 `src/components/admin/CfConsole.svelte`：分词器、历史、输出、取消、内联确认，按需 `import()` 浏览器版 cf
- [x] 5.4 `npx astro check`、`npx astro build`、`npm run build:site`；确认首屏不加载命令分组的 chunk，产物里有许可文件
  - 根 `package.json` 新增 `build:cf`（`fetch-upstream` 后构建 cf-browser），`build` 与 `dev` 都在构建 app 之前先跑它：app 依赖 cf-browser 的 `dist/`，而 `.ref/cf` 在干净克隆里不存在。已在固定提交时 `fetch-upstream` 不联网
  - `/admin` 首屏只加载管理页组件、`account`、Svelte 运行时；cf 入口在第一次执行命令时才 `import()`
  - 只含 cf 模块的分块放在 `_astro/cf/`（`astro.config.mjs` 的客户端 `chunkFileNames`），`build-site.mjs` 把 `_astro/cf/**` 标为 lazy：管理器安装量 0.2 MiB，150 个 lazy；站点不再生成 sourcemap
- [x] 5.5 按管理器（`/settings`）的界面重做 admin（design.md A9）：`src/lib/admin-sections.ts`、`src/lib/admin.ts`（身份检查、`cfApi`、Pages 类型与状态文案），`/admin` 是分区列表，`src/pages/admin/[section].astro` 是分区页；组件有 `AdminStatus`、`AdminSection`、`AdminDenied`、`ProjectPanel`、`DeploymentsPanel`（筛选、分页、重试、回滚）、`DomainsPanel`（添加、重新验证、移除）；`CfConsole` 改成同样的卡片样式，列宽按输出区宽度计算。删除 `AdminPanel.svelte`
- [x] 5.6 `npx astro check`、svelte2tsx + `tsc` 检查全部 `.svelte`（并注入一个错误，确认能被检出）、`npx astro build`（产出 `/admin` 与四个分区页；`AdminSection` 分块 27 KiB，cf 仍在第一次执行命令时才 `import()`）

## 6. 收尾

- [x] 6.1 `npx tsc -b tsconfig.packages.json`、`npx openspec validate cf-account-admin --strict`、`npx openspec validate plugin-system --strict`（另：cf-browser 类型检查通过，146 个相关测试全部通过）
- [ ] 6.2 真实登录与 admin 操作由用户在部署后的站点上亲自验证（丢弃一次热身，之后两次结果一致）
- [x] 6.3 整理 OAuth 凭据整体设计需要考虑的问题（design.md 的 Deferred 一节）交给用户。代理防滥用转 §7，`.github/upstream-pins.json` 登记仍待决定。

## 7. 初始化后隐藏 Cloudflare 代理并快速核验管理员

管理系统已接单 #151，主责平台可行性、Function 和初始化交接。插件系统已接单 #153，负责配合平台验证及其构建与内核范围。先反馈 7.0 证据，再联调 7.1–7.2 和 `site-setup` §10。

- [ ] 7.0【管理系统主责／插件系统配合】以当前套餐用 `cf` 在隔离 Git 集成 Pages 项目确认派生子树的 `_routes.json` 精确调用、构建与 Function 可读取同一套配置、路由配置对访客不可下载，确认 WAF 的可用配额和 Function 前的拦截能力。检查自定义域、项目默认域、分支别名、现行及历史部署专属地址。记录仍可触发旧代理的地址类型、可用保护和对签名制品读取的影响。用隔离 OAuth 身份核对指定项目 GET 的结果，区分所有者、仅有只读访问的协作者和无项目访问权限的用户。若只读协作者也会得到匹配项目，先报告实际权限和结果，待明确管理员身份规则后再启用此判定。
  - #151 的隔离测试提交为 `2a74606`。管理系统确认正式项目跳过该提交，正在隔离项目构建并测量路由调用量。
- [ ] 7.1【管理系统】按 A12 实现私有路由、`cf` 入口恢复、OAuth 转发、绑定项目的只读探针和短期凭据证明。用错误路径、主机、凭据、项目、过期证明及 OAuth 正常轮询覆盖 Function 测试。保留现有同源检查和固定上游。
- [ ] 7.2【管理系统／插件系统配合】实现官方管理器的一次入口导入、内核转发和身份状态。维持逻辑 `/__cf` 登记及浏览器 cf 调用。站点构建仅启用派生子树，验证公网构建产物无路径段或密钥，无配置时展示明确的管理功能状态。核对匿名访客、第三方管理器、已登录非管理员、所有者及刷新退出。
  - #153 回报生产、预览、缺少密钥、配置无效、非 Pages 五种本地构建夹具通过。夹具替换了路由派生配置读取器，检查 `_routes.json` 的路由列表，并扫描其他产物及构建输出是否包含测试密钥或路径段。Cloudflare Pages 是否会向访客返回 `_routes.json` 由 7.0 实测。
  - #153 回报本地内核调用方回归和四项签名测试通过。调用方夹具覆盖官方管理器、未登录状态及匿名、第三方、插件页、worker UI 来源和跨源请求的拦截。双方已约定站点构建与 Function 共用 `functions/__cf/route-config.mjs` 的 `readCfRouteConfig(env)`，构建只取 `segment`。读取器实现可用后，用真实读取器复跑构建夹具。登录后的转发与入口导入联调还需要 #151 提供内核账号接口、项目探针响应及短期转发证明的约定。
- [ ] 7.3【隔离验收／管理系统】连同 `site-setup` §10 实测新旧路由及随机路径的 Function 调用数、冷启动一次指定项目查询、账户或网络状态分流、端点轮换，确认旧入口失效。自定义域在套餐支持时启用并验证边缘限流。对备用域名和历史部署记录实测结果，关闭仍可公开调用的旧代理入口。处理历史部署须保护 S13 的签名部署专属地址与正常静态访问。超出当前权限的线上变更写明范围交所有者确认。运行全仓检查、两阶段构建和本轮三个 OpenSpec 严格校验。
