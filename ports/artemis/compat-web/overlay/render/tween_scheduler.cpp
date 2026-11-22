#include "render/tween_scheduler.h"
#include <algorithm>
#include <cmath>
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <utility>

namespace artc {
namespace {
bool InSubtree(const std::string& layer, const std::string& root) {
  return layer == root || layer.compare(0, root.size() + 1, root + ".") == 0;
}

double Number(const std::string& text, double fallback = 0) {
  if (text.empty()) return fallback;
  char* end = nullptr;
  const double value = std::strtod(text.c_str(), &end);
  return end == text.c_str() || !std::isfinite(value) ? fallback : value;
}

float Endpoint(const std::string& text, float fallback = 0) {
  if (text.empty()) return fallback;
  errno = 0;
  char* end = nullptr;
  const float value = std::strtof(text.c_str(), &end);
  return end == text.c_str() || errno == ERANGE || !std::isfinite(value) ? fallback : value;
}

int RepeatCount(const std::string& text) {
  return static_cast<int>(std::clamp(Number(text), -1.0,
                                    double(std::numeric_limits<int>::max())));
}

int EaseCode(const std::string& name) {
  int direction = 0;
  std::string family = name;
  if (name.compare(0, 10, "easeinout_") == 0) {
    family = name.substr(10); direction = 2;
  } else if (name.compare(0, 8, "easeout_") == 0) {
    family = name.substr(8); direction = 1;
  } else if (name.compare(0, 7, "easein_") == 0) {
    family = name.substr(7);
  }
  const char* families[] = {"quad", "cubic", "quart", "quint", "expo", "circ",
                            "sine", "back", "elastic", "bounce"};
  if (family == "exponential") family = "expo";
  if (family == "circular") family = "circ";
  if (family == "sin") family = "sine";
  for (int i = 0; i < 10; ++i)
    if (family == families[i]) return 1 + 3 * i + direction;
  return 0;
}

double Bounce(double t) {
  if (t < 1 / 2.75) return 7.5625 * t * t;
  double offset = 2.625 / 2.75, height = .984375;
  if (t < 2 / 2.75) { offset = 1.5 / 2.75; height = .75; }
  else if (t < 2.5 / 2.75) { offset = 2.25 / 2.75; height = .9375; }
  t -= offset;
  return 7.5625 * t * t + height;
}

double InCurve(int family, double t) {
  constexpr double pi = 3.14159265358979323846;
  if (family < 4) return std::pow(t, family + 2);
  switch (family) {
  case 4: return std::exp2(10 * (t - 1));
  case 5: return 1 - std::sqrt(1 - t * t);
  case 6: return 1 - std::cos(pi * t / 2);
  case 7: return t * t * (2.70158 * t - 1.70158);
  case 8: return -std::exp2(10 * (t - 1)) * std::sin((t - 1.075) * 2 * pi / .3);
  case 9: return 1 - Bounce(1 - t);
  default: return t;
  }
}
}

// Parse once; only the active segment reads an implicit starting value.
void TweenScheduler::Add(const std::string& id,
                         const std::map<std::string, std::string>& attrs,
                         double now_ms, const Read& read, const Write& write) {
  auto get = [&](const std::string& key) {
    const auto it = attrs.find(key);
    return it == attrs.end() ? std::string() : it->second;
  };
  Segment segment;
  segment.id = id;
  segment.param = get("param");
  if (segment.param.empty()) {
    for (const char* key : {"alpha", "left", "top", "x", "y", "xscale", "yscale",
                            "zoom", "rotate", "w", "h"})
      if (attrs.count(key)) { segment.param = key; break; }
  }
  if (segment.param.empty()) return;
  segment.time_ms = std::max(0.0, Number(get("time")));
  segment.delay_ms = std::max(0.0, Number(get("delay")));
  segment.loop_delay_ms = std::max(0.0, Number(get("loopdelay")));
  segment.loops = RepeatCount(get("loop"));
  segment.yoyos = segment.loops ? 0 : RepeatCount(get("yoyo"));
  segment.ease = EaseCode(get("ease"));
  const std::string pair = get(segment.param);
  const size_t comma = pair.find(',');
  segment.to = Endpoint(!get("to").empty() ? get("to") :
                              comma == std::string::npos ? pair : pair.substr(comma + 1));
  if (!get("from").empty() || comma != std::string::npos) {
    segment.from = Endpoint(!get("from").empty() ? get("from") :
                                            pair.substr(0, comma));
    segment.implicit = false;
  }
  if (collecting_) {
    collected_.push_back(std::move(segment));
    return;
  }
  // Registration exposes the initial value, but the first tick owns the clock.
  if (!segment.implicit) write(id, segment.param, segment.from);
  (void)now_ms;
  (void)read;
  Register(std::move(segment));
}

void TweenScheduler::Register(Segment segment) {
  tracks_.erase(std::remove_if(tracks_.begin(), tracks_.end(), [&](const Track& track) {
    return track.segments.front().id == segment.id &&
           track.segments.front().param == segment.param;
  }), tracks_.end());
  Track track;
  track.segments.push_back(std::move(segment));
  tracks_.push_back(std::move(track));
}

void TweenScheduler::Begin() {
  if (collecting_) return;
  collected_.clear();
  collecting_ = true;
}

// Independent properties run in parallel; each property retains its own FIFO.
void TweenScheduler::End(double now_ms) {
  if (!collecting_) return;
  collecting_ = false;
  std::map<std::pair<std::string, std::string>, size_t> groups;
  for (auto& segment : collected_) {
    const auto key = std::make_pair(segment.id, segment.param);
    const auto found = groups.find(key);
    if (found == groups.end()) {
      // Erasure changes indices, so assemble first and register afterwards.
      groups.emplace(key, groups.size());
    }
  }
  std::vector<Track> grouped(groups.size());
  for (auto& segment : collected_)
    grouped[groups.at({segment.id, segment.param})].segments.push_back(std::move(segment));
  for (auto& track : grouped) {
    Segment first = std::move(track.segments.front());
    track.segments.pop_front();
    Register(std::move(first));
    auto& queue = tracks_.back().segments;
    while (!track.segments.empty()) {
      queue.push_back(std::move(track.segments.front()));
      track.segments.pop_front();
    }
  }
  collected_.clear();
  (void)now_ms;
}

float TweenScheduler::Curve(int ease, double t) {
  if (t <= 0) return 0;
  if (t >= 1) return 1;
  if (ease <= 0) return static_cast<float>(t);
  const int family = (ease - 1) / 3, direction = (ease - 1) % 3;
  if (direction == 0) return static_cast<float>(InCurve(family, t));
  if (direction == 1) return static_cast<float>(1 - InCurve(family, 1 - t));
  if (family == 7) {
    const double overshoot = 1.70158 * 1.525;
    const double u = t < .5 ? 2 * t : 2 * t - 2;
    return static_cast<float>(t < .5 ? u * u * ((overshoot + 1) * u - overshoot) / 2
                                    : (u * u * ((overshoot + 1) * u + overshoot) + 2) / 2);
  }
  if (family == 8) {
    constexpr double pi = 3.14159265358979323846;
    const double wave = std::sin((20 * t - 11.125) * 2 * pi / 4.5);
    return static_cast<float>(t < .5 ? -std::exp2(20 * t - 10) * wave / 2
                                    : std::exp2(-20 * t + 10) * wave / 2 + 1);
  }
  return static_cast<float>(t < .5 ? InCurve(family, 2 * t) / 2
                                  : 1 - InCurve(family, 2 - 2 * t) / 2);
}

// Completing a child arms the next child at this tick, never at a predicted end.
bool TweenScheduler::Update(double now_ms, const Read& read, const Write& write) {
  bool changed = false;
  for (auto track = tracks_.begin(); track != tracks_.end();) {
    auto& queue = track->segments;
    while (!queue.empty()) {
      Segment& segment = queue.front();
      if (!segment.started) {
        segment.started = true;
        segment.start_ms = now_ms + segment.delay_ms;
        segment.end_ms = segment.start_ms + segment.time_ms;
      }
      if (segment.implicit && !segment.resolved) {
        if (now_ms < segment.start_ms) break;
        segment.from = read(segment.id, segment.param, segment.to);
        segment.resolved = true;
      }
      float value = segment.from;
      bool done = false;
      if (now_ms > segment.start_ms) {
        if (now_ms >= segment.end_ms) {
          int& repeats = segment.loops ? segment.loops : segment.yoyos;
          if (repeats) {
            // A long stall restarts at now; a short overrun retains the prior end.
            const double anchor = now_ms - segment.end_ms < segment.time_ms
                                      ? segment.end_ms : now_ms;
            segment.start_ms = anchor + segment.loop_delay_ms;
            segment.end_ms = segment.start_ms + segment.time_ms;
            if (!segment.loops) {
              std::swap(segment.from, segment.to);
            }
            if (repeats > 0) --repeats;
            value = segment.from;
          } else { value = segment.to; done = true; }
        }
        if (!done && now_ms > segment.start_ms && segment.time_ms > 0) {
          const double progress = (now_ms - segment.start_ms) / segment.time_ms;
          value = segment.from + (segment.to - segment.from) * Curve(segment.ease, progress);
        }
      }
      changed = write(segment.id, segment.param, value) || changed;
      if (!done) break;
      queue.pop_front();
    }
    if (queue.empty()) track = tracks_.erase(track); else ++track;
  }
  return changed;
}

bool TweenScheduler::Finish(const std::string& id, const Read& read, const Write& write) {
  bool changed = false;
  auto finish = [&](Segment& segment) {
    if (!InSubtree(segment.id, id)) return false;
    if (segment.implicit && !segment.resolved)
      segment.from = read(segment.id, segment.param, segment.to);
    const float end = segment.yoyos >= 0 && (segment.yoyos & 1)
                        ? segment.from : segment.to;
    changed = write(segment.id, segment.param, end) || changed;
    return true;
  };
  for (auto& track : tracks_)
    track.segments.erase(std::remove_if(track.segments.begin(), track.segments.end(), finish),
                         track.segments.end());
  collected_.erase(std::remove_if(collected_.begin(), collected_.end(), finish), collected_.end());
  tracks_.erase(std::remove_if(tracks_.begin(), tracks_.end(),
                              [](const Track& track) { return track.segments.empty(); }), tracks_.end());
  return changed;
}

void TweenScheduler::Remove(const std::string& id) {
  for (auto& track : tracks_)
    track.segments.erase(std::remove_if(track.segments.begin(), track.segments.end(),
      [&](const Segment& segment) { return InSubtree(segment.id, id); }), track.segments.end());
  collected_.erase(std::remove_if(collected_.begin(), collected_.end(),
    [&](const Segment& segment) { return InSubtree(segment.id, id); }), collected_.end());
  tracks_.erase(std::remove_if(tracks_.begin(), tracks_.end(),
    [](const Track& track) { return track.segments.empty(); }), tracks_.end());
}

void TweenScheduler::Rename(const std::string& id, const std::string& to) {
  auto rename = [&](Segment& segment) {
    if (InSubtree(segment.id, id)) segment.id = to + segment.id.substr(id.size());
  };
  for (auto& track : tracks_) for (auto& segment : track.segments) rename(segment);
  for (auto& segment : collected_) rename(segment);
}

void TweenScheduler::Clear() {
  tracks_.clear();
  collected_.clear();
  collecting_ = false;
}

double TweenScheduler::Duration(const Segment& segment) {
  if (segment.loops < 0 || segment.yoyos < 0) return std::numeric_limits<double>::infinity();
  const double repeats = segment.loops ? segment.loops : segment.yoyos;
  return segment.time_ms * (1 + repeats) + segment.loop_delay_ms * repeats;
}

double TweenScheduler::Remaining(double now_ms) const {
  double remaining = 0;
  for (const auto& track : tracks_) {
    double total = 0;
    for (const auto& segment : track.segments) {
      if (segment.started) {
        const double repeats = segment.loops < 0 || segment.yoyos < 0
          ? std::numeric_limits<double>::infinity()
          : double(segment.loops ? segment.loops : segment.yoyos);
        const double repeat_span = segment.time_ms + segment.loop_delay_ms;
        total += std::max(0.0, segment.end_ms - now_ms) +
                 (std::isinf(repeats) ? repeats : repeats * repeat_span);
      } else total += segment.delay_ms + Duration(segment);
    }
    remaining = std::max(remaining, total);
  }
  return remaining;
}

size_t TweenScheduler::Count() const {
  size_t count = 0;
  for (const auto& track : tracks_) count += track.segments.size();
  return count;
}
}
