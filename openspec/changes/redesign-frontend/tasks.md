# Tasks

> 游戏库 4.5–4.9 已获准实施。`frontend` 负责 4.5、4.7、4.9 的界面和 4.8 的端到端浏览器验收。`file-system` 负责 4.6、4.8 的目录链路验证，以及 4.9 的 core 删除契约。先并行做界面和目录链路，再把两边的提交合到同一候选版本，跑完 4.8 与 4.9 的联合验收。各自将实现和验收结果写回本节。
> 开始 3.1–3.3 前，`proposal.md`“待用户决定”(a)(b)(e) 必须有答案。游戏库 4.5–4.9 与固定外壳、右上角操作 3.4–3.7 可独立推进；后者在现有普通页、横屏及窄屏/竖屏布局上验收，不扩大首版设备承诺。

## 1. 前置

- [ ] 1.1 确认包重命名已提交，且 `apps/app/src/lib/platform.ts` 使用 `@online-gal/engine-core-*` 新包名。验证：`git status --porcelain apps/app packages` 为空；`pnpm --filter @online-gal/app build` exit 0。
- [ ] 1.2 【用户】回答 `proposal.md` 的 (a)–(f)，并写入 `design.md` Decisions。验证：`openspec validate redesign-frontend --strict` exit 0。
- [ ] 1.3 记录重构前的功能清单基线：对照 `game-library-ui`、`player-ui`、`info-pages` 的功能对等要求，在旧界面逐项确认存在。验证：清单写入 `docs/current-work.md`。

## 2. 基础设施

- [x] 2.1 在 `apps/app` 安装 `tailwindcss` 4.3.3、`@tailwindcss/vite` 4.3.3 与 `daisyui` 5.7.50（2026-10-29 完成，`astro build` exit 0）。原文：安装 `tailwindcss` 与 `@tailwindcss/vite`，在 `astro.config.mjs` 的 `vite.plugins` 中加入插件，不改动其中的隔离头与挂载逻辑。验证：`pnpm --filter @online-gal/app build` exit 0；`git diff apps/app/astro.config.mjs` 只多出插件相关行。
- [ ] 2.2 改写 `apps/app/src/styles/global.css`：`@import "tailwindcss";` 加 `@theme` 令牌（按 1.2 的视觉方向）。进展：已加入 daisyUI 插件与 `swiss` 主题，构建通过；待在浏览器确认 Tailwind preflight 未破坏旧页面外观。旧 CSS 变量暂时保留。验证：构建通过，现有页面外观不崩。
- [ ] 2.3 新建 `src/lib/*.svelte.ts` 共享状态模块（游戏库列表、设置），只经 `src/lib/platform.ts` 访问 core。验证：`astro check` 0 error。

## 3. 外壳与说明页

- [ ] 3.1 重写 `src/layouts/Layout.astro`（头部、导航、页脚）为 Tailwind，静态外壳不加载 Svelte 脚本。右上角控制与现有切页的最小脚本边界见 3.4–3.7。
- [ ] 3.2 新增已知限制页（`src/pages/limitations.astro` 或按 (e) 的路由），内容来自 `README.md`。验证：页面零 JS；游戏库与不支持提示能链接到它。
- [ ] 3.3 重写引擎/支持说明页：静态说明用 `.astro`，可用性列表用一个小 Svelte 岛读取 `registry.list()`。验证：满足 `info-pages` 的 “Engine support page reflects the build”。

- [ ] 3.4 【frontend】固定普通页面顶部导航栏与底部页脚。通过共享布局协调高度、safe-area、内容滚动留白、竖屏 dock 和「添加游戏」按钮，保留现有网格、面包屑和 `/play` 的 `bare` 模式。
- [ ] 3.5 【frontend】在导航栏右端添加「搜索游戏」和汉堡菜单入口。汉堡菜单复用现有导航链接及当前栏目状态，并完成点击、触控、键盘、Escape、点击外部和 swup 切页的打开/关闭流程。
- [ ] 3.6 【frontend】为游戏库接通按名称、标题、标签筛选的本地搜索。从首页直接聚焦，从其他普通页切到首页后聚焦，支持迟挂载。区分空库与无匹配，清空后恢复完整列表。不得用搜索词发起网络查询或写入 URL。
- [ ] 3.7 【frontend】在桌面横屏、窄屏竖屏和带安全区的浏览器条件验证首页、设置、设置子页与 `/play`。使用超出一屏的内容检查两栏不移动、首尾内容与键盘焦点不被遮挡，dock、添加按钮和菜单不重叠。覆盖菜单焦点、反复切页、站内和跨页搜索、空库与无匹配。验证静态外壳不阻塞首屏、`astro check`、站点前端构建与 OpenSpec 严格校验；在本节记录浏览器、命令和结果。

## 4. 游戏库

