# Tasks: plugin-system

把插件系统做成本地服务器：一个 Service Worker 内核接管本源的请求，从已安装的插件中应答；官方插件来自 Cloudflare Pages，第三方插件来自 GitHub 标签；管理器由 Astro 构建后打包成插件。`packages/plugin-host` 保留为插件规则层。范围以 `design.md` 的 Non-Goals 为界。管理系统（Cloudflare 账号、`/__cf`、浏览器版 cf、`/admin`）属于变更 `cf-account-admin`，由单独的对话负责，不在本文件跟踪；它与内核的接口是 design.md K19 的登记。

## 1. 插件规则层：契约与包骨架（`packages/plugin-host`）

- [x] 1.1 新建 `packages/plugin-host`（`package.json`、`tsconfig.json`），只依赖 `@online-gal/engine-api`（`GameSource`/`WritableStore`），加入 `tsconfig.packages.json` 与 pnpm workspace
- [x] 1.2 `src/errors.ts`：`PluginErrorCode` 联合与 `PluginError`，`asPluginError()` 把任意抛出值收敛为带码错误
- [x] 1.3 `src/manifest.ts`：`PluginManifest`、`parseManifest()`（拒绝未知字段/坏 id/非 semver/未知 capability/重复声明，返回冻结对象）、`assertApiCompatible()`（精确匹配 `PLUGIN_API_VERSION`）
- [x] 1.4 `src/capabilities.ts`：`PLUGIN_CAPABILITIES`、`DEFAULT_GRANTS`（`official: vfs.read+ui.notify`，`third-party: []`）、`GrantLedger`（只授予已声明的、每次调用 `require`、`close()` 清空、事件历史）

## 2. 插件规则层：受控表面与预算

- [x] 2.1 `src/budget.ts`：`ResourceBudget { memoryBytes, timeMs }`、`DEFAULT_BUDGETS`、`BudgetMeter`（窗口、`checkpoint()`、分配前检查的 `chargeMemory()`）
- [x] 2.2 `src/host-api.ts`：`HostApi` 构造冻结的 `PluginContext`（`vfs`/`store`/`net`/`notify`/`log`/`now`），每个方法都 `grants.require` + `budget.checkpoint`
- [x] 2.3 路径闸门 `assertRelativePath()`：拒绝盘符、UNC、前导分隔符、NUL、含 `..` 段、超长路径，且在校验前不触碰规范化
- [x] 2.4 `store` 前缀隔离：落盘到 `plugins/<pluginId>/`，`list()` 按目录边界过滤并剥离私有根
- [x] 2.5 未配置的宿主能力（`net`/`notify`）抛 `capability-unavailable`，绝不回退到 `fetch` 或全局对象

## 3. 插件规则层：宿主、启动与服务图

- [x] 3.1 `PluginHost.register()`/`boot(entries)`：全部闸门通过后才调 `load()`；结果按输入顺序、不抛；`register(entry)` = `boot([entry])`
- [x] 3.2 `provides`/`requires`：依赖可满足性、拓扑序、环检测；同一依赖层并行 `load()`，`setup` 仍按拓扑序；`setup` 返回值与 `provides` 严格一致；`ctx.use` 只给已声明的
- [x] 3.3 特权能力 `plugins.manage`：第三方不可声明、不进默认、仅按 id 批准、不可授出
- [x] 3.4 `grant`/`revoke`/`unload`/`dispose`；停止顺序为消费者先于提供者、`dispose()` 逆序；失败与卸载都先 `ledger.close()` 再 `dispose()`
- [x] 3.5 `dispatch(hook, payload)` 逐插件隔离；`plugins` 检查视图（状态、授权、拒绝次数、预算、失败原因）
- [x] 3.6 manifest 新增 `role: "manager"`；删除 `ui.mount` 与页面根（管理器就是站点本身，不再挂载）

## 4. 插件规则层：测试（本地工作台 `.test/`，gitignored）

