# 第三方与许可清单（Third-Party Inventory）

> **本文件不是法律意见。** 它记录的是**仓库自身声明的**许可与来源信息，以及需要人工复核的边界。凡写"未解决"的地方，表示本轮**没有**取得可据以判断再分发的材料；它既不是"违法"，也不是"没问题"。
>
> 审计戳：快照 HEAD `9dd18ecfd3de16f5b271c34e87a31aea30c753d6`，branch `main`（当前工作树 12 modified，见 `../architecture/current-state.md` §1.2）。数据来源：仓库内文件、`git ls-tree -r -l HEAD`、已安装的 `node_modules` manifest **与 LICENSE 文件**、`pnpm-lock.yaml`。**未访问**任何 registry、GitHub API 或网络资源；**未**打印任何密钥值或游戏内容。
>
> **证据分层（本文件统一约定）**：
> - **manifest 声明** = 包自己的 `package.json` 里的 `license` 字段（SPDX 字符串），只代表声明。
> - **许可文件存在** = 磁盘上读到 `LICENSE`/`LICENSE.txt`/`LICENSE.md` 文件，只代表文件存在。
> - **已读** = 本轮实际打开了这些文件/路径（本轮只做到这层，**没有逐行读正文**）。
> - **未枚举** = 传递依赖（未逐个读）。
> - **法律未知** = 未做任何再分发结论。
> 四者**不得**互相替代。
>
> `docs/fetch-engine-artifacts.md` 的历史 hash 是**冻结记录**，本文件只引用、不改写。

---

## 0. 结论摘要

| 类别 | 状态 |
| --- | --- |
| 直接外部 npm 依赖 | **9 行 / 8 个唯一包**（`typescript` 被根与 app 共享，同一解析版 5.9.3）。名字、声明范围、解析版本、角色、**声明许可**与**许可文件路径均已读**（见 §1）。全部是 devDependency。**无运行时第三方 npm 依赖**（所有 `dependencies` 都是 `workspace:*`） |
| 直接依赖的许可 | 已读：**8 个唯一包中 7 个 MIT、1 个（`typescript`）Apache-2.0**。这是**声明字段 + 许可文件路径存在**的层面，**不是**再分发结论，也**没有**逐行读许可正文 |
| 传递依赖的许可 | **未枚举**。`pnpm-lock.yaml` **不记录 license 字段**；要得到传递许可必须逐包读发布 manifest（本轮未做） |
| 站点内是否带入第三方前端运行时 | **未知**：devDependency 的存在/缺失**不是**证明。构建时依赖（如 `svelte`）很可能进入站点产物，但"实际进入了哪些运行时字节"需要**构建产物清单**，本轮**未核**。不得写"无第三方前端运行时" |
| 随仓交付的第三方字节 | **已列出**（见 §2、§3）；其中 Artemis SDK 的再分发权**未解决** |
| 引擎产物与 source offer | **已列出**（见 §4）；compat 路线有 source offer，bridge 路线的许可状态**未解决** |
| 下载工具链 | **已列出**（见 §5），全部有 pin commit |
| 外部 GitHub Actions | **已列出**（见 §6），全部为可变 tag，无 40-hex SHA |
| 字体 / 第三方素材 | **未知**：本轮没有对 `packages/runtime-ons/assets/**` 与 `ports/**/overlay/**` 内的二进制素材做逐文件许可归因（见 §7） |
| 远程发布资产 | **未知**：未查询（见 §8） |

---

## 1. 直接外部 npm 依赖（完整清单：名字 + 解析版 + 声明许可 + 许可文件）

所有 workspace 依赖都是 `workspace:*`。**整个仓库的外部依赖面只有下表 9 行**，且**没有一项**是运行时依赖。下表由最终独立评审（read-only，读已安装的 `node_modules` manifest 与许可文件）补齐；**未访问 registry，未做法律结论**。

