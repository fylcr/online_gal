# Spec Delta

## Purpose

定义插件系统：一个 Service Worker 内核，充当站点在本地的服务器。页面只注册它一次；之后本源的每个请求都由内核从已安装的插件中应答，当前管理器（manager）就是站点本身。真正的服务器只是插件的来源：官方插件来自本站的官方注册表（Cloudflare Pages），第三方插件来自 GitHub 仓库的标签。插件内部的权限与服务规则由 `plugin-host` capability 规定。

## ADDED Requirements

### Requirement: The kernel answers same-origin requests locally

内核 MUST 拦截本源的请求并从已安装插件的本地副本应答；除下列路径外，MUST NOT 把本源请求转发给服务器：`/sw.js`（内核更新）、`/registry/**`（官方注册表）与 `/__ca/**`（匿名微型 CA）MUST 直接到达服务器；跨源请求 MUST 不被拦截。路由 MUST 为：`/__kernel/api/<name>` → 内核 API；`/__kernel/` → 内核自带页面；已登记的扩展前缀（任何方法）→ 该扩展的内核模块；`/__plugins/<id>/<path>` → 该插件的文件；其余 GET/HEAD → 当前管理器的文件。非 GET/HEAD 且不是内核 API 或已登记前缀的请求 MUST 不被拦截。内核的每个响应 MUST 带 `Cross-Origin-Opener-Policy: same-origin`、`Cross-Origin-Embedder-Policy: credentialless` 与 `X-Content-Type-Options: nosniff`（引擎需要跨源隔离）。

#### Scenario: A reload is served without the server

- **WHEN** 管理器已安装，用户刷新页面
- **THEN** 页面与它引用的全部文件都由内核从本地应答，服务器只收到浏览器自己对 `/sw.js` 的更新检查
- **AND** 页面 `crossOriginIsolated` 为真

#### Scenario: The site keeps working offline

- **WHEN** 管理器已安装后服务器不可达，或 Service Worker 被浏览器停止后重启
- **THEN** 导航仍得到管理器页面

#### Scenario: Kernel script and registry bypass the kernel

- **WHEN** 页面请求 `/sw.js`、`/registry/index.json` 或 `/__ca/` 下的路径
- **THEN** 内核不应答，请求原样到达服务器


### Requirement: Other changes extend the kernel by registration

插件系统之外的变更 MUST 通过登记扩展内核，MUST NOT 另改本 capability 的条款。一项登记包括一个顶层前缀 `/__<name>`（MUST NOT 是 `__kernel`、`__plugins` 或 `__ca`），以及以下可选部分：接管该前缀下任何方法请求的内核模块、内核 API 名字空间 `/__kernel/api/<name>/**`、服务器 Function `functions/__<name>/`。登记的前缀 MUST 成为管理器包的保留路径与站点构建的保留名；有 Function 时，`_routes.json` 的 `include` MUST 只列出该登记需要到达服务器的路由。登记可使用 `/__<name>/*`，也可在其下限定子路径。内核模块 MUST 自行核对调用方。当前的登记是 `/__cf`（变更 `cf-account-admin`）。

#### Scenario: Registered and unregistered prefixes

- **WHEN** 页面以 `DELETE` 请求已登记前缀下的 `/__cf/api/zones/z1`，又以 `POST` 请求未登记的 `/__x/y`
- **THEN** 前者交给该扩展的内核模块应答；后者不被内核拦截，原样到达服务器


### Requirement: Static-host path resolution inside the file list

内核 MUST 像静态主机一样解析路径：`a/` → `a/index.html`；`a` 在只有 `a/index.html` 时 MUST 以 308 重定向到 `a/`（保留查询串）；`a` → `a.html`。查找 MUST 只在该插件已校验的文件清单内进行，任何 URL 都 MUST NOT 命中清单之外的文件；无法解码的路径 MUST 返回 400。管理器缺页时若其包含 `404.html`，MUST 以 404 状态返回它。内核 MUST 支持单段 `Range` 请求（206，不可满足时 416 并带 `Content-Range: bytes */<size>`），多段或其他单位的范围 MAY 以完整 200 应答。

#### Scenario: Directory, extensionless and missing paths

- **WHEN** 请求 `/settings`、`/settings/`、`/about`（文件为 `about.html`）与 `/nope`
- **THEN** 分别得到 308 到 `/settings/`、`settings/index.html`、`about.html` 与管理器的 `404.html`（状态 404）
- **AND** `/settings%2F..%2Findex.html` 一类解码后含 `../` 的路径只与清单中的文件名逐字比较，得到 404；无法解码的路径得到 400