- [x] 4.1 `.test/plugin-host.test.ts` 25 项：manifest 拒绝、版本闸门、默认授权、撤销与卸载、受控表面与路径闸门、预算、失败隔离
- [x] 4.2 `.test/plugin-kernel.test.ts` 18 项：启动顺序、服务图、并行加载、特权能力、停止顺序、`role`

## 5. ~~内核化：页面先启动内核，再加载管理器插件~~（已被第 6–9 节取代）

用户否定了"每页都加载一次插件系统"（见 `proposal.md`）。原 5.1–5.7 中仍成立的部分已并入第 3 节；页面根、`ui.mount`、构建期内联插件 chunk 与 `modulepreload` 方案作废。

## 6. 插件系统：Service Worker 内核（`packages/plugin-system`）

- [x] 6.1 新建 `packages/plugin-system`，依赖 `@online-gal/plugin-host`，devDependency `esbuild@0.27.7`；加入 `tsconfig.packages.json`
- [x] 6.2 `src/format.ts`：`plugin.json` format 1 的 `parsePackage()`（复用 `parseManifest`/`assertApiCompatible`）、安全相对路径、上限（5000 文件 / 单文件 256 MiB / 合计 1 GiB）、管理器保留路径、注册表 `parseRegistry()`、`sha256Hex()`
- [x] 6.3 `src/sources.ts`：`official:<id>` 与 `github:<owner>/<repo>@<tag>` 的解析与 URL 构造；来源决定 `provenance`，第三方不得使用 `online-gal.` 前缀
- [x] 6.4 `src/kernel.ts` 路由：同步的 `routeRequest()`；`/sw.js`、`/registry/**` 放行；`/__kernel/**`、`/__plugins/<id>/**`、其余导航与子资源由内核应答；所有应答带 COOP/COEP/nosniff
- [x] 6.5 静态托管语义：目录索引、`/a` → `/a/` 的 308（保留查询串）、`404.html`、HEAD、单段 Range（206/416）、`Cache-Control: no-cache`；解码失败为 400
- [x] 6.6 状态：`og-kernel` 缓存中的 JSON（`STATE_FORMAT = 1`），启动时读一次；先写状态再切换内存；改变状态的操作排队执行；格式不认识时从空状态开始
- [x] 6.7 安装：新一代写入独立缓存，6 路并发下载，逐文件校验大小与 SHA-256，全部通过且状态写入成功后才切换；失败删除新缓存；同包且文件完好时不重新下载
- [x] 6.8 更新与清理：保留最多一代旧文件供已打开页面使用，没有客户端时清除；一小时后清理无主缓存；管理器文件丢失时从原来源 `repair`
- [x] 6.9 管理器选择：没有可用管理器时先换用其他已安装管理器，再安装默认 `online-gal.manager`；Service Worker 安装阶段预先执行，失败不阻止启用
- [x] 6.10 官方后台更新：导航时 `waitUntil` 检查注册表，间隔至少 6 小时，只更新摘要变化了的官方插件；首次安装默认管理器时顺带记一次检查，首次导航不再重复取注册表（首次访问的服务器请求 12 个降到 11 个，FCP 无可测变化）
- [x] 6.11 内核 API `/__kernel/api/*`（`state`/`registry`/`install`/`uninstall`/`activate`/`update`/`reset`）：JSON 对象请求、拒绝导航式请求、统一响应与按错误码映射 HTTP 状态；`postMessage` 通知打开的页面
- [x] 6.12 `/__kernel/` 内核页与导航失败时的恢复页，不依赖任何插件
- [x] 6.13 `src/sw.ts`：把 `install`/`activate`/`fetch` 接到 `Kernel`；`skipWaiting` + `clients.claim`
- [x] 6.14 `format.ts`：文件条目可选 `lazy: true`（非布尔拒绝；至少一个非 lazy 文件）；格式版本仍为 1
- [x] 6.15 安装只下载非 lazy 文件，安装记录保存 lazy 清单与 `base`；请求时 `fetchLazy` 下载并校验，并发请求共用一次下载；完整性检查与修复只看非 lazy 文件；上一代同样保留 lazy 清单
- [x] 6.16 lazy 文件以 `fetch-failed` 失败时立即 `refreshOfficial`（忽略更新间隔，仅官方插件），有新版本就改从新版本应答