| 位置 | 包名 | 声明范围 | 解析版本 | 角色 | **声明许可** | **许可文件（已解析路径）** |
| --- | --- | --- | --- | --- | --- | --- |
| 根 `package.json:24-26` | `@types/jsdom` | `^30.0.0` | `30.0.0` | dev / types | **MIT** | `node_modules/.pnpm/@types+jsdom@30.0.0/node_modules/@types/jsdom/LICENSE` |
| 根 | `jsdom` | `^30.1.0` | `30.1.0` | dev / test env | **MIT** | `node_modules/.pnpm/jsdom@30.1.0/node_modules/jsdom/LICENSE.txt` |
| 根 | `typescript` | `^5.6.3` | `5.9.3` | dev / compiler | **Apache-2.0** | `node_modules/.pnpm/typescript@5.9.3/node_modules/typescript/LICENSE.txt`（+ `ThirdPartyNoticeText.txt`） |
| `apps/app/package.json:25-30` | `@astrojs/check` | `^0.9.4` | `0.9.10` | dev | **MIT** | `apps/app/node_modules/@astrojs/check/LICENSE` |
| `apps/app` | `@astrojs/svelte` | `^7.0.0` | `7.2.5` | dev / 集成 | **MIT** | `apps/app/node_modules/@astrojs/svelte/LICENSE` |
| `apps/app` | `@types/node` | `^22.10.2` | `22.20.3` | dev / types | **MIT** | `apps/app/node_modules/@types/node/LICENSE` |
| `apps/app` | `astro` | `^5.1.1` | `5.18.2` | dev / 构建 | **MIT** | `apps/app/node_modules/astro/LICENSE` |
| `apps/app` | `svelte` | `^5.16.0` | `5.57.0` | dev / UI | **MIT** | `apps/app/node_modules/svelte/LICENSE.md` |
| `apps/app` | `typescript` | `^5.6.3` | `5.9.3` | dev / compiler | **Apache-2.0** | 同 #3 的解析 shim |

- **9 行 / 8 个唯一包**：`typescript` 被根与 app 共用，解析到同一版本 5.9.3。
- **`tsx` 不是依赖**：根 `package.json` 的 devDependencies 恰好是 `@types/jsdom`、`jsdom`、`typescript`；全仓无 `tsx`。
- **声明 vs 已读 vs 法律结论**：上表的许可是各包 manifest **声明字段** + **许可文件路径存在**（两者均已读）；**没有**逐行读正文，**不是**再分发结论。

**关于"角色"与站点包含**：上表全部是 **devDependency**，但 devDependency 这一事实**不证明**站点产物里没有第三方运行时字节。例如 `svelte` 是构建时依赖，很可能被打进站点产物；`astro`、`@astrojs/*` 同理。要回答"站点实际带入了哪些第三方运行时"，需要**构建产物清单**，本轮**未核**。因此本文**不**声称"没有第三方前端运行时"。

**关于 puppeteer**（也在 workflow 运行时安装）：

| 位置 | 包名 | 版本约束 | 说明 |
| --- | --- | --- | --- |
| `.github/workflows/build-wasm-artemis-bridge.yml` `real-game` 作业 | `puppeteer` | **无**（`npm install --no-save puppeteer`） | 手动派发、需非空 `game_tree`；该作业继承 workflow 级 `contents: write` |

**上表为什么不是"之前未读"**：早前草稿把直接依赖的许可写成"传递；本轮未读" —— 那**不**准确，已由最终独立评审 read-only 补全（读已安装 manifest 与许可文件路径）。仍然成立的是两条限制：① 传递依赖许可**未枚举**；② 未做**法律结论**。

---

## 2. Lockfile 范围与去重（数字必须分清）

`pnpm-lock.yaml`（tracked，lockfileVersion `9.0`，15 个 importer）：

| 指标 | 值 |
| --- | ---: |
| `packages:` 条目数 | **506** |
| 唯一包名数 | **454** |
| 有多于一个版本的包名数 | **50**（多为 `@esbuild/*` 平台包） |
| `peerDependencies` 出现次数 | **54** |
| 已安装的 `node_modules/.pnpm` 条目数 | **414** |

**必须区分的三个数字**：

