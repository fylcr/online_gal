# Tasks

> 全部任务未勾选。前端实施由用户在 `apps/` 中亲自进行；代理只在用户要求时协助。
> 开始第 3 组前，`proposal.md`“待用户决定”(a)(b)(e) 必须有答案。

## 1. 前置

- [ ] 1.1 确认包重命名已提交，且 `apps/app/src/lib/platform.ts` 使用 `@online-gal/engine-core-*` 新包名。验证：`git status --porcelain apps/app packages` 为空；`pnpm --filter @online-gal/app build` exit 0。
- [ ] 1.2 【用户】回答 `proposal.md` 的 (a)–(f)，并写入 `design.md` Decisions。验证：`openspec validate redesign-frontend --strict` exit 0。
- [ ] 1.3 记录重构前的功能清单基线：对照 `game-library-ui`、`player-ui`、`info-pages` 的功能对等要求，在旧界面逐项确认存在。验证：清单写入 `docs/current-work.md`。

## 2. 基础设施

- [ ] 2.1 在 `apps/app` 安装 `tailwindcss` 与 `@tailwindcss/vite`，在 `astro.config.mjs` 的 `vite.plugins` 中加入插件，不改动其中的隔离头与挂载逻辑。验证：`pnpm --filter @online-gal/app build` exit 0；`git diff apps/app/astro.config.mjs` 只多出插件相关行。
- [ ] 2.2 改写 `apps/app/src/styles/global.css`：`@import "tailwindcss";` 加 `@theme` 令牌（按 1.2 的视觉方向）。旧 CSS 变量暂时保留。验证：构建通过，现有页面外观不崩。
- [ ] 2.3 新建 `src/lib/*.svelte.ts` 共享状态模块（游戏库列表、设置），只经 `src/lib/platform.ts` 访问 core。验证：`astro check` 0 error。

## 3. 外壳与说明页

- [ ] 3.1 重写 `src/layouts/Layout.astro`（头部、导航、页脚）为 Tailwind，无客户端 JS。验证：构建后首页 HTML 的外壳部分不引用 Svelte 脚本。
- [ ] 3.2 新增已知限制页（`src/pages/limitations.astro` 或按 (e) 的路由），内容来自 `README.md`。验证：页面零 JS；游戏库与不支持提示能链接到它。
- [ ] 3.3 重写引擎/支持说明页：静态说明用 `.astro`，可用性列表用一个小 Svelte 岛读取 `registry.list()`。验证：满足 `info-pages` 的 “Engine support page reflects the build”。

## 4. 游戏库

- [ ] 4.1 拆分 `Library.svelte` 为 `src/components/library/` 下的小组件（导入区、工具栏、卡片、详情、提示）。验证：单文件约 300 行以内；`astro check` 0 error。
- [ ] 4.2 实现可运行三态（可开始/不支持/未识别），状态只来自 platform。验证：`grep -rn "artemis\|siglus\|tyrano" apps/app/src/components/library` 无引擎名单硬编码。
- [ ] 4.3 实现空库、搜索无结果、初始化失败、浏览器能力不足提示。验证：在无跨源隔离的 `astro dev`（临时去头）下看到提示，验证后恢复。
- [ ] 4.4 删除旧 `Library.svelte` 与其专用 CSS。验证：`game-library-ui` 功能对等清单逐项通过；构建通过。

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
