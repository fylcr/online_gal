#pragma once
#include <deque>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace artc {

class TweenScheduler {
public:
  using Read = std::function<float(const std::string&, const std::string&, float)>;
  using Write = std::function<bool(const std::string&, const std::string&, float)>;
  void Add(const std::string& id, const std::map<std::string, std::string>& attrs,
           double now_ms, const Read& read, const Write& write);
  void Begin();
  void End(double now_ms);
  bool Update(double now_ms, const Read& read, const Write& write);
  bool Finish(const std::string& id, const Read& read, const Write& write);
  void Remove(const std::string& id);
  void Rename(const std::string& id, const std::string& to);
  void Clear();
  double Remaining(double now_ms) const;
  size_t Count() const;

private:
  struct Segment {
    std::string id, param;
    float from = 0, to = 0;
    double delay_ms = 0, time_ms = 0, loop_delay_ms = 0;
    double start_ms = 0, end_ms = 0;
    int loops = 0, yoyos = 0, ease = 0;
    bool implicit = true, started = false, resolved = false;
  };
  struct Track { std::deque<Segment> segments; };
  void Register(Segment segment);
  static double Duration(const Segment& segment);
  static float Curve(int ease, double progress);
  std::vector<Track> tracks_;
  std::vector<Segment> collected_;
  bool collecting_ = false;
};

}
