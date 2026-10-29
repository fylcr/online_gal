# Spec Delta

## Purpose

定义设置页、引擎/支持说明页与已知限制页。

## ADDED Requirements

### Requirement: Settings feature parity
设置页 MUST 保留现有设置项（静音、各类音量、文字速度、显示表面、诊断信息）及其持久化，以及现有的数据管理操作。修改设置 SHALL 立即保存并在下次进入播放器时生效。

#### Scenario: Volume persists
- **WHEN** 用户修改音量后刷新页面
- **THEN** 设置页显示修改后的值

### Requirement: Engine support page reflects the build
引擎/支持说明页 MUST 按当前构建实际注册的 provider 显示每个引擎是否可用，不可用时显示 platform 给出的原因。页面 MUST NOT 把仅识别的引擎描述为可运行。

#### Scenario: Beta build
- **WHEN** 在只开放 ONS 与 KiriKiri 的构建中打开该页
- **THEN** 只有 ONS 与 KiriKiri 显示为可用，其余显示不可用原因

### Requirement: Known limitations page
站点 MUST 提供已知限制说明，内容来自 `README.md` 的支持范围（例如编译后的 TJS2 `.ks` 不支持、不支持插件），并说明游戏文件只在浏览器本地处理、不会上传。

#### Scenario: Linked from library
- **WHEN** 用户在游戏库或不支持提示处点击“了解限制”
- **THEN** 打开已知限制页
