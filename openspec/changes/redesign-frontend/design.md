# Design

## Context

- `apps/app` 现状：Astro 7.3、`@astrojs/svelte` 9、Svelte 5.43，静态输出，部署在 Cloudflare Pages（`https://onlinegal.de5.net/`，2026-11-23 起为官方地址，旧地址 `onlinegal.cc.cd` 301 跳转；默认地址 `onlinegal.pages.dev`）。
- 重构基线为 `Library.svelte`、`Player.svelte`、`Settings.svelte` 与 `Engines.svelte`。当前 `src/pages/index.astro` 已换成 `GameCard.astro` 与 `mock-games.ts` 的静态网格，`src/pages/play.astro` 目前只有返回游戏库的外壳。
- `src/lib/platform.ts` 是唯一组合根：`getPlatform()`、`browserSupport()`、`defaultSettings`、`formatPlayTime/formatDate/relativeTime`，并按编译期常量配置 KiriKiri 与 Artemis 引擎。
- 组件实际调用的 API：`platform.importDirectory/importFileList/importDataTransfer`、`library.list/get/patch/remove/redetect/openSource`、`saves.list/get/put/remove/exportAll/importAll/fileStore`、`settings.save/patch`、`loadSettings`、`registry.list`、`probeFailures`；`Session` 的 `start/stop/pause/resume/isPaused/save/restore/captureThumbnail/applySettings`；`@online-gal/core` 的 `runtimeLabel`、`unsupportedReason`、`isPlayable`。
- 设置字段：`mute`、`volumes`、`textSpeed`、`surface`、`showDiagnostics`。播放器已有全屏（`requestFullscreen`）、键盘处理、存档导入/导出、缩略图。
- 用户习惯：纯 Astro + Tailwind；Svelte 不冲突即保留。游戏库 4.5–4.9 按 `tasks.md` 的分工由 `frontend` 和 `file-system` 实施。

## Goals / Non-Goals

**Goals**
- 统一的 Tailwind 设计体系，替换手写 CSS 变量。
- 展示与交互分离：非交互部分零 JS，交互岛粒度更小、单文件更短。
- 现有功能全部保留，可运行状态如实显示。

**Non-Goals**
- 沿用 core/VFS 的现有公开契约。本地目录兼容修正与游戏删除的存档清理由 `file-system` 负责。超出本节的 API 需求另开 change。
- 不改引擎加载、隔离头、产物挂载（属于 `first-public-beta-ons-kirikiri`）。
- 不引入 React/Vue、daisyUI 以外的组件库、全局状态库、CSS-in-JS。

## Decisions

1. **Tailwind v4 经 `@tailwindcss/vite`**：在 `astro.config.mjs` 的 `vite.plugins` 加入，`src/styles/global.css` 改为 `@import "tailwindcss";` 加 `@theme` 令牌。理由：v4 不需要 `tailwind.config.js`，`.astro` 与 `.svelte` 共用同一份令牌。备选 `@astrojs/tailwind` 集成已不用于 v4。
2. **设计令牌放 `@theme`**：颜色、圆角、阴影、字体等用 CSS 变量命名（如 `--color-surface`），组件只用 Tailwind 类，不再写组件内 `<style>`（第三方嵌入或 canvas 尺寸等确有需要的例外须加注释）。
3. **Astro 与 Svelte 分工**：布局、导航、页脚、引擎/限制说明、空状态等静态内容用 `.astro`。交互岛：游戏库（导入、列表、详情）、播放器、设置表单。每个岛拆成多个小组件，放 `src/components/<area>/`，单文件建议不超过约 300 行。
4. **状态与数据流**：仍经 `src/lib/platform.ts`；组件间共享状态用 Svelte 5 runes 的 `.svelte.ts` 模块（如 `src/lib/library.svelte.ts`），不用外部状态库。
5. **可运行状态来自 platform**：卡片的“开始/不支持/未识别”只由 `isPlayable`、`unsupportedReason`、`registry.list()` 的可用性决定，界面不维护引擎名单。beta 裁剪后的原因文本由 platform 提供，界面原样显示。
6. **daisyUI 5 作为唯一组件库**（用户批准）：`global.css` 中 `@plugin "daisyui" { themes: false; }` 关闭内置主题，只用自定义主题 `swiss`（`@plugin "daisyui/theme"`）。组件优先用 daisyUI 类，不够时补 Tailwind 工具类；语义色用 daisyUI 色名（`base-*`、`primary` 等），不写 `dark:`。
7. **视觉方向：瑞士主义 / 国际主义平面设计风格**（回答 (a)）：白底黑字、单一信号红 `#e30613` 作为 `primary`；圆角 0、无阴影（`--depth: 0`）、1px 细线分隔；无衬线字体（Helvetica Neue/Helvetica/Arial，中文回落 Noto Sans SC/微软雅黑）；靠网格、对齐、字号对比和留白建立层级，不用装饰性渐变、插画或卡片阴影；文字左对齐、不两端对齐。
8. **迁移方式**：按页面逐个重写，每完成一页即可构建、可用；旧组件在对应新页面完成后删除，避免两套并存过久。
   - 2026-10-30 用户改为整体重写：旧 `components/`、`layouts/`、`pages/` 与旧 CSS 变量已一次删除，保留 `src/lib/platform.ts`、`astro.config.mjs`。
