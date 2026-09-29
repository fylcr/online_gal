# Artemis 浏览器移植：进展与验证

> **当前状态（路线已改向，以下正文按历史保留）**
>
> **现在只剩一条 Artemis 路线：我们自己的重写。** 本文的主角是 **artemis-compat
> （上游，GPL-3.0）的 wasm 重写**，源码在 `ports/artemis/src/`，**目前唯一的出货形态是桥模块**
> （`.github/workflows/build-wasm-artemis-bridge.yml`）。**还没有现代宿主**——
> `packages/runtime-artemis` 因此报 `unavailable`，模拟器会明确拒绝而不是半跑起来。
>
> **已退役（本文档记录了它，但不要再照着做）**：早期那条「给上游 `artemis-compat` 源码
> 补一层浏览器宿主、编出 `artemis-web-android.wasm`」的路线。随它一起删除的有
> `.github/scripts/build-artemis.sh`、`.github/workflows/build-wasm-artemis.yml`、
> `ports/artemis/build/web-deploy.sh` 和 `ports/artemis/engine/0001-web-host.patch`。
> 所以本文里凡是提到 `web-deploy.sh`、`artemis-web-android` 或那份 patch 的段落，
> **都是历史记录，不是现行的部署指令**——§9（Web 宿主）、§12.2 起的「部署脚本」等
> 段落请这样读。宿主胶水本身（`ports/artemis/harness/android-host.html` 与
> `ports/artemis/build/artemis-web-android-worker.js`）和 `build-emscripten.sh` 的
> `web-android` stage 也已删除；`ports/artemis/build/android-shim/` 里的 C++ 保留下来，
> 只作参考。
>
> **仍然保留、但不是出货路线的两样东西**：`ports/artemis/build/official-web-deploy.py`
> 与驱动它的 `ports/artemis/tools/real-game-test.mjs`。真实游戏 bench 的对照树**故意**是
> 「官方模块 + 官方胶水」——它是**判据**（尺子），用来衡量我们的重写，不是一条我们自己的
> 发布通道。
>
> 仍在推进的是**宿主的头一刀**（由另一条工作线负责），那之后本节要重写。

