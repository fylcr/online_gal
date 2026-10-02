# ONScripter 源码导航

本目录保存 OnscripterYuri 的构建配方、事件 shim 和浏览器诊断宿主。
**上游引擎源码通过固定 commit 拉取，不复制到本目录；游戏数据不入库。**
统一布局与上传步骤见 [`source-layout.md`](../../docs/source-layout.md)。

## 固定输入

固定输入记录在 [`upstream-pins.json`](../../.github/upstream-pins.json)，
对应 workflow 的默认值也需要同步维护。

| 输入 | 当前值 |
| --- | --- |
| 上游 | [YuriSizuku/OnscripterYuri](https://github.com/YuriSizuku/OnscripterYuri)，`master` |
| commit | `08f744b31cc1907b66a15f0402e62321a131ed81` |
| 版本 | `0.7.7beta` |
| Emscripten | `6.0.9` |
| 检出位置 | `.ref/onscripter-yuri`，允许浅克隆，无 submodule |

## 仓库内的源码

| 位置 | 职责 |
| --- | --- |
| [`build/ons_host_shim.h`](build/ons_host_shim.h) / [`ons_host_shim.cpp`](build/ons_host_shim.cpp) | 替换 worker 中无法响应 DOM 输入的 SDL 等待通道，提供宿主事件注入 |
| [`build/run-web-build-shimmed.sh`](build/run-web-build-shimmed.sh) | 现行配方；把 shim、鼠标位置接缝和诊断导出编入上游 web target |
| [`build/fetch-web-engine.sh`](build/fetch-web-engine.sh) | 优先下载并校验发布制品；无可用制品时可回退到源码构建 |
| [`harness/`](harness/) | 独立 page / worker / audio relay 和带 Range、COOP/COEP 的诊断服务器 |
| [`runtime-ons/src/`](../../packages/runtime-ons/src/) | 统一 provider、worker 宿主、字节源桥接和解释器回退 |
| [`runtime-ons/assets/`](../../packages/runtime-ons/assets/) | **已跟踪的引擎资产例外**：`onsyuri.html`、`onsyuri.js`、`onsyuri.wasm` |

[`build/run-web-build.sh`](build/run-web-build.sh) 是不带 shim 的比较路线；
[`build/build-web.sh`](build/build-web.sh) 与 [`build/scaffolding/`](build/scaffolding/)
是早期实验，不是当前 CI 主入口。保持原位置，避免破坏已有引用。

## 构建与取用

- **Linux CI 主入口**：[`build-ons.sh`](../../.github/scripts/build-ons.sh)，由
  [`build-wasm-ons.yml`](../../.github/workflows/build-wasm-ons.yml) 调用。
  CI 准备 emsdk 和上游检出，提供 `python -> python3` shim，然后运行现行配方。
- **本地源码入口**：从仓库根运行
  `bash ports/ons/build/run-web-build-shimmed.sh`。
  它通过 [`env.sh`](../env.sh) 解析 `REPO`、`REF_DIR`、`EMSDK_DIR`；
  同一配方也被 Linux CI 使用，不是仅限 Windows 的脚本。
- **取已发布引擎**：`node ports/lib/fetch-engine.mjs ons`；
  清单与哈希见 [`engine-artifacts.json`](../lib/engine-artifacts.json)。
  包装入口 `bash ports/ons/build/fetch-web-engine.sh` 可在无制品时构建；
  **校验失败退出 3 并停止，不会静默构建覆盖损坏的下载**。
- 本地 shim 配方会从 git 恢复它修改的上游文件，结束时再次恢复。
  请使用专用、无个人修改的检出；不要指向尚未保存修改的上游工作树。

输出先在上游 `build_web/`，CI 收集到被忽略的 `dist/ons/`。
`fetch-web-engine.sh --from-source` 则会覆盖已跟踪的 `runtime-ons/assets/`；
更新这些资产必须单独审查、测试和核对哈希，不属于单纯的目录整理。

## 包与运行条件

[`runtime-ons/package.json`](../../packages/runtime-ons/package.json) 声明
`@online-gal/runtime-ons` 为 private、GPL-3.0-or-later，不发布 npm。
CI 使用 `package-engine-assets: true`，将本次引擎资产和 workspace 依赖闭包装入 tarball；
运行时默认从包根 `assets/` 解析引擎文件。

worker 的同步取数需要 `SharedArrayBuffer`，部署需正确设置 COOP/COEP。
[`harness/serve.mjs`](harness/serve.mjs) 默认端口 4620，支持
`ONS_ENGINE_DIR` / `ONS_GAME_DIR`；旧默认目录不代表新源码构建的输出位置，使用时显式设置。
功能兼容性和历史测量见 [`ons-source-build.md`](../../docs/ons-source-build.md)；
**本次源码整理没有重新构建或重新验收游戏。**

## 许可边界

项目自有适配代码为 GPL-3.0-or-later。OnscripterYuri 源文件声明 GPL-2.0-or-later，
可在满足其义务的前提下按 GPL-3.0 组合；再分发引擎二进制仍需提供对应源码与版权/许可声明。
上游检出不入本仓库，不表示这些义务消失。出处与保留声明见
[`THIRD_PARTY_NOTICES.md`](../../THIRD_PARTY_NOTICES.md)。
游戏、存档、SDK、上游检出和本地诊断证据不作为源码提交。
