# Design: plugin-system

## Context

`scripting-permissions` 只给插件留了三条边界（API 版本、来源分级、不绕过授权）。用户先要求"所有功能都作为插件"，随后把插件系统定为**本地服务器**（原话："插件系统更像是一个 worker 拦截了请求以方便在本地提供更自由的页面和功能"）：

```
加载官方管理器：  浏览器 → 插件系统 → 服务器(只在安装/更新时) → 新的管理器 → 插件系统 → 浏览器
换成第三方管理器：浏览器 → 插件系统 → 第三方管理器 → 插件系统 → 浏览器
```

并明确：管理器仍由 Astro 构建、再打包成插件；插件系统只要 Node.js 能构建即可，**性能和稳定性优先**；官方插件由 Cloudflare Pages 分发，第三方插件来自 GitHub 仓库的标签。

已有约束：
- 引擎依赖跨源隔离（`SharedArrayBuffer`），`apps/app/astro.config.mjs` 的 `ISOLATION_HEADERS` 是 COOP `same-origin` + COEP `credentialless` + `nosniff`。插件系统应答的一切都必须带这三个头。
- 管理器是 Astro 静态多页站点（`output: 'static'`），构建产物约 24 个文件、3.1 MiB。
- Service Worker 里 `import()` 会抛 TypeError，`importScripts()` 只能在安装阶段取新 URL；module 形式的 Service Worker 在 Firefox 147 才支持。

## Goals / Non-Goals

**Goals**
- 页面注册一次插件系统，之后每次打开、刷新、深链都由本地应答；服务器只负责分发插件。
- 换管理器只改本地状态，不需要服务器参与。
- 插件包可校验：每个文件都有大小与 SHA-256；官方包由注册表钉住摘要。
- 任何一步失败都不破坏已安装的状态；最坏情况下有不依赖任何插件的恢复页。
- 管理器源码不改。

**Non-Goals**
- 非管理器插件的执行模型（在哪运行、钩子表、能调什么）。现在只能作为静态文件挂在 `/__plugins/<id>/`。
- 第三方管理器的权限分级。它与官方管理器同源运行，安装即信任（见风险）。
- 引擎拆成独立插件、插件管理界面（内核 API 已就绪）。
- 作者签名与发布者身份体系：发布者不保管长期私钥（microca 通道还隐藏身份；github 通道公开仓库路径与工作流名）；只有官方包有来源证明（K14–K17），第三方包仍只靠哈希与来源。
- 账号与站点管理：属于变更 `cf-account-admin`，它通过 K19 的登记接入内核。

## Decisions

### K1 — 插件系统是 Service Worker，不是每页加载的内核

Service Worker 在页面之前拿到导航请求，跨页面、跨刷新常驻，天然就是"本地服务器"。前一版设计让每个页面先启动一个内核再加载管理器，用户明确否定（"每页都要加载一次插件系统，这是不对的"）。

代价：浏览器禁用 Service Worker（部分隐私模式）时插件系统不可用。首次访问由站点服务器直接返回官方管理器的静态页，Service Worker 在页面 `load` 之后才注册，首屏不等内核（见 K9、K10）。

### K2 — 两个产物，内核尽量小

- **管理器**由 GitHub Actions 运行 `astro build` 生成 `apps/app/dist`，用 `apps/app/plugin.manifest.json` 打成官方插件制品。Pages 的站点阶段从发布制品装配，详见 K21。
- **插件系统**：`packages/plugin-system/src/sw.ts` 由 esbuild 打成单文件 classic script `/sw.js`（压缩后 31.4 KiB）。只依赖 `@online-gal/plugin-host` 的 manifest 校验，不打包任何 UI 框架。

`sw.ts` 只把 Service Worker 事件接到 `Kernel`；全部决策都在 `Kernel` 里，`Kernel` 通过 `KernelOptions` 注入 `caches`/`fetch`/时钟，因此同一份代码在 Node 测试里跑。classic 而不是 module：module Service Worker 在 Firefox 147 之前不可用。

### K3 — 内核从不执行插件代码

内核只**提供文件**，插件代码在页面里运行。这既是限制（Service Worker 里不能动态加载代码），也是稳定性选择：插件写错不会让内核崩溃，内核崩溃也就不会让站点打不开。因此第三方管理器能做的事和普通网页一样，不比官方管理器多。

### K4 — 只用 Cache Storage

- 每个插件的每一代文件放在独立的缓存 `og-plugin:<id>:<时间戳>-<随机>`，键为 `<origin>/__kernel/file/<path>`。
- 内核状态是 `og-kernel` 缓存里的一个 JSON（`STATE_FORMAT = 1`）：当前管理器、上次检查时间、已安装插件（manifest、来源、`plugin.json` 摘要、文件清单、缓存名、上一代）。
- 不用 IndexedDB：Cache Storage 原样保存 `Response`，应答时可直接流式返回；整代删除就是删一个缓存名；不需要额外的序列化层。
- 先写状态再切换内存中的状态：写入失败时旧状态继续生效。改变状态的操作经同一个队列逐个执行。
- 状态格式不认识时从空状态开始，并在导航时重新安装默认管理器，而不是崩溃。

### K5 — 插件包：`plugin.json` + 逐文件哈希

```json
{ "format": 1,
  "manifest": { "id": "online-gal.manager", "version": "0.1.0+3fa2c1d07b9e", "apiVersion": 1,
                "provenance": "official", "role": "manager", "name": "..." },
  "files": [ { "path": "index.html", "size": 1234, "sha256": "<64 hex>" } ] }
```

