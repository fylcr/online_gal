# Artemis compat clean Web baseline — serial source board

> **当前摘要（2026-10-11 · 用户亲测未解决 · 第六/七轮真实绘制NEEDS / 独审FINDINGS · 待父最终决策）— 唯一现状**
> - 4670原根按钮组新增compat（净基线17200e64），key=clean-compat；父已批准按钮等价映射（实际无select）。旧official/compat/external及trial路径含义保留，固定服务端GAME，无目录picker/game-id。
> - iframe=/compat/pkg/clean-root-baseline-02/index.html?embed=1&run=<run>&ns=<run>，沿new-run/fresh；独立saves:clean-root-baseline-02:<run>:Artemis。START同步锁控件，STOP严格匹配frame/source/origin/run/ns/engine/candidate的artemis-clean-stopped后才卸载解锁；在途load补发STOP，不复活，失败/无ACK保持锁定。
> - range/adapter静审PASS；root final review5e56777c-9a3c-4ef0-ba44-f9dff2b65ac1 PASS_STATIC、无P0/P1；唯一P2修复复审实际JSON为PASS_STATIC/findings=[]（绑定run/output68d27884-6fd1-4680-bc04-448f05a5714d）。此前源码静态验收已接受，运行未验；冻结identity含义不变。
> - 净来源17200e64ea99681e3c27d694433c0ff78d7a7cee加隔离A/B/C/VFS；不导入0005/旧R1–R4，不copy D树。音频/字体沿净C实际管线。全部测试用户亲做，4670当前process/source/在线未核，运行同等及低刷新改善未知。

> - time08：取消全帧快照/yield后present推断；父接受time08新独审PASS_STATIC并授权clock-ms-01隔离源码实施，源独审父已接受，原EXIT1/EXIT2保留；recipe静审父已接受，新clock-ms-02编译/供包EXIT0，包静审父已接受，新clock root入口PASS_ENTRY_STATIC父已接受；用户亲测报告未解决原问题，具体步骤/测量/重复次数未提供；默认基线不变。非官方全等/FPS结论，版本unknown/许可未清，旧失败保持。

> - 新clock-compat按钮与clock-root-ms-02 iframe已静态接线；固定GAME、独立namespace，STOP按session candidate严格ACK释放；未核在线。

> - 包PASS_PACKAGE_STATIC及入口PASS_ENTRY_STATIC均父验收；用户先STOP等释放→刷新4670原根→选择整数毫秒clock-ms-02→START固定GAME，在线/实际页面未核；用户最新反馈“还是不行”，不再待首次反馈。

> - 暂停NowMs；第五有限成果/Execute正常receiver父已接受，完整提交未READY。第六/七轮NEEDS，fresh独审FINDINGS（有限ACCEPT_STATIC/完整BLOCKED），owner参数P2待后续限定，待父最终决策；无实施或运行验收。
> - 主-152真实data零/内-88 signed+4已核；+316是前置判定，+80非0仍可达L18 render。真链经renderer+16/f3259/f4882/f4715至owner+352/+356，GL draw/no-op swap端点已读但尚未交汇；owner字段已写p1，剩余追f4452实参p1身份/vptr到draw，不能自动呈现或称READY。
> - 状态9只两对象gate，Stage+48/Trans!=0xd/旧P2保持；健康桥/STOP身份ACK/MapPoint不变。无新候选/产品资产服务，完整替换不fallback→失败累积→成功后缩减、所有测试用户亲做保持。版本/recipe/provenance/许可/在线/FPSunknown，旧rejected/工具失败历史不改。

## 第六/七轮真实绘制边界同步（2026-10-12 · NEEDS / FINDINGS · 完整契约BLOCKED · 待父最终决策）

父已接受第五有限成果与独审正常Execute receiver闭合，不接受完整提交READY；第六材料父已读，当时独审未完成。本轮实际作者run8a80d369-2b6d-40ff-816e-f3141e9337e4，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/e160df06-9e2f-4cf0-989d-cce2d74b15ce/core-platform-split/render-call-path-07.md；fresh审查run92eea3b4-e498-4eb0-99d0-962958da2fa7，同目录render-contract-review-07.md。作者NEEDS_NAMED_EVIDENCE；审查FINDINGS，有限静态成果ACCEPT_STATIC、局部完整契约NEEDS/完整替换BLOCKED，P2一项、无P0/P1，Merge OK with notes仅分析材料。新结果待父最终决策，不授实施/运行。本writer全文读取作者/审查及render-path-resolution-07/round06-report.md；三份原样copy到新guard并核hash，不改原输出/receipt，第六criterion-1 not-satisfied和旧rejected仍保持。

第六selected-byte修复逐byte取最后初始化覆盖，uncovered/null不补零：主-152地址576028四字节真实data 00 00 00 00（段7119），不是默认零猜测；正常O外调整仍O。576160仅首byte覆盖、576628未覆盖限制保留。+316经f3256/f1603为前置状态/成员树判定，不是draw/submit；+80非0仅跳过+316，经L20仍到真正L18 render，不自动跳过绘制。第七主-88地址576092真实04 00 00 00为signed+4，f4860 receiver=O+4，flags=O+44、float=O+16；float==0且(flags & ~4)==0返回0，否则返回flags（并不保证否则非零）。metadata4不是table4/f4281，helper附带无语义lookup不得当映射。

真正绘制链已补至owner虚接口：Display L18前置接口/分支后，保留Stage+48条件，renderer+16→table3170/f3259，按+56成员索引；正常member0主+16→f4882，内-24真实signed+4转O+4/次级vptr576648，+148→f4715，尺寸/变换/可见性门控后+156/+160→f4435/f4434，读adjusted+468 owner并调用+352/+356。正常member0不等所有运行成员固定。Execute正常receiver按父接受独审已闭，不再标unknown；f3519/f4109不是已证Lua恢复，状态9仅两对象更新gate。Stage+132对象+28、stage+128-1、Stage+48与Trans!=0xd及旧P2保持，Asyncify非Lua预算。

GL/宿主终点分层：WAT a.xa=function76、a.gb=function113；f1506实际draw调用(5,4,5121,0)，f3921实际swap import。作者JS切片xa→GLctx.drawElements、gb→GLFW.swapBuffers空正文；独审源核imports/calls，但JS offset/hash属于作者派生，未独立重算或执行。这些终点未与selected owner+352/+356交汇，也未证明该Display可达f3921；不能由no-op swap推出自动呈现/rAF策略/GPU通过，提交时机仍unknown。

独审P2：owner字段赋值无需再找，f4452正常构造已写mem[p0+472]=p1，且(O+4)+468=O+472；主/次vptr写入同段923988-923994。准确剩余缺口是追正常f4452实参p1的实际owner身份→最终vptr→+352/+356→draw，不以同布局f2020/f2052拼接。仅在本新增record照录finding与限定，不修改作者原报告或称已修复/父接受。含提交决策完整READY仍未成立；有限不透明core框架讨论可用，Present资源/viewport/compositor seam暂保留，Frame提交决策不得等同f4860/f3256或现授整体删除Present，无需全引擎/全Wait/全部成员还原。

clock-ms-02用户亲测未解决；官方Android/WASM共享core及平台处理后compat完整替换、不patch/fallback→失败记录累积→解决后缩减目标与用户测试权保持。健康生命周期/single-flight/RPC/range/audio/dispose/严格STOP身份ACK/MapPoint不变。无新候选/产品/assets/services，版本/recipe/provenance/许可/4670在线/FPSunknown。runtime旧rejected、IndexError/offset/assert/timeout/410/503/交付/grep/doc转义及历史原P2不改。本轮作者和fresh审无新infra失败；本doc不运行helper或新分析。

仅三doc当前摘要/本record与新guard；原第五record起完整历史suffix bytes保持。cwd=D:/online_gal，branch=main，HEAD9dd18ecf/index空/既存dirty，七项有限冻结hash未变，非全仓证明。证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/render-contract-closeout-07/含round06-report.md、round07-report.md、round07-review.md、copy-proof、三doc before/after/diff、before/after/proof与status/cached/partial。没有测试/编译/目标/browser/HTTP/服务进程/分析器/网络/git写/config/子代理；完成后结束唯一writer。

## 对象链第五轮材料同步（2026-10-12 · 有限ACCEPT_STATIC / 完整提交设计BLOCKED · 待父最终决策）

父接受上轮诚实partial陈述，不接受对象闭合。恢复解析已产生机械lookup，但runtime bc37db72 acceptance rejected原因是Stage/swap criterion未满足，不改写全部通过。本轮三实际报告全文读取，新结果待父最终接受，不授实现：Execute run=da1c8351-08d0-434e-a8e4-99e190ec68f3，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/307a463a-1c36-48d7-9aad-247ed5104969/core-platform-split/execute-instance-chain-05.md；Renderer run=44776830-0e07-4697-aa27-4430ea65df3c，同目录renderer-member-chain-05.md；Review run=74a073f7-c535-4f94-81aa-a63462e27400，同目录object-chain-review-05.md。两分析NEEDS；独审ACCEPT_STATIC仅有限静态成果、No issues found，含提交/swap职责的完整平台替换设计仍BLOCKED。报告原样copy/hash核同，新guard不改原输出或receipt。

新增更正不改旧历史：旧创建前assert含is_absolute和字符串startswith；恢复报告说明is_absolute=True，实际原生反斜杠字符串与前向斜杠startswith失败，不能继续归因为Path不接受Windows absolute。旧assert/30s超时/180s恢复事实仍保留。2449/2450/1032/2451是invoke table参数，不是imports编号，分别映射f3840/f3743/f201/f3742；独审WAT确认函数imports共198（0–197），table起点1，table index不等于函数号。旧误称仅以本新增record更正。

Execute真实LE槽：744164的03 0b 00 00→2819→f3519(object,time)，其f1539处理256队列项并清320项低6位；744472的ca 07 00 00→1994→f4109(object)，条件事件/虚槽+56/flag更新。独审直接source/data/elem抽核支持映射，不能叫Lua resume或据此否定全可达图脚本恢复。Execute作者仍列receiver缺口，独审新增table3726=f2798，构造table3734=f3744；正常成功初始化路径f3744 receiver→owner+64→全局849876→f3551已闭合，不再要求重查此赋值。只证明选定正常路径，不证所有实例/异常或vptr永不变。状态9只门控两对象更新，不跳过整个Stage；这些目标不是已证脚本预算，Wait/core职责与平台调度分开。

