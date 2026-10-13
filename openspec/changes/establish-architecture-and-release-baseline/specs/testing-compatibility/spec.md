# Spec Delta

## Purpose

定义测试与兼容性的证据契约：测试分层与各自能证明什么、命令台账的必填字段、源码级证据与真游戏 oracle 的区别，以及可移植性与环境前置的边界，使"通过"不会被读成比它实际证明的更多。

## ADDED Requirements

### Requirement: Test tiers are named and separated

测试 MUST 按可运行环境分层：可在纯 Node 下运行的契约/集成测试、需要本地工作台与真实游戏数据的测试、需要 C++ 工具链与上游检出的原生探测。每一层 MUST 有独立的命令入口，且某一层缺失时 MUST NOT 让另一层显示为通过。

#### Scenario: The workbench is absent

- **WHEN** 需要本地工作台与游戏数据的测试层在缺少这些资源时被运行
- **THEN** 它以非零退出码明确报告"没有执行任何测试"，而不是空跑后报告成功
- **AND** 输出区分"未运行"与"通过"

#### Scenario: Native probes lack a toolchain

- **WHEN** 原生探测缺少编译器、头文件或上游 pin 检出
- **THEN** 该用例报告跳过并说明缺少什么，MUST NOT 报告为通过
- **AND** 硬失败与跳过的分界按用例自身定义，不按工具是否存在统一处理

### Requirement: Command ledger with environment

每一轮验收 MUST 记录命令台账：命令、工作目录、使用的 Node/pnpm/编译器版本、退出码，以及结论分类（通过 / 失败 / 阻塞 / 未运行）。运行环境 MUST 与目标 CI 环境区分记录。

#### Scenario: A result is cited

- **WHEN** 文档引用一次测试或构建结果
- **THEN** 读者能查到该命令的 cwd、版本、退出码与结论分类
- **AND** 该结果所在机器与 CI 版本的差异被标注，不会被当作 CI 复现

#### Scenario: A result is blocked by environment

- **WHEN** 失败由工具链、链接方式或临时目录造成而非仓库缺陷
- **THEN** 该归因被标为"暂定/未排除项目问题"直到有可复核的原始输出
- **AND** 没有留存原始输出的归因 MUST NOT 被写成已证明

### Requirement: Source evidence is not engine availability

代码阅读、静态门禁通过、构建成功、真实游戏可运行、再分发授权 MUST 被当作五个不同结论。任何测试或构建的通过 MUST NOT 被表述为引擎在浏览器中可用。

#### Scenario: A static chain is read end-to-end

- **WHEN** 静态分析找齐了调用链两端
- **THEN** 结论仍标为"静态已读"，不升级为"绘制已发生"
- **AND** 只有在浏览器中观测到真实绘制才算通过该判据

#### Scenario: A build succeeds

- **WHEN** WASM 或前端构建成功
- **THEN** 结论只覆盖构建本身，不覆盖引擎运行
- **AND** 未被验收的产物在文档中保持"未验收"

### Requirement: Real-game oracle discipline

真实游戏测试 MUST 是兼容性的唯一 oracle，且单次运行 MUST NOT 作为判据：MUST 丢弃一次热身运行，并要求两次结果一致。

#### Scenario: A compatibility change is accepted

- **WHEN** 有人据一次真实游戏运行结果宣布修复
- **THEN** 该结论被要求补做热身丢弃与两次一致
- **AND** 没有测量重复的报告被标为不可作为判据

#### Scenario: A user reports a failure without steps

- **WHEN** 用户报告"还是不行"但没有步骤或测量
- **THEN** 该报告被记录为未解决信号，MUST NOT 被推断为某个具体根因
- **AND** 诊断结论必须来自可复现的测量

### Requirement: Uncovered surfaces stay listed

测试文档 MUST 维护"未覆盖/未运行"清单，包含被授权范围排除的面向，例如发布产物生成、C++ 原生探测、IndexedDB 配额与跨标签行为、大 archive 峰值内存、权限与插件。

#### Scenario: A reader asks what is not covered

- **WHEN** 读者打开测试文档的未覆盖清单
- **THEN** 每一条未覆盖项说明它为什么未运行以及需要什么前置条件
- **AND** 未被列入的面向不得被默认为已覆盖

#### Scenario: Test counts versus coverage

- **WHEN** 文档给出测试文件数量
- **THEN** 明确该数字是文件/用例计数而不是覆盖率
- **AND** 不把文件数当作被验证行为的度量
