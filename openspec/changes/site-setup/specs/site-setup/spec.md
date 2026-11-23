# Spec Delta

## Purpose

定义站点初始化。构建脚本根据 Pages 项目里的初始化记录决定产出初始化构建还是站点构建。初始化构建只有初始化页；所有者只用 Cloudflare OAuth 登录，不手填任何 token，确认所有者后一次写入 Pages 项目，再触发一次新的构建。之后的站点构建不含任何初始化代码，访客无需登录即可正常使用。

## ADDED Requirements

### Requirement: Git-integrated Pages build settings can be applied from the repository

仓库 MUST 提供 `npm run configure:pages -- <project>`，使用已认证的 `cf` CLI 给指定的 Git 集成 Pages 项目写入构建命令 `npm run build:pages`、仓库根目录和输出目录 `dist/site`。命令 MUST 在写入前确认项目有 Git 来源，且不得写入环境变量、密钥、域名、Git 来源或生产分支。配置相符时 MUST 跳过写入。除显式使用 `--deploy` 外 MUST NOT 触发部署。

#### Scenario: Configure a new Git-integrated project without using the form

- **WHEN** 操作者指定一个已连接 Git、尚未配置构建命令的 Pages 项目
- **THEN** 命令写入三项构建设置并重新读取确认
- **AND** 只有传入 `--deploy` 才用项目已有的生产分支发起新部署

#### Scenario: Preview or reject an incompatible project

- **WHEN** 操作者使用 `--dry-run`，或指定一个没有 Git 集成的项目
- **THEN** 命令不写入配置也不触发部署，并分别显示所需配置或拒绝原因

### Requirement: One build script chooses the phase

`npm run build:pages` MUST 按以下顺序决定阶段：命令行 `--phase`；环境变量 `ONLINE_GAL_SITE` 非空则为站点构建；`CF_PAGES === '1'` 则为初始化构建；有 `CLOUDFLARE_API_TOKEN`、`CLOUDFLARE_ACCOUNT_ID`、`ONLINE_GAL_PAGES_PROJECT` 时查询项目，生产环境有非空的 `ONLINE_GAL_SITE` 则为站点构建，否则为初始化构建。查询失败或无法判断时 MUST 以非零状态退出。脚本 MUST NOT 打印任何变量的值。

#### Scenario: Cloudflare build before and after initialization

- **WHEN** Cloudflare 的构建环境里没有 `ONLINE_GAL_SITE`，之后写入它再构建一次
- **THEN** 第一次产出初始化构建，第二次产出站点构建

#### Scenario: Release workflow cannot reach the API

- **WHEN** 在发布工作流里查询 Pages 项目失败
- **THEN** 构建以非零状态退出，不产出任何阶段的产物

### Requirement: The setup build contains only the setup system

初始化构建的 `dist/site/` MUST 只含初始化页、它的脚本和样式、清理旧内核的 `sw.js`、`_worker.js`、`_routes.json` 与 `_headers`。它 MUST NOT 构建或包含插件系统内核、注册表、管理器页面和引擎。任何路径 MUST 显示初始化页。`_worker.js` MUST 只提供 `/__cf/**` 与 `/__setup/**`，其余请求交给静态资源。所有响应 MUST 带 `Cache-Control: no-store`，并禁止被嵌入框架。

#### Scenario: Any path shows the setup page

- **WHEN** 访客在未初始化的站点上打开 `/settings/`
- **THEN** 得到初始化页，不加载 Service Worker 内核

#### Scenario: A previously installed kernel is removed

- **WHEN** 一个浏览器以前装过站点构建的内核，打开初始化构建
- **THEN** 新的 `sw.js` 删除本源的缓存、注销自己，并重新载入窗口，之后显示初始化页

### Requirement: The site build contains nothing of the setup system

站点构建 MUST 与 `plugin-system` 的站点构建一致，且 MUST NOT 含 `_worker.js`、`/__setup` 路由、初始化页或初始化代码。构建脚本 MUST 在站点构建后检查 `dist/site/` 下没有文件含字符串 `__setup` 或 `ONLINE_GAL_SETUP`，并且仓库里不存在 `functions/__setup/`；不满足时 MUST 以非零状态退出。

