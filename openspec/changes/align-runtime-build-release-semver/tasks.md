# Tasks

## 1. 版本与区间预检

- [x] 1.1 在现有 Node/Git 可复用的 `.github/scripts/` 辅助模块实现 SemVer 校验、BASE 提取与自动版本：接受 `0.0.1-beta-202610140759` 与完整合法 SemVer（含 `1.0.0+a`），拒绝 `v` 前缀、前导零和空白；从含先行版或 `+metadata` 的 package version 只取 `major.minor.patch`；用构建开始时一次读取的 `TZ=Asia/Shanghai` 时钟生成默认 beta 形式 `BASE-beta-YYYYMMDDHHmm`。验证：fixture 覆盖合法/非法、`+` 保留且不按 precedence 归一、ONS `0.1.3`、KAG `0.1.1`、UTC+8 日界与同一分钟格式稳定，且不改成 `beta.time`、不追加秒、不新增 alpha UI。
- [x] 1.2 实现精确 `before..after` 分类：只承认对应发布输入，覆盖多提交、删除与重命名两端；全零、缺历史、非法 SHA 失败。验证：ONS-only、KRKR-only、sibling-only、README/docs/assets/ports/lock/licenses/`.github/scripts` 均给出 0/1 预期；`before`、`after`、version、SHA 与路径只作为程序参数，命令不使用 `HEAD~1`，也不把这些值拼进 shell。
- [x] 1.3 实现发布前碰撞预检与第二次存在性检查的纯函数/命令包装：按完整版本字符串精确身份判断 tag、release、asset 或历史版本已存在时失败，不按 SemVer precedence 合并 `1.0.0+a` 与 `1.0.0+b`，不追加秒、run id、bump 或覆盖标志。验证：same-minute、rerun、并发二次检查 fixture 都在“构建”标记前返回拒绝并保留原字节；若模拟的新 release 已创建而 upload 失败，则非 0 退出且不 clobber、不 delete，而不是断言部分结果不存在。

## 2. 接入 ONS/KRKR 构建发布顺序

- [x] 2.1 把 ONS/KRKR push paths 收窄到各自发布输入，并把 job 顺序改为预检、昂贵构建、二次存在性检查、新 release。验证：workflow fixture 中 Astro-only 为 0 build/0 release，runtime-ONS-only 为 1 ONS/0 KRKR；路径超限未触发时记录为不构建且不发布。
- [x] 2.2 仅在 ONS/KRKR caller 的不可变路径移除固定引擎版本兜底、`gh release view || reuse` 与 `--clobber`；canonical version、tag、packed package version、tarball 文件名、release manifest 复用同一完整 SemVer 并保留 `+` 与 `-`，Git tag 通过 `git check-ref-format`，下载 URL 只编码 path segment，上游 ref 只进 provenance，CI 不提交 package version。验证：身份 fixture 全部字段一致且含 `+` 不被 strip；旧 tag 无 upload/clobber/delete；create 成功后的 upload 失败保留新建 release 与已上传 asset 并以非 0 退出。Artemis compat/bridge 语义不在此任务中改变。
- [x] 2.3 手动 dispatch 改为显式 `publish=true` 加该引擎 version；缺省、false、空白、非法版本在构建前退出。验证：manual valid 才有构建标记，invalid/blank/duplicate 为 0 build/0 release。

## 3. 保留相邻门禁与消费边界

- [x] 3.1 drift 改为只报告并删除引擎 `workflow run` 派发；compat 保持 `publish: false`，bridge 保持原手动限制。验证：drift fixture 无 dispatch，Artemis caller 不能从 reusable 适配获得新的自动发布。
- [x] 3.2 确认 `build-packages.yml` push 仍只构建不发布，PR 仍不进入 ONS/KRKR 昂贵 native/WASM；不删除现有类型/unit 测试，也不删除 `build-packages.yml` 既有 g++ portable probes。验证：packages push 与 PR fixture 的 ONS/KRKR release 数为 0，现有测试入口与 g++ probe 仍被 workflow 引用。
- [x] 3.3 增加消费回归：新版本不修改 `ports/lib/engine-artifacts.json` 旧 pin/hash，fetch 仍命中 `engine-ons-0.7.7beta` 与 `engine-krkr-20260920`，正式/先行版必须显式选择。验证：catalog 字节比较与 fetch fixture 通过，且没有 latest 自动提升。

## 4. 集成证据

- [x] 4.1 运行预检、workflow fixture 与消费回归整组，核对矩阵：Astro-only、runtime ONS-only、docs/PR/sibling、manual valid/invalid/blank/duplicate、UTC+8 边界、same-minute/rerun、`1.0.0+a` 与 `1.0.0+b` 精确身份、upload-after-create 失败保留且不 clobber/delete、multi-commit rename、drift no-dispatch、PR 无 ONS/KRKR native/WASM 但保留 packages g++ probes、provenance 一致、collision before build。验证：一条集成命令报告每项 0/1 计数且 exit 0；不调用真实 GitHub Release。

## Workflow follow-up

- 本 tasks 全部完成后仍需新的用户 apply 授权；规划完成不是实施完成。
- 旧 change `establish-architecture-and-release-baseline` 的 34 项任务保持原字节与未勾选状态；其 push 永不发布/task 5.2 需用户另行整体计划更新。
- 生产 catalog 推广不在本 change 内，须人工核验后独立审批。
