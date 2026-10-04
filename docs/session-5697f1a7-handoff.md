# online_gal 会话交接文档（session-5697f1a7）

> 本文件是 DSH 会话 `session-5697f1a7-c4df-4418-8e3c-56165a1ff753`（工作区 `D:\online_gal`，2026-09-17 → 2026-10-04，用户手动结束，266 轮 / 113,847 条事件 / 516 MB 转写）的交接总结。
> 它的用途是让**零上下文的新对话**能直接接着开发，不必重读那份转写。
> 生成方式：把该会话的 50 份滚动压缩摘要 + 用户原始发言 + 尾部事件 + Agent Team 流水做机器抽取与分层压缩。**下文所有具体路径、行号、字节数、哈希、错误串都是会话内记录的证据，不是重新测量过的结论**；标注（未验证）处需现场复核。

## 0b. 最新恢复入口（2026-10-05 · compat 线；本节是唯一现状，下面 §0–§2 为 09-17→10-04 历史）

- **ACK 修复已 settled（原 Grok `790232ff…` 已 closing 释放）**：busy 期到达的 ACK 被直接丢弃（旧回执 4 在 native 倒退成 2）已实测；修复 = 单飞行 RPC + 每 handle 最新累计 pending 槽 + epoch 与 `frames <= playedFrames` 双重守卫。实测 native consumed **4** / host `playedFrames` **4**，两个 audio 测试实际 **exit 0**（`.test/artemis-compact-audio-ack-final/b5-final2.log`、`integrated-final.log`）。**这只是 candidate，不是可玩；旧 game run 不自动覆盖 ACK fix，仍需新 CI 之后真实游戏 warmup×2。**
- **本批 4 门 actual exit 全 0**：`build` 0、`check` 0、`test:portable` 0（**tests/pass 555、fail 0** — 新测试真纳入，非旧 554）、`check-sources` 0；打包 freshness **未跳**（checked 3）、engine ledger 3、离线 install+import smoke 0。新 worker `worker-host.js` `dd5c205c…`，`compat-worker.js` `3216e0f8…`（56712 B 未变）；runtime-dist 与 app-dist worker **byte-identical**。
- **旧 code candidate `93`** 的 CI 554/pass552/fail0/skip2、artifacts/tgz 与 `ffd` byte-equal 是 **`93` 的证据**，不是本批 ACK 修复的证明；**HEAD（本批 commit 前）`a4b6ef3` doc-only**。
- **START 真诊断（`bfa4`）**：无缩放 ⇒ b.2.0 直用；热区 **(60,112,160x48)**，点 (140,136) 原生 count 1 **实进 ADV**，`cb73` 换算假设不成立，**MapPoint/prod 未动**；Enter key13 仍 unmeasured。
- **`647` NOT PASS = 历史失败**，非当前 blocker；**两 run 真 save 恢复仍未取得**。**唯一 browser `a5b4f6ea-80b0-42c7-a24d-9d7109202e13`** 只服务 `93` CI 冻结包，本批**未干扰**；no new server/browser/引擎。**next = 新 candidate push 后 new CI + 真实 warmup×2**。

## 0. 阅读须知

- 来源 7 份只读输入：`fact1.md`（摘要 1–12）、`fact2.md`（13–24）、`fact3.md`（25–36）、`fact4.md`（37–50，最新）、`01_users.md`（用户原始发言）、`tail.md`（最后压缩之后的尾部）、`04_team.md`（Agent Team 流水）。
- 冲突判定：**fact4 > fact3 > fact2 > fact1**；`tail.md` 给最新状态；`01_users.md` 在「用户要什么」上最权威。
- **（未验证）** = 输入无直接证据或须现场复核。`mNNNNN` 与 `01_users.md` 的 `seq` 是两套编号；表中「日期/seq」按原文比对所得，未比对者标（未验证）。
- 交接时的仓库实况（2026-10-04 由接手会话直接 `git` 复核）：分支 `main`、**工作树干净**、**HEAD = `d2b1ad0`**「fix(artemis): match native exception records and heap-growth ABI」(`2026-10-04 01:57:20 +0800`)、共 109 个提交、无未跟踪文件。

## 1. 一句话交接摘要

| 项 | 值 |
|---|---|
| 项目 | `D:\online_gal`：浏览器端 galgame 模拟器，把**真实引擎编译成 WebAssembly**；前端必须 Astro；许可 **GPL-3.0-or-later** |
| 三线 | ONScripter（完成并接入）、KiriKiri/KAG3（完成并接入、已暂停）、Artemis（手写 C++ 重写 + 独立现代宿主，**当前主线**） |
| 当前 HEAD（已复核） | `d2b1ad0`「fix(artemis): match native exception records and heap-growth ABI」(`2026-10-04 01:57:20 +0800`)；上一提交 `db70bd0e84f1bbbb5149a27a3ea37791bc066503`「fix(artemis): compile large wasm asynchronously on browser hosts」（fact4 §1）；工作树干净、无未跟踪文件 |
| 当前唯一卡点 | Artemis **现代自写宿主**下 `bridge.wasm` boot 后约 **3 秒**进入异常分支，进入时**异常指针已是 0**；同宿主官方 WASM 对照运行 **50 秒**未触发（tail.md turn 261） |
| 下一步第一件事 | 在 bridge **每个异常查找调用之前**插观测，定位第一次 `c()` 的源码位置；并行做 `FinishTableIndex` 与参考转译体的**只读 trap-diff**（tail.md turn 262；task-26） |
| 次要卡点 | `host/eh.ts` 异常 ABI 须按官方 SDK glue 重建；`host/boot.ts` 的 `trace.honoured` 保守性未完成；`host/gl.ts` 99 行真实现是「让 guest 走出第一帧」的墙 |

## 2. 用户意图与长期硬指令

### 2.1 项目本体

- 参考 `AetherKiri/AetherKiri` 与 `Weiss-UltimateSavior/Tyranor-Next` 建浏览器模拟器（seq 8，09-17）；前端**必须 Astro**（seq 204）。
- KiriKiri/KAG3 与 ONScripter 都是 C++，转 TS 不可行 ⇒ **编译真引擎为 wasm**（seq 1936、8038）。
- 仓库 `fylcr/online_gal`，**PRIVATE**，pnpm monorepo，包空间 `@online-gal/*`（输入里也见 `@online_gal/*`）。
- Artemis 基座 `Weiss-UltimateSavior/artemis-compat`（seq 17465，09-20 20:50，「工作量最大的一个」）。

### 2.2 硬指令时间线

| m 编号 | 日期/seq | 要点 | 状态 |
|---|---|---|---|
| — | 09-17 15:47 / 8 | 参考两项目建模拟器 | 生效 |
| — | 09-17 15:51 / 204 | 前端 Astro | 生效 |
| — | 09-17 18:53 / 2172 | 换 GPL-3.0 | 生效 |
| — | 09-18 00:01 / 5956 | `.ref/` 放参考、`.test/` 放测试并 gitignore | 生效 |
| — | 09-18 00:18 / 6141 | `.test` 不入库；想入库的测试放 `test` | 生效 |
| — | 09-18 01:24 / 7300 | 重写文件读取，统一浏览器 filesystem | 生效 |
| m03520 | 09-22 07:40 / 27437（比对） | 鼠标映射有问题…先暂停放在这里 | **冻结，不得触碰** |
| m05543 | 09-22 08:42 / 29210（比对） | 不改许可证；都是 gpl2later/gpl3，直接用 gpl3 | **critical** |
| m05607 | 09-22 08:52 / 29335（比对） | 模拟代码放根目录对应位置，测试只放 `.test`，及时推送 | 生效 |
| m17062 | （未验证） | Artemis = 自研手写重写 C++ 即引擎源码，无独立 rewrite 路径 | 生效 |
| m17086 | （未验证） | 只留 asyncify | 生效 |
| m17783 | 09-27 20:28 / 47854（比对） | GitHub 构建 wasm；引擎更新才编译；只重建有改动的引擎 | 生效 |
| m20064 | 09-28 08:23 / 51978（比对） | 「一定要把 github 构建的 wasm 下载下来进行真实游戏测试」 | **验收硬要求** |
| m21015 | 09-28 13:17 / 53638（比对） | ①重写产物 = artemis-compat 的 wasm 版本、源码 GPL-3.0 开源 ②其他版本可去掉 ③用补的 web 宿主调用重写 SDK ④每引擎各自成包，workflow 产物**就是包**，再写聚合模拟器 ⑤不上 npm，使用方也必须 GPL-3.0 | 打包/许可主线 |
| m21048 | 09-28 13:32 / 53710（比对） | 「转写应该是完全源代码重写的」 | **已被实测否证**（§6） |
| m21120 | — | 委派子代理 | 被 m23873 取代 |
| m23249 | 09-29 19:54 / 58124（比对） | 测试时及时关窗 | 生效 |
| m23566 | 09-29 23:17 / 58750（比对） | 跑三场景：官方模块+官方胶水／重写模块+官方胶水／官方模块+重写宿主 | 三场景全失败 |
| m23622 | 09-29 23:54 / 58845（比对） | 「这三个都不行」 | 成因未解释（§7） |
| m23873 | 09-30 01:10 / 59361（比对） | 不再使用子代理 | 写入 `USER.md` critical，**后被放宽** |
| m24051 | 09-30 07:36 / 59689（比对） | 先让官方 wasm 正常加载真游戏 | 未收口（§7） |
| m24411 | 09-30 09:23 / 60345（比对） | sync 不适合需求，从路线图踢出去 | 只出 asyncify |
| m24494 | 09-30 09:44 / 60504（比对） | 参考伪 C 与 artemis-compat 继续推产品 | 被 m27387 取代 |
| m27315 | 09-30 17:01 / 65541（比对） | 一直到 GitHub Action 能构建可用 wasm 为止 | **已达成**（run #36 绿 + 产物过帧锚） |
| m27387 | 09-30 17:14 / 65655（比对） | 停止自动生成，逐文件读代码手改，更像 artemis-compat、可读，注释只讲「做什么」不讲来历 | 统治后续全部工作 |
| m27431 | 09-30 17:16 / 65717（比对） | 可以开子代理，但必须确保代码质量 | 放宽 m23873 |
| m28037 / m28046 | 09-30 19:01 / 67291（比对） | 子代理不要同时跑太多；可用 `doulor`/`doulor-free` 与名字含 `free`/`stealth/` 的免费模型 | 生效 |
| m29147 | 09-30 22:56 / 69268（比对） | 测试不需要再跑官方版 | 生效（`--no-control`） |
| m35652 / m38515 | （未验证） | 进度锚点：**327/340 文件 = 71.4% 官方行** | 数字口径之一 |
| m38626 | （未验证） | 继续 + 少量并行 | 与 m28037 一致 |
| m41619 / m41620 | 10-02 12:59–13:00 / 92344、92358（比对） | 你只负责统筹，细活交子代理，最后你审查；主要委托 deepseek flash4.1 | 生效 |
| m45508 | 10-02 21:25 / 99302（比对） | 先提交，然后修改到能正常使用为止；你负责统领与验收 | 工作方式定调 |
| — | 10-03 19:03 / 109280 | 已装 bsk cli | 生效 |
| — | 10-04 10:46 / 113829 | 总结一下，确保能在新对话里继续开发 | 本文档来源 |

### 2.3 子代理、实例与目录纪律

- 子代理：m27431/m28037/m28046 **允许但低并发**、质量归主 agent；优先 `deepseek flash4.1`，备选 `doulor`/`doulor-free`/名字含 `free`、`stealth/` 者。子代理**不能再派生**（`subagent depth 2 exceeds maxDepth 1`）；`send_message` 报 `Error: active teammate "<id>" not found` **不能**证明其已停止。
- 单实例：**同时只允许一个 artemis/krkr 浏览器实例**（seq 17315）、**同时只一个 bridge 构建**；gate/bisect 期间**禁写 `ports/artemis/src/**`**。
- 目录：模拟代码放仓库根对应位置（`packages/`、`ports/`、`apps/`）；可提交测试放 `test/`；`.ref/**`、`.test/**`、`.work_f3785/`、`rewrite-staging/` **永不提交**；`scripts/check-layout.mjs` 在 `packages/|apps/|ports/` 引用 `.test/` 时 **exit 1**。
- 许可：仓库 GPL-3.0-or-later；包不上 npm，消费方必须 GPL-3.0（m21015⑤）。
- 命名：一律 **artemis-compat**（非 artemis-compact）；产物头注逐字 `Written against artemis-compat, with the official WebAssembly decompile as the reference`（m10180）。

