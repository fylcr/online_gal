# 把 asyncify wasm 转写成可读代码 — 交接文档（**已执行完毕**）

> **状态：这份交接已经被执行，本文档现在是结案记录 + 产物坐标，不再是待办。**
> 下文的目标、路线对照、tag 命名 oracle 仍然全部有效（它们是这次转写的依据），
> 但「计划这样做」的部分已经变成了「已经这样做完」。
>
> **产物现在在哪（这是接手人最需要的两条）：**
>
> | flavour | 产物 | 位置 | git |
> |---|---|---|---|
> | asyncify | **157 个产品**（`.cpp`）+ `util/engine_context.h` | **`ports/artemis/src/`**（`core/` `script/` `util/`） | **已入库** |
> | sync | **284 个产品**（`.cpp`）+ `util/engine_context.h` | **`.ref/artemis-official/rewrite-sync/src/`** | 本机未入库（`.ref/` 被忽略） |
>
> 重写规范是 `ports/artemis/REWRITE.md`（可读 C++ 的风格宪法 + 四档工序 R0~R3），
> 词汇映射表是 `ports/artemis/rewrite-names.tsv`（334 行）。
> **旧的 `.ref/artemis-official/rewrite/` 路径已删除**，不要再去找它。
> 验收脚本不变：`bash ports/artemis/reverse/build-cxx-route.sh asyncify|sync`。
>
> **还没做完的**（见 §12）：asyncify 侧的 batch-4B 剩余体、`f3744` 的切分、
> P4 长尾约 3,474 个函数进 `generated/`。sync 侧没有欠账。

---

## 1. 任务（已完成，以下是当时的定义）

把官方 Artemis 的 **asyncify WebAssembly** 逆向产物**重新写成可读的 C++ 代码**，产出放到

```
D:\online_gal\.ref\artemis-official\code
```

> ⚠ **上面这个路径是当时写的计划落点，不是最终落点。**实际执行时选择被改了两次：
> `.ref/artemis-official/code/` 成了**早期历史产物**（64 个 `.cpp`，配套规范是
> `code/TRANSCRIBE.md`，`ports/artemis/REWRITE.md` 明确写「历史产物不再动」）；
> 真正的 asyncify 产品搬到了**入库的 `ports/artemis/src/`**（157 个 `.cpp`），
> sync 产品留在 `.ref/artemis-official/rewrite-sync/src/`（284 个 `.cpp`）。
> 见文首那张表。

用户的要求（原话要点）：参考引擎 `D:\online_gal\.ref\artemis-compat` **可参考**，但它
**没有 wasm 版本**；要**尽可能细分任务并并发委托子代理**以加速；子代理模型指定为
**commandcode 提供的 mimo-v2.6-flash**；范围先做 **asyncify** 那一个 wasm。

### 一条必须先说清的事实：**只有 wabt 那条路产出的才是「正常 wasm」**

这是本任务的前提，之前没有写明，现在补上：

- **① `.wasm → .wat → wat2wasm`** 产出的是**正常的、规范的 wasm**——它与官方模块
  **逐字节相同**（sync `f05ae1cb…`、asyncify `7ceabed2…`）。要走 wabt。
- **② `wasm2c → C → emcc`** 虽然行为一致，但产物是 clang 为生成 C 做的 codegen，
  41.9 MB（官方 6.0 MB），**不是「正常 wasm」**。而且生成起点就要 wabt。
- **③ `C++ → em++`** 同理，行为一致但不是「正常 wasm」，**但完全不用 wabt**——
  这是用户要的目标形态。

**所以两句话要分开读：**
1. **「只有 wabt 构建出来的才是正常 wasm」是事实**——① 是唯一能逐字节复现官方二进制的路线，
   这是它的价值，也是它的局限：它只能**复现**，不能**构建**（它不产生新源码，
   且依赖 wabt 这个转换工具）。
2. **任务要求是：把逆向结果写成正常可读的 C++，让构建不依赖 wabt**——即 ③。
   ③ 不需要逐字节相同，**行为一致即可**（已验 `differ: 0`，见 §10）。

### ⚠ 产物的验收标准（用户后来明确了，这条改变一切）

> 「加上这句话（只有 wabt 构建才是正常 wasm），然后让其变成正常可读 C++ 代码，
> **不需要依赖 wabt 就能正常构建 wasm**。」

**要的不是一份「读起来像 C++」的文档，是一份真的能编的 C++ 代码。** 具体三条：

1. **产物是 C++**（`.cpp`/`.h`/`.hpp`），不是 C、不是伪代码。
2. **`emcc`/`clang++` 能把它直接编成 wasm** —— 调用链里**不许出现 wabt**
   （没有 `wasm2wat`、没有 `wat2wasm`、没有 `wasm2c`）。工具链就是「C++ → emcc → wasm」。
3. **编出来的 wasm 要能跑**，行为与原模块一致。

### 路线对照（务必先看懂这个，否则会做错方向）

| 路线 | 形态 | 产物 | 与官方 | 是「正常 wasm」吗 | 依赖 wabt |
|---|---|---|---|---|---|
| **官方** | — | 5 998 665 B | 基准 sha256 `7ceabed2…d145e` | — | — |
| **① wabt 文本往返** | `.wasm → .wat → wat2wasm` | 5 998 665 B | **逐字节相同** | ✅ 是 | ★ 就是 wabt |
| **② wasm2c → C → emcc** | `.wasm → .c → emcc → .wasm` | 41 984 131 B | 行为一致（`differ: 0`） | ❌ 否，体积 ~7 倍 | ★ 起点是 wabt 的 `wasm2c` |
| **③ 目标：C++ → em++** | `.cpp → em++ → .wasm` | 41 852 136 B | 行为一致（`differ: 0`） | ❌ 否 | **完全不用 wabt** ✅ **已打通，见 §10** |

