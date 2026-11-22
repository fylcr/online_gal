# Proposal

## 归属

本变更属于管理系统，由负责 `cf-account-admin` 的对话实现。初始化复用该变更的 Cloudflare 登录（`CloudflareAccount`）、`/__cf` 中转和站点管理员判定（A6）。

另有两处要配合：

- **插件系统**：撤销上一版为 `/__setup` 加的内核放行（`plugin-system` tasks.md 11.3）；发布工作流的构建命令改用 `npm run build:pages`（`plugin-system` tasks.md 10.10）；以后需要初始化的项（例如 K18 的 CA 私钥）按本变更 design.md S7 加入清单（`plugin-system` tasks.md 11.2）。
- **前端外壳**：`apps/app` 里不再有任何初始化代码，`Layout.astro` 不再引用门槛组件。

代码边界见 design.md S9。

## Why

用户要求（2026-11-22）："现在先做一个初始化页面，第一次部署到cf会进入初始化页面，登录cf后才能正常工作"。

用户对范围的决定（2026-11-22，原话）：

- A："把需要初始化的都初始化了，例如cf token、github token等等"
- B："只有所有者才需要登录"
- C："初始化后直接重新部署关闭初始化系统"
- D：（重置功能）"不做"
- E："现有的地址改为onlinegal.cc.cd，之后我进行一次初始化，把cf token、accout id等等写进pages secret里"
- F："构建的时候直接用脚本 / 当检测到初次构建的时候只构建出初始页，其他所有的系统都不加载 / 初始化完成后再进行一次构建，与初始化有关的都不被构建，其他的系统才正常加载，与之前设计的一样"

据此，流程如下：

1. 构建脚本检查 Pages 项目里有没有初始化记录 `ONLINE_GAL_SITE`。
2. 没有：**初始化构建**。只产出初始化页和它的服务器接口，任何路径都显示初始化页；插件系统、管理器、注册表、引擎都不构建。
3. 所有者在初始化页用 Cloudflare 登录，页面确认该账号拥有本站的 Pages 项目。
4. 所有者填写并校验全部初始化项（Cloudflare API token、GitHub token；账号 id 自动填入）。
5. 一次写入 Pages 项目：初始化项写成生产环境的 secret，再写入 `ONLINE_GAL_SITE`。
6. 页面触发一次新的构建。脚本这次读到 `ONLINE_GAL_SITE`，产出**站点构建**：与 `plugin-system` 原设计一致，不含任何初始化代码。
7. 新构建生效后，初始化系统在服务器上不存在，访客无需登录即可正常使用。

不提供重置功能。初始化之后要改密钥，只能到 Cloudflare 控制台修改。

## What Changes

- **构建脚本** `scripts/build-pages.mjs`（`npm run build:pages`）：自动判断阶段，产出初始化构建或站点构建，并检查站点构建里没有初始化代码。
- **新 workspace** `apps/setup`：初始化页（唯一页面）、服务器接口 `/__setup/status` 与 `/__setup/verify`、`_worker.js` 入口、初始化项清单、初始化构建脚本。
- **初始化项清单** `apps/setup/src/lib/setup-items.ts`：第一批 `CLOUDFLARE_ACCOUNT_ID`、`CLOUDFLARE_API_TOKEN`、`GITHUB_TOKEN`。
- **重新构建**：Cloudflare Git 构建的项目用 create deployment；发布工作流构建的项目（官方站点）用 GitHub token 重跑那次工作流；其他情况给出手动命令。
- **删除上一版的实现**：`apps/app` 里的 `/setup` 页、门槛脚本和组件、`functions/__setup/`，以及共用文件里的 `__setup` 登记。
- **官方站点地址**：`https://onlinegal.cc.cd/`，作为 Pages 项目的自定义域名。用户上线后在该地址上亲自完成一次初始化。

## 仍待用户决定

以下各项已在 design 里写了默认做法，执行时按默认进行。

- (f) **token 的用途与权限**：目前没有任何服务器功能读取这些 secret。
  - 默认：只校验 token 有效，不检查权限范围。唯一例外是官方站点初始化后重跑发布工作流，需要 GitHub token 对仓库有 Actions 写权限；没有就提示手动重跑。
- (g) **要不要同时写入 GitHub 仓库的 Actions secrets**：发布工作流需要仓库里的 `CLOUDFLARE_API_TOKEN` 和 `CLOUDFLARE_ACCOUNT_ID`。
  - 默认：不写，由用户在 GitHub 上配置（`plugin-system` 7.7）。如果要写，需要用仓库公钥加密（libsodium sealed box），会新增一个加密依赖。

## Capabilities

### New Capabilities

- `site-setup`：两阶段构建、初始化状态、初始化项、初始化页，以及初始化完成后关闭初始化系统。

### Modified Capabilities

- 无。`plugin-system` 撤销 `/__setup` 登记由该变更自己修改；`cf-account-admin` 的 A3 记一条例外（初始化构建没有内核，页面内存持有 OAuth token）。

## Impact

- **新文件**：`scripts/build-pages.mjs`、`apps/setup/**`、测试 `.test/site-setup.test.ts`（gitignored）。
- **删除**：`functions/__setup/`、`apps/app/src/pages/setup.astro`、`apps/app/src/components/setup/`、`apps/app/src/lib/site-gate.js`、`apps/app/src/lib/site-setup.ts`、`apps/app/src/lib/site-setup-items.ts`。
- **改动**：根 `package.json`（`build:pages`、`build:setup`、`check`）、`pnpm-lock.yaml`、`apps/app/src/layouts/Layout.astro`（删引用）、`packages/plugin-system/src/format.ts` 与 `scripts/build-site.mjs`（删 `__setup`）、`kernel.ts`（由插件系统的对话撤销放行）。
- **部署**：Pages 构建命令和发布工作流的构建步骤改为 `npm run build:pages`，输出目录仍是 `dist/site`。
- **Cloudflare**：生产环境新增三个 secret；生产和预览环境新增明文变量 `ONLINE_GAL_SITE`；初始化时多一次构建。
- **依赖**：不新增外部依赖。`apps/setup` 只用仓库里已有的同版本包。
