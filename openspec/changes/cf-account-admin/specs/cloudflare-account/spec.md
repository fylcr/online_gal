# Spec Delta

## Purpose

定义 Cloudflare 账号功能：用 cf 自己的身份和设备码流程登录，凭据只存在于插件系统内核中并加密存放，经同源代理访问 Cloudflare，并在账号拥有本站时开放 admin 入口。OAuth 凭据的整体设计由用户另行决定，本 capability 只覆盖 Cloudflare 一个提供方。

## ADDED Requirements

### Requirement: Sign-in uses cf's own identity and device flow

登录 MUST 使用 cf 的 OAuth client id 与 cf 的默认权限列表，二者 MUST 在构建时从 cf 钉住的 `@cloudflare/workers-auth` 版本读取，MUST NOT 手抄；读取的版本与 cf 固定 commit 声明的版本不一致时，构建 MUST 失败。登录 MUST 走设备码流程（RFC 8628）。轮询 MUST 由页面驱动，每次调用只发一个 token 请求；收到 `slow_down` 时，返回给页面的间隔 MUST 加 5 秒。

#### Scenario: A user signs in from settings

- **WHEN** 用户在设置的账号分区点击登录，在新标签里输入验证码并授权
- **THEN** 页面显示验证码、验证链接与倒计时，并按间隔轮询
- **AND** 授权后状态变为已登录，显示邮箱与用户 id

#### Scenario: The code expires

- **WHEN** 300 秒内没有完成授权
- **THEN** 轮询结果为 `expired`，进行中的登录被清除，用户可以重新开始

### Requirement: Credentials exist only inside the kernel, encrypted at rest

access token、refresh token 与设备码 MUST 只存在于内核的内存与保管库中。内核的任何 API 响应 MUST NOT 包含它们。保管库 MUST 用一把不可导出的 AES-GCM-256 密钥加密每条记录，每次写入使用新的 12 字节 iv，附加认证数据为 `online-gal-vault/v1/<provider>`；解密失败的记录 MUST 视为不存在并被删除。记录 MUST 按提供方分开存放。

#### Scenario: Stored data never contains a plaintext token

- **WHEN** 登录完成后检查 IndexedDB `online-gal-vault` 中的全部记录
- **THEN** 任何记录里都找不到 access token 或 refresh token 的明文

#### Scenario: Status does not leak tokens

- **WHEN** 已登录的官方管理器请求 `/__kernel/api/account/cloudflare/status`
- **THEN** 响应只含登录状态、用户、权限、到期时间与站点管理员判定，不含任何 token 或设备码

### Requirement: The account registers `/__cf` with the plugin system

账号功能 MUST 按 `plugin-system` 的登记规则登记前缀 `/__cf`：内核模块接管 `/__cf/**` 的任何方法，只为页面中转 `/__cf/api/**`，其余路径 MUST 得到 403 `account-forbidden`；内核 API 名字空间为 `/__kernel/api/account/`，其中 `account/cloudflare/status` 用 GET，`login`、`poll`、`logout` 用 POST；服务器 Function 为 `functions/__cf/[[path]].ts`。

#### Scenario: A third-party page asks for the relay

- **WHEN** 插件页面以 `DELETE` 请求 `/__cf/api/zones/z1`
- **THEN** 内核返回 403 `account-forbidden`，请求不到达服务器

#### Scenario: A page asks for the OAuth endpoints

- **WHEN** 官方管理器页面请求 `/__cf/oauth/token`
- **THEN** 内核返回 403 `account-forbidden`；只有内核自己会调用 OAuth 端点

### Requirement: Only the official manager may use the account

内核 MUST 只为当前管理器来源为 `official:`、且路径不在 `/__plugins/`、`/__kernel/` 下的客户端（官方管理器页面及其 worker）提供账号 API 与 `/__cf/**` 中转。其他调用方，以及无法确定 clientId 的请求，MUST 得到 403 `account-forbidden`。页面带来的 `Authorization` 头 MUST 被丢弃，由内核按保管库中的凭据重新设置。

