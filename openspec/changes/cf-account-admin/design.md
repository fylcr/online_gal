# Design: cf-account-admin

## Context

插件系统（变更 `plugin-system`）把站点做成本地服务器：一个 Service Worker 内核从已安装的插件中应答本源请求，当前管理器就是站点本身。第三方管理器和官方管理器在**同一个源**下运行。官方站点部署在 Cloudflare Pages（自定义域名 `onlinegal.de5.net`，2026-11-23 起；默认地址 `onlinegal.pages.dev`），`functions/` 下的 Pages Functions 随站点一起部署（K18 的 `/__ca` 也在这里）。

用户要：设置里有账号；用 cf 自己的身份登录 Cloudflare；登录账号拥有本站时能进 admin；admin 里有浏览器版 cf，直接管理 Cloudflare。用户的二轮决定见 proposal.md。

已有约束：
- 前端技术栈固定为 Astro 7 + Svelte 5（计划接入 Tailwind v4），不加其他框架、组件库或状态库。
- 内核响应必须带 COOP `same-origin`、COEP `credentialless` 与 `nosniff`。
- 内核构建为 classic script（esbuild），Service Worker 里不能 `import()`。
- Cloudflare 的 OAuth 与 API 端点不允许其他源跨域访问（实测）。

## Goals / Non-Goals

**Goals**
- 用 cf 的 client id、cf 的默认全部权限和设备码流程登录。
- token 只在内核里出现，加密存放；页面、插件和第三方管理器都拿不到。
- 浏览器版 cf 复用上游生成的命令，不手抄 API。
- admin 入口只对拥有本站 Pages 项目的账号显示。

**Non-Goals**
- OAuth 凭据的整体设计（多提供方、第三方插件提供 OAuth、跨源隔离）：用户暂缓（proposal 第 1 项）。
- 浏览器里运行 Node 专属的手写命令（`dev`、`build`、`deploy`、本地模式、隧道、更新检查、遥测）。
- 多账号同时登录：同一时刻只保存一个 Cloudflare 登录。

## Decisions

### A1 — 登录：cf 的身份、全部默认权限、设备码流程

- client id 与默认权限列表**不手抄**，构建时从 `@cloudflare/workers-auth@0.18.4` 的 `/cf` 入口读取，生成 `packages/plugin-system/src/account/cf-identity.generated.ts`。版本与 cf `c240e41` 的 `package.json` 钉住的版本必须一致；不一致时构建失败。
  - 0.3 实测：`getClientIdFromEnv()` 返回 `cbca97e7-c331-4cdd-8fd8-e25a451b98bf`（它会读 `CLOUDFLARE_CLIENT_ID`，生成脚本运行时必须清掉这个环境变量）；`DefaultScopeKeys` 共 477 项，与 `CF_CLI.getDefaultScopeKeys()` 完全相同，已经去掉了 cf 不申请的权限，不需要再从构建产物里提取。
  - 端点不在包的导出里，按下一条固定写在代理与内核中。
- 端点：`POST https://dash.cloudflare.com/oauth2/device/auth`（`client_id`、`scope`）；`POST /oauth2/token`（`grant_type=urn:ietf:params:oauth:grant-type:device_code` 或 `refresh_token`）；`POST /oauth2/revoke`（`token_type_hint=refresh_token`）。请求体都是 `application/x-www-form-urlencoded`。
- 选设备码而不用授权码：cf 客户端登记的回调地址是本机端口，回调回不到网页；设备码流程不需要回调，cf 自己默认也用它（`useDeviceFlowByDefault: true`）。
- 轮询由页面驱动：页面每隔 `interval` 秒调一次内核的轮询 API，内核每次只发一个 token 请求。Service Worker 空闲约 30 秒就会被浏览器停止，不能指望内核自己计时。收到 `slow_down` 时把间隔加 5 秒并告知页面。

### A2 — 同源代理：`functions/__cf/[[path]].ts`

