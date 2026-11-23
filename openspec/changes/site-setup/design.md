# Design: site-setup

## Context

- **用户要求**（2026-11-22）："现在先做一个初始化页面，第一次部署到cf会进入初始化页面，登录cf后才能正常工作"。
- **用户决定**（同日，原话见 proposal.md）：
  - A：需要初始化的全部初始化，例如 cf token、github token；
  - B：只有所有者需要登录；
  - C：初始化后直接重新部署，关闭初始化系统；
  - D：不做重置；
  - E：官方地址改为 `onlinegal.cc.cd`，用户之后亲自做一次初始化，把 cf token、account id 等写进 Pages secret。
- **用户决定 F**（2026-11-22，第三轮，原话）："构建的时候直接用脚本 / 当检测到初次构建的时候只构建出初始页，其他所有的系统都不加载 / 初始化完成后再进行一次构建，与初始化有关的都不被构建，其他的系统才正常加载，与之前设计的一样"。
  - 据此改为**两阶段构建**（S2）。上一版的页面门槛、内核放行 `/__setup`、站点里的 `/setup` 页全部取消（S6）。
- **用户决定 G**（2026-11-23，原话）："初始化应该通过cf oauth拿权限，不需要手动再填填cf token了，然后github token暂时不需要，反正就是初始页只用cf oauth就能完成了，oauth的时候尽可能获取所有权限，避免后续的时候权限不够"。
  - 据此：初始化页只靠 OAuth 登录完成，不手填、不校验 token；第一批初始化项只剩 `CLOUDFLARE_ACCOUNT_ID`；`github-rerun` 只给手动入口；proposal 的 (f)、(g) 结案（S5 第 2、4、6、7 步，S7）。

可以复用的已有能力（`cf-account-admin`）：

- `packages/plugin-system/src/account/cloudflare.ts` 的 `CloudflareAccount`：设备码登录（`login`/`poll`）、`status()`（含 A6 的 `siteAdmin` 判定）、`forward(request, pathAndQuery)`（补上 `Authorization` 后经 `/__cf` 发往 Cloudflare）。
  - 构造参数 `{ origin, siteHost, fetch, vault, now }`，不依赖 Service Worker。`vault` 是 `account/vault.ts` 的 `Vault`，底层 `VaultStore` 可以换成内存实现。
  - `packages/plugin-system/src/index.ts` 已导出 `account/vault.js` 和 `account/cloudflare.js`。
  - `account/cf-identity.generated.ts` 在版本库里，不需要运行 `build:cf` 生成。
- `functions/__cf/[[path]].ts`：同源中转。`/__cf/api/<rest>` → `api.cloudflare.com/client/v4/<rest>`，`/__cf/oauth/**` → `dash.cloudflare.com/oauth2/**`；转发页面带来的 `authorization`、`content-type`、`accept`、`content-length`；只接受同源请求。导出 `onRequest(context: { request })`。
- A6 站点管理员判定：比较当前主机名与项目的 `subdomain`、`domains[]`；在 localhost 上改用 `SITE_HOST`（默认 `onlinegal.de5.net`，2026-11-23 决定 J 前为 `onlinegal.cc.cd`）。

本变更要遵守的约束：

- **变量要重新构建才生效**：Pages 的环境变量和 secret 只对之后的新部署生效。
- **Pages 的构建环境能读到项目变量**：Git 集成的项目，控制台或 API 写入的变量在构建命令里以 `process.env` 出现（已在隔离项目实测，见 0.1）。
- **直接上传的项目 Cloudflare 不会构建**：retry 只会复用旧产物，不能产生新的构建。所以 C 的"重新部署"必须是一次新的构建（S5 第 7 步）。
- **`_worker.js` 会覆盖 `functions/`**：输出目录里有 `_worker.js` 时，Pages 进入 advanced mode，整个 `functions/` 目录被忽略；`_routes.json` 仍然生效。
- **官方站点的发布方式**：`plugin-system` K15、K17 要求在官方发布工作流中生成来源证明并发布同一份产物。用户要求使用 `cf` CLI。目前 `cf` 缺少 Pages 静态资源上传命令，正式发布方式待协调。Git 集成分支重建已在隔离项目验证，它不会携带 GitHub Actions 生成的来源证明。
- **管理器技术栈固定**：Astro 7 + Svelte 5 + Tailwind v4/daisyUI，不加其他框架（`openspec/config.yaml`）。

## Goals / Non-Goals

**Goals**

- 用一个构建脚本完成两种构建（用户决定 F）：
  - **初始化构建**：站点未初始化时，只构建初始化页和它需要的服务器接口；插件系统、管理器、注册表、引擎都不构建，也不加载。
  - **站点构建**：站点已初始化时，构建产物与 `plugin-system` 原有设计一致，不含任何与初始化有关的代码、页面、接口或路由。
- 所有者只用 Cloudflare OAuth 登录（决定 G），不手填任何 token；确认所有者后一次写入 Pages 项目，再触发一次新的构建；构建完成后站点对所有人正常工作。
- 初始化只做一次（用户决定 D）。状态存放在 Cloudflare 项目里，与浏览器无关。
- OAuth token 只在页面内存中停留，不落盘，也不写成 Pages secret（access token 约 1 小时过期，refresh token 续期时会更换）。
- 不新建 Cloudflare 资源（KV、D1），不新增外部依赖（新的 workspace 只使用仓库里已有的包和版本）。

**Non-Goals**

- 访问控制。初始化构建是公开的静态页，所有写操作都要所有者的 Cloudflare 登录。
- 重置或重新初始化（用户决定 D）。初始化之后改密钥，到 Cloudflare 控制台修改。
  - 如果有人在控制台删掉 `ONLINE_GAL_SITE` 再构建，脚本会重新产出初始化构建。这是控制台操作的后果，不是本变更提供的功能。
- 从预览地址初始化。A6 不匹配预览主机名，会停在所有者一步。
- 手填或校验任何 token（决定 G）。登录申请的权限范围见 S5 第 2 步。
- 初始化 GitHub token，或写入 GitHub 仓库的 Actions secrets（决定 G，原 (g)）。
- 用 GitHub API 自动重跑工作流。`github-rerun` 只给手动入口（S5 第 7 步）。
- 读取这些 secret 的服务器功能。初始化只负责写入，使用由以后的功能各自实现。

