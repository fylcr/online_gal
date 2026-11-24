# Design

## Context

`packages/engine-api/src/types.ts` 定义 `GameSource` 的目录、存在性、大小及字节读取接口。`packages/core/src/library.ts` 的 `openSource()` 在来源之上装配本地写入覆层。`packages/vfs/src/host-source.ts` 将 `GameSource` 接入 KiriKiri 的字节读取路径。

## Goals / Non-Goals

WebDAV 替换原计划的 OpenList 接入。用户选择 WebDAV 目录时使用 WebDAV 挂载契约，HTTP(S) 直链、本地目录、文件导入及其他来源继续各走原有入口。

## Decisions

### 1. 在现有浏览器 Service Worker 中读取

`packages/vfs/src/webdav-source.ts` 将 `GameSource` 的操作交给现有根作用域 `/sw.js` 中登记的 `/__webdav` 内核模块。内核负责分发与调用方校验，VFS 处理挂载和 WebDAV 请求。内部请求携带挂载标识和相对路径。Pages 仅以静态站点分发该能力，`/__webdav` 只在浏览器 Service Worker 中处理。页面等待接管或更新时报告暂不可用并允许重试。

持久化非敏感挂载描述及游戏根。每次访问从已配置的 HTTPS WebDAV 根解析路径，在发送请求和接收 `href` 时检查同源与根边界。使用挂载的最终 URL，意外重定向作为地址配置错误处理。身份信息使用用户提供的只读授权，随挂载隔离，敏感值留在会话内，重新打开时按需重新授权。当前会话的受控页面可在 Service Worker 重启后重新提供挂载和授权。验收记录只保留脱敏的请求信息。

### 2. 实现标准 WebDAV 目录与字节读取

- `PROPFIND` 配合 `Depth: 1` 列出直接子项，`Depth: 0` 获取单项元数据。解析 `207 Multi-Status` 中 `DAV:` 命名空间的 `href`、`resourcetype`、`getcontentlength` 和逐条 `propstat` 状态。使用可在 Service Worker 中运行的有界 XML 解析，验证集合自身、嵌套路径、Unicode、空目录与属性缺失等情况。
- 整文件采用有字节预算的 `GET`。随机读取使用单段 `Range`，校验 `206`、`Content-Range`、实际字节数以及已知的文件大小。忽略 Range、返回无效范围或超限分别按能力或读取错误处理。仅在明确的不存在响应下将文件判为不存在。响应不透明或 `fetch` 未透露原因时提示检查网络或 CORS。
- 将 WebDAV 加到来源选择和 `openSource()` 分支。归档读取走已有字节接口，存档与配置写入走已有本地覆层。直链按既有直链流程选择和读取。当前主线的 `SourceKind` 可见 `directory`、`files`、`local`，联调时沿对应来源确认直链的入口及归属，并如实记录实测覆盖。

### 3. 将跨源读取设为验收条件

用独立 HTTPS WebDAV 服务提供只读合成夹具，包含嵌套目录、Unicode 文件、已知二进制字节和一份合成游戏。站点从隔离候选和生产的实际浏览器来源分别访问。夹具支持无需登录的 OPTIONS 预检，并对配置的候选和生产来源允许 `PROPFIND`、`GET` 与使用到的 `Depth`、`Content-Type`、`Range`、`Authorization` 请求头。实际响应与适用的错误响应返回允许的来源，并在范围响应中暴露 `Content-Range`。只读认证和错误路径在隔离夹具验证。

通过站点正常来源入口打开游戏，由该部署实际控制页面的 Service Worker 获取响应和文件字节。验收同时检查正确的文件内容、区分授权失败、阻止根外路径以及缺少 CORS 许可或范围头时的报错。以浏览器 JavaScript 的可读结果作为判据。

### 4. 每日契约检查与发布

GitHub Actions 提供每日定时和手动入口。每个 UTC 日用真实 Chromium 分别测候选和生产，即使站点提交未变。记录 UTC 时间、站点提交、制品摘要、实际控制页面的 Service Worker 版本和每项结果。任务失败发送告警，缺跑由独立检查发现。

发布沿用 Pages 静态发布流程。候选的读取契约通过后核对正式制品摘要。发布后用新客户端与已受旧版 Service Worker 控制的客户端复测。若站点制品回归，恢复最近通过生产验收的制品并复测。若测试端点不可用，报告端点失败及受影响的契约。

### 5. 实施归属

文件系统负责人维护 WebDAV 适配、VFS、浏览器夹具与日检。插件系统负责人按 K19 登记 `/__webdav` 并接入现有 `/sw.js` 构建。来源入口与持久化由文件系统和现有 `packages/core`、`apps/app` 负责人协作。共享文件和构建按仓库规则协调。

## Risks / Trade-offs

- WebDAV 服务需要有可配置的 CORS 和浏览器可读的 Range 响应。连接验证报告实际失败的阶段，服务配置示例随实现提供。
- WebDAV `207` 可以同时包含成功与失败属性。适配按条目解析状态，拒绝把部分失败或无效 XML 当成完整目录。

## Migration Plan

1. 从隔离 HTTPS 站点实测独立 WebDAV 夹具的跨源目录、元数据和文件读取。
2. 接入内核、VFS 与来源入口，验证其他来源及本地覆层。
3. 启用每日检查与发布门禁，验收首次静态发布和 Service Worker 更新。