## 3. 仓库现状

### 3.1 目录

| 路径 | 内容 |
|---|---|
| `packages/` | 12 个 `@online-gal/*` 包 + 聚合包 `packages/engines` |
| `apps/` | Astro 前端（`apps/app`） |
| `ports/` | `env.sh`、`ons/`、`krkr/{kirikiroid2,krkrsdl2}`、`artemis/{build,engine,harness,reverse,tools}`；早期 110 文件/924K |
| `test/` | 可提交测试；`test/kag-runtime.test.ts` = 端到端 KAG 会话测试 |
| `.test/` | 本地真实游戏、一次性脚本、截图（不入库）；`.ref/` 为官方 SDK 与参考仓库（不入库） |
| `.github/` | `build-wasm-reusable.yml`（305+ 行）+ 四个 per-engine workflow、`scripts/{build-artemis-bridge.sh,make-package.mjs}`、`upstream-pins.json`、`check-upstream-drift.yml` |
| `ports/lib/` | `fetch-engine.mjs`、`engine-artifacts.json`、`fetch-web-engine.sh`、`web-deploy.sh`、`gen-wasm2c.sh` |
| `docs/` | 见 §8.4 |

### 3.2 包与契约

- 12 包：`core`、`engine-api`、`image-codec`、`player-host`、`runtime-artemis`、`runtime-kag`、`runtime-ons`、`runtime-siglus`、`runtime-tyrano`、`runtime-web`、`tjs2-bytecode`、`vfs`；构建 `build:packages` = `tsc -b tsconfig.packages.json`。
- 版本：`packages/engines` 0.1.5→0.1.6→0.1.7；`runtime-artemis` 0.1.4→0.1.5→0.1.6（提交前不提版本，`--clobber` 会破已记录 digest）。
- 契约：`RuntimeProvider`/`RuntimeInstance`/`RuntimeHost`；调度 **分值 → 优先级 → 注册顺序**；`RuntimeInstance` 面 = `load/tick/render/sendInput/pause/resume/save/restore/dispose/describe?/report?`；运行时**从不自己排帧**，调 `RuntimeHost.requestFrame()`，宿主驱动 `tick(deltaMs)`/`render()`。
- 数据源：`GameSource{list,readFile,readRange?,size,exists,isDirectory}`；`ByteSource={size(),readRange}`；`gameSourceByteSource` 只从 `@online-gal/vfs/archives` 导出。
- 输入/呈现：`player-host/src/input-gate.ts` `InputGestureQueue`（`MAX_QUEUED_GESTURES=8`，只入完整手势）；`presentation.ts` `resolvePresentationViewport`（仅请求比例更宽才裁剪）；`CommandTracker` 报 `clean|degraded|unsupported`；惰性 FS 见 `vfs/src/lazy.ts` + `sync-bridge.ts`。

### 3.3 `BUILTIN_PROVIDERS`（分值）

`krkr2`(100)、`onscripter-wasm`(95)、`onscripter`(90)、`siglus`(90, **unavailable**)、`artemis`(85, **unavailable**)、`tyrano`(80)、`web`(20)。

### 3.4 引擎支持状态

| 引擎 | 基座 | 状态 | 关键证据 |
|---|---|---|---|
| ONScripter | 上游 OnscripterYuri（子模块 `packages/OnscripterYuri`，branch `aether-main`，HEAD `08f744b`），GPL-2.0-or-later | **完成并接入** | `onsyuri.wasm` **4,658,702 B**；1.8 GB `arc.nsa` 只读 **0.56 MB**（≈3300:1）；音频 peak **0.7744**；`Display: 793 x 446 (32 bpp)`、`Audio: 48000 Hz 16 bit stereo` |
| KiriKiri/KAG3 | **krkrsdl2**（`dccbdf6a14829ffee778123350dfc954f919d835`）；KrKr2-Next 无 Emscripten 支持（ANGLE/bullet3/4×Boost/OpenMP）；tynext 内核是预编译 `libkrkrsdl3.so`（28,421,328 B）无源码 | **完成并接入，已暂停** | Edge 渲染真实 KAG3 `1617×1004`/`2561×1398`，按需读 2.69 GB XP3 的 **11.34%**；CI 产物真游戏 `lit>24=87.09%`、signature `f007553e` |
| Artemis | ①官方 SDK 作**诊断权威** ②`ports/artemis/src` 手写重写（splice 进官方转写树）③`packages/runtime-artemis` 独立现代宿主 | **未完成**，当前主线（§4.3） | — |
| Siglus/Tyrano | `packages/runtime-siglus`（`gameexe.ts`/`probe.ts`/`runtime.ts`） | `load()` 以 `UnsupportedEngineError` 诚实拒绝，`availability=unavailable`；缺测试、未加入 `apps/app/package.json` | 未移植 `siglus_audio_output.cpp`、`siglus_jpeg_decoder.cpp` |
| TJS2 bytecode | `packages/tjs2-bytecode` | 仅容器/opcode/宽度/反汇编（**127 opcodes**），**VM 未实现**；编译过的 `.ks` 被拒 | — |

### 3.5 提交链（按输入时间先后）

`11f2ad417f33ddd9af4718c656671856b44d2605`（fact1 的 origin/main）→ `6a9c5c7`（官方 SDK 逆向 + vendored 9 文件）→ `475bcaf`（`ports/artemis/src` 159 文件 + `rewrite-names.tsv` + `REWRITE.md`）→ `8c4e9f6`（CI 流水线 + 消费侧）→ `80fbc0a`/`c382a2c`/`ece90b3`/`308534b`/`27a1a23`（krkr vcpkg 缓存）/`1cc2c22`（28 份 md 返工）/`58b43bf`（artemis 退役）/`0520e3e`（行尾归一）/`580cdd6`（fd imports）/`00ccc32`（readv 根因 + gl 桶）→ `351f244`（fact2 的 #23/#24 HEAD）→ `bfb6053`（run #36 绿）→ `fc122cd`/`7b1e82a`（锚回归 #1/#3）→ `f526c7d3`（util 子代理，22 文件，18,877→7,739 行，**待审计**）→ `8e35c18`（316 文件 +55,289/−103,699）→ `ae71ceb`（14 文件 +8,380/−9；run `36923019535`）→ `c3f5f919f0bab9cf84ce464de1fdede4a13c7bd3`（333 文件 +163,908/−57,597，仅保存/整理）→ **`db70bd0e84f1bbbb5149a27a3ea37791bc066503`** → **`d2b1ad0`（交接时 HEAD，`2026-10-04 01:57:20 +0800`，工作树干净）**。

### 3.6 出货目标与发布物

- 出货：`runtime-ons`/`onscripter-wasm`、`runtime-kag`/`krkr2`（workflow 产物即包）；**有意不发布** artemis 0.1.4，`ARTEMIS_PUBLISH_ARTIFACTS` 未置位时 Artemis 发布关闭。
- 早期命名：`engine-ons-0.7.7beta`、`engine-krkr-20260920`、`engine-packages-0.1.5`；后续要求文档对齐「不再随包发布 build cache」后的真实资产（`ons 0.1.3`/`kag 0.1.1`/`engines 0.1.3`）。
- 陈旧 release asset 待删：`online-gal-runtime-ons-0.1.1.tgz`，id `595083575`。

## 4. 三条引擎线 + 平台/CI 线

### 4.1 ONScripter（完成并接入）

- **AetherKiri 不模拟 ONS**：它编译真实 OnscripterYuri 再外套 host adapter（`bridge/onscripter_runtime/onscripter_runtime.cpp` 77 KB）；只移植 host 层修不了 OOM，答案在引擎 `ScriptHandler.cpp`。
- 已修坑：`makeStyleableCanvas` 返回 Proxy 致真实 OffscreenCanvas 卡 `300x150` → `withStyleProperty()` 返原生 canvas；鼠标误用 `canvas.width/rect.width`（≈0.29）→ `drawnRect()` + `engineSize`；`alignAspectRatio`（`ONScripter.cpp:59/71`）**只缩不放**；`#if !defined(WEB)` 跳过引擎坐标换算 ⇒ 宿主须给屏幕空间坐标；音频 stub 每次 `getChannelData()` 新建数组丢样本（peak 0.0000）。
- 未完成：存档持久化（IDBFS 导入导出，`-lidbfs.js` 已在 flags）；音频队列消费反馈（`queued` 只增不减，读 0.82 s vs 目标 0.25 s）；滚轮/触摸输入。

### 4.2 KiriKiri / KAG3（完成并接入、暂停点）

- 存储缝：`TVPCreateStream`（`StorageIntf.cpp`）汇聚全部；`_TVPCreateStream` 先调 `TVPGetPlacedPath`（真实 FS 查找，须先拦截）；唯一打开点 `StorageImpl.cpp:941 SDL_RWFromFile`。
- **卡点**：ranged read 仍产出**零 range 请求**——拦截与 `krkr_try_ranged_stream` 静态可编译但从不执行；待参考 `.ref/kirikiroid2-web`（**worker + 代理同步读**）。
- **教训**：`tTVPHttpRangeStream` 已声明/实现但**从未被构造**；诊断 `krkr_js_diag` 亦从未触发 ⇒ **用可观测行为（range 计数）验证，不看二进制符号**——最危险的失败是「构建成功却没做该做的事」。
- 四个 JS-桥坑：①`-sEXPORTED_FUNCTIONS=` 是**替换**非合并 ②`--pre-js` 满足不了 wasm 符号 ⇒ 必须 `--js-library` 且经 `-DCMAKE_EXE_LINKER_FLAGS` ③`EM_JS` 注释里的撇号/反引号会终止 C 字符串 ④js-library 顶层 `Module.x=` 触发 `Internal compiler error JS compiler`。
- 真实游戏证据（用下载的 CI 产物）：6 个 CI 文件 + 硬链 3 个 xp3，`serve-range.mjs` 跑 4630；`kiri-shot.mjs --seconds 75` @ `?xp3=/data.xp3` → canvas `756x488`、206 请求为 262,144 B 块（散布 511 MB/982 MB/1.6 GB）、`lit>24=87.09%`、`bright>200=50.17%`、mean `r174.67 g161.10 b156.41`、signature **`f007553e`**。
- 确定性：修 `17a8be5`（根因 `-DKRKR2_BUILD_VERSION="${GITHUB_SHA}-${GITHUB_RUN_ID}"` + 未固定时钟 ⇒ 改 `TZ=UTC`+`SOURCE_DATE_EPOCH`+`VCPKG_KEEP_ENV_VARS`+UI 资产 mtime 归一）后 krkr CI 清单**被采纳**（`index.html` 52,965→53,590、`shell.html` 102,762→105,920、`vlfs.js` 60,605→64,564；CI 曾漏跑 `patch-vlfs-host-source.mjs`）。
- 暂停点：用户两次「先暂停／确保能随时恢复」（09-19 13:56 seq 16092；09-20 00:51 seq 16440）。`packages/vfs/src/archives/xp3.ts` 仍 **5 个永久 `it.skip`**。

### 4.3 Artemis

#### 4.3.1 官方 SDK 诊断线（**只作诊断权威**）

- `.ref/artemis/`：`artemis.wasm` **2,682,787 B**（plain，sha `f05ae1cb25eaaff8…`）、asyncify **5,998,665 B**（sha `7ceabed28b84e768…`）、`libartemis.so` **8,149,024 B**。
- `wasm2wat`→`wat2wasm` **字节相同** ⇒ `.ref/…/artemis.wasm.rebuilt-wabt.wasm` **就是官方二进制**。
- 官方 wasm **单线程**、已 strip、用 `FS_createDataFile` 整包载入；**只有 asyncify flavour 能跑**。官方 glue 坐标乘 `canvas.width/rect.width`（**不是** `devicePixelRatio`）——正是 m03520 冻结缺陷的机制。
- `.so` 是**符号神谕**：99 类、tag 面 **103 个 `CArtemis::Command*`**、入口 `CommandTag`@`0x50f8c0`；补 tag `CommandSkip`@`0x52613c`、`CommandFile`@`0x556f54`，`mouse/caption/autosave/trophy` 记 `TagNoOp`，最终 **0 unhandled**。
- 纪律：**不得把官方 glue 诊断当作现代宿主验收**。

#### 4.3.2 手写重写线（`ports/artemis/src`）

