# 引擎源码布局与 GitHub 上传边界

这份导航回答三件事：**源码在哪里、构建从哪里进入、哪些本地资料不能被顺手提交**。
本次采用原位整理：保留全部引擎修改、已有 Git 索引、游戏和诊断数据，不搬动大目录、
不重建 wasm、不把目录检查当成功能验收，也不自动提交或发布。

## 一、三套引擎的源码边界

| 引擎 | 本仓库维护的源码 | 构建时取得的引擎输入 | Linux CI 主入口 |
| --- | --- | --- | --- |
| ONScripter | [`ports/ons/`](../ports/ons/README.md) 的事件 shim、配方及 harness；[`runtime-ons`](../packages/runtime-ons/) 的浏览器适配 | 固定 commit 的 OnscripterYuri C++ 检出 | [`build-ons.sh`](../.github/scripts/build-ons.sh) |
| KiriKiri | [`ports/krkr/`](../ports/krkr/README.md) 的 VLFS 接缝、vcpkg overlays、配方；[`runtime-kag`](../packages/runtime-kag/) 的 iframe 接线及解释器回退 | 固定 commit 的 kirikiroid2-web C++ 检出及 submodules | [`build-krkr.sh`](../.github/scripts/build-krkr.sh) |
| Artemis | [`ports/artemis/src/`](../ports/artemis/src/) 的手写 C++ 产品、台账、拼接器；[`runtime-artemis/src/host/`](../packages/runtime-artemis/src/host/) 的独立现代宿主 | 已跟踪官方 asyncify wasm，经固定 WABT 转译后替换部分函数体；未替换生成体仍保留 | [`build-artemis-bridge.sh`](../.github/scripts/build-artemis-bridge.sh) |

ONS / KiriKiri 没有把整份上游 C++ 复制进本仓库，但固定输入和适配源码都在仓库中。
这不免除分发引擎二进制时的对应源码、版权及许可义务。
Artemis 当前也不能称为已脱离 SDK 的完整独立源码引擎。

固定版本见 [`upstream-pins.json`](../.github/upstream-pins.json)，实际 workflow 的默认值也要同步维护。
共享构建和包闭包装配分别见
[`build-wasm-reusable.yml`](../.github/workflows/build-wasm-reusable.yml) 与
[`make-package.mjs`](../.github/scripts/make-package.mjs)。
本地脚本不是一律跨平台：KiriKiri 的本地配方和 Artemis 的分段装配工具仍有 Windows / 本机路径。

## 二、源码交付与包可用性是两件事

| 引擎 | 当前 GitHub 构建交付形状 | 必须保留的限制 |
| --- | --- | --- |
| ONS | runtime-ons private tarball，装入本次引擎资产和 workspace 依赖闭包 | worker 同步取数需要 SharedArrayBuffer；部署须正确设置隔离响应头 |
| KiriKiri | runtime-kag private tarball + 单独的六个引擎页面资产 | tarball 未内置这六个文件；需要部署页面并配置 `pageUrl`，不是完全自包含包 |
| Artemis | runtime-artemis private tarball 的 `assets/` 放入本次 asyncify bridge.wasm | public provider 的 `load()` 仍拒绝启动，现代宿主尚未形成可用 session；有文件不等于可运行 |

全部运行时包保持 `private: true`、GPL-3.0-or-later，不发布 npm。
下游不应自行编译 wasm，但 KiriKiri 的自包含包目标及 Artemis 的现代宿主/完整游戏验收仍未完成。
官方胶水部署与 generated-body masks 仅作诊断，不能代替手写引擎出货验收。
历史下载与测试记录见 [`fetch-engine-artifacts.md`](fetch-engine-artifacts.md)；本次未重跑这些记录。

## 三、留在本机的内容