**① 的「逐字节相同」证明逆向出的 `.wat` 忠实**，但它是**校验手段**：
**它不产生新代码。** 之前的文档把 ① 当成「推荐的重建定义」——在「验证逆向正确性」这个
意义上没错，但用户要的**交付物**不是 ①，因为 ① 既没有新源码、又要 wabt。
用户要的是 **③**。

**② 是半步**：它确实产出了能编的 C，但那份 C 是 `wasm2c` 逐指令展开的机器生成物
（`i32_load_default32(...)` / `var_i0` / `var_p0`），**不是可读 C++**，而且生成它就要用 wabt。
②的意义是**证明「这份语义能用 C 编回 wasm 并行为一致」**——③ 要在这条已验证的语义基础上，
产出人能读的 C++。

**③ 已经把这件事做成了**（§10）：同一份语义改成真 C++，`em++` 一次调用链成 wasm，
行为与 ② 逐字相同。**所以「必须用 wabt 才能构建」这个前提不成立**——wabt 是转换工具，
不是构建链的一环。剩下的工作是把 ② 那份机器生成物**改写成人类可读的形态**，
构建链本身已经是 ③。

`.ref/` 整体在 `.gitignore:34` 里被忽略，所以**留在 `.ref/` 下的产物**不入 git
（与 `artemis-official/` 其余部分一致）——sync 的 284 个产品就是这种情况。
**但 asyncify 的 157 个产品不在这里：它们在入库的 `ports/artemis/src/`。**

### 一个已实测的好消息（以及一个已实测的陷阱）

**陷阱先讲**：`g++ -fpermissive` 能让那份 C 当 C++ 编过（两个 flavour 都 exit 0），
**但 `clang`/`em++` 的 `-fpermissive` 不做这件事**。实测：

```
em++ -O0 -fpermissive -x c++ ... artemis.wasm2c.c
#  → error: no matching function for call to 'w2c_a_Db'
#    note: candidate function not viable: cannot convert argument of
#          incomplete type 'void *' to 'struct w2c_a *' for 1st argument
#  → 20 errors generated（-ferror-limit 截断，实际更多）
```

`void* → struct w2c_a*` 的隐式转换在 C 里合法，在 C++ 里非法；GCC 把它降级成警告，
clang 不降。**所以不能靠编译器宽容，必须真改代码。** 这也意味着**验收工具是 em++，不是 g++**。

**好消息**：要改的地方**只有一处模式**。

```bash
g++ -fsyntax-only -x c++ -O0 -I runtime -I <flavour> <flavour>/artemis.wasm2c.c   # 不加 -fpermissive
#  → 195 errors，全部同一个模式：
#    error: invalid conversion from 'void*' to 'w2c_a*' [-fpermissive]
```

195 个错全部落在 **`wasm_tailcall_w2c_a_*` 包装器**里（108 个包装器），形如：

```c
WEAK_FUNC_DECL(wasm_tailcall_w2c_a_Db, wasm_fallback_artemis_w2c_a_Db) {
  next->fn = NULL;
  u32 result = w2c_a_Db(*instance_ptr);   /* ← 这里：instance_ptr 是 void** */
  wasm_rt_memcpy(tail_call_stack, &result, sizeof(result));
}
```

`instance_ptr` 的签名是 `void**`（见 `artemis.wasm2c.c:840-844` 的 `WEAK_FUNC_DECL` 宏），
所以 `*instance_ptr` 是 `void*`。**全文 `*instance_ptr` 共 198 处，全在这一个区块内
（行 108856–110693）。** 一次机械替换就够：

```bash
sed 's/\*instance_ptr/*(struct w2c_a**)instance_ptr/g' artemis.wasm2c.c > artemis.cpp
#  198 处替换，零残留；随后 em++ -x c++ 编译 → exit 0, 0 error
```

**含义**：C 与 C++ 之间的距离**就是这 198 个转型**（外加 runtime 文件里的 4 处，见下）。
所以「转写成 C++」不是要把 280 万行重写一遍——机械层可以留着，功夫花在**起名与语义**上。

**第二个坑（也是实测）**：`ports/artemis/reverse/build/` 里的 runtime 文件
（`wasm-rt-impl.c`、`wasm-rt-mem-impl.c`）是 **C**，直接用 em++ 当 C++ 编有 4 个错，
在 `wasm-rt-impl-tableops.inc:54` 和 `:80`：
`error: assigning to 'wasm_rt_externref_t *' (aka 'void **') from incompatible type 'void *'`
（`table->data = calloc(...)` 与 `= new_data`）。
**最省事的做法是让 runtime 保持 C**：`emcc -c -x c` 单独编成 `.o`，只把 wasm2c 生成体当 C++ 编，
两者链接即可。不要为了「统一 C++」去改 wabt 的 runtime。

**第三个坑**：`ports/artemis/reverse/out/asyncify-import-stubs.c` 第 8 行
`#include "artemis.h"`，而生成的头叫 `artemis.wasm2c.h`。写个一行桥接头
（`inc/artemis.h` 里 `#include "artemis.wasm2c.h"`）放进 `-I` 即可，别改生成物。


---

## 2. 出发点：三份已存在的产物