- **506** = lockfile 里的解析条目数（含同名的多版本各算一条）。
- **454** = 去重后的包名数。
- **414** = 本机 `node_modules/.pnpm` 里实际存在的条目数（安装结果，不等于 lock 条目数）。
- 早期报告里的 **"1236 个已安装 manifest"** 是**第四个**数字：它数的是每个嵌套 manifest 文件，**不是** lock 包数，**不是** 唯一组件数。这三个/四个数字**不可互换**。

**`license` 字段不是授权**：即使读到某个包的 `license: MIT`，它也只表示该包**声明**的许可，不等于本项目对它的再分发结论。而且 **`pnpm-lock.yaml` 对任何包都不记录 license 字段**，所以 lockfile **完全无法**回答许可问题。

**传递依赖覆盖：明确未完成。** 本轮没有逐包读发布 manifest，因此**不声明**传递依赖的许可分布。任何"依赖许可已审"的表述都是不实的。

---

## 3. 随仓交付的第三方字节

### 3.1 Artemis 官方 SDK（**在 HEAD 内**，再分发权**未解决**）

由 `git ls-tree -r -l HEAD -- ports/artemis/vendor` 证明**在 HEAD**（9 个 blob，约 24 MB）：

| 文件 | 字节 |
| --- | ---: |
| `Android/arm64-v8a/libartemis.so` | 8,149,024 |
| `Android/armeabi-v7a/libartemis.so` | 7,395,052 |
| `Copyright.txt` | 20,717 |
| `WebAssembly/artemis.html` | 3,912 |
| `WebAssembly/artemis.js` | 264,298 |
| `WebAssembly/artemis.wasm` | 2,682,787 |
| `WebAssembly/asyncify/artemis.html` | 3,912 |
| `WebAssembly/asyncify/artemis.js` | 276,695 |
| `WebAssembly/asyncify/artemis.wasm` | 5,998,665 |

- `Copyright.txt:1-2` = `Artemis Engine` / `Copyright (C)2009 Daiju Hanaoka (Mikage) All rights reserved.`，其后是捆绑库 notices（CGFontGetGlyphsForUnicharsAlternate / GlyphTable.m — Ryutaro Kurai；FontLabel — Apache-2.0, Zynga 2009；freetype；jpeglib / Independent JPEG Group；libogg 等）。
- `.gitignore:70-76` 用否定规则 `!ports/artemis/vendor/**` 把该子树**明确保留在版本控制内**，并注释 "**this exception is NOT a redistribution grant**"。`.gitattributes` 对 `ports/artemis/vendor/**` 设 `-text -diff`（字节精确 pin）。
- `THIRD_PARTY_NOTICES.md` §"Artemis Engine SDK"：捆绑库的授权"**not a grant to redistribute Artemis itself**"；早期关于商业费用/许可程序的声明**已被撤回**；"**No verified Artemis redistribution grant is recorded here**"；根 GPL 声明"cannot supply that missing permission"，"retaining these bytes is not approval to publish them"。
- **状态：未解决。需要人工确认。** 删除该目录**不**能解决已派生输出的来源问题（`THIRD_PARTY_NOTICES.md` 明示），因此"直接删掉"**不是**缓解措施。

### 3.2 手写 Artemis 产品代码（`ports/artemis/src/`）

- 340 个 `.cpp` + 2 个 `.h`，按 `THIRD_PARTY_NOTICES.md` 声明为产品文件；同时该 notice 说明 asyncify bridge **仍含未替换的 SDK 生成体**，且改名或新写 C++ "**does not establish that all resulting code is independently redistributable**"。
- 该 notice **拒绝**做出"每个产品文件许可状态完全相同"的绝对判断。
- **状态：未解决（部分）。** 本轮**没有**逐文件判断 340 个 `.cpp` 的来源（产品 / wasm2c 转译 / SDK 生成体），因此报不出"有多少是干净的"。

### 3.3 ONS 引擎资产（tracked，字节冻结）

