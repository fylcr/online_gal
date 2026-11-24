# Spec Delta

## Purpose

在站点的浏览器 Service Worker 内运行 OpenList-Worker 的只读文件逻辑，为 VFS 提供远程游戏目录，并每天实测候选与线上接口。

## ADDED Requirements

### Requirement: OpenList runs in the site Service Worker

站点构建 MUST 将当次候选的 OpenList-Worker TypeScript 读取逻辑编译进现有的浏览器 `/sw.js`。来自受控页面的 `/__openlist` 文件请求 MUST 由这个 Service Worker 处理。Pages MUST 以静态文件分发制品，`/__openlist` MUST NOT 加入 Pages Functions 路由。实际分发的制品 MUST 按适用许可提供对应源码及许可信息。

#### Scenario: OpenList is ready in a controlled browser

- **WHEN** Chromium 测试页被当次构建的站点 Service Worker 接管并通过现有的 VFS 入口打开挂载
- **THEN** 浏览器 Service Worker 使用编译入包的 OpenList 逻辑处理列表、元数据和字节读取
- **AND** 验收记录可将结果关联到当前控制页面的 Service Worker 版本和上游提交

#### Scenario: The page is awaiting Service Worker control

- **WHEN** 页面发起远程读取，但尚未受站点 Service Worker 控制
- **THEN** VFS 报告等待接管或暂不可用，并允许接管后重试
- **AND** 直达静态站点的 `/__openlist` 请求无法执行文件接口

### Requirement: Read-only remote game source

VFS MUST 能打开已配置的 OpenList 游戏根目录，列出直接子项，查询类型与大小并读取文件。Service Worker MUST 按已配置的挂载来源和路径边界读取。引擎写入 MUST 进入现有可写覆层，本地来源 MUST 保持可用。

#### Scenario: Browse and open a configured game directory

- **WHEN** 调用方打开含多页列表、嵌套目录和 Unicode 文件名的游戏目录
- **THEN** 返回完整的直接子项、正确的类型和大小，并可读取指定文件的原始字节
- **AND** 对同一路径写入后，游戏按现有覆层规则读到新内容，远程文件保持原样

#### Scenario: A path, permission or request fails

- **WHEN** 路径超出游戏根、下载目标超出配置，或请求遇到鉴权、限流、网络或接口错误
- **THEN** 对应的 VFS 操作报告可区分的失败原因
- **AND** 只有明确的文件不存在结果才作为不存在处理

#### Scenario: The browser stops an idle Service Worker

- **WHEN** 浏览器终止空闲 Service Worker，受控页面随后再次列出或读取已配置的挂载
- **THEN** Service Worker 重新加载挂载所需的配置并恢复读取

### Requirement: Verified browser download and byte ranges

项目 MUST 在真实浏览器用候选和生产部署的同源只读夹具验证实际下载链，并用隔离夹具验证适用的鉴权结果。声明支持随机访问时，每次非空范围读取 MUST 校验 HTTP `206`、`Content-Range` 和实际长度。范围不可用时 MUST 报告能力或读取错误。整文件和允许的降级读取 MUST 遵守字节预算。

#### Scenario: Read a range against known bytes

- **WHEN** VFS 请求已知文件中间的一段字节
- **THEN** Service Worker 可读取实际的范围响应头，校验偏移及长度，契约套件对照夹具校验返回字节

#### Scenario: The storage driver returns invalid ranges or too much data

- **WHEN** 存储驱动忽略 Range、返回不一致的范围或发送超过预算的字节
- **THEN** 适配器报告能力或读取错误，超过预算时取消数据流

### Requirement: Daily upstream and production contract checks

项目 MUST 每个 UTC 日解析官方 `OpenListTeam/OpenList-Worker` 当前默认分支的提交，并在真实浏览器分别实测候选与生产 Service Worker 的契约。每次记录 MUST 包含 UTC 时间、默认分支、候选 SHA、制品摘要、浏览器实际使用的 Service Worker 版本和各契约判定。上游选择 MUST 持续跟踪官方默认分支，SHA 用于标识当次验收制品。任务失败或缺跑 MUST 有可见告警。

#### Scenario: The upstream default branch advances or changes

- **WHEN** 官方默认分支推进或上游切换默认分支
- **THEN** 后续巡检重新解析分支与 HEAD，并用对应的静态制品在隔离的浏览器 Service Worker 中验证契约

#### Scenario: The upstream revision stays the same

- **WHEN** 上游 HEAD 与前次相同
- **THEN** 当天仍通过 VFS 对实际接管页面的候选与生产 Service Worker 各执行一轮文件读取契约检查

#### Scenario: Upstream lookup fails or a daily check is missed

- **WHEN** 解析上游失败，或一个 UTC 日缺少巡检完成记录
- **THEN** 解析失败时仍运行生产检查，分别记录上游错误和生产结果
- **AND** 缺跑由独立检查告警，两种情况均可手动补跑

### Requirement: Promotion gated by compatible interfaces

正式发布 MUST 通过候选浏览器验收并核对发布制品摘要。发布后 MUST 验证新客户端和已有 Service Worker 的客户端实际运行的版本及读取契约。候选不兼容时 MUST 保留最近通过验收的生产制品并告警。生产复测失败时 MUST 能恢复上次通过的制品并复测。

#### Scenario: A candidate breaks an interface

- **WHEN** 候选的列表、元数据、读取权限、下载或 Range 契约失败
- **THEN** 发布门禁拦下候选，报告对应的上游提交和失败操作，生产继续用通过验收的版本
- **AND** 修复后重新解析上游，再次运行候选验收

#### Scenario: A release fails its production checks

- **WHEN** 发布制品通过隔离验收，但线上 Service Worker 的读取契约或更新测试失败
- **THEN** 恢复最近通过生产验收的静态制品，并记录浏览器接管恢复版本后的契约结果
