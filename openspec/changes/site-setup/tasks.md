# Tasks: site-setup

设计细节见 design.md，用户决定见 proposal.md。

- **负责**：管理系统的对话（`cf-account-admin` 的对话）。
- **配合**：`plugin-system` tasks.md 11.3（撤销内核对 `/__setup` 的放行）与 10.10（发布工作流用 `npm run build:pages`），由插件系统的对话完成。
- **当前要做**：第 9 节（决定 I，方案 A）与第 10 节（私有 Cloudflare 入口）。第 8 节已完成。9.5 与 10.3 隔离验收通过后，由用户执行 7.6，再记录 9.6 生产验收。
- **完成标准**：初始化构建只包含初始化页及其接口。站点构建产物与初始化代码隔离。官方站点按 S13 完成引导部署、签名部署及 DoH 验收。

## 历史：上一版已完成的工作（v2，按用户决定 F 部分作废）

- [x] H.1 `functions/__setup/[[path]].ts` 的 `status`，以及 `verify` 的实现。→ 代码移到 `apps/setup/server/setup.ts` 继续使用（2.1）。
- [x] H.2 在 `format.ts`、`build-site.mjs` 登记 `__setup`。→ 作废，在 3.2 删除。
- [x] H.3 `.test/site-setup.test.ts` 的 `status` 用例与门槛用例。→ `status` 用例保留并改路径；门槛用例删除。
- [x] H.4 上一版用 `wrangler pages dev` 实测 `status`，历史结果见 design.md S4。
- [x] H.5 门槛 `site-gate.js` 的测试。→ 作废，门槛已取消。

## 0. 可行性验证（需要用户的 Cloudflare 账号，用测试项目，不要用生产项目）

- [x] 0.1 建一个 **Git 集成**的测试 Pages 项目，构建命令 `npm run build:pages`，输出 `dist/site`。确认：
  1. 没有 `ONLINE_GAL_SITE` 时构建日志选了 `setup`；用 API 写入后，create deployment（`branch=<生产分支>`）触发的新构建选了 `site`。
  2. 输出目录有 `_worker.js` 时，`functions/` 被忽略，`_routes.json` 生效，未知路径返回根 `index.html`。
  3. API PATCH 写 `env_vars` 是合并：之前的无关变量还在；`GET` 项目时 secret 只有键名；预览环境没有 secret。
  - 结果见 design.md 的 Verification record。隔离 Git 项目在同一提交上运行 `npm run build:pages`，先后部署 `setup` 和 `site`。

## 1. 构建脚本

- [x] 1.1 新建 `scripts/build-pages.mjs`（S2）：判断阶段、清空 `dist/site/`、调用对应阶段、写 `GITHUB_OUTPUT`、日志不含变量值。
- [x] 1.2 站点阶段：运行 `npm run build:site`，再做 S6 的检查。
- [x] 1.3 初始化阶段：调用 `apps/setup/scripts/build-setup.mjs`（S3）：`astro build`（带 `PUBLIC_ONLINE_GAL_SETUP` 配置）→ 复制到 `dist/site/` → esbuild 打包 `_worker.js` → 写 `sw.js`、`_routes.json`、`_headers` → S6 的初始化阶段检查。
- [x] 1.4 根 `package.json` 加 `build:pages`、`build:setup`；`check` 加上 `@online-gal/setup` 的 `astro check`。
- [x] 1.5 测试（`.test/site-setup.test.ts`）：阶段判断的五条规则（API 用假 `fetch`）、`rebuild` 配置的三种取值、S6 两个阶段的检查各有一个失败用例。

## 2. `apps/setup`

- [x] 2.1 建 workspace `apps/setup`（S5 的文件和依赖）。把 `functions/__setup/[[path]].ts` 移成 `apps/setup/server/setup.ts`，`status` 加 `phase: "setup"`；写 `apps/setup/server/worker.ts`（S3）。
- [x] 2.2 `apps/setup/src/lib/memory-vault.ts` 和 `setup.ts`：用 `CloudflareAccount` 实现 S5 的第 1–8 步（登录、所有者、读项目、校验、写入、三种重新构建、确认切换、中断恢复、本机预演）。可以从 `apps/app/src/lib/site-setup.ts` 和 `SetupFlow.svelte` 搬运逻辑。
- [x] 2.3 `apps/setup/src/lib/setup-items.ts`（S7 第一批）、`src/pages/index.astro`、`src/components/SetupFlow.svelte`。
- [x] 2.4 确认页面产物不含内核代码；如果从包入口导入会带进来，在 `packages/plugin-system/package.json` 加 `./account` 子路径导出（S5）。
- [x] 2.5 测试：`status`/`verify` 用例改到新路径；初始化页逻辑用假 `fetch` 覆盖：非所有者、已写入直接构建、写入请求体、`pages-git` 触发与轮询、`github-rerun` 成功与 403 退回手动、切换检测、本机预演不发写请求、token 不进任何存储。