Renderer值链：v8=Stage+140、v10=Stage+16是pair字段地址；f2900分配control并以内嵌+20构造f3731/vptr755684，object/control分别写Stage+24/+28。初始member0由f3617/f4455/f4452构造，主vptr576180、次级576648；control与对象/调整指针区分。独审核755696→1472→f4543、+140→1504→f4551，未调整主表576192→1062→f4884只是候选。f4543按+56、f4551/f4468按+60选成员，不能假定同永久成员。

尚缺所选Display成员object+=*(vptr-152)之后实际vptr/+80目标及返回值提交门控意义，+316及后续到实际swap；f4884另有-20调整转+148。不能将主表+80或+140清flag、+316直接叫present。Renderer读576028四字节时单段不覆盖导致IndexError: list index out of range，调整文件未产；同bash后续lookup EXIT0不抹错。首组JSON offset被elem覆盖，仅可作elem坐标，后helper分data_binary_offset/elem_binary_offset，旧文件不冒改。helper属于guard内离线分析，非引擎执行/测试或候选产品。本doc不运行helper/恢复解析。

独审区分：core作为不透明依赖的单飞行Frame/Step/DrainQueued有限框架设计已有材料，但包括提交决策的最小完整替换仍非READY；必要缺口仅调整后+80/返回意义、选定+316→swap、compat提交决策保留/删除/迁移。不要求全引擎、全部Wait或全部动态成员穷举，不以每帧/yield后present猜测。健康Open/Boot/Release、RPC/range/audio/dispose、严格STOP身份ACK和MapPoint冻结；core全等/版本/转换recipe/provenance/许可unknown。Trans!=0xd新报告限定与旧P2历史保持，Asyncify不等Lua预算，四行预算非官方策略。

clock-ms-02用户亲测未解决；共用core与平台处理后compat整段替换方向和完整替换→失败记录累积→解决后缩减保持，全部测试用户亲做。无新候选/代码/assets/services，4670在线/FPS未知，旧410/503/交付/grep/doc转义/误位/ENOENT/超时/失败原报告冻结。仅三doc当前摘要/本record和新guard，原第四record起history完整bytes保持；cwd=D:/online_gal、branch=main、HEAD9dd18ecf/index空/既存dirty，七项有限冻结hash不变。证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/object-chain-closeout-05/含三报告copy/hash、三doc before/after/diff、before/after/proof、status/cached/partial。本writer无新分析/目标/测试/构建/HTTP/进程服务/网络/git写；完成即结束。

## 对象解析第四轮partial同步（2026-10-12 · NEEDS / ACCEPT_STATIC部分陈述 / 解析BLOCKED · 待父最终决策）

父已接受第三轮NEEDS有限材料与P2 VALID，原报告Trans type0摘要遗漏仍留历史，不撤销原FINDINGS。本轮三实际报告已全文读取；新结果待父最终决策，未授实施。Execute run=fdcaa707-50d1-4a1b-8640-94aa18b0f9f4，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/31511135-6dfe-44dc-84ce-52721aa7d0fa/core-platform-split/execute-table-join-04.md；Renderer run=b73cd3a0-4b5b-47ce-8e59-33fe82f7b2e0，同目录renderer-table-resolution-04.md；独审run=69c6d036-81b3-45f9-b351-f5ec0685a38d，同目录resolved-boundary-review-04.md。原样copy到新guard/execute-report.md、renderer-report.md、review-report.md，copy hash相等，不改原路径/receipt。

两报告NEEDS_NAMED_EVIDENCE；独审ACCEPT_STATIC仅两份partial/缺口陈述，BLOCKED指对象解析与目标闭合验收，No issues found不能升级READY。Renderer创建前第一assert（含is_absolute与前缀）失败，后续不存在/父链检查/mkdir均未执行：

```text
Traceback (most recent call last):
  File "<stdin>", line 4, in <module>
AssertionError
Command exited with code 1
```

object-resolution-04未创建，没有helper/data/elem解码、slice、真实LE槽值或table lookup；独审只ls见目录不存在，未重放Python或验证其环境。不能断言安全guard已完成，不能当WAT材料缺失/引擎失败。Execute如实消费失败屏障，未接管解码。helper本应是guard内离线数据分析（非产品/候选/引擎执行/测试），但本轮没有产出。本doc writer不恢复解析或探环境。

局部源事实与候选分层：候选构造分配9796字节后写vptr744156/receiver+0，另对象写vptr744464/receiver+8；构造receiver尚未连接Execute实际receiver。Execute正常路径time+144→tween，状态+2876!=9只门控两个虚调用object与object/time；不可扩大成整个Stage更新被跳过。744164/744472只是候选基址+8，无真实LE值→elem/table索引→函数正文，第二调用不可命名Lua resume，Wait/Trans可达与恢复后Display未知。

Stage+24虚槽+12 ABI为object/stage+196/1.0f，+140传object，仅槽/参数已证；renderer创建赋值/vptr/data/table/目标缺失。Display读取+132对象/+28检查后按+128-1分支；case0槽+296、另一输出对象调整指针/槽+80及renderer+316不能混成同对象，+316不可直接叫最终swap，提交链未闭合。无新机械目标结论。

本轮两报告已保留Trans限定：type0 Evolution；仅+0xc70!=0xd时保存旧状态到+0xc74、转4、字段+0x10虚槽+0x38(object,0)；Now/+0xba4写在内门控外。原第三P2不抹去。四行ExecuteLine是compat策略非官方已证；Asyncify不等于Lua yield，既往JS独审partial不被本轮重证。版本unknown不否定局部对应或证明core全等，core可保留性/官方转换recipe/provenance/许可仍unknown。

用户clock-ms-02亲测未解决；目标先辨官方Android/WASM共用核心及平台处理，再compat整段替换。首替Frame/Step/DrainQueued提交仍候选，不授实施；健康Open/Boot/Release、TS单飞行/RPC/range/audio/dispose/严格STOP身份ACK与MapPoint保持。完整替换→失败记录累积→解决后缩减目标不丢。无新候选/代码/assets/services，4670在线/FPS未知，全部运行测试用户亲做。旧410/503/交付/grep/doc转义/误位/ENOENT等历史保持。

本轮仅三doc摘要/本record及新guard；原第三轮record起完整历史suffix bytes保留。cwd=D:/online_gal，branch=main，HEAD9dd18ecf/index空/既存dirty，七项有限冻结hash不变。证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/object-resolution-closeout-04/含三报告copy-proof、三doc before/after/diff、history/frozen证明、status/cached/partial。无分析重做、测试/构建/HTTP/目标/服务/进程/网络/git写；writer完成即结束。

## 命名边界第三轮材料同步（2026-10-12 · NEEDS_NAMED_EVIDENCE / FINDINGS · 待父最终决策）

本次补证run3728ca77-2e60-488d-808b-03427cc46114，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/badb6095-f982-4d1f-aa25-6d2058410e6a/core-platform-split/named-boundary-03.md，结论NEEDS_NAMED_EVIDENCE。实际独审run82b2b0bd-8280-45fc-a9f3-66b2485a7012，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/badb6095-f982-4d1f-aa25-6d2058410e6a/core-platform-split/named-boundary-review-03.md，Review verdict FINDINGS，Merge verdict OK with notes仅决策材料，不授实施。本轮新结果待父最终决策，不能写父已接受或READY。两报告全文读取、原样离线copy为guard/named-report.md、named-review.md并核hash相同，不改原报告/receipt。

新增局部证据：保存frame time→tween→状态9门控两个虚调用→stage更新→清16bit flag→sound，shell随后Display仍是局部顺序。Stage对象+24虚槽+12实际传object、stage+196、float1.0；槽+140传object，尚无table目标/renderer实现名。Execute字段+8/+0对象初始化→虚表+8→table目标仍缺，不将第二调用命名Lua resume。Android Wait全文支持分类门控、定时flag/deadline与状态选择；Trans中间截断仍有消费gap，不能称全文已读。

独审P2 finding原样待更正：补证摘要第11行Trans type0遗漏当前状态+0xc70 != 0xd门控。按审查0063f3fc.txt:162-170，type0执行Stage Evolution；仅当前状态不是0xd时保存旧状态到+0xc74、写状态4、调用字段+0x10对象虚槽+0x38（object,0）；独采Now及+0xba4写入在该内层门控外。此为审查指出的限定，不宣称原报告已修；不修改冻结原报告。核心等待/transition状态职责不能由纯Web帧调度替换冒称保真。

补证报告称官方JS有界切片已见timeout/rAF/immediate main-loop、Asyncify unwind/pause/wake rewind/resume、_usleep忙等、swapInterval timing；独审JS单行270.2KB超过50KB上限，独立核验partial，未重新证明切片/这些细节，不等于证伪。Asyncify不等于Lua yield；GLFW实际swap及Stage renderer仍缺证，无60Hz/最终present保证。原grep错误保持，本轮补证称无新工具错误，不把正常exit当FINDINGS消除。

首替仍待补证候选：host/web_main.cpp的Host::Frame/Step/DrainQueued与提交职责，经artc_web_frame，旧职责整体退出不fallback；若等待语义不符先替换core等待/恢复职责，不强行纯平台。保留Open/Boot/Release、健康single-flight/RPC/range/audio/dispose、MapPoint冻结及root严格STOP身份ACK；core可保留性未全证。下一命名缺口两组：Execute字段+8/+0构造赋值/虚表+8 table入口/目标正文，确认后追wait/resume；Stage+24 renderer初始化/槽+12/+140及Display提交到实际swap。无需全库还原，尚未实施或新增候选。

clock-ms-02用户亲测未解决；用户目标官方Android/WASM共用core与平台处理后compat整段替换，完整替换→未解决记录累积→解决后缩减目标保持。版本identity/官方转换recipe/compat Android provenance/许可仍unknown；4670在线/FPS未知，全部运行测试用户亲做。官方原身份及旧候选冻结，无代码/assets/services变化。此前410/503/交付/grep/doc转义/guard误位/ENOENT全部保留，不继续排查。

