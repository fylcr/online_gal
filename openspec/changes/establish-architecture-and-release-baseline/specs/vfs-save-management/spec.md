# Spec Delta

## Purpose

定义虚拟文件系统与存档的持久化契约：只读游戏层与可写覆层的关系、IndexedDB 键空间、删除与迁移的完整回收、唯一路径规范化规则，以及路径逃逸防护，使存档语义在实现变化时仍可被校验。

## ADDED Requirements

### Requirement: Game layer and writable overlay

平台 MUST 把每个打开的游戏表示为只读源（directory handle、存档目录或 archive）叠加一个可写覆层：覆层 MUST 把引擎写入落在可写存储中，MUST NOT 修改只读源。后挂载的覆层 MUST 覆盖先挂载的，且引擎读到的字节 MUST 是覆层优先的结果。

#### Scenario: Engine writes a file during play

- **WHEN** 引擎在运行中写一个文件
- **THEN** 写入落在可写覆层，只读源保持原字节
- **AND** 同一路径的后续读取返回覆层内容

#### Scenario: An overlay is mounted over another

- **WHEN** 同一个游戏路径挂载了不止一个可写层
- **THEN** 后面挂载者的内容在读取时优先
- **AND** 未命中的路径回落到只读源

### Requirement: IndexedDB key-space ownership

`WritableStore` 的键 MUST 由 store 名与路径确定性构成。删除一个游戏 MUST 回收该游戏 id 在**全部**相关对象存储与键前缀下的记录，包括存档槽、引擎可写覆层与已导入的文件索引；删除 MUST NOT 因为前缀形状不一致而残留字节。迁移（改名/移动）MUST 把同一组键一并搬移。

#### Scenario: A user deletes a game from the library

- **WHEN** 用户在库中删除一个游戏，且该游戏已有存档槽、已有引擎写入的覆层文件、已导入的文件索引
- **THEN** 该 game id 下的存档槽、覆层字节与文件索引在删除完成后都不再存在
- **AND** 界面文案关于"存档一并移除"的承诺与实际行为一致

#### Scenario: Keys under another game id

- **WHEN** 删除或迁移一个 game id
- **THEN** 其他 game id 的记录保持原样
- **AND** 被删除集合由该 id 的键边界决定，而不是由共享前缀的形状巧合决定

#### Scenario: Store is opened with a non-default database name

- **WHEN** 调用方以非默认数据库名构造存档存储
- **THEN** 同一存储提供的记录存储与引擎可写文件存储落在同一个数据库中
- **AND** 传入的数据库名参数不被任何硬编码常量覆盖

#### Scenario: Multi-step delete or import fails midway

- **WHEN** 删除或导入需要跨越多个对象存储
- **THEN** 中间失败的残留状态被明确定义并可恢复，或全部操作在一个事务内完成
- **AND** 部分完成的状态被记录为已知行为而不是静默接受

### Requirement: Single path normalisation rule

仓库 MUST 只有一套路径规范化语义用于确定键。规范化 MUST 统一分隔符、去掉前导分隔符、丢弃空段与 `.`、并处理 `..`；在内存源与 archive 源之间，同一输入 MUST NOT 因为两套规则而产生不同键。

#### Scenario: Same path through two layers

- **WHEN** 同一路径字符串经由内存源与 archive 源各自规范化
- **THEN** 两者产生相同的规范化结果
- **AND** 差异存在时以选定的一套为准，并记录为待迁移项而不是并存两套

#### Scenario: A path attempts to escape the root

- **WHEN** 路径中包含 `..` 段
- **THEN** 结果 MUST NOT 指向游戏根之外的键
- **AND** 处理方式是显式定义的（拒绝并报错，或规范化后不逃逸），并测试覆盖

### Requirement: Archive read budget

archive 读取 MUST 明确其峰值内存：对 range 可寻址的成员走 range，对需要整段解码的成员 MUST 有可配置上限并在超限时显式失败，MUST NOT 以"lazy"命名掩盖整成员缓冲。

#### Scenario: A compressed member larger than the budget is read

- **WHEN** 调用方读取一个解压后超过预算的压缩成员
- **THEN** 读取以显式错误失败并提供该成员的大小信息
- **AND** 不会有不受限的整成员缓冲被静默分配

#### Scenario: A stored member supports ranges

- **WHEN** 成员未压缩且可寻址
- **THEN** 读取走 range 路径，不需要解出整成员
- **AND** 内存占用与请求长度成比例
