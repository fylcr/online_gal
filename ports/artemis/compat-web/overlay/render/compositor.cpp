#include "render/compositor.h"

// Material, text and layer helpers remain from artemis-compat (GPL-3.0-or-later).
// Scene transaction replacement boundaries are recorded in docs/artemis-rewrite-r3.md.
#include "render/line_break.h"
#include "render/layer_kind.h"
#include "pack/pack_manager.h"
#include "pack/pf8_reader.h"
#include "log/logger.h"

// KrKr2-Next: the GLES2 compositor is also used on OpenHarmony (the host
// bridge makes an EGL context current before driving it). ARTC_HAS_GLES is
// set by cpp/artemis/CMakeLists.txt for every platform with a GLES2 sysroot.
#if defined(__ANDROID__) && !defined(ARTC_HAS_GLES)
#define ARTC_HAS_GLES 1
#endif

#if defined(ARTC_HAS_GLES)
#include "render/gles2_headers.h"
#include "render/shader_compat.h"
#define STB_IMAGE_IMPLEMENTATION
#include "render/stb_image.h"
#include "render/stb_truetype.h"   // implementations live in glyph_atlas.cpp
#endif

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cmath>
#include <set>
#include <cstdio>
#include <limits>
#include "util/save_storage.h"

namespace artc {

bool Compositor::LoadShader(const std::string& id,const std::string& file) {
    std::vector<uint8_t> bytes;
    if(!packs_ || !packs_->Read(file,bytes)) {
        Log(kLogError,"lyshader: cannot read "+file);return false;
    }
    const bool ok=shaders_.Load(id,std::string(bytes.begin(),bytes.end()));
    Log(ok?kLogInfo:kLogError,"lyshader: "+id+(ok?" loaded ":" failed ")+file);
    if(ok)++revision_;
    return ok;
}


// Artemis layer z is a two-level sort. The leading integer is the primary
// layer ("600", "1", "-273"); within the same primary layer the SECOND id
// segment breaks ties the way the engine draws it:
//   * numeric second segments sort by value — the message layer ("1.80…")
//     sits ABOVE the scene group ("1.0…") because 80 > 0;
//   * a letter second segment sorts ABOVE any number — the title button
//     ("500.d.…") sits ABOVE the title background ("500.0").
// Official z-order spec (spec/layer.md): stacking is decided by the LAYER ID.
//   * purely-numeric ids sort numerically (larger = drawn on top);
//   * an id with a leading number sorts by that number's VALUE first, then
//     by the string tail (so "500.d.1.0" > "500.0", and "1hoge" < "20hoge",
//     matching the documented example);
//   * an id with NO leading number ("bg", "froid"...) sorts to the bottom
//     (letter-only names are framework helper/background layers and must not
//     cover the numeric layer-set UI and scene layers).
int ZCmp(const std::string &a, const std::string &b) {
    auto lead = [](const std::string &s, int *start) -> long long {
        *start = 0;
        size_t i = 0;
        while (i < s.size() && s[i] >= '0' && s[i] <= '9') ++i;
        if (i == 0) return -1;                 // no leading number
        long long v = 0;
        errno = 0;
        char *end = nullptr;
        v = std::strtoll(s.c_str(), &end, 10);
        if (end == s.c_str() || errno == ERANGE) v = 0;
        *start = static_cast<int>(i);
        return v;
    };
    int sa = 0, sb = 0;
    const long long pa = lead(a, &sa);
    const long long pb = lead(b, &sb);
    if (pa == -1 && pb == -1) return a == b ? 0 : (a < b ? -1 : 1);
    if (pa != pb) return pa < pb ? -1 : 1;
    // same leading value: compare the whole strings lexicographically
    // (numeric tails compared on equal numbers fall back to the id text).
    if (a == b) return 0;
    return a < b ? -1 : 1;
}

// Layer hierarchy depth ("500.d.1.0" → 4 segments). Ties at the same z break
// toward deeper layers: the title background ("500.0") and its buttons
// ("500.d.*") share z=500, and the background must render *below* the buttons
// regardless of insertion order. Behavior observed from the real engine: the
// background is created after the buttons yet never covers them.
int SectionCount(const std::string &id) {
    int n = 1;
    for (const char c : id)
        if (c == '.') ++n;
    return n;
}

int LayerZCmp(const Layer& a, const Layer& b) {
    if (a.message_overlay != b.message_overlay) return a.message_overlay ? 1 : -1;
    return ZCmp(a.id, b.id);
}

// Full paint order: explicit paint hints (smaller = earlier/behind) come
// before the id z order; layers without a hint (0) stay put relative to the
// id order, negative hints draw behind and positive hints in front.
bool DrawOrderLess(const Layer *a, const Layer *b) {
    const int pa = a->effect.paint, pb = b->effect.paint;
    if (pa != pb) {
        if (pa == 0) return pb > 0;
        if (pb == 0) return pa < 0;
        return pa < pb;
    }
    const int c = LayerZCmp(*a, *b);
    if (c != 0) return c < 0;   // ascending: lower z first
    return SectionCount(a->id) < SectionCount(b->id);
}

void Compositor::EffectiveRect(const Layer &l, float *ex, float *ey,
                               float *ea, bool *ev) const {
    float w, h;
    EffectiveRect(l, ex, ey, &w, &h, ea, ev);
}

// The reference CDisplayObject::ApplyPropertyToMatrix composes
// T(position) T(anchor) R(clockwise degrees) S(scale) S(reverse) T(-anchor).
// Compose the complete ancestor chain, so rotation and mirroring move face
// parts, glyphs and hit regions together, including nonuniform parent scales.
Compositor::Transform Compositor::EffectiveTransform(const Layer& l) const {
    std::vector<const Layer*> chain;
    std::string id=l.id;
    size_t dot;
    while ((dot=id.rfind('.'))!=std::string::npos) {
        id.resize(dot);
        for (const auto& parent:layers_)
            if (parent.id==id) { chain.push_back(&parent); break; }
    }
    Transform m;
    auto apply=[&](const Layer& n) {
        const float radians=std::remainder(n.rotate,360.f)*3.14159265358979323846f/180.f;
        float co=std::cos(radians), si=std::sin(radians);
        // Exact quadrants should not produce cracks or fail edge hit tests.
        if (std::abs(co)<1e-7f) co=0;
        if (std::abs(si)<1e-7f) si=0;
        const float sx=n.sx*(n.reverse_x ? -1.f : 1.f);
        const float sy=n.sy*(n.reverse_y ? -1.f : 1.f);
        const float a=co*sx, b=si*sx, c=-si*sy, d=co*sy;
        const float tx=n.x+n.ax-a*n.ax-c*n.ay;
        const float ty=n.y+n.ay-b*n.ax-d*n.ay;
        const auto origin=m.Point(tx,ty);
        const float ma=m.a*a+m.c*b, mb=m.b*a+m.d*b;
        const float mc=m.a*c+m.c*d, md=m.b*c+m.d*d;
        m.a=ma; m.b=mb; m.c=mc; m.d=md; m.tx=origin.first; m.ty=origin.second;
        m.alpha*=n.alpha; m.visible=m.visible && n.visible;
    };
    for (auto it=chain.rbegin();it!=chain.rend();++it) apply(**it);
    apply(l);
    const auto origin=m.Point(l.content_x,l.content_y);
    m.tx=origin.first; m.ty=origin.second;
    return m;
}

void Compositor::EffectiveRect(const Layer& l, float* ex, float* ey, float* ew,
                               float* eh, float* ea, bool* ev) const {
    const auto m=EffectiveTransform(l);
    *ea=m.alpha; *ev=m.visible;
    if (m.b==0 && m.c==0) {
        // Preserve the signed dimensions exposed by the scale-only API.
        *ex=m.tx; *ey=m.ty; *ew=m.a*l.w; *eh=m.d*l.h;
        return;
    }
    const auto p0=m.Point(0,0), p1=m.Point(l.w,0);
    const auto p2=m.Point(0,l.h), p3=m.Point(l.w,l.h);
    *ex=std::min({p0.first,p1.first,p2.first,p3.first});
    *ey=std::min({p0.second,p1.second,p2.second,p3.second});
    *ew=std::max({p0.first,p1.first,p2.first,p3.first})-*ex;
    *eh=std::max({p0.second,p1.second,p2.second,p3.second})-*ey;
}

bool Compositor::ContainsPoint(const Layer& l, float x, float y) const {
    const auto m=EffectiveTransform(l);
    const float det=m.a*m.d-m.b*m.c;
    if (!m.visible || !l.texture || !std::isfinite(det) || std::abs(det)<1e-8f) return false;
    x-=m.tx; y-=m.ty;
    const float local_x=(m.d*x-m.c*y)/det, local_y=(m.a*y-m.b*x)/det;
    return local_x>=0 && local_y>=0 && local_x<l.w && local_y<l.h;
}

bool Compositor::ParentDelta(const std::string& id, float dx, float dy, float* x, float* y) const {
    // An identity child with this id collects exactly the real layer's ancestors.
    Layer child; child.id=id;
    const auto m=EffectiveTransform(child);
    const float det=m.a*m.d-m.b*m.c;
    if (!std::isfinite(det) || std::abs(det)<1e-8f) return false;
    *x=(m.d*dx-m.c*dy)/det;
    *y=(m.a*dy-m.b*dx)/det;
    return true;
}

bool Compositor::HomeLayerToTextPen(const std::string& text_id,
                                    const std::string& glyph_id) {
    const Layer* text = nullptr;
    for (const auto& l : layers_)
        if (l.id == text_id) { text = &l; break; }
    if (!text) return false;
    const auto tm = EffectiveTransform(*text);
    const auto pen = tm.Point(text->text_pen_x, text->text_pen_y);
    // An identity probe with the glyph's id collects exactly its ancestors,
    // so the stage-space pen converts back into the glyph's parent space.
    Layer probe; probe.id = glyph_id;
    const auto gm = EffectiveTransform(probe);
    const float det = gm.a * gm.d - gm.b * gm.c;
    if (!std::isfinite(det) || std::abs(det) < 1e-8f) return false;
    const float dx = pen.first - gm.tx, dy = pen.second - gm.ty;
    const float lx = (gm.d * dx - gm.c * dy) / det;
    const float ly = (gm.a * dy - gm.b * dx) / det;
    ++revision_;
    for (auto& l : layers_) {
        if (l.id != glyph_id) continue;
        l.x = lx; l.y = ly; l.own_pos = true;
        return true;
    }
    // Same fallback as SetProps: a pure group layer may never have been
    // created by lyc — materialize it so children inherit the transform.
    Layer g;
    g.id = glyph_id;
    g.x = lx; g.y = ly; g.own_pos = true;
    layers_.push_back(g);
    return true;
}

std::string Compositor::HitLayer(float x, float y) const {
    const Layer *best = nullptr;
    for (const auto &l : layers_) {
        if (!ContainsPoint(l,x,y)) continue;
        if (!best) best = &l;
        else {
            const int c = LayerZCmp(l, *best);
            const bool lFront = c > 0 ||
                                (c == 0 &&
                                 SectionCount(l.id) > SectionCount(best->id));
            if (lFront) best = &l;
        }
    }
    return best ? best->id : std::string();
}

// [var system="get_layer_info"] — stored (relative) offsets and draggable
// bounds. slider_dragX reads left to derive the percentage, so we return the
// raw stored value, not the cascade-resolved absolute position.
// ---------------------------------------------------------------------------
// KrKr2-Next: tween engine (platform independent)
// ---------------------------------------------------------------------------
namespace {
// Artemis exposes 10 easing families (quad/cubic/quart/quint/expo/circ/sine/
// back/elastic/bounce) in in/out/inout directions, plus linear. Encode a curve
// as 1 + family*3 + direction so parsing and dispatch stay compact.
enum EaseFamily { kFamQuad = 0, kFamCubic, kFamQuart, kFamQuint, kFamExpo,
                  kFamCirc, kFamSine, kFamBack, kFamElastic, kFamBounce, kFamCount };
enum EaseDir { kDirIn = 0, kDirOut, kDirInOut };

int ParseEase(const std::string &name) {
    if (name.empty() || name == "none" || name == "linear") return 0;
    std::string family = name;
    int dir = kDirIn;
    if (name.rfind("easeinout_", 0) == 0) { dir = kDirInOut; family = name.substr(10); }
    else if (name.rfind("easein_", 0) == 0) { dir = kDirIn; family = name.substr(7); }
    else if (name.rfind("easeout_", 0) == 0) { dir = kDirOut; family = name.substr(8); }
    int fam = -1;
    if (family == "quad") fam = kFamQuad;
    else if (family == "cubic") fam = kFamCubic;
    else if (family == "quart") fam = kFamQuart;
    else if (family == "quint") fam = kFamQuint;
    else if (family == "expo" || family == "exponential") fam = kFamExpo;
    else if (family == "circ" || family == "circular") fam = kFamCirc;
    else if (family == "sine" || family == "sin") fam = kFamSine;
    else if (family == "back") fam = kFamBack;
    else if (family == "elastic") fam = kFamElastic;
    else if (family == "bounce") fam = kFamBounce;
    if (fam < 0) return 0; // unknown -> linear
    return 1 + fam * 3 + dir;
}

float OutBounce(float t) {
    const float n1 = 7.5625f, d1 = 2.75f;
    if (t < 1 / d1) return n1 * t * t;
    if (t < 2 / d1) { t -= 1.5f / d1; return n1 * t * t + 0.75f; }
    if (t < 2.5f / d1) { t -= 2.25f / d1; return n1 * t * t + 0.9375f; }
    t -= 2.625f / d1; return n1 * t * t + 0.984375f;
}

float InBounce(float t) { return 1 - OutBounce(1 - t); }

// In-direction curve for one family, in [0,1].
float EaseIn(int fam, float t) {
    const float pi = 3.14159265f;
    switch (fam) {
    case kFamQuad: return t * t;
    case kFamCubic: return t * t * t;
    case kFamQuart: return t * t * t * t;
    case kFamQuint: { const float u = t * t; return u * u * t; }
    case kFamExpo: return t <= 0 ? 0 : std::pow(2.0f, 10 * t - 10);
    case kFamCirc: return 1 - std::sqrt(1 - t * t);
    case kFamSine: return 1 - std::cos(t * pi / 2);
    case kFamBack: { const float c1 = 1.70158f, c3 = c1 + 1; return c3 * t * t * t - c1 * t * t; }
    case kFamElastic: {
        if (t <= 0) return 0;
        if (t >= 1) return 1;
        const float c4 = (2 * pi) / 3;
        return -std::pow(2.0f, 10 * t - 10) * std::sin((t * 10 - 10.75f) * c4);
    }
    case kFamBounce: return InBounce(t);
    default: return t;
    }
}

float ApplyEase(int ease, float t) {
    if (t <= 0) return 0;
    if (t >= 1) return 1;
    if (ease <= 0) return t; // linear
    const int fam = (ease - 1) / 3;
    const int dir = (ease - 1) % 3;
    if (dir == kDirIn) return EaseIn(fam, t);
    if (dir == kDirOut) return 1 - EaseIn(fam, 1 - t);
    // inout: back/elastic/bounce use a widened-parameter blend, the rest are
    // the reflected in-curve across the midpoint.
    const float pi = 3.14159265f;
    if (fam == kFamBack) {
        const float c1 = 1.70158f, c2 = c1 * 1.525f;
        return t < 0.5f
                   ? (std::pow(2 * t, 2) * ((c2 + 1) * 2 * t - c2)) / 2
                   : (std::pow(2 * t - 2, 2) * ((c2 + 1) * (2 * t - 2) + c2) + 2) / 2;
    }
    if (fam == kFamElastic) {
        const float c5 = (2 * pi) / 4.5f;
        return t < 0.5f
                   ? -(std::pow(2.0f, 20 * t - 10) * std::sin((20 * t - 11.125f) * c5)) / 2
                   : (std::pow(2.0f, -20 * t + 10) * std::sin((20 * t - 11.125f) * c5)) / 2 + 1;
    }
    if (fam == kFamBounce)
        return t < 0.5f ? (1 - InBounce(1 - 2 * t)) / 2
                        : (1 + OutBounce(2 * t - 1)) / 2;
    return t < 0.5f ? EaseIn(fam, t * 2) / 2
                    : 1 - EaseIn(fam, (1 - t) * 2) / 2;
}

float ToF(const std::string &s, float def = 0) {
    if (s.empty()) return def;
    errno = 0;
    char *end = nullptr;
    const float v = std::strtof(s.c_str(), &end);
    if (end == s.c_str() || errno == ERANGE) return def;
    return v;
}
int ToI(const std::string &s, int def = 0) {
    if (s.empty()) return def;
    errno = 0;
    char *end = nullptr;
    const long v = std::strtol(s.c_str(), &end, 10);
    if (end == s.c_str() || errno == ERANGE) return def;
    return static_cast<int>(v);
}
} // namespace

bool Compositor::IsProhibitHead(uint32_t cp) const {
    return prohibit_head_.empty() ? ProhibitLineStart(cp) : prohibit_head_.count(cp) != 0;
}
bool Compositor::IsProhibitFoot(uint32_t cp) const {
    return prohibit_foot_.empty() ? ProhibitLineEnd(cp) : prohibit_foot_.count(cp) != 0;
}
bool Compositor::IsWordpart(uint32_t cp) const {
    return wordparts_.count(cp) != 0;
}

namespace {
std::vector<uint32_t> DecodeUtf8Codepoints(const std::string &s) {
    std::vector<uint32_t> out;
    for (size_t i = 0; i < s.size();) {
        const unsigned char c = static_cast<unsigned char>(s[i]);
        uint32_t cp = c;
        size_t n = 1;
        if (c >= 0xF0) { cp = c & 0x07; n = 4; }
        else if (c >= 0xE0) { cp = c & 0x0F; n = 3; }
        else if (c >= 0xC0) { cp = c & 0x1F; n = 2; }
        for (size_t k = 1; k < n && i + k < s.size(); ++k)
            cp = (cp << 6) | (static_cast<unsigned char>(s[i + k]) & 0x3F);
        out.push_back(cp);
        i += n;
    }
    return out;
}
} // namespace

void Compositor::SetProhibitRules(const std::string &head, const std::string &foot) {
    prohibit_head_.clear();
    prohibit_foot_.clear();
    for (uint32_t cp : DecodeUtf8Codepoints(head)) prohibit_head_.insert(cp);
    for (uint32_t cp : DecodeUtf8Codepoints(foot)) prohibit_foot_.insert(cp);
}
void Compositor::SetWordparts(const std::string &parts) {
    wordparts_.clear();
    for (uint32_t cp : DecodeUtf8Codepoints(parts)) wordparts_.insert(cp);
}
void Compositor::SetIndentRules(const std::string &pair, int range, bool nest) {
    indent_pair_ = pair;
    indent_range_ = range;
    indent_nest_ = nest;
}

void Compositor::SetLayerMesh(const std::string &id, const std::vector<float> &vertices) {
    if (id.empty()) return;
    for (auto &l : layers_)
        if (l.id == id) {
            if (l.mesh != vertices) { l.mesh = vertices; ++revision_; }
            return;
        }
}

void Compositor::SetMessageLayered(const std::string& id, bool layered) {
    if (id.empty()) return;
    SetProps(id, {});
    for (auto& l : layers_) if (l.id == id) {
        l.message_overlay = !layered;
        return;
    }
}

void Compositor::SetTextTween(const std::string& id, const std::map<std::string, std::string>& attrs) {
    if (id.empty()) return;
    const auto get = [&](const char* key) { const auto it=attrs.find(key); return it==attrs.end() ? std::string() : it->second; };
    if (get("type") != "in") return;
    SetProps(id, {});
    for (auto& l : layers_) if (l.id == id) {
        if (get("mode") == "init") l.text_in.clear();
        else if (get("mode") == "add") {
            const auto param=get("param");
            if (param=="alpha" || param=="left" || param=="top")
                l.text_in.push_back({param, std::max(0.f,ToF(get("delay"))),
                    std::max(0.f,ToF(get("time"))), ToF(get("diff")), ParseEase(get("ease"))});
        }
        return;
    }
}

void Compositor::SetGlyphTimes(Layer& l, std::vector<TextGlyph>& glyphs, const std::string& text) {
    const bool append=text.compare(0,l.text.size(),l.text)==0;
    std::map<size_t,double> old_times;
    if(append) for(const auto& g:l.glyphs) old_times[g.order]=g.start_ms;
    double delay=0;
    for(const auto& t:l.text_in) delay=std::max(delay,t.delay_ms);
    const size_t first_new=old_times.empty() ? 0 : old_times.rbegin()->first+1;
    const double next=old_times.empty() ? now_ms_ : std::max(now_ms_,old_times.rbegin()->second+delay);
    for(auto& g:glyphs) {
        const auto old=old_times.find(g.order);
        g.start_ms=old!=old_times.end() ? old->second : next+(g.order-first_new)*delay;
    }
    l.glyphs=std::move(glyphs); l.text=text;
}

double Compositor::TextEnd(const Layer& l) {
    if (l.text_in.empty() || l.glyphs.empty()) return 0;
    double duration=0;
    for(const auto& tw:l.text_in) duration=std::max(duration,tw.time_ms);
    double end=0;
    for(const auto& g:l.glyphs) end=std::max(end,g.start_ms+duration);
    return end;
}

double Compositor::PendingTextMs(double now_ms) const {
    double end=now_ms;
    for(const auto& l:layers_) {
        float x,y,a; bool visible;
        EffectiveRect(l,&x,&y,&a,&visible);
        if(visible && a>0) end=std::max(end,TextEnd(l));
    }
    return end-now_ms;
}

bool Compositor::FinishText(double now_ms) {
    bool changed=false;
    for(auto& l:layers_) {
        float x,y,a; bool visible;
        EffectiveRect(l,&x,&y,&a,&visible);
        const double remaining=TextEnd(l)-now_ms;
        if (!visible || a<=0 || remaining<=0) continue;
        for(auto& g:l.glyphs) g.start_ms-=remaining;
        changed=true;
    }
    if(changed) ++revision_;
    return changed;
}

bool Compositor::ReadParam(const Layer &l, const std::string &param, float *v) {
    if (param == "alpha") { *v = l.alpha * 255.0f; return true; }
    if (param == "left" || param == "x") { *v = l.x; return true; }
    if (param == "top" || param == "y") { *v = l.y; return true; }
    if (param == "xscale") { *v = l.sx * 100.0f; return true; }
    if (param == "yscale") { *v = l.sy * 100.0f; return true; }
    if (param == "zoom") { *v = l.sx * 100.0f; return true; }
    if (param == "rotate") { *v = l.rotate; return true; }
    if (param == "w") { *v = l.w; return true; }
    if (param == "h") { *v = l.h; return true; }
    return false;
}

bool Compositor::ApplyParam(Layer &l, const std::string &param, float v) {
    if (param == "alpha") { l.alpha = std::clamp(v / 255.0f, 0.0f, 1.0f); return true; }
    if (param == "left" || param == "x") { l.x = v; l.own_pos = true; return true; }
    if (param == "top" || param == "y") { l.y = v; l.own_pos = true; return true; }
    if (param == "xscale") { l.sx = v / 100.0f; return true; }
    if (param == "yscale") { l.sy = v / 100.0f; return true; }
    if (param == "zoom") { l.sx = l.sy = v / 100.0f; return true; }
    if (param == "rotate") { l.rotate = v; return true; }
    if (param == "w") { l.w = v; return true; }
    if (param == "h") { l.h = v; return true; }
    return false;
}

float Compositor::ReadTweenValue(const std::string& id, const std::string& param,
                                 float fallback) const {
  for (const auto& layer : layers_)
    if (layer.id == id) { ReadParam(layer, param, &fallback); break; }
  return fallback;
}

bool Compositor::WriteTweenValue(const std::string& id, const std::string& param, float value) {
  for (auto& layer : layers_)
    if (layer.id == id) return ApplyParam(layer, param, value);
  return false;
}

void Compositor::AddTween(const std::string& id,
                          const std::map<std::string, std::string>& attrs, double now_ms) {
  if (std::none_of(layers_.begin(), layers_.end(),
                   [&](const Layer& layer) { return layer.id == id; })) {
    Layer layer; layer.id = id; layers_.push_back(std::move(layer));
  }
  tween_scheduler_.Add(id, attrs, now_ms,
    [this](const std::string& layer, const std::string& param, float fallback) {
      return ReadTweenValue(layer, param, fallback);
    }, [this](const std::string& layer, const std::string& param, float value) {
      return WriteTweenValue(layer, param, value);
    });
  ++revision_;
}

void Compositor::BeginTweenSet() { tween_scheduler_.Begin(); }
void Compositor::EndTweenSet(double now_ms) {
  tween_scheduler_.End(now_ms);
  ++revision_;
}
void Compositor::DeleteTweens(const std::string& id) {
  tween_scheduler_.Finish(id,
    [this](const std::string& layer, const std::string& param, float fallback) {
      return ReadTweenValue(layer, param, fallback);
    }, [this](const std::string& layer, const std::string& param, float value) {
      return WriteTweenValue(layer, param, value);
    });
  ++revision_;
}

void Compositor::SetAnimeFrame(const std::string &id, const std::string &mode,
                               const std::string &file, int time_ms, int loop,
                               const std::map<std::string, std::string> &props,
                               double now_ms) {
    if (id.empty()) return;
    ++revision_;
    AnimeState &st = anime_[id];
    if (mode == "init") {
        st = AnimeState();
        st.loop = loop;
        st.frames.push_back({static_cast<double>(time_ms), file, props});
        if (!file.empty()) LoadImage(id, file);
        if (!props.empty()) SetProps(id, props);
        st.active_file = file;
        st.active_index = 0;
    } else if (mode == "add") {
        st.frames.push_back({static_cast<double>(time_ms), file, props});
    } else if (mode == "end") {
        std::stable_sort(st.frames.begin(), st.frames.end(),
                         [](const AnimeFrame &a, const AnimeFrame &b) {
                             return a.time_ms < b.time_ms;
                         });
        st.total_ms = time_ms > 0 ? time_ms
                     : (st.frames.empty() ? 0 : st.frames.back().time_ms);
        st.start_ms = now_ms;
        st.active_index = -1;
        st.active_file.clear();
        bool changed = false;
        AdvanceAnime(now_ms, &changed);
    }
}

void Compositor::AdvanceAnime(double now_ms, bool *changed) {
    std::vector<std::string> gone;
    for (auto &entry : anime_) {
        AnimeState &st = entry.second;
        if (st.frames.empty() || st.total_ms <= 0) continue;
        bool exists = false;
        for (const auto &l : layers_) if (l.id == entry.first) { exists = true; break; }
        if (!exists) { gone.push_back(entry.first); continue; }
        const double elapsed = now_ms - st.start_ms;
        double t;
        if (st.loop < 0) {
            t = elapsed > 0 ? std::fmod(elapsed, st.total_ms) : 0;
        } else {
            const double rounds = st.loop == 0 ? 1 : static_cast<double>(st.loop);
            if (elapsed >= st.total_ms * rounds) t = st.total_ms; // hold last
            else t = elapsed > 0 ? std::fmod(elapsed, st.total_ms) : 0;
        }
        int index = 0;
        for (size_t i = 0; i < st.frames.size(); ++i)
            if (st.frames[i].time_ms <= t) index = static_cast<int>(i);
        if (index == st.active_index) continue;
        const AnimeFrame &f = st.frames[index];
        if (!f.file.empty() && f.file != st.active_file) {
            LoadImage(entry.first, f.file);
            st.active_file = f.file;
        }
        if (!f.props.empty()) SetProps(entry.first, f.props);
        st.active_index = index;
        if (changed) *changed = true;
    }
    for (const auto &id : gone) anime_.erase(id);
}

bool Compositor::SceneRenderer::Activity::Advance(double now_ms) {
    if (phase == Phase::Idle || phase == Phase::Done) return phase == Phase::Done;
    if (phase == Phase::Armed) {
        start_ms = now_ms;
        end_ms = now_ms + duration_ms;
        phase = Phase::Running;
    }
    if (now_ms <= start_ms) { progress = from; return false; }
    if (now_ms < end_ms) {
        progress = int(from + double(to - from) * (now_ms - start_ms) / duration_ms);
        return false;
    }
    progress = to;
    phase = Phase::Done;
    return true;
}

double Compositor::SceneRenderer::Remaining(double now_ms) const {
    return Active() ? std::max(0.0, activity_.end_ms - now_ms) : 0;
}

bool Compositor::SceneRenderer::Update(double now_ms) {
    if (!Active()) return false;
    if (activity_.Advance(now_ms)) {
        // Completion releases the overlay only after its final integer setter.
        // There is no VM callback binding in the compat transition contract.
        Cancel();
    }
    return true;
}

bool Compositor::Update(double now_ms) {
    // Use the incoming now_ms rather than the stale now_ms_ to avoid incorrect first-frame timing.
    bool changed = PendingTextMs(now_ms) > 0;
    now_ms_ = now_ms;
    changed = tween_scheduler_.Update(now_ms,
      [this](const std::string& layer, const std::string& param, float fallback) {
      return ReadTweenValue(layer, param, fallback);
    }, [this](const std::string& layer, const std::string& param, float value) {
      return WriteTweenValue(layer, param, value);
    }) || changed;
    changed = scene_renderer_.Update(now_ms) || changed;
    AdvanceAnime(now_ms, &changed);
    if (changed) ++revision_;
    return changed;
}

double Compositor::PendingAnimationMs(double now_ms) const {
    double remain = tween_scheduler_.Remaining(now_ms);
    remain = std::max(remain, scene_renderer_.Remaining(now_ms));
    return remain < 0 ? 0 : remain;
}

std::string Compositor::DescribeDrawList(size_t max_layers) const {
    std::vector<const Layer *> sorted;
    for (const auto &l : layers_) sorted.push_back(&l);
    std::stable_sort(sorted.begin(), sorted.end(), DrawOrderLess);
    std::string out = "layers=" + std::to_string(layers_.size()) +
                      " tweens=" + std::to_string(tween_scheduler_.Count()) +
                      " trans=" + std::to_string(TransitionActive() ? 1 : 0) + " |";
    size_t n = 0;
    for (const Layer *l : sorted) {
        float ex, ey, ew, eh, ea; bool ev;
        EffectiveRect(*l, &ex, &ey, &ew, &eh, &ea, &ev);
        if (!ev || !l->texture) continue;
        if (n++ >= max_layers) { out += " ..."; break; }
        char buf[160];
        std::snprintf(buf, sizeof(buf), " %s[%s](%d,%d %dx%d a=%.2f)", l->id.c_str(),
                      KindName(KindOf(*l)), (int)ex, (int)ey, (int)ew, (int)eh, ea);
        out += buf;
    }
    return out;
}

std::vector<std::string> Compositor::HitLayers(float x, float y) const {
    std::vector<const Layer *> hits;
    for (const auto &l : layers_) {
        if (!ContainsPoint(l,x,y)) continue;
        hits.push_back(&l);
    }
    // topmost first: higher z (ZCmp) then deeper id wins
    std::stable_sort(hits.begin(), hits.end(), [](const Layer *a, const Layer *b) {
        const int c = LayerZCmp(*a, *b);
        if (c != 0) return c > 0;
        return SectionCount(a->id) > SectionCount(b->id);
    });
    std::vector<std::string> ids;
    ids.reserve(hits.size());
    for (const Layer *l : hits) ids.push_back(l->id);
    return ids;
}

Compositor::LayerInfo Compositor::GetLayerInfo(const std::string &id) const {
    LayerInfo info;
    for (const auto &l : layers_) {
        if (l.id != id) continue;
        info.found = true;
        info.left = l.x;
        info.top = l.y;
        info.width = l.w;
        info.height = l.h;
        info.draggable = l.draggable;
        info.has_dragarea = l.has_dragarea;
        info.drag_l = l.drag_l;
        info.drag_t = l.drag_t;
        info.drag_r = l.drag_r;
        info.drag_b = l.drag_b;
        break;
    }
    return info;
}

void Compositor::DumpRects() {
    for (const Layer &l : layers_) {
        if (!l.visible || !l.texture) continue;
        if (!dumped_.insert(l.id).second) continue;
        float ex, ey, ea; bool ev;
        EffectiveRect(l, &ex, &ey, &ea, &ev);
        if (!ev) continue;
        Log(kLogInfo, "rect: " + l.id + " x=" + std::to_string(ex) +
                          " y=" + std::to_string(ey) + " w=" + std::to_string(l.w) +
                          " h=" + std::to_string(l.h) + " z=" + std::to_string(l.z) +
                          " a=" + std::to_string(ea));
    }
}

#if defined(ARTC_HAS_GLES)

namespace {
const char *kVs = R"(attribute vec2 a_pos;
attribute vec2 a_uv;
attribute float a_opacity;
uniform vec2 u_screen;
uniform float u_top_down;
varying vec2 v_uv;
varying float v_opacity;
void main() {
    vec2 clip = vec2(a_pos.x / u_screen.x * 2.0 - 1.0,
                     1.0 - a_pos.y / u_screen.y * 2.0);
    clip.y=mix(clip.y,-clip.y,u_top_down);
    gl_Position = vec4(clip, 0.0, 1.0);
    v_uv = a_uv;
    v_opacity = a_opacity;
})";

const char *kFs = R"(precision mediump float;
varying vec2 v_uv;
varying float v_opacity;
uniform sampler2D u_tex;
uniform float u_alpha;
void main() {
    vec4 c = texture2D(u_tex, v_uv);
    gl_FragColor = vec4(c.rgb, c.a * u_alpha * v_opacity);
})";

