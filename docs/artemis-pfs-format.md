# Artemis PFS（pf8）归档格式

本文是 pf8 读取器的规格依据。读取器本体在 **`packages/runtime-artemis/src/pfs.ts`** ——
**不在** `@online-gal/vfs` 里（`packages/vfs/src/archives/` 只有 `nsa` / `xp3` / `zip` / `asar`，
没有 pf8）。它由**两个独立实现 + 真实归档字节**三方对齐得出：

| 来源 | 位置 | 作用 |
| --- | --- | --- |
| **GARbro** `ArcPFS.cs`（morkt，MIT） | `.ref/garbro/ArcFormats/Artemis/ArcPFS.cs` | 久经考验的描述，含版本分支与编码回退 |
| **artemis-compat** `pf8_reader.cpp` | `.ref/artemis-compat/src/pack/pf8_reader.cpp` | 自研实现，注解里写明「pad」「original engine behavior」 |
| 真实归档 | `root.pfs`（**2,690,454,559 B**） | 用字节裁定分歧 |

脚本经 `ARTEMIS_GAME_DIR` 定位真实归档，默认落在 `.ref/artemis-game/`（真实游戏数据永不入库）；
本机实际位于 `.test/test-games/Artemis/`（另有 `root.pfs.000`、两个 `chs_patch_*.pfs`）。

验证脚本：`ports/artemis/build/parse-pf8-index.py`（按 GARbro 布局解析真实归档）。

## 一、文件头

偏移为文件绝对偏移。

```
0   "pf"        2 字节 ASCII              GARbro 只检查这两个字节
2   version     1 字节 ASCII 数字          '6' / '8' 走 OpenPf；'2' 走 OpenPf2
3   index_size  u32 LE
7   count       i32 LE                    ← 属于 index blob 的一部分（见下）
7   index       index_size 字节           blob = [count][条目...]
```

**一个易错点**：`index` 的起点是 **7**，也就是说 blob 的**前 4 字节就是 count**。所以条目从 blob 内偏移 **4** 开始，即文件绝对偏移 **11**。

两个实现只是切法不同，结果一致：

- GARbro：`index = ReadBytes(7, index_size)`，循环从 `index_offset = 4` 起 → 绝对 11
- artemis-compat：`ReadAt(11, table, ...)`，循环从 `pos = 0` 起 → 绝对 11

## 二、条目记录（版本 6 / 8）

```
u32  name_length     以字节计，不含终止符
     name            name_length 字节，编码见下
u32  pad             两个实现都读掉并忽略；本归档中 21587 条**全为 0**
u32  offset          文件内绝对偏移
u32  size            字节数
```

**步长 `name_length + 16`**。记录形状：

```
[4 name_length][name_length][4 pad][4 offset][4 size] = 4 + n + 4 + 4 + 4
```

### 这个步长极易算错

GARbro 把推进拆成两条语句，中间夹着读取：

```csharp
int name_length = index.ToInt32 (index_offset);
var name = encoding.GetString (index, index_offset+4, name_length);
index_offset += name_length + 8;        // 名字字段 + 名字 + pad
entry.Offset = index.ToUInt32 (index_offset);
entry.Size   = index.ToUInt32 (index_offset+4);
index_offset += 8;                      // offset + size
```

**第一条 `+8` 已经把「4 字节 name_length 字段」和「4 字节 pad」一起算进去了**，所以两次相加是 `name_length + 16`，不是 `name_length + 20`。

artemis-compat 的写法反而更清楚，因为它把 pad 显式读出来：

```cpp
const uint32_t name_len = ...;  pos += 4;
if (pos + name_len + 12 > table.size()) break;   // name + pad + offset + size
e.name.assign(table.data() + pos, name_len);  pos += name_len;
(void)rd();                     // pad —— 显式读掉
e.offset = rd();
e.size   = rd();
```

`+12` 的检查正好对应 `name_len + 4(pad) + 4(offset) + 4(size)`。

**踩过的坑（值得记住）**：我按 `name_length + 20` 解析，reads 立刻在第二条记录处炸掉，报「implausible name_length 1953394534」。那个数字是 `0x746E6F66`，也就是 ASCII 的 **`font`** —— 读串位之后，把**名字**当成了长度。**任何「长度字段看起来像可读文本」的现象，都说明记录边界已经错了。**

## 三、版本 2（`OpenPf2`）的三处差异

```
count 在偏移 0xB（不是 7）
条目从 index 内偏移 8 开始（不是 4）
步长是 name_length + 0x10：即两个被跳过的 u32，而非一个
```

也就是说版本 2 的记录是 `[4 name_length][name][4 pad][4 pad2][4 offset][4 size]`。名字固定按 **cp932** 解，无编码设置。

## 四、密钥派生

**只有版本 4、5、8、9 加密**；版本 6 和 2 **不加密**（GARbro 只对前者构造 `PfsArchive` 并带 `Key`）。

```
key = SHA1(index)          index = 文件偏移 [7, 7 + index_size) 的字节
```

两个实现一致：
- GARbro：`sha1.ComputeHash(index)`，其中 `index = ReadBytes(7, index_size)`
- artemis-compat：注释写明 `SHA1 over the hashed region [7, 7 + index_size)`

