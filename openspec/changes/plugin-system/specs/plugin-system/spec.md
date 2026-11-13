# Spec Delta

## Purpose

定义插件系统：一个 Service Worker 内核，充当站点在本地的服务器。页面只注册它一次；之后本源的每个请求都由内核从已安装的插件中应答，当前管理器（manager）就是站点本身。真正的服务器只是插件的来源：官方插件来自本站的官方注册表（Cloudflare Pages），第三方插件来自 GitHub 仓库的标签。插件内部的权限与服务规则由 `plugin-host` capability 规定。

## ADDED Requirements

### Requirement: The kernel answers same-origin requests locally

内核 MUST 拦截本源的请求并从已安装插件的本地副本应答；除下列路径外，MUST NOT 把本源请求转发给服务器：`/sw.js`（内核自身，用于更新）与 `/registry/**`（官方注册表，用于安装与更新）MUST 直接到达服务器；跨源请求 MUST 不被拦截。路由 MUST 为：`/__kernel/api/<name>` → 内核 API；`/__kernel/` → 内核自带页面；`/__plugins/<id>/<path>` → 该插件的文件；其余 GET/HEAD → 当前管理器的文件。非 GET/HEAD 且不是内核 API 的请求 MUST 不被拦截。内核应答的每个响应 MUST 带 `Cross-Origin-Opener-Policy: same-origin`、`Cross-Origin-Embedder-Policy: credentialless` 与 `X-Content-Type-Options: nosniff`，因为引擎需要跨源隔离。

#### Scenario: A reload is served without the server

- **WHEN** 管理器已安装，用户刷新页面
- **THEN** 页面与它引用的全部文件都由内核从本地应答，服务器只收到浏览器自己对 `/sw.js` 的更新检查
- **AND** 页面 `crossOriginIsolated` 为真

#### Scenario: The site keeps working offline

- **WHEN** 管理器已安装后服务器不可达，或 Service Worker 被浏览器停止后重启
- **THEN** 导航仍得到管理器页面

#### Scenario: Kernel script and registry bypass the kernel

- **WHEN** 页面请求 `/sw.js` 或 `/registry/index.json`
- **THEN** 内核不应答，请求原样到达服务器

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

插件 MUST 以 `plugin.json`（format 1）发布：`{ format, manifest, files: [{ path, size, sha256 }] }`，与插件文件放在同一目录下。内核 MUST 以 `package-invalid` 拒绝未知字段、不支持的 format、空文件清单、重复路径、非安全相对路径、非法大小与非法哈希；manifest 问题 MUST 沿用 `plugin-host` 的 `manifest-invalid` 与 `api-version-mismatch`。超过上限（5000 个文件、单文件 256 MiB、合计 1 GiB）MUST 以 `too-large` 拒绝。`role: "manager"` 的包 MUST NOT 含 `sw.js`、`__kernel/**`、`__plugins/**` 或 `registry/**`。

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

- **WHEN** 安装过程中某个文件的字节与 `plugin.json` 记录的哈希不符
- **THEN** 安装以 `integrity-failed` 失败
- **AND** 已安装的状态与文件保持原样，本次下载的半成品被删除

### Requirement: Lazy files are fetched on first request

文件条目 MAY 带 `lazy: true`，但插件 MUST 至少有一个非 lazy 文件。安装 MUST 只下载并校验非 lazy 文件；lazy 文件 MUST 在第一次被请求时才从原来源下载，校验大小与 SHA-256 通过后才写入并应答，同一文件的并发请求 MUST 共用一次下载。判断本地文件是否完好 MUST 只看非 lazy 文件。官方插件的 lazy 文件以 `fetch-failed` 失败时，内核 MUST 立即检查注册表，有新版本则安装并改从新版本应答；其余失败 MUST 返回 503 且不写入任何内容。

#### Scenario: Install skips lazy files

- **WHEN** 包中的 `engines/glue.js` 标记为 lazy，管理器被安装
- **THEN** 安装期间服务器没有收到对它的请求
- **AND** 页面第一次请求它时才下载，之后的请求由本地应答

#### Scenario: A lazy file fails verification

