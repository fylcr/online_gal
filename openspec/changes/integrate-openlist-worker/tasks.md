# Tasks

## 1. 验证浏览器 Service Worker 兼容性

- [ ] 1.1 解析 `OpenListTeam/OpenList-Worker` 当前官方默认分支与 HEAD，梳理列表、元数据、下载及 URL Tree 驱动的依赖。构建包含实际复用的上游 TypeScript 的最小 classic Service Worker，并在隔离 HTTPS 静态站点用 Chromium 验证它接管并处理一次完整读取。记录使用的上游模块、平台依赖、浏览器替代实现、上游许可和对应源码的提供方式。
- [ ] 1.2 在部署的静态资源中配置同源 URL Tree 夹具，包含多页目录、嵌套路径、Unicode 名称和已知字节的二进制文件。通过 Service Worker 验证列表、元数据、实际下载及中间字节的 `206` 和 `Content-Range`。记录请求和响应，并用隔离夹具检查签名失效及存储拒绝。

外部来源 `https://pan.t-satoru.top/s3b/TP` 的目录、元数据与下载实测见[设计记录](design.md#外部来源实测)。下载端的 CORS 限制阻塞了该地址的浏览器读取，列入跨源联调。

## 2. 接入文件系统与现有内核

- [ ] 2.1 由文件系统实现 OpenList 浏览器适配和只读请求处理，插件系统依 K19 在 `packages/plugin-system/src/kernel.ts` 路由、`packages/plugin-system/src/format.ts` 包保留名及 `packages/plugin-system/scripts/build-site.mjs` 构建保留名接入 `/__openlist`。确认服务端路由不包含该前缀，并验证未接管页面的错误、接管后正常读取以及 Service Worker 被浏览器终止后的恢复。
- [ ] 2.2 在 `packages/vfs` 实现 `GameSource`，并协调 `packages/core` 接入来源保存和重开。验证完整分页、根路径边界、文件和目录元数据、HTTP 和 JSON 错误、下载授权失效、可信目标、`200`/`206`、无效范围和超限取消。
- [ ] 2.3 用正常的站点启动入口打开合成游戏。验证归档读取、随机范围、本地覆层写入和本地来源回归。将 1.2 整理为候选与生产共用的浏览器套件，用缺失或错误的 `Content-Range`、错配字节和旧 Service Worker 验证断言实际会失败。

## 3. 每日检查和滚动发布

- [ ] 3.1 配置 GitHub Actions 每日定时和手动补跑。每次解析官方当前默认分支和 HEAD，构建或复用对应的静态候选制品。分别对隔离候选和生产部署的同源夹具运行 Chromium 套件。验证 HEAD 推进、默认分支改名、HEAD 未变、上游查询失败四种场景，记录两组实际接口结果。
- [ ] 3.2 添加失败通知和独立的缺跑监控。演练一次失败、一次缺跑和一次手动补跑。记录 UTC 时间、上游 SHA、Online Gal 构建输入、候选及生产实际运行的 Service Worker 版本、制品摘要与失败契约项。
- [ ] 3.3 用现有 Pages 静态发布流程接入门禁。演练不兼容候选被拦下、发布制品与验收摘要一致，以及生产检查失败时回退并重新验证现有浏览器的控制版本。验证候选构建无生产发布凭据。

## 4. 联调验收

- [ ] 4.1 记录首次生产静态发布、候选匹配的 Service Worker 摘要、对应源码获取方式和 Chromium 契约复测结果。核对 `/__openlist` 由现有 Service Worker 处理，包含一条旧版本客户端更新证据。取得下次日检对两个目标都发出真实请求的记录，然后写入本节。
- [ ] 4.2 按团队约定协调共享产物，运行相关测试、`npm run check` 和 `openspec validate integrate-openlist-worker --strict`。记录结果并按仓库流程交付。