下文描述初始化阶段的固定公网路由与通用转发约束。初始化后的站点按 A12 使用私有网络入口。

- 映射：`/__cf/api/<rest>` → `https://api.cloudflare.com/client/v4/<rest>`（所有方法，保留查询串）；`/__cf/oauth/device/auth`、`/__cf/oauth/token`、`/__cf/oauth/revoke` → `https://dash.cloudflare.com/oauth2/...`（只允许 POST）；其余路径 404。
- 请求头只转发白名单：`authorization`、`content-type`、`accept`、`content-length`。`cookie`、`origin`、`referer`、`cf-connecting-ip`、`x-forwarded-*` 一律不转发。响应去掉 `set-cookie`，加 `cache-control: no-store`。请求体和响应体都以流的方式透传，不缓冲。
- 只接受浏览器标明同源的请求（`Sec-Fetch-Site: same-origin`，或 `Origin` 等于本站），否则 403。这只能挡住其他网页借道，挡不住非浏览器客户端；站点构建的防滥用见 A12。
- 不记录请求头和请求体。代理能看到 token，用户已接受。
- 选 Pages Function 而不是单独的 Worker：与 `/__ca` 同一次部署、同源（不需要 CORS），没有第二个项目要维护。
- 本地用 Wrangler 4.33.1 验证。站点产物的 `_routes.json` 同时包含 `/__ca/*` 和 `/__cf/*`。真实 `[[path]].ts` 在本地对跨站请求、OAuth 错误方法和未知路径分别返回 403、405 和 404。
- 同一次 `wrangler pages dev` 中，用模拟上游验证 `handle` 的双向流。模拟上游收到首段请求体后才允许客户端继续上传，客户端收到首段响应体后才允许模拟上游发送剩余内容。查询串得到保留，响应去掉 `set-cookie` 并设置 `cache-control: no-store`。下载首段为 5 字节时测试超时，改用 64 KiB 首段后通过。复测日志见 `.test/admin-pages-smoke/verified.log`。

### A3 — 受限安全环境就是内核

- token（access、refresh）和设备码只存在于内核的内存和加密保管库中。内核 API 返回的状态**从不**包含它们。
- 页面里的 cf 向 `/__cf/api/*` 发请求时不带 token。内核拦截 `/__cf/**`，核对调用方，补上 `Authorization: Bearer <access>`，再用自己的 `fetch` 转发到 A12 的私有网络路径（Service Worker 自己发出的请求不会再被自己拦截），最终到达 A2 的代理。页面带来的 `Authorization` 头一律丢弃。
- 调用方核对：用 `FetchEvent.clientId` 取到发起请求的客户端，再判断它的 URL。只有当前管理器的来源是 `official:` 且该客户端的路径不在 `/__plugins/` 和 `/__kernel/` 下时才放行（官方管理器页面及其创建的 worker）。第三方管理器、插件页面、取不到 clientId 的请求一律 403 `account-forbidden`。
- 内核不受控时（首次访问还没被 Service Worker 接管、或在 `astro dev` 下），页面显示「插件系统未就绪」。站点构建的公网固定路径不会进入代理。

- **例外**（2026-11-22，因变更 `site-setup` 用户决定 F）：未初始化站点的初始化构建里没有内核。初始化页自己构造 `CloudflareAccount`（内存 `VaultStore`，不落盘），OAuth token 只在页面内存里，请求直接经 `/__cf` 中转。只存在于初始化构建，站点构建不含这段代码。见 `site-setup` design.md S5。

### A4 — 加密保管库