// KrKr2-Next: [trans] overlay — the previous frame fades out over the new
// layer state. Plain fade uses u_t; rule transitions threshold the rule
// image (dark pixels change first) with a `vague` soft edge.
const char *kTransFs = R"(precision mediump float;
varying vec2 v_uv;
uniform sampler2D u_tex;
uniform sampler2D u_rule;
uniform float u_t;
uniform float u_vague;
uniform int u_use_rule;
void main() {
    vec4 c = texture2D(u_tex, v_uv);
    float a;
    if (u_use_rule == 1) {
        float r = texture2D(u_rule, vec2(v_uv.x, 1.0 - v_uv.y)).r;
        float edge = mix(-u_vague, 1.0 + u_vague, u_t);
        a = smoothstep(edge - u_vague, edge + u_vague, r);
    } else {
        a = 1.0 - u_t;
    }
    gl_FragColor = vec4(c.rgb, c.a * a);
})";

// Artemis layer ids sort by their leading integer ("600.4.0" → 600,
// "1.80.mw" → 1, "-273" → -273); non-numeric ids keep insertion order.
uint32_t CompileShader(uint32_t type, const char *src) {
    const std::string code = ShaderSourceForBackend(src);
    const char *text = code.c_str();
    uint32_t s = glCreateShader(type);
    glShaderSource(s, 1, &text, nullptr);
    glCompileShader(s);
    return s;
}
} // namespace

