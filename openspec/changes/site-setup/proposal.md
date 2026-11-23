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
- G（2026-11-23，原话）："初始化应该通过cf oauth拿权限，不需要手动再填填cf token了，然后github token暂时不需要，反正就是初始页只用cf oauth就能完成了，oauth的时候尽可能获取所有权限，避免后续的时候权限不够"
- H（2026-11-23，原话）："重新初始化，然后初始化的时候申请一个可以申请cf apitoken的token，账户token不是用户token"。只对已初始化的站点开一次重新初始化，不恢复通用的重置功能（决定 D 仍然有效）。初始化时用刚登录的 OAuth 凭据申请一个**账户所有**的 API 令牌（account-owned token，`POST /accounts/{account_id}/tokens`），不申请用户令牌（user token，`POST /user/tokens`），并写成生产环境 secret。该令牌自带「Account API Tokens Edit」权限，以后新的初始化项可以用它再申请令牌，不必再重新初始化。

据此，流程如下：

1. 构建脚本检查 Pages 项目里有没有初始化记录 `ONLINE_GAL_SITE`。
2. 没有：**初始化构建**。只产出初始化页和它的服务器接口，任何路径都显示初始化页；插件系统、管理器、注册表、引擎都不构建。
3. 所有者在初始化页用 Cloudflare 登录，页面确认该账号拥有本站的 Pages 项目。
4. 所有者不填写任何 token，也不校验 token。页面用刚登录的 OAuth 凭据直接完成写入和构建；账号 id 由登录结果自动填入。OAuth token 只在页面内存里使用，MUST NOT 写入 Pages secret（access token 约 1 小时过期，refresh token 每次续期都会更换，存成 secret 很快失效）。GitHub token 暂不初始化。
5. 初始化页一次写入 Pages 项目的初始化记录和生产密钥。决定 I 的 Git 集成项目还写入一次性的签名引导标记。
6. 初始化页触发**站点构建**。方案 A 先部署不签名的站点，让 `/__ca/issue` 上线，再触发签名构建。
7. 验证线上签名产物后，初始化页发布 `_trust` TXT 并等待 DoH 验收。站点构建生效后，访客无需登录。

不提供重置功能。初始化之后要改密钥，只能到 Cloudflare 控制台修改。

## What Changes

- **Pages 构建设置**：新增 `npm run configure:pages -- <project>`，用 `cf` CLI 给已连接 Git 的 Pages 项目填入构建命令、根目录和输出目录。预览或触发生产部署由操作者显式选择。
- **构建脚本** `scripts/build-pages.mjs`（`npm run build:pages`）：自动判断阶段，产出初始化构建或站点构建，并检查站点构建里没有初始化代码。
- **新 workspace** `apps/setup`：初始化页（唯一页面）、服务器接口 `/__setup/status` 与 `/__setup/verify`、`_worker.js` 入口、初始化项清单、初始化构建脚本。
- **初始化项清单** `apps/setup/src/lib/setup-items.ts`：第一批只有 `CLOUDFLARE_ACCOUNT_ID`（`source: 'account'`，登录后自动填入，不手填）。删除 `CLOUDFLARE_API_TOKEN` 与 `GITHUB_TOKEN` 两项（决定 G）。以后有服务器功能需要长期调用 Cloudflare API 时，再由那个变更新增一项，那时另行决定 token 来源。
- **决定 H**：第 7 节。已初始化的站点重新初始化一次；初始化时申请一个账户所有的 API 令牌，权限为「Account API Tokens Edit」，写成生产环境 secret `CLOUDFLARE_ACCOUNT_TOKEN`。
- **重新构建**：Cloudflare Git 构建的项目用 create deployment；发布工作流构建的项目只显示手动入口（重跑链接 `https://github.com/<repo>/actions/runs/<runId>`，或推一个新的 `v*` 标签），不再用 GitHub token 自动重跑；其他情况给出手动命令。
- **删除上一版的实现**：`apps/app` 里的 `/setup` 页、门槛脚本和组件、`functions/__setup/`，以及共用文件里的 `__setup` 登记。
- **官方站点地址**：`https://onlinegal.de5.net/`（2026-11-23 决定 J，原为 `onlinegal.cc.cd`），作为 Pages 项目的自定义域名。用户在该地址上亲自完成初始化。旧地址只做 301 跳转。

## 已决定（原 (f)、(g)，2026-11-23 决定 G）

- (f) **token 的用途与权限**：不再手建 API token。登录申请 `CF_DEFAULT_SCOPES`（cf CLI 默认的全部 477 项）加 `offline_access`，这是 cf 官方客户端会申请的最大集合，不另造 scope。实际授予以 token 响应的 `scope` 为准。写入前必须有 `pages:write`，否则停下来说明，不写入。
- (g) **GitHub**：暂不初始化 GitHub token，也不写仓库的 Actions secrets。官方站是 Git 集成 Pages，重新构建走 `pages-git`，不用 GitHub token。

