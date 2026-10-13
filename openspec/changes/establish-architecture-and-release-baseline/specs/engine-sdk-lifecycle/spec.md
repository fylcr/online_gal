# Spec Delta

## Purpose

定义引擎 provider 与宿主会话的稳定生命周期契约：注册与识别、加载、暂停、释放，以及宿主会话的启动与停止，明确每一步的资源责任与失败语义，使引擎可替换而宿主不需要感知引擎内部。

## ADDED Requirements

### Requirement: Uniform lifecycle vocabulary

每一个引擎 provider MUST 以同一组生命周期动词对外：识别（probe）、加载（load）、暂停（pause）、释放（dispose）。MUST NOT 为个别引擎引入互不兼容的动词集合；引擎特有步骤 MUST 通过加载选项表达而不是新的生命周期方法。

#### Scenario: A provider is registered

- **WHEN** 任一 provider 被注册进运行时注册表
- **THEN** 它只通过统一动词被宿主驱动
- **AND** 注册阶段验证其声明的 `runtimeId` 与自身一致且不重复

#### Scenario: Failure to load

- **WHEN** 一个 provider 因缺少运行条件（例如所需页面 URL 或资源不可用）无法加载
- **THEN** 该 provider 报告不可运行/不可用，而不是返回一个会 404 或挂起的地址
- **AND** 选择链回落到下一个能真正运行的 provider

### Requirement: Dispose releases resources

`dispose` MUST 释放该 provider 在加载期间占用的宿主侧资源（frame、worker、canvas 上下文、事件监听、定时器），并 MUST 可重复调用而不加剧泄漏。

#### Scenario: Session is stopped

- **WHEN** 宿主会话停止
- **THEN** 输入、渲染循环与运行时句柄先被拆除，随后 `dispose` 被调用
- **AND** 该引擎实例注册过的监听与定时器不再触发

#### Scenario: Pause resumption

- **WHEN** 会话被暂停后恢复
- **THEN** `pause` 只暂停时钟/输入而不释放加载成本高的资源
- **AND** 任何被 `pause` 停止的循环在恢复后继续推进

### Requirement: Session start and stop contract

宿主会话 MUST 暴露 `start` 与 `stop`：`start` 顺序 MUST 为加载运行时、启动循环、首次呈现；`stop` MUST 停止循环、清除输入、解除挂载、释放运行时。`stop` MUST 幂等。

#### Scenario: Start succeeds

- **WHEN** 会话 `start` 被调用且运行时加载成功
- **THEN** 循环开始推进且至少完成一次呈现调用
- **AND** 会话对外可观测状态为运行中

#### Scenario: Component is destroyed

- **WHEN** 承载会话的界面组件被销毁
- **THEN** `stop` 被调用一次或多次而不抛错
- **AND** 引擎资源不跨会话残留

### Requirement: Save round-trip through the host

宿主 MUST 以"先让运行时写，再登记槽记录"的顺序保存：只有运行时保存成功后才产生存档槽记录；运行时拒绝时 MUST NOT 返回槽记录。存档记录与引擎自身的可写文件 MUST 一起移动。

#### Scenario: Save succeeds

- **WHEN** 用户触发保存且运行时接受
- **THEN** 引擎可写文件与槽记录都被写入可写存储
- **AND** 之后从该槽读取得到同样的引擎状态

#### Scenario: Save is refused

- **WHEN** 运行时拒绝保存
- **THEN** 不产生槽记录，也不留下半写入的槽
- **AND** 调用方收到明确的失败结果