#### Scenario: Setup code leaks into the site build

- **WHEN** 站点构建的某个文件含有 `__setup`
- **THEN** `npm run build:pages` 以非零状态退出

### Requirement: Setup endpoints exist only in the setup build

`GET /__setup/status` MUST 返回 `{ "phase": "setup", "initialized": boolean }`，可附 `v` 与 `warning`，MUST NOT 返回账号 id、用户 id 或项目名。`POST /__setup/verify` MUST 只在运行时 `ONLINE_GAL_SITE` 缺失或为空字符串时可用，非空时 MUST 返回 404；它 MUST 只接受同源的 JSON 请求，校验 Cloudflare API token 或 GitHub token 是否有效，MUST NOT 回显 token 或记录日志。

#### Scenario: Verify after the record is written

- **WHEN** 初始化构建在运行时读到 `ONLINE_GAL_SITE`，有人请求 `POST /__setup/verify`
- **THEN** 返回 404

### Requirement: Only the site owner initializes, without the kernel

初始化页 MUST 在页面内用 `CloudflareAccount`（内存保管库）完成设备码登录，申请的权限范围 MUST 是 `CF_DEFAULT_SCOPES` 的全部项加 `offline_access`。OAuth token MUST 只存在于页面内存，MUST NOT 写入任何浏览器存储、URL、日志或 Pages 项目变量。token 响应里的 `scope` 缺少 `pages:read` 或 `pages:write` 时，页面 MUST 停下并说明缺少的权限，不写入。只有 A6 判定为拥有本站 Pages 项目的账号 MUST 能进入写入步骤。

#### Scenario: Not the owner

- **WHEN** 登录的账号没有本站的 Pages 项目
- **THEN** 页面停在所有者一步，只提供退出并更换账号

#### Scenario: Granted scope lacks Pages write

- **WHEN** 登录成功，但 token 响应的 `scope` 里没有 `pages:write`
- **THEN** 页面说明缺少 `pages:write`，不发出 PATCH，只提供退出并更换账号

### Requirement: Setup is completed with OAuth only and written once

初始化页 MUST NOT 显示任何 token 输入框或校验步骤，MUST NOT 调用 `POST /__setup/verify`。初始化项清单里出现 `source: "input"` 的项时，页面 MUST 停下说明不支持，不写入。写入 MUST 是一次 PATCH：每项写成生产环境的 `secret_text`，`ONLINE_GAL_SITE` 以 `plain_text` 写入生产与预览环境。项目里已有非空的 `ONLINE_GAL_SITE` 时，页面 MUST 继续已记录的流程并保留其中的密钥。方案 A 在首次站点部署验证后，允许一次单独的 PATCH 清除生产引导标记。在本机地址上，页面 MUST NOT 写入或触发构建，只显示预演。

#### Scenario: Reload after writing

- **WHEN** 写入成功后所有者刷新页面并重新登录
- **THEN** 页面从已完成的步骤继续，复用进行中的部署及原有密钥

#### Scenario: Written body carries no token

- **WHEN** 所有者点击"写入并构建"
- **THEN** PATCH 请求体的生产环境有 `ONLINE_GAL_SITE`、`CLOUDFLARE_ACCOUNT_ID`、`CLOUDFLARE_ACCOUNT_TOKEN` 与 `ONLINE_GAL_CA_PRIVATE_JWK`。方案 A 还写入生产 `secret_text` `ONLINE_GAL_CA_BUILD_TOKEN`、生产 `plain_text` `ONLINE_GAL_CA_BOOTSTRAP=1` 和生产记录中的公开承诺数据。预览环境只收到原有的 `ONLINE_GAL_SITE`。请求体 MUST NOT 含 OAuth access token、refresh token 或 GitHub token

### Requirement: Reinitialization clears only the setup record

`npm run reinit:pages -- <project>` MUST 用已认证的 `cf` CLI 把指定项目生产与预览环境的 `ONLINE_GAL_SITE` 设为 `null`，MUST NOT 改动其他变量，也 MUST NOT 打印任何 secret 的值。读回确认记录已删除后，MUST 用项目已有的生产分支触发一次生产构建。`--dry-run` MUST NOT 写入或部署。

