# Spec Delta

## Purpose

定义重构后播放器页面的外壳、控制与存档功能。

## ADDED Requirements

### Requirement: Player feature parity
重构后的播放器 MUST 保留现有全部功能：按 `?id=` 加载游戏、暂停/继续、全屏、现有键盘快捷键、存档到槽位（含缩略图与时间）、读取存档、删除存档、导出与导入全部存档、应用设置（音量、静音、文字速度、显示表面）、诊断信息开关、返回游戏库并正确停止会话。

#### Scenario: Save and load
- **WHEN** 用户在播放中存档到某槽位，再读取该槽位
- **THEN** 槽位显示缩略图与时间
- **AND** 读取后游戏恢复到该存档

#### Scenario: Leave player
- **WHEN** 用户从播放器返回游戏库
- **THEN** 当前会话被停止，不残留音频或引擎实例

### Requirement: Game surface has priority
播放中游戏画面 MUST 占据主要区域并保持原始宽高比；控制栏 SHALL 可隐藏或自动淡出，不遮挡游戏输入区域。

#### Scenario: Fullscreen
- **WHEN** 用户进入全屏
- **THEN** 游戏画面按宽高比铺满屏幕，控制栏不常驻遮挡

### Requirement: Clear loading and failure states
播放器 MUST 区分“加载中”、“游戏不存在”、“本构建不支持该引擎”、“启动失败”四种状态，并给出原因与返回游戏库的入口。不支持的游戏 MUST NOT 创建引擎实例。

#### Scenario: Unknown id
- **WHEN** 打开的 `/play?id=` 不对应任何游戏
- **THEN** 显示“游戏不存在”与返回入口
