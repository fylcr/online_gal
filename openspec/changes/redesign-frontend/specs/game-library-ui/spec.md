# Spec Delta

## Purpose

定义重构后游戏库页面必须提供的功能与可运行状态的呈现方式。

## ADDED Requirements

### Requirement: Library feature parity
重构后的游戏库 MUST 保留现有全部功能：选择文件夹导入（`showDirectoryPicker`，不可用时回退到目录 input）、选择文件/压缩包导入、拖放导入、导入后提示识别结果（含低置信度提示）、搜索（名称、标题、标签）、按引擎筛选、按最近游玩/名称/添加时间排序、游戏详情、重新识别、删除（含其存档，需确认）、游玩时长与最近游玩时间显示。

#### Scenario: Import by drag and drop
- **WHEN** 用户把一个游戏文件夹拖入游戏库
- **THEN** 显示导入进度
- **AND** 完成后提示识别出的引擎，新游戏出现在列表中

#### Scenario: Delete requires confirmation
- **WHEN** 用户删除一个游戏
- **THEN** 先要求确认，确认后游戏与其存档都被移除

### Requirement: Truthful playability state
每个游戏卡片与详情 MUST 根据 platform 给出的状态显示三类之一：可开始、不支持（显示 platform 给出的原因，例如“本测试版不支持”）、未识别。只有可开始的游戏 SHALL 显示“开始”入口。界面 MUST NOT 硬编码引擎名单。

#### Scenario: Unsupported title has no start button
- **WHEN** 游戏的引擎在当前构建中不可用
- **THEN** 卡片显示 platform 给出的不支持原因
- **AND** 卡片上没有“开始”按钮

### Requirement: Browser capability warning
当 `browserSupport()` 报告缺少运行所需能力（如跨源隔离、SharedArrayBuffer）时，游戏库 MUST 显示说明缺少什么的提示，而不是让用户在启动时才失败。

#### Scenario: Not cross-origin isolated
- **WHEN** 页面在 `crossOriginIsolated === false` 的环境中打开
- **THEN** 游戏库顶部显示缺少跨源隔离的提示

### Requirement: Empty and error states
游戏库 MUST 为“尚无游戏”、“搜索无结果”、“初始化失败”分别提供明确的界面，空库状态 MUST 直接给出导入入口。

#### Scenario: First visit
- **WHEN** 用户首次打开且库为空
- **THEN** 页面显示导入入口与支持的导入方式说明