#### Scenario: Range requests are answered from stored files

- **WHEN** 请求一个已安装文件的 `bytes=2-5`、`bytes=-3` 与超出长度的范围
- **THEN** 前两者得到 206 与正确的字节和 `Content-Range`，后者得到 416

### Requirement: Verified plugin packages

插件 MUST 以三个文件发布，放在同一目录下：`plugin.json`（format 2，只含指向 `{ format, manifest, bundle? }`，两个路径都是同目录或子目录的相对路径，且 MUST NOT 含摘要、文件清单或 manifest）、`mf-manifest.json`（`{ format: 1, manifest, files: [{ path, size, sha256 }] }`）与官方包必需的 `plugin.bundle.json`（来源证明）。内核 MUST 继续接受 format 1 的 `plugin.json`（内嵌 manifest 与 files，无证明），但只用于 GitHub 来源。

#### Scenario: The envelope only points at the manifest

- **WHEN** 内核读取 format 2 的 `plugin.json`
- **THEN** 它只得到 `manifest` 与可选 `bundle` 两个路径，文件清单与摘要只来自 `mf-manifest.json`
- **AND** 改动 `plugin.json` 的指向不改变 `mf-manifest.json` 的字节，反之亦然

#### Scenario: A format 1 package from the official source

- **WHEN** 官方来源的 `plugin.json` 是 format 1（内嵌 manifest 与 files）
- **THEN** 安装以 `package-invalid` 失败

### Requirement: Package contents are validated

内核 MUST 以 `package-invalid` 拒绝未知字段、不支持的 format、空文件清单、重复路径、非安全相对路径、非法大小与非法哈希；manifest 问题 MUST 沿用 `plugin-host` 的 `manifest-invalid` 与 `api-version-mismatch`。超过上限（5000 个文件、单文件 256 MiB、合计 1 GiB）MUST 以 `too-large` 拒绝。`role: "manager"` 的包 MUST NOT 含 `sw.js`、`__kernel/**`、`__plugins/**`、`__ca/**`、`registry/**` 或已登记前缀下的路径。

#### Scenario: A path could escape the file list

- **WHEN** 文件清单含空段、`.`/`..` 段、控制字符、`\ ? # % : * " < > |` 之一，或路径超过 512 字符
- **THEN** 包以 `package-invalid` 被拒绝，不下载任何文件

#### Scenario: A package is too large

- **WHEN** 文件清单超过 5000 项，或单个文件超过 256 MiB，或合计超过 1 GiB
- **THEN** 包以 `too-large` 被拒绝

#### Scenario: A manager tries to shadow a reserved URL

- **WHEN** 管理器包的文件清单含 `__kernel/x.js` 或 `sw.js`
- **THEN** 包以 `package-invalid` 被拒绝

### Requirement: Downloaded files are verified before use

下载的每个文件 MUST 在写入缓存之前核对大小与 SHA-256。任何不符 MUST 以 `integrity-failed` 终止本次安装，且不得影响已安装的插件。

#### Scenario: A file does not match its hash

- **WHEN** 安装过程中某个文件的字节与 `mf-manifest.json` 记录的哈希不符
- **THEN** 安装以 `integrity-failed` 失败
- **AND** 已安装的状态与文件保持原样，本次下载的半成品被删除

### Requirement: Lazy files are fetched on first request

文件条目 MAY 带 `lazy: true`，但插件 MUST 至少有一个非 lazy 文件。安装 MUST 只下载并校验非 lazy 文件；lazy 文件 MUST 在第一次被请求时才从原来源下载，校验大小与 SHA-256 通过后才写入并应答，同一文件的并发请求 MUST 共用一次下载。判断本地文件是否完好 MUST 只看非 lazy 文件。官方插件的 lazy 文件以 `fetch-failed` 失败时，内核 MUST 立即检查注册表，有新版本则安装并改从新版本应答；其余失败 MUST 返回 503 且不写入任何内容。

#### Scenario: Install skips lazy files

- **WHEN** 包中的 `engines/glue.js` 标记为 lazy，管理器被安装
- **THEN** 安装期间服务器没有收到对它的请求
- **AND** 页面第一次请求它时才下载，之后的请求由本地应答

