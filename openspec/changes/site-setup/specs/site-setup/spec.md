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

初始化页 MUST NOT 显示任何 token 输入框或校验步骤，MUST NOT 调用 `POST /__setup/verify`。初始化项清单里出现 `source: "input"` 的项时，页面 MUST 停下说明不支持，不写入。写入 MUST 是一次 PATCH：每项写成生产环境的 `secret_text`，`ONLINE_GAL_SITE` 以 `plain_text` 写入生产与预览环境。项目里已有非空的 `ONLINE_GAL_SITE` 时，页面 MUST NOT 再次写入，只能触发重新构建。在本机地址上，页面 MUST NOT 写入或触发构建，只显示预演。

#### Scenario: Reload after writing

- **WHEN** 写入成功后所有者刷新页面并重新登录
- **THEN** 页面直接进入重新构建，不再写入

#### Scenario: Written body carries no token

- **WHEN** 所有者点击"写入并构建"
- **THEN** PATCH 请求体的生产环境只有 `ONLINE_GAL_SITE` 与 `CLOUDFLARE_ACCOUNT_ID`，预览环境只有 `ONLINE_GAL_SITE`，不含 OAuth access token、refresh token 或 GitHub token

### Requirement: Writing triggers a new build and the page hands over

写入后，页面 MUST 按构建时写入的方式触发一次新的构建：`pages-git` 用 create deployment，`github-rerun` MUST NOT 调用 GitHub API，只显示那次工作流的手动重跑入口和推新 `v*` 标签的方法，`manual` 显示命令；失败时 MUST 显示原因和手动方法。新构建成功后，页面 MUST 等到 `/__setup/status` 不再返回 `phase: "setup"`，再回到原来的路径。

#### Scenario: Site build goes live

- **WHEN** 新构建成功，`/__setup/status` 返回站点构建的 HTML
- **THEN** 页面回到所有者最初打开的路径，站点正常工作

### Requirement: Other changes add setup items through the list

需要在初始化时写入的密钥 MUST 登记在 `apps/setup/src/lib/setup-items.ts`，名字 MUST 匹配 `^[A-Z][A-Z0-9_]*$` 且不是 `ONLINE_GAL_SITE`。读取这些 secret 的功能读不到时 MUST 给出明确错误，MUST NOT 在响应或日志中输出它们的值。

#### Scenario: A new item on an initialized site

- **WHEN** 站点初始化后，另一个变更登记了新的初始化项
- **THEN** 该项要在 Cloudflare 控制台手动添加；使用它的功能在缺少时报错
