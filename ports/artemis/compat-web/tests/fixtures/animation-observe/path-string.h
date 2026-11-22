#pragma once
#include <filesystem>
#include <string>
namespace std {
inline string to_string(const filesystem::path &path) { return path.string(); }
}
