// Absolute-elapsed clock check. Not the shipping Compositor: that object is
// compiled by artemis-compat-tween.test.mjs from pin 17200e64. This probe only
// checks that Update(now_ms) keys off (now - start), so 60/39/120 Hz and a
// stall share one scale. A private .test copy cannot run on CI.
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <string>
#include <vector>
namespace {
enum EaseFamily { kFamQuad = 0 };
enum EaseDir { kDirIn = 0, kDirOut };
int ParseEase(const std::string &name) {
    if (name.rfind("easeout_", 0) == 0 && name.substr(8) == "quad") return 1 + kFamQuad * 3 + kDirOut;
    return 0;
}
float EaseIn(int, float t) { return t * t; }
float ApplyEase(int ease, float t) {
    if (t <= 0) return 0;
    if (t >= 1) return 1;
    if (ease <= 0) return t;
    const int fam = (ease - 1) / 3;
    const int dir = (ease - 1) % 3;
    if (dir == kDirIn) return EaseIn(fam, t);
    if (dir == kDirOut) return 1 - EaseIn(fam, 1 - t);
    return t < 0.5f ? EaseIn(fam, t * 2) / 2 : 1 - EaseIn(fam, (1 - t) * 2) / 2;
}
float ToF(const std::string &s, float def = 0) {
    try { return s.empty() ? def : std::stof(s); } catch (...) { return def; }
}
}  // namespace
struct Layer { std::string id; float sy = 1; };
struct Compositor {
    struct Tween {
        std::string id, param; float from = 0, to = 0;
        double start_ms = 0, delay_ms = 0, time_ms = 0; int ease = 0;
        bool from_current = true, started = false, yoyo = false; int repeat = 0;
        double Duration() const { return time_ms <= 0 ? delay_ms : delay_ms + time_ms * (1.0 + repeat); }
        float FinalValue() const { return yoyo && repeat >= 0 && (repeat & 1) ? from : to; }
    };
    double now_ms_ = 0; std::vector<Layer> layers_; std::vector<Tween> tweens_;
    static bool ReadParam(const Layer &l, const std::string &param, float *v) {
        if (param == "yscale") { *v = l.sy * 100.0f; return true; } return false;
    }
    static bool ApplyParam(Layer &l, const std::string &param, float v) {
        if (param == "yscale") { l.sy = v / 100.0f; return true; } return false;
    }
    void AddTween(const std::string &id, const std::map<std::string, std::string> &attrs, double now_ms);
    bool Update(double now_ms);
    double PendingTextMs(double) const { return 0; }
    void AdvanceAnime(double, bool *) {}
};
void Compositor::AddTween(const std::string &id, const std::map<std::string, std::string> &attrs, double now_ms) {
    auto get = [&](const char *k) -> std::string { auto it = attrs.find(k); return it == attrs.end() ? std::string() : it->second; };
    Tween tw; tw.id = id; tw.param = get("param");
    tw.time_ms = std::max(0.0f, ToF(get("time"), 0));
    tw.delay_ms = std::max(0.0f, ToF(get("delay"), 0));
    tw.ease = ParseEase(get("ease")); tw.start_ms = now_ms;
    const std::string from = get("from"), to = get("to");
    tw.to = ToF(to); tw.from = ToF(from); tw.from_current = false;
    tweens_.push_back(tw);
}
bool Compositor::Update(double now_ms) {
    bool changed = PendingTextMs(now_ms_) > 0;
    now_ms_ = now_ms;
    for (auto it = tweens_.begin(); it != tweens_.end();) {
        Tween &tw = *it;
        const double since = now_ms - tw.start_ms;
        if (since < tw.delay_ms) { ++it; continue; }
        Layer *target = nullptr;
        for (auto &l : layers_) if (l.id == tw.id) { target = &l; break; }
        if (!target) { it = tweens_.erase(it); continue; }
        if (!tw.started) {
            tw.started = true;
            if (tw.from_current) { float cur = tw.to; if (ReadParam(*target, tw.param, &cur)) tw.from = cur; }
        }
        const double run = since - tw.delay_ms;
        const bool done = tw.time_ms <= 0 || (tw.repeat >= 0 && run >= tw.time_ms * (1.0 + tw.repeat));
        float v = tw.FinalValue();
        if (!done) {
            const double cycle = std::floor(run / tw.time_ms);
            const float t = static_cast<float>(std::fmod(run, tw.time_ms) / tw.time_ms);
            const bool reverse = tw.yoyo && std::fmod(cycle, 2.0) >= 1.0;
            const float from = reverse ? tw.to : tw.from;
            const float to = reverse ? tw.from : tw.to;
            v = from + (to - from) * ApplyEase(tw.ease, t);
        }
        ApplyParam(*target, tw.param, v);
        changed = true;
        if (done) it = tweens_.erase(it); else ++it;
    }
    AdvanceAnime(now_ms, &changed);
    return changed;
}
static int fail = 0;
static void expect(const char *name, double got, double want) {
    if (std::fabs(got - want) > 1e-4) { std::printf("FAIL %s got=%.6f want=%.6f\n", name, got, want); ++fail; }
    else std::printf("ok %s %.6f\n", name, got);
}
static double run(const char *name, double step, bool stall) {
    Compositor c; c.layers_.push_back({"500.ch.2", 1.5f});
    c.AddTween("500.ch.2", {{"param","yscale"},{"from","150"},{"to","100"},{"time","2000"},{"ease","easeout_quad"}}, 0);
    double t = 0; double mid = -1;
    for (; t < 2000; t += step) { c.Update(t); if (mid < 0 && t >= 1000) mid = c.layers_[0].sy * 100.0; }
    if (stall) { c.Update(1000); }
    double at1000 = -1; Compositor d; d.layers_.push_back({"500.ch.2", 1.5f});
    d.AddTween("500.ch.2", {{"param","yscale"},{"from","150"},{"to","100"},{"time","2000"},{"ease","easeout_quad"}}, 0);
    d.Update(0); d.Update(1000); at1000 = d.layers_[0].sy * 100.0;
    c.Update(2000);
    std::printf("%s step=%.3f mid=%.4f stall1000=%.4f end=%.4f\n", name, step, mid, at1000, c.layers_[0].sy * 100.0);
    expect(name, c.layers_[0].sy * 100.0, 100.0);
    expect("mid", at1000, 112.5);
    return at1000;
}
int main() {
    double a = run("60Hz", 1000.0 / 60.0, false);
    double b = run("39Hz", 1000.0 / 39.0, false);
    double c = run("120Hz", 1000.0 / 120.0, false);
    Compositor s; s.layers_.push_back({"500.ch.2", 1.5f});
    s.AddTween("500.ch.2", {{"param","yscale"},{"from","150"},{"to","100"},{"time","2000"},{"ease","easeout_quad"}}, 5000);
    s.Update(5000); s.Update(6000);
    expect("stall-same-absolute", s.layers_[0].sy * 100.0, 112.5);
    if (std::fabs(a - b) > 1e-4 || std::fabs(a - c) > 1e-4) { std::printf("FAIL rate changed midpoint\n"); ++fail; }
    std::printf(fail ? "probe-fail %d\n" : "probe-ok\n", fail);
    return fail ? 1 : 0;
}