基座是 [Weiss-UltimateSavior/artemis-compat](https://github.com/Weiss-UltimateSavior/artemis-compat)
（clean-room Artemis 兼容引擎，C++17，96 个文件约 30K 行）。目标是把这个引擎搬进浏览器。

样本是真实游戏 `.ref/artemis-game/`，**5.8 GB**：

| 文件 | 大小 | 文件数 |
| --- | --- | --- |
| `root.pfs` | 2,690,454,559 | 21,587 |
| `root.pfs.000` | 3,355,688,890 | 7,637 |
| `chs_patch_official.pfs` | 53,616,656 | 447 |
| `chs_patch_baijingmu.pfs` | 21,999,465 | 445 |

## 一、基座的可移植性来自三个既有接缝

| 接缝 | 含义 |
| --- | --- |
| `artemis_core` **不带 GL 即可编译** | CLI 与回归测试链接的就是它；只有 `ARTC_HAS_GLES=1` 才拉进 GLES2 合成器 |
| `artemis-mac` 是宿主模板 | 「重编 `${ARTC_CORE_SOURCES}` + 换宿主 main + 开 GLES」 |
| `Pf8Reader::ReadAt(offset, buf, len)` | 单一字节读原语，按需加载的落点 |

要替换的平台面只有 11 个文件（其中 3 个是 `src/jni/*`，Web 不需要）：`__android_log`（trivial）、
EGL/GLES2（Emscripten 提供）、**OpenSL ES（必须重写为 Web Audio）**。无 pthread、无 mmap。

## 二、修掉一个让参考仓库编不过的缺陷

**该 commit 在任何平台都编不过。** 头文件与 `EngineContext` 已重构成「audio/sounds 由 EngineContext
持有、`LuaEngine` 非拥有」，但 `lua_engine.cpp` 还停在旧的「自己 `new`」模型：

```
error: out-of-line definition of 'Init' does not match any declaration in 'artc::LuaEngine'
```

按头文件明写的契约（`// non-owning (EngineContext owns)`）一次修齐三处：

| 位置 | 原状 | 修正 |
| --- | --- | --- |
| `Init` 定义 | 6 个参数，自己 `new Audio()` | 8 个（与头文件和调用点一致），接收非拥有指针 |
| `~LuaEngine` | `delete sounds_; delete audio_;` | 不删（非拥有；删了即 double free） |
| 存档恢复路径 | `delete sounds_; sounds_=new AudioChannels(...)` | `sounds_->Reset()`（现成 API，同一意图） |

## 三、Emscripten 构建

`ports/artemis/build/build-emscripten.sh`，三个阶段：

| 阶段 | 产物 | 说明 |
| --- | --- | --- |
| `core` | `libartemis_core.a` **1,734,048 B** | 无 GL，证明 30K 行能编 |
| `cli` | `artc.wasm` **961,896 B** | CLI（`list`/`extract`/`drive`…） |
| `cli-node` | 同上 + `NODERAWFS` | 让 wasm 能读**宿主**文件 |

**为什么走 Web 构建而非先原生构建**：原生需要 `find_package(ZLIB REQUIRED)`，本机 MinGW 没有 zlib；
而 **Emscripten 自带 zlib 端口，sysroot 里 `zlib.h`/`libz.a` 现成**（早先预热 libpng 时建的）。
所以 Web 构建同时是目标也是阻力最小的首编路径。

### 必须设的两个 Emscripten 标志

**`-sNODERAWFS=1`** —— 否则模块只看得见 MEMFS，`artc list root.pfs` 会报文件不存在，
**看起来像读取器坏了，其实是文件系统没挂上**。`--preload-file` 不可行：样本包本身就 2.69 GB。

**`-sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=64MB -sMAXIMUM_MEMORY=4GB`** —— Emscripten 6.x 的
`ALLOW_MEMORY_GROWTH` **默认关闭**，堆就是 `INITIAL_MEMORY`（默认 16 MB），第一次像样的分配就中止：

```
Aborted(OOM)
  at abortOnCannotGrowMemory (artc.cjs)
  at _emscripten_resize_heap
```

这正是第一次启动真实游戏时的死法：日志一路走到
`lyc {id=1.0.-1, file=:bg/black}`（引擎建第一个背景图层，也就是要加载真实资源）然后崩。
**加上增长后同一命令 exit=0。Web 宿主同样需要这几个标志。**

## 四、验证结果

### 4.1 包层：wasm 读真实 2.69 GB 归档

```
$ node artc.cjs list root.pfs
font\extra_28.rft                        off=0x001217f6 size=512006
pc\ja\artemis.png                        off=0x011d9d0f size=180572
pc\ja\blog\name\あすみ.png               off=0x01213ccf size=4667
pc\ja\blog\name\おじいちゃん.png         off=0x01214f0a size=7471
```

日文名按 Shift_JIS 正确解码，偏移 `0x001217f6` = 1,185,782 —— 与独立解析结果一致（数据紧跟索引）。

### 4.2 引擎启动：真实游戏跑进 ADV 主循环

```
$ node artc.cjs drive root.pfs --os windows --frames 60
engine: pack=root.pfs os=windows stage=1280x720      ← system.ini 解析
iet label: top                                        ← .iet 解释器
iet calllua: system_initlua → system_loadinglua → system_dataloading → system_initialize
tag[trace]: skip / automode / autosave / backlog / hide / rclick / delonpush{key=1..24+}
lyprop {id=1.80...}
jump {label=main, file=system/script.asb}            ← 跳进编译后的脚本
asb: load system/script.asb label=main               ← .asb runner
lydel {id=startmask}                                 ← 摘掉启动遮罩
lyc {id=10, file=:bg/white}                          ← 建白背景
call {label=estag03, file=system/script.asb}         ← 调用场景标签
trans {input=1, type=1, time=1000}                   ← 1 秒过渡
drive: click-wait engaged @ frame 4                  ← 正在等点击
exit=0
```

**即：包层 → system.ini → .iet 解释器 → Lua 5.1.5 → adv 框架 → .asb 脚本 → 场景标签 → 等输入**，
整条链路在 WebAssembly 上跑通。

### 4.3 输入：点击推动剧情并加载新资源

```
$ node artc.cjs drive root.pfs --frames 120 --tap 640,360@6 --tap 640,360@40
click: fallback -> onpush key 1
drive: click-wait released @ frame 7
click: hit='10' registered=no                        ← 命中测试命中图层 10
drive: click-wait released @ frame 41
tag[trace]: call {label=estag05, file=system/script.asb}   ← estag03 → estag05
tag[trace]: lyc {id=10, file=pc/ja/caution}          ← 从包里加载新资源
drive: end frames=120 click-waits engaged=3          ← 翻过 3 页文本
exit=0   错误数=0
```

**输入驱动推进 + 运行时资源加载都成立。**

## 五、VFS 侧：按需读取（独立于 wasm 的一条线）

`packages/runtime-artemis/src/pfs.ts` 新增范围读 API，并接进 probe：

```ts
readPfsTableFrom(source: PfsByteSource): Promise<PfsArchive | null>
readPfsEntryFrom(source, archive, entry): Promise<Uint8Array | null>
```

`PfsByteSource` 是结构化声明的 `{ size, readRange }`，与 VFS 的 `ByteSource` 形状一致，
`gameSourceByteSource` 直接适配；包本身不必依赖 VFS 实例。

**修掉的缺口**：`probe.ts` 原先带 **512 MB 上限整文件读包**，所以 2.69 GB 的 `root.pfs` 被整个跳过，
探针把封装型游戏报成「包不可读」——**看起来像包损坏，实际只是太大**。

实测（生产 probe，真实游戏目录）：

```
packageEntries:    21587 entries 全部解出
systemIni from:    root.pfs!system.ini（2422 字符）
bytes read:        1,189,400 / 2,690,454,559 = 0.044%
reads: 3
  off=0                len=7            ← 头
  off=7                len=1,185,775    ← 索引
  off=2,690,450,941    len=3,618        ← system.ini，位于 2.69 GB 文件的最后
```

**索引必须整体读**：密钥是 `SHA-1(整个 index)`，尾部那 172,712 字节虽从不被解析却要参与哈希，
跳过会得到不同摘要、所有条目解成噪声。所以下限是 `11 + index_size`，不是条目表的大小。

## 六、按 GARbro 核对时修掉的两个读取器缺陷

详见 `docs/artemis-pfs-format.md`。摘要：

1. **解密判定用了「像不像文本」的启发式，且在真实归档上判反。** `looksLikeText` 把
   ≥ 0x20 且 ≠ 0x7F 的字节都算可打印（**含整个 0x80–0xFF**），密文恰好密集在那里：
   `pc\ja\artemis.png` 密文可打印占比 0.898（过阈值），解出的 PNG 0.598（不过）——
   于是**每个 PNG/OGG/字体都被当成密文返回**。改为由容器版本决定（GARbro：仅 4/5/8/9 加密）。
2. **`& 0x7fffffff` 掩掉高位，使 ≥ 2 GiB 的偏移静默出错。** `root.pfs` 有 3,940 条、
   `root.pfs.000` 有 6,582 条落在这个区间，**一款游戏约 35% 的条目被静默算错**且不抛错。
   改为读普通 u32。

两个缺陷都补了**能被旧代码证伪**的回归测试（临时还原后 3 例确实失败）。

## 七、尚未完成

> **本节已按当前状态重列。** 原先这里列的五条里有三条已经做完了，而那三条当时读起来
> 都像「还没开始」——所以先记下它们是怎么落地的，再列剩下的：
>
> - 原 1「**Web 宿主**」（`.ref/artemis-compat/src/host/web_main.cpp`）
>   ——**已完成**，`requestAnimationFrame` 驱动、输入来自浏览器事件，真实游戏在真实 Chrome 里
>   渲染并响应点击，见 **9.17–9.18**。
>   （顺带纠一处旧文：帧循环参照的 `host_drive.cpp` **不在 `src/host/` 下，而在
>   `.ref/artemis-compat/src/cli/host_drive.cpp`**；`src/host/` 只有
>   `mac_main.mm` / `web_main.cpp` / `web_page.html` / `web_worker.js`。本节的裸 `src/`
>   一律指 `.ref/artemis-compat/src/`。）
> - 原 2「**渲染**」`ARTC_HAS_GLES=1` + Emscripten GLES2/EGL → WebGL2 canvas——**已完成**；
>   当时的担心（合成器 FBO/过渡路径需实测）确实兑现成了一个真 bug，但不是 GL 模拟的问题，
>   而是 `emscripten_webgl_create_context(0, …)` 传了槽 0，见 **9.17**。
> - 原 4「**归档供数**」——**已完成**，落点不是这里设想的「wasm 侧接 VFS 范围读」，而是
>   `Pf8Reader::ReadAt` 走 worker 里的**同步 XHR**（9.15–9.16）；实测从冷启动到片头只读了
>   包链的 **0.41%**（9.18）。
>
> 按依赖顺序，仍然没做的是：

1. **音频**：`.ref/artemis-compat/src/audio/audio.{cpp,h}`、`pcm_ring.h` 里的
   **OpenSL ES 必须重写为 Web Audio**（`audio.h:2` 自述「Android: OpenSL ES streaming player」）。
   这是唯一一处「必须新写后端」而非「换编译器」的地方。
2. **Provider**：`artemis` 在 `BUILTIN_PROVIDERS` 里仍是 `unavailable`（priority 85）——
   它的判定读 `artemisEngineHosts`，而本仓库里那个数组仍是空的
   （`packages/runtime-artemis/src/runtime.ts:80`）。

> 前面划掉的那三条**为什么做得出来**，分别记在 **§十七**（重写后的引擎已入库）与
> **§十八**（官方 SDK 直部署 + 替换台）：前者给出可读可改的源码树，后者给出
> 「这套证据可重跑、有退出码」的验收方式。剩下这两条恰好都属于**四道闸测不出来**的
> 那类工作——它们不改变导入导出面，也不改变稳定帧。

## 八、复现命令

```bash
# 构建（三选一）
bash ports/artemis/build/build-emscripten.sh core
bash ports/artemis/build/build-emscripten.sh cli
bash ports/artemis/build/build-emscripten.sh cli-node

# 运行（NODERAWFS 才能读宿主路径；ESM/CommonJS 需换目录，见下）
mkdir -p /tmp/artc-node && cd /tmp/artc-node
cp <build>/artc.{js,wasm} . && mv artc.js artc.cjs && echo '{"type":"commonjs"}' > package.json
cd .ref/artemis-game
node /tmp/artc-node/artc.cjs list  root.pfs
node /tmp/artc-node/artc.cjs drive root.pfs --frames 120 --tap 640,360@6

# VFS 侧验证（不需要 wasm）
node ports/artemis/build/verify-probe-real-game.mjs
node ports/artemis/build/verify-pfs-real-archive.mjs
```

> **为什么要把 `artc.js` 改名并换目录**：本仓库 `.test/package.json` 是 `"type": "module"`，
> 而 Emscripten 输出是 CommonJS，直接 `node artc.js` 会在**第一行**报
> `ReferenceError: require is not defined in ES module scope` —— 那是模块系统的问题，
> 不是构建或引擎的问题。

## 九、Web 宿主（`src/host/web_main.cpp`）

> ⚠️ **已退役的路线，本节按历史保留。** 这一整节描述的是给上游 `artemis-compat` 补一个
> 浏览器宿主（`src/host/web_main.cpp`）并编出 `artemis-web-android.wasm` 的做法。
> **那份 patch（`ports/artemis/engine/0001-web-host.patch`）与这条线对应的
> `build-artemis.sh` / `build-wasm-artemis.yml` / `web-deploy.sh` 已全部删除**，
> 下面的 `src/host/web_main.cpp` 路径指的是 `.ref/` 里的上游参考源码，不是入库文件。
> 仍然有效的教训（帧序、Asyncify vs JSPI、按需供数、那些真机验证过的坑）都留在本节里，
> 但**重建这条路请看 §十七（我们的重写）和 §十八（官方 SDK + 替换台）**。

已写好并在**真实浏览器里验证通过**。照 `mac_main.mm` 的帧序写，刻意保持逐步一致：

```
reset/exit 门 → 启动 → drain 输入 → RunEnterFrame → drain 标签 → Step
              → compositor.Update → 重绘门控的 Present → EndFrame
```

平台面替换：Cocoa 窗口 + legacy OpenGL → canvas + WebGL2；`NSTimer` → `emscripten_set_main_loop(fn, 0, 1)`；
`NSEvent` → 浏览器事件 → 导出的 14 个 `artc_web_*` C 入口。

重绘门控保留且**在 Web 上更重要**：浏览器合成器为每一帧收费，而 ADV 场景大部分时间是静止的。

### 9.1 参照 Android 宿主（`src/jni/native_activity.cpp`，643 行）

Tyranor-Next 的 `artemis_loader.cpp`（163 行）只是**插件 bootstrap 加载器**——读 intent 里的
`engineLibName`、白名单校验、`dlopen` 插件目录里的真实 so、把 `ANativeActivity_onCreate` 原样转发。
它不是宿主驱动。**真正的 Android 宿主在库自己的 `native_activity.cpp`**，那里的架构与 Web 的对应关系很清楚：

| Android | Web |
| --- | --- |
| **一个引擎线程**跑启动链，**所有 EGL 调用都在该线程**（EGL 上下文与线程绑定） | rAF 单线程，GL 天然绑定 |
| 独立 looper 线程收输入，避免主线程 ANR | 浏览器事件入队，每帧 drain |
| `ShowNativeInput`：**阻塞式**对话框——投到 UI 线程后轮询直到关闭，期间暂停引擎 | `confirm`/`prompt` **本身同步阻塞**，直接等价映射 |

第 9 行还点明：`libartemis_audio_bridge.so` 用 **`dlsym`** 命中引擎符号（所以加载用 `RTLD_GLOBAL`）。
**音频接缝是符号级的，不是头文件级的**——这决定了 Web 音频后端要替换的位置。

### 9.2 在真实浏览器里验证到的

```
mounted /game/root.pfs (257 bytes)
opened /game/root.pfs (1280x720)
```

控制台：

```
[INFO]  web host: GL OpenGL ES 3.0 (WebGL 2.0 (OpenGL ES 3.0 Chromium))
```

即：模块加载 → **WebGL2 上下文创建成功** → 把包写进 Emscripten FS → **引擎自己的 `Pf8Reader`
解析 pf8 成功** → 报出舞台尺寸。用的是 `ports/artemis/build/make-min-pack.py` 造的 257 字节最小包
（只含 `system.ini`），所以期望的行为就是「`Open()` 成功、`BootFramework()` 因缺 `system/init.lua` 而失败」——
**把这区分清楚，才不会把「宿主与容器已证明」误当成「框架已证明」。**

### 9.3 Web 侧踩到的五个坑

1. **`--shell-file` 只在输出 HTML 时生效，否则被静默忽略**（无警告、无 HTML）。CMake 默认后缀是 `.js`，
   于是构建看着正常、页面却是**默认 shell**——没有 `#canvas`，上下文无从创建。
   修法：`set_target_properties(... SUFFIX ".html")`。
2. **改 `web_shell.html` 不会触发重新链接。** 它不是源文件，CMake 不跟踪；改了页面重编却毫无变化，
   服务出去的还是旧版，**修了像没修**。修法：`set_property(TARGET ... APPEND PROPERTY LINK_DEPENDS <shell>)`。
   这一条把我骗了一轮：`mount failed: undefined` 反复出现，其实是旧 HTML 里的 `error.message`。
3. **`Module._artc_web_open(PACK, OS)` 直接传 JS 字符串是错的**——导出函数收指针，字符串会被当数字，
   函数拿到空参数，日志报成 `engine: no .pfs pack at `（空路径），**看着像文件不存在，其实是封送错误**。
   修法：`Module.cwrap('artc_web_open', 'number', ['string','string'])`。
4. **`FS.writeFile` 写到嵌套路径前要 `FS.mkdirTree`。** 否则抛的是光秃秃的 `ErrnoError`，
   只有 `errno` 字段有用，消息里完全看不出是缺目录。
5. **必须 `--no-entry -sINVOKE_RUN=0`**：宿主没有 `main`，导出 `_main` 会链接失败
   （`undefined exported symbol: "_main"`）。

### 9.4 按需供数：JSPI 接缝（已实现并在浏览器验证）

**先做的是只读排查**，结论比预想的好：`pf8_reader` 的 I/O 收敛成**单一接缝**——

```cpp
Pf8Reader::ReadAt(uint64_t offset, void *buf, size_t len) const
    // POSIX: ::pread(fd_, p + done, len - done, offset + done)
```

**位置化、无状态**（源码注释写明「explicit offsets → safe for concurrent reads, no per-call fopen」）。
所以整个引擎的包数据读取只有一个函数要改。

改动：

```cpp
#if defined(__EMSCRIPTEN__)
EM_ASYNC_JS(int, artc_host_read_range, (const char* path, double offset, void* buf, int len), {
    const bytes = await window.__artcReadRange(UTF8ToString(path), offset, len);
    HEAPU8.set(bytes.subarray(0, n), buf);
    return n;
});
#endif

bool Pf8Reader::ReadAt(...) const {
#if defined(__EMSCRIPTEN__)
    return artc_host_read_range(path_.c_str(), (double)offset, buf, (int)len) == (int)len;
#elif ...
```

并且 **Emscripten 下不再 `::open`**（`Open()` 只记录路径）——否则会要求包存在于 Emscripten FS 里，
那就等于要求整包常驻，正是这个移植要避免的事。

**为什么是 JSPI 而不是同步 XHR**：同步范围读只在 worker 里合法，而把引擎放进 worker 会让页面的全局对象
不再对它可见（KiriKiri 那次正是死在这里）。JSPI 让读**挂起**而非阻塞，那条要求就不存在了。

**为什么帧入口是导出函数而不是 `emscripten_set_main_loop`**：参考实现
（`platforms/web/jspi_jsc_mainloop_fix.js`）记录了——glue 靠 **`wasmTable.get(ptr) === instance.exports.f`
的函数对象身份**判断该不该包 `WebAssembly.promising`，**V8 满足、JSC 不满足**，后者会在首次挂起读抛
`SuspendError: Suspending() wrapper called outside of a promising() context`。**直接调导出就绕开了这个比较**，
两个引擎行为一致。所以页面用 `requestAnimationFrame` 驱动 `artc_web_frame` 并 `await`。

构建标志：

```
-sJSPI=1
-sJSPI_EXPORTS=artc_web_frame,artc_web_open     # 帧入口与启动入口，漏一个就会 SuspendError
```

**浏览器验证结果**（最小包，`__artcReadRange` 由页面提供）：

```
/game/root.pfs
stage 1280x720        ← 来自包内的 system.ini
started 1
presented 0
last error boot failed    ← 预期：最小包没有 system/init.lua
exit requested
```

**证明是构造性的**：FS 写入路径已整个移除，引擎唯一能拿到字节的途径就是 `__artcReadRange`；而
`stage 1280x720` 只有读到包内 `system.ini` 才可能得出。**即「宿主 + JSPI 按需供数」已证明，
「框架启动」尚未证明**——把这个区分摆明，才不会把前者当成后者。

### 9.6 HTTP Range 供数：已接上，有一处仍未通过

`__artcReadRange` 已从「内存里的包字节」换成 **HTTP Range**（1 MiB 块 + 有上限的块缓存），
并对真实 2.69 GB 的 `root.pfs` 实测：

```
reads 67   requests 2   (2.0 MB, 3 cached)
```

**67 次 `ReadAt` 只产生 2 次 HTTP 请求、2.0 MB = 归档的 0.074%**——按需机制本身成立且量级正确。

配套写了一个最小的 Range 服务器（`ports/artemis/build/serve-range.mjs`）：
`coi-server.py` 会为 ETag 对**整个文件**算 SHA-256，对 2.69 GB 直接返回 **200**；而字节源**只接受 206**，
因为「服务器把整个文件发过来了」是在多 GB 归档上唯一会被误当「只是慢」的失败。

### 9.7 一处 `EM_ASYNC_JS` 封送缺陷（已修）

`const char *` 参数**没能穿过 `EM_ASYNC_JS` 的封送**：JS 侧拿到的指针不是那个字符串，
`UTF8ToString` 解出了二进制，页面被要求取一个形如

```
/%05y%EF%BF%BDqY%EF%BF%BD%EF%BF%BD%1F)%EF%BF%BD%EF%BF%BDXc%1FCA%EF%BF%BD...
```

的路径（`EF%BF%BD` 是解码器对非 UTF-8 字节给出的替换字符）。**症状是 404 一个胡扯文件名，读起来像路由问题而不是 ABI 问题。**

修法：**把字符串从异步签名里彻底拿掉**。路径只在 `Open()` 时经**同步** `EM_JS` 登记一次
（`window.__artcPacks[id] = path`），异步调用只收 `(int id, double offset, void* buf, int len)`。
修复后请求的块立刻变成正常的 `0-1048575`、`1048576-2097151`。

### 9.8 仍未通过的：框架启动

```
[ERROR] iet: not found in packs: system/first.iet
[ERROR] engine: system/first.iet missing; boot aborted
```

**而 `system/first.iet` 确实在 `root.pfs` 里**（独立核实过：存为 `system\first.iet`，21,587 条中的一条；
另外三个包都没有）。同一份代码用 CLI 跑同一份包**能到 `system/script.asb` 并进入 ADV 主循环**，
且裸相对路径、`./` 相对路径、绝对路径**三种形态都通过**——所以**包路径形态不是原因**。

未解决的差异只可能在这些地方：wasm/JSPI 下的读是否与原生一致、`ARTC_HAS_GLES=1` 的编译差异、
或索引查找在 Emscripten 下的行为。**需要下一步定位。**

> ⚠️ **一处需要撤回的过度声称。** 我先前写过「`stage 1280x720` 来自包内 `system.ini`，所以按需供数
> 已被证明」。**这是错的**：最小包声明的是 `800x600`，宿主仍报 `1280x720` —— 那是 `EngineContext`
> 的默认值 `stage_w_ = 1280, stage_h_ = 720`。**该数值不能作为「读到了 `system.ini`」的证据。**
> 目前已证明的是：模块加载、WebGL2 上下文、把包交到 `Pf8Reader` 并让它读到头部魔数（否则 `Open`
> 就会失败）与索引；**尚未证明包内容被正确读出**——`first.iet` 查不到恰恰指向这一点。

### 9.10 定位 9.8：读取器是好的，故障在它之上

在 `Pf8Reader::Open` 的解析循环后加了临时诊断（`[artc-diag]`），浏览器实测：

```
[artc-diag] pf8: id=1 path='/game/root.pfs' index_size=1185775 table_bytes=1185771
            count=21587 parsed=21587 find(system/first.iet)=yes first='font\extra_28.rft'
```

**这就是决定性的分岔点**：`index_size`、`count`、`parsed`、首条目名，与我用 Python 独立解析的结果**逐项一致**；
而 **`find(system/first.iet)=yes`** —— 读取器**找得到**那一条。

所以 9.8 的「not found in packs」**不是读取器的问题，也不是字节源的问题**（索引能正确解析本身就要求
块拼装的读是对的）。故障在 `Pf8Reader` **之上**：`PackManager::Read` 或它遍历的那条链。

#### 一条强线索：64 个幽灵 `Pf8Reader`

上一轮在浏览器里 dump `window.__artcPacks`（由 `artc_host_register_pack` 写入）：

```json
{"1":"/game/root.pfs",
 "2":"e_bytes=1185771 count=21587 parsed=21587",
 "3":"e_bytes=1185771 count=21587 parsed=21587",
 … 共 65 条，其中 64 条的「路径」相同}
```

**注册了 65 个包，但 C++ 侧只发生了一次 `Open`（`id=1`，且路径正确）。**

那 64 条的「路径」是一条**诊断格式串的片段**（`"table_bytes="` 的后半截）——即 `path` 指针指向了 `.rodata`。
而 `PackManager::Read` 是**倒序**遍历 `packs_` 的（`for (auto it = packs_.rbegin(); …)`），
所以这些幽灵实例会被**优先**查询。

**下一步就是查它们从哪来**：一次 `Open` 却出现 65 次注册，说明 `artc_host_register_pack` 被调用了 65 次，
而 `AllocatePackId()` 的静态计数器也走到了 65。可能的来源是 `PackManager::OpenChain` 的候选探测、
或 `Pf8Reader` 被以某个 `.rodata` 指针为参数构造（`Open(const std::string&, …)` 的实参悬垂）。
**这属于需要边查边判断的一类，不宜再靠猜。**

#### 一处观察方法上的教训

上一轮我据「`stage 1280x720`」判定 `system.ini` 被读到——**那是引擎默认值，不是证据**（见 9.8 的撤回）。
这一轮改用**独立可核对的量**（条目数、首条目名、与 Python 解析逐项比对）才把范围一次锁定。
**能被独立方法复算的观测才值得当证据。**

### 9.11 已解决：根因是 JSPI 的栈切换，改用 Asyncify

> **⚠ 本节结论已被推翻。** Asyncify 后来也被证伪（理由与 JSPI 同源），最终方案是
> **worker + 同步 XHR**。观察本身是真的，归因是错的——见 **9.15**。

**9.8／9.10 的「not found in packs」不是查找、不是字节源、不是包路径形态**——是 `artc_web_open` 里
**JSPI 挂起之后，调用方的栈帧地址变了**。

诊断链条（每一步都有可独立核对的量）：

```
[artc-open]  path='/game/root.pfs' len=14          ← 进入 OpenChain 时正确
[artc-chain] base='rst.iet)=yes ...' len=115       ← base->Open() 返回后被破坏
```

`base_path` 是 14 字节的 `std::string`，**短于 SSO 阈值，数据就在栈上**；挂起后 `size()` 变成 115、
内容变成 `fprintf` 的输出。**长度与内容同时变化排除了「堆缓冲区过期」**——是**字符串对象本身从另一个地址读出来的**，
即栈帧没有被保住。

（中途排除了两个错误假设：一是页面没 `await` 导致重入——修了但没解决；二是 JSPI 与 Lua 的 `setjmp`/`longjmp` 冲突——
`-sSUPPORT_LONGJMP=wasm` 需要 wasm 原生异常（`-fwasm-exceptions`），能构建但问题依旧。）

**改用 `-sASYNCIFY=1`**：Asyncify 由**生成代码显式保存/恢复 C++ 栈**，不依赖引擎的栈切换支持。改完立刻：

```
[artc-chain] base='/game/root.pfs' len=14 after base open     ← 栈完好 ✓
[artc-open]  path='/game/root.pfs.000' len=18                 ← 补丁包路径正确
```

代价是体积：wasm **1,188,046 → 1,730,035 B**。**这个交换是值得的。**

### 9.12 结果：真实游戏在浏览器里完整启动

```
[Artemis] engine: boot sequence finished
[Artemis] asb: load system/ui.asb label=last
[Artemis] asb: load system/save.asb label=last
[Artemis] asb: load system/script.asb label=last
[Artemis] asb: load system/first.iet label=game_start     ← 从 2.69 GB 包里按需取到
[Artemis] SetText: basefont.font01 400x12 '　'            ← 用真实字体排版
[Artemis] tag[trace]: wait {input=1}                      ← 进入 ADV 主循环
```

**两个包都打开了**：`root.pfs` 21,587 条 + `root.pfs.000` 7,637 条——**补丁链也通了**。

页面覆盖层的计量：

```
reads 134   requests 23   (22.8 MB, 66 cached)
started 1   presented 1   revision 29   last error (none)
```

**134 次 `ReadAt` → 23 次 HTTP 请求 → 22.8 MB，而包链总计 6,046,143,449 B（6.05 GB）= 0.38%。**

**与原生 CLI 跑到同一状态**（`drive` → ADV 等输入）。**文件读取通了。**

### 9.13 清理与遗留

临时诊断（`[artc-open]` / `[artc-chain]` / `[artc-diag]`）**已移除**，移除后重建、行为逐项一致
（`reads 134 / requests 23 / 22.8 MB / started 1 / presented 1 / revision 29`）。

一条无害警告仍在：`font not found in packs: font/extra` —— 那是游戏按**字体名**引用而该名字没有对应条目，
原生 CLI 下同样出现，不影响启动。

### 9.14 画面：当前是黑的，原因已缩小到「高偏移载荷读」

> **⚠ 本节结论不成立。** 黑屏与「高偏移载荷读」无关——`ReadAt` 那一步是对的，
> 真正的原因是画布**根本没拿到 GL 上下文**（canvas target 传了 `0`），见 **9.17**。

覆盖层读数（干净构建）：

```
reads 72   requests 6   (5.8 MB, 5 cached)
started 1  presented 1  revision 2   frames 3   last error (none)
```

**画布是黑的**，引擎日志给出了两点直接证据：

```
[Artemis] iet text: 𯬯ݖ����h4F񣸯l...      ← .iet 脚本正文是二进制乱码
[Artemis] draw[1]: drawn=0                 ← 什么都没画
```

即 **`.iet` 文件被「找到并读出」了，但读出来的字节是错的**，于是脚本无法解析、无图层可画、引擎随即退出
（`frames 3` 后 `exit_requested`）。

**这一条把范围收得很紧了**：

| | 状态 |
| --- | --- |
| 包的打开、索引解析 | **字节正确**（条目数与首条目名和独立 Python 解析逐项一致） |
| 按名字定位文件 | **正确**（`system/first.iet` 被找到） |
| 文件的**载荷内容** | **错误**（乱码） |

而唯一的量级线索是：**索引与所有已核实正确的读都发生在 2 GiB 以下**；出错的
`system/first.iet` 在 **offset 2,688,417,092 —— 超过 2^31**。

#### 已排除的（本轮全部用可独立复算的量排除）

1. **块拼装的范围读是错的？——不是。** 页面内自检，把「块拼装的读」与「对同一区间直接 `fetch`」并排打印：

   ```
   selfcheck off=2688417092 same=true
     via=d7 40 43 c0 f2 b9 a3 83 dd 7d 82 48 c0 9d ee 47
   selfcheck off=1256194048 same=true
   ```

   而这 16 字节与我用 Python 从真实文件读出的**逐个一致**。**字节源在超过 2 GiB 的偏移上也是对的。**

2. **引擎请求的偏移是错的？——不是。** 记录引擎实际传进来的载荷读：

   ```
   payload id=1 off=-1604516355 offu32=2690450941 len=3618    ← system.ini
   payload id=1 off=-1606550204 offu32=2688417092 len=1976    ← system/first.iet
   ```

   `offu32` 还原后的值与**独立 Python 解析完全一致**（`system.ini` 3,618 B @ 2,690,450,941；
   `first.iet` 1,976 B @ 2,688,417,092）。偏移以**负 int32** 到达，`>>> 0` 已正确还原。

3. **`double` 参数封送？** 改成 `int`（uint32）传参，**无变化**。

#### 因此只剩一个环节

**字节在 JS 里是对的、请求的区间是对的，那么丢失只可能发生在 `HEAPU8.set(bytes, buf)` 这一步** ——
要么写到了别处（`buf` 封送不对），要么跨 `await` 的 `HEAPU8` 已失效。

**下一步（一个自足测试，已在 `pf8_reader.cpp` 里留了钩子位置）**：写完 `buf` 后**立刻从堆里读回**
并与写入内容比对，再让 C++ 侧打印 `buf` 的头几字节：

```js
HEAPU8.set(bytes.subarray(0, n), buf);
const back = HEAPU8.subarray(buf, buf + n);   // 与 bytes 比对
```

- **读回不一致** → 写没落到 `buf`（封送或 `HEAPU8` 失效）
- **读回一致、但 C++ 看到的不是它** → 调用方在 `ReadAt` 之后又用别的来源覆盖了缓冲区

这条能把最后一段也切成两半。

> **一个值得记下的教训**：我用「只记前 N 次调用」来观察，结果那 N 次全被 `OpenChain` 的 62 次
> 探测（每个只读 11 字节包头）占满，真正的载荷读全都落在窗口之外——**看起来像「载荷根本没被读」**。
> 改成按偏移过滤（`> 1000000`）才立刻看到那 5 次载荷读。**观察窗口本身就是一种偏见，会伪造结论。**

#### 本轮的收窄：故障已从「读」推到「解密」

**写侧被彻底洗清。** 在 `artc_host_read_range` 里写入后立刻从堆里读回：

```
writeback off=2690450941 n=3618 buf=4382760 heapSameObject=true readbackMatches=true first=c3 back=c3
writeback off=2688417092 n=1976 buf=4414104 heapSameObject=true readbackMatches=true first=d7 back=d7
```

堆视图没失效、读回与写入逐字节一致、`first=d7` 与真实文件首字节相符。**`ReadAt` 是对的。**

**于是顺着调用链往下，看到 `Pf8Reader::Read` 的第 300 行：**

```cpp
out.resize(e.size);
if (!ReadAt(e.offset, out.data(), out.size())) { ... }
DecryptRange(out.data(), out.size(), 0);      // ← 无条件 XOR 解密
```

**而用真实文件验算这一步应该产出什么：**

```
raw 前 16:    d7 40 43 c0 f2 b9 a3 83 dd 7d 82 48 c0 9d ee 47
XOR 后前 48:  b'////////////////////////////////////////////////'   ← 48 个斜杠
```

（`d7^f8=2f`、`40^6f=2f`、`43^6c=2f`、`c0^ef=2f`——逐字节都是 `/`。）

**但引擎日志里显示的是另一种乱码**（`iet text: 𯬯ݖ����h4F...`），**不是 48 个斜杠**。两者不一致，
说明**引擎实际使用的密钥与 `SHA-1(index)` 不同**。

同一份代码在原生 CLI 下跑同一份包是正常的，所以差异在 Web 构建侧。密钥由 `Open` 里
`ReadAt(7, index_size)` 取「hashed 区」再 SHA-1 得到——而该区域的读取在 Web 下**索引本身解析是正确的**
（21,587 条、首条目名逐项吻合），所以要么 `hashed` 缓冲在 SHA-1 之前被改动，要么 `encrypted_`/`key_`
在 Web 分支下的赋值路径与原生不同。

**下一步（确切）**：在 `Open` 里 `sha1(hashed)` 之后打印密钥的前 4 字节，与
`f86f6cef…` 比对。一致 → 故障在 `DecryptRange` 的用法；不一致 → 故障在 `hashed` 的取用。

> 这一轮再次印证了那条教训：**我先前所有「猜机制」的尝试（JSPI+longjmp、double 封送、HEAPU8 失效）
> 全部落空，而每次靠一次自足比对就前进一段。** 写侧、读侧、字节源、偏移——四步都是比对出来的，
> 没有一步是猜出来的。

## 九·补 定案：worker + 同步 XHR，根因是 canvas target 传了 `0`

> 9.11～9.14 是在**尚未定案时**写下的过程记录。**9.11 的结论（改用 Asyncify）已被推翻**，
> 9.14 认定的黑屏原因（`HEAPU8.set`）也不成立。下面是最终成立的那一版，读 9.11/9.14 时请以本节为准。

### 9.15 Asyncify 同样不可行，理由与 JSPI 是同一个

Asyncify **确实**保住了调用方的栈——9.11 里 `base_path` 从 14 字节变回 115 字节的观察是真的。
但它救不了**读**：Asyncify 在 `await` 处把 JS 控制权交出去，而 JS 那一侧拿到 range 之后写回的时机
落在 **wasm 恢复之后**。证据是同一缓冲区的两侧读数：

```
[artc-mid] data_before=0x47a250 data_after=0x47a250 same=1 bytes=00 00 00 00
writeback  off=2690450941 n=3618 buf=47a250 ...          ← 出现在上面这行之后
```

指针相同、写回内容正确，**只是顺序不对**——这就是「读一次对、读三次全零」的来源。

所以 JSPI 与 Asyncify 的失败是**同一件事**：它们都在让一次**异步的读看起来同步**。
浏览器里另有一条路能让读**真的同步**——同步 `XMLHttpRequest`。它在主线程上被
`InvalidAccessError` 禁止，在 **worker 里合法**。把引擎放进 worker，读就不再需要挂起任何东西。

### 9.16 定案的架构

| 文件 | 职责 |
| --- | --- |
| `src/host/web_page.html` | 先定背板尺寸 → `transferControlToOffscreen()` → 转发指针/键盘/尺寸 |
| `src/host/web_worker.js` | worker：`self.Module`（含 `canvas`）**先于** `importScripts` 建立，再驱动帧循环 |
| `src/pack/pf8_reader.cpp` | `ReadAt` 走同步 XHR，无 `await`、无挂起 |
| `src/host/web_main.cpp` | 14 个 `artc_web_*` 导出，页面驱动帧 |

wasm **1,101,287 B**（Asyncify 版 1,729,115 B，JSPI 版 1,188,046 B）——**架构正确之后体积也回来了**。
`web_shell.html`（单线程宿主）已删除：它内嵌的正是被证伪的那两条路线。

### 9.17 真正的根因：`emscripten_webgl_create_context(0, …)` 不解析任何画布

`CreateGlContext()` 一直是 `emscripten_webgl_create_context(0, &attrs)`。而 glue 里是：

```js
var specialHTMLTargets = [0, globalThis.document ?? 0, globalThis.window ?? 0];
var findEventTarget = target => {
  target = target > 2 ? UTF8ToString(target) : target;      // 指针解码，0 保持为 0
  return specialHTMLTargets[target] || globalThis.document?.querySelector(target);
};
function _emscripten_webgl_do_create_context(target, attributes) {
  var canvas = findCanvasEventTarget(target);
  if (!canvas) { return 0; }                                // ← 一直死在这里
  ...
}
```

**槽 0 是字面量 `0`，document 在槽 1。** 于是 `target = 0` → `specialHTMLTargets[0]` 为假 →
落到 `document.querySelector(0)`：

- **主线程**：`querySelector(0)` 抛 `TypeError`
- **worker**：没有 `document`，直接 `return 0`

两种情况下都**在碰 WebGL 之前就返回了 0**，所以现场只有一句 "could not create a WebGL2 context"，
既没有 GL 错误也没有别的解释——看起来像显卡/driver 问题，其实是**参数写错了**。

修法两处：

1. `web_main.cpp` 用 `kCanvasTarget = "#canvas"`（与页面元素名一致）；
2. `web_worker.js` 提供一个只回答 `querySelector('#canvas')` 的 `document` 垫片，返回被转移进来的
   `OffscreenCanvas`——于是**同一个选择器在主线程与 worker 上都成立**。

同一个 `0` 还出现在 `emscripten_get_canvas_element_size(0, …)`，所以 `Tick()` 每帧的背板尺寸
一直在 `-4` 之后 fallback 回 stage 尺寸（1280×720），画布实际是 750×485。

另外 `GL.createContext` 内部有一段 Safari 修正，其返回判定读 `gl instanceof WebGLRenderingContext`；
worker 里这个构造器不存在，`instanceof` 对 `undefined` 会**抛**而不是返回 false，因此补了一个占位构造器。

顺带修掉一个**诊断性**缺陷：`CreateGlContext()` 失败只写 `Log(...)`，不写 `last_error`，
所以页面读到的是空的 `last_error`。现在 `artc_web_open` 的两条失败路径都写 `SetLastError`。

### 9.18 结果：真实游戏在真实浏览器里渲染并响应输入

`node ports/artemis/harness/browser/web-input.mjs`（headless Edge）：

```
opened /game/root.pfs (1280x720)
stage 1280x720     started 1     last error (none)
frames 961  presented 553  revision 801
```

- **标题画面**：ハミダシクリエイティブ 的 logo + 四名角色 + START/CONTINUE/LOAD/CONFIG/SPECIAL/EXIT
- **点击 START**（CDP 派发真实鼠标事件，走完整链路 页面 `pointerdown` → worker → `artc_web_pointer` → Lua）：

```
click: hit='1.0.-1' registered=no
click: fallback -> onpush key 1
lydel {id=startmask}
call {label=estag03, file=system/script.asb}
lyc {id=10, file=pc/ja/caution}
wait {input=1, time=2500}
```

  → 离开菜单、删掉 `startmask`、走真实脚本、进入 まどそふと 片头 LOGO。**输入是通的。**

按需供数的实测（服务器端计数，`GET /__totals`）：

```
rangeHits 123   packBytes 24,968,301 B   bytesSent 26,197,202 B (含 1.2 MB 宿主产物)
```

**24,968,301 / 6,046,143,449 = 0.41%** —— 从冷启动到片头只读了包链的 **0.41%**，
而整链 6.05 GB 无法整体载入。

启动期日志里 63 个 `root.pfs.0NN -> 404` **不是缺陷**：`PackManager::OpenChain` 按设计探测
`<base>.000` … `<base>.063` 共 64 个补丁位，只有 `.000` 存在。

### 9.19 运行中崩溃的根因：`-fno-exceptions` 把 `stof` 的抛出直接变成 `abort()`

**症状**：玩几分钟后画面停住，控制台一句裸的 `Aborted()`，没有原因、没有栈。

打开 `-sASSERTIONS=1` 之后它自己说出了名字（这也是这条教训的由来：**断言版本应该更早开**）：

```
Aborted(native code called abort())
frame 50764 threw: Aborted(native code called abort())
```

引擎日志里紧挨着的那两行就是答案：

```
tag[trace]: lytween {y=off,0, time=250, from=off, id=1.80, ease=easeout_quad, to=0, param=top}
invalid_argument was thrown in -fno-exceptions mode with message "stof: no conversion"
Aborted(native code called abort())
```

**根因**：`Compositor::ToF` 原本写成

```cpp
try { return s.empty() ? def : std::stof(s); } catch (...) { return def; }
```

这段代码的**意图**是「解析不出来就用默认值」。但 Emscripten 默认 `-fno-exceptions`，
用这个模式编译出的 libc++ **不传播抛出，而是直接 `std::terminate` / `abort()`**——
`catch (...)` 是**死代码**。于是 `from=off` 里的 `off` 进了 `std::stof`，整个引擎被拉下来。

**顺带发现的同类地雷**：`lyprop` / `lyc` 的属性解析里，`x/y/w/h/anchorx/anchory/z/xscale/yscale`
用的是**裸 `std::stof` / `std::stoi`**（两处结构相同的块，共 24 + 2 处）。
也就是说 `top=off` 会以完全相同的方式崩——**而本作的脚本里就有 `y=off,0`**，只差一个属性名。

**修法**（三条）：

1. `ToF` / `ToI` 改用 `strtof` / `strtol`：失败通过 `end` 指针与 `errno` 报告，**不抛**。
   这就是原来 `try/catch` 想表达的东西，换一种在**所有目标上都成立**的写法。
   `ZCmp` 里那处 `std::stoll`（同样套在死掉的 `try` 里）一并改成 `strtoll`。
2. 24 处 `std::stof(kv.second)`、2 处 `std::stoi(kv.second)` 全部换成 `ToF` / `ToI`。
3. `from=off` 现在走它本来该走的语义：`from_current`（默认就是 `true`）= **从图层当前值开始动画**。
   注意这一点**即使不崩也是错的**——原代码会把 `from_current` 置为 `false` 再崩。

**验证**：同一个脚本、同一个窗口尺寸（2561×1398），崩溃点原本在 ~50,764 帧，
修后连续跑 **442 s / 70,395 帧 / revision 4173，crash lines = 0**，穿过原崩溃点与用户报的 revision 4222。

### 9.20 另一个独立缺陷：runtime 初始化前转发输入

在真实 Chrome 里第一次跑就复现了另一个 `Aborted`，断言版直接点名：

```
Aborted(Assertion failed: call to '_artc_shim_push_motion' via reference taken before
                     Wasm module initialization)
```

**触发条件很日常**：页面在模块就绪前就挂好了 `pointermove`，**鼠标在加载期停在页面上**即可。
无头脚本从不移动鼠标，所以一直没暴露——这也是「换真实浏览器」这一步的价值。

#### 真正的机制（比「加个 ready 门」更要紧的一点）

一开始以为是「导出在 `initRuntime()` 完成前**还不存在**」，据此写了 `if (Module._artc_shim_push_motion)`
当守卫。**这个守卫是错的，而且错得刚好相反**：Emscripten 胶水给每个 `EXPORTED_FUNCTIONS` 槽位
**预先填了一个真实存在的函数**：

```js
var _artc_shim_push_motion = Module["_artc_shim_push_motion"] =
    makeInvalidEarlyAccess("_artc_shim_push_motion");
```

`makeInvalidEarlyAccess` 返回的是**真函数、truthy**，只有**调用它**才 `abort()`。
所以 `if (Module._artc_shim_push_motion)` **恒为真**，什么也没挡住。

**后果比「调用被忽略」重得多**：`abort()` 会把整个运行时置为死亡状态，于是

- 一次在实例化期间到达的指针事件就**在引擎启动之前把它杀掉**；
- 之后 `pump()` 因为 `runtimeReady` 永远为 false 而**静默返回**；
- 现象是 `packs registered` 与 `artc_android_boot` **永不出现**、画面全黑，
  而控制台只有成对的 `[artc] Aborted(...)` + `[artc] worker error [object ErrorEvent]`。

也就是说：**症状看起来像「引擎没启动」，成因却是「有人先打了它一下」**。本次是加载期先到了两个
pointer 事件（`ptr=2`）。

**修法**（`ports/artemis/build/artemis-web-android-worker.js`）：不再依赖导出的存在性，改为显式查自己的标志位：

```js
const live = function (fn) { return runtimeReady && typeof fn === 'function'; };
// 三处守卫：if (live(self.Module && self.Module._artc_shim_push_motion)) / _artc_shim_push_key
```

初始化前的输入直接丢弃——引擎还没起来，没有状态可作用。

**教训（可复用）**：Emscripten 的 `Module._x` 不是「未就绪时不存在」，而是
**「未就绪时存在、但一调就 abort」**。任何用 truthiness 当就绪守卫的代码都是假的守卫；
必须查 `runtimeReady` 这类自己的标志，或 `typeof` + 自己的判定。

**验证**：`?v=41`（1500×900 CSS、dpr 1.5）→ `packs registered` → `artc_android_boot(2252x1479) -> 1`
→ `viewport: 2252x1266 at 0,106` → `draw[1]/draw[301]/draw[601]`；`getImageData` 非黑像素
**85.592%**、`maxLum=255`；点击后进入游戏（`startmask` 出现、`1.80.mw.adv_adv` 建层）。

### 9.21 动画位置不对：`off` 只在两处之一被识别

**症状**：部分动画把图层放到了错误的位置——像是「先跳到 0 再滑到目标」，而不是从它当前的位置开始动。

`off` 是引擎里**表示「图层当前值」**的记号。9.19 修完之后，全仓库认得它的地方**只有一处**
（`from=off`）；而 `lytween` 的数值还可以写成**成对形式**：

```
lytween {y=off,0, time=250, from=off, id=1.80, ease=easeout_quad, to=0, param=top}
                     ^^^^^^ 本作实际写出来的就是这种
```

成对分支当时是：

```cpp
else if (comma != std::string::npos) { tw.from = ToF(pair.substr(0, comma)); tw.from_current = false; }
```

`ToF("off")` 返回 `0`，于是 `from = 0`、`from_current = false`——
**图层被摆到 0，再从 0 滑向目标**。它应该做的是 `from_current = true`，
让首帧从图层**当前**位置取值（这正是 `from_current` 字段存在的理由，且它默认为 `true`）。

**修法**：成对分支与 `from=off` 走同一条判断，`off` → `from_current = true`。

**这一条同时解释了两件事**：为什么只有「某些」动画不对（写成 `from=` / `to=` 的那些一直是对的，
写成 `<param>=off,<target>` 的才错），以及为什么它此前从未暴露成别的现象——
在 9.19 之前，这个字符串会先撞上 `std::stof` 把引擎打崩，根本走不到「位置画错」这一步。

**验证**：重建后真实 Chrome（2561×1398）标题菜单往返，boot 正常、tweens 正常、crash lines = 0。

**仍未排除**：`lytween` 的 `param` 回退顺序是
`{"alpha","left","top","x","y",…}`——一个标签里同时写 `x=…` 与 `y=…` 时只会动 `x`。
官方标签本就是「一个标签一个参数」，故未改动，但若仍有轴向只动一边的现象，这里是第一嫌疑。

### 9.22 顺序问题：一次**失败并已回滚**的尝试 —— 不要重做

四个排序调用点都写成：

```cpp
const int c = ZCmp(a, b);
if (c != 0) return c < 0;
return SectionCount(a) < SectionCount(b);   // ← 永不执行
```

`ZCmp` 在首数字相同后比较整个字符串，且只有两 id 完全相等才返回 0，所以 `c == 0 ⇔ a == b`，
**那行深度 tiebreak 确实是死代码**。我据此改成「z → 深度 → id」，理由是旁边的注释声称
「深一层者在上」是引擎行为。

**改完层级立刻错了，已回滚。**

原因：`500.b.*`（按钮）深度是 4，`500.ch.*`（角色）深度是 3。深度优先会把**按钮画到角色上面**。
按 id 文本排——`'b' < 'c'`，按钮在下、角色在上——才是正确的。

**真正的教训是：那行 tiebreak 拿不到的结果，恰好就是对的。**

| | 顺序（同 z） |
|---|---|
| 注释宣称的规则（深者在上） | `500.b.2.0`（4）> `500.ch.0`（3）→ 按钮压住角色 ✗ |
| `ZCmp` 实际给出的（按 id 文本） | `'b' < 'c'` → 按钮在下 ✓ |

现在四个调用点统一走 `ZOrder(a, b) { return ZCmp(a, b); }`——**行为与改动前完全一致**，
提出这个名字只是为了四个点读起来一致，并在函数注释里写明「此路已试过，不要再修」。
验证：标题 `draw[]` 顺序为 `… 500.b.1 500.b.2.0 … 500.b.7.0 500.ch.0 500.ch.1 500.lo.0`，
角色在按钮之后绘制。

**教训**：注释里写的「引擎行为」不等于引擎行为；把一条死代码「修活」之前，要先证明它拿到的结果比现状更差。
这条与 9.19/9.21 是同一类错误的镜像——那两条是真 bug，这一条是我自己造出来的。

## 十、仍未完成

> 这一节同样按当前状态重列。原列表里「provider 未接线」一条仍然成立，另两条仍在；
> 但它的**口径**变了：真实游戏现在跑的是官方 SDK 那条路线（§12）以及**重写后的引擎**（§十七），
> 所以「未完成」要分成「宿主侧（`ports/artemis/` 这棵树）」与
> 「产品侧（`packages/runtime-artemis/` 的接线）」两栏来看。

**宿主侧（`ports/artemis/` 的 Web 宿主）**

- **音频**：`.ref/artemis-compat/src/audio/audio.{cpp,h}`、`pcm_ring.h` 的 OpenSL ES
  （`audio.cpp:25-26` include 的是 `<SLES/OpenSLES.h>` 与 `<SLES/OpenSLES_Android.h>`）
  必须重写为 Web Audio。**接缝是符号级的**：`.ref/tynext-git/engine/src/main/cpp/artemis_loader.cpp:9`
  写明它 `dlopen(..., RTLD_GLOBAL)` 把真实 so 的符号摊进全局表，
  「供 `artemis_audio_bridge` 的 `dlsym` 命中」——所以宿主侧只要提供同名符号即可，
  不需要改引擎的调用点。调用确实已经到了：
  `.ref/artemis-compat/src/audio/audio.cpp:43` 的
  `Log(kLogInfo, "audio(host): play " + key + " " + file + " loop=" + …)`
  就是日志里 `audio(host): play track:1 sound/sysse/none.ogg loop=0` 的出处，只是还没出声。
  这是目前最大的一块空白：`ports/artemis/` 里没有任何 Web Audio 代码，
  页面与 worker 也都不碰音频。
- **读缓存**：同一段载荷在一次启动里被反复取（`23540810-23624647` 读了 20 次），目前没有缓存层。
  不影响正确性，但每次都是真实的 HTTP 往返。
- **引擎日志过于啰嗦**：`[INFO] tag[trace]:` 每帧数千行。宿主侧只做到「把关键的几类镜像进页面」
  （`ports/artemis/build/artemis-web-android-worker.js` 的白名单正则），**没有在源头降级**：
  `artemis-compat` 的 `src/log/logger.h:25` 里 `kMinLogLevel = kLogInfo` 仍写死，
  所以这几千行照旧进控制台。

**产品侧（`packages/runtime-artemis/`）**

- **provider 未接线**：`artemisProvider.availability` 读 `artemisEngineHosts.length > 0`
  （`packages/runtime-artemis/src/runtime.ts:194`），而该数组在仓库里是**空的**
  （`:80`，注释写明是刻意的占位），所以永远是 `unavailable`（priority 85）。
  §12 那条路线跑通之后，这块缺的是**把 host 注册进去**，不是引擎能力。
  另注：`ARTEMIS_MISSING_CAPABILITIES`（`:100`）列的五条——RFVP/Artemis 的 WebAssembly 构建、
  Lua 5.1 宿主 API、PFS 解包与 `.iet` 解释器、E-mote/PSB 解码、OP/ED 视频后端——
  现在**前三条已经不缺了**（引擎就是那个 wasm），是文案没跟上。

> **一处保留的自我更正。** 本节末尾原先挂着这句话，它比它更正的那条结论更值得留：
> 原 9.14 的结论（`HEAPU8.set` 是黑屏原因）**不成立**：那一步是对的，见 9.15–9.17。


## 九·补二、要补的标签到底是几个：**59 个，不是 186 个**

文档里 186 个标签、而 `artemis-compat` 只认得其中 104 个，看上去是最大的缺口。这个比较是错的，
因为它把**两套词汇**当成了一套。

**第一套：`.ast` 场景格式的词汇。** 真实游戏的 `script/*.ast`（218 个文件、11.2 MB）里出现
37 个名字（`text` 32092 次、`fg` 19985 次、`vo` 15064 次、`bg` 6579 次 …）。但它们**不是引擎标签**：
`.ast` 由**游戏自带的 Lua** 解析（包里 57 个 `.lua`，`system/init.lua` 逐个 `e:include`），
名字表就在 `system/init.lua:5599`（`{"tone","timezone","bg","fgf","fg","fgact",…}`），
分派在 `system/image/image_fg.lua` 等处（`elseif p3 == "fg"`、`elseif nm == "fg"`）。
**引擎从来不看见这些名字。** 拿它们算缺口，得到「37 个里缺 32 个」，是个吓人且无意义的数字。

**第二套：引擎标签**，即游戏 Lua 通过 `e:tag{...}` 交给引擎的那些。这才是我们要实现的接口，
也正是 `.ref/artemis-compat/src/script/lua_tag_dispatch.cpp` 的 `TagTable()` 所分派的。

对真实游戏测量第二套（`pfs-grep --filter '\.lua$' --dump` 后统计 `e:tag{"…"`）：

    59 distinct / 478 calls
      有同名处理器       52 distinct / 467 calls
      seton*/delon* 前缀  7 distinct /  11 calls
      未处理             0 distinct /   0 calls（本段补齐后）

**真实接口是 59 个标签，不是 186 个。** 补齐的 4 个都不是逻辑缺口，而是平台表面：

| 标签 | 调用 | 官方 `Docs/tag/` | 处理方式 |
| --- | --- | --- | --- |
| `mouse` | 4 | 有，**注明 Windows 专用** | 无操作：网页的光标由页面掌管 |
| `caption` | 1 | 有，**注明 Windows 专用**（设窗口标题栏） | 无操作 |
| `autosave` | 3 | 有（`allow=1/2` 退出时或每次等待时存档） | 无操作：游戏初始化时就设成 `allow=0`，且我们没有退出时存档路径 |
| `trophy` | 1 | **根本没有这个文档** | 无操作，平台自定义 |

另外 7 个（`setonwindowbutton`、`deloncommandskipin/out`、`deloncontrolskipin/out`、
`setoncontrolskipin/out`）本来就由 `TagGenericSetOn` 的 `seton*`/`delon*` 前缀注册表兜住
（`src/script/lua_tags_input.cpp:126`），把它们算成缺失同样是跨层比较的错。

**旁证**：59 个里有 55 个能在官方 `artemis.wasm` 的字符串里找到（`var`/`rp`/`rt`/`lyc` 这四个
太短，被字符串提取的 4 字符下限滤掉了）；而 `autosave` 在官方 **arm64 `libartemis.so` 的
字节里找不到**——官方 Android 版也没有它。这印证了「表里那 4 个不是我们的缺口」。

工具：`ports/artemis/tools/tag-usage-gap.mjs`（`--lua` 看引擎层、`--ast` 看游戏层、`--probe` 查官方二进制）。


## 十一、本轮定位方法的复盘

值得留下的是**定位顺序**，而不是任何单个结论：

1. 先用**可独立复算的量**（条目数、首条目名、与 Python 解析逐项比对）确认「读取器是好的」，
   而不是靠「看起来跑起来了」——上一次我正是据 `stage 1280x720` 误判，而那是默认值。
2. 再用**长度变化**（14 → 115）区分「堆缓冲区过期」与「栈帧地址变了」——两者的修法完全不同。
3. 用**逐次 `Open` 的实参日志**把「谁传了错路径」钉死，而不是猜 `PackManager` 的逻辑。
4. 每一步都让**症状与原因分居不同层**：`404 一个胡扯文件名` 是 ABI 问题，`stop after frame 1`
   是导出返回值问题，`black canvas` 是载荷字节问题。**这三者表面上都像「引擎没起来」。**


## 十二、改向：直接跑官方 WebAssembly 构建

`artemis-compat` 这条线的输入缺陷长期收敛不掉，于是**停止继续打磨我们的 port**，改为直接用
**官方 SDK 自带的 WebAssembly 构建**（`.ref/artemis/WebAssembly/`）跑真实游戏。本节记录这条线，
它现在是**唯一验证过能在浏览器里完整跑起真实游戏**的路线。

### 12.1 只有 asyncify 模式能用

官方给两种投递方式，**不是同一种东西的两种打包**：

- `WebAssembly/`（plain）：把整包 `artemis.data` 下载后 `FS_createDataFile` 进内存。readme 自称
  「オンメモリモードが基本」，对 15.8 MB 的 SDK 示例没问题；真实游戏不行——`pack_file.txt`
  把单个 `.pfs` 限在 2 GB，而本作是 `root.pfs` 2.69 GB + `root.pfs.000` 3.36 GB。
- `WebAssembly/asyncify/`：引擎按脚本需要**逐个文件**去 `<ASYNCIFY_URL><path>` 取。这是唯一可行的路。

asyncify 需要 `system.ini` 的 `[WASM]` 段补两项：`SAVEPATH`（必需）与 `ASYNCIFY_URL`（以 `/` 结尾）。
`ASYNCIFY_HASH_LIST_URL` 是**可选**的（readme 明写「必須ではありません」，只用于 IndexedDB 缓存校验）。

**本作发布的 `system.ini` 恰好缺了 `SAVEPATH`**（`[WASM]` 段只有 `WIDTH/HEIGHT/CHARSET/BOOT`），
所以直接跑会在引擎初始化工具层时卡住。补上即可。

### 12.2 整条路线固化为一个脚本，且不依赖任何 SDK 专有二进制

`ports/artemis/build/official-web-deploy.py`：

```bash
python ports/artemis/build/official-web-deploy.py \
  --game .test/artemis-official-async/game \
  --out  .test/artemis-official-async/deploy
cd .test/artemis-official-async/deploy && python -m http.server 4700 --bind 127.0.0.1
# 打开 http://127.0.0.1:4700/web/artemis.html
```

它取代了官方 Windows 工具链里的两个专有二进制：

| SDK 工具 | 本脚本的替代 | 为什么必须自己写 |
| --- | --- | --- |
| `GeneratePackFile2.exe` | `write_pf8()` | 官方 packer 是 Windows-only 专有二进制；asyncify 模式下 pack 只需含 `system.ini`，自己写是 30 行的事 |
| `ApplyWasmData.exe` | `embed_package_size()` | 只是把包长度写回 `artemis.js` 的元数据 |

`.ref/` 与 `.test/` 都在 `.gitignore` 里；脚本只**读** SDK。许可见 §15——官方
`Copyright.txt` 只授权给向 Mikage 申请过的人，商业需付费，**不是 GPL**；但这份 SDK 的二进制
现在**是入库的**（`ports/artemis/vendor/artemis-sdk/`），那是一个有意的决定，理由与代价写在 §15。

### 12.3 自写 pf8 与官方 packer 逐字节相同

`write_pf8()` 的输出与 `GeneratePackFile2.exe` 在**单条目**（本脚本唯一用到的情形）下
**逐字节相同**：同一份 `system.ini` 两边都是 `3752 bytes`，`IDENTICAL`。
跨 7 组受控 fixture 比对，`one_sys`/`five_mix`/`nested`/`empty`/`big`/`jp` 全部 IDENTICAL。

唯一残留差异是多条目时 payload 的**物理排放顺序**（官方近似大小写不敏感的字母序，我们按 index 的
字节序）。payload 位置来自 index 里的 offset，**任何读者都看不见**；只有对整个文件做哈希时才体现。

**顺带挖出一个格式事实：官方引擎在记录表之后还读一段 directory trailer**，而 GARbro 的
`ArcPFS.cs` 与我们的 `packages/runtime-artemis/src/pfs.ts` 都停在第 `count` 条记录、都不读它。
少了它，引擎打开 pack 时直接断言：

```
Assertion failed: mlf_ >= minimum_max_load_factor, at: .../boost/unordered/
  detail/implementation.hpp,3027,min_buckets_for_size
Assertion failed: false, at: .../Contribs/babel/babel.cpp,1513,translate
```

trailer 的形状（用受控 fixture 反推，`write_pf8()` 的 docstring 有完整说明）：

```
u32  record_count + 1
repeat record_count times:  u32 (index offset just past THIS record's name), u32 0
u32 0, u32 0
u32 (index offset just past the LAST record)
```

另外两条同源的格式事实：**版本必须是 6**（`keyed = 4/5/8/9` 会用 `SHA-1(index)` 异或 payload，
声明 8 却写明文会 XOR 成乱码再在 boost 里断言），**名字编码是 CP932 不是 UTF-8**。

### 12.4 踩到的坑：`ApplyWasmData.exe` 改的不是一个数，是两个

第一版 `embed_package_size()` 只替换 `remote_package_size`，部署**静默失败**：

```
canvas 1×1、status 空、控制台 0 条消息、一个 /game/... 请求都没发
```

真相是 `artemis.js` 顶部的元数据把包长记了**两处**：

```js
loadPackage({"files":[{"filename":"/root.pfs","start":0,"end":236,"audio":0}],
             "remote_package_size":3752,"package_uuid":"..."})
```

`remote_package_size` 决定**下载**多少字节，各 file entry 的 `end` 决定交给 `FS_createDataFile`
的**字节范围**。只改前者，`DataRequest` 拿到的是被截断的缓冲区，解包失败且**什么都不报**。
`ApplyWasmData.exe` 两个都改。现在 `embed_package_size()` 两个都改，并同时换掉 `package_uuid`
（它给 asyncify 的文件缓存做键，留着旧值会供应陈旧字节）。

**这个坑值得记形状**：它不是「部署到子目录坏了」——那只是凑巧同时发生，让我一度去查
`ASYNCIFY_URL` 的 origin 相对性。**真正的原因是元数据内部不一致，而它的失败模式是完全沉默的。**

**教训**：「静默失败 + 只改一个字段」是最好的对照实验机会——把能跑的那份与不能跑的那份
**逐字段 diff**（`diff <(tr ',' '\n' < a.js) <(tr ',' '\n' < b.js)`），比去猜目录结构快得多。

### 12.5 验证

部署到 `http://127.0.0.1:4700/deploy/web/artemis.html`（pageId 6，1500×900 CSS，dpr 1.5）：

- canvas **2191×1321**，`calledRun: true`
- `preserveDrawingBuffer` 为 false，**必须在 `requestAnimationFrame` 回调内 `readPixels`**，
  否则读到的全是 0
- 非黑像素 **2,701,503 / 2,894,311 = 93.338%**，`maxLum = 255`
- ASCII 亮度图可辨：顶部 logo 带、中部人物立绘、右侧菜单块
- 请求流（全部来自 `/game/`，证明是**按需**读散装树，不是整包进内存）：
  `system/first.iet` → `system/init.lua` → 20+ 个 `system/**.lua` → `system/{ui,save,script}.asb`
  → `script/brandlogo.ast` → `pc/ja/title/{btn,bg,char,logo,menu,char_l,char_s}.png`
  → `sound/bgm/bgm99.ogg`；SAVEPATH 解析为 `artemis_e1ca47e694`
  （拉丁化后为空的日文 `SAVEPATH = まどそふと\ハミダシクリエイティブ` 走 sha1 前 10 位，
  避免所有游戏塌进同一个存档槽）

**旁证：官方路线的坐标映射是亚像素级正确的**，所以 m03520 冻结的 mouse 缺陷**在我们的 port 里，
不在引擎/web 层**。官方胶水把权威坐标写在 `window.Browser.mouseX/mouseY`（由它自己的
`calculateMouseEvent` 写入），可直接当标定参照：

```js
var rect = Module["canvas"].getBoundingClientRect();
var cw = Module["canvas"].width, ch = Module["canvas"].height;
var x = event.pageX - (scrollX + rect.left);   // 再乘：
x = x * (cw / rect.width);                     // <- backing-store 像素
```

**关键点是乘 `canvas.width / rect.width`，而不是盲乘 `devicePixelRatio`**——两者只在
`canvas.width == cssWidth * dpr` 精确成立时才等价。我们的
`ports/artemis/harness/android-host.html` 传的是 CSS px、由 worker 乘 `dpr`，
`canvas.width = Math.round(size.width * dpr)` 的取整差与 resize 后的累积差都会破坏这个等式。
这是 m03520 那个缺陷的**具体、可验证的机制**（用户已要求先冻结，未改）。

### 12.6 差点让我误判的陷阱：窗口被折叠后 rAF 被限流到 1.2 Hz

测输入时有一轮结论是「坐标送达正确、但点击完全没反应，画面像素签名一动不动」。
**这个结论是错的，成因在观测侧**：浏览器窗口当时被折叠成了标题栏大小
（`window.outerWidth/outerHeight = 159×27`），页面虽然 `visibilityState: "visible"`、
`document.hidden: false`、`document.hasFocus(): true`，但**合成器不再驱动 `requestAnimationFrame`**：

```
rAF 1.2 Hz   ← 窗口折叠（outerWidth 159×27）
rAF 161.5 Hz ← resolve_size 1500×900 后
```

引擎的主循环挂在 rAF 上，所以**帧根本没在推进**，输入自然什么都不触发。恢复窗口尺寸后，
同一次点击立刻生效：悬停高亮改变像素、点击后请求数 15 → 19 → 23 → 27、
画面进入事件 CG（`image/ev/ev_com_01/*.png` + `sound/vo/hiy/fem_hiy_0000N.ogg`）。

**教训（可复用）**：判定「输入没反应」之前，**先量 `requestAnimationFrame` 的实际频率**。
`visibilityState` / `hasFocus` 都正常**不代表** rAF 在被驱动；窗口尺寸与 rAF 频率才是直接证据。
一个 1.5 秒的 rAF 计数就能把「引擎缺陷」与「观测环境」分开——这与 9.x 那轮
「截图工具返回缓存帧」是同一类错误的两个变体：**症状看起来像被测对象坏了，其实是测量通道坏了。**

### 12.7 部署脚本自身的四个缺陷（都已被真实运行暴露）

`official-web-deploy.py` 第一次真正跑起来之后，剩下没跑通的地方全在脚本里，不在引擎里。
四个都属于「错了不会当场报错」的那一类，所以逐条记下来。

**（1）`patch_system_ini` 只幂等于「同一组选项」，跨选项集会留下陈旧键。**
它原地改 `game/system.ini`，原先只跳过本次 `extra` 里的键，**不删自己上次写下的键**。
于是「先 `--hashlist`、再去掉 `--hashlist` 重跑」会把上一轮的 `ASYNCIFY_HASH_LIST_URL`
留在文件里，把引擎指向一个已不存在的列表。实证：一次带 `--hashlist` 的部署被超时打断后，
重跑得到的 `artemis.data` 是 **3,817 B** 而不是 **3,752 B**，差值正是残留那一行。
修法是把受管键显式列出（`SAVEPATH` / `ASYNCIFY_URL` / `ASYNCIFY_HASH_LIST_URL`），
**本轮没设的一律删掉**，并打印 `[WASM] -KEY (stale, removed)`。
修完三连测：设三个键 → 去掉一个（被删、文件中不再出现）→ 再跑一遍（逐字节稳定）。

**（2）`build_hashlist` 用 locale 默认编码写文件，中文/日文路径直接崩。**
`open(out, "w")` 在 Windows 上是 GBK，而这个游戏有 **528 个日文路径**
（`image/line/title01/アメリ.png` 之类），于是写到一半
`UnicodeEncodeError: 'gbk' codec can't encode character '\u30fb'`。
注意这个错误**不是**提取出来时就已经坏掉的名字（全树 `U+FFFD` 计数为 0，名字是干净的 UTF-8），
纯属写文件时的编码选择。已改成 UTF-8 字节写，并按 **UTF-8 字节序**排序，与引擎在别处
命名文件的方式一致（它请求时把路径按 UTF-8 百分号编码——这一点已由
`pc/ja/mw/name/武田信玄.png` 成功 200 证实）。

**（3）hashlist 的 URL 写成了根绝对路径。**
文件写在页面同目录（`<out>/web/hashlist.txt`），URL 却配成
`http://127.0.0.1:<port>/hashlist.txt`——只有部署目录**恰好就是服务根**时才碰对。
部署成 `<out>/deploy-hl/web/artemis.html` 时就 404，而**引擎会把 404 页面当成列表读**
（`Detected invalid hash entry. <!DOCTYPE HTML>`）。改成**页面相对**的 `hashlist.txt`，
两种摆放都对。`ASYNCIFY_URL` 保持绝对是有意的，因为它指向的是 `/game/` 这棵散装树。

**（4）`embed_package_size` 的分析见 12.4，那一节留的是同一个教训**：这几个缺陷的
失败模式全都不是「报错停下」，而是「继续跑、但跑的是别的东西」。

**验证（改完后重跑一次完整 `--hashlist` 部署）**：
`hashlist.txt: 29,224 entries, 2,102,078 bytes, trailing newline = False`；
页面 `.../deploy-hl/web/artemis.html` 拉 `hashlist.txt` 得到 **200 / 2,097,730 B / 29,224 行**
（含日文行 `image/line/title01/アメリ.png,326cbb…`），控制台里那条
`Detected invalid hash entry` **已消失**；canvas **2251×1351**、`calledRun: true`、
非黑像素 **2,851,993 / 3,041,101 = 93.782%**、`maxLum 255`；
25 个 `/game/` 请求推进到标题画面（`pc/ja/title/{btn,bg,char,logo,menu,char_l,char_s}.png`
+ `sound/bgm/bgm99.ogg`）。

### 12.8 中文补丁：原版说明要求改 `root.pfs.001`，网页路线要换个做法

本作目录里有两个汉化补丁（`chs_patch_official.pfs` 447 条、`chs_patch_baijingmu.pfs` 445 条），
`使用方法（必读）.txt` 说启用方法是**把补丁改名成 `root.pfs.001`**——那是给**客户端**用的：
客户端的 `.pfs` 链按后缀递增往上叠。

**网页这条路没有 `.pfs` 链可叠**：asyncify 模式是引擎按路径去 `<ASYNCIFY_URL><path>` 取**散装文件**，
根本没有 pack 叠加这一步。所以正确的做法是在**解包时**就把补丁叠进去——
`extract-pfs.mjs` 按参数顺序折叠（后给的覆盖先给的），所以：

```bash
node ports/artemis/tools/extract-pfs.mjs -o .test/artemis-official-async/game-chs \
  .test/test-games/Artemis/root.pfs \
  .test/test-games/Artemis/root.pfs.000 \
  .test/test-games/Artemis/chs_patch_official.pfs
```

（`root.pfs.000` 是 `root.pfs` 的**续包**不是补丁——本作的图都在它里面——所以两者并列传，
补丁排在最后。）

**必须在部署时让两个树用不同的 `SAVEPATH`。** `SAVEPATH` 是 IndexedDB 缓存的键，
日文树与中文树共用同一个键时，引擎会把日文那棵树缓存的字节喂给中文部署——
即 9.x 那类「改了没生效」的静默故障。故中文部署用
`--savepath artemis_e1ca47e694_chs`（`--asyncify-url` 也要指向 `game-chs/`）。

**验证（三路证据）**：

| 检查 | 日文树 | 中文树 |
| --- | --- | --- |
| `pc/ja/title/logo.png` | 122,608 B / `40bb03f1…` | 166,780 B / `96e2ffb9…` |
| `script/あすみ-01.ast` | 111,230 B / `174df445…` | 103,725 B / `8af96494…` |
| `script/共通-01.ast` 的汉字数 | 1,065 | **2,169** |

最后一行是决定性的一条：同一句 `{"savetitle", text=…}` 在日文树里是
`本日より新ガチャ開催です`，在中文树里是`今日起新卡开抽`。
（另：`pc/ja/save/char.ipt` 只有一个补丁带，属该补丁自己的界面资源。）

部署 `.../deploy-chs/web/artemis.html` 后：canvas **2251×1351**、`calledRun: true`、
**110 个 `/game-chs/` 请求**、非黑像素 **2,851,932 / 3,041,101 = 93.780%**、
`maxLum 255`；ASCII 亮度图能看出标题 logo 带、人物立绘、对话框与右侧菜单。

## 十三、官方标签面：从二进制里读出来的 101 个，不是文档里的 187 个

**目的**：把「artc 还缺哪些标签」从猜测变成可核对的事实，好决定移植工作量。
此前 `tag-coverage.mjs` 拿 `Docs/tag/*.txt` 当权威，那个口径**两个方向都错**。

### 权威在哪

`libartemis.so`（arm64）的动态符号表里有 **`artemis::CArtemis::Command<Name>`，一个标签一个非虚方法**。
`CArtemis::CommandTag` 是每个 `e:tag{...}` 唯一的入口，它按标签名 switch 到这些方法。
所以这张符号表**就是**标签面，不是它的近似。`ports/artemis/tools/tag-oracle.mjs` 把它读出来并与 artc 对表。

### 文档口径错在哪

- **高估**：`Docs/tag/` 187 个文件名，但 24 个名字在整个引擎里根本不出现，
  且若干是「一个文档讲一个家族」。真实面 = **101 个**。
- **对改名瞎**：文档叫 `font_close.txt`，二进制里注册的字面量是 **`"/font"`**。
  拿文档去 diff 会报「缺 font_close」，而引擎其实以另一个拼写拥有它。

### 只有二进制才看得见的三件事（每件都改变结论）

1. **闭合标签带前导斜杠**：`CommandFontClose` → `"/font"`，同理 `/ruby`、`/link`、
   `/chgmsg`、`/tweenset`、`/voice`。
2. **`seton*`/`delon*` 不是逐个方法**：只有一个 `CommandSeton` 和一个 `CommandDelon`，
   内部按后缀 switch。所以 artc 显式表里没有某个 `setonX`，**不等于**缺口。
   `tag-oracle.mjs` 为此单独报前缀家族，并去读 `lua_tags_input.cpp` 里 artc 自己的
   前缀注册表（早期只读派发表，会误报 `prefix registry: NO`，把整个家族算成缺口）。
3. **方法大小即真实行为**：`CommandTrophy` / `CommandIme` / `CommandLua` / `CommandVibrate`
   都是 **4 字节**——空桩。「实现」它们意味着**有意地什么都不做**。

### 结果：artc 缺 25 个 → 补掉 2 个后剩 23 个

除空桩（`ime` `lua` `vibrate`）与平台面（`openbrowser` `statusbar` `purchase`
`callnative` `httpget` `avoid` `debugprint` `debugreload` `voice` `/voice` `uidel` `unison` `slider`）之外，
**真实游戏确实用到、artc 却没有的有两个**，本轮已补：

| 标签 | 官方实现 | 真实游戏调用点 | 改法 |
| --- | --- | --- | --- |
| `skip` | `CommandSkip` `0x52613c` size `0x3e0` | `system/adv/autoskip.lua:12,27,37,66` `e:tag{"skip", allow="1/0"}`；另有 `:25,48,64` `e:tag{"exec", command="skip", mode="0"}` | 新增 `TagSkip`，**镜像 `automode` 的形状但不与它合并**：`skip_*` 是独立状态（框架分开驱动二者），`SetSkipMode` 触发 `commandskipin`/`commandskipout` 命名事件（`setoncommandskipout` 经 `TagGenericSetOn` 存的就是这个键名）。**顺带修掉一个静默洞**：`TagExecAutoMode` 原先 `if (m["command"] != "automode") return false;`，让 `command="skip"` 变成 no-op —— 框架以为跳停已关，实际跳过模式还在跑 |
| `file` | `CommandFile` `0x556f54` size `0xa3c` | `system/adv/fileio.lua:291` `deleteFile(path)` → `e:tag{"file", command="delete", target=path}`；调用者 `system/adv/fsave.lua:308-311` `sv.deleteno()` | 新增 `artc::DeleteSaveFile(directory, file)`，**复用 `SavePath` 的同一套 `..` 逃逸防护**，否则这个标签就是一个任意文件删除原语。只实现 `delete`；`wasm_sync*` 有意不实现 |

补完 `tag-oracle.mjs` 报 `artc registered: 104 names`，缺口从 25 降到 **23**，
且剩下 23 个里没有真实游戏用到的（`skip`、`file` 是仅有的两个，现已消失）。

`skip` 的语义按 `Docs/tag/scenario/skip.txt` 是 `allow`（0/1 是否允许跳过）与
`unread`（0/1 是否跳过未读段落，`1` 表「跳过到已读处为止」）。
`file` 的命令族是 `copy` / `delete` / `clear_cache` / `wasm_sync` /
`wasm_sync_add_persistent` / `wasm_sync_delete_persistent`（后三个正是官方
**WASM SYNC** 模式的配置入口，asyncify 模式不需要改脚本，故我们这条路不必实现它们）。

### 验证

`tests/save_regressions.cpp` 新增 2b) 用例（六个断言）：删除已有槽位、删完读不回来、
**再删一次要返回 false**（让调用者能区分「删掉了」和「本来就没有」）、`../` 逃逸被拒
**且外部文件真的还在**（否则「通过」只是因为本来就没东西可删）、空目录名返回 false。

