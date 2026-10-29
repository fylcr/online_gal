# Design

## Context

见 `proposal.md` 的 Why。本设计只规划未来 apply，不改代码。以下事实于本轮在工作树（HEAD `b81ec0c`，包重命名进行中、未提交）上读取：

- 注册有两份手写列表：`packages/core/src/platform.ts` 的 `BUILTIN_PROVIDERS`（8 个：`kirikiriWasmProvider`、`kirikiriProvider`、`onscripterWasmProvider`、`onscripterProvider`、`siglusProvider`、`artemisProvider`、`tyranoProvider`、`webProvider`），由 `Platform` 构造时 `registerAll`；`packages/engines/src/index.ts` 的 `ENGINE_PACKAGES`。前端 `apps/app` 只依赖 `@online-gal/core`，不依赖 `@online-gal/engines`。
- 新包名（重命名进行中）：`@online-gal/engine-core-onscripter-yuri`（`onscripter-wasm`）、`@online-gal/engine-interpreter-onscripter`（`onscripter`）、`@online-gal/engine-core-kirikiroid2-web`（`krkr2-wasm`）、`@online-gal/engine-interpreter-kirikiri-kag3`（`krkr2`）。HEAD 的 tracked 树仍是 `packages/runtime-*`。`ports/lib/fetch-engine.mjs` 的 ONS `dest` 仍写 `packages/runtime-ons/assets`。
- ONS：`packages/engine/core/onscripter-yuri/assets/{onsyuri.wasm,onsyuri.js,onsyuri.html}` 已 tracked，本轮实测 size 4660610 / 198947 / 24399，`onsyuri.wasm` sha256 `85419fe5…421a`，与 `ports/lib/engine-artifacts.json` 的 `engine-ons-0.7.7beta` 条目一致。运行时默认 `engineBase = new URL('../assets/', import.meta.url)`；生产 `astro build` 后这些文件是否被 Vite 正确输出并可访问，**本轮未验证**。
- KiriKiri：`apps/app/astro.config.mjs` 只在 dev/preview 用中间件把 `.ref/kirikiroid2-web/out/web/release` 挂到 `/krkr2-web/`；`apps/app/src/lib/platform.ts` 只在 `import.meta.env.DEV` 时 `configureKirikiriWasm({ pageUrl: '/krkr2-web/index.html' })`，所以生产构建里 `krkr2-wasm` 处于撤回状态，KiriKiri 游戏由解释器 `krkr2` 接手。台账 `engine-krkr-20260920` 记 6 个文件（`index.wasm` 22666256 B 等）。
- 仓库 private：台账注释写明未认证请求 release asset 得到 404，`fetch-engine.mjs` 在有 `GITHUB_TOKEN`/`GH_TOKEN` 时走 release API。
- 隔离头 `ISOLATION_HEADERS`（COOP same-origin、COEP credentialless、nosniff）只在 Astro dev/preview server 生效；`apps/app/public/` 目前只有 `favicon.svg`，没有静态主机头文件。
- Artemis 已有“撤回”机制：`__ARTEMIS_ASSETS_AVAILABLE__` 编译期布尔。库卡片在 `availability === 'unavailable'` 时显示“引擎不可用”，否则显示“开始”。
- Cloudflare Pages：只观察到一次 preview 检查成功（`https://9c415fb0.onlinegal.pages.dev`），未打开该 URL；仓库内没有 `_headers` 或 wrangler 配置；生产部署状态未知。

## Goals / Non-Goals

**Goals:**

- 一次构建开关决定 beta 范围，开发构建不受影响。
- 生产输出自包含两种引擎的字节，并在静态主机上满足跨源隔离。
- 公开前的检查项（许可、源码、验收、头）都可逐项判定。

**Non-Goals:**

- 不删除、不重命名任何引擎包；不改 runtimeId；不改 WASM 源与上游 pin。
- 不重新设计引擎发布机制；沿用 `align-runtime-build-release-semver`。
- 不处理 Artemis（用户已延期）、鼠标映射冻结缺陷或旧 34 项任务。
- 不合并两份注册列表（属于旧计划 task 4.1），本 change 只在两处应用同一个 beta 选择。
- 代理不执行真实游戏验收。