9. **路由**（回答 (e)）：`/`、`/play?id=`、`/settings` 不变。2026-10-30 用户把引擎页并入设置：`/engines` 改为静态重定向到 `/settings/engines`（`astro.config.mjs` 的 `redirects`），`info-pages` 的“Engine support page reflects the build”由设置页的“引擎”一栏承担。
13. **桌面网格**（用户 2026-10-30，取代 Decision 11 的 `px-3`）：外边距 24px；第 1 栏 48px 放 logo 与 dock（左缘对齐）；栏间距 48px；第 2 栏从 120px 起，页面标题、内容、页脚左缘对齐。头部高 96px，logo 48px，标题 `text-3xl`。
    **页面标题在头部**（用户 2026-10-30）：页面名从内容区移到头部 logo 右侧（`#page-title`），作为 swup 第二个 container 随访问替换并一起淡入淡出。
14. **设置页为层级菜单**（用户 2026-10-30，取代折叠方案）：`/settings` 是大项列表，每项进入独立子页 `/settings/<id>`（`src/pages/settings/[section].astro`，大项定义在 `src/lib/settings-sections.ts`）。现有大项：播放、音频、引擎、数据、高级，内容待填。设置内容居中，`max-w-2xl`。
16. **游戏库卡片**（用户 2026-10-30）：版式参考 KunGal（`KunMoe/kun-galgame-forum` 的 `components/galgame/card/`）：竖版 5:7 封面、左上引擎标签、封面右下角游玩时长（底部渐变遮罩，未玩过显示“未玩过”）、标题固定两行高、卡片左下角厂商（每张卡片等高）；**厂商字段 `GameRecord` 目前没有**，真实显示需另开 change 改 `packages/core`；网格按容器宽度分 2–7 列（container query）。该项目是 **AGPL-3.0**，与本仓库 GPL-3.0-or-later 不同，故只参考版式、自行实现，**不复制其代码或素材**。不可启动的卡片封面变灰并覆盖“本测试版不支持/未识别”标记，不是链接。现有 `src/components/library/GameCard.astro` 及网格可作版式基线，生产数据接入见 Decision 17。
15. **面包屑**（用户 2026-10-30）：页面标题 `<h1 id="page-title">` 留在导航栏 logo 右侧，设置及其子页一律显示“设置”。面包屑只在设置子页出现（`/settings` 列表页没有），在导航栏下方，左缘对齐第 2 栏（`lg:pl-30`），`text-sm`；路径式：根用 `>` 表示设置首页（链接到 `/settings`），每一级前加 `/`，符号与文字间留空（如 `> / 音频`、`> / 音频 / 子项`），末级为当前页。`#breadcrumbs` 容器每页都渲染（swup 要求两侧都有 container），无面包屑时为空。两者都是 swup container。标题不变的访问（如 `/settings` ↔ `/settings/<id>`）标题不做淡入淡出：`swup.ts` 在 `visit:start` 比较当前标题与目标 dock 项名称，相同则给 `<html>` 加 `data-static-title`，`global.css` 据此关闭 `#page-title` 的过渡，`visit:end` 移除。`/engines` 重定向改到 `/settings/engines`。dock 高亮对子页按前缀匹配（`/settings/*` 高亮“设置”）。
10. **页面切换用 swup 4**（用户 2026-10-30 指定）：`swup` 4.10.0 + `@swup/a11y-plugin`、`@swup/head-plugin`、`@swup/scripts-plugin`（只重跑 body 脚本，让新内容里的 Astro 岛水合）。只替换 `Layout.astro` 的 `#swup`、`#page-title` 与 `#breadcrumbs`，其余头部导航与页脚常驻；淡入淡出 180ms，尊重 `prefers-reduced-motion`。`/play` 不走 swup（`ignoreVisit`），进出播放器都是整页加载，保证引擎实例随文档销毁。初始化在 `src/lib/swup.ts`。swup 是导航库，不是 UI 框架/组件库/状态库，不违反技术栈约束。
11. **导航栏与 logo**（用户 2026-10-30）：无边框，左侧 logo、右侧按钮组，中间留白；两侧内边距 `px-3`，主内容不限宽。logo 是用户用 AI 生成的 `D:\atri\logo1.webp`，存为 `src/assets/logo.webp`，经 `astro:assets` + `sharp` 缩成 40px@1x/2x。**许可 CC BY-NC 4.0，不属于 GPL**，记录于 `src/assets/LICENSE.md` 与根 `THIRD_PARTY_NOTICES.md`。用户计划开 donations，认为与 NC 不冲突。
12. **桌面端竖向 dock**（用户 2026-10-30）：`src/components/Dock.astro`，固定在左侧垂直居中，`#swup` 之外常驻，零 JS；图标内联 SVG，hover 显示 daisyUI tooltip。圆角（dock `rounded-2xl`、按钮 `rounded-xl`），是对 Decision 7“圆角 0”的局部例外；当前页用加深底色（`base-content/12`）而不是换色，hover 为 `base-content/5`。加深底色是 dock 内唯一一块高亮元素，靠 `--dock-i` 平移到当前项（300ms，`prefers-reduced-motion` 下关闭）；`swup.ts` 在 `visit:start` 就移动，与内容淡出同时进行。`lg`（≥1024px）以下暂隐藏，移动端方案待 (b)。

