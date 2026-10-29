# KiriKiri 源码导航

现行原生引擎路线是 **kirikiroid2-web**；本目录保留构建配方、vcpkg overlays、
浏览器字节源补丁及诊断工具。上游 C++ 引擎通过固定 commit 拉取，不复制到本目录。
统一布局与上传步骤见 [`source-layout.md`](../../docs/source-layout.md)。

## 固定输入

记录见 [`upstream-pins.json`](../../.github/upstream-pins.json)，
对应 workflow 的默认值也需要同步维护。

| 输入 | 当前值 |
| --- | --- |
| 上游 | [fenghengzhi/kirikiroid2-web](https://github.com/fenghengzhi/kirikiroid2-web)，`web` |
| commit | `13dda190f8370d02b6cf59286a088529b355658c` |
| 检出 | `.ref/kirikiroid2-web`；检出固定 commit 后初始化其 submodules |
| Emscripten | `6.0.9`，上游有精确版本检查 |
| vcpkg | `b1e15efef6758eaa0beb0a8732cfa66f6a68a81d`；需要完整历史，不能浅克隆 |

## 仓库内的源码

| 位置 | 职责 |
| --- | --- |
| [`kirikiroid2/overlay-ports/`](kirikiroid2/overlay-ports/) | gettext、libexif 的 portfile 和补丁；由构建入口复制到上游 overlay 位置 |
| [`patch-vlfs-host-source.mjs`](kirikiroid2/patch-vlfs-host-source.mjs) | 修改上游 web 页面，安装 `window.__onlineGalVfsSource` 字节源接缝 |
| [`run-build.sh`](kirikiroid2/run-build.sh) | 本地 Windows / Git Bash 配方；含 MinGW、WinFlexBison、Python Scripts 等本机路径 |
| [`browser-test/`](kirikiroid2/browser-test/) 及 `diag-*` / `verify-*` 工具 | 构建与取数诊断，不是另一份引擎 |
| [`engine/core/kirikiroid2-web/src/wasm-runtime.ts`](../../packages/engine/core/kirikiroid2-web/src/wasm-runtime.ts) | 统一运行时接口；以 iframe 承载引擎页面 |
| [`wasm-page.ts`](../../packages/engine/core/kirikiroid2-web/src/wasm-page.ts) / [`wasm-config.ts`](../../packages/engine/core/kirikiroid2-web/src/wasm-config.ts) | 宿主字节源接缝、页面 URL 配置及 XP3/ZIP mount 选择 |

[`krkrsdl2/`](krkrsdl2/) 是已被替代的早期移植路线，保留同步 XHR / Range 等诊断材料，
不是现行 CI 目标。本次整理不移动或删除它。

## 构建入口

**Linux CI 主入口**：[`build-krkr.sh`](../../.github/scripts/build-krkr.sh)，由
[`build-wasm-krkr.yml`](../../.github/workflows/build-wasm-krkr.yml) 调用。
CI 准备上游、submodules、emsdk 和 vcpkg，然后：

1. 固定构建版本，并从上游 commit 时间设置 `SOURCE_DATE_EPOCH`。
2. 修复 emsdk 6.0.9 的布尔 port flag，复制 overlays，安装 VLFS 字节源接缝。
3. 预热指定 Emscripten ports，再配置/构建上游 CMake preset。
4. 运行上游 web release 检查，并收集六个输出文件。

**本地 Windows 入口**：`bash ports/krkr/kirikiroid2/run-build.sh`。
它读取 [`env.sh`](../env.sh)，但仍含 Windows 工具位置和本机默认路径，
**不能当作可移植的 Linux 配方**；换机器需逐项配置，CI 使用独立脚本。
构建和补丁器会修改专用上游检出，勿指向有未保存个人修改的工作树。

## 引擎资产与 private 包

CI 输出在 `dist/krkr/`，文件为：`index.wasm`、`index.js`、`vlfs.js`、
`index.html`、`shell.html`、`assets.zip`；不作为源码提交。
已发布资产的取用入口是 `node ports/lib/fetch-engine.mjs krkr`，
哈希清单见 [`engine-artifacts.json`](../lib/engine-artifacts.json)。

[`engine/core/kirikiroid2-web/package.json`](../../packages/engine/core/kirikiroid2-web/package.json) 与 [`engine/interpreter/kirikiri-kag3/package.json`](../../packages/engine/interpreter/kirikiri-kag3/package.json) 声明
`@online-gal/engine-core-kirikiroid2-web`、`@online-gal/engine-interpreter-kirikiri-kag3` 为 private、GPL-3.0-or-later，不发布 npm。
当前 workflow **分别交付运行时 tarball 与引擎页面资产**，并未把六个引擎文件装入 tarball。
下游不必编译 wasm，但需要部署整组引擎页面文件，并用
`configureKirikiriWasm({ pageUrl })` 或运行时 `pageUrl` 指向其 `index.html`。
**只安装 tarball 不等于已有引擎页面，也不能称为完全自包含的包。**
iframe 使用同源字节源接缝；部署需遵守页面与宿主的源及隔离要求。
历史实现和测试记录见 [`kirikiri-web-plan.md`](../../docs/kirikiri-web-plan.md)。
本次源码整理不重新构建或重新验收游戏。

## 许可边界

项目自有适配代码为 GPL-3.0-or-later。kirikiroid2-web 的根 LICENSE 保留
W.Dee 与 contributors 的 BSD 风格条件以及依赖库声明；不能只根据上游 README 的 MIT 标签改写许可。
再分发源码、引擎资产及包时保留上游版权、条件与依赖声明，出处见
[`THIRD_PARTY_NOTICES.md`](../../THIRD_PARTY_NOTICES.md)。
游戏、存档、`.ref/`、下载工具链、vcpkg cache 和诊断产物不作为源码上传。