- IndexedDB 数据库 `online-gal-vault`（版本 1）有两个对象仓库：`keys` 存一把不可导出的 AES-GCM-256 `CryptoKey`（`generateKey(..., false, ['encrypt','decrypt'])`，键 `vault-key-v1`）；`credentials` 按提供方存 `{ provider, v: 1, iv, ct }`。iv 为 12 字节随机数，每次写入都换新；附加认证数据是 `online-gal-vault/v1/<provider>`。
- 明文为 `{ accessToken, refreshToken, expiresAt, scopes, user: { id, email } | null, obtainedAt }`。进行中的设备码登录 `{ deviceCode, expiresAt, interval }` 也加密存放（键为 `<provider>#pending`），这样 Service Worker 被停止后可以继续轮询。
- 用 IndexedDB 而不用 Cache Storage（`plugin-system` K4 的例外）：`CryptoKey` 只能通过结构化克隆保存，Cache Storage 存不了。
- **已知限制**：加密只保护静态数据，防止直接复制浏览器配置文件或从磁盘读出明文 token。同源的任何脚本都能打开这个数据库，用这把不可导出的密钥解密，所以它挡不住第三方管理器里的恶意代码。A3 的调用方核对只拦截"通过内核取用"这条路，拦不住直接读 IndexedDB。真正的隔离需要单独的源，属于用户暂缓的整体设计。
- 保管库按提供方存放，接口是 `get(provider)`、`put(provider, value)`、`delete(provider)`。底层存储抽象为 `VaultStore`，IndexedDB 实现用于生产，内存实现用于 `.test/`。

### A5 — 刷新、失效与退出

- 转发前如果 `expiresAt - now < 60s`，先用 refresh token 刷新。同一时刻只发一个刷新请求，其他请求等待它的结果。
- 刷新返回 `invalid_grant` 或 `invalid_client` 时清除凭据，转为未登录，原请求得到 401 `account-signed-out`。网络错误不清除凭据。
- 退出：先尽力吊销 refresh token（失败也继续），然后**一定**清除凭据和进行中的登录。
- 登录、刷新、退出都会清空 A6 的缓存。

### A6 — 站点管理员判定

下文的账号及项目枚举用于初始化时发现本站项目。所有者绑定按 S14 执行。初始化后的后台通过 A12 核验同一 Cloudflare 用户与指定项目。

- 内核列出账号（`GET /accounts`，分页），对每个账号列出 Pages 项目（`GET /accounts/{id}/pages/projects`，分页），把当前主机名（小写）与项目的 `subdomain` 和 `domains[]` 比对。命中的第一个为 `{ accountId, accountName, projectName }`，否则为 `null`。
- 在 `localhost`、`127.0.0.1`、`[::1]` 上比对构建时的站点主机名 `SITE_HOST`（`build-site.mjs` 写入内核，默认 `onlinegal.de5.net`，即站点的自定义域名；2026-11-23 前为 `onlinegal.cc.cd`），方便本地调试 admin。
- 结果在内核内存中缓存 10 分钟，A5 的事件会清空它。查询失败（网络或 403）时结果为 `null` 并附原因，不缓存。
- 这只是界面上的门槛，不授予任何内核特权。真正的权限由 Cloudflare 按 token 校验；admin 能做的事，登录用户用 cf 本来也能做。

### A7 — 浏览器版 cf：`packages/cf-browser`