## 7. 构建与分发

- [x] 7.1 `apps/app/plugin.manifest.json`：官方管理器 `online-gal.manager`，`role: "manager"`；构建时版本追加内容哈希
- [x] 7.2 `packages/plugin-system/scripts/pack-plugin.mjs`：把任意目录打成插件包（第三方作者用它生成 `plugin.json`），使用与内核相同的 `parsePackage`；`--lazy <glob>`（可重复，`!` 开头的 glob 把匹配的文件留作立即下载）标记按需下载的文件，标记计入内容哈希
- [x] 7.3 `packages/plugin-system/scripts/build-site.mjs`：产出 `dist/site/`——站点根的官方管理器静态页（原样复制，每个 `.html` 在 `</body>` 前注入 `load` 之后注册 `/sw.js` 的脚本；无顶层 `404.html`）、`sw.js`（esbuild classic、压缩，约 31.5 KiB）、`_headers`、`/registry/index.json` 与带内容哈希的 `/registry/p/<id>/<hash>/`
- [x] 7.4 `_headers`：全站 COOP/COEP/nosniff；注册表索引 `no-cache`（`/sw.js` 不要求，更新检查靠 `updateViaCache: 'none'`）；`/registry/p/*` `immutable`
- [x] 7.5 根 `package.json` 新增 `build:site`；`build`/`dev`/`test` 不变
- [x] 7.6 构建脚本给官方管理器的 lazy 配置是 `engines/**` 与 `**/*.map`：24 个文件共 3.1 MiB，安装时下载 14 个、约 0.1 MiB（10 个 lazy）。曾试过除入口页外全部 lazy，站点根提供静态页之后没有收益，已改回
- [ ] 7.7（2026-11-23 用户决定改写，原"关闭 Git 集成、改直接上传"作废）官方站点继续用 Cloudflare Pages 的 Git 集成构建，不关闭、不直接上传。需要用户做的只剩生产环境 secret `ONLINE_GAL_CA_BUILD_TOKEN`（随机 32 字节 b64url，设计见 design.md K15 开头）：`/__ca` 上线并验证之后再配置，配置后的下一次生产构建开始给官方包签名。预览环境不配置。加 DNS 白名单记录还要等 10.3 的验签与 10.4 的 DoH 查询在真实 Service Worker 中接通并实测通过，否则“签名产物能通过验证”无从确认。
- [ ] 7.8（2026-11-23 用户决定，等仓库公开之后做，现在不做）给 `main` 开分支保护：禁止强制推送、禁止删除分支，不要求 PR、不要求状态检查。现在开不了：仓库是私有的，免费账号对私有仓库的分支保护与规则集都返回 403（`Upgrade to GitHub Pro or make this repository public`）。公开是 `github` 通道的前提（K15），公开时一并做。

## 8. 测试与验证