## Decisions

### S1 — 写入 Pages 项目的内容

全部在 S5 第 6 步用一次 PATCH 写入。

**初始化记录 `ONLINE_GAL_SITE`**

- 取值：明文 JSON `{"v":1,"account":"<accountId>","project":"<projectName>","by":"<userId>","at":"<ISO 8601>"}`，类型为 `plain_text`。
- 同时写入 `deployment_configs.production.env_vars` 和 `deployment_configs.preview.env_vars`，这样预览构建也是站点构建。
- 判定（构建脚本 S2 和服务器接口 S4 相同）：
  - 不存在或为空字符串：未初始化。
  - 存在：已初始化。能解析出 `v === 1` 是正常情况；无法解析时仍按已初始化处理并给出警告，记录损坏时不会把站点退回初始化构建。

**初始化项**

- S7 清单里的每一项，按项名写成 `secret_text`，**只写生产环境**。预览部署可以由任意分支产生，不应该拿到这些凭据。

**写入方式**

- PATCH 请求体只带上面这些键，不包含项目的其他变量。
- Cloudflare 的 PATCH 会合并 `env_vars`，删除一个变量要显式设为 `null`。所以写入不影响其他变量。合并行为要在 0.1 实测确认。

**为什么用环境变量**：不需要创建资源和绑定；构建环境和 Function 都能直接读到；状态只会从"未初始化"变成"已初始化"；secret 本来就是 Pages 存放凭据的方式（用户决定 E）。

**约束**：仓库不得给 Pages 项目加入会覆盖 API 或控制台变量的部署配置文件。Pages 项目变量由 Cloudflare 项目设置维护。

### S2 — 构建脚本 `scripts/build-pages.mjs`（用户决定 F）

**入口**（根 `package.json`）

- `npm run build:pages` → `node scripts/build-pages.mjs`：自动判断阶段。Pages 的构建命令和发布工作流的构建步骤都用它。
- `npm run build:setup` → `node scripts/build-pages.mjs --phase=setup`：本地调试初始化构建。
- `npm run build:site` 保持原样（`plugin-system` 的站点构建）。脚本的站点阶段调用它，再做 S6 的检查。
- 输出目录两个阶段都是 `dist/site/`。脚本开始时先清空 `dist/site/`。

**判断阶段**，按顺序取第一个能决定的：

1. 命令行 `--phase=setup` 或 `--phase=site`。
2. 环境变量 `ONLINE_GAL_SITE` 存在且非空 → `site`。Cloudflare 的 Git 构建会把项目变量注入构建环境。
3. `CF_PAGES === '1'`（Cloudflare 的构建环境）→ `setup`。
4. （可选路径，只用于发布工作流或本机手动构建；初始化不再写入 `CLOUDFLARE_API_TOKEN`，这三个变量要由操作者自己提供，决定 G）`CLOUDFLARE_API_TOKEN`、`CLOUDFLARE_ACCOUNT_ID`、`ONLINE_GAL_PAGES_PROJECT` 三者都有 → 请求 `GET https://api.cloudflare.com/client/v4/accounts/{id}/pages/projects/{project}`：
   - 生产环境的 `env_vars` 含非空 `ONLINE_GAL_SITE` → `site`，否则 → `setup`；
   - 请求失败或 `success` 不为真 → 以非零状态退出，不猜。
5. 以上都不满足 → 以非零状态退出，提示"无法判断构建阶段，请传 --phase=setup 或 --phase=site"。

**日志与输出**

- 打印选定的阶段和依据（第几条规则）。不打印任何变量的值，`ONLINE_GAL_SITE` 只说"存在/不存在"。
- 有 `GITHUB_OUTPUT` 时，追加一行 `phase=<setup|site>`，供工作流决定是否做来源证明。

**为什么用一个脚本自动判断**：同一份代码、同一个构建命令，Cloudflare 的 Git 构建和 GitHub Actions 都能用；写入变量后只要再构建一次，就自动得到站点构建，不需要人改配置。

### S3 — 初始化构建的产物

**只构建这些**：`apps/setup` 的页面、`_worker.js`、清理旧内核的 `sw.js`、`_routes.json`、`_headers`。

- 不运行 `build:packages`、`build:cf`、`apps/app` 的构建、`build-site.mjs`，也不构建引擎和插件包。页面需要的 `@online-gal/plugin-system` 源码由 `apps/setup` 自己的打包直接编译。
- 实现放在 `apps/setup/scripts/build-setup.mjs`，由 `build-pages.mjs` 调用。

**`dist/site/` 顶层只允许以下条目**（S6 的检查会核对）：

| 条目 | 内容 |
|---|---|
| `index.html` | 初始化页，唯一的页面 |
| `_astro/` | 初始化页的脚本和样式 |
| `favicon.svg` | 可选 |
| `sw.js` | 清理旧内核的 Service Worker（见下） |
| `_worker.js` | 服务器入口（见下） |
| `_routes.json` | `{"version":1,"include":["/__cf/*","/__setup/*"],"exclude":[]}` |
| `_headers` | 见下 |

- **没有顶层 `404.html`**：Pages 对找不到的路径返回根 `index.html`，所以任何路径都显示初始化页，不需要门槛和跳转。
- **`_worker.js`**：入口 `apps/setup/server/worker.ts`，用 esbuild 打成单个 ES module。
  - `/__cf` 和 `/__cf/**` → `functions/__cf/[[path]].ts` 的 `onRequest({ request })`；
  - `/__setup` 和 `/__setup/**` → `apps/setup/server/setup.ts` 的 `onRequest({ request, env })`；
  - 其他 → `env.ASSETS.fetch(request)`。
  - 因为有 `_worker.js`，仓库里的 `functions/` 在初始化构建中被忽略；`/__ca` 等插件系统接口不存在。
- **`sw.js`（清理旧内核）**：同一个浏览器以前可能装过站点构建的内核，它会继续用缓存接管页面，初始化页就出不来。
  - `install` 时 `skipWaiting()`；`activate` 时删除本源的全部 CacheStorage，`registration.unregister()`，再让每个窗口客户端 `navigate(client.url)`。没有 `fetch` 处理。
  - 初始化页不注册它。浏览器在导航时检查已注册内核的更新，会拿到这个文件并执行。
