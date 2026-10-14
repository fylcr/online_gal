# Spec Delta

## Purpose

定义每个 ONS/KRKR runtime release 的独立 SemVer 身份、UTC+8 先行版时间，以及 tag、包、manifest 与文件名不可变且一致的规则。

## ADDED Requirements

### Requirement: Automatic versions use a fixed beta timestamp
自动版本 MUST 以对应 tracked package version 的 `major.minor.patch` 为 BASE，并使用 `BASE-beta-YYYYMMDDHHmm`。时间 MUST 取昂贵构建开始时的 `Asia/Shanghai`（UTC+8），且 MUST 为 12 位。系统 MUST NOT 自动递增 patch，也 MUST NOT 把该时间改写为 `beta.time` 或多段点分标识。

#### Scenario: ONS automatic version
- **WHEN** ONS tracked package version 的正常核心是 `0.1.3`，构建开始的上海时间为 2026-10-14 07:59
- **THEN** 自动版本是 `0.1.3-beta-202610140759`

#### Scenario: Existing prerelease base
- **WHEN** tracked package version 含先行版或 `+` 构建元数据
- **THEN** 自动 BASE 只取其中的 `major.minor.patch`
- **AND** 不形成多层 alpha/beta 链

### Requirement: Manual versions are complete SemVer
手动版本 MUST 是不含 `v` 前缀的完整合法 SemVer，包括 `+` 后的构建元数据。数字标识 MUST NOT 有前导零，输入 MUST NOT 为空白。`0.0.1-beta-202610140759` MUST 被接受为合法 SemVer，其先行版是单个连字符后的一个标识符。`1.0.0+a` MUST 被接受，且 MUST NOT 因与无 metadata 版本 precedence 相同而被拒绝或改写。

#### Scenario: Accepted manual version
- **WHEN** 操作者提交 `0.0.1-beta-202610140759` 或 `1.0.0+a`
- **THEN** 版本校验通过并可用于后续预检
- **AND** 规范字符串保持提交值，包括其中的 `-` 与 `+`

#### Scenario: Rejected manual version
- **WHEN** 操作者提交 `v1.2.3`、`01.2.3`、空白，或不是 `X.Y.Z` 核心的字符串
- **THEN** 系统在昂贵构建前拒绝

### Requirement: Prerelease ordering is not latest
同一正常核心的先行版 MUST 低于该正式版。系统 MUST 按点分标识符比较先行版，MUST NOT 把单个连字符标识符改写成点分形式来改变排序。系统 MUST NOT 承诺 GitHub latest 或依赖解析器的 latest 指向高于正式版的先行版。

#### Scenario: Same core release and prerelease
- **WHEN** 比较 `1.0.0-beta-202610140759` 与 `1.0.0`
- **THEN** 先行版的优先级更低

#### Scenario: Hyphen versus dot identifiers
- **WHEN** 比较 `1.0.0-beta-2` 与 `1.0.0-beta.2`
- **THEN** 系统保持二者为不同的先行版标识结构，不把前者规范化成后者

### Requirement: Identity is created once and provenance stays separate
一次发布 MUST 只创建一次版本，并在 canonical version、tag、packed package version、tarball 文件名与 release manifest 中复用该完整字符串。这些字段 MUST 保留 `+` 与 `-`，MUST NOT strip 或规范化掉构建元数据。tag MUST 为 `engine-ons-<SemVer>` 或 `engine-krkr-<SemVer>`，且 MUST 通过 `git check-ref-format`。下载 URL MUST 只编码 path segment，MUST NOT 改写所存储的版本，也 MUST NOT 把 path 中的 `+` 当成 query/form 空格。本要求不承诺 GitHub 远端已实时验证含 `+` 的 asset 名。CI MUST NOT 把版本写回 tracked package commit。上游 ref 与 provenance MUST 使用独立字段，MUST NOT 被当作 runtime SemVer。

