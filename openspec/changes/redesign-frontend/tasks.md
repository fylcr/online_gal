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

- [x] 3.4 【frontend】固定普通页面顶部导航栏与底部页脚。通过共享布局协调高度、safe-area、内容滚动留白、竖屏 dock 和「添加游戏」按钮，保留现有网格、面包屑和 `/play` 的 `bare` 模式。
- [x] 3.5 【frontend】在导航栏右端添加「搜索游戏」和汉堡菜单入口。汉堡菜单复用现有导航链接及当前栏目状态，并完成点击、触控、键盘、Escape、点击外部和 swup 切页的打开/关闭流程。
- [x] 3.6 【frontend】为游戏库接通按名称、标题、标签筛选的本地搜索。从首页直接聚焦，从其他普通页切到首页后聚焦，支持迟挂载。区分空库与无匹配，清空后恢复完整列表。不得用搜索词发起网络查询或写入 URL。
- [x] 3.7 【frontend】在桌面横屏、窄屏竖屏和带安全区的浏览器条件验证首页、设置、设置子页与 `/play`。使用超出一屏的内容检查两栏不移动、首尾内容与键盘焦点不被遮挡，dock、添加按钮和菜单不重叠。覆盖菜单焦点、反复切页、站内和跨页搜索、空库与无匹配。验证静态外壳不阻塞首屏、`astro check`、站点前端构建与 OpenSpec 严格校验；在本节记录浏览器、命令和结果。

### 固定外壳与本地搜索的 Chromium 验收

在隔离工作树运行 `npm run build:packages`，再用 `pnpm --filter @online-gal/app exec astro dev --host 127.0.0.1 --port 4358` 启动应用。将 `LIBRARY_BROWSER_TOOLS` 指向安装了 Playwright 的目录，运行 `node test/frontend-shell-browser.e2e.mjs`。可用 `CHROMIUM_PATH` 指定浏览器、`ONLINE_GAL_SHELL_URL` 覆盖服务地址。

- Chromium 156.0.8078.4 在 1280×800 横屏、320×640 触屏竖屏及注入安全区的条件下通过。禁用 JavaScript 时，普通页首屏仍显示静态头栏、标题和页脚。长页滚动时两栏固定，末尾的焦点不被遮挡；窄屏安全区变化时，dock、添加按钮和菜单不重叠。`/play` 不显示普通外壳。
- 验证菜单当前栏目、键盘和触控打开、Tab、Escape 焦点恢复、点击外部关闭及反复 Swup 切页。搜索从首页或设置页进入时均聚焦输入框；拦住 Svelte 组件加载直到切页结束，放行后仍能聚焦。名称、标题和标签过滤、空库、无匹配及清空均通过，输入搜索词时不发请求也不改变 URL。
- `pnpm --filter @online-gal/app check` 通过（19 个文件，0 错误、0 警告、0 提示）；`pnpm --filter @online-gal/app build`、`openspec validate redesign-frontend --strict` 和 `node test/local-library-browser.e2e.mjs` 通过。

### 游戏库页面收敛（弹窗占位）

按后续要求，只改游戏库页面，设置页和播放页不动。右下角圆形「添加游戏」按钮和页面内搜索框已去除，游戏库页除导航栏和 dock 外不再有按钮。导航栏右侧按 daisyUI 的 `btn btn-ghost btn-circle` 放置搜索和汉堡按钮。搜索打开 daisyUI 默认 `<dialog class="modal">` 占位；汉堡按钮为 daisyUI `dropdown dropdown-end` 下拉菜单，下拉内容为占位元素。两者之后重新设计。空库在整个浏览器视口正中显示无框、无按钮的「暂无游戏」（`fixed inset-0` 居中，放在 `@container` 区块之外，避免容器包含块改变定位基准）。卡片上的「游戏详情」按钮已去除，右键菜单保留原样，长按与菜单重设计暂不做。删除确认同样换成 daisyUI 占位弹窗，暂不执行删除。

