# Proposal

## Why

文件系统需要从远程目录读取游戏文件。OpenList 实测中目录和元数据可读，下载地址却缺少 CORS 响应头，浏览器无法读取实测的 `206` 字节范围。改用由存储方配置跨域访问的标准 WebDAV。OpenList 试测见提交 `789a7fa`。

## What Changes

- 撤销尚未实现的 OpenList-Worker 接入、`/__openlist` 路由登记及 OpenList 上游更新任务。新增 WebDAV 只读目录来源。
- 保留 HTTP(S) 直链、本地目录、文件导入及其他来源的入口和行为。直链继续按其已有下载契约处理。游戏写入继续使用本地覆层。
- 在真实浏览器验证 WebDAV 的跨源目录、元数据、整文件和 Range 读取。GitHub Actions 每日对隔离候选与生产执行同一套契约检查。

## Capabilities

### New Capabilities

- `webdav-remote-source` 提供只读 WebDAV 游戏目录挂载、浏览器跨域读取及接口巡检。

### Modified Capabilities

无。

## Impact

- `packages/vfs` 提供 WebDAV `GameSource`，`packages/core` 接入来源打开与恢复。文件系统和插件系统负责人协作，通过 K19 将 `/__webdav` 接入现有浏览器 `/sw.js`。
- Pages 静态分发站点。受控浏览器直接访问配置好的 WebDAV 服务，使用可配置 CORS 的独立 HTTPS WebDAV 测试端点做验收。
