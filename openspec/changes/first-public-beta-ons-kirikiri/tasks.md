# Tasks

> 全部任务保持未勾选。规划完成不等于实施完成；apply 需要用户另行授权，且 `proposal.md`“待用户决定” (a)–(f) 必须先有答案。
> 标 **【用户】** 的任务只能由用户本人执行，代理只准备材料、不代跑、不代判。

## 1. 前置：包重命名已落地

- [ ] 1.1 确认另一代理的包重命名已提交并 push：`packages/engine/core/{onscripter-yuri,kirikiroid2-web,artemis-compat,...}` 与 `packages/engine/interpreter/{onscripter,kirikiri-kag3}` 在 HEAD tracked，`packages/runtime-*` 不再存在，runtimeId 未变。验证：`git ls-tree --name-only HEAD packages/ packages/engine/core packages/engine/interpreter` 与 `git status --porcelain packages/` 为空；`grep -rn "runtimeId" packages/engine/*/*/src/index.ts` 仍为 `onscripter-wasm`/`onscripter`/`krkr2-wasm`/`krkr2`。未落地则停止，不进入第 2 组。
- [ ] 1.2 确认重命名后的基线可构建：验证 `npm run build:packages` 与 `pnpm --filter @online-gal/app build` exit 0，并记录 `ports/lib/fetch-engine.mjs` 中 ONS `dest` 是否已改为新路径（不在本 change 修改它，若仍是 `packages/runtime-ons/assets` 则记为前置缺口交回重命名任务）。
- [ ] 1.3 【用户】回答 `proposal.md` 的 (a)–(f)，并把答案写入本 change 的 `design.md` Decisions（由代理按答案修订，用户确认）。验证：`openspec validate first-public-beta-ons-kirikiri --strict` exit 0，且 design 中不再有未决的 (a)–(f)。

## 2. 范围裁剪

- [ ] 2.1 在 `packages/core/src/platform.ts` 新增 `BETA_RUNTIME_IDS` 与按 id 选择 provider 的导出，并给 `PlatformOptions` 增加“替换内置列表”的选项；`BUILTIN_PROVIDERS` 内容不变。验证：新增单测断言默认 `new Platform()` 注册 8 个 provider，传入 beta 列表时只注册 beta 集合；`tsc -b tsconfig.packages.json` exit 0。
- [ ] 2.2 在 `packages/engines/src/index.ts` 增加与 2.1 相同 id 集合的 beta 过滤导出，`ENGINE_PACKAGES` 不变。验证：单测断言过滤结果的 runtimeId 集合等于 `BETA_RUNTIME_IDS`，且重复 id 校验仍在模块加载时生效。
- [ ] 2.3 在 `apps/app/astro.config.mjs` 增加由 `ONLINE_GAL_BETA=1` 控制的编译期常量；`apps/app/src/lib/platform.ts` 在 beta 下用 beta 列表创建 `Platform`，并把范围外 provider 包装为不可用的仅识别条目（带 beta 原因）。beta 下不调用 `configureArtemisCompat`。验证：`ONLINE_GAL_BETA=1 pnpm --filter @online-gal/app build` 与默认 build 均 exit 0；beta 输出中 `grep -r "engines/artemis-compat" apps/app/dist` 无匹配。
- [ ] 2.4 在 `apps/app/src/components/Library.svelte`、`apps/app/src/pages/play.astro`（或 `Player.svelte`）与 `apps/app/src/components/Engines.svelte` 显示“本测试版不支持”，范围外游戏无“开始”按钮，`/play?id=` 不创建实例。验证：组件/逻辑单测覆盖 beta 原因分支；`pnpm --filter @online-gal/app check` 0 error；默认构建下文案不变。
- [ ] 2.5 更新 `README.md` 的支持范围段，说明 beta 构建只含 ONS 与 KiriKiri、其他引擎仍在仓库但不随 beta 发布。验证：`npm run check:sources` 与 Markdown 链接检查 exit 0。

## 3. 产物与构建

