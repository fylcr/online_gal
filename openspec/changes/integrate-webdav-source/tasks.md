# Tasks

本变更在 `packages/vfs` 增加只读网络文件挂载，首个协议为 WebDAV，接替 `integrate-openlist-worker`。直链与其他来源的回归见 2.4。

## 1. 实测 WebDAV 跨源读取

- [ ] 1.1 配置独立 HTTPS WebDAV 的只读合成夹具，包含嵌套目录、Unicode 名称、空目录及已知字节的文件。记录站点来源、预检允许的方法与请求头，以及暴露的响应头。使用隔离 HTTPS 站点的真实 Chromium 验证 `PROPFIND` 的 `207`、整文件和中段 `206` 的内容，并用一份缺少 CORS 头的响应验证探针会失败。
- [ ] 1.2 在 `packages/vfs` 实现并导出网络挂载入口，以 WebDAV 适配器返回现有 `GameSource`。验证从 VFS 直接调用目录列表、存在性、文件类型、大小、整文件和范围读取。以 `Depth: 0/1`、`DAV:` XML、逐条 `propstat`、根外 `href`、缺失属性及错误状态夹具验证目录和元数据。覆盖只读授权、直接下载、失效授权、意外重定向、缺失或错误的 `Content-Range`、字节不符与超限取消。

## 2. 接入文件系统和浏览器内核

- [ ] 2.1 文件系统提供浏览器处理模块，插件系统按 K19 接入 `/__webdav`，更新 `packages/plugin-system/src/format.ts`、`packages/plugin-system/src/kernel.ts` 和 `packages/plugin-system/scripts/build-site.mjs` 的登记。验证受控页面正常读取、等待接管时可重试、空闲中止后恢复，及 Pages 服务器路由没有暴露文件接口。核对部署制品与工作流没有遗留的 `/__openlist` 接入或 OpenList 跟踪任务。
- [ ] 2.2 在现有来源选择接入「网络文件」，通过 `packages/core` 的来源描述和 `openSource()` 打开、保存和恢复 VFS 挂载。验证同一会话能在 Service Worker 重启后继续使用挂载，重新打开时可按需授权。检查日志和持久化记录不含敏感授权值。
- [ ] 2.3 通过网络文件入口打开 WebDAV 合成游戏，用现有引擎宿主读取接口和归档接口验证按需取目录和文件范围。核对网络记录中，挂载和局部读取只下载所需数据。验证存档写入本地覆层后远程文件字节与原始夹具一致。
- [ ] 2.4 确认直链、本地目录、文件导入及其他来源的入口，按原流程实测可用的来源。直链以跨源 HTTPS 合成文件验证，记录走原有下载契约的实际请求。若目标构建中尚无直链入口，记录它的归属与待验项，待可用后补测。

## 3. 联调验收

- [ ] 3.1 首次交付及相关读取行为变更时，用真实 Chromium 从隔离候选运行目录、元数据、整文件和范围读取验收。相关变更发布后，用新客户端和从旧版 Service Worker 更新的客户端复测，记录实际 WebDAV 响应、字节验证结果和 Service Worker 版本。
- [ ] 3.2 按团队约定协调共享构建，运行相关测试、`npm run check` 和 `openspec validate integrate-webdav-source --strict`。把联调证据及未通过的来源写回本节。
