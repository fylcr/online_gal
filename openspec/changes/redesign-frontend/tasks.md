# Tasks

> 前端实施按现有归属推进。`file-system` 负责 4.6 的 core/VFS 目录链路，并将该任务的实现与验收结果写回本节。
> 开始第 3 组前，`proposal.md`“待用户决定”(a)(b)(e) 必须有答案。游戏库 4.5–4.8 可独立推进。

## 1. 前置

- [ ] 1.1 确认包重命名已提交，且 `apps/app/src/lib/platform.ts` 使用 `@online-gal/engine-core-*` 新包名。验证：`git status --porcelain apps/app packages` 为空；`pnpm --filter @online-gal/app build` exit 0。
- [ ] 1.2 【用户】回答 `proposal.md` 的 (a)–(f)，并写入 `design.md` Decisions。验证：`openspec validate redesign-frontend --strict` exit 0。
- [ ] 1.3 记录重构前的功能清单基线：对照 `game-library-ui`、`player-ui`、`info-pages` 的功能对等要求，在旧界面逐项确认存在。验证：清单写入 `docs/current-work.md`。

## 2. 基础设施

- [x] 2.1 在 `apps/app` 安装 `tailwindcss` 4.3.3、`@tailwindcss/vite` 4.3.3 与 `daisyui` 5.7.50（2026-10-29 完成，`astro build` exit 0）。原文：安装 `tailwindcss` 与 `@tailwindcss/vite`，在 `astro.config.mjs` 的 `vite.plugins` 中加入插件，不改动其中的隔离头与挂载逻辑。验证：`pnpm --filter @online-gal/app build` exit 0；`git diff apps/app/astro.config.mjs` 只多出插件相关行。
- [ ] 2.2 改写 `apps/app/src/styles/global.css`：`@import "tailwindcss";` 加 `@theme` 令牌（按 1.2 的视觉方向）。进展：已加入 daisyUI 插件与 `swiss` 主题，构建通过；待在浏览器确认 Tailwind preflight 未破坏旧页面外观。旧 CSS 变量暂时保留。验证：构建通过，现有页面外观不崩。
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

- [ ] 4.5 【前端】以 `getPlatform().library.list()` 替换 `MOCK_GAMES`，移除生产首页的占位数据引用。保留静态首屏外壳，实现真实列表和空状态。在右下角加入使用现有主题主色、支持键盘与触控的圆形「添加游戏」按钮。验证空库、已有游戏库、窄屏安全区、键盘打开导入入口，以及导航到游戏库外时按钮移除。
- [ ] 4.6 【file-system】验证并补齐 `Platform.importDirectory`、`FileSystemDirectorySource`、`library.openSource` 和目录 input 回退。夹具含同名不同层文件与可识别的松散资源。校验完整相对路径、文件大小、指定范围字节、句柄挂载的按需读取、刷新后的恢复、只读源和本地覆层。目录 input 须按完整路径配对文件，避免后缀匹配。校验失权及重选可恢复同一记录 ID 和存档。
- [ ] 4.7 【前端，与 file-system 联调】从「添加游戏 → 本地文件夹」接入只读目录选择器及目录 input 回退。呈现识别结果、导入失败、重新授权与重选。成功后刷新库列表。让可启动条目经 `/play?id=` 打开既有会话。目标分支若只有播放页外壳，先补这一最小启动路径。
- [ ] 4.8 【前端 + file-system】用隔离浏览器配置和合成目录验证选择、列表、进入播放、刷新后重开及失权恢复。单独验证目录 input 回退、取消或失败时保留记录、源文件字节未变、写入只在覆层。记录测试命令、浏览器版本和结果，并校验 `astro check` 与 `openspec validate redesign-frontend --strict`。真实游戏仍按 7.2 验收。

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