- [x] 8.1 `.test/plugin-system.test.ts` 34 项（其中 lazy 文件 10 项）：内存 CacheStorage 与注入的 `fetch`，覆盖路由、静态托管语义、编码路径、包与注册表校验、两种来源、原子安装与回滚、上一代、修复、管理器切换与回退、后台更新、内核 API
- [x] 8.2 三套测试合计 77 项全绿；`npx tsc -b tsconfig.packages.json` 通过
- [x] 8.3 端到端（本地 Chromium + CDP，仿 Cloudflare Pages 的静态服务器 + 假 GitHub raw 服务器）：14/14 通过；80 ms 服务器延迟下深链首次访问 FCP/LCP 约 430–540 ms（3 次：427、470、536 ms；上一版先装内核再显示约 900–1000 ms；样本少）；安装时不下载引擎与 sourcemap，引擎文件在第一次请求时才下载一次且字节一致，第二次由本地应答；首次访问与刷新后的页面都跨源隔离（首访检查的是生成的 `_headers` 内容，测试服务器按它加响应头，并没有测 Cloudflare 本身）；刷新时服务器收到 0 个请求（约 306 ms）；离线可用；内核页可用；从 GitHub 标签安装第三方管理器并接管站点；切回官方并卸载第三方
- [x] 8.4 `openspec validate plugin-system --strict` 通过

## 9. 待用户补齐后展开（不在本变更）

- [ ] 9.1 非管理器插件的执行模型：在哪运行（页面 / Worker / 沙箱 iframe）、钩子表与服务表、调用时机；届时接上 `plugin-host` 的 `boot`/服务图/授权
- [ ] 9.2 第三方管理器的信任：确认界面、警告或权限分级
- [ ] 9.3 引擎是否拆成独立插件，以及管理器与引擎之间的接口
- [ ] 9.4 更新策略：是否提示、能否关闭自动更新、第三方是否检查新标签
- [ ] 9.5 管理器内的插件管理界面（安装、切换、卸载、失败展示；内核 API 已就绪）
- [ ] 9.6 与 `Session`/运行时接线：插件在游戏启动、存档、脚本执行中的调用点
- [ ] 9.7 lazy 文件的预取策略（例如空闲时预取引擎）与离线提示：现在没取过的引擎文件离线时会返回 503
- [x] 9.8 站点根的注册脚本在 Service Worker 已激活却没有接管时 `reload` 一次（10 秒内最多一次）：已用本地 Chromium + CDP 回归覆盖；用户切到第三方管理器后硬刷新先看到官方静态页，再经一次恢复 reload 显示所选 manager，第二次硬刷新不会循环
- [ ] 9.9 Service Worker 接管前后对未知路径的行为不一致：接管前 Cloudflare 回退到根 `index.html`，接管后内核对没有 `404.html` 的管理器返回纯文本 404；管理器在站点上存两份（根目录与 `registry/p/`）
- [x] 9.10（管理器打不开的原因，2026-11-23 核实）官方包目录哈希覆盖整个目录，并让内核取包时绕过 HTTP 缓存。详情见 design.md K14 末两条。
  - 现象：线上 `https://onlinegal.cc.cd/registry/index.json` 已是 format 2（条目 `path` 为 `p/online-gal.manager/e4b8b5aa31bd842d/`），该目录的 `plugin.json` 也已是 `{ "format": 2, "manifest": "mf-manifest.json" }`。但这个 URL 之前发布过 format 1 的 `plugin.json`，响应头是 `Cache-Control: public, max-age=31536000, immutable`。拿到过旧文件的浏览器会一年内都用旧副本，新内核就报 `package-invalid: official packages must use format 2`，管理器打不开。
  - 改动：`scripts/pack-plugin.mjs` 的 `contentHash()` 纳入 `plugin.json` 与 `mf-manifest.json` 的实际字节，后者通过文件清单覆盖包内资源；`scripts/build-site.mjs` 保留原版本后缀算法，写定版本后再算目录哈希；`src/kernel.ts` 取 `plugin.json`、`mf-manifest.json` 与 bundle 都传 `{ cache: 'no-store' }`。
  - 验证：`.test/plugin-system-package-cache.test.ts` 3 项通过，覆盖只改封套字节、发布目录匹配实际包内容、三次包元数据请求绕过 HTTP 缓存；`npx tsc -b tsconfig.packages.json` 和 `npx openspec validate plugin-system --strict` 通过。
  - 这只修新构建。线上那个旧 URL 已经被污染，修复之后还要重新构建并部署一次，让注册表指向新目录。部署之前，拿到过旧文件的浏览器靠 `cache: 'no-store'` 恢复。

