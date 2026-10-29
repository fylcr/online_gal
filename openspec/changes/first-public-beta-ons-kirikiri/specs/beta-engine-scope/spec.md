# Spec Delta

## Purpose

定义公开 beta 构建中允许注册、显示与启动的引擎集合，以及对 beta 范围外但已被识别的游戏如何如实呈现，避免任何非 ONS/KiriKiri 引擎在 beta 中显得可运行。

## ADDED Requirements

### Requirement: Beta can start only ONS and KiriKiri providers
beta 构建中可启动的 provider 集合 MUST 是 `onscripter-wasm`、`onscripter`、`krkr2-wasm`、`krkr2` 的子集，具体是否含两个解释器 fallback 由用户决定。Artemis、Siglus、TyranoBuilder、web-html5 provider MUST NOT 以可启动形式注册；beta 只允许为它们保留仅识别条目，其可用性 MUST 为不可用且不能创建实例。非 beta（开发）构建 MUST 保持现有完整注册集合不变。

#### Scenario: Beta build registry contents
- **WHEN** 以 beta 形态构建前端并列出可启动 provider 的 runtimeId
- **THEN** 列表只含上述四个 runtimeId 中被批准的那些
- **AND** Artemis、Siglus、Tyrano、web 若出现，只作为不可用的仅识别条目，尝试创建实例会被拒绝

#### Scenario: Development build unchanged
- **WHEN** 以非 beta 形态构建或启动开发服务器
- **THEN** 已注册 provider 集合与本 change 之前相同

### Requirement: Out-of-scope engines remain in the repository
beta 范围裁剪 MUST NOT 删除或移出 Artemis、Siglus、Tyrano、web 的源码包，MUST NOT 改变它们的 runtimeId，且 beta 构建 MUST NOT 把它们的引擎字节（例如 Artemis compat 产物）输出到站点目录。

#### Scenario: Excluded engine bytes absent from beta output
- **WHEN** beta 构建完成
- **THEN** 站点输出目录中没有 `/engines/artemis-compat/` 或其他范围外引擎的产物
- **AND** 仓库中这些包的源码仍存在

### Requirement: Detected out-of-scope games show as unsupported in beta
beta 构建中，被识别为范围外引擎的游戏 MUST 在库卡片与详情中显示“本测试版不支持”，MUST NOT 提供可点击的“开始”入口，且直接访问其播放地址 MUST 给出同样的不支持说明而不是尝试启动。

#### Scenario: Artemis title imported into beta
- **WHEN** 用户在 beta 站点导入一个含 `root.pfs` / `system.ini` 的 Artemis 游戏
- **THEN** 库卡片显示“本测试版不支持”
- **AND** 卡片上没有“开始”按钮

#### Scenario: Direct play URL for unsupported title
- **WHEN** 用户直接打开该游戏的 `/play?id=<id>`
- **THEN** 页面显示“本测试版不支持”，不创建引擎实例

### Requirement: Engine panel lists only beta engines as available
beta 构建的引擎面板 MUST 只把 beta 注册集合中的引擎列为可用。范围外引擎若被列出，MUST 标注“本测试版不支持”，MUST NOT 标注“可用”。

#### Scenario: Engine panel in beta
- **WHEN** 用户打开 beta 站点的 `/engines`
- **THEN** 只有 ONS 与 KiriKiri 相关条目可能显示“可用”
- **AND** 其他引擎不显示“可用”
