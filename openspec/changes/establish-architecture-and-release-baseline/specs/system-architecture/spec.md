# Spec Delta

## Purpose

定义一个可校验的模块边界与依赖方向契约：仓库分层、组合根唯一性、公开入口稳定性，以及持久架构基线文档的位置与证据等级要求，使架构漂移在评审阶段就可被发现而不是在构建失败时才暴露。

## ADDED Requirements

### Requirement: Repository layering

仓库 MUST 只把可构建的产品与移植代码放在 `packages/`、`apps/`、`ports/` 之下；`.test/` MUST 只承载测试、真实游戏与本地一次性脚本，`.ref/` MUST 只承载参考检出与官方 SDK。构建产出 MUST 落在已声明的输出子目录（例如 `ports/<port>/out/`、`packages/*/dist/`），MUST NOT 落在仓库根。

#### Scenario: A committed source file references the local workbench

- **WHEN** `packages/`、`apps/` 或 `ports/` 下被 git 跟踪且未豁免的文件出现对 `.test/` 的路径引用
- **THEN** 布局检查以非零退出码失败，并逐条列出违规的 `路径:行号`
- **AND** 失败信息说明修复方向是把被引用的代码移回 `packages/`、`apps/` 或 `ports/`

#### Scenario: The two existing Artemis tests write to local scratch

- **WHEN** `ports/artemis/compat-web/tests/artemis-compat-first-transition.test.mjs` 或 `ports/artemis/compat-web/tests/artemis-compat-tween.test.mjs` 使用 `.test/` 来存放它们生成的解包文件和日志
- **THEN** 布局检查 MUST 跳过这两个文件；豁免 MUST NOT 扩展到其他测试或产品代码

#### Scenario: Archive or toolchain is unpacked

- **WHEN** 任何上游 archive 或 tar 被解包
- **THEN** 解包目标在解包前被断言为绝对路径且落在 `.test/`、`.ref/` 或已声明构建输出目录之下
- **AND** 断言失败时命令以非零退出码终止且不留下落盘文件

### Requirement: Single composition root

引擎 provider 的注册列表 MUST 有唯一权威来源。应用启动路径 MUST NOT 依赖两处独立维护的 provider 副本，且同一 `runtimeId` MUST NOT 被静默注册两次。

#### Scenario: A provider is added or reordered

- **WHEN** 新增或重排任一引擎 provider
- **THEN** 只有一处注册定义需要修改，且启动路径用的注册顺序随之改变
- **AND** 重复的 `runtimeId` 在模块加载阶段抛出错误，而不是让先注册者静默胜出

#### Scenario: Discovery is documented

- **WHEN** 读者需要知道平台注册了哪些 runtime 及其中哪些当前可运行
- **THEN** 文档给出包名、`runtimeId`、kind 与可用性来源，并区分"注册"与"实测可运行"

### Requirement: Public entry stability

每个包 MUST 声明稳定的公开入口；`exports` 的类型/开发入口指向源码、默认入口指向构建产物这一分叉 MUST 被显式记录。包 MUST NOT 通过深路径导入另一个包的内部源码模块。

#### Scenario: A consumer imports a package

- **WHEN** 任一包或应用导入另一个包
- **THEN** 导入使用该包 `exports` 中声明的入口（`.` 或已声明的子路径）
- **AND** 对未声明的深路径导入作为评审阻塞项被记录

### Requirement: Declared dependency direction

每个包的 `package.json` `dependencies`、其 `tsconfig.json` project references 与源码 import 三者 MUST 一致：manifest 声明的依赖 MUST 被源码使用，源码使用的外部包 MUST 被 manifest 与 references 同时声明。

#### Scenario: A package imports a workspace peer

- **WHEN** 包的源码值导入另一个 workspace 包
- **THEN** 该包同时出现在 manifest 依赖与该包 tsconfig 的 references 中
- **AND** 三者不一致时被登记为依赖契约缺陷，并给出 `路径:行号`

#### Scenario: A declared dependency is unused

- **WHEN** manifest 或 references 声明了一个源码中没有任何 import 的 workspace 依赖
- **THEN** 该边被标为"声明但未使用"，并要求人工确认是有意重导出还是漂移
- **AND** 在得到确认前 MUST NOT 把它当作故障或悄悄删除

### Requirement: Architecture baseline as durable audit

仓库 MUST 在 `docs/architecture/` 下维护持久基线文档，记录审计戳（HEAD、分支、工作区与 HEAD 的差异计数）、证据等级图例、模块矩阵、已验证依赖边与 finding registry。

#### Scenario: A reader needs to know what is actually proven

- **WHEN** 读者打开 `docs/architecture/current-state.md`
- **THEN** 文档给出审计戳、覆盖率计数、把 dirty 与 untracked 计入的模块清单，以及每一条依赖边的"已验证 / 推断 / 未知"标记
- **AND** 任何未经运行验证的结论 MUST NOT 被写成已证明

#### Scenario: A baseline is revised

- **WHEN** 基线文档被更新
- **THEN** 每条 finding 保留稳定 ID、证据 `路径:行号`、影响、复现或验证方式、建议范围、回滚与人工确认需求
- **AND** 未复核的旧结论被显式标记为历史或被修正，而不是静默覆盖
