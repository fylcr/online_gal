# Spec Delta

## ADDED Requirements

### Requirement: Official local search is a separate Orama plugin

官方管理器 MUST 用独立的 `online-gal.search` 官方插件提供 Orama 全文搜索。插件 MUST 通过既有官方安装、完整性和信任校验，并在管理器发起首个非空查询时按需加载到独立浏览器 Worker。管理器页面与 Service Worker 内核 MUST NOT 内置 Orama。加载 MUST NOT 成为游戏库首屏或基础操作的前置条件。

#### Scenario: Load only when search is used
- **WHEN** 全新浏览器打开游戏库或设置页，或用户仅打开搜索输入框
- **THEN** 静态页面仍按原规则绘制，浏览器不启动或下载搜索 Worker
- **AND** 进入游戏库并执行非空查询后，管理器使用已验证的官方插件获取增强结果，不重新加载页面

#### Scenario: Official search is not available
- **WHEN** Service Worker 尚未接管，或插件缺失、未通过校验、无法加载、崩溃或已卸载
- **THEN** 原有搜索框、名称/标题/标签基础筛选和游戏库操作仍可用
- **AND** 未验证的代码不得执行，用户卸载后不自动装回，除非其显式重装或重置插件状态

### Requirement: Local index follows the current library

插件 MUST 用 Orama 索引当前游戏库的 ID、名称、标题和标签，支持中文、日文和拉丁字母查询。管理器 MUST 使用当前记录及现有排序和引擎筛选展示匹配项。查询词与索引元数据 MUST NOT 被发送到网络、放入 URL 或记录到日志/遥测。索引 MUST NOT 读取或持有游戏文件、路径、文件夹权限、封面或存档。

#### Scenario: Search multilingual metadata locally
- **WHEN** 用户搜索命中当前库的中文或日文名称、拉丁标题或标签
- **THEN** 对应记录进入当前搜索结果，清空后恢复完整列表，空库与无匹配保持不同状态
- **AND** 只有插件静态资产可因插件加载而下载，查询和元数据留在浏览器

#### Scenario: Edits and late results do not resurrect old cards
- **WHEN** 查询进行中又输入新词、导入、改名、改标签或删除游戏
- **THEN** 管理器只显示最新查询及当前库对应的结果，已删除或不再匹配的条目不会因迟到响应重现

### Requirement: Search artifacts follow the official release chain

搜索插件 MUST 独立打包及发布，站点构建 MUST 用锁定并校验过的制品装配同源官方注册表，沿用签名策略。setup 构建 MUST NOT 下载搜索制品。站点 MUST NOT 为搜索新增服务端运行时或凭据。

#### Scenario: Assemble signed search independently
- **WHEN** 已初始化的站点装配启用了搜索插件的发布引用
- **THEN** 发布使用已校验的插件制品，签名构建对搜索包应用与其他官方包相同的签名规则
- **AND** 断网时已缓存的搜索插件仍可查询本地库，未缓存时使用基础筛选

#### Scenario: A corrupt artifact does not replace the last deployment
- **WHEN** 启用了搜索插件的构建遇到错误的归档摘要、文件哈希或签名失败
- **THEN** 构建失败，上一成功部署仍可用