## Decisions

1. **beta 范围用构建期常量 + 显式 beta provider 列表，不删代码。**
   - 新增一个 Vite `define` 常量（例如 `__ONLINE_GAL_BETA__`，由环境变量 `ONLINE_GAL_BETA=1` 在 `astro.config.mjs` 中设定），与现有 `__ARTEMIS_ASSETS_AVAILABLE__` 同一模式。
   - `packages/core` 导出 `BETA_RUNTIME_IDS`（四个 runtimeId，是否含解释器按用户决定 (e)）与一个按 id 过滤的选择函数；`Platform` 接受 `providers` 选项来自 app 层。app 在 beta 下传入过滤后的列表，`BUILTIN_PROVIDERS` 原样保留给开发构建。`ENGINE_PACKAGES` 增加同一过滤导出，供 CI smoke 断言。
   - 备选 A：在 beta 分支删除其他包。破坏仓库、与用户“不删引擎”冲突，否决。备选 B：运行时设置开关。用户可在公开站点打开不受验收的引擎，否决。
   - 风险：`Platform` 当前 `providers` 选项是“注册在内置之前的额外 provider”，不是替换。apply 需新增“替换内置列表”的选项，而不是复用该语义。

2. **范围外游戏呈现为“本测试版不支持”。**
   - 识别仍运行（保留诊断价值），但 beta 中范围外引擎不注册，识别结果来自一个只做 probe 的“范围外识别”表还是直接显示“引擎未识别”需要取舍。选择：beta 注册集合之外，额外保留范围外 provider 的 **probe**（不提供 `create`），结果标记为新的呈现状态 `beta-unsupported`，UI 显示“本测试版不支持”。这样 Artemis/Siglus 游戏不会被误报成“引擎未识别”。
   - 实现方式：在 app 层把范围外 provider 包装成 availability 为 `unavailable` 的只识别 provider，并带 beta 原因字段；库与 `/play` 依据该字段显示文案。不需要改 `RuntimeAvailability` 联合类型。
   - 备选：完全不注册、显示“引擎未识别”。实现最简单，但与用户要求的文案不符，否决。

3. **产物准备脚本在构建前运行，输出到 Astro `public/` 的 gitignored 子目录。**
   - 新增 `apps/app/scripts/beta-engine-assets.mjs`：调用 `node ports/lib/fetch-engine.mjs krkr --out <abs>/apps/app/public/krkr2-web --json`，对 ONS 从 tracked `assets/` 复制到 `apps/app/public/engines/ons/` 并复用 fetch-engine 的同一台账校验（以 `--out` 落到该目录、或在复制后按台账算 sha256）。脚本先断言目标绝对路径位于 `apps/app/public/` 下，失败即非 0 退出（遵守 openspec/config.yaml 的落点断言）。`.gitignore` 加两个子目录。
   - ONS 在 beta 中显式设置 `engineBase = '/engines/ons/'`，不依赖 `import.meta.url` 在打包后的行为（该行为本轮未验证）。
   - KiriKiri 在 beta 构建中调用 `configureKirikiriWasm({ pageUrl: '/krkr2-web/index.html' })`，条件是脚本写出的产物清单存在且校验通过（编译期布尔，模式同 Artemis）。
   - 私有仓库下 CI 取 release 需要 token；公开部署主机（如 Cloudflare Pages 构建）若无 token 会 exit 2。选择：beta 构建在 GitHub Actions 中完成（有 `GITHUB_TOKEN`），产出静态目录后再上传主机；或在仓库/ releases 公开后直接取。最终取决于 (a)/(f)。
   - 备选：把 30 MB KiriKiri 产物提交进 `public/`。违反“下载产物不提交”的台账约定，否决。

