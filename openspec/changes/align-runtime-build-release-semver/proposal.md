# Proposal

## Why

`main` 上一次匹配路径的 push 现在会让 ONS 与 KiriKiri 既做昂贵的 native/WASM 构建，又用固定 tag 和 `--clobber` 覆盖同一条 GitHub Release。本地未推送的 Astro 7 升级改了根 `package.json` 与 `pnpm-lock.yaml`，而这两个文件正在两条引擎 workflow 的 push 路径里，因此不能推送。需要把“是否构建并发布”收成同一条件：只有对应 runtime 自己的发布输入在 push 区间内变化时才做该引擎的昂贵构建并发布一个不可变版本。

## What Changes

- ONS（`packages/runtime-ons`）与 KRKR（`packages/runtime-kag`，不是 `tjs2-bytecode`）的昂贵 native/WASM 构建与 GitHub Release 使用同一自动条件：`main` push 的精确 `before..after` 区间碰到该 runtime 的发布输入才运行，成功后发布一个新的独立版本。
- 发布输入只包括该包自己的 `src/**`、自己的 `scripts/**`（ONS 有，KAG 没有）、`package.json` 与 `tsconfig.json`。runtime README/docs、committed `assets/**`、ports、共享包、根 lock、Astro、workflow、许可证与上游 pin 的单独变化都不自动构建、不自动发布。
- 普通 TypeScript/unit 的 packages CI 保留；PR 不跑 ONS/KRKR 引擎 native/WASM，也不发布这两个 runtime。这不删除 `build-packages.yml` 既有 g++ portable probes、轻量测试或类型门禁。
- 手动发布必须显式选择 `publish=true` 并填写该引擎的合法 SemVer；未选择、空白或非法版本在昂贵构建前拒绝。上游 drift 只报告，不派发构建或发布。
- 每次 ONS/KRKR 发布使用不可变 tag `engine-ons-<SemVer>` 或 `engine-krkr-<SemVer>`。已有 tag、release 与 asset 保持原字节；这两条 caller 禁止 `--clobber`、复用已有 release，以及把上游版本冒充 runtime 版本。不可变模式不是所有发布路径的一刀切；Artemis 保持现有关闭语义。
- 自动版本以 tracked package version 的 `major.minor.patch` 为 BASE，固定为 `BASE-beta-YYYYMMDDHHmm`（构建开始时的 `Asia/Shanghai`，12 位）。不回写 tracked `package.json`，不自动递增 patch。手动版本接受完整合法 SemVer（含 `+` 构建元数据），不含 `v` 前缀，不留空白；规范字符串原样保留，不按 precedence 去掉 `+`。
- 历史 catalog pin 继续指向旧 tag。新 release 不自动改生产默认引擎或主源码 ledger；人工核验后的 catalog 更新单独审批。消费方必须显式选择正式版或先行版，不读取 SemVer “latest”。
- **BREAKING（仅相对未批准的旧政策，不改其文件）**：`establish-architecture-and-release-baseline` 的“普通 push 永不发布”以及 task 5.2“只允许显式派发发布”与本目标冲突。本 change 单独声明取代该发布政策部分，待用户整体更新旧计划；旧 34 项任务保持未勾选、原字节不变，本提案不宣称它们已批准。

## Capabilities

### New Capabilities

- `runtime-release-gating`: 决定 ONS/KRKR 何时做昂贵构建、何时拒绝、drift/PR/普通 packages CI 各自做什么。
- `runtime-semver-identity`: 定义自动与手动 SemVer、UTC+8 时间、不可变 tag，以及 tag/包名/manifest/文件名的一致身份。
- `runtime-release-consumption`: 定义历史 pin、新版本 catalog 与正式/先行版消费的兼容边界。

### Modified Capabilities

- 无。`openspec/specs/` 目前没有已落地 capability。旧 change 里的 `third-party-release-governance` 仍是未批准 delta，不在本 change 中修改。

## Impact

- 未来实施才会改 `.github/workflows/build-wasm-ons.yml`、`build-wasm-krkr.yml`、`build-wasm-reusable.yml` 中仅供 ONS/KRKR caller 使用的发布路径、`check-upstream-drift.yml`，以及版本/预检辅助脚本（优先复用现有 Node 与 Git，不预设新 Action）。允许改 reusable 的共用步骤，但不可变模式只由 ONS/KRKR caller 显式开启。`build-packages.yml` 保持 push 只构建不发布，且保留其既有 g++ portable probes。
- 版本身份进入 `make-package.mjs`、`make-manifest.mjs` 的发布时输入；规范版本保留 `+`。下载 URL 只对 path segment 编码（例如 `+` → `%2B`），不改写存储的版本。`ports/lib/engine-artifacts.json` 与 `fetch-engine.mjs` 的历史固定 tag 行为保持，直到另一次人工 catalog 审批。
- Artemis compat 继续硬编码 `publish: false`；bridge 继续仅手动且受现有许可限制，并保持现有 clobber 语义。本 change 不打开 Artemis 发布，也不产生实际 Release。上述工作流与脚本尚未落地，目标用 SHALL/MUST 描述，不得写成已实现。
- 不改产品、前端、WASM 源、lock、许可证、`docs/current-work.md` 或旧 OpenSpec change。