- **`_headers`**：
  ```
  /*
    Cache-Control: no-store
    X-Content-Type-Options: nosniff
    Referrer-Policy: no-referrer
    X-Frame-Options: DENY
    Content-Security-Policy: frame-ancestors 'none'
  ```
  `no-store` 保证换成站点构建后浏览器不会拿到旧的初始化页。
- **构建时写入页面的配置**：脚本把下面的 JSON 通过环境变量 `PUBLIC_ONLINE_GAL_SETUP` 交给 `astro build`，页面用 `import.meta.env.PUBLIC_ONLINE_GAL_SETUP` 读取。不得放入任何密钥。
  ```json
  { "siteHost": "onlinegal.de5.net", "rebuild": { "kind": "..." }, "builtAt": "<ISO 8601>" }
  ```
  - `siteHost` 取环境变量 `SITE_HOST`，默认 `onlinegal.de5.net`（与 A6 相同）。
  - `rebuild` 决定 S5 第 7 步怎样触发新的构建：

  | 构建环境 | `rebuild` |
  |---|---|
  | `CF_PAGES === '1'` | `{ "kind": "pages-git" }` |
  | `GITHUB_ACTIONS === 'true'` | `{ "kind": "github-rerun", "repo": "<GITHUB_REPOSITORY>", "runId": "<GITHUB_RUN_ID>" }` |
  | 其他 | `{ "kind": "manual" }` |

### S4 — 服务器接口 `apps/setup/server/setup.ts`

只存在于初始化构建（S3 的 `_worker.js`）。由原 `functions/__setup/[[path]].ts` 移来，行为不变，只在 `status` 里加上 `phase`。

**`GET /__setup/status`**

- 返回 200 JSON：`{ "phase": "setup", "initialized": boolean, "v"?: 1, "warning"?: string }`。
  - `initialized` 按 S1 判定运行时的 `env.ONLINE_GAL_SITE`。初始化构建在运行时读到它，说明变量已写入但还没重新构建（例如有人对初始化构建点了 retry）。
  - 不返回账号 id、用户 id 和项目名。
  - 响应头带 `Cache-Control: no-store` 和 `X-Content-Type-Options: nosniff`。
- `HEAD` 与 `GET` 返回相同的状态和响应头，没有响应体。其他方法返回 405，并带 `Allow: GET, HEAD`。
- 上一版用 Wrangler 4.33.1 在本地实测：未绑定时 `{"initialized":false}`，绑定合法记录时 `{"initialized":true,"v":1}`；HEAD 200 空正文；POST 405；未知路径 404。

**`POST /__setup/verify`**

- **当前无使用者**（决定 G）：初始化页不再手填 token，也不再调用这个接口。接口和它的两个 `kind` 先保留，供以后需要手填并校验的初始化项使用；本变更不删除，也不为它新增功能。

- **何时可用**：只在 `env.ONLINE_GAL_SITE` 缺失或为空时可用。非空（包括无法解析）时，任何方法都返回 404。
- **检查顺序**：
  1. 方法不是 POST：405，带 `Allow: POST`。
  2. 来源不是同源：403。判定与 `functions/__cf/[[path]].ts` 相同：`Sec-Fetch-Site: same-origin`，或者 `Origin` 等于本站。
  3. 请求体有问题：400。要求 `Content-Type` 是 `application/json`；不超过 8 KiB；形如 `{ "kind": "cloudflare", "token": string, "accountId": string }` 或 `{ "kind": "github", "token": string }`；`accountId` 匹配 `^[0-9a-f]{32}$`；`token` 长度 1–512 且没有空白字符。
- **校验 `cloudflare`**：先 `GET https://api.cloudflare.com/client/v4/user/tokens/verify`，`success` 为真且 `result.status === "active"` 即有效；否则再试 `GET .../accounts/{accountId}/tokens/verify`。结果 `{ "ok": boolean, "status"?: string, "expiresOn"?: string }`。
- **校验 `github`**：`GET https://api.github.com/user`，带 `Authorization: Bearer <token>`、`Accept: application/vnd.github+json`、`X-GitHub-Api-Version: 2022-11-28`、`User-Agent: online-gal-setup`。200 即有效。结果 `{ "ok": boolean, "login"?: string, "scopes"?: string[] | null, "expiresAt"?: string | null }`（`scopes` 来自 `X-OAuth-Scopes`，`expiresAt` 来自 `GitHub-Authentication-Token-Expiration`）。
- **上游失败**：每次上游请求限时 10 秒。超时或网络错误返回 200 和 `{ "ok": false, "reason": "upstream" }`；token 无效返回 200 和 `{ "ok": false, "reason": "invalid" }`。
- **不泄露**：响应不回显 token；不记录日志；不转发上游的其他字段；响应头带 `no-store` 和 `nosniff`。

**其他路径**：`/__setup/` 下除 `status`、`verify` 以外的路径返回 404。

### S5 — 初始化页

**文件**（新 workspace `apps/setup`，包名 `@online-gal/setup`）

- `apps/setup/src/pages/index.astro`：唯一的页面。
- `apps/setup/src/components/SetupFlow.svelte`：流程界面。
- `apps/setup/src/lib/setup.ts`：所有请求逻辑，以便在 Node 里用假 `fetch` 测试。
- `apps/setup/src/lib/setup-items.ts`：初始化项清单（S7）。
- `apps/setup/src/lib/memory-vault.ts`：内存 `VaultStore`。
- 依赖只用 `apps/app` 已有的同版本包：`astro`、`@astrojs/svelte`、`svelte`、`tailwindcss`、`@tailwindcss/vite`、`daisyui`，加上 `@online-gal/plugin-system`（`workspace:*`）和 `esbuild`（与 `plugin-system` 相同的 `0.27.7`）。
- 页面打包产物不得包含内核（`kernel.ts`、`sw.ts`）和插件规则层的代码。如果从包入口导入会带进这些代码，就在 `packages/plugin-system/package.json` 加子路径导出 `./account`（这一行归 `cf-account-admin`）。

