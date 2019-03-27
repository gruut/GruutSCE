#ifndef GRUUTSCE_DATAMAP_HPP
#define GRUUTSCE_DATAMAP_HPP

#include <string>
#include <unordered_map>

#include "../config.hpp"

namespace gruut{
namespace gsce{

struct DataRecord {
  std::string value;
  VariableType type;
  DataRecord() : value(""), type(VariableTyp::TEXT){

  }
  DataRecord(std::string value_, VariableType type_) : value(std::move(value_)), type(type_){

  };
};


class Datamap {
private:
  std::unordered_map<std::string, DataRecord> m_storage;

public:
  bool set(const std::string &key, std::string &value, VariableType type) {
    DataRecord vv(value,type);
    m_storage.insert({key, vv});
    return true;
  }

  bool get(const std::string &key, DataRecord &data) {
    auto map_it = m_storage.find(key);
    if(map_it == m_storage.end())
      return false;

    data = map_it->second;
    return true;
  }

};

}}

#endif //GRUUTSCE_DATAMAP_HPP