- **WHEN** 服务器返回的字节与 `plugin.json` 记录的哈希不符
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

信任 MUST 由来源决定，不由 manifest 自称。官方来源的包 MUST 声明 `provenance: "official"`，id 与版本 MUST 与注册表条目一致，`plugin.json` 的 SHA-256 MUST 等于注册表记录的 `digest`。GitHub 来源的包 MUST 声明 `provenance: "third-party"`，且 id MUST NOT 以 `online-gal.` 开头。

#### Scenario: A GitHub plugin claims to be official

- **WHEN** 从 `github:alice/m@v1` 安装的包声明 `provenance: "official"`，或使用 `online-gal.manager` 作为 id
- **THEN** 安装分别以 `provenance-mismatch` 与 `reserved-id` 失败

#### Scenario: The registry digest does not match

- **WHEN** 官方注册表记录的 `digest` 与下载到的 `plugin.json` 不符
- **THEN** 安装以 `integrity-failed` 失败，不下载任何插件文件

### Requirement: Atomic install, update and uninstall

安装与更新 MUST 原子化：新版本在全部文件下载并校验完成、状态写入成功之前，旧版本 MUST 继续应答；任何一步失败 MUST 让已安装状态与文件完全保持原样。同一个包（`plugin.json` 摘要相同且本地文件完好）再次安装 MUST NOT 重新下载。更新后，上一代文件 MUST 在仍有页面打开时保留，供更新前加载的页面继续取用它引用的旧文件；没有页面打开时 MUST 被清理，且同一时间最多保留一代旧文件。无状态引用的插件缓存（例如被中断的安装）MUST 在一小时后被清理。卸载当前管理器 MUST 以 `manager-in-use` 拒绝。修改状态的操作 MUST 逐个执行。

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

构建 MUST 产出可直接部署到 Cloudflare Pages 的目录：站点根的官方管理器静态页（每个 `.html` 带一段 `load` 之后注册 `/sw.js` 的脚本，其余文件字节不变）、`sw.js`、`_headers` 与官方注册表 `registry/`。注册表里的插件包 MUST NOT 含这段脚本，使包的摘要与内核校验的字节一致。站点 MUST NOT 含顶层 `404.html`，使服务器对任何未知路径都返回根 `index.html`。`sw.js` MUST 是 classic script。

#### Scenario: Only the site root registers the worker

- **WHEN** 构建站点
- **THEN** 站点根的每个 `.html` 页面含一份 `/sw.js` 注册脚本，`registry/` 下的任何文件都不含

#### Scenario: Unknown paths fall back to the root page

- **WHEN** 构建站点
- **THEN** 输出目录没有顶层 `404.html`

### Requirement: Site headers and caching

`_headers` MUST 给全站加上与内核应答相同的跨源隔离响应头。`/sw.js` 与 `/registry/index.json` MUST 以 `Cache-Control: no-cache` 发布；`/registry/p/**` 的路径 MUST 含内容哈希，并以 `immutable` 长缓存发布。

#### Scenario: The headers file matches what the kernel serves

- **WHEN** 构建站点
- **THEN** `_headers` 的 `/*` 规则含与内核应答相同的 COOP、COEP 与 `nosniff`
- **AND** `/sw.js` 与 `/registry/index.json` 为 `no-cache`，`/registry/p/*` 为 `immutable`

### Requirement: Plugin packing

官方插件的版本 MUST 带内容哈希后缀，使每次不同的构建都有不同的版本。打包工具 MUST 用与内核相同的 `parsePackage` 校验产物，使打包成功的包一定能被内核接受；第三方作者 MUST 能用同一工具打包自己的仓库。

#### Scenario: The manager is rebuilt

- **WHEN** 管理器源码改变后重新构建站点
- **THEN** 注册表中管理器的版本与路径都改变，旧路径的长缓存不影响新版本

#### Scenario: A packed package is accepted by the kernel

- **WHEN** 用 `pack-plugin.mjs` 打包一个目录并成功
- **THEN** 产物已经通过内核使用的同一份 `parsePackage` 校验，内核安装它不会因格式被拒绝