**登录方式（没有内核）**

- 页面自己构造 `CloudflareAccount`：`{ origin: location.origin, siteHost: 配置.siteHost, fetch: globalThis.fetch.bind(globalThis), vault: new Vault(内存 VaultStore), now: Date.now }`。
- 页面没有 Service Worker，所以 `forward` 发往 `/__cf/**` 的请求直接到达 `_worker.js` 里的 `__cf` 中转，`Authorization` 照常转发。
- OAuth token 只在这个对象的内存里，页面关闭即丢失。这是 `cf-account-admin` A3（"token 只在内核"）的例外，已记入该变更的 design.md A3。

**步骤**：每一步失败都停在这一步，显示原因并提供"重试"，不自动跳过。

1. **状态**：查询 `/__setup/status`。
   - 得到 HTTP 响应但不是 `phase: "setup"` 的 JSON（404、HTML 等）：站点构建已经生效，这是缓存的旧页面。`location.reload()` 一次（用 `sessionStorage` 防止循环），仍然如此时显示"请刷新页面"。网络错误按失败处理，不算生效。
   - `initialized: true`：显示"初始化已写入，等待重新构建"，登录后直接到第 7 步。
   - `initialized: false`：进入第 2 步。
   - `next` 取当前地址的 `pathname + search`；`safeNext` 只接受以 `/` 开头、且不以 `//` 开头的值，否则用 `/`。
2. **登录**：申请的权限范围是 `CF_DEFAULT_SCOPES`（`packages/plugin-system/src/account/cf-identity.generated.ts`，由 `packages/cf-browser/scripts/identity.mjs` 从 `@cloudflare/workers-auth` 0.18.4 生成，共 477 项，是 cf CLI 默认申请的全部权限）加 `offline_access`。这已是 cf 官方 client id 能申请的最大集合，满足决定 G 的"尽可能获取所有权限"，不改登录代码，不另造 scope。登录成功后以 token 响应里的 `scope` 为准；缺少 `pages:read` 或 `pages:write` 时停在这一步，说明缺哪一项，并提供"退出并更换账号"。`login()` 取验证码，显示验证码、"打开验证页"按钮（`verificationUriComplete`，没有时用 `verificationUri`）和倒计时，由页面按 `interval` 调用 `poll()`，处理 `pending`、`slow-down`、`expired`、`denied`。
3. **所有者**：
   - `status().siteAdmin` 为空：显示 `siteAdminError`，或"当前账号没有拥有本站（<hostname>）的 Pages 项目"，并提供"退出并更换账号"（`logout()` 后回到第 2 步）。
   - 不为空：显示账号名和项目名。然后 `GET /accounts/{a}/pages/projects/{p}`，读取生产环境 `env_vars` 的键名和 `production_branch`：
     - 有非空 `ONLINE_GAL_SITE`：已经写入、只差构建，恢复记录里的写入时间并直接到第 7 步。不再填写或写入。
     - 没有：进入第 4 步。
4. **确认写入**（决定 G：不再填写和校验）：只读列出将要写入的键、类型和环境（S7 清单，第一批只有 `CLOUDFLARE_ACCOUNT_ID`，值取 `siteAdmin.accountId`，打码显示）和 `ONLINE_GAL_SITE`。页面没有任何 token 输入框和"校验"按钮。"写入并构建"按钮直接可用。
   - 清单里若出现 `source: "input"` 的项（以后的变更登记），页面 MUST 停下并提示"该项需手动填写，初始化页暂不支持"，不写入。是否恢复手填由那个变更决定。
5. （保留编号，原"内核"步骤已取消。）
6. **写入**：
   - 支持 Web Locks 的浏览器先锁住同源同项目的提交。锁内重新查询 `/__setup/status`，再读取项目的 `ONLINE_GAL_SITE`。任一显示已写入或已初始化，就停止并按第 3 步处理。
   - `PATCH /accounts/{accountId}/pages/projects/{projectName}`，请求体：
     ```json
     { "deployment_configs": {
         "production": { "env_vars": {
             "ONLINE_GAL_SITE": { "type": "plain_text", "value": "<S1 的 JSON 字符串>" },
             "CLOUDFLARE_ACCOUNT_ID": { "type": "secret_text", "value": "<accountId>" } } },
         "preview": { "env_vars": {
             "ONLINE_GAL_SITE": { "type": "plain_text", "value": "<同上>" } } } } }
     ```
     - 实际的 secret 键由 S7 清单生成，上面是第一批（决定 G 后只有一项）。
     - MUST NOT 把 OAuth access token 或 refresh token 写进任何变量：access token 约 1 小时过期，refresh token 每次续期会被更换，写成 secret 很快就失效。
     - `by` 是 `status().user.id`，取不到时用空字符串；`at` 是当前时间。
   - 成功后再 `GET` 一次项目，确认生产环境包含全部初始化项的键名，并核对非敏感的 `ONLINE_GAL_SITE` 记录。读回其他会话的记录时，继续检查该记录对应的部署。
   - 把写入时间存入 `sessionStorage['online-gal:setup-written-at']`。
   - OAuth 凭据继续留在 `CloudflareAccount` 内存里，第 7 步触发构建和轮询还要用；流程结束或页面关闭即丢失。
7. **重新构建**（用户决定 C、F）：按配置的 `rebuild.kind`：
   - `pages-git`：`POST /accounts/{a}/pages/projects/{p}/deployments`，`multipart/form-data`，字段 `branch=<production_branch>`。Cloudflare 从该分支重新构建，构建环境里有了 `ONLINE_GAL_SITE`，脚本产出站点构建。把新部署 id 存入 `sessionStorage['online-gal:setup-deployment']`，每 5 秒查询 `GET .../deployments/{id}` 的 `latest_stage`。
   - `github-rerun`（决定 G：没有 GitHub token，不调 GitHub API）：只显示两种手动方法：打开 `https://github.com/{repo}/actions/runs/{runId}` 点 "Re-run all jobs"，或推一个新的 `v*` 标签。发布工作流要产出站点构建，需要操作者自己在仓库里配好 S2 第 4 条的三个变量（`plugin-system` 7.7）。当前官方站是 `pages-git`，不走这条。
   - `manual`：设置好 S2 判断阶段所需的环境变量后，可运行 `npm run build:pages` 检查产物。Git 集成项目用 `cf pages deployments create <project> --branch <production-branch>` 触发生产构建。直接上传项目使用已配置的发布流程发布 `dist/site`。
   - `github-rerun` 和 `manual`：每 5 秒查询 `GET .../deployments?env=production`，等待一条 `created_on` 晚于写入时间、`latest_stage.name` 为 `deploy` 且 `latest_stage.status` 为 `success` 的部署。无法恢复写入时间时，以本站状态接口确认切换。
   - 任一方式：状态为 `failure` 或 `canceled` 时显示失败，提供"重新构建"，只重做这一步，因为变量已经写入。超过 30 分钟提示到 Cloudflare 控制台查看。
