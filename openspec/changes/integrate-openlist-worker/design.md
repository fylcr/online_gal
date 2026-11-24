# Design

## Context

`packages/engine-api/src/types.ts` 定义 `GameSource` 的目录枚举、整文件读取、范围读取、类型与大小查询。`packages/core/src/library.ts` 的 `openSource()` 打开来源。`packages/vfs/src/host-source.ts` 已将 `GameSource` 接进 KiriKiri 的字节读取路径。新来源沿用这些边界和现有可写覆层。

上游 `src/backend/worker.ts` 装配了 Cloudflare 入口和 Durable Object。`src/backend/server/fs.ts` 的 `POST /api/fs/list` 和 `POST /api/fs/get` 分别提供列表与元数据，当前使用 `content`、`total`、`size`、`is_dir` 和 `raw_url` 等字段，HTTP 200 也可能携带失败的 JSON `code`。这些文件是首次验证的代码定位。日检使用当次候选的实际接口与实现。

## Goals / Non-Goals

提供在浏览器 Service Worker 中运行的远程游戏读取，沿用现有引擎和 VFS 打开流程。每天跟踪官方上游，验证候选与生产接口。当前交付范围为目录列表、元数据和文件读取。CORS 配置、预检和跨源存储兼容性留到后续接入时验证。

## Decisions

### 1. 复用现有浏览器 Service Worker

在 `packages/vfs` 增加浏览器运行时适配，从候选提交编译实际使用的 OpenList 列表、元数据、所选驱动或读取逻辑。按需把状态、配置和下载能力通过浏览器适配层注入，例如用 IndexedDB 持久化挂载配置。第一步验证至少一条实际复用上游代码且面向远程存储的列表、元数据、下载与 Range 调用链。用 Chromium 找出 Node 内置模块、Cloudflare 绑定、DOM API 等不兼容依赖。无法直接编译的分支使用浏览器实现替换，并记录仍在使用的上游模块、适配范围和验证结果。

经插件系统 K19 登记 `/__openlist` 为纯内核路由，静态打包进已有的根作用域 classic `/sw.js`。内核负责登记、路由分发和校验受控客户端，文件系统负责路由内的读取。列表和元数据入口为 `POST /__openlist/api/fs/list`、`POST /__openlist/api/fs/get`，文件经同前缀的只读字节端点流式传输。来源仅在当前页面已受站点 Service Worker 控制后启用。初始化、更新和暂不可用状态走明确的 VFS 错误。Pages 承载静态制品，此前缀不加入 `_routes.json`。测试用不受控浏览器直接访问该路由，核对它未作为服务端文件 API 执行。

### 2. VFS 与远程存储

`packages/vfs/src/openlist-source.ts` 用挂载标识和规范化游戏根调用 Service Worker。`packages/core/src/types.ts` 和 `packages/core/src/library.ts` 保存并恢复来源。路由只按已配置的挂载、协议、来源和根目录读取。每次请求检查根边界，列表在分页结束前枚举所有直接子项，分页不前进或响应不一致时明确失败。分别解析 HTTP 状态与 JSON `code`。只有明确的文件不存在可作为不存在处理。存档和运行时写入沿用本地覆层，并回归本地来源。

首轮选用上游 `src/backend/drivers/url_tree/` 的 URL Tree 驱动作为验收夹具。每个部署发布一份含多页目录、嵌套路径、Unicode 名称和已知大小的只读目录描述，以及对应的合成二进制文件。根据部署自身的 origin 解析文件 URL，驱动读取目录与元数据，Service Worker 向同源静态文件发起实际 HTTPS 下载和 Range 请求。隔离与生产使用相同夹具和套件。生产日检走线上部署实际接管页面的 Service Worker。

授权和目标校验仍按挂载契约执行。有读取授权的挂载使用短期或仅限游戏根的读取权限，发送授权或使用下载 URL 前校验目标。挂载描述、构建和日志中不含存储管理凭据。下载签名过期时按记录的已配置挂载重新取得元数据。授权拒绝与签名过期用隔离夹具覆盖。

