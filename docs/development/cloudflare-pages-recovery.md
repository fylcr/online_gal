# 从本机恢复 Cloudflare Pages 初始化和私有入口

在已登录 `cf` 的所有者账户中操作。命令通过仓库锁定的 `cf@1.0.0-rc.2` 读取 Pages 生产项目。`PROJECT`、`ACCOUNT_ID`、`HOST` 分别替换成已确认的项目、账户和生产域名。有多个 `cf` 登录时加 `--profile` 指定当前账户使用的登录配置。先在隔离项目演练，生产重新初始化按 OpenSpec 7.6 执行。

## 关闭初始化标签页后续跑签名部署

运行 `npm run build:packages` 生成验签器，然后检查当前步骤。

```sh
npm run recover:pages-release -- PROJECT --account ACCOUNT_ID --host HOST
```

只读命令根据生产初始化记录的时间和分支核对同一轮部署。输出包括当前阶段、对应的部署 ID，验签通过后还有官方包的 ID、版本和摘要。如果历史存在多条无法区分的候选，或生产域名指向另一部署，命令会停止。先在 Pages 核对项目、变更时间和部署 ID，再继续。

| 阶段 | 操作 |
| --- | --- |
| `bootstrap-needed` | 核对生产引导标记和部署历史，确认缺少可复用的首次部署后，触发记录里的生产分支。 |
| `bootstrap-running` 或 `signing-running` | 等输出的部署 ID 完成，然后重跑只读检查。 |
| `bootstrap-ready` | 首次部署身份和 CA 就绪检查已通过。只删除生产引导标记，再重跑只读检查。 |
| `signing-needed` | 引导标记已删除并确认首次部署仍在生产域名生效。触发第二次生产部署。 |
| `bootstrap-history-missing` | 当前没有可复用的首次部署，先检查 Pages 历史和项目配置。 |
| `awaiting-dns` | 签名制品已在部署专属地址验签，生产域名在验签前后均匹配。按 OpenSpec 9.5 继续 DNS 验收。 |

下列 `cf` 命令适用于 `1.0.0-rc.2`。使用与只读检查相同的账户和登录配置。若从终端直接运行，在该终端设置 `CLOUDFLARE_ACCOUNT_ID` 并指向已核对的账户。第一次站点部署和签名部署的构建命令相同，必须先按上表判断处于哪个阶段。

```sh
cf pages deployments create PROJECT --branch BRANCH
```

只在得到 `bootstrap-ready` 且重新核对目标账户、项目、分支和引导部署 ID 后清除标记。将下列内容保存为本机的 `set-signed-phase.json`，然后通过 `--body @文件路径` 传入。该补丁只有生产引导标记这一项。

```json
{"deployment_configs":{"production":{"env_vars":{"ONLINE_GAL_CA_BOOTSTRAP":null}}}}
```

```sh
cf pages edit PROJECT --body @set-signed-phase.json
npm run recover:pages-release -- PROJECT --account ACCOUNT_ID --host HOST
```

出现 `signing-needed` 后触发第二次部署，等该部署完成再重跑只读检查。失败部署由操作者核对后重试。独立复验单个签名部署时，可将已从 Pages 和部署身份端点核对的 HTTPS 专属地址传给现有脚本。`SALT` 和 `HASH` 取自这一轮的公开信任候选。

```sh
node packages/plugin-system/scripts/check-release.mjs HOST SALT HASH --deployment-origin https://DEPLOYMENT_ID_PREFIX.PROJECT.pages.dev
```

## 旧项目补写或轮换私有管理入口

在已确认能运行私有入口代码的 Git 集成项目上，先以只读方式核对所有者。旧项目从原始初始化记录核对当前 `cf` 登录的 Cloudflare 用户。已绑定项目通过与账户和项目绑定的摘要核对用户，所有者的用户 ID 由 Pages 生产 secret 提供给运行时。

```sh
npm run rotate:cf-entry -- PROJECT --account ACCOUNT_ID --host HOST
```

确认分支和操作类型后，指定一个位于私有目录且尚不存在的文件保存管理链接。生产部署请求成功后会输出部署 ID。等该部署接管生产域名后，用新浏览器打开链接并完成 Cloudflare 登录。

```sh
npm run rotate:cf-entry -- PROJECT --account ACCOUNT_ID --host HOST --apply --entry-file PRIVATE_ENTRY_FILE
```

`--apply` 为旧项目补写所有者绑定，也可为已有绑定的项目更新路由密钥。遇到写入结果不明时，保留已生成的入口文件并核对当前 Pages 项目与部署记录。生产域名切换后应实测旧入口失效，部署专属地址和分支别名按 OpenSpec 7.3 单独验收。
