# Proposal

## Why

文件系统需要从远程目录读取游戏文件。OpenList 实测中目录和元数据可读，下载地址却缺少 CORS 响应头，浏览器无法读取实测的 `206` 字节范围。改用由存储方配置跨域访问的标准 WebDAV。OpenList 试测见提交 `789a7fa`。

## What Changes

- 在 `packages/vfs` 增加只读网络文件挂载，首个协议为 WebDAV。撤销尚未实现的 OpenList-Worker 接入、`/__openlist` 路由登记及 OpenList 上游更新任务。
- 保留 HTTP(S) 直链、本地目录、文件导入及其他来源的入口和行为。直链继续按其已有下载契约处理。游戏写入继续使用本地覆层。
- 在首次交付以及相关读取行为变更时，用真实浏览器验证跨源目录、元数据、整文件和 Range 读取。取消每日检测。

## Capabilities

### New Capabilities

- `webdav-remote-source` 规定 VFS 网络挂载和首个 WebDAV 适配器的读取契约。

### Modified Capabilities

无。

## Impact

- `packages/vfs` 通过现有 `GameSource` 提供按需访问的网络文件。游戏库通过网络文件入口配置、打开和恢复挂载，运行时和归档继续使用既有 VFS 读取接口。浏览器传输由 K19 登记的 `/__webdav` 接入现有 `/sw.js`。
- Pages 静态分发站点。受控浏览器直接访问配置好的 WebDAV 服务，使用可配置 CORS 的独立 HTTPS WebDAV 测试端点做验收。