8. **确认切换**：每 5 秒查询 `/__setup/status`，最多 5 分钟，直到得到不是 `phase: "setup"` 的 HTTP 响应（站点构建里没有这个接口，会返回 404 或 HTML）。
   - 然后清除 `sessionStorage` 里的本流程记录，显示"初始化完成"，`location.replace(safeNext(next))`。
   - 超时：提示稍后刷新。

**中断恢复**

- 刷新后从第 1 步重新开始。
- 第 3 步能发现"已写入、未构建"，直接到第 7 步，不会再次写入。
- `sessionStorage` 里有部署 id 时，第 7 步直接接着轮询这个部署。
- 刷新后 OAuth 凭据丢失，要重新登录一次才能继续第 7 步。

**token 的处理**

- 只存放在组件和 `CloudflareAccount` 的内存里。不写入 `localStorage`、`sessionStorage`、IndexedDB，不出现在 URL 和日志中。
- 请求经 `_worker.js` 里的 `__cf` 中转发往 Cloudflare，OAuth token 在 `Authorization` 头里。这条中转路径不得记录请求体，这是 `cf-account-admin` 已有的约束。

**本机地址**

- 在 `localhost`、`127.0.0.1`、`[::1]` 上，A6 把主机名映射到 `siteHost` 对应的线上项目。
- 为了不让本地调试改动线上项目，第 6、7 步在本机地址上不发请求，只显示将要写入的键、类型和环境（值打码）以及将要执行的重新构建方式，作为"本地预演"。
- 第 1–4 步照常进行，因为它们只读。

### S6 — 站点构建不含初始化系统（用户决定 F）

**源码位置**：与初始化有关的全部代码只放在 `apps/setup/` 和 `scripts/build-pages.mjs`。

- `functions/` 下只留站点构建的接口（`__cf`，以后的 `__ca`）。`functions/__setup/` 移到 `apps/setup/server/setup.ts`。
- `apps/app` 里不留任何初始化代码。删除上一版留下的：
  - `apps/app/src/pages/setup.astro`
  - `apps/app/src/components/setup/`（`SetupFlow.svelte`、`SiteGate.astro`）
  - `apps/app/src/lib/site-gate.js`、`apps/app/src/lib/site-setup.ts`、`apps/app/src/lib/site-setup-items.ts`
  - `apps/app/src/layouts/Layout.astro` 里的 `<SiteGate />` 引用和对应的 import
  - 其中可复用的逻辑（请求、校验、清单）移到 `apps/setup`。
- 撤销上一版在共用文件里的登记：
  - `packages/plugin-system/src/format.ts` 保留路径正则里的 `__setup`；
  - `packages/plugin-system/scripts/build-site.mjs` 的 `RESERVED` 里的 `__setup`，`_routes.json` 的 `/__setup/*`；
  - `packages/plugin-system/src/kernel.ts` 对 `/__setup` 的放行和 `test/plugin-system-route.test.mjs` 里对应的用例，由插件系统的对话做（`plugin-system` tasks.md 11.3）。

**站点阶段的检查**：`build-pages.mjs` 在 `npm run build:site` 之后核对，任一不满足就以非零状态退出：

- 仓库里不存在 `functions/__setup/`；
- `dist/site/_worker.js` 不存在；
- `dist/site/_routes.json` 的 `include` 不含 `/__setup/*`；
- `dist/site/` 下任何文件的内容都不含字符串 `__setup` 和 `ONLINE_GAL_SETUP`。

**初始化阶段的检查**：`dist/site/` 顶层条目只能是 S3 表里列出的；`dist/site/` 下没有 `registry/`；`_worker.js` 和 `_astro/` 里不含插件系统内核的标识（例如 `registry/index.json`、`__kernel`）。

### S7 — 初始化项清单 `apps/setup/src/lib/setup-items.ts`

**条目结构**

```ts
export interface SetupItem {
  /** Pages secret 的名字，匹配 ^[A-Z][A-Z0-9_]*$，不得是 ONLINE_GAL_SITE */
  name: string;
  label: string;                 // 中文名称
  help: string;                  // 一两句说明：用途、去哪创建
  helpUrl?: string;              // 创建入口
  source: 'account' | 'input' | 'generated';   // account：取 siteAdmin.accountId，只读；generated：页面在写入前生成
  verify?: 'cloudflare' | 'github'; // 对应 POST /__setup/verify 的 kind
  owner: string;                 // 登记它的变更名
}
export const SETUP_ITEMS: readonly SetupItem[];
```

**第一批**（`owner: 'site-setup'`，来自用户决定 A、E）

| `name` | `source` | `verify` | `helpUrl` |
|---|---|---|---|
| `CLOUDFLARE_ACCOUNT_ID` | `account` | — | — |
| `CLOUDFLARE_ACCOUNT_TOKEN` | `generated` | — | — |

- 决定 G 删除了 `CLOUDFLARE_API_TOKEN` 与 `GITHUB_TOKEN` 两项。OAuth token 会过期，不能当长期 secret。
- 决定 H 新增两项，都是 `source: 'generated'`，生成方式见 S11，不手填、不校验：
  - `CLOUDFLARE_ACCOUNT_TOKEN`（`owner: 'site-setup'`）：初始化页用 OAuth 凭据申请的账户令牌，权限只有「Account API Tokens Edit」，资源限于已确认的账户。
  - `ONLINE_GAL_CA_PRIVATE_JWK`（`owner: 'plugin-system'`）：浏览器里生成的 ECDSA P-256 私钥 JWK，K18 微型 CA 的长期私钥。这是 `plugin-system` tasks.md 11.2 的登记。