bool Compositor::InitGl() { return scene_renderer_.InitGl(); }

bool Compositor::SceneRenderer::InitGl() {
    if (ready_) return true;
    auto link = [](const char* fragment) -> uint32_t {
        const uint32_t vertex = CompileShader(GL_VERTEX_SHADER, kVs);
        const uint32_t pixel = CompileShader(GL_FRAGMENT_SHADER, fragment);
        const uint32_t program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, pixel);
        glBindAttribLocation(program, 0, "a_pos");
        glBindAttribLocation(program, 1, "a_uv");
        glBindAttribLocation(program, 2, "a_opacity");
        glLinkProgram(program);
        glDeleteShader(vertex);
        glDeleteShader(pixel);
        GLint linked = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (linked != GL_TRUE) { glDeleteProgram(program); return 0; }
        return program;
    };
    brush_.program = link(kFs);
    transition_brush_.program = link(kTransFs);
    if (!brush_.program || !transition_brush_.program) {
        ReleaseGl();
        Log(kLogError, "compositor: scene program link failed");
        return false;
    }
    auto& b = brush_;
    b.a_pos = glGetAttribLocation(b.program, "a_pos");
    b.a_uv = glGetAttribLocation(b.program, "a_uv");
    b.a_opacity = glGetAttribLocation(b.program, "a_opacity");
    b.u_screen = glGetUniformLocation(b.program, "u_screen");
    b.u_top_down = glGetUniformLocation(b.program, "u_top_down");
    b.u_tex = glGetUniformLocation(b.program, "u_tex");
    b.u_alpha = glGetUniformLocation(b.program, "u_alpha");
    auto& t = transition_brush_;
    t.a_pos = glGetAttribLocation(t.program, "a_pos");
    t.a_uv = glGetAttribLocation(t.program, "a_uv");
    t.u_screen = glGetUniformLocation(t.program, "u_screen");
    t.u_tex = glGetUniformLocation(t.program, "u_tex");
    t.u_rule = glGetUniformLocation(t.program, "u_rule");
    t.u_t = glGetUniformLocation(t.program, "u_t");
    t.u_vague = glGetUniformLocation(t.program, "u_vague");
    t.u_use_rule = glGetUniformLocation(t.program, "u_use_rule");
    ready_ = true;
    return true;
}