- manifest 复用 `plugin-host` 的 `parseManifest`/`assertApiCompatible`（新增 `role: "manager"`）。
- 路径必须是安全相对路径，且只在文件清单内查找，所以没有 URL 能逃出清单。
- 上限：5000 个文件、单文件 256 MiB、合计 1 GiB。
- 管理器不能包含 `sw.js`、`__kernel/`、`__plugins/`、`registry/`：这些 URL 永远不会交给管理器，包含它们只会被静默遮蔽，所以直接拒绝。
- 文件条目可带 `lazy: true`：安装时不下载，第一次被请求时再下载（见 K13）。格式版本仍是 1：旧内核忽略这个标记并下载全部文件，新内核读没有标记的旧包就是全部立即下载。至少要有一个非 lazy 文件，否则完整性检查与修复没有可检查的对象。
- 每个文件在写入缓存之前都先核对大小与哈希（非 lazy 的在安装时，lazy 的在第一次请求时）；安装时任一不符就丢弃整代。

### K6 — 来源决定信任

| 来源 | 位置 | 要求 |
|---|---|---|
| `official:<id>` | `/registry/index.json` → `/registry/p/<id>/<内容哈希>/` | `provenance: official`；id、版本与注册表一致；`plugin.json` 摘要 = 注册表 `digest` |
| `github:<owner>/<repo>@<tag>` | `raw.githubusercontent.com/<owner>/<repo>/refs/tags/<tag>/` | `provenance: third-party`；id 不得以 `online-gal.` 开头 |

- GitHub 只接受标签：同一个标签总是同一份文件，安装两次结果相同。
- 用 `raw.githubusercontent.com` 而不是 GitHub API：它允许跨源 GET，也不受 API 每小时 60 次的未认证限制。
- 第三方作者在仓库根目录运行 `pack-plugin.mjs` 生成 `plugin.json` 并提交，打标签即发布。打包工具用的是内核同一个 `parsePackage`，打包成功的包一定能被内核接受。
- 已安装的 id 不能被另一个来源替换（`source-conflict`）；同一仓库换标签算同一来源，就是更新。

### K7 — 原子安装与"上一代"

- 新一代下载到新缓存；全部校验通过、状态写入成功之后才切换。之前的任何失败都删除新缓存，旧版本照常应答。
- 同一个包（摘要相同且本地文件完好）不重新下载。
- 更新时保留上一代：更新前加载的页面还会请求它引用的旧文件（Astro 产物的文件名带哈希），内核在当前版本找不到时会去上一代找。没有页面打开时清掉上一代；同时最多保留一代。
- 被中断的安装留下的无主缓存，一小时后清理（给仍在进行的安装留余地）。
- 管理器文件被浏览器清除时，导航会从原来源重新下载（`repair`）。

### K8 — 更新策略

- 官方：导航时在后台检查注册表（`event.waitUntil`），两次间隔至少 6 小时；只更新摘要变化了的已安装官方插件。不阻塞导航，失败只记日志。首次安装默认管理器时刚读过注册表，内核顺带记一次检查时间，所以首次访问之后的第一次导航不会立刻再取一遍注册表。
- 第三方：只在用户安装另一个标签时改变。标签本身就是用户选定的版本。
- 内核自身：浏览器对 `/sw.js` 的更新检查（注册时 `updateViaCache: 'none'`，检查更新绕过 HTTP 缓存，不要求 `no-cache`），新内核 `skipWaiting` + `clients.claim` 立即接管。

### K9 — 引导、内核页与恢复

- 站点根直接放官方管理器的静态页，没有引导页。站点没有顶层 `404.html`：Cloudflare Pages 于是对任何未知路径都返回根 `index.html`，所以首次打开未知路径得到的是管理器首页。内核接管之后则不同：内核对没有 `404.html` 的管理器返回纯文本 404。这个差异目前没有处理（`tasks.md` 9.9）。
- 首次访问由 Pages 直接返回官方管理器页面。K21 切换后，根目录 HTML 在 `load` 之后再让浏览器绘制首屏，随后注册 `/sw.js`，带 `updateViaCache: 'none'`。Service Worker 已激活但绕过了本次加载时（例如 Shift+F5），`reload` 一次，用 `sessionStorage` 记录时间，10 秒内最多一次。注册脚本只注入站点根 HTML。
- `/__kernel/`：内核自带的页面（安装、切换、卸载、检查更新、重置），不依赖任何插件，管理器坏了也能用。
- 恢复页：内核应答导航失败时显示错误码，提供重试和进入 `/__kernel/`。
- 重置：删除全部插件与状态，页面注销 Service Worker 后回到站点根的静态页面（它在 `load` 之后会重新注册内核）。

### K10 — 性能

- **刷新与再次访问**：页面和所有资源都从本地缓存流式返回，没有网络往返。端到端实测：刷新时服务器收到 0 个请求，页面加载完成约 306 ms（本地 Chromium，包含渲染）。
- **路由是同步的**：`routeRequest` 只看 URL，决定是否 `respondWith`，不读存储。
- **热路径**：状态只在 Service Worker 启动时读一次，之后在内存里；每代文件清单转成 `Set` 后按代缓存（`WeakMap`）；打开的缓存对象复用。
- **不需要 navigation preload**：内核应答的请求从不访问网络，预加载只会浪费一次请求。
- **首次访问**：站点服务器直接返回官方管理器的静态页，Service Worker 在页面 `load` 之后才注册并在后台安装，首屏不等内核。安装下载 14 个文件约 0.1 MiB，另外 10 个（引擎与 sourcemap，约 3 MiB）是 lazy，等第一次被请求时再下载（见 K13）。实测在 80 ms 服务器延迟下，深链首次访问 FCP/LCP 约 430–540 ms（改回部分 lazy 之后三次：427、470、536 ms；此前全 lazy 配置一次：452 ms）；上一版先装内核和管理器再显示，约 900–1000 ms。首次访问的服务器请求数是 22 个（后台安装也下载首屏资源），不能和上一版的 11 个直接比较。样本很少，只在本机无头 Chromium 上用模拟延迟测过。第一次用到引擎时仍要下载它。这是"本地服务器"模式的固有成本，换来之后每次打开都不依赖网络。
- **分发**：注册表里的插件路径带内容哈希，以 `immutable` 长缓存发布；`/registry/index.json` 是 `no-cache`；`/sw.js` 不要求 `no-cache`，内核更新检查靠注册时的 `updateViaCache: 'none'`。

