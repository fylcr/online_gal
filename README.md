# Online Gal · 浏览器环境的线上视觉小说模拟器

一个**运行在浏览器中**的多引擎视觉小说平台。导入本地游戏文件夹或压缩包，平台识别引擎并选择运行时；是否能启动取决于该运行时及引擎资产是否可用。游戏文件不上传，存档写入浏览器本地数据库。

## 源码、构建与当前交付边界

本仓库原位维护三套原生引擎的适配源码与构建入口，未把 ONS / KiriKiri 的完整上游检出复制入库。
详细目录、上传清单与检查方式见 [`源码布局指南`](docs/source-layout.md)。

| 引擎 | 源码导航 | 当前边界 |
| --- | --- | --- |
| ONScripter | [`ONS 导航`](ports/ons/README.md) | 固定上游源码 + 事件 shim / worker 宿主；CI private 包包含引擎资产；需要跨源隔离 |
| KiriKiri | [`KiriKiri 导航`](ports/krkr/README.md) | 固定上游源码 + VLFS 补丁 / overlays；页面资产与 runtime-kag tarball 分开交付，须部署并配置 `pageUrl` |
| Artemis | [`Artemis 导航`](ports/artemis/README.md) | 默认出货路线是 **compat 上游源码编译成 WASM**（provider 默认走它）；页面需跨源隔离与实际引擎配置，旧 SDK 桥只留显式 legacy 诊断；**全真游戏验收尚未过** |

包保持 private / GPL-3.0-or-later，不发布 npm。**目录完整、构建成功、合成测试通过、真实游戏可用和再分发授权是不同结论。**
本次仅整理导航和上传边界，保留现有源码、Git 索引及本地资料，不重新构建、不运行游戏、不提交或推送。
下文保留历史实现和测量；当前 Artemis 状态以上述边界及其导航为准。

参考并移植了两套移动端模拟器的架构思想：