都在 `D:\online_gal\.ref\artemis-official\wasm\asyncify\`（详见该目录上一级 `README.md`）。

| 文件 | 规模 | 性质 |
|---|---|---|
| `artemis.wasm.pseudo.c` | 1 066 165 行 / 5 674 函数 | **WARDEN** lift 的伪 C。可读性最高（`mem[...]`、`goto`、`p0`/`v1` 这类变量名），但**不能编译**，且未建模 opcode 退化成 `op_0x…`（全文件 2 214 次） |
| `artemis.wasm2c.c` | 2 817 939 行 / 5 609 个函数定义 | **wasm2c** 出的真 C，gcc 能吃（编译出 37 109 934 B 目标文件）。命名是 `w2c_artemis_f199`，内存访问是 `i32_load_default32(...)` 这种展开形式，**读起来比伪 C 差** |
| `artemis.wasm2c.h` | 39 858 B | 上者配套类型头（含 199 个 import 原型、导出表、`wasm_rt_*` 类型） |

外加 `artemis.wasm.rebuilt.wasm`（emcc 把那份 C 链回的 wasm，41 984 131 B）——**不是**转写对象，
只是「那份 C 能编能跑」的证据。

**转写的实质**：把 `funcNNN` / `w2c_artemis_fNNN` 这些匿名函数，变成有名字、有类型、
有语义的代码。**难点从来不是翻译指令，而是给谁起名。**

---

## 3. 核心发现：官方 tag 手册就是命名 oracle

这是本次侦察最重要的结果，也是整件事可行的前提。

### 3.1 SDK 里带着 186 份官方 tag 文档

```
D:\online_gal\.ref\artemis\Docs\tag\          186 个 .txt（每份 = 一个标签的完整规格）
├── graphics/   31     ├── scenario/   29     ├── sound/    17
├── system/     53     ├── var/        38     ├── script/   15
└── preprocessor/ 3
```

每份文档写全了参数名、取值域、省略时的语义、以及互相影响的注意事项。
例：`Docs/tag/graphics/lyprop.txt` 列出 `id / left / top / alpha / anchorx / anchory /
xscale / yscale / rotate / reversex / reversey / clip / layermode / negative / grayscale /
colormultiply / visible / clickablethreshold / draggable / dragarea / intermediate_render /
intermediate_render_mask` 共 22 个参数，并说明中间渲染与 alpha 相乘的顺序问题。

**这不是我推的语义，是官方随引擎发布的规格。** 转写时该当作权威。

另外 `Docs/spec/` 还有 `script_syntax.txt`、`layer.txt`、`pack_file.txt`、`system_ini.txt`、
`system_variables.txt`、`key_assign.txt`、`macro.txt`、`requirement.txt`。

> ⚠ **`Docs/` 只存在于有授权的 `.ref/artemis` 里，不在 vendored 树里。**
> `ports/artemis/vendor/artemis-sdk/` 是个**被裁过的子集**——只有 `Android/`、
> `WebAssembly/`、`Copyright.txt` 三样，**没有 `Docs/`**。
> 它的两个 wasm 与授权版**逐字节相同**（asyncify 都是 md5 `5d31853df6ad7a209abbe2508d9c6c60`），
> 所以 **tag 文档是这套流程的硬依赖：没有 `.ref/artemis`，这套命名 oracle 就不可用。**
> `ports/artemis/reverse/` 下的脚本用 `$ARTC_SDK` → `.ref/artemis` → vendored 的顺序解析 SDK
> （见 `ports/artemis/reverse/README.md:177-185`），但那套回退**救不了 tag 文档**——vendored
> 树里根本没有那个目录。`extract-tag-map.py` 的 `--tagdir` 必须显式指向有 `Docs/tag` 的树。

### 3.2 那些 tag 名就字面躺在 wasm 的数据段里

关键一步：**153 / 186 个 tag 名在 wasm 里是 NUL 结尾的字符串字面量**。
把它们在数据段的地址找出来，再找**哪个函数引用那个常量地址**，就得到
「这个函数实现了这个 tag」。

```
tag 名有地址的: 153 / 186         （只有 153 是因为 33 个用于 tag 的短名如 if/wait/rt/jump
                                   没被编译成独立字面量，或在 sync 版才有）
