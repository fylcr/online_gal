# Proposal

## Why

文件系统需要通过 OpenList-Worker 读取其挂载的远程游戏文件。上游的目录、元数据和下载接口可能独立变化，项目需要每日检查上游并运行接口测试，提前发现现网兼容性问题。

## What Changes

- 接入 [OpenListTeam/OpenList-Worker](https://github.com/OpenListTeam/OpenList-Worker)，通过上游的自托管 Node.js 入口提供远程游戏目录，作为 VFS 只读来源。存档和游戏运行期间的写入继续进入本地覆层。
- 每日解析官方仓库默认分支的当前提交，验证隔离候选和生产 OpenList 服务的实际接口。即使上游提交未变也执行接口检查。
- 兼容性检查决定候选版本能否发布。通过的候选进入正常更新流程，失败时告警并保留最近通过验收的生产版本，修复后继续更新。提交 SHA 用于追溯测试和发布，依赖选择始终跟踪官方上游。

## Capabilities

### New Capabilities

- `openlist-remote-source` 提供 OpenList-Worker 只读远程挂载、已使用接口的契约测试和滚动更新保障。

### Modified Capabilities

无。

## Impact

- `packages/vfs` 新增远程 `GameSource`，`packages/core` 将其接入现有来源打开路径。
- 配置与运维涵盖独立 Node.js 服务、受限的测试存储、每日 GitHub Actions 检查和部署记录。文件系统会话负责改动与验收，共享接入点由现有负责人协调。
