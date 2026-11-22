# Tasks: site-setup

计划书。执行的对话按顺序做，设计细节以 design.md 为准（S1–S9），用户决定 A–F 见 proposal.md。

- **负责**：管理系统的对话（`cf-account-admin` 的对话）。
- **配合**：`plugin-system` tasks.md 11.3（撤销内核对 `/__setup` 的放行）与 10.10（发布工作流用 `npm run build:pages`），由插件系统的对话完成。
- **未决事项**：proposal 的 (f)、(g) 没有回答前，按 design 的默认做法执行。
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
  1. 把 `onlinegal.cc.cd` 绑定为 Pages 项目的自定义域名（已绑定则跳过）。
  2. 准备 Cloudflare API token 和 GitHub token（官方站点要自动重新构建，GitHub token 需要仓库的 Actions 写权限）。
  3. 发布一次，应得到初始化构建。
  4. 打开 `https://onlinegal.cc.cd/` 的任意页面，登录、填写并校验、写入，等待第二次构建完成后回到原页面。
  5. 控制台核对：生产环境有三个 secret 和 `ONLINE_GAL_SITE`，预览环境只有 `ONLINE_GAL_SITE`。
  6. 无痕窗口打开任意页面正常；`curl https://onlinegal.cc.cd/__setup/status` 得不到 `phase: "setup"` 的 JSON。
