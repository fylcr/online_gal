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

### Requirement: Delete games from the card context menu

每张游戏卡片 MUST 在右键菜单提供「从游戏库删除」。选择该项 MUST 打开确认窗口，显示游戏名称并说明本站保存的该游戏存档会一并删除。用户确认后，平台 MUST 删除所选游戏的库记录和本站存档。用户磁盘上的原始游戏文件夹和文件 MUST 保持原样。游戏详情 MUST 保留同一个删除流程，供键盘和触屏用户使用。

#### Scenario: Remove the right-clicked game
- **WHEN** 用户右键一张游戏卡片，选择「从游戏库删除」并确认
- **THEN** 游戏库移除所选条目及其本站存档，其他条目和存档保持原样
- **AND** 刷新后该卡片仍已移除，原始游戏文件保持原样

#### Scenario: Cancel card deletion
- **WHEN** 用户关闭右键菜单或取消删除确认
- **THEN** 游戏记录、存档和卡片保持原样

### Requirement: Game library shows real records and an add button

游戏库 MUST 从 platform 显示真实游戏记录。页面 MUST 在游戏库视口右下角提供圆形「添加游戏」按钮，颜色使用当前主题的主要操作和对应文字色令牌。按钮 MUST 支持键盘与触控，并为卡片与移动端安全区留出空间。

#### Scenario: Open the library with and without games

- **WHEN** 用户打开游戏库
- **THEN** 空库显示空状态，非空库只显示真实记录
- **AND** 两种情况下都能用右下角的「添加游戏」打开导入入口

### Requirement: Play an extracted local game folder

用户 MUST 能从「添加游戏」选择一个已解压的本地游戏文件夹。平台 MUST 保留所选目录内的相对路径，通过现有只读 `GameSource` 读取文件，运行时写入 MUST 进入每个游戏的存档覆层。支持目录句柄时 MUST 按需读取文件及字节范围。目录选择器不可用时，游戏库 MUST 在浏览器支持的情况下提供目录 input 回退。

#### Scenario: Import, then open a local game

- **WHEN** 用户选择含有嵌套目录的已解压游戏根目录
- **THEN** 游戏库显示识别结果及真实记录
- **AND** 可启动的条目经同一记录 ID 打开播放器，VFS 按完整相对路径向引擎提供文件

#### Scenario: Directory permission expires after reload

- **WHEN** 刷新页面后打开条目，而目录授权已失效或文件夹已移动
- **THEN** 页面提供用户可触发的重新授权或重选入口
- **AND** 恢复成功后保留原记录 ID 和存档，取消或失败时保留原条目以便重试

#### Scenario: Folder input fallback preserves nested paths

- **WHEN** 浏览器没有可用的目录句柄，而用户从目录 input 选择含 `config.txt` 与 `sub/config.txt` 的目录
- **THEN** VFS 从各自的完整相对路径返回对应内容，刷新后也可以打开已导入的游戏

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
