#ifndef VERONN_SCE_DATA_STORAGE_HPP
#define VERONN_SCE_DATA_STORAGE_HPP

#include <string>

#include "../config.hpp"
#include "datamap.hpp"

namespace veronn::vsce {

struct DataAttribute {
  std::string name;
  std::string value;
  DataAttribute() = default;
  DataAttribute(std::string &name_, std::string &value_) : name (name_), value (value_) {}
};

struct UserScopeRecord {
  std::string var_name;
  std::string var_value;
  EnumAll var_type;
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
  EnumAll var_type;
  std::string var_info;
  uint64_t up_time;
  uint64_t up_block;
  std::string pid;
};

struct UserAttributeRecord {
  std::string uid;
  std::string register_day;
  std::string register_code;
  EnumGender gender;
  std::string isc_type;
  std::string isc_code;
  std::string location;
  int age_limit;
};

struct UserCertRecord {
  std::string uid;
  std::string sn;
  uint64_t nvbefore;
  uint64_t nvafter;
  std::string x509;
};

class DataManager {
private:
  Datamap m_tx_datamap; // set of { keyword : value }
  std::unordered_map<std::string, nlohmann::json> m_query_cache;

  // TODO : replace cache tables to real cache

  std::map<std::string, std::vector<UserScopeRecord>> m_user_scope_table;
  std::map<std::string, ContractScopeRecord> m_contract_scope_table;

  std::map<std::string, UserAttributeRecord> m_user_attr_table;
  std::map<std::string, std::vector<UserCertRecord>> m_user_cert_table;

  std::function<nlohmann::json(nlohmann::json&)> m_read_storage_interface;

public:
  DataManager() = default;

  void attachReadInterface(std::function<nlohmann::json(nlohmann::json&)> &read_storage_interface){
    m_read_storage_interface = read_storage_interface;
  }

  template <typename S1 = std::string, typename S2 = std::string>
  void updateValue(S1 &&key,S2 &&value) {
    m_tx_datamap.set(key,value);
  }

  std::optional<std::string> eval(std::string_view expr_view){
    std::string expr(expr_view);
    return eval(expr);
  }

  std::string eval(std::string &expr){
    vs::trim(expr);

    if(expr.empty())
      return {};

    if(expr[0] != '$')
      return expr;

    auto eval_str = m_tx_datamap.get(expr);
    if(!eval_str)
      return {};

    return eval_str.value();
  }



  std::optional<std::string> evalOpt(std::string_view expr_view){
    std::string expr(expr_view);
    return evalOpt(expr);
  }

  std::optional<std::string> evalOpt(std::string &expr){
    vs::trim(expr);

    if(expr.empty())
      return {};

    if(expr[0] != '$')
      return expr;

    return m_tx_datamap.get(expr);
  }

  template <typename S = std::string>
  std::optional<std::string> get(S && expr) {
    return m_tx_datamap.get(expr);
  }

  void clear(){
    m_user_scope_table.clear();
    m_contract_scope_table.clear();
    m_tx_datamap.clear();
  }

  Datamap& getDatamap() {
    return m_tx_datamap;
  }

  std::vector<DataAttribute> getWorld() {
    nlohmann::json query = {
        {"type","world.get"},
        {"where",{}}
    };

    return queryIfAndParseData(query);
  }

  std::vector<DataAttribute> getChain() {
    nlohmann::json query = {
        {"type","chain.get"},
        {"where",{}}
    };

    return queryIfAndParseData(query);
  }

  template <typename S = std::string>
  std::vector<DataAttribute> getUserInfo(S &&user_id_) {
    if(user_id_.empty())
      return {};

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

    return queryIfUserAttrAndParseData(query, user_id);
  }