构建器：该机无原生工具链且有 zlib 依赖，用 emscripten 单编该用例跑通
（`em++` 而非 `emcc`——C++ 驱动不对会报 `undefined symbol: std::__2::cerr`；还要带 `-lz`，否则 `undefined symbol: crc32`）：

```bash
cd .ref/artemis-compat
em++ -O1 -std=c++17 -I src tests/save_regressions.cpp src/util/save_storage.cpp \
     -lz -sNODERAWFS=1 -sALLOW_MEMORY_GROWTH=1 -sEXIT_RUNTIME=1 -o /tmp/artc-t2/save_reg.js
node /tmp/artc-t2/save_reg.js
```

结果：新用例六个断言全过。仍报 `FAIL: fork` 与 `FAIL: oversized write fails cleanly`
各一条 —— 那是该文件里**既有的 POSIX-only 用例**（`fork`、`RLIMIT_FSIZE`），
Emscripten 本来就模拟不了，与本轮改动无关。

集成侧另有一路证据：当时用 `web-deploy.sh`（**该脚本已随宿主路线一起退役删除**）重建后
真实游戏仍正常启动（canvas `2252x1352`、`artc_android_boot(2252x1352) -> 1`、
`viewport: 2252x1266`、非黑像素 **93.733%**、`maxLum 255`）。注意产物 `.wasm`
**无 name section、无 DWARF**，`grep` 在里面找不到新符号 —— 这类改动只能靠行为或原生测试证。

