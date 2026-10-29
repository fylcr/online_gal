# Spec Delta

## Purpose

约束 `apps/app` 前端重构的技术栈、样式体系、Astro/Svelte 分工与数据访问边界。

## ADDED Requirements

### Requirement: Fixed frontend stack
前端 MUST 只使用 Astro、Svelte 5 与 Tailwind CSS v4（经 `@tailwindcss/vite`）。MUST NOT 引入其他 UI 框架、组件库、全局状态库或 CSS-in-JS。站点 MUST 保持静态输出。

#### Scenario: Dependency audit
- **WHEN** 检查 `apps/app/package.json` 的 dependencies 与 devDependencies
- **THEN** 新增的前端依赖只有 `tailwindcss` 与 `@tailwindcss/vite`
- **AND** `astro build` 的输出是静态文件

### Requirement: Single design token source
颜色、圆角、阴影、字体等设计令牌 MUST 定义在 `src/styles/global.css` 的 Tailwind `@theme` 中。组件 SHALL 只用 Tailwind 类表达样式；组件内 `<style>` 只允许用于 Tailwind 无法表达的情况，且 MUST 附注释说明原因。

#### Scenario: No stray component styles
- **WHEN** 在 `apps/app/src` 中搜索 `<style`
- **THEN** 每一处都带有说明原因的注释

### Requirement: Static content ships no JavaScript
布局、导航、页脚、说明类页面与空状态 MUST 用 `.astro` 渲染且不加载客户端 JS。Svelte 岛 SHALL 只用于游戏库、播放器与设置等需要交互的区域。

#### Scenario: Info page is JS-free
- **WHEN** 构建后打开引擎/支持说明页或已知限制页
- **THEN** 页面不加载任何 Svelte 运行时脚本

### Requirement: Platform access through the composition root
组件 MUST 只通过 `src/lib/platform.ts`（或其导出的 `.svelte.ts` 状态模块）访问 `@online-gal/core` 与引擎包。本重构 MUST NOT 修改 `packages/` 下任何包的行为、IndexedDB schema 或 `astro.config.mjs` 中的隔离头与引擎挂载逻辑。

#### Scenario: No direct engine imports in components
- **WHEN** 在 `apps/app/src/components` 中搜索 `@online-gal/engine-`
- **THEN** 没有匹配

### Requirement: Build and check stay green
重构的每个阶段 MUST 保持 `pnpm --filter @online-gal/app build` 与 `pnpm --filter @online-gal/app check` 通过。

#### Scenario: Per-page milestone
- **WHEN** 任意一个页面重写完成
- **THEN** 上述两条命令 exit 0，且 `astro check` 报告 0 error
