# Spec Delta

## Purpose

定义插件规则层：插件如何被声明（manifest）、按依赖排序启动、彼此提供服务、被授权、被限制、被终止，以及失败时宿主如何保持可用。插件包的分发、安装与把管理器作为站点提供由 `plugin-system` capability 负责；本 capability 只规定"一个插件能做什么"。非管理器插件的执行位置与钩子表不在本变更内（见 `proposal.md` 的待补齐项 (a)），因此 `boot` 目前没有生产调用方。

## ADDED Requirements

### Requirement: Manifest gate before code execution

插件 MUST 在 manifest 中声明 `id`、`name`、`version`、`apiVersion` 与 `provenance`（`official` | `third-party`）；manifest 中出现未知字段或字段类型不符 MUST 被拒绝。宿主 MUST 在执行该插件的任何代码之前完成 manifest 校验与 API 版本协商；`apiVersion` 与宿主支持版本不一致时 MUST 拒绝加载并返回稳定错误码，且 MUST NOT 调用该插件的 `setup`、`dispose` 或任何钩子。

#### Scenario: An incompatible plugin is loaded

- **WHEN** 插件声明 `apiVersion: 2`，宿主支持 `1`
- **THEN** 注册结果为 `failed`，错误码为 `api-version-mismatch`
- **AND** 该插件的 `setup` 与钩子从未被调用

#### Scenario: A malformed manifest is rejected

- **WHEN** manifest 缺少 `id`、版本号不是三段式，或出现未知字段
- **THEN** 注册结果为 `failed`，错误码为 `manifest-invalid`
- **AND** 错误信息指名第一个出错的字段

#### Scenario: Duplicate plugin id does not replace the active plugin

- **WHEN** 已有同 `id` 的插件处于活动状态，又以同一个 `id` 注册
- **THEN** 第二次注册返回 `failed` 与 `duplicate-plugin-id`
- **AND** 先前插件的授权、状态与钩子不受影响

### Requirement: Provenance-scoped default grants

宿主 MUST 按 `provenance` 给出不同的默认授权集，第三方的默认集 MUST 为空。默认授权 MUST 只对 manifest 显式声明过的 capability 生效。`grant` MUST 只接受该插件 manifest 声明过的 capability，否则拒绝并返回 `capability-not-requested`。任何插件（含官方）都 MUST NOT 获得 manifest 之外的权限，也 MUST NOT 绕过 `require` 检查直接触达宿主对象。

#### Scenario: A third-party plugin has no default capability

- **WHEN** 第三方插件在没有宿主显式授予的情况下调用 `vfs.readFile`
- **THEN** 调用以 `capability-denied` 失败
- **AND** 拒绝被记入该插件的拒绝记录，且不产生任何读取副作用

#### Scenario: An official plugin requests more than it declared

- **WHEN** 宿主尝试给官方插件授予一个 manifest 未声明的 capability
- **THEN** 授予被拒绝并返回 `capability-not-requested`
- **AND** 该插件的授权集不发生变化

### Requirement: Session-scoped grant and revocation

授权 MUST 绑定到插件的一次加载会话；宿主 MUST 能在会话进行中撤销单项授权，撤销 MUST 对后续调用立即生效。加载会话结束（卸载、失败终止或宿主销毁）时 MUST 清空该插件的全部授权，且后续会话 MUST NOT 继承。

#### Scenario: A grant is revoked mid-session

- **WHEN** 宿主在插件运行期间撤销 `vfs.read`
- **THEN** 该插件下一次 `vfs.readFile` 被拒绝，错误码为 `capability-denied`
- **AND** 撤销前已完成的读取不被回滚

#### Scenario: Unloading revokes everything

- **WHEN** 插件被卸载或因超限被终止
- **THEN** 其授权集为空，且任何仍持有上下文的调用都被拒绝

### Requirement: Controlled host surface

插件 MUST 只通过宿主构造的上下文访问外部：`log`、只读 `vfs`、私有 `store`、经宿主代理的 `net`、`notify`、`now`、`use`（已声明依赖的服务）与 `plugins`（特权）。宿主 MUST NOT 把原始 DOM、IndexedDB、Worker、引擎句柄、`fetch` 或其他未受控网络原语交给插件。未配置的宿主能力（包括尚未挂载游戏时的 `vfs`、宿主没有可写层时的 `store`）被调用时 MUST 返回 `capability-unavailable`，MUST NOT 静默降级为其他行为。

#### Scenario: A plugin reaches for an unconfigured network proxy