绑到函数的:      46 个函数
```

最重的几个（行数 = 在 `artemis.wasm2c.c` 里的定义体行数）：

| 函数 | 行数 | tag 数 | 认领的 tag |
|---|---:|---:|---|
| `w2c_artemis_f3744` | 99 251 | **103** | `lyprop` `chgmsg` `seplay` `takess` `calllua` `lyc` `lyevent` `trans` … 主 tag 分派器 |
| `w2c_artemis_f3633` | 78 669 | 33 | `base64_encode` `character_reference_to_utf8` `convert_encoding` `date` `explode` `file_crc` … var 类 |
| `w2c_artemis_f3650` | 9 601 | 21 | `delonautomodein` `delonclick` `delondrag` … 全部 `delon*` |
| `w2c_artemis_f3651` | 14 998 | 21 | `setonautomodein` `setonclick` … 全部 `seton*` |
| `w2c_artemis_f2945` | 14 467 | 6 | `anime` `lyc` `lyprop` `scein` `sceout` `video` |
| `w2c_artemis_f3666` | 2 400 | 5 | `automode` `backlog` `hide` `rclick` `skip` |
| `w2c_artemis_f3112` | 6 511 | 5 | `debug` `file` `random` `tag` `var` |
| `w2c_artemis_f3222` | 6 669 | 5 | `else` `elseif` `if` `loop` `lua` |
| `w2c_artemis_f3196` | 6 898 | 5 | `font` `print` `rt` `ruby` `voice` |

（sync 版同样跑得出来，且定位率更高：170 / 186 个 tag 绑到 71 个函数；最重的是
`w2c_artemis_f6590` 21 234 行 / 136 tag。所以 sync 版的 tag 映射其实**更全**——
若要给 asyncify 的 tag 补语义，可以拿 sync 版做交叉参照。）

结构核实过：`f3744` 在 tag 地址处的代码是
`var_i1 = 718325u;`（→ 指向 `"chgmsg"`）然后比较/拷进临时对象，即**典型的 strcmp 分派**。
这个函数里有 4 990 处 `instance->w2c_g3` 读取，而 `g3` 是 asyncify 的状态机全局
（`Global[3]`，`i32 mutable`）——**asyncify 把函数变成了状态机，`g3` 决定从哪一段续跑**。
转写时这是必须还原的结构，不能当普通变量。

### 3.3 转写范围因此可以算出来

从 tag 认领的 46 个函数出发，沿调用图走三层，覆盖：

```
roots   46 函数   383 591 行
+1 层   150 函数
+2 层    64 函数
+3 层    58 函数
closure 318 函数   505 768 行      全文 5 609 函数 / 2 705 479 行
```

**这就是分期的依据**：318 个函数是「有名字可依据的骨架」——只占 5 609 个函数的 5.7%，
却占 2 705 479 行里的 **18.7%**（505 768 行），因为 tag 分派器正是那几个巨函数。
它们覆盖了**全部 tag 语义**。剩下的 5 291 个函数没有任何 tag 锚点，
是渲染/数学/压缩/字体这类叶节点——可以后做（用 `.so` 符号 + 字符串锚点起名），
或按调用它们的上层反推。

### 3.4 辅助锚点（已量化，供第二期用）

除 tag 名之外，数据段里还有 5 404 个可定位的 ASCII 字符串（≥4 字符）。其中
`GL_` 170 / `gl` 210 / `font` 81 / `file` 120 / `lua` 102 / `error` 67 / `layer` 68 / `png` 39。
**1 393 个函数至少引用一个字符串**，覆盖 1 558 615 行（57.6%）。

注意几个**不在**里面的：`system/init.lua`、`pf8`、`.psb`、`Lua`、`error` 在 asyncify wasm 的
数据段里搜不到（`strings` 级也没有）。`.iet` 有 1 处（`w2c_artemis_f859`）。
所以别指望 Lua 引擎那侧能从字符串认出来——它可能是编译进去的，也可能在 sync 版里。

### 3.5 另一条 oracle：Android `.so` 有真符号

`artemis-official/android/arm64-v8a/libartemis.so.index.tsv` 里有 **492 个 `artemis::`** 符号，
类骨架清晰：

```
CArtemis, CArtemisHttp, CArtemisInput, CArtemisKeyConfig, CArtemisLayer,
CArtemisPackFile, CArtemisPackFileFactory, CArtemisParser, CArtemisTouch, CArtemisVariables
```

加上 `CArtemis::CArtemisInput` / `::CCaret` / `::CGlyphInfo` / `::CIconInfo` / `::STATUS` 等。
wasm 与 arm64 `.so` 的字符串交集 **1 946**（wasm 唯一串 2 476 / so 唯一串 16 082）。
**.so 是给 wasm 函数起名的第二个字典**——尤其那些没有 tag 锚点的叶节点。
（注意：so 是 arm64 native，函数体与 wasm 不对应，只有**名字**可借。）

---

## 4. 现成工具

### `ports/artemis/reverse/extract-tag-map.py`（本次新写）

```bash
python ports/artemis/reverse/extract-tag-map.py \
    --wasm2c .ref/artemis-official/wasm/asyncify/artemis.wasm2c.c \
    --tagdir .ref/artemis/Docs/tag \
    -o       .ref/artemis-official/wasm/asyncify/tag-map.json
```

输出 `tag-map.json`：`tag_addr`（tag → 线性地址列表）、`tag_owner`（函数 → tag 列表）、
`sizes`（函数行数）、`unlocated`（没定位到的 tag）。

**三个坑，别再踩：**

1. **Windows 上必须传原生路径**（`D:/...`）。这台的 Python **不解析 MSYS 的 `/d/...`**，
   路径错时它**静默报「0 tags found」而不是报错**——所以脚本里加了 tag 数非零断言。
2. **函数定义行不带 `static`**，前向声明才带。用 `^static ... w2c_artemis_f\d+` 匹配会
   **全部匹配到前向声明**，于是每个地址都「绑」到空 span，结果同样是静默的 0。
   必须匹配 `w2c_artemis* instance` 参数——只有定义有。
3. **`--tagdir` 必须指向有 `Docs/tag` 的树**（即 `.ref/artemis`）。vendored 树没有这个目录，
   指过去同样是静默的 0。

已跑出的结果（两个 flavour 都有，`sort_keys=True` + 尾部换行，**逐字节可复现**）：
`wasm/asyncify/tag-map.json`（172 947 B，153 tag / 46 函数）、
`wasm/sync/tag-map.json`（298 513 B，**170 tag / 71 函数**——sync 版定位率更高）。

### 其它可复用

| 路径 | 用途 |
|---|---|
| `ports/artemis/reverse/rebuild.sh` | 校验**逆向忠实度**（wabt 三条断言）。⚠ 它走 wabt，**不是**转写产物的验收链——验收用 `build-cxx-route.sh`（见 §10） |
| **`ports/artemis/reverse/build-cxx-route.sh`** | **★ 验收用这个**：C++ → em++ → wasm，**全程无 wabt**，并自动跑行为比对。`bash build-cxx-route.sh asyncify` 或 `sync`。见 §10 |
| `ports/artemis/reverse/reverse.sh` | 产出 `.wat` + wasm2c C + so 的逐函数 `.c` |
| `ports/artemis/reverse/ghidra/DecompileAll.java` | Ghidra headless 脚本：**一个函数一个 `.c` 文件** + `index.tsv`。`ports/artemis/reverse/README.md:156-158` 说明这个粒度是**故意的**——「One file per function rather than one 100 MB blob, so callers of a symbol stay greppable」。**转写委托正该沿用这个粒度**，别再造单体文件 |
| `.ref/wabt/build/wasm-objdump.exe` | `-h` 段表、`-x` 导出/导入/类型、`-s` 字节转储 |
| `.ref/wabt/build/wasm2c.exe` | 1.0.42-8-g02418f72，重新生成那份 C（**只在产生主输入时需要，构建链上用不到**） |
| `.ref/artemis-official/scripts/verify_outputs.py` | 现成的自检（函数数交叉核对 + manifest 一致性）；新产物应挂进去 |
| `docs/artemis-web-port.md` §16 | 逆向链的完整说明（wabt 逐字节路线 vs wasm2c 行为路线） |

**构建 wabt 工具链的坑**（若需重建）：从 `.ref/wabt` 源码构建时 `wasm2c` 能链上，但
`wasm2wat`/`wat2wasm` 会 `undefined reference to __imp__strtof_l`（本机 MinGW 的
`include/stdlib.h:404` 把 `_strtof_l` 声明成 `_CRTIMP`，而 wabt 的 `src/literal.cc:48`
有 `#define strtof_l _strtof_l`；`libmsvcrt.a` 只定义 `_strtof_l` 没有导入桩）。
**修法**：配置时加 `-DCMAKE_CXX_FLAGS="-D_CRTIMP=" -DCMAKE_C_FLAGS="-D_CRTIMP="`。