**注意哈希区间含 count 那 4 字节**（因为 blob 从 7 开始）。

解密方式：**20 字节 SHA1 摘要作为周期 XOR 密钥**对**条目数据**做 XOR（索引本身不加密，只加密内容）。GARbro 用 `ByteStringEncryptedStream(input, key)` 包住条目流。

真实归档 `root.pfs` 的密钥：

```
f86f6cefdd968cacf252ad67efb2c1683446a1e3
```

## 五、编码

GARbro 有一个 `PFSEncodingCP` 设置，默认 `DefaultEncoding`，**失败时在 cp932 与 UTF-8 间回退**：

```csharp
try { return OpenPf (file, version, PfsEncoding.Get<Encoding>()); }
catch (System.ArgumentException) { return OpenPf (file, version, GetAltEncoding()); }
```

`GetAltEncoding` 取「当前不是 932 就用 932，否则用 UTF-8」。

本归档的路径名是 ASCII 为主，cp932 与 UTF-8 都能解析出同样的前若干条；含中日文的名字才会分歧。**解析器应把编码当作可配置项并支持回退，不要写死。**

## 六、路径分隔符是反斜杠

条目名形如：

```
font\extra_28.rft
font\sourcehansans-medium.otf
pc\ja\artemis.png
pc\ja\blog\name\雪.png
```

**是 `\` 而不是 `/`**。接进 VFS 时必须归一化，否则按 `/` 拼的查找键全部落空。

## 七、真实归档的验证结果

`root.pfs`（版本 8）：

| 项 | 值 |
| --- | --- |
| index_size | 1,185,775 |
| count | **21,587** |
| 解析出的条目 | **21,587**（全部） |
| SHA1 密钥 | `f86f6cefdd968cacf252ad67efb2c1683446a1e3` |
| 条目越界（offset+size > 文件大小） | **0** |
| 条目 size 总和 | **2,689,268,777** |
| 文件大小 | **2,690,454,559** |
| 差值 | **1,185,782** |

**差值恰好等于 `7 + index_size`（7 + 1,185,775 = 1,185,782）** —— 也就是说文件结构就是 `[7 字节头][索引][数据]`，**所有条目大小之和加上头与索引，正好等于文件大小，一个字节不差**。这是布局正确的最强证据。

第一条目的 offset 也正好是 **1,185,782**，即数据紧跟索引之后。

## 八、一个两个实现都没读的尾部区域（开放问题）

条目表在索引内偏移 **1,013,063** 处结束，而 `index_size` 是 1,185,775 —— **尾部还有 172,712 字节无人解读**。GARbro 与 artemis-compat 都只读 `count` 条就停。

实测该区域：

```
首 32 字节: 54 54 00 00 | 19 00 00 00 | 00 00 00 00 | 3a 00 00 00 | 00 00 00 00 | 67 00 00 00 | ...
首 u32:     21588   ← 恰好是 count + 1
其后:       25, 0, 58, 0, 103, 0, 136, 0, ...   递增的 u32 序列
非零字节占比: 36.55%
```

`count + 1` 加一串递增序号，像是**按名字查找的索引/桶表**（或哈希链头）。**本文不断言它的语义**——只知道两个实现都不需要它。

**但这并不意味着尾部可以跳过。** 索引**必须整体读取**，因为密钥是 `SHA-1(整个 index)`，那 172,712 字节虽然
从不被解析，**却要参与哈希**——只哈希前 1,013,063 字节会得到不同的摘要，所有条目都会解成噪声。
所以按需加载的下限是 **`7 + index_size`**（该归档为 **1,185,782 字节**，约 1.13 MB），而不是「条目表的大小」。

**实测确认**（`readPfsTableFrom` 对真实 `root.pfs` 发出的范围请求，另加随后取 `system.ini` 那一条）：

```
[[0, 7], [7, 1,185,775]]                        →  表 1,185,782 B
[[2,690,450,941, 3,618]]                        →  system.ini 条目 3,618 B
                                          合计  1,189,400 B / 2,690,454,559 B = 0.044%