- 上游：`cloudflare/cf` 的 `c240e41bd83591bcf001be1fee094234d154b286`，只取 `packages/cli`。`scripts/fetch-upstream.mjs` 稀疏克隆到 `.ref/cf` 并核对 commit。固定版本记在 `packages/cf-browser/upstream.json`，等用户放开 `.github/**` 后再登记进 `.github/upstream-pins.json`，并作为 `engines` 的同级记录：`engines` 下的每个 key 都会被定时漂移检查拿去触发 workflow。
- 原样复用：`src/api/**`、`src/commands/_generated/**`（含 `_operations`）。
- 替换（0.2 实测后定下）：`packages/cf-browser/src/lib/` 有浏览器实现的 `#lib/*` 优先，其余用上游原文件。替换的是 `auth`、`local`、`output`、`progress`、`prompt`、`raw-fetch`、`telemetry/index`、`telemetry/run`、`ui/theme`；原样复用的是 `body-parser`、`body-unions`、`cli-exit`、`dry-run`、`global-options`、`hand-written-dry-run`、`input-validation`、`lazy-command`、`resolve`、`schema-flags`、`ui/sanitize`。上游 lib 内部的相对引用也按同一规则改指向替换实现。esbuild 解析插件另外把 `#api`、`#generated`、`#operations/*` 指向上游。
- 生成的分组会挂载少量手写命令（`#commands/*`，例如 `pages deploy`）。它们需要文件系统、子进程或 workerd，构建时一律换成隐藏的占位命令，执行时报"cf <命令> needs a local environment and is not available in the browser"。全局选项里只在本机有意义的 `--local`、`--persist-to`、`--profile`、`--mode`、`--temporary-account`、`--event-code` 隐藏并拒绝。
- 构建闸门：打包图里出现 Node 内置模块（`node:*` 或裸名 `fs`、`os` 等）、或白名单外的包时构建失败，并报出引入它的模块。白名单：`yargs` 及其浏览器依赖（`cliui`、`yargs-parser`、`string-width`、`strip-ansi`、`wrap-ansi`、`ansi-regex`、`ansi-styles`、`get-east-asian-width`、`emoji-regex`）、上游源码、`packages/cf-browser/src`。唯一的例外：上游的 `body-parser`、`input-validation`、`schema-flags` 引用了 `node:fs`/`node:path`/`node:util` 的个别函数，这三处改指向 `src/node-standins.ts`（读文件直接报错、`resolve` 只取文件名、结构相等比较），其他模块引用这些内置模块仍然失败。
- 产物：ESM，入口 `runCf(argv, io)`；命令按上游的 `lazyCommand` 拆成 chunk，用到时才加载。0.2 实测：145 个 chunk 共约 7.5 MiB，入口约 125 KiB（yargs 与分组索引），构建约 3 秒。管理器页面里可以 `import()`，限制只在 Service Worker 里。
- 一次只执行一条命令（上游的 yargs 实例与 `process.exitCode` 风格的状态都是全局的）；并发调用直接报错。
- `auth` 分组在浏览器里重写：`login`、`logout`、`whoami` 调用内核的账号 API。
- 缺省账号与区域：生成命令的 `zones create`、`zones list` 已经把 `--account-id` 用作自己的参数，不能再加同名全局选项。所以账号改用控制台会话变量 `CLOUDFLARE_ACCOUNT_ID`（区域用 `CLOUDFLARE_ZONE_ID`），与 cf CLI 认的环境变量同名，在控制台用 `export NAME=value` 设置，只在本次会话有效。没有设置时依次取站点管理员判定的账号、唯一账号；都没有就报错，列出可见账号并提示 `export CLOUDFLARE_ACCOUNT_ID=<id>`。
- 类型检查：`node packages/cf-browser/scripts/typecheck.mjs`。上游 lib 的类型引用会带进一些为 Node 写的上游模块，它们的诊断不归我们修，脚本只让本包的错误导致失败。因为依赖 `.ref/cf` 检出，本包不加入 `tsconfig.packages.json`。
- 遥测替换为空实现，不向任何地方发送数据。
- 许可：上游的 `LICENSE-MIT` 与 `LICENSE-APACHE` 以 `.txt` 随产物分发，chunk 头部保留版权声明。应用构建（Vite 8 / rolldown）重新打包时，声明挂在文件开头的 import 上，随 import 改写一起消失；所以应用用 `?url&no-inline` 把两份许可作为独立文件输出，控制台常驻版权声明并链接过去。上游没有 NOTICE 文件。MIT 与 Apache-2.0 都兼容本项目的 GPL-3.0-or-later。

### A8 — admin 控制台

- `CfConsole.svelte`：输入行、命令历史（↑↓，存在 `sessionStorage`，不落盘）、输出区（文本，或格式化的 JSON）。不引入 xterm.js。
- 命令行由自写的小型分词器解析（单双引号、反斜杠转义），开头的 `cf` 可省略。
- 执行中可以取消：`AbortController` 一直传到 `fetch`。
- 上游需要交互确认的地方（例如删除），由 `io.confirm` 在控制台内联显示"确认 / 取消"。