- [ ] 4.1 拆分 `Library.svelte` 为 `src/components/library/` 下的小组件（导入区、工具栏、卡片、详情、提示）。验证：单文件约 300 行以内；`astro check` 0 error。
- [ ] 4.2 实现可运行三态（可开始/不支持/未识别），状态只来自 platform。验证：`grep -rn "artemis\|siglus\|tyrano" apps/app/src/components/library` 无引擎名单硬编码。
- [ ] 4.3 实现空库、搜索无结果、初始化失败、浏览器能力不足提示。验证：在无跨源隔离的 `astro dev`（临时去头）下看到提示，验证后恢复。
- [ ] 4.4 删除旧 `Library.svelte` 与其专用 CSS。验证：`game-library-ui` 功能对等清单逐项通过；构建通过。

- [x] 4.5 【前端】以 `getPlatform().library.list()` 替换 `MOCK_GAMES`，移除生产首页的占位数据引用。保留静态首屏外壳，实现真实列表和空状态。在右下角加入使用现有主题主色、支持键盘与触控的圆形「添加游戏」按钮。验证空库、已有游戏库、窄屏安全区、键盘打开导入入口，以及导航到游戏库外时按钮移除。
- [x] 4.6 【file-system】验证并补齐 `Platform.importDirectory`、`FileSystemDirectorySource`、`library.openSource` 和目录 input 回退。夹具含同名不同层文件与可识别的松散资源。校验完整相对路径、文件大小、指定范围字节、句柄挂载的按需读取、刷新后的恢复、只读源和本地覆层。目录 input 须按完整路径配对文件，避免后缀匹配。校验失权及重选可恢复同一记录 ID 和存档。
- [x] 4.7 【前端，与 file-system 联调】从「添加游戏 → 本地文件夹」接入只读目录选择器及目录 input 回退。呈现识别结果、导入失败、重新授权与重选。成功后刷新库列表。让可启动条目经 `/play?id=` 打开既有会话。目标分支若只有播放页外壳，先补这一最小启动路径。
- [x] 4.8 【前端 + file-system】用隔离浏览器配置和合成目录验证选择、列表、进入播放、刷新后重开及失权恢复。单独验证目录 input 回退、取消或失败时保留记录、源文件字节未变、写入只在覆层。记录测试命令、浏览器版本和结果，并校验 `astro check` 与 `openspec validate redesign-frontend --strict`。真实游戏仍按 7.2 验收。

- [x] 4.9 【前端 + file-system】为每张真实游戏卡片加右键「从游戏库删除」，与详情共用显示游戏名及存档影响的确认流程，经 platform 按游戏 ID 删除。`file-system` 核对 core 删除同时清理该 ID 的存档槽和运行时写入的覆层文件。用两条各有存档槽和覆层文件的游戏记录验收关闭菜单、取消确认和确认删除。刷新后复查所选记录及两类存档已移除，验证另一条记录及其两类存档、两份原始本地文件夹的完整性。

### 本地目录与删除的 core 验收

运行 `npm run build:packages`，将 `LIBRARY_BROWSER_TOOLS` 指向安装了 `playwright` 和 `esbuild` 的目录，再运行 `node test/local-library-storage.e2e.mjs`。可通过 `CHROMIUM_PATH` 指定浏览器。`npm run check` 和 `openspec validate redesign-frontend --strict` 通过。

Chromium 156.0.8078.4 在隔离配置中导入两份含松散 `00.txt` 的合成游戏。真实浏览器目录句柄在刷新后可重新打开。打开前没有读取 `config.txt` 的文件正文，第一次范围请求只切片 `[3, 10)`。测试通过浏览器句柄控制许可状态，确认打开只查询授权，点击恢复按钮才请求 `read` 权限。重新选择目录后保留记录 ID、存档槽和覆层，拒绝授权时保留原绑定。用真实目录 input 先传入 `sub/config.txt`，刷新后根层和嵌套的同名文件仍各自返回正确字节。

按 ID 调用 `GameLibrary.remove()` 后刷新，所选记录、存档槽、覆层文件和目录 input 的导入副本均已清理。另一条目录记录及其存档槽、覆层内容仍可读取。逐个复核源文件字节与夹具一致。

额外回归验证了两种失败都保留已有记录及内容：重选只有空子目录的目录，以及目录 input 的两条路径归一化为同一路径。重导入和删除按两个独立数据库连接测试了两种事务顺序。删除先提交时，重导入分配新 ID，只写入新 ID 的文件。重导入先提交时，删除清除该 ID 的记录和所有文件。用 `game` 与 `game-other` 验证前缀相同的合法 ID 互不影响，并校验拒绝空 ID、含路径分隔符或 NUL 的 ID，以及与存档覆层命名空间冲突的 ID。

### 游戏库与播放页的联合 Chromium 验收

在联合候选工作树运行 `npm run build:packages`，再用 `pnpm --filter @online-gal/app exec astro dev --host 127.0.0.1 --port 4322` 启动应用。沿用上节的 `LIBRARY_BROWSER_TOOLS` 和可选的 `CHROMIUM_PATH`，在另一终端运行 `node test/local-library-browser.e2e.mjs`。服务地址可以通过 `ONLINE_GAL_LIBRARY_URL` 覆盖。`npm run check` 中的 app 和 setup `astro check` 均为 0 错误、0 警告、0 提示。`pnpm --filter @online-gal/app build` 和 `openspec validate redesign-frontend --strict` 通过。

