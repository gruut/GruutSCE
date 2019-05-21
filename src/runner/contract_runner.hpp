#ifndef VERONN_SCE_CONTRACT_RUNNER_HPP
#define VERONN_SCE_CONTRACT_RUNNER_HPP

#include "../config.hpp"
#include "../data/data_manager.hpp"
#include "../condition/condition_manager.hpp"
#include "../handler/input_handler.hpp"
#include "../handler/get_handler.hpp"
#include "../handler/set_handler.hpp"
#include "../chain/transaction.hpp"
#include "element_parser.hpp"
#include "../handler/fee_handler.hpp"

namespace veronn::vsce {

class ContractRunner {

private:
  pugi::xml_node m_contract_node;
  DataManager m_data_manager;
  ConditionManager m_condition_manager;
  InputHandler m_input_handler;
  GetHandler m_get_handler;
  SetHandler m_set_handler;
  ElementParser m_element_parser;
  TransactionJson m_tx_json;
  FeeHandler m_fee_handler;


public:
  ContractRunner() = default;

  void clear(){
    m_data_manager.clear();
  }

  bool update(nlohmann::json &set_query){
    return true;
  }

  bool setWorldChain() {

    auto world_attr = m_data_manager.getWorld();
    auto chain_attr = m_data_manager.getChain();

    if(world_attr.empty() || chain_attr.empty()){
      return false;
    }

    attrToMap(world_attr, "$world");
    attrToMap(chain_attr, "$chain");

    return true;
  }

  void setContract(pugi::xml_node &contract_node) {
    m_contract_node = contract_node; // for future use
    m_element_parser.setContract(contract_node);
  }

  void setTransaction(Transaction &tx) {
    nlohmann::json tx_agg_json = tx.getJson();
    setTransaction(tx_agg_json);
  }

  bool setTransaction(TransactionJson &tx_agg_json) {

    m_tx_json = tx_agg_json;

    auto time = json::get<std::string>(m_tx_json,"time");
    auto cid = json::get<std::string>(m_tx_json,"cid");
    auto receiver = json::get<std::string>(m_tx_json,"receiver");
    auto fee = json::get<std::string>(m_tx_json,"fee");
    auto user_id = json::get<std::string>(m_tx_json["user"],"id");
    auto txid = json::get<std::string>(m_tx_json,"txid");
    auto world = json::get<std::string>(m_tx_json,"world");
    auto chain = json::get<std::string>(m_tx_json,"chain");

    if(!time || !cid || !receiver || !fee || !user_id || !txid || !world || !chain)
      return false;

    auto user_pk = json::get<std::string>(tx_agg_json["user"],"pk");

    m_data_manager.updateValue("$tx.txid", txid.value());
    m_data_manager.updateValue("$txid", txid.value());
    m_data_manager.updateValue("$tx.world", world.value());
    m_data_manager.updateValue("$tx.chain",chain.value());

    m_data_manager.updateValue("$tx.time", time.value());
    m_data_manager.updateValue("$time", time.value());

    std::vector<std::string> cid_components = vs::split(cid.value(),"::");

    m_data_manager.updateValue("$tx.body.cid", cid.value());
    m_data_manager.updateValue("$cid", cid.value());
    m_data_manager.updateValue("$author", cid_components[1]);
    m_data_manager.updateValue("$chain", cid_components[2]);
    m_data_manager.updateValue("$world", cid_components[3]);
    m_data_manager.updateValue("$tx.body.receiver", receiver.value());
    m_data_manager.updateValue("$receiver", receiver.value());

    m_data_manager.updateValue("$tx.body.fee", fee.value());
    m_data_manager.updateValue("$fee", fee.value());

    m_data_manager.updateValue("$tx.user.id", user_id.value());
    m_data_manager.updateValue("$user", user_id.value());

    m_data_manager.updateValue("$tx.user.pk", user_pk.value());

    nlohmann::json tx_endorsers_json = m_tx_json["body"]["endorser"];

    for(int i = 0 ; i < tx_endorsers_json.size(); ++i){
      std::string id_key = "$tx.endorser[" + to_string(i) + "].id";
      std::string pk_key = "$tx.endorser[" + to_string(i) + "].pk";
      m_data_manager.updateValue(id_key, json::get<std::string>(tx_endorsers_json[i], "id").value_or(""));
      m_data_manager.updateValue(pk_key, json::get<std::string>(tx_endorsers_json[i], "pk").value_or(""));
    }

    m_data_manager.updateValue("$tx.endorser.count", std::to_string(tx_endorsers_json.size()));

    return true;
  }

  bool readUserAttributes(){

    auto user_attr = m_data_manager.getUserInfo("$user");
    auto receiver_attr = m_data_manager.getUserInfo("$receiver");
    auto author_attr = m_data_manager.getUserInfo("$author");

    if(user_attr.empty() || receiver_attr.empty() || author_attr.empty())
      return false;

    attrToMap(user_attr, "$user");
    attrToMap(receiver_attr, "$receiver");
    attrToMap(author_attr, "$author");

    return true;


    /*
    int num_endorser;
    try{
      num_endorser = std::stoi(m_data_manager.eval("$tx.endorser.count"));
    }
    catch(...){
      num_endorser = 0;
    }

    for(int i = 0; i < num_endorser; ++i) {
      std::string prefix = "$tx.endorser[" + to_string(i) + "]";
      auto endorser_attr = m_data_manager.getUserInfo(prefix + ".id");
      attrToMap(endorser_attr, prefix);
    }
    */
  }