### A9 — 设置"账号"分区与 admin 页

- `SETTINGS_SECTIONS` 新增 `{ id: 'account', title: '账号', description: 'Cloudflare 登录与站点管理' }`，分区页渲染 `AccountPanel.svelte`。
- 未登录：按钮"使用 Cloudflare 登录" → 显示验证码、"打开验证页"链接（新标签打开 `verification_uri_complete`）和倒计时，并按间隔轮询；过期后可以重新开始。
- 已登录：显示邮箱与用户 id、权限数量、到期时间，以及"退出登录"；站点管理员额外显示"进入站点管理"。
- `/admin` 的结构和界面与 `/settings` 一致：分区由 `apps/app/src/lib/admin-sections.ts` 的 `ADMIN_SECTIONS` 定义，`/admin`（`apps/app/src/pages/admin.astro`）顶部是账号与项目卡片，下面是分区列表；每个分区是 `/admin/<id>`（`apps/app/src/pages/admin/[section].astro`），标题统一为"管理"，面包屑从 `/admin` 开始。卡片、列表、按钮与设置页用同一套 daisyUI 样式（`rounded-2xl bg-base-200`、`divide-y` 列表、`max-w-2xl`）。
- 每个页面都要先确认管理员身份；不是站点管理员时说明原因，并给出账号设置的链接。分区：
  - 项目（`ProjectPanel`）：项目名、账号、生产分支、来源仓库与构建配置、当前生产部署。
  - 部署（`DeploymentsPanel`）：按全部、生产或预览筛选，分页显示最近的部署。失败或已取消的部署可以重试；生产环境可以回滚到更早的、成功的生产部署（当前生产部署除外）。
  - 域名（`DomainsPanel`）：默认的 `*.pages.dev`，以及各自定义域名的验证状态和错误信息。可以添加域名、重新验证未生效的域名、移除域名；移除当前访问的域名时会额外警告。
  - 控制台（`CfConsole`）：A8。
- 所有写操作（重试、回滚、移除域名）都要在所在行内二次确认；失败的原因显示在该行，不影响其他行。这些操作都走 `/__cf/api/**`，权限仍由 Cloudflare 按 token 校验。
- 内核不受控、或当前是第三方管理器时（API 返回 403 `account-forbidden`），两处都显示明确的不可用原因。

### A10 — 与插件系统的衔接：登记 `/__cf`

- 本变更不修改 `plugin-system` 的条款，只按它的登记规则（plugin-system design.md K19，spec 的 "Other changes extend the kernel by registration"）登记前缀 `/__cf`：
  - 内核模块：A3 的中转，接管 `/__cf/**` 的所有方法，页面只能用 `/__cf/api/**`；
  - 内核 API 名字空间 `account/`：`/__kernel/api/account/<provider>/{status,login,poll,logout}`，status 用 GET，其余用 POST；
  - Function：A2 的 `functions/__cf/[[path]].ts`。
- 登记带来的保留：管理器包不能含 `__cf/**`，`build-site.mjs` 的 RESERVED 含 `__cf`，`_routes.json` 保留 `/__ca/*`，按 A12 仅登记 `/__cf` 的实际私有子路径。站点声明的逻辑 `/__cf/**` 由内核模块应答。
- 需要插件系统提供登记规则之外的东西时，在 `plugin-system` 变更里提，不在这里直接改它的 spec。
- 账号模块打进 `sw.js`。它只依赖 WebCrypto、IndexedDB 和 `fetch`，记下 `sw.js` 增加的体积。

### A11 — 归属与代码边界

管理系统（本变更的全部内容）由单独的对话负责，插件系统的工作只维护登记规则。两边的改动都还没有提交，拆分提交时用显式 pathspec，共用文件按下面的块分开。