| 参考项目 | 借鉴内容 |
| --- | --- |
| [AetherKiri](https://github.com/AetherKiri/AetherKiri) | **单一播放器宿主 + 版本化运行时提供者（provider）调度**：一个宿主承载多个引擎运行时，探测（probe）返回匹配分值，调度器按分值 → 优先级 → 注册顺序选出唯一运行时；导入的游戏只读挂载，运行时写入重定向到独立可写层。 |
| [Tyranor-Next](https://github.com/Weiss-UltimateSavior/Tyranor-Next) | **引擎识别表**（`.xp3` / `startup.tjs` / `nscript.dat` / `tyrano/` / `js/rmmz_core.js` 等特征文件）、`engine/` 与 `app/` 的分层职责，以及「内置 Web 运行环境」承载 TyranoBuilder 与 RPG Maker MV/MZ 的思路。 |

与原项目最大的差别是宿主替换：AetherKiri 的宿主是 Godot，Tyranor-Next 是 Android/Kotlin；本项目的宿主是 **Canvas 2D + iframe + Web Audio**，全部逻辑用 TypeScript 实现。

---

## 支持的引擎

| 引擎 | 识别特征 | 运行时 | 状态 |
| --- | --- | --- | --- |
| KiriKiri / KAG3 | `data.xp3`、`startup.tjs`、`Config.tjs`、`first.ks`、`System.tjs` | **KiriKiri2 的 WASM 构建**（优先）+ 自研 KAG3 解释器与 TJS2 源码解释器（回退） | ✅ 可运行（WASM 需构建已发布；明文脚本可回退到解释器） |
| ONScripter | `nscript.dat`、`nscript.___`、`nscr_sec.dat`、`onscript.nt2/nt3`、`.nsa` | **OnscripterYuri 的 WASM 构建**（优先）+ 自研 NScripter 命令解释器（回退） | ✅ 可运行 |
| TyranoBuilder | `tyrano/`、`data/scenario/first.ks`、`tyrano/plugins/kag/kag.js` | 内置 Web 运行时 + 桌面导出兼容 shim | ✅ 可运行 |
| RPG Maker MV / MZ | `js/rpg_core.js`、`js/rmmz_core.js`、`www/` 布局 | 内置 Web 运行时 | ✅ 可运行 |
| 通用 HTML5 / NW.js | `index.html`、`app.asar` | 内置 Web 运行时 | ✅ 可运行 |
| SiglusEngine | `Scene.pck`、`Gameexe.ini`/`Gameexe.dat`（含 `Data/` 布局） | 已接入识别与配置读取；执行需 `siglus_rs` 的 WebAssembly 构建 | ⚠️ 仅识别 |
| ArtemisEngine | `root.pfs`、`system.ini`、`boot.ini`、`system/first.iet`、`root.pfs.NNN` 补丁包 | 识别 / PFS 读取 / SHA-1 / provider + **compat 源码 WASM 引擎**（classic worker，主线程 tick） | ⚠️ compat 引擎**已编译**并有合成测试；**全真游戏验收尚未过**，不标「可玩」 |
| Ren'Py | `.rpa`、`game/script.rpy`、`renpy/` | — | 仅识别（无探针） |

「仅识别」的引擎会被记录在游戏条目上，导入后**明确提示不可运行**，而不是给出一个点了没反应的启动按钮。
Artemis 的 `availability` 是**实时 getter**：未通过 `configureArtemisCompat()` 指定 compat 引擎时求值为
`unavailable`，已配置且页面满足能力前提时才是 `runnable`；它**不再**根据 host 列表静态求值。
[`ArtemisRuntime.load()`](packages/runtime-artemis/src/runtime.ts) 的旧无条件 `UnsupportedEngineError`
已撤下：`load()` 默认返回 **compat 源码 WASM 会话**，旧的 asyncify SDK 桥只有在游戏显式按名字要求
（`gameConfig.wasm` / `wasmUrl`）时才被选中，且那是**诊断路径而非默认回退**。
「默认可运行」仍有**实际前提**：页面必须 `crossOriginIsolated === true`（含 `SharedArrayBuffer`、
`OffscreenCanvas`）并通过 `configureArtemisCompat()` 给出实际引擎配置；缺任一项仍诚实地报
`unavailable`。**前提满足不等于游戏已验证** —— 全真游戏验收尚未过，不得读成已可玩。Siglus 尚无
可用的平台执行路径。

> 官方 Artemis wasm / minified glue 的历史游戏测试与 generated-body 诊断，不证明手写引擎、
> 独立现代宿主或 public package 已通过真实游戏验收，也不是另一条出货路线。

### 自研解释器没有实现的部分（本项目最大的限制之一）

**这不是引擎移植，是脚本层的重新实现。** 这一点必须说清楚：

KiriKiri2 是 C++ 引擎（参考实现 Kirikiroid2 的 `src/` 约 7.2 MB，仅 TJS2 子系统就包含
`tjsInterCodeGen.cpp` 110 KB、`tjsInterCodeExec.cpp` 92 KB、`tjsLex.cpp` 46 KB、
`tjsVariant.cpp` 31 KB 等数十个文件）。本项目 `packages/runtime-kag/src/tjs/` 全部加起来
约 81 KB TypeScript，**不可能**等价于它。实际做的事情是：

| 层 | 是否重写 | 说明 |
| --- | --- | --- |
| KAG3 命令语言（`.ks` 的 `[tag]`） | ✅ 重写 | KAG3 本身只是一套脚本层，没有原生实现，命令语义可完整重写 |
| TJS2 **源码**语法子集 | ✅ 重写 | 树遍历解释器，覆盖 KAG 脚本常用的语法 |
| TJS2 **字节码**格式读取与反汇编 | ✅ 完成 | 容器、操作码表、宽度表、反汇编，全部有回归测试 |
| TJS2 **字节码 VM** | ❌ 无 | 能读、能反汇编，但**不能执行**；编译型 `.ks` 会被明确拒绝 |
| TJS2 完整对象模型 | ❌ 部分 | 无 `property` 访问器语义、命名空间、完整标准库 |
| TVP 原生插件（`loadplugin`） | ❌ 无 | 需要 `.dll`/`.so`，浏览器里没有对应物 |
| 原生引擎的 WASM 移植 | 分引擎验收 | ONS / KiriKiri 有历史真实游戏证据；Artemis 的官方诊断不等于手写引擎或现代宿主可用；Siglus 尚无可用构建 |

因此**下面这条限制针对的是本项目的解释器，而不是平台整体**：解释器的兼容性预期应当是
「简单到中等的 KAG3 游戏、且脚本为明文」。以下情况会失败（且会明确报错，不会静默）：

- `.ks` 为编译型（TJS2 字节码）——没有 VM，运行时**拒绝加载**而不是当作空脚本播放；
- 脚本大量依赖 TJS2 高级特性（自定义类层次、`property` 访问器、原生插件）；
- 使用 `[trans rule=]` 遮罩转场、`[movie]`、自定义 shader。

其余限制：

- **XP3 加密包**：只处理 XOR 混淆（`info` flag `0x80000000`），需从 `startup.tjs` 取得密钥。
- **视频播放**：`[movie]`、`mpegplay` 等命令被识别但跳过，并记入兼容性报告。

---

## 快速开始

```bash
pnpm install
pnpm dev          # 构建 packages 后启动 Astro 开发服务器（http://127.0.0.1:4321）
```

生产构建与预览：

```bash
pnpm build        # 构建全部 packages + 静态站点（apps/app/dist）
pnpm preview
```

本地完整套件（依赖未入库的工作台与游戏；历史用例数不是本次整理的验收结果）：

```bash
pnpm test
```

> `pnpm test` 需要本地的 `.test/` 工作台，而它**不在仓库里**（见下文「测试」）。因此刚克隆下来的
> 仓库跑 `pnpm test` 会**明确报错退出**，而不是打一个 0 用例的绿色结果——空 glob 下
> `node --test` 会以 0 退出，那种「假通过」比失败更糟。要跑测试，先把 `.test/` 与 `.ref/` 恢复。

不依赖私有游戏的源码组织检查可直接运行：

```bash
npm run check:layout
npm run check:sources
npm run test:sources
```

常规引擎构建输出不进源码仓库；已有 ONS 资产与 Artemis SDK 输入是明确保留的例外。
CI **按引擎分别构建**，artifact 与可选 release 发布分开处理；不要求下游自己编译 wasm。
KiriKiri 页面资产需单独部署；Artemis 的 compat 引擎**已编译**并走 provider 默认路径，但页面需跨源
隔离与引擎配置，且**全真游戏验收尚未过**。
取用与历史校验见 [`制品下载指南`](docs/fetch-engine-artifacts.md)：

```bash
node ports/lib/fetch-engine.mjs ons          # 下载 + 校验 sha256 + 落位
```

> GitHub 构建、下载校验与游戏测试的历史记录见制品指南；它们不证明当前未提交源码或 Artemis
> 现代宿主已验收。本次整理不派发 workflow、不发布 release。fetch 的退出码 2 表示制品不可得；
> ONS 获取入口可在此类不可用情形回落源码构建，但退出码 3（校验失败）必须停止，不能悄悄回落。

类型检查：

```bash
pnpm check        # tsc -b（全部 packages）+ astro check
```

### 部署注意

- 站点是**纯静态**的，可直接部署到任意静态托管。
- ONS worker 的同步取数桥依赖 `SharedArrayBuffer`，需要跨源隔离。设置 `Cross-Origin-Opener-Policy: same-origin` 与适用的 `Cross-Origin-Embedder-Policy`（如 `credentialless`；开发服务器已配置），并核对引擎页面及资源的响应头。不能把隔离头当作可有可无的未来优化。

---

## 架构

```
                    ┌──────────────────────────────────────────┐
                    │  apps/app  (Astro 静态站点 + Svelte 岛)   │
                    │  游戏库 / 游玩页 / 设置 / 引擎面板         │
                    └────────────────────┬─────────────────────┘
                                         │
                    ┌────────────────────▼─────────────────────┐
                    │  @online-gal/core                        │
                    │  游戏库 · 导入 · 存档 · 设置 · 会话启动    │
                    └────────────────────┬─────────────────────┘
                                         │
        ┌────────────────────────────────▼────────────────────────────────┐
        │  @online-gal/engine-api   —— 宿主与运行时之间的契约              │
        │  RuntimeProvider { probe, create } · RuntimeInstance · RuntimeHost│
        │  RuntimeRegistry（分值 → 优先级 → 注册顺序 的调度器）             │
        │  选项表 · 兼容性统计（CommandTracker）                            │
        └───┬──────────┬──────────┬──────────┬──────────┬────────────────┘
            │          │          │          │          │
   ┌────────▼───┐ ┌────▼─────┐ ┌──▼──────┐ ┌─▼───────┐ ┌▼──────────┐
   │ runtime-   │ │ runtime- │ │ runtime-│ │ runtime-│ │ runtime-  │
   │ kag        │ │ ons      │ │ siglus  │ │ artemis │ │ web/tyrano│
   │ KAG3 + WASM│ │ ONS  WASM│ │ 识别+INI│ │ PFS+WASM│ │ HTML5 内嵌 │
   └────────┬───┘ └────┬─────┘ └──┬──────┘ └─┬───────┘ └┬──────────┘
            │          │          │          │          │
            │          │          └──────────┴──────────┘
            │          │                     │
            │     ┌────▼─────────────────────▼────┐
            └────►│  @online-gal/tjs2-bytecode    │
                  │  操作码表 · 宽度表 · 分类分析  │
                  └───────────────────────────────┘
                                   │
        ┌──────────────────────────▼──────────────────────────┐
        │  @online-gal/player-host                            │
        │  Stage（图层合成 · 过渡 · 震动）· TextRenderer（排版 ·  │
        │  逐字显示 · 注音）· AudioEngine · GameLoop · 输入翻译 · │
        │  手势队列 · 宽屏几何                                  │
        └──────────────────────────┬──────────────────────────┘
                                   │
        ┌──────────────────────────▼──────────────────────────┐
        │  @online-gal/vfs          @online-gal/image-codec    │
        │  FileSystem / FileList /  ZIP · XP3 · NSA/NS2/SAR ·   │
        │  IndexedDB / Overlay /    ASAR · PFS 读取器 ·          │
        │  Composite 数据源          TGA 解码 · 原生格式回退      │
        └─────────────────────────────────────────────────────┘
```

### 核心概念

**运行时提供者（RuntimeProvider）** 是唯一的扩展点，对应 AetherKiri 的 `engine_runtime_provider_v1_t`：

```ts
interface RuntimeProvider {
  readonly runtimeId: string;
  readonly displayName: string;
  readonly priority: number;              // 同分值时的优先级
  readonly optionKeys?: readonly OptionKey[];   // 本运行时支持的调优项
  readonly containers?: readonly string[];      // 可读取的归档格式
  readonly availability?: RuntimeAvailability;  // 能否真正启动游戏
  probe(game: ProbeContext): Promise<ProbeResult | null>;  // 纯函数，不得抛错
  create(host: RuntimeHost, options: RuntimeOptions): RuntimeInstance;
}
```

`probe` 返回 0 表示「不是我的」，正数表示匹配分值。调度器按 **分值 → 优先级 → 注册顺序** 选唯一赢家，与 AetherKiri 的平局链一致。

**`availability`** 是 UI 唯一无法自行推断的信息：探针能回答「这是什么引擎」，但无法回答「现在能不能跑」。声明 `unavailable` 的提供者会让游戏库显示「引擎不可用」而不是一个点了会失败的启动按钮。

**游戏数据源（GameSource）** 把「游戏内容从哪来」抽象成一个只读接口，五种实现：File System Access 目录句柄、`FileList`（目录选择器 / 拖放）、IndexedDB 存档、归档读取器适配、多层 Composite。KiriKiri 游戏的 `data.xp3` 就是作为额外一层挂载到松散文件之上的。

**可写覆盖层（OverlayGameSource）** 保证导入的游戏永远只读：引擎写入 `savedata/` 之类的路径被重定向到以游戏为命名空间的 IndexedDB，与原项目「游戏目录只读 + 应用私有目录存档」的划分一致。

### 跨引擎适配层（移植自 AetherKiri `bridge/engine_api`）

AetherKiri 的 `bridge/` 目录合计约 1.45 MB，其中 `engine_api/`（361 KB）是不含具体引擎的适配骨架，
`godot_extension/`（600 KB）是 Godot 表现层。后者在浏览器里没有对应物——`Stage` + `Presenter`
已经承担了同样的职责——所以真正需要移植的是 `engine_api/` 这一层，它已经落在
`@online-gal/engine-api` 里：

| AetherKiri 概念 | 本项目对应 | 说明 |
| --- | --- | --- |
| `engine_runtime_provider_registry` | `RuntimeRegistry` | 分值 → 优先级 → 注册顺序的派发 |
| `engine_option_t` 键值对 | `options.ts` | **键名逐一照搬**，使原项目的按游戏调优笔记仍然有效 |
| provider 的 `probe` 返回分值 | `RuntimeProvider.probe` | 0 = 不匹配，正数 = 匹配 |
| 可选 `engine_runtime_host_v1_t` 服务 | `RuntimeHost` | 日志、时钟、设置、请求重绘 |
| 插件调试信息 | `CommandTracker` + `CompatibilityReport` | 见下 |

**兼容性报告**是浏览器端对 AetherKiri 插件调试信息的替代：原生构建会用 mock 对象吞掉加载失败的插件
并记录下来（`mock_enabled` 选项控制是否关闭这种吞掉行为）；浏览器里没有插件可 mock，等价信号就是
「引擎发出、但运行时没能实现的脚本命令」。因此：

- 每个会话统计 honoured / skipped / approximated 三类计数；
- `strict_mode=true` 把跳过升级为 error（对应 `mock_enabled=0`）；
- `compatibility_report=false` 完全关闭统计；
- 报告结论分三档：`clean`（全部支持）/ `degraded`（有降级替换）/ `unsupported`（有命令被跳过），
  在游玩页菜单里可见，并附具体命令、次数与行号。

这条能力解决的是「游戏能启动」和「游戏能正常工作」之间的差别——之前只能靠肉眼判断。

**GPU 相关选项没有移植**：`renderer` / `render_backend` / `frame_output` 用于在原项目中
在 Godot 自有 GPU 资源、外部 GPU 渲染目标和 CPU 回读之间选择；浏览器里运行时直接画进宿主
canvas，只有一条路径，没有可选项可表达。

### 已移植的适配层组件

| AetherKiri 源文件 | 本项目实现 | 移植时保留的语义 |
| --- | --- | --- |
| `engine_input_queue_gate.h` | `player-host/input-gate.ts` | 忙碌时只缓存**完整手势**（down+up 成对）、上限 8 个、超出丢最新的而不是最旧的；失焦/卸载时丢弃未完成手势，避免「按下不放」被重放 |
| `onscripter_presentation.h` | `player-host/presentation.ts` | `ons.wide` 包保持 4:3 脚本坐标、只按**更宽**的宽高比裁剪顶部；更窄的请求不裁剪，以免丢内容 |
| `onscripter_save_storage.cpp` | `runtime-ons/save-storage.ts` | `save<NN>.dat` 与 `envdata`/`gloval.sav`/`kidoku.dat` 等固定状态文件的识别；旧布局存档**不覆盖**迁移到 `savedata/` |
| `engine_options.h` | `engine-api/options.ts` | 选项键名逐一照搬，附带类型/范围/enum 校验 |
| 插件调试信息 | `engine-api/compat.ts` | honoured / skipped / approximated 计数与三档结论 |

浏览器里**不需要**移植的部分：`PrimaryClickQueueGate` 中针对 Godot「同一次物理按下被全局回调与
Control 回调各报一次」的去重（`attachInput` 只绑一个监听器，不存在这个问题，但去重逻辑仍保留为通用
保护）；`PrepareSaveStorage` 中「优先写游戏目录旁的 `savedata`」——浏览器导入的目录按设计只读，
可写覆盖层始终承担这个角色。

### 二进制归档读取：正在统一为浏览器可用的分层文件系统

**目标**：把所有归档读取（XP3 / NSA / NS2 / SAR / PFS / ZIP）统一到一套**惰性**抽象上，使文件在
浏览器里也能按需读取，而不是先整体载入内存。

**为什么必须做**：不这么做，WASM 运行时就必须把整个游戏塞进 Emscripten 的虚拟文件系统。对
GB 级归档（你的 ONS 游戏有 1.8 GB 和 1.2 GB 两个）这是硬伤。这条判断**已经被两条实测路线修正**：
KiriKiri 与 ONScripter 现在都通过宿主字节源接缝按 Range 喂数据（ONS 实测只读了 1.8 GB 归档里的
0.56 MB，KiriKiri 实测真实 XP3 只读了 11.32%）。也就是说**按需加载的关键不在归档读取器，
而在宿主与 WASM 之间的取数接缝**；这套分层文件系统仍然是平台侧（非 WASM）路径的统一抽象。
同样地，自研 TS 路径在真实 ONS 游戏上 `load()` 就耗尽 4 GB 内存。

**已有的基础**：`GameSource`（统一目录/文件视图，含 `readRange`）与 `ByteSource`（随机访问）
两个接口已经在 `engine-api` / `vfs` 里，归档读取器也都能给出条目偏移。缺的是把「压缩/未压缩」的
差别暴露出来，让调用方能对未压缩条目直接做区间读取。

**本轮进展（NSA 格式）**：

真实档案（1.8 GB，17525 个条目）实测确认了两点，此前都是错的：

| | 之前的假设 | 实测 |
| --- | --- | --- |
| 字节序 | 小端 | **大端**（引擎用 `buf[0] << 8 \| buf[1]` 读） |
| 头部结构 | 4 字节计数 + 独立偏移表 | **`u16be` 计数 + `u32be` 基址 + 从字节 6 开始的扁平记录表** |

小端解读会把 17525 读成 30020、把基址读成 GB 级数字——这正是旧读取器在真实档案上**解析出 0 个
条目**的原因。改用大端后，同一档案能解析出**全部 17525 个条目、名称全部正确**，耗时约 60 ms。

**仍未确定**：记录的字段归属。名称补零到 20 字节后，还有 13 字节要分给「两个 32 位长度 + 压缩
字节 + 32 位偏移」，但补零究竟落在哪一段，决定了这 13 字节怎么切。我用「偏移必须单调不减且落在
档案内」作为判据试了几种切法，**没有一种通过**。所以读取器的字段顺序目前仍是猜测，对应的测试已
标为 `skip` 而不是继续断言猜测。

**下一步**：把记录布局定下来（判据已经写好，见 `.test/archives.test.ts` 里该测试上方的注释），
然后按这个形状建立统一抽象：

```ts
interface LazyFile {
  size(): number;
  read(offset: number, length: number): Promise<Uint8Array>; // 未压缩条目直接透传
  readAll(): Promise<Uint8Array>;                            // 压缩条目才整体解码
}
```

`GameSource.readRange` 已经是对外形状，缺的是把它接到每个归档读取器的未压缩路径上。

### 统一惰性文件系统（已实现）

`packages/vfs/src/lazy.ts` 提供跨引擎的统一文件系统。契约取自 AetherKiri 的浏览器宿主
（其 `AetherKiriReadLocalFileSync(fileId, offset, length)` 对 `Blob` 的做法），因为这是
能在不整体载入游戏的前提下服务多 GB 档案的最小契约。

**核心抽象**：

```ts
interface LazyFile {
  size(): Promise<number>;
  read(offset: number, length: number): Promise<Uint8Array>;
}
```

三种实现覆盖全部来源：`RangedLazyFile`（Blob / HTTP 区间）、`BufferLazyFile`（内存切片）、
`ArchiveEntryLazyFile`（归档内某个条目）。`LazyFileSystem` 把若干挂载点组成虚拟树，
`fileId` 与路径分离——路径会被规范化并被后续挂载遮蔽，而 id 在挂载生命周期内稳定。

**真实档案实测**（`.test/mount-real-nsa.ts`，1.8 GB 的 `arc.nsa`）：

| 步骤 | 结果 |
| --- | --- |
| 打开归档 | 17525 个条目，77 ms，**读取 1280 KB** |
| 构建惰性条目 | 17 ms，**0 额外字节**（纯元数据） |
| 文件系统逻辑内容 | 17525 个文件，1.74 GB |
| 读 64 B @ 0 | **恰好 64 字节**，magic `OggS` |
| 读 128 B @ 33 | **恰好 128 字节** |
| 读 16 B（另一条目） | **恰好 16 字节**，magic `OggS` |

**从磁盘总共读取 1280 KB，占该归档的 0.07%。** 这是「按需加载」的可检验形式——
`stats()` 同时报告实际读取字节与「若整体载入本应读取的字节」，所以惰性是被断言的，
不是被宣称的。

顺带修正了 NSA 读取器自身的一处浪费：原先固定读取 16 MB 头表窗口，而实际表不足 1 MB。
现在按需倍增，从 16384 KB 降到 1280 KB（12.8 倍），条目数与耗时不变。

### ONScripter：上游 WASM 构建（已实测可运行）

**结论先行：不再继续修自研的 ONS 加载器，改用 OnscripterYuri 官方的 Emscripten/WASM 构建。**

原因很直接——自研 TS 实现在真实游戏上 `load()` 就耗尽 4 GB 内存，而官方 WASM 构建**在浏览器里
直接把这个真实游戏跑起来了**。引擎保真度不是「接近」，而是它就是引擎本身。

上游 releases 直接提供 `onsyuri_v0.7.7beta_web.7z`（1.3 MB，含 `onsyuri.js` 199 KB +
`onsyuri.wasm` 4.6 MB）。实测记录（`.test/test-games/onscript`，真实商业游戏）：

```text
ONScripter Yuri v0.7.7beta, (Jh0.8.0,Ons20181218,NSC2.96)
Display: 793 x 446 (32 bpp)          Audio: 48000 Hz 16 bit stereo
## onsyuri_fs: mount_onsgame /onsyuri/onscript
## onsyuri_fs: mount_onssave /onsyuri_save/onscript
页面标题 = "Tsukiniyorisou + Append for ONScripter"   ← 由脚本自身设置
已加载文件：0.txt 1.txt 2.txt arc.nsa arc1.nsa default.ttf（含两个 GB 级归档）
```

页面标题取自脚本自身，说明剧本被**成功解析**（我的 TS 版完全做不到这一步）。

#### 它是怎么被喂数据的

`onsyuri.html` 的加载器需要一个 `onsyuri_index.json`，然后在 Emscripten 虚拟文件系统里挂载，
最后 `callMain(["--root", …, "--font", …, "--save-dir", …])`：

```json
{
  "title": "onscript",
  "gamedir": "/onsyuri/onscript",
  "savedir": "/onsyuri_save/onscript",
  "lazyload": true,
  "files": [{ "path": "0.txt", "url": "./game/0.txt", "lazyload": true }],
  "args": []
}
```

`lazyload` 按文件打开时才去取（`fetch` + 写入虚拟 FS），所以 **GB 级归档不需要预先下载**——
这正是浏览器端跑大型 ONS 游戏的关键。`savedir` 由 `IDBFS` 支持，即存档落在 IndexedDB。

配套工具已放在 `.ref/onsyuri-web/`：`serve.mjs`（带 Range 支持的静态服务器，`.wasm` 以正确
MIME 提供）与 `make-index.mjs`（生成索引）。

#### 这对本项目意味着什么

这是架构上的一次修正：ONS 这一条从「自研解释器」改为「宿主 + 上游 WASM 运行时」，与
Siglus/Artemis 的「宿主 + FFI 契约」是同一形状，只是这里的二进制已经存在。自研 ONS 解释器保留
下来仍然有价值——它是纯 TS、可调试、且对明文脚本的子集能给出逐条兼容性报告——但**不应再作为
该引擎的主运行时**。

许可证方面没有问题：OnscripterYuri 是 GPL-2.0-or-later，可并入本项目的 GPL-3.0-or-later，
WASM 产物随附其 LICENSE。

**仍未做**：接入平台的 `RuntimeProvider` 契约**已经完成**——`@online-gal/runtime-ons` 里的
`onscripter-wasm` provider（`priority: 95`，高于自研解释器的 90）已注册进 `BUILTIN_PROVIDERS`，
平台能直接启动这个 WASM 引擎。仍未做的是另一半：用 `onsyuri_index.json` 的生成替换现有导入流程。

### 用真实游戏验证桥接层（`.test/test-games/`）

把三款真实游戏放进 `.test/test-games/` 后逐一跑过，结论如下。

**识别层正常**——三款都被正确归类，且证据链符合预期：

| 游戏 | 识别 | 分数 | 证据 |
| --- | --- | --- | --- |
| KiriKiri | `krkr2` | 70 / medium | `adult.xp3` |
| ONScripter | `onscripter` | 120 / **high** | `0.txt` + `arc.nsa`，startup=`0.txt` |
| Artemis | `artemis` | 90 / medium | `root.pfs` + `root.pfs.000` 补丁包 |

> 表里是**自研解释器**的探针结果。现在命中同一个游戏时，WASM 引擎（`krkr2-wasm` / `onscripter-wasm`）
> 会因优先级更高而先认领，见「各运行时实现范围」。

Artemis 那条正好验证了补丁包编号规则：说明书里写的「把汉化补丁改名为 `root.pfs.001`」机制被
正确识别为「基础包 + 编号补丁」。

**执行层暴露了两个此前完全看不见的缺陷**，两者都只在真实数据下出现：

1. **XP3 读取器读不了任何真实档案。** 签名常量与测试 fixture 是照同一份错误理解写的，所以测试全绿
   而真实档案一个都打不开。真实头部已在两个互相独立的档案上核实一致：

   ```text
   58 50 33 0d 0a 20 0a 1a 8b 67 01 | <u64le 索引偏移 @ byte 10>
   ```

   `info` 节点的真实布局也已从字节偏移反推出来（**全部是定长小端整数，没有 var-int**）：

   ```text
    0  "info"
    4  u32le   info 块长度
    8  u64le   flags
   16  u64le   打包后大小
   24  u64le   原始大小
   32  u16le   名称长度（UTF-16 码元数）
   34  UTF-16   名称
   ```

   之前按 var-int 读，得到 `infoSize=13, nameLength=0`，而字节里明明是 `52` 和 `15`——**所有条目名
   因此为空、索引拿不到可用条目**。修正布局是让真实 XP3 可读的下一步。

2. **真实 ONScripter 游戏在 `load()` 阶段耗尽内存**（1 GB 堆即可复现，脚本本身只有 6 MB）。
   已确认不是资源规模问题，而是加载路径上的无界累积；尚未定位到具体位置。

`.test/detect-games.ts` 可对这三款游戏重复运行识别诊断。

### 桥接层移植清单

| AetherKiri 桥接源 | 本项目 | 状态 |
| --- | --- | --- |
| `engine_api/src/engine_input_queue_gate.h` | `player-host/src/input-gate.ts` | ✅ 已移植 |
| `onscripter_runtime/include/onscripter_presentation.h` | `player-host/src/presentation.ts` | ✅ 已移植 |
| `onscripter_runtime/src/onscripter_save_storage.cpp` | `runtime-ons/src/save-storage.ts` | ✅ 已移植（含迁移优先级） |
| `engine_api/src/engine_api.cpp`（注册/选项/调试信息） | `engine-api/` | ✅ 已移植 |
| `siglus_runtime/include/siglus_ffi.h` | `runtime-siglus/src/runtime.ts` | ✅ 接口已转写 |
| `rfvp_runtime/src/rfvp_runtime_provider.cpp` | `runtime-artemis/src/runtime.ts` | ✅ 接口已转写 |
| `onscripter_runtime/src/onscripter_runtime.cpp`（75 KB） | `runtime-ons/src/` | ⚠️ 自研重写，**非逐行移植** |
| `siglus_runtime/src/siglus_audio_output.cpp` | — | ❌ 未移植 |
| `siglus_runtime/src/siglus_jpeg_decoder.cpp` | — | ❌ 未移植 |

需要说明的一点：`onscripter_runtime.cpp` 本身**不是** ONScripter 引擎，而是一层宿主——它设置工作
目录、调用 `ons->openScript()` 与 `ons->init()`、并转发输入与帧。引擎逻辑在 OnscripterYuri 子模块
（`.ref/onscripter-yuri`，55 个源文件）。因此上面第 2 条的内存问题**不能靠移植桥接层解决**：那是本
项目自己重写的脚本加载路径引入的，答案在 OnscripterYuri 的 `ScriptHandler.cpp` 里——`readScript()`
把 `0.txt`…`99.txt` 读成**单个连续缓冲区**，`labelScript()` 一次扫描建立标签索引，执行只是在这个
缓冲区上移动指针。本项目目前的实现与这个设计不同，这正是要对照的地方。

### 移植过程中修复的真实缺陷

这些此前静默存在的 bug 都由新测试捕获：

1. **命令表查找缺失**：`First message line.` 这类无引号叙述文本被解析成命令。ONScripter 靠命令表
   区分命令与文本，现在 `parseNscript` 同样查表（`ONS_COMMANDS`），解析器与解释器不再可能漂移。
2. **`selnum` 变量参数**：`selnum %0,"A",*a,"B",*b` 的第一个参数是变量槽，之前只识别整数形式，
   导致选项错位、最后一个跳转目标丢失（表现为「菜单能点但跳不动」）。
3. **菜单选择的重入**：`chooseOption` 在输入回调里直接改 `pc` 并调用 `run()`，而输入回调可能仍在
   `run()` 的循环栈内，外层循环的 `pc++` 会覆盖跳转。现在改为 `pendingJump`，由循环顶部消费。
4. **`movie` 被当作已处理**：它和 `anime`/`midi` 一起躺在「识别但不实现」的 no-op 分支里并返回
   「已处理」，于是兼容性报告永远显示 clean。现在逐个记录原因，报告能正确给出
   `unsupported` 并附说明。
5. **存档迁移的自反馈循环**：迁移会把刚写入 `savedata/` 的文件当成源文件重复复制。
6. **PFS 记录布局**：我最初按错误的字段顺序（多读一个字段）实现，导致每个包只能读出第一条条目；
   现在按「名字长度 / 名字 / 数据长度 / 偏移 / 长度」读取，并用**表字节的 SHA-1 作为载荷密钥流**。
7. **编译型 `.ks` 被当作明文**：容器头同时包装「原始源码」与「TJS2 字节码」，两者解码成文本后
   无法区分。没有 VM 时后者会被当成空脚本**静默播放**。现在在解码前检查原始字节的签名并拒绝加载。

8. **`@tag` 行形式完全缺失**：KAG3 有两种标签写法。`[tag attr=value]` 是教程里常见的括号形式，
   而 `@call storage="snow.ks"` 这种**行首 `@` 形式**在真实剧本里至少同样常见。缺了它不会报错——
   该行不被识别为标签，就落到文本分支，于是**游戏把自己的脚本源码当对白显示出来**。

9. **`[call]` 被当成 `[jump]`**：`[return]` 是空操作，也没有调用栈。真实剧本常用
   `@call storage="snow.ks"` 加载插件定义，而该脚本以 `[end]` 结束——于是**整个游戏在进入第一句
   对白之前就结束了**，且没有任何报错。现在 `[call]` 压栈（保存整个程序、标签表、文件名与计数器），
   `[return]` 与「被调用脚本里的 `[end]`」都会返回调用方；只有最外层脚本的 `[end]` 才真正结束游戏。

10. **`@call storage="snow.ks"` 解析到错误路径**：KAG3 把裸脚本名解析到**当前剧本所在目录**
    优先于游戏根目录。该剧本位于 `scenario/first.ks`，而文件在 `scenario/snow.ks`；只按根目录解析
    找不到文件，于是这次调用被静默跳过——失败在很久之后、以**另一个文件里 `@eval` 的未定义变量**
    形式暴露出来。现在按「当前剧本目录 → 根目录 → `scenario/`」依次尝试候选路径。

11. **`[history]` 等待一个永远无法满足的条件**：指针处理只恢复 `click`/`input` 两类等待，
    而 `[history]` 等的是一个名为 `history` 的标签——运行时里没有任何东西能满足它，剧本就**永久停住**。
    真实 demo 在第一句对白之前正是这个标签，这就是「什么都没画」的原因。现在 `[history]` 与点击共用
    同一条恢复路径（点一下关闭历史记录，与原生引擎一致）。

12. **`global` 与 `mp` 未定义**：`snow.ks` 用 `typeof(global.snow_object) == 'undefined'` 判断插件
    是否已注册，而 `mp.storage != f.bg` 是转场宏开头的惯用守卫。二者此前都是 `undefined`，
    读取即抛「cannot read property of void」，`@if` 直接中断、分支被静默丢弃——**而那条分支正是初始化
    插件的分支**。现在 `global` 是指向解释器全局环境的代理，`mp` 是真实的宏参数表（随存档持久化）。

> 第 8–12 条全部来自**把真实商业剧本跑起来**，而不是读代码或写 fixture。自编 fixture 永远发现不了
> 它们：fixture 用的是实现者已经知道的写法（括号标签、`[jump]`、根目录路径），而真实数据用的是
> 发行版实际采用的写法。这也是新增 `.test/real-game.test.ts` 的原因——它把探针与运行时指向
> AetherKiri 内置的真实 KAG3 游戏目录，并断言**画出了剧本里真实存在的对白**。

---

## 各运行时实现范围

### KiriKiri / KAG3（`packages/runtime-kag`）

这个包里有**两条**执行路径，先说的是主线：

- **`krkr2-wasm`（`wasm-provider.ts`，`priority: 105`）**——真正的 KiriKiri2 引擎，编译成
  WebAssembly 后在 iframe 里跑，归档由宿主字节源接缝按 Range 喂进去。它只在
  「构建产物已发布 **且** 游戏源里有可挂载的归档」时才认领游戏，否则让给解释器。
- **自研解释器（`runtime.ts`，`priority: 100`）**——下面这一节讲的都是它。

`krkr2-wasm` 优先级更高：同一个游戏两者都命中时，真引擎胜出。构建配方在
`ports/krkr/kirikiroid2/`，`bash ports/krkr/kirikiroid2/run-build.sh` 退出码 0。

自研的 TJS2 源码解释器与 KAG3 场景执行器。

**TJS2 覆盖**：词法分析、递归下降语法分析、树遍历求值。支持变量作用域（含 `var` 全局提升）、算术/位/比较/逻辑运算符与正确优先级、`?:`、复合赋值、`++`/`--`、函数声明与表达式、默认参数、`arguments`、对象与数组字面量、`for`/`for-in`/`while`/`do-while`/`switch`、`try`/`catch`/`finally`/`throw`、基于原型链的 `class` 与继承、`typeof`（TJS2 拼写为 `void` 而非 `undefined`）、`instanceof`、`in`、可选链。除法为零时抛错而非返回 `Infinity`。

⚠️ **这是源码级解释器，不是字节码 VM。** KiriKiri 实际执行的是 TJS2 编译字节码；本实现只处理
TJS2 *源码文本*。因此编译型 `.ks`（`TJS2` 头部的字节码）会被**明确拒绝**而不是当作空脚本播放
（`first.ks is compiled TJS2 bytecode, which this runtime cannot execute`），
详细的格式分析见 `@online-gal/tjs2-bytecode`。

内置对象：`kag`、`System`、`Storages`、`Scripts`、`Dictionary`、`Math`/`JSON`/`Date`，以及 `setTimeout`/`clearTimeout`。

**KAG3 覆盖**：消息流（`[cm]` `[ct]` `[r]` `[er]` `[br]` `[p]` `[l]` `[wait]` `[click]`）、控制流（`[jump]` `[call]` `[goto]` `[end]` `[if]`/`[elseif]`/`[else]`/`[endif]` 真实分支求值、`[eval]`）、图层（`[bg]` `[image]` `[ld]` `[layopt]` `[position]` `[cl]` `[trans]` `[quake]` `[msgon]`/`[msgoff]`）、文本格式（`[locate]` `[font]` `[ruby]` `[color]` `[size]` `[bold]` `[italic]`）、音频（`playbgm`/`playse`/`playvoice`/`stop*`/`fadebgm`）、交互（`[select]`、`[history]`）、存档（`[save]`/`[load]`）、`[macro]`/`[endmacro]`（加载期参数替换展开）。

**脚本容器**：明文 UTF-16/UTF-8/Shift_JIS `.ks`/`.tjs`，以及编译后的二进制容器（16 字节头 + 定长密钥 XOR + 可选 zlib）。

**明确未实现**：`[trans]` 的 `rule=` 遮罩转场（降级为交叉淡化并记入兼容性报告的 `approximated`）、原生插件（`loadplugin`）、自定义 shader、TJS2 的 `property` 访问器语义、`with` 语句。

### TJS2 字节码（`packages/tjs2-bytecode`）

**目前是格式工具，不是 VM。** 产出是对 TJS2 字节码格式的完整逆向，以及一个能读取并反汇编它的分析器。

**已验证的部分（容器 + 操作码 + 反汇编全链路）：**

- **容器**：`TJS2` + `"100\0"` + 文件长度 + `DATA`/`OBJS` 标签。数据区是七个带类型的常量数组
  （byte/short/long/longlong/double/string/octet），其中三处需要 4 字节对齐填充——**漏掉填充会让
  后面每个字段整体偏移**，这正是我最初卡住的地方。对象记录含 12 个上下文头字段、源码位置表、
  指令数组、变体槽、父/属性/超类引用。
- **操作码表**（`opcodes.ts`）：127 个操作码 = 18 个前置 + 16 个访问器运算符 × 4 种变体 + 44 个后置。
- **指令宽度表**：TJS2 字节码是扁平的 16 位字数组、操作数**没有长度前缀**，唯一遍历方式就是知道
  每个操作码吃几个字。`VM_CALL` 家族尤其特别：参数个数在固定槽位，用 `-1`/`-2` 作哨兵表示
  「参数已在栈上」和「后面还有一张 `(类型, 寄存器)` 表」——所以字必须按 **int16** 符号扩展来读，
  否则 `-1` 会变成 65535 并把遍历带出数组。
- **反汇编**：把常量操作数解析成可读形式（字符串、整数、实数、octet），跳转目标标为代码地址。
  关键断言是**指令行走必须恰好落在 code 数组末尾**——错一个宽度就会让后面全部错位。
- **脚本分类**：区分字节码 / 明文源码 / 未知，并对真实游戏文件验证。

**这一轮抓到的一个静默 bug，值得单独记：** 我最初把 `VM_CHKINS` 同时列在「前置」和「后置」两组里，
于是操作码表有 128 项而不是 127，**它后面所有操作码的编号全部 +1**。这不会报错——只会让
`VM_RET` 从 118 变成 119、让半个表被叫错名字，任何按编号生成的字节码都会被静默误读。
现在 `buildOpcodeNames()` 会检测重名并抛错，测试也把表长（127）与关键编号（`VM_BNOT`=82、
`VM_RET`=118、`VM_DEBUGGER`=126）钉死了。

操作码表与格式布局来自 BSD 许可的 [Kirikiroid2](https://github.com/zeas2/Kirikiroid2)
（`src/core/tjs2/`），出处与许可证声明见 [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)。

**仍未实现**：字节码 **VM**（求值循环）。本平台目前对编译型 `.ks` 的处理是明确拒绝加载，
而不是把它当空脚本播放。

### ONScripter（`packages/runtime-ons`）

这个包同样有**两条**执行路径：`onscripter-wasm`（`wasm-provider.ts`，`priority: 95`）跑的是
从源码构建的 OnscripterYuri 真引擎（见上文「ONScripter：上游 WASM 构建」），
`runtime.ts` 的 `priority: 90` 解释器是它打不开的游戏的回退。两者共享同一个探针。
下面讲的是解释器。

NScripter 命令语言解释器，扁平程序计数器 + 整数/字符串变量槽模型。

支持：`goto`/`gosub`/`return`/`jumpf`/`jumpb`、算术与比较（`mov`/`add`/`sub`/`mul`/`div`/`mod`/`inc`/`dec`/`numalias`/`stralias`/`itoa`/`atoi`）、条件（`if`/`notif`）、文本与消息（`clr`/`br`/`page`/`er`/`indent`/`texton`/`textoff`）、图形（`bg`/`ld`/`csp`/`csp2`/`cl`/`quake`）、音频（`play`/`playonce`/`wave`/`mp3`/`mp3loop`/`bgmstop`/`stop`）、选择（`select`/`selnum`）、存档（`save`/`load` 直接写 `savedata/save<NN>.dat`）、`caption`、`*define` 块中的 `mode` 分辨率读取与 `define`/`game` 结构标记。

**容器解码**：`nscript.dat`（XOR 0x84/0x85）、`nscript.___` 与 `nscr_sec.dat`（XOR 0xA5/0xA6）、NT2/NT3 字符串数组、明文编号文本。

**明确未实现**：`anime` 动画、`vsp` 视频精灵、`movie`、`midi`、`systemcall`、`savescreenshot`、`fileexist`（固定回答「不存在」）。这些都会进入兼容性报告，附上具体原因。

### SiglusEngine（`packages/runtime-siglus`）

**只做识别与配置读取，不执行。** 这是刻意的设计，不是半成品：

Siglus 的引擎本身不在 AetherKiri 的 C++ 桥里，而在 Rust 的 `siglus_rs`（MPL-2.0）——
桥只是个 FFI 宿主。已移植的部分：

- **探针与评分**：`Scene.pck` 得 90（与 AetherKiri 的 `Probe()` 一致），这个分值是 Siglus 独有的；
  `Gameexe.ini`/`Gameexe.dat` 单独只给 55，因为不少无关工具也会生成同名文件，给 90 会让这类目录
  压过真正的 KiriKiri / ONScripter 游戏。两者同时命中得 130。
- **`Data/` 布局**：部分版本把标记文件放在 `Data/` 下，根目录找不到时会继续找。
- **`Gameexe.ini` 解析**：读取 `#SCREEN_SIZE`（游戏原生画布尺寸，AetherKiri 也是用它而不是 resize
  引擎）与 `#WINDOW_TITLE`；支持 `;` / `//` 注释且不破坏引号内的分号。加密的 `Gameexe.dat`
  **没有公开解码器**，因此尺寸保持未知而不是猜测。
- **`SiglusEngineHost` 接口**：把 `siglus_ffi.h` 的 C ABI 逐条转写成 TypeScript
  （见 `packages/runtime-siglus/src/runtime.ts` 的对照表）。它声明但未实现，作用是记录
  「一个 WebAssembly 移植需要暴露什么」，并让运行时**不会**伪装成能跑。

因此 `SiglusRuntime.load()` 直接以 `UnsupportedEngineError` 失败，并列出缺失能力（`siglus_rs` 的
wasm 构建、WebGPU 离屏读回、`Scene.pck` 解包、`Gameexe.dat` 解码器、视频/音频后端）。这比一个
能构造、报告成功、然后什么都不画的运行时诚实——后者对玩家而言和「游戏坏了」无法区分。

> **许可证提示**（下一步真去移植 `siglus_rs` 时才会用到）：该引擎是 **MPL-2.0**。MPL-2.0 是文件级
> copyleft，与 GPL-3.0 属于不同体系，但其 §3.3 提供了「二次许可」条款，明确允许把 MPL 代码与
> GPL-3.0 组合，因此组合本身可行；约束是**必须保留 MPL 源文件的许可证声明**，且对 MPL 文件的修改
> 需以 MPL 回馈。当前仓库只是声明接口、未复制任何 MPL 代码，所以尚无冲突，但移植时必须在
> `THIRD_PARTY_NOTICES.md` 中登记。

### ArtemisEngine（`packages/runtime-artemis`）

public package 当前能识别游戏、读取 PFS，并且 **provider 默认返回 compat 源码 WASM 会话**；
compat 引擎已编译、已有合成测试，但**全真游戏验收尚未过**。详见 [`Artemis 源码导航`](ports/artemis/README.md)。

- **默认出货路线是 compat 源码编译，不是 SDK 桥**：`.github/workflows/build-wasm-artemis-compat.yml`
  → `.github/scripts/build-artemis-compat.sh` → `ports/artemis/compat-web/build.sh`，
  编译上游 [`artemis-compat`](https://github.com/Weiss-UltimateSavior/artemis-compat)（GPL-3.0）
  pin `9f622629f595db85e6f76c45c06edde29d7d9961`，产出 `artemis-compat.{js,wasm}` 与随制品走的
  **对应完整源码** `artemis-compat-source.tar.gz`（上游 `git archive` + 本仓库
  `compat-web/patches/`、`compat-web/overlay/`、build 配方、`LICENSE`、
  `third_party/lua-5.1.5/COPYRIGHT`），`publish: false`。上游 pin 只带 `src/host/mac_main.mm`
  与 native CMake target ⇒ **Web host / Web CMake / `ReadAt` / numeric 全是本地 port**。
- **页面侧前提是跨源隔离 + 资产配置**：compat worker 用 **SharedArrayBuffer 镜像**做同步读，
  `crossOriginIsolated !== true` 或缺 `SharedArrayBuffer` 时不满足能力检查，会被
  `compat/runtime.ts` 列进 missingCapabilities 而不是假装能跑。开发服务器已配置
  `Cross-Origin-Opener-Policy: same-origin` + `Cross-Origin-Embedder-Policy: credentialless`
  （`apps/app/astro.config.mjs`），**部署必须自行核对响应头**。引擎资产由
  `apps/app/scripts/artemis-assets.mjs` 守门（dev/preview 中间件 + `astro:build:done` 拷贝）；
  **静态资产配置与真实游戏验收都尚未过门**，不得当成已验收。
- **旧 SDK 桥只保留显式诊断**：`build-wasm-artemis-bridge.sh` /
  `build-wasm-artemis-bridge.yml` 已移除 push/pull_request 自动触发，只能手动 dispatch；
  官方 glue 与 generated-body masks 是**诊断路线**，不作为出货宿主，也不证明 compat 可玩
  （checkpoint 语义与音频仍未验证）。

- [`runtime.ts`](packages/runtime-artemis/src/runtime.ts) 的 `availability` 是**实时 getter**：
  未配置 compat 引擎时 `unavailable`，已配置且页面满足 `crossOriginIsolated` / `SharedArrayBuffer` /
  `OffscreenCanvas` 时 `runnable`。`load()` **不再无条件抛错**，默认走 `CompatArtemisRuntime`。
- [`src/index.ts`](packages/runtime-artemis/src/index.ts) 在 SHA-1 / PFS / probe / provider 之外，
  还导出 `configureArtemisCompat()`、`CompatArtemisRuntime`、`CompatReadHost` / `CompatWorkerHost`；
  导入表、GL 或 Asyncify 组件测试不等于游戏验收，**当前亦无全真游戏证据**。
- 出货引擎是 **compat 上游源码编译**（GPL-3.0，pin `9f622629…`）。`ports/artemis/src/` 的
  **340 个映射函数 / 340 个 C++ 产品文件**属于**历史路线**台账，其中四个新产品尚未进入索引；
  `artemis-compat` 是真实编译输入，不是「命名/结构参考」——旧的错误表述已撤回。
- 官方 wasm / minified glue 部署与 generated-body masks 是**诊断路线**，不作为出货宿主，
  不证明当前手写冷启动、public runtime 或 GitHub 包已可用。当前手写冷启动失败，完整风格与
  whitespace gate 也未通过；不能沿用旧的「引擎语义完整 / 已可运行 / style PASS」结论。
- [`历史移植记录`](docs/artemis-web-port.md) 保留既有实验与欠账；
  本次只整理源码入口，不推进冷启动修复、现代宿主接入或真实游戏复验。
  SDK 及行为重表达产品的再分发授权另行核实。

下面几条是 `packages/runtime-artemis` 这个包自己的范围：

- **探针与评分**：标记文件与补丁命名规则来自 Tyranor-Next 文档化的指纹表
  （`docs/Artemis自动引擎识别优化计划方案.md` 与 `ArtemisEngineFingerprintDetector.kt`），
  作为**事实**使用；没有复制其代码。该项目是 GPL-2.0-or-later，与 GPL-3.0 兼容。
  评分区分证据强度：`root.pfs` 基础包 70；松散 `system/first.iet` 或包内命中该条目 +60；
  松散 `system.ini` +25；`root.pfs.NNN` 补丁包 +20；**只有一个 `.pfs` 容器**时仅 25 分并给低置信度，
  因为 `.pfs` 这个扩展名并不专属于 Artemis。
- **PFS 读取器**（`pfs.ts`）：`pf8`/`pf7`/`pf6` 容器，表头 + 变长记录。这解决了一个实际问题——
  打包发行的游戏根目录**只有 `root.pfs` 一个文件**，不看包内目录无法判断游戏是否完整。
  现在能列出包内条目，并直接从 `system.ini` 读取配置。
- **SHA-1**（[`sha1.ts`](packages/runtime-artemis/src/sha1.ts)）：PFS 的载荷常用
  `SHA-1(表字节)` 作为 XOR 密钥流。Web Crypto 支持 SHA-1，但 digest API 是异步的；
  这里提供同步纯 TypeScript 实现，按 FIPS 180-1 并用 Node 的 `crypto` 交叉验证。
  这是格式兼容用途，不是推荐将 SHA-1 用于安全设计。
- **`ArtemisEngineHost` 接口**：把 AetherKiri `bridge/rfvp_runtime` 的 `aether_rfvp_*` 契约
  转写成 TypeScript。选 RFVP 而不是 Artemis 本身来记录，是因为 RFVP 是 Rust 实现且已经渲染到
  调用方提供的表面，移植到 WebAssembly 属于换渲染后端而非重新设计。
  这里也记下了一个 AetherKiri 才需要操心、浏览器里不存在的坑：Artemis 内嵌 Lua 5.1，而
  OnscripterYuri 链接 Lua 5.4，同一进程里两个 ABI 会互相绑错符号，所以它对每个 Lua 全局做了
  编译期前缀——浏览器里没有这个问题，但**也说明即使用 Lua 5.4 也不能替代 5.1**
  （`goto`、整除、`setfenv`、被移除的 `unpack` 都会让脚本出现可观察差异）。

### TyranoBuilder / HTML5（`packages/runtime-web`、`packages/runtime-tyrano`）

这些引擎自带渲染器和游戏循环，因此不做帧合成：游戏在 **iframe** 中运行，宿主通过 `postMessage` 按需喂给它文件。

由于导入的目录没有可加载的 URL，运行时注入的引导脚本会：

1. 把游戏自身的 `<script src>` 内联进文档（blob 页面无法发起子资源请求）；
2. 代理 `fetch` 与 `XMLHttpRequest`，把虚拟路径转成 blob URL；
3. 拦截元素 `src`/`href` 赋值；
4. 补上 NW.js/Electron 桌面导出会探测的全局对象（`process`、`nw`、`require`）。

TyranoBuilder 额外注入一个 shim，让桌面导出回退到 Web 存储路径而不是在启动时抛错。

---

## 归档格式支持

| 格式 | 说明 |
| --- | --- |
| ZIP | 中央目录解析、ZIP64、store/deflate；UTF-8 与 Shift_JIS 文件名 |
| XP3 | 完整索引树（`File` → `info`/`segm`/`adlr`）、XP3 变长整数、zlib 段、XOR 混淆条目（需密钥） |
| NSA / NS2 / SAR | 偏移表 + NUL 结尾文件名；SAR 的 zlib 负载 |
| ASAR | Electron JSON 索引 + 字符串字节偏移 |
| PFS | Artemis 的 `pf8`/`pf7`/`pf6` 容器；目录表 + `SHA-1` 密钥流的载荷 XOR |

所有读取器**只解析索引**，文件体按需解压并做 LRU 缓存——这是让数 GB 的 XP3 包可以瞬间打开、内存可控的关键。

---

## 测试

测试套件位于 `.test/`，参考实现的上游检出位于 `.ref/`。**两者都不入库**：
`.ref/` 体积大且不是本项目可再分发的代码；`.test/` 是本地工作台，依赖真实游戏数据
（`~13 GB`）与上面那批上游检出，同样不能再分发。需要随仓库发布的测试请放在 `test/`——
`test/` 没有被忽略，`.test/` 才是。两个名字分开，克隆下来的仓库就可以自带 `test/`
而不与这套本地测试冲突。

**克隆后可运行 `npm run test:sources` 的源码上传回归测试，不需要游戏或工具链。**
`pnpm test` 仍是依赖本地工作台的完整套件，缺少该工作台时会明确报错退出，不会伪装成 0 用例通过。
以下目录和用例数是本地历史套件的导航，不是本次整理重跑后的结果。

```
.test/
├── detection.test.ts     # 引擎识别分值表与调度器平局链
├── dispatch.test.ts      # 跨引擎派发：谁赢、可用性是否传到记录、选项键是否合法
├── script.test.ts        # KAG3 标签解析（含属性引号、行续接、字面方括号）
├── kag-macros.test.ts    # 宏展开、条件块剪枝、标签重建
├── tjs.test.ts           # TJS2 源码解释器（表达式、作用域、控制流、类、异常）
├── archives.test.ts      # ZIP / NSA / XP3 读取器（冻结的字节夹具）
├── ons.test.ts           # NScripter 容器解码与命令解析
├── siglus.test.ts        # Gameexe.ini 解析、探针评分、不可用时的报错
├── artemis.test.ts       # Artemis 标记、PFS 目录读取、探针评分
├── artemis-pfs.test.ts   # SHA-1（FIPS 向量 + Node 交叉验证）与 PFS 读取器
├── tjs2-bytecode.test.ts # 字节码容器往返、操作码表、宽度表、反汇编、分类
├── adapter.test.ts       # 选项表与兼容性统计
├── adapter-layer.test.ts # 手势队列、宽屏几何、savedata 命名与迁移
├── session-launch.test.ts # 应用启动路径：运行时选择、不可用引擎、存档往返、真实出帧
├── real-game.test.ts     # 真实游戏目录：识别、启动、绘制真实对话
├── kag-runtime.test.ts   # 端到端：KAG 会话（含跨脚本控制流与拒绝字节码）
├── ons-runtime.test.ts   # 端到端：ONScripter 会话
├── ons-wasm-provider.test.ts # `onscripter-wasm` provider：注册、优先级、缺画布时不半启动
├── kiri-host-source.test.ts  # 宿主取数通道：URL→key、按 key 缓存、越界短读、缺文件语义
├── kiri-vlfs-host-source.test.ts # 接缝端到端：vm 载入真实 vlfs.js，断言 fetch 零调用 + 反向对照
├── kiri-wasm-page.test.ts    # createKiriPage：URL 参数、load 时才注入、dispose 语义
├── kiri-wasm-provider.test.ts # `krkr2-wasm` provider：优先级、部署门控、挂载推导
├── lazy-fs.test.ts       # 字节粒度懒加载 VFS
├── sync-bridge.test.ts   # 同步桥（WASM 需要同步读，而平台接口是异步的）
├── harness/              # 模拟浏览器（jsdom + canvas 桩 + 假时钟），只服务测试
└── test-games/           # 真实游戏数据（~13 GB），只读输入，永不提交
```

> 上表是完整的本地套件（`npm test` 实测 **416 tests / 411 pass / 0 fail / 5 skipped**），
> 但**不是每个用例都能在任何机器上跑**。因缺少 `.ref/` 检出而
> 自行 skip 的有 `real-game.test.ts` 与 `kiri-vlfs-host-source.test.ts`（各自 `existsSync`
> 判断后才注册用例）。`archives.test.ts` 里另有 5 个 `it.skip`，那是**永久跳过**：XP3 分块
> 索引读取器还没实现，与其拿夹具把它们凑绿，不如留成待办并写清为什么。当前的 5 skipped 指的就是这 5 个。

`kag-runtime.test.ts` 与 `ons-runtime.test.ts` 走的是应用真实调用路径
（`create` → `load` → `tick`/`sendInput` → `save`/`restore`），用 `.test/harness/browser.ts`
提供的模拟浏览器（jsdom + 录音式 canvas 桩 + 可控假时钟）驱动场景。验证的是**模块之间的接线**：
能识别却无法启动、`[wait]` 之后不再恢复、存档丢失程序计数器这类问题都由它捕获。

---

## 项目结构

```
apps/app/                    Astro 静态站点（页面 + Svelte 交互岛）
packages/
├── engine-api/              运行时契约、调度器、选项表、兼容性统计、内存数据源
├── vfs/                     数据源、归档读取器、路径工具
├── image-codec/             图像解码（原生格式 + TGA）
├── player-host/             图层合成、文本排版、音频、帧循环、输入、手势队列、宽屏几何
├── runtime-kag/             KiriKiri/KAG3（TJS2 源码解释器 + 场景执行器）
├── tjs2-bytecode/           TJS2 字节码格式（容器 · 操作码表 · 宽度表 · 反汇编 · 分类）
├── runtime-ons/             ONScripter（容器解码 + 命令解释器 + savedata 约定）
├── runtime-siglus/          SiglusEngine（识别 + Gameexe.ini 读取 + FFI 契约声明）
├── runtime-artemis/         ArtemisEngine（识别 + PFS 读取 + SHA-1 + FFI 契约声明）
├── runtime-web/             HTML5 内嵌运行时
├── runtime-tyrano/          TyranoBuilder（复用 Web 运行时 + shim）
└── core/                    游戏库、存档、设置、会话启动
ports/                       把上面的运行时接到真实引擎的构建配方与浏览器 harness
├── env.sh                   shell 侧共享环境（仓库根、.ref、emsdk、路径转换）
├── lib/ref.mjs              ES 模块侧的同一件事，供 *.mjs 补丁器使用
├── ons/                     ONScripter：构建配方 + shim + worker harness
├── krkr/kirikiroid2/        KiriKiri（现行）：构建配方 + vcpkg overlay ports + 浏览器测试
├── krkr/krkrsdl2/           KiriKiri（早期方案，已被取代，保留其诊断结论）
└── artemis/                 Artemis：asyncify 桥接构建、手写产品及历史诊断工具
    ├── src/                 手写 C++ 产品（工作树 340 个；四个新产品尚未进索引）
    ├── REWRITE.md           重写约束；存在约束不表示全树已通过
    ├── rewrite-names.tsv    函数与产品路径台账（须区分占位和有效映射）
    ├── reverse/             拼接器、桥接构建与诊断配方；非独立完整引擎
    └── vendor/              已跟踪 SDK 参考输入；再分发授权未解决
scripts/                     仓库自身的布局、上传检查及测试入口
test/                        可交付的源码上传回归测试，无需私有游戏
.test/                       本地跨包套件、游戏与证据（gitignored）
```

### 代码放在哪里（约定）

**模拟/移植源码放在对应的 `packages/`、`apps/`、`ports/`；随仓库交付的测试放在 `test/`。**
`.test/` 是被忽略的本地套件、游戏与证据工作台，不能承载 clean-clone 所需代码。
目录边界、已有资产例外和逐路径上传检查见 [`源码布局指南`](docs/source-layout.md)。

这条约定来自一次真实的代价：整套 WASM 移植——构建脚本、Android shim、harness，甚至引擎
patch——最初全都躺在 `.test/` 里，结果**没有任何东西能推送**，新克隆也根本构建不出来。后来把
它们移进了 `ports/`。

`npm run check:layout`（也挂在 `npm test` 前面）会扫描所有**已跟踪**的 `packages/`、`apps/`、
`ports/` 代码文件，若出现对 `.test/` 的引用就失败。它只看代码，注释会先被剥掉，因此正文里
解释这条约定的文字不会误报。

ONS / KiriKiri 从忽略的 `.ref/` 或 CI 临时目录取得固定上游源码；Artemis 桥接使用已跟踪
SDK wasm 作为转译输入。Emscripten / WABT / vcpkg 等依赖另行准备，不复制工具链入库。
Linux CI 与本地配方并非完全相同：KiriKiri 本地入口和部分 Artemis 工具仍有 Windows 或本机
路径默认值，不能承诺任意平台 clone 后直接构建。主入口与前提见 [`ports 导航`](ports/README.md)。

工作区使用 pnpm workspace；packages 通过 `tsc -b` 增量编译，开发时 Astro/Vite 直接解析 TypeScript 源码（`development` 导出条件），生产构建消费 `dist/`。

---

## 许可证

项目自有代码声明 **GPL-3.0-or-later**，见 [`LICENSE`](LICENSE)。第三方材料的出处与
适用声明见 [`THIRD_PARTY_NOTICES.md`](THIRD_PARTY_NOTICES.md)；根许可证不覆盖缺少授权的 SDK。
选择 GPL 与 AetherKiri 的适配知识来源相一致，但兼容性不等于全部分发义务已履行。

| 项目 | 许可证 / 材料边界 | 当前使用与分发要求 |
| --- | --- | --- |
| [AetherKiri](https://github.com/AetherKiri/AetherKiri) | GPL-3.0-or-later | provider / 宿主架构参考；实际引入代码须保留声明并履行 GPL 义务 |
| [OnscripterYuri](https://github.com/YuriSizuku/OnscripterYuri) | 源码头部声明 GPL-2.0-or-later | 固定源码构建 wasm；分发二进制仍须提供对应源码及声明 |
| [Kirikiroid2](https://github.com/zeas2/Kirikiroid2) | BSD 风格 | TJS2 格式表参考；保留适用版权与许可声明 |
| [Tyranor-Next](https://github.com/Weiss-UltimateSavior/Tyranor-Next) | GPL-2.0-or-later | 当前借用引擎特征事实与分层设计，未复制实现 |
| [siglus_rs](https://github.com/xmoezzz/siglus_rs) | MPL-2.0，尚未引入 | 如后续移植，核对二次许可适用条件并保留 MPL 声明 |
| [artemis-compat](https://github.com/Weiss-UltimateSavior/artemis-compat) | GPL-3.0 | **出货路线编译的源码**（pin `9f62262…`），对应完整源码随制品提供；上游 GPL 原文不等于官方 SDK 的授权 |
| [kirikiroid2-web](https://github.com/fenghengzhi/kirikiroid2-web) | 实际 LICENSE 为 Kirikiri BSD 风格及各依赖声明，不能只看 README 的 MIT 标识 | 上游构建及 VLFS 补丁；源码与二进制声明义务均需保留 |
| 官方 Artemis SDK | 作者保留全部权利；本仓库未记录经核实的本体再分发授权 | 保留参考输入不等于许可；SDK、剩余转译体和重表达产品须单独审查 |

未核实的「商业需付费」「特定授权程序」以及把保留 SDK 写成已接受违法的旧表述已撤回。
不能仅删除 vendor 目录便认定已有衍生产物的授权已解决，也不能把 GPL 或手写改名当作授权。
本次不删除 SDK、不改写其版权声明、不发布衍生产物。

### 包与下游义务

workspace 包与应用保持 `private: true`、GPL-3.0-or-later，不发布 npm。GitHub artifact / 可选
release tarball 的获取方式见 [`制品下载指南`](docs/fetch-engine-artifacts.md)。
ONS 包带引擎资产；KiriKiri 页面资产单独部署；Artemis 出货路线是 compat 源码 WASM 构建，
其对应完整源码随制品发布（`artemis-compat-source.tar.gz`），**全真游戏验收尚未过**。

分发基于本项目的 GPL 组合或修改版本，应按适用 GPL 条款保留许可、提供对应源码并履行其他义务；
是否构成组合程序需看实际集成方式，不能概括为「任何链接一律同许可」。
上游源码不入本仓库，不免除已交付二进制的对应源码、版权及依赖许可义务。

### Artemis 当前仍保留什么

当前**出货路线**是 **compat 上游源码编译**（pin `9f622629…`，GPL-3.0），不是 asyncify SDK 桥。
`ports/artemis/src/` 的 **340 个映射函数 / 340 个工作树产品文件**属于**历史路线**台账；
台账覆盖与函数数不是完整语义、风格或游戏行为验收，也不是出货构建的要求。
旧的 157 / 159 产品及「100% tag 语义完成」数字不再代表当前状态。

继续收敛需要完成**全真游戏验收**（丢弃一次热身 + 两次一致）、页面侧跨源隔离与资产配置核对，
以及历史路线 SDK 转译体 / 行为重表达产品的再分发授权审查。
本次只整理陈旧表述、不改写源码与许可；具体历史记录见 [`Artemis 导航`](ports/artemis/README.md)
与 [`移植交接记录`](docs/artemis-wasm-transcription-handoff.md)。
