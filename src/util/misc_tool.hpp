#ifndef GRUUTSCE_VS_TOOL_HPP
#define GRUUTSCE_VS_TOOL_HPP

#include <string>
#include <algorithm>
#include <map>
#include <chrono>
#include <cctype>
#include <locale>

#include <botan-2/botan/base58.h>
#include <botan-2/botan/base64.h>
#include <botan-2/botan/exceptn.h>
#include <botan-2/botan/secmem.h>

#include "date.hpp"

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

  template <typename T>
  inline static std::string encodeBase64(T &&t) {
    try {
      return Botan::base64_encode(std::vector<uint8_t>(begin(t), end(t)));
    } catch (Botan::Exception &e) {
      return std::string("");
    }
  }

  template <typename T>
  inline static std::vector<uint8_t> decodeBase64(T &&input) {
    try {
      auto s_vector = Botan::base64_decode(input);
      return std::vector<uint8_t>(s_vector.begin(), s_vector.end());
    } catch (Botan::Exception &e) {
    }

    return std::vector<uint8_t>();
  }

  template <typename T>
  inline static std::string encodeBase58(T &&t) {
    try {
      return Botan::base58_encode(std::vector<uint8_t>(begin(t), end(t)));
    } catch (Botan::Exception &e) {
      return std::string("");
    }
  }

  template <typename T>
  inline static std::vector<uint8_t> decodeBase58(T &&input) {
    try {
      auto s_vector = Botan::base58_decode(input);
      return std::vector<uint8_t>(s_vector.begin(), s_vector.end());
    } catch (Botan::Exception &e) {
    }

    return std::vector<uint8_t>();
  }

};

#endif //GRUUTSCE_VS_TOOL_HPP