#### Scenario: A lazy file fails verification

- **WHEN** 服务器返回的字节与 `mf-manifest.json` 记录的哈希不符
- **THEN** 该请求得到 503，缓存中没有写入这个文件
- **AND** 服务器修正后，下一次请求会重新下载并成功

#### Scenario: The server no longer hosts the installed version

- **WHEN** 站点重新部署，已安装官方管理器某个没取过的 lazy 文件所在目录已不存在，注册表里已有新版本
- **THEN** 内核立即安装新版本，并用新版本的同路径文件应答
- **AND** 注册表里没有新版本时，该请求得到 503

### Requirement: Two plugin sources

插件来源 MUST 只有两种：`official:<id>`（本站 `/registry/index.json` 中的条目）与 `github:<owner>/<repo>@<tag>`（`raw.githubusercontent.com` 上该标签的文件）；其他写法 MUST 以 `source-invalid` 拒绝。GitHub 来源 MUST 以标签定位文件。已安装的插件 MUST NOT 被另一个来源的同 id 包替换（`source-conflict`）；同一 GitHub 仓库换标签视为同一来源，即更新。

#### Scenario: An unknown source is rejected

- **WHEN** 来源写作 `https://example.com/p` 或 `github:alice/m`（缺标签）
- **THEN** 请求以 `source-invalid` 被拒绝，不发出任何网络请求

#### Scenario: Another source tries to take an installed id

- **WHEN** `alice.manager` 已从 `github:alice/m@v1` 安装，又从 `github:mallory/m@v1` 安装同 id 的包
- **THEN** 安装以 `source-conflict` 失败，已安装的插件不变

### Requirement: Trust is decided by source

信任 MUST 由来源决定，不由 manifest 自称。官方来源的包 MUST 声明 `provenance: "official"`，id 与版本 MUST 与注册表条目一致，`mf-manifest.json` 的 SHA-256 MUST 等于注册表记录的 `digest`。GitHub 来源的包 MUST 声明 `provenance: "third-party"`，且 id MUST NOT 以 `online-gal.` 开头。

#### Scenario: A GitHub plugin claims to be official

- **WHEN** 从 `github:alice/m@v1` 安装的包声明 `provenance: "official"`，或使用 `online-gal.manager` 作为 id
- **THEN** 安装分别以 `provenance-mismatch` 与 `reserved-id` 失败

#### Scenario: The registry digest does not match

- **WHEN** 官方注册表记录的 `digest` 与下载到的 `mf-manifest.json` 不符
- **THEN** 安装以 `integrity-failed` 失败，不下载任何插件文件

### Requirement: Atomic install, update and uninstall

安装与更新 MUST 原子化：新版本在全部文件下载并校验完成、状态写入成功之前，旧版本 MUST 继续应答；任何一步失败 MUST 让已安装状态与文件完全保持原样。同一个包（内容清单 `mf-manifest.json`，format 1 为 `plugin.json`，摘要相同且本地文件完好）再次安装 MUST NOT 重新下载。更新后，上一代文件 MUST 在仍有页面打开时保留，供更新前加载的页面继续取用它引用的旧文件；没有页面打开时 MUST 被清理，且同一时间最多保留一代旧文件。无状态引用的插件缓存（例如被中断的安装）MUST 在一小时后被清理。卸载当前管理器 MUST 以 `manager-in-use` 拒绝。修改状态的操作 MUST 逐个执行。

#### Scenario: An update fails halfway

- **WHEN** 官方插件的新版本在下载过程中出现网络错误
- **THEN** 已安装的版本继续应答，状态里的版本号不变

#### Scenario: A page loaded before an update asks for an old file

- **WHEN** 管理器更新时仍有页面打开，该页面请求旧版本独有的文件
- **THEN** 内核从上一代文件应答
- **AND** 所有页面关闭后上一代文件被删除

### Requirement: The active manager is the site

内核 MUST 记录一个当前管理器；只有 `role: "manager"` 的插件 MAY 被激活（`not-a-manager`）。导航时没有可用管理器时，内核 MUST 先改用已安装的其他管理器，再不行则安装并激活默认官方管理器 `online-gal.manager`；Service Worker 安装阶段 MUST 预先执行同样的步骤，但失败 MUST NOT 阻止内核启用。切换管理器 MUST 只改变本地状态，之后的导航立即由新管理器应答，服务器不参与。管理器的本地文件被浏览器清除时，导航 MUST 从它原来的来源重新下载后应答。