- `source: 'input'` 与 `verify` 字段保留在类型里，当前没有条目使用。`source: 'generated'` 是决定 H 新增的，表示值由页面在写入前生成，不取自账号、不由用户填写。

**扩展规则**

- 其他变更需要在初始化时写入新的项，就在清单末尾加条目，`owner` 写自己的变更名。
- 需要新的校验或生成方式时，同时在 S4 的 `verify` 里加一个 `kind`，并在本 design 记录。这一处由管理系统的对话实现，登记方提出需求即可。
- 只能在站点初始化之前加入。已经初始化的站点不会再有初始化构建（用户决定 D）。新加的项在已初始化的站点上要到 Cloudflare 控制台手动添加；登记方的功能读不到这个 secret 时，必须给出明确报错，不能静默失败。
- 已知的后续登记：`plugin-system` K18 微型 CA 的私钥和 OIDC 主体映射（`plugin-system` tasks.md 11.2）。

**使用者**：读取这些 secret 的服务器功能都在站点构建里，以后实现，一律从 `env.<name>` 读取。任何 Function 都不得在响应或日志中输出这些值。

### S8 — 初始化后关闭初始化系统（用户决定 C、F）

- **结构上关闭**：站点构建里没有初始化页、`/__setup` 接口和 `_worker.js`（S6）。新构建生效后，初始化系统在服务器上不存在，任何路径都由站点构建应答。
- **初始化构建在变量写入后仍在线的窗口里**（写入到新构建生效之间，或有人对初始化构建点了 retry）：
  - `/__setup/verify` 返回 404（S4）；
  - 初始化页在第 1、3、6 步发现已写入，就进入重新构建。同浏览器的标签页写入由 Web Locks 串行。Pages 项目 PATCH 接口没有文档化的条件写入参数，不同设备的所有者若同时提交，仍可能在两个读取之间先后写入。初始化应由一位所有者在一个会话中操作。
- **旧浏览器**：初始化构建的 `Cache-Control: no-store` 保证换成站点构建后浏览器重新取页面。

### S9 — 归属与代码边界

- **只属于本变更**，由管理系统的对话负责：
  - `apps/setup/**`（页面、服务器接口、`_worker.js` 入口、构建脚本、清单）
  - `scripts/build-pages.mjs`
  - `.test/site-setup.test.ts`（gitignored）
- **共用文件中属于本变更的部分**：
  - 根 `package.json` 的 `build:pages`、`build:setup` 两条脚本，以及 `check` 里对 `@online-gal/setup` 的检查；
  - `pnpm-lock.yaml` 里 `apps/setup` 的条目；
  - `apps/setup/src/lib/setup-items.ts` 里其他变更加入的条目属于各自的变更（S7）。
- **借用的代码**：`functions/__cf/[[path]].ts`、`packages/plugin-system/src/account/**` 属于 `cf-account-admin`（同一个对话），本变更只导入，不为初始化改它们的行为。
- **不属于本变更**：`npm run build:site` 及其产物、`kernel.ts`、`.github/workflows/release.yml`（`plugin-system`）。
- **提交**：使用显式 pathspec。共用文件只提交上面列出的部分。

### S10 — 从仓库配置 Pages 的 Git 构建

Pages 的构建命令由项目设置决定，所以读取仓库源码的构建无法为自身填入控制台表单。项目创建时连接 GitHub 或 GitLab；仓库提供 `npm run configure:pages -- <project>`，供已有权限的操作者通过 `cf@1.0.0-rc.2` 填入三项设置：`npm run build:pages`、仓库根目录、`dist/site`。不把凭据或部署配置文件放入仓库。

- 命令使用 `cf` 的登录状态和账号选择。可以用 `--profile` 或 `CLOUDFLARE_ACCOUNT_ID` 选定目标账号；项目名必须明确给出。查询不到项目或项目没有 Git 来源时停止，不自动创建项目或更改 Git 来源。
- `--dry-run` 只读取项目和显示目标设置。实际写入只发送 `build_config` 的三个字段，重新读取确认；配置已正确时跳过 PATCH。
- 附加 `--deploy` 才用读回的生产分支发起新部署；返回部署 ID 只表示请求已受理。未初始化的生产项目在该部署生效后会暂时只显示初始化页。
- 该命令不修改域名、分支、变量、密钥和官方发布流水线，也不为 Direct Upload 项目添加 Git 集成。

## Verification record

2026-11-22 使用 `cf@1.0.0-rc.2` 验证，未变更生产项目：

- **S1 — 项目变量**：隔离的 Git 集成项目上，`cf pages edit` 保留无关变量 `PROBE_KEEP`。`cf pages get` 返回生产环境三个测试 secret 的键名和 `secret_text` 类型，`value` 为空。预览只收到初始化记录，保留 `PROBE_KEEP`。
- **S2 — Git 重建**：隔离项目 `site-setup-git-probe-og-20261122` 在提交 `0bbbb2d38ca68ccd40afa3e99af065ee57cdac43` 上执行 `npm run build:pages`，输出 `dist/site`。部署 `c74d1120-8bf4-4656-8044-3f03b7c38074` 选择 `setup`（规则 3）。写入测试初始化记录后用 `cf pages deployments create` 重建同一生产分支，部署 `ebe8181b-dee9-424f-ab66-23d387b92831` 选择 `site`（规则 2）。两个部署的 `latest_stage` 均为 `deploy/success`。
- **S3 — Pages 路由**：用最小夹具确认 `_worker.js` 优先于同路由的 `functions/`。完整初始化部署的设置深链、未知路径和 `/registry/index.json` 均返回初始化 HTML，`/__setup/status` 返回初始化 JSON，`/__cf/api/user` 返回 Cloudflare 的未授权响应。响应含 `no-store` 和 `DENY`。完整站点部署生效后，状态接口改为返回站点 HTML，注册表返回 JSON，`/sw.js` 无初始化标识。
- **本地产物**：运行两阶段构建及生成的 Worker；浏览器打开设置深链后显示 Cloudflare 登录入口且没有内核接管。正式站 `dist/site` 不含 `_worker.js`、`__setup` 或 `ONLINE_GAL_SETUP`。15 项 `site-setup` 单测通过。
- **官方发布依赖**：`cf` 已验证可以触发 Git 集成 Pages 构建。其 Pages Direct Upload 能提交已上传资源的 manifest 与 Worker，缺少 Pages 静态资源上传命令。发布 GitHub Actions 生成来源证明的完整产物仍待解决。

