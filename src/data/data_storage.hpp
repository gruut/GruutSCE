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

struct UserScopeRecord {
  std::string var_name;
  std::string var_value;
  uint8_t var_type;
  std::string var_owner;
  uint64_t up_time;
  uint64_t up_block;
  std::string tag;
  std::string pid;
};

struct ContractScopeRecord {
  std::string contract_id;
  std::string var_name;
  std::string var_value;
  uint8_t var_type;
  std::string var_info;
  uint64_t up_time;
  uint64_t up_block;
  std::string pid;
};

struct UserAttributeRecord {
  std::string uid;
  std::string register_day;
  std::string register_code;
  uint8_t gender;
  std::string isc_type;
  std::string isc_code;
  std::string location;
  int age_limit;
  std::string sigma;
};

struct UserCertRecord {
  std::string uid;
  std::string sn;
  uint64_t nvbefore;
  uint64_t nvafter;
  std::string x509;
};

class DataStorage {
private:
  Datamap m_tx_datamap; // set of { keyword : value }
  std::unordered_map<std::string, nlohmann::json> m_query_cache;

  // TODO : replace cache tables to real cache

  std::map<std::string, UserScopeRecord> m_user_scope_table;
  std::map<std::string, ContractScopeRecord> m_contract_scope_table;
  std::map<std::string, UserAttributeRecord> m_user_attr_table;
  std::map<std::string, UserCertRecord> m_user_cert_table;

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

