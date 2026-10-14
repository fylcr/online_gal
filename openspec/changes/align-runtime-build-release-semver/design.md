# Design

## Context

见 `proposal.md` 的 Why。本设计只规划未来 apply，不改 workflow。

当前事实（HEAD `73edc9017652035cebc67e83e4c36276dbdaad7c`）：

- `.github/workflows/build-wasm-ons.yml:128` 与 `build-wasm-krkr.yml:105` 在 `push` 时把 `publish` 设为真。ONS 版本硬编码 `0.7.7beta`，tag `engine-ons-0.7.7beta`；KRKR 版本硬编码 `kirikiroid2-web-20260920`，tag `engine-krkr-20260920`。
- `.github/workflows/build-wasm-reusable.yml:643-668` 用 `gh release view || gh release create` 后 `gh release upload --clobber`。这会覆盖同一 tag 的 asset。
- `packages/runtime-ons/package.json` 版本是 `0.1.3`，构建含 `scripts/copy-runtime-assets.mjs`。`packages/runtime-kag/package.json` 版本是 `0.1.1`，只有 `tsc -b`，没有 `scripts/`。
- ONS CI 用 `--assets-from dist/ons`，所以 committed `assets/**` 不是当前发布输入。KRKR 不把 wasm 打进 runtime tarball，loose engine files 仍随 release 上传。
- `ports/lib/engine-artifacts.json` 把上述两个旧 tag 与 hash 固定为 fetch 目标。`build-packages.yml:354` 已是仅手动 `publish==true` 才发布。
- drift workflow 只传 `upstream_commit`。ONS/KRKR 的 dispatch `publish` 默认 true，所以缺省输入今天会发布。compat 是字面量 `publish: false`；bridge 只有手动入口。
- GitHub `paths` 超过约 300 个变更文件时可能不触发。该行为本轮未远程实测，规划为未知，不用它代替精确区间判定。

SemVer 2.0.0 中文页（https://semver.org/lang/zh-CN/，本轮实际读取）规定：核心为无前导零的 `X.Y.Z`；先行版是连字符后的点分标识符；`+` 后是比较时忽略的构建元数据；`v1.2.3` 不是 SemVer。先行版低于同核心正式版。`1.0.0-alpha < 1.0.0-alpha.1 < 1.0.0-alpha.beta < 1.0.0-beta < 1.0.0-beta.2 < 1.0.0-beta.11 < 1.0.0-rc.1 < 1.0.0`。因此 `0.0.1-beta-202610140759` 合法，但是一个标识符，不等于 `0.0.1-beta.202610140759`。

## Goals / Non-Goals

**Goals:**

- 用一份可执行计划把昂贵构建、预检与发布串成失败关闭的序列。
- 让 apply 能复用现有 workflow、Node 与 Git，而不新增 Action 或依赖。
- 把测试矩阵固定到可观察结果：0 或 1 次昂贵构建、0 或 1 次对应 release。

**Non-Goals:**

- 不实施、不构建、不发布、不推送。
- 不把 ports/共享包/lock 重新纳入自动昂贵构建。旧 scout 的“共享 lock 仍可构建”已被本用户目标覆盖。
- 不修改旧 34 项任务，不把 Artemis compat/bridge 打开，不自动更新 catalog，不回写 tracked package version。
- 不新增 alpha 通道 UI。alpha 只保留为未来显式配置，不是本次行为。

## Decisions

1. **路径过滤只负责减少无关 workflow，精确区间负责授权。**
   - push `paths` 收窄到对应发布输入，避免 Astro/lock/port/workflow 单独变化启动昂贵 job。
   - job 内仍用事件 `before`/`after` 对完整区间做二次核对。超过 paths 文件数限制而未触发时，结果是不构建、不发布；这符合“无昂贵意外”，但不能被写成 GitHub 已保证的路径语义。
   - 备选是保留宽 paths、job 内跳过发布。用户已否决：非发布输入不得启动昂贵构建。

2. **版本与碰撞预检位于 native 构建之前。**
   - 自动模式从对应 `package.json` 取 BASE，生成一次 `BASE-beta-YYYYMMDDHHmm`。
   - 手动模式只接受显式 `publish=true` 与完整 SemVer（含 `+` 构建元数据）；blank/default 不能落到 `0.7.7beta` 或 `kirikiroid2-web-*`。
   - 碰撞按完整版本字符串的精确身份比较，不按 SemVer precedence。`1.0.0+a` 与 `1.0.0+b` precedence 相同，但是两个合法且不同的版本。
   - 预检 tag、release、同名 asset 与版本历史。任一精确身份已存在即失败，不追加秒或 run id。
   - 按引擎 concurrency 保留，发布前再查一次。第二次检查若发现已有 tag/release/asset，失败关闭且不覆盖其字节。
   - 上传不是事务。若本次新 release 的 `gh release create` 已成功而后续 upload 失败：退出非 0，保留该新建 release 与已上传 asset；不 clobber、不 delete、不宣称这次发布不存在。不承诺发布原子性、删除回滚或自动再次覆盖恢复。
   - 备选是构建后才查 tag。它浪费昂贵构建，否决。