### K11 — 内核 API

`/__kernel/api/`：GET `state`、`registry`；POST `install { source, activate? }`、`uninstall { id }`、`activate { id }`、`update`、`reset`。

- POST 必须是 `application/json` 的对象；以导航方式到达的请求（跨站表单 POST）一律拒绝。跨站页面的 `fetch` 本来就到不了本源的 Service Worker。
- 响应统一为 `{ ok: true, ... }` 或 `{ ok: false, error: { code, message } }`，HTTP 状态由错误码决定（例如 `source-invalid` 400、`integrity-failed` 502、`manager-in-use` 409）。
- 内核错误码（`KernelErrorCode`）：`source-invalid`、`package-invalid`、`registry-invalid`、`not-in-registry`、`fetch-failed`、`integrity-failed`、`too-large`、`provenance-mismatch`、`reserved-id`、`source-conflict`、`not-installed`、`not-a-manager`、`manager-in-use`、`request-invalid`、`storage-missing`；manifest 问题沿用 `manifest-invalid`、`api-version-mismatch`。
- 状态变化通过 `postMessage({ source: 'online-gal-kernel', type, ... })` 通知所有打开的页面：`plugin-installed`、`plugin-removed`、`manager-changed`。

### K12 — `plugin-host` 保留为规则层

`packages/plugin-host` 是此前实现的插件规则（D1–D8 仍然成立），插件系统只复用它的 manifest 校验：

- D1 独立包，只依赖 `engine-api`。
- D2 manifest 是唯一入口：严格字段、精确 API 版本、校验先于任何代码。
- D3 来源只影响默认授权与上限：第三方默认零权限，官方默认 `vfs.read` + `ui.notify`，且只在 manifest 声明过时生效。
- D4 每次调用都检查授权；撤销立即生效；卸载清空。
- D5 路径在规范化之前检查。
- D6 网络只经宿主注入的代理，没有默认实现。
- D7 预算是协作式的（时间 + 经宿主分配的字节），超限即终止该插件。
- D8 单插件失败不扩散。

`boot`/服务图（`provides`/`requires`）/特权能力 `plugins.manage` 也保留，但**现在没有调用方**：它们是为非管理器插件在页面里运行准备的，要等待补齐项 (a) 定下执行模型后接线。已删除的部分：`ui.mount` 与页面根（管理器现在就是站点本身，不需要"挂载"）。

此前的 D9（内核优先，每页启动内核）、D13 中与页面根相关的部分、D14（构建期把管理器做成内联插件 chunk、`modulepreload`）**被 K1–K10 取代**。

### K13 — 按需加载（lazy 文件）

插件包里的文件可以标成 `lazy`：安装时不下载，第一次被请求时再下载。目的是让安装只下载页面与样式等马上要用的文件，体积大而不常用的引擎和 sourcemap 等到用到时再下载。

- **打包**：`pack-plugin.mjs --lazy <glob>`（可重复；`**` 跨目录，`*` 不跨目录；`!` 开头的 glob 把匹配的文件留作立即下载，例如 `--lazy '**' --lazy '!index.html'`）。lazy 标记计入内容哈希，所以只改 lazy 配置也会得到新的目录，`immutable` 缓存不会对不上。官方管理器标记 `engines/**` 与 `**/*.map`：引擎（wasm、glue、worker、源码包）要到游戏启动才用，sourcemap 要打开开发者工具才用。管理器页面与资源保持立即下载：站点根的静态页让首屏不依赖安装，而 Service Worker 接管后的刷新和离线打开需要它们已经在本地缓存里。（曾试过除 `index.html` 外全部 lazy：站点根直接提供静态页之后没有收益，实测反而让刷新时重新下载了 4 个文件，所以改回。）
- **安装**：只下载并校验非 lazy 文件。安装记录保存 lazy 清单（路径、大小、哈希）和下载根 `base`。实测官方管理器 24 个文件共 3.1 MiB，安装时下载 14 个、约 0.1 MiB，另外 10 个是 lazy。
- **应答**：缓存里没有的 lazy 文件，在被请求时按 `base` 下载，大小与哈希通过后才写入并应答；同一个文件的并发请求共用一次下载。校验失败返回 503，不写入任何内容，服务器修好后下一次请求会重新下载。
- **管理器不用改**：引擎的 worker、glue、wasm 都是同源的普通路径，请求自然经过内核，所以按需加载对管理器代码不可见。
- **完整性只看非 lazy 文件**：没取过的 lazy 文件不算丢失，不会触发整包修复；修复也只重新下载非 lazy 文件。
- **上一代**：更新后保留的上一代带着自己的 lazy 清单和 `base`，更新前打开的页面仍能取到它还没来得及取的文件。
- **服务器重新部署后旧目录消失**：lazy 文件下载以 `fetch-failed` 失败时，内核立即检查官方注册表（不等 6 小时间隔），有新版本就安装并改从新版本应答；没有就如实返回 503，不会重试循环。只对官方插件这样做：第三方标签是固定的，不会被替换。
- **代价**：第一次用到引擎时要等它下载；离线时没取过的 lazy 文件取不到（503）。是否在空闲时预取引擎，留给用户决定（`tasks.md` 9.7）。

### K14 — 包拆成三个文件（取代 K5 的封装与 K6 的 digest 钉住）

