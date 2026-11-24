# Spec Delta

## Purpose

为 VFS 增加按需读取的网络文件挂载，首个协议为 WebDAV。游戏通过既有 VFS 接口读取远程目录和字节，存档继续由现有本地覆层保存。

## ADDED Requirements

### Requirement: VFS network file mounts

VFS MUST 以现有 `GameSource` 暴露只读网络挂载，首个支持的协议为 WebDAV。网络挂载 MUST 支持列出直接子项、读取存在性、类型与大小，以及按需读取整文件或字节范围。游戏库 MUST 可从网络文件入口打开和恢复挂载，引擎和归档 MUST 沿用现有读取接口。运行时写入 MUST 交给现有本地覆层。HTTP(S) 直链及其他游戏来源 MUST 保留各自的入口与读取契约。

#### Scenario: Open a configured network game directory

- **WHEN** 用户从网络文件入口打开包含嵌套目录及 Unicode 文件名的 WebDAV 游戏根
- **THEN** 游戏通过 VFS 列目录、取得元数据和按需读取正确的文件字节
- **AND** 读取可寻址归档的一段时只获取所需范围
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

### Requirement: Browser acceptance for network file changes

项目 MUST 在首次交付及网络挂载、WebDAV 读取或内核传输行为变更时，用真实 Chromium 验收跨源读取。相关版本发布后 MUST 复测新客户端和从旧版 Service Worker 更新的客户端，并记录实际接管页面的版本与结果。

#### Scenario: Accept a changed network file implementation

- **WHEN** 首次交付网络文件功能，或修改相关读取行为
- **THEN** 隔离候选通过端到端读取验收后才发布
- **AND** 发布后通过实际控制页面的 Service Worker 验证目录、元数据和字节读取
