# Spec Delta

## Purpose

定义未来浏览器内脚本执行与云同步后端必须遵守的最小接口与权限边界，使这两项能力在被实现之前就有可校验的隔离、授权与降级约束。本 capability 只规定边界，不规定实现，也不表示任何部分已经实现。

## ADDED Requirements

### Requirement: Static deployment stays sufficient

平台 MUST 在没有后端服务的情况下保持完整可用：识别、运行、存档与设置 MUST 全部能在浏览器内完成。任何未来后端能力 MUST 是可选增强，MUST NOT 成为启动或游玩的前置条件。

#### Scenario: No backend is configured

- **WHEN** 部署只提供静态资源
- **THEN** 应用启动、游戏运行、存档读写与设置持久化全部工作
- **AND** 不存在因为缺少后端而阻断的路径

#### Scenario: A backend is unavailable at runtime

- **WHEN** 一个可选后端暂时不可达
- **THEN** 用户仍能继续游玩，只有依赖该后端的功能被标记为不可用
- **AND** 本地存档不被清除或覆盖

### Requirement: Script sandbox with explicit grants

任何浏览器内脚本执行（例如 QuickJS 一类嵌入式解释器）MUST 运行在明确的能力授权模型下：默认 MUST NOT 获得文件系统、网络或宿主对象访问；每一项能力 MUST 由宿主显式授予并可按会话撤销。

#### Scenario: A script requests an ungranted capability

- **WHEN** 脚本尝试访问未授予的宿主能力
- **THEN** 该调用失败并返回可诊断错误，不影响宿主其余部分
- **AND** 不产生对只读游戏源或宿主存储的副作用

#### Scenario: Grants are scoped to a session

- **WHEN** 会话结束或被销毁
- **THEN** 该会话获得的所有脚本授权失效
- **AND** 后续会话不会继承前一个会话的授权

### Requirement: Engine-embedded scripting is not a sandbox

引擎内部自带的脚本运行时（例如某个引擎嵌入的 Lua）MUST NOT 被当作本 capability 的沙箱实现或复用为通用脚本宿主；两者的隔离假设不同，不得互相替代。

#### Scenario: A new scripting feature is proposed

- **WHEN** 有需求要复用某个引擎内的脚本运行时来实现平台级脚本能力
- **THEN** 该方案被拒绝或要求单独的安全评审
- **AND** 平台级脚本能力仍走独立的沙箱与授权模型

### Requirement: Export formats as the sync seam

面向未来的云同步 MUST 以现有的可导出/可导入数据契约为缝（存档记录与可写覆层文件的导出表示），MUST NOT 在契约确定前引入后端框架或改变键空间。

#### Scenario: Export and import round-trip

- **WHEN** 用户导出一个游戏的存档集合，再在另一浏览器导入
- **THEN** 导入后的游戏状态与导出前一致
- **AND** 导入失败时既有数据不被部分覆盖

#### Scenario: Conflict handling is designed before sync ships

- **WHEN** 云同步被实现
- **THEN** 冲突检测与合并规则先于传输实现被定义并测试
- **AND** 未定义冲突规则的同步 MUST NOT 被发布

### Requirement: Permission denial by default

脚本宿主 MUST 以**默认拒绝**为基线：未显式授予的能力不可用，不得通过隐式默认、类型强转或环境探测绕过；被拒绝的调用 MUST 返回可诊断错误而不得静默降级为其他行为。

#### Scenario: An ungranted capability is invoked

- **WHEN** 脚本调用未授予的能力
- **THEN** 调用被拒绝并返回可诊断错误
- **AND** 宿主其余部分与只读游戏源不产生副作用

#### Scenario: A grant is revoked mid-session

- **WHEN** 宿主在会话进行中撤销某项能力
- **THEN** 该能力的后续调用立即被拒绝
- **AND** 已授权且已完成的副作用不被回溯回滚为不一致状态

### Requirement: Controlled file and network API

文件与网络访问 MUST 只经宿主提供的受控 API 暴露（以游戏根为边界的虚拟文件系统视图与经宿主代理的请求），MUST NOT 向脚本暴露原始 DOM、IndexedDB、Worker、引擎句柄或未受控的网络原语。

#### Scenario: A script reads a file

- **WHEN** 脚本请求读取一个路径
- **THEN** 该路径经宿主的唯一路径规范化规则与根边界校验
- **AND** 越出游戏根或只读源之外的访问被拒绝

#### Scenario: A script performs a network request

- **WHEN** 脚本发起网络访问
- **THEN** 请求经宿主代理，并受目标与频率约束
- **AND** 未经授权的目标被拒绝，且不泄露宿主凭证

### Requirement: Resource budget enforcement

脚本执行 MUST 受内存、执行时间与资源句柄数量上限约束；超限 MUST 以错误终止该执行单元，MUST NOT 拖垮宿主页面或使会话进入不可恢复状态。

#### Scenario: A script exceeds its time budget

- **WHEN** 脚本超过分配的执行时间
- **THEN** 该执行被终止并报告超限
- **AND** 宿主仍可继续渲染与处理输入

#### Scenario: A script allocates beyond its memory budget

- **WHEN** 脚本尝试超出内存预算
- **THEN** 分配失败并以错误返回
- **AND** 不会因单次分配拖垮整个页面

### Requirement: Plugin API version and provenance policy

The plugin/script API MUST 声明版本：版本不匹配的插件 MUST 在加载时被拒绝。必须区分**官方**与**第三方**插件，两者的权限集不同且第三方默认获得最小权限；任何插件都 MUST NOT 绕过宿主的授权模型。

#### Scenario: An incompatible plugin is loaded

- **WHEN** 插件的声明版本与宿主支持的 API 版本不匹配
- **THEN** 加载被拒绝并给出可诊断原因
- **AND** 不执行该插件的任何代码

#### Scenario: A third-party plugin requests host privileges

- **WHEN** 第三方插件请求超出其默认权限集的能力
- **THEN** 宿主按与官方插件不同的、更小的默认权限集处理
- **AND** 授权必须显式发生，且可按会话撤销

### Requirement: Cloud ownership and identity contract

未来云同步 MUST 在实现前先定义数据**所有权**与身份契约：谁拥有哪份存档、认证（authn）与授权（authz）如何判定、共享或转移如何处理。MUST NOT 在未定义这些的前提下实现任何上传路径。

#### Scenario: A sync operation is authorized

- **WHEN** 客户端请求同步某份存档
- **THEN** 该请求在授权的身份与所有权范围内被接受或拒绝
- **AND** 拒绝时不泄露其他用户的数据存在性

#### Scenario: Ownership changes

- **WHEN** 存档的所有权被转移或共享
- **THEN** 先前的访问凭证不再有效
- **AND** 变更被审计记录

### Requirement: Data versioning and conflict resolution

同步数据 MUST 携带可比较的版本标识；合并 MUST 基于明确定义的冲突规则，MUST 保留用户可选择回退到任一已知版本的能力。MUST NOT 以"最后写入胜"作为未声明的默认。

#### Scenario: Two devices modify the same slot

- **WHEN** 两台设备各自修改同一存档槽后再同步
- **THEN** 冲突按预先定义的规则处理，并向用户呈现可选择的解决方式
- **AND** 不发生静默覆盖

#### Scenario: A payload schema changes

- **WHEN** 导出/同步格式的 schema 版本变化
- **THEN** 旧版本数据被识别并可迁移
- **AND** 无法迁移时显式失败而不损坏本地数据
