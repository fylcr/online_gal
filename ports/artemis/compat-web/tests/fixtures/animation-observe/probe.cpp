#include <bits/stdc++.h>
#define private public
#include "script/lua_engine.h"
#include "render/compositor.h"
#undef private
#include "script/asb_parser.h"
#include "pack/pack_manager.h"
#include "config/ini.h"
#include "log/logger.h"
static unsigned calls = 0;
extern "C" void real_tween(artc::Compositor *, const std::string &, const std::map<std::string, std::string> &, double) asm("__real_@TWEEN@");
extern "C" void wrapped_tween(artc::Compositor *c, const std::string &id, const std::map<std::string, std::string> &attrs, double now) asm("__wrap_@TWEEN@");
extern "C" void wrapped_tween(artc::Compositor *c, const std::string &id, const std::map<std::string, std::string> &attrs, double now) {
    ++calls; real_tween(c, id, attrs, now);
}
static unsigned records = 0;
static bool early = false;
static void check(bool ok, const char *name) {
    if (!ok) { std::cerr << "FAIL " << name << '\n'; std::exit(2); }
}
int main() {
    artc::PackManager packs; artc::Ini ini; artc::Compositor comp; artc::LuaEngine lua;
    artc::PackManager::FileProvider provider;
    provider.read = [](const std::string &path, std::vector<uint8_t> &out) {
        std::string portable = path; std::replace(portable.begin(), portable.end(), '\\', '/');
        if (portable.find("synthetic.iet") != std::string::npos) {
            const std::string script = "*top\n[stop]\n"; out.assign(script.begin(), script.end()); return true;
        }
        if (portable.find("image/cg") != std::string::npos) {
            const uint8_t png[] = {137, 80, 78, 71, 13, 10, 26, 10, 0, 0, 0, 13, 73, 72, 68, 82, 0, 0, 0, 1, 0, 0, 0, 1, 8, 2, 0, 0, 0,
                144, 119, 83, 222, 0, 0, 0, 12, 73, 68, 65, 84, 120, 156, 99, 248, 207, 192, 0, 0, 3, 1, 1, 0, 201, 254, 146, 239,
                0, 0, 0, 0, 73, 69, 78, 68, 174, 66, 96, 130};
            out.assign(png, png + sizeof(png)); return true;
        }
        return false;
    };
    check(packs.OpenChain("synthetic/root.pfs", {}, provider), "synthetic FileProvider");
    comp.Init(1280, 720);
    check(lua.Init(&packs, ini, "android", 1280, 720, &comp, nullptr, nullptr), "real Lua init");
    artc::SetLogSink([&](int, const std::string &line) {
        if (line.rfind("anim ", 0) == 0) { ++records; if (lua.seen_tags_.empty()) early = true; }
    });
    const uint8_t pixel[] = {255, 0, 0, 255};
    comp.SetPixels("repeat", pixel, 1, 1);
    const std::vector<std::pair<std::string, std::string>> attrs = {
        {"id", "repeat"}, {"param", "alpha"}, {"from", "0"}, {"to", "255"}, {"time", "100"}};
    check(lua.DoString("for i=1,3 do e:tag{'lytween',id='repeat',param='alpha',['from']='0',to='255',time='100'} end", "lua-observer"), "three l_tag calls");
    check(calls == 3, "real TagLyTween calls from Lua");
    for (int i = 0; i < 3; ++i) check(lua.DispatchTag("lytween", attrs, false), "direct DispatchTag");
    check(calls == 6, "real TagLyTween calls from DispatchTag");
    check(comp.tweens_.size() == 1, "original same-property replacement semantics");
    const double start = comp.tweens_[0].start_ms;
    comp.Update(start + 50); check(std::fabs(comp.Layers()[0].alpha - 0.5f) < 0.0001f, "real alpha midpoint");
    comp.Update(start + 100); check(comp.Layers()[0].alpha == 1 && comp.PendingAnimationMs(start + 100) == 0, "real alpha end");
    check(lua.DoString("outs=0; function observe_out(e) outs=outs+1 end; e:setEventHandler{onClickWaitOut='observe_out'}", "wait-hook"), "wait hook");
    lua.SetTimedWait(1);
    lua.wait_until_ = std::chrono::steady_clock::now() - std::chrono::milliseconds(100);
    check(lua.waiting_ && lua.timed_wait_, "expired wait is raw true before observation");
    check(lua.DispatchTag("lytween", attrs, false), "tag during expired wait");
    check(lua.waiting_ && lua.timed_wait_, "raw wait remains true after record");
    check(lua.DoString("assert(outs==0)", "before-poll"), "no OUT hook before poll");
    check(!lua.IsWaiting() && !lua.waiting_, "normal engine poll releases wait");
    check(lua.DoString("assert(outs==1)", "after-poll"), "exactly one normal OUT hook");
    artc::AsbRunner runner; runner.SetPackSource(&packs);
    check(runner.Jump("synthetic.iet", "top"), "real runner load");
    lua.SetScriptRunner(&runner);
    check(lua.DispatchTag("lyc", {{"id", "cg"}, {"file", "image/cg\nresource"}}, false), "CG resource");
    check(std::any_of(comp.Layers().begin(), comp.Layers().end(), [](const auto &l) { return l.id == "cg" && l.texture != 0; }), "real PNG load at GLES recorder boundary");
    check(lua.DispatchTag("lyprop", {{"id", "cg"}, {"xscale", "125"}, {"yscale", "75"}, {"zoom", "110"},
        {"anchorx", "12"}, {"anchory", "34"}, {"clip", "0,0,1,1"}, {"alpha", "128"}}, false), "CG transform");
    check(lua.DispatchTag("trans", {{"time", "100"}, {"type", "1"}}, false), "transition start");
    check(lua.transition_wait_, "original transition wait semantics");
    check(lua.DispatchTag("lytween", {{"id", "bounded"}, {"param", "alpha"}, {"from", std::string(1000, 'x')}, {"to", "0"}, {"time", "0"}}, false), "bounded attr");
    const unsigned before = records;
    check(lua.DispatchTag("lyprop", {{"id", "cg"}, {"left", "5"}}, false), "irrelevant CG event");
    check(lua.DispatchTag("print", {{"data", "NOT_IN_ANIM"}}, false), "non-animation tag");
    check(records == before, "non-animation has no new record");
    check(calls == 8, "all eight real tween calls");
    if (records) check(early, "record precedes first-seen summary dedup");
    std::cout << "SEMANTICS calls=" << calls << " alpha_mid=0.5 alpha_end=1 raw_wait=1 poll_wait=0 outs=1 transition_wait=1\n";
    std::cout << "RECORDS " << records << '\n';
    artc::SetLogSink({});
}