- 机制：`build-bridge-route.sh asyncify [outdir]`；`:214-218` 每轮从官方 `artemis.wasm2c.c` 重建 `$OUTDIR/artemis.cpp`；`:226 patch-native-traps.py`；`:231 rw-splice.py` 按 `rewrite-names.tsv` 把 `w2c_artemis_f<NNN>` 体换成我们的 C++ ⇒ **轮次不累积**（规格见 §5.2）。
- 规模：159 文件/~71,006 行（fact3）→ tsv 314→350→361→**400 行**；product 计数 **327→334→336→340**；手写早期 ≈ **2.8%**（157/5,610）；进度锚点 **327/340 文件 = 71.4% 官方行**。
- 巨件：`render/compositor_hide.cpp` f3688（5,535 行/202,030 B，labels/gotos 57/57）；`render/compositor_lydel.cpp` f3656（8,597 行/267,929 B，219/219）；`render/compositor_effect.cpp` f2945（5,703 行/223,964 B，resume 0–242）；`script/lua_tags_builtin.cpp` f3633（54 part 装配 33,341 行/1,321,222 B；`AUDIT CLEAN`、max width 100、EnterFrame/LeaveFrame 1/1、labels/gotos 994/991、segue guards 1830/1830）。
- 已提交锚回归缺陷 ①–⑤（①`script/convert_value_to_integer.cpp` 槽尺度 `(slot << 2)` → `<< 4u`，commit `fc122cd`；③`script/emit_condition_jump.cpp` 加 `bool register_fits`，commit `7b1e82a`）；**第六个及其后的缺陷未定位**。手改净化样例（行数）：`util/format_unsigned_octal.cpp` 59→20、`util/parse_decimal_number.cpp` 69→22、`script/compile_comparison.cpp` 448→212、`script/compact_entry_queue.cpp` 634→225、`script/check_match.cpp` 760→288。
- 现场（序号 36）：`git status --porcelain ports/artemis/src` = **311 modified**；构建 40,773,758 B、`rw-splice: 327 product(s) spliced`、**5/5 确定性 FAIL**；bisect 收敛到 **`files[77:155]` = 78 个 `script/` 文件**（`LA1` files[0:155] FAIL size=41,020,136 两轮一致；`LA2` files[0:77] PASS size=41,147,764，`g=75 hash=fd00de90 raf≈1900 console=28 exc=0`）。
- 头号静态候选：`ports/artemis/src/script/grow_value_stack.cpp`（59 行，窗口 ∩ `suspect_resume_layout`），签名 `void GrowValueStack(EngineContext* ctx, uint32_t record, uint32_t needed)`，回绕先弹 8 字节帧（`capacity=frame[0]`、`record=frame[1]`）再弹 4 字节 resume 字。
- 拼接树侧遗留：冷启动 A（非 util 侧走到 `macro.iet`/`macro.iet.iet` 后停住，永不请求 `debug/lua/index.lua`/`trial.ini`）与 B（util 侧 `call_indirect` 越界 `RuntimeError: table index is out of bounds at …artemis.wasm:wasm-function[666]:0xa63cd`）；全手写冷跑 61 次请求后 `Uncaught TypeError: Cannot read properties of undefined (reading 'get_exception_info')` in `___cxa_end_catch`（根因未定）。
- f3633 装配余留：9 处 `I32_TRUNC_S_F32` 未定义（首个装配行 10467/10700/10933/25858）须改 `static_cast<uint32_t>(static_cast<int32_t>(number))`（越界回退保持 `2147483648u`）；第 2890 行残留裸 `i0`；staging 缺 part19/20/35（resume 码缺 294..332、634..654）；W01 契约项 `pending_throw_path` 被引用但无处定义；`lua_tags_builtin.part33.cpp` 有一行 102 列。

#### 4.3.3 独立现代宿主 `packages/runtime-artemis`（当前主线）

- 已接线（以较晚摘要 #49/#50 为准）：`src/runtime.ts` 接独立 core、ASM/IDB/audio、boot 前 hydrate、归一输入、save/restore、生命周期竞态归属、动态 availability（`hasFailure: sessionFailed` 修掉 `undefined` 失败被当健康；`supportedCommandCount` 取 `trace.honoured.size`）；`index.ts` 亦导出 runtime/assets/session-core/browser-services/audio。**但「已可用」未被独立 host 冷跑证实**。
- 分片：第一片 fd 桶落在 `@online-gal/vfs` 上 = **已完成**；第二片 runtime(8)+eh(17)+asyncify(4) 并让 `bridge.wasm` 首次在无官方胶水下 boot = **已完成**；「boot 卡在 guest 自己的 main，判定帧 660/657/656」= **已完成**；当前 in_progress = **gl 桶 99 行真实现**（task-25）、**trap-diff FinishTableIndex**（task-26）。
- **当前失败点（最新）**：`db70bd0` 冷跑暴露 trap/exception-host 问题。同宿主下官方 WASM 持续绘制并启动音频、对照 50 秒未触发；GitHub bridge 约 **3 秒**进入「异常指针为 0」的分支。已排除：①「导出 ABI 漂移」与「g3 是异常标志」不成立（`Lc` 表存在，源码明确 g3=Asyncify 运行阶段）②那处**重复 guard 在故障前一次也没执行** ③「正常 catch 结束后被重放」不成立（首次进入异常查找时宿主异常指针已是 0，此前无 C++ throw 记录）。观测到的恢复点 **`602`**。下一步：观察点前置到每个异常查找调用之前，定位第一次 `c()` 的源码位置。
- EH（异常 ABI）：`host/eh.ts` 改动**未接受**（被拒版本 688 行/约 +734 行：漏 `CatchInfo.base`、凭空发明指针搜索/释放）；`host/boot.ts` 另有约 +31 行未接受；worker 的 `test/artemis-eh-abi.test.mjs`（~380 行）**无效被拒**（声明 1 参而体用 2/3 参、嵌套数组畸形/无 end/无 imported memory、header 写成 payload+offset 而非 payload−16+offset）。Lead 自持 `.test/artemis-eh-independent.test.mjs`（134 行）**有效**：`RttiGuard` 非 `TYPE=28672` 时 trap、`28672` 时按 rig 返 0/1；脏 header 引用计数 **77** 在 `payload-12`；catch/rethrow 字节在 `payload-4`/`payload-3`。须**重读官方 SDK glue EH 例程**后再派修复。
- `trace.honoured`：`host/boot.ts` 约 `:557` 守卫 `const parked = thenable || setup.suspend?.state === ASYNCIFY_STATE.Unwinding;` 排除挂起的 `fa`，但**仍把未完成的非阻塞 `Qa`/`Jc`、排队 ASM 持久化、静默无效 `nc(0,720,0,0)` 计为 honoured**。证据 `.test/artemis-honoured-deferred-independent.log`（`pwsh-6704`）：15 测试 14 过 1 失败 —— `AssertionError [ERR_ASSERTION]: nonblocking transfer has not completed at the return` / `true !== false`，位于 `test/artemis-boot.test.mjs:121`；「34c」Flash worker 标识/结果需先恢复。
- 其它宿主组件：`host/audio.ts`（~1,650 行 WebAudio/OpenAL）编译过但功能 **7 项失败**（ID 恒 0、对齐错误；未做 state/pump、未接 `stringCache`）；`browser-services.ts`（~1,000 行）未验收（save-root 作用域、排队持久化、video EIO 孤儿、指针刷新、teardown 竞态）；`host/gl.ts` 三项反例待验（重复注册同 target/event 留 2 listener；异步帧中 pause 再 resume 应排 1 帧却排 0；同步 idle 抛错时 dispose 释放 `[]` 而非 `[8192]`），失败 worker 误删 `ticking:false`；`assets.ts` 已 **35/35**。
- 下载/回调契约（已定型，实现未完备）：`fa` 阻塞必须 park；`Qa` 成功 `viii(arg,buffer,length)`/错误 `vi(arg)`，回调真正完成后 free；`Jc` 立即返单调 handle，成功/进度 `viiii(handle,arg,buffer,len)`/`viiii(handle,arg,loaded,total)`，错误 `viiii(handle,arg,status,statusText)`（JS string→wasm i32=0），仅 flag 置位才 free；memcpy 用 `HEAPU8.copyWithin(dest>>>0,src>>>0,src+num>>>0)`；别名 `bd`/`dd`/`ld`/`id`（1/2/3/4 参），`Lc` 表回调**恰好 3 参**、fallback `ed` = entry+3 参；**不得用 `Function.length` 推断路由**；堆增长后须刷新所有视图。
- 组件测试（**均非真实游戏验收**）：assets 35/35、downloads 37/37、FD-IDB 14/14、GL/boot/suspend 112/112、session-core 10/10、`artemis-module-loading.test.mjs` 12、portable 313/313、`.test/artemis-public-integration.test.mjs` 15（被忽略）。
- 工程/发布：建议把 `rg2.mjs` 的 `__artcAbort` 提前退出 + `--profile` 合进 `real-game-test.mjs` 后删 `rg2.mjs`（后者至今**仍缺** `aborted` 提前退出补丁）。
- 许可：SDK 版权 `Artemis Engine`/`Copyright (C)2009 Daiju Hanaoka (Mikage) All rights reserved.`；根 GPL 与依赖许可**不能**为生成余留建立再分发授权，尚无结论；官方 SDK `Copyright.txt` 只授予向 Mikage 邮件获许可者/商业实体付费 ⇒ **专有，绝不可进 GPL-3.0-or-later 仓库**。KiriKiri/ONS 已下载证据不是 Artemis 验收；历史 release hash（尤其 `docs/fetch-engine-artifacts.md:282-283,370,373`）不得改写。
- 目标状态：`goal-83337ba3-1cc5-418a-b7f6-1a2570098709` revision 7、**paused/disarmed**、rounds 8/max 40；模型不能自行 resume（`Error: the model cannot resume a paused goal; the user must resume it`）⇒ 不得声称目标活动。

### 4.4 平台与 CI 线

- 已完成（团队流水）：docs 四件套、per-engine wasm 构建发布、消费侧 fetch+verify+回退、Artemis wasm2c 反编译可否 CI 再生、CI 产出即包（ONS 打样）、聚合包 `@online-gal/engines`、artemis 207 导入权威分类表、artemis「其他版本」退役、打包字节可复现、包内随附 LICENSE + THIRD_PARTY_NOTICES（GPL §5）、krkr/artemis/engines 产出 package、LICENSE 行尾泄露修复、构建新鲜度守卫、聚合包跨 checkout 逐字节复现 + 引擎字节台账守卫、CI 自证「导入地图 = bridge.wasm 的 ABI」、发布前把聚合包装到仓库外验证（守 lockfile 不被 workspace 解析污染）。
- 结束时仍 in_progress：task-25（gl 桶真实现）、task-26（trap-diff）；成员 `trap-diff` active。
- 打包四 pin：`normalizeModes`(0755/0644)、`gz[9]=3`（node26 `0x0a` vs CI node22 `0x03`）、`normalizedLineEndings`（跳过 `assets/`）、未压缩 **tar-stream sha256** 当跨工具链不变量。
- 第三处泄露 = **`LICENSE` 行尾**：CI 35,149 B vs 本地 CRLF 35,823 B（差 674 = 行数），`git status` 静默（`eol=lf` 覆盖 `core.autocrlf=true`），由 `0520e3e` 修复。
- CI 字节可复现（三次 ONS 相同），**Windows 本地是离群**；Windows CRLF vs Linux LF ⇒ 判差异先归一化。本地 vs CI 桥差 124 B（第一步即不同；两次相隔很远的 CI 构建 digest 相同 ⇒ runner 可复现）。
- vcpkg：`lukka/run-vcpkg@v11` 的 `x-gha` 后端**从不导出** `ACTIONS_CACHE_URL`/`ACTIONS_RUNTIME_TOKEN` ⇒ 静默不存；改 `actions/cache@v4`+`files,$DIR,readwrite` ⇒ cold configure 3022 s vs warm 12 s，整 job 48m33s→14m07s。
- `wasm-rt-impl.o` 本地 4,524 B vs CI 4,634 B，两函数各 +37 B = 两个 `abort()` 守卫；修法 = `patch-runtime-alloc.py` + 两处均从仓库参考树编译。守卫在 `.inc`（LF）、`.c`（CRLF）两次 include；`.ref/**` 不受 git 跟踪，手改不回退。

## 5. 关键技术事实与规格

### 5.1 容器格式