**但请注意（§10）**：wabt 只在**产生主输入**时有用（`wasm2c` 出那份 C、`wasm2wat` 供①校验）。
**转写产物的构建链里没有 wabt**，也不需要它。

---

## 5. 参考引擎的边界（重要，别越线）

`D:\online_gal\.ref\artemis-compat` 自称 **clean-room**，`README.md:6-7` 与 `AGENT.md:9-10`
都写明：

> 实现规格来自**行为级逆向**（格式/标签/接口/时序）与公开规格，**不转写任何反编译伪代码**。

**所以：可以读它来理解「引擎该有什么行为」，不可以把它当转写目标或把它的实现搬到产物里。**
它的模块布局（`src/` 共 31 228 行）与我们的转写**目的不同**——它是自研实现，我们要的是
官方那份的忠实转写。两者对照着看很有价值（尤其 `src/script/` 5 777 行是 tag 分发，
`src/render/` 18 928 行是合成器），但产物必须是**官方 wasm 的转写**，不是它的复刻。

它也**没有 wasm 版本**——asyncify 状态机那部分在它那里没有对应物，别去找。

---

## 6. 历史记录：mimo-v2.6-flash 的委托路线被拒（**已不影响任何事**）

**这一节现在只是留档。**当年它是唯一的未决项，但转写并没有被它挡住——
asyncify 与 sync 两边的产品都已经产出来了（见文首表格），也就是说这件事**当时就该按这个结论处理：
它是配置层的事，不是转写工作的一部分**。接手人不需要读这节才能开工。

以下是当时穷尽的观察，保留是因为「为什么会被拒」的排查过程本身有复用价值。

**状态（当时）：本会话内无解，用户已指示停止排查（m01082「别试了，直接做交接工作」）。**
新会话**不要**把它当作必过的前置关卡去反复试探——它属于配置层的事，不是转写工作的一部分。
带着「按可用路线开工」的姿态进来即可。

### 观察到的事实（已穷尽，不必重跑）

`~/.dsh/settings.yaml`（**853 行，全机唯一一份 settings，无 local 覆盖、无重复键**）：

```yaml
12  agent-default-model:
13    provider: commandcode
14    model: xiaomi/mimo-v2.6-pro          # ← 注意这里是 pro

41    commandcode:                          # 在 llm-pi-ai.providers 下
44      baseURL: https://api.commandcode.ai/provider/v1
...
820       - id: xiaomi/mimo-v2.6-pro
823       - id: xiaomi/mimo-v2.6-pro-ultraspeed
826       - id: xiaomi/mimo-v2.6-flash      # ← 模型确实注册在 commandcode 名下

841 subagent-model-selection:
842   enabled: true
843   allowedModels:
844     - provider: commandcode
845       model: poolside/laguna-s-2.1-free
846     - provider: commandcode
847       model: deepseek/deepseek-v4.1-flash
848     - provider: commandcode
849       model: xiaomi/mimo-v2.6-flash     # ← 已加，但仍被拒
```

实测（本会话多次，**连「只回 OK」的一行探针也拒**）：

```
Error: child LLM route "commandcode/xiaomi/mimo-v2.6-flash" is not allowed for this Session
```

`list_subagent_models(provider: commandcode)` 只 advertise 两个：
`deepseek/deepseek-v4.1-flash`、`poolside/laguna-s-2.1-free`。

### 已排除的解释

| 假设 | 否证 |
|---|---|
| 模型没注册 | `settings.yaml:826` 就在 commandcode 的 models 里 |
| provider id 写错 | `commandcode` 正是 `agent-default-model.provider` 用的同一个 |
| 前缀写错 | 带 `xiaomi/` 与不带都试过，报错完全一致 |
| 配置被别的层覆盖 | 全机只有 `~/.dsh/settings.yaml` 一份；无 `.local` 变体；`subagent-model-selection` 与 `llm-pi-ai` 各只出现一次 |
| 报错≠allowlist 判定 | 对照：`commandcode/claude-sonnet-5`（**在模型表、不在 allowedModels**）报**同一个错**；而 `deepseek/deepseek-v4.1-flash`（在 allowedModels）**正常返回 OK**。→ 该错误确实就是 allowedModels 判定 |

### 结论与建议

allowlist 是**会话启动时载入**的，本会话载入的版本里没有第三项（所以同一份文件改了也不生效）。
**新会话理应放行** —— 但**不要为此专门开一轮试探**。

