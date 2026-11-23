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

管理系统 #151 与插件系统 #153 已交付联调基线 `7a11f45`，包括私有 Function、构建、内核、初始化交接、本地恢复和轮换。该提交位于联调分支。远端隔离预览分支头 `898d158` 另含预览夹具。集成时从 `7a11f45` 及后续修复续接。管理系统负责 `site-setup` 10.2 的审查修复及 7.2 剩余的管理页接线，随后以同一联调版本完成 7.3 和 `site-setup` 10.3。

- [ ] 7.0【管理系统主责／插件系统配合】以当前套餐用 `cf` 在隔离 Git 集成 Pages 项目确认派生子树的 `_routes.json` 精确调用、构建与 Function 可读取同一套配置、路由配置对访客不可下载，确认 WAF 的可用配额和 Function 前的拦截能力。检查自定义域、项目默认域、分支别名、现行及历史部署专属地址。记录仍可触发旧代理的地址类型、可用保护和对签名制品读取的影响。用隔离 OAuth 身份记录所有者、只读协作者和无项目权限用户的身份与项目查询结果，按 A12 核对授权。凭据未齐时先用夹具验证并记录真实验收缺口。
  - #151 的隔离测试提交为 `2a74606`，正式项目已跳过该提交。隔离 Git 部署 `f7e00f36-b0e9-47b1-9232-cedf3bb36561` 达到 `deploy/success`。#153 在该部署专属地址观察到 `/_routes.json` 返回站点 HTML，响应未包含 Function 标记或路由清单。`/registry/index.json` 仍返回有效的官方注册表。#151 回报隔离部署的精确路径命中 Function，旧路径和错误路径返回静态层响应。同一私有路径在部署专属地址到达 Function 后被主机守卫拒绝。`/_routes.json` 在默认主机和部署专属主机上均未返回清单，隔离产物扫描和构建日志未发现测试密钥或路由段。Pages Tail 会话已创建，WebSocket 握手超时。#151 随后查询平台分析数据，未捕获已知进入 Function 的对照请求，平台调用计数仍待实测。
  - Tail 连通后以隔离部署日志为准。如果握手仍超时，由有权限的成员读取隔离项目 Functions Metrics 的 `Total`。使用无背景流量的窗口分别测基线、已知会触发 Function 的对照请求、旧路径和随机路径，记录部署 ID、UTC 窗口、请求数和聚合完成后的调用数。对照请求计入 `Total` 且旧路径、随机路径窗口的增量为 0 时通过。若聚合粒度或背景流量使结果无法区分，保留待验收状态。WAF 可用时另核对规则命中与调用数。验收记录对入口和认证材料脱敏。[Cloudflare Functions Metrics 文档](https://developers.cloudflare.com/pages/functions/metrics/)
  - Cloudflare 文档显示 Pages 实时日志事件可包含请求 URL，有项目部署日志访问权限的人员可查看，平台不会存储该实时流。隔离验收只保存调用量和脱敏状态，项目上的实际日志权限与其他日志接收端仍待核对。[Cloudflare Pages 日志文档](https://developers.cloudflare.com/pages/functions/debugging-and-logging/)
  - #151 在正式区只读确认套餐为 Free，限流规则入口返回 404/10003。Cloudflare 文档列出一条限流规则的配额，按 IP 计数，当前缺少隔离自定义域名来测 Function 前拦截。签名部署地址须继续提供公开签名制品。
  - 对隔离项目默认域和 `f7e00f36` 专属地址请求旧路径，均返回 200 HTML。请求 4 个历史部署专属地址（`b2c5aaea`、`8b60adea`、`588eb8de`、`055bc3a6`）的同一路径，均返回带 Cloudflare API 版本头的 403 JSON（错误 9106/9107）。生产分支没有生成分支别名。隔离预览测试提交 `898d158` 的部署 `8a981226-d2ab-4e8b-88d1-61e790245cc3` 成功。API 返回实际预览分支别名。该别名和预览专属地址的旧路径、错误路径与 `/_routes.json` 均返回 200 HTML，官方注册表均返回有效 JSON 与 `Access-Control-Allow-Origin: *`。
  - Pages Access 的内建开关覆盖预览，旧生产部署专属地址的保护仍需验证。Cloudflare 文档提供可包含子域的 Bulk Redirects 和历史部署删除。覆盖全路径的跳转会改变 S13 验签请求的目标，删除则移除制品的专属地址。7.3 按地址核对可用保护，并保留验证所需的签名制品。[预览访问保护](https://developers.cloudflare.com/pages/configuration/preview-deployments/) · [Pages 域名重定向](https://developers.cloudflare.com/pages/how-to/redirect-to-custom-domain/)
  - 现有隔离测试只有一个已授权的账户管理员上下文。真实 OAuth 验收需取得两种额外测试身份，分别为同项目只读协作者和无项目访问用户。按 A12 记录所有者可进入管理页及两个非所有者身份被拒绝的结果。WAF 验收需要可绑定隔离 Pages 项目的测试自定义域和相应规则权限。
  - 对既有隔离签名部署 `b2c5aaea-1928-4cce-8208-da5dda323981` 携带站点 Origin 请求 `/registry/index.json` 和注册表路径下的 `plugin.json`、`mf-manifest.json`、`plugin.bundle.json`，四者均为 200 JSON 且 `Access-Control-Allow-Origin: *`。私有路由接入后需要复跑。
- [ ] 7.1【管理系统】按 A12 实现私有路由、`cf` 入口恢复、OAuth 转发、绑定所有者与项目的只读探针和短期凭据证明。用错误路径、主机、凭据、项目、可读项目的非所有者、过期证明及 OAuth 正常轮询覆盖 Function 测试。保留现有同源检查和固定上游。
  - #151 在本地联调分支提交 `00c7f41`，包含私有 Function、真实配置读取器和 `test/cf-private-relay.test.mjs`。在该工作区复跑 `node --test test/cf-private-relay.test.mjs`，5 项通过。覆盖构建与运行时的路径一致性、预览及缺少所有者绑定、路径和主机检查、用户与项目的查询顺序、凭据证明的有效期与绑定、初始化和私有入口的 OAuth 轮询。本地联调提交 `7a11f45` 加入已认证 `cf` 的旧项目补写和入口轮换。`npm run rotate:cf-entry` 默认只读核对目标账户、项目和绑定所有者，应用时将新入口保存到所有者指定的私有文件。夹具覆盖原所有者匹配、非所有者拒绝、轮换保留发布配置，以及写入结果不明时保留入口文件并停止触发部署。线上轮换验收归入 7.3。
- [ ] 7.2【管理系统／插件系统配合】实现官方管理器的一次入口导入、内核转发和身份状态。维持逻辑 `/__cf` 登记及浏览器 cf 调用。站点构建仅启用派生子树，验证公网构建产物无路径段或密钥，无配置时展示明确的管理功能状态。核对匿名访客、第三方管理器、已登录非管理员、所有者及刷新退出。
  - #153 回报生产、预览、缺少密钥、配置无效、非 Pages 五种本地构建夹具通过。夹具替换了路由派生配置读取器，检查 `_routes.json` 的路由列表，并扫描其他产物及构建输出是否包含测试密钥或路径段。Cloudflare Pages 是否会向访客返回 `_routes.json` 由 7.0 实测。
  - #153 在联调分支提交 `c924557`，接入真实读取器并生成精确的 Pages 路由。已复跑 `node --test test/plugin-system-microca.test.mjs`，4 项通过。覆盖引导和签名构建启用入口、预览和配置缺失或无效时关闭入口、公开产物和日志扫描，以及签名包验签。#153 已接入内核的入口保管和凭据证明转发。
  - 在 #153 联调工作区联合复跑 `test/cf-private-relay.test.mjs`、`test/plugin-system-cf-relay.test.mjs` 和 `test/plugin-system-microca.test.mjs`，15 项通过。覆盖所有者登录、只读协作者无管理员权限、入口加密保存和内核重启恢复、OAuth 刷新后重新核验、入口轮换、退出以及签名构建。官方管理页与真实 Cloudflare OAuth 的端到端验证归入 7.3。
  - #153 在独立工作区临时合入 #106 的内核与发布验签改动。插件系统 TypeScript 构建、内核调用方夹具，以及签名和部署专属地址验签的组合测试均通过，组合测试共五项。调用方夹具覆盖官方管理器、未登录状态及匿名、第三方、插件页、worker UI 来源和跨源请求的拦截，并确认签名包和 CA 请求仍能通过内核访问。
  - #153 的内核提交 `a76dbff` 已进入含 S13 的本地联调分支 `local/task153-private-relay-integration`（`1010b2b`）。#151 的本地联调提交 `dbdaac3` 与该提交的文件树一致。#153 回报联调版通过 `npm run check`、三个 OpenSpec 严格校验及 24 项针对性测试，`npm run build:site` 通过未签名构建分支。我方在 `1010b2b` 复跑 Function、内核、签名构建三组夹具，15 项通过。#151 的联调工作区已接通官方管理页入口导入、导入成功后清除片段及 4 种身份状态反馈。正在补浏览器夹具验收，核对 Service Worker 接管页面后能完成导入、刷新后可继续使用已保存入口、退出后需要重新登录。真实 Cloudflare OAuth 验收归入 7.3。
  - 官方管理器先将片段中的入口段导入 `POST /__kernel/api/account/cloudflare/import-entry`。请求为 `application/json`，提交 `{ "segment": "<43 个 base64url 字符>" }`，成功返回 `{ "ok": true }`。成功后清除 URL 片段，再沿用 `POST login`、`POST poll`、`GET status` 和 `POST logout`。浏览器 cf 仍请求逻辑 `/__cf/api/**`，由内核保管入口并附加短期转发证明。直接使用 `CloudflareAccount` 的初始化页在切换前等待 `setEntry(segment)`，入口仅在当前内存保留。
  - 已登录状态以 `siteAdminError` 区分 `entry-required`、`entry-invalid`、`not-admin`、`temporarily-unavailable`。其中前两种提示导入入口，第三种显示无管理权限，最后一种允许重试。上游 `session-expired` 回到未登录状态。单次转发收到 `proof-invalid` 时清除缓存证明，下一次操作重新探测。官方管理页与真实 Cloudflare OAuth 的端到端验证归入 7.3。
- [ ] 7.3【隔离验收／管理系统】连同 `site-setup` §10 实测新旧路由及随机路径的 Function 调用数、每次冷探针最多一次用户查询和一次指定项目查询、协作者拒绝、账户或网络状态分流、端点轮换，确认旧入口失效。自定义域在套餐支持时启用并验证边缘限流。对备用域名和历史部署记录实测结果，关闭仍可公开调用的旧代理入口。处理历史部署须保护 S13 的签名部署专属地址与正常静态访问。超出当前权限的线上变更写明范围交所有者确认。运行全仓检查、两阶段构建和本轮三个 OpenSpec 严格校验。
  - #151 回报隔离项目的 4 个历史部署专属地址仍可经旧路径进入中转。历史部署的关闭单独验收，即使当前部署的固定路径已停用也继续跟踪。先按部署 ID 记录构建提交、地址类型、已有调用证据，以及是否含仍需访问的签名制品或回滚版本。对保留的签名部署核对注册表、包清单和 bundle 的公开读取与验签。按地址记录可移除或可保护的入口和处置后的平台调用结果。需要删除部署时由所有者确认准确范围。