| 格式 | 规格 |
|---|---|
| XP3 | 签名 11 字节 `58 50 33 0d 0a 20 0a 1a 8b 67 01`；index offset 在**字节 11**（`findIndexStart()` 前扫 ≤ `INDEX_PREAMBLE_BYTES=12`）；`tag[4]+size[u64le]`；index flag `&0x07`：0=raw、1=zlib，`0x80`=continued；`0x80` 重定向 → 第二 offset 在 **+9**；`info`=`flags[u32] orgSize[u64] arcSize[u64] nameLen[u16] name[UTF-16LE]`；`segm`=28 字节记录；实测 body：0 `"info"`、8 u64le flags、16 packed、24 original、32 u16le nameLen、34 UTF-16 name。**禁记**：`info` 不是 var-int；`segm` chunk size 不能当跳距 |
| NSA/SAR | **大端**；字节 6 起 `u16be` count + `u32be` base offset；记录 `name\0 u8 compression u32be offset u32be size u32be original`；条目**默认不压缩**（`fseek`+`fread`）⇒ 天然随机访问；压缩变体 NBZ/LZSS/SPB；不压缩条目直接给字节范围，不得无条件 `inflateZlib()`；增量窗口 16384 KB→**1280 KB** |
| pf8/pf6 | header `0 "pf" 2 version 3 index_size u32LE 7 count i32LE 7 index`；记录 `[u32 name_len][name][u32 pad][u32 offset][u32 size]`，步长 `name_len+16`；key=`SHA1(index[7,7+index_size))`，仅版本 4/5/8/9 加密；payload 自条目起点 XOR（子范围需 `start % 20`）；**索引明文**；名字 **CP932**；**版本必须 6**；pf6 索引尾部（GARbro 与 `pfs.ts` 都漏）：`u32 record_count+1`/重复 `record_count` 次 `u32(该记录名之后的 index offset), u32 0`/`u32 0,u32 0`/`u32(最后一条记录之后的 offset)`，漏掉 ⇒ `Assertion failed: mlf_ >= minimum_max_load_factor` |
| 按需读取下界 | **1,185,782 = 7 + index_size**，实读 **1,189,400**；旧值 `11 + index_size = 1,185,786` **已废弃**（`tail.md` turn 111 记录仍写 11+index_size，须按 7 核对当前实现） |
| 真实归档 | `root.pfs` **2,690,454,559 B**（21,587 条）；`root.pfs.000` **3,355,688,890 B**（7,637 条）；合计 **29,224** = loose 树文件数；key `f86f6cefdd968cacf252ad67efb2c1683446a1e3`；`system.ini` 3,618 B → `WIDTH=1280 HEIGHT=720 FPS=60 CHARSET=UTF-8 SIDECUT=0`，`[WASM]` 段**无 `SAVEPATH`/`ASYNCIFY_URL`**；HTTP Range 实读 **22.8 MB / 6,045,144,449 B = 0.38%**；>2 GiB 偏移字节精确；偏移以负 int32 到达，用 `>>> 0` 还原 |
| 内容统计 | 真实游戏用到 **59 个引擎 tag / 478 次**（`lyprop`123、`var`43、`exec`29…）；`.ast` 命令另一套词汇（37 个/89,074 次，`text`32092、`fg`19985…），名字表 `system/init.lua:5599`；`write_pf8` 与 `GeneratePackFile2.exe` **逐字节相同**（3752 B）；hashlist 29,224 条、`path,sha1` 每行 LF、**无尾换行**；`SAVEPATH` 是 IndexedDB 缓存键（JP/CH 共用会静默读错字节）；中文部署非黑 **2,851,932/3,041,101 = 93.780%** |
| 打包禁令 | `patch_system_ini(...,output=)` 会删掉未传入的 managed keys（`SAVEPATH`/`ASYNCIFY_URL`/`ASYNCIFY_HASH_LIST_URL`）⇒ **手工改包永不安全**；pack/INI 不匹配 ⇒ `mlf_ >= minimum_max_load_factor` |

### 5.2 桥 ABI 与 splice

- ABI：**207 函数 import + 1 memory import `a.a`**（3072 pages = 201,326,592 B，max 65536）= **208 imports**、**198 distinct** 函数名（9 个重复改名）；桶 `{gl:99, fd:48, async:30, eh:17, runtime:8, asyncify:4, todo:1}=207`；导出 **67**；表导出名 **`Lc`**。
- 表槽 → fnid：`artemis.wasm2c.c:103573` 是 elem 段 `w2c_artemis_e0`，**槽 N 在第 `103573+N` 行**（4,277 条）；已验证 857→f5442、987→f209 SetStackTop、1001→f5726、1002→f920、2328→f3857、3373→f772。
- splice 纪律：`rw-splice.py` 按 `rewrite-names.tsv` 用自写体替换 `w2c_artemis_f<NNN>`；thunk 在**全局作用域**（`rw-splice.py:159`）⇒ target 必须带 `artc::`；tsv 列 `fn/fNNN/current/target/file/depth/note` 只用第 1/2/4/5 列（第 6 列 extras 必需，多余项惰性无害）。**`ARTEMIS_REWRITE` 缺 `rewrite-names.tsv` 会静默产出未拼接的 41,864,950 B 桥且仍过帧锚**（假绿灯陷阱）。
- 运行期：掩码 `static volatile`（防 O1 折叠）；void 生成臂 `gen(...); return;`；`_impl` 前置声明先于 chooser；生成层 thunk 在 runtime 模式须转发 chooser（`target if runtime else gen`）；passthrough 三语义（生成体改名 `_wasm`／产品体 `<short>_disabled_probe`+前向器／thunk 转发 `_wasm`）。
- 掩码 `RW_PT_RUNTIME=1`：340 bits/11 words（`static uint32_t artc_pt_mask[11]`，位序按 relfile，`pt-map.tsv` 同目录）；**置位 = 该产品静音（用生成体）**；哨兵 44 B 在 `29341378`/`0x1bfb6c2`，未打补丁时全位 1 ⇒ 每跑先 `ptmask.py`。`ARTC_OPT_LEVEL` 默认 **-O1**。
- 排查纪律：Chrome `wasm-function[N]` = 含 import 的 `func[N]`，**绝不减 207**；`rw-splice:` 行约 30 KB ⇒ 只 `grep -oE "rw-splice: [0-9]+ product"`；永不 `grep -v wasm_rt_local_memory`；`grep -n "^void w2c_artemis_f265"` 会匹配 `f2658`；`grep -c 'EnterFrame();' …/artemis.cpp` = product 数可判桥出处。
- 工具真相：有效门 = `style-gate.sh` + `wasm-diff.mjs`（67/67，早期记 66/66）+ `wasm-replace-test.mjs`（4/4）+ `npm test` + 真游戏帧锚；`sig.py` 只有 4 项有效（`leftovers: none`；首行 `#include "util/engine_context.h"` 且 `artc=1 closing=1`；`signature SAME`）。
- 差分/判定工具：`D:/tmp/dt2/`（`dt2.py run|accept|sweep|one`，`--keep` 才留 `work/<slug>/`；生成器 `lib/dtgen.py` 的 `parse_engine_context` `:272` 扫 `CALL_INDIRECT` → 44 键 `w2c_artemis_t1…t44`；`build/dt_runtime.cpp` 是生成产物 ⇒ 只改 `lib/dtgen.py`）；`/d/tmp/bisect.py`（模式 `one|list|bisect`；`BISECT_SRC`(`:9`)、`BASE=D:/tmp/verify-base`、`STATE=D:/tmp/bstate`、`WASM=ports/artemis/reverse/out/bridge-asyncify/bridge.wasm`、`GAME=.test/artemis-official-async/game-chs`、`WAIT=BISECT_WAIT||120000`、`CLEAN=D:/tmp/bisect-clean2.tsv`，`build()` 走 `bash -lc "source ports/env.sh && ARTEMIS_REWRITE=D:/tmp/bstate …"`；**判定需两次一致测量**，否则 `UNSTABLE … -> unknown`）；权威判定已由 grouping gate（`gbuild.sh`/`grun.sh`）取代；堆指纹探针 `D:/tmp/addhash.py` 用 FNV-1a（起 `2166136261u`、乘 `16777619u`）。

### 5.3 Asyncify 契约与相位门

- `artemis.wasm2c.h:31-39`：`struct w2c_artemis { w2c_a* w2c_a_instance; wasm_rt_memory_t* w2c_a_a; u32 w2c_g0..g4; wasm_rt_funcref_table_t w2c_Lc; }`；**g0=GuestStackSp、g3=ResumePhase、g4=ReturnFrameSp**。
- `util/engine_context.h`：`:17 namespace artc {`；`:18 using EngineContext = ::w2c_artemis;`；`:25 LoadU32`；`:43 kNilValueAddr=176936u`；`:47-48 EnterFrame(){FUNC_PROLOGUE;} / LeaveFrame(){FUNC_EPILOGUE;}`；`:52 ResumePhase=ctx->w2c_g3`；`:53 ReturnFrameSp=ctx->w2c_g4`；`:54-56 kResumeRunning=0u / Suspended=1u / Rewinding=2u`；`:63 GuestStackSp`；`:336` PROVISIONAL trampoline 块；`:593 SyncMemoryBase`；`:639 kDataD015C=0xD015Cu` 后接提升五常量 `kData1FFFF`/`kData20000`/`kData7FFFC000`/`kDataFFFFFFFF`/`kControlStructureTooLongMessage`；**`:643` 唯一共享 `constexpr uint32_t kDataD015C = 0xD015Cu;`**（852316 = asyncify 状态字，`:645` 闭合）；`:602-686` 为无 tsv 行 callee 的 inline jumper（`ReleaseFontRecord→f3206`、`AccumulateGlyphBounds→f1410`、8 个 Lua tag helper 与 4 个 Lua 值 helper）。
- 宿主导出：`Ud`/`Vd`/`Wd`/`Xd` = start_unwind/stop_unwind/start_rewind/stop_rewind，**无** `asyncify_get_state`；`Nc`(malloc)/`Oc`(free)/`Pc`(errno)/`Xc`/`Yc`/`Zc`；`artemis-export-shims.c:115-119` 权威签名 `u32 artc_export_Zc(u32 p0)`（**1 参**）。
- 挂起语义：import 触发 unwind → 外层 raw-root 边界 stop unwind → Promise settle → start rewind → **重放同一 raw 函数/参数/receiver** → rewinding import stop rewind、释放块并返回已存值；包装 root 返回占位，须 `await waitUntilIdle()` 再读 `driver.result`。phase=Suspended ⇒ 门为真，随后置 1 并立即 `goto suspend_point` 存帧 ⇒ **每个挂起点都要显式 `resume_point = N;`**。
- **闸极性（生成代码即规范）**：闸形 `phase ? (rp==K) : 1` ⇒ 家风格 `if (ResumePhase(ctx) == kResumeRunning || <flag>)`；闸中 `!=` 反向；`? :` 选择式仍用 `!=`；单相运行块 `if (ResumePhase(ctx) == kResumeRunning)`。**活臂**：合法 `!= kResumeRunning ? x : <compute>` 或 `== kResumeRunning ? <compute> : x`；非法 `== kResumeRunning ? <live> : <compute>`；裸 `ResumePhase(ctx) ? A : B` = rewinding 臂在前。
- 必须不变（`REWRITE.md`）：`namespace artc`、文件路径、函数名/签名；正文顶部恰好一个 `EnterFrame()`、**每个 return 前**一个 `LeaveFrame()`；每个 `Load*/Store*`/host 调用的**大小、有效地址、顺序**；整数宽度与截断点；resume 分支原样；被调工作须在 `LeaveFrame()` 前。
- **单 TU 纪律**（`REWRITE.md` §7.2）：全部产品同一 TU/`namespace artc`（合并文件 651 次）⇒ 文件作用域名全树唯一（已被五常量同名撞过）；初始化局部声明不得被 `goto` 跨过 ⇒ 常量用函数内 `constexpr`。跨产品调用：有 tsv 行用裸真名 + 前向声明块（`// Forward declarations of the callees below.`），无 tsv 行用裸 `w2c_artemis_fNNN(ctx,…)`。
- 风格靶 `.ref/artemis-compat`（pin `9f622629f595db85e6f76c45c06edde29d7d9961`）：无 banner、`namespace artc {`、真名与真控制流、一行注释、`} // namespace artc`；2 空格缩进、≤100 列、C++17、无 std 容器；禁 `block_N`/`loop_N`/`func_exit`/`spill_frame`，挂起标签 `suspend_point`。布局：TValue 16 B ⇒ `<< 4u`；指令字 4 B ⇒ `<< 2u`。

### 5.4 Emscripten / 构建坑