- `packages/runtime-ons/assets/**` **tracked** 且被 `.gitattributes` 设 `-text -diff` 做字节精确 pin；上游是 `YuriSizuku/OnscripterYuri @ 08f744b31cc1907b66a15f0402e62321a131ed81`，许可 **GPL-2.0-or-later**。
- 交付时随附其 LICENSE（`README.md` 陈述），并**有**义务随附对应源码。
- **状态：高置信度可独立分发**（仍需履行 GPL 的源码提供义务）。

### 3.4 其他随仓转录的第三方

| 来源 | 上游 | 随仓内容 | 许可（仓库声明） |
| --- | --- | --- | --- |
| Kirikiroid2 | `zeas2/Kirikiroid2` | **转录** opcode / 宽度 / 容器表 → `packages/tjs2-bytecode/src/{opcodes.ts,container.ts}` | BSD 风格（完整 notice 已复制进 `THIRD_PARTY_NOTICES.md`） |
| garbro | `morkt/garbro` | `packages/vfs/src/archives/nsa.ts` 的 NSA/NS2/SAR 布局为**独立 TS 重实现**，未复制代码 | MIT（notice 已复制） |
| AetherKiri | `AetherKiri/AetherKiri` | 未复制源码；`packages/engine-api/src/options.ts` **逐字复现**选项键字符串；ONScripter 存档命名被转录 | GPL-3.0-or-later |
| Tyranor-Next | `Weiss-UltimateSavior/Tyranor-Next` | 未复制代码（仅标记名/布局事实） | GPL-2.0-or-later |

**"转录"与"重实现"是不同结论**：转录 opcode 表需要上游许可覆盖；独立重实现通常不需要。两者的状态不同，不能合并成一句话。

---

## 4. 引擎下载来源、pin、产物与 source offer

### 4.1 下载台账（`ports/lib/engine-artifacts.json`，**已观察到具体数值**）

URL 形如 `https://github.com/fylcr/online_gal/releases/download/<releaseTag>/<name>`。

**ONS** — 版本 `0.7.7beta`，上游 `YuriSizuku/OnscripterYuri @ 08f744b31cc1907b66a15f0402e62321a131ed81`，emsdk `6.0.9`，tag `engine-ons-0.7.7beta`：

| 文件 | 字节 | sha256 |
| --- | ---: | --- |
| `onsyuri.wasm` | 4,660,610 | `85419fe5de8a29d812ab6e…b8fd8bb644d0fc72d58421a`（中段被读工具的 secret scrubber 替换为占位符；可见前后缀与 `build-packages.yml` 自记的 `85419fe5…` 一致，**该遮蔽已在原始报告披露、未被静默修补**） |
| `onsyuri.js` | 198,947 | `c88398fe55cd958bb9544091bbc8c16bffd8321866cff254db9c29f07602df9b` |
| `onsyuri.html` | 24,399 | `78d57d847ad8518806e27f93e1240eea5eb3c3a63b09039ad4c049333297b6c6` |

**krkr** — 版本 `kirikiroid2-web-20260920`，上游 `fenghengzhi/kirikiroid2-web @ 13dda190f8370d02b6cf59286a088529b355658c`，tag `engine-krkr-20260920`：

| 文件 | 字节 | sha256 |
| --- | ---: | --- |
| `index.wasm` | 22,666,256 | `e8a0e801484d2f6fca6482870c6d8fdd5e149e172dc8e1fa7f0146f0de5e8318` |
| `index.js` | 343,419 | `949fad038c4866685d1f40d96d5bce389ff67f6802dda243116a11d5bce5efea` |
| `vlfs.js` | 64,564 | `00f47397805e0ac87383814d461f7f6eaf72e00af40fb716cea7adf11ffea7ef` |
| `index.html` | 53,590 | `6c8c4994738f0ac6d94ad727fb5d6b35783b6da3b8107567fcb793894093ca25` |
| `shell.html` | 105,920 | `3a49723e96f6399e2fe4ce21218be90e6e1320bdef6dd9650b6a728e63b1fb7c` |
| `assets.zip` | 7,853,397 | `5d375b1ab6af2ba271b532505eb1c80547783b3fbd7fd4c3bf3322087ba8218b` |

