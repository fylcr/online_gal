# Artemis 源码导航

本目录保存手写 C++ 引擎产品、桥接构建工具和诊断工具。浏览器侧的独立现代宿主位于
[`packages/engine/core/artemis-compat/src/host/`](../../packages/engine/core/artemis-compat/src/host/)。
整理目录不等于引擎或包已通过真实游戏验收。

## 当前出货路线与历史路线

**出货路线 = [`compat-web/`](compat-web/)**：在 **`9f622629…`** 这个 pin 上直接编译
`artemis-compat` 上游源码（GPL-3.0），加上本目录的补丁集与 overlay，得到
`artemis-compat.js` / `artemis-compat.wasm`。**不使用官方 SDK 任何字节**，不是重命名的
wasm2c 桥，也不是「旧 bridge 的改版」。CI 入口
[`build-artemis-compat.yml`](../../.github/workflows/build-wasm-artemis-compat.yml) →
[`build-artemis-compat.sh`](../../.github/scripts/build-artemis-compat.sh) →
[`compat-web/build.sh`](compat-web/build.sh)。

**历史 / 诊断路线 = `src/` + 官方 asyncify SDK + [`reverse/`](reverse/)**：把保留的官方
asyncify 模块经 wasm2c 转译后，与 `src/` 里手写产品拼接成 bridge。该路线**只保留手动
dispatch**（`build-wasm-artemis-bridge.yml` 已移除 `push` / `pull_request` 触发），因为它的
产物与"它检出的源码内容"没有关系：`skip-upstream-checkout: true` 之下，唯一的源码身份只是
`upstream_commit` 输入，所以每次推送到 `ports/artemis/src/**` 都白跑一次 30 分钟 wabt 构建与
103 MB 转译。**该路线的再分发问题仍未解决**，删除自动触发只是不再让它冒充出货路径，不是
解决许可问题，也没有删除它产物的历史 hash 记录。

## 源码与职责

| 位置 | 职责 | 是否是源码交付的一部分 |
| --- | --- | --- |
| [`compat-web/`](compat-web/) | **出货配方**：pin 归档、补丁集、host/CMake overlay、源码 offer 生成器 | 是；这是当前出货构建的全部输入 |
| [`src/`](src/) | `namespace artc` 的手写 C++ 产品；由拼接器替换对应生成函数 | 是；**历史路线**的输入，出货路线不读它 |
| [`src/util/engine_context.h`](src/util/engine_context.h) | guest 内存、Asyncify 状态及跨产品调用接口 | 是 |
| [`rewrite-names.tsv`](rewrite-names.tsv) | 官方函数编号、语义名称和产品路径的台账 | 是；按函数编号最后一行生效，不把重复历史行当产品数 |
| [`REWRITE.md`](REWRITE.md) | 命名、代码风格和重写约束 | 是；约束存在不表示全树已通过 |
| [`reverse/rw-splice.py`](reverse/rw-splice.py) | 在生成层中替换函数体并生成调用跳板 | 是；历史路线 |
| [`reverse/build-bridge-route.sh`](reverse/build-bridge-route.sh) | 编译桥接 wasm，默认 `ARTC_OPT_LEVEL=-O1` | 是；历史路线 |
| [`tools/assemble-rewrite-parts.py`](tools/assemble-rewrite-parts.py) | 本地分段装配与结构审计；不是 clean-clone 构建依赖 | 是；当前工具仍有本机路径默认值 |
| [`reverse/test-rw-splice-diagnostics.py`](reverse/test-rw-splice-diagnostics.py) | 拼接器诊断回归测试 | 是；合成测试不替代游戏验收 |
| [`harness/glue/`](harness/glue/) | 已记录的 ABI 导入分类与身份校验工具 | 是；只描述 SDK 桥的导入面，**对出货 compat 模块无效** |
| [`build/`](build/) / [`tools/`](tools/) / [`harness/`](harness/) | 本地部署、分析和诊断入口；包含历史路线 | 保留，勿当作另一条出货路线 |
| [`vendor/artemis-sdk/`](vendor/artemis-sdk/) | 已跟踪的官方参考二进制和版权声明 | **单独许可风险；不由本项目 GPL 声明覆盖；出货路线不读** |

本次工作树台账对应 **340 个映射函数 / 340 个 C++ live 产品文件**，所有路径均存在。
占位和头文件行不算产品；后面的有效映射覆盖前面的同编号映射。
其中四个新产品尚未进入 Git 索引：
[`compositor_effect.cpp`](src/render/compositor_effect.cpp)、
[`compositor_hide.cpp`](src/render/compositor_hide.cpp)、
[`compositor_lydel.cpp`](src/render/compositor_lydel.cpp)、
[`lua_tags_builtin.cpp`](src/script/lua_tags_builtin.cpp)。提交时必须显式检查它们，不能只提交已跟踪文件。
`npm run check:sources` 检查产品是否存在且未被忽略，但不会自动暂存或证明行为正确。
这 340 个产品属于**历史路线**的台账，**不是出货构建的要求**；把两者混在一起的旧表述已撤回。

## 构建入口

