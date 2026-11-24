# Spec Delta

## Purpose

为游戏库增加标准 WebDAV 只读目录挂载。游戏可以经浏览器直接列目录、查询元数据和读取远程文件，存档继续由现有本地覆层保存。

## ADDED Requirements

### Requirement: WebDAV directory source and existing source behavior

游戏库 MUST 提供独立的 WebDAV 目录来源。WebDAV 挂载 MUST 支持列出直接子项、读取类型与大小、读取文件，并把运行时写入交给现有本地覆层。HTTP(S) 直链及其他游戏来源 MUST 保留各自的入口与读取契约。

#### Scenario: Open a configured WebDAV game directory

- **WHEN** 用户打开包含嵌套目录及 Unicode 文件名的已配置 WebDAV 游戏根
- **THEN** 游戏可以列目录、取得元数据和读取正确的文件字节
- **AND** 游戏在覆层读到自身写入的内容，远程文件保持原样

#### Scenario: Open a game from its existing source

- **WHEN** 用户从直链、本地目录或导入文件打开游戏
- **THEN** 游戏沿相应来源的既有流程读取并运行
- **AND** 直链来源按自己的下载契约读取字节

### Requirement: Browser Service Worker handles WebDAV reads

受控页面的 `/__webdav` 文件请求 MUST 由站点现有的浏览器 Service Worker 处理。WebDAV 网络请求 MUST 使用已配置的 HTTPS 来源和游戏根。挂载授权 MUST 限于对应来源和根。Pages MUST 以静态站点分发 WebDAV 能力，`/__webdav` MUST NOT 加入 Pages Functions 路由。

#### Scenario: Service Worker control is unavailable

- **WHEN** 页面等待 Service Worker 接管
- **THEN** 读取报告暂不可用并允许重试
- **AND** 直达 Pages 的相同路径无法执行文件请求

#### Scenario: The browser stops an idle Service Worker

- **WHEN** 浏览器终止空闲 Service Worker，当前受控会话再次读取
- **THEN** 读取可用当前会话的挂载配置恢复

#### Scenario: A mount read is rejected

- **WHEN** 目标越过已配置的边界、授权被拒或响应不满足读取契约
- **THEN** VFS 报告读取失败，按可观察的响应区分原因
- **AND** 只有明确的文件不存在作为不存在处理

### Requirement: Browser-readable WebDAV methods and byte ranges

项目 MUST 用真实浏览器验证跨源 WebDAV 预检、`PROPFIND` 目录和元数据、`GET` 整文件及 Range 读取。非空范围读取 MUST 校验 `206`、可读的 `Content-Range` 和实际长度。适配器 MUST 限制读取的字节数，超过预算时取消传输。

#### Scenario: The browser reads an exact byte range

- **WHEN** 已配置的 WebDAV 服务允许站点来源并返回正确的 `206`
- **THEN** 浏览器可读取范围头并返回与请求匹配的字节

#### Scenario: The browser cannot read a valid range

- **WHEN** 服务拒绝 CORS、忽略 Range、隐藏 `Content-Range` 或返回无效范围
- **THEN** WebDAV 操作报告失败或缺少随机读取能力

### Requirement: Daily browser contract checks and release verification

项目 MUST 每个 UTC 日使用真实 Chromium 对隔离候选与生产各做一次 WebDAV 契约检查，记录站点制品、实际接管页面的 Service Worker 版本和每项结果。失败或缺跑 MUST 告警并支持手动补跑。WebDAV 变更发布 MUST 以通过隔离验收的制品为准，并在生产复测。

#### Scenario: Both targets are checked even if the site build has stayed the same

- **WHEN** 定时任务执行
- **THEN** 候选和生产各发起一次真实跨源 WebDAV 契约测试，并分别记录结果

#### Scenario: Contract verification fails

- **WHEN** 候选契约失败
- **THEN** 阻止该候选发布并报告失败操作

#### Scenario: Production checks detect a site artifact regression

- **WHEN** 生产复测确认站点制品回归
- **THEN** 恢复最近通过生产验收的制品，并验证浏览器接管恢复版本后的读取结果
