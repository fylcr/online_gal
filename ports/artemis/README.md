# Artemis 源码导航

本目录保存手写 C++ 引擎产品、桥接构建工具和诊断工具。浏览器侧的独立现代宿主位于
[`packages/runtime-artemis/src/host/`](../../packages/runtime-artemis/src/host/)。
目标出货形态只有 **asyncify 手写引擎 + 独立宿主**；官方 SDK / minified glue 不是出货宿主。
整理目录不等于引擎或包已通过真实游戏验收。

## 源码与职责

| 位置 | 职责 | 是否是源码交付的一部分 |
| --- | --- | --- |
| [`src/`](src/) | `namespace artc` 的手写 C++ 产品；由拼接器替换对应生成函数 | 是；保留所有现有产品与修改 |
| [`src/util/engine_context.h`](src/util/engine_context.h) | guest 内存、Asyncify 状态及跨产品调用接口 | 是 |
| [`rewrite-names.tsv`](rewrite-names.tsv) | 官方函数编号、语义名称和产品路径的台账 | 是；按函数编号最后一行生效，不把重复历史行当产品数 |
| [`REWRITE.md`](REWRITE.md) | 命名、代码风格和重写约束 | 是；约束存在不表示全树已通过 |
| [`reverse/rw-splice.py`](reverse/rw-splice.py) | 在生成层中替换函数体并生成调用跳板 | 是 |
| [`reverse/build-bridge-route.sh`](reverse/build-bridge-route.sh) | 编译桥接 wasm，默认 `ARTC_OPT_LEVEL=-O1` | 是 |
| [`tools/assemble-rewrite-parts.py`](tools/assemble-rewrite-parts.py) | 本地分段装配与结构审计；不是 clean-clone 构建依赖 | 是；当前工具仍有本机路径默认值 |
| [`reverse/test-rw-splice-diagnostics.py`](reverse/test-rw-splice-diagnostics.py) | 拼接器诊断回归测试 | 是；合成测试不替代游戏验收 |
| [`harness/glue/`](harness/glue/) | 已记录的 ABI 导入分类与身份校验工具 | 是；不等同于官方胶水出货 |
| [`build/`](build/) / [`tools/`](tools/) / [`harness/`](harness/) | 本地部署、分析和诊断入口；包含历史路线 | 保留，勿当作另一条出货路线 |
| [`vendor/artemis-sdk/`](vendor/artemis-sdk/) | 已跟踪的官方参考二进制和版权声明 | **单独许可风险；不由本项目 GPL 声明覆盖** |

本次工作树台账对应 **340 个映射函数 / 340 个 C++ live 产品文件**，所有路径均存在。
占位和头文件行不算产品；后面的有效映射覆盖前面的同编号映射。
其中四个新产品尚未进入 Git 索引：
[`compositor_effect.cpp`](src/render/compositor_effect.cpp)、
[`compositor_hide.cpp`](src/render/compositor_hide.cpp)、
[`compositor_lydel.cpp`](src/render/compositor_lydel.cpp)、
[`lua_tags_builtin.cpp`](src/script/lua_tags_builtin.cpp)。提交时必须显式检查它们，不能只提交已跟踪文件。
`npm run check:sources` 检查产品是否存在且未被忽略，但不会自动暂存或证明行为正确。

## 构建入口

- **Linux CI 主入口**：[`build-artemis-bridge.sh`](../../.github/scripts/build-artemis-bridge.sh)。
  由 [`build-wasm-artemis-bridge.yml`](../../.github/workflows/build-wasm-artemis-bridge.yml)
  调用；仅接受 `BRIDGE_FLAVOUR=asyncify`，使用 Emscripten **6.0.9**。
- CI 从 [`vendor` 中的 asyncify wasm](vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm)
  用固定 WABT commit `02418f72a70466b45805891843f114c7f602eb34` 重建 wasm2c 输入，
  校验后拼接当前 `src/`。因此当前桥接构建**仍依赖官方二进制及未替换的生成函数体**，
  不是脱离 SDK 的完整独立源码引擎。
- **本地桥接入口**：从仓库根运行
  `bash ports/artemis/reverse/build-bridge-route.sh asyncify <outdir>`。
  需要已准备的 `ARTEMIS_OFFICIAL` 转译树、export shims、wasm2c runtime 与 Emscripten；
  裸 clone 不能只执行这一行。完整准备过程见 CI 脚本。
- 底层脚本保留的 `sync` 参数依赖未跟踪的另一份重写树；它是历史诊断入口，
  **不属于当前支持的 CI 或出货目标**。
- 输出写入被忽略的 `dist/` 或 `reverse/out/`，不混入 C++ 源码提交。

`artemis-compat` 的参考 commit 为 `9f622629f595db85e6f76c45c06edde29d7d9961`，
记录于 [`桥接 workflow 的参考输入`](../../.github/workflows/build-wasm-artemis-bridge.yml)。
当前 CI 设置 `skip-upstream-checkout: true`，并不编译该参考检出的完整引擎源码。

## 浏览器包与当前验收边界

- [`runtime-artemis`](../../packages/runtime-artemis/) 是 private workspace 包，不发布 npm。
  [`src/index.ts`](../../packages/runtime-artemis/src/index.ts) 目前导出 PFS / SHA-1 / probe / provider，
  **没有导出完整现代宿主 session**。
- [`host/boot.ts`](../../packages/runtime-artemis/src/host/boot.ts) 提供导入表和启动基础设施；
  其存在不是 public provider 已能启动游戏的证明。
- [`runtime.ts`](../../packages/runtime-artemis/src/runtime.ts) 中 `load()` 仍抛出
  `UnsupportedEngineError`。仅调用 `registerArtemisEngineHost` 不能补齐实际运行路径。
- CI 使用 `package-engine-assets: true` 把本次 `bridge.wasm` 装入包的 `assets/`。
  **有字节不等于即拿即用**；现代宿主集成、完整冷启动游戏验收，以及 GitHub 构建产物下载后
  的真实游戏复验仍待完成。当前手写引擎冷启动失败，不应标成 runnable。
- 官方 glue 部署和 generated-body masks 只用于差分诊断，不可冒充手写引擎或现代宿主验收。

## 本地资料与许可

游戏、`.ref/` 检出、`.test/` 证据、`rewrite-staging/`、`rewrite-fixes/`、
下载工具链和编译产物留在本机，不作为源码上传；本次整理不删除它们、不装配新引擎。

项目自有代码声明 GPL-3.0-or-later。官方 SDK 的
[`Copyright.txt`](vendor/artemis-sdk/Copyright.txt) 开头保留作者全部权利；其中依赖库的许可
不能视为 Artemis 本体的再分发授权。现有 SDK、其转译体和行为重表达产品的授权边界需独立核实，
不能因为改了名称或附上 GPL 就认定已解决。当前不删除参考材料、不改写其许可，也不默认发布。
详细出处见 [`THIRD_PARTY_NOTICES.md`](../../THIRD_PARTY_NOTICES.md)。

统一导航与上传检查步骤见 [`源码布局指南`](../../docs/source-layout.md)。