- [ ] 3.1 新增 `apps/app/scripts/beta-engine-assets.mjs`：先断言输出绝对路径位于 `apps/app/public/` 下（否则非 0 退出），再用 `node ports/lib/fetch-engine.mjs krkr --out <abs>/apps/app/public/krkr2-web --json` 取 6 个 KiriKiri 文件，并把 ONS 3 个 tracked 资产复制到 `apps/app/public/engines/ons/` 后按 `ports/lib/engine-artifacts.json` 校验 size + sha256；写出一份产物清单。`.gitignore` 增加这两个目录。验证：在无 `.ref/` 的干净副本（`.test/` 下）运行脚本 exit 0 且 9 个文件 hash 与台账一致；把一个文件改坏后重跑 exit 3 且不留下该文件；指向仓库根的 `--out` 被拒绝。
- [ ] 3.2 把 3.1 接入 beta 构建：`ONLINE_GAL_BETA=1` 时先运行该脚本再 `astro build`；beta 下按产物清单存在与否设定 KiriKiri 编译期布尔，存在时 `configureKirikiriWasm({ pageUrl: '/krkr2-web/index.html' })`，ONS 显式 `engineBase: '/engines/ons/'`。验证：beta `apps/app/dist/krkr2-web/` 含 6 个文件、`apps/app/dist/engines/ons/` 含 3 个文件且 hash 一致；默认构建输出不含这两个目录，dev 中间件行为不变。
- [ ] 3.3 新增 `apps/app/public/_headers`（若 (a) 选 Cloudflare Pages；否则该主机的等价配置文件），对 `/*` 与 `/krkr2-web/*` 设置 COOP `same-origin`、COEP `credentialless`、`X-Content-Type-Options: nosniff`。验证：beta 输出含该文件且内容与 `ISOLATION_HEADERS` 三项一致（脚本比对）。
- [ ] 3.4 新增 `apps/app/scripts/check-beta-deploy.mjs <baseUrl>`：请求首页、`/krkr2-web/index.html`、`/krkr2-web/index.wasm`、`/engines/ons/onsyuri.js`、`/engines/ons/onsyuri.wasm`，断言 200、三项隔离头与 `.wasm` 为 `application/wasm`。验证：对 `astro preview` 本地运行 exit 0（运行完立即停止 preview 进程）；去掉一个头的模拟响应使其 exit 非 0。
- [ ] 3.5 核对目标主机单文件大小上限是否容纳 `index.wasm`（22,666,256 B）与 `assets.zip`（7,853,397 B）。验证：引用主机官方文档链接写入 `design.md` Risks；超限则停止并交回用户改 (a)。
- [ ] 3.6 在 CI 增加 beta 构建作业（`.github/workflows/build-packages.yml` 新 job 或新 workflow，仅构建与上传工件，不发 release），使用 `GITHUB_TOKEN` 取私有 release。验证：同一 commit 的 Actions 运行中该作业 exit 0，工件含 9 个引擎文件与 `_headers`；不触发 ONS/KRKR 引擎重建或发布。

## 4. 许可与源码

- [ ] 4.1 在 beta 构建中把 `LICENSE`、`THIRD_PARTY_NOTICES.md`、OnscripterYuri LICENSE、kirikiroid2-web LICENSE 与 port notices（gettext GPL-3.0-only、libexif LGPL-2.1-or-later）放到 `apps/app/dist/licenses/`，新增许可页 `apps/app/src/pages/licenses.astro` 并从首页链接。验证：beta 输出中每份许可存在且与源文件 sha256 相同；`astro check` 0 error。
- [ ] 4.2 新增源码提供说明页 `apps/app/src/pages/source.astro`，为 `onsyuri.wasm`/`onsyuri.js`、`index.wasm`/`index.js`/`vlfs.js` 列出上游仓库与 commit、本仓库补丁/overlay 路径（`ports/ons/build/`、`ports/krkr/kirikiroid2/`）、构建命令与 sha256，并写明按 (f) 决定的获取渠道。验证：页面数据由构建从 `ports/lib/engine-artifacts.json` 与 `.github/upstream-pins.json` 生成，单测断言与台账一致。
- [ ] 4.3 按 (f) 落实源码获取渠道（例如生成可下载的源码归档并随 release 发布，或公开源码仓库）。验证：在未登录 GitHub 的环境中按 4.2 页面说明能取得源码归档，归档 sha256 与页面记录一致。
- [ ] 4.4 【用户】确认 KiriKiri（kirikiroid2-web 日文 BSD 风格 Kirikiri/W.Dee LICENSE 及依赖 notices）可再分发，并由代理把结论、日期、确认人写入 `docs/governance/third-party-inventory.md` 的 KiriKiri 行。验证：该行不再是“需人工确认”。
- [ ] 4.5 新增发布清单脚本 `apps/app/scripts/beta-release-checklist.mjs`：KiriKiri 行仍为“需人工确认”、源码页缺失、许可文件缺失或验收记录未完成时退出非 0。验证：在 4.4 前运行 exit 非 0 并指出许可项；补齐后 exit 0。

