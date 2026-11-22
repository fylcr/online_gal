# Tasks: site-setup

计划书。执行的对话按顺序做，设计细节以 design.md 为准（S1–S10），用户决定 A–G 见 proposal.md。

- **负责**：管理系统的对话（`cf-account-admin` 的对话）。
- **配合**：`plugin-system` tasks.md 11.3（撤销内核对 `/__setup` 的放行）与 10.10（发布工作流用 `npm run build:pages`），由插件系统的对话完成。
- **未决事项**：无。proposal 的 (f)、(g) 已由用户决定 G（2026-11-23）结案。
- **当前要做**：无。第 6 节（决定 G）已于 2026-11-23 完成并部署（`d6c39cf`，部署 `42d9a7f6`，日志 `setup (规则 3)`），等用户做 4.3 第 4 步：在 `https://onlinegal.cc.cd/` 上点“登录 Cloudflare”。
- **完成标准**：初始化构建只有初始化页；写入后自动（或按提示手动）再构建一次，得到与原设计一致、不含任何初始化代码的站点构建。

## 历史：上一版已完成的工作（v2，按用户决定 F 部分作废）

- [x] H.1 `functions/__setup/[[path]].ts` 的 `status`，以及 `verify` 的实现。→ 代码移到 `apps/setup/server/setup.ts` 继续使用（2.1）。
- [x] H.2 在 `format.ts`、`build-site.mjs` 登记 `__setup`。→ 作废，在 3.2 删除。
- [x] H.3 `.test/site-setup.test.ts` 的 `status` 用例与门槛用例。→ `status` 用例保留并改路径；门槛用例删除。
- [x] H.4 上一版用 `wrangler pages dev` 实测 `status`，历史结果见 design.md S4。
- [x] H.5 门槛 `site-gate.js` 的测试。→ 作废，门槛已取消。

## 0. 可行性验证（需要用户的 Cloudflare 账号，用测试项目，不要用生产项目）

- [x] 0.1 建一个 **Git 集成**的测试 Pages 项目，构建命令 `npm run build:pages`，输出 `dist/site`。确认：
  1. 没有 `ONLINE_GAL_SITE` 时构建日志选了 `setup`；用 API 写入后，create deployment（`branch=<生产分支>`）触发的新构建选了 `site`。
  2. 输出目录有 `_worker.js` 时，`functions/` 被忽略，`_routes.json` 生效，未知路径返回根 `index.html`。
  3. API PATCH 写 `env_vars` 是合并：之前的无关变量还在；`GET` 项目时 secret 只有键名；预览环境没有 secret。
  - 结果见 design.md 的 Verification record。隔离 Git 项目在同一提交上运行 `npm run build:pages`，先后部署 `setup` 和 `site`。

## 1. 构建脚本

- [x] 1.1 新建 `scripts/build-pages.mjs`（S2）：判断阶段、清空 `dist/site/`、调用对应阶段、写 `GITHUB_OUTPUT`、日志不含变量值。
- [x] 1.2 站点阶段：运行 `npm run build:site`，再做 S6 的检查。
- [x] 1.3 初始化阶段：调用 `apps/setup/scripts/build-setup.mjs`（S3）：`astro build`（带 `PUBLIC_ONLINE_GAL_SETUP` 配置）→ 复制到 `dist/site/` → esbuild 打包 `_worker.js` → 写 `sw.js`、`_routes.json`、`_headers` → S6 的初始化阶段检查。
- [x] 1.4 根 `package.json` 加 `build:pages`、`build:setup`；`check` 加上 `@online-gal/setup` 的 `astro check`。
- [x] 1.5 测试（`.test/site-setup.test.ts`）：阶段判断的五条规则（API 用假 `fetch`）、`rebuild` 配置的三种取值、S6 两个阶段的检查各有一个失败用例。

## 2. `apps/setup`

- [x] 2.1 建 workspace `apps/setup`（S5 的文件和依赖）。把 `functions/__setup/[[path]].ts` 移成 `apps/setup/server/setup.ts`，`status` 加 `phase: "setup"`；写 `apps/setup/server/worker.ts`（S3）。
- [x] 2.2 `apps/setup/src/lib/memory-vault.ts` 和 `setup.ts`：用 `CloudflareAccount` 实现 S5 的第 1–8 步（登录、所有者、读项目、校验、写入、三种重新构建、确认切换、中断恢复、本机预演）。可以从 `apps/app/src/lib/site-setup.ts` 和 `SetupFlow.svelte` 搬运逻辑。
- [x] 2.3 `apps/setup/src/lib/setup-items.ts`（S7 第一批）、`src/pages/index.astro`、`src/components/SetupFlow.svelte`。
- [x] 2.4 确认页面产物不含内核代码；如果从包入口导入会带进来，在 `packages/plugin-system/package.json` 加 `./account` 子路径导出（S5）。
- [x] 2.5 测试：`status`/`verify` 用例改到新路径；初始化页逻辑用假 `fetch` 覆盖：非所有者、已写入直接构建、写入请求体、`pages-git` 触发与轮询、`github-rerun` 成功与 403 退回手动、切换检测、本机预演不发写请求、token 不进任何存储。