- `mf-manifest.json`：原 `plugin.json` 的正文 `{ format: 1, manifest, files }`；路径、上限、lazy、保留路径仍按 K5。
- `plugin.json`：只做指向，`{ "format": 2, "manifest": "mf-manifest.json", "bundle": "plugin.bundle.json" }`，`bundle` 可省略。不含摘要、文件清单与 manifest，所以改它（换路径、换 CDN）不动 mf-manifest.json，改 mf-manifest.json 也不必动它。指向只能是同目录或子目录的相对路径。
- `plugin.bundle.json`：来源证明（K15）。完整性与来源的证据在被指向的文件里，不在 plugin.json 里：篡改 plugin.json 只能让指向落空或指向别的文件，而证明签的是 mf-manifest.json 的摘要，对不上就被拒绝。
- 官方注册表条目的 `digest` 改为 mf-manifest.json 的 SHA-256。校验顺序：plugin.json → mf-manifest.json（核对 digest）→ bundle（K16）→ 逐文件哈希；任一步失败整包拒绝。
- 官方包的目录哈希（`p/<id>/<hash>/`）MUST 覆盖整个目录的内容，不只是文件清单。`plugin.json`、`mf-manifest.json` 和存在时的 `plugin.bundle.json` 的字节 MUST 进入这个哈希。同一个 URL 永远只对应一份内容。只改 `plugin.json`（例如 format 1 升到 format 2、补上 `bundle` 指针）也 MUST 换一个新目录，不能覆盖旧 URL。
- 内核取官方包的 `plugin.json`、`mf-manifest.json` 与 bundle 时 MUST 带 `cache: 'no-store'`，不能用浏览器的 HTTP 缓存副本。这是对上一条的兜底：旧部署已经把 format 1 的 `plugin.json` 发布在带 `Cache-Control: public, max-age=31536000, immutable` 的 URL 上，那些浏览器会一年内都拿到旧文件。
- 兼容：仍接受 format 1 的 plugin.json（内嵌，无证明），只用于 GitHub 来源；官方来源必须 format 2。

### K15 — 发布端：两条签发通道，发布者都不保管长期私钥

`plugin.bundle.json` 带 `channel` 字段，取值 `github` 或 `microca`，决定 K16 走哪个验证分支。两个分支各自算出一个承诺哈希，交给同一份 DNS 白名单比对；一个 mf-manifest.json 只需要其中一条通道的证明。

- 2026-11-23 用户决定，日常发布继续使用 Cloudflare Pages 的 Git 集成。官方包在 Pages 生产构建里走 `microca` 通道签名。K21 新增的 GitHub Actions 只负责生成管理器制品。此工作流和 10.10 预留的 `github` 签名通道分别实施。Pages 构建环境没有 GitHub OIDC 身份。此前任务 #68 的两套构建产出过不同的 manifest 字节。K21 在 Actions 写定文件清单与 manifest 版本后，由 Pages 直接对校验过的原始 manifest 签名。
- Pages 构建的身份：生产环境 secret `ONLINE_GAL_CA_BUILD_TOKEN`（随机 32 字节，b64url）。构建拿它向当前线上站点的 `/__ca` 换凭证，CA 把它映射到固定的 `anon_id` `cf-pages`；DNS 白名单加这一项的 `microca` 承诺哈希。令牌泄露只需换令牌，CA 私钥与 DNS 不变。预览环境不配置这个令牌，预览部署不签名。
- 构建行为：站点初始化的方案 A 在首次生产部署之前写入令牌与 `ONLINE_GAL_CA_BOOTSTRAP=1`。构建按 `site-setup` S13 判定首构建，跳过签名并保留签发路由。引导标记清除后，启用了签名的生产构建须取得凭证、写入 Rekor 并验证产物，任一步失败即中止。预览和未启用签名的项目沿用无令牌时的构建行为。
- 上线顺序由 `site-setup` S13 编排。第一次生产站点部署使 `/__ca` 可用。第二次部署签名并在生产域名生效。线上包按候选承诺验签通过后才发布信任 TXT，再从真实 Service Worker 验证 DoH 和线上包（K16）。
- 按 K14，版本后缀沿用文件清单哈希。先写定 manifest 版本并签名，随后把最终封套、manifest、文件清单和产出的 `plugin.bundle.json` 字节纳入目录哈希，最后生成注册表路径。相同 manifest 签出不同 bundle 时须发布到不同目录。内核取 bundle 仍带 `cache: 'no-store'`。

**通道一：`github`（可选，GitHub Actions 发布；2026-11-23 起不是日常路径）**

1. 启用这条通道时，官方站点由发布工作流（`.github/workflows/release.yml`，打标签触发）构建和发布：构建站点（`npm run build:pages`，2026-11-22 因 `site-setup` 用户决定 F 改：它先判断阶段，未初始化时产出初始化构建，此时没有注册表，跳过来源证明；已初始化时调用 `npm run build:site`）→ 对注册表里每个官方包的 mf-manifest.json 用 `actions/attest-build-provenance` 各生成一份 attestation（单 subject，摘要就是它的 SHA-256；需要 `id-token: write` 与 `attestations: write`）→ 把 bundle 写成同目录的 `plugin.bundle.json` → `cf pages deploy dist/site` 连同 `functions/` 一起上传。启用前提：`cf` CLI 能上传目录（2026-11-23 的 `cf pages deploy` 不上传），并且 Git 集成构建要停掉，否则 Pages 会重新构建出不同字节的包。现在这两条都不满足，所以这条通道暂不启用。仓库必须公开，这样走公共 Sigstore；私有仓库走 GitHub 自己的 Sigstore 实例，信任根不同，本变更不支持。
2. GitHub 产出的 Sigstore bundle 原样放进 `plugin.bundle.json` 的 `sigstore` 字段，不改写。
3. 暴露的元数据是证书里公开的仓库路径与工作流文件名，不含个人邮箱、登录名与本机信息。这条通道不匿名：DNS 里的哈希只是白名单，遮不住 bundle 里本来就公开的仓库路径。

