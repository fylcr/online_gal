# Spec Delta

## Purpose

定义浏览器版 cf：用固定 commit 的上游源码构建 cf 的生成命令，在官方管理器的 admin 页里运行；请求经内核中转，页面不接触 token。

## ADDED Requirements

### Requirement: cf commands are built from pinned upstream source

`packages/cf-browser` MUST 从固定 commit（`packages/cf-browser/upstream.json` 记录，初始为 `cloudflare/cf@c240e41bd83591bcf001be1fee094234d154b286` 的 `packages/cli`）构建；`fetch-upstream.mjs` 取到的检出 commit 不符时，构建 MUST 失败。上游的 `src/api/**` 与 `src/commands/_generated/**` MUST 原样使用，MUST NOT 手抄或改写。只有生成代码引用的 `#lib/*` 模块可以替换为浏览器实现。

#### Scenario: A generated command runs in the browser

- **WHEN** 站点管理员在控制台输入 `cf zones list`
- **THEN** 执行的是上游生成的 `zones list` 命令，输出与 cf CLI 对同一账号的结果一致

### Requirement: The browser bundle has no Node dependency

打包图中出现 Node 内置模块（`node:*` 或裸名），或出现白名单（`yargs` 及其浏览器依赖、上游源码、`packages/cf-browser/src`）以外的包时，构建 MUST 失败；唯一的例外是 design.md A7 列出的三个上游模块，它们引用的内置函数改由 `src/node-standins.ts` 提供。需要本机环境的手写命令与只在本机有意义的全局选项 MUST 在执行时明确拒绝。产物 MUST 是 ESM，命令按分组拆成 chunk，用到时才加载。遥测 MUST 是空实现，不发送任何数据。

#### Scenario: An upstream change pulls in a Node module

- **WHEN** 升级上游后，某个生成命令开始依赖 `node:fs`
- **THEN** 构建失败并报出引入它的模块路径

#### Scenario: Opening the console loads only the index

- **WHEN** 打开 `/admin` 但还没有执行任何命令
- **THEN** 页面只加载 yargs 与分组索引，没有加载任何命令分组的 chunk

### Requirement: Requests carry no token in the page

浏览器版 cf 创建的 API 客户端 MUST 以 `/__cf/api` 为基础地址，MUST NOT 在页面中设置或保存任何 token；认证由内核中转时补上。`auth login`、`auth logout`、`auth whoami` MUST 调用内核的账号 API。命令需要账号而会话变量 `CLOUDFLARE_ACCOUNT_ID` 未设置时，MUST 依次使用站点管理员判定的账号、唯一账号；都没有时 MUST 报错，列出可见账号并提示 `export CLOUDFLARE_ACCOUNT_ID=<id>`。

#### Scenario: whoami in the console

- **WHEN** 已登录时在控制台输入 `cf auth whoami`
- **THEN** 输出内核状态里的用户与账号信息，页面的网络请求里没有 `Authorization` 头

#### Scenario: Several accounts and no session variable

- **WHEN** 登录的账号能看到多个 Cloudflare 账号、都不拥有本站，且控制台没有设置 `CLOUDFLARE_ACCOUNT_ID`，执行 `cf pages list`
- **THEN** 命令失败，不发出项目列表请求，输出可见账号的 id 与名称，并提示 `export CLOUDFLARE_ACCOUNT_ID=<id>`

#### Scenario: A command that needs a local environment

- **WHEN** 在控制台执行 `cf pages deploy dist` 或带 `--local` 的命令
- **THEN** 命令失败并说明它在浏览器中不可用，不发出任何请求

### Requirement: The admin console is a Svelte component without a terminal library

控制台 MUST 用 Svelte 实现：输入行、↑↓ 命令历史（只存在 `sessionStorage`）、文本或格式化 JSON 输出、执行中的取消（`AbortController` 一直传到 `fetch`），以及需要确认时在控制台内联显示"确认 / 取消"。命令行 MUST 支持单双引号与反斜杠转义，开头的 `cf` MAY 省略。控制台 MUST 提供 `export NAME=value`（与不带参数的 `export` 列出当前值），设置的变量只在本次会话有效，作为浏览器版 cf 的环境变量。MUST NOT 引入终端模拟库或新的前端框架。

#### Scenario: Cancelling a long request

- **WHEN** 执行中的命令被取消
- **THEN** 对应的 `fetch` 被中止，控制台显示已取消，可以继续输入

### Requirement: Upstream license notices ship with the bundle

`packages/cf-browser` 的产物 MUST 附带上游的 `LICENSE-MIT` 与 `LICENSE-APACHE`（以 `.txt` 结尾，让静态托管按文本返回），每个 chunk 的头部 MUST 保留上游的版权声明。官方管理器的构建会重新打包这些 chunk，头部注释随之丢失，所以管理器产物 MUST 把两份许可作为独立文件输出，控制台 MUST 始终显示 cf 的版权声明并链接到这两份文件。

#### Scenario: The built manager contains the notices

- **WHEN** 构建官方管理器
- **THEN** 产物里同时有 cf 的两份许可文件（不内联为 data URL），控制台显示版权声明并链接到它们
