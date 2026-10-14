# Spec Delta

## Purpose

定义新 runtime release 与历史固定 catalog pin 的兼容方式，确保旧 tag 仍可获取，且新版本不会未经核验改变生产默认引擎。

## ADDED Requirements

### Requirement: Historical pins remain valid
已记录的历史 release tag 与其 asset MUST 在发布新版本后继续可用。新版本 MUST NOT 改写这些历史 tag、asset 字节或既有 ledger 记录。

#### Scenario: New ONS version is published
- **WHEN** 新 ONS tag 发布成功
- **THEN** 既有 `engine-ons-0.7.7beta` tag 与其 asset 仍可按原身份获取
- **AND** 既有 ledger 中的 hash 不被本次发布改写

#### Scenario: New KRKR version is published
- **WHEN** 新 KRKR tag 发布成功
- **THEN** 既有 `engine-krkr-20260920` tag 与其 asset 仍可按原身份获取

### Requirement: Catalog updates are explicit
发布新版本 MUST NOT 自动修改主源码 artifact ledger、生产默认引擎或当前 catalog pin。catalog/hash 更新 MUST 作为单独变更，在人工核验后显式审批。系统 MUST NOT 让 latest catalog 变化覆盖旧固定 release，也 MUST NOT 只改 release 而留下会让用户获取失败的旧手写 hash。

#### Scenario: Release without catalog edit
- **WHEN** 一次 runtime release 成功且没有单独的 catalog 审批
- **THEN** 生产默认引擎与主源码 ledger pin 保持原值
- **AND** 旧 pin 的获取目标与 hash 不变

#### Scenario: Verified catalog promotion
- **WHEN** 维护者核验新 release 后提交单独 catalog 更新
- **THEN** 该更新同时改变目标 tag 与对应 hash
- **AND** 不把该推广描述为本次自动发布的副作用

### Requirement: Consumers select stability explicitly
消费方 MUST 显式选择正式版或先行版，并按完整版本字符串识别身份。系统 MUST NOT 以 SemVer latest、GitHub latest、precedence 或最高版本排序代替该选择。`1.0.0+a` 与 `1.0.0+b` MUST 保持为两个不同消费目标。Node SDK 的 release 获取链 MUST NOT 被一次 runtime release 自动切换为生产 rollout。下载 URL MUST 只编码 path segment，MUST NOT 改写 catalog 中存储的版本。

#### Scenario: Prerelease is available
- **WHEN** 同一核心同时存在正式版与 beta 先行版
- **THEN** 未显式选择先行版的消费路径不采用该 beta
- **AND** 未显式选择正式版的消费路径也不因排序自动跳到正式版

#### Scenario: SDK fetch chain
- **WHEN** 新 runtime tag 已存在但 catalog pin 未更新
- **THEN** 现有 Node SDK fetch 链继续使用原 pin
- **AND** 不因新 tag 存在而改变生产 rollout

### Requirement: A later release packs the checkout it builds
根 lock、共享依赖、许可证或 `.github/scripts/**` 的单独变化 MUST NOT 自动触发 runtime 发布。下一次真正的 runtime release MUST 打包该次 checkout 的闭包、`LICENSE` 与 `THIRD_PARTY_NOTICES.md`，MUST NOT 把这些文件描述为过期依赖快照，也 MUST NOT 因此自动修改 catalog pin 或生产默认。

#### Scenario: Shared file changes without a runtime input
- **WHEN** push 只改变根 lock、共享依赖、许可证或 `.github/scripts/**`
- **THEN** 不自动构建、不自动发布，catalog pin 与生产默认保持不变

#### Scenario: Next runtime release uses that checkout
- **WHEN** 之后一次 runtime 发布输入变化并成功发布
- **THEN** 该 release 打包当时 checkout 的闭包与许可证
- **AND** 仍不自动修改 catalog pin 或生产默认
