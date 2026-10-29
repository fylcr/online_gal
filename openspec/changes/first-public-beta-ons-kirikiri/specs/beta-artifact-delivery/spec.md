# Spec Delta

## Purpose

定义公开 beta 的生产静态构建如何获得、校验、携带并提供 ONS 与 KiriKiri 引擎产物，使部署后的站点不依赖本机 `.ref/` 检出，且引擎所需的跨源隔离在静态主机上成立。

## ADDED Requirements

### Requirement: Engine bytes come from the pinned release and are verified
beta 构建 MUST 通过 `ports/lib/fetch-engine.mjs`（或等价地调用其校验逻辑）从 `ports/lib/engine-artifacts.json` 记录的固定 release 获取 KiriKiri 的 6 个页面资产，并对 ONS 的 3 个资产做同一 size + sha256 校验。任一文件缺失或校验失败时，beta 构建 MUST 以非 0 退出，MUST NOT 产出缺该引擎的站点。

#### Scenario: Verified fetch succeeds
- **WHEN** 固定 release 的 `index.wasm`、`index.js`、`vlfs.js`、`index.html`、`shell.html`、`assets.zip` 与台账 size/sha256 一致
- **THEN** beta 构建把它们放进站点输出并继续

#### Scenario: Hash mismatch
- **WHEN** 任一 KiriKiri 或 ONS 文件的 sha256 与台账不符
- **THEN** beta 构建失败退出，且站点输出中不留下该文件

### Requirement: Production build does not read .ref
beta 构建 MUST NOT 从 `.ref/` 读取任何引擎文件；在不存在 `.ref/` 目录的干净检出上，beta 构建 MUST 能成功（只要网络或缓存可取得固定 release）。

#### Scenario: Clean checkout
- **WHEN** 在没有 `.ref/` 的干净检出上运行 beta 构建
- **THEN** 构建成功，输出包含 KiriKiri 页面资产

### Requirement: Static output serves both engines at stable paths
beta 站点输出 MUST 在 `/krkr2-web/` 下提供 KiriKiri 页面资产，并在一个固定路径下提供 ONS 的 `onsyuri.js` / `onsyuri.wasm`。前端发布给 provider 的 URL MUST 指向这些实际存在的路径；不得发布未随站点输出的 URL。

#### Scenario: KiriKiri page URL published only when served
- **WHEN** beta 站点构建完成
- **THEN** KiriKiri provider 的 `pageUrl` 指向 `/krkr2-web/index.html`，且该文件在输出中存在

#### Scenario: ONS engine base resolves
- **WHEN** 浏览器从 beta 站点加载 ONS 引擎
- **THEN** `onsyuri.js` 与 `onsyuri.wasm` 的请求返回 200 与正确的 `Content-Type`

### Requirement: Static host sends isolation headers
beta 部署 MUST 对站点页面与 `/krkr2-web/` 下的文件下发 `Cross-Origin-Opener-Policy: same-origin`、`Cross-Origin-Embedder-Policy: credentialless` 与 `X-Content-Type-Options: nosniff`，`.wasm` MUST 以 `application/wasm` 提供。头配置 MUST 随仓库提交，不依赖控制台手工设置。

#### Scenario: Deployed page is cross-origin isolated
- **WHEN** 在已部署的 beta 站点打开首页与 `/krkr2-web/index.html`
- **THEN** 两者的 `crossOriginIsolated` 都为 `true`

#### Scenario: Missing headers
- **WHEN** 部署预检发现任一上述头缺失
- **THEN** 该部署不得被标为公开 beta
