# Tasks

本变更接替 `integrate-openlist-worker`，范围为新增 WebDAV 目录及撤销 OpenList 计划。直链与其他来源的回归见 2.4。

## 1. 实测 WebDAV 跨源读取

- [ ] 1.1 配置独立 HTTPS WebDAV 的只读合成夹具，包含嵌套目录、Unicode 名称、空目录及已知字节的文件。记录站点来源、预检允许的方法与请求头，以及暴露的响应头。使用隔离 HTTPS 站点的真实 Chromium 验证 `PROPFIND` 的 `207`、整文件和中段 `206` 的内容，并用一份缺少 CORS 头的响应验证探针会失败。
- [ ] 1.2 在 VFS 实现 WebDAV 客户端。以 `Depth: 0/1`、`DAV:` XML、逐条 `propstat`、根外 `href`、缺失属性及错误状态夹具验证目录和元数据。覆盖只读授权、直接下载、失效授权、意外重定向、缺失或错误的 `Content-Range`、字节不符与超限取消。

## 2. 接入文件系统和浏览器内核

- [ ] 2.1 文件系统提供浏览器处理模块，插件系统按 K19 接入 `/__webdav`，更新 `packages/plugin-system/src/format.ts`、`packages/plugin-system/src/kernel.ts` 和 `packages/plugin-system/scripts/build-site.mjs` 的登记。验证受控页面正常读取、等待接管时可重试、空闲中止后恢复，及 Pages 服务器路由没有暴露文件接口。核对部署制品与工作流没有遗留的 `/__openlist` 接入或 OpenList 跟踪任务。
- [ ] 2.2 将 WebDAV `GameSource` 接入来源选择、保存和重开，验证同一会话能在 Service Worker 重启后继续使用挂载，重新打开时可按需授权。检查日志和持久化记录不含敏感授权值。
- [ ] 2.3 通过正常站点入口打开 WebDAV 合成游戏，覆盖目录和归档字节读取、随机范围及本地覆层。检查写入后远程文件字节与原始夹具一致。
- [ ] 2.4 确认直链、本地目录、文件导入及其他来源的入口，按原流程实测可用的来源。直链以跨源 HTTPS 合成文件验证，记录走原有下载契约的实际请求。若目标构建中尚无直链入口，记录它的归属与待验项，待可用后补测。

## 3. 日检与发布门禁

- [ ] 3.1 配置 GitHub Actions 每日定时和手动执行。对隔离候选与生产分别由实际接管页面的 Service Worker 读取独立 WebDAV 夹具。演练站点提交未变的一次日检及失败、缺跑和手动补跑，记录 UTC 时间、制品摘要、Service Worker 版本和各项结果。
- [ ] 3.2 接入 Pages 静态发布门禁，演练候选不兼容时被拦下、发布制品与验收摘要一致，以及站点制品回归后恢复上一个通过的版本并在浏览器复测。

## 4. 联调验收

- [ ] 4.1 记录首次生产部署、隔离候选和生产的实际 WebDAV 响应与字节，以及旧版 Service Worker 客户端更新后的结果。取得一次定时日检对两个目标均发出真实请求的记录。
- [ ] 4.2 按团队约定协调共享构建，运行相关测试、`npm run check` 和 `openspec validate integrate-webdav-source --strict`。把联调证据及未通过的来源写回本节。