#### Scenario: First visit to a deep link

- **WHEN** 首次访问 `/settings/`
- **THEN** 服务器直接返回官方管理器的 `/settings/` 静态页，页面 `load` 之后才注册 `/sw.js`，首屏不等待内核
- **AND** 内核在后台安装默认管理器，之后的刷新与导航由内核从本地应答

#### Scenario: Switching to a third-party manager

- **WHEN** 用户从 `github:alice/m@v1` 安装第三方管理器并激活
- **THEN** 之后的导航由第三方管理器应答，服务器不收到任何本源请求
- **AND** 切回官方管理器后可以卸载第三方管理器

#### Scenario: Stored files were evicted

- **WHEN** 当前管理器的缓存被浏览器删除后发生导航
- **THEN** 内核从原来源重新下载并应答该页面

### Requirement: Official updates in the background

内核 MUST 在导航时于后台检查官方注册表，两次检查之间 MUST 至少间隔 6 小时；检查 MUST 只更新已安装、来源为官方且摘要与注册表不同的插件。第三方插件 MUST 只在用户要求时改变（安装另一个标签）。单个插件更新失败 MUST 只记录，不影响其他插件，也不影响页面应答。

#### Scenario: An official plugin has a new version

- **WHEN** 距上次检查超过间隔，注册表中官方管理器的摘要变了
- **THEN** 新版本在后台安装，下一次导航由新版本应答
- **AND** 已安装的第三方插件不被改动

### Requirement: Kernel API, kernel page and recovery

内核 MUST 在 `/__kernel/api/` 提供：GET `state`、GET `registry`、POST `install { source, activate? }`、`uninstall { id }`、`activate { id }`、`update`、`reset`。POST MUST 要求 `content-type: application/json` 与对象请求体；以导航方式到达的 API 请求（跨站表单）MUST 以 `request-invalid` 拒绝。失败响应 MUST 为 `{ ok: false, error: { code, message } }`，并带与错误码对应的 HTTP 状态。内核 MUST 在 `/__kernel/` 提供不依赖任何插件的管理页面。内核应答导航失败时 MUST 返回不依赖任何插件的恢复页，显示错误码并提供重试与重置。`reset` MUST 删除内核的全部状态与插件缓存，之后页面注销 Service Worker 并回到站点根的静态页面。

#### Scenario: No manager can be installed

- **WHEN** 首次导航时官方注册表不可达
- **THEN** 页面显示恢复页与错误码 `fetch-failed`，而不是空白页或浏览器错误页

#### Scenario: A cross-site form posts to the kernel API

- **WHEN** 一个导航请求以 POST 到达 `/__kernel/api/install`
- **THEN** 内核以 400 与 `request-invalid` 拒绝，不安装任何东西

### Requirement: Site output layout

构建 MUST 产出可直接部署到 Cloudflare Pages 的目录：站点根的官方管理器静态页（每个 `.html` 带一段 `load` 之后注册 `/sw.js` 的脚本，其余文件字节不变）、`sw.js`、`_headers`、`_routes.json`（只让 `/__ca/*` 与已登记 Function 的前缀调用 Function）与官方注册表 `registry/`；微型 CA 的 Function 在仓库的 `functions/__ca/`，已登记的 Function 在 `functions/__<name>/`，都与该目录一起部署。注册表里的插件包 MUST NOT 含这段脚本，使包的摘要与内核校验的字节一致。站点 MUST NOT 含顶层 `404.html`，使服务器对任何未知路径都返回根 `index.html`。`sw.js` MUST 是 classic script。

#### Scenario: Only the site root registers the worker

- **WHEN** 构建站点
- **THEN** 站点根的每个 `.html` 页面含一份 `/sw.js` 注册脚本，`registry/` 下的任何文件都不含

#### Scenario: Unknown paths fall back to the root page

- **WHEN** 构建站点
- **THEN** 输出目录没有顶层 `404.html`

### Requirement: Site headers and caching