## 3. 清理上一版

- [x] 3.1 删除 `functions/__setup/`、`apps/app/src/pages/setup.astro`、`apps/app/src/components/setup/`、`apps/app/src/lib/site-gate.js`、`site-setup.ts`、`site-setup-items.ts`，去掉 `Layout.astro` 里的 `<SiteGate />` 和 import。
- [x] 3.2 删除 `packages/plugin-system/src/format.ts` 保留路径正则和 `scripts/build-site.mjs` 的 `RESERVED`、`_routes.json` 里的 `__setup`。
- [x] 3.3 确认 `plugin-system` 11.3 已完成（`kernel.ts` 里没有 `__setup`）。

## 4. 验收

- [x] 4.1 `npm run build:setup`。按用户要求使用 `cf` CLI：在本地以生成的 Worker 和静态目录做浏览器冒烟，用 `cf` 在隔离 Pages 项目确认远端路由。验收：任意页面路径显示初始化页；`/__setup/status` 返回 `phase: "setup"`；`/__cf/**` 可用；没有插件注册表。
- [ ] 4.2 `npm run build:pages -- --phase=site`：检查通过，`dist/site/` 搜不到 `__setup`；`npm run check`、`npx tsc -b tsconfig.packages.json`、`npx openspec validate site-setup --strict` 通过，现有 plugin 与账号测试没有回归。
  - 独立发布工作树用冻结锁文件安装依赖，`npm run check`、两阶段构建、15 项初始化与账号测试、OpenSpec 严格校验通过。旧插件工作台测试在本工作区未找到，保留该项待核对。
- [x] 4.3【用户】上线并亲自初始化（用户决定 E）。2026-11-23 用户确认已完成，第 5、6 步用 `cf` CLI 核对通过。依赖 `plugin-system` 10.10 提供符合 `cf` CLI 要求的正式发布方式，并发布带来源证明的站点产物。
  1. [x] 把 `onlinegal.cc.cd` 绑定为 Pages 项目的自定义域名。2026-11-23 已绑定，域名状态 active，Cloudflare 自动建了 CNAME `onlinegal.cc.cd → online-gal.pages.dev`（代理）。
  2. ~~准备 Cloudflare API token 和 GitHub token~~ 作废（决定 G）：不需要准备任何 token。
  3. [x] 发布一次，应得到初始化构建。生产部署 786e2585 已是初始化构建。注意：此时是 Pages 的 Git 集成直接构建，没有经过 plugin-system 10.10 的正式发布工作流和来源证明；10.10 完成后正式站点构建改走该流程。
  4. [x] 2026-11-23 用户确认已完成：打开 `https://onlinegal.cc.cd/`，登录 Cloudflare 并同意全部权限，确认写入，第二次构建完成后回到原页面。
  5. [x] 2026-11-23 用 `cf pages projects get online-gal` 核对（`cf` CLI v1.0.0-beta.5，已登录）：生产环境是 `CLOUDFLARE_ACCOUNT_ID`（secret_text）与 `ONLINE_GAL_SITE`（plain_text），预览环境只有 `ONLINE_GAL_SITE`；没有 `CLOUDFLARE_API_TOKEN`、`GITHUB_TOKEN`。
  6. [x] 2026-11-23 核对：`curl https://onlinegal.cc.cd/__setup/status` 返回 `200 text/html`，不是 `phase: "setup"` 的 JSON；站点根是游戏库。

## 5. 配置 Pages 构建设置

- [x] 5.1 实现 `npm run configure:pages -- <project>`（S10）：仅使用固定版 `cf`，确认 Git 集成，写入并核对构建命令、根目录、输出目录；支持预览、幂等和显式触发生产部署。
- [x] 5.2 `test/configure-pages.test.mjs` 验证写入范围、无需更新、预览、拒绝非 Git 项目、读回失败时不部署、使用原生产分支；真实 `cf pages edit --dry-run` 核对请求体。
- [x] 5.3 在新 Pages 项目确认配置及部署。项目 `online-gal`（GitHub `fylcr/online_gal`，生产分支 `main`）。原构建命令被手填成 `npm run  build:page`，部署 634044a0 因 `Missing script: "build:page"` 失败。2026-11-23 运行 `npm run configure:pages -- online-gal --deploy` 改为 `npm run build:pages` / 根目录 / `dist/site`，部署 786e2585 成功，日志 `[build:pages] setup (规则 3)`。在 786e2585.online-gal.pages.dev 上核对过：任意路径返回初始化页，`/__setup/status` 返回 `{"phase":"setup","initialized":false}`，`/__cf/**` 能转发到 Cloudflare API。同日在 `https://onlinegal.cc.cd` 上复核，页面与部署地址字节一致，证书已签发（Google Trust Services WE1，至 2027-02-20），http 301 到 https。注意：`/__cf/api/user` 的裸请求返回 `403 Forbidden` 是 `functions/__cf/[[path]].ts` 同源守卫的设计行为，带 `Sec-Fetch-Site: same-origin` 才会转发到 Cloudflare API。

