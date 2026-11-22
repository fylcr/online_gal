# clock-ms-01 — opt-in integer millisecond experiment

Complete LuaEngine translation-unit replacement from GPL-3.0-or-later upstream
pin 17200e64ea99681e3c27d694433c0ff78d7a7cee. Only NowMs conversion changes;
all other upstream bytes, including original line endings, are retained.
No proprietary decompiled implementation is included. Upstream licenses and
third-party notices remain in the corresponding source archive.

Unset ARTC_CLEAN_CLOCK retains the baseline manifest and sources. Only the
explicit value clock-ms-01 is accepted. Planned command (NOT executed here;
requires separately authorized new output/cache and independent static review):

```bash
ARTC_CLEAN_CLOCK=clock-ms-01 ARTC_CLEAN_CANDIDATE=clock-ms-01 bash ports/artemis/compat-web-clean/build.sh --parallel 2
```

Relative init epoch, independent sampling, ClockNow/Pause/Resume, absolute steady
deadlines, C ABI double and l_now implementation are unchanged. No frame cache,
external timestamps, absolute counter, mask, wrap, clamp or scheduler changes.
Nonnegative samples lose [0,1)ms; negative durations truncate toward zero.
Repeated values and TagWait pending>1/truncation boundaries can affect later
sequencing. No overall <1ms bound, official equivalence or FPS improvement claim.
l_now's later sampling time can change indirectly despite unchanged code.

Source bundle includes the selected manifest, replacement, baseline recipe,
upstream license/notices and source/port hash inventories. Reconstruction must
pass -DARTC_CLEAN_CLOCK=clock-ms-01 as recorded in BUILD.txt. Existing audio/PF8
substitutions and upstream header/include resolution remain in place.
SDK/version comparability, user runtime effects and proprietary SDK licensing
remain unknown. All runtime testing is user-owned; no output or runtime rewiring
is delivered in this stage.