#### Scenario: Dry run changes nothing

- **WHEN** 操作者对一个已初始化的项目使用 `--dry-run`
- **THEN** 命令只显示将要删除的键，不写入、不触发构建

### Requirement: Setup creates an account-owned token, never a user token

初始化页 MUST 用刚登录的 OAuth access token 调 `POST /accounts/{account_id}/tokens` 申请一个账户所有的 API 令牌，MUST NOT 调 `POST /user/tokens`。令牌的权限组 MUST 只有 `Account API Tokens Write` 一项，资源 MUST 限于已确认的这一个账户。权限组 id MUST 在运行时从 `GET /accounts/{account_id}/tokens/permission_groups` 按名称取得，MUST NOT 写死在代码里。令牌值 MUST 只写入生产环境 secret `CLOUDFLARE_ACCOUNT_TOKEN`，MUST NOT 显示在页面上，也 MUST NOT 进入浏览器存储、URL 或日志。登录的 `scope` 缺少 `account_api_tokens:create` 时，页面 MUST 停下说明，不写入。

#### Scenario: Granted scope cannot create account tokens

- **WHEN** 登录成功，但 token 响应的 `scope` 里没有 `account_api_tokens:create`
- **THEN** 页面说明缺少该权限，不发出创建令牌的请求，也不发出 PATCH

#### Scenario: The created token is account-owned

- **WHEN** 所有者点击"写入并构建"
- **THEN** 页面只向 `/accounts/{account_id}/tokens` 发送创建请求，请求体的 `permission_groups` 只有 `Account API Tokens Write` 一项，不向 `/user/tokens` 发送任何请求

### Requirement: Setup generates the CA private key in the browser

初始化页 MUST 在浏览器里用 `crypto.subtle.generateKey` 生成一对 ECDSA P-256 密钥，把私钥 JWK（只含 `kty`、`crv`、`x`、`y`、`d`，且 `x`、`y`、`d` 都是 43 个字符的 base64url）写入生产环境 secret `ONLINE_GAL_CA_PRIVATE_JWK`。公钥 MUST NOT 写入任何变量。页面 MUST 显示公钥指纹（RFC 7638，对规范化公钥 JWK 取 SHA-256 的小写十六进制）并写入 `sessionStorage`，MUST NOT 显示私钥或 `d`，也 MUST NOT 把私钥发到除随后那一次 PATCH 以外的任何地址。创建令牌或生成密钥成功但 PATCH 失败时，MUST 丢弃两者并在重试时重新申请、重新生成。

#### Scenario: The private key never leaves the write request

- **WHEN** 所有者点击"写入并构建"
- **THEN** 私钥 JWK 只出现在发往 Cloudflare Pages 项目的那一次 PATCH 里，不出现在页面文本、浏览器存储、URL 或其他请求中，页面只显示公钥指纹

### Requirement: Writing triggers a new build and the page hands over

方案 A 的生产 Git 集成部署遵循下一条 Requirement。其余情形下，写入后，页面 MUST 按构建时写入的方式触发一次新的构建：`pages-git` 用 create deployment，`github-rerun` MUST NOT 调用 GitHub API，只显示那次工作流的手动重跑入口和推新 `v*` 标签的方法，`manual` 显示命令；失败时 MUST 显示原因和手动方法。新构建成功后，页面 MUST 等到 `/__setup/status` 不再返回 `phase: "setup"`，再回到原来的路径。

#### Scenario: Site build goes live

- **WHEN** 新构建成功，`/__setup/status` 返回站点构建的 HTML
- **THEN** 页面回到所有者最初打开的路径，站点正常工作

### Requirement: Production setup signs official packages before publishing trust

方案 A 启用时，初始化流程 MUST 生成生产构建令牌和独立的随机 salt，并计算 K16 的 `cf-pages` 承诺哈希。令牌明文 MUST 仅在生成时的内存和发往该项目的初始化 PATCH 中使用。第一次站点构建 MUST 跳过签名并部署签发接口。只有确认对应的生产部署已生效且签发接口带生产令牌绑定后，流程才可清除生产引导标记并触发签名构建。两次部署 MUST 遵守站点构建与初始化构建的隔离规则。