本轮只三doc当前摘要和本record及新guard，原GPT核心/平台边界材料同步标题起完整历史suffix bytes保持。cwd=D:/online_gal、branch=main、HEAD9dd18ecf/index空/既存dirty，七项有限保护hash未变，非全仓证明。证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/named-boundary-closeout-03/含两报告copy/hash、三doc before/after/diff、history/frozen证明与status/cached/partial。未分析/测试/构建/目标/HTTP/服务/进程/网络/git写/子代理；writer收口后结束。

## GPT核心/平台边界材料同步（2026-10-12 · 两轮有限内容父已接受 · NEEDS_NAMED_EVIDENCE）

第一报告run5dcb90bc-b1ed-4793-a423-9a8060ac723b，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/51eda1fb-1fc4-429a-b141-29aa2dcb05c4/core-platform-split/gpt-three-way-comparison-01.md。第一独审331a9565-063e-4610-a338-0a95190c2bcd，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/331a9565-063e-4610-a338-0a95190c2bcd_reviewer_output.md，对应reviewer_meta.json exitCode=1；父已接受ACCEPT_STATIC/findings=[]内容，但review/workflow51eda1fb-1fc4-429a-b141-29aa2dcb05c4仍failed，原doc lane未启动。Exact error：

```text
Required file-only output was not produced: C:\Users\ASUS\.pi\agent\sessions\--D--online_gal--\subagent-artifacts\outputs\51eda1fb-1fc4-429a-b141-29aa2dcb05c4\core-platform-split\gpt-three-way-review-01.json
```

第一报告/原审.md/meta原样copy为新guard/first-report.md、first-review.md、first-review-meta.json，hash相同；不创建缺失.json、不伪造成功receipt，不重复审查。内容接受不消除交付失败。