使用已知字节夹具实测 `Range`。随机读取校验 `206`、`Content-Range`、实际长度和测试字节。上游忽略 Range 或范围不一致时报告能力或读取错误。整文件和允许的降级读取设定字节预算，流超过预算就取消。Service Worker 经过空闲终止后，重新发起列表及读取，验证可按挂载配置恢复。

#### 外部来源实测

2026-11-24 UTC，在 Edge 的 `https://onlinegal.de5.net/` 页面发起只读请求，测试 `https://pan.t-satoru.top/s3b/TP`。

- OpenList 目录和元数据接口均返回 HTTP `200`、JSON `code: 200`。`/api/fs/list` 报告 145 个目录，`page: 2, per_page: 3` 返回 3 项。`/s3b/TP/ATRI` 下选取的文件在列表和 `/api/fs/get` 中均为 `1633575957` 字节。
- 对元数据返回的跨源下载地址请求 `Range: bytes=16-47`。存储返回 `206` 和 `Content-Range: bytes 16-47/1633575957`。直接 HTTP 请求读到 32 字节，Online Gal 页面上的 `fetch` 因下载响应缺少 `Access-Control-Allow-Origin` 而报 `MissingAllowOriginHeader`。
- `/s3b/TP` 作为跨源联调样本。复测以 Online Gal 浏览器能读取 `206`、`Content-Range` 和对应字节为准。同源 URL Tree 夹具用于首轮验收和每日检查。

### 3. 每天跟踪官方默认分支

使用独立的 GitHub Actions 定时及手动入口。每个 UTC 日依据官方仓库元数据解析当前默认分支和 HEAD。用该 SHA 构建包含适配器的静态候选站点，或复用同时匹配上游 SHA、Online Gal 构建输入和制品摘要的候选。下一次运行重新解析 HEAD。上游查询失败时仍执行生产检查，保留两个目标各自的结果。

在隔离 HTTPS 地址与线上站点分别打开真实 Chromium，等待对应的 `/sw.js` 控制测试页，通过正常 VFS 调用检查列表、元数据和文件字节。校验实际控制客户端的版本标识，避免旧 Service Worker 或缓存为失败候选产生通过结果。每天记录 UTC 时间、上游默认分支和 SHA、Online Gal 构建输入、制品摘要、实际运行的 Service Worker 版本和每项检查结果。失败发出告警，缺跑由独立检查发现，支持手动补跑。

### 4. 发布与恢复

构建未经验证的上游候选与正式发布使用不同权限。发布任务核对当次已验收的 `/sw.js` 以及 OpenList 运行时依赖的静态制品摘要，沿用现有 Pages 静态发布流程。若该流程需要重建，发布前核对与验收制品的摘要一致。生产串行发布并立即复测，覆盖新浏览器和已有旧 Service Worker 的升级。复测失败时恢复最近一次生产验收通过的制品，验证浏览器已激活恢复版本后再记录恢复成功。候选失败时继续每日跟踪上游，修复适配后重跑。

随制品保留上游来源、许可和修改记录。针对上游仓库当前的 AGPL-3.0 许可与项目既有 GPL-3.0-or-later 规则，核对实际分发的组合制品适用条款，按分发要求提供该版本对应源码，包括适配和构建所需文件。

### 5. 实施归属

`file-system` 会话负责 OpenList 适配、VFS 接口、浏览器测试和每日巡检。插件系统负责人接入 K19 登记并核对 `/sw.js` 构建与更新。现有 `packages/core` 或 `apps/app` 的负责人按 VFS 契约接入和验收。共享发布和相关文件协调后修改。

## Migration Plan

1. 用最小静态候选和同源 URL Tree 夹具验证复用的上游模块确实可在浏览器 Service Worker 中运行。
2. 接入内核路由、GameSource、来源恢复和本地覆层，整理候选与生产共用的浏览器契约测试。
3. 启用每日巡检与告警。门禁通过后按现有 Pages 静态发布流程上线，记录首个可恢复版本。