## 6. 决定 G：初始化只用 Cloudflare OAuth

负责：管理系统的对话。依据 design.md S5 第 2、4、6、7 步与 S7，spec 的 "Only the site owner initializes"、"Setup is completed with OAuth only"、"Writing triggers a new build" 三条。不改 `packages/plugin-system/src/account/cloudflare.ts` 的登录代码，它已经申请 `CF_DEFAULT_SCOPES`（477 项）加 `offline_access`，并把 token 响应的 `scope` 存进凭据的 `scopes`（`cloudflare.ts:219-224`、`:329-335`）。

- [x] 6.1 `apps/setup/src/lib/setup-items.ts`：第一批只留 `CLOUDFLARE_ACCOUNT_ID`（`source: 'account'`），删 `CLOUDFLARE_API_TOKEN` 与 `GITHUB_TOKEN`。`SetupItem` 类型里的 `source: 'input'` 和 `verify` 保留不删。
- [x] 6.2 `apps/setup/src/lib/setup.ts`：
  1. 登录后检查凭据的 `scopes`：含 `pages:read` 和 `pages:write` 才继续；缺哪项就停下说明并提供"退出并更换账号"。`scopes` 为空数组（响应没带 `scope`）时不拦截，靠后续请求的报错。
  2. 删除填写和校验步骤，不再调用 `POST /__setup/verify`；清单里出现 `source: 'input'` 的项时停下提示"该项需手动填写，初始化页暂不支持"，不写入。
  3. PATCH 请求体只含 `ONLINE_GAL_SITE`（生产 + 预览，`plain_text`）和 `CLOUDFLARE_ACCOUNT_ID`（仅生产，`secret_text`）。任何 OAuth token 都 MUST NOT 进请求体。
  4. `github-rerun` 不调 GitHub API，只显示 `https://github.com/{repo}/actions/runs/{runId}` 的手动重跑入口和"推一个新的 `v*` 标签"，然后照常轮询部署。`pages-git` 与 `manual` 不变。
  5. 删除写入后"清空内存 token"和"GitHub token 保留到第 7 步"的逻辑；OAuth 凭据留在 `CloudflareAccount` 内存里供第 7 步使用。
- [x] 6.3 `apps/setup/src/components/SetupFlow.svelte`：去掉 token 输入框、"创建 token"链接和"校验"按钮；第 4 步改为只读清单（账号 id 打码）加"写入并构建"按钮；补上缺少权限时的提示和 `github-rerun` 的手动入口。
- [x] 6.4 `apps/setup/server/setup.ts` 的 `verify` 不删，在代码注释里写明"当前无使用者（site-setup 决定 G）"。
- [x] 6.5 测试（`.test/site-setup.test.ts` 及初始化页逻辑测试）：删除校验相关与 `github-rerun` 成功/403 的用例；新增：PATCH 请求体只有两个键且不含任何 token、缺 `pages:write` 时不发 PATCH、`scopes` 为空时不拦截、`github-rerun` 不发出任何到 `api.github.com` 的请求、清单含 `input` 项时停下。保留 token 不进任何浏览器存储的用例。
- [x] 6.6 验收：`npm run check`、`npm run build:setup`、上一条的测试、`npx openspec validate site-setup --strict` 通过；`dist/site/` 的初始化页里搜不到 `type="password"` 和 `api.github.com`。
  - 2026-11-23 实测：`npm run check` 0 错误（`apps/app` 20 文件、`apps/setup` 7 文件）；`npm run build:setup` 日志 `[build:pages] setup (规则 1)`；18 项初始化与账号测试全过；`openspec validate site-setup --strict` 通过。`dist/site/index.html` 与 `_astro/` 里搜不到 `type="password"`、`api.github.com`、`__setup/verify`、`CLOUDFLARE_API_TOKEN`、`GITHUB_TOKEN`；`api.github.com/user` 和 `__setup/verify` 只出现在 `_worker.js`（6.4 保留的服务器校验）。另跑了 `.test/site-setup-browser.mjs` 的 Edge 冒烟：深链接 `/settings/account/` 渲染出“登录 Cloudflare”，无内核，`/__setup/status` 由生成的 Worker 应答。