本次补证run0463d439-d75c-415a-ac70-1f452ba7f1b1，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/a2afa683-42b4-4bba-bcaf-3f359c847e2f/core-platform-split/core-frame-boundary-02.md，判定NEEDS_NAMED_EVIDENCE；独审run0443d3bc-9548-4bb5-87d7-bb86baa19008，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/a2afa683-42b4-4bba-bcaf-3f359c847e2f/core-platform-split/core-frame-boundary-review-02.md，ACCEPT_STATIC/No issues found，仅报告质量，不是READY/实施/运行通过。初始本doc任务标待父最终决策；本writer转义错误停报后，父完整读取本次两报告并接受有限结论，明确不授实现。补证两条/usr/bin/grep: Unmatched ( or \(及partial保留失败。

真实边界：Android↔WASM整数ms与Lua now独采31bit掩码是强局部对应；Execute→Display仅候选链，不是core全等。本次wasm2c补出两个虚调用void(object)、void(object,frame_time)，module_instance不算引擎参数；动态目标未知，第二调用不可命名Lua resume。stage execute/display门控/分支已见，真实参数/renderer提交仍缺证，不把前置差异硬归Web平台。官方版本identity/source与Android→WASM转换recipe、compat实际Android产物provenance仍unknown；Android库产出不等于游戏可用。

首替候选收窄为host/web_main.cpp的Host::Frame()+Step()+DrainQueued()及提交判定，经既有artc_web_frame；不整包重写TS/Worker。暂保留Open/Boot/Release、健康single-flight/RPC/range/audio/dispose/root身份ACK及MapPoint冻结；core暂作依赖，不宣称等待恢复已证可保留。下一步只三项指定静态材料：Execute两虚调用对象初始化/虚表/table目标；其可达Wait/Trans yield/到期恢复与stage/Display顺序；stage间接参数/renderer提交槽及既有JS调度切片。不要求全库还原，不把TS/Worker资源管理当根因，不实施。

官方原身份Android558b4f4b08e722b8c9bb336362846f755288bb0f7e8c38a45dc33c43d58fedcf、WASM7ceabed28b84e7689fc4b629fa232e2857f15c3b6e6fa3e09fc39dba082d145e与净pin17200e64ea99681e3c27d694433c0ff78d7a7cee保留，此doc轮不重算二进制。clock-ms-02用户亲测未解决；逐步完整替换→未解决留记录累积→解决后缩减范围目标保持。全部运行测试用户亲做，无新代码/assets/services/候选，4670在线/FPS未知，许可未清，专有伪C不搬入GPL仓库。

本轮doc inline辅助命令解析前SyntaxWarning/SyntaxError unicodeescape EXIT1，零doc编辑，exact/partial存doc-write-error.txt；父授权用write UTF-8 record再读取插入，原失败不改写成功。Flash410/Opus503/guard误位/ENOENT/交付失败等历史完整保留，不再查Pi/provider。cwd=D:/online_gal、branch=main、HEAD9dd18ecf/index空/既存dirty，有限七项冻结hash不变。仅三doc摘要/本record及guard证据，原Opus503标题起历史suffix完整bytes保持；无测试/编译/运行/HTTP/进程/服务/分析器/git写/子代理。证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/gpt-core-boundary-closeout-02/保存原结果副本/copy-proof、三doc before/after/diff、history/frozen证明、status/cached/partial及失败恢复。writer完成即结束。

## Opus503基础设施有界排查（2026-10-12 · blocked · 无对照结论）

父核workflow253ee03d-89eb-4452-a18f-081a34144e1c及scout e872c38d-9582-40d0-b24e-192dc328210d均failed/terminal，required file-only output未产出。requestedModel=magpie/avemujica-aws/claude-opus-5-5，resolvedModel=magpie/avemujica-aws/claude-opus-5-5:low，thinking来自scout默认low。父实际读transcript前6行证实一次docs/current-work.md read已完成，不能说没有读文件；只产开场白，无函数映射/对照报告，无仓库写入证据。Exact error：

```text
503: {"code":null,"message":"avemujica-aws: claude-opus-5-5 requires adaptive thinking; omit thinking or use thinking.type=adaptive and output_config.effort (request id: 202610112359031917316858268d9d6Jpci08Dz)","param":null,"type":"api_error"}
```

原退役路由410仍保留。用户clock-ms-02亲测未解决；三方核心/平台对照暂停、无新候选。此次仅同协议基础设施静读，不是引擎证据。有界查明pi-subagents applyThinkingSuffix保留显式:off；pi-ai off转undefined reasoningEffort，但provider分支不同（deepseek会发thinking.disabled）。未定位实际magpie/avemujica-aws payload实现，因此本Opus路由:off是否真正省略上游thinking、同模型adaptive是否有安全per-run API均unknown，不建议自造字段/global hack，不发live probe。父/operator须另裁恢复，writer不重试或换CLI/foreground/model。

窄源码锚：C:/Users/ASUS/.pi/agent/npm/node_modules/pi-subagents/src/runs/shared/child-tool-plan.js:48-56；src/runs/background/async-execution.js:840-860；C:/Users/ASUS/.pi/agent/npm/node_modules/@earendil-works/pi-ai/dist/providers/openai-completions.js:339-340,445-450,475-478。证据仅这些层，不能推断magpie实际payload。

实际路径猜测ls原错误保留：/usr/bin/ls: cannot access 'C:/Users/ASUS/.pi/agent/npm/node_modules/pi-subagents/src/runs/async': No such file or directory；同命令后续ls成功整体EXIT0不抹错误，经父授权只读实际background目录继续。证据guard误建为headless-pass01/followup-opus503而非既有guard子dir；已断言绝对.ref/无reparse/不存在，父事后批准保留使用，不改写原范围偏离。没有配置/模块/provider修改或执行，没有凭据读取、引擎改动/运行测试；全部测试用户亲做。cwd=D:/online_gal、branch=main、HEAD9dd18ecf/index空/既存dirty，完整历史suffix保持，版本identity unknown/许可未清。writer收口后结束，无在途工作。

## core/platform材料定位基础设施阻塞（2026-10-11 · preparation-blocked）

父报告本波定位child 0c61a0ca-ff14-44c7-bcae-86773d04a6c1首个模型请求即失败，无可用分析/定位报告。workflow f127776a-7d3d-4f94-8f6e-1e212e215f76标preparation-blocked，不启动C++对照，待父同协议恢复，不自行换模型/CLI/provider或重跑定位。Exact error：

```text
410: {"code":null,"message":"Ollama Cloud: deepseek-v4-flash:0731 was retired at 2026-09-25 00:00:00 -0700 PDT (ref: df0be300-9f95-41ad-84cc-a20d0c68fe02)","param":null,"type":"api_error"}
Required file-only output was not produced: C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/f127776a-7d3d-4f94-8f6e-1e212e215f76/core-platform-split/evidence-inventory.md
```

本阻塞不是分析结论，不改变用户亲测未解决/既有构建静审事实；尚无三方对照结论或新候选。文档writer后续只读输出另遇GBK UnicodeEncodeError EXIT1，exacterror保留guard/followup-read-error.txt；父授权改用read及UTF-8证据继续同run，原失败不改写成功。cwd=D:/online_gal、branch=main、actual HEAD9dd18ecfd3de16f5b271c34e87a31aea30c753d6/index空/既存dirty，相关status/index/窄partial供父恢复前审阅。仅三doc追加与当前摘要同步，原用户反馈record起历史完整保留；无定位/对照重跑、测试/编译/进程或服务操作。writer本次完成后结束，无在途具体工作。

## 用户未解决反馈与core/platform方向调整（2026-10-11 · 仅文档记录）

最新用户反馈（本轮m02169）clock-ms-02“还是不行”：记录为用户亲测报告未解决原问题，不再标待首次反馈。未提供具体测试步骤、测量/FPS或重复次数，不编造这些证据；不据此否定所有时钟问题。clock-ms-02的真实构建/供包EXIT0、PASS_PACKAGE_STATIC与PASS_ENTRY_STATIC父验收仍成立，但静态通过并不保证解决用户问题。保留本候选及旧候选assets/source/identity/入口不动，作为累积失败与回溯材料。

用户提出先定位官方Android库与官方WASM相同部分作为引擎本体，再按官方Android→WASM的处理重建compat浏览器平台层。现在暂停继续NowMs/整数毫秒转换小修改；阶段转为官方Android↔官方WASM↔净compat共用引擎与平台层三方静态对照。父将另派只读材料定位及复杂C++对照，本writer不执行分析或产品实现；父尚未接受任何新对照结论，尚无新候选。“能正常出来安卓库文件”仅按Android产物构建路径线索记录，不改写为真实Android游戏运行验证，也不证明核心与官方等价。

总目标保留：逐步整段直接替换→用户亲测未解决则留记录并累积继续→解决后逐步缩小范围定位。新方向调整替换对象，不授权本writer自动测试/build或继续时间seam。全部测试用户亲做；版本identity unknown、许可未清、REA/既有工具失败及WSL EXIT1/Git Bash EXIT2/review-file-only/路径ENOENT与授权纠正历史全部保留，成功记录不覆盖失败。

本轮仅三doc当前摘要和新增本record、ignored证据guard。原clock入口父静态验收收口起完整历史suffix bytes保持；代码/ports源码recipe/out/dist/.test新旧供包root/API/服务/官方伪CAndroid/vendor/旧trial冻结。cwd=D:/online_gal，branch=main，HEAD9dd18ecf/index空/既存dirty，无git写、运行/HTTP/进程检查/测试/编译/服务操作或子代理。证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/core-platform-pivot-record-01/保存三doc before/after/diff、有限冻结库存、status/index/partial与历史证明，不称全仓零漂移。

## clock入口父静态验收收口（2026-10-11 · doc-only · 待用户亲测）

父已消费包独审22b236ed-fcfb-44af-bb9f-41e887c12154 PASS_PACKAGE_STATIC/findings=[]与入口独审efe4d221-2d6e-445e-892c-406c0f4c00ff PASS_ENTRY_STATIC/findings=[]及limitations，均接受。包执行作者runId=cd885bf6-da5a-4c3c-bcd4-a7b45ef0a296，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/80f33a38-4820-42b8-a6ab-4384c3cc8b83/platform-comparison/{clock-ms-02-build-supply}.md；入口作者runId=6752d12c-41f9-47b0-bc53-172acaee67fe，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/f7bd5a65-d1da-47be-9e06-3ecb7cb78550/platform-comparison/{clock-root-entry-01}.md。独审原outputReference分别为C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/22b236ed-fcfb-44af-bb9f-41e887c12154_reviewer_output.md及C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/efe4d221-2d6e-445e-892c-406c0f4c00ff_reviewer_output.md，本收口实际全文读取。

已构建供包及原root新增clock-compat按钮；candidate/output=clock-ms-02，sourceSeam=clock-ms-01。WASM e61a1f49a7658518e3ec3b2d70441cd43824de7f432b0a8799367ee1cf8d2661，JS f79c78092dc5cc9701888931f7938d615f41b5fa9a244d34c09677984da0ddae。默认official与旧候选/净基线保持；共享STOP按session/frame/origin/run/ns/engine/candidate ACK才释放。全部运行测试用户亲做，目前没有用户效果反馈。

用户步骤：先STOP现引擎并等锁释放→刷新原http://127.0.0.1:4670/→选择compat（整数毫秒 clock-ms-02）→START固定服务端GAME，不选dir/game-id；STOP后等完成再切换。server映射仅静读，4670在线/实际页面未知，未访问URL，不称游戏可运行/FPS改善/官方全等/Android版本等价。版本unknown/许可未清，REA没有成功取证。

本轮仅三doc当前摘要及新record、ignored guard证据；历史从原clock-root-ms-02入口源码接线起完整bytes保持，root/新旧PKG/源码recipe/产物服务冻结。cwd为D:/online_gal，branch为main，不是D:/online_gal/main子目录；前作者报告连写瑕疵、review初次ENOENT及supervisor授权纠正保留，不改旧调用为成功。旧WSL EXIT1/Git Bash EXIT2/file-only交付失败仍保留。main/HEAD9dd18ecf/index空/既存dirty，无测试/编译/运行/请求/进程或服务检查及git写。证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/clock-entry-closeout-02/保存三doc before/after/diff、历史与有限冻结库存、status/cached/partial。此为已验入口文档同步，不新增代码审或运行PASS。

## clock-root-ms-02入口源码接线（2026-10-11 · 待独审 · 运行未测）

父接受22b236ed-fcfb-44af-bb9f-41e887c12154 PASS_PACKAGE_STATIC/findings=[]后授权本seam。原按钮组新增clock-compat：compat（整数毫秒 clock-ms-02），默认official及旧official/compat/external/净clean分支保留。START固定服务端GAME，不需目录/game-id；iframe=/compat/pkg/clock-root-ms-02/index.html?embed=1&run=<run>&ns=<run>，store=saves:clock-root-ms-02:<run>:Artemis。URL仅serve源码映射推导，未访问，不证明4670在线。

PKG/clock-ms-02整包28文件与已审dist bytes/hash核同；新clock-root-ms-02复制净root三文件，range-source SHAba173d662fd304ae5d36981361af1552772594b58493e0f84318a7321eea9718不变，entry/index仅asset/候选身份/hash/namespace替换，其外bytes等价。复用整套clean modules/worker/helper及冻结input/MapPoint，未改生命周期/调度/字体/音频/VFS；direct打开不自动启动。

共享clean STOP协议保留：同步锁控件、在途load补STOP，root session独立记录engine/candidate/run/ns；frame来源+origin+四身份匹配artemis-clean-stopped才卸载解锁，净ACK不能释放clock。初次及new-run失败恢复沿原applyRun/refreshMeta，静态身份取对应clock-ms-02/identity.json，不借旧run.json/net hash。未访问/迁移/清空IDB，不声称与AI全局互斥。

证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/clock-root-entry-01/保存root before/after完整diff、adapter identity-only逆替换等同性、copy-hashes、冻结库存/status/index/partial及history证明。main/HEAD9dd18ecf/index空、既存dirty保持；原失败/旧包历史保留。全部运行测试用户亲做；无build/emit/tests/check/typecheck/目标/browser/game/HTTP/服务/FPS/网络，不自签入口PASS，效果未知/版本unknown/许可未清。待独审与父验收后用户再按单实例亲测。

## clock-ms-02隔离编译与文件供包（2026-10-11 · EXIT0 · 待包独审）

父接受recipe独审68a12c77-69f7-4298-b135-5ab1f60923b5 PASS_RECIPE_STATIC/findings=[]后另授新guard/out/dist。显式Git Bash、offline=1、CLOCK=clock-ms-01、CANDIDATE=clock-ms-02、parallel2一次build EXIT0：真实候选TU编译及链接，ZLIB/cache同root；固定pin17200e64原archive无改。clock-ms-02仅输出编号，仍clock-ms-01单一转换seam。旧WSL EXIT1/Git Bash EXIT2与file-only交付失败完整保留，不以本次成功抹旧记录。

独立JS emit EXIT0、aux供包EXIT0，dist/clock-ms-02共28文件，配对本次JS/WASM/sourcebundle、worker/helper/modules、identity/README及delivery-sources.zip；out内本地package.zip。父额外授权仅生成entry/index副本显示标签与saves:clock-ms-02 namespace精确替换，原模板/生命周期逻辑不改，规则/diff附包。未打开/迁移IDB、不接root/.test、不部署，无可用URL声明。

证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/clock-build-supply-02/含完整invocation/env/PID/logs/exits、真实build-proof/input/output hash、template diff/identity规则、包库存差异和窄保护库存。main/HEAD9dd18ecf/index空、既存dirty保持；本轮源/recipe/manifest/旧资产冻结，原history suffix bytes保持。

待fresh只读包审，不自签PASS；全部运行测试用户亲做。本轮无tests/check/typecheck/合成执行/wasm instantiate/browser/game/FPS/HTTP/服务/网络；纯JS emit为授权构建。运行效果/在线/官方等价未测，版本unknown/专有许可未清，GPL源码包不解决专有SDK授权。父包验收后另授入口接线。

## clock-sysroot-recipe-01静态修复（2026-10-11 · 待独审 · 未编译）

父另授单seam：build.sh:39起按非空绝对EM_CACHE选择ZLIB sysroot，沿ports_unix→pwd-P→ports_win把同一cache路径导出给SDK；include/libz.a同root并保留缺项失败/不自动安装。unset/空时沿原SDK/cache，空变量unset避免SDK parser None路径错误；Windows drive斜杠/反斜杠、Git Bash绝对路径及空格均引用，relative/drive-relative值fail closed。ports/env.sh不修改EM_CACHE；现SDK tools/config.py:48-49/135-144及cache.py:198证实cache合同，未运行SDK探针。BUILD.txt重建说明同步有效cache路径与引用要求。无新选项，不改CMake/manifest/NowMs TU或目标列表。

源码配方已修待独审，不自签PASS；原WSL EXIT1、Git Bash EXIT2与review-file-only交付失败仍保留，恢复审查b66860e3-3c5d-4c90-94a5-f3fc8e53e6a0结论BLOCKED不变。双sysroot混用有证，不称唯一根因已证。out/clock-ms-01失败现场冻结，无候选包/URL；未编译/configure/测试/check/typecheck/bash-n/compiler探针/运行/HTTP/服务。用户亲测、效果与版本unknown/许可未清。

新证据D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/clock-sysroot-recipe-01/保存两recipe before/after bytes/hash/diff、窄冻结库存、git status/index/partial及doc history等同性。main/HEAD9dd18ecf/index空，既存dirty保持；下一候选输出仅计划clock-ms-02（仍clock-ms-01源码seam），未创建，须独审及父另授。

## clock-ms-01隔离编译停报（2026-10-11 · BLOCKED_BUILD · 无包）

父实际接受源独审de50d121-263b-4b0f-b365-592250d7c92d PASS_STATIC/findings=[]后另授本阶段。冻结源码/recipe/manifest未改。先建新guard/保护库存及独立离线Git/EM_CACHE，第一次入口误解析WSL bash，ARTC_CLEAN_CANDIDATE缺失EXIT1；父限定批准显式C:/Program Files/Git/usr/bin/bash.exe重试，原命令opt-in/offline/parallel2实际EXIT2。配置成功、Lua编译部分推进，audio TU报libc++ cstddef/math.h wrapper错误；不是NowMs源码逻辑结论。两次原日志/exit/env/PID与失败现场保留，不第三次重试。

只读generated flags/response/CMakeCache证明ZLIB -isystem指向全局cache，libc++在独立EM_CACHE，math.h宏来自全局cache。双sysroot混用有证，唯一根因/完整驱动搜索序未核。建议父另授build.sh:39让有效EM_CACHE与ZLIB include/library同root（unset默认不变），重建指南注明一致sysroot，需新输出/cache及独审；本轮不实施修复。generated build.make仅列候选lua_engine.cpp，旧TU停用，但尚未成功编译该TU/链接。

out/clock-ms-01保留source/build partial，dist/clock-ms-01未建；未emit/package，无JS/WASM配对包或URL，不接root/.test。全部运行测试用户亲做；未tests/check/typecheck/目标运行/browser/HTTP/服务/FPS/网络，效果及许可未清、版本unknown。main/HEAD9dd18ecf/index空、既存dirty保持，无git写。证据根D:/online_gal/.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/clock-build-supply-01/含两次完整logs/invocations/exits、保护前后库存、配置/输入hash与setup-diagnosis.md；原从源码实施record起历史bytes保留。

## clock-ms-01源码实施（2026-10-11 · 待独立静审，未编译）

父接受time08独审d2d0b241-814e-4169-b8fe-ee238f0c550d PASS_STATIC并另授本单seam；旧ENOENT/provider stream失败仍为历史。新增ports/artemis/compat-web-clean/candidates/clock-ms-01完整lua_engine.cpp，来源固定pin17200e64ea99681e3c27d694433c0ff78d7a7cee的git archive，与baseline-02原bytes核同。仅NowMs转换改为duration_cast<milliseconds>.count后显式double，其它bytes保留；不是搬专有反编译代码。

ARTC_CLEAN_CLOCK=clock-ms-01明确opt-in，unset沿原manifest；CMake既有替换循环移除原TU后加入候选，保留audio/PF8与archive headers/includes。hash及sourcebundle记录包含候选，默认净来源/原产物/入口不改。本轮只recipe/source/manifest，未建out/clock-ms-01、未编译/供包/测试/check/运行/HTTP/服务。全部测试用户亲做；待独审及父另授下一构建阶段，无用户效果反馈。

相对epoch、每次独立采样、ClockNow/Pause/Resume、steady deadline/CABI double与l_now实现不改；非负单样本损失[0,1)ms、负duration向零。TagWait pending>1/截断等边界可影响后续时序，不能承诺整体变化<1ms；l_now后续读取时机可能间接变化。非官方全等/FPS根因或收益保证；版本unknown、许可未清。

证据：.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/clock-ms-source01/（before/after库存、source.diff/source-proof、静态selection闭包与git partial）。辅助脚本SyntaxError EXIT1与CRLF匹配AssertionError EXIT1均保留，经父限定恢复后按真实CRLF定点替换，不归一化整TU。main/HEAD9dd18ecf/index空、既存dirty保留，不stage/commit或改冻结历史。此前root/source源码静态验收已接受，运行未验；本轮未新测root/source/在线。

## time08证据索引恢复（2026-10-11 · 待复审）

规格语义不改。独审faee169d-e0d9-4c63-846e-a50e6cb9863d因错误路径evidence/invocation.json的ENOENT中止，未完成；正确留存路径为time-contract08/evidence/time08-invocation.json。索引修复workflow13dda9c9-b65c-4c1c-8e79-4cf628a75eff曾provider stream断开、review未启动，旧BLOCK/失败不改写PASS。本次仅静读恢复完整绝对路径/size/SHA及长函数offset索引，见.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/time-contract08/evidence-index-repair.md。留存-process/-noanalysis、EXIT0/PID97140仅历史记录，不重跑Ghidra或探进程。产品未改；待下一lane独审，未授实施。

## time-contract08（2026-10-11 · READY_FOR_IMPLEMENTATION_SPEC实验规格待独审）

父反馈static07独审只PASS_STATIC两个observed边界，不是全帧快照/脚本yield后present或实施许可。本轮正式撤销这两推断。复用static07 DB，以-process libartemis.so/-noanalysis补时间双证：CLua.Now006c3af8独立采CTime.Now并31bit mask；Animation CalcFrame00580050、CommandWait00615bfc、CommandTrans0063f3fc另采。全部函数SHA558b4f4b…，直接Ghidra12.1.4/JDK27正常EXIT0；不重import/reanalysis、不执行目标。

父准许规格收敛为LuaEngine::NowMs完整转换职责直接替换实验：duration_cast<milliseconds>(ClockNow-init_time).count再double，原相对double转换停用；保留相对epoch、Pause/Resume期限移动、独立采样/CABI/预算/render/输入顺序。已有Lua l_now整数化不变，Lua可见时刻通常不变；内部浮点消费者量化，非官方绝对32bit/31bit时钟等价、非FPS根因或收益保证。新独立candidate overlay按manifest/CMake源替换，不污染净archive/旧供包；本轮未实施，待独审及父另授写域。

规格/全部调用点与伪C/xref/汇编：.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/time-contract08/；绑定time-contract08.md。SDK可比性、Execute动态字段读闭包、用户运行效果未知，后两项不扩大时钟小seam。三doc仅摘要/新record，历史suffix保持；session历史/产品/vendor/原件冻结，全部运行测试用户亲做，许可未清。

## Android static07取证检查点（2026-10-11 · PARTIAL待独审）

import06因项目路径含.ref在ProjectLocator校验失败，未导入；历史失败不改写。static07使用获批ignored DB D:/online_gal/ports/artemis/out/android-static07/project/android，direct Ghidra12.1.4/JDK27，原.so SHA558b4f4b08e722b8c9bb336362846f755288bb0f7e8c38a45dc33c43d58fedcf。正常autoanalysis 182s、timeout=false、ELF/AARCH64:LE:64:v8A/default、imagebase00100000、21330 relocations，保存成功。GCC异常表/PNG/Varnode分析warnings保留，不以EXIT0冒称全闭包。focus01 helper误调用旧import冲突，未覆盖；停报后父批准修commandline，同DB focus02–07成功，原失败证据保留。

原SHA限定：006f4838→pthread入口006f523c→android_main006f1270；active gate内poll→CTime.Now00605ff8（clock_gettime4整数ms）→必要Sleep00606074后重采样→Execute0065faac→Display00667950。Display→Stage006e90c4仍有renderer虚表frontier；EndRender005fb800确实eglSwapBuffers，但动态连接未闭合。Lua resume实现有证，不能断言Execute每帧resume/yield预算。Android/WASM引擎版本可比性未知；16ms shell等待不等于60Hz，也不是Web应搬的usleep。

证据根.ref/artemis-platform-analysis/android-558b4f4b08e7/headless-pass01/android-import07/，完整伪C/xref/反汇编及logs/index ignored。首个候选为B帧时间快照owner，但暂停/epoch与脚本预算未闭合，尚不授产品替换；待独审/父裁定。root/P2原静审状态及净pin/资产身份沿原记录；不回迁R1–R4。所有测试/游戏用户亲做；本轮只分析器，无目标执行/build/服务/HTTP/FPS，许可未清。

## root静审收口（2026-10-11 · 父最终验收尚待）

原root最终独审review5e56777c-9a3c-4ef0-ba44-f9dff2b65ac1 PASS_STATIC、无P0/P1；其唯一P2已修。本轮实际消费复审JSON verdict=PASS_STATIC、findings=[]、No issues found：初始GET暂存与fresh失败恢复守卫准确，不覆盖成功fresh/已有身份/活动会话；START/STOP ACK锁及旧URL/trial未变。复审绑定run/output目录68d27884-6fd1-4680-bc04-448f05a5714d，outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/68d27884-6fd1-4680-bc04-448f05a5714d/root-parity/run-recovery-review.json；未提供另一个reviewer runId，不臆造。原final outputReference=C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/2c55e2a1-7d99-4957-b4fe-462ebd843504/root-parity/final-review.json。

静态root新增b-clean/pick('clean-compat')、label compat（净基线17200e64），固定服务端GAME/原START；STOP等真实释放ACK才卸载解锁。净17200e64+A/B/C/VFS，无0005/R1–R4回迁，无新增重写或动画修复。source/adapter独审、原workflow交付失败及恢复事实沿原记录保留。

仅三doc当前摘要及本新增record，历史从root P2标题起原bytes保留；产品root/source/adapter/WASM/glue冻结，main/HEAD9dd18ecf、cached空，无stage/commit。全部测试用户亲做；本轮无编译/运行/请求/服务probe或启停。父最终验收尚待；验收后用户先停其他引擎→刷新http://127.0.0.1:4670/根页→净基线按钮→START；STOP未ACK不启另一引擎。当前服务在线/process是否用最新page未核；源码同流程接入不等于运行一致，性能/许可未知。

## root P2 run身份恢复（2026-10-11 · 已修待复审）

实际读取final-review.json：PASS_STATIC、无P0/P1，唯一P2为首次/run.json在fresh期间成功被丢弃、fresh失败后runId永久为空。新增initialRun暂存与restoreInitialRun守卫；初始响应到达及fresh两种失败分支调用，只有无runId/无runPending/无live/无cleanSession才applyRun。fresh成功、已取得身份或活动会话不被旧GET覆盖；不加HTTP重试。原root接线/STOP ACK/旧分支路径、组件与服务不改。

本轮仅root四处定点文本替换与三doc当前摘要/新增record；历史从root integration标题起bytes保留。main/HEAD9dd18ecf、cached空、既存dirty保持；仅静读/编辑/窄hash/只读git，无测试或模拟/HTTP/browser/build/check/服务probe。全部测试用户亲做，运行/4670在线/性能/许可未知；待P2复审及父验收后才交付原root固定GAME流程，不推进重写。

## root integration阶段（2026-10-11 · 待final静审）

仅比较根page/index.html与三doc当前摘要/本轮record。已审source/adapter、净C/引擎/服务/资产、旧简化page/compat-index、AI接口保持；不增加pause/save/AI、不声称AI ownerLock全局互斥。new-run期间同步排他，初始run迟到不覆盖已选run；clean消息独立显示且不借旧title/status假释放。冻结identity fetch仅编写未请求。

adapter独审实际adapter-review.json为PASS_STATIC/findings[]（父消费review7c77fb15-3240-488f-b667-15f3fb6808c5）；range恢复workflow4d20916e-7839-4d46-816b-1256ef699e55/review8f191691-33f7-40b0-b7ef-67174f9f9197 PASS_STATIC，原c047b60d交付失败保留。ROOT待final fresh review，不将组件静审等同运行验收。

main/HEAD9dd18ecf，cached空，既存dirty保留。仅静读/文本编辑/窄hash/只读git；无build/emit/check/typecheck/test/模拟/HTTP/API/browser/game/WASM/FPS/网络/安装/服务进程probe/stage。历史suffix编辑前后窄hash核对；不改历史hash/record。父final验收后用户先停其它实例，在http://127.0.0.1:4670/选择clean-compat→START，STOP等待ACK；无ACK请保持单实例锁，不先启动另一引擎。净baseline-02原手动页保留，loose内存/启动耗时、游戏/输入/音频/字体/动画/许可未知。

## Historical phase (2026-10-11)

Parent accepted fresh static review PASS_STATIC: both original entry P1 FIXED, no new reachable P0/P1/P2. Original build/source archive/TS emit/package EXIT0. Clean baseline-02 awaits user-owned runtime testing; no low-refresh improvement claim. Doc-only closeout; frozen artifacts and history retained.

## D父静态验收收口（2026-10-11）

父采纳实际fresh review：workflow f0b63844-4ba4-4ac2-a4db-1dfcaddf72e1，author a9739990-21ca-4c59-8caf-a9371b249694，review eba86791-c11a-4b79-b64e-308633c67549；绑定artifacts为 C:/Users/ASUS/.pi/agent/sessions/--D--online_gal--/subagent-artifacts/outputs/f0b63844-4ba4-4ac2-a4db-1dfcaddf72e1/clean-web/d-entry-lifecycle-review.md 与同目录 d-entry-lifecycle-fix.md。报告实际已读：两原P1 FIXED，无新可达P0/P1/P2，PASS_STATIC — OK。末次误猜before快照名ENOENT原注记保留，独审未独立重算hash；结论基于此前完整entry.diff及入口/消费者静读。

原native编译/源码归档/TSemit/供包EXIT0保持，不重编重包；净commit17200e64ea99681e3c27d694433c0ff78d7a7cee，无R1–R4迁入。WASM294bff398f1ed7d49b2cb5d1accb5441b0e92e851d863505a27fdf2ea7b503cd、glue2fad40ac126c6bf8a4bc938e9eca840fd4be8d6080a4ba0683c5e7680ec71f1a、entryf50865a7afab40af8de07b48f7aab616c15bbd1c36fd91916fa06f2cc56351a7身份沿用原证据，本doc-only未重算资产hash、不改代码/输出/identity/原日志。baseline-01原失败与所有历史记录保留。

候选等待用户亲测：先停其它引擎，在 http://127.0.0.1:4670/compat/pkg/clean-baseline-02/index.html 手动选游戏目录、填稳定game-id、START。URL仅existing serve静态推导，未访问、服务在线未知；全部运行测试用户亲做。原低刷新未解决，不能标改善；游戏/音频/存档/动画/FPS/运行与许可未知，GPL不清除官方SDK专有风险。用户反馈新基线结果后父另拆下一原创整块重写；本阶段不推进源码。


## D入口两P1定点修复（2026-10-11 · 待fresh复审）

实际独审childb9ce7181-09a1-4e5f-a357-9f16a969042c为BLOCK，非通过。本次只修入口生命周期/会话checkpoint。真实模板out/baseline-02/candidate-entry.js已父追加授权；package.mjs原copy(run/candidate-entry.js,'entry.js')不改不执行。模板/双entry bytes一致，hash从8e524c55c9991dd28c8ea3679e572539f4f2ee969db003ab43a2a1e54fcbdc2c变f50865a7afab40af8de07b48f7aab616c15bbd1c36fd91916fa06f2cc56351a7；精确diff与原identity/hash保留entry-fix-evidence/。

点击handler同步调用动作；START在任何await前取得starting状态/局部session并禁用控件。STOP在starting/operation拒绝，停止保持stopping至whenDisposed完成；失败不错误解锁。loop/input/notify绑定局部实例，startup终态等load settle后释放，不遗留旧实例。选择目录异步也锁入口；运行中目录/game-id控件禁用且namespace使用启动捕获值。SAVE/RESTORE排他operation，checkpoint绑定session，START/STOP清空，晚到SAVE只在同session/operation且未终态存入；终态释放先清checkpoint，旧结果不能跨会话回填。未改C/VFS/schema或原save数据。

只静态编辑/hash/库存，未编译/测试/页面模拟/browser/game/HTTP/API/服务/FPS；native/glue/WASM/helper/sourcebundle及baseline-01冻结。原build/emit/package/log/hash保留历史，当前identity仅更新entry hash，新增修复证据不覆写历史报告；待fresh复审，不宣称候选可运行。


## D baseline-02集成供包（2026-10-11 · 待fresh静审/用户亲测）

父另授新baseline-02与离线对象cache；实际A writer/reviewer报告读完，workflowe196bbdd-4596-4142-9e6c-e18840379d8b / reviewer52cd855c-9d07-439e-97c1-e77c07eb86ff PASS_STATIC。唯一授权A hash变化make-source-bundle.mjs=45f76ed081d8bc7ba69d91412ae60a54f638bbab5390aa301ad945b07eff19d3；未改其他A/B/C/VFS。baseline-01原EXIT1/库存保持。

`ARTC_CLEAN_OFFLINE=1 ARTC_CLEAN_CANDIDATE=baseline-02 bash ports/artemis/compat-web-clean/build.sh --parallel 2` 一次EXIT0，真实B host/PF8/audio编译；固定commit17200e64/tree20fe790f git archive输入（非.ref checkout，无旧overlay）；A source.tar.gz成功。`node node_modules/typescript/bin/tsc -p ports/artemis/compat-web-clean/out/baseline-02/tsconfig.emit.json` EXIT0，仅独立源码emit。`node ports/artemis/compat-web-clean/out/baseline-02/package.mjs` EXIT0，原日志/exit/输入和完整输出hash在out/baseline-02。

C工厂createCleanArtemisRuntime(host,options,urls)；classic worker与独立worker-audio helper，加许可复用protocol/read-host/audio-relay基础层；不含旧runtime/worker-host/compat-worker调度。engine-api闭包、现VFS sources/path和现player-host GameLoop原byte emit，单rAF无新公共调度。页面手动选择目录、填写稳定game-id后START；既有IndexedDbStore schema不变，name=saves:clean-baseline-02:<game-id>隔离，原C CLEAN_STORE_PREFIX不改。先停其他实例；STOP释放旧实例后才可再START；pause/resume、save/restore按钮仅用户动作。

父明确新增PKG child写域例外：D:/online_gal/.test/artemis-animation-delivery-ci/package/unpacked/package/clean-baseline-02/，不改497个旧PKG文件、任何旧page/index/serve。serve.js:92,402–407静态/compat/pkg映射及MIME已读，入口 `http://127.0.0.1:4670/compat/pkg/clean-baseline-02/index.html` **仅静态映射推导，未访问，服务在线未知**。所有worker/glue/WASM/helper URL由新child下import.meta.url变绝对，配对同build；out/candidate与PKG child逐文件SHA一致。

JS/WASM/sourcebundle/worker/helper/C/VFS/page完整hash见d-evidence.json与candidate/identity.json；来源见package-evidence.json/input-hashes.json，静态import闭包零缺项；protected-before库存非doc零漂移、baseline-01保持、三docs原历史suffix保持、staged空。不冒称worktreeclean。未运行browser/game/HTTP/API/服务/WASM/FPS/测试/截图，不安装依赖/stage/commit；HTML已完整读daisyUI技能，仅语义类/原生控件无CSS依赖。全部运行测试用户亲做，游戏/音频/保存/动画/FPS/官方等价未知；GPL源码包不清除官方SDK专有许可风险，未输入官方伪C。供包完成待fresh独审，不等于运行验收。


## A tar-path source repair (2026-10-11)

Only make-source-bundle.mjs tar invocation changes: basename archive and staging
operands, with execFileSync cwd set to the already guarded run directory. Windows
D: colon no longer reaches tar filename parsing; POSIX operands resolve identically.
No shell strings, GNU-specific flags, dependencies, new packing logic or changes
to identity/contents/guards/no-overwrite/hash/error propagation. Caller is clean
build.sh:81; old CI bundle call targets the old port and is untouched. Fresh static
review required before separate D authorization for a new candidate/cache name.
baseline-01 and its evidence are preserved, not resumed or overwritten. All tests
remain user-owned; no packaging/build/runtime acceptance claim. Existing history
records below remain byte-for-byte intact.

## D integration-only停报（2026-10-11）

VFS独审workflow456403c7-9c8e-4198-a5f8-af06323b66ee / child6d635885-9ef6-4bff-929b-218b78a2ac3b已实际读PASS_STATIC，source hash96c9031efb258563ba85b9e0bbc9aab835ad45cdf21884e56bcbff2b11337a6b一致；父据此另授D。授权离线新cache恢复净clone Git对象，origin/HEAD/tree固定核对，通过git archive构建（非.ref checkout）。

原命令 `ARTC_CLEAN_OFFLINE=1 ARTC_CLEAN_CANDIDATE=baseline-01 bash ports/artemis/compat-web-clean/build.sh --parallel 2` 整体EXIT1。CMake到 `[100%] Built target artemis-compat-clean`，之后make-source-bundle.mjs:68的tar失败：`tar (child): Cannot connect to D: resolve failed`；Node `Error: Command failed: tar -czf D:\online_gal\portsrtemis\compat-web-clean\outaseline-01rtemis-compat-source.tar.gz -C D:\online_gal\portsrtemis\compat-web-clean\outaseline-01\source-bundle .`，child128 / exec status2。原日志与exit保留out/baseline-01/build.log、build-exit.txt。

遵守失败即停，未修A配方/产品逻辑、未重试/另打包，未TS emit/C worker装配/helper copy/page接线。**无candidate URL、不可交付部分产物。** JS152974B SHA2562fad40ac126c6bf8a4bc938e9eca840fd4be8d6080a4ba0683c5e7680ec71f1a / WASM1481163B SHA256294bff398f1ed7d49b2cb5d1accb5441b0e92e851d863505a27fdf2ea7b503cd为本次同build残留，不代表完整package。

输入A/B/C/VFS全hash见out/baseline-01/input-hashes.json，重新核对零漂移；artifact/build身份见d-evidence.json，继承VFS保护库存核对见protection-check.json。main HEAD9dd18ecf，cached空；旧线/净clone不作为copy来源、旧page/index未改。未browser/game/FPS/HTTP/API/服务/截图/运行测试、未stage/commit/push/reset/stash/clean。所有测试用户亲做；待另授A供包tar路径修复/静审再恢复D，当前勿测试部分JS/WASM。未来完整交付后用户先停其他实例，再按独立candidate入口手动选择游戏/start、输入/resize/pause/resume/save/reload验证；本轮无可用入口。

No compilation, tests, SDK diagnostic execution, networking, game, browser,
HTTP/API or service actions in A. All tests are reserved for the user.
First build an independent clean Web baseline; only afterwards rewrite animation
on that line. Never migrate R1–R4. Their history and user results remain intact;
R4 low-refresh failure is not a measured root cause, and R1's old success does
not transfer to this baseline.

Origin `https://github.com/Weiss-UltimateSavior/artemis-compat.git`,
`refs/heads/main`, commit `17200e64ea99681e3c27d694433c0ff78d7a7cee`,
tree `20fe790f35e382795182b24c9e27e823c1020566`, observed
`2026-10-11T07:47:13Z`. New HEAD equals old pin. Reference checkout is read-only
`.ref/artemis-compat-latest-17200e64/source`; never a build input. Git archive
attributes can differ from checkout bytes. Manifest lives only at
`ports/artemis/compat-web-clean/source-manifest.json`.

## VFS transaction settlement (2026-10-11)

Only packages/vfs/src/sources.ts IndexedDbStore.run changes. Transaction handlers
are installed before action; request success captures its original result,
transaction complete alone resolves it. Request error and transaction error/abort
reject; abort without tx.error gets AbortError. Native Promise settles once.
Transaction/objectStore/action synchronous exceptions still reject through the
Promise executor; no retry, wipe, wrapper, reopen or schema/namespace changes.
Read/getAllKeys failure or abort cannot become absent-data/success. Existing
readonly/readwrite, read conversions, write/remove void and list sorting remain.
Complete confirms transaction completion, not absolute power-loss durability.

Read full IndexedDbStore and all four run callers, actual WritableStore contract
and both implementations, full OverlayGameSource/core storage and clean C runtime,
worker-host and worker save/hydration methods. Guard
.ref/artemis-compat-clean-vfs-baseline/ reuses 6786 resize inventory keys including
VFS source, captures source/three docs raw bytes and partial tracked diff; untracked
ledger/source identities are hashed explicitly. Only run and three docs may differ;
two current documents retain historical raw suffixes. A/B/C, clean clone, old line,
assets and index frozen. Source ready awaits independent review; no compile/tests,
IDB/mock execution or runtime/save-success claim. All tests user-owned. Suggested
future user check: force transaction abort after request success for read/write/list/
delete; verify rejection and unchanged old saved bytes, then normal completion
returns original values. This check is not run or implemented here. D requires
separate integration-only authorization after review; no upgraded upstream claim.

## Board (multi-seam, one writer at a time, cwd D:/online_gal)

- A: build.sh, CMakeLists.txt, source-manifest.json, make-source-bundle.mjs,
  README.md and this ledger; complete source-only, pending independent review.
- B SOURCE_READY: host/web_main.cpp, host/web_abi.h, platform/pf8_reader.cpp,
  audio/audio_web.h/.cpp, audio/audio_validation.h under the new port. All six
  exist; one owner for lifecycle and CABI. Audio trio traced against clean
  headers before reuse; original host platform owner, no whole-old-host copy.
  Await fresh independent static review.
- C SOURCE_READY: only new packages/runtime-artemis/src/compat-clean/{runtime.ts,
  worker-host.ts,compat-worker.js}. New ordinary single-flight frame owner; no
  R4 demand/credit/mailbox transplant. Only independently read protocol,
  read-host, worker-audio and audio-relay foundations reused.
- D last: separately approved compile-only, isolated pair/package/factory/copy
  wiring/candidate page/ledger registration. No existing route overwrite.
  Tests, game and animation acceptance remain user-owned.

C fresh review BLOCK: P1/P2 source fixes applied; awaiting independent re-review. Backing assignments now require actual per-dimension changes; CSS CABI resize stays unconditional after validation. Closed input exits before gesture audio; public async unlock rejects after dispose without a synchronous throw. No compile/tests; separate VFS transaction fix/review remains mandatory before D delivery.

## B backing resize invalidation (2026-10-11)

Review 1676e756-da8c-48c5-a33d-b446799d2ac6 confirmed old C P1 CSS-only reset
and P2 disposed-input synchronous throw fixed. Remaining static backing resize
clears OffscreenCanvas while B revision/animating gate skipped redraw. This round
changes only host/web_main.cpp and these three docs, no C or ABI changes.
Host::output_invalidated defaults false; every positive CSS resize notification
sets it even if CSS unchanged, since C may have changed backing dimensions.
Frame adds it to the existing revision/animating condition, clears only after
Present returns, then follows original drawn_rev and EndFrame. Multiple resize
notifications coalesce. No unconditional Draw or new scheduling/frame policy.
Release clears session invalidation: open begins with Release, failure/exit/
dispose also release; first-frame boot/revision sentinel remains unchanged.

Parent explicitly approved paused resize boundary: Host::Frame early return
and C CleanWorkerHost::frame refusal remain. Pending survives pause and resume
notifications; first permitted frame after resume presents. While paused the
reset backing may remain blank; no immediate-paused-repaint claim, no Lua/Update
or artificial clock advance. Present failure never reaches success clearing;
existing Guard ends/releases that failed session rather than carrying stale
invalidation into reopen. CSS/letterbox/defaultFBO/MapPoint unchanged.

Read entire B host and C worker-host/compat-worker, current summaries, C ledger
and independent review. Guard .ref/artemis-compat-clean-resize-baseline/ reuses
C final inventory keys, adds current B/C sources and three docs: 6786 paths.
Before bytes/hash and tracked diff saved; untracked platform/browser sources
listed separately. Only host+three docs allowed to differ; historical doc bytes
below the original first summary retained. MapPoint raw SHA256 remains
8de2728ba79a24f25bd9fa58073a96372efbaa48e08ed678e880b6e65e2b1262.
Author static evidence is not independent verification. Source complete awaits
cross-seam review. VFS transaction early resolve still unfixed, mandatory separate
writer/review before D integration-only. No new candidate, compilation/tests,
engine/browser/IDB/network/service actions or staging. Commit17200e64 unchanged
from old pin; no upgraded-engine/FPS/root-cause claim. All tests user-owned.

## C browser source handoff (2026-10-11)

Exact D factory: `createCleanArtemisRuntime(host: RuntimeHost, options:
RuntimeOptions, urls: CleanEngineUrls): CleanArtemisRuntime`, or constructor
with the same arguments. Import from `compat-clean/runtime.js`; this round
changes no public index, package exports, asset copy or candidate page. D must
provide distinct explicit absolute workerUrl/engineJsUrl/wasmUrl from one A
build, with glue `artemis-compat.js` and paired `artemis-compat.wasm`. Classic
worker is compat-clean/compat-worker.js; either concatenate independently
reviewed compat/worker-audio.js BEFORE it, or supply absolute audioHelperUrl.
Main TS dependency closure: engine-api plus new worker-host and existing
compat/{protocol,read-host,audio-relay}. Never include old runtime/worker-host/
compat-worker scheduling owners or resolve their configured assets by default.

Existing player-host GameLoop supplies the single rAF through Runtime.tick;
C drops busy ticks, permits one frame RPC, never schedules another on ACK.
One Worker alone instantiates WASM. Init handshake and all RPCs have 30s
settlement deadlines; epoch/request identity rejects old replies. First frame
boots B and must report started=1/running. Resize sets Offscreen backing before
CABI CSS resize; B policy/clock/MapPoint unchanged. Error/exit stops driving,
pending rejects, reader wakes, speaker closes, native dispose then bounded
Worker termination releases MEMFS/GL. Input and explicit pause/resume are gated;
no performance flags, trace controls or worker frame timer.

Parent approved recursive ALL non-mounted loose preloading, sequential reads
and writes into real /game MEMFS before saved-file hydration/native open/boot.
Only root and .000–.063 containers with actual pf2/pf6/pf8 magic are range
mounted; no arbitrary extension skip. Path escapes/pack conflicts fail load.
Any enumerate/read/write failure terminates load, without deleting IDB. MEMFS
accumulates all loose: startup memory/time cost is not bounded. Range bridge
uses fixed 4MiB SAB chunks, exact count/-1 with fatal diagnosis, fresh HEAPU8,
and main never Atomics.wait. Existing source list/readFile/readRange interfaces
are consumed; assumed correctness/completeness of those interfaces is not
verified by static inspection.

Save/load native ARCV bank/CRC validation precedes mutation and file-set
bounds remain. Exported FS.write and FS.rename observe real native writes
(including atomic temporary rename), so checkpoint captures saves, not whole
loose tree. Existing WritableStore prefix artemis-compat/ and host game
namespace `saves:${gameId}` stay unchanged. Writes are awaited/error propagated,
atomic=false, no schema/IDBFS changes. HARD PRE-D DEPENDENCY: existing
packages/vfs/src/sources.ts:385–397 IndexedDbStore.run resolves on
request.onsuccess before tx.oncomplete and misses abort. Parent separately
approved VFS writer+review before D; C does not fix or bypass it and cannot
claim durable transaction commit. Pull audio reuses consumed ACK relay,
<=4096 stereo helper, native monotonic handles/loop/EOF, epochs on restore,
mute settings and gesture unlock; no produced/wall-clock fake ACK.

Read scope: current summaries/ledger, source contract and B author/freshreview;
actual index, full old runtime/worker-host (reference only), protocol/read-host,
worker-audio/audio-relay, engine-api types/sources, player-host loop/core session,
VFS source/store consumers; clean PackManager/EngineContext/PF8/save/Lua
snapshot and log semantics, manifest/CMake/ABI/B range adapter. Local SDK
libfs/libsyscall/libwasi write/rename seams only statically inspected, not run.
No fourth source/ABI/engine edits. types actually reside in engine-api/types.ts,
not the guessed compat/types.ts (ENOENT stopped, parent approved bounded find).
An exact-text ledger edit failed without writes; three docs byte-equal-before
confirmed in recovery-partial.json, parent approved read/targeted-edit recovery.

Guard .ref/artemis-compat-clean-browser-baseline/ retains 6783-key before,
three raw document snapshots, recovery partial diff/source hashes, final
before/after inventory and new C hashes. Allowed docs separated from protected
sources; old current history suffix retained. No compilation/tests/check/
network/browser/engine/HTTP/service actions, no staging. SOURCE_READY is not
review acceptance, official pseudo equivalence, FPS improvement or game-ready.

## Frozen ABI and platform semantics

Manifest `cAbi` contains exact declarations derived from old native exports
and actual worker consumers, not guessed names. All symbols retain `artc_web_`.
`open(const char*,const char*) -> int` creates GL and opens the pack but does not
run framework; first `frame(void) -> int` starts it. Frame returns 1 continue /
0 exit; no internal rAF. started/stage_width/stage_height/revision/presented/
exit_requested are int reports, last_error returns const char*, pause(int)
returns int, now_ms returns double, dispose returns void, save/load(const char*)
return int. pointer(float,float,int), pointer_move(float,float), key(int,int),
resize(int,int) are void. log_sink retains its C callback signature.
Pause freezes engine clock/audio; dispose idempotently shuts down while GL is
current before destroying it. Save/load remain ARCV variable-bank operations,
not invented scene snapshots. windows default and `/game/root.pfs`, `/game`
parent save paths remain unchanged; worker dialogs must refuse without window.

MapPoint stays exactly the old host's lines 338–345 (frozen known defect):

```cpp
    void MapPoint(float px, float py, float *sx, float *sy) const {
        const float s = std::min((float)css_w / (float)stage_w, (float)css_h / (float)stage_h);
        const float vw = (float)stage_w * s, vh = (float)stage_h * s;
        const float vx = ((float)css_w - vw) * 0.5f, vy = ((float)css_h - vh) * 0.5f;
        *sx = (px - vx) / vw * (float)stage_w;
        *sy = (py - vy) / vh * (float)stage_h;
    }
```

Baseline sequencing comes from clean src/host/mac_main.mm: RunEnterFrame,
DrainQueued, Step (original four-instruction bound), Update(NowMs), another
NowMs read, native drawn_rev/animating redraw gate, Draw when eligible, EndFrame.
Parent withdrew the single-clock/unconditional-Draw simplification: this native
gate is not the old scene-commit gate. Boot/reset/exit retain clean order, no old
pre-BootFramework Draw. No old 4096 transaction, patched tween/script or rAF.
Upstream render/script/core stays complete and unchanged.

Audio signatures and exports are frozen in manifest: handles and cumulative ACK
are double (safe integer validation); read_stereo(double,uintptr_t,int,intptr_t,
intptr_t) returns int, stereo int16, maximum 4096 frames, 0 empty / -1 refused.
Aligned in-heap destinations and optional gain/pan int spans must be validated;
never overproduce. ACK cannot go backwards or exceed produced. PlaybackMs is
consumed frames only, not decode cursor/wall clock. Source EOF waits for ACK;
unknown/stopped handles never resolve to new voices.

PF8 replacement is only the native I/O seam: Module.artcReadRange(path,offset,
dest,length) gives exact length or -1. Worker range bridge uses a 4MiB SAB,
missing -1 / fatal -2, splits larger reads and reacquires HEAPU8 after growth.
Retain the clean pack parser, seek/range bounds, patch order and loose sidecars;
no whole-pack MEMFS preload. No patches 0001–0006 as inputs.

## Evidence and limits

Pre/post protection inventories and static diffs are in guarded new
`.ref/artemis-compat-clean-build-baseline/`. Inventory starts with the prior
1157-path protected-after.json keys and expands existing old compat port,
runtime-artemis and fresh clone (including metadata/git) files. It is not an
all-disk claim. Original metadata/clone, old package/page/assets/pin/source and
session handoff are protected. Only two current-doc prefixes may change;
all their prior history bytes are retained after the original top summary.
No stage/commit/push/reset/stash/clean; main HEAD remains 9dd18ecf.

Manifest/bundle/build share identity and replacements; bundle carries complete
platform/recipe and byte inventories, not merely WASM. The GNU GPL upstream
license and project GPL-3.0-or-later do not clear official SDK/pseudo-C proprietary
rights. No official text is copied. SDK health, linkability, runtime ownership,
visual behavior, FPS and licensing remain unverified. Reviewer gate required.

## B implementation and recovery (2026-10-11)

Original run d89b6dd6-a36c-42c9-a9ba-90c0c4844f35 retained only a truncated
Traceback at default Path.read_text. Diagnosis ee6d3ace-e089-4770-897d-686243e51a86
separately reproduced GBK UnicodeDecodeError: byte 0x94 at position 1200; UTF-8
strict succeeded. This is not the recovered original full error. Resume reused
five partial originals, parsed/rehashed all 6774 before entries with zero drift,
and explicitly used UTF-8 strict for Python text/JSON. Later long docs heredoc
was truncated: bash line87 wanted PY; stdin:20 SyntaxError: unterminated
triple-quoted string literal (detected at line86), EXIT1. No Python writes ran.
Parent approved one bounded continuation via short commands and targeted edits.

The six new B sources implement all 28 manifest declarations, including nine
audio exports and log_sink, with shared exact host/web_abi.h. No A recipe writes.
open(pack,os_id), default windows: parent corrected the mistaken saveDir argument;
EngineContext::Open uses pack parent, /game for /game/root.pfs. Open creates GL/
pack only; first explicit frame Start/BootFramework. Reopen, dispose, boot failure
and exit Shutdown while context selected, then destroy GL. Context loss fails
with last_error and teardown; no auto-restoration or silent success. Lifecycle,
save/load and PCM decoder exceptions are diagnosed. Pause calls clean Lua
PauseClock/PauseAudio and Resume hooks; no new clock/performance strategy.
Letterbox/default FBO uses canvas backing size, resize stores CSS dimensions;
worker owns backing resize. Native redraw gate remains, no invented resize
invalidation. Worker window dialogs decline; clean compositor/stb owns fonts.

Clean mac_main.mm anchors: Boot :128–183, DrainQueued/Step :226–265,
Present/Tick :267–366, gate :335–349. Reset/exit precede Boot, then input,
RunEnterFrame/DrainQueued/Step with original four-line wait checks, Update and
separate NowMs read, native gate/Draw/EndFrame. No R1waitgate/R2Step4096 or
scene-commit/R3renderer/R4owner. Frozen old host MapPoint :338–345 function and
spacing raw bytes match; known defect untouched, evidence in guard directory.

PF8 is clean src/pack/pf8_reader.cpp full implementation with only Open/ReadAt
I/O changes and Emscripten include. Open(path) uses Module.artcReadRange exact
bytecount/-1, bounded signed return/32-bit offset/heap spans, negative checked
before unsigned conversion. OpenFd retains real owned fd/pread/close. From
OpenFd onward normalized text equals clean upstream; header/index/SHA1/key/
lookup/decrypt/range algorithms unmodified. No fake range descriptor/provider.
Clean PackManager::OpenChain :22–58 scans .000–.063 across gaps; Read :60–80
searches reverse (later wins), then LoosePath/ifstream without provider.

Audio trio reuse: ports/artemis/compat-web/overlay/audio/audio_web.h/.cpp and
audio_validation.h, self-owned repository GPL-3.0-or-later platform code; clean
GPL Audio/PcmStream/Vorbis headers/decoder unchanged. Full independent pull
backend/header/validation reused, no R1–R4 scheduling dependencies. Only backend
functional revision: handle allocation monotonic for WASM-instance lifetime,
not per Audio, survives stop/destroy/reopen; entire double handle <=2^53-1,
checked before replacement, exhaustion false, no wrap/recycle. <=4096 stereo
int16 frames with aligned heap validation; finite integral handle/ACK validation,
backward/past-produced/stale refusal. PlaybackMs consumed-only, EOF waits for ACK,
Stop clears indexes/decoder, paused voices return empty, loops use clean Vorbis.
Zero-frame pull still resolves handle and rejects stale identity.

C consumption (not implemented): #canvas selector resolving OffscreenCanvas,
matching 28 CABI (open os_id), first explicit frame, backing resize, range exact
count/-1 with missing -1/fatal -2 worker handling, 4MiB SAB chunks and fresh
HEAPU8 after growth, /game/root.pfs plus patch descriptor mapping. Packed files
remain range-backed, never whole-pack MEMFS. Loose sidecars must exist as real
MEMFS files BEFORE synchronous ifstream reads; without a synchronous preparation
path C must prepare necessary loose files before start/load, not assume async
can penetrate ifstream. Which files need preparation remains C scoping work.
Restore saves in /game before boot through existing FS/IDB contract; persistence
sequencing belongs to C. Save/load use clean snapshots, no invented export.

B guard .ref/artemis-compat-clean-platform-baseline/ keeps original five before
files; new MapPoint.txt, source diffs, resume-partial-hashes.json/diff,
protected-after.json, static-evidence.json, new-files-hashes.json and
diff-after.patch cover author static evidence. Allowed before changes only
three docs; both current history suffix bytes retained. A manifest SHA256 stays
baac3a5c17d097729b8f0682c62c7229854ecdd69bec9de2073696bf93b8b32b.
No compilation/tests/network/SDK/browser/game/API/service operations or staging.
C/D absent; SDK/linkability/visual/audio end-to-end/FPS/game/official proprietary
redistribution rights unknown. SOURCE_READY is author completion, not review or
runtime acceptance; fresh independent static reviewer is next gate.