内容底边距不再为已删除的添加按钮留空：横屏只给页脚留位，竖屏额外给 dock 留位。

- Chromium 156.0.8078.4 运行 `node test/frontend-shell-browser.e2e.mjs` 通过。1280×800 横屏与 320×640 触屏竖屏下，导航栏和页脚保持固定，禁用 JavaScript 时首屏仍显示静态头栏和页脚。空库「暂无游戏」的中心与视口中心在横屏和竖屏下偏差均不超过 1px，且无导入按钮；游戏库页面本身无可见按钮，也没有 `#game-search` 和「添加游戏」按钮。搜索按钮用键盘打开占位弹窗，Escape 关闭。汉堡按钮不再打开弹窗，聚焦或点击后在按钮下方展开占位下拉，竖屏下不超出 320px 视口；点击页面其他位置或 Swup 切页后收起。反复 Swup 切页、长页滚动焦点不被页脚遮挡、`/play` 不显示普通外壳均通过。
- `pnpm --filter @online-gal/app check` 通过（19 个文件，0 错误、0 警告、0 提示）。`node test/local-library-browser.e2e.mjs` 依赖已移除的「添加游戏」入口和导入弹窗，在导入弹窗重新设计前暂时不能跑。

### 页脚文字对齐调整

**变更前：** 页脚文字使用 `.site-content` 类，桌面横屏左边距为 `calc(7.75rem + safe-left)`（与标题对齐），手机竖屏左边距为 `calc(1.75rem + safe-left)`（与 logo 对齐）。右边距桌面为 `max(1.5rem, safe-right)`，手机同。两端距离在不同屏幕宽度下为固定像素值，不随页面宽度缩放。

**变更后：** 页脚文字使用独立的 `.site-footer-text` 类，左右边距均为 `calc(100% / 12)`，即页面宽度的十二分之一。桌面和手机统一，不再区分横竖屏。页脚不再与 navbar logo 或标题对齐，而是按页面宽度的固定比例留白。

### 导航栏 logo、标题与页脚的比例对齐

记页面宽度为 `P`，页脚左缘 `F = P / 12`，logo 宽度为 `w`（`size-10` = 2.5rem，`≥640px` 时 `size-12` = 3rem）。

**优先方案（标题与页脚对齐）：** 标题左缘落在 `F`；logo 到页面边缘的距离等于 logo 到标题的距离，即 logo 居中于页边距，`logo-left = (F - w) / 2`，两段间距均为 `d = (F - w) / 2`。

**优先方案的成立条件：** 需要 `d > 2/3·w`，化简为 `F > 7/3·w`（页面宽度 `> 1344px`）。不成立时回退到下列三档，标题与 logo 的间距恒为 `2/3·w`，logo 按 `F` 与 `w` 的比例贴合页脚：

| 页面宽度 | 条件 | logo 对齐方式 | `logo-left` | 标题间距 |
| --- | --- | --- | --- | --- |
| `≤480px` | `F ≤ w` | 左缘对齐页脚 | `F` | `2/3·w` |
| `481–1152px` | `w < F ≤ 2w` | 中线对齐页脚 | `F - w/2` | `2/3·w` |
| `1153–1344px` | `F > 2w` | 右缘对齐页脚 | `F - w` | `2/3·w` |
| `≥1345px` | `F > 7/3·w` | 居中于页边距（优先方案） | `(F - w)/2` | `(F - w)/2` |

断点取阈值像素加一，因为规则用的是「大于」而非「大于等于」。`global.css` 中以 `--logo-w`、`--logo-left`、`--title-gap` 三个变量实现上述四档，`.site-content`（正文与面包屑）的左边距为 `logo-left + w + title-gap`，因此正文跟随标题对齐。`--logo-left` 内含 `--site-safe-left`。

