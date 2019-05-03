#ifndef GRUUTSCE_SET_HANDLER_HPP
#define GRUUTSCE_SET_HANDLER_HPP

#include "../data/datamap.hpp"
#include "../data/data_storage.hpp"
#include "../condition/condition_manager.hpp"
#include <algorithm>
#include <optional>
#include <regex>
#include <cctype>
#include <unordered_map>

namespace gruut::gsce {

const auto REGEX_BASE64 = "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?$";

enum class SetType : int{
  USER_JOIN,
  USER_CERT,
  V_CREATE,
  V_INCINERATE,
  V_TRANSFER,
  SCOPE_USER,
  SCOPE_CONTRACT,
  CONTRACT_NEW,
  CONTRACT_DISABLE,
  ITEM_TRADE,
  V_TRADE,
  RUN_QUERY,
  RUN_CONTRACT,
  NONE
};

const std::unordered_map<std::string, SetType> SetTypeMap {
  {"user.cert", SetType::USER_JOIN}, {"user.cert", SetType::USER_CERT},
      {"v.create", SetType::V_CREATE}, {"v.incinerate", SetType::V_INCINERATE},
      {"v.transfer", SetType::V_TRANSFER}, {"scope.user", SetType::SCOPE_USER},
      {"scope.contract", SetType::SCOPE_CONTRACT}, {"contract.new", SetType::CONTRACT_NEW},
      {"contract.disable", SetType::CONTRACT_DISABLE},
      {"item.trade", SetType::ITEM_TRADE}, {"v.trande", SetType::V_TRADE},
      {"run.query", SetType::RUN_QUERY}, {"run.contract", SetType::RUN_CONTRACT},
};

class SetHandler {
public:
  SetHandler() = default;