## 已决定（决定 H，2026-11-23）

- **重新初始化只开一次**：已初始化的站点要跑一次初始化构建，写入新的初始化项后再回到站点构建。不做通用的重置功能（决定 D 不变），不给页面加「重置」按钮。方式是一个只在本机跐的命令，用已认证的 `cf` CLI 清掉生产与预览环境的 `ONLINE_GAL_SITE`，不动其他变量，再触发一次生产构建。
- **账户令牌，不是用户令牌**：初始化页用刚登录的 OAuth access token 调 `POST /accounts/{account_id}/tokens`，申请一个账户所有的 API 令牌，写成生产环境 secret `CLOUDFLARE_ACCOUNT_TOKEN`。MUST NOT 调 `POST /user/tokens`，MUST NOT 把 OAuth access token 或 refresh token 写进任何变量。
- **权限只要一项**：该令牌的权限组是「Account API Tokens Edit」（API 权限名 `Account API Tokens Write`），资源限于已确认的这一个账户。它不直接调用 Cloudflare API，只用来以后再申请令牌；新的初始化项需要令牌时用它申请，不必再重新初始化。
- **不手填**：令牌由页面申请后直接写入，不显示输入框，不把明文留在页面上。登录的 `scope` 缺少 `account_api_tokens:create` 时停下说明，不写入。

## 已决定（决定 I、J，2026-11-23）

- **决定 I：构建令牌和 DNS 白名单记录也在初始化时写入**。用户原话：「放进初始化，然后DOH实测一下」。`plugin-system` 的构建签名身份与 `_trust.<域名>` 的 TXT 记录由初始化页生成并写入，用户不再手工配置。
  - (i1) 用户选择**方案 A（两次站点构建）**。第一次部署带有签发接口和生产密钥的站点，跳过官方包签名。该部署在站点域名生效后，初始化页自动发起第二次构建，后者通过 `/__ca/issue` 领取凭证并签名。先用待发布的信任策略验证线上官方包，再发布 TXT，最后从真实 Service Worker 验证 DoH 与签名。执行见 design.md S13、tasks.md 第 9 节。
- **决定 J：官方地址换成 `onlinegal.de5.net`**。用户原话：「将域名换成onlinegal.de5.net,之前的域名删除解析，改成301跳转到新域名」。原因：`cc.cd` 没有 DNSSEC 委托，DoH 对 `onlinegal.cc.cd` 永远是 `AD: false`，K16 的 DNS 白名单在旧域名上用不了；`onlinegal.de5.net` 实测两家解析器都是 `AD: true`。设计见 design.md S12，任务见 tasks.md 第 8 节。

## Capabilities

### New Capabilities

- `site-setup`：两阶段构建、初始化状态、初始化项、初始化页，以及初始化完成后关闭初始化系统。

### Modified Capabilities

- 无。`plugin-system` 撤销 `/__setup` 登记由该变更自己修改；`cf-account-admin` 的 A3 记一条例外（初始化构建没有内核，页面内存持有 OAuth token）。

## Impact

- **新文件**：`scripts/build-pages.mjs`、`scripts/configure-pages.mjs`、`apps/setup/**`、`test/configure-pages.test.mjs` 和 `.test/site-setup.test.ts`（后者 gitignored）。
- **删除**：`functions/__setup/`、`apps/app/src/pages/setup.astro`、`apps/app/src/components/setup/`、`apps/app/src/lib/site-gate.js`、`apps/app/src/lib/site-setup.ts`、`apps/app/src/lib/site-setup-items.ts`。
- **改动**：根 `package.json`（`build:pages`、`build:setup`、`configure:pages`、`check`）、`pnpm-lock.yaml`、`apps/app/src/layouts/Layout.astro`（删引用）、`packages/plugin-system/src/format.ts` 与 `scripts/build-site.mjs`（删 `__setup`）、`kernel.ts`（由插件系统的对话撤销放行）。
- **部署**：Pages 构建命令和发布工作流的构建步骤改为 `npm run build:pages`，输出目录仍是 `dist/site`。
- **Cloudflare**：生产环境保存账户 ID、账户令牌、CA 私钥和构建令牌。生产与预览环境保存 `ONLINE_GAL_SITE`。方案 A 在两次站点构建之间移除一次性生产引导标记，验证签名后在新域名发布一条 TXT。
- **依赖**：不新增 workspace 依赖。`apps/setup` 只用仓库里已有的同版本包；项目配置命令通过 npm 调用固定版本的 `cf` CLI。
