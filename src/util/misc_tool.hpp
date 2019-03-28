#ifndef GRUUTSCE_VS_TOOL_HPP
#define GRUUTSCE_VS_TOOL_HPP

#include <string>
#include <algorithm>
#include <map>
#include <chrono>
#include <cctype>
#include <locale>

#include "date.hpp"

namespace gruut {
namespace gsce {
class vs {
public:

  static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
      return !std::isspace(ch);
    }));
  }

  static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
      return !std::isspace(ch);
    }).base(), s.end());
  }

  static inline void trim(std::string &s) {
    ltrim(s);
    rtrim(s);
  }

  static inline std::string ltrim_copy(std::string s) {
    ltrim(s);
    return s;
  }

  static inline std::string rtrim_copy(std::string s) {
    rtrim(s);
    return s;
  }

  static inline std::string trim_copy(std::string s) {
    trim(s);
    return s;
  }

  static std::string toLower(const std::string &src) {
    std::string dst("",src.size());
    std::transform(src.begin(), src.end(), dst.begin(), ::tolower);
    return dst;
  }

  static uint64_t isotime2timestamp(const std::string &iso_time) {
    std::istringstream in{iso_time};
    date::sys_time<std::chrono::milliseconds> time_point;
    in >> date::parse("%FT%TZ", time_point);
    if (in.fail())
    {
      in.clear();
      in.exceptions(std::ios::failbit);
      in.str(iso_time);
      in >> date::parse("%FT%T%Ez", time_point);
    }

    return static_cast<uint64_t>(time_point.time_since_epoch().count());
  }

};
}
}

#endif //GRUUTSCE_VS_TOOL_HPP