#### Scenario: Successful release identity
- **WHEN** ONS 版本 `0.1.3-beta-202610140759` 通过全部预检并构建成功
- **THEN** tag、packed manifest、tarball 文件名与 release manifest 都使用这一版本
- **AND** tracked `packages/runtime-ons/package.json` 不被该 CI 改写提交

#### Scenario: Build metadata is preserved
- **WHEN** 手动版本是 `1.0.0+a`
- **THEN** canonical version、tag `engine-ons-1.0.0+a` 或 `engine-krkr-1.0.0+a`、packed package version、tarball 文件名与 release manifest 都保留 `1.0.0+a`
- **AND** 下载 URL 的 path segment 把 `+` 编码为 `%2B`，存储版本仍是 `1.0.0+a`

#### Scenario: Upstream label is present
- **WHEN** 构建使用上游 commit 或旧的 `0.7.7beta` / `kirikiroid2-web-*` 标签
- **THEN** 这些值只出现在 provenance 字段
- **AND** 不替代 runtime SemVer 或 tag 版本

### Requirement: Collisions fail before the expensive build
同一分钟、重跑或并发产生的已有精确版本 MUST 在昂贵构建前被拒绝。碰撞身份 MUST 是完整版本字符串，MUST NOT 使用 SemVer precedence。系统 MUST 预检 tag、release、asset 与版本历史；任一精确身份已存在时 MUST 失败关闭并保留现有字节。系统 MUST NOT 静默追加秒、run id、自动 bump 或覆盖。按引擎的队列只作为辅助，发布前 MUST 再次验证不存在。上传不是事务：若本次新 release 已创建而后续 upload 失败，系统 MUST 退出非 0 并保留该 release 与已上传 asset，MUST NOT clobber、delete，或宣称它不存在，也 MUST NOT 承诺删除回滚或自动再覆盖。

#### Scenario: Same-minute collision
- **WHEN** 目标版本的 tag、release 或 asset 已存在
- **THEN** 系统在 native 构建前失败
- **AND** 现有 tag、release 与 asset 字节保持不变

#### Scenario: Metadata versions are distinct
- **WHEN** `1.0.0+a` 已发布，而新请求的完整版本是 `1.0.0+b`
- **THEN** 系统不把二者当成同一次碰撞
- **AND** 两个版本字符串都保持原样，不按 precedence 合并

#### Scenario: Rerun reaches publish
- **WHEN** 预检后、发布前的第二次存在性检查发现已有 tag、release 或 asset
- **THEN** 发布失败关闭
- **AND** 不覆盖这些已有字节，也不删除它们

#### Scenario: Upload fails after create
- **WHEN** 本次新 release 的创建已成功，而后续 asset 上传失败
- **THEN** 系统以非 0 退出并保留该新建 release 与已上传 asset
- **AND** 不 clobber、不 delete、不把这次结果说成不存在的发布

### Requirement: Existing releases are immutable
当发布由 ONS 或 KRKR caller 以不可变模式发起时，已发布 tag 与 asset MUST 保留。该模式 MUST NOT 使用 `gh release upload --clobber`、`gh release view || reuse`，或复用旧 tag 上传新字节。不可变模式 MUST NOT 自动套用到所有发布路径。Artemis compat MUST 保持字面 `publish: false`；bridge MUST 保持既有手动 opt-in 与现有 clobber 语义。修改 reusable 共用步骤 MUST NOT 扩大这些路径的自动或手动权限。

#### Scenario: Existing tag is targeted
- **WHEN** ONS 或 KRKR 请求的 tag 已经存在
- **THEN** 系统拒绝该发布
- **AND** 不上传、不替换该 tag 下的任何 asset

#### Scenario: Artemis paths stay closed
- **WHEN** reusable 的共用步骤被改为服务 ONS/KRKR 不可变发布
- **THEN** compat 仍是字面 `publish: false`
- **AND** bridge 仍使用既有手动 opt-in，且其现有 clobber 语义不变