#### Scenario: A third-party manager is refused

- **WHEN** 当前管理器来自 `github:` 来源，其页面请求 `/__cf/api/user` 或账号状态
- **THEN** 内核返回 403 `account-forbidden`，不发出任何网络请求

#### Scenario: The official manager calls the API without a token

- **WHEN** 已登录时官方管理器页面请求 `/__cf/api/user` 且不带 `Authorization`
- **THEN** 内核通过私有 Function 路径带上 `Authorization: Bearer <access>` 和本站校验所得的短期证明转发，页面得到 Cloudflare 的响应

### Requirement: Tokens are refreshed and sign-out always clears them

转发前若 access token 距到期不足 60 秒，内核 MUST 先刷新，且同一时刻只发一个刷新请求。刷新得到 `invalid_grant` 或 `invalid_client` 时，内核 MUST 清除凭据，并以 401 `account-signed-out` 应答原请求；网络错误 MUST NOT 清除凭据。退出时内核 MUST 尝试吊销 refresh token，无论吊销是否成功，都 MUST 清除凭据与进行中的登录。

#### Scenario: Concurrent requests share one refresh

- **WHEN** token 将在 30 秒后到期，页面同时发出 5 个 `/__cf/api/*` 请求
- **THEN** 内核只发出一个刷新请求，5 个请求都使用新 token 转发

#### Scenario: Sign-out while offline

- **WHEN** 网络不可达时用户退出登录
- **THEN** 吊销失败，但凭据仍被清除，状态变为未登录

### Requirement: Same-origin proxy function

初始化构建的 `functions/__cf/[[path]].ts` MUST 将 `/__cf/api/<rest>` 转发到 `https://api.cloudflare.com/client/v4/<rest>`，保留方法与查询串。`/__cf/oauth/{device/auth,token,revoke}` MUST 只接受 POST，转发到 `https://dash.cloudflare.com/oauth2/` 对应端点。未知路径 MUST 返回 404。初始化构建 MUST 在 `_routes.json` 启用 `/__cf/*`。

#### Scenario: Cookies and arbitrary hosts are not forwarded

- **WHEN** 请求 `/__cf/api/zones` 时带了 `Cookie`，另有一个请求访问 `/__cf/other/x`
- **THEN** 上游请求里没有 `Cookie`；后一个请求得到 404，没有发出任何上游请求

### Requirement: The relay constrains upstream requests and responses

初始化与站点代理 MUST 只转发 `authorization`、`content-type`、`accept`、`content-length` 请求头。响应 MUST 去掉 `set-cookie` 并带 `cache-control: no-store`。代理 MUST 校验同源。`Sec-Fetch-Site` 存在时必须为 `same-origin`，缺失时 `Origin` 必须等于本站。检查不通过 MUST 返回 403。代理 MUST NOT 记录请求头或请求体。

#### Scenario: A cross-site request is refused

- **WHEN** 其他网站的页面向 `/__cf/api/user` 发请求（`Sec-Fetch-Site: cross-site`）
- **THEN** 代理返回 403，不发出上游请求

### Requirement: Site admin is decided by owning this site's Pages project

初始化发现项目时 MUST 在登录账号可见的项目中查找 `subdomain` 或 `domains[]` 等于当前主机名的项目。本地调试 MUST 使用构建时的站点主机名。初始化授权须符合 `site-setup` 的所有者绑定要求。站点 MUST 通过私有探针取得管理员结果，格式为 `{ accountId, accountName, projectName }` 或 `null`。响应最多缓存 10 分钟。失败结果 MUST NOT 缓存。登录、刷新与退出 MUST 清空缓存。

#### Scenario: The owner sees the admin entry

- **WHEN** 已绑定的所有者在 `onlinegal.de5.net` 登录，私有探针确认其仍能访问绑定本站的 Pages 项目
- **THEN** 页面显示"进入站点管理"，`/admin` 列出项目、部署、域名、控制台四个分区，每个分区是 `/admin/<id>`