- [x] 6.7 提交并推送到 `main`，等 Cloudflare 的 Git 构建在 `online-gal` 项目上部署新的初始化构建（仍应是 `setup (规则 3)`），在 `https://onlinegal.cc.cd/` 确认页面只有“登录 Cloudflare”一个按钮。之后通知用户做 4.3 第 4 步。
  - 2026-11-23 实测：提交 `d6c39cf` 推到 `main`，Pages 的 Git 集成自动构建生产部署 `42d9a7f6-988a-483a-b7bf-c69ea2104cee`（`github:push`，`latest_stage` 为 `deploy`/`success`），构建日志 `Executing user command: npm run build:pages` → `[build:pages] setup (规则 3)`，上传 6 个文件。`https://42d9a7f6.online-gal.pages.dev/` 与 `https://onlinegal.cc.cd/` 的 `index.html` 字节一致（sha256 `1a6f6780…`，与本地 `dist/site` 一致），`/__setup/status` 返回 `{"phase":"setup","initialized":false}`。headless Edge 打开 `https://onlinegal.cc.cd/settings/account/?resume=true`：只有“登录 Cloudflare”一个按钮，`input[type=password]` 0 个、`input` 0 个，无 console 报错，无 Service Worker。
  - 之后再推 `main`（例如只改本文档的 `8fae3f1`）会重建同一份初始化构建：部署 `5c3771aa`，日志同样是 `[build:pages] setup (规则 3)`；两个部署的 `index.html` 只差 Astro 生成的 `<astro-island uid>`（同一组件产物 `_astro/SetupFlow.BYwmvpK9.js`），`onlinegal.cc.cd` 逐次跟随最新部署。


## 7. 决定 H：重新初始化并申请账户令牌

负责：管理系统的对话。依据 design.md S11 与 S7，spec 的 "Reinitialization clears only the setup record"、"Setup creates an account-owned token, never a user token" 两条，以及改写后的 "Written body carries no token"。不改 `packages/plugin-system/src/account/cloudflare.ts` 的登录代码：`CF_DEFAULT_SCOPES` 已含 `account_api_tokens:create`。

- [x] 7.1 `apps/setup/src/lib/setup-items.ts`：`SetupItem.source` 增加 `'generated'`；清单末尾加两项，都是 `source: 'generated'`：`CLOUDFLARE_ACCOUNT_TOKEN`（`owner: 'site-setup'`，初始化时申请的账户令牌，权限只有「Account API Tokens Edit」）和 `ONLINE_GAL_CA_PRIVATE_JWK`（`owner: 'plugin-system'`，浏览器里生成的 ECDSA P-256 私钥 JWK，K18 微型 CA 的长期私钥）。
- [x] 7.2 `apps/setup/src/lib/setup.ts`（S5 第 6 步，发 PATCH 之前）：
  1. 登录凭据的 `scopes` 含 `account_api_tokens:create` 才继续；缺少时停在第 2 步说明，不写入。`scopes` 为空数组时不拦截。
  2. `GET /accounts/{accountId}/tokens/permission_groups`，按 `name` 精确匹配 `Account API Tokens Write`，取它的 `id`。匹配不到或请求失败时停下说明，不写入。id 不得写死在代码里。
  3. `POST /accounts/{accountId}/tokens`，请求体 `name` 为 `online-gal-setup`，`policies` 一条 `allow`，`permission_groups` 只有上面取到的那一项，`resources` 为 `{ "com.cloudflare.api.account.<accountId>": "*" }`。MUST NOT 向 `/user/tokens` 发任何请求。
  4. 响应的 `value` 不显示、不进浏览器存储、URL 或日志，只放进随后 PATCH 的 `CLOUDFLARE_ACCOUNT_TOKEN`（生产环境 `secret_text`）。
  5. 用 `crypto.subtle.generateKey`（ECDSA P-256，`extractable: true`）生成密钥对，导出私钥 JWK，只留 `kty`、`crv`、`x`、`y`、`d`（三者都是 43 字符 base64url，否则重试生成），`JSON.stringify` 后放进 PATCH 的 `ONLINE_GAL_CA_PRIVATE_JWK`（生产环境 `secret_text`）。公钥不写入变量。用 `packages/plugin-system/src/attest.ts` 的 `microCaFingerprint` 算指纹，显示在页面上并写入 `sessionStorage`。私钥与 `d` 不显示、不进存储、URL 或日志，也不发到除这次 PATCH 以外的任何地址。
  6. 令牌或密钥已生成但 PATCH 失败时，丢弃两者，重试时重新申请、重新生成，不重用。
