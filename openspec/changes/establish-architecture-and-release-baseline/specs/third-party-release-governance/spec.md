# Spec Delta

## Purpose

定义第三方与专有字节的治理契约：仓库内第三方字节的清单与许可状态记录、发布产物的 source offer 与二进制一致性、以及发布门禁，使"这份字节从哪里来、能否再分发、谁批准发布"始终是可回答的问题。

## ADDED Requirements

### Requirement: Third-party inventory and licence status

仓库 MUST 维护一份第三方清单，列出直接外部依赖、随仓交付的第三方代码与 SDK、上游引擎 pin、下载工具链，以及每一项记录的许可状态。许可状态 MUST 区分"仓库声明"、"已知许可文本"与"未解决/需人工复核"，MUST NOT 把声明字段当作再分发授权。

#### Scenario: A reader checks whether bytes may be redistributed

- **WHEN** 读者查询某个随仓交付的第三方组件
- **THEN** 清单给出其来源、大小、许可文本位置，以及状态是"已记录"还是"未解决"
- **AND** 对未解决项明确写出"这不是法律意见，需人工确认"

#### Scenario: A new third-party byte is added

- **WHEN** 有新的第三方字节进入仓库或进入发布产物
- **THEN** 该字节在清单中获得条目，并带有来源与许可状态
- **AND** 状态为未解决的字节 MUST NOT 被默认允许随发布产物分发

### Requirement: No implied grant from repository licence

仓库根部的 GPL 声明 MUST NOT 被表述为对专有 SDK 或第三方字节的再分发授权；保留字节与"可以发布" MUST 被当作两个不同结论。

#### Scenario: A release includes SDK-derived bytes

- **WHEN** 某发布产物包含由专有 SDK 派生的字节
- **THEN** 发布前存在该 SDK 的书面再分发许可或有明确的人工保留决定
- **AND** 在取得许可前该发布路径保持关闭

### Requirement: Source offer and binary consistency

对以源码构建交付的引擎产物，发布 MUST 同时提供对应的源码 offer（pinned 上游归档 + 补丁 + overlay + 构建脚本 + 说明），且 offer 的校验和被自校验；发布产物的版本/pin 记录 MUST 与实际构建所用 commit 一致。

#### Scenario: A release is produced

- **WHEN** 一次引擎发布被创建
- **THEN** 该发布包含源码 offer 与其校验和，且构建期间断言"使用的 commit == 记录的 pin"
- **AND** 断言失败时发布中止，不产出标错 provenance 的产物

#### Scenario: Two records disagree about a pin

- **WHEN** 文档记录的上游 pin 与工作流实际使用的 commit 不一致
- **THEN** 该不一致被登记为发布治理缺陷并要求人工裁定哪一个 commit 是既有证据对应的
- **AND** MUST NOT 通过改写冻结的历史哈希来消除冲突

### Requirement: Release gating and least privilege

发布 MUST 只由一个显式的人工触发决定，普通 push MUST NOT 直接发布；构建与发布工作流 MUST 使用最小权限，且其引用的外部 Action MUST 以不可变的 40 位 commit SHA 固定。

#### Scenario: A push to the default branch occurs

- **WHEN** 一次 push 命中某引擎工作流的路径过滤
- **THEN** 该工作流只产出构建工件，不创建或覆盖 release
- **AND** 需要发布时要显式派发并给出发布意图

#### Scenario: An external Action is referenced

- **WHEN** 任一工作流引用第三方 Action
- **THEN** 引用固定到完整 commit SHA，版本号只作为注释
- **AND** 工作流权限不超过其实际需要的范围

### Requirement: Protected review and recovery surfaces

仓库 MUST 为许可关键路径指定 CODEOWNERS 并要求评审；备份与恢复状态 MUST 被记录为"已知"或"未知"，MUST NOT 在未查询的情况下被表述为不存在。

#### Scenario: A change touches a licence-critical tree

- **WHEN** 变更涉及专有 SDK、发布 overlay 或随发布交付的资产目录
- **THEN** 该变更需要被指定 owner 评审
- **AND** 路径与责任人被记录在仓库内的 CODEOWNERS 中

#### Scenario: Backup and restore status is reported

- **WHEN** 有人询问备份与恢复能力
- **THEN** 答案区分仓库内可重建的内容与只存在于单机的内容
- **AND** 远程侧状态（保护规则、备份）在被实际查询前标为未知