- 4.5 在 Chromium 156.0.8078.4 的 320px 触屏窗口中通过空库、真实条目、键盘打开和触屏选择验收。注入右侧 12px 和底部 20px 安全区后，按钮按对应距离偏移并位于导航栏上方。使用 Swup 前往设置后按钮从 DOM 移除，返回游戏库后重新出现。
- 4.7–4.8 使用真实 OPFS 目录句柄与 IndexedDB 持久化，由脚本控制目录选择和授权结果。合成 HTML 游戏从 `/play?id=` 进入现有运行时会话，刷新后可重开。拒绝授权、取消或失败以及重选空文件夹均保留原记录和存档。重新授权或选中有效目录后可继续使用原 ID 播放。用真实目录 input 导入另一份临时磁盘文件夹，核对根层和子目录同名文件、刷新后重开，以及写入本地覆层后的读取结果。
- 4.9 核对 Escape 和点击外部关闭右键操作、取消确认、详情复用确认及按 ID 确认删除。两条记录各有一个存档槽和内容不同的同名覆层文件。刷新后所选记录及两类存档已清除，另一条记录及槽位和覆层字节仍在。逐个复核磁盘夹具和 OPFS 原始目录字节。真实 ONS 与 KiriKiri 的验收列在 7.2。

### 生产部署记录

- 联合候选 `0a03330102fe72f352a35db3fa255c775fe117cf` 已合入 `main`。Pages 生产部署 `99c40bce-5504-48ae-98ab-d9cdedd686cd` 成功，`onlinegal.de5.net` 指向该部署。线上 `/`、`/play/` 和 `/settings/` 均返回 200，带 `COOP: same-origin` 和 `COEP: credentialless`。首页 HTML 含真实游戏库组件和「添加游戏」按钮。
- Edge 158.0.4370.33 在线上独立浏览器配置中验证了空库、圆形添加按钮、键盘打开导入窗和 `crossOriginIsolated === true`。通过目录 input 导入临时合成 HTML 游戏，从游戏库启动并返回，随后右键取消删除、确认删除、刷新后检查空库。逐字节复核了临时目录中的源文件。真实 ONS 与 KiriKiri 游戏的验收见 7.2。
- 同一提交的 Build packages 报告 portable suite 612 项中 580 通过、30 失败、2 跳过。按测试名对照此前主线，29 项已出现。额外一项在 `test/artemis-compat-compact-audio-restore-check.test.mjs:397` 观察到额外的 `census-key` 和 `pull`，待用相同 CI 条件复核。[本次运行](https://github.com/fylcr/online_gal/actions/runs/45543375775) · [主线基线](https://github.com/fylcr/online_gal/actions/runs/45424792679)
- 同一提交的 Build wasm (Artemis compat) 在 `configure` 阶段退出 1，此前主线也在同一阶段失败。配置错误详情写入构建工作区的 `ports/artemis/compat-web/out/build.log`，公开运行只显示退出码。[本次运行](https://github.com/fylcr/online_gal/actions/runs/45543375939) · [此前运行](https://github.com/fylcr/online_gal/actions/runs/45111953742)

## 5. 设置

- [ ] 5.1 重写设置页，所有设置项与数据管理操作保留，修改即保存。验证：修改音量后刷新页面值仍在。

## 6. 播放器

- [ ] 6.1 `play.astro` 提供静态外壳与加载提示，播放器岛拆分到 `src/components/player/`（画面、控制栏、存档面板、状态页）。验证：首屏不是空白；`astro check` 0 error。
- [ ] 6.2 实现四种状态（加载中/不存在/不支持/启动失败），不支持时不创建引擎实例。验证：访问不存在的 id 显示“游戏不存在”。
- [ ] 6.3 实现控制栏自动隐藏、全屏保持宽高比、现有快捷键、存档面板（存/读/删/导入/导出）。验证：`player-ui` 功能对等清单逐项通过。
- [ ] 6.4 删除旧 `Player.svelte`。验证：构建通过；返回游戏库后无残留音频。

## 7. 收尾与验收

- [ ] 7.1 删除 `global.css` 中不再使用的旧变量；检查所有 `<style>` 都有原因注释。验证：`grep -rn "<style" apps/app/src` 每处带注释。
- [ ] 7.2 【用户】在真实 ONS 与 KiriKiri 游戏上跑一遍游戏库→播放→存读档→返回流程（丢弃一次热身，两次一致）。验证：结果记录在 `docs/current-work.md`。
- [ ] 7.3 部署到 Cloudflare Pages 预览并检查：首页 200、`crossOriginIsolated === true`、各页面可打开。验证：记录预览 URL 与检查结果。