- [x] 7.3 `scripts/reinit-pages.mjs`（`npm run reinit:pages -- <project>`）：用已认证的 `cf` CLI 读项目，确认有 Git 来源；PATCH 把生产与预览环境的 `ONLINE_GAL_SITE` 设为 `null`，其他变量不动；读回确认已删除后，用 `cf pages deployments create <project> --branch <production_branch>` 触发生产构建并打印部署 id。支持 `--dry-run`（不写入、不部署）。MUST NOT 打印任何 secret 的值，不接受 token 参数。
- [x] 7.4 测试：PATCH 请求体的生产环境含 `CLOUDFLARE_ACCOUNT_TOKEN` 与 `ONLINE_GAL_CA_PRIVATE_JWK`，且不含任何 OAuth token；缺 `account_api_tokens:create` 时不发创建请求也不发 PATCH；创建请求只发往 `/accounts/{id}/tokens`，`permission_groups` 只有一项且其 id 来自权限组查询；不向 `/user/tokens` 发请求；生成的私钥 JWK 只有五个字段且 `x`、`y`、`d` 都是 43 字符，页面只出现公钥指纹；`reinit:pages --dry-run` 不写入、不部署，正式运行只把 `ONLINE_GAL_SITE` 设为 `null`。
- [x] 7.5 验收：`npm run check`、`npm run build:setup`、上一条的测试、`npx openspec validate site-setup --strict` 通过。提交并推送到 `main`。
  - 28 项 Node 测试通过（站点初始化、`configure-pages`、`reinit-pages`），Chromium 使用假 OAuth/API 验证页面与会话存储只含公钥指纹，不含令牌或私钥；生产项目 `npm run reinit:pages -- online-gal --dry-run` 未写入或部署。
- [ ] 7.6【用户】（第 8 节已完成，9.5 与 10.3 隔离验收以及对应实现上线后执行，生产结果记入 9.6）在本机对 `online-gal` 运行 `npm run reinit:pages -- online-gal`，等初始化构建生效后到 `https://onlinegal.de5.net/` 重新登录并完成初始化。完成后 `curl https://onlinegal.de5.net/__setup/status` 应返回站点构建的 HTML，不再是 `phase: "setup"` 的 JSON。

## 8. 决定 J：官方地址换成 `onlinegal.de5.net`

负责：管理系统的对话。依据 design.md S12。只用已认证的 `cf` CLI；`cf` 缺少某项操作的命令时停下回报。不触发重新初始化（7.6 由用户做）。

- [x] 8.1 代码默认值：把 S12 列出的五处 `onlinegal.cc.cd` 改为 `onlinegal.de5.net`；之后 `git grep -n onlinegal.cc.cd -- . ':!openspec' ':!docs'` 应无结果。跑 `test/plugin-system-microca.test.mjs`、`.test/site-setup.test.ts` 与 `npm run check`，提交并推送 `main`。这一步先于 8.3。
  - 2026-11-23：提交 `c0cb752` 已推送到 `main`；三项 CA/microca 测试通过，19 项 setup 测试通过，`npm run check` 通过。Pages 部署 `19d6ba0c-af52-45c5-b44c-6fe83597a493` 是 `deploy/success`，提交 SHA 为 `c0cb7523d16abf62d446a716c5d8edfd95d0aeb8`。未提交或改动其他工作区内容。
- [x] 8.2 新域名：确认 zone `onlinegal.de5.net` 属于 `online-gal` 项目所在的账户；把它加为该项目的自定义域名；等到域名状态 `active`，`curl -sI https://onlinegal.de5.net/` 返回 200。zone 的 DNSSEC 保持开启。
  - `onlinegal.de5.net` zone 与 Pages project 同属账户；Cloudflare DNSSEC 状态 `active`、父域 DS 存在。Apex CNAME `onlinegal.de5.net → online-gal.pages.dev` 为 proxied，Pages custom domain、DNS 验证和证书均 `active`。`curl -sSI https://onlinegal.de5.net/` 返回 `200 OK`。