void Compositor::SceneRenderer::Cancel() {
    // Cancellation destroys resources without executing completion.
    activity_ = Activity{};
    if (captured_texture_) glDeleteTextures(1, &captured_texture_);
    if (rule_texture_) glDeleteTextures(1, &rule_texture_);
    captured_texture_ = rule_texture_ = 0;
    vague_ = 0;
}

void Compositor::SceneRenderer::ReleaseGl() {
    Cancel();
    if (scene_fbo_) glDeleteFramebuffers(1, &scene_fbo_);
    if (scene_texture_) glDeleteTextures(1, &scene_texture_);
    if (brush_.program) glDeleteProgram(brush_.program);
    if (transition_brush_.program) glDeleteProgram(transition_brush_.program);
    brush_ = Brush{};
    transition_brush_ = TransitionBrush{};
    scene_fbo_ = scene_texture_ = 0;
    width_ = height_ = 0;
    retained_ = ready_ = false;
    items_.clear();
    textures_.clear();
}

bool Compositor::SceneRenderer::AllocateScene(int width, int height) {
    if (width == width_ && height == height_ && scene_fbo_) return true;
    retained_ = false;
    Cancel();
    if (scene_fbo_) glDeleteFramebuffers(1, &scene_fbo_);
    if (scene_texture_) glDeleteTextures(1, &scene_texture_);
    scene_fbo_ = scene_texture_ = 0;
    width_ = height_ = 0;
    if (width <= 0 || height <= 0) return false;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &scene_texture_);
    glBindTexture(GL_TEXTURE_2D, scene_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glGenFramebuffers(1, &scene_fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_texture_, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &scene_fbo_);
        glDeleteTextures(1, &scene_texture_);
        scene_fbo_ = scene_texture_ = 0;
        Log(kLogError, "compositor: scene framebuffer incomplete");
        return false;
    }
    width_ = width;
    height_ = height;
    return true;
}

bool Compositor::SceneRenderer::CaptureFrame() {
    if (!ready_ || !retained_) return false;
    GLint target = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &target);
    glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo_);
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &captured_texture_);
    glBindTexture(GL_TEXTURE_2D, captured_texture_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGetError();
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width_, height_, 0);
    const bool copied = glGetError() == GL_NO_ERROR;
    glBindFramebuffer(GL_FRAMEBUFFER, target);
    if (!copied) {
        glDeleteTextures(1, &captured_texture_);
        captured_texture_ = 0;
    }
    return copied;
}

bool Compositor::BeginTransition(double now_ms, int time_ms, const std::vector<uint8_t>& rule,
                                 int rule_w, int rule_h, int vague) {
    const bool was_active = scene_renderer_.Active();
    const bool begun = scene_renderer_.BeginTransition(*this, now_ms, time_ms, rule, rule_w, rule_h, vague);
    if (was_active || begun) ++revision_;
    return begun;
}

bool Compositor::SceneRenderer::BeginTransition(Compositor& compositor, double now_ms, int time_ms,
                                               const std::vector<uint8_t>& rule, int rule_w, int rule_h, int vague) {
    Cancel();
    if (time_ms <= 0 || !std::isfinite(now_ms) || !CaptureFrame()) return false;
    if (rule_w > 0 && rule_h > 0 && uint64_t(rule_w) * rule_h <= rule.size()) {
        const size_t count = size_t(rule_w) * rule_h;
        if (count <= std::numeric_limits<size_t>::max() / 4) {
            std::vector<uint8_t> pixels(count * 4, 255);
            for (size_t i = 0; i < count; ++i)
                std::fill_n(pixels.data() + i * 4, 3, rule[i]);
            glGetError();
            rule_texture_ = compositor.CreateTexture(pixels.data(), rule_w, rule_h);
            if (glGetError() != GL_NO_ERROR) {
                if (rule_texture_) glDeleteTextures(1, &rule_texture_);
                rule_texture_ = 0;
            }
        }
    }
    const int softness = std::clamp(vague, 0, 255);
    vague_ = softness / 255.f;
    activity_.from = -softness;
    activity_.to = 255 + softness;
    activity_.duration_ms = time_ms;
    activity_.phase = Activity::Phase::Armed;
    // The first update belongs here to retain Lua's existing wait deadline.
    activity_.Advance(now_ms);
    return true;
}

void Compositor::SceneRenderer::Overlay() {
    if (!Active() || !captured_texture_) return;
    const auto& b = transition_brush_;
    glUseProgram(b.program);
    glUniform2f(b.u_screen, float(width_), float(height_));
    glUniform1i(b.u_tex, 0);
    glUniform1i(b.u_rule, 1);
    glUniform1f(b.u_t, float(activity_.progress - activity_.from) / (activity_.to - activity_.from));
    glUniform1f(b.u_vague, std::max(vague_, 0.002f));
    glUniform1i(b.u_use_rule, rule_texture_ != 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, rule_texture_ ? rule_texture_ : captured_texture_);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, captured_texture_);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    const float vertices[] = {0,0,0,1, float(width_),0,1,1,
                              0,float(height_),0,0, float(width_),float(height_),1,0};
    glVertexAttribPointer(b.a_pos, 2, GL_FLOAT, GL_FALSE, 16, vertices);
    glVertexAttribPointer(b.a_uv, 2, GL_FLOAT, GL_FALSE, 16, vertices + 2);
    glEnableVertexAttribArray(b.a_pos);
    glEnableVertexAttribArray(b.a_uv);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_BLEND);
}

uint32_t Compositor::CreateTexture(const uint8_t *pixels, int w, int h) {
    uint32_t tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    return tex;
}

// Glyph cache GL side. A page uploads once on first use and re-uploads only
// when its generation advanced (new cells were placed since); repeat text
// (typewriter re-sets, page flips, repeated UI strings) costs no upload.
uint32_t Compositor::GlyphPageTexture(size_t page) {
    const auto &pages = glyph_atlas_.Pages();
    if (page >= pages.size()) return 0;
    const auto &p = pages[page];
    uint32_t &tex = glyph_page_textures_[page];
    uint64_t &uploaded = glyph_page_uploaded_[page];
    if (tex && uploaded == p.generation) return tex;
    if (!tex) {
        tex = CreateTexture(p.pixels.data(), GlyphAtlas::Page::kWidth,
                            GlyphAtlas::Page::kHeight);
    } else {
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GlyphAtlas::Page::kWidth,
                        GlyphAtlas::Page::kHeight, GL_RGBA, GL_UNSIGNED_BYTE,
                        p.pixels.data());
    }
    uploaded = p.generation;
    return tex;
}

void Compositor::DropGlyphGl() {
    for (auto &kv : glyph_page_textures_)
        if (kv.second) glDeleteTextures(1, &kv.second);
    glyph_page_textures_.clear();
    glyph_page_uploaded_.clear();
    glyph_atlas_.Invalidate();
}

bool Compositor::LoadImage(const std::string &id, const std::string &file) {
    ++revision_;
    if (!scene_renderer_.Ready() || !packs_) return false;
    // Scripts usually omit extensions (:bg/black); the real engine matches
    // packed assets by trying the common image extensions.
    static const char *kExtensions[] = {"", ".png", ".jpg", ".jpeg"};
    std::vector<uint8_t> bytes;
    std::string resolved;
    for (const char *ext : kExtensions) {
        resolved = file + ext;
        if (packs_->Read(resolved, bytes) || ReadSaveFile(SavePath(save_directory_,resolved),bytes)) break;
    }
    if (bytes.empty()) {
        Log(kLogWarn, "lyc: image not found: " + file);
        return false;
    }
    int w, h, channels;
    uint8_t *pixels = stbi_load_from_memory(bytes.data(), (int)bytes.size(), &w, &h, &channels, 4);
    if (!pixels) {
        Log(kLogWarn, "lyc: decode failed: " + file);
        return false;
    }
    uint32_t tex = CreateTexture(pixels, w, h);
    stbi_image_free(pixels);

    // replace or create layer
    for (auto &l : layers_) {
        if (l.id == id) {
            if (l.texture) glDeleteTextures(1, &l.texture);
            l.texture = tex;
            l.tex_w = w; l.tex_h = h;
            l.content_x = l.content_y = 0;
            l.glyphs.clear(); l.text.clear();
            // A replacement is a new image surface. Face differences often
            // have different bounding boxes; retaining the previous surface's
            // size stretches the new eyes/mouth away from their scripted origin.
            // Keep the layer transform, but reset image-local geometry. A lyprop
            // following lyc can apply a new sprite-sheet crop/display size.
            l.w = (float)w; l.h = (float)h;
            l.u0 = l.v0 = 0; l.u1 = l.v1 = 1;
            Log(kLogDebug, "lyc: replaced " + id);
            return true;
        }
    }
    Layer l;
    l.id = id;
    l.texture = tex;
    l.tex_w = w; l.tex_h = h;
    l.w = (float)w; l.h = (float)h;
    l.z = 0;
    layers_.push_back(l);
    Log(kLogDebug, "lyc: added " + id + " " + std::to_string(w) + "x" + std::to_string(h));
    return true;
}

void Compositor::LoadMask(const std::string& file) {
    if(file.empty() || masks_.count(file) || !scene_renderer_.Ready() || !packs_)return;
    auto& mask=masks_[file];std::vector<uint8_t> bytes;
    for(const char* ext:{"",".png",".jpg",".jpeg"})if(packs_->Read(file+ext,bytes))break;
    int w=0,h=0,channels=0;
    if(bytes.empty() || bytes.size()>64*1024*1024 ||
       !stbi_info_from_memory(bytes.data(),int(bytes.size()),&w,&h,&channels) ||
       w<1 || h<1 || w>8192 || h>8192 || uint64_t(w)*h>16777216) {
        Log(kLogWarn,"intermediate mask unavailable: "+file);return;
    }
    auto* pixels=stbi_load_from_memory(bytes.data(),int(bytes.size()),&w,&h,&channels,4);
    if(!pixels){Log(kLogWarn,"intermediate mask decode failed: "+file);return;}
    mask={CreateTexture(pixels,w,h),w,h};stbi_image_free(pixels);
}