  template <typename S1 = std::string, typename S2 = std::string>
  int64_t getUserKeyCurrency(S1 &&user_id_) {
    if(user_id_.empty())
      return {};

    std::string user_id = user_id_;

    if(user_id[0] == '$') {
      user_id = eval(user_id);
    }

    std::vector<DataAttribute> ret_vec;

    int64_t keyc_amount = 0;

    std::string scope_key = user_id + "KEYC";
    auto it_tbl = m_user_scope_table.find(scope_key);
    if (it_tbl != m_user_scope_table.end() && !it_tbl->second.empty()) {
      for(auto &each_row : it_tbl->second) {
        if(each_row.tag.empty() && each_row.var_type == EnumAll::KEYC && each_row.var_name == "KEYC") {
          keyc_amount = vs::str2num<int64_t>(each_row.var_value);
          break;
        }
      }
    }

    if(!ret_vec.empty())
      return keyc_amount;

    nlohmann::json query = {
        {"type", "user.scope.get"},
        {"where",
         {"uid", user_id},
         {"name", "KEYC"},
         {"type", "KEYC"},
         {"notag", true}
        }
    };

    auto result = queryIfUserScopeAndParseData(query, user_id);

    if(!result.empty() && result[0].name == "KEYC"){
      keyc_amount = vs::str2num<int64_t>(result[0].value);
    }

    return keyc_amount;

  }

  template <typename S = std::string>
  std::vector<DataAttribute> getUserCert(S &&user_id_) {
    if(user_id_.empty())
      return {};

    std::string user_id = user_id_;

    if(user_id[0] == '$') {
      user_id = eval(user_id);
    }

    nlohmann::json query = {
        {"type","user.cert.get"},
        {"where",
         {"uid",user_id}
        }
    };

    return queryIfUserCertAndParseData(query, user_id);
  }

  template <typename S1 = std::string, typename S2 = std::string>
  std::optional<UserScopeRecord> getUserScopeRecordByName(S1 &&id, S2 &&name) {

    if(name == "*")
      return std::nullopt;

    std::string scope_key = id + name;

    auto ret_record = findUserScopeTableByName(id,name);

    if(ret_record)
      return ret_record;

    nlohmann::json query = {
        {"type", "user.scope.get"},
        {"where",
         {"uid", id},
         {"name", name}
        }
    };

    queryIfUserScopeAndParseData(query, id, "", false); // try to cache - ignore return value;

    return findUserScopeTableByName(id,name);
  }

  template <typename S1 = std::string, typename S2 = std::string, typename S3 = std::string>
  std::optional<UserScopeRecord> getUserScopeRecordByPid(S1 &&id, S2 &&name, S3 &&pid) {

    std::string scope_key = id + name;

    auto ret_record = findUserScopeTableByPid(id,name,pid);

    if(ret_record)
      return ret_record;

    nlohmann::json query = {
        {"type", "user.scope.get"},
        {"where",
         {"uid", id},
         {"pid", pid}
        }
    };

    queryIfUserScopeAndParseData(query, id, "", false); // try to cache - ignore return value;

    return findUserScopeTableByPid(id,name,pid);
  }