- 只属于本变更的文件：
  - `functions/__cf/`
  - `packages/cf-browser/**`
  - `packages/plugin-system/src/account/**`
  - `apps/app/src/components/account/**`、`apps/app/src/components/admin/**`
  - `apps/app/src/lib/` 下的 `account.ts`、`admin.ts`、`admin-sections.ts`、`cf-console.ts`
  - `apps/app/src/pages/admin.astro`、`apps/app/src/pages/admin/**`
- 与 `plugin-system` 共用的文件中，属于本变更的部分：
  - `packages/plugin-system/src/kernel.ts`：`KernelOptions.vault`，`routeRequest` 的 `kind: 'cloudflare'` 分支，`api()` 里的 `account/` 分派，以及 `account()`、`assertAccountCaller()`、`accountApi()`、`relayCloudflare()`。
  - `packages/plugin-system/src/errors.ts` 的 `account-forbidden`、`account-signed-out`、`account-unavailable`、`account-failed` 及其 HTTP 状态。
  - `packages/plugin-system/src/index.ts` 对 `account/vault.js`、`account/cloudflare.js` 的导出；`packages/plugin-system/src/sw.ts` 的 `IndexedDbVaultStore` 与 `__SITE_HOST__`。
  - `packages/plugin-system/src/format.ts` 保留路径里的 `__cf`。
  - `packages/plugin-system/scripts/build-site.mjs`：`__SITE_HOST__`（默认 `onlinegal.de5.net`）、官方管理器 lazy 列表里的 `_astro/cf/**`、RESERVED 里的 `__cf`、`_routes.json` 的 `/__cf/*`。
- 其他共用文件：
  - 根 `package.json` 的 `build:cf`，以及 `build`、`dev` 对它的调用；`pnpm-lock.yaml` 里 `packages/cf-browser` 的 importer。
  - `apps/app/package.json` 的 `@online-gal/cf-browser`；`apps/app/astro.config.mjs` 的 cf 分块规则（`chunkFileNames`）。
  - `apps/app/src/lib/settings-sections.ts` 的 `account` 分区，`apps/app/src/pages/settings/[section].astro` 的 `AccountPanel`。这两处属于前端，改动时同时遵守 `redesign-frontend`。
- 关闭 sourcemap（`apps/app/astro.config.mjs` 与 `build-site.mjs`）是为本变更做的决定，但作用于整个站点；要改回来时两边一起商量。

### A12 — 初始化后的私有 Cloudflare 入口和后台身份核验