| 边界 | 处理方式 |
| --- | --- |
| `.ref/`、`.test/` | 上游检出、游戏、存档、本地套件与证据；已忽略，不删除 |
| `dist/`、`out/`、`build-final/`、编译缓存 | 构建输出，不作为源码提交 |
| Artemis `rewrite-staging/`、`rewrite-fixes/`、`.audit*`、`.work_*` | 分段工作台与审计草稿；已补充精确忽略项，保留磁盘数据 |
| 下载的 WABT、Zig、emsdk、vcpkg 二进制缓存 | 本机/CI 依赖，不是自有源码 |
| `test/` | 可随仓库交付、无需私有游戏的测试；本次上传检查器回归测试放这里 |

**已有资产例外不自动删掉：**

- [`runtime-ons/assets/`](../packages/runtime-ons/assets/) 的三个引擎资产已跟踪；更新需另行验收。
- [`artemis/vendor/artemis-sdk/`](../ports/artemis/vendor/artemis-sdk/) 的九个官方参考文件已跟踪。
  其中 [`Copyright.txt`](../ports/artemis/vendor/artemis-sdk/Copyright.txt) 对 Artemis 本体保留全部权利，
  依赖库的许可不能作为整个 SDK 的再分发授权；保留文件也不是授权已解决。

根 GPL 声明只表述本项目自有代码的许可，不能覆盖第三方 SDK，不能凭改名或手写重表达消除出处问题。
SDK、转译剩余体和行为重表达产品的再分发范围需要独立核实；本次不删除或改写第三方声明。
详细记录见 [`THIRD_PARTY_NOTICES.md`](../THIRD_PARTY_NOTICES.md)。

## 四、Artemis 产品台账与漏提交风险

[`rewrite-names.tsv`](../ports/artemis/rewrite-names.tsv) 中不是每个 `fn` 行都对应一个产品：
占位、头文件和历史映射要区分；拼接器按函数编号收集存在的产品，后面的有效映射覆盖前面的。
当前工作树有 **340 个映射函数 / 340 个 C++ 产品文件**。

以下四个新产品还没有进入当前 Git 索引，提交时必须显式纳入，不能只执行 `git add -u`：

- [`compositor_effect.cpp`](../ports/artemis/src/render/compositor_effect.cpp)
- [`compositor_hide.cpp`](../ports/artemis/src/render/compositor_hide.cpp)
- [`compositor_lydel.cpp`](../ports/artemis/src/render/compositor_lydel.cpp)
- [`lua_tags_builtin.cpp`](../ports/artemis/src/script/lua_tags_builtin.cpp)

本次只检查完整性，不装配分段修复，也不宣称全树风格或游戏行为已经通过。

## 五、上传前检查

从仓库根执行：

```bash
npm run check:layout
npm run check:sources
npm run test:sources
```

- `check:layout` 检查已跟踪适配代码不依赖本地 `.test/`。
- [`check-sources.mjs`](../scripts/check-sources.mjs) 默认读工作树：检查主入口、导航、
  C++ 台账文件存在且未忽略、三套运行时的 private/GPL 声明，以及 Git 可见路径中的工作台/二进制风险。
  未进索引的必需文件会明确警告；通过不代表待提交快照已经完整。
- `test:sources` 运行 [`check-sources.test.mjs`](../test/check-sources.test.mjs)；
  临时 Git fixture 不需要游戏、emsdk 或编译引擎，不改变当前仓库索引。

实际准备提交时，**先审查、再逐路径暂存**；不要用 `git add .` 扫入工作台，
不要覆盖已有暂存的引擎修改。导航变更、检查器、必需的新产品和原有引擎修改可以分开审查。
暂存后检查真正的索引内容：

```bash
npm run check:sources -- --staged
git diff --cached --check
git diff --cached --stat
git diff --cached
```

`--staged` 读取索引中的台账、配置和包声明；工作树后来补的文件不能替缺失的暂存内容挡错。
检查不自动暂存，不判定授权，不触发 GitHub Actions，也不替代下载 GitHub wasm 后的真实游戏测试。
既有代码的 whitespace gate 尚有失败，不能把本次文档/布局通过写成全仓库 clean。

**本次整理的完成标准：导航清晰、路径不丢、上传边界可检查。**
Artemis 冷启动修复、现代宿主接入、自包含包、提交/推送及 GitHub 制品真实游戏复验另行继续。