4. **隔离头随仓库提交。**
   - 若主机为 Cloudflare Pages：提交 `apps/app/public/_headers`，对 `/*` 与 `/krkr2-web/*` 写 COOP/COEP/nosniff；`.wasm` 的 MIME 由主机默认提供，部署后用 `curl -I` 核对。
   - 若主机为其他：在 design 的 Migration Plan 中替换为该主机的等价配置。主机选择属 (a)。
   - 加一个部署后预检脚本（`apps/app/scripts/check-beta-deploy.mjs`），对给定 URL 请求首页、`/krkr2-web/index.html`、`/krkr2-web/index.wasm`、`/engines/ons/onsyuri.wasm`，断言状态码、头与 `Content-Type`。

5. **版本身份。**
   - app 版本在 beta 分支/提交中设为用户决定的值（拟议 `0.2.0-beta.1`），构建时注入站点“关于”区域，同时注入两个引擎的 `releaseTag` 与上游 commit（读自 `ports/lib/engine-artifacts.json`）。
   - 引擎字节默认沿用现有 pin `engine-ons-0.7.7beta` / `engine-krkr-20260920`（已有 hash、已被校验），避免为 beta 新发 release。若用户要新 tag，按 `align-runtime-build-release-semver` 手动发布并单独审批 catalog 更新。

6. **许可与源码提供。**
   - 构建把 `LICENSE`、`THIRD_PARTY_NOTICES.md`、OnscripterYuri LICENSE、kirikiroid2-web LICENSE 与 port notices 复制到 `public/licenses/`（gitignored 生成物或 tracked 副本，二选一在 apply 时按“上游 LICENSE 是否已在仓库中”决定），并新增站点许可页。
   - 源码提供：为每个分发二进制生成源码归档说明（上游 URL + commit、本仓库补丁/overlay 路径、构建命令、sha256）。仓库 private 时，必须由 (f) 决定公开渠道；在 (f) 决定之前，许可闸门视为未通过。
   - KiriKiri 许可人工确认是硬闸门：发布清单脚本读取 `docs/governance/third-party-inventory.md` 中 KiriKiri 行，仍为“需人工确认”即退出非 0。

7. **验收归用户。**
   - 代理只准备验收表模板与记录位置（`docs/beta/acceptance-0.2.0-beta.1.md` 一类路径，名字随版本），不运行游戏。用户按“丢弃一次热身 + 两次一致”执行，同一时刻只开一个引擎实例，结束后关闭浏览器与本地服务。

## Risks / Trade-offs

- [私有仓库导致构建主机取不到 release] → beta 构建放在有 token 的 CI；或先决定 (f)。
- [COEP credentialless 在部分浏览器不支持] → 列入浏览器矩阵 (d)；不支持时提示而不是卡住。
- [KiriKiri 依赖 JSPI，浏览器覆盖窄] → 浏览器矩阵与能力检测；若 (e) 选择附带解释器，JSPI 缺失时可回落解释器，但解释器限制须写进限制页。
- [ONS `import.meta.url` 默认路径在生产包中的行为未验证] → beta 显式设置 `engineBase`，并在部署预检中请求该文件。
- [重命名未落地即开工会与另一代理的写入冲突] → 第一阶段只做前置核对，未落地不进入后续任务。
- [Cloudflare Pages 单文件上限] → `index.wasm` 22.7 MB；需核对主机单文件限制（本轮未查证），超限需改主机或分片方案，属 (a)。
- [范围外 provider 的只识别包装仍会引入其包代码到 bundle] → 可接受（不含引擎字节）；若需完全排除，改为轻量识别表，作为后续优化。

## Migration Plan

1. 重命名落地并 push 后，在新分支实施本 change。
2. beta 构建只在 `ONLINE_GAL_BETA=1` 时生效；默认构建与开发服务器不变，因此可随时回到原行为。
3. 部署到预览环境 → 预检脚本通过 → 用户验收 → 许可闸门通过 → 再切到公开地址。
4. 回滚：撤下公开部署或回退到上一预览；不涉及数据迁移（IndexedDB 结构不变）。

## Open Questions

见 `proposal.md` 的“待用户决定” (a)–(f)。它们会改变部署主机、版本、验收规模与源码提供方式，apply 前必须由用户回答；下列可延后：

- 关于页的具体布局与文案。
- 许可页是否同时提供下载打包版。