- 新会话开工时顺手带上 `provider: "commandcode"` + `model: "xiaomi/mimo-v2.6-flash"` 委托第一个子任务即可：
  **成功就继续用它，被拒就换下面的可用路线，不要反复试。**
- 注意 `agent-default-model` 指向的是 `mimo-v2.6-pro`，而 allowlist 里允许的是
  `mimo-v2.6-flash`（**两个不同 id**）。若新会话也拒，且确实要用 mimo，才去动配置（把 pro 也加进去）。

**实测可用的两条路线**（本会话验过，返回 `OK`）：

| 用途 | provider | model | 状态 |
|---|---|---|---|
| 轻量编程（默认） | `commandcode` | `poolside/laguna-s-2.1-free` | ✅ 可用 |
| 接手（laguna 做不了时） | `commandcode` | `deepseek/deepseek-v4.1-flash` | ✅ 可用 |

⚠ `subagent_fork` **不能选模型**（会把当前模型继承下去），编程委托不要用它。

---

## 7. 当时的推进方式（**实际就是这样执行的**）

下面这段是当时定下的做法，后来照着做完了。保留全文，因为它是理解
`ports/artemis/REWRITE.md`（四档工序）和 `ports/artemis/reverse/p2-*.md`（分批进度）的钥匙。

### 分期

- **第一期（建议先做）**：`tag-map.json` 里的 **46 个 root 函数**（383 591 行）。
  每个都有明确 tag 语义 + 官方文档背书。这是价值最高、验证最容易的一部分。
- **第二期**：闭包剩下的 272 个函数（约 122 000 行）。
- **第三期**：叶节点与渲染/字体/压缩（用 `.so` 符号 + 字符串锚点起名）。
- `f3744`(99 251 行) / `f3633`(78 669 行) 两个巨函数**必须再拆**——它们是分派器，
  天然可按 tag 切成 103 / 33 块，每块独立。

### 并发委托怎么切（用户明确要求）

按**函数**切，不按文件切——每个函数是一个自包含单元：

- 输入给子代理：该函数的 `wasm2c` C 定义体（行号区间可从重跑 `extract-tag-map.py` 得到）、
  它认领的 tag 列表、**对应 tag 的官方文档全文**（`Docs/tag/<cat>/<tag>.txt`）、
  以及被它调用的下层函数的签名。
- 要求产出：**可读 C++**（`.cpp`/`.hpp`），带**原始行号注释**便于回溯。
  产出要能进 `build-cxx-route.sh` 那条链——即写的是真的 C++，不是文档。
- 验收：能编 + `build-cxx-route.sh` 行为不变（`differ: 0`）。

**一个函数一个子代理**，`f3744` 按 tag 切 103 份。粒度小、彼此独立——正好符合
「互不依赖、各自有独立验收标准」的委托前提。

**不要重写机械层**。§10 已经证明：生成体以 C++ 编译并行为一致，代价只有 198 处转型。
所以子代理的任务是**起名 + 语义还原 + 组织**，不是把 270 万行重敲一遍。建议的切法：

1. 先定「一函数一文件」的骨架与命名规范（`w2c_artemis_f3744` → 按 tag 语义起名）；
2. 先用 3～5 个**最小的** tag 归属函数做样本定格式：
   `w2c_artemis_f5551` / `f5670`（各 90 行 1 tag）、`f5598`（210 行 1 tag）、
   `f5596`（786 行 4 tag）、`f3460`（570 行 1 tag）；
3. 样本定稿后成批并发委托，每批用 `build-cxx-route.sh` 复核 `differ: 0`。

### 验收用什么

**用 `ports/artemis/reverse/build-cxx-route.sh`**（见 §10）。它把转写产物用 **em++ 单独**
链成 wasm 并逐导出比行为，**全程不调用 wabt**：

```bash
bash ports/artemis/reverse/build-cxx-route.sh asyncify   # 或 sync
#  → 打印 agreement / differ: 0，非零退出即失败
```

**行为等价优先于可读性**。已实测的基线：asyncify `agreement 20/66, differ 0`、
sync `agreement 16/18, differ 0`。转写产物必须保持这条链可跑：
**转写不能改行为**，能编译、能链回、行为一致，才算过。

⚠ 注意别用 `rebuild.sh` 当验收——它走 wabt（`wasm2wat`/`wat2wasm`/`wasm2c`），
验收工具链会与用户「不依赖 wabt」的要求冲突。`rebuild.sh` 现在是**校验逆向忠实度**用的
（① 逐字节往返），不是构建/验收转写产物的链路。

---

## 8. 关键数字速查（免得重新量）