- **出货（Linux CI 主入口）**：[`build-artemis-compat.sh`](../../.github/scripts/build-artemis-compat.sh)。
  由 [`build-wasm-artemis-compat.yml`](../../.github/workflows/build-wasm-artemis-compat.yml)
  调用，复用 [`build-wasm-reusable.yml`](../../.github/workflows/build-wasm-reusable.yml)，
  Emscripten **6.0.9**。workflow 检出 pin 到 `.ref/artemis-compat`（`skip-upstream-checkout: false`），
  并把同一个 pin 以 `ARTC_PIN` 传给脚本，脚本断言 `ARTC_REF == UPSTREAM_PATH` 后才归档。
  产物 `dist/artemis-compat/`：`artemis-compat.js`、`artemis-compat.wasm`、
  `artemis-compat-source.tar.gz`(+`.sha256`)、`LICENSE`、`THIRD_PARTY_NOTICES.md`。
  **`publish: false`**，直到真实游戏验收；**不设 `glue-import-map`**（那张表是 SDK 桥的导入面）。
- **本地出货入口**：`bash ports/artemis/compat-web/build.sh -j2`（见
  [`compat-web/README.md`](compat-web/README.md)），产物在 `ports/artemis/compat-web/out/build/`。
- **历史桥接入口**：`build-artemis-bridge.sh`；仅接受 `BRIDGE_FLAVOUR=asyncify`，
  用固定 WABT commit `02418f72a70466b45805891843f114c7f602eb34` 从
  [`vendor` 中的 asyncify wasm](vendor/artemis-sdk/WebAssembly/asyncify/artemis.wasm)
  重建 wasm2c 输入。**只能手动 dispatch**，不是出货路径。
- 底层脚本保留的 `sync` 参数依赖未跟踪的另一份重写树；它是历史诊断入口，
  **不属于当前支持的 CI 或出货目标**。
- 输出写入被忽略的 `dist/` 或 `compat-web/out/`，不混入 C++ 源码提交。

## 浏览器包与当前验收边界

- [`engine/core/artemis-compat`](../../packages/engine/core/artemis-compat/)（`@online-gal/engine-core-artemis-compat`） 是 private workspace 包，不发布 npm。
  [`src/index.ts`](../../packages/engine/core/artemis-compat/src/index.ts) 目前导出 PFS / SHA-1 / probe / provider；
  [`src/compat/`](../../packages/engine/core/artemis-compat/src/compat/) 是 compat 模块的 worker 与页侧读数宿主。
- **当前验收边界（不得含糊）**：compat 引擎**已编译**，worker 与宿主使用 **SharedArrayBuffer
  镜像**解决同步读。**音频已不再是上游非 Android 分支的 stub**（旧文写 `PlayStream` 返回 false）：
  `overlay/audio/audio_web.cpp` 提供本地 **PCM pull** 后端，页面按 voice 拉取交错立体声 int16 并自行
  送入 WebAudio，且**已编译**。但**主线程才刚刚开始接线，尚未在真实游戏里听见过声音**。
  `artc_web_save` / `artc_web_load` 的 checkpoint 语义也仍未验证：写入的是 ARCV 变量库检查点、
  依赖游戏的 `onSave` / `onLoad`，**不是 BOWS 导出、也不是被恢复的场景**。
  因此 **不能说这个构建"可玩"**：编译通过只证明能编，不证明游戏行为。
- CI 的 `real-game` 作业在 compat 路线上**没有**配置（它是 bridge workflow 的历史部分）。
  完整冷启动游戏验收仍待完成，不应标成 runnable。页面侧存档经 `runtimePersistQueue` 串行化，
  覆盖 checkpoint 与整个 `await store`，**不并发覆盖**，但**不是 atomic**；stored 多次写入亦非原子。
- 官方 glue 部署和 generated-body masks 只用于**历史路线**的差分诊断，不可冒充出货 compat 模块。

## 本地资料与许可

游戏、`.ref/` 检出、`.test/` 证据、`rewrite-staging/`、`rewrite-fixes/`、
下载工具链和编译产物留在本机，不作为源码上传；本次整理不删除它们、不装配新引擎。

项目自有代码声明 GPL-3.0-or-later。**出货 compat 模块是上游 GPL-3.0 源码的编译产物**，
其对应源随制品一起提供（`artemis-compat-source.tar.gz` = pin 的 `git archive` + 本目录补丁集与
overlay），捆绑的 Lua 5.1（MIT）、stb（MIT/公有领域）、zlib（zlib 许可）各自的声明记录在
[`THIRD_PARTY_NOTICES.md`](../../THIRD_PARTY_NOTICES.md)。
官方 SDK 的 [`Copyright.txt`](vendor/artemis-sdk/Copyright.txt) 开头保留作者全部权利；
其中依赖库的许可不能视为 Artemis 本体的再分发授权。**SDK、其转译体和行为重表达产品的授权边界
（历史路线）仍需独立核实**，不能因为改了名称或附上 GPL 就认定已解决。
当前不删除参考材料、不改写其许可，也不默认发布。详细出处见
[`THIRD_PARTY_NOTICES.md`](../../THIRD_PARTY_NOTICES.md)。

统一导航与上传检查步骤见 [`源码布局指南`](../../docs/source-layout.md)。