- `EM_JS`/`EM_ASYNC_JS` **都无法 marshal `const char*`** → 传静态缓冲整数地址；`ALLOW_MEMORY_GROWTH` 默认**关**；`--shell-file` 除非输出后缀 `.html` 否则静默忽略，且 shell 改动**不触发重链接**（需 `LINK_DEPENDS`）。
- `Module._fn(string)` 传垃圾 → 用 `ccall`/`cwrap`；`FS.writeFile` 嵌套路径需 `FS.mkdirTree`；无 `main` 需 `--no-entry -sINVOKE_RUN=0`；Asyncify 插桩导出**不返回函数值**；`-sPTHREADS=1` 被拒 → `-pthread`。
- 主线程禁 `Atomics.wait` 与同步 XHR + `responseType='arraybuffer'`（`TypeError: Atomics.wait cannot be called in this context` / `InvalidAccessError: Failed to set the 'responseType' property …`），worker 内合法 ⇒ **引擎必须整体进 worker**。
- 缓存 sanity key=`"<version>|<LLVM_ROOT>"`，不匹配即 `cache.erase()`；**导出 `EM_CONFIG` 会翻转 `LLVM_ROOT` 斜杠方向** → 抹缓存 → `Could NOT find ZLIB`、`attempt to lock the cache while a parent process is holding the lock`；实测 `EMSDK=D:/emsdk`+unset 存活、+`EM_CONFIG` 被抹 ⇒ **一律不得 `export EM_CONFIG`**；恢复顺序 `embuilder.exe build sysroot` **然后** `build zlib`。
- `-fno-exceptions` 下 `std::stof` 直接 abort（`invalid_argument … "stof: no conversion"`）⇒ `Compositor::ToF/ToI` 改 `strtof`/`strtol`+`errno==ERANGE`、`ZCmp` 用 `strtoll`，替换 24 处 `std::stof`+2 处 `std::stoi`。
- 其他：`set(CMAKE_CXX_FLAGS …)` 对自身 target 无效→`target_compile_options`；shim 被 DCE→`EMSCRIPTEN_KEEPALIVE`；`thread pool is exhausted`→`-sPTHREAD_POOL_SIZE=6`；`still waiting on run dependencies: loading-workers`→pthread 重 importScripts 吞掉 `cmd:1`⇒用 `self.name.indexOf('em-pthread')===0` 守卫；`--shared-memory is disallowed by ldo.c.o`⇒`-pthread` 须加在 android 块**内**；emsdk **6.0.9**；`DETERMINISTIC` 已退役（`tools/settings.py:267`）；zlib 是 emscripten port，需 `embuilder build zlib`。
- 宿主构建：`.test/artemis-web-build/build-emscripten.sh` 三阶段（core/cli/cli-node，含 `NODERAWFS` 与内存增长标志）；`.ref/artemis-compat/CMakeLists.txt` 的 `artemis-web` 目标（`--no-entry`、WebGL2、内存增长、shell 作为链接依赖）；宿主页硬编码 `Module={INITIAL_MEMORY:201326592}`（192 MiB）；`LAYOUT_BASE`=176 MiB=184,549,376，由 `build-bridge-route.sh:279-320` 注入。
- 16 MiB RESERVE 实验失败（`bridge.wasm` 40,773,979 B/sha256 `5cd2235dfffcc2fb`，run1 同指纹）⇒ 回退 4 MiB ⇒「堆 arena 太小」不是原因。
- 官方 sync 按需加载（若再走该路）：官方胶水 `FS.createLazyFile` 用同步 HEAD + `doXHR`（同步 GET+`Range:`+`arraybuffer`+`overrideMimeType("text/plain; charset=x-user-defined")`）；Chrome 禁主线程同步二进制 XHR，胶水在字节 **78,947** 守卫；主线程实测同步 `x-user-defined` OK、同步 `arraybuffer` 抛错、**同步 Range + `x-user-defined` → 206、65,536 B ✅**；设计 = patch 守卫 + 同步 XHR shim + 把 pack 注册为 lazy file 并清空内嵌 `files`。

### 5.5 V8 与 -O1

- **-O0 不可用**：`WebAssembly.compile(): Compiling function #1509 failed: local count too large @+4525972`（**95,112 locals > V8 50,000**）⇒ 生产 `ARTC_OPT_LEVEL=-O1`（该函数降至 **135 locals**）。O1 下**非 volatile** 掩码会被折叠、void chooser 双分支执行 ⇒ 掩码必须 `static volatile`；修复后 11/11 回归才可用。

### 5.6 CDP warm-origin 观测坑

- `Storage.clearDataForOrigin` 曾传非法枚举（`indexdb,websql,locals_storage,…`，正确为 `indexeddb`/`local_storage`）且 `.catch(()=>{})` 吞错 ⇒ **历史 PASS 全在 warm origin**；修正后才暴露冷启动缺陷。后果：`--wait 45000` 的 stall 指纹是**假象**；判定协议 = **丢弃一次热身 + 两次一致运行**；机器须空闲（三个子代理同时忙会误报 `FAIL: fetched but drew nothing`）；无残留 `python -m http.server`。
- 冷启动串 ~18 MB、>90 s 才出第 1 帧 ⇒ `--wait ≥ 240000`（**300000 才稳定 PASS×2**）。
- 健康指纹样例：`pass=True`、`g=87`、`lit≈87.5–97.11`、`frames=3`、`bytes≈17,867,773`、`finalHash=550f7464`（或 `6029ab5`）；全树 313 文件 `--wait 300000` PASS×2 = `g=87 bytes=17,867,773 lit=97.11 frames=3 fh=550f7464` = 纯 HEAD 同指标。
- 其它观测污染：`evaluate_script` 建的 `<img>` 覆盖层**从未移除**，又被 `take_screenshot` 的**缓存帧**放大（sha256 全同 `21bd643b…`）⇒ 误判「重影/重叠是引擎 bug」；`resize_page` 至 1500x900 后文字干净。**绝不** `document.body.innerHTML=''`（毁 OffscreenCanvas）；**绝不**在实时页面留注入 DOM。
- 观测窗口偏差 ≥3 次：`head`/`slice(0,N)` 挤掉关键记录（62 个 `off=0 len=11` 的 `OpenChain` 探针填满窗口）；打印原始 wasm 字节致 import 误解析（`env total: 1`）⇒ **按被测属性过滤，绝不按条数；绝不打印 wasm 字节**。

### 5.7 浏览器实例与端口

| 端口 | 用途 |
|---|---|
| 4599 / 4620 | ONS 基线 / ONS worker |
| 4630 / 4650 | krkr web / artc Android-host |
| 4700 / 4770 | 官方 wasm（`official-web-deploy.py` 默认）/ `.test/ab` A/B 场景（配置错误，需重建） |
| 4710 / 4711 | Artemis 自己的浏览器 / Range 服务器 |
| 9222 | **chrome-mcp 的 Chrome，一律不要动**（`--user-data-dir=…\chrome-mcp-profile`） |

- `ports/artemis/harness/browser/chrome.mjs:44 ensureChrome(port=9222)` **复用已在运行的 chrome-mcp 浏览器** ⇒ profile/代码缓存跨轮残留。`taskkill //PID <pid> //T //F`（MSYS 要 `//`）只用于清理自己的 4710/4711；MSYS pid ≠ Windows WINPID（`ps -W` 第 4 列），`pkill -f` 不可用。
- `serve-range.mjs` 支持 **206**、同源（无 CORS 头）、不存在路径 404 无扩展名回退；**勿用** `coi-server.py`（Range 答 200）。

### 5.8 工具与路径坑

- **PowerShell 反复破坏 UTF-8 与引号**（README ×3、正则变 `/鏄庢枘/`、行被合并、全角空格被改）⇒ **一律用 `write`/`edit` 与 `.mjs` 脚本，禁用 PowerShell 写文本与 `node -e`**；改完用 Node 校验编码。
- `write`/`read`/`edit` 只认 **Windows 路径**（`D:/online_gal/...`）；传 `/d/...` 会变成 `D:/d/...` 并静默建树；`edit` 前须 `read`，多行 `old_string` 常失败 ⇒ 单行锚点或 `python - <<'PYEOF'`（写文件必须 `encoding='utf-8'`，否则 `UnicodeDecodeError: gbk`）；`edit` 会报 `ReplaceFileW EIO (Win32 1175)`。
- `subprocess.call(shell=True)` 走 cmd.exe ⇒ 一律 `bash -lc "…"` 且 `encoding='utf-8'`；Git Bash `/tmp`=`C:/Users/ASUS/AppData/Local/Temp`；`.test/package.json` 是 `"type":"module"` ⇒ Emscripten CJS 产物须 `.cjs`；测试 import harness 必须写 `.ts`。
- 私有仓库：匿名 API 与未认证资产请求都 404（与「未发布」不可区分）；token 由 `git credential fill`（`protocol=https`/`host=github.com`）取 `password`；资产下载 `curl -sL --proxy socks5h://127.0.0.1:10808 -H "Authorization: Bearer $TOK" -H "Accept: application/octet-stream" https://api.github.com/repos/fylcr/online_gal/releases/assets/<id>`；`gh` = `/d/programe/github cil/gh`（路径含空格）；`gh api` 不接受 `-R`；`?head_sha=` 刚 push 后可能 `total_count 0`；`DELETE /releases/tags/<tag>` 404（按 release **id**）；并发下大资产会静默截断（串行 + 验 sha256）。
- `wasm-objdump.exe` 输出 CRLF ⇒ 先 `replace(/\r\n/g,'\n')`；`git ls-files` 八进制转义非 ASCII 路径（用 `-c core.quotepath=false`）；grep 工具跳过点目录（查 `.github/` 需显式 `path`）；`.ref/artemis-compat` 的 `core.autocrlf=true` 让 worktree 全 CRLF；管道退出码是 tail 的（用 `${PIPESTATUS[0]}`）；`git commit -F -` 不带 pathspec 会扫进他人 staged 删除；`D:/tmp/dt2` 新链接的 exe 会被杀软隔离（有时四个 opt 探针全失败 `good=[]`）⇒ 让脚本建 exe 后手工跑；`take_screenshot`/`evaluate_script` 的 `filePath` 写工作区被拒 → 用内联截图。
- 元教训：**同类失败反复出现时，怀疑工具选择而非单个实例**；最危险的失败形态是「**构建成功但完全没做它该做的事**」；`chrome-mcp` 间歇 `Failed to connect to MCP server`（Edge 未开必现）→ 改用 `chrome-devtools` MCP。

## 6. 已被推翻 / 已废弃的结论（防重做）

### 6.1 路线与角色

1. **Tyranor-Next 自称 GPL-2.0-only** → 实为 **GPL-2.0-or-later**（`LICENSE` L297-299 含 "either version 2 … or any later version"），代码可移植；上游 `README.md` L171 与之矛盾，值得上报。
2. **用 Godot 兼容 krkr/Artemis** → `aether_kiri_godot.wasm`（52.6 MB，26,292×`TJS`、130×`XP3`）在 `1.0.4-alpha.1` 与 `1.0.3` 下均 `WebAssembly.instantiate` 失败：`RuntimeError: memory access out of bounds`。
3. **「位置不对是引擎代码问题」** → 用户「安卓端正常运行，用的是一样的代码」⇒ 宿主外壳问题。
4. **AetherKiri 有可用的 Artemis 模拟代码**（m07407）→ 其 runtime 私有/不可用。
5. **`Windows/Artemis.exe` 能提供命名语义** → 已加壳（131 函数、`Analyzed:false`、317 符号）⇒ 以 `.so` 为准。
6. **sync 风味仍在路线图**（序号 25）→ m24411 踢出，只出 asyncify；`.test/ab/game-packs`（5.7 GB）已删；`docs/artemis-web-port.md` §19.4 仅作历史证据。
7. **plain/sync 能跑本作** → pack 5.63 GB > wasm32 上限 4.00 GB；一个 tab 只提交 ~1.75 GB（2 GB→`RangeError`）；plain 缺 `_emscripten_wget_data`；**`ASYNCIFY_URL` 对 sync 完全无效**（两份胶水里 0 次，是 guest 读 ini）。
8. **重建 wasm 不能直接替换官方 wasm**（曾列三阻塞）→ `build-bridge-route.sh` 打通后角色两次变化（验证证据 → 真候选 drop-in）；Gate3 残余 `trap-kind-only`（asyncify 46/plain 2）是链接期间接表填充率，非转译错误。
9. **`tx-gen2.py` 自动生成主线**（m24494）→ m27387 停止生成、逐文件手改；`--check` 的 identical 计数**不再是漂移证据**；「生成输出不得手改」（REWRITE.md §8）已划掉。
10. **m21048「完全源代码重写」** → 早期实测约 **2.8%** 手写（157/5,610）；只有 §16 伪 C（`artemis.wasm.pseudo.c` 42,325,228 B，覆盖 5,609 函数）是全量的；序号 27 为 293 生成/26 手写。
11. **「不用子代理」**（m23873 写 `USER.md` critical）→ m27431/m28037/m28046 放宽。
12. **`runtime.ts` 是空壳、public runtime 无条件不可用** → 后续已接线独立 core/ASM-IDB-audio/hydrate/输入归一/save-restore/动态 availability；**但「已可用」也未独立冷跑证实**。
13. **`Zc` 取 0 参** → `artemis-export-shims.c:115-119` 为 `u32 artc_export_Zc(u32 p0)`（1 参）。

