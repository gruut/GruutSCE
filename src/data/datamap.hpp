#ifndef GRUUTSCE_DATAMAP_HPP
#define GRUUTSCE_DATAMAP_HPP

#include <string>
#include <unordered_map>

#include "../config.hpp"

namespace gruut::gsce {

struct DataRecord {
  std::string value;
  bool updatable;
  DataRecord() : value(""), updatable(false){
  }

  DataRecord(std::string value_, bool updatable_) : value(std::move(value_)), updatable(updatable_){
  };
};


class Datamap {
private:
  std::unordered_map<std::string, DataRecord> m_storage;

public:

  template <typename S = std::string, typename D = DataRecord>
  void set(S &&key, D &&vv) {
    if(key.empty())
      return;

    auto ret = m_storage.insert({key,vv}); // as new
    if (!ret.second) { // as update
      if(ret.first->second.updatable) // only updatable
        ret.first->second = vv;
    }
  }

  template <typename S1 = std::string, typename S2 = std::string>
  void set(S1 &&key, S2 &&value, bool updatable = false) {
    if(key.empty())
      return;

    DataRecord vv(value, updatable);
    set(key,vv);
  }

  template <typename S = std::string>
  bool get(S &&key, DataRecord &data) {
    auto map_it = m_storage.find(key);
    if(map_it == m_storage.end())
      return false;

    data = map_it->second;
    return true;
  }

  template <typename S = std::string>
  std::string get(S &&key) {
    auto map_it = m_storage.find(key);
    if(map_it == m_storage.end())
      return std::string("");

    return data = map_it->second;
  }

  void clear(){
    m_storage.clear();
  }


};

}

#endif //GRUUTSCE_DATAMAP_HPP