bool Compositor::LoadFont(const std::string &file) {
    if (!scene_renderer_.Ready() || !packs_) return false;
    if (font_ready_ && file == font_path_) return true;
    std::vector<uint8_t> data;
    if (!packs_->Read(file, data)) {
        Log(kLogWarn, "font not found in packs: " + file);
        return false;
    }
    auto *info = new stbtt_fontinfo;
    const int offset = stbtt_GetFontOffsetForIndex(data.data(), 0);
    if (offset < 0 || !stbtt_InitFont(info, data.data(), offset)) {
        Log(kLogError, "font init failed: " + file);
        delete info;
        return false;
    }
    delete static_cast<stbtt_fontinfo *>(font_info_);
    font_data_ = std::move(data);
    font_path_ = file;
    font_info_ = info;
    font_ready_ = true;
    // Glyph indices belong to the previous face — cached cells are stale.
    DropGlyphGl();
    Log(kLogInfo, "font loaded: " + file + " (" + std::to_string(font_data_.size()) + " B)");
    return true;
}

bool Compositor::SetPixels(const std::string& id, const uint8_t* rgba, int width, int height) {
    if(!rgba || width<=0 || height<=0 || width>8192 || height>8192) return false;
    SetProps(id,{});
    for(auto& l:layers_) if(l.id==id) {
        if(!l.texture || l.w==l.tex_w) l.w=width;
        if(!l.texture || l.h==l.tex_h) l.h=height;
        if(l.texture && l.tex_w==width && l.tex_h==height) {
            glBindTexture(GL_TEXTURE_2D,l.texture);
            glTexSubImage2D(GL_TEXTURE_2D,0,0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,rgba);
        } else {
            if(l.texture) glDeleteTextures(1,&l.texture);
            l.texture=CreateTexture(rgba,width,height);
        }
        l.tex_w=width; l.tex_h=height;
        l.glyphs.clear(); l.text.clear(); l.content_x=l.content_y=0;
        l.u0=l.v0=0; l.u1=l.v1=1;
        return l.texture!=0;
    }
    return false;
}

bool Compositor::SetText(const std::string &id, const std::string &text,
                         float size, uint32_t color, float wrapWidth,
                         const std::map<std::string, std::string>& style,
                         const std::vector<TextRuby>& ruby) {
    ++revision_;
    if (text.empty()) {
        for (auto& l : layers_) if (l.id == id) {
            l.glyphs.clear(); l.text.clear();
            l.text_pen_x = l.text_pen_y = 0;
            if (l.texture) glDeleteTextures(1, &l.texture);
            l.texture = 0;
            l.tex_w = l.tex_h = 0;
            l.w = l.h = 0;
        }
        return true;
    }
    if (!font_ready_) return false;
    auto *info = static_cast<stbtt_fontinfo *>(font_info_);
    const auto number = [&](const char* name, float fallback) {
        const auto it = style.find(name);
        return it == style.end() ? fallback : std::atof(it->second.c_str());
    };
    size = std::clamp(size, 1.0f, 256.0f);
    const float scale = stbtt_ScaleForMappingEmToPixels(info, size);
    const int outline = std::clamp(static_cast<int>(number("outline", 0)), 0, 8);
    const float tracking = number("kerning", 0);
    const auto alignment = style.find("align");
    const std::string align = alignment == style.end() ? "left" : alignment->second;
    const auto stroke = style.find("outlinecolor");
    const uint32_t outline_color = stroke == style.end() ? 0 :
        static_cast<uint32_t>(strtoul(stroke->second.c_str(), nullptr, 16));
    int ascent = 0, descent = 0, linegap = 0;
    stbtt_GetFontVMetrics(info, &ascent, &descent, &linegap);
    // CJK fonts often keep a much taller hhea clipping box than their
    // typographic em box. Using it as a baseline pushes centered button
    // captions below their scripted top and inflates every line's spacing.
    stbtt_GetFontVMetricsOS2(info, &ascent, &descent, &linegap);
    const float sascent = ascent * scale;
    const float sdescent = descent * scale;
    // spacemiddle separates the reserved ruby row from the main glyph row.
    // It is often negative; omitting rubysize makes adjacent lines overlap.
    const float ruby_row = std::max(0.0, number("rubysize", ruby.empty() ? 0 : size/2));
    const float row_top = number("spacetop", 0) + ruby_row + number("spacemiddle", 0);
    const int baseline = static_cast<int>(std::ceil(sascent + row_top)) + outline;
    const float advance_h = sascent - sdescent + linegap * scale +
        row_top + number("spacebottom", 0);

    // UTF-8 → codepoints → glyphs, then two-pass word layout:
    //   pass 1 measures every glyph's advance;
    //   pass 2 places them into lines, wrapping whenever pen_x would exceed
    //   wrapWidth (>0). Line width = max over the line; total height = #lines.
    const auto decode = [](const std::string& text) {
    std::vector<uint32_t> cps;
    for (size_t i = 0; i < text.size();) {
        const unsigned char c = text[i];
        uint32_t cp = c; int len = 1;
        if ((c & 0xE0) == 0xC0 && i + 1 < text.size()) { cp = c & 0x1F; len = 2; cp = (cp << 6) | (text[i + 1] & 0x3F); }
        else if ((c & 0xF0) == 0xE0 && i + 2 < text.size()) { cp = c & 0x0F; len = 3; cp = (cp << 6) | (text[i + 1] & 0x3F); cp = (cp << 6) | (text[i + 2] & 0x3F); }
        else if ((c & 0xF8) == 0xF0 && i + 3 < text.size()) { cp = c & 0x07; len = 4; cp = (cp << 6) | (text[i + 1] & 0x3F); cp = (cp << 6) | (text[i + 2] & 0x3F); cp = (cp << 6) | (text[i + 3] & 0x3F); }
        i += len; cps.push_back(cp);
    }
    return cps;
    };
    auto cps=decode(text);
    const size_t base_count=cps.size();
    std::vector<int> glyphs;
    std::vector<int> advances;
    glyphs.reserve(cps.size()); advances.reserve(cps.size());
    for (uint32_t cp : cps) {
        const int gi = stbtt_FindGlyphIndex(info, static_cast<int>(cp));
        int adv = 0, lsb = 0;
        stbtt_GetGlyphHMetrics(info, gi, &adv, &lsb);
        glyphs.push_back(gi);
        advances.push_back(static_cast<int>(std::lround(adv * scale + tracking)));
    }
    if (glyphs.empty()) return false;
    struct RubyGroup { size_t first, last, glyph_first, glyph_last; int base_width, width; };
    std::vector<RubyGroup> ruby_groups;
    const float ruby_size=std::clamp(number("rubysize",size/2),1.0,256.0);
    const float ruby_scale=stbtt_ScaleForMappingEmToPixels(info,ruby_size);
    for(const auto& r:ruby) {
        if(r.start>text.size() || r.length>text.size()-r.start || r.text.empty()) continue;
        const size_t first=decode(text.substr(0,r.start)).size();
        const size_t last=first+decode(text.substr(r.start,r.length)).size();
        if(first>=last || last>base_count) continue;
        if(!ruby_groups.empty() && first<ruby_groups.back().last) continue;
        if(std::find(cps.begin()+first,cps.begin()+last,'\n')!=cps.begin()+last) continue;
        int base_width=0, ruby_width=0;
        for(size_t k=first;k<last;++k) base_width+=advances[k];
        const size_t glyph_first=glyphs.size();
        for(uint32_t cp:decode(r.text)) {
            int gi=stbtt_FindGlyphIndex(info,static_cast<int>(cp)),adv=0,lsb=0;
            stbtt_GetGlyphHMetrics(info,gi,&adv,&lsb);
            cps.push_back(cp); glyphs.push_back(gi);
            advances.push_back(static_cast<int>(std::lround(adv*ruby_scale)));
            ruby_width+=advances.back();
        }
        const int width=std::max(base_width,ruby_width);
        // Ruby and its base occupy one unbreakable block. A longer reading
        // reserves room in the line, so it cannot collide with adjacent text.
        ruby_groups.push_back({first,last,glyph_first,glyphs.size(),base_width,width});
    }

    // layout: (line, pen_x_in_line)
    std::vector<int> lx(glyphs.size()), ly(glyphs.size());
    std::vector<int> line_w(1, 0);
    struct Unit { size_t first,last; int width; const RubyGroup* ruby; };
    std::vector<Unit> units;
    for(size_t k=0;k<base_count;) {
        if(cps[k]=='\r') { lx[k++]=-1;continue; }
        const RubyGroup* group=nullptr;
        for(const auto& r:ruby_groups) if(r.first==k) {group=&r;break;}
        const size_t end=group ? group->last : k+1;
        units.push_back({k,end,group ? group->width : advances[k],group});k=end;
    }
    const bool prohibit=number("prohibit",0)!=0, hung=number("hung",0)!=0;
    // [indent] hanging indent: an opening pair character shifts following
    // (wrapped or explicit) lines to its right edge until the matching close.
    const std::vector<uint32_t> indent_pairs=DecodeUtf8Codepoints(indent_pair_);
    auto indent_open=[&](uint32_t cp,uint32_t* close)->bool {
        if(indent_pairs.size()<2)return false;
        for(size_t i=0;i+1<indent_pairs.size();i+=2)
            if(indent_pairs[i]==cp){*close=indent_pairs[i+1];return true;}
        return false;
    };
    int pen = 0, line = 0, indent_x = 0;
    int chars_in_line = 0;
    std::vector<std::pair<uint32_t,int>> indent_stack; // {expected close, prior indent}
    for(size_t u=0;u<units.size();) {
        const size_t k=units[u].first;
        if(cps[k]=='\n') {lx[k]=-1;pen=indent_x;++line;line_w.push_back(0);chars_in_line=0;++u;continue;}
        size_t end=u+1;int width=units[u].width;
        // Keep an opening bracket with its following text, and a closing
        // mark with the preceding text. Ruby remains one indivisible unit.
        while(prohibit && end<units.size() && cps[units[end].first]!='\n' &&
              (IsProhibitFoot(cps[units[end-1].last-1]) ||
               IsWordpart(cps[units[end-1].last-1]) && IsWordpart(cps[units[end].first]) ||
               (IsProhibitHead(cps[units[end].first]) && !(hung && HangPunctuation(cps[units[end].first]))))) {
            width+=units[end].width;++end;
        }
        if(pen>0 && wrapWidth>0 && pen+width>wrapWidth && !(hung && HangPunctuation(cps[k]))) {
            pen=indent_x;++line;line_w.push_back(0);chars_in_line=0;
        }
        for(;u<end;++u) {
            const auto& unit=units[u];
            if(const auto* group=unit.ruby) {
              int base_pen=pen+(group->width-group->base_width)/2;
              for(size_t j=group->first;j<group->last;++j) {
                  lx[j]=base_pen; ly[j]=line; base_pen+=advances[j];
              }
              int reading_width=0;
              for(size_t j=group->glyph_first;j<group->glyph_last;++j) reading_width+=advances[j];
              int ruby_pen=pen+(group->width-reading_width)/2;
              for(size_t j=group->glyph_first;j<group->glyph_last;++j) {
                  lx[j]=ruby_pen; ly[j]=line; ruby_pen+=advances[j];
              }
              chars_in_line+=static_cast<int>(group->last-group->first);
            } else {
              lx[unit.first]=pen;ly[unit.first]=line;
              const uint32_t base_cp=cps[unit.first];
              uint32_t close=0;
              if(indent_open(base_cp,&close)) {
                  const bool within=indent_range_<0 || chars_in_line<indent_range_;
                  if(within && (indent_stack.empty() || indent_nest_)) {
                      indent_stack.push_back({close,indent_x});
                      indent_x=pen+advances[unit.first];
                  }
              } else if(!indent_stack.empty() && indent_stack.back().first==base_cp) {
                  indent_x=indent_stack.back().second;
                  indent_stack.pop_back();
              }
              ++chars_in_line;
            }
            pen+=unit.width;line_w[line]=std::max(line_w[line],pen);
        }
    }
    const int n_lines = static_cast<int>(line_w.size());
    int tex_w = 0;
    for (int w : line_w) tex_w = w > tex_w ? w : tex_w;
    if (wrapWidth > 0) tex_w = std::max(tex_w, static_cast<int>(std::ceil(wrapWidth)));
    tex_w += outline * 2;
    if (tex_w <= 0 || tex_w > 4096) {
        Log(kLogWarn, "SetText: degenerate width " + std::to_string(tex_w));
        return false;
    }
    const int line_h = std::max(1, static_cast<int>(std::ceil(advance_h)));
    int bottom = line_h * n_lines + outline * 2;
    for (size_t k = 0; k < glyphs.size(); ++k) {
        if (lx[k] < 0) continue;
        int x0, y0, x1, y1;
        const float gs=k<base_count ? scale : ruby_scale;
        stbtt_GetGlyphBitmapBox(info, glyphs[k], gs, gs, &x0, &y0, &x1, &y1);
        bottom = std::max(bottom, baseline + ly[k] * line_h + y1 + outline);
    }
    const int tex_h = bottom;
    if (tex_h <= 0 || tex_h > 4096) return false;

    // [glyph homing=1] — where the next character would go: the end of the
    // final line, including that line's alignment shift (matching the glyph
    // placement below). A trailing newline parks the pen on the fresh line.
    const int end_free = tex_w - 2 * outline - line_w[line];
    const float pen_x = static_cast<float>(pen) +
        (align == "center" ? end_free / 2.0f
                           : (align == "right" ? static_cast<float>(end_free) : 0.0f));
    const float pen_y = static_cast<float>(line * line_h);

    // Each glyph occupies its own padded atlas cell. Overlapping outlines
    // and kerning must not reveal neighbouring letters during a character
    // tween. The complete line layout stays fixed throughout the animation.
    // Cells come from the persistent glyph cache: repeat text skips stbtt
    // rasterization, CPU compositing and the GPU upload entirely.
    std::vector<TextGlyph> positioned;
    std::vector<const GlyphAtlas::Entry *> cells;
    for (int attempt = 0;; ++attempt) {
        bool evicted = false;
        positioned.clear();
        cells.clear();
        for (size_t k = 0; k < glyphs.size(); ++k) {
            if (lx[k] < 0) continue;
            const float gs = k < base_count ? scale : ruby_scale;
            const auto *cell = glyph_atlas_.Lookup(
                info, {glyphs[k], gs, outline, color, outline_color});
            if (!cell) return false; // degenerate cell (legacy failure)
            if (glyph_atlas_.WasReset()) { evicted = true; break; }
            const int free_width=tex_w-2*outline-line_w[ly[k]];
            const int shift=align=="center" ? free_width/2 : (align=="right" ? free_width : 0);
            TextGlyph g;
            g.order=k;
            for(const auto& r:ruby_groups) if(k>=r.glyph_first && k<r.glyph_last) g.order=r.first;
            const float glyph_baseline=k<base_count ? baseline : outline+number("spacetop",0)+ascent*ruby_scale;
            g.x=lx[k]+cell->xoff+shift;
            g.y=glyph_baseline+ly[k]*line_h+cell->yoff-outline;
            g.w=cell->w; g.h=cell->h;
            g.u0=cell->u0; g.v0=cell->v0; g.u1=cell->u1; g.v1=cell->v1;
            positioned.push_back(g);
            cells.push_back(cell);
        }
        if (!evicted) break;
        // Watermark eviction: cells placed earlier in this pass and the other
        // layers' glyphs reference evicted pages — drop them and retry on the
        // fresh cache. A single text exceeding the page budget alone fails.
        if (attempt >= 1) return false;
        for (auto &l : layers_) { l.glyphs.clear(); l.text.clear(); }
        DropGlyphGl();
    }
    // Ensure every referenced page has an up-to-date GPU texture.
    for (size_t i = 0; i < positioned.size(); ++i)
        positioned[i].tex = GlyphPageTexture(cells[i]->page);

    // upsert layer; message-layer default position = bottom-left. Text
    // layers reference the shared atlas pages, not a per-SetText texture.
    for (auto &l : layers_) {
        if (l.id == id) {
            if (l.texture) glDeleteTextures(1, &l.texture);
            l.texture = 0;
            l.tex_w = tex_w; l.tex_h = tex_h;
            SetGlyphTimes(l, positioned, text);
            // The placeholder layer may carry a degenerate (0-sized) rect
            // from its ghost creation — the new raster defines the display
            // size, otherwise the quad collapses and nothing is drawn.
            l.w = (float)tex_w;
            l.h = (float)tex_h;
            l.content_x = number("left", 0);
            l.content_y = number("top", 0);
            l.text_pen_x = pen_x;
            l.text_pen_y = pen_y;
            Log(kLogInfo, "SetText: replaced " + id + " " +
                              std::to_string(tex_w) + "x" + std::to_string(tex_h) +
                              " (glyphs " + std::to_string(l.glyphs.size()) + ")");
            return true;
        }
    }
    Layer l;
    l.id = id;
    l.texture = 0;
    l.tex_w = tex_w; l.tex_h = tex_h;
    SetGlyphTimes(l, positioned, text);
    l.w = (float)tex_w; l.h = (float)tex_h;
    l.content_x = number("left", 0);
    l.content_y = number("top", 0);
    l.text_pen_x = pen_x;
    l.text_pen_y = pen_y;
    l.z = 100; // above scene layers
    layers_.push_back(l);
    Log(kLogInfo, "SetText: " + id + " " + std::to_string(tex_w) + "x" +
                      std::to_string(tex_h) + " '" + text.substr(0, 24) + "'");
    return true;
}