**通道二：`microca`（日常发布：Cloudflare Pages 生产构建；另用于应急、本地、GitLab、GitHub 账号异常；匿名）**

本地与 CI 流程相同：

1. 进程内存生成临时 ECDSA P-256 密钥对，对 mf-manifest.json 的 SHA-256 签名，随后销毁私钥。
2. 把平台身份（GitHub Actions OIDC、本地 Google / Microsoft 登录，或 Pages 构建持有的 `ONLINE_GAL_CA_BUILD_TOKEN`）与临时公钥交给匿名微型 CA（K18，随站点部署的 Pages Function）；CA 核验后签发 10 分钟有效的 JWS（ES256）`{ anon_id, eph_pk, iat, exp }`，不含邮箱、账号、仓库、IP。
3. 把（摘要、签名、临时公钥）提交公共 Rekor（hashedrekord），取回条目、SET 与 inclusion proof。
4. 写出 `plugin.bundle.json`：`v`、`channel`（固定为 `microca`）、`ca`（微型 CA 公钥 JWK，内联）、`cred`（JWS）、`sig`、`rekor`（body、integratedTime、logIndex、logId、set、proof）。

`ca` 内联在 bundle 里，但它自己不被信任：只有被 K16 的 DNS 哈希认可才算数。`github` 通道没有内联信任根：它的密码学根是内置在内核里的 Sigstore 公共信任根，谁算官方仍由 DNS 白名单决定。

### K16 — 验证：DNS TXT 白名单决定谁算官方（两条通道共用）

- `_trust.<域名>` 的 TXT：`v=1; salt=<b64url>; hashes=<项>,<项>`，两条通道共用一个 salt。每项是下面两种承诺哈希之一的十六进制，可带时间窗 `<hex>:<after>:<before>`（unix 秒，可空）；承诺哈希：`github` 通道是 `SHA-256("github|" + issuer + "|" + repoId + "|" + workflowUri + "|" + salt)`，`microca` 通道是 `SHA-256("microca|" + ca_fingerprint + "|" + anon_id + "|" + salt)`；`ca_fingerprint` 是 bundle 内 CA 公钥 JWK 规范化后的 SHA-256。前缀防止两条通道的哈希互相冒充；`anon_id` 与 `workflowUri` 都不得含 `|`，`repoId` 必须是十进制数字串，不符则拒绝。
- `github` 分支顺序：① 同样先查 DoH 取白名单；② 验 Sigstore bundle：Fulcio 证书链到内置的 Sigstore 公共信任根、证书在签发时有效、Rekor 条目的 `integratedTime` 落在证书有效期内、DSSE 签名有效、in-toto statement 的 subject 摘要等于 mf-manifest.json 的 SHA-256；③ 从证书取 `issuer`（必须是 `https://token.actions.githubusercontent.com`）、仓库 ID（Fulcio 的 Source Repository Identifier 扩展，据我所知 OID 是 1.3.6.1.4.1.57264.1.15，待 10.0 确认）与 SAN 里的工作流 URI，**去掉 `@<ref>` 后缀**得到 `workflowUri`（形如 `https://github.com/<owner>/<repo>/.github/workflows/<文件>`），否则每换一个标签哈希就变，DNS 要跟着改；另要求证书里的 ref 是 `refs/tags/*`，只认打标签的发布；④ 算 `github` 承诺哈希，必须在白名单内，时间窗按 Rekor `integratedTime` 判断。
- `microca` 分支顺序：① 并行查 DoH（cloudflare-dns.com、dns.google），至少一家 AD=true，多家作答时内容必须一致；② 用 `ca` 公钥验 `cred` 的 JWS，且 `exp - iat` 不超过 600 秒；③ 算 `microca` 承诺哈希，必须在白名单内；④ 验 Rekor：body 中的摘要、签名、公钥与实际一致，SET 用内置的 Rekor 公钥验（DER 签名转 raw），inclusion proof 按 RFC 6962 重算根，`integratedTime` 落在 `[iat, exp]` 内且满足该项时间窗；⑤ 用 `eph_pk` 验 `sig`。
- `microca` 分支全部用 Web Crypto，不引入 SDK。Rekor 公钥与 Sigstore 公共信任根内置在 `sw.js`，轮换要发新内核。
- `github` 分支的验证器是未决点：`@sigstore/verify` 据我所知依赖 Node 的 `crypto`，不能直接进 Service Worker；备选是浏览器实现（如 sigstore-browser）或自写只覆盖 GitHub attestation 形态（P-256 叶证书、固定 Fulcio 链）的最小验证器。选哪个、`sw.js`（现约 33 KiB）会涨多少，由 tasks 10.0 的结果决定；在那之前 `github` 分支不实现。
- 只在安装与后台更新时验证（首屏之后），不进请求热路径；结果写入状态（匿名哈希、integratedTime、logIndex、DNS 检查时间）。

### K17 — 失败语义、范围与已知限制