**没有 `artemis` 条目** —— ledger 自己的 `$comment` 说明 host build 已退役、"there are no bytes here for a fetch to download"；`fetch-engine.mjs` 只接受 `ons` 或 `krkr`。

**已知尖角（ledger `$comment` 自记）**：

- ledger 的 hash 是 **Linux runner** 的构建结果，不是开发机：`onsyuri.wasm` runner 4,660,610 B vs 本机 4,660,612 B（函数索引排列不同），因 emscripten 6.0.9 无可用 `DETERMINISTIC`；krkr 差约 17 KB（vcpkg host triplets 差异）。
- krkr 的可复现性依赖 `SOURCE_DATE_EPOCH`（上游 commit 时间）+ `TZ=UTC` + `VCPKG_KEEP_ENV_VARS`。
- 台账**手工**从 release 的 `manifest.json` 更新，**没有**任何东西强制新鲜度；陈旧条目会静默退化为"fetch exit 3 → 源码构建"。
- 仓库是 private，未鉴权请求 release 资产返回 404；token 从环境变量（`GITHUB_TOKEN`/`GH_TOKEN`）读取，**从不**走命令行。

### 4.2 校验机制（credit）

`fetch-engine.mjs` 在**改名落位前**校验大小 + sha256；两阶段（先全部下载校验、再一起移动）保证不会出现半更新的引擎；exit 2 = 资产不可用（ONS 可回落源码构建），exit 3 = 校验失败（**必须停止，不得静默回落**）。

### 4.3 上游工具链 pin（`.github/upstream-pins.json`）

| 工具 | 上游 | Commit |
| --- | --- | --- |
| Emscripten SDK | `emscripten-core/emsdk` | `6.0.9`（`emcc 6.0.9 (4e4223852a0835923411059a3929907d7df1232e)`） |
| vcpkg | `microsoft/vcpkg` | `b1e15efef6758eaa0beb0a8732cfa66f6a68a81d`（`full-clone: true` 必需；shallow clone 会让 `read-tree` exit 128） |
| wabt | `WebAssembly/wabt` | `02418f72a70466b45805891843f114c7f602eb34`（`1.0.42-8-g02418f72`），另硬编码为 `build-artemis-bridge.sh` 的 `WABT_COMMIT` |

`ports/artemis/build/` 与 `ports/artemis/reverse/build/` 下另按 `fetch-*.sh` 下载 wabt/Zig（gitignored，约 570 MB）。

### 4.4 引擎与交付路线（每条单独一行的结论）