## 10. 包拆分与来源证明（format 2；设计见 design.md K14–K17）

- [ ] 10.0 可行性验证（先于 10.3）：公共 Rekor 接受裸 ECDSA 公钥的 hashedrekord 并返回 SET 与 inclusion proof；DoH（cloudflare-dns.com、dns.google）能从 Service Worker 以 CORS 取 TXT 且带 AD 位；GitHub 通道另验：(a) 官方仓库公开、attestation 走公共 Sigstore；(b) Sigstore bundle 能在 Service Worker 里验证（`@sigstore/verify` 依赖 Node crypto，需试浏览器实现或最小自写验证器）并记下 `sw.js` 体积变化；(c) 证书里能取到 ref 与仓库 ID 扩展（Source Repository Identifier，据我所知 OID 1.3.6.1.4.1.57264.1.15；取不到就回报用户，不静默回退到不含仓库 ID 的哈希）；(d) `actions/attest-build-provenance` 的 `bundle-path` 输出格式（据我所知是 JSONL，单 subject 一行）；(e) Pages Functions 在 `__ca/` 路径下的路由，以及 `_headers` 不作用于 Function 响应；结果记入 design.md
- [x] 10.1 `src/format.ts`：`parseEnvelope()`（plugin.json format 2，只允许 format/manifest/bundle，路径限同目录或子目录）；`parsePackage()` 改为接收 mf-manifest.json 正文；format 1 保留为只读兼容（仅 GitHub 来源）；`parseRegistry()` 的 digest 改指 mf-manifest.json
- [ ] 10.2（plugin.json → mf-manifest.json → 核对 registry digest 已完成并通过测试；`KernelOptions.fetchTrustRecord` 可选注入已加：配置后官方包无 bundle 被拒、`github:` 在受限模式下于下载前被拒、官方包装成 `unverified`、记录在内存缓存 1 小时，均有测试；`refreshOfficial` 与 `checkUpdates` 经同一个 `installLocked` 走这条路径，未单独测试；`sw.ts` 尚未启用 DoH 验证，现网仍不强制验证；`microca` 验签见 10.3，DoH 恢复后的后台补验未做）`src/kernel.ts` `installLocked()`：plugin.json → mf-manifest.json（核对 registry digest）→ bundle 验证 → 逐文件哈希；`refreshOfficial` 同步改；DoH 取不到时进入受限模式（只装官方来源、不验 bundle、标 `unverified`；`github:` 来源以 `attestation-unavailable` 拒绝），后台检查在 DoH 恢复后补验
- [x] 10.3 `src/attest.ts`、`src/rekor.ts`：使用 Web Crypto 验证匿名凭证 JWS、白名单和时间窗、Rekor SET、签名 checkpoint、RFC 6962 inclusion proof，以及临时密钥对 `mf-manifest.json` 的签名；内核传入原始 manifest 字节，避免对摘要再次哈希。`test/plugin-system-microca.test.mjs` 覆盖有效签名、摘要篡改、错误白名单和篡改的 SET／Merkle 路径；生产 DoH 集成仍见 10.2／10.4。
- [ ] 10.3b（`githubCommitment` 已加 repoId 参数、非十进制数字串被拒，`normalizeWorkflowUri` 已完成，测试已同步；Sigstore bundle 验证未做，等 10.0）`src/attest.ts` `github` 分支：Sigstore bundle 验证（方案依 10.0）、`workflowUri` 去 `@<ref>` 规范化、ref 必须是 `refs/tags/*`、subject 摘要对 mf-manifest.json、`SHA-256("github|" + issuer + "|" + repoId + "|" + workflowUri + "|" + salt)`（repoId 取自证书的 Source Repository Identifier，必须是十进制数字串；`githubCommitment` 增加该参数，`.test/plugin-attest.test.ts` 同步改）
- [ ] 10.4（`hashAllowed` 的时间窗比对已完成并有测试；DoH 查询、AD 位、多解析器一致性、TXT 解析未做）`src/trust-dns.ts`：DoH 查询、AD 位、多解析器一致性、TXT 解析（`v`/`salt`/`hashes`，条目可带 after/before，单条超 255 字节按多段拼接）；结果缓存进状态，有效期取 DoH 返回的 TTL、上限 1 小时；现取失败且没有未过期缓存才算「DoH 不可达」（受限模式，K17）
- [ ] 10.5（`InstalledPlugin.attestation` 可选字段已加，并在 `state` API 的 `PluginInfo` 里暴露；因为是可选字段，旧状态能直接读，暂未升 `STATE_FORMAT`；撤销标记未做）`STATE_FORMAT` 升到 2：记录 `attestation`（`verified` | `unverified` | `revoked`）与撤销标记；旧状态按格式不认识从空状态重建
- [x] 10.6 错误码 `attestation-invalid`、`attestation-unavailable`，并映射 HTTP 状态
- [x] 10.7 `scripts/sign-plugin.mjs` 用一次性 P-256 密钥签 manifest，向 `/__ca/issue` 取凭证并向公共 Rekor 写入 `hashedrekord`。`scripts/build-site.mjs` 先定版本并计算含封套的目录哈希，再签名、验证并写出 `plugin.bundle.json`，该 bundle 不进目录哈希。未配令牌时保留原封套、不出 bundle，并记录跳过；配置后任何环节失败都中止。`test/plugin-system-microca.test.mjs` 已覆盖跳过、CA 不可达及 mock Rekor 的成功验签；未配令牌的站点构建通过。`github` 发布见可选 10.10。
- [x] 10.8 Pages 构建身份：`functions/__ca/issue.ts` 常量时间比较 `ONLINE_GAL_CA_BUILD_TOKEN`，固定 `anon_id` 为 `cf-pages`，用 `ONLINE_GAL_CA_PRIVATE_JWK` 签发 600 秒 ES256 JWS；未配置时明确拒绝。站点 `_routes.json` 已包含 `/__ca/*`，setup 阶段的 `_routes.json` 和 `_worker.js` 不包含该路由。单元测试覆盖授权、拒绝和缓存头；真实 Pages 路由与上线顺序按 7.7 验证。
  - 非 Pages 的 OIDC 主体绑定、初始化密钥登记（11.2）及 DNS TXT 配置不在本次 Pages 构建签名范围。先验证线上路由，再由用户按 11.2/7.7 准备生产密钥和令牌；预览不配置令牌。真实签名产物通过内核验证、DoH 实测通过后才加 TXT。
