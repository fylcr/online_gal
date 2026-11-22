# Proposal

## Why

现有前端 `apps/app`（Astro 7 + Svelte 5）设计与可用性都差：样式是 `src/styles/global.css` 的手写 CSS 变量加组件内样式，没有统一的设计体系；四个页面都是整页 Svelte 岛（`Library.svelte` 800 行、`Player.svelte` 830 行），展示与交互混在一起，难以修改。公开测试版（change `first-public-beta-ons-kirikiri`）需要一个能对外的界面。用户计划进入 `apps/` 亲自重构整个前端，本 change 为该重构提供范围、契约与验收标准。

## What Changes

- 技术栈固定为 **Astro + Svelte 5 + Tailwind CSS v4**（经 `@tailwindcss/vite` 接入）**+ daisyUI 5**（Tailwind 插件，用户 2026-10-29 批准）。不引入其他 UI 框架、组件库或状态库。
- 页面结构：外壳、导航、说明类页面改为纯 `.astro`（零 JS）；只有需要交互的部分（游戏库、播放器、设置表单）保留 Svelte 岛。
- 重新设计游戏库、播放器、设置、引擎/支持说明页的布局与交互，并新增“已知限制”说明。
- `apps/app/src/lib/platform.ts` 仍是唯一客户端组合根；组件只通过它访问 `@online-gal/core`。本 change 不修改 `packages/` 下任何包的行为。
- 现有功能 MUST 保持可用（见 `game-library-ui`、`player-ui` 的功能对等要求），重构不得静默删除功能。
- 可启动与不可启动的判断由 platform 提供，界面不硬编码引擎名单，从而兼容 beta 只开放 ONS/KiriKiri 的裁剪。

## 待用户决定

- (a) ~~视觉方向~~ 已定：瑞士主义（国际主义平面设计风格），见 `design.md` Decision 7。
- (b) 是否支持手机/平板（触屏、竖屏），还是首版只保证桌面浏览器。
- (c) 是否保留只有深色主题，还是加浅色/跟随系统。
- (d) 界面语言：只中文，还是预留多语言。
- (e) ~~路由是否变动~~ 已定（2026-10-30）：`/`、`/play?id=`、`/settings` 不变；`/engines` 并入设置并重定向到 `/settings/engines`；设置子页为 `/settings/<id>`，见 `design.md` Decision 9。
- (f) 现有用户反馈的具体问题（哪些操作难用），用于排序任务优先级。

## Capabilities

### New Capabilities

- `frontend-foundation`：技术栈、Tailwind 设计令牌、Astro/Svelte 分工、组合根与构建约束。
- `game-library-ui`：游戏库的导入、展示、筛选、详情与可运行状态呈现。
- `player-ui`：播放器外壳、控制、存档与错误呈现。
- `info-pages`：设置、引擎/支持说明、已知限制页。

### Modified Capabilities

无（`openspec/specs/` 目前为空）。

## Impact

- 代码：仅 `apps/app/`（`src/`、`astro.config.mjs`、`package.json`、新增 Tailwind 相关依赖）。`pnpm-lock.yaml` 因新依赖变化。
- 依赖：依赖包重命名已落地（`@online-gal/engine-core-*` / `@online-gal/engine-interpreter-*`）。与 `first-public-beta-ons-kirikiri` 的 2.4 任务（“本测试版不支持”文案）重叠：该文案的界面实现以本 change 为准，beta change 只提供不可用原因。
- 不影响：`packages/`、`ports/`、WASM 产物、IndexedDB schema、`astro.config.mjs` 中的隔离头与 KiriKiri/Artemis 挂载逻辑。
