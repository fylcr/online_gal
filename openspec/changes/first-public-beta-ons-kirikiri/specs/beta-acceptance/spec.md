# Spec Delta

## Purpose

定义公开 beta 的验收证据：由用户亲自执行的真实游戏矩阵、浏览器矩阵与已知限制页，并把构建成功、合成测试、真实游戏可玩、再分发许可分开记录，避免用组件测试替代真实游戏结论。

## ADDED Requirements

### Requirement: Real-game acceptance is user-run and repeated
beta 的真实游戏验收 MUST 由用户本人执行，代理 MUST NOT 代跑或替用户宣告通过。每个标题、每个浏览器的每项检查 MUST 丢弃一次热身运行，再取两次结果一致的运行作为判据；只有一次通过 MUST 记为未定。

#### Scenario: Inconsistent runs
- **WHEN** 某标题热身后的两次运行一次通过、一次失败
- **THEN** 该项记为未定，不计入通过

#### Scenario: Agent-produced result
- **WHEN** 验收记录中某项由代理运行得出
- **THEN** 该项不计入 beta 真实游戏验收

### Requirement: Acceptance matrix covers core play functions
验收矩阵 MUST 至少包含用户选定的 N 个 ONS 标题与 N 个 KiriKiri 标题（N 待用户决定），每个标题 MUST 记录：启动到可交互、文本推进与选项输入、音频（BGM 与音效）、存档与读档、刷新页面后读回存档。每项结果 MUST 为通过、失败或未定之一并附说明。

#### Scenario: Matrix row complete
- **WHEN** 用户完成一个标题的验收
- **THEN** 该行五项均有结果，且记录浏览器与版本、站点版本、引擎 tag

#### Scenario: Save survives reload
- **WHEN** 用户存档后刷新页面并读档
- **THEN** 读回到存档位置，记为通过；否则记为失败

### Requirement: Browser matrix is declared before public release
beta MUST 在公开前声明支持的浏览器及最低版本（由用户决定），站点 MUST 在不满足能力（例如缺 `crossOriginIsolated`、WebAssembly 或 KiriKiri 所需的 JSPI）时给出明确提示，而不是卡住。

#### Scenario: Unsupported browser
- **WHEN** 用户用缺少 KiriKiri 所需能力的浏览器启动 KiriKiri 游戏
- **THEN** 页面说明当前浏览器不受支持，不进入无响应状态

### Requirement: Known-limitations page is published
beta 站点 MUST 提供已知限制页，至少列出：只支持 ONS 与 KiriKiri；解释器拒绝编译型 TJS2 `.ks`；不支持 TVP 原生插件（`loadplugin`）；`[movie]` 等视频命令被跳过；XP3 只处理 XOR 混淆；以及验收中记为失败或未定的项。

#### Scenario: Limitations page reachable
- **WHEN** 用户从 beta 站点首页进入已知限制页
- **THEN** 能读到上述每一条限制

### Requirement: Evidence conclusions are recorded separately
beta 验收记录 MUST 分别给出五个结论：产物目录完整、构建成功、合成测试通过、真实游戏可玩、再分发许可已确认。任一结论 MUST NOT 由其他结论推出。

#### Scenario: Build passes but no real-game run
- **WHEN** 构建与合成测试通过，但用户尚未完成真实游戏矩阵
- **THEN** “真实游戏可玩”记为未定，beta 不得公开