2026-11-22 的发布记录：`1ba0a94` 仅将两份 Artemis 临时目录测试从布局检查中豁免；`8656bfb` 将初始化构建、Admin 和配套 OpenSpec 推送至 GitHub `main`，Pages 使用当时配置的 `npm run build:site` 构建通过，未启用生产初始化。GitHub Artemis 构建和 portable 测试在上一个基线提交上也失败。

## Risks / Trade-offs
- **上线即锁**：未初始化的站点只有初始化页。用户已决定上线后亲自完成一次初始化（决定 E）。
- **`/__setup/verify` 和 `/__cf` 在初始化构建里对任何同源页面开放**：任何人都可以借 verify 检查一个 token 是否有效；它只返回是否有效以及少量公开元数据，不能写入。决定 G 之后页面不再调用 verify，风险不变。写入和构建都要所有者的 Cloudflare 登录。
- **OAuth token 在页面内存里**：初始化构建没有内核，只能这样（A3 的例外）。页面只有一个，没有第三方代码，`X-Frame-Options: DENY` 防止被嵌入。
- **OAuth token 经过本站的 `/__cf` 中转**：都不记录。部署代码的人本来就能读取 secret，没有扩大信任范围。
- **申请 477 项权限**：登录页会列出很长的授权清单，所有者要看清楚再同意。token 只在内存里，用完即弃，不写入项目。
- **没有长期 Cloudflare token**（决定 G）：站点里需要长期调 Cloudflare API 的功能，以后要另行登记初始化项或到控制台手动添加。决定 H 补上一个例外：初始化时申请一个只带「Account API Tokens Edit」的账户令牌，它不直接调用 API，只用来以后再申请令牌。
- **账户令牌的明文只出现一次**：Cloudflare 只在创建响应里返回令牌值。写入失败时页面丢弃明文并重新申请，不重试同一个值。
- **`github-rerun` 只能手动重跑**：GitHub 只允许在一定期限内重跑一次运行，超过期限要推新标签。
- **判断依赖 API**：发布工作流里查询项目失败时构建直接失败，不会误产出站点构建或初始化构建。
- **部署配置文件会覆盖变量**：见 S1 的约束。

## Migration Plan

1. 实现本变更；`plugin-system` 完成 11.3（撤销 `/__setup` 放行）和 10.10（发布工作流改用 `npm run build:pages`）。
2. 用户把 `onlinegal.cc.cd` 绑定为 Pages 项目的自定义域名（4.3）。
3. 发布一次：项目里没有 `ONLINE_GAL_SITE`，产出初始化构建。
4. 用户在 `https://onlinegal.cc.cd/` 完成初始化；页面触发第二次构建，产出站点构建。
5. 2026-11-23 决定 J：地址换成 `onlinegal.de5.net`，旧地址改为 301 跳转（S12），之后在新地址上重新初始化（S11、tasks 7.6）。

**回滚**：删除 `apps/setup/` 和 `scripts/build-pages.mjs`，构建命令改回 `npm run build:site`。项目里的 `ONLINE_GAL_SITE` 和 secret 留着没有影响，也可以在控制台删除。

### S11 — 重新初始化并申请账户令牌（用户决定 H，2026-11-23）

决定 D 仍然有效：不做通用的重置功能，页面不出现「重置」按钮。本节只给已经初始化的站点开一次重新初始化，用来写入新的初始化项。

**重新初始化命令** `npm run reinit:pages -- <project>`（`scripts/reinit-pages.mjs`）

- 只用已认证的 `cf` CLI，不另读 token。先 `cf pages project get <project> --json` 读出项目，确认有 Git 来源和 `production_branch`。
- PATCH 项目，把生产与预览环境的 `ONLINE_GAL_SITE` 设为 `null`，其他变量一律不动（Cloudflare 的 PATCH 合并 `env_vars`，删除要显式设 `null`，见 S1）。
- 再读一次项目，确认两处 `ONLINE_GAL_SITE` 都已不存在，才继续。读回仍在时以非零状态退出，不触发构建。
- 触发一次生产构建：`cf pages deployments create <project> --branch <production_branch>`，打印新部署 id。构建环境里没有 `ONLINE_GAL_SITE`，脚本按 S2 规则 3 产出初始化构建。
- 支持 `--dry-run`：只显示将要删除的键和将要触发的构建，不写入、不部署。
- MUST NOT 删除或打印任何 secret 的值。命令不接受 token 参数。

**CA 私钥**（初始化页 S5 第 6 步，写入 PATCH 之前，与账户令牌一起生成）

- 在浏览器里生成一对 ECDSA P-256 密钥（`crypto.subtle.generateKey`，`extractable: true`），导出私钥 JWK，只取 `kty`、`crv`、`x`、`y`、`d` 五个字段，`JSON.stringify` 后写入生产环境 secret `ONLINE_GAL_CA_PRIVATE_JWK`（`secret_text`）。`x`、`y`、`d` 必须都是 43 个字符的 base64url，否则 `functions/__ca/issue.ts` 会拒绝。
- 公钥不写入任何变量。页面用 `packages/plugin-system/src/attest.ts` 的 `microCaFingerprint`（RFC 7638，对规范化公钥 JWK 取 SHA-256，小写十六进制）算出指纹，显示给所有者并写入 `sessionStorage`，供以后核对 DNS 记录。私钥 JWK 与 `d` 不得显示、不得进浏览器存储、URL 或日志。
- 私钥只在浏览器里生成，不经过服务器。MUST NOT 把私钥发到除随后那一次 PATCH 以外的任何地址。
- 这一对密钥决定 K18 的信任根。换密钥会改变指纹，必须同步改 DNS（K15 的限制 1）。重新初始化会生成新的一对，旧的签名产物全部失效。

