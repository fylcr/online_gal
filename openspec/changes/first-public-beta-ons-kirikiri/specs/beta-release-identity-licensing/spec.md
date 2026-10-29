# Spec Delta

## Purpose

定义公开 beta 的版本身份、随站许可文件、GPL 源码提供，以及 KiriKiri 再分发许可的人工确认闸门，使“公开”只在身份可追溯、许可义务可履行时发生。

## ADDED Requirements

### Requirement: Beta has one recorded version identity
公开 beta MUST 有一个 SemVer 先行版应用版本（拟议 `0.2.0-beta.1`，最终值由用户决定），并 MUST 记录所含引擎的 release tag、上游仓库与 commit。站点 MUST 在可见位置显示该应用版本与两个引擎 tag。引擎 release MUST 沿用 `engine-ons-<SemVer>` / `engine-krkr-<SemVer>` 不可变方案，MUST NOT 覆盖已有 tag 或 asset。

#### Scenario: Version visible on site
- **WHEN** 用户打开 beta 站点的设置或关于区域
- **THEN** 能看到应用版本，以及 ONS 与 KiriKiri 的 release tag 与上游 commit

#### Scenario: Existing tag reused
- **WHEN** beta 选择沿用 `engine-krkr-20260920` 历史 pin
- **THEN** 该 tag 的 asset 字节不被改写，站点记录的 sha256 与台账一致

### Requirement: Licence texts ship with the site
beta 站点输出 MUST 包含仓库 `LICENSE`（GPL-3.0-or-later）、`THIRD_PARTY_NOTICES.md` 的内容，以及 OnscripterYuri（GPL-2.0-or-later）与 kirikiroid2-web 上游 LICENSE 原文及其 port notices（gettext overlay GPL-3.0-only、libexif LGPL-2.1-or-later），并可从站点页面链接访问。

#### Scenario: Licence page reachable
- **WHEN** 用户在 beta 站点点击许可链接
- **THEN** 能读到上述每一份许可原文

### Requirement: Source offer covers every shipped binary
对 beta 站点分发的每个二进制（`onsyuri.wasm`、`index.wasm` 及其 JS 胶水），MUST 提供对应源码的获取方式：上游 commit、本仓库补丁/overlay、构建脚本与 sha256。获取方式 MUST 在站点上写明，且在仓库保持 private 时仍对 beta 用户可用。

#### Scenario: User requests source
- **WHEN** beta 用户按站点上的源码说明操作
- **THEN** 能取得与所分发二进制对应的完整构建源与说明，无需仓库访问权限

### Requirement: KiriKiri licence confirmation gates going public
在 KiriKiri 再分发许可获得用户（项目 owner）书面人工确认之前，beta MUST NOT 公开，含 KiriKiri 产物的部署 MUST NOT 被标为公开 beta。确认结论与日期 MUST 记录在 `docs/governance/third-party-inventory.md`。

#### Scenario: Confirmation missing
- **WHEN** 第三方清单中 KiriKiri 仍为“需人工确认”
- **THEN** 发布清单的许可项为未通过，公开步骤不得执行

#### Scenario: Confirmation recorded
- **WHEN** owner 记录了 KiriKiri 许可可再分发的确认
- **THEN** 发布清单的许可项可标为通过