- [x] 8.3 旧域名跳转：列出 zone `onlinegal.cc.cd` 的全部 DNS 记录（类型、名称、内容、是否代理）写进回报；按 S12 建 Single Redirect；`curl -sI "https://onlinegal.cc.cd/a/b?x=1"` 返回 301，`Location: https://onlinegal.de5.net/a/b?x=1`。
  - 操作前旧 zone 只有 `CNAME onlinegal.cc.cd → online-gal.pages.dev`（proxied，记录 ID `b7001bc58ce68cb6c50b39951664db7c`）。zone Single Redirect ruleset `b233ad59a76a42098d5dbcd36aa22100`，rule `2e556ab8ceea40f3b4adf95f7e1cd2fb`。HTTP 和 HTTPS `/a/b?x=1` 请求均为 `301 Moved Permanently`，Location 都保留路径与查询参数。
- [x] 8.4 解绑与删解析：从 `online-gal` 项目删除自定义域名 `onlinegal.cc.cd`；删除顶点指向 `*.pages.dev` 的 CNAME，加占位代理记录 `A @ 192.0.2.1`。只删指向 Pages 的记录，其他记录不动。再验一次 8.3 的 301，并记下 `curl -sI http://onlinegal.cc.cd/` 的实测结果。
  - Pages 现在只绑定 `onlinegal.de5.net`。旧 zone 中唯一的 Pages CNAME 已删除，添加了 proxied `A onlinegal.cc.cd → 192.0.2.1`（记录 ID `25d7940a97360b96d5cef1a2fc02abe0`），没有改动其他 DNS 记录。替换后 `curl -sSI http://onlinegal.cc.cd/` 返回 `301 Moved Permanently`，Location `https://onlinegal.de5.net/`；HTTP 与 HTTPS `/a/b?x=1` 仍 301 并保留路径和查询参数。
- [x] 8.5 在本节写验收记录：两个域名的状态、跳转规则 id、8.3 与 8.4 的响应头、8.1 的提交号与部署 id。
  - `onlinegal.de5.net` 为 Pages active custom domain，HTTPS 200，zone DNSSEC active；`onlinegal.cc.cd` 已解绑 Pages，现有 proxied placeholder A 为唯一记录。Redirect ruleset `b233ad59a76a42098d5dbcd36aa22100` / rule `2e556ab8ceea40f3b4adf95f7e1cd2fb` 返回 301 并保留 path/query。代码提交 `c0cb7523d16abf62d446a716c5d8edfd95d0aeb8`；Pages deploy `19d6ba0c-af52-45c5-b44c-6fe83597a493` (`deploy/success`)。旧区原 DNS 清单只有 proxied Pages CNAME `b7001bc58ce68cb6c50b39951664db7c`。

## 9. 决定 I：初始化构建令牌及 DNS 白名单（方案 A）

依据 design.md S13。管理系统负责初始化页、部署编排和隔离验收。插件系统负责站点构建、签名和真实 Service Worker 检查。两边按 S13 的变量和顺序对接。

