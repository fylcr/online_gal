# Proposal

## 归属

用户决定把管理系统（Cloudflare 账号、`/__cf` 代理、浏览器版 cf、设置里的账号分区、`/admin`）从插件系统的工作里分出去，由单独的对话处理。本变更就是管理系统的全部范围；变更 `plugin-system` 只提供登记规则。代码边界见 design.md A11，未完成的事项见 tasks.md 的 0.1、6.2 和下文"暂缓"一节。

## Why

用户要求（按时间顺序，后者覆盖前者）：

1. 管理器设置里加"账号"，用 Cloudflare 登录；登录的账号拥有本站时，可以进入 admin 管理页。
2. 登录直接用 cf（Cloudflare 官方 CLI）自己的源码和身份，不注册第三方 OAuth 客户端。
3. 把 cf CLI 放进浏览器，直接管理 Cloudflare 的设置。cf 开源（`cloudflare/cf`，MIT OR Apache-2.0），用 TypeScript 编写。
4. 第二轮决定：
   - token 加密存在浏览器里，OAuth 账号数据放在受限的安全环境中。第三方插件也可能提供 OAuth 数据，所以 OAuth 凭据的整体存储设计**暂缓**，等其他工作完成后由我列出需要考虑的地方。
   - 申请 cf 默认的**全部**权限。
   - 直接使用 cf 的 client id：内置的就是 cf CLI，等于用 cf 管理网站；以后受限再说。
   - 代理可以看到 token；代理的隐藏与防滥用（避免被刷请求、浪费 Workers 额度）**后续设计**。
   - 先写 openspec 再动手，顺序：代理 → cf 浏览器构建 → 登录 → 设置页 → admin。

勘察得到的约束（均实测）：

- `dash.cloudflare.com/oauth2/*` 与 `api.cloudflare.com/client/v4/*` 对其他源不返回 `Access-Control-Allow-Origin`，浏览器不能直连。Service Worker 也绕不过 CORS，必须有同源代理。
- cf 默认用设备码流程登录（RFC 8628）：`device/auth` 返回 `device_code`、8 位 `user_code`、`verification_uri(_complete)`，`expires_in` 300 秒、`interval` 5 秒；不需要回调地址，适合网页。
- cf 的 `src/api/` 是纯 `fetch` 请求层；约 4000 个从 OpenAPI 生成的命令（`src/commands/_generated/`）只依赖 yargs、`#api`/`#operations` 和七个 `#lib/*` 模块。只替换这七个模块就能在浏览器里运行。手写命令（`dev`、`build`、`deploy`、本地 d1、隧道等）依赖文件系统、子进程和 workerd，不进入浏览器。
- cf 的身份（client id `cbca97e7-c331-4cdd-8fd8-e25a451b98bf`）、默认权限列表和设备码流程由 `@cloudflare/workers-auth@0.18.4` 的 `/cf` 入口提供（同样是 MIT OR Apache-2.0），被 cf `c240e41bd83591bcf001be1fee094234d154b286` 钉住。

## What Changes

- 新增同源代理 Pages Function `functions/__cf/[[path]].ts`：`/__cf/api/*` → `api.cloudflare.com/client/v4/*`，`/__cf/oauth/{device/auth,token,revoke}` → `dash.cloudflare.com/oauth2/*`，其余路径 404。`build-site.mjs` 的 `_routes.json` 加入 `/__cf/*`，保留名加入 `__cf`。
- 插件系统内核新增账号模块（`packages/plugin-system/src/account/`）：设备码登录、加密保管库、刷新、吊销、登录状态与站点管理员判定。token 只存在于内核中，页面永远拿不到；页面对 `/__cf/**` 的请求由内核加上 `Authorization` 后转发。只有官方管理器的页面能使用账号功能。
- 新增包 `packages/cf-browser`（`@online-gal/cf-browser`）：用固定 commit 的上游源码（`.ref/cf`）构建浏览器版 cf。生成的命令与 `src/api/` 原样使用，七个 `#lib/*` 换成浏览器实现；命令按顶层分组拆包、按需加载；对外提供 `runCf(argv, io)`。
- 管理器（`apps/app`）：设置新增"账号"分区（登录、账号信息、退出、进入站点管理）；新增 `/admin` 页（本站 Pages 项目信息 + cf 控制台）。控制台用 Svelte 自己写，不引入终端库。

- 初始化后启用按 Cloudflare 账户派生的私有 Function 路径。所有者首次在新浏览器使用保存的入口，经 cf OAuth 登录后，由 Function 对本站 Pages 项目发起一次只读查询。旧公网代理退出 Function 路由，自定义域可补充 WAF 限流。见 design.md A12。

## 暂缓、待用户后续设计

1. OAuth 凭据的整体存储设计：第三方插件提供的 OAuth、多个提供方、真正的隔离（同源代码能用保管库的密钥解密，见 design.md A4）。本变更只做 Cloudflare 一个提供方，接口按提供方分开存放，不预设最终形态。实现完成后整理需要考虑的问题交给用户。
2. 把 cf 的固定版本登记进 `.github/upstream-pins.json`：需要用户放开 `.github/**`。在那之前，固定版本记在 `packages/cf-browser/upstream.json`。

## Capabilities

### New Capabilities

- `cloudflare-account`：Cloudflare 登录、凭据保管、同源代理、站点管理员判定、设置里的账号分区。
- `cf-browser`：浏览器里的 cf 命令与 admin 控制台。

### Modified Capabilities

- 逻辑前缀 `/__cf` 保留。Function 仅启用该前缀下的私有子路径，登记约定由 `plugin-system` K19 维护。初始化接入由 `site-setup` S14 跟踪。

## Impact

- 新文件：`functions/__cf/[[path]].ts`，`packages/plugin-system/src/account/*`，`packages/cf-browser/**`，`apps/app/src/components/account/*`，`apps/app/src/components/admin/*`，`apps/app/src/pages/admin.astro`。
- 改动：`packages/plugin-system/src/kernel.ts`（路由）、`packages/plugin-system/scripts/build-site.mjs`（`_routes.json`、保留名、站点主机名）、`apps/app/src/lib/settings-sections.ts`、`apps/app/src/pages/settings/[section].astro`、`tsconfig.packages.json` 与 pnpm workspace。
- 依赖：`packages/cf-browser` 的开发依赖新增 `yargs@18.2.0`、`@cloudflare/workers-auth@0.18.4`（只取身份常量），版本与 cf `c240e41` 一致。前端技术栈不变。
- 本地 `.ref/cf` 检出（gitignored）由 `packages/cf-browser/scripts/fetch-upstream.mjs` 按固定 commit 稀疏克隆。