#### Scenario: The signing endpoint is ready

- **WHEN** 第一笔站点部署已在生产域名生效，且无凭据 POST `/__ca/issue` 返回 `401 unauthorized` 和 `no-store`
- **THEN** 流程清除并读回生产引导标记，发起签名构建并等待其在生产域名生效。校验过程 MUST 保留本次流程已写入的密钥

#### Scenario: The signed site is live

- **WHEN** 第二笔部署在生产域名生效，从该部署专属 HTTPS 地址读取的全部官方包通过 digest 检查和基于候选承诺的验签，且验签后生产域名仍指向该部署
- **THEN** 初始化流程用只限目标 zone 的临时账户令牌写入候选 TXT，已有相同记录则复用。写入尝试结束后 MUST 撤销临时令牌。已有不同记录或多条记录时 MUST 保留原记录并报告冲突
- **AND** 流程 MUST 从真实 Service Worker 验证两个 DoH 端点的 CORS、AD 位和 TXT，按 K16 验证策略与线上包。通过后清理进度并回到原路径

#### Scenario: Deployment, verification, or DNS propagation is interrupted

- **WHEN** 首部署未就绪、签名构建失败、包验证失败、验签后复核的生产部署身份不匹配，或 DoH 仍未传播
- **THEN** 流程 MUST 保留可恢复的进度并允许从失败步骤继续。只有线上包验签和生产部署身份复核均通过才可改动 TXT。若 TXT 已写入且匹配候选值，重试 MUST 复用该记录

#### Scenario: The last setup tab is closed after bootstrap

- **WHEN** 第一次站点部署已接管生产域名，且所有初始化标签页均已关闭
- **THEN** 所有者 MUST 能用已认证的本地 `cf` 和仓库验签入口，从现有项目状态和部署历史继续。恢复 MUST 保留已写入的密钥、复用可恢复的部署，并执行相同的部署身份检查、专属地址验签与验签后复核

### Requirement: Setup hands off the private Cloudflare route before switching deployments

初始化 MUST 用完成 S11 的同一 OAuth 会话向 Cloudflare `GET /user` 验证用户身份。确认目标项目和正式主机后，将该用户 ID 与派生入口使用的随机密钥分别写为生产 Pages secret `ONLINE_GAL_CF_OWNER_ID` 和 `ONLINE_GAL_CF_ROUTE_KEY`。预览 MUST NOT 继承两项配置。初始化 MUST 在首次站点部署前把入口提供给所有者保存。两次部署 MUST 使用相同的所有者绑定与入口。

#### Scenario: Setup continues after the first site deployment replaces the setup page

- **WHEN** 首次站点部署接管正式主机，初始化页仍在当前标签页打开
- **THEN** 初始化页 MUST 用现有 OAuth 会话完成私有 Function 的所有者和项目核验，随后经该入口继续本轮部署及 DNS 流程
- **AND** 新站点部署 MUST 停止在固定 `/__cf` 网络路径触发 Function

#### Scenario: An existing site upgrades the relay

- **WHEN** 旧项目已有初始化记录但缺少私有路由密钥或所有者绑定
- **THEN** 站点构建 MUST 关闭公开中转，静态站点继续服务。所有者 MUST 能用已认证的本地 `cf` 补写所有者绑定、路由配置并部署，保留其余初始化配置

### Requirement: Other changes add setup items through the list

需要在初始化时写入的密钥 MUST 登记在 `apps/setup/src/lib/setup-items.ts`，名字 MUST 匹配 `^[A-Z][A-Z0-9_]*$` 且不是 `ONLINE_GAL_SITE`。读取这些 secret 的功能读不到时 MUST 给出明确错误，MUST NOT 在响应或日志中输出它们的值。

#### Scenario: A new item on an initialized site

- **WHEN** 站点初始化后，另一个变更登记了新的初始化项
- **THEN** 该项要在 Cloudflare 控制台手动添加；使用它的功能在缺少时报错