**曾出现的缺陷：** 标题原本嵌在一个 `flex min-w-0 items-center` 的包装 div 内，`margin-left` 中的百分比按**包含块**宽度解析，该 div 宽度仅 96px，`--title-gap` 算出 `-20px`，标题压到 logo 上。修复方式：把 logo 与标题提为 `.site-inline` 的直接子元素，`.site-inline` 不再设水平内边距，使其内容宽等于视口宽；按钮组改用 `margin-left: auto` 与右侧外边距。同时移除了 `.site-inline` 上的 `gap-2`，避免额外 8px 破坏间距计算。

**导航栏 logo 改用原图：** 此前用 Astro 的 `<Image>` 组件，构建时把 1254×1254 的原图缩到 48/96px 并重新编码，显示发糊。现改为直接引用 `public/logo.webp` 原文件（`<img src="/logo.webp">`），构建不再处理图片，产物 `dist/logo.webp` 与原文件字节一致。

**bsk 浏览器验收（Chromium 156，`emulate` 直接设定视口）：**

- 四档规则与表中公式逐一比对均通过（容差 0.6px）：1440px 与 1400px 命中优先方案，1300px 右缘对齐，1000px 与 500px 中线对齐，400px 与 320px 左缘对齐。
- 优先方案下 logo 到页边与 logo 到标题的距离相等（1440px 均为 35.99px），标题与页脚的横向偏差 0.02px。
- 断点边界 480/481/1152/1153/1344/1345px 全部与公式一致，无跳变错档。
- 正文与面包屑左边距在每个断点上都等于标题左边距（120/131/100px）。
- `/`、`/settings/audio/` 与 320px 竖屏下均无横向溢出。空库「暂无游戏」在每个测试宽度下都精确居中于视口（`vw/2`）。
- 键盘 Tab 顺序为 logo → 搜索 → 汉堡 → dock（游戏库），焦点元素全部可见且不被遮挡。搜索按钮触发 `dialog.showModal()`，汉堡聚焦后下拉展开（`visibility: visible`，宽 208px）。`/play?id=nope` 保持 bare 外壳：无页脚、无 `#swup`、无 navbar logo，显示「游戏不存在」。
- `pnpm --filter @online-gal/app check`（19 个文件，0 错误、0 警告、0 提示）、`pnpm --filter @online-gal/app build`（14 页）与 `openspec validate redesign-frontend --strict` 均通过。
### dock 提示、dock 左缘与汉堡按钮边距

**变更前：** dock 每项只有一个 `size-10` 图标按钮，名称靠 daisyUI `tooltip` 在悬停/聚焦时浮出（`landscape:tooltip-right portrait:tooltip-top`），`data-tip` 存 `SectionNav` 的 `label`。dock 左缘为 `calc(1.5rem + var(--site-safe-left))`，与导航栏 logo 无关，因此在宽屏下 dock 明显偏左于 logo。导航栏按钮组右外边距为 `max(1rem, var(--site-safe-right))`，与 logo 的左外边距不是同一个值。

**变更后（最终状态）：** dock 保持图标 + daisyUI 悬停提示（`landscape:tooltip-right portrait:tooltip-top`，`data-tip` 取 `SectionNav` 的 `label`），项高仍为 `size-10`，滑块步长两个方向都是 `2.75rem`，`--site-dock-size` 维持 `3rem`。

> 中途曾一度把提示改成常驻标签（图标上、文字下），项间距随之变成横屏 `3.75rem`、`--site-dock-size` 改为 `4rem`；按用户后续要求已完全回退。`[SUPERSEDED by tooltip revert]`

dock 左缘改为 `left: var(--logo-left)`，与导航栏 logo 的左缘取同一个值，因此横屏下 dock 容器左边与 logo 左边完全重合。竖屏 dock 仍按设计在底部居中（`left: calc((100% + safe-left - safe-right) / 2)`），不参与左对齐。

导航栏按钮组的右外边距改为 `var(--logo-left)`，即汉堡按钮到页面右缘的距离等于 logo 到页面左缘的距离。