void Compositor::SetProps(const std::string &id,
                          const std::map<std::string, std::string> &attrs) {
    ++revision_;
    if(auto mask=attrs.find("intermediate_render_mask");mask!=attrs.end())LoadMask(mask->second);
    for (auto &l : layers_) {
        if (l.id != id) continue;
        l.effect.Set(attrs);
        if (const auto zoom=attrs.find("zoom"); zoom!=attrs.end())
            l.sx=l.sy=ToF(zoom->second,100)/100.f;
        for (const auto &kv : attrs) {
            if (kv.first == "x" || kv.first == "left") { l.x = ToF(kv.second); l.own_pos = true; }
            else if (kv.first == "y" || kv.first == "top") { l.y = ToF(kv.second); l.own_pos = true; }
            else if (kv.first == "w") l.w = ToF(kv.second);
            else if (kv.first == "h") l.h = ToF(kv.second);
            else if (kv.first == "alpha") {
                const float a = ToF(kv.second);
                l.alpha = std::clamp(a / 255.0f, 0.0f, 1.0f);   // script uses 0-255
            }
            else if (kv.first == "rotate") l.rotate = ToF(kv.second);
            else if (kv.first == "reversex") l.reverse_x = ToF(kv.second)!=0;
            else if (kv.first == "reversey") l.reverse_y = ToF(kv.second)!=0;
            else if (kv.first == "anchorx") l.ax = ToF(kv.second);
            else if (kv.first == "xscale") { const float v = ToF(kv.second); l.sx = v / 100.0f; }
            else if (kv.first == "yscale") { const float v = ToF(kv.second); l.sy = v / 100.0f; }
            else if (kv.first == "ownpos") l.own_pos = true;
            else if (kv.first == "anchory") l.ay = ToF(kv.second);
            else if (kv.first == "visible") l.visible = (kv.second != "0");
            else if (kv.first == "z") l.z = ToI(kv.second);
            else if (kv.first == "draggable") l.draggable = (kv.second != "0");
            else if (kv.first == "dragarea") {
                // {left, top, right, bottom} — offsets in stage units
                if (std::sscanf(kv.second.c_str(), "%f,%f,%f,%f",
                                &l.drag_l, &l.drag_t, &l.drag_r, &l.drag_b) == 4) {
                    l.has_dragarea = true;
                }
            }
            else if (kv.first == "clip") {
                // "x,y,w,h" → normalized UV crop
                int cx = 0, cy = 0, cw = 0, chh = 0;
                if (std::sscanf(kv.second.c_str(), "%d,%d,%d,%d", &cx, &cy, &cw, &chh) == 4 &&
                    l.effect.intermediate==0 && l.tex_w > 0 && l.tex_h > 0 && cw > 0 && chh > 0) {
                    l.u0 = (float)cx / l.tex_w;      l.v0 = (float)cy / l.tex_h;
                    l.u1 = (float)(cx + cw) / l.tex_w; l.v1 = (float)(cy + chh) / l.tex_h;
                    l.w = (float)cw;   // clip defines the displayed sub-image size
                    l.h = (float)chh;
                }
            }
        }
        l.z = 0;   // leading id number is the authoritative z
        return;
    }
    // lyprop may target a pure group layer never created by lyc — materialize
    // it as a texture-less holder so children inherit its transform.
    Layer g;
    g.id = id;
    g.visible = true;
    g.z = 0;
    layers_.push_back(g);
    SetProps(id, attrs);
}

void Compositor::DeleteLayer(const std::string &id) {
    ++revision_;
    tween_scheduler_.Remove(id);
    // group delete: an id names the whole subtree (artemis groups layers as
    // "500.d.1.0", "500.z.*", ...; scripts delete the parent "500" only).
    const std::string prefix = id + ".";
    for (auto it = layers_.begin(); it != layers_.end();) {
        if (it->id == id ||
            it->id.compare(0, prefix.size(), prefix) == 0) {
            if (it->texture) glDeleteTextures(1, &it->texture);
            it = layers_.erase(it);
        } else {
            ++it;
        }
    }
}

bool Compositor::RenameLayer(const std::string &id, const std::string &to) {
    if (id.empty() || to.empty() || id == to) return false;
    ++revision_;
    bool any = false;
    const std::string prefix = id + ".";
    for (auto &l : layers_) {
        if (l.id == id) { l.id = to; any = true; }
        else if (l.id.compare(0, prefix.size(), prefix) == 0) {
            l.id = to + l.id.substr(id.size());
            any = true;
        }
    }
    tween_scheduler_.Rename(id, to);
    return any;
}

void Compositor::ReleaseGl() {
    shaders_.ReleaseGl();
    for(auto& mask:masks_)if(mask.second.texture)glDeleteTextures(1,&mask.second.texture);
    masks_.clear();
    DropGlyphGl();
    ++revision_;
    for (auto &l : layers_) {
        if (l.texture) glDeleteTextures(1, &l.texture);
    }
    layers_.clear();
    tween_scheduler_.Clear();
    ReleaseStageMaskGl();
    scene_renderer_.ReleaseGl();
}

void Compositor::Shutdown() {
    ReleaseGl();
    delete static_cast<stbtt_fontinfo *>(font_info_);
    font_info_ = nullptr;
    font_data_.clear();
    font_path_.clear();
    font_ready_ = false;
    present_cb_ = nullptr;
}

bool Compositor::Snapshot(SnapshotImage& output) const { return scene_renderer_.Snapshot(output); }

bool Compositor::SceneRenderer::Snapshot(SnapshotImage& output) const {
    if (!ready_ || !retained_ || uint64_t(width_) * height_ > 16777216) return false;
    SnapshotImage captured;
    captured.width = width_;
    captured.height = height_;
    captured.rgba.resize(size_t(width_) * height_ * 4);
    GLint target = 0, alignment = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &target);
    glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
    glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo_);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, captured.rgba.data());
    const GLenum error = glGetError();
    glPixelStorei(GL_PACK_ALIGNMENT, alignment);
    glBindFramebuffer(GL_FRAMEBUFFER, target);
    if (error != GL_NO_ERROR) return false;
    const size_t row = size_t(width_) * 4;
    for (int top = 0, bottom = height_ - 1; top < bottom; ++top, --bottom)
        std::swap_ranges(captured.rgba.begin() + top * row,
                         captured.rgba.begin() + (top + 1) * row,
                         captured.rgba.begin() + bottom * row);
    output = std::move(captured);
    return true;
}

void Compositor::Draw() { scene_renderer_.Draw(*this); }