| 引擎 | 上游 | pin | 仓库声明的许可 | 随附许可文件 | 本地改动 | 交付 | **能否独立分发** |
| --- | --- | --- | --- | --- | --- | --- | --- |
| **ONS** | `YuriSizuku/OnscripterYuri` `master` | `08f744b31cc1907b66a15f0402e62321a131ed81` | GPL-2.0-or-later | 是 | host shim `ports/ons/build/ons_host_shim.{h,cpp}` force-include 到 `SDL_WaitEvent` 之前；上游 `script/cross_web.sh` **未改** | release `engine-ons-0.7.7beta` + `runtime-ons` 私有 tarball | **是（高置信度）**，仍需履行 GPL 源码义务 |
| **KiriKiri** | `fenghengzhi/kirikiroid2-web` `web` | `13dda190f8370d02b6cf59286a088529b355658c` | 上游 README 写 "MIT"，**实际 LICENSE** 是日文 BSD 风格 Kirikiri/W.Dee + 依赖 notices；仓库**明确警告**不要用 README 标签或笼统的 MIT 假设 | 是（保留上游 LICENSE + notices） | VLFS host-source 缝 `ports/krkr/kirikiroid2/patch-vlfs-host-source.mjs`；vcpkg overlay `overlay-ports/gettext/**`（GPL-3.0-only）与 `overlay-ports/libexif/**`（LGPL-2.1-or-later）；emsdk port-flag 修正 | release `engine-krkr-20260920`（6 个页面资产）+ 单独 `runtime-kag` tarball；**需要页面部署 + `pageUrl`** | **需人工确认**（BSD 风格 + 下载的 port notices；tarball 明确非自包含） |
| **Artemis — 出货（compat）** | `Weiss-UltimateSavior/artemis-compat` `main` | `17200e64ea99681e3c27d694433c0ff78d7a7cee` | GPL-3.0（上游） | 是：`LICENSE` + `THIRD_PARTY_NOTICES.md` 被 stage 进 `dist/artemis-compat/`；上游 Lua 5.1.5 MIT `COPYRIGHT` 复制进 bundle | 项目自撰 GPL-3.0-or-later：`overlay/host/web_main.cpp`、`overlay/cmake/web.cmake`、`overlay/audio/audio_web.{cpp,h}`、`audio_validation.h`、`patches/0001-pf8-readat-host-source.patch`、`0002-numeric-strto.patch`，以及 `0003-tween-loopdelay.patch`、`0004-animation-dispatch-observe.patch`（列在 `scripts/check-sources.mjs`） | `dist/artemis-compat/`（6 文件），**无 GitHub Release** | **从未发布**：`publish: false`，直到真游戏启动 |
| **Artemis — legacy（bridge）** | 同一上游，但 workflow 默认 `9f622629f595db85e6f76c45c06edde29d7d9961` | pins 注释**称**与 compat 同一 commit（**与实际不符**，见 F-GOV-3） | **未解决** —— 派生自专有 SDK 二进制 | 未新增 | `ports/artemis/src/**` 上的 wasm2c 转译（frozen 参考 md5：asyncify `d895a74b11ba767b06c82d722c407517` 107,593,501 B；sync `080822b6e14345d339312d1e1bcfd0aa` 47,741,355 B），**未替换的 SDK 生成体仍在** | `dist/artemis-bridge/build/bridge.wasm`；仅手动派发；`publish` 受 `vars.ARTEMIS_PUBLISH_ARTIFACTS == 'true'` 门控 | **需人工确认 —— 仓库自己说再分发的可行性未解决** |
| **Siglus** | `xmoezzz/siglus_rs` | — | MPL-2.0，**未包含** | — | 仅识别/读配置，无执行路径 | 无 | N/A（未交付代码） |
| **Tyranor-Next** | `Weiss-UltimateSavior/Tyranor-Next` | — | GPL-2.0-or-later，**未复制代码** | — | 独立 TS 探针 | 无 | N/A |
| **AetherKiri** | `AetherKiri/AetherKiri` | — | GPL-3.0-or-later，**未复制源码**；选项键字符串逐字复现；ONScripter 存档命名被转录 | — | 适配层设计 | 无 | N/A |
| **Kirikiroid2** | `zeas2/Kirikiroid2` | — | BSD 风格（完整 notice 已复制） | 在仓库内 | opcode/宽度/容器表**转录** | 在仓库内 | 是（保留 BSD notice） |
| **garbro** | `morkt/garbro` | — | MIT（notice 已复制） | 在仓库内 | NSA/NS2/SAR 布局的独立 TS 重实现，**未复制代码** | 在仓库内 | 是 |

**source offer 机制（credit）**：`make-source-bundle.mjs` 产出 pinned `git archive` + patches + overlays + `build.sh`/`RUN-BUILD.sh` + `upstream-root.diff` + `NOTICE`；`build-artemis-compat.sh` 写出 `artemis-compat-source.tar.gz.sha256` 并用 `sha256sum --check --quiet` **自校验**；`build-artemis-compat.sh:32-42` 在 `UPSTREAM_COMMIT != ARTC_PIN` 时**直接判失败**，理由是"a provenance record that names a commit nobody built is worse than no record"。

### 4.5 许可关键路径的两处断言

1. `THIRD_PARTY_NOTICES.md:135` 把 compat 的上游 pin 写成 `9f622629f595db85e6f76c45c06edde29d7d9961` —— 那是 **bridge** 的 pin，不是 compat 的 `17200e64…`。这是一处**真实的文档/CI 冲突**；**未经批准不改**该文件。
2. `package-dir: packages/runtime-artemis` 被**两个** Artemis workflow 共用，因此同一个包名既可能承载 compat 模块，也可能在 `assets/` 下承载 bridge 的 `bridge.wasm`。