标签使竖屏 dock 从 3rem 高变成 4rem，`--site-dock-size` 随之由 3rem 改为 4rem（`p-1` 0.5rem + 图标 2.5rem + 标签 1rem），竖屏内容底边距变量 `--site-content-end` 自动跟着变大。

**bsk 浏览器验收（Chromium 156，`emulate` 直接设定视口）：**

- dock 左缘与 logo 左缘之差在横屏 1440/1300/1000px 下均为 **0.00px**（两者分别落在 35.99/60.32/59.32px）。
- 汉堡按钮右缘距页面右缘与 logo 左缘距页面左缘完全相等（`insetMatch` = 0.000），实测 1440px 为 35.99 vs 35.99，1300px 为 60.32 vs 60.32，1000px 为 59.32 vs 59.32，500/400/320px 竖屏为 21.66/33.32/26.66 对同值。
- 每个 dock 项「标签顶 − 图标底」= 0.00px（紧贴下方），标签水平中心与图标中心之差 = 0.00px；两个标签文本为 `游戏库`、`设置`。
- 滑块高亮在切到 `/settings` 后落在第二项图标上（`hlOnIcon` = true，`translate: 0px 60px`，即 `--dock-i` 1 × 3.75rem）。竖屏 `translate: 44px`（1 × 2.75rem）。
- 竖屏 iPhone-14（390×844）下 dock 高 64px = 4rem，底部距页脚 8px = `--site-dock-gap`，滚动底部留白 132px = 页脚 48 + 8 + dock 64 + 12。
- 反复 Swup 切换 `/` ↔ `/settings` 四次，dock 始终只有一个实例，标签与边距不变，`aria-current` 与滑块同步更新。
- 各宽度无横向溢出；搜索按钮仍触发 `dialog.showModal()`（`:modal` 为 true），Escape 可关闭；汉堡聚焦后下拉展开（`visibility: visible`，宽 208px，右缘与按钮右缘对齐）。
- `pnpm --filter @online-gal/app check`（19 个文件，0 错误、0 警告、0 提示）、`pnpm --filter @online-gal/app build`（14 页）均通过。

**验收时的一个测量陷阱：** bsk 的 Agent Window 标签页处于后台（`document.hidden` 为 true），合成器不推进过渡，读取 `translate` 会拿到停在 `currentTime: 0` 的过渡起始值，造成「滑块不动」的误判。脚本内先 `await new Promise(r => setTimeout(r, 450))` 再读取即可拿到终值。同理，`emulate` 切换设备后立刻截图可能拿到未绘制的空帧，logo 会整块发白；延时重拍即正常。

### 外壳沾满视口（`100vh` → 动态视口）

**变更前：** 最外层两级（`<body>` 与 `body > div`）都用 Tailwind `min-h-screen`，即 `min-height: 100vh`。桌面浏览器上 `100vh` 总是等于窗口高度，但在移动端/桌面端 URL 栏伸缩或 Windows 缩放时，`100vh` 取的是「大视口」，与当前可见高度不一致，表现为页面不沾满或底部被浏览器 UI 遮住。且播放页已用 `h-dvh`，外壳与它不一致。

**变更后：** 两处改为 `min-h-dvh`（`100dvh`，跟随动态视口），并给 `body > div` 补上 `w-full`，让它不依赖父元素块级默认宽度就能沾满横向。桌面下 `dvh` 与 `vh` 等价，布局像素不变。

**实测（Chromium 156，无设备仿真，视口 1693×869）：** `html`/`body`/`body > div` 三者均为 `1693.33 × 869.33`，即完全等于视口；`min-height` 解析为 `869.333px`；`scrollWidth - clientWidth` 与 `scrollHeight - clientHeight` 均为 **0**；`<html>`/`<body>`/`body > div` 三者均无 `style` 内联属性；`getComputedStyle(html).zoom` 与 `visualViewport.scale` 均为 `1`。

### 当前元素布局安排（最终状态，除 dock 与 navbar 外页面内无按钮）

