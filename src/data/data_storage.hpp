#ifndef GRUUTSCE_DATA_STORAGE_HPP
#define GRUUTSCE_DATA_STORAGE_HPP

#include <string>

#include "../config.hpp"
#include "datamap.hpp"

namespace gruut::gsce {

class DataStorage {
private:
  Datamap m_tx_datamap; // set of { keyword : value }
  Datamap m_datamap;
  std::unordered_map<std::string, nlohmann::json> m_storage_cache;

  std::function<nlohmann::json(nlohmann::json&)> m_read_storage_interface;
  std::function<void(nlohmann::json&, nlohmann::json&)> m_write_storage_interface;

public:
  DataStorage() = default;

  void attachReadInterface(std::function<nlohmann::json(nlohmann::json&)> read_storage_interface){
    m_read_storage_interface = std::move(read_storage_interface);
  }

  void attachWriteInterface(std::function<void(nlohmann::json&, nlohmann::json&)> write_storage_interface){
    m_write_storage_interface = std::move(write_storage_interface);
  }

  template <typename S1 = std::string, typename S2 = std::string>
  void updateValue(S1 &&key,S2 &&value, bool updatable = true) {
    m_tx_datamap.set(key,value, updatable);
  }

  template <typename S1 = std::string, typename S2 = std::string, typename S3 = std::string>
  std::optional<std::vector<std::pair<std::string,std::string>>> fetchDataFromStorage(S1 &&scope, S2 &&id, S3 &&name){

    if(scope.empty() || id.empty() || name.empty())
      return std::nullopt;

    nlohmann::json query;

    if(scope == "user") {
      query["type"] = "user.scope.get";
      query["where"]["uid"] = id;
    } else if (scope == "contract"){
      query["type"] = "contract.scope.get";
      query["where"]["cid"] = id;
    } else {
      return std::nullopt;
    }

    std::string query_key = TypeConverter::toString(nlohmann::json::to_cbor(query));

    nlohmann::json query_result;

    auto it_cache = m_storage_cache.find(query_key);
    if(it_cache != m_storage_cache.end()) {
      query_result = (*it_cache).second;
    } else {
      query_result = m_read_storage_interface(query);
      m_storage_cache[query_key] = query_result;
    }

    std::vector<std::pair<std::string,std::string>> ret_vec;

    if(name == "*") {
      for(auto &each_row : query_result) {
        if(each_row["tag"].get<std::string>().empty())
          ret_vec.emplace_back(std::make_pair(each_row["name"].get<std::string>(),each_row["name"].get<std::string>()));
      }
    } else {
      for(auto &each_row : query_result) {
        if(each_row["name"].get<std::string>() == name && each_row["tag"].get<std::string>().empty()) {
          ret_vec.emplace_back(std::make_pair(name, each_row["name"].get<std::string>()));
        }
      }
    }

    return ret_vec;

  }


  template <typename S = std::string>
  std::string getVal(S &&key){
    return m_datamap.get(key);
  }

  template <typename S1 = std::string, typename S2 = std::string>
  void setVal(S1 && key, S2 && value){
    m_datamap.set(key,value);
  }

  template <typename S = std::string>
  std::string eval(S && expr){
    if(expr.empty())
      return {};

    if(expr[0] != '$')
      return expr;

    return m_tx_datamap.get(expr);
  }

  void flush(){
    // TODO : write all data to write interface
  }

  void flushToStorage(){
    // TODO : flush all data to storage
  }

  void clear(){
    m_tx_datamap.clear();
  }

  Datamap& getDatamap() {
    return m_tx_datamap;
  }

};

}

#endif //GRUUTSCE_DATA_STORAGE_HPP