- 官方来源（取得了 DNS 记录）：无 bundle 或验证失败 → `attestation-invalid`，不降级，也不替换已安装的旧版本。
- 受限模式（现取 DoH 全部失败且没有未过期的缓存）：内核只安装网站派发的插件，即官方注册表里随站点一起构建的包；这样的安装不验证 bundle，信任依据是本站 TLS 与注册表 digest，状态标 `unverified`，在 `/__kernel/` 与 `state` API 可见。`github:` 来源在受限模式下一律以 `attestation-unavailable`（可重试）拒绝。DoH 恢复后，后台检查补验已装的 `unverified` 官方插件：通过标 `verified`，失败标 `revoked`。DNS 记录的缓存有效期取 DoH 返回的 TTL，上限 1 小时。
- GitHub 来源：bundle 可选；存在但无效 → 拒绝；有效也不改变信任，信任仍由来源决定（K6），保留 id 规则不变。
- 撤销：后台检查重取 DNS，已装官方插件的哈希不在白名单或落入排除窗口 → 状态标 `revoked`，在 `/__kernel/` 与 `state` API 可见，不再接受该 bundle。已激活的官方管理器如何降级依赖执行模型（tasks 9.1），本变更不做。
- 双轨执行（通过 → 主线程，未通过 → Worker 沙箱 + XSS 网关）不在本变更：内核从不执行插件代码（K3），这里只产出「已验证 / 未验证 / 已撤销」标记，供 9.1 使用。
- 限制：(1) 微型 CA 持有一把长期私钥（Pages 项目的加密环境变量），「零私钥」只对发布者成立；换 CA 密钥会改变指纹，必须同步改 DNS。(2) AD 位是解析器的声明，浏览器无法独立验证，信任落在解析器与域名注册商上。(3) CA 公钥内联使 DNS 成为唯一信任根：能改 TXT 的人能授权任何 CA。(4) DoH 在部分网络被屏蔽：内核进入受限模式，官方首装不会失败，但装上的官方包是 `unverified`；受限模式下信任退回本站 TLS 与注册表 digest。(5) 只实现 Rekor v1 的 SET 与 inclusion proof，v2 的 checkpoint 另议。(6) `github` 通道不匿名，bundle 里公开仓库路径与工作流名。(7) `github` 哈希已并入仓库 ID（用户已决定）：仓库被删除后由他人重名注册，仓库 ID 不同，同名工作流签出的 attestation 不会命中同一个哈希。10.0 必须确认证书里有该扩展；没有就回报用户，不静默回退到不含仓库 ID 的哈希。(8) 日常的 `microca` 通道：任何能推 `main` 的人都能签出官方包，因为 Pages 生产构建会执行仓库里的任意代码，也读得到构建令牌。所以 `main` 的推送权限就是官方发布的安全边界。仓库公开后给 `main` 开分支保护，禁止强制推送和删除，不要求 PR（用户 2026-11-23 决定，tasks 7.8）；公开之前没有保护，靠只有用户本人有推送权限。`github` 通道启用后：任何能推 `refs/tags/*` 并触发该工作流的人都能签出官方包，它的边界是标签保护与工作流文件的改动权限。(9) 受限模式下官方包没有来源证明，信任退回本站 TLS 与注册表 digest。网络层攻击者能阻断 DoH、把用户推进受限模式，但这只能让用户装到本站原样派发的包，拿不到比站点本身更多的东西；站点或 Pages 部署被攻破、攻击者发无 bundle 的包时，DoH 可达的用户会拒绝，DoH 被屏蔽的用户会装上并标 `unverified`。(10) 内核（`sw.js`）与注册表来自同一次 Pages 部署，能改部署的人可以直接换掉 `sw.js` 跳过全部验证；来源证明防的是注册表内容被单独替换、非官方发布者冒充官方，并给执行模型（9.1）提供「官方」身份依据，防不了整站被换。(11) 微型 CA 的私钥与站点同在一个 Pages 项目，能读该项目加密环境变量的人既能签发匿名凭证也能改站点；`github` 通道不依赖这把密钥，但两条通道都依赖同一个 Cloudflare 账号。Pages 构建令牌也在同一个项目的生产环境里，能读它的人同样能签出 `cf-pages` 身份的凭证。

### K18 — 微型 CA 随站点以 Pages Functions 部署

- 源码在仓库的 `functions/__ca/`，路由 `/__ca/*`；随 Pages 的 Git 集成构建一起上线（2026-11-23 用户决定，见 K15 开头；10.8 实现时确认初始化阶段的构建不带 `/__ca`，或者带上也无害），没有单独的 Worker 项目。`build-site.mjs` 生成 `_routes.json`，只让 `/__ca/*` 调用 Function，静态资源不经过它。目录名在 Pages 路由里是否有限制，实现时用 `cf dev` 实测。
- 内核对 `/__ca/**` 与 `/sw.js`、`/registry/**` 一样放行到服务器：发布端的本地登录会在浏览器里打开该路径，不放行就会被当前管理器的 404 接住。管理器包的保留路径同步加上 `__ca/**`，理由同 K5。
- `_headers` 不作用于 Function 的响应，CA 的响应自己带需要的头；发布端是脚本与登录页，不需要跨源隔离头。
- CA 私钥与「允许的平台身份 → `anon_id`」映射放在 Pages 项目的加密环境变量，不进仓库；公钥 JWK 由发布端写进 bundle。CA 只核验 GitHub Actions / Google / Microsoft 的 OIDC 令牌，或 Pages 构建令牌（`ONLINE_GAL_CA_BUILD_TOKEN`，常量时间比较，映射到 `anon_id` `cf-pages`），然后签发 K15 的 10 分钟 JWS，不保存令牌，也不在日志里记录平台身份。
- 域名为 `onlinegal.de5.net`（2026-11-23 用户决定 J，见 `site-setup` design.md S12），TXT 在 `_trust.onlinegal.de5.net`；旧域名 `onlinegal.cc.cd` 没有 DNSSEC，只做 301 跳转，不放 TXT。
- 需要用户提供：允许的 OIDC 主体、Cloudflare API token（写入 GitHub 仓库 secrets，只在启用 `github` 通道时需要）。
- 2026-11-22 补充：用户决定凡是需要初始化的都在站点初始化时完成。CA 私钥与 OIDC 映射的写入入口是变更 `site-setup` 的初始化项清单（其 design.md S7），不另做配置入口。见 tasks.md 11.2。