### 6.2 桥 / ABI / 构建

14. **「bridge 只导入 9 个 host 函数」** → 真值 **207 函数 + 1 memory**（208 imports、198 distinct）。
15. **`1,163 exports`**（序号 25 与其运行时日志 `LINK OK, exports 1163`）→ **序号 26 起统一为 67 exports**（wasm-diff gate **67/67**）。
16. **按需读取下界 `11 + index_size = 1,185,786`** → 正确 **7 + index_size = 1,185,782**（实读 1,189,400）。
17. **`-O0` 可用** → `local count too large`（95,112 > 50,000）⇒ 生产 **-O1**。
18. **旧运行期掩码结论**（O1 折叠非 volatile 掩码 + void chooser 双分支执行）→ 修复后 11/11 回归才可用。
19. **`-fno-exceptions` 下 `std::stof` 安全** → 直接 abort ⇒ 全量替换。
20. **`export EM_CONFIG` 无害** → 翻转 `LLVM_ROOT` 斜杠、抹缓存、`Could NOT find ZLIB`。
21. **`DETERMINISTIC` 可用** → Emscripten 6.0.9 已退役（`tools/settings.py:267`）。
22. **16 MiB RESERVE 能修 stall** → 失败并回退 4 MiB。
23. **未拼接基线不会「通过」** → 41,864,950 B（缺 `rewrite-names.tsv`，日志 `rw-splice: no rewrite-names.tsv yet -- nothing to splice into artemis.cpp`）**也能过帧锚**；`ARTEMIS_REWRITE=/d/tmp/x` 会被解析成 `D:\d\tmp\x`。
24. **「ONS 本地 vs CI 只差 2 data 字节」**（旧表只比长度）→ 以 CI 字节为基线；**「krkr 现有 CI 清单不可采纳」** → 修好 `build-krkr.sh` + 固定版本后**采纳确定性 CI 字节**。
25. **`cf/s=2`、`draw[]` 150 s/300 帧 = 2 Hz** → 两者都被 `native_activity.cpp:562 if (drawn_rev != Revision() || animating)` 门控，静态标题上测的是门开次数；`loopdbg` 实测 **loops≈32/s**（`eglSwapInterval` 被 `--wrap` 故意返回 0 ⇒ 每帧 sleep 16 ms）；**绝不从 `draw[]`/`cf/s` 读循环速率**。**「`loopdbg` 无输出 = 代码没跑」也错**（worker `:175` 的 printErr 是白名单正则）。
26. **其它自家错误**：「A 从不调用 wget 族」；「`_emscripten_wget_data` arg0 是结构体」（胶水用 `UTF8ToString(url)`）；「5,998,665 B 的 control 是渲染 oracle」（loose 树 24 次游戏请求全 404；`e5e2043` 更正为「loose bench 只测 ABI 不测渲染」）；「`trap(0)`」；「`wasm-function[N]` 要减 207」；「`_glfwInit` 把 0 当成功」（须返回 1）；「compat 也探 1,000 分片」（compat 64、官方 1,000）。
27. **Ghidra 可逆向 `.wasm`** → wasm 插件 v2.4.0 在 Ghidra 12.1.4 加载即崩：`java.lang.NoSuchMethodError: 'void wasm.format.StructureBuilder$SBStructure.checkAncestry(...)'` at `WasmLoader.doLoad(WasmLoader.java:689)`；`decompile_function` 会按相同代码折叠返回错函数 ⇒ **必须核对返回 `name`**。

### 6.3 判据与观测方法（最贵的教训）

28. **bisect 的「BASE+subset」前向窗口法** → 窗口 `g=6 stalled exc=0` ≠ 全树 `g=0 abort exc=1` ⇒ 反向 bisect；**又被推翻**：≥2 个独立 trap 时反向切片会把 TRAP 翻成 STALL ⇒ 改失败模式感知 `fbisect.py` `(p,g,exc)` ⇒ 终由 **grouping gate**（`gbuild.sh`/`grun.sh`）取代为权威。
29. **单文件定罪**（`script/format_number.cpp` body 是元凶）→ 同一份失败字节换路径后 **PASS 3/3**，其 stub 版（`/d/tmp/fm_stub.cpp`，md5 `ec73223cb5f6912f2108c23aadf8740d`）也 PASS。
30. **`#6` 归属 `script/lua_engine.cpp`** → 序号 36：`lua_engine.cpp` **不在** 311 个修改文件中；BASE（全 HEAD，41,296,963 B）**2/2 PASS**、live 树（40,773,758 B，327 products）**5/5 FAIL** ⇒ 缺陷在 311 文件带内，收敛到 **`files[77:155]`（78 个 `script/` 文件）**。
31. **「同字节 ⇒ 同结果」与「9222 上的 Chrome 要杀掉」** → (a) 判据 context/timing 敏感，`ensureChrome` 复用 chrome-mcp 浏览器、profile/代码缓存跨轮残留，刚 FAIL 的同一份字节稍后测 **3/3 PASS**；(b) 9222 上的 10 个 `chrome.exe --user-data-dir=…chrome-mcp-profile --remote-debugging-port=9222` 属 **chrome-mcp，一律不要动**。**判据性结论必须做对照**。
32. **短 wait 的 stall 有意义** → 全树 313 文件 `--wait 300000` PASS×2 = 纯 HEAD 同指标；纯 HEAD 在 `--wait 45000` 也打印 stall 指纹 ⇒ **「已知良好桥突然失败」的真因是历史 PASS 全在 warm origin**。
33. **已废弃的门/工具**：`gatecheck.py` 降级、`gatecheck2.py` 因系统性过度计数**弃用为门**；`sig.py`/`base-prov*.py`/`resume-scan`/`arm-scan`/`litscan`/`elsescan`/`dup-scan`/`lblocks`/`depthcmp`/`addrcmp2` 全为**线索**；`.test/forward-verify.sh` 曾为权威流水线，**已废弃**（`isolate()` 会把单元集直接回显为 culprit 而不过测；构建失败 rc=2 被当 FAIL）；`.test/bisect-anchor.sh` 前提也失败；`sig.py` 的 `VERDICT` **不是门**（干净样本也 FAIL，且发现不了丢门）。
34. **旧 ledger `D:/tmp/bisect-clean.tsv`**（及 79 行 backfill）作废 → 改用 `D:/tmp/bisect-clean2.tsv`。
35. **弱证据纪律**：dt2 `kind=trap` 分歧**不是**定罪（通常只是走到未 stub 的 callee）；`kind=trace_len` 单独弱（`expect_token` 一例是 stub id 互换 `0x1ED2A2/0x1ED2A3` 的假阳性）；64-trial CLEAN 仅指前 64 trials；**调用多重集 ≠ 行为**；机械扫描看不见「门作用域、短路 goto、语句嵌套、标签位置、值写给了哪个变量」。
36. **组件绿 ≠ 验收**：全生成掩码桥健康、官方 glue 健康、ABI/结构/合成测试、白帧、worker 回执均**不构成**手写引擎或现代宿主真实游戏验收。

### 6.4 已清洗的误判嫌疑文件（勿再当嫌疑人）

`script/format_number.cpp` f900（后 `A2b` PASS×2）、`util/reclaim_free_block.cpp` f1212（红鲱鱼）、`util/create_shared_record.cpp`（dt2 CLEAN）、`util/integer_to_string.cpp`（`C2` PASS×2）、`script/expect_token.cpp`、`render/init_layer_storage.cpp`、`render/release_layer_render_data.cpp`、`util/release_plain_node_tree.cpp`（`A1` PASS×2）；`f4467` 是叶子访问器 `LoadU32(p0)*12+p0+32`。

### 6.5 已回退 / 已作废的改动与说法

- 深度 tiebreak 层序（`ZCmp` 只在 id 相等返 0，tiebreak 不可达；可达后把 depth-4 `500.b.*` 按钮压到 depth-3 `500.ch.*` 角色上）⇒ 退回 `int ZOrder(a,b){return ZCmp(a,b);}` 并注释**禁止重做**；自创 `IsOff()`/`OffStage()`；帧配速实验（用户「不行」）；`patch-prejs.mjs`。
- docs 错误：§9.21「`off=当前值`」错（`off` 就是 `0`）；§9.14「黑屏」结论错（真实原因是 canvas target 未解析）；`docs/artemis-pfs-format.md` 曾错误声称「尾部不必读」（密钥需整个索引参与哈希）。
- **`TagWait` 会等 tween** → `lua_tags_nav.cpp:50-107` 明说不等慢速装饰性 tween；但 `PendingAnimationMs` 会算进 tween，故 `wait {input=1,time=0}` 在 158 ms 返回（应约 9,985 ms）。
- **对话「重影/重叠」是引擎 bug / 动画定位缺陷由我们改的代码引起** → 前者是自造观测污染（`<img>` 覆盖层 + 截图缓存帧，sha256 全同 `21bd643b…`）；后者逐帧 transform 链完全精确（`.p.m x=-450 y=300 ax=640 ay=360`），而 CG 只覆盖 stage 的 x∈[0,1007]、y∈[617,1131]（stage letterbox 到 y∈[249,1141]）⇒ 宿主外壳差异。
- **`_main` 不返回就是失败**（`_main THREW String: unwind` 是 Asyncify 语义）。
- 已删除产物/路径：`engines.artemis` pin、`build-artemis.sh`、`build-wasm-artemis.yml`、`web-deploy.sh`、`0001-web-host.patch`、`harness/android-host.html`、`artemis-web-android-worker.js`（均在 `58b43bf`）、`engine-packages-0.1.4` release、`.ref/artemis-official/rewrite/`。

## 7. 未完成清单与下一步顺序（Artemis 优先）

### 7.1 建议顺序

1. **定位 bridge 的第一次异常查找**：在每个异常查找调用前插观测，找出第一次 `c()` 的准确源码位置；恢复「34c」Flash worker 状态；**不得**用放过 `r(0)` 的方式掩盖故障；观测窗口与服务器用完即关。
2. **EH（异常 ABI）**：依官方 SDK glue + `.ref/.../artemis.wasm` 的 wasm2c 转写重建 EH layout/arity/catch 语义，派**窄范围** Flash worker 修 `host/eh.ts`（补 `CatchInfo.base`、禁发明指针搜索/释放），用 `.test/artemis-eh-independent.test.mjs` 独立验证；修或替换被拒的 `test/artemis-eh-abi.test.mjs`。
3. **`trace.honoured`**：修 `host/boot.ts`（排除未完成的非阻塞 `Qa`/`Jc`、排队 ASM 持久化、静默无效 `nc`，但不一刀切排除所有 0 返回）；独立重跑 `node --test test/artemis-boot.test.mjs` 确认 `:121` 已修。
4. **gl 桶 99 行真实现**（task-25，唯一挡路的墙）+ 独立复验 `host/gl.ts` 三项反例；随后修 `host/audio.ts`（7 项失败）与 `browser-services.ts`。
5. **`FinishTableIndex` trap-diff**（task-26）：只读差分我们的实现与参考转译体。已知现场：`frame 1 trapped with RuntimeError: unreachable`，最内 `wasm-function[10315]`，链 `func212(129 B,+83,唯一调用者 10286)` ← `func1012 artc::LoadU32` ← `func1116 artc::FinishTableIndex`；trap 记录 `code=1 offset=1,684,955,505 = 0x646E6171`（ASCII `"qand"`）、`mem->size=197,132,288` ⇒ 我们重写里的垃圾指针越界读，最可能 `ports/artemis/src/script/finish_table_index.cpp`。浏览器侧同次：`pfs table: 21587 entries`、`mounted: 21588`、`LINK OK, exports 1163`、`_main THREW String: unwind`、rAF 画 180 帧但 **guest 真帧 = 2**、lit 0.00%。
6. **冻结 shipping 源** ⇒ TS/package 构建 + ABI + tracked portable + source/layout 检查 ⇒ 显式 pathspec 提交、push、观察**同一 commit** 的 GitHub 构建。
7. **下载该 commit 的 Artemis wasm / 私有 runtime artifact**（校验 provenance/ABI/闭包），用**独立宿主**对 pristine 游戏跑 **≥240000 ms 冷启动闸**，含真实输入、pause/resume、save/load 证据；区分 FS/IDB API 持久化与真实标题菜单存取；清理自有浏览器标签/服务器 job。

