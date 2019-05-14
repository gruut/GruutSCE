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
  static uint64_t  simpletime2timestamp(const std::string &simple_time) {
    std::istringstream in{simple_time};
    date::sys_time<std::chrono::milliseconds> time_point;
    in >> date::parse("%F", time_point);

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
  inline static std::vector<std::byte> decodeBase64(T &&input) {
    try {
      auto s_vector = Botan::base64_decode(input);
      return std::vector<std::byte>(s_vector.begin(), s_vector.end());
    } catch (Botan::Exception &e) {
    }

    return std::vector<std::byte>();
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
  inline static std::vector<std::byte> decodeBase58(T &&input) {
    try {
      auto s_vector = Botan::base58_decode(input);
      return std::vector<std::byte>(s_vector.begin(), s_vector.end());
    } catch (Botan::Exception &e) {
    }

    return std::vector<std::byte>();
  }

  static std::vector<std::string> split (const std::string &s, const std::string &delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
      token = s.substr (pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
  }

  template <typename T>
  static T str2num (const std::string &s) {
    T ret_val = 0;
    if(!s.empty()) {
      try{
        ret_val = std::stoi(s);
      }
      catch (...){

      }
    }

    return ret_val;
  }

};

#endif //GRUUTSCE_VS_TOOL_HPP
