# Proposal

## Why

VFS 已有归档输入的偏移读取接口。当前 ZIP 可分段读取索引，但提取 DEFLATE 成员时仍将该成员的压缩和解压字节整段放入内存。游戏读取大型压缩包的局部内容时，需要按偏移读取压缩数据，并分块解码所选成员。

## What Changes

- 在 `packages/vfs` 为 ZIP、7z、RAR4 和 RAR5 提供只读归档挂载、按需读取、成员流式解压与字节范围读取。输入复用本地 `File`/`Blob` 和既有网络 `GameSource`。
- ZIP 优先使用 `@zip.js/zip.js`，包括 Zip64 和经验证的加密 ZIP。7z 优先验证 `7z-wasm`，RAR 优先验证 libarchive 的读回调和分块输出。候选库通过真实浏览器的读取、资源预算和许可验证后接入。
- 通过现有归档入口和 `GameSource` 交付。按成员区分可直接读取的字节范围与需要从压缩块起点解码的字节范围。引擎仍通过已有宿主接口读字节，运行时写入落在本地覆层。

## Capabilities

### New Capabilities

- `vfs-streaming-archive-decompression` 定义归档输入、成员流式输出、有界范围读取及格式兼容性。

## Impact

- `packages/vfs/src/archives/` 负责格式探测、索引与成员读取，`packages/vfs/src/sources.ts` 负责归档来源的能力传播，`packages/core` 负责将可用格式接入既有游戏来源入口和恢复流程。
- 解码在浏览器端按需加载。网络归档复用直链与 `integrate-webdav-source` 的传输契约，站点继续通过 Pages 静态分发解码器。