`_headers` MUST 给全站加上与内核应答相同的跨源隔离响应头。`/registry/index.json` MUST 以 `Cache-Control: no-cache` 发布；`/sw.js` 不要求 `no-cache`，内核的更新检查 MUST 靠注册时的 `updateViaCache: 'none'`（浏览器检查更新时绕过 HTTP 缓存），因此 CDN 或 zone 层对 `/sw.js` 的缓存不影响内核更新；`/registry/p/**` 的路径 MUST 含覆盖整个目录（含 `plugin.json`、`mf-manifest.json` 及存在时的 `plugin.bundle.json`）的内容哈希，并以 `immutable` 长缓存发布。同一个这样的 URL MUST 永远只对应一份内容：只改 `plugin.json`（format 1 升到 format 2、补上 `bundle` 指针）也 MUST 换一个新目录，MUST NOT 覆盖旧 URL。

#### Scenario: The headers file matches what the kernel serves

- **WHEN** 构建站点
- **THEN** `_headers` 的 `/*` 规则含与内核应答相同的 COOP、COEP 与 `nosniff`
- **AND** `/registry/index.json` 为 `no-cache`，`/registry/p/*` 为 `immutable`

#### Scenario: A plugin.json-only change gets a new directory

- **WHEN** 两次构建同一个官方包，文件清单不变、只有 `plugin.json` 的字节不同
- **THEN** 两次的目录哈希不同，注册表指向两个不同的路径

#### Scenario: Only the signed bundle changes

- **WHEN** 两次签名构建发布相同版本的 manifest，但生成的 `plugin.bundle.json` 字节不同
- **THEN** 两次部署的注册表指向不同的包目录

#### Scenario: The worker script is checked past the HTTP cache

- **WHEN** 站点页面注册 `/sw.js`
- **THEN** 注册选项带 `updateViaCache: 'none'`
- **AND** 不要求 `/sw.js` 的响应带 `no-cache`

### Requirement: Plugin packing

官方插件的版本 MUST 带文件清单哈希后缀。文件清单的内容改变时，版本 MUST 随之更新。打包工具 MUST 用与内核相同的 `parsePackage` 校验产物，使打包成功的包一定能被内核接受；第三方作者 MUST 能用同一工具打包自己的仓库。

#### Scenario: The manager is rebuilt

- **WHEN** 管理器源码改变后重新构建站点
- **THEN** 注册表中管理器的版本与路径都改变，旧路径的长缓存不影响新版本

#### Scenario: A packed package is accepted by the kernel

- **WHEN** 用 `pack-plugin.mjs` 打包一个目录并成功
- **THEN** 产物已经通过内核使用的同一份 `parsePackage` 校验，内核安装它不会因格式被拒绝

### Requirement: The microca channel is verified in a fixed order

`channel` 为 `microca` 的 `plugin.bundle.json` MUST 依次确认：DNS TXT 白名单（经至少一个报告 AD=true 的 DoH 解析器取得，多个解析器作答时内容一致）；`cred` 的 ES256 签名（用 bundle 内联的 CA 公钥）与不超过 600 秒的有效期；`SHA-256("microca|" + ca_fingerprint + "|" + anon_id + "|" + salt)` 在白名单内且满足该项的时间窗；Rekor 条目与摘要、签名、临时公钥一致，SET 与 inclusion proof 有效，`integratedTime` 落在凭证有效期内；`sig` 是临时公钥对 mf-manifest.json 摘要的签名。

#### Scenario: The manifest is changed after signing

- **WHEN** mf-manifest.json 的字节与签名时不同，即使 plugin.json 被同步改过
- **THEN** 安装以 `attestation-invalid` 失败，不下载任何插件文件

#### Scenario: The signer is not on the allowlist

- **WHEN** 哈希不在 TXT 白名单，或 `integratedTime` 在该项时间窗之外
- **THEN** 安装以 `attestation-invalid` 失败

### Requirement: Official packages carry a verified attestation

官方来源的包 MUST 带 `plugin.bundle.json` 并通过其所属通道的验证，否则 MUST 以 `attestation-invalid` 拒绝，且 MUST NOT 降级为第三方或替换已安装的版本。DNS 不可达时的行为由受限模式一条规定。

#### Scenario: DNS cannot be reached

- **WHEN** 所有 DoH 解析器都不可达，用户安装官方管理器
- **THEN** 安装成功，状态为 `unverified`，不验证 bundle
- **AND** 已安装的版本继续应答

#### Scenario: An official package has no bundle

- **WHEN** 官方来源的包没有 `plugin.bundle.json`
- **THEN** 安装以 `attestation-invalid` 失败

### Requirement: DNS unreachable puts the kernel in restricted mode