## 5. 部署

- [ ] 5.1 在关于/设置区域显示 app 版本（按 (b)，拟议 `0.2.0-beta.1`）与两个引擎 `releaseTag` + 上游 commit，版本值写入 `apps/app/package.json`，引擎信息由构建从台账注入。验证：beta 输出页面中能检索到三者，且与 `package.json`、`ports/lib/engine-artifacts.json` 一致。
- [ ] 5.2 若 (b) 要求新引擎 tag：按 `align-runtime-build-release-semver` 手动发布（显式 `publish=true` + 完整 SemVer），再单独审批更新 `ports/lib/engine-artifacts.json`。验证：新 tag 存在、旧 tag 字节不变、台账新条目与 release `manifest.json` 一致。不需要新 tag 时把本任务记为不适用并说明。
- [ ] 5.3 把 CI 产出的 beta 静态目录部署到 (a) 选定主机的**预览**地址（不是公开地址）。验证：`node apps/app/scripts/check-beta-deploy.mjs <previewUrl>` exit 0，并记录部署 ID 与 commit SHA。

## 6. 用户验收

- [ ] 6.1 新增验收记录模板 `docs/beta/acceptance-<version>.md`：每行一个标题 × 浏览器，列启动、文本推进与选项、音频、存档/读档、刷新后读档五项，以及热身/第 1 次/第 2 次结果、浏览器版本、站点版本、引擎 tag；另列五个独立结论（目录完整、构建成功、合成测试、真实游戏可玩、许可已确认）。验证：模板存在、链接检查通过。
- [ ] 6.2 新增已知限制页 `apps/app/src/pages/limitations.astro`（内容源自 `README.md`）：只支持 ONS 与 KiriKiri；解释器拒绝编译型 TJS2 `.ks`；不支持 TVP 原生插件；`[movie]` 等被跳过；XP3 只处理 XOR 混淆；浏览器要求；验收中失败/未定项。验证：首页可链接到该页；`astro check` 0 error。
- [ ] 6.3 在 `packages/core/src/capabilities.ts` 的能力检测与播放页中，对缺 `crossOriginIsolated`、WebAssembly 或 KiriKiri 所需 JSPI 的浏览器给出“当前浏览器不受支持”提示。验证：单测用模拟 scope 覆盖三种缺失；默认构建行为不回退。
- [ ] 6.4 【用户】在预览地址上对 (c) 选定的 ONS 与 KiriKiri 标题、(d) 选定的浏览器执行验收：每项丢弃一次热身、再跑两次一致；同一时刻只开一个引擎实例；结束后关闭浏览器标签与本地服务。验证：用户填写 6.1 记录，所有行五项均为通过，或失败/未定项已写进 6.2 限制页并由用户接受。

## 7. 发布

- [ ] 7.1 运行发布清单：`node apps/app/scripts/beta-release-checklist.mjs` exit 0（许可确认、源码页、许可文件、验收记录全部通过），并对预览地址重跑 `check-beta-deploy.mjs` exit 0。验证：两条命令的输出与 commit SHA 留存在 `docs/beta/acceptance-<version>.md`。
- [ ] 7.2 【用户】批准公开：按 (a)/(f) 把预览部署提升到公开地址，或修改仓库/releases 可见性。验证：用户在验收记录中写下批准与日期；代理随后对公开 URL 运行 `check-beta-deploy.mjs` exit 0。
- [ ] 7.3 用显式 pathspec 提交 beta 相关改动并 push，在 GitHub 上观察同一 commit 的构建结果。验证：同一 SHA 的 Actions 运行 exit 0；`docs/current-work.md` 当前摘要更新为 beta 已公开及其版本、地址与验收记录路径。

## Workflow follow-up

- 本 change 任务完成后按项目评审要求归档；归档前确认四个 capability 的 delta 已同步到 `openspec/specs/`。
- 旧 change `establish-architecture-and-release-baseline` 的 34 项任务与 `align-runtime-build-release-semver` 不在本 change 中修改。
- Artemis、Siglus、Tyrano、web 是否进入后续 beta 另开 change。