## 3. 清理上一版

- [x] 3.1 删除 `functions/__setup/`、`apps/app/src/pages/setup.astro`、`apps/app/src/components/setup/`、`apps/app/src/lib/site-gate.js`、`site-setup.ts`、`site-setup-items.ts`，去掉 `Layout.astro` 里的 `<SiteGate />` 和 import。
- [x] 3.2 删除 `packages/plugin-system/src/format.ts` 保留路径正则和 `scripts/build-site.mjs` 的 `RESERVED`、`_routes.json` 里的 `__setup`。
- [x] 3.3 确认 `plugin-system` 11.3 已完成（`kernel.ts` 里没有 `__setup`）。

## 4. 验收

- [x] 4.1 `npm run build:setup`。按用户要求使用 `cf` CLI：在本地以生成的 Worker 和静态目录做浏览器冒烟，用 `cf` 在隔离 Pages 项目确认远端路由。验收：任意页面路径显示初始化页；`/__setup/status` 返回 `phase: "setup"`；`/__cf/**` 可用；没有插件注册表。
- [ ] 4.2 `npm run build:pages -- --phase=site`：检查通过，`dist/site/` 搜不到 `__setup`；`npm run check`、`npx tsc -b tsconfig.packages.json`、`npx openspec validate site-setup --strict` 通过，现有 plugin 与账号测试没有回归。
  - 独立发布工作树用冻结锁文件安装依赖，`npm run check`、两阶段构建、15 项初始化与账号测试、OpenSpec 严格校验通过。旧插件工作台测试在本工作区未找到，保留该项待核对。
- [ ] 4.3【用户】上线并亲自初始化（用户决定 E）。依赖 `plugin-system` 10.10 提供符合 `cf` CLI 要求的正式发布方式，并发布带来源证明的站点产物。
  1. [x] 把 `onlinegal.cc.cd` 绑定为 Pages 项目的自定义域名。2026-11-23 已绑定，域名状态 active，Cloudflare 自动建了 CNAME `onlinegal.cc.cd → online-gal.pages.dev`（代理）。
  2. ~~准备 Cloudflare API token 和 GitHub token~~ 作废（决定 G）：不需要准备任何 token。
  3. [x] 发布一次，应得到初始化构建。生产部署 786e2585 已是初始化构建。注意：此时是 Pages 的 Git 集成直接构建，没有经过 plugin-system 10.10 的正式发布工作流和来源证明；10.10 完成后正式站点构建改走该流程。
  4. 等第 6 节部署后，打开 `https://onlinegal.cc.cd/` 的任意页面，点"登录 Cloudflare"，在授权页同意全部权限，确认写入，等待第二次构建完成后回到原页面。
  5. 控制台核对：生产环境只有 `CLOUDFLARE_ACCOUNT_ID` 一个 secret 和 `ONLINE_GAL_SITE`，预览环境只有 `ONLINE_GAL_SITE`；没有 `CLOUDFLARE_API_TOKEN`、`GITHUB_TOKEN`。
  6. 无痕窗口打开任意页面正常；`curl https://onlinegal.cc.cd/__setup/status` 得不到 `phase: "setup"` 的 JSON。

## 5. 配置 Pages 构建设置

- [x] 5.1 实现 `npm run configure:pages -- <project>`（S10）：仅使用固定版 `cf`，确认 Git 集成，写入并核对构建命令、根目录、输出目录；支持预览、幂等和显式触发生产部署。
- [x] 5.2 `test/configure-pages.test.mjs` 验证写入范围、无需更新、预览、拒绝非 Git 项目、读回失败时不部署、使用原生产分支；真实 `cf pages edit --dry-run` 核对请求体。
- [x] 5.3 在新 Pages 项目确认配置及部署。项目 `online-gal`（GitHub `fylcr/online_gal`，生产分支 `main`）。原构建命令被手填成 `npm run  build:page`，部署 634044a0 因 `Missing script: "build:page"` 失败。2026-11-23 运行 `npm run configure:pages -- online-gal --deploy` 改为 `npm run build:pages` / 根目录 / `dist/site`，部署 786e2585 成功，日志 `[build:pages] setup (规则 3)`。在 786e2585.online-gal.pages.dev 上核对过：任意路径返回初始化页，`/__setup/status` 返回 `{"phase":"setup","initialized":false}`，`/__cf/**` 能转发到 Cloudflare API。同日在 `https://onlinegal.cc.cd` 上复核，页面与部署地址字节一致，证书已签发（Google Trust Services WE1，至 2027-02-20），http 301 到 https。注意：`/__cf/api/user` 的裸请求返回 `403 Forbidden` 是 `functions/__cf/[[path]].ts` 同源守卫的设计行为，带 `Sec-Fetch-Site: same-origin` 才会转发到 Cloudflare API。

## 6. 决定 G：初始化只用 Cloudflare OAuth

