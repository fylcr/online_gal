# Spec Delta

## Purpose

约束 `apps/app` 前端重构的技术栈、样式体系、Astro/Svelte 分工与数据访问边界。

## ADDED Requirements

### Requirement: Fixed frontend stack
前端 MUST 只使用 Astro、Svelte 5、Tailwind CSS v4（经 `@tailwindcss/vite`）与 daisyUI 5。MUST NOT 引入其他 UI 框架、组件库、全局状态库或 CSS-in-JS。站点 MUST 保持静态输出。

#### Scenario: Dependency audit
- **WHEN** 检查 `apps/app/package.json` 的 dependencies 与 devDependencies
- **THEN** 新增的前端依赖只有 `tailwindcss`、`@tailwindcss/vite` 与 `daisyui`
- **AND** `astro build` 的输出是静态文件

### Requirement: Single design token source
颜色、圆角、阴影、字体等设计令牌 MUST 定义在 `src/styles/global.css` 中：颜色、圆角、边框与深度放 daisyUI 自定义主题 `swiss`（`@plugin "daisyui/theme"`），字体等其余令牌放 Tailwind `@theme`。组件 SHALL 只用 Tailwind 类表达样式；组件内 `<style>` 只允许用于 Tailwind 无法表达的情况，且 MUST 附注释说明原因。

#### Scenario: No stray component styles
- **WHEN** 在 `apps/app/src` 中搜索 `<style`
- **THEN** 每一处都带有说明原因的注释

### Requirement: Static shell renders without JavaScript
布局、导航、页脚、说明类页面与静态空状态 MUST 用 `.astro` 渲染，静态外壳 MUST NOT 依赖客户端 JS 才能显示。Svelte 岛 SHALL 只用于游戏库、播放器与设置等交互区域。导航增强仅可复用现有切页逻辑及轻量原生脚本，不新增外壳岛。

#### Scenario: Static shell and information pages do not depend on hydration
- **WHEN** 用户在客户端脚本尚未运行时打开普通页面
- **THEN** 导航栏、页脚与静态内容已经可见
- **AND** 说明页不会为了外壳加载 Svelte 运行时脚本

### Requirement: Navigation bar and footer remain fixed

在采用普通外壳的页面，导航栏 MUST 固定在视口顶部、页脚 MUST 固定在视口底部；两者在内容滚动时保持可见。内容布局 MUST 为两栏、安全区及现有悬浮控件预留空间，不遮挡可滚动内容或键盘焦点。全屏播放器 SHALL 继续使用不含两栏的 `bare` 布局。

#### Scenario: Scroll a long page in desktop and portrait views
- **WHEN** 用户在横屏或窄屏竖屏滚动普通页面并以键盘导航至末尾
- **THEN** 导航栏和页脚仍留在视口边缘，首尾内容和聚焦项不被遮挡
- **AND** 竖屏 dock 与游戏库的「添加游戏」按钮不会被页脚覆盖或彼此重叠

#### Scenario: Enter and leave the full-screen player
- **WHEN** 用户从游戏库进入 `/play` 再返回
- **THEN** 播放器内不显示两栏，返回后恢复普通外壳与原有导航

### Requirement: Navbar search and hamburger menu

普通外壳 MUST 在导航栏右上角依次显示「搜索游戏」和汉堡菜单控制。控制必须有可访问名称、可见焦点和触控目标。菜单 MUST 提供现有站点导航，显示当前栏目，并与现有 dock 共存。

#### Scenario: Open and dismiss the navigation menu
- **WHEN** 用户通过键盘或触控打开汉堡菜单
- **THEN** 用户可访问游戏库和设置链接，设置子页仍标记「设置」为当前栏目
- **AND** 选择链接、按 Escape 或点击外部可以关闭菜单；用 Escape 关闭时焦点回到菜单按钮
- **AND** swup 切页后不会保留过期菜单或重复绑定交互

#### Scenario: Use the search control
- **WHEN** 用户在普通页面激活「搜索游戏」
- **THEN** 已在游戏库时聚焦搜索框；不在游戏库时进入游戏库并在搜索框可用后聚焦

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