**账户令牌**（初始化页 S5 第 6 步，写入 PATCH 之前）

- 用刚登录的 OAuth access token 调 `POST /accounts/{accountId}/tokens`，申请一个**账户所有**的 API 令牌。MUST NOT 调 `POST /user/tokens`（那是用户令牌，决定 H 明确排除）。
- 请求体：
  - `name`：`online-gal-setup`。
  - `policies`：一条 `effect: "allow"`，`permission_groups` 只有一项，`resources` 为 `{ "com.cloudflare.api.account.<accountId>": "*" }`，只限已确认的这一个账户。
  - 权限组 id 不写死。先 `GET /accounts/{accountId}/tokens/permission_groups`，按返回项的 `name` 精确匹配 `Account API Tokens Write`，取它的 `id`。匹配不到时停下说明，不写入。
- 登录凭据的 `scopes` 缺少 `account_api_tokens:create` 时，停在第 2 步说明缺少的权限，不写入。`scopes` 为空数组时不拦截，靠这一步的报错。
- 响应里的 `value` 只在创建时返回一次。页面不显示它，不写入任何浏览器存储、URL 或日志，只放进随后 PATCH 的 `CLOUDFLARE_ACCOUNT_TOKEN`（生产环境 `secret_text`）。
- 创建成功但 PATCH 失败：丢弃账户令牌的明文和刚生成的私钥，重试时重新申请令牌、重新生成密钥，不重用旧值。留下的旧令牌由所有者到 Cloudflare 控制台的「Manage account → Account API tokens」删除；没写进项目的私钥本来就只在内存里，丢弃即消失。
- 该令牌不直接调用 Cloudflare API。它只带「Account API Tokens Edit」，以后新的初始化项需要令牌时用它再申请，不必再重新初始化。

**依据**（2026-11-23 核对 Cloudflare 文档）

- 创建账户令牌的接口是 `POST /accounts/{account_id}/tokens`，要求的权限是 `Account API Tokens Write`（控制台名称「Account → Account API Tokens → Edit」）。见 `https://developers.cloudflare.com/api/resources/accounts/subresources/tokens/methods/create/`。
- 账户令牌归账户所有，不随创建它的用户离开而失效；用户令牌代表个人。见 `https://developers.cloudflare.com/fundamentals/api/get-started/account-owned-tokens/`。
- `cf` CLI 的登录自 `@cloudflare/workers-auth` 0.10.0 起申请 `account_api_tokens:create`，本仓库的 `CF_DEFAULT_SCOPES` 已含该项，登录代码不用改。见 `https://github.com/cloudflare/workers-sdk/releases/tag/%40cloudflare/workers-auth%400.10.0`。

### S12 — 官方地址换成 `onlinegal.de5.net`（用户决定 J，2026-11-23）

- 新地址 `https://onlinegal.de5.net/`，是 Pages 项目 `online-gal` 的自定义域名（zone 顶点）。zone `onlinegal.de5.net` 在 Cloudflare 上，NS 为 `fred.ns.cloudflare.com`、`magali.ns.cloudflare.com`，父域 `de5.net` 已有它的 DS。2026-11-23 实测 cloudflare-dns.com 与 dns.google 两家都返回 `AD: true`（`plugin-system` design.md「10.0 验证结果」）。
- 换域名的原因：`cc.cd` 没有给 `onlinegal.cc.cd` 配 DNSSEC 委托（DS 查询无 Answer，`AD: false`），K16 要求的「至少一家 AD=true」在旧域名上永远满足不了。
- 旧地址 `onlinegal.cc.cd`：从 Pages 项目解绑，删除 zone 里指向 Pages 的解析（顶点指向 `*.pages.dev` 的 CNAME）。301 跳转要求请求仍然到达 Cloudflare 边缘，所以顶点换成一条占位的代理记录 `A @ 192.0.2.1`（proxied；192.0.2.1 是文档保留地址，不会被回源访问），再在 zone 上建一条 Single Redirect：
  - 表达式 `(http.host eq "onlinegal.cc.cd")`；
  - 目标 `concat("https://onlinegal.de5.net", http.request.uri.path)`，保留查询串；
  - 状态码 301。
  - zone 里指向 Pages 之外的记录（如有）不动，先列出再回报。旧域名不放 `_trust` TXT。
- 操作顺序（避免中断）：① 绑定新域名，等到域名状态 `active`、HTTPS 返回 200；② 在旧 zone 建跳转规则，确认 301；③ 从项目解绑旧域名；④ 删旧顶点 CNAME，加占位记录；⑤ 再确认 301。
- 代码默认值：`SITE_HOST` 的默认值从 `onlinegal.cc.cd` 改为 `onlinegal.de5.net`，共五处：`apps/setup/scripts/build-setup.mjs`（`setupConfig`）、`apps/setup/src/pages/index.astro`、`packages/plugin-system/scripts/build-site.mjs`（`__SITE_HOST__`）、`packages/plugin-system/scripts/sign-plugin.mjs`（`signPlugin` 的 `siteHost` 默认值）、`test/plugin-system-microca.test.mjs`。这一步必须在旧域名开始 301 之前推送：`sign-plugin.mjs` 用 `redirect: 'error'` 请求 `https://<SITE_HOST>/__ca/issue`，旧默认值遇到 301 会让签名失败。
- 信任记录跟着站点源走：内核按当前源的主机名查 `_trust.<主机名>`（`packages/plugin-system/src/kernel.ts` 的 `siteHost` 缺省取 `new URL(origin).hostname`），新地址查 `_trust.onlinegal.de5.net`。
- 浏览器里旧源的数据不迁移：`onlinegal.cc.cd` 上的 Service Worker、已安装插件与本地存储属于旧源，跳转后不再使用。决定 J 没有要求迁移。
- 初始化：重新初始化（tasks 7.6）在新地址上做。初始化页的站点管理员判定（A6）比对项目的 `domains[]`，解绑旧域名后只认新地址。
- 工具：只用已认证的 `cf` CLI（`openspec/config.yaml` 的规则）。`cf` 缺少某项操作（例如 Rulesets）的命令时停下回报，不改用 wrangler，也不手建 token。