17. **真实游戏库与本地目录**

    - 首页用 `getPlatform().library.list()` 投影真实记录。初始 HTML 保留 Astro 外壳和可绘制的静态内容，交互岛加载库数据。库为空时显示空状态，同时提供右下角「添加游戏」按钮。移除 `MOCK_GAMES` 和只供其使用的演示数据引用，已有游戏记录和存档随原库继续可用。
    - 圆形按钮位于游戏库视口右下角，使用现有 `primary` / `primary-content` 色令牌，是零圆角规则的局部例外。用 `+` 图标和「添加游戏」可访问名称，支持键盘、清晰焦点与触控。按安全区预留边距，避免遮挡末行卡片，并随游戏库导航生命周期挂载。按钮打开导入选项，优先提供「本地文件夹」，已有文件、压缩包及拖放入口沿用各自流程。
    - 「本地文件夹」在用户手势内调用 `showDirectoryPicker({ mode: "read" })`，得到句柄后经 `Platform.importDirectory` 交给 `FileSystemDirectorySource`。浏览器不提供可用的目录句柄时，从目录 input 的 `webkitRelativePath` 获得 `FileList`，经 `Platform.importFileList` 交给现有文件源。路径相对于所选根目录。同名文件按完整相对路径定位，保留原文件名和子目录。
    - 句柄路径持久化句柄及游戏记录，检测和游玩按需读取，`readRange` 只读取指定片段。目录 input 回退使用现有浏览器文件存储，选择前说明会占用本地站点存储。游戏文件一直由浏览器读取，源目录保持只读，运行时写入进入每个游戏的存档覆层。归档、WebDAV 等来源沿用各自流程。
    - 刷新后使用既有记录恢复挂载。权限未授予或目录移动时，在对应卡片、详情或播放器提供「重新授权」或「重新选择文件夹」。权限申请由用户手势触发。成功恢复该记录的来源后沿用原游戏 ID 和存档。取消选择、读不到文件和存储失败各有对应反馈，已有条目可以再次尝试。
    - 识别结果来自 platform。可启动游戏沿用 `/play?id=`，以持久化的游戏 ID 恢复源并进入现有会话。当前播放页只有外壳，若目标分支仍缺此接线，交付范围包含从本地目录条目到 `library.openSource` 与引擎会话的最小完整路径，播放器其余重构按第 6 节推进。
    - 右键卡片时，菜单操作绑定该卡片的游戏 ID。右键菜单和游戏详情共用删除确认流程。确认后经 `getPlatform().library.remove(id)` 按 ID 删除库记录、存档槽及运行时覆层文件。成功后刷新列表，失败时提示原因。源目录保持只读。

