# ports · 原生引擎源码与构建入口

这里保存 ONS、KiriKiri 与 Artemis 的移植代码、构建配方和诊断工具。
**目录整理不是功能验收或发布授权。** 完整布局与上传步骤见
[`源码布局指南`](../docs/source-layout.md)。

| 引擎 | 仓库内的主要内容 | 引擎输入 | 详细导航 |
| --- | --- | --- | --- |
| ONScripter | 事件 shim、构建配方、worker harness | 固定 commit 的 OnscripterYuri 检出 | [`ons/README.md`](ons/README.md) |
| KiriKiri | VLFS 字节源补丁、vcpkg overlays、构建配方 | 固定 commit 的 kirikiroid2-web 检出 | [`krkr/README.md`](krkr/README.md) |
| Artemis | 手写 C++ 产品、函数台账、wasm2c 拼接器 | 官方 asyncify wasm + 手写产品；仍保留未替换生成体 | [`artemis/README.md`](artemis/README.md) |

ONS / KiriKiri 的浏览器宿主分别在
[`runtime-ons`](../packages/runtime-ons/) 和 [`runtime-kag`](../packages/runtime-kag/)。
Artemis 的目标只有 **asyncify 手写引擎 + 独立现代宿主**，宿主源码在
[`runtime-artemis/src/host/`](../packages/runtime-artemis/src/host/)；
当前 public provider 尚不能启动游戏，官方胶水部署仅作诊断，不是另一条出货路线。

## 主构建入口

| 引擎 | Linux CI 脚本 | workflow | 输出 |
| --- | --- | --- | --- |
| ONS | [`build-ons.sh`](../.github/scripts/build-ons.sh) | [`build-wasm-ons.yml`](../.github/workflows/build-wasm-ons.yml) | `dist/ons/` 的引擎资产 + runtime-ons tarball |
| KiriKiri | [`build-krkr.sh`](../.github/scripts/build-krkr.sh) | [`build-wasm-krkr.yml`](../.github/workflows/build-wasm-krkr.yml) | `dist/krkr/` 的页面资产 + 独立 runtime-kag tarball |
| Artemis | [`build-artemis-bridge.sh`](../.github/scripts/build-artemis-bridge.sh) | [`build-wasm-artemis-bridge.yml`](../.github/workflows/build-wasm-artemis-bridge.yml) | asyncify `bridge.wasm` + runtime-artemis tarball；可用性未验收 |

共享流程见 [`build-wasm-reusable.yml`](../.github/workflows/build-wasm-reusable.yml)，
private 包的依赖闭包与许可装配见 [`make-package.mjs`](../.github/scripts/make-package.mjs)。
KiriKiri 当前需另行部署页面资产、配置 `pageUrl`；不能把其 tarball 当作自包含引擎包。

本地脚本和 CI 入口并非全部等价：ONS 的 shimmed 配方也被 Linux CI 使用，
而 KiriKiri 的本地 `run-build.sh` 含 Windows 工具和本机默认路径。
[`env.sh`](env.sh) 与 [`lib/ref.mjs`](lib/ref.mjs) 提供共享路径解析，
**不代表每个历史脚本都无绝对路径或在任意机器可直接运行**。

## 固定输入与本地资料

固定版本记录在 [`upstream-pins.json`](../.github/upstream-pins.json)，
workflow 的默认 commit 也须同步维护。当前 Emscripten 为 **6.0.9**。

| 检出 / 工具 | 固定 commit | 使用位置 |
| --- | --- | --- |
| OnscripterYuri `master` | `08f744b31cc1907b66a15f0402e62321a131ed81` | `.ref/onscripter-yuri` |
| kirikiroid2-web `web` | `13dda190f8370d02b6cf59286a088529b355658c` | `.ref/kirikiroid2-web`；初始化 submodules |
| vcpkg | `b1e15efef6758eaa0beb0a8732cfa66f6a68a81d` | KiriKiri 依赖；完整历史 |
| artemis-compat | `9f622629f595db85e6f76c45c06edde29d7d9961` | 只读结构/命名参考，不是当前桥接 CI 编译的上游源码 |
| WABT | `02418f72a70466b45805891843f114c7f602eb34` | 从官方 Artemis asyncify wasm 重建转译输入 |

`.ref/`、`.test/`、编译输出、下载工具链、分段重写工作台与游戏/存档只留本机。
例外必须明确：

- [`runtime-ons/assets/`](../packages/runtime-ons/assets/) 已跟踪三个引擎资产，更新需单独验收。
- [`artemis/vendor/artemis-sdk/`](artemis/vendor/artemis-sdk/) 已跟踪官方 SDK 参考文件，
  **其再分发授权尚未核实，不由根 GPL 声明覆盖**。

早期 `krkr/krkrsdl2/`、ONS scaffolding、Artemis sync 和官方胶水路线保留作历史诊断，
本次不搬动、不删除，也不提升为当前支持路线。

## 源码与许可检查

- `npm run check:layout`：检查已跟踪移植代码不依赖本地 `.test/`。
- `npm run check:sources`：检查源码台账、主入口、private 包声明与提交路径边界。
- `npm run check:sources -- --staged`：读取 Git 索引，要求实际待提交内容包含完整导航和 live 产品。

检查不自动暂存，不构建 wasm，不运行游戏，也不判定再分发授权。
项目自有代码为 GPL-3.0-or-later；上游源码和二进制须保留各自版权/许可并履行对应源码义务。
Artemis SDK 与其行为重表达产品的边界需要单独核实，详见
[`THIRD_PARTY_NOTICES.md`](../THIRD_PARTY_NOTICES.md)。