负责：管理系统的对话。依据 design.md S5 第 2、4、6、7 步与 S7，spec 的 "Only the site owner initializes"、"Setup is completed with OAuth only"、"Writing triggers a new build" 三条。不改 `packages/plugin-system/src/account/cloudflare.ts` 的登录代码，它已经申请 `CF_DEFAULT_SCOPES`（477 项）加 `offline_access`，并把 token 响应的 `scope` 存进凭据的 `scopes`（`cloudflare.ts:219-224`、`:329-335`）。

- [x] 6.1 `apps/setup/src/lib/setup-items.ts`：第一批只留 `CLOUDFLARE_ACCOUNT_ID`（`source: 'account'`），删 `CLOUDFLARE_API_TOKEN` 与 `GITHUB_TOKEN`。`SetupItem` 类型里的 `source: 'input'` 和 `verify` 保留不删。
- [x] 6.2 `apps/setup/src/lib/setup.ts`：
  1. 登录后检查凭据的 `scopes`：含 `pages:read` 和 `pages:write` 才继续；缺哪项就停下说明并提供"退出并更换账号"。`scopes` 为空数组（响应没带 `scope`）时不拦截，靠后续请求的报错。
  2. 删除填写和校验步骤，不再调用 `POST /__setup/verify`；清单里出现 `source: 'input'` 的项时停下提示"该项需手动填写，初始化页暂不支持"，不写入。
  3. PATCH 请求体只含 `ONLINE_GAL_SITE`（生产 + 预览，`plain_text`）和 `CLOUDFLARE_ACCOUNT_ID`（仅生产，`secret_text`）。任何 OAuth token 都 MUST NOT 进请求体。
  4. `github-rerun` 不调 GitHub API，只显示 `https://github.com/{repo}/actions/runs/{runId}` 的手动重跑入口和"推一个新的 `v*` 标签"，然后照常轮询部署。`pages-git` 与 `manual` 不变。
  5. 删除写入后"清空内存 token"和"GitHub token 保留到第 7 步"的逻辑；OAuth 凭据留在 `CloudflareAccount` 内存里供第 7 步使用。
- [x] 6.3 `apps/setup/src/components/SetupFlow.svelte`：去掉 token 输入框、"创建 token"链接和"校验"按钮；第 4 步改为只读清单（账号 id 打码）加"写入并构建"按钮；补上缺少权限时的提示和 `github-rerun` 的手动入口。
- [x] 6.4 `apps/setup/server/setup.ts` 的 `verify` 不删，在代码注释里写明"当前无使用者（site-setup 决定 G）"。
- [x] 6.5 测试（`.test/site-setup.test.ts` 及初始化页逻辑测试）：删除校验相关与 `github-rerun` 成功/403 的用例；新增：PATCH 请求体只有两个键且不含任何 token、缺 `pages:write` 时不发 PATCH、`scopes` 为空时不拦截、`github-rerun` 不发出任何到 `api.github.com` 的请求、清单含 `input` 项时停下。保留 token 不进任何浏览器存储的用例。
- [x] 6.6 验收：`npm run check`、`npm run build:setup`、上一条的测试、`npx openspec validate site-setup --strict` 通过；`dist/site/` 的初始化页里搜不到 `type="password"` 和 `api.github.com`。
  - 2026-11-23 实测：`npm run check` 0 错误（`apps/app` 20 文件、`apps/setup` 7 文件）；`npm run build:setup` 日志 `[build:pages] setup (规则 1)`；18 项初始化与账号测试全过；`openspec validate site-setup --strict` 通过。`dist/site/index.html` 与 `_astro/` 里搜不到 `type="password"`、`api.github.com`、`__setup/verify`、`CLOUDFLARE_API_TOKEN`、`GITHUB_TOKEN`；`api.github.com/user` 和 `__setup/verify` 只出现在 `_worker.js`（6.4 保留的服务器校验）。另跑了 `.test/site-setup-browser.mjs` 的 Edge 冒烟：深链接 `/settings/account/` 渲染出“登录 Cloudflare”，无内核，`/__setup/status` 由生成的 Worker 应答。
- [x] 6.7 提交并推送到 `main`，等 Cloudflare 的 Git 构建在 `online-gal` 项目上部署新的初始化构建（仍应是 `setup (规则 3)`），在 `https://onlinegal.cc.cd/` 确认页面只有“登录 Cloudflare”一个按钮。之后通知用户做 4.3 第 4 步。
  - 2026-11-23 实测：提交 `d6c39cf` 推到 `main`，Pages 的 Git 集成自动构建生产部署 `42d9a7f6-988a-483a-b7bf-c69ea2104cee`（`github:push`，`latest_stage` 为 `deploy`/`success`），构建日志 `Executing user command: npm run build:pages` → `[build:pages] setup (规则 3)`，上传 6 个文件。`https://42d9a7f6.online-gal.pages.dev/` 与 `https://onlinegal.cc.cd/` 的 `index.html` 字节一致（sha256 `1a6f6780…`，6439 B，与本地 `dist/site` 一致），`/__setup/status` 返回 `{"phase":"setup","initialized":false}`。headless Edge 打开 `https://onlinegal.cc.cd/settings/account/?resume=true`：只有“登录 Cloudflare”一个按钮，`input[type=password]` 0 个、`input` 0 个，无 console 报错，无 Service Worker。