- [ ] 9.0【管理系统】在隔离 Git 集成 Pages 项目验证生产变量的构建与 Function 生效时机、生产域名上的部署身份，以及按 S14 切换后的同源私有中转入口可用性。历史试验中固定 `/__cf` 的结果仅作既有机制的证据。使用隔离 zone 和与 S11 相同的账户令牌申请权限，验证可以申请仅对单个 zone 有 DNS 写权限的临时账户令牌，创建并读回测试 TXT 后撤销。记录权限组名称、资源范围和撤销结果。能力不满足时回报具体错误，由 OpenSpec 管理对话决定是否调整授权路径。
  - 继续验收需要一个已启用 DNSSEC 的隔离 zone，其中的测试主机可绑定隔离 Pages 项目。用实际的所有者登录授权核对账户令牌权限组、仅限该 zone 的 DNS 写权限及测试 TXT 创建、读回、清理与令牌撤销。已记录的 `403 / 9109` 来自 CLI OAuth，初始化页所有者登录的实际权限仍待验证。
  - 2026-11-23 只读复核：当前权限来源是已登录的 `cf@1.0.0-rc.2` OAuth (`cf auth scopes`，不是 Pages 内 S11 secret)。`cf auth scopes --filter account_api_tokens` 返回 `No granted scopes match`；DNS scope 报告含 `dns_records read, edit`、`zone read, write`。`cf accounts tokens permission-groups list --scope com.cloudflare.api.account.zone` 的脱敏请求路径为 `GET /client/v4/accounts/<account-id>/tokens/permission_groups?scope=com.cloudflare.api.account.zone`，Cloudflare HTTP 403、错误码 `9109`、消息 `Unauthorized to access requested resource`。不带 scope 与按 `DNS Write` 名称筛选也返回相同 403。
  - 当前生产账号下 `cf zones list` 只有生产 zones `onlinegal.cc.cd` 与 `onlinegal.de5.net`，没有隔离 zone。本次用 `cf pages create` 新建隔离 Git 项目 `online-gal-task106-probe`（GitHub source 同官方仓库、生产分支 `main`、禁用自动/预览部署、无自定义域名），只部署于 `*.pages.dev`；未创建临时账户令牌、TXT 或写入 DNS。
  - 2026-11-23 隔离 Pages 验证（源提交 `c05e89fcd8cc45ecf498f1ea945da95ea03af107`）：setup 部署 `055bc3a6-5edd-4d3c-b375-e097eec4ccef`，后续 site 部署 `588eb8de-26a5-40ac-aa96-3d49197f40bb`。每次 build 的 `CF_PAGES_URL` 与 `cf pages deployments get` 的部署 URL 一致，`CF_PAGES_BRANCH=main`；只 PATCH 生产变量不会改变现有部署（仍报 `phase=setup`、无 test secret），新 build 才返回 `phase=site`、`ONLINE_GAL_SITE` 与 Secret 变量。
  - 用隔离随机 P-256 CA key/构建 token 的 bootstrap 部署 `8b60adea-1204-4fb4-966b-f8b84681effc` 成功；无凭据 `POST /__ca/issue` 返回 `401 {"error":"unauthorized"}`、`no-store`。`ONLINE_GAL_CA_BOOTSTRAP: null` 经 `cf pages edit` 后 `cf pages get` 确认该键已删除，密钥仍为 `secret_text`（API 不回显值）；第二次部署 `b2c5aaea-1928-4cce-8208-da5dda323981` 成功，stable `*.pages.dev` 的 `CF_PAGES_URL` 更新为部署 URL，插件包 `online-gal.manager` 的实际线上字节通过 `verifyOfficialRelease`（未做 DNS/DoH 验收）。
  - 切换后的同源 `GET /__cf/api/user` 带正确 `Sec-Fetch-Site` 与 `Origin` 到达 Cloudflare API，返回其 JSON 错误 `9106`/`9107`（未发送认证头），响应含 `api-version` 与 `no-store`，证明已转发而非被同源守卫拦截。隔离项目无自定义生产域名；生产域名部署身份未在隔离域名验证。S11 账户令牌权限组读权限仍为 403/9109，且无隔离 zone；9.0 留待 OpenSpec 管理对话裁定权限路径及提供隔离 zone 后继续，不对生产 DNS 操作。
- [ ] 9.1【管理系统】更新 `apps/setup/src/lib/setup-items.ts` 和 `apps/setup/src/lib/setup.ts`。确认站点所有者、项目、生产分支、zone 和 DNS 现状。复用 S11 的 CA 生成结果并按 S13 生成构建令牌和候选 TXT。一次 PATCH 写入原有初始化项、构建令牌、引导标记与含生产分支和候选信任记录的生产初始化记录。预览只写原有初始化记录。在本机预演中生成写入清单。
  - #106 在本地实现所有者、项目与生产分支校验、构建令牌与候选 TXT 生成、生产和预览 PATCH 清单，以及空记录、相同记录和冲突的测试夹具。真实 DNS 现状读取待 9.0 验收。
- [x] 9.2【插件系统】按 S13 接入生产引导标记，构建日志能区分两次构建。首构建跳过签名并产出 CA 路由。第二次与后续受签名约束的构建强制签名，失败时以非零状态结束。发布的不可变路径能区分不同的签名封套和 bundle 字节。提供基于现有验证器的线上包检查和真实 Service Worker DoH 验收入口。
  - #107 实现提交 `442f7eb`。`@online-gal/plugin-system/release-check` 提供 `verifyOfficialRelease` 和 `verifyReleaseInServiceWorker`。后者通过 `/sw.js` 报告两家解析器的可读性（包括 CORS）、AD、TXT 和官方包验签结果。DoH 验证器随本提交入库，普通安装和更新接入由 `plugin-system` 10.2/10.4 跟踪。
  - 干净检出 `442f7eb` 复验通过：`npm run check`、`npm run build:site`、`site-setup` 与 `plugin-system` 的 OpenSpec 严格校验、`test/plugin-system-microca.test.mjs` 4/4。签名测试使用本地 CA 和模拟 Rekor，覆盖首部署及预览不签名、缺少令牌或 Rekor 不可用时构建失败、重签改变目录但保留版本、替换为旧 bundle 时拒绝验签。Chromium 通过真实 Service Worker 访问本地 HTTPS DoH 夹具，验证了分段 TXT、AD、两家解析器冲突、CORS 不可读与拒绝未签名包。
  - 官方 Pages 对 `442f7eb` 的部署 `c35f291f-34da-426a-815f-e62bfa7f5723` 成功。构建日志确认进入站点构建的未签名分支，`onlinegal.de5.net/sw.js` 已包含验收入口及两家解析器。真实 TXT 和生产签名流程的结果分别在 9.5、9.6 记录。
