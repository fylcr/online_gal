# Spec Delta

## Purpose

定义引擎产物如何被按需发现与加载：catalog 条目与 artifact manifest 必须携带的标识、完整性哈希、ABI 与来源/许可可用性信息，以及资源缺失时的撤回语义，使"引擎是否可运行"是一个可判定的声明而不是一个 404 后的偶发失败。

## ADDED Requirements

### Requirement: On-demand catalog instead of bundling

引擎字节 MUST NOT 被静态打包进前端 bundle；MUST 通过声明式 catalog 条目按需加载。catalog 条目 MUST 命名其产物（engine id、版本、可获取的 URL 或本地挂载路径）并 MUST 与运行时注册解耦，使新增引擎不需要改动应用启动代码。

#### Scenario: Application starts without any engine artifact

- **WHEN** 部署中没有任何引擎产物
- **THEN** 应用仍能构建并启动，引擎 provider 报告不可用
- **AND** 不因为缺失产物而发布指向不存在文件的 URL

#### Scenario: An engine artifact becomes available

- **WHEN** 某个引擎的产物被放入声明的挂载位置
- **THEN** 该引擎在 catalog 中可见并可被加载
- **AND** 应用启动代码无需为它新增分支

### Requirement: Artifact manifest identity

每个可交付引擎产物 MUST 有 artifact manifest，至少包含：产物名、字节大小、内容 sha256、上游来源与 commit、构建工具链版本，以及该产物的许可与 source offer 是否可用。manifest MUST 与发布产物在同一提交中保持一致。

#### Scenario: A consumer verifies a downloaded artifact

- **WHEN** 消费者按 manifest 下载引擎产物
- **THEN** 下载字节在被采用前按大小与 sha256 校验
- **AND** 校验失败即中止，且 MUST NOT 静默回落到其他来源

#### Scenario: A manifest is stale

- **WHEN** 产物字节与其 manifest 记录不一致
- **THEN** 该不一致被报告为失败而不是被当作"回落到源码构建"的静默降级
- **AND** 台账的更新责任与手动更新事实被文档化

### Requirement: ABI and runtime precondition declaration

每个引擎产物 MUST 声明其运行前置条件（例如需要的页面地址、跨源隔离、共享内存、离屏画布），且宿主 MUST 在不满足前置条件时把该引擎报告为不可用而不是尝试运行。

#### Scenario: Isolation headers are absent

- **WHEN** 一个要求跨源隔离的引擎产物在未隔离的页面中被加载
- **THEN** provider 报告不可用并给出原因
- **AND** 不启动会挂起或崩溃的加载流程

### Requirement: Withdrawal over dangling URL

当某引擎的候选产物不存在或未通过静态门禁时，平台 MUST 撤回该 provider，MUST NOT 发布会 404 的地址或把可运行的解释器降级为失败。

#### Scenario: No candidate build is present

- **WHEN** 期望的候选产物目录不存在或未通过 manifest 校验
- **THEN** 该 provider 在启动时被撤回并记录一条可读警告
- **AND** 同一游戏的识别回落到仍可运行的 provider，而不是进入启动失败

#### Scenario: Assets exist but were never accepted

- **WHEN** 产物文件存在但从未经过真实游戏验收
- **THEN** 它 MUST NOT 被文档或界面表述为可玩
- **AND** 发布开关保持关闭直到验收记录存在