### 7.2 其余未完成

| # | 项 | 关键事实 |
|---|---|---|
| A | 拼接树冷启动两缺陷 | A：非 util 侧走到 `macro.iet`/`macro.iet.iet` 后停住，永不请求 `debug/lua/index.lua`/`trial.ini`；B：util 侧 `call_indirect` 越界（`wasm-function[666]:0xa63cd`） |
| B | 全手写冷跑黑屏 | 61 次请求后 `Uncaught TypeError: Cannot read properties of undefined (reading 'get_exception_info')` in `___cxa_end_catch`，根因未定 |
| C | f3633 装配余留 | 见 §4.3.2 末段（9 处 `I32_TRUNC_S_F32`、裸 `i0`、staging 缺 part19/20/35、`pending_throw_path`、102 列行） |
| D | 手改净化未完成 | 311 文件（曾记 315/313），**32 个仍带生成标签**；旧估算剩 ~132（c-1 15·c-2 2·c-3 12·c-4 18·c-5 18·c-6 17·d-2 8·d-3 8·d-4 12·d-5 11·d-6 11；a-1/a-2/b-1/b-2/b-3/d-1/e 已 0） |
| E | 规则/文件待裁决 | `script/lua_keywords.cpp` 保留 65 闸/360 行忠实版（105 行/5 闸塌缩版弃用，备份 `.test/other/lua_keywords.cpp`）；`script/patch_upvalue_jump_target.cpp` 必须与原调用多重集完全一致（现 **166 vs 165**，多一处 `LoadU32(+24)` 与一个尾部 `return;`） |
| F | 子代理产出待审计 | `util/`（`f526c7d3`，22 文件，18,877→7,739 行）：5 处手工恢复的 `} else {` 合并点取值、`format_traceback_frame.cpp` 两处被改的错误恢复 select（~:262、~:352）、`compute_numeric_power.cpp` 重加的 `saved_bits = ReinterpretF64((base + result));`（:285）、`reclaim_free_block.cpp` 删 2 个死 load（125→123）+ 4 个重复 `I32_ROTL(kFreeUseFlagMask, X);` |
| G | 质量门卫生 | `git diff --cached --check`/`git diff --check` 均 exit 2；空白债约 300/2,344 行（例 `engine_mode.cpp:35`、`input/auto_read.cpp:70`）；四次 `inplace.py` 命中（三个 `util/unordered_*`，判无害）待审计 |
| H | m24051 官方 wasm 加载真游戏 | 官方 sync wasm + 补丁胶水 + shim + lazy packs 在 `.test/ab/real-lazy/` 只到 `Creating mount point /realrun`→`Mounting Indexed DB`→`Loading file system …`，canvas 750×485，**未出 `Loading file system complete!`**；同树第二次运行**打印了 complete!** ⇒ 间歇/挂载相关；教训：一个场景要跑出两次一致结果再下结论 |
| I | m23622 三场景全失败 | 场景 = 官方模块+官方胶水／重写模块+官方胶水／官方模块+重写宿主；A/B v1 已知错误：`.test/ab` 端口 4770、`--asyncify-url` 被写成文件 URL `http://127.0.0.1:4770/game/root.pfs`、落地页仍是旧 `/run-a/web/artemis.html` 布局 ⇒ 需重建（同源 pack、正确 `--asyncify-url`、匹配 `<out>/web/` 的落地页） |
| J | Artemis 发布 | `ARTEMIS_PUBLISH_ARTIFACTS` 未置位；`rg2.mjs` 缺 `aborted` 提前退出补丁（建议合进 `real-game-test.mjs` 后删） |
| K | 陈旧 release asset | `online-gal-runtime-ons-0.1.1.tgz`（id `595083575`）待删 |
| L | XP3 reader | `packages/vfs/src/archives/xp3.ts` 5 个永久 `it.skip`（分块索引 reader 未实现、`0x80` 重定向与商业档案未打通）；`.test/xp3-spec.ts`/`.test/xp3-redirect.ts` 是否提升进 `packages/vfs/` 待裁决 |
| M | krkr ranged read 零请求 | 拦截与 `krkr_try_ranged_stream` 可编译但从不执行；待参考 `kirikiroid2-web` 存储层与线程模型 |
| N | ONS 未做 | 存档持久化（IDBFS 导入导出，`-lidbfs.js` 已在 flags）；音频队列消费反馈（`queued` 只增不减，读 0.82 s vs 目标 0.25 s）；滚轮/触摸输入 |
| O | Artemis 旧口径未做 | 音频 OpenSL ES→Web Audio（缝是**符号级**，用 `dlsym`）；视频（`-DARTC_ENABLE_FFMPEG=OFF` 只编 no-op stub，正解是宿主 `<video>` 覆盖层，镜像 Android `VideoViewActivity.kt`）；`artemis` provider 仍 `unavailable`(85) |
| P | 调试残留待清 | `[artc-*]` 诊断、`__artcSelfCheck`、worker `canvas:` log、`.test/test-games/Artemis/` 内部署副本、`ports/artemis/reverse/.build-dbg.sh`（-g -O0，~4 min）、`ports/artemis/tools/rg-full-console.mjs`、demo 探针、`.test/{mk-dbg-tree.py,patch-dbg2.py,patch-dbg3.py,elsescan.py,litscan.py}`、`pf8_reader.cpp` 的临时 `[artc-diag]` 仪表（定位完成后应移除） |
| Q | 原生测试受阻 | `-DARTC_BUILD_TESTS=ON`→`CMAKE_C_COMPILER not set`；MinGW64→`Could NOT find ZLIB`；`emcc`→`undefined symbol: std::__2::cerr`；`em++`→`undefined symbol: crc32` ⇒ `tests/save_regressions.cpp` 的 2b) 删除用例**尚未真跑**；`wasm-sections.mjs` 报 `name section: ABSENT`/`DWARF: absent`；wasm 内 grep `TagSkip|TagFile` 得 **0** |
| R | 许可未决 | 根 GPL 与依赖许可不能为生成余留建立再分发授权；官方 SDK `Copyright.txt` 专有不可入 GPL 仓库 |
| S | 宿主代码落位 | `.ref/`/`.test/` 都 gitignore；`packages/runtime-artemis/src/host/*.ts` 已落 `packages/` |
| T | `[WASM]` 段缺键 | 真实游戏 `system.ini` 缺 `SAVEPATH`/`ASYNCIFY_URL` 是否阻塞官方 wasm 路线**未决** |
| U | 未提交产物（历史记录） | `packages/runtime-artemis/src/host/{gl,boot,fd,layout,asyncify}.ts` 及其 `README.md`、`ports/artemis/harness/glue/imports-map.{json,md,mjs}`、`tag-oracle.mjs`、`artemis-web-port.md` §13、重生成的 `0001-web-host.patch`（曾 86,124 B/9 个 `diff --git`，后 98,305 B/14 个/0 个 CR 字节）——交接时工作树干净（`git status --porcelain` 无输出），故这些内容此刻要么已随提交入库、要么已不存在；**是否逐项都在 HEAD 里未复核（未验证）** |
| V | 文档漂移（已复核） | `ports/artemis/README.md:26-30` 仍写「四个新产品尚未进入 Git 索引」（`compositor_effect.cpp`、`compositor_hide.cpp`、`compositor_lydel.cpp`、`lua_tags_builtin.cpp`）；交接时 `git ls-files` 显示四者**都已被跟踪**、`git check-ignore` 不命中 ⇒ 该段落已过期，应随下一次文档返工一起修 |

## 8. 在新对话里如何继续

### 8.1 环境

- 工作目录 `D:\online_gal`；Git Bash（`pwsh` 被接到 bash ⇒ 只用 bash 语法）。
- emsdk **6.0.9** @ `D:\emsdk`（仅 Git Bash）；CMake **4.3.1**；meson **1.12.0**+ninja；vcpkg（必须全量 clone）。
- 上游 pin：ons `08f744b31cc1907b66a15f0402e62321a131ed81`；krkr `13dda190f8370d02b6cf59286a088529b355658c`；artemis-compat `9f622629f595db85e6f76c45c06edde29d7d9961`（原 `a4f8a257cf87cb8f65ab4e3f83501ab327a55424`）；vcpkg `b1e15efe…`；wabt `02418f72…`。
- git 代理 `socks5h://127.0.0.1:10808`；`gh` 在 `/d/programe/github cil/gh`；token `/tmp/.tok`。
- 写文件只用 `write`/`edit`（Windows 路径）或带 `encoding='utf-8'` 的 python；**禁止 PowerShell 写文本**。

### 8.2 构建

```bash
pnpm run build:packages                     # = tsc -b tsconfig.packages.json
bash ports/artemis/reverse/build-bridge-route.sh asyncify [outdir]
#   :214-218 重建 artemis.cpp；:226 patch-native-traps.py；:231 rw-splice.py
bash -lc "source ports/env.sh && ARTEMIS_REWRITE=D:/tmp/bstate …"
ptmask.py                                   # 运行期掩码（未打补丁时全位 1）
/d/tmp/bisect.py                            # 模式 one | list | bisect
gbuild.sh / grun.sh                         # grouping gate（权威判定）
```

- Artemis Web 宿主三阶段：`.test/artemis-web-build/build-emscripten.sh`（core/cli/cli-node，含 `NODERAWFS` 与内存增长标志）；最小合成包 `make-min-pack.py`（257 字节，只含 `system.ini`）。
- 官方对照部署：`official-web-deploy.py --game <loose 树> --port 4700`（`--game` 必须 loose 树）。
- 消费侧：`ports/lib/fetch-engine.mjs` + `engine-artifacts.json`、`fetch-web-engine.sh`、`web-deploy.sh`、`gen-wasm2c.sh`；CI 用 `.github/scripts/build-artemis-bridge.sh` + `make-package.mjs`。

### 8.3 测试与验收命令

```bash
pnpm test                                   # 基线 439 tests/85 suites/434 pass/5 skipped（#21 起）
                                            # 更早：416/79/411 pass/0 fail/5 skipped
node --test test/artemis-boot.test.mjs
pnpm run test:portable                      # scripts/run-portable-tests.mjs（313/313）
node --test .test/artemis-pfs.test.ts       # PFS 回归（断言范围读的流量形状）
node --test .test/artemis-eh-independent.test.mjs
node scripts/check-layout.mjs               # packages/|apps/|ports/ 引用 .test/ 即 exit 1
node .test/artemis-web-build/real-game-test.mjs --game .test/artemis-official-async/game \
  --engine ports/artemis/reverse/out/bridge/build/bridge.wasm --flavour asyncify --wait 180000 --json
node .test/artemis-web-build/rg2.mjs --game … --engine … --flavour asyncify --port 4710 \
  --cdp-port 9222 --wait 300000 --no-control --hashlist … --json --json-out … --savepath … [--keep]
node .test/artemis-web-build/serve-range.mjs                 # 206 + 请求日志
node .test/artemis-web-build/verify-pfs-real-archive.mjs      # 生产函数解真实归档加密条目
node .test/artemis-web-build/verify-probe-real-game.mjs       # 生产 probe 跑真实 5.8 GB 目录
node .test/krkr2-web-build/browser-test/kiri-shot.mjs --seconds 75   # @ ?xp3=/data.xp3
node .test/krkr2-web-build/browser-test/mcp-real-xp3.mjs             # chrome-devtools-mcp 驱动真实 XP3
style-gate.sh ; node wasm-diff.mjs ; node wasm-replace-test.mjs
```

`rg2.mjs` flags：`--game --engine --flavour --port --cdp-port --wait --no-control --hashlist --json --json-out --savepath --keep`；默认 `port:4710, cdpPort:9222, wait:30000`。m29147 后测试**不需要再跑官方版**（`--no-control`）。官方 sync 证据：`.test/ab/shots/sync-lazy.png`（启动零报错、画面出图，max 248/lit 1.5%）。