**记号：** W = 视口宽，F = W/12（页脚文字距两侧边缘的距离），w = logo 宽（W<640px 为 2.5rem，否则 3rem）。横屏 = 宽 > 高（桌面），竖屏 = 高 ≥ 宽（手机）。外壳变量定义在 `html.site-shell`（`apps/app/src/styles/global.css`）。

**层级（自上而下）：** `html.site-shell` > `body.min-h-dvh` > `div.flex.min-h-dvh.w-full.flex-col`，其下依次为：

1. `header[data-site-header]`：`fixed inset-x-0 top-0 z-30`，下边 1px 细线，上内边距 `--site-safe-top`。内层 `.site-inline`（`flex`，行高 `--site-header-row`：W<640 为 4.5rem，否则 5rem，无水平 padding）从左到右为：`.site-logo`（`margin-left: --logo-left`）、`h1.site-title`（`margin-left: --title-gap`）、`.site-actions`（`display: contents`，不生成盒子，搜索按钮与汉堡按钮直接成为 `.site-inline` 的 flex 子项；按图标而非按钮定位，见下文「导航栏右侧图标定位」），含搜索按钮和汉堡按钮（daisyUI `btn-ghost btn-circle`，40×40，搜索打开占位 `dialog#site-search`，汉堡为 `dropdown-end` 占位菜单）。
2. `div#breadcrumbs.site-content`：swup 容器，`padding-top: --site-header-height`，无面包屑时为空。
3. `Dock`：`fixed z-20`，在 `#swup` 之外，跨页保持挂载。
4. `main#swup.site-content`：`flex-1 w-full`，`pt-6`，`pb: --site-content-end`。
5. `footer[data-site-footer]`：`fixed inset-x-0 bottom-0 z-10`，内层 `.site-footer-text` 高 `--site-footer-row`（3rem），上边 1px 细线，`padding-inline: W/12`，文字 `GPL-3.0-or-later`。
6. `dialog#site-search`：daisyUI modal 占位。

**水平对齐规则（`--logo-left` / `--title-gap`）：**

| 视口宽 W | logo 左缘 | 标题与 logo 间距 | 标题/内容左缘 |
|---|---|---|---|
| ≤480px | F（logo 左缘贴 F） | 2.5rem×2/3≈26.67px | F+66.67px |
| 481–639px | F−w/2（logo 中心在 F） | ≈26.67px | F+46.67px |
| 640–1152px | F−w/2 | 32px | F+56px |
| 1153–1344px | F−w（logo 右缘贴 F） | 32px | F+32px |
| ≥1345px | (F−w)/2 | (F−w)/2 | F（与页脚文字左缘对齐） |

以上均再加 `--site-safe-left`。`.site-content` 的 `padding-left = logo-left + w + title-gap`，所以标题、面包屑、主内容共用同一条左边线；右内边距为 `max(1.5rem, --site-safe-right)`。汉堡**图标**右缘到页面右缘等于 `--logo-left`，搜索**图标**右缘到汉堡图标左缘也等于 `--logo-left`，即右侧两个图标的间距与 logo 到左缘的距离一致（见下文「导航栏右侧图标定位」）。

**竖直安排：** 页头高 `--site-header-height = 行高 + safe-top + 1px`；页脚高 `--site-footer-height = 3rem + safe-bottom`；`--site-content-end`：横屏为页脚高 + 1rem，竖屏为页脚高 + `--site-dock-gap`(0.5rem) + `--site-dock-size`(3rem) + 0.75rem。

**dock：** 图标 `size-10`、容器 `p-1`，名称用 daisyUI `tooltip`（横屏 `tooltip-right`，竖屏 `tooltip-top`）。横屏为竖排，`top:50%` 垂直居中，`left: --logo-left`（与 logo 左缘重合）；竖屏为横排，贴底并浮在页脚上方（`bottom: 页脚高 + 0.5rem`），水平居中。