    return queryIfAndParseData(query);
  }

  std::optional<std::vector<DataAttribute>> getChain() {
    nlohmann::json query = {
        {"type","chain.get"},
        {"where",{}}
    };

    return queryIfAndParseData(query);
  }

  template <typename S = std::string>
  std::optional<std::vector<DataAttribute>> getUserInfo(S &&user_id_) {
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

    return queryIfAndParseData(query);
  }

  template <typename S1 = std::string, typename S2 = std::string, typename S3 = std::string>
  std::optional<std::vector<DataAttribute>> getScopeVariables(S1 &&scope, S2 &&id, S3 &&name){

    if(scope.empty() || id.empty() || name.empty() || !(scope == "user" || scope == "contract"))
      return std::nullopt;

    std::vector<DataAttribute> ret_vec;

    if(name != "*") {
      std::string scope_key = scope + id + name;
      if (scope == "user") {
        auto it_tbl = m_user_scope_table.find(scope_key);
        if (it_tbl != m_user_scope_table.end()) {
          ret_vec.emplace_back(name,it_tbl->second.var_value);
        }
      } else { // contract
        auto it_tbl = m_contract_scope_table.find(scope_key);
        if (it_tbl != m_contract_scope_table.end()) {
          ret_vec.emplace_back(name,it_tbl->second.var_value);
        }
      }

    } else {
      if (scope == "user") {
        for (auto &each_row : m_user_scope_table) {
          if(each_row.second.var_owner == id) {
            ret_vec.emplace_back(each_row.second.var_name,each_row.second.var_value);
          }
        }
      } else {
        for (auto &each_row : m_contract_scope_table) {
          if(each_row.second.contract_id == id) {
            ret_vec.emplace_back(each_row.second.var_name,each_row.second.var_value);
          }
        }
      }
    }

    if(!ret_vec.empty())
      return ret_vec;


    // NOT IN SCOPE CACHE TABLE!

    nlohmann::json query = {
        {"type", scope == "user" ? "user.scope.get" : "contract.scope.get"},
        {"where",
          {"uid", id},
          {"notag", true}
        }
    };

    if(!name.empty() && name != "*")
      query["where"]["name"] = name;

    if(scope == "user") {
      return queryUserScopeIfAndParseData(query, id);
    } else {
      return queryContractScopeIfAndParseData(query, id);
    }
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

  std::optional<std::vector<DataAttribute>> queryIfAndParseData(nlohmann::json &query) {
    auto query_result = queryAndCache(query);

    if(!query_result)
      return std::nullopt;

    std::vector<DataAttribute> ret_vec;

    for(auto &each_row : query_result.value().second) {
      for(int i = 0; i < each_row.size(); ++i) { // last row will be selected
        ret_vec.emplace_back( query_result.value().first[i].get<std::string>(),each_row[i].get<std::string>());
      }
    }

    return ret_vec;
  }

  std::optional<std::vector<DataAttribute>> queryUserScopeIfAndParseData(nlohmann::json &query, const std::string &id){
    auto query_result = queryAndCache(query);

    if(!query_result)
      return std::nullopt;

    std::vector<DataAttribute> ret_vec;

    std::vector<std::string> data_name;
    for (auto &each_name : query_result.value().first)
      data_name.push_back(each_name.get<std::string>());

    for(auto &each_row : query_result.value().second) {

        UserScopeRecord buf_record;

        for (int i = 0; i < data_name.size(); ++i) {
          if (data_name[i] == "var_name")
            buf_record.var_name = each_row[i].get<std::string>();
          else if (data_name[i] == "var_value")
            buf_record.var_value = each_row[i].get<std::string>();
          else if (data_name[i] == "var_type")
            buf_record.var_type = vs::str2num<uint8_t>(each_row[i].get<std::string>());
          else if (data_name[i] == "var_owner")
            buf_record.var_owner = each_row[i].get<std::string>();
          else if (data_name[i] == "up_time")
            buf_record.up_time = vs::str2num<uint64_t>(each_row[i].get<std::string>());
          else if (data_name[i] == "up_block")
            buf_record.up_block = vs::str2num<uint64_t>(each_row[i].get<std::string>());
          else if (data_name[i] == "tag")
            buf_record.tag = each_row[i].get<std::string>();
          else if (data_name[i] == "pid")
            buf_record.pid = each_row[i].get<std::string>();
        }

        if(buf_record.var_name.empty())
          continue;

        std::string scope_key = "user" + id + buf_record.var_name;
        m_user_scope_table[scope_key] = buf_record;

        ret_vec.emplace_back(buf_record.var_name,buf_record.var_value);
      }

    return ret_vec;
  }

  std::optional<std::vector<DataAttribute>> queryContractScopeIfAndParseData(nlohmann::json &query, const std::string &id){

    auto query_result = queryAndCache(query);

    if(!query_result)
      return std::nullopt;

    std::vector<DataAttribute> ret_vec;

    std::vector<std::string> data_name;
    for (auto &each_name : query_result.value().first)
      data_name.push_back(each_name.get<std::string>());

    for(auto &each_row : query_result.value().second) {

      ContractScopeRecord buf_record;

      for (int i = 0; i < data_name.size(); ++i) {
        if (data_name[i] == "var_name")
          buf_record.var_name = each_row[i].get<std::string>();
        else if (data_name[i] == "var_value")
          buf_record.var_value = each_row[i].get<std::string>();
        else if (data_name[i] == "var_type")
          buf_record.var_type = vs::str2num<uint8_t>(each_row[i].get<std::string>());
        else if (data_name[i] == "contract_id")
          buf_record.contract_id = each_row[i].get<std::string>();
        else if (data_name[i] == "up_time")
          buf_record.up_time = vs::str2num<uint64_t>(each_row[i].get<std::string>());
        else if (data_name[i] == "up_block")
          buf_record.up_block = vs::str2num<uint64_t>(each_row[i].get<std::string>());
        else if (data_name[i] == "pid")
          buf_record.pid = each_row[i].get<std::string>();
      }

      if(buf_record.var_name.empty())
        continue;

      std::string scope_key = "user" + id + buf_record.var_name;
      m_contract_scope_table[scope_key] = buf_record;

      ret_vec.emplace_back(buf_record.var_name,buf_record.var_value);
    }

    return ret_vec;
  }

  std::optional<std::pair<nlohmann::json,nlohmann::json>> queryAndCache(nlohmann::json &query) {
    std::string query_key = TypeConverter::toString(nlohmann::json::to_cbor(query));
    nlohmann::json query_result;

    auto it_cache = m_query_cache.find(query_key);
    if(it_cache != m_query_cache.end()) {
      query_result = (*it_cache).second;
    } else {
      query_result = m_read_storage_interface(query);
      m_query_cache[query_key] = query_result;
    }

    auto query_result_name = json::get<nlohmann::json>(query_result,"name");
    auto query_result_data = json::get<nlohmann::json>(query_result,"data");

    if(!query_result_name || !query_result_data)
      return std::nullopt;

    return std::make_pair(query_result_name.value(), query_result_data.value());
  }

};

}

#endif //GRUUTSCE_DATA_STORAGE_HPP
