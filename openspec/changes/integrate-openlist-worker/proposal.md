# Proposal

## Why

文件系统需要借助 OpenList-Worker 读取远程游戏文件。浏览器 Service Worker 可以在站点内运行这部分 TypeScript。上游的文件接口和驱动会更新，需要每日实测候选与线上版本，先发现变化，再发布更新。

## What Changes

- 将 [OpenListTeam/OpenList-Worker](https://github.com/OpenListTeam/OpenList-Worker) 的只读文件逻辑编译进站点现有的浏览器 Service Worker，通过它为 VFS 提供远程游戏目录。游戏运行期间的写入继续走本地覆层。
- 每个 UTC 日解析官方仓库当前默认分支的 HEAD，用真实浏览器检查候选和线上 Service Worker 的列表、元数据、下载和 Range。上游提交未变时也执行两组检查。
- 候选通过后才进入站点发布流程。失败时报告受影响的接口，保留或恢复最近通过生产验收的版本。SHA 用于追溯当次构建和验收，每日检查重新解析上游。

## Capabilities

### New Capabilities

- `openlist-remote-source` 提供浏览器 Service Worker 内的 OpenList 只读挂载、接口契约检查和滚动更新。

### Modified Capabilities

无。

## Impact

- `packages/vfs` 负责浏览器运行时适配和远程 `GameSource`，`packages/core` 接入现有来源打开路径。现有 `/sw.js` 通过 K19 登记增加 `/__openlist` 路由，由文件系统会话和插件系统负责人协调。
- 候选和生产都使用 Pages 静态站点。选定的远程存储需支持实际的浏览器读取链路。GitHub Actions 负责日检，站点发布沿用现有流程。
