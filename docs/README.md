# 文档索引

本目录文档按「先读 / 按引擎 / 审计基线 / 状态与交接」分组。带日期的交接与记录是历史快照，以各文自己的状态标注为准（不少文件自述为历史、已被取代、拟议或路线已改向）。`.test/`、`.ref/` 的内容不入库，上传边界见本索引第一节的 `source-layout.md`。

## 一、先读（边界与入口）

- [source-layout.md](source-layout.md) — 源码在哪里、构建入口、哪些本地资料不能提交
- [engine-packages.md](engine-packages.md) — `@online-gal/engines` 聚合入口与引擎选择
- [fetch-engine-artifacts.md](fetch-engine-artifacts.md) — 下载已发布引擎并校验大小与 sha256

## 二、ONScripter

- [ons-source-build.md](ons-source-build.md) — 从源码构建 ONS web 目标；开篇章节才是当前状态
- [ons-worker-host.md](ons-worker-host.md) — worker 内按字节范围加载；中段诊断已被后续工作取代
- [ons-runtime-provider.md](ons-runtime-provider.md) — ONS 打包为平台运行时 provider
- [ons-emulation-baseline.md](ons-emulation-baseline.md) — 基线测量；自述该缺口已由后续工作闭合
- [sync-bridge-main-thread-constraint.md](sync-bridge-main-thread-constraint.md) — 同步字节范围读取的桥接约束分析

## 三、KiriKiri

- [kirikiri-web-plan.md](kirikiri-web-plan.md) — KiriKiri 浏览器化：真实引擎跑通、XP3 按需读取

## 四、Artemis（半成品，状态以各文标注为准）

- [artemis-web-port.md](artemis-web-port.md) — 自述路线已改向，正文按历史保留
- [artemis-pfs-format.md](artemis-pfs-format.md) — PFS（pf8）归档格式的规格依据
- [artemis-wasm-transcription-handoff.md](artemis-wasm-transcription-handoff.md) — 自述已执行完毕，现为结案记录
- [artemis-official-compat-handoff.md](artemis-official-compat-handoff.md) — 4670 接口交接；文中当前摘要为唯一现状

## 五、AetherKiri / Godot 调研记录

- [aetherkiri-artemis-availability.md](aetherkiri-artemis-availability.md) — 结论：运行时为私有代码，不可复用
- [aetherkiri-web-boot-failure.md](aetherkiri-web-boot-failure.md) — alpha 构建自身启动失败的诊断
- [godot-host-module-failure.md](godot-host-module-failure.md) — 哪个 WebAssembly 模块失败，以及原因

## 六、审计基线

审计时点 HEAD `9dd18ec`。其中 target-state、migration-plan、ci-release-policy 自述为拟议，不是已落地事实。

- [architecture/current-state.md](architecture/current-state.md) — 审计时点的当前状态基线（含证据等级）
- [architecture/target-state.md](architecture/target-state.md) — 自述希望成立的架构，不是当前状态
- [architecture/migration-plan.md](architecture/migration-plan.md) — 自述没有任何阶段被批准的迁移计划
- [architecture/module-dependency-rules.md](architecture/module-dependency-rules.md) — §1–§3 实测事实，§4–§7 拟议规则
- [governance/third-party-inventory.md](governance/third-party-inventory.md) — 第三方与许可清单，非法律意见
- [governance/repository-risk.md](governance/repository-risk.md) — 仓库风险台账，逐条给出触发条件
- [governance/ci-release-policy.md](governance/ci-release-policy.md) — 拟议 CI 与发布政策，自述无一条已落地
- [development/workflow.md](development/workflow.md) — 实际存在的命令及其真实副作用
- [development/testing.md](development/testing.md) — 测试分层与兼容性现状
- [establish-architecture-and-release-baseline](../openspec/changes/establish-architecture-and-release-baseline/) — 对应 OpenSpec 变更；提案未批准，任务 0/34 已勾选

## 七、状态与交接

- [current-work.md](current-work.md) — 滚动检查点；文首「当前摘要」块为唯一现状
- [session-5697f1a7-handoff.md](session-5697f1a7-handoff.md) — 历史交接，2026-09-17 到 2026-10-04
- [项目状态与恢复指南.md](项目状态与恢复指南.md) — 更早的能力矩阵与实测数据
