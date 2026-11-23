# Spec Delta

## Purpose

让 Online Gal 的虚拟文件系统可读取 OpenList-Worker 提供的远程游戏目录，并持续用真实浏览器和独立部署验证列表、元数据、下载与按字节读取的兼容性。

## ADDED Requirements

### Requirement: Read-only remote game source

平台 MUST 能打开已配置的 OpenList-Worker 游戏根目录，列出子目录和文件，查询类型和大小并读取文件。引擎产生的写入 MUST 继续进入现有可写覆层。未配置远程源时，本地来源的打开流程 MUST 保持可用。

#### Scenario: Browse and open a configured game directory

- **WHEN** 调用方打开一个含多页列表、嵌套目录和 Unicode 文件名的已配置游戏目录
- **THEN** 返回完整的直接子项、正确的类型和大小，并可读取指定文件的原始字节
- **AND** 同一路径上的运行时写入和再读取遵循现有可写覆层规则，OpenList-Worker 上的文件字节保持原样

#### Scenario: A remote path or request fails

- **WHEN** 路径试图越出配置的游戏根，或者 Worker 返回鉴权、限流、网络或接口错误
- **THEN** 路径被拒绝或错误被明确报告
- **AND** 只有明确的文件不存在结果才可作为不存在处理

### Requirement: Verified browser download and byte ranges

远程来源 MUST 通过站点的真实浏览器访问验证下载链。声明支持随机访问时，每次非空范围读取 MUST 返回所请求的字节，并校验范围响应和实际长度。范围不可用时 MUST 明确报告能力或读取错误，并约束浏览器内存使用。

#### Scenario: Read the interior of a remote file

- **WHEN** 浏览器请求一个已知文件中间的一段字节
- **THEN** 浏览器可读取 `206`、`Content-Range` 及响应体，适配器校验偏移、长度和字节内容

#### Scenario: A storage driver ignores the range request

- **WHEN** 存储驱动对范围请求返回整文件 `200` 或返回不一致的范围
- **THEN** 适配器明确报告随机访问不可用或该次读取失败
- **AND** 响应体超过已配置读取预算时取消读取

### Requirement: Daily upstream and production contract checks

项目 MUST 每个 UTC 日解析官方 `OpenListTeam/OpenList-Worker` 当前默认分支的提交。每天 MUST 实测生产 Worker 的接口并验证上游候选的接口，即使提交未变化也执行。项目 MUST 记录运行时间、候选提交、生产部署标识和各契约判定，并对检查失败或缺跑发出可见告警。

#### Scenario: The upstream default branch advances or moves

- **WHEN** 官方默认分支的 HEAD 推进，或上游更改默认分支
- **THEN** 后续巡检自动解析当前分支和 HEAD，并用对应制品在隔离环境运行浏览器契约测试
- **AND** 每次运行记录的提交只标识当次候选和部署来源，下一次巡检重新解析上游

#### Scenario: The revision stays the same

- **WHEN** 上游 HEAD 与前一天相同
- **THEN** 当天照常向候选和生产 Worker 发起实际接口请求，并记录结果

#### Scenario: A daily check is missed

- **WHEN** 一个 UTC 日的巡检缺少完成记录
- **THEN** 监控产生独立于该缺跑任务的告警，并允许手动补跑

### Requirement: Promotion gated by compatible interfaces

生产更新 MUST 使用通过候选验收的制品，并在发布后复测生产接口。候选不兼容时 MUST 保留最近一次通过验收的生产部署并发出告警。生产复测失败时 MUST 能回滚并重新验证，随后继续跟踪上游。

#### Scenario: An upstream contract changes incompatibly

- **WHEN** 上游改动导致列表分页、元数据、下载、Range、CORS 或配置的读取权限契约失败
- **THEN** 候选无法进入生产，报告指出提交和失败操作，现有通过验收的部署继续服务
- **AND** 修复适配器或上游后，用重新解析的候选重复验收和发布流程

#### Scenario: Production smoke tests fail after promotion

- **WHEN** 候选通过隔离验收，但发布后生产接口复测失败
- **THEN** 恢复最近一次通过生产验收的部署，并记录恢复后的契约检查结果
