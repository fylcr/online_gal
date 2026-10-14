# Spec Delta

## Purpose

定义 ONS 与 KRKR 昂贵 native/WASM 构建和 GitHub Release 的同一触发条件，以及 PR、drift、手动派发与普通 packages CI 的拒绝边界。

## ADDED Requirements

### Requirement: Expensive build and release share one automatic condition
ONS 与 KRKR 的昂贵 native/WASM 构建及其成功后的 GitHub Release MUST 使用同一自动条件。仅当 `main` push 的完整 `before..after` 区间包含该 runtime 自己的发布输入变化时，系统 MUST 运行该引擎构建并在成功后发布。未命中时系统 MUST NOT 运行该昂贵构建，也 MUST NOT 发布。

#### Scenario: ONS release input changes
- **WHEN** `main` push 的完整区间包含 `packages/runtime-ons` 的发布输入变化
- **THEN** 系统只运行 ONS 昂贵构建，并在成功后发布一个 ONS 版本
- **AND** KRKR 昂贵构建与 KRKR Release 都不发生

#### Scenario: KRKR release input changes
- **WHEN** `main` push 的完整区间包含 `packages/runtime-kag` 的发布输入变化
- **THEN** 系统只运行 KRKR 昂贵构建，并在成功后发布一个 KRKR 版本
- **AND** ONS 昂贵构建与 ONS Release 都不发生

#### Scenario: Non-release input changes
- **WHEN** push 只改变 Astro、根 lock 或根 package、公共包、toolchain、port、workflow、许可证、上游 pin、runtime README/docs，或 ONS committed assets
- **THEN** 对应昂贵引擎构建次数为 0，发布次数为 0

### Requirement: Release inputs are package-owned
ONS 发布输入 MUST 仅为 `packages/runtime-ons/src/**`、`packages/runtime-ons/scripts/**`、`packages/runtime-ons/package.json` 与 `packages/runtime-ons/tsconfig.json`。KRKR 发布输入 MUST 仅为 `packages/runtime-kag/src/**`、`packages/runtime-kag/package.json` 与 `packages/runtime-kag/tsconfig.json`。系统 MUST NOT 把 `packages/tjs2-bytecode`、兄弟包、ports 或共享 lock 当作自动发布输入。

#### Scenario: ONS script changes
- **WHEN** 完整 push 区间只改变 `packages/runtime-ons/scripts/**`
- **THEN** ONS 昂贵构建与 ONS 发布都发生
- **AND** KRKR 昂贵构建次数为 0

#### Scenario: Sibling package changes
- **WHEN** 完整 push 区间只改变 `engine-api`、`vfs`、`image-codec` 或 `player-host`
- **THEN** ONS 与 KRKR 的昂贵构建和发布次数都为 0

### Requirement: Push range is exact and fail-closed
自动判定 MUST 使用触发事件给出的精确 `before` 与 `after`，覆盖该区间内所有提交、删除与重命名。首次 push、全零 before、缺失历史或非法 SHA MUST 在 native 构建前失败关闭。系统 MUST NOT 使用 `HEAD~1`、只检查最后一次提交，或把未验证的事件路径直接拼进 shell。

#### Scenario: Multi-commit rename
- **WHEN** 一次 push 含多个提交，并把发布输入重命名到允许路径内或从允许路径内移出
- **THEN** 该 runtime 视为已变化并进入昂贵构建与发布
- **AND** 判定覆盖区间两端，而不是只看最后提交

#### Scenario: Missing history
- **WHEN** before 为全零、SHA 非法，或本地历史缺少事件端点
- **THEN** 系统在 native 构建前失败
- **AND** 不创建 Release，也不猜测应否发布

### Requirement: Manual publish requires an explicit valid version
手动 workflow MUST 在 `publish` 被显式选为 true 且对应引擎收到合法 SemVer 时才允许昂贵构建。未选择 publish、空白版本或非法版本 MUST 在昂贵构建前拒绝。系统 MUST NOT 用空白默认值或旧的硬编码引擎版本兜底。

#### Scenario: Explicit valid manual request
- **WHEN** 操作者显式选择 publish 并提交该引擎的合法 SemVer
- **THEN** 系统允许该引擎的昂贵构建，并仅在构建成功后发布该版本

#### Scenario: Missing or invalid manual request
- **WHEN** publish 未选择、版本空白，或版本不是合法 SemVer
- **THEN** 系统在昂贵构建前拒绝
- **AND** native/WASM 构建次数为 0，发布次数为 0

### Requirement: Drift reports without dispatch
上游 drift MUST 只产生报告。它 MUST NOT 自动派发 ONS、KRKR、Artemis 或其他引擎的构建或发布。

#### Scenario: Upstream commit moves
- **WHEN** drift 检查发现上游 pin 与远端提交不同
- **THEN** 系统记录该 drift
- **AND** 不派发 workflow，也不创建 Release

### Requirement: Lightweight checks remain independent
PR MUST NOT 运行 ONS/KRKR 的昂贵 native/WASM，也 MUST NOT 发布这两个 runtime。该限制 MUST NOT 删除 `build-packages.yml` 既有 g++ portable probes、轻量测试或类型门禁。普通 packages CI MUST 继续提供类型与 unit 检查；其 push MUST 构建但不发布。

#### Scenario: Pull request
- **WHEN** 任意相关改动进入 pull request
- **THEN** ONS/KRKR 昂贵 native/WASM 构建次数为 0，这两个 runtime 的发布次数为 0
- **AND** `build-packages.yml` 既有 g++ portable probes 与轻量检查仍可运行

#### Scenario: Packages workflow push
- **WHEN** 普通 packages workflow 因 TypeScript 包变化在 push 时运行
- **THEN** 系统执行其既有构建或检查
- **AND** 不创建 GitHub Release

### Requirement: Existing restricted publish paths stay closed
本策略的不可变发布 MUST 只由 ONS/KRKR caller 显式开启，MUST NOT 成为所有路径的一刀切。compat MUST 保持字面 `publish: false`；bridge MUST 保持既有手动 opt-in 与现有 clobber 语义。允许修改 reusable 共用步骤，但 MUST NOT 扩大 Artemis 的新自动或手动权限。

#### Scenario: Reusable workflow is adapted
- **WHEN** 后续实施修改 ONS/KRKR 共用的 workflow 调用约定
- **THEN** Artemis compat 仍是字面 `publish: false`
- **AND** Artemis bridge 仍使用既有手动 opt-in 与现有 clobber 语义