```

**两个数不要混**：**1,185,782 = 7 + index_size** 是「按需加载的下限」（表本身，约 1.13 MB）；
**1,189,400** 是**一次真正跑起来**读掉的总字节数（下限 + 那一条 3,618 B 的 `system.ini`）。
（这一条先前写反过一次；更正时把 1,189,400 当成了算错的数 —— 它其实是实测总量，只是不等于下限。
旧文里的 `11 + index_size` = 1,185,786 才是真正对不上的那个。以上均为本次实测值。）

## 九、按本文核对既有实现时发现并修正的两个缺陷

`packages/runtime-artemis/src/pfs.ts` 里原先已有 PFS 读取器，多数部分（表偏移 7、count 取表内偏移 0、
步长 `name_length + 16`、反斜杠归一化、`SHA-1(table)` 派生密钥）**都是对的**。但有两处与本文不符，
均已修正并补了能被旧代码证伪的回归测试。

### 1. 解密判定用了「像不像文本」的启发式，且在真实归档上判反

原逻辑只在「解出来像文本、原文不像」时才用解密结果，否则返回原文。而 `looksLikeText` 把
**任何 ≥ 0x20 且 ≠ 0x7F 的字节**都算可打印——**包含整个 0x80–0xFF 区间**，密文恰好密集在那里。
实测 `pc\ja\artemis.png`：

```text
raw     first 8:  71 3f 22 a8 d0 9c 96 a6     looksLikeText -> true   (可打印占比 0.898)
decoded first 8:  89 50 4e 47 0d 0a 1a 0a     looksLikeText -> false  (可打印占比 0.598)
```

**密文通过了阈值，解出来的 PNG 反而没通过** —— 于是**每一个 PNG／OGG／字体都被当成密文返回**。

正确做法是**由版本决定**，这也正是 GARbro 的做法（`ArcPFS.cs`：只有版本 4、5、8、9 带密钥）。
已改为 `PfsArchive.keyed` 由版本推导，去掉启发式。

### 2. 32 位字段被掩掉了高位，使 ≥ 2 GiB 的偏移/长度静默错误

原实现用 `getUint32(...) & 0x7fffffff` 读取。**GARbro 与 artemis-compat 都读普通 u32**，这个掩码是本实现
自己的发明。而真实归档会越界：

| 归档 | 大小 | offset 或 size ≥ 2 GiB 的条目 |
| --- | --- | --- |
| `root.pfs` | 2.69 GB | **3,940** / 21,587 |
| `root.pfs.000` | 3.36 GB | **6,582** / 7,637 |

**一款游戏就有 10,522 条（约 35%）被静默算错**，且失败是无声的——掩码后的偏移指向错误的字节，不会抛错。
已改为读普通 u32，由 `MAX_TABLE_BYTES` / `MAX_NAME_BYTES` / `MAX_ENTRIES` 负责合理性检查。

### 验证

- **单元**：`.test/artemis-pfs.test.ts` 新增 5 例；把两个缺陷临时还原后这 5 例中 3 例**确实失败**，
  证明它们能证伪旧代码，而不是「新旧都过」的空测试
- **真实归档**：`ports/artemis/build/verify-pfs-real-archive.mjs` 用**生产函数** `readPfsEntryData`
  直接解真实归档的加密条目：

```text
pc/ja/artemis.png   解出 89 50 4e 47 0d 0a 1a 0a   ← PNG 魔数
font/extra_28.rft   解出 72 66 74 33 fc be 07 00   ← "rft3" = RFT 字体自身的签名
```

一个是 PNG 魔数、一个是 RFT 的内部签名，**两个都是二进制资源**——即启发式判错的那一类。
`readPfsTable` 因需整包进内存（>2 GiB 不可能）无法用于该归档，该脚本改为按偏移读取并直接组装 archive 对象。

### 按范围读取已实现（原先的「下一步」已完成）

`readPfsTable(bytes: Uint8Array)` 是**整文件进内存**的 API，对这个 2.69 GB 归档连读都读不进来
（`readFileSync` 在 >2 GiB 时直接 `ERR_FS_FILE_TOO_LARGE`）。**这一条已经解决**，同一文件里已有按范围读取的一对：

| 函数 | 作用 |
| --- | --- |
| `readPfsTableFrom(source: PfsByteSource)` | 只读头 + 索引；`PfsByteSource` 是 `{ size(): Promise<number>; readRange(offset, length): Promise<Uint8Array> }` —— 与 VFS 的 `ByteSource` 结构相同，所以 `GameSource` 用 `gameSourceByteSource(source, path)` 即可适配 |
| `readPfsEntryFrom(source, archive, entry)` | 取单条条目，keyed 容器上按 XOR 还原 |

**实测**：对真实 `root.pfs` 调 `readPfsTableFrom`，发出的范围请求只有 `[[0,7],[7,1185775]]`，
合计 **1,185,782 B ≈ 1.13 MB（0.044%）**；随后 `readPfsEntryFrom` 用**生产函数**取出
`pc/ja/artemis.png` → `89 50 4e 47 0d 0a 1a 0a`（PNG 魔数）、
`font/extra_28.rft` → `72 66 74 33 fc be 07 00`（"rft3" 签名）。

调用方：`packages/runtime-artemis/src/probe.ts:186`（探测时按范围读表），以及两个工具
`ports/artemis/tools/extract-pfs.mjs`、`ports/artemis/tools/pfs-grep.mjs`（两者都从
`packages/runtime-artemis/dist/pfs.js` 导入 `readPfsTableFrom` / `readPfsEntryFrom`）。

**一处需要注意的边界**：XOR 相位是「相对条目起点」的。`readPfsEntryFrom` 总是从 `entry.offset`
读整条，所以相位为 0 永远正确；但若将来要做**条目内部**的子范围读取，相位必须改成
`rangeStart % 20`（GARbro 的 `ByteStringEncryptedStream` 用 `m_base_pos = start_pos % m_key.Length`
处理同一件事）。当前没有这种调用。`packages/runtime-artemis/src/pfs.ts:244-250` 记了这一点。