### 8.4 关键文档索引

| 文档 | 内容 |
|---|---|
| `README.md` | 架构说明、支持范围、构建与测试方式 |
| `docs/项目状态与恢复指南.md` | 真实 XP3 验证、pkgconf 端口补丁为死代码、测试基线算术、MCP 复测一节 + 四个坑 + 单实例约束 |
| `docs/artemis-web-port.md` | Artemis 移植旗舰叙事：构建三阶段、两个必需 Emscripten 标志、宿主与输入验证、JSPI 接缝、HTTP Range、§9.10–9.22（定位结果、根因、`-fno-exceptions` abort、pre-init 输入 bug、off-pair 动画位置、dead depth-tiebreak 与已回退的排序尝试） |
| `docs/artemis-pfs-format.md` | PFS（pf8）规格：GARbro + artemis-compat + 真实 2.69 GB 归档三方对齐；步长陷阱、密钥派生、编码回退、反斜杠路径、未解的尾部表、两个缺陷记录 |
| `docs/fetch-engine-artifacts.md` | 历史 release hash（**不得改写**，尤其 `:282-283,370,373`） |
| `ports/artemis/REWRITE.md` | 重写宪法（~386 行）：§0 规模真相（重写只覆盖 tsv 的 fn 行，其余 ~5,300 生成函数隔离在 `generated/`）、§2 八条风格、§7.2 单 TU、必须不变与相位门规范 |
| `ports/artemis/reverse/*.md` | bridge 工具集（`build-bridge-route.sh`、`rw-splice.py`、`wasm-rename-{export,import}.py`、`wasm-satisfy-imports.py`、`instrument-oob.py`、`artc-trap-probe.c`） |
| `ports/*/README.md`、`packages/runtime-artemis/src/host/README.md` | 各移植目录与现代宿主分桶（fd/gl/boot/layout/asyncify/eh/audio/browser-services） |
| `packages/engine-api/src/{types,registry}.ts` | 运行时提供者契约与调度器 |
| `packages/runtime-kag/src/{runtime,scene}.ts`、`src/tjs/interpreter.ts` | KAG3 提供者、场景执行器（指令流/条件分支/图层/过渡/存档）、TJS2 树遍历解释器 |
| `packages/core/src/library.ts` | 游戏库、导入、探测与数据源组装 |

### 8.5 约定

- 模拟代码放仓库根对应位置；可提交测试放 `test/`；永不提交 `.ref/**`、`.test/**`、`.work_f3785/`、`rewrite-staging/`（用 `git status --porcelain` + `scripts/check-layout.mjs` 检查）。
- 提交用显式 pathspec（`git commit -F -` 不带 pathspec 会扫进他人 staged 删除）；提交前升级 `runtime-artemis`/`engines` 版本；`git diff --check` 目前 exit 2，**不能当绿色门**。
- 提交后观察**同一 commit** 的 GitHub 构建，下载该 commit 的 artifact 做真实游戏验收（m20064）。
- 用完即清：浏览器标签、`python -m http.server`、Range 服务器、诊断窗口。

### 8.6 验收标准（什么算真证据）

- **唯一可靠判据 = 真实游戏测试**，且**单次运行不是判据**：需「丢弃一次热身 + 两次一致运行」。
- 相对判定（两侧像素相同 + 请求数 + `lit%` + 帧数），**绝不用字面 hash**。
- 必须用**下载的 CI 产物**（m20064），校验 provenance/ABI/闭包；本地构建只作对照。
- 冷启动 `--wait ≥ 240000`（300000 才稳定 PASS×2）；机器须空闲。
- **不算证据**：全生成掩码桥健康、官方 glue 健康、ABI/结构/合成测试、组件单测全绿、白帧、worker 回执、`_main THREW String: unwind`。
- 现代宿主验收须在**无官方胶水**前提下、对 pristine 游戏跑 ≥240000 ms 冷启动闸，含真实输入、pause/resume、save/load。

## 9. 证据索引

### 9.1 关键文件

| 路径 | 标识 |
|---|---|
| `.ref/artemis-official/wasm/asyncify/artemis.wasm2c.c` | 官方 asyncify 转写真值（107,593,501 B；另一处记 106,799,631 B，**（未验证）**） |
| `.ref/artemis-official/wasm/asyncify/artemis.wasm2c.h:31-39` | `w2c_artemis` 结构与 g0–g4 |
| `artemis.wasm.pseudo.c` | 42,325,228 B 伪 C，覆盖 5,609 函数 |
| `artemis.wasm.headers.h`（648,162 B）、`artemis.wasm.functions.tsv`、`artemis.wasm2c.c:103573` | 头/函数表/elem 段 `w2c_artemis_e0`（4,277 条；槽 N 在第 `103573+N` 行） |
| `ports/artemis/src/util/engine_context.h` | `:17/:18/:25/:43/:47-48/:52-56/:63/:336/:593/:602-686/:639/:643` |
| `ports/artemis/reverse/{build-bridge-route.sh,rw-splice.py:159,pt-map.tsv}`、`ports/artemis/rewrite-names.tsv` | 拼接与掩码映射（`build-bridge-route.sh` 的 `:214-218`、`:226`、`:231`、`:279-320`） |
| `ports/artemis/REWRITE.md` | §0/§2/§7.2 |
| `packages/runtime-artemis/src/{runtime,index,pfs,probe}.ts`、`src/host/{eh,boot,gl,fd,layout,asyncify,audio,browser-services,assets}.ts` | 现代宿主 |
| `artemis-export-shims.c:115-119` | `u32 artc_export_Zc(u32 p0)` |
| `.github/scripts/{build-artemis-bridge.sh,make-package.mjs}`、`scripts/run-portable-tests.mjs` | 桥构建与打包 |
| `.test/artemis-web-build/{build-emscripten.sh,real-game-test.mjs,rg2.mjs,serve-range.mjs,parse-pf8-index.py,verify-pfs-real-archive.mjs,verify-probe-real-game.mjs,make-min-pack.py}` | 验证脚本 |
| `.test/artemis-eh-independent.test.mjs`（134 行）/ `test/artemis-eh-abi.test.mjs`（~380 行，被拒） | EH fixture |
| `.test/artemis-honoured-deferred-independent.log`（`pwsh-6704`）、`test/artemis-boot.test.mjs:121` | honoured 证据 |
| `.ref/artemis-compat`（pin `9f622629f595…`） | `src/script/lua_engine.cpp`（修所有权 bug）、`src/pack/pf8_reader.cpp`、`src/host/{web_main.cpp,web_shell.html}`、`CMakeLists.txt` |
| `.ref/artemis/` | `artemis.wasm` 2,682,787 B；asyncify 5,998,665 B；`libartemis.so` 8,149,024 B |
| `.test/artemis-official-async/{game,game-chs}` | 真实游戏树（29,224 文件/5.7 GB）；`system.ini` 3,618 B / sha256 `de3e548de1b5115d815a2ea4ce421f3076e6e1393f22039bead2b0bdbadf20f7` |
| `.test/artemis-web-build/{artemis-input-before,artemis-input-after,artemis-play}.png` | 标题屏 / 点 START 后的 `まどそふと` bumper / 真实 2561x1398 canvas（442 s、frames 70477、last error (none)） |
| `.test/ab/shots/sync-lazy.png` | 官方 sync wasm + 官方胶水 lazy pack（启动零报错、max 248/lit 1.5%） |
| `.test/krkr2-web-build/{run-build.sh,browser-test/kiri-shot.mjs,browser-test/mcp-real-xp3.mjs}` | krkr 构建与真实浏览器验证 |

### 9.2 CI run / artifact 与桥字节

| 项 | 值 |
|---|---|
| run #36（`bfb6053`） | 全绿；artifact `wasm-artemis-bridge` id **11087582732**（24,821,871 B）+ package（445,483 B） |
| run `36888247775` | artifact id `11175995805`、package id `11176590006` |
| run `36923019535`（`ae71ceb`） | artifact `11192208716`（24,735,079 B）/ package `11192503462`（445,483 B），解包 `/d/tmp/ci-art2/` |
| run `37131764522` / package run `37131764249`（`db70bd0`） | 同 SHA 成功；真实游戏 job 跳过 |
| artemis host CI 复现 | 两次 run 的 `.wasm` 相同：**1,151,025**/`a1787043…` |
| 官方模块 | asyncify 5,998,665 B/`7ceabed28b84e768…`；plain 2,682,787 B/`f05ae1cb25eaaff8…` |
| `br-glive4`（327，PASS×2 warm） | 40,773,887 B/`8a1631ff03441257` |
| `br-new2`（336，PASS×2 warm） | 40,686,361 B/`34be68a86548beb9` |
| `br-ci2`（与 br-new2 function-for-function 相同） | 40,686,393 B/`d44b8c685bb45a80` |
| 掩码桥 `bridge-cold-mask-fixed-o1/bridge.wasm` | 29,341,741 B/sha256 `8865f128…82cf7` |
| ABI 桥 `bridge-f3633-o1-final/bridge.wasm` | 23,683,379 B/sha256 `71dcf8c0…3759` |
| 未拼接基线 / batch18（327，`4b2429c`；CI 41,322,022） | 41,864,950 / 41,321,914 |
| batch6(171)/batch12(295)/e4/e5/e6 | 41,555,727 / 41,351,533 / 41,216,702 / 41,188,595 / 41,179,322 |
| CI `bridge.wasm`（早期口径；本地 fresh 41,757,390，差 124 B） | 41,757,498/`2b30f5c54072…` |
| 16 MiB RESERVE 实验（失败，已回退 4 MiB） | 40,773,979/`5cd2235dfffcc2fb` |
| 掩码哨兵 44 B @ `29341378`/`0x1bfb6c2`；`br-pt1.wasm` | — / 45,419,307 |
| live 树（311 modified，327 products，5/5 FAIL）/ BASE（全 HEAD，2/2 PASS） | 40,773,758 / 41,296,963 |
| `LA1` files[0:155] FAIL / `LA2` files[0:77] PASS | 41,020,136 / 41,147,764（LA2：`g=75 hash=fd00de90 raf≈1900 console=28 exc=0`） |
| `onsyuri.wasm` / `wasm-rt-impl.o` | 4,658,702 / 本地 4,524 vs CI 4,634（差 2×37 B = 两个 `abort()` 守卫） |
| CI gate / 陈旧 asset | wasm-diff 67/67、wasm-replace-test 4/4；`online-gal-runtime-ons-0.1.1.tgz` id `595083575` |

### 9.3 帧锚与健康指纹

| 场景 | 指标 |
|---|---|
| Artemis bridge 两侧对照 | `finalHash 5a524c8a`、canvas 1509x730、lit 85.95、87 次游戏请求、`pixelIdentical true`；`gameBytes` 两侧差三个数量级（17,867,773 vs 33,460）**未解释** |
| 门数字 | canvas 1510x731 / lit **86.09%** / 3 frames / 87 game files / settled **18322660** / 逐像素相同 |
| 另一健康签名 | canvas 2268x1098（或 2269x1102）/ mean 218.11~220.01 / `finalHash da43db44` / rafTicks 2026 |
| local fresh bridge A/B | canvas 756×488、lit 87.3%、91 游戏文件、0 trap；`gl-oracle.mjs` draws fresh **8,529** vs CI **8,506** vs 官方 yardstick **14,305** |
| krkr 真实游戏 | canvas 756x488、206×262,144 B、`lit>24=87.09%`、`bright>200=50.17%`、mean `r174.67 g161.10 b156.41`、signature `f007553e` |
| 失败签名（可能同出一树，堆布局相关） | `g=5 lit=0 hash=7247ae64` = 取 5~6 个文件后静默停住；`g=0 hash=0 raf≈1812` + `[error] PANIC: unprotected error in call to Lua API ( value)`（该串在树里 grep 不到 ⇒ 内嵌 Lua 库 panic） |
| 冷启动/内存/V8 | 串 ~18 MB、>90 s 出第 1 帧、`--wait ≥ 240000`；`INITIAL_MEMORY`=201,326,592（192 MiB）；`LAYOUT_BASE`=184,549,376（176 MiB）；`a.a` 3072 pages；V8 locals 上限 **50,000**（-O0 时 f1509 有 95,112；-O1 降到 135） |
| 目标 | `goal-83337ba3-1cc5-418a-b7f6-1a2570098709` revision 7、paused/disarmed、rounds 8/40 |