**空库提示：** `[data-empty-library]` 为 `fixed inset-0 grid place-items-center`，居中文字，无边框、无按钮。

**实测（Chromium 156，W=1679、H=775，横屏，F=139.92px）：** `--logo-left = (F−3rem)/2`，logo 左缘 45.97px；标题左缘 139.94px（与 F 差 0.02px）；logo 与标题间距 45.97px；汉堡按钮右缘距页面右缘 45.64px（与 logo 左距的 0.33px 差来自 1679.33px 的小数视口宽）；dock 左缘 45.97px，与 logo 左缘差 0；dock 48×92px；`#swup` 左内边距 139.938px、右 24px、上 24px、下 64px；页头高 80.67px，页脚高 48px；`docH = clientH = 775`，横向溢出 0。

### 导航栏右侧图标定位（按 svg 图标而非按钮）

**变更前：** `.site-actions` 是 `flex` 容器，`margin-right: var(--logo-left)`，搜索与汉堡是两个相邻的 40×40 `btn-circle`，按钮之间没有间距。量到的是按钮盒子：汉堡按钮右缘到页面右缘 = `--logo-left`；而 20px 的图标在按钮内左右各留 0.625rem 空白，所以图标实际距右缘 = `--logo-left` + 0.625rem，搜索图标右缘到汉堡图标左缘只有 1.25rem。

**变更后：** 定位点改为 svg 图标边缘。`.site-actions` 改为 `display: contents`，两个按钮直接成为 `.site-inline` 的 flex 子项，`--logo-left` 里的百分比因此与 logo 一样按整幅视口宽的页头解析。`--btn-pad: 0.625rem`（btn-circle 2.5rem 减 size-5 1.25rem 再除以 2）。规则：第一个按钮 `margin-left: auto; margin-right: max(0px, var(--logo-left) - 2 * var(--btn-pad))`，最后一个按钮 `margin-right: max(0px, var(--logo-left) - var(--btn-pad))`，两个按钮 `flex: none` 保持 40px 点击区域，不会被压缩。`max(0px, …)` 是为极窄视口兜底，避免两个按钮重叠。下拉菜单仍是 `dropdown-end`，另加 `margin-inline-end: var(--btn-pad)`，让菜单右缘对齐汉堡**图标**右缘而不是按钮右缘。

**实测（Chromium 156，视口 1679.33×775，横屏）：** `--logo-left` = 45.97px。汉堡图标 1613.36–1633.36px，右缘距页面右缘 45.97px（`innerWidth` 取整成 1679 时读数为 45.64px，差的 0.33px 来自取整）。搜索图标 1547.4–1567.4px，右缘到汉堡图标左缘 45.97px，与 logo 左缘一致。两个按钮仍为 40×40，标题宽 72px，横向溢出 0。聚焦汉堡后下拉菜单宽 208px，右缘 1633.36px，与图标右缘差 0。`astro check` 0 错误、0 警告、0 提示。后台标签页下过渡不推进，菜单 `opacity` 会读到起始值 0，但不影响几何位置。

### Edge 复现「页面缩成一团」的结论

在 Edge（bsk 实例 7e2c2b96）打开 `http://127.0.0.1:4358/`，CSS 视口只有 170×176，`devicePixelRatio` 为 7.5（Chrome 中为 1.5），即该站点在 Edge 里被页面缩放到约 500%，页面按极窄屏布局：`h1` 宽 0、被 `wrap-anywhere` 压成逐字竖排（高 67.5px），整页高 213px 大于视口 176px，dock 压在内容上，看起来缩成一团。用户确认这是 Edge 页面缩放造成的，与代码无关，`Ctrl+0` 重置缩放即可。为应对该缩放曾试加标题 `truncate`，已撤销，`h1` 仍为 `wrap-anywhere`。上一节「外壳沾满视口」对 `100vh` 的分析并不是这一症状的原因；`min-h-dvh` 仍保留，因其在桌面下与 `100vh` 等价、没有副作用。

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