3. **时间固定为构建开始时的上海时区。**
   - 使用 `TZ=Asia/Shanghai` 与 `YYYYMMDDHHmm`，不是 UTC，也不是 14 位秒格式。
   - 同一分钟碰撞按决策 2 拒绝，不改变用户格式。
   - 备选 `beta.YYYYMMDDHHmm` 会变成另一个点分标识并改变 SemVer 排序，否决。

4. **发布身份只写 release 产物，不写 Git commit。**
   - tag、packed manifest、tarball 文件名、release manifest 使用同一个完整版本字符串，保留 `+` 与 `-`，不得 strip，也不得把 `+metadata` 规范化掉。
   - Git tag 必须能通过 `git check-ref-format`。本机已验证 `refs/tags/engine-ons-1.0.0+a` 合法；不得把 `+` 当成非法 ref。下载 URL 只编码 path segment（`+` → `%2B`），存储版本不变。path 中的 `+` 不是 query/form 里表示空格的 `+`。不承诺 GitHub 远端对 `+` asset 名已实时验证。
   - 上游 commit、emsdk 与旧引擎标签进入 provenance 字段。
   - `make-package.mjs` 接受发布时 version override；tracked `package.json` 不由 CI 提交。
   - 备选是发布成功后自动 bump commit。它把发布与源码历史耦合，否决。

5. **消费兼容保持 pin，不追随 latest。**
   - 新 tag 不影响 `engine-artifacts.json` 的旧 pin。
   - catalog 推广是后续独立、人工核验的变更，不在本 change 的 apply 中顺手修改。
   - 备选是发布时自动改 ledger。它可能把未核验 wasm 变成生产 fetch 目标，否决。

6. **drift 与 Artemis 边界。**
   - drift job 删除自动 `workflow run` 派发，只保留报告。
   - 不可变模式（禁止 `--clobber` 与 `view || reuse`）只由 ONS/KRKR caller 显式打开。允许修改 reusable 的共用步骤，但 Artemis 路径行为不变：compat 继续字面量 `publish: false`，bridge 继续现有手动 opt-in 与现有 clobber 语义，不扩大新的自动或手动权限。
   - 当前目标代码未落地。上述是未来 apply 的 SHALL/MUST，不得写成已实现。
   - 备选是给 drift 传 `publish=false` 但仍派发昂贵构建。用户要求 drift 不派发构建，否决。

7. **旧政策冲突单独悬挂。**
   - 旧 delta 要求普通 push 不发布，task 5.2 要求只手动发布。本设计的 runtime 自动发布取代这一部分。
   - apply 不编辑旧 change。整体计划合并仍需用户另一次授权。

## Risks / Trade-offs

- [paths 超过文件数限制时漏触发] → 接受为不构建、不发布；apply 记录该未知，不另造补偿调度。
- [浅 checkout 缺少 before] → 预检必须先取两端；取不到即失败，禁止用 HEAD 代替。
- [同一分钟重跑不能发布] → 保留旧字节；操作者改用显式手动 SemVer 或等待下一分钟。
- [兄弟包、根 lock、共享依赖、许可证、`.github/scripts/**` 或 port 单独变化不自动发布] → 下一次真正的 runtime release 打包的是当时 checkout 的闭包、`LICENSE` 与 `THIRD_PARTY_NOTICES.md`，不是一份被单独标成“过期依赖文件”的快照。runtime 与 wasm 可能暂时不匹配；这是用户选择，不扩大精确 allowlist，也不自动改 catalog pin 或生产默认。
- [GitHub latest 指向旧正式 tag] → spec 不承诺 latest；消费方显式选择版本。
- [上传中途失败] → 已存在字节不覆盖；新建 release 若已创建则保留并退出非 0。不承诺事务、删除回滚或自动再覆盖。
- [version、SHA 与路径进入命令] → `before`、`after`、version、git SHA 与路径只作为预检程序参数传入，不拼进 shell。本轮不新增 CODEOWNERS 或 Action SHA pin。
- [旧 baseline 与本 change 同时存在] → 文档标明冲突与未批准状态，避免实现者把 34 项任务当已批准依赖。

## Migration Plan

1. apply 先加纯预检与版本函数，并用 fixture 覆盖矩阵；不接真实发布。
2. 再改 ONS/KRKR caller 与 reusable 顺序：预检、构建、二次存在性检查、创建新 release。
3. drift 改为只报告。compat/bridge 只做回归断言，不改发布能力。
4. 回滚是恢复 caller 的旧路径与旧发布表达式；已创建的新 tag 不删除、不覆盖。
5. 历史 pin 在迁移后继续有效。生产默认引擎不变，直到独立 catalog 审批。

## Open Questions

无。远程 branch protection、Actions allow-list 与 GitHub paths 超限的精确平台行为保持未知，但不改变上述失败关闭行为。