### 复现

```bash
node ports/artemis/tools/tag-oracle.mjs           # 面 + 缺口
node ports/artemis/tools/tag-oracle.mjs --sizes   # 逐个大小，标出空桩
node ports/artemis/tools/tag-oracle.mjs --json .test/tag-oracle.json
```

## 十四、动画缺陷的根因：一个 `__ANDROID__` 把 `Renderer` 的**类布局**劈成了两半

用户原话：「动画不对……artemis-compact 在安卓端的处理是正常的，在 web 端就不正常了，
可能是 wasm 里有什么地方不一样，尝试逆向这一部分分析一下。」

先按平台差异排掉了三条**错误**假说（都实测过，不要再回头查）：

| 假说 | 实测 | 结论 |
| --- | --- | --- |
| mailbox 唤醒通道死了 | `checkMailbox` 计数 **188/s** | 错。Chrome 151 有 `Atomics.waitAsync`（`waitAsyncPolyfilled=false`），唤醒走 `__builtin_wasm_memory_atomic_notify`（`thread_mailbox.c:93-110`），**不发 postMessage**，所以 `cmd4/s 0` 是正常现象 |
| pump 频率是帧率瓶颈 | 兜底定时器 `FALLBACK_MS` 250→8，`pumps/s` 4→67，`cf/s` **仍是 2** | 错 |
| drain 很贵 | 同步计时：每秒 256 次、共 10 ms、单次最大 4 ms | 错 |

**方法论坑（踩了两次，务必记住）**：`draw[N]` 与 `cf/s` 都被重绘门控
（`native_activity.cpp` 的 `if (drawn_rev != Revision() || animating)`）挡住，
静态标题页上它们量的是「门开了几次」，**不是循环帧率**。曾据此误判「引擎只有 2 Hz」。
要测循环必须在 `while (g_state.running)` 里直接埋计数 —— 埋完之后实测 `loops≈32/s`，
`block`（Clear+Draw+Present）在 5.7–203.5 ms 间抖动。

### 根因

`src/render/renderer.h` 把四个成员用 `#if defined(__ANDROID__)` 包起来：

```c
private:
#if defined(__ANDROID__)
    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLSurface surface_ = EGL_NO_SURFACE;
    EGLContext context_ = EGL_NO_CONTEXT;
    void *window_ = nullptr;
#endif
```

而 `CMakeLists.txt` 的 `artemis-web-android` 目标**只给 `renderer.cpp` 加了**
`__ANDROID__`（`:246-247` `set_source_files_properties(... COMPILE_DEFINITIONS "__ANDROID__=1")`，
意图是让 `audio.cpp` 留在非 OpenSL 路径上）——

```cmake
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/render/renderer.cpp
    PROPERTIES COMPILE_DEFINITIONS "__ANDROID__=1")
```

**但 `native_activity.cpp:41` 也 `#include "render/renderer.h"`**，而它的编译行里没有这个宏
（`flags.make` 只有 `CXX_DEFINES = -DARTC_HAS_GLES=1`）。于是同一个类在两个翻译单元里
**布局不同**：`renderer.cpp` 看到 4 个句柄（+32 字节），`native_activity.cpp` 看不到。

致命的是 `SwapPaced()` / `Ready()` / `Width()` / `Height()` 都**定义在类体内**（天生 inline），
每个 TU 各发射一份弱定义，链接器留一份给所有调用者。`native_activity.cpp` 里
`artc::Renderer renderer;` 按**小布局**分配对象，`renderer.cpp` 的 `Init()` 按**大布局**写入。

在 **wasm32** 上实测（4 字节指针、12 字节 `std::string`）：

| | 无句柄（`native_activity.cpp` 看到的）48 字节 | 有句柄（`renderer.cpp` 看到的）64 字节 |
| --- | --- | --- |
| 0 | `ready_` | `display_`（4 B） |
| 4 | `width_` | `surface_` |
| 8 | `height_` | `context_` |
| 12 | `vp_x_` | `window_` |
| 16 | `vp_y_` | `ready_` |
| 17 | — | `swap_paced_` |
| 20 | `vp_w_` | `width_` |
| 36 | `last_error_` | — |

两个后果：

1. **inline 访问器读的是小布局的偏移，而那些偏移现在落在句柄内部**。
   `kPseudoDisplay = (void*)0xE6400001`、`kPseudoSurface = (void*)0xE6400003`，
   小端字节序是 `01 00 40 E6`：

   - `ready_`@0 读 `display_` 的**第 0 字节** = `0x01` → 恰好为真，所以渲染看起来没坏；
   - `swap_paced_`@1 读**第 1 字节** = **`0x00`** → **永远 false**；
   - `width_`@4 / `height_`@8 读到的就是 `surface_` / `context_` 的整数值 → 垃圾
     （它们落在 `g_state.win_w/win_h`，而这两者无人读，所以没暴露出别的症状）。

2. **`Init()` 会越界写 16 字节**（64 − 48）到栈上那个按小布局分配的对象之后 ——
   在 `native_activity.cpp` 里紧随其后的是 `booted` 与 `drawn_rev`（**重绘门控的 revision**）。

`swap_paced_` 恒假 → `native_activity.cpp` 的收尾

```c
if (!drew || !renderer.SwapPaced())
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
```

**每一帧都多睡 16 ms**，压在 14 ms 的帧耗时上 → 30.6 ms/帧 ≈ **32 Hz**，
而 Android 上 `eglSwapInterval(1)` 成功、`swap_paced_=true`、由 vsync 背压 → 60 Hz。
**这正是「安卓正常、web 不正常」**：设备端编译器对整个 TU 集合都定义 `__ANDROID__`，
布局一致，什么问题都没有。至于「残影/帧错乱」的观感，除帧率减半外，
还有第 2 条越界写踩到 `booted` / `drawn_rev` 的可能。

定位线索是这个**自相矛盾**：`Init()` 打印 "eglSwapInterval(1) applied"，
而同一个对象的 `SwapPaced()` 在帧循环里恒为 `false` —— 一个对象给出两个答案，
只可能是两个 TU 对类的认识不一致。

### 修法

1. **`src/render/renderer.h`**：四个句柄改成不透明指针，**不依赖任何宏**，
   让类在**每个**翻译单元里布局一致。这是修根因，也是「把差异搬回 artemis-compat」的正解 ——
   设备端本来就是这个布局。

```c
    void *display_ = nullptr;
    void *surface_ = nullptr;
    void *context_ = nullptr;
    void *window_ = nullptr;
```

2. **`ports/artemis/build/android-shim/src/shim.cpp`**：`__wrap_eglSwapInterval` 原来
   **故意 `return 0`**（回落到宿主自计时）。这本身是 web 与设备的第二处行为分歧：
   设备报告成功并靠 **swap 阻塞**获得 vsync，web 端报告失败、由引擎每帧 `sleep_for(16ms)` ——
   那是个**固定睡眠**，与显示时钟互相拍频（`16 ms 睡眠 + 14 ms 工作 = 30.6 ms`）。
   现在如实报告「已应用」，并由 `__wrap_eglSwapBuffers` 承担 vsync 的角色：`commit_frame()`
   之后按绝对时间表补足到下一个 1/60 s 边界。按绝对表而非固定睡眠，所以帧率不会漂；
   迟到超过一个周期时**重排时间表**而不是追赶，否则会喷出一串连帧、看起来就是一次卡顿。
   `sleep_for` 在 Emscripten pthread 上落到 `emscripten_thread_sleep`，
   会顺带服务 queued-call 队列并 futex 等待，所以引擎线程停在这里时主 runtime 线程仍在抽干代理的 GL 工作。

### 验证

改变**只有** `renderer.h` 的一处布局（外加上面那处 vsync 语义），症状随之一并转好：

| 指标 | 修前 | 修后 |
| --- | --- | --- |
| `SwapPaced()` / `paced=` | **0**（`Init` 却打印 "eglSwapInterval(1) applied" —— 自相矛盾就是线索） | **1** |
| 动画时 `loops/s` | 16–17（`block≈500ms`、`sleep≈500ms`） | **61**（`block≈1000ms`、`sleep=0.0ms`） |
| 静态时 `loops/s` | 32（`sleep≈920ms`，每帧多睡 16 ms） | 32（门控关着时确实该睡） |
| `npm test` | — | **416 tests / 79 suites / 411 pass / 0 fail / 5 skipped** |
| `npm run check:layout` | — | layout ok |

诊断用的 `loopdbg` 埋点已全部移除，产物里 `grep loopdbg` 为 false。
`fix` 当时同时进了 `ports/artemis/engine/0001-web-host.patch`（现 15 个 `diff --git`；
**这份 patch 已随宿主路线退役删除**）。

**遗留**：`initdbg` 式的 `LOGI` 走 shim 的 `__android_log_print`（只写 stderr），
而 `Log(kLogInfo, …)` 走引擎 sink —— 两者都能到页面，但
**worker 的 `printErr` 过滤器是白名单**（当时的 `artemis-web-android-worker.js`，
产物名也一并退役了），新加的诊断字符串必须先加进那个正则，否则「没有输出」会被误读成
「代码没跑」。这一条是本轮最容易骗到自己的陷阱。

## 十五、官方 SDK 入库：一个被推翻的决定

此前对官方 SDK 的裁定是**「专有，不得入仓」**——`official-web-deploy.py` 的 docstring 写着
"nothing from the SDK may be committed"，README 写着 not redistributable。这个决定现在**推翻了**，
依据是用户原话：

> 直接将之前使用官方wasm的那个版本推送到github上，暂时不考虑法律问题

推翻之后新增 `ports/artemis/vendor/artemis-sdk/`（24 MB，9 个文件）：

```
Copyright.txt                                   (20717 B)
WebAssembly/artemis.{html,js,wasm}              (3912 / 264298 / 2682787)
WebAssembly/asyncify/artemis.{html,js,wasm}     (3912 / 276695 / 5998665)
Android/arm64-v8a/libartemis.so                 (8149024)
Android/armeabi-v7a/libartemis.so               (7395052)
```

**布局刻意镜像官方 SDK 本身**，所以 `official-web-deploy.py` 能把它原样当 `--sdk` 用，
不需要任何适配代码。注意 plain 版的三个文件**直接放在 `WebAssembly/` 下**，没有 `plain/` 子目录
——官方 SDK 就是这样。

九份文件都 `cmp` 过与 `.ref/artemis` 对应文件**逐字节相同**。

### `.gitignore` 必须开一个口子

`ports/**/*.wasm` 这条规则会吃掉 vendor 里的 wasm（还有 `ports/**/out/`、`ports/**/*.o`
分别吃掉 `reverse/out/` 与 `reverse/build/*.o`）。已在 `.gitignore` 末尾加了
`!ports/artemis/vendor/**` 并写明理由：这些是**移植的输入**而不是产物，
`ports/artemis/reverse/` 的逆向配方正是拿这些字节做校验，`official-web-deploy.py` 也拿它当
`.ref/artemis` 的替代品。

### 为什么值得承担

两条：**可复现**（一个 clone 就能跑起官方 wasm 那条路线，不必先向 Mikage 申请授权）与
**可核对**（逆向脚本声明的字节与官方发布一致，否则那些声明无从检查）。

代价同样明确：这个 GPL-3.0-or-later 仓库里现在有专有二进制。这是**已知的、有意承担的风险**，
不是疏忽。`default_sdk()` 的逻辑是「存在 licensed `.ref/artemis` 就用它，否则退回 vendor」，
所以有授权的使用者仍然走自己的树。

## 十六、逆向：把官方二进制变成可读、可校验的源码

用户要求「把官方sdk的wasm和andriod全部逆向成伪C代码并**确保能够正确构建出同样二进制文件**」。
「确保」不是声明，是**检查**——所以 `ports/artemis/reverse/` 里每一半都带一个会失败的断言。
用法与产出见该目录的 [`README.md`](../ports/artemis/reverse/README.md)。

```
fetch-wabt.sh   （一次性）wabt 1.0.42    — wasm 侧
fetch-zig.sh    （一次性）zig 0.16.0     — so 侧
reverse.sh      wasm | so [arch] | so-all
rebuild.sh      wasm | so [arch] | all   ← 校验；非零退出即断言失败
```

### 两条线问的不是同一个问题

| | 输入 | 产出 | 断言 |
| --- | --- | --- | --- |
| **wasm** | `artemis.wasm` 2.68 MB（plain）+ `asyncify/artemis.wasm` 6.00 MB | `.wat` 48 MB / 180 MB；`wasm2c` 的 C 48 MB / 108 MB；**再链回 wasm** | `wat2wasm(wasm2wat(bin))` **逐字节相同**，那份 C **能编译**，且**链回的 wasm 与原模块行为一致** |
| **so** | `libartemis.so` arm64-v8a 8.15 MB + armeabi-v7a 7.40 MB | 9042 / 9175 个「一函数一 `.c`」 | 重建出的**导出符号面完全相同** |

两个目标都必要的第二个理由：**wasm 产物无 name section**，函数名就是 `a.b`、`a.c`、`a.aa`…；
而安卓 native 有 **13k 个真实动态符号**，是那些名字的对照表。

### wasm：往返是精确的，而这不是运气

```
OK   plain:    wat2wasm(wasm2wat(bin)) == bin (2682787 bytes, sha256 f05ae1cb25eaaff8…)
OK   asyncify: wat2wasm(wasm2wat(bin)) == bin (5998665 bytes, sha256 7ceabed28b84e768…)
```

官方产物是 stock Emscripten release build，section 布局本就规范，wabt 的 writer 能精确复现。
检查仍然要跑：一旦两侧用不同方式构建，往返立刻不再相同，脚本会**说出来**而不是静默通过。

**wabt 必须钉版本（1.0.42），理由就在这**：`.wat` 不是规范形式，换一个 wabt 可能选另一种
等价拼写，于是 `wat2wasm` 拿不回原始字节——一个与 SDK 无关的失败。

后半段断言「那份 C 是真 C」：gcc 要吃下 119 万行（plain）与 250 万行（asyncify），
产出 18.5 MB 与 37.1 MB 的目标文件。**编译得过，才让伪 C 成为「可用的源码」而不只是「可读的文本」。**

### wasm：那份 C 还能链回 wasm，并且行为与原模块一致

编译得过不等于对，所以第三条断言把环闭合：把 `wasm2c` 的 C **再链成一个 wasm 二进制**，
两个模块用同一套宿主桩实例化、共享的导出逐个以相同实参调用、比较返回值。

```
OK   plain:    agreement: 16/18, trap-kind-only: 2 -> Yc, _c, differ: 0
        of the agreements: 15 returned equal values, 1 threw the same trap
OK   asyncify: agreement: 20/66, trap-kind-only: 46 -> $c, Ad, Bd, … differ: 0
        of the agreements: 19 returned equal values, 1 threw the same trap
```

链回的二进制**不可能**与原文件逐字节相同——一个是 Emscripten release build，另一个是 clang 为
生成 C 做的 codegen——所以比较是**行为**的。两个辅助脚本补上裸模块缺的东西：
`wasm-import-stubs.py` 为 195（asyncify 198）个 import 各写一个返回零的定义（内存访问器不是桩，
它真的分配，否则第一次读内存就 fault）；`wasm-export-shims.py` 用
`__attribute__((export_name(...)))` 把每个 `w2c_*` 恢复原名，于是 differ 不需要名字表。

**plain 是强结果**（18 个里 15 个返回值相等）；**asyncify 是弱证据，脚本自己说出来**：
66 个里 46 个只是「两边都抛」。这不是配方的缺陷——两个模块的间接调用表**满度不同**
（官方 5884 项，链回的 4472 项，clang 只填从重建代码可达的槽），同一次 `call_indirect`
一侧落在有主槽位（`function signature mismatch`）、另一侧落在空槽（`unreachable`）。
两边都不返回，差异属于链接而不属于 transpile。`--strict` 可把它变回失败。

三个在 harness 里**各自都一度和「重建坏了」无法区分**的坑，动它之前值得先知道：

- **每次比较都要新实例**：导出共享同一块线性内存且会写它，复用实例会让先调用的扰动后调用的，
  结果就取决于名字的排序。
- **精确名字优先于裸符号**：链回的模块同时导出恢复后的名字和 `w2c_artemis_*`，两者 un-mangle
  成同一个 key，而调用裸符号会把实例指针当第一个实参传进去——也就是 `NULL`。
- **`__wasm_call_ctors()` 必须显式调用**：Emscripten 是从它自己的 JS glue 里调的，这里没有 glue；
  不调则一个静态初始化都不跑，包括支撑导出 shim 的那个，于是每个导出都在解引用一个清零的实例。

`emcc` 在这条断言里是**软依赖**：缺 `emcc` / `node` / 可用的 Python 3 时脚本带一条说明跳过，
`rebuild.sh` 其余部分在裸机器上照跑。链接行里的 `--export-all -sEXPORT_ALL` 是承重的：
没有它 Emscripten 会把 JS glue 没引用的东西全部 dead-strip，产物缩到 4 KB、零导出，
differ 只会报「0 shared exports」。

### 不依赖 wabt 的构建链：`C++ → em++ → wasm`

上面三条断言走的是 wabt（`wasm2wat` / `wat2wasm` / `wasm2c`）。**wabt 是转换工具，不是构建链的
一环**，所以 `ports/artemis/reverse/build-cxx-route.sh` 另立一条**完全不用 wabt** 的链：

```
bash build-cxx-route.sh asyncify   # 或 sync
```

它把 `wasm2c` 的 C 当 C++ 编（`em++` 一次调用链成 wasm），只做三处让步：

1. **198（sync 195）处 `void*` 转型**。`instance_ptr` 是 `void**`，`*instance_ptr` 因而是
   `void*`，而被调者要 `struct w2c_a*`——C 里合法、C++ 里非法。**`clang` 的 `-fpermissive`
   不像 GCC 那样把它降级成警告**，所以必须真改：`*(struct w2c_a**)instance_ptr`。
   全部落在 `wasm_tailcall_w2c_a_*` 包装器里。
2. **runtime 保持 C**。`build/wasm-rt-impl.c` 与 `wasm-rt-mem-impl.c` 当 C++ 编各有 2 个错
   （`wasm-rt-impl-tableops.inc:54` / `:80`，`table->data = calloc(...)` 与 `= new_data`
   的 `void*` → `void**`）。用 `emcc -c -x c` 单独编，别去改 wabt 的 runtime。
3. **一行桥接头**。`out/*-import-stubs.c` 里写的是 `#include "artemis.h"`，而生成的头叫
   `artemis.wasm2c.h`；放一个 `inc/artemis.h` 转发即可，不要改生成物。

结果（两个 flavour 都跑过，脚本自跑行为比对并断言 `differ: 0`）：

```
asyncify: agreement: 20/66, trap-kind-only: 46, differ: 0   → 41 852 136 B
sync:     agreement: 16/18, trap-kind-only:  2, differ: 0   → 18 182 343 B
```

**与上面 wasm2c→emcc 那一轮的数字逐字相同**——C++ 化没有引入任何回归。差别只在体积：
41.9 MB（②）vs 41.9 MB（③），都是 clang 为生成 C 做的 codegen，与官方 6.0 MB 的
Emscripten release build 不可比。**所以「必须用 wabt 才能构建」不成立**：构建链是纯粹的
`C++ → em++ → wasm`。这条链的用途是给转写成可读 C++ 的产物做验收——转写不许改行为，
改完用同一条命令复核。

排错提示：`em++` 不接受 `-fpermissive` 来代替上面的改动（会报
`cannot convert argument of incomplete type 'void *' to 'struct w2c_a *'`），
所以**验收工具是 `em++`，不是 `g++`**——`g++` 会宽容地编过，掩盖真正的类型问题。

### so：导出面逐个符号重建

```
OK   arm64-v8a:   rebuilt export surface is identical (13051 symbols)
OK   armeabi-v7a: rebuilt export surface is identical (13196 symbols)
```

共享库**无法**从反编译伪 C 逐字节重建——反编译丢类型，也没有编译器能复现 NDK release 的
指令选择。能检查的是互操作依赖的那部分：`build-so-stub.sh` 生成一个导出名与弱/强绑定都相同的
翻译单元，为目标 ABI 编成 `.so`，`rebuild.sh` 用 `llvm-nm -D` 双向 diff 两份名单。

**两个方向都要查，失败含义不同**：只在官方侧的名字 = 调用者解析不到；
只在我们侧的名字 = 这个配方**凭空发明**了一个符号，调用者可能意外绑定到它、拿到桩而非真实现。

`A`（absolute）符号两侧都剔除：它们是 `__bss_start`、`_end`、`__bss_end__` 那七个，
由链接器脚本合成，任何翻译单元都定义不出来，缺席不说明配方有问题。
它们正是原始 13058 与可比 13051 之间的**全部**差异；armeabi-v7a 的 13252 与 13196 同理。
`W`/`V`（weak 与模板 guard 变量）只比**名字集合**，不比绑定强度：官方弱、我们强，调用者一样解析得到。

### 交叉编译器：两个不行的，一个行的

- **MinGW 的 gcc 不行**：它永远链 x86-64 COFF，`-shared -fPIC` 产出的文件**没有动态符号表**，
  而且把每个弱符号改名成 `.weak.<name>.<anchor>`。两种失败都**看起来像配方坏了，其实不是**。
- **emsdk 的 clang 也不行**：它的 LLVM 只编了 `wasm32 wasm64 x86 x86-64`，
  `--target=aarch64-linux-android` 直接 `unable to create target`；目录里也没有 `ld.lld.exe` / `llvm-mc`。
- **本机没有 AArch64 工具链、没有 NDK**，`rustup target add aarch64-linux-android` 超时未完成。
  改用 **Zig**：一次下载覆盖两个 ABI，不需要 NDK。

```
zig cc --target=aarch64-linux-gnu    -shared -fPIC -O0 -nostdlib stub.c -o out.so
zig cc --target=arm-linux-gnueabihf  -shared -fPIC -O0 -nostdlib stub.c -o out.so
```

`-nostdlib` 让产物只剩那个翻译单元——导出面要看的正是它。`fetch-zig.sh` 也钉版本，
理由与 wabt 相同。

### Ghidra 两个坑

- **Ghidra 12.1.4 删掉了 `DecompInterface.getDefaultOptions()`**。调它编译不过，
  而**可见症状是** `ERROR REPORT SCRIPT ERROR: DecompileAll.java : The class could not be found`
  ——报的是「找不到类」，不是「找不到方法」，很容易往错方向查。
- **headless 不会自建项目目录**，会 `Abort due to Headless analyzer error: Directory not found`；
  `reverse.sh` 先 `mkdir`。项目路径还不能含 `.` 开头的组件。

结果 9039 ok / 3 failed 与 9173 ok / 2 failed。命名函数带得出真实签名，例如
`CommandTag__0060f8c0.c` → `void __thiscall artemis::CArtemis::CommandTag(CArtemis *this, CScriptBlock *param_1, bool param_2)`，
函数体就是那张标签注册表本身。`Windows/Artemis.exe` 是加壳的（131 函数、0 指令），
**所以授权判据用 `.so`，不用它**。

### 两种 SDK 布局都验过

`reverse.sh` 与 `rebuild.sh` 用同一套解析：`$ARTC_SDK` → 存在则用 `.ref/artemis` → 否则用
vendor。native 的位置两种树不同（完整 SDK 是 `Android/app/src/main/jniLibs/<abi>/`，
vendor 是 `Android/<abi>/`），`so_path()` 两个都试。**两条线在两套布局下都跑过，结果相同。**

## 十七、重写后的引擎：`ports/artemis/src/**` 现在是**入库源码**

§16 的产物是「可读的伪 C」。本节这一层是再往后一步：那份伪 C 被**逐个函数**改写成
artemis-compat 风格的 C++，而且改写的结果**进了仓库**——不再是逆向目录里的中间物。

### 17.1 树在哪、有多少

```
ports/artemis/src/            159 个文件，71,006 行（157 个 .cpp + 2 个 .h）
  ├── script/                 79 个
  ├── util/                   76 个
  └── core/                    4 个
ports/artemis/REWRITE.md      改写风格宪法（§1 四类锚 / §2 风格宪法 / §3 指纹清单）
ports/artemis/rewrite-names.tsv  334 行，逆向名 → artc 风格名的总表
```

`src/` 下**每个文件**都 `namespace artc`（159/159，`reverse/style-gate.sh` 断言的
`[must] namespace artc present`）。

**旧的 `.ref/artemis-official/rewrite/` 路径已删除**——产物搬进了 `ports/artemis/src/`。
文档里（或任何笔记里）再写那个路径就是错的：`ls` 它会 `No such file or directory`。
几个 `reverse/p2-*-progress.md` 里还留着旧路径，那是**当时为真的记录**，各文件开头已声明
「Paths below are the ones that were true when each note was written」——不要把那些当成现状。

### 17.2 `rewrite-names.tsv`：一张表同时是账本与验收面

列头写在文件自己的注释行里：

```
# fn<TAB>fNNN<TAB>current<TAB>target<TAB>file<TAB>depth<TAB>note
# rt<TAB>old<TAB>target<TAB>file<TAB>note
# host<TAB>old<TAB>target<TAB>note
```

| 量 | 值 | 怎么算的 |
| --- | --- | --- |
| `fn` 行总数 | **314** | `awk -F'\t' '$1=="fn"'` |
| 其中 `file` 列非空 | 204 | |
| 其中 `file` 列指向的文件**确实存在** | **157** | 逐行 `os.path.isfile` |
| 表里出现过的不同路径 | 179 | |
| `src/` 下实际文件数 | **159** | |

两个数对不上是**对的**，而且差在哪是可以逐条指名的：表里有 22 个路径指向
`render/`、`host/`、`config/` 等**尚未落地**的文件（如 `render/compositor.cpp`、
`script/lua_tag_dispatch.cpp`、`render/video_player.cpp`）。这些行的 `note` 列自己
写着证据类别——全表里 `PROVISIONAL` 19 处、`跳板` 16 处、`未重写` 12 处，
就是「名字已登记、文件还没写」的记号。
而 `src/` 里有两个文件**不出现在任何 `fn` 行的 file 列**：
`util/engine_context.h` 与 `script/value_slot.h`——它们是**边界词汇头**，
不代替任何 `fNNN`，所以没有行可以挂。**「314 行」不是「314 个函数已重写」**：
它是一张总表，含已落地的、待落的、和只登记名字的。

### 17.3 风格宪法是**可断言**的，不是倡议

`ports/artemis/reverse/style-gate.sh` 把 `REWRITE.md` §2/§3 变成 grep 断言。用法是
`bash style-gate.sh [tree]`，`tree` 默认 `<repo>/ports/artemis`（它下面的 `src/`
就是产品树），退出 0 才算过。当前实跑（本机）：

```
== style-gate: /d/online_gal/ports/artemis
ok    [w2c_]
ok    [FUNC_PROLOGUE|FUNC_EPILOGUE]
ok    [goto[[:space:]]+var_B]
ok    [i32_load|i64_load|f32_load|f64_load|i32_store|_default32]
ok    [\bUNREACHABLE\b]
ok    [TRAP\(]
ok    [wasm_rt_]
ok    [dep] render -> script            （render 不许 include script）
ok    [dep] script.h -> render
ok    [dep] src/{script,render,audio,pack,config,util,input,save} -> core (host|jni only)
ok    [must] namespace artc present
WARN  bare pool-ish literal (name it):
      util/allocate_string_buffer.cpp:42  kCollectorStepIndexAddr = 856940u
      util/allocate_string_buffer.cpp:50  kThrowTypeWordFirst   = 798992u
      util/allocate_string_buffer.cpp:51  kThrowTypeWordSecond  = 798956u
      util/allocate_string_buffer.cpp:52  kThrowTypeInfoAddr    = 799044u
      util/dispatch_resource_step.cpp:42  kF3785ConstFFFFFFFF   = 4294967295u
== style-gate: PASS ==
```

两道闸是分开的：**禁词根**（`w2c_`、`FUNC_PROLOGUE`/`FUNC_EPILOGUE`、
`goto var_B…`、`i32_load` 一族、`UNREACHABLE`、`TRAP(`、`wasm_rt_`——即
「一眼看得出这是 wasm2c 直译」的指纹）与**依赖方向**（render 不许 include script、
八个 `src/` 子目录只有 host/jni 才许 include `core/`）。WARN 只是警告、不失败，
除非 `STYLE_GATE_STRICT=1` 把它升格。

### 17.4 表里的名字**不是**可互换的，这正是那个未跟踪的 sync 树的由来

**这条不是理论，是构建脚本里写死的注释**（`build-bridge-route.sh:79-82`）：
*wasm2c 按函数在**它那个**模块里的位置编号，所以同一个 `fNNN` 在两个 flavour 里
是两个不同的函数；splicing 混用**不会报错**——四道闸只看 ABI 形状，不看那段代码是谁的。*
换句话说，**错树是会静默通过的**，只能靠「选树」这一步本身来防。

因此：

| flavour | 树 | 位置 | 是否入库 |
| --- | --- | --- | --- |
| asyncify | 159 文件 / 71,006 行 | `ports/artemis/src/` | **是**（`git ls-files` 159 条） |
| sync | 285 文件 / 38,414 行，`rewrite-names.tsv` 284 `fn` 行 | `.ref/artemis-official/rewrite-sync/` | **否**，`.gitignore:34` 的 `.ref/` 吃掉 |

`build-bridge-route.sh` 里的选择是**显式**的、且刻意不互相回退
（`ARTEMIS_REWRITE` 可以覆盖，但默认分支绝不跨过去）：

```bash
REWRITE_TREE="${ARTEMIS_REWRITE:-}"
if [ -z "$REWRITE_TREE" ]; then
  if [ "$FLAVOUR" = "asyncify" ]; then
    REWRITE_TREE="$REPO_ROOT/ports/artemis"
  else
    REWRITE_TREE="$REPO_ROOT/.ref/artemis-official/rewrite-sync"
  fi
fi
```

脚本里紧挨着的注释就是理由：*«wasm2c numbers functions by position in ITS module, so one
fNNN is a different function in each, and splicing the wrong one in is silent -- every gate
still passes, because the gates check the ABI shape and not whose code that shape is.»*

**结论：「sync 那条线不能从一个 clone 里重建」**——这是有意的现状，不是遗漏。
sync 侧的笔记（`reverse/p2-sync-rewrite-progress.md`）自己第一行就写着 Not tracked。

### 17.5 重写产物怎么进构建：`rw-splice.py` 的「产品 + 桥」

`reverse/rw-splice.py` 是这一步的机制（用法
`python rw-splice.py <combined.c> <rewrite-root>`）：对每个在表里登记、且 `src/` 下
真有文件的 `fNNN`，它把生成层里 `w2c_artemis_fNNN(...)` 的定义**换掉**——

1. 原位放入产品本体（verbatim；产品自己按 artc 风格命名，并自己用
   `EnterFrame()`/`LeaveFrame()` 框住）；
2. 再留一个**同名同签名的纯转发 thunk** 指回产品。

第一个产品之前还会注入一段 `namespace artc` 原型块（类型按生成层的拼法写），
好让产品之间不论定义顺序都能互相调用。**生成层的每个调用点、每条 `static` 前向声明
都不必改名就仍然成立**，而函数体已经是人手写的。

产品个数从构建日志里可直接数出来，例如批 3 那次的
`rw-splice: 59 product(s) spliced`（58→59）。计数本身也是可复现的：
`instrument-profile.py` 数的就是产品里那行 `  EnterFrame();`，索引取自
`// ---- rewrite: f<NNN>` 标记——所以调用总数与生成体逐一对得上，含产品之间的互调。

### 17.6 合入锚：四类断言，缺一条就退回

`REWRITE.md` §1 写死了每批必过的四类锚，且都指向**现成的、可重跑的命令**：

| 锚 | 判据 | 工具 |
| --- | --- | --- |
| 行为锚 | 真实游戏稳定帧，双侧像素级一致（**证 ABI：同输入同输出**；松散树不供数，渲染保真不由此锚承担——见 §18.4 的更正与 packs 那条线） | `ports/artemis/tools/real-game-test.mjs --hashlist` |
| 调用锚 | 剖析计数逐位相同（`calls` + `first-run order` 双同） | `instrument-profile.py` / `profile-report.mjs` |
| 模块锚 | `wasm-diff` 66/66 不回退；`npm test` 416 绿 | `reverse/wasm-diff.mjs` |
| 保真锚 | R0/R1 产物反映射后 α-IDENTICAL | `reverse/fidelity-alpha-diff.py` |

**帧锚的判据本身也被修过一次，这一点要留着**：早期把 `finalHash == '550f7464'` 当过滤器，
后来发现窗口尺寸漂移会让好签名变成 `a2e1dcf5`（lit 97.11% → 87.5%），于是判据改成
**请求数（87 vs 83）+ lit 百分比 + frames 数**，hash 只在同一 session era 内作辅助。
「hash 不是判据」这条现在是 `REWRITE.md` 里的常用工序要点，不是脚注。

> **一个观察口径的坑，与 9.x 那几轮同源**：`profile` 的 JSON 必须与它实测的那份
> `bridge.wasm` **同目录、同批次**产出。单看 mtime 相邻会认错父——批 3 就踩过：
> 一份 JSON 里装的是批 2 的值，而旁边的树只有 53 个产品标记，于是报出
> 「155×」与「29,226→2」两个假红。**判据是「JSON 里出现的批内产品计数，要在同目录产物里数得出
> 对应数量的 `^// ---- rewrite: fNNN` 标记」，不是时间戳。**

## 十八、官方 SDK 直部署，与「重建模块能不能顶上去」的兑现

§15 把 SDK 入库、§16 把官方二进制变成可读源码、§17 把重写结果入库。本节是合起来的那一步：
**用官方 SDK 直接部署真实游戏**（免编译、免 wabt），并且**证明自建模块能取代其中的引擎**。

### 18.1 一条不依赖任何专有二进制的部署路线

`ports/artemis/build/official-web-deploy.py` 是这条路线的固化：

| 参数 | 作用 |
| --- | --- |
| `--game`（必填） | `extract-pfs.mjs` 产出的松散资源树（必须含 `system.ini`） |
| `--out`（必填） | 部署目录 |
| `--port` | 默认 4700 |
| `--sdk` | 官方 SDK 根；不给走 `default_sdk()` |
| `--flavour` | `asyncify`（默认）或 `plain` |
| `--engine-wasm` / `--engine-js` | **用自建模块顶掉官方引擎**（18.3 要用） |
| `--hashlist` | 顺带写 `hashlist.txt`，供帧锚比对 |

`default_sdk()`（`:76`）的选择顺序是：`.ref/artemis/WebAssembly`（完整官方 SDK，若本机有）
→ 否则 `VENDORED_SDK`，即 `ports/artemis/vendor/artemis-sdk/WebAssembly/`。
**两种布局都认**——这就是 §15 那个入库决定的价值：一台没有官方 SDK 的机器上照样能部署。

`copy_sdk_build()`（`:291`）在把候选模块放进去时，会读候选自己声明的
`declared_initial_memory()`，需要的话**改页面**而不是改模块：往 `artemis.html` 里那个
minified 的单一 `Module={...}` 字面量中插入 `Module["INITIAL_MEMORY"]`
（`:418` 的 `set_initial_memory()`）。原因是胶水在创建 buffer **之前**就读这个键，
不插进去它就退回自己的 `||16777216` 兜底，于是模块声明的最小值不被满足、
**实例化在内存导入上失败**——那个报错看起来像 wasm 的问题，其实是页面的问题。
这段逻辑只在候选需要 > 16 MiB 时才动手。

### 18.2 两种 flavour 的模块**不是**同一个东西

```
ports/artemis/vendor/artemis-sdk/WebAssembly/
  ├── artemis.wasm              2,682,787 B   ← plain（同步）
  ├── artemis.js                  264,298 B
  └── asyncify/
        ├── artemis.wasm        5,998,665 B   ← asyncify
        └── artemis.js            276,695 B
```

（**没有 `plain/` 子目录**：plain 的那两个文件就直接平铺在 `WebAssembly/` 下，
这正是官方 SDK 自己的布局，vendor 是照抄的。）

asyncify 模块比 plain 大一倍多，因为异步化把每个可能挂起的函数都拆成了状态机。
它的导入段是 **198 个函数 + 1 个内存 `a.a`，全部挂在模块 `a` 下**——即 199 条导入。
胶水把 `Mc` 映射成 `___wasm_call_ctors`（`Module["___wasm_call_ctors"]=Module["asm"]["Mc"]`），
这个细节在 18.3 会变成一条必须满足的约束。

**为什么线上只有 asyncify 能跑**：资源包约 6 GB，而 wasm32 的线性内存上限是 4 GiB，
何况一个浏览器标签页实测只能 commit 约 1.75 GB——所以「把整包读进内存」的模型在这台
游戏上必定破产。plain 模块**没有** `_emscripten_wget_data`
（`asyncify/artemis.js` 里 1 处，plain 的 `artemis.js` 里 **0 处**），只有 asyncify
那条带按需取数。这不是引擎缺陷，是**加载模型的限制**：官方 SDK 自带的样例在 plain 下
启动得好好的——这把 §12.1 的结论又确认了一遍。

### 18.3 替换台：四道闸，外加一台真浏览器

「自建模块能不能取代官方模块」在 `ports/artemis/reverse/wasm-replace-test.mjs`
里被拆成四道闸：

| 闸 | 问的问题 |
| --- | --- |
| Gate 1 · import face | 官方胶水自己的 import 对象，能不能构造它？ |
| Gate 2 · export face | 官方胶水要读的短名，它都导出吗？ |
| Gate 3 · behaviour | 与官方模块**逐个导出**对打，结果是否一致？（`wasm-diff.mjs`） |
| Gate 4 · host wiring | 它真的伸手去用宿主了吗（GL 等）？ |

用法 `node wasm-replace-test.mjs [--official <wasm>] [--glue <js>] [--strict] [--json] <candidate.wasm>`，
四条结论字符串是固定的：`viable replacement` / `faithful but not a replacement` /
`would load but would not render` / `not a replacement`。判定是
`dropIn = g1.pass && g2.pass`、`runnable = dropIn && g4.pass`。

`build-bridge-route.sh` 是让「自建产物过四闸」成立的那条构建线，两个 flavour 都过：

| | asyncify | plain |
| --- | --- | --- |
| Gate 1 import face | 198/198 全消费，内存是导入的 | 195/195，内存导入 |
| Gate 2 export face | 67/67 | 19/19 |
| Gate 3 behaviour | agreement 20/66，differ 0 | 16/18，differ 0 |
| Gate 4 host wiring | 207 imports，GL 65/65 | 204，GL 65/65 |

四件事必须**同时**成立，每一件最初都像死路（`reverse/README.md` 有完整推导）：
宿主 ABI 可以**导入**而不是打桩（`import_module("a")` + `wasm-import-stubs.py --forward`）；
内存可以**共享**而不是独占（`-sIMPORTED_MEMORY -Xlinker --import-memory=a,a`，
且 `(uint8_t*)0` 是空指针常量、clang 会把对它的解引用折掉，值必须过一层 `volatile` 才不透）；
两个租户需要**可分辨的分界**（访客把自己的分配器读内存描述符的 `size` 当堆顶，
返回整块就等于告诉它「本模块的静态区与栈都归你」，所以访问器只报一个**栅栏**、
布局落在栅栏之上——在官方那 16 MiB 里划栅栏会把访客饿死，它跑到距栅栏只剩四字节）；
以及 **`Mc` 不只是个导出**——胶水把 `Mc` 当 `___wasm_call_ctors` 调，所以模块启动跑的
是**那个 shim**，每个 shim 必须惰性初始化，否则实例指针一直是 null，第一次访问就
`(*NULL).size` 崩掉。

**尺子必须与产物同源**（`rebuild.sh` 里的注）：契约要来自**同 flavour 的官方产物**。
plain 导出表是 `Ic`、196 个导入；asyncify 是 `Lc`、199 个。拿一个当另一个的尺子，
会把没变的东西量成「不再保真」——这个错误是被 plain 那条对照自己抓出来的。

**验证**：`reverse/rebuild.sh`（用法 `{wasm|so [arch]|all}`）跑完整往返。它的
`verify_replacement_verdict()`（`:187`）把「不许漂移」写成断言：wabt 往返那条产物
**必须**停在 `faithful but not a replacement`——既不能在测试器里变成 drop-in（那说明
测试器或构建变了），也不能掉到「连逻辑都不保真」（那是真回归）。这条断言当场就有意义：
本机 `out/` 里那份 `asyncify-rebuilt.wasm` 正是 wabt 往返产物，`g1.pass=false`、
`g2.missing=['Lc']`、`dropIn=false`、`logicFaithful=true`——**而 bridge 那条线过四闸**。
两个结果并存才是对的，它们回答的不是同一个问题。

`wasm-diff.mjs` 的读数要读全，不能只读一个数：

```
agreement: 20/66, trap-kind-only: 46, differ: 0
  of the agreements: 19 returned equal values, 1 threw the same trap
```

即 66 个共享导出**全部有交代**——20 个取值一致，46 个「两边都 trap」（重建模块的
间接调用表比官方那份稀疏，落到空槽），**differ 为 0**。加 `--strict` 会把
trap-kind 也算失败，所以默认口径下它只作提示。

### 18.4 最后一公里：真实游戏里的**稳定帧**

过闸只证明「装得上」，不证明「跑得起」。那要用真标题、真页面、真浏览器来证——
`ports/artemis/tools/real-game-test.mjs` 做的就是这件事：

```
node ports/artemis/tools/real-game-test.mjs --game <松散树> --engine <candidate.wasm>
```

它用 `official-web-deploy.py` **并排部署**官方与候选两份，各起一个服务
（`--port` 默认 4710，对照组用 `port+1`），通过 CDP 驱动**真实 Chrome**，
在 `rAF` 里采样，比较**稳定帧**。常用旗标：`--flavour`（默认 asyncify）、
`--port`、`--cdp-port`、`--wait`（默认 30000）、`--hashlist`、`--json` / `--json-out`、
`--savepath`、`--keep`。退出码 0 的定义是**两条同时成立**：候选渲染出了非黑画面，
**且**它确实去取了游戏资源——只渲染一个欢迎页、没读游戏，不算跑起来了。

**结果**：自建 asyncify 模块的稳定帧与官方构建**像素级相同**——
`550f7464`，87 个文件被服务。⇒「一个从 wasm2c 输出重建的模块可以成为官方胶水下的
**即插即用件**」不再是推断：它在真浏览器里与官方构建**同输入同输出**。

> ⚠ **但这次像素一致不能当渲染证据，2026-09-29 实测更正。** 引擎要的资源名**全部无扩展名**
> （`image/bg/black`、`debug/lua/index.lua`、`font/extra`…），而这份松散树 29,224 个文件里
> **零个无扩展名**（15,401 `.ogg` / 7,867 `.png` / 5,647 `.sli` / 218 `.ast` / 57 `.lua` / 14 `.tbl`），
> `serve-range.mjs:87-98` 又对不在磁盘上的路径直接 404、**没有扩展名回退** ⇒ 那 24 条游戏请求
> **全 404**：实测 `netCounts {200:3, 404:24}`、`gameBytesByStatus {404:0}`、控制台 24 条
> `Download file <name> failed.`（`.test/team-artemis/a17-out.json`），两张帧的 stats 是
> `mean 218.09→173.38`、`lit 87.5%→87.39%`、`distinct16 16`（16 个亮度桶全占、零游戏字节）
> 的**占位画面**。所以「87 个文件被服务」是**请求数**、不是供给量，exit 0 里那条
> 「确实去取了游戏资源」在松散树上只被请求数满足。**这条 bench 证的是 ABI（同输入同输出），
> 不是渲染保真**；渲染锚在下一段那条路线上，而且必须用**包**供数。

（`550f7464` 是那一次的实测值，不是永久判据：窗口尺寸一变它就会漂，
判据在 17.6 那条注里——**请求数 + lit% + frames**。此外这台台子**双侧都会 flake**：
批 3 记的 6 跑里，2 跑官方没渲染、2 跑候选 stalled，所以单跑一次不构成证据。）

**部署的那一份现在出自 runner，不再是本机构建。** artemis 没有 release（引擎对着专有 SDK
编译），所以 `ports/lib/engine-artifacts.json` 里它那两项一度描述的是本机构建——而台账的全部
含义就是「这些字节出自哪一个构建」，那一版答的是本机。pin 前移到 `9f622629f595` 后，
`build-wasm-artemis.yml` 在 runner 上产出了**新 pin** 的第一份制品，台账随即换成 runner 的
字节（`artemis-web-android.wasm` 1,151,025 B / `a1787043…`）。而且它在 runner 上**可复现**：
同一个 pin 的两次构建（run 36363984069 与 36368756255）产出的两个文件逐字节相同，js 仍是
原来的 182,842 B / `a1c03dd6…`——台账里 js 那一项没动，不是因为没人管它，而是因为两边的 js
本来就一样。这份制品随后在真实的
`.test/test-games/Artemis/`（`root.pfs` 2,690,454,559 B + `root.pfs.000` 3,355,688,890 B）
上跑过，对照 pin 之前的本机旧模块（1,147,110 B / `79dce328…`）：同一张
`android-host.html`、同一个 `?pack=/root.pfs`、同一台无头 Edge，两侧都是画布 `756x488`、
`litPct 86.90`、`signature 1acafc51`、`mean r180.12 g168.70 b170.81`，**1,106,784 个通道
字节里 0 个不同**，73 条宿主日志、`draw[901]` 的层表、1120 条控制台条目都逐字相同。
⇒ 「runner 产出的字节能跑真实游戏」在 artemis 上是量出来的，而且它换掉的那一份是同一帧。
（取字节这条路是 `ports/artemis/build/serve-range.mjs`：页面只认 `206`，所以它不能换成
`coi-server.py`。）

### 18.5 本节与 §7/§10 的关系

§7 的「原 1/2/4」之所以能从「尚未完成」里划掉，靠的就是本节这条路线：Web 宿主与渲染
在 §9.17–9.18 打通，归档供数在 §9.15–§9.16 换成 worker + 同步 XHR；本节补的是
**部署与验收**这一层——它让「跑起来了」这句话有一个可重跑、可对照、退出码有定义的证据。
而 §10 剩下的两项（音频、读缓存）恰好正是本节这套证据**测不出来**的那类工作：
它们不改变导入导出面，也不改变稳定帧，所以四道闸全绿也照样是缺的。