- [ ] 10.9（拆分格式夹具已改；`.test/plugin-attest.test.ts` 覆盖解析、承诺哈希（含 repoId）、规范化与白名单；`.test/plugin-system.test.ts` 已补受限模式（官方包 `unverified`、`github:` 下载前被拒、记录被篡改不算不可达）、官方包无 bundle 被拒、记录缓存 1 小时、`/__ca/**` 放行与管理器保留路径；plugin 测试合计 105 项通过；bundle 篡改、DNS 缺哈希或时间窗外、解析器不一致、GitHub 来源坏 bundle、DoH 恢复后补验要等验证器实现后补）`.test/plugin-system.test.ts` 增补：拆分格式、digest 不符、bundle 篡改、DNS 缺哈希或时间窗外、解析器不一致、官方无 bundle、GitHub 来源坏 bundle、未知 `channel`、`github` 通道 `@<ref>` 后缀不同哈希相同、非 `refs/tags/*` 被拒、受限模式（装官方包标 `unverified`、`github:` 被拒、DoH 恢复后补验）、`github` 通道缺 repoId 或 repoId 非数字被拒、不同 repoId 哈希不同；`npx tsc -b tsconfig.packages.json` 与 `openspec validate plugin-system --strict`
- [ ] 10.10（可选，2026-11-23 降级：日常发布改由 Pages 构建走 `microca`，见 7.7 与 design.md K15；本任务只在启用 `github` 通道时做）`.github/workflows/release.yml`（用户已同意放开 `.github/**`，只新增这一个文件）：`v*` 标签触发；权限 `contents: read`、`id-token: write`、`attestations: write`；`npm ci` → `npm run build:pages`（2026-11-22 因 `site-setup` 决定 F 改；该步骤的 env 给 `CLOUDFLARE_API_TOKEN`、`CLOUDFLARE_ACCOUNT_ID`、`ONLINE_GAL_PAGES_PROJECT`，用于判断阶段；它向 `GITHUB_OUTPUT` 写 `phase`，只有 `phase == 'site'` 时才做下面的来源证明）→ 对 `dist/site/registry/p/*/*/mf-manifest.json` 逐个 `actions/attest-build-provenance`，把 bundle（`bundle-path` 输出，据我所知是 JSONL，单 subject 一行）写成同目录 `plugin.bundle.json` → `cf pages deploy dist/site`（连同 `functions/`；2026-11-23 用户决定改用 `cf` CLI，不用 `wrangler`）。启用前提：`cf pages deploy` 能上传目录（现在不能），且 Git 集成构建已停掉，否则 Pages 会重新构建出字节不同的包。第三方 action 固定到提交 SHA；`v*` 标签保护规则与该文件的 CODEOWNERS 是这条通道的安全边界（design.md K17 (8)）；需要用户配置 Pages 项目与仓库 secrets

