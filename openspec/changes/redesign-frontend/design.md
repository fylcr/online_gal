# Design

## Context

- `apps/app` 现状：Astro 7.3、`@astrojs/svelte` 9、Svelte 5.43，静态输出，部署在 Cloudflare Pages（`https://onlinegal.pages.dev/`）。
- 页面：`src/pages/index.astro` → `Library.svelte`（`client:load`）；`play.astro` → `Player.svelte`（`client:only="svelte"`，`bare` 布局，读 `?id=`）；`settings.astro` → `Settings.svelte`；`engines.astro` → `Engines.svelte`。`src/layouts/Layout.astro` 含头部导航与页脚。
- `src/lib/platform.ts` 是唯一组合根：`getPlatform()`、`browserSupport()`、`defaultSettings`、`formatPlayTime/formatDate/relativeTime`，并按编译期常量配置 KiriKiri 与 Artemis 引擎。
- 组件实际调用的 API：`platform.importDirectory/importFileList/importDataTransfer`、`library.list/get/patch/remove/redetect/openSource`、`saves.list/get/put/remove/exportAll/importAll/fileStore`、`settings.save/patch`、`loadSettings`、`registry.list`、`probeFailures`；`Session` 的 `start/stop/pause/resume/isPaused/save/restore/captureThumbnail/applySettings`；`@online-gal/core` 的 `runtimeLabel`、`unsupportedReason`、`isPlayable`。
- 设置字段：`mute`、`volumes`、`textSpeed`、`surface`、`showDiagnostics`。播放器已有全屏（`requestFullscreen`）、键盘处理、存档导入/导出、缩略图。
- 用户习惯：纯 Astro + Tailwind；Svelte 不冲突即保留。前端细节由用户在 `apps/` 中亲自实施。

## Goals / Non-Goals

**Goals**
- 统一的 Tailwind 设计体系，替换手写 CSS 变量。
- 展示与交互分离：非交互部分零 JS，交互岛粒度更小、单文件更短。
- 现有功能全部保留，可运行状态如实显示。

**Non-Goals**
- 不改 `packages/` 的 API 或行为；需要新 API 时另开 change。
- 不改引擎加载、隔离头、产物挂载（属于 `first-public-beta-ons-kirikiri`）。
- 不引入 React/Vue、组件库（daisyUI 等）、全局状态库、CSS-in-JS。

## Decisions

1. **Tailwind v4 经 `@tailwindcss/vite`**：在 `astro.config.mjs` 的 `vite.plugins` 加入，`src/styles/global.css` 改为 `@import "tailwindcss";` 加 `@theme` 令牌。理由：v4 不需要 `tailwind.config.js`，`.astro` 与 `.svelte` 共用同一份令牌。备选 `@astrojs/tailwind` 集成已不用于 v4。
2. **设计令牌放 `@theme`**：颜色、圆角、阴影、字体等用 CSS 变量命名（如 `--color-surface`），组件只用 Tailwind 类，不再写组件内 `<style>`（第三方嵌入或 canvas 尺寸等确有需要的例外须加注释）。
3. **Astro 与 Svelte 分工**：布局、导航、页脚、引擎/限制说明、空状态等静态内容用 `.astro`。交互岛：游戏库（导入、列表、详情）、播放器、设置表单。每个岛拆成多个小组件，放 `src/components/<area>/`，单文件建议不超过约 300 行。
4. **状态与数据流**：仍经 `src/lib/platform.ts`；组件间共享状态用 Svelte 5 runes 的 `.svelte.ts` 模块（如 `src/lib/library.svelte.ts`），不用外部状态库。
5. **可运行状态来自 platform**：卡片的“开始/不支持/未识别”只由 `isPlayable`、`unsupportedReason`、`registry.list()` 的可用性决定，界面不维护引擎名单。beta 裁剪后的原因文本由 platform 提供，界面原样显示。
6. **迁移方式**：按页面逐个重写，每完成一页即可构建、可用；旧组件在对应新页面完成后删除，避免两套并存过久。

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