---

## 5. 工作流引用的外部 Action 与下载工具

| Action | 引用形式 | 是否为不可变 SHA | 出现位置 |
| --- | --- | --- | --- |
| `actions/checkout` | `@v4` | **否** | `build-packages.yml:155`、`build-wasm-artemis-bridge.yml:113`、`build-wasm-reusable.yml:191`、`check-upstream-drift.yml:39` |
| `actions/setup-node` | `@v4` | **否** | `build-packages.yml:158`、`build-wasm-artemis-bridge.yml:116` |
| `actions/cache` | `@v4` | **否** | `build-wasm-reusable.yml:243,300` |
| `actions/upload-artifact` | `@v4` | **否** | `build-wasm-reusable.yml:540,635`、`build-packages.yml:320`、`build-wasm-artemis-bridge.yml:154` |
| `actions/download-artifact` | `@v4` | **否** | `build-wasm-artemis-bridge.yml:127` |
| `lukka/run-vcpkg` | `@v11` | **否** | `build-wasm-reusable.yml:309` |
| 本地可复用工作流 | `./.github/workflows/build-wasm-reusable.yml` | N/A | `build-wasm-artemis-compat.yml:94`、`-bridge.yml:69`、`-krkr.yml:75`、`-ons.yml:88` |

**工作流权限**：`build-wasm-reusable.yml:167-168` `contents: write`；`check-upstream-drift.yml:31-34` `contents: read` + `actions: write`；其余工作流为 `contents: write`（因为它们创建 release）。

**下载工具**：见 §4.3；另有 `puppeteer`（无版本，见 §1 注释行）。

**未记录**：仓库级的 "allowed actions" 策略（远程，未查）。

---

## 6. 字体与第三方素材（**未完成**）

本轮**没有**对下列二进制素材做逐文件许可归因，因此**不能**声称"素材许可已审"：

- `packages/runtime-ons/assets/**`（tracked、字节冻结）——上游 ONS 资产的许可需按其自身 notice 判断。
- `ports/artemis/compat-web/overlay/**` 与上游 Lua 5.1.5（MIT）——compat bundle 里复制了其 `COPYRIGHT`。
- `ports/artemis/vendor/artemis-sdk/` 内的 HTML/JS（见 §3.1）。
- 游戏数据**不在**仓库内（`.ref/`、`.test/` 均 gitignored），本文件不涉及游戏内容。

**状态：未知。** 要完成它需要逐文件打开并读其随附 notice。

---

## 7. 明确不做的事

- **不打印**任何 secret、token、游戏内容或用户数据。
- **不改写** `docs/fetch-engine-artifacts.md` 的历史 hash，**不改写** `.github/upstream-pins.json` 的 `expected`（md5/字节数）。
- **不做法律结论**。本文件只记录仓库自己的声明与需要人工复核的边界。
- **不把"删掉 vendor 目录"当作许可缓解**：`THIRD_PARTY_NOTICES.md` 明示删除**不**解决已派生输出的来源问题。

---

## 8. 远程侧未知

**未查询**（需要单独授权）：release 资产是否存在及其内容、仓库变量 `ARTEMIS_PUBLISH_ARTIFACTS` 的值、branch/tag protection、Actions allow-list、Dependabot 告警、任何备份状态。

这些**不是"不存在"**，而是**未知**。

---

## 9. 完成本清单还需要的材料

1. 逐包读发布 manifest 与 LICENSE，产出**传递依赖**许可分布（直接依赖 9 行已完成，见 §1；传递依赖仍未做）。
2. 逐文件归因 `ports/artemis/src/**` 的 340 个 `.cpp`：产品 / wasm2c 转译 / SDK 生成体。
3. 逐文件归因 `packages/runtime-ons/assets/**` 与各 overlay 内的素材与字体。
4. 取得 Artemis SDK 的书面再分发许可，或作出明确的人工保留决定。
5. 裁定 compat pin 的文档冲突（`9f622629…` vs `17200e64…`）。
6. 在授权后查询远程发布与保护状态。