#### Scenario: A destructive admin action asks first

- **WHEN** 站点管理员在 `/admin/deployments` 点"回滚到此"，或在 `/admin/domains` 点"移除"
- **THEN** 所在行先显示确认，取消时不发出任何请求；确认后才经 `/__cf/api/**` 调用，失败原因显示在该行

#### Scenario: Another account does not

- **WHEN** 登录账号没有匹配的项目
- **THEN** 账号分区不显示入口，直接打开 `/admin` 或任何 `/admin/<id>` 时都显示无权访问的原因与账号设置的链接

### Requirement: Initialized deployments expose only a private Cloudflare relay route

完成初始化的生产站点 MUST 将经验证的 Cloudflare 账户、Pages 项目和正式主机名与独立随机 Pages secret 绑定，以派生私有 Function 路径。站点构建 MUST 在 Pages 调用层精确包括该私有子路径，并 MUST NOT 包含固定的 `/__cf/*` 通配、公网发现入口或旧 `/__cf/api/**` 与 `/__cf/oauth/**` 网络回退。公开可下载的构建产物和应用日志 MUST NOT 包含私有路径或其密钥。Function MUST 核对规范路径及正式主机。所有者 MUST 能私下导入入口，通过 cf OAuth 登录，并在入口失效后通过已认证的本地 `cf` 恢复管理入口。

#### Scenario: Public paths do not trigger Functions

- **WHEN** 请求命中旧固定路径或错误的私有路由段
- **THEN** Pages MUST 在调用层排除请求，没有 Function 调用或 Cloudflare 上游转发

#### Scenario: An owner uses a new browser

- **WHEN** 所有者在新浏览器打开初始化时保存的入口
- **THEN** 官方管理器 MUST 导入后清除 URL 片段，随后使用 cf OAuth，之后可直接打开 `/admin`

### Requirement: The private function verifies the administrator against the configured project

私有 Function MUST 用当前 OAuth 凭据查询 Cloudflare `GET /user` 并匹配生产 `ONLINE_GAL_CF_OWNER_ID`。匹配后 MUST 用同一凭据查询绑定项目并核对正式主机。冷探针最多各请求一次。`/admin` 及其子页 MUST 要求有效探针结果。通用 API 转发 MUST 验证合法的短期证明，该证明 MUST 绑定入口、主机和当前凭据，刷新、轮换入口或退出时 MUST 从内核清除。Cloudflare 按当前凭据执行操作授权。

#### Scenario: The caller can reach the route but cannot read the configured project

- **WHEN** 调用者掌握私有路径，但缺少有效的本站项目访问权限，或 Cloudflare 返回的是不同项目
- **THEN** Function MUST 拒绝授予管理员状态或通用 API 转发证明

#### Scenario: A collaborator can read the project

- **WHEN** 调用者登录后可以读取绑定项目，但其 Cloudflare 用户 ID 与已绑定的所有者不同
- **THEN** 私有 Function MUST 返回 `403 not-admin`，MUST NOT 查询项目或签发转发证明

#### Scenario: Rate limiting interrupts a check

- **WHEN** 核验请求触发边缘限流或遇到网络错误
- **THEN** 管理器 MUST 显示可重试的暂不可用状态，MUST NOT 将其缓存为无权访问

### Requirement: Settings has an account section

`SETTINGS_SECTIONS` MUST 包含 `account` 分区，提供登录、账号信息、退出，以及站点管理员才有的 admin 入口。内核不受控或返回 `account-forbidden` 时，该分区 MUST 显示具体的不可用原因，而不是空白或通用错误。

#### Scenario: Not yet controlled by the kernel

- **WHEN** 在 `astro dev` 下或首次访问、页面还没被 Service Worker 接管时打开账号分区
- **THEN** 分区显示"插件系统未就绪"，不显示登录按钮