  std::optional<nlohmann::json> parseSet(std::vector<std::pair<pugi::xml_node,std::string>> &set_nodes, ConditionManager &condition_manager, DataStorage &data_collector){
    nlohmann::json query = nlohmann::json::array();
    for(auto &[set_node, id] : set_nodes){
      if(set_node.empty() || condition_manager.getEvalResultById(id))
        continue;
      std::string type_str = set_node.attribute("type").value();
      auto it = SetTypeMap.find(type_str);
      auto set_type = (it == SetTypeMap.end() ? SetType::NONE : it->second);
      optional<nlohmann::json> contents = handle(set_type, set_node, data_collector);
      if(!contents.has_value())
        continue;
      query.emplace_back(contents);
    }
    if(query.empty())
      return {};
    return query;
  }
private:
  std::optional<nlohmann::json> handle(SetType set_type, pugi::xml_node &set_node, DataStorage &data_collector) {
    nlohmann::json contents;
    auto option_nodes = set_node.select_nodes("/option");
    switch (set_type) {
    case SetType::USER_JOIN:{
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        if (option_name == "gender") {
          if (data.empty() || (data != "MALE" && data!="FEMALE" && data != "OTHER"))
            return {};
        }
        else if(option_name == "regi_day") {
          if(!data.empty()) {
            if(!std::all_of(data.begin(), data.end(), ::isdigit))
              return {};
          }
        }
        contents[option_name] = data;
      }
      if(contents["gender"] == "other"){
        auto regi_code = json::get<std::string>(contents, "regi_code");
        if(!regi_code.has_value())
          return {};
        if(regi_code.value().empty())
          return {};
      }
      break;
    }
    case SetType::USER_CERT:{
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        if (option_name == "notbefore" || option_name == "notafter") {
          if (data.empty())
            return {};
          if(!std::all_of(data.begin(), data.end(), ::isdigit))
            return {};
        }
        else if(option_name == "x509") {
          string begin_str = "-----BEGIN CERTIFICATE-----";
          string end_str = "-----END CERTIFICATE-----";
          auto found1 = data.find(begin_str);
          auto found2 = data.find(end_str);
          if(found1 == string::npos || found2 ==string::npos) {
            return {};
          }
          auto content_len = data.length() - begin_str.length() - end_str.length();
          auto content = data.substr(begin_str.length(), content_len);
          std::regex rgx(REGEX_BASE64);
          if(!std::regex_match(data, rgx))
            return {};
        }
        contents[option_name] = data;
      }
      break;
    }
    case SetType::V_CREATE: {
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        if (option_name == "amount") {
          if (data.empty() || data[0] == '0' || data[0] == '-' || data.length() > 16)
            return {};
        }
        else if(option_name == "type") {
          if (data.empty() || ( data != "GRU" && data != "FIAT" && data != "COIN" && data!= "XCOIN" && data != "MILE"))
            return {};
        }
        contents[option_name] = data;
      }
      break;
    }
    case SetType::V_INCINERATE: {
      auto option_node = option_nodes.first().node();
      std::string option_name = option_node.attribute("name").value();
      std::string option_value = option_node.attribute("value").value();
      std::string data = data_collector.eval(option_value);
      if(option_name != "pid" || data.empty())
        return {};
      std::regex rgx(REGEX_BASE64);
      if(!std::regex_match(data, rgx))
        return {};
      contents[option_name] = data;
      break;
    }
    case SetType::SCOPE_USER:{
      std::string for_att = set_node.attribute("for").value();
      if(for_att.empty() || ( for_att != "user" && for_att != "author"))
        return {};
      contents["for"] = for_att;
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        //TODO : need something for checking tag, pid condition.
        contents[option_name] = data;
      }
      break;
    }
    case SetType::SCOPE_CONTRACT:{
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        contents[option_name] = data;
        //TODO : check 'cid'
      }
      auto pid = json::get<std::string>(contents, "pid");
      if(!pid.has_value())
        return {};
      std::regex rgx(REGEX_BASE64);
      if(!pid.value().empty()){
        if(!std::regex_match(pid.value(), rgx))
          return {};
      }
      break;
    }
    case SetType::CONTRACT_NEW: {
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        //TODO : check 'cid'
        if (option_name == "before" || option_name == "after") {
          if (data.empty())
            return {};
          if(!std::all_of(data.begin(), data.end(), ::isdigit))
            return {};
        }
        contents[option_name] = data;
      }
      break;
    }
    case SetType::CONTRACT_DISABLE: {
      auto option_node = option_nodes.first().node();
      std::string option_name = option_node.attribute("name").value();
      std::string option_value = option_node.attribute("value").value();
      std::string data = data_collector.eval(option_value);
      if(option_name != "cid")
        return {};
      contents["cid"] = "";
      break;
    }
    case SetType::V_TRANSFER:{
      std::string from_att = set_node.attribute("from").value();
      if(from_att.empty() || ( from_att != "user" && from_att != "author" && from_att != "contract"))
        return {};
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        //TODO : need something for checking tag, pid condition
        contents[option_name] = data;
      }
      break;
    }
    case SetType::RUN_QUERY: {
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        if(option_name == "type"){
          if(data == "run.query" || data == "user.cert")
            return {};
        }
        else if(option_name == "after"){
          if(data.empty())
            return {};
          if(!std::all_of(data.begin(), data.end(), ::isdigit))
            return {};
        }
        contents[option_name] = data;
      }
      break;
    }
    case SetType::RUN_CONTRACT: {
      for (auto &each_node : option_nodes) {
        auto option_node = each_node.node();
        std::string option_name = option_node.attribute("name").value();
        std::string option_value = option_node.attribute("value").value();
        std::string data = data_collector.eval(option_value);
        //TODO : check 'cid'
        if(option_name == "after"){
          if(data.empty())
            return {};
          if(!std::all_of(data.begin(), data.end(), ::isdigit))
            return {};
        }
        contents[option_name] = data;
      }
      break;
    }
    default:
      return {};
    }
    return contents;
  }
};

}

#endif //GRUUTSCE_SET_HANDLER_HPP