18. **固定导航栏、固定页脚与右上角操作**

    - `apps/app/src/layouts/Layout.astro` 的非 `bare` 布局把顶部导航栏固定到视口上沿、页脚固定到下沿。游戏库、设置和设置子页等普通页面都适用；`/play` 保持 `bare` 和现有整页进入/退出行为。保留 logo、页面标题、页脚内容与现有主题，滚动时两栏可读且不透出下方内容。
    - 内容上下空间和焦点滚动位置随两栏的实际高度和 safe-area 一起计算。面包屑在导航栏下方，不要求固定。保留现有横屏侧边、竖屏底部 dock；竖屏 dock 和游戏库右下角的「添加游戏」按钮相对页脚上沿留出空间。使用同一套偏移，避免各处写独立的固定高度。最后一行内容、可聚焦项以及窄屏的标题和操作都须完整可达。
    - 右侧按钮组依次为放大镜「搜索游戏」和三横线「打开导航菜单」。搜索入口指向游戏库的搜索区。在游戏库内直接定位并聚焦输入框，从其他普通页进入游戏库后再定位并聚焦（含 Astro 岛延迟挂载的情况）。搜索只过滤本地库记录，不把搜词放入 URL 或发往网络。
    - 汉堡菜单复用 dock 的现有链接「游戏库」「设置」，保留 dock。标明当前栏目（含设置子页），选择链接后关闭；Escape 或点击菜单外可关闭，键盘关闭时焦点回到菜单按钮。优先使用原生 HTML 控件，保留静态外壳；如需用脚本衔接焦点、菜单关闭与现有 swup 生命周期，只使用轻量原生脚本，不新增 Svelte 外壳岛或依赖。切页多次后不得重复注册监听或留下打开的菜单。

## Risks / Trade-offs

- 重写大组件可能丢功能 → `game-library-ui`、`player-ui` 列出功能对等清单，逐项验收。
- `client:only` 的播放器无 SSR 外壳，首屏空白 → 由 `.astro` 渲染静态外壳与加载提示。
- Tailwind 类过长影响可读性 → 重复组合抽成 Svelte/Astro 小组件，不用 `@apply` 堆样式。
- 与 beta change 并行修改 `Library.svelte` 等文件 → 规定 beta 的界面文案在本 change 的新组件里实现。

## Migration Plan

1. 接入 Tailwind 与令牌，旧样式暂存共存。
2. 依次重写：外壳与说明页 → 游戏库 → 设置 → 播放器。
3. 每页完成后删除对应旧组件与旧 CSS；最后删除 `global.css` 中不再使用的变量。
4. 回滚：每页一个提交，可单独 revert。

## Open Questions

见 `proposal.md`“待用户决定” (a)–(f)。其中 (a)(b)(e) 会改变要做的东西，在开始第 3 组任务前须有答案。