#### 10.0 验证结果

- **公共 Rekor v1：通过。** 向 `POST https://rekor.sigstore.dev/api/v1/log/entries` 发送 `hashedrekord` `0.0.1`，签名为 ECDSA P-256 DER，`publicKey.content` 为 base64 包裹的裸 SPKI PEM 公钥（不含证书）。实测 HTTP 201，返回 `verification.signedEntryTimestamp` 与 `verification.inclusionProof`（含 `checkpoint`、`hashes`、`logIndex`、`rootHash`、`treeSize`）。同一回执的顶层 `logIndex` 为 `4047830186`、proof `logIndex` 为 `3925925924`；前者用于 SET，后者用于该树的 Merkle 证明，不能混用。
- **DoH 从浏览器以 CORS 取 TXT + AD：通过（2026-11-23 实测）。** 经本机代理实测（直连 `cloudflare-dns.com:443` 与 `dns.google:443` 15 秒超时，不是拒绝）：
  - `https://cloudflare-dns.com/dns-query` 与 `https://dns.google/resolve`，请求带 `accept: application/dns-json` 与 `Origin: https://onlinegal.cc.cd`。两家都返回 HTTP 200，响应头 `Access-Control-Allow-Origin: *`，JSON 都带布尔 `AD` 位。从浏览器跨源取可用。
  - `onlinegal.cc.cd` 取不到 AD：`_trust.onlinegal.cc.cd` 的 TXT 查询两家都是 `Status: 3`、`AD: false`；父域 `cc.cd` 的 NS 查询也是 `AD: false`；`onlinegal.cc.cd` 的 DS 查询 `Status: 0` 但没有 `Answer`。注册商没有给这个域配 DNSSEC 委托，K16 要求的「至少一家 AD=true」在这个域上永远满足不了。
  - `onlinegal.de5.net` 有完整 DNSSEC：`de5.net` 的 NS 与 DS 查询、`onlinegal.de5.net` 的 NS 与 DS 查询，两家都返回 `AD: true` 且带 RRSIG。DS 为 `2371 13 2 97b89f63223f3831069cee79d59c3f111350311b949607fb0eb8aa62b17cbaec`。该域目前没有 A 记录（SOA 权威应答，`AD: true`）。
  - 还没做的：从真实 Service Worker 里发同样的请求。CORS 头已证实放行任意来源，这一步只差浏览器内复测。
- **GitHub 通道：暂缓（10.10 可选）。** 当前仓库不公开；浏览器验证器、体积及真实产物的证书扩展和输出格式不作为本轮 Pages 签名的验收项。

### K19 — 其他变更通过登记扩展内核

- 用户决定把管理系统（Cloudflare 账号、`/__cf` 中转、浏览器版 cf、`/admin`）拆成变更 `cf-account-admin`，由单独的对话处理。本变更只负责插件系统，所以 spec 不写具体的扩展，只规定登记规则。
- 一项登记是一个顶层前缀 `/__<name>`，可选带三样东西：内核模块（接管该前缀下任何方法的请求）、内核 API 名字空间 `/__kernel/api/<name>/**`、服务器 Function `functions/__<name>/`。登记的前缀同时成为管理器包的保留路径（理由同 K5）和站点构建的保留名；有 Function 时进入 `_routes.json`。
- 与放行不同：`/sw.js`、`/registry/**`、`/__ca/**` 是插件系统自己的放行路径，内核不应答；登记前缀由内核模块应答，调用方由模块自己核对。内核响应的 COOP、COEP、nosniff 头对模块同样适用。
- ~~只有 Function、没有内核模块的登记~~（2026-11-22 加入，同日撤回）：`site-setup` 改为两阶段构建，`/__setup` 只存在于不含插件系统的初始化构建，站点构建里不再有它。撤销见 tasks.md 11.3。
- 登记通过 `packages/plugin-system/src/format.ts` 的保留路径、`packages/plugin-system/src/kernel.ts` 的 `routeRequest`、`packages/plugin-system/scripts/build-site.mjs` 的 `RESERVED` 接入。只有包含服务器 Function 的登记需要更新 `_routes.json` 的服务器路由。
- 当前登记：
  - `/__cf` → 变更 `cf-account-admin`（capability `cloudflare-account`），内核模块加 Function。内核保留逻辑前缀，生产 `_routes.json` 仅启用账户派生子树，见该变更 A12。它在共用文件里占用的代码块见该变更 design.md A11。
  - `/__webdav` → 变更 `integrate-webdav-source`（capability `webdav-remote-source`，待实现），通过现有浏览器 `/sw.js` 的内核模块处理。
  - 插件系统的改动不要顺手改这些块。

### K20 — 归属与代码边界

- 属于本变更：
  - `packages/plugin-system/**`（`src/account/**` 和 `cf-account-admin` design.md A11 列出的代码块除外）
  - `packages/plugin-host/**`
  - `apps/app/plugin.manifest.json`
  - `functions/__ca/`、`.github/workflows/build-manager-plugin.yml`（K21）、`.github/workflows/release.yml`（可选，10.10）
  - 根 `package.json` 的 `build:site`，`tsconfig.packages.json` 里 `plugin-host` 与 `plugin-system` 两条 reference
- 提交状态：format 1 的内核与管理器外壳已在 `e1340d2` 提交。format 2 与来源证明的改动还没有提交，包括新文件 `src/attest.ts`、`src/trust-dns.ts`，以及 `format.ts`、`kernel.ts`、`errors.ts`、`index.ts`、`sw.ts`、`scripts/pack-plugin.mjs`、`scripts/build-site.mjs` 的修改。其中后五个文件也含 `cf-account-admin` 的代码块，提交时按 A11 分开。

### K21 — Actions 发布管理器插件，Pages 装配并保持首访顺序

**发布**