1. **派生入口。** 初始化确认所有者、目标 Pages 项目和正式主机名后，生成 32 字节随机 Pages secret `ONLINE_GAL_CF_ROUTE_KEY`。使用 HMAC-SHA-256 对有版本的 UTF-8 编码元组 `["online-gal/cf-route/v1", accountId, projectName, canonicalHost]` 派生无填充 base64url 路径段。网络入口为 `/__cf/<路由段>/*`，只接受 `GET .../site-admin`、A1 的 OAuth POST 和 `.../api/**`。元组用 JSON 数组编码，主机及账户 ID 使用校验后的规范值。生产构建和 Function 使用同一配置。密钥独立于 Cloudflare OAuth、CA 和构建密钥。有效路由只出现在服务器端路由配置和所有者保存的入口。
2. **入口交接。** 初始化向所有者提供一次可保存的 `/admin#cf-entry=<路由段>` 链接。导入官方管理器后用 `replaceState` 清除片段，内核关联到本站凭据保管库，之后可直接访问 `/admin`。新浏览器需导入入口一次，随后仍走原 cf OAuth 设备码登录。站点首页、公开设置和公开构建产物无入口索引。摘除所有应用日志、错误、遥测中的完整路由和认证材料。Cloudflare 若记录路径，记录的访问权限和保留期纳入 7.0 核对。所有者可用已认证的本地 `cf` 流程更换路由密钥并获得新入口，应对遗失或泄露。
3. **快速核验。** 所有者是完成 S11 初始化并按 S14 绑定的 Cloudflare 用户。`ONLINE_GAL_CF_OWNER_ID` 是生产 Pages `secret_text`，供 Function 校验。缺失或无效时按缺少私有配置处理。管理器发出一次 `GET .../site-admin`，Function 用当前 OAuth access token 调固定 `GET /user`，校验 Cloudflare 成功结果的 `result.id` 与所有者绑定。不匹配时返回 `403 not-admin`，结束探针。匹配后用同一 token 调 `GET /accounts/{accountId}/pages/projects/{projectName}`，核对项目与正式主机，两个检查通过后才签发证明。冷探针各最多调用一次，避免枚举账号或项目。对无权访问、登录过期、入口失效和暂不可用分别反馈。缓存沿用 A5/A6，最多 10 分钟，临时失败可重试。实际操作继续由 Cloudflare 按当前 token 授权。
4. **限制转发。** 私有入口下保留 A1 的三个 OAuth POST 端点用于登录、刷新和吊销。通用 API 代理要求通过探针后签发的短期证明。证明最多有效 10 分钟，绑定当前入口、主机和 OAuth token 的摘要，仅存在内核或本次初始化会话内。可从路由密钥按独立用途派生签名材料。Function 在向上游发请求前检查证明，拒绝缺失、已过期、换 token 或跨入口的证明。内核仍执行 A3 的调用方检查，浏览器 cf 继续使用逻辑 `/__cf/api/**`。认证和验证材料均经 HTTPS 传递。
5. **防刷与验收。** `_routes.json` 精确接入该账户派生的子树，老的固定公网路径和未知路由段交由静态层处理。预览、其他主机和公开产物均不得暴露生产入口。Function 同时校验预期主机和完整路由。隔离部署中通过可观测的调用量核对旧路径和随机路径没有触发 Function。当前自定义域名支持时，对 CF 中转路径启用 WAF 限流并核对拦截发生于 Function 执行前，阈值容纳设备码轮询和正常后台操作。单独排查项目默认域、分支别名和历史部署专属地址。仍能运行旧公开代理的历史部署须在生产验收前关闭公开入口。任何别名保护须验证签名部署专属地址的验签读取仍可用。

## Risks / Trade-offs

- **借用 cf 的 client id**：授权页显示的是 cf。Cloudflare 可能限制非 CLI 的用法，用户已接受，受限后再说。client id 来自 A1 的构建期读取，只改一处。
- **全部权限意味着高价值 token**：同源代码可以解密（A4 的限制）。在整体设计完成前，管理器以外的代码都是潜在风险。
- **代理被借道**：A2 的同源检查挡不住非浏览器客户端，可能被刷请求、消耗 Workers 额度。A12 用精确路由减少公网调用，并在可用时加边缘限流。
- **Service Worker 寿命**：登录轮询由页面驱动，进行中的设备码加密存放；SW 重启后从保管库恢复。
- **产物体积**：约 4000 个命令按分组拆包；控制台首屏只加载 yargs 和分组索引。实测：cf-browser 145 个 chunk 共 7.5 MiB，入口 125 KiB；`/admin` 首屏不加载 cf，第一次执行命令时才 `import()`；只含 cf 模块的分块由 `apps/app/astro.config.mjs` 的客户端 `chunkFileNames` 放进 `_astro/cf/`，官方管理器把 `_astro/cf/**` 标为 lazy，安装时只下载约 0.2 MiB（未分目录时安装要下载 7.7 MiB）；`sw.js` 因账号模块增加 20.9 KiB（共 59.2 KiB）。站点产物原有约 27 MiB 的 `.map`，大头是 cf 的大分组（如 zero-trust 的 map 1.9 MB）；按用户决定，app 与内核 `sw.js` 都不再生成 sourcemap。
- **上游变动**：cf 还是预发布版（钉住的提交是 1.0.0-rc.1）。钉住 commit，升级时重跑构建闸门和测试。固定版本暂存 `packages/cf-browser/upstream.json`，不登记进 `.github/upstream-pins.json`：用户决定之后统一写一个每天触发的工作流，把各上游一起更新。
- **API 速率**：A6 每次判定要列出账号和项目，缓存 10 分钟以免触发 Cloudflare 的速率限制（每 5 分钟 1200 次）。