取不到 DNS 信任记录（所有 DoH 解析器都不可达且没有未过期的缓存）时，内核 MUST 进入受限模式：只安装官方来源的插件，不验证 bundle，状态标 `unverified`；`github:` 来源 MUST 以可重试的 `attestation-unavailable` 拒绝。DoH 恢复后的后台检查 MUST 补验 `unverified` 的官方插件：通过标 `verified`，失败标 `revoked`。

#### Scenario: A GitHub source is refused in restricted mode

- **WHEN** 所有 DoH 解析器都不可达，用户安装 `github:alice/m@v1`
- **THEN** 安装以 `attestation-unavailable` 失败，不下载任何插件文件

#### Scenario: DNS comes back

- **WHEN** DoH 恢复后，后台检查验证一个 `unverified` 的官方插件
- **THEN** bundle 有效则状态改为 `verified`，无效或缺失则改为 `revoked`

### Requirement: GitHub bundles are optional and never raise trust

GitHub 来源的 `plugin.bundle.json` 可选；存在但无效 MUST 以 `attestation-invalid` 拒绝，有效也 MUST NOT 改变 `provenance` 与保留 id 规则。

#### Scenario: A GitHub bundle that fails verification

- **WHEN** 从 GitHub 标签安装的包带 `plugin.bundle.json`，但验证失败
- **THEN** 安装以 `attestation-invalid` 失败，而不是当作没有 bundle 继续

#### Scenario: A valid bundle does not make a GitHub plugin official

- **WHEN** 从 `github:alice/m@v1` 安装的包带有效的 bundle，并声明 `provenance: "official"` 或使用 `online-gal.manager` 作为 id
- **THEN** 安装分别以 `provenance-mismatch` 与 `reserved-id` 失败

### Requirement: The attestation channel selects the verification branch

`plugin.bundle.json` MUST 带 `channel`，取值 `github`（内联 GitHub 生成的 Sigstore bundle）或 `microca`；未知值 MUST 以 `attestation-invalid` 拒绝。两条通道 MUST 共用同一条 DNS TXT（`v=1; salt=<b64url>; hashes=<项>,<项>`），各自算出带通道前缀的承诺哈希，命中白名单才算官方。

#### Scenario: An unknown channel

- **WHEN** 官方包的 `plugin.bundle.json` 带 `channel: "other"` 或没有 `channel`
- **THEN** 安装以 `attestation-invalid` 失败，不下载任何插件文件

#### Scenario: One channel's hash cannot stand in for the other

- **WHEN** 白名单只含某个 `microca` 承诺哈希，而包走 `github` 通道
- **THEN** `github` 通道算出的哈希因前缀不同而不命中，安装以 `attestation-invalid` 失败

### Requirement: The github channel is verified against the Sigstore public root

`github` 通道 MUST 验证内联的 Sigstore bundle：证书链到内置的 Sigstore 公共信任根，Rekor 的 `integratedTime` 落在证书有效期内，签名有效，subject 摘要等于 mf-manifest.json 的 SHA-256。`issuer` MUST 是 `https://token.actions.githubusercontent.com`，ref MUST 是 `refs/tags/*`。承诺哈希 MUST 为 `SHA-256("github|" + issuer + "|" + workflowUri + "|" + salt)`，`workflowUri` MUST 去掉 `@<ref>` 后缀。

#### Scenario: A new tag keeps the same commitment

- **WHEN** 同一工作流分别在 `refs/tags/v1` 与 `refs/tags/v2` 下签出官方包
- **THEN** 两者的 `workflowUri` 相同，承诺哈希相同，DNS 白名单无需改动

#### Scenario: A branch build is rejected

- **WHEN** attestation 来自 `refs/heads/main` 的工作流运行
- **THEN** 安装以 `attestation-invalid` 失败

### Requirement: Attestation stays off the request path

证明与 DNS 的检查 MUST 只在安装与后台更新时进行，结果写入状态；请求应答 MUST NOT 做任何证明或 DNS 工作。后台检查 MUST 重新取 DNS，已安装官方插件的哈希不再有效时 MUST 标记为 `revoked`，在 `state` API 中可见，并拒绝再安装该 bundle。

#### Scenario: A hash is removed from DNS

- **WHEN** 后台检查发现已安装官方管理器的哈希已不在白名单
- **THEN** 该插件在状态中标记为 `revoked`，导航仍照常应答
- **AND** 再次安装同一 bundle 以 `attestation-invalid` 失败