## 11. 登记规则补充（2026-11-22，因变更 `site-setup`；设计见 design.md K19）

- [x] 11.1 实现“只有 Function 的登记前缀，内核放行”。**已由 11.3 撤销。**
  - 改动：`packages/plugin-system/src/kernel.ts` 的 `routeRequest` 对 `/__setup` 与 `/__setup/**` 的任何方法返回 `null`；只改路由实现，不改保留路径或 `RESERVED`。
  - 测试：新增 `test/plugin-system-route.test.mjs`（portable tests），覆盖 GET/POST/DELETE、前缀根路径、相似路径边界；5/5 通过。仓库的 `.test/` 工作台在当前克隆中不存在。
  - 验证：`tsc -b tsconfig.packages.json` 通过；`openspec validate plugin-system --strict` 通过。全 portable suite 604 项中 574 通过、30 失败；失败未在本次处理中调查。
- [ ] 11.2（与 10.8 一起做，不要提前做）把 K18 微型 CA 的私钥和"允许的平台身份 → `anon_id`"映射，登记为站点初始化项，不另做配置入口。
  - 用户决定（2026-11-22）：凡是需要初始化的，都在站点初始化时完成。
  - 登记方式：在 `apps/setup/src/lib/setup-items.ts` 末尾加条目，`owner: 'plugin-system'`。规则见变更 `site-setup` 的 design.md S7。
  - 如果需要新的生成或校验方式（例如在浏览器里生成 ES256 密钥、显示公钥指纹和 `_trust.<域名>` 的 TXT 记录），先回报 OpenSpec 管理对话，由它写入 `site-setup` 的设计，再由管理系统的对话实现。
  - `functions/__ca/` 读不到这些 secret 时，必须给出明确错误，不能静默失败。
- [x] 11.3（2026-11-22，因 `site-setup` 用户决定 F：初始化代码不进站点构建）撤销 11.1：删除 `packages/plugin-system/src/kernel.ts` 里对 `/__setup` 的放行，删除 `test/plugin-system-route.test.mjs` 里对应的用例（文件只剩这些用例时整个删除）。`format.ts`、`build-site.mjs` 里的 `__setup` 由 `site-setup` 3.2 删除。完成后 `dist/site/` 里搜不到 `__setup`。