void Compositor::SceneRenderer::TakeSnapshot(const Compositor& compositor) {
    // Freeze geometry, coverage and bindings before submitting any scene work.
    // Material texture names are borrowed only for this synchronous transaction.
    std::vector<const Layer*> order;
    order.reserve(compositor.layers_.size());
    textures_.clear();
    for (const Layer& layer : compositor.layers_) {
        order.push_back(&layer);
        if (layer.texture) textures_.emplace_back(layer.id, layer.texture);
    }
    std::stable_sort(order.begin(), order.end(), DrawOrderLess);
    items_.clear();
    items_.reserve(order.size());
    for (const Layer* source : order) {
        Item item;
        item.id = source->id;
        item.effect = source->effect;
        item.transform = compositor.EffectiveTransform(*source);
        const auto& matrix = item.transform;
        auto vertex = [&](std::vector<float>& data, float x, float y, float u, float v, float alpha) {
            const auto point = matrix.Point(x, y);
            data.insert(data.end(), {point.first, point.second, u, v, alpha});
        };
        auto rectangle = [&](std::vector<float>& data, float x, float y, float w, float h,
                             float u0, float v0, float u1, float v1, float alpha) {
            const int corners[] = {0, 1, 2, 2, 1, 3};
            for (int corner : corners)
                vertex(data, x + ((corner & 1) ? w : 0), y + ((corner & 2) ? h : 0),
                       (corner & 1) ? u1 : u0, (corner & 2) ? v1 : v0, alpha);
        };
        if (!source->glyphs.empty()) {
            for (const TextGlyph& glyph : source->glyphs) {
                float x = glyph.x, y = glyph.y, alpha = 1;
                const double elapsed = compositor.now_ms_ - glyph.start_ms;
                for (const TextTween& tween : source->text_in) {
                    const float fraction = elapsed < 0 ? 0 :
                        (tween.time_ms <= 0 ? 1 : std::min(1.0, elapsed / tween.time_ms));
                    const float offset = tween.diff * (1 - ApplyEase(tween.ease, fraction));
                    if (tween.param == "left") x += offset;
                    else if (tween.param == "top") y += offset;
                    else if (tween.param == "alpha") alpha = std::clamp(1 + offset / 255.f, 0.f, 1.f);
                }
                if (alpha <= 0 || glyph.w <= 0 || glyph.h <= 0) continue;
                auto batch = std::find_if(item.batches.begin(), item.batches.end(),
                    [&](const Batch& entry) { return entry.texture == glyph.tex; });
                if (batch == item.batches.end()) {
                    item.batches.push_back(Batch{glyph.tex, {}});
                    batch = item.batches.end() - 1;
                }
                rectangle(batch->vertices, x, y, glyph.w, glyph.h,
                          glyph.u0, glyph.v0, glyph.u1, glyph.v1, alpha);
            }
        } else if (source->texture) {
            Batch batch{source->texture, {}};
            if (!source->mesh.empty() && source->mesh.size() % 4 == 0) {
                for (size_t i = 0; i < source->mesh.size(); i += 4)
                    vertex(batch.vertices, source->mesh[i], source->mesh[i + 1],
                           source->mesh[i + 2], source->mesh[i + 3], 1);
            } else {
                rectangle(batch.vertices, 0, 0, source->w, source->h,
                          source->u0, source->v0, source->u1, source->v1, 1);
            }
            item.batches.push_back(std::move(batch));
        }
        const float determinant = matrix.a * matrix.d - matrix.b * matrix.c;
        if (std::isfinite(determinant) && std::abs(determinant) > 1e-8f) {
            auto& coverage = item.coverage;
            coverage.inverse[0] = matrix.d / determinant;
            coverage.inverse[1] = -matrix.b / determinant;
            coverage.inverse[3] = -matrix.c / determinant;
            coverage.inverse[4] = matrix.a / determinant;
            coverage.inverse[6] = (matrix.c * matrix.ty - matrix.d * matrix.tx) / determinant;
            coverage.inverse[7] = (matrix.b * matrix.tx - matrix.a * matrix.ty) / determinant;
            const auto clip = item.effect.parameters.find("clip");
            if (item.effect.intermediate && clip != item.effect.parameters.end()) {
                auto* rect = coverage.rect;
                coverage.clip = std::sscanf(clip->second.c_str(), "%f,%f,%f,%f", rect, rect+1, rect+2, rect+3) == 4 &&
                    std::all_of(rect, rect+4, [](float value) { return std::isfinite(value); }) && rect[2] >= 0 && rect[3] >= 0;
            }
            const auto mask = compositor.masks_.find(item.effect.mask);
            if (mask != compositor.masks_.end()) {
                coverage.mask = mask->second.texture;
                coverage.mask_width = mask->second.width;
                coverage.mask_height = mask->second.height;
            }
            if (item.effect.stage_mask) {
                coverage = LayerCoverage{};
                coverage.mask = item.effect.stage_mask;
                coverage.mask_width = width_;
                coverage.mask_height = height_;
                coverage.mask_alpha = true;
            }
        }
        items_.push_back(std::move(item));
    }
    for (size_t i = 0; i < items_.size(); ++i) {
        size_t end = i + 1;
        const std::string prefix = items_[i].id + ".";
        while (end < items_.size() && items_[end].id.compare(0, prefix.size(), prefix) == 0) ++end;
        items_[i].subtree_end = end;
    }
}

void Compositor::SceneRenderer::SubmitVertices(uint32_t texture, const std::vector<float>& vertices,
                                              float alpha, bool top_down) {
    if (vertices.empty()) return;
    const auto& b = brush_;
    glUseProgram(b.program);
    glUniform2f(b.u_screen, float(width_), float(height_));
    glUniform1f(b.u_top_down, top_down ? 1 : 0);
    glUniform1f(b.u_alpha, alpha);
    glUniform1i(b.u_tex, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glVertexAttribPointer(b.a_pos, 2, GL_FLOAT, GL_FALSE, 20, vertices.data());
    glVertexAttribPointer(b.a_uv, 2, GL_FLOAT, GL_FALSE, 20, vertices.data() + 2);
    glVertexAttribPointer(b.a_opacity, 1, GL_FLOAT, GL_FALSE, 20, vertices.data() + 4);
    glEnableVertexAttribArray(b.a_pos);
    glEnableVertexAttribArray(b.a_uv);
    glEnableVertexAttribArray(b.a_opacity);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 5);
    glDisableVertexAttribArray(b.a_opacity);
    glVertexAttrib1f(b.a_opacity, 1);
}

void Compositor::SceneRenderer::Submit(const Item& item, float inherited, bool top_down) {
    for (const Batch& batch : item.batches)
        SubmitVertices(batch.texture, batch.vertices, item.transform.alpha / inherited, top_down);
}

void Compositor::SceneRenderer::Traverse(Compositor& compositor, size_t first, size_t last, size_t depth,
                                       uint32_t target, bool top_down, float inherited) {
    size_t cursor = first;
    while (cursor < last) {
        const Item& item = items_[cursor];
        if (!item.transform.visible || item.transform.alpha <= 0) { ++cursor; continue; }
        const size_t end = std::min(last, item.subtree_end);
        if (item.effect.Active()) {
            const uint32_t group = compositor.shaders_.Begin(depth, width_, height_, target, top_down);
            if (group) {
                Submit(item, item.transform.alpha, true);
                Traverse(compositor, cursor + 1, end, depth + 1, group, true, item.transform.alpha);
                compositor.shaders_.End(depth, item.effect, target, top_down,
                                        item.transform.alpha / inherited, textures_, item.coverage);
                cursor = end;
                continue;
            }
        }
        Submit(item, inherited, top_down);
        ++cursor;
    }
}

void Compositor::SceneRenderer::Present(uint32_t target, const int* viewport) {
    glBindFramebuffer(GL_FRAMEBUFFER, target);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    glDisable(GL_BLEND);
    const auto& b = brush_;
    glUseProgram(b.program);
    glUniform2f(b.u_screen, float(width_), float(height_));
    glUniform1f(b.u_top_down, 0);
    glUniform1f(b.u_alpha, 1);
    glUniform1i(b.u_tex, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, scene_texture_);
    glDisableVertexAttribArray(b.a_opacity);
    glVertexAttrib1f(b.a_opacity, 1);
    const float vertices[] = {0,0,0,1, float(width_),0,1,1,
                              0,float(height_),0,0, float(width_),float(height_),1,0};
    glVertexAttribPointer(b.a_pos, 2, GL_FLOAT, GL_FALSE, 16, vertices);
    glVertexAttribPointer(b.a_uv, 2, GL_FLOAT, GL_FALSE, 16, vertices + 2);
    glEnableVertexAttribArray(b.a_pos);
    glEnableVertexAttribArray(b.a_uv);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void Compositor::SceneRenderer::Draw(Compositor& compositor) {
    if (!ready_) return;
    GLint target = 0, viewport[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &target);
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (!AllocateScene(compositor.stage_w_, compositor.stage_h_)) {
        glBindFramebuffer(GL_FRAMEBUFFER, target);
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        return;
    }
    TakeSnapshot(compositor);
    glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo_);
    glViewport(0, 0, width_, height_);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    Traverse(compositor, 0, items_.size(), 0, scene_fbo_, false, 1);
    glDisable(GL_BLEND);
    Overlay();
    retained_ = true;
    Present(target, viewport);
    // Copy the hook before invoking it: it may release or reenter this owner.
    // No scene, scratch, or compositor state may be touched after the call.
    const auto present = compositor.present_cb_;
    if (present) present();
}

void Compositor::Init(int stage_w, int stage_h) {
    stage_w_ = stage_w;
    stage_h_ = stage_h;
    InitGl();
}

void Compositor::ReleaseStageMaskGl() {
    for (auto &entry : stage_masks_) {
        if (entry.second.fbo) glDeleteFramebuffers(1, &entry.second.fbo);
        if (entry.second.texture) glDeleteTextures(1, &entry.second.texture);
    }
    stage_masks_.clear();
}

// E-mote stencil composite: draw the named mask layers' alpha into the
// stage-sized texture for `key`. Mask sources are ordinary part layers (their
// texture and effective transform are already in the compositor), so this is a
// second pass over them; the result is sampled in stage space by masked
// layers. Distinct keys keep distinct masks alive until the next render.
uint32_t Compositor::RenderStageMask(int key, const std::vector<std::string> &ids) {
    if (!scene_renderer_.Ready() || ids.empty()) return 0;
    const auto& prog_ = scene_renderer_.SharedBrush();
    StageMaskTarget &target = stage_masks_[key];
    if (!target.fbo) {
        glGenTextures(1, &target.texture);
        glBindTexture(GL_TEXTURE_2D, target.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, stage_w_, stage_h_, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenFramebuffers(1, &target.fbo);
    }
    GLint previous = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous);
    glBindFramebuffer(GL_FRAMEBUFFER, target.fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           target.texture, 0);
    glViewport(0, 0, stage_w_, stage_h_);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(prog_.program);
    glUniform2f(prog_.u_screen, float(stage_w_), float(stage_h_));
    glUniform1f(prog_.u_top_down, 0);
    glUniform1i(prog_.u_tex, 0);
    glActiveTexture(GL_TEXTURE0);
    glDisableVertexAttribArray(prog_.a_opacity);
    glVertexAttrib1f(prog_.a_opacity, 1);
    glEnable(GL_BLEND);
    // Premultiplied over-compositing accumulates overlapping mask shapes into
    // one coverage (the reference host draws every mask command).
    glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    bool any = false;
    for (const auto &id : ids) {
        const Layer *layer = nullptr;
        for (const auto &l : layers_) if (l.id == id) { layer = &l; break; }
        if (!layer || !layer->texture) continue;
        const auto transform = EffectiveTransform(*layer);
        if (!transform.visible || transform.alpha <= 0) continue;
        glUniform1f(prog_.u_alpha, 1);
        glBindTexture(GL_TEXTURE_2D, layer->texture);
        const auto p0 = transform.Point(0, 0), p1 = transform.Point(layer->w, 0);
        const auto p2 = transform.Point(0, layer->h), p3 = transform.Point(layer->w, layer->h);
        float verts[16] = {
            p0.first, p0.second, layer->u0, layer->v0,
            p1.first, p1.second, layer->u1, layer->v0,
            p2.first, p2.second, layer->u0, layer->v1,
            p3.first, p3.second, layer->u1, layer->v1,
        };
        glVertexAttribPointer(prog_.a_pos, 2, GL_FLOAT, GL_FALSE, 16, verts);
        glEnableVertexAttribArray(prog_.a_pos);
        glVertexAttribPointer(prog_.a_uv, 2, GL_FLOAT, GL_FALSE, 16, verts + 2);
        glEnableVertexAttribArray(prog_.a_uv);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        any = true;
    }
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_FRAMEBUFFER, previous);
    return any ? target.texture : 0;
}

