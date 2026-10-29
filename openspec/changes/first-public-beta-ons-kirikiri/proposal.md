# Proposal

## Why

用户目标是“先把 ons 和 kirikiri 这俩版本掏出来做第一个公开测试版本”：只拿出 ONScripter 与 KiriKiri 两个引擎，作为项目第一个公开测试版（beta）。当前前端注册全部 8 个 provider，KiriKiri 页面资产只在 dev/preview 从 `.ref/` 挂载，生产 `astro build` 不携带它们（`apps/app/astro.config.mjs` 注释原文：“Bundling the artifacts into a production build is a separate packaging step”），KiriKiri 再分发许可在 `docs/governance/third-party-inventory.md` 标为“需人工确认”。因此现在既不能把站点公开，也不能说清公开的是什么。

## What Changes

- 新增 beta 构建形态：可启动的只有 ONS 与 KiriKiri 的四个 provider（`onscripter-wasm`、`onscripter`、`krkr2-wasm`、`krkr2`）。Artemis、Siglus、Tyrano、web 四个 provider 在 beta 构建中 MUST NOT 以可启动形式注册，最多保留为“仅识别、不可用”的条目；源码保留在仓库，不删除。是否随 beta 附带两个解释器 fallback 见“待用户决定 (e)”。
- 库中被识别为其他引擎的游戏 MUST 显示“本测试版不支持”，不得出现无效的“开始”按钮。
- 生产构建 MUST 携带并静态提供 ONS 资产与 KiriKiri 页面资产（来自固定 release，经 `ports/lib/fetch-engine.mjs` 做 size + sha256 校验），构建期 MUST NOT 依赖 `.ref/`；静态主机 MUST 下发 COOP `same-origin` / COEP `credentialless` 隔离头。
- 定义公开 beta 的版本身份（拟议 app `0.2.0-beta.1`，引擎沿用 `align-runtime-build-release-semver` 的不可变 tag 方案）、源码提供（source offer）、`LICENSE` / `THIRD_PARTY_NOTICES.md` 随站分发，以及“KiriKiri 许可人工确认”作为公开前的硬闸门。
- 定义 beta 验收：用户亲自执行的真实游戏矩阵（存读档、音频、输入、刷新恢复）、浏览器矩阵，以及已知限制页。
- **前置依赖**：本 change 依赖工作树中另一代理正在进行的包重命名先落地（`packages/engine/core/*`、`packages/engine/interpreter/*`，runtimeId 不变）。本 change 一律使用新包名；现有 CI 与台账中的 `runtime-ons` / `runtime-kag` 只作为历史名出现。
- 本 change 只是计划。下文所有目标用 SHALL/MUST 描述，均未实现。

## 待用户决定

以下问题会改变范围、部署或验收，apply 前须由用户回答，计划不替用户默认：

- (a) “公开”指什么：公开网站部署（哪个主机？Cloudflare Pages？）、把 GitHub 仓库/releases 设为公开，还是两者都要？
- (b) beta 版本号：是否采用 app `0.2.0-beta.1`？引擎 release 是否另发新的不可变 tag，还是沿用 `engine-ons-0.7.7beta` / `engine-krkr-20260920` 历史 pin？
- (c) 用哪些测试游戏、各多少个（ONS N 个、KiriKiri N 个，N 待定）？
- (d) 支持哪些浏览器（及最低版本）？KiriKiri 依赖 JSPI，可用浏览器范围可能比 ONS 窄。
- (e) 两个解释器 fallback（`onscripter`、`krkr2`）是否随 beta 发布，还是只发两个 WASM 引擎？
- (f) 若站点公开而仓库保持 private，GPL 源码提供义务如何满足（公开源码归档下载、单独公开仓库、书面 offer 或其他）？

## Capabilities

### New Capabilities

- `beta-engine-scope`: beta 构建中可注册与可显示的引擎集合，以及对其他已识别引擎的“本测试版不支持”呈现。
- `beta-artifact-delivery`: 生产静态构建如何获得、校验、携带并以隔离头提供 ONS 与 KiriKiri 引擎产物。
- `beta-release-identity-licensing`: 公开 beta 的版本命名、源码提供、许可文件随站分发，以及 KiriKiri 许可人工确认闸门。
- `beta-acceptance`: 用户亲自执行的真实游戏验收矩阵、浏览器矩阵与已知限制页。

### Modified Capabilities

- 无。`openspec/specs/` 目前没有已落地 capability。`establish-architecture-and-release-baseline` 的 `engine-catalog-loading`、`third-party-release-governance` 与 `align-runtime-build-release-semver` 的三个 capability 仍是未归档 delta，本 change 不修改它们，只复用后者已定的发布机制。

## Impact

- 未来 apply 才会改：`packages/core/src/platform.ts`（`BUILTIN_PROVIDERS`）、`packages/engines/src/index.ts`（`ENGINE_PACKAGES`）的 beta 选择方式；`apps/app/astro.config.mjs`、`apps/app/src/lib/platform.ts`、`apps/app/src/components/Library.svelte` / `Engines.svelte`；新增 beta 资产准备脚本（放在 `apps/app/scripts/`）与静态主机头文件；`README.md` / 已知限制页；可能新增 source offer 产物。
- 不改 WASM 源、上游 pin、`ports/lib/engine-artifacts.json` 中既有 hash、Artemis 相关文件，也不改两个既有 OpenSpec change。
- 外部系统：可能涉及 Cloudflare Pages（当前只观察到一次 preview 检查成功，生产部署状态未知）与 GitHub 仓库可见性，取决于 (a)/(f)。