  template <typename S1 = std::string, typename S2 = std::string, typename S3 = std::string>
  std::vector<DataAttribute> getScopeVariables(S1 &&scope, S2 &&id, S3 &&name){

    if(scope.empty() || id.empty() || name.empty() || !(scope == "user" || scope == "contract"))
      return {};

    std::vector<DataAttribute> ret_vec;

    if(name != "*") {
      std::string scope_key = id + name;
      if (scope == "user") {
        auto it_tbl = m_user_scope_table.find(scope_key);
        if (it_tbl != m_user_scope_table.end() && !it_tbl->second.empty()) {
          for(auto &each_row : it_tbl->second) {
            if(each_row.tag.empty()) {
              ret_vec.emplace_back(name, each_row.var_value);
              break;
            }
          }

        }
      } else { // contract
        auto it_tbl = m_contract_scope_table.find(scope_key);
        if (it_tbl != m_contract_scope_table.end()) {
          ret_vec.emplace_back(name, it_tbl->second.var_value);
        }
      }

    } else {
      if (scope == "user") {
        for (auto &each_item : m_user_scope_table) {
          for (auto &each_row : each_item.second) {
            if (each_row.var_owner == id && each_row.tag.empty()) {
              ret_vec.emplace_back(each_row.var_name, each_row.var_value);
            }
          }
        }
      } else {
        for (auto &each_item : m_contract_scope_table) {
          if(each_item.second.contract_id == id) {
            ret_vec.emplace_back(each_item.second.var_name,each_item.second.var_value);
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
      return queryIfUserScopeAndParseData(query, id);
    } else {
      return queryIfContractScopeAndParseData(query, id);
    }
  }


private:
  template <typename S1 = std::string, typename S2 = std::string>
  std::optional<UserScopeRecord> findUserScopeTableByName(S1 &&id, S2 &&name){
    std::string scope_key = id + name;

    bool null_tag = false;
    bool found = false;

    UserScopeRecord ret_record;

    auto it_tbl = m_user_scope_table.find(scope_key);
    if (it_tbl != m_user_scope_table.end() && !it_tbl->second.empty()) {
      for(auto &each_row : it_tbl->second) {
        if(each_row.var_name == name) {
          found = true;

          if(each_row.tag.empty()) {
            null_tag = true;
            ret_record = each_row;
          }

          if(!null_tag)
            ret_record = each_row;
        }
      }
    }

    if(found)
      return ret_record;

    return std::nullopt;
  }

  template <typename S1 = std::string, typename S2 = std::string, typename S3 = std::string>
  std::optional<UserScopeRecord> findUserScopeTableByPid(S1 &&id, S2 &&name, S3 &&pid){

    std::string scope_key = id + name;

    auto it_tbl = m_user_scope_table.find(scope_key);
    if (it_tbl != m_user_scope_table.end() && !it_tbl->second.empty()) {
      for(auto &each_row : it_tbl->second) {
        if(each_row.pid == pid) {
          return each_row;
        }
      }
    }

    return std::nullopt;
  }


  std::vector<DataAttribute> queryIfAndParseData(nlohmann::json &query) {
    auto [result_name,result_data] = queryAndCache(query);

    std::vector<DataAttribute> ret_vec;

    if(!result_name.empty() && !result_data.empty()) {

      for (auto &each_row : result_data) {
        for (int i = 0; i < each_row.size(); ++i) { // last row will be selected
          std::string value = each_row[i].get<std::string>();
          ret_vec.emplace_back(result_name[i], value);
        }
      }
    }

    return ret_vec;
  }

  std::vector<DataAttribute> queryIfUserCertAndParseData(nlohmann::json &query, const std::string &id) {
    auto [result_name,result_data] = queryAndCache(query);

    std::vector<DataAttribute> ret_vec;

    if(!result_name.empty() && !result_data.empty()) {

      m_user_cert_table[id].clear();

      for (auto &each_row : result_data) {

        UserCertRecord buf_record;

        for (int i = 0; i < result_name.size(); ++i) {
          if (result_name[i] == "sn")
            buf_record.sn = each_row[i].get<std::string>();
          else if (result_name[i] == "after")
            buf_record.nvbefore = vs::str2num<uint64_t>(each_row[i].get<std::string>());
          else if (result_name[i] == "before")
            buf_record.nvafter = vs::str2num<uint64_t>(each_row[i].get<std::string>());
          else if (result_name[i] == "cert")
            buf_record.x509 = each_row[i].get<std::string>();

          std::string value = each_row[i].get<std::string>();

          ret_vec.emplace_back(result_name[i], value);
        }

        m_user_cert_table[id].emplace_back(buf_record);
      }

    }

    return ret_vec;
  }

  std::vector<DataAttribute> queryIfUserAttrAndParseData(nlohmann::json &query, const std::string &id){
    auto [result_name,result_data] = queryAndCache(query);

    std::vector<DataAttribute> ret_vec;

    if(!result_name.empty() && !result_data.empty()) {

      UserAttributeRecord buf_record;

      for (int i = 0; i < result_name.size(); ++i) {
        if (result_name[i] == "uid")
          buf_record.uid = id;
        else if (result_name[i] == "register_day")
          buf_record.register_day = result_data[0][i].get<std::string>();
        else if (result_name[i] == "register_code")
          buf_record.register_code = result_data[0][i].get<std::string>();
        else if (result_name[i] == "gender")
          buf_record.gender = static_cast<EnumGender>(vs::str2num<uint8_t>(result_data[0][i].get<std::string>()));
        else if (result_name[i] == "isc_type")
          buf_record.isc_type = result_data[0][i].get<std::string>();
        else if (result_name[i] == "isc_code")
          buf_record.isc_code = result_data[0][i].get<std::string>();
        else if (result_name[i] == "location")
          buf_record.location = result_data[0][i].get<std::string>();
        else if (result_name[i] == "age_limit")
          buf_record.age_limit = vs::str2num<int>(result_data[0][i].get<std::string>());

        std::string value =  result_data[0][i].get<std::string>();
        ret_vec.emplace_back(result_name[i],value);
      }

      m_user_attr_table[id] = buf_record;

    }

    return ret_vec;
  }

  std::vector<DataAttribute> queryIfUserScopeAndParseData(nlohmann::json &query, const std::string &id, const std::string &pid = "", bool get_val = true){
    auto [result_name,result_data] = queryAndCache(query);

    std::vector<DataAttribute> ret_vec;

    if(!result_name.empty() && !result_data.empty()) {

      for (auto &each_row : result_data) {

        UserScopeRecord buf_record;

        for (int i = 0; i < result_name.size(); ++i) {
          if (result_name[i] == "var_name")
            buf_record.var_name = each_row[i].get<std::string>();
          else if (result_name[i] == "var_value")
            buf_record.var_value = each_row[i].get<std::string>();
          else if (result_name[i] == "var_type")
            buf_record.var_type = static_cast<EnumAll>(vs::str2num<uint8_t>(each_row[i].get<std::string>()));
          else if (result_name[i] == "var_owner")
            buf_record.var_owner = each_row[i].get<std::string>();
          else if (result_name[i] == "up_time")
            buf_record.up_time = vs::str2num<uint64_t>(each_row[i].get<std::string>());
          else if (result_name[i] == "up_block")
            buf_record.up_block = vs::str2num<uint64_t>(each_row[i].get<std::string>());
          else if (result_name[i] == "tag")
            buf_record.tag = each_row[i].get<std::string>();
          else if (result_name[i] == "pid")
            buf_record.pid = each_row[i].get<std::string>();
        }

        if (buf_record.var_name.empty())
          continue;

        if(get_val) {
          if (pid.empty()) {
            if (buf_record.tag.empty())
              ret_vec.emplace_back(buf_record.var_name, buf_record.var_value);
          } else {
            if (buf_record.pid == pid)
              ret_vec.emplace_back(buf_record.var_name, buf_record.var_value);
          }
        }

        // update cache

        std::string scope_key = id + buf_record.var_name;

        bool is_new = true;

        for(auto &user_scope_record : m_user_scope_table[scope_key]) {
          if(user_scope_record.pid == buf_record.pid){
            user_scope_record.var_value = buf_record.var_value;
            is_new = false;
            break;
          }
        }

        if(is_new)
          m_user_scope_table[scope_key].emplace_back(buf_record);
      }
    }

    return ret_vec;
  }

  std::vector<DataAttribute> queryIfContractScopeAndParseData(nlohmann::json &query, const std::string &cid){

    auto [result_name,result_data] = queryAndCache(query);

    std::vector<DataAttribute> ret_vec;

    if(!result_name.empty() && !result_data.empty()) {

      for (auto &each_row : result_data) {

        ContractScopeRecord buf_record;

        for (int i = 0; i < result_name.size(); ++i) {
          if (result_name[i] == "var_name")
            buf_record.var_name = each_row[i].get<std::string>();
          else if (result_name[i] == "var_value")
            buf_record.var_value = each_row[i].get<std::string>();
          else if (result_name[i] == "var_type")
            buf_record.var_type = static_cast<EnumAll>(vs::str2num<uint8_t>(each_row[i].get<std::string>()));
          else if (result_name[i] == "contract_id")
            buf_record.contract_id = each_row[i].get<std::string>();
          else if (result_name[i] == "up_time")
            buf_record.up_time = vs::str2num<uint64_t>(each_row[i].get<std::string>());
          else if (result_name[i] == "up_block")
            buf_record.up_block = vs::str2num<uint64_t>(each_row[i].get<std::string>());
          else if (result_name[i] == "pid")
            buf_record.pid = each_row[i].get<std::string>();
        }

        if (buf_record.var_name.empty())
          continue;

        std::string scope_key = cid + buf_record.var_name;
        m_contract_scope_table[scope_key] = buf_record; // insert or update

        ret_vec.emplace_back(buf_record.var_name, buf_record.var_value);
      }
    }

    return ret_vec;
  }

  std::pair<std::vector<std::string>,nlohmann::json> queryAndCache(nlohmann::json &query) {
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
      return {};

    std::vector<std::string> data_name;
    for (auto &each_name : query_result_name.value())
      data_name.push_back(each_name.get<std::string>());

    return {data_name, query_result_data.value()};
  }

};

}

#endif