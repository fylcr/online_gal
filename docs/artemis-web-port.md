# Artemis 浏览器移植：进展与验证

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

按依赖顺序：

1. **Web 宿主**（`src/host/web_main.cpp`）——仿 `host_drive.cpp` 的帧循环，但由 `requestAnimationFrame`
   驱动、输入来自浏览器事件。`drive` 已证明除渲染/音频外的整条栈可用。
2. **渲染**：`ARTC_HAS_GLES=1` + Emscripten 的 GLES2/EGL → WebGL2 canvas。Emscripten 的 GLES2 模拟
   与引擎的 GLES2 用法应当匹配，但合成器的 FBO/过渡路径需要实测。
3. **音频**：`src/audio/audio.{cpp,h}`、`pcm_ring.h` 里的 **OpenSL ES 必须重写为 Web Audio**。
   这是唯一一处「必须新写后端」而非「换编译器」的地方。
4. **归档供数**：wasm 当前经 NODERAWFS 读宿主文件；浏览器里要改成经 emscripten 的 FS 走
   VFS 范围读（`readPfsTableFrom` 那条线已就绪，需在 wasm 侧接上）。
5. **Provider**：`artemis` 目前在 `BUILTIN_PROVIDERS` 里标 `unavailable`（priority 85）。

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

`node ports/artemis/build/web-input.mjs`（headless Edge）：

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

- **音频**：`src/audio/audio.{cpp,h}`、`pcm_ring.h` 的 OpenSL ES 必须重写为 Web Audio。
  `artemis_loader.cpp` 第 9 行点明音频桥用 **`dlsym`** 命中引擎符号，**接缝是符号级的**。
  日志里 `audio(host): play track:1 sound/sysse/none.ogg loop=0` 说明调用已经到达，只是还没出声。
- **provider**：`artemis` 在 `BUILTIN_PROVIDERS` 里仍是 `unavailable`（priority 85）。
- **读缓存**：同一段载荷在一次启动里被反复取（`23540810-23624647` 读了 20 次），目前没有缓存层。
  不影响正确性，但每次都是真实的 HTTP 往返。
- **引擎日志过于啰嗦**：`[INFO] tag[trace]:` 每帧数千行，需要按宿主降级到 warning。

原 9.14 的结论（`HEAPU8.set` 是黑屏原因）**不成立**：那一步是对的，见 9.15–9.17。


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

`.ref/` 与 `.test/` 都在 `.gitignore` 里，SDK 与本作数据都不会被提交；脚本只**读** `.ref/`。
（许可：官方 `Copyright.txt` 只授权给向 Mikage 申请过的人，商业需付费——**不是 GPL，不得入仓**。）

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

集成侧另有一路证据：`web-deploy.sh` 重建后真实游戏仍正常启动
（canvas `2252x1352`、`artc_android_boot(2252x1352) -> 1`、`viewport: 2252x1266`、
非黑像素 **93.733%**、`maxLum 255`）。注意产物 `.wasm` **无 name section、无 DWARF**，
`grep` 在里面找不到新符号 —— 这类改动只能靠行为或原生测试证。

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
`artc::Renderer renderer;` 按**小布局**分配对象，`renderer.cpp` 的 `Init()` 按**大布局**写入：

- `Ready()` 读 `display_` 的**第 0 字节** → `0x01` → 恰好为真，所以渲染看起来没坏；
- `SwapPaced()` 读 `display_` 的**第 1 字节**。`kPseudoDisplay = (void*)0xE6400001`
  （`android-shim/src/shim.cpp:272`），小端字节序是 `01 00 40 E6`，第 1 字节 = **`0x00`**
  → **永远 false**；
- `Width()` / `Height()` 读 `display_` 的 4..12 字节 → 垃圾（落在 `g_state.win_w/win_h`，而这两者无人读，所以没暴露出别的症状）。

`SwapPaced()` 恒假 → `native_activity.cpp` 的收尾

```c
if (!drew || !renderer.SwapPaced())
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
```

**每一帧都多睡 16 ms**，压在 14 ms 的帧耗时上 → 30.6 ms/帧 ≈ **32 Hz**，
而 Android 上 `eglSwapInterval(1)` 成功、`swap_paced_=true`、由 vsync 背压 → 60 Hz。
**这正是「安卓正常、web 不正常」**：设备端编译器对整个 TU 集合都定义 `__ANDROID__`，
布局一致，什么问题都没有。此外 `Init()` 还会**越界写 32 字节**到栈上那个按小布局分配的对象之后，
可能波及紧随其后的 `booted` / `drawn_rev`（重绘门控），这与「残影/帧错乱」的观感一致。

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
`fix` 同时进了 `ports/artemis/engine/0001-web-host.patch`（现 15 个 `diff --git`）。

**遗留**：`initdbg` 式的 `LOGI` 走 shim 的 `__android_log_print`（只写 stderr），
而 `Log(kLogInfo, …)` 走引擎 sink —— 两者都能到页面，但
**worker 的 `printErr` 过滤器是白名单**（`artemis-web-android-worker.js`），
新加的诊断字符串必须先加进那个正则，否则「没有输出」会被误读成「代码没跑」。
这一条是本轮最容易骗到自己的陷阱。









