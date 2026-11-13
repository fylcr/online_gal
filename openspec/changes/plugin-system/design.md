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
- 签名。完整性靠哈希与来源，不靠作者签名。

## Decisions

### K1 — 插件系统是 Service Worker，不是每页加载的内核

Service Worker 在页面之前拿到导航请求，跨页面、跨刷新常驻，天然就是"本地服务器"。前一版设计让每个页面先启动一个内核再加载管理器，用户明确否定（"每页都要加载一次插件系统，这是不对的"）。

代价：浏览器禁用 Service Worker（部分隐私模式）时插件系统不可用。首次访问由站点服务器直接返回官方管理器的静态页，Service Worker 在页面 `load` 之后才注册，首屏不等内核（见 K9、K10）。

### K2 — 两个产物，内核尽量小

- **管理器**：`astro build` 原样产出 `apps/app/dist`，`build-site.mjs` 用 `apps/app/plugin.manifest.json` 打包成插件。管理器不知道自己被打包。
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
- 内核自身：浏览器对 `/sw.js` 的更新检查（`no-cache`，`updateViaCache: 'none'`），新内核 `skipWaiting` + `clients.claim` 立即接管。

### K9 — 引导、内核页与恢复

- 站点根直接放官方管理器的静态页，没有引导页。站点没有顶层 `404.html`：Cloudflare Pages 于是对任何未知路径都返回根 `index.html`，所以首次打开未知路径得到的是管理器首页。内核接管之后则不同：内核对没有 `404.html` 的管理器返回纯文本 404。这个差异目前没有处理（`tasks.md` 9.9）。
- 站点根的每个 `.html` 在 `</body>` 前带一段注册脚本：页面 `load` 之后才注册 `/sw.js`（`updateViaCache: 'none'`），安装不和首屏争抢。Service Worker 已激活却没有接管这次加载（硬刷新 Shift+F5 会绕过它）时 `reload` 一次，用 `sessionStorage` 记录时间，10 秒内最多一次，避免死循环。插件包里的 `.html` 不带这段脚本，摘要不变。
- `/__kernel/`：内核自带的页面（安装、切换、卸载、检查更新、重置），不依赖任何插件，管理器坏了也能用。
- 恢复页：内核应答导航失败时显示错误码，提供重试和进入 `/__kernel/`。
- 重置：删除全部插件与状态，页面注销 Service Worker 后回到站点根的静态页面（它在 `load` 之后会重新注册内核）。

### K10 — 性能

- **刷新与再次访问**：页面和所有资源都从本地缓存流式返回，没有网络往返。端到端实测：刷新时服务器收到 0 个请求，页面加载完成约 306 ms（本地 Chromium，包含渲染）。
- **路由是同步的**：`routeRequest` 只看 URL，决定是否 `respondWith`，不读存储。
- **热路径**：状态只在 Service Worker 启动时读一次，之后在内存里；每代文件清单转成 `Set` 后按代缓存（`WeakMap`）；打开的缓存对象复用。
- **不需要 navigation preload**：内核应答的请求从不访问网络，预加载只会浪费一次请求。
- **首次访问**：站点服务器直接返回官方管理器的静态页，Service Worker 在页面 `load` 之后才注册并在后台安装，首屏不等内核。安装下载 14 个文件约 0.1 MiB，另外 10 个（引擎与 sourcemap，约 3 MiB）是 lazy，等第一次被请求时再下载（见 K13）。实测在 80 ms 服务器延迟下，深链首次访问 FCP/LCP 约 430–540 ms（改回部分 lazy 之后三次：427、470、536 ms；此前全 lazy 配置一次：452 ms）；上一版先装内核和管理器再显示，约 900–1000 ms。首次访问的服务器请求数是 22 个（后台安装也下载首屏资源），不能和上一版的 11 个直接比较。样本很少，只在本机无头 Chromium 上用模拟延迟测过。第一次用到引擎时仍要下载它。这是"本地服务器"模式的固有成本，换来之后每次打开都不依赖网络。
- **分发**：注册表里的插件路径带内容哈希，以 `immutable` 长缓存发布；`/sw.js` 与 `/registry/index.json` 是 `no-cache`。

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

## Risks / Trade-offs

- **第三方管理器同源运行，等于完全信任** → 它能读写本站全部存储（游戏库、存档）并调用内核 API（包括卸载其他插件和重置）。目前靠"用户主动从 GitHub 标签安装"作为信任依据。是否需要确认界面或权限分级，等补齐项 (b)。
- **首访与回访的页面来源不同** → 内核接管之前由站点服务器直接返回静态页，之后由内核从本地缓存应答；两边来自同一次构建，但站点根的 `.html` 多一段注册脚本。用户切到第三方管理器后，硬刷新（Shift+F5，绕过 Service Worker）会先看到官方管理器。站点上管理器存两份（根目录和 `registry/p/`，各约 3.1 MiB）。
- **浏览器存储被清除** → 导航时从原来源重新下载；离线时显示恢复页。
- **没有 Service Worker 的环境** → 站点就是官方管理器的普通静态站点，没有插件系统：不能安装或切换管理器，也不再有提示原因的引导页。
- **内核更新立即接管** → 新旧内核共用状态格式；格式不兼容时从空状态重建并重新安装默认管理器。改动状态格式时必须提升 `STATE_FORMAT`。
- **同步死循环** → 不适用于内核：内核不执行插件代码。
- **raw.githubusercontent.com 不可达**（部分网络环境） → 第三方安装以 `fetch-failed` 失败，已安装的插件不受影响。

## Migration Plan

- Cloudflare Pages：构建命令改为 `npm run build:site`，输出目录改为 `dist/site`。`build`/`dev`/`test` 不变，开发仍用 Astro dev server（不经过插件系统）。
- 首次部署后，用户第一次打开直接得到官方管理器的静态页，Service Worker 在页面 `load` 之后注册；之后的导航由内核应答。
- **回滚**：Service Worker 一旦注册会一直接管页面，单纯回滚到旧的静态站点不会让它消失。回滚时必须部署一个只做 `self.registration.unregister()` 并刷新客户端的 `/sw.js`（浏览器会因 `no-cache` 取到它），之后再换回旧站点。