```
asyncify wasm:  5 998 665 B   sha256 7ceabed28b84e7689fc4b629fa232e2857f15c3b6e6fa3e09fc39dba082d145e
  段表: Type 100 / Import 199 / Function 5674 / Table 1 / Global 5 / Export 67 / Elem 1 / Code 5674 / Data 7310
  ★ 没有 name 段（sync 版也没有）
  导出 67 个 = 1 个 table[0]("Lc") + 66 个函数；全是工具链符号：
    Mc=___wasm_call_ctors  Nc=_malloc  Oc=_free  Pc=___errno_location  Qc=_main
    Rc=_htonl  Sc=_htons  Tc=_ntohs  Uc=__get_tzname  Vc=__get_daylight  Wc=__get_timezone
    $c=___cxa_can_catch  _c=_setThrew  ad=___cxa_is_pointer_type  Yc=stackRestore …
    以及 dynCall_* 一族（Ad…zd）和 4 个 asyncify 入口
    Ud=_asyncify_start_unwind  Vd=_asyncify_stop_unwind  Wd=_asyncify_start_rewind  Xd=_asyncify_stop_rewind
  Global: g0=6100320(栈指针)  g1..g4=0，g3 是 asyncify 状态（i32 mutable）
  数据段只铺到 ~821 008；热地址 mem[852316]（47 601 次引用）是堆/BSS，不是字符串池

函数规模（wasm2c C 的定义体行数）:
  5 609 个定义，总 2 705 479 行；中位数 193，均值 482，最大 99 251
  ≤50 行 1 239 个 / 50–200 行 1 624 / 200–1 000 行 2 291
  1 000–5 000 行 393 / 5 000–20 000 行 60 / >20 000 行 2

三份产物的函数计数（别混，已逐一对齐过）:
  wasm Function 段        5 674   ← 唯一权威计数（func_index 199..5871 为定义段）
  warden-index.tsv 行     5 674   ← 与 Function 段一致
  WARDEN pseudo.c 函数头  5 674   ← 一致；其中 78 个带名字、5 596 个是 func_NNN
  wasm2c 定义             5 609   ← ★ 少 64 个

  ★ 那少的 64 个，就是**被导出的那 64 个函数**（导出表 65 个索引，落在 f199..f5871 区间内的
    正好 64 个；差 1 个是 table[0]）。wasm2c 把它们生成为**对外符号**，不写成
    `^[a-z0-9_]+ w2c_artemis_fN(w2c_artemis* instance…` 这种静态定义形态，所以上面的正则数不到。
    **不是缺失，是形态不同**——转写时这 64 个是入口，必须在清单里单独列出（它们就是
    `_malloc`/`_free`/`_main`/`dynCall_*`/`asyncify_*` 一族）。

  pseudo.c 那 78 个名字**去重后只有 59 个**（`Zc` 出现 9 次、`Pc` 9 次、`Vd`/`Ud`/`Rc` 各 2 次…），
  且这 59 个**全部**落在 wasm 的 66 个导出名集合里——WARDEN **没有额外认出一个名字**。
  重复是一个 wasm 函数被 lift 成多块却沿用同一导出名标注所致。
```

---

## 9. 一句话总结

**不要从零开始猜函数在干什么**：官方 SDK 自带 **186 份 tag 规格文档**，其中 **153 个 tag 名
就是 wasm 数据段里的字符串字面量**，绑到 **46 个函数**，沿调用图展开就是 **318 个函数 /
505 768 行**的骨架——这是有据可依的第一期。剩下的用 `.so` 的 492 个 `artemis::` 符号和
5 404 个字符串锚点起名。工具（`extract-tag-map.py`）和地图（`tag-map.json`）都已就位。

**这条结论已被执行验证**：两边的产品都产出来了（asyncify 157 / sync 284），
命名靠的就是这里的 tag oracle 加 `.so` 符号（映射表落在 `ports/artemis/rewrite-names.tsv`）。

---

## 10. ✅ 路线 ③ 已打通（本文档最重要的一节）

「C++ → em++ → wasm，全程不碰 wabt」**不是设想，已经跑通并验过行为一致**。

### 跑通的命令（唯一工具是 em++ 与 emcc）

**已固化为脚本，直接跑它即可**：

```bash
bash ports/artemis/reverse/build-cxx-route.sh asyncify   # 或 sync
```

它做三件事（下面①~③），然后自动跑行为比对并断言 `differ: 0`，失败即非零退出。

#### ① 唯一的源码改动：198 处 `void*` 转型

```bash
sed 's/\*instance_ptr/*(struct w2c_a**)instance_ptr/g' artemis.wasm2c.c > artemis.cpp
```

（asyncify 198 处 / sync 195 处，见 §1 的说明。）

#### ② 桥接头

生成的头叫 `artemis.wasm2c.h`，而 `out/*-import-stubs.c` 里写的是 `#include "artemis.h"`：

```bash
printf '#include "artemis.wasm2c.h"\n' > inc/artemis.h
```

#### ③ runtime 保持 C，生成体当 C++，一次 em++ 链成

```bash
emcc -c -O0 -x c -I runtime build/wasm-rt-impl.c     -o wasm-rt-impl.o
emcc -c -O0 -x c -I runtime build/wasm-rt-mem-impl.c -o wasm-rt-mem-impl.o

em++ -O0 -Wno-c23-extensions -Wl,--export-all -sEXPORT_ALL \
  -I inc -I runtime -I <flavour> \
  artemis.cpp <flavour>-import-stubs.c <flavour>/artemis-export-shims.c \
  wasm-rt-impl.o wasm-rt-mem-impl.o \
  -o cxx-route.js
```

`--export-all -sEXPORT_ALL` 是**承重**的：没有它 Emscripten 会把 JS glue 未引用的
全部 dead-strip，产物缩到几 KB、零导出，比对只会报「0 shared exports」。

### 验证结果（两个 flavour 都跑过）

```
# asyncify
agreement: 20/66, trap-kind-only: 46 -> $c, Ad, Bd, … , differ: 0
  of the agreements: 19 returned equal values, 1 threw the same trap
→ out/cxx-route-asyncify/cxx-route.wasm   41 852 136 B

# sync
agreement: 16/18, trap-kind-only: 2 -> Yc, _c, differ: 0
  of the agreements: 15 returned equal values, 1 threw the same trap
→ out/cxx-route-sync/cxx-route.wasm       18 182 343 B
```

**asyncify 的数字与「② wasm2c→C→emcc」那一轮逐字相同**（同样 20/66、同样 46 个
trap-kind-only）——说明 C++ 路线与已验证的 C 路线**行为等价**，C++ 化没有引入回归。

`trap-kind-only` 是**间接调用表满度不同**造成（官方 5 884 项 vs 重建 4 472 项）：
同一次 `call_indirect`，一侧落在有主槽位报 `function signature mismatch`，另一侧落在空槽
报 `unreachable`。两边都不返回，差异属**链接**而非转写。`--strict` 可把它变成失败。

### 由此确定的「可读 C++」工作形态（**已按此执行**）

机械层（280 万行）**不需要重写**：它已经能以 C++ 编译并行为一致。真正的工作是

- **起名**：`w2c_artemis_f3744` → 有意义的名字（靠 §3 的 tag oracle 与 `.so` 符号）；
- **语义**：`i32_load_default32(instance, ...)` / `var_i0` / `var_p0` 这类展开形态
  写成有类型的局部变量与结构体成员；
- **组织**：按函数切成一函数一文件（沿用 `ports/artemis/reverse/ghidra/DecompileAll.java`
  的粒度理由，见 `ports/artemis/reverse/README.md:156-158`）。

即：**先有一份「能编且行为正确」的基线**（现在有了），再在其上做可读性改造，
每改一批都用上面的命令重编 + `wasm-diff.mjs` 复核，保证行为不漂。

**实际执行时，验收链多了一层**：单段转写不可能各自去链一次整包 wasm，所以流程落成
「写片段 → `code-assemble.sh` 把片段拼回生成文件并改名 → 再走 `build-cxx-route.sh` 编译比对」。
`ports/artemis/reverse/code-assemble.sh` 就是这个拼装器（片段首行是 `// tx: f<NNN> -> <new_name>`，
巨型函数用 `f<NNN>.partNN.cpp` 分片），它自己最后调用 `build-cxx-route.sh`，
**所以「每批必须 `differ: 0`」这条闸是自动跑的**。批量命名的规范与四档工序（R0 气味清洗 /
R1 纯搬运风格化 / R2 结构化改写 / R3）写在 `ports/artemis/REWRITE.md`。

各批的执行结果见 `ports/artemis/reverse/p2-batch1-names.md`、`p2-batch2-progress.md`、
`p2-batch3-progress.md`、`p2-batch4-progress.md`、`p2-batch4b-progress.md`（asyncify）与
`p2-sync-rewrite-progress.md`（sync）。

---

## 11. 执行结果（**这次交接的答案**）

三条核心结论，都已经是既成事实：

1. **「只有 wabt 构建出来的才是正常 wasm」是误读**，§1 当时就写清了这一点：
   ① 的「逐字节相同」只是**校验手段**（证明逆向出的 `.wat` 忠实），② 是**半步**
   （wasm2c 逐指令展开的机器生成物，不可读，且生成它就要 wabt）。**要的是 ③。**
2. **③ 打通了，并且两条 flavour 都把产品产出来了**（§10）：
   `C++ → em++ → wasm`，**全程不调用 wabt**。改动量只有 198 处 `void*` 转型 +
   runtime 保持 C 编译 + 一个一行桥接头。**不存在「必须用 wabt 才能构建」这件事**——
   它只是一个**转换工具**，不是构建链的一部分。
   验收脚本：`ports/artemis/reverse/build-cxx-route.sh asyncify|sync`。
3. **这件事比预想的可行得多**：官方 tag 手册就是现成的命名字典，第一期 318 个函数的范围
   当时就算清楚了，工具（`extract-tag-map.py`）和地图（`tag-map.json`）都已就位。
   机械层不用重写，力气花在**起名 + 语义还原 + 组织**上。

**产品落地位置**（本文档开头那张表是权威，这里再点一次）：

| flavour | 产品数 | 位置 | 入库 |
|---|---:|---|---|
| asyncify | 157 | `ports/artemis/src/` | ✅ 已入库（含 `REWRITE.md`、`rewrite-names.tsv`） |
| sync | 284 | `.ref/artemis-official/rewrite-sync/src/` | ❌ 本机保留，`.ref/` 被忽略 |

`mimo-v2.6-flash` 的委托路线**当时没解决，也没有挡住任何事**——那只是配置层的问题（见 §6）。
它不再是「唯一没解决的」：转写用的是实测可用的那条路线。

---

## 12. 还没做完的部分（**接手人从这里继续**）

按 `ports/artemis/reverse/p2-sync-rewrite-progress.md` 尾节与 `ports/artemis/REWRITE.md` §0：

- **sync 侧无欠账**：284 个产品；四道闸全绿 —— `style-gate.sh` PASS、`code-assemble.sh`
  18/18 `differ: 0`（284 products spliced，未调用 wabt）、`wasm-diff.mjs --strict` EXIT 0、
  `build-bridge-route.sh` EXIT 0。（asyncify 侧同样四道全绿，见
  `ports/artemis/reverse/p2-sync-rewrite-progress.md` §8 的八格闸表。）
- **asyncify 侧还欠**（这就三件）：
  1. batch-4B 剩下的产品体（约 **28,417 行**，当时只落了 3 个）；
  2. `f3744`（99,251 行、认领 103 个 tag 的主分派器）的**切分**；
  3. P4 长尾约 **3,474 个未触达函数**进 `generated/` 隔离区
     （生成器所有、盖 generated 戳、风格闸豁免；工单触及时逐个升格 R0/R1）。
- 每落一个新产品，都要重跑一遍 `ports/artemis/reverse/style-gate.sh` 与行为锚
  （真实游戏帧哈希 == `bef44818`）。

**注意 flavour 路径**（p2-sync-rewrite-progress.md 明确记下的坑）：asyncify 的基线是
`.ref/artemis/WebAssembly/**asyncify**/artemis.wasm`，不是 sync 的
`.ref/artemis/WebAssembly/artemis.wasm`——猜错会得到 `FileNotFoundError`，而不是一个错答案。
