#ifndef GRUUTSCE_DATA_STORAGE_HPP
#define GRUUTSCE_DATA_STORAGE_HPP

#include <string>

#include "../config.hpp"
#include "datamap.hpp"

namespace gruut::gsce {

struct DataAttribute {
  std::string name;
  std::string value;
  DataAttribute() = default;
  DataAttribute(std::string &name_, std::string &value_) : name (name_), value (value_) {}
};


class DataStorage {
private:
  Datamap m_tx_datamap; // set of { keyword : value }
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

  std::optional<std::vector<DataAttribute>> getWorld() {
    nlohmann::json query = {
        {"type","world.get"},
        {"where",{}}
    };

    return queryIfAndReturn(query);
  }

  std::optional<std::vector<DataAttribute>> getChain() {
    nlohmann::json query = {
        {"type","chain.get"},
        {"where",{}}
    };

    return queryIfAndReturn(query);
  }

  template <typename S = std::string>
  std::optional<std::vector<DataAttribute>> getUserAttribute(S && user_id_) {
    if(user_id_.empty())
      return std::nullopt;

    std::string user_id = user_id_;

    if(user_id[0] == '$') {
      user_id = eval(user_id);
    }

    nlohmann::json query = {
        {"type","user.info.get"},
        {"where",
          {"uid",user_id}
        }
    };

    return queryIfAndReturn(query);
  }

  template <typename S1 = std::string, typename S2 = std::string, typename S3 = std::string>
  std::optional<std::vector<DataAttribute>> getScopeVariables(S1 &&scope, S2 &&id, S3 &&name){

    if(scope.empty() || id.empty() || name.empty() || !(scope == "user" || scope == "contract"))
      return std::nullopt;

    nlohmann::json query = {
        {"type", scope == "user" ? "user.info.get" : "contract.scope.get"},
        {"where",
          {"uid", id},
          {"notag", true}
        }
    };

    if(!name.empty() && name != "*") {
      query["where"]["name"] = name;
    }

    return queryIfAndReturn(query);
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

private:

  std::optional<std::vector<DataAttribute>> queryIfAndReturn(nlohmann::json &query) {
    std::string query_key = TypeConverter::toString(nlohmann::json::to_cbor(query));

    nlohmann::json query_result;

    auto it_cache = m_storage_cache.find(query_key);
    if(it_cache != m_storage_cache.end()) {
      query_result = (*it_cache).second;
    } else {
      query_result = m_read_storage_interface(query);
      m_storage_cache[query_key] = query_result;
    }

    auto query_result_name = json::get<nlohmann::json>(query_result,"name");
    auto query_result_data = json::get<nlohmann::json>(query_result,"data");

    if(!query_result_name || !query_result_data)
      return std::nullopt;

    std::vector<DataAttribute> ret_vec;

    for(auto &each_row : query_result_data.value()) {
      for(int i = 0; i < each_row.size(); ++i) {
        ret_vec.emplace_back(query_result_name.value()[i].get<std::string>(),each_row[i].get<std::string>());
      }
    }

    return ret_vec;
  }

};

}

#endif //GRUUTSCE_DATA_STORAGE_HPP