- **WHEN** 插件在宿主未注入代理时调用 `net.request`
- **THEN** 调用以 `capability-unavailable` 失败
- **AND** 宿主不发起任何网络请求

#### Scenario: Plugin writes do not touch game files

- **WHEN** 插件调用 `store.write`
- **THEN** 写入落在宿主可写层的 `plugins/<pluginId>/` 前缀下
- **AND** 插件无法通过 `store` 读到其他插件或游戏文件的内容

### Requirement: Path boundary for plugin file access

插件提供的路径 MUST 在规范化之前被校验：绝对路径（盘符、UNC、前导分隔符）、含 NUL 的路径、以及任何含 `..` 段的路径 MUST 被拒绝并返回 `path-outside-root`。只有校验通过的路径才允许交给路径规范化与 VFS 访问。

#### Scenario: A plugin tries to escape the game root

- **WHEN** 插件请求 `../../secrets.txt` 或 `C:\\Windows\\win.ini`
- **THEN** 调用以 `path-outside-root` 失败
- **AND** 游戏源与宿主存储都没有被读取

#### Scenario: A normal relative path still works

- **WHEN** 已获 `vfs.read` 的插件请求 `script/main.ks`
- **THEN** 读取成功并返回字节
- **AND** 读取的字节数被计入该插件的内存预算

### Requirement: Resource budget enforcement

宿主 MUST 对每次插件激活与每次钩子调用施加时间预算，并对经宿主代理的内存分配（读入、请求体、响应体）计数；超限 MUST 以 `budget-exceeded` 终止该插件的执行单元（转 `failed`、清空授权、调用 `dispose`），MUST NOT 阻断宿主或其他插件。由于当前执行模型是进程内同步调用，预算 MUST 被表述为**协作式**：在宿主可观测的边界检查，不声称能抢占正在执行的同步代码；若要硬抢占，MUST 改用隔离执行环境并单独评审。

#### Scenario: A hook exceeds its time budget

- **WHEN** 一次钩子调用返回时已超过该插件的 `timeMs`
- **THEN** 结果为 `ok: false` 与 `budget-exceeded`
- **AND** 该插件转 `failed`，其他插件仍能继续收到同一钩子

#### Scenario: A read exceeds the memory budget

- **WHEN** 插件读取的字节数使已计入内存超过其预算
- **THEN** 调用以 `budget-exceeded` 失败
- **AND** 宿主页面不因该次分配进入不可恢复状态

### Requirement: Failure isolation and diagnostics

单个插件的 `setup` 抛错、钩子抛错或超限 MUST 只影响该插件：宿主 MUST 将其标记为 `failed` 并保留稳定错误码与消息，MUST 继续派发同一钩子给其余活动插件，MUST 保持宿主自身可用。宿主 MUST 能列出每个插件的状态、授权集、拒绝次数与失败原因。

#### Scenario: A plugin throws during setup

- **WHEN** 插件的 `setup` 抛出异常
- **THEN** 注册结果为 `failed` 且错误可读
- **AND** 已注册的其他插件保持 `active`

#### Scenario: A hook throws

- **WHEN** 一个活动插件的钩子抛出异常
- **THEN** 该插件转 `failed`，其 `dispose` 被调用，授权被清空
- **AND** 同一钩子的结果列表仍包含其余插件的正常结果

### Requirement: Boot with deferred code loading

插件 MUST 以条目交给宿主；条目 MUST 由未受信的 manifest 与独立的 `load()` 组成。宿主 MUST 在调用某插件的 `load()` 之前完成该插件的 manifest 校验、API 版本协商、重名检查、授权与预算构建、服务冲突检查与依赖图检查；任一项失败 MUST NOT 调用 `load()`。`load()` 抛错或返回没有 `setup` 的模块 MUST 只让该插件转 `failed`。同一批内，提供者均已活动的插件 MUST 同时开始获取代码（按依赖层并行），消费者 MUST 在其提供者全部活动之后才获取代码；`setup` MUST 仍按依赖顺序逐个执行，与代码到达的先后无关。批量启动 MUST 为每个条目按输入顺序返回一条结果，MUST NOT 因单个条目抛出。并发的启动 MUST 被串行化。宿主 MUST 能在没有游戏挂载、没有可写层的情况下启动。

#### Scenario: A rejected plugin's code is never fetched

- **WHEN** 一批条目中有 manifest 非法、依赖无人提供、依赖成环的插件
- **THEN** 它们的 `load()` 从未被调用，结果分别为 `manifest-invalid`、`dependency-missing`、`dependency-missing`
- **AND** 同批其余合法插件照常进入 `active`