void Compositor::SetLayerStageMask(const std::string &id, uint32_t texture) {
    for (auto &l : layers_) {
        if (l.id != id) continue;
        if (l.effect.stage_mask == texture) return;
        l.effect.stage_mask = texture;
        ++revision_;
        return;
    }
}

void Compositor::SetLayerPaint(const std::string &id, int paint) {
    for (auto &l : layers_) {
        if (l.id != id) continue;
        if (l.effect.paint == paint) return;
        l.effect.paint = paint;
        ++revision_;
        return;
    }
}

#else // host stubs — no GL; keep layer math (rects / z / hit-test) functional
      // so `artc drive` can trace and click real UI without a GPU.

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool Compositor::InitGl() { return scene_renderer_.InitGl(); }
bool Compositor::SceneRenderer::InitGl() { return false; }
void Compositor::SceneRenderer::ReleaseGl() {
    activity_ = Activity{};
    ready_ = retained_ = false;
    items_.clear();
    textures_.clear();
}
void Compositor::SceneRenderer::Draw(Compositor&) {}
bool Compositor::SceneRenderer::Snapshot(SnapshotImage&) const { return false; }
bool Compositor::SceneRenderer::BeginTransition(Compositor&, double now_ms, int time_ms,
                                               const std::vector<uint8_t>&, int, int, int) {
    activity_ = Activity{};
    if (time_ms <= 0 || !std::isfinite(now_ms)) return false;
    activity_.duration_ms = time_ms;
    activity_.phase = Activity::Phase::Armed;
    activity_.Advance(now_ms);
    return true;
}
void Compositor::SceneRenderer::Cancel() { activity_ = Activity{}; }
// A non-zero fake handle keeps Layer::texture a valid "has pixels" flag for
// HitLayer/DumpRects; no GL object is created on the host.
constexpr uint32_t kHostTexture = 0xFFFFFFFFu;

bool Compositor::SetPixels(const std::string& id, const uint8_t* rgba, int width, int height) {
    if(!rgba || width<=0 || height<=0) return false;
    SetProps(id,{});
    for(auto& l:layers_) if(l.id==id) {
        if(!l.texture || l.w==l.tex_w) l.w=width;
        if(!l.texture || l.h==l.tex_h) l.h=height;
        l.texture=kHostTexture; l.tex_w=width; l.tex_h=height;
        l.glyphs.clear(); l.text.clear(); return true;
    }
    return false;
}
uint32_t Compositor::CreateTexture(const uint8_t *, int, int) { return 0; }
bool Compositor::LoadImage(const std::string &id, const std::string &file) {
    ++revision_;
    if (!packs_) return false;
    static const char *kExtensions[] = {"", ".png", ".jpg", ".jpeg"};
    std::vector<uint8_t> bytes;
    std::string resolved;
    for (const char *ext : kExtensions) {
        resolved = file + ext;
        if (packs_->Read(resolved, bytes)) break;
    }
    if (bytes.empty()) {
        Log(kLogWarn, "lyc: image not found: " + file);
        return false;
    }
    int w = 0, h = 0, channels = 0;
    uint8_t *pixels = stbi_load_from_memory(bytes.data(), (int)bytes.size(),
                                            &w, &h, &channels, 4);
    if (!pixels) {
        Log(kLogWarn, "lyc: decode failed: " + file);
        return false;
    }
    stbi_image_free(pixels);
    for (auto &l : layers_) {
        if (l.id == id) {
            l.texture = kHostTexture;
            l.tex_w = w; l.tex_h = h;
            l.content_x = l.content_y = 0;
            l.w = (float)w; l.h = (float)h;
            l.u0 = l.v0 = 0; l.u1 = l.v1 = 1;
            return true;
        }
    }
    Layer l;
    l.id = id;
    l.texture = kHostTexture;
    l.tex_w = w; l.tex_h = h;
    l.w = (float)w; l.h = (float)h;
    l.z = 0;
    layers_.push_back(l);
    return true;
}
void Compositor::SetProps(const std::string &id,
                          const std::map<std::string, std::string> &attrs) {
    ++revision_;
    for (auto &l : layers_) {
        if (l.id != id) continue;
        l.effect.Set(attrs);
        if (const auto zoom=attrs.find("zoom"); zoom!=attrs.end())
            l.sx=l.sy=ToF(zoom->second,100)/100.f;
        for (const auto &kv : attrs) {
            if (kv.first == "x" || kv.first == "left") { l.x = ToF(kv.second); l.own_pos = true; }
            else if (kv.first == "y" || kv.first == "top") { l.y = ToF(kv.second); l.own_pos = true; }
            else if (kv.first == "w") l.w = ToF(kv.second);
            else if (kv.first == "h") l.h = ToF(kv.second);
            else if (kv.first == "alpha") {
                const float a = ToF(kv.second);
                l.alpha = std::clamp(a / 255.0f, 0.0f, 1.0f);
            }
            else if (kv.first == "rotate") l.rotate = ToF(kv.second);
            else if (kv.first == "reversex") l.reverse_x = ToF(kv.second)!=0;
            else if (kv.first == "reversey") l.reverse_y = ToF(kv.second)!=0;
            else if (kv.first == "anchorx") l.ax = ToF(kv.second);
            else if (kv.first == "xscale") { const float v = ToF(kv.second); l.sx = v / 100.0f; }
            else if (kv.first == "yscale") { const float v = ToF(kv.second); l.sy = v / 100.0f; }
            else if (kv.first == "ownpos") l.own_pos = true;
            else if (kv.first == "anchory") l.ay = ToF(kv.second);
            else if (kv.first == "visible") l.visible = (kv.second != "0");
            else if (kv.first == "z") l.z = ToI(kv.second);
            else if (kv.first == "draggable") l.draggable = (kv.second != "0");
            else if (kv.first == "dragarea") {
                if (std::sscanf(kv.second.c_str(), "%f,%f,%f,%f",
                                &l.drag_l, &l.drag_t, &l.drag_r, &l.drag_b) == 4) {
                    l.has_dragarea = true;
                }
            }
            else if (kv.first == "clip") {
                int cx = 0, cy = 0, cw = 0, chh = 0;
                if (std::sscanf(kv.second.c_str(), "%d,%d,%d,%d", &cx, &cy, &cw, &chh) == 4 &&
                    l.effect.intermediate==0 && l.tex_w > 0 && l.tex_h > 0 && cw > 0 && chh > 0) {
                    l.u0 = (float)cx / l.tex_w;      l.v0 = (float)cy / l.tex_h;
                    l.u1 = (float)(cx + cw) / l.tex_w; l.v1 = (float)(cy + chh) / l.tex_h;
                    l.w = (float)cw;
                    l.h = (float)chh;
                }
            }
        }
        l.z = 0;
        return;
    }
    // texture-less ghost layer for pure transform holders
    Layer g;
    g.id = id;
    g.visible = true;
    g.z = 0;
    layers_.push_back(g);
    SetProps(id, attrs);
}
void Compositor::DeleteLayer(const std::string &id) {
    ++revision_;
    tween_scheduler_.Remove(id);
    // group delete: an id names the whole subtree (artemis groups layers as
    // "500.d.1.0", "500.z.*", ...; scripts delete the parent "500" only).
    const std::string prefix = id + ".";
    for (auto it = layers_.begin(); it != layers_.end();) {
        if (it->id == id ||
            it->id.compare(0, prefix.size(), prefix) == 0) {
            it = layers_.erase(it);
        } else {
            ++it;
        }
    }
}
bool Compositor::RenameLayer(const std::string &id, const std::string &to) {
    if (id.empty() || to.empty() || id == to) return false;
    ++revision_;
    bool any = false;
    const std::string prefix = id + ".";
    for (auto &l : layers_) {
        if (l.id == id) { l.id = to; any = true; }
        else if (l.id.compare(0, prefix.size(), prefix) == 0) {
            l.id = to + l.id.substr(id.size());
            any = true;
        }
    }
    tween_scheduler_.Rename(id, to);
    return any;
}
bool Compositor::LoadFont(const std::string &file) {
    Log(kLogInfo, "font (host, no raster): " + file);
    return false;
}
bool Compositor::SetText(const std::string &id, const std::string &text,
                         float size, uint32_t color, float wrapWidth,
                         const std::map<std::string, std::string>& style,
                         const std::vector<TextRuby>& ruby) {
    ++revision_;
    if (text.empty()) return false;
    (void)color;   // host mock ignores the baked-in color (no rasterization)
    (void)wrapWidth;
    // fake-texture message layer: width ~ proportional to glyph count
    const int tw = std::max(1, static_cast<int>(size * text.size()));
    const int th = std::max(1, static_cast<int>(size * 1.5f));
    for (auto &l : layers_) {
        if (l.id == id) {
            l.texture = kHostTexture;
            l.tex_w = tw; l.tex_h = th;
            l.w = (float)tw; l.h = (float)th;
            l.text_pen_x = (float)tw; l.text_pen_y = 0;
            Log(kLogInfo, "SetText: replaced " + id + " " + std::to_string(tw) + "x" +
                              std::to_string(th) + " '" + text.substr(0, 24) + "'");
            return true;
        }
    }
    Layer l;
    l.id = id;
    l.texture = kHostTexture;
    l.tex_w = tw; l.tex_h = th;
    l.w = (float)tw; l.h = (float)th;
    l.text_pen_x = (float)tw; l.text_pen_y = 0;
    l.x = 40.0f;
    l.y = (float)stage_h_ - (float)th - 60.0f;
    l.z = 100;
    layers_.push_back(l);
    Log(kLogInfo, "SetText: " + id + " " + std::to_string(tw) + "x" +
                      std::to_string(th) + " '" + text.substr(0, 24) + "'");
    return true;
}
void Compositor::Shutdown() { scene_renderer_.ReleaseGl(); tween_scheduler_.Clear(); layers_.clear(); present_cb_ = nullptr; }
void Compositor::ReleaseGl() { ++revision_; layers_.clear(); tween_scheduler_.Clear(); scene_renderer_.ReleaseGl(); }
uint32_t Compositor::RenderStageMask(int, const std::vector<std::string> &) { return 0; }
void Compositor::SetLayerStageMask(const std::string &id, uint32_t texture) {
    for (auto &l : layers_) {
        if (l.id != id) continue;
        l.effect.stage_mask = texture;
        ++revision_;
        return;
    }
}
void Compositor::SetLayerPaint(const std::string &id, int paint) {
    for (auto &l : layers_) {
        if (l.id != id) continue;
        l.effect.paint = paint;
        ++revision_;
        return;
    }
}
bool Compositor::Snapshot(SnapshotImage& output) const { return scene_renderer_.Snapshot(output); }
bool Compositor::BeginTransition(double now_ms, int time_ms, const std::vector<uint8_t>& rule,
                                 int rule_w, int rule_h, int vague) {
    const bool was_active = scene_renderer_.Active();
    const bool begun = scene_renderer_.BeginTransition(*this, now_ms, time_ms, rule, rule_w, rule_h, vague);
    if (was_active || begun) ++revision_;
    return begun;
}
void Compositor::Draw() { scene_renderer_.Draw(*this); }
void Compositor::Init(int stage_w, int stage_h) {
    stage_w_ = stage_w;
    stage_h_ = stage_h;
}

#endif

} // namespace artc