  std::optional<nlohmann::json> run() {

    nlohmann::json result_query = R"(
      "txid":"",
      "status":true,
      "info":"",
      "authority": {
        "author": "",
        "user": "",
        "receiver": "",
        "self": "",
        "friend":[]
      },
      fee: {
        "author": "",
        "user": "",
      },
      "queries": [])"_json;

    auto& data_map = m_data_manager.getDatamap();

    result_query["txid"] = m_data_manager.eval("$tx.txid");

    auto& head_node = m_element_parser.getNode("head");
    auto& condition_nodes = m_element_parser.getNodes("condition");


    // check if contract is runnable

    if(!head_node.second.empty()) {

      std::string condition_id = (head_node.second[0] == '~') ? head_node.second.substr(1) : head_node.second;

      for (auto &each_condition : condition_nodes) {
        std::string each_id = each_condition.first.attribute("id").value();
        if(each_id == condition_id){
          m_condition_manager.evalue(each_condition.first,data_map);
          break;
        }
      }

      if (!m_condition_manager.getEvalResultById(head_node.second)) {
        result_query["status"] = false;
        result_query["info"] = VSCE_ERROR_MSG["RUN_CONDITION"];
        return result_query;
      }
    }

    TimeHandler contract_time_handler;
    if(!contract_time_handler.evalue(head_node.first,data_map)) {
      result_query["status"] = false;
      result_query["info"] = VSCE_ERROR_MSG["RUN_PERIOD"];
      return result_query;
    }

    // OK ready to go

    for(auto &each_condition : condition_nodes) {
      m_condition_manager.evalue(each_condition.first,data_map);
    }
    result_query["authority"]["author"] = m_data_manager.eval("$author");
    result_query["authority"]["user"] = m_data_manager.eval("$user");
    result_query["authority"]["receiver"] = m_data_manager.eval("$receiver");
    result_query["authority"]["self"] = m_data_manager.eval("$tx.body.cid");

    // TODO : result_query["authority"]["friend"]

    // process input directive

    auto& input_node = m_element_parser.getNode("input");
    if(!m_input_handler.parseInput(m_tx_json,input_node.first,m_data_manager)){
      result_query["status"] = false;
      result_query["info"] = VSCE_ERROR_MSG["RUN_INPUT"];
      return result_query;
    }

    // process get directive

    for(auto &each_condition : condition_nodes) {
      m_condition_manager.evalue(each_condition.first,data_map);
    }

    auto& get_nodes = m_element_parser.getNodes("get");
    m_get_handler.parseGet(get_nodes,m_condition_manager,m_data_manager);
    
    // TODO : oracle handler (pending)
    // TODO : script handler (pending)

    // no more change values here
    
    for(auto &each_condition : condition_nodes) {
      m_condition_manager.evalue(each_condition.first,data_map);
    }

    // process fee directive

    auto &fee_nodes = m_element_parser.getNodes("fee");
    auto [pay_from_user, pay_from_author] = m_fee_handler.parseGet(fee_nodes,m_condition_manager, m_data_manager);

    if(pay_from_user > 0 && m_data_manager.getUserKeyCurrency("$user") < pay_from_user) {
      result_query["status"] = false;
      result_query["info"] = VSCE_ERROR_MSG["NOT_ENOUGH_FEE"] + " (user)";
      return result_query;
    }

    if(pay_from_author > 0 && m_data_manager.getUserKeyCurrency("$author") < pay_from_author) {
      result_query["status"] = false;
      result_query["info"] = VSCE_ERROR_MSG["NOT_ENOUGH_FEE"] + " (author)";
      return result_query;
    }

    result_query["fee"]["user"] = std::to_string(pay_from_user);
    result_query["fee"]["author"] = std::to_string(pay_from_author);

    // process set directive

    auto& set_nodes = m_element_parser.getNodes("set");
    auto set_query = m_set_handler.parseSet(set_nodes, m_condition_manager, m_data_manager);

    if(set_query.empty())
      return std::nullopt;

    // TODO : set result validation

    nlohmann::json valid_set_query = nlohmann::json::array();

    for(auto &each_set_query : set_query) {

      std::string set_type_str = json::get<std::string>(each_set_query,"type").value_or("");

      if(set_type_str.empty())
        continue;

      auto it = SET_TYPE_MAP.find(set_type_str);
      auto set_type = (it == SET_TYPE_MAP.end() ? SetType::NONE : it->second);

      if(set_type == SetType::NONE)
        continue;



      valid_set_query.emplace_back(each_set_query);
    }


    result_query["query"] = valid_set_query;

    return result_query;
  }


private:

  template <typename S = std::string>
  void attrToMap(std::vector<DataAttribute> &attr_list, S &&prefix){
    if(attr_list.empty())
      return;

    for(auto &each_attr : attr_list) {
      std::string key = prefix;
      key.append(".").append(each_attr.name);
      m_data_manager.updateValue(key, each_attr.value);
    }

  }


};


}

#endif