#### Scenario: Independent plugins are fetched together

- **WHEN** 一批中 `library` 与 `settings` 互不依赖，`manager` 依赖 `library` 提供的服务
- **THEN** `library` 与 `settings` 的 `load()` 在任何一个完成之前都已被调用
- **AND** 即使 `settings` 的代码先到，`setup` 顺序仍为 `library`、`settings`
- **AND** `manager` 的 `load()` 在 `library` 活动之后才被调用

#### Scenario: Code fails to load

- **WHEN** 某插件的 `load()` 抛出（例如动态导入失败）
- **THEN** 该插件结果为 `plugin-failed`，错误信息保留原因
- **AND** 同批其余插件的启动不受影响

#### Scenario: The host boots before a game is chosen

- **WHEN** 宿主在没有游戏源与可写层时启动，插件调用 `vfs.readFile` 或 `store.read`
- **THEN** 调用以 `capability-unavailable` 失败

### Requirement: Service graph between plugins

插件 MUST 在 manifest 中用 `provides` 声明向其他插件发布的服务、用 `requires` 声明所需服务；同一插件 MUST NOT 依赖自己提供的服务。宿主 MUST 先激活提供者再激活消费者，与输入顺序无关。每个服务 MUST 只有一个提供者：已活动的提供者优先，其次输入顺序在前者；后来者 MUST 以 `service-conflict` 失败。`setup` 的返回值 MUST 恰好包含 `provides` 列出的服务，且每个服务至少有一个方法，否则以 `service-invalid` 失败。消费者 MUST 只能通过 `use` 取得自己 `requires` 声明过的服务，未声明的以 `capability-denied` 拒绝。消费者拿到的 MUST 是只含方法的冻结外观对象，每次调用 MUST 先确认提供者仍处于活动状态。

#### Scenario: Providers start before consumers

- **WHEN** 消费者条目排在提供者之前交给宿主
- **THEN** 提供者的 `setup` 先于消费者执行，消费者在 `setup` 中即可调用该服务

#### Scenario: A provider fails during setup

- **WHEN** 某服务的提供者 `setup` 抛错
- **THEN** 提供者为 `plugin-failed`，依赖它的消费者为 `dependency-failed`
- **AND** 消费者的 `load()` 从未被调用

#### Scenario: Two plugins provide the same service

- **WHEN** 两个插件都声明提供同一服务，无论同批还是并发启动
- **THEN** 先者 `active`，后者 `service-conflict`

#### Scenario: A consumer keeps a facade after its provider stops

- **WHEN** 提供者被卸载后，消费者仍调用之前取得的外观对象
- **THEN** 调用以 `dependency-failed` 失败

### Requirement: Privileged capabilities are host-approved

`plugins.manage`（查看、授权、撤销、卸载其他插件）是特权能力：只有 `official` 插件 MAY 在 manifest 中声明，第三方声明 MUST 以 `manifest-invalid` 拒绝；特权能力 MUST NOT 出现在任何默认授权集中，MUST 只能由宿主在启动配置中按插件 id 批准；持有 `plugins.manage` 的插件 MUST NOT 授出特权能力。

#### Scenario: An official plugin declares a privileged capability without approval

- **WHEN** 官方插件声明 `plugins.manage`，但宿主未按 id 批准
- **THEN** 该插件的授权集为空，调用 `plugins.list` 以 `capability-denied` 失败

#### Scenario: The manager tries to escalate

- **WHEN** 被批准 `plugins.manage` 的插件调用 `plugins.grant(..., 'plugins.manage')` 给另一个声明了它的插件
- **THEN** 调用以 `capability-denied` 失败，任何插件的授权集都不变

### Requirement: Ordered teardown

停止一个提供者时，宿主 MUST 先以 `dependency-failed` 停止依赖它的消费者（新激活者先停），且在消费者 `dispose` 期间提供者 MUST 仍能应答。宿主整体销毁 MUST 按激活顺序的逆序停止全部插件。停止的插件 MUST 失去授权与它通过 `plugins.subscribe` 注册的全部订阅。

#### Scenario: Unloading a provider

- **WHEN** 宿主卸载一个仍有消费者的提供者
- **THEN** 消费者的 `dispose` 先于提供者的 `dispose` 执行
- **AND** 提供者为 `unloaded`，消费者为 `failed` 且错误码为 `dependency-failed`

#### Scenario: Disposing the host

- **WHEN** 宿主销毁，插件按 `mount → engine → manager` 的顺序激活过
- **THEN** `dispose` 按 `manager → engine → mount` 的顺序执行，全部插件为 `unloaded`