- 从明确的源码 SHA 在 GitHub Actions 构建 `apps/app`。复用现有锁文件、依赖构建、引擎制品输入和管理器 manifest。用现有打包规则生成 format 2，保持 `online-gal.manager`、`role: "manager"`、官方来源声明、内容版本和 lazy 文件标记。发布的 GitHub Release 资产包含 `plugin.json`、最终版 `mf-manifest.json` 和已列出的文件。`mf-manifest.json` 在发布端写定；生产站点的微型 CA bundle 由 Pages 生成。
- 工作流经明确的发布触发从受信任的分支或标签取源码。先验证制品，后发布并读回校验。发布记录包含源码 SHA、release tag、asset 名、归档 SHA-256、插件版本，另记录引用的引擎制品标识以便复现。发布任务持有 `contents: write`，普通检查任务使用只读权限。重跑时复用同一标识的相同字节，遇到不一致则失败。

**Pages 装配**

- Git 中的制品引用文件锁定 release tag、asset 名、归档 SHA-256 与源码 SHA。发布成功且校验通过后再更新该引用，Git 集成对引用变更触发部署。后续内核或部署配置的变更可复用相同制品。S13 的首次未签名站点部署和后续签名站点部署使用同一制品引用。`setup` 阶段只构建初始化页，不下载管理器制品。生产切换前先发布当前可用管理器的制品，在隔离项目核验完整链路，然后提交初始引用及装配入口。保留可用旧制品，回滚时恢复上一兼容引用并重新构建。
- 官方仓库当前为私有。使用 GitHub Release API 下载选定的资产。Pages 设置单仓库、仅 Contents 读取权限的构建 secret。生产只向受信任的构建提供凭据，预览凭据与生产隔离。没有授权的预览以明确的取包错误退出。构建端支持显式本地制品输入，用于测试和手工构建。下载凭据只发到 GitHub API，不带入资产的转址、构建产物、浏览器或日志。安装和更新始终访问同源 `/registry/`。
- 校验归档摘要后在独立的 `.test/` 构建子目录处理。复用校验器检查路径、大小上限、格式、官方管理器身份、API 版本、入口和每个文件的哈希，拒绝覆盖保留目录及部署产物的文件。失败时以非零状态退出。Pages 产出 `/sw.js`、响应头、Function 路由和注册表，保留部署身份标记。站点阶段直接从已验证的插件文件复制管理器页面与资源。站点根 HTML 仅增添 K9 的注册逻辑，注册表内的 HTML 保持原字节。
- 按 K15 和 S13 对发布的 `mf-manifest.json` 原始字节签名并自验。若要设置 `plugin.json` 内的 bundle 引用，只在装配阶段改封套。最终注册表按 `mf-manifest.json` 出摘要，目录哈希覆盖最终封套、manifest 和 bundle。签名出错时保留上一个成功的 Pages 部署。`site-setup` 的 setup 分支走 S3。`npm run dev` 及前端独立开发沿用 Astro。

**首屏**

- Pages 返回根页或深链的官方管理器静态页及资源。静态页首屏可在 `/sw.js` 和 `/registry/` 被延迟或阻断时完成绘制。站点根 HTML 的幂等逻辑通过 `load` 门槛后让浏览器完成一次绘制，再注册 `/sw.js`。可见页面以实际 FCP 时间戳和注册时刻做回归判定，隐藏页面恢复可见后仍可进入注册流程。
- Service Worker 安装和默认管理器激活在后台进行。接管后的导航、离线、硬刷新恢复和用户选定的第三方管理器沿用 K9、K10。用同一浏览器和网络条件记录迁移前后 FCP，并核对首访请求顺序和跨源隔离响应头。

## Risks / Trade-offs

- **第三方管理器同源运行，等于完全信任** → 它能读写本站全部存储（游戏库、存档）并调用内核 API（包括卸载其他插件和重置）。目前靠"用户主动从 GitHub 标签安装"作为信任依据。是否需要确认界面或权限分级，等补齐项 (b)。
- **首访与回访的页面来源不同** → 内核接管之前由站点服务器直接返回静态页，之后由内核从本地缓存应答；两边来自同一次构建，但站点根的 `.html` 多一段注册脚本。用户切到第三方管理器后，硬刷新（Shift+F5，绕过 Service Worker）会先看到官方管理器。站点上管理器存两份（根目录和 `registry/p/`，各约 3.1 MiB）。
- **浏览器存储被清除** → 导航时从原来源重新下载；离线时显示恢复页。
- **没有 Service Worker 的环境** → 站点就是官方管理器的普通静态站点，没有插件系统：不能安装或切换管理器，也不再有提示原因的引导页。
- **内核更新立即接管** → 新旧内核共用状态格式；格式不兼容时从空状态重建并重新安装默认管理器。改动状态格式时必须提升 `STATE_FORMAT`。
- **同步死循环** → 不适用于内核：内核不执行插件代码。
- **raw.githubusercontent.com 不可达**（部分网络环境） → 第三方安装以 `fetch-failed` 失败，已安装的插件不受影响。

## Migration Plan

- Cloudflare Pages 执行 `npm run build:pages` 输出 `dist/site`。按 K21 先建立 Actions 发布与取包权限，再把已初始化的站点分支切到制品装配。原有初始化阶段与签名部署按 `site-setup` S13 推进。
- 首访直接取得官方管理器静态页，首屏绘制后注册 Service Worker，后续导航由内核应答。
- **回滚**：Service Worker 一旦注册会一直接管页面，单纯回滚到旧的静态站点不会让它消失。回滚时必须部署一个只做 `self.registration.unregister()` 并刷新客户端的 `/sw.js`（注册时带 `updateViaCache: 'none'`，浏览器检查更新会绕过 HTTP 缓存取到它），之后再换回旧站点。