## Deferred: OAuth 凭据整体设计（待用户）

用户要求：其他工作完成后，由我列出需要考虑的地方。以下是本变更实现后整理的待决问题，已交给用户，等待设定。

1. **加密能防什么**：A4 的密钥不可导出，但与密文同存于本源的 IndexedDB。拷走浏览器配置文件拿不到明文；本源的任何脚本（第三方管理器、同源插件页、XSS）都能拿到密钥句柄直接解密。要真正隔离，只有两条路：
   - 把保管库搬到独立的源（例如单独的子域），只通过 `postMessage` 交互；
   - 密钥不落盘，每次会话用口令或 WebAuthn PRF 解锁。
   需要决定威胁模型。
2. **第三方提供者的接入方式**：谁可以声明提供者（只有官方插件，还是任何插件通过清单字段加授权）。声明里至少要有：
   - 接口地址、client id、权限范围；
   - 流程类型：设备码流程不需要回调；PKCE 重定向需要本源的回调路由，例如 `/__kernel/oauth/callback/<provider>`；
   - token 允许发往的上游主机白名单。否则恶意声明可以把别的提供者的 token 带到自己的主机。
3. **服务端中转是否通用化**：很多 OAuth 接口不允许跨域，和 `/__cf` 一样需要服务端中转。通用中转等于开放代理，存在 SSRF 和滥用风险，只能按提供者在构建期或官方注册表里固定主机。第三方提供者的接口是否允许走本站中转，需要决定。
4. **插件怎么用 token**：建议插件永远拿不到明文，只能请内核代发请求，由内核加上 Authorization 并按主机白名单放行（现在的 `/__cf` 就是这样）。需要定下来的还有：
   - 授权名，例如 plugin-host 的 `account:<provider>`；
   - 按插件弹出的同意界面；
   - 一个插件能否使用另一个插件提供的账号。
5. **账号模型**：现在每个提供者只有一个账号。需要决定：
   - 是否支持多账号与切换；
   - 账号是否归属于提供它的插件。
6. **生命周期**：哪些事件要清除或吊销凭据，待定的有：
   - 卸载或禁用提供者插件；
   - 切换到第三方管理器；
   - 内核进入受限模式（DoH 不可达）；
   - 长时间未使用。
   另外两项也要定：是否需要空闲自动上锁；危险操作（如控制台里的删除）是否要求再次确认身份。
7. **格式与升级**：保管库的 AAD 带版本（`online-gal-vault/v1/<provider>`）。需要定：
   - 内核升级时怎么迁移；
   - 密钥是否定期轮换；
   - 旧格式解密失败时一律删除（现行做法）还是尝试迁移。
8. **存储会被浏览器清掉**：Safari 的 ITP 可能在 7 天无交互后清空 IndexedDB，无痕模式关闭即清。需要定：
   - 是否申请 `navigator.storage.persist()`；
   - 凭据丢失后的提示。
9. **审计**：是否记录哪个页面或插件在何时用哪个账号调用了什么接口，并在账号设置里展示。

## Verification record

- `8656bfb` 将 `cf-account-admin` 随 `site-setup` 推送到 GitHub `main`；Cloudflare Pages 同一提交部署成功。真实账号登录和 Admin 操作仍由用户按任务 6.2 验收。

## Migration Plan

全部是新增功能，没有数据迁移。保管库数据库首次使用时创建。回滚时删除这些新文件和路由，再去掉 `_routes.json` 里的 `/__cf/*` 即可；已有的保管库数据库会成为无人读取的数据，下次使用时不受影响。