- [ ] 9.3【管理系统】编排两次部署、线上签名验证、DNS 写入和 DoH 验收。第一次部署在生产域名确认 CA 就绪才清除引导标记。第二次部署须从该部署专属 HTTPS 地址读取并验签官方包，再确认生产域名仍指向它。两项检查通过后才发布 TXT。UI 显示当前步骤和可重试错误，两个部署 ID 可恢复。提供关闭页面后按 S13 从现有配置继续的步骤。TXT 写入成功而 DoH 尚未传播时，从查询步骤继续。
  - #106 完成两次部署、CA 就绪检查、部署 ID 恢复和重试。本地实现通过 `verifyOfficialReleaseAt` 从签名部署专属地址验签，再复核生产域名指向，流程到「签名已验证，等待 DNS 验收」为止。已有初始化界面可从部署历史恢复 ID。站点切换后关闭唯一标签页的独立恢复入口由 9.3a 补齐。9.0 的隔离 Pages 结果来自此前的试验部署，最新实现仍在本地工作区，隔离自定义域名上的验签后身份复核待 9.5。
- [ ] 9.3a【管理系统主责／插件系统配合】按 S13 编写独立恢复操作手册并补齐现有 CLI 的部署专属地址验签入口。验证读取阶段只输出非敏感状态。在夹具中覆盖仍有引导标记、标记已清除且签名部署进行中、签名已完成及生产域名已指向另一部署。复用已有部署 ID，核对标记变更仅限生产引导标记，恢复演练到等待 DNS 验收。
- [ ] 9.4【共同测试】覆盖生产与预览 PATCH 体、secret 值仅进入正确的请求、首构建产物不签名且包含 CA 路由、中间清标记失败、CA 尚未就绪、签名错误、第二次部署失败、续跑复用部署、旧 bundle 与 DNS 冲突。确认上述失败情况下保持正确的步骤，且签名产物通过验证之前 DNS 写入请求为零。跑针对性测试、`npm run check`、两阶段构建和这两个变更的 OpenSpec 严格校验。
  - #106 本地结项报告记录针对性测试 23/23，覆盖候选 TXT 夹具、专属验签地址、生产主机换部署时拒绝继续、重试复用部署和 DNS 请求计数为零。`npm run check`、`npm run build:setup`、`npm run build:pages -- --phase=site` 与 `site-setup` 严格校验通过。本地 site 构建走未签名分支，完整两阶段和真实 DoH 验收待 9.5。
- [ ] 9.5【共同验收／上线前】在隔离 Git 集成项目跑完整流程。第一次部署接管隔离自定义域名后，关闭唯一初始化标签页，从本地入口接续至验收完成。记录两个部署 ID、官方 bundle 验证结果、真实 Service Worker 的 CORS／AD／TXT 和验证结果、临时令牌撤销情况。提交文档和实现，确认官方 Pages 的 Git 源分支已包含第 9 节代码并成功构建。9.5 与 10.3 通过后再通知用户执行 7.6。
- [ ] 9.6【生产验收／7.6 后】记录 `onlinegal.de5.net` 的两次生产部署、签名检查、`_trust.onlinegal.de5.net` 与真实 Service Worker DoH 结果，核对初始化页已退出并清理引导标记。仅在结果通过后勾选本项和插件系统 7.7。

## 10. 初始化后交接私有 Cloudflare 入口

管理系统 #151 与插件系统 #153 对照 `cf-account-admin` §7 实施。联调须保留第 9 节已有的部署身份、签名地址验证和关闭标签页后的续跑。

- [ ] 10.1【管理系统】按 S14 登记并在生产 PATCH 写入路由密钥，提供所有者私有入口，验证预演与进度存储无泄露。复用 `cf-account-admin` A12 的派生与项目核验逻辑。
- [ ] 10.2【管理系统主责／插件系统配合】联调初始化、首次站点部署、签名部署切换。用同一 OAuth 会话与入口继续，验证清引导标记前可用性。关闭唯一初始化标签页后复用 9.3a，从本地 `cf` 续跑。覆盖既有项目补写和入口轮换。
- [ ] 10.3【隔离验收】与 `cf-account-admin` 7.3 联合验收。记录旧路径和随机路径均无 Function 调用、新浏览器所有者登录及非管理员拒绝、初始化切换后能续跑、现有签名部署地址仍可验签。生产重新初始化继续由所有者执行，在 9.5 与该项通过后进行。
