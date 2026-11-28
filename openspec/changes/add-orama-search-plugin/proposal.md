# Proposal

## Why

导航栏的「搜索游戏」需要搜索本地游戏库。把全文搜索作为独立的官方 Orama 插件交付，保留管理器首屏与基础库操作，不把搜索引擎绑进管理器或插件内核。

## What Changes

- 新增官方插件 `online-gal.search`，使用 `@orama/orama` 在浏览器中索引游戏名称、标题、标签，借用现有导航栏入口与游戏库结果列表。
- 通过现有官方注册表与插件校验链安装和更新。第一次真正执行非空查询时按需加载；插件不可用时保留管理器的本地基础筛选。
- 索引由当前游戏库记录生成，不扫描或上传游戏文件。不新增搜索后端、云服务、部署密钥或公共搜索 API。

## Capabilities

### New Capabilities

- `local-library-search-plugin`：官方搜索插件的安装、浏览器内全文搜索、本地库同步、降级与发布验收。

### Modified Capabilities

无。沿用 `redesign-frontend` 的本地游戏库搜索交互与 `plugin-system` 的安装、签名、首屏规则。

## Impact

- `plugin-system` 主责：搜索插件包、Worker 消息协议、官方制品发布与站点装配。
- `frontend` 主责：在 `apps/app` 接入现有导航栏和游戏库，维护基础筛选与浏览器交互验收。
- 依赖 `redesign-frontend` 3.5–3.6 的搜索入口与基础筛选。生产制品合入沿用 `plugin-system` K21 / 12.5；搜索不阻塞固定外壳的独立交付。
