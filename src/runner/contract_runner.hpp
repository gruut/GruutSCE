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
  DataManager m_tx_data_storage;
  ConditionManager m_condition_manager;
  InputHandler m_input_handler;
  GetHandler m_get_handler;
  SetHandler m_set_handler;
  ElementParser m_element_parser;
  TransactionJson m_tx_json;
  FeeHandler m_fee_handler;


public:
  ContractRunner() = default;

  bool setWorldChain() {

    auto world_attr = m_tx_data_storage.getWorld();
    auto chain_attr = m_tx_data_storage.getChain();

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

    m_tx_data_storage.updateValue("$tx.txid", txid.value());
    m_tx_data_storage.updateValue("$tx.world", world.value());
    m_tx_data_storage.updateValue("$tx.chain",chain.value());

    m_tx_data_storage.updateValue("$tx.time", time.value());
    m_tx_data_storage.updateValue("$time", time.value());

    std::vector<std::string> cid_components = vs::split(cid.value(),"::");

    m_tx_data_storage.updateValue("$tx.body.cid", cid.value());
    m_tx_data_storage.updateValue("$author", cid_components[1]);
    m_tx_data_storage.updateValue("$chain", cid_components[2]);
    m_tx_data_storage.updateValue("$world", cid_components[3]);
    m_tx_data_storage.updateValue("$tx.body.receiver", receiver.value());
    m_tx_data_storage.updateValue("$receiver", receiver.value());

    m_tx_data_storage.updateValue("$tx.body.fee", fee.value());
    m_tx_data_storage.updateValue("$fee", fee.value());

    m_tx_data_storage.updateValue("$tx.user.id", user_id.value());
    m_tx_data_storage.updateValue("$user", user_id.value());

    m_tx_data_storage.updateValue("$tx.user.pk", user_pk.value());

    nlohmann::json tx_endorsers_json = m_tx_json["body"]["endorser"];

    for(int i = 0 ; i < tx_endorsers_json.size(); ++i){
      std::string id_key = "$tx.endorser[" + to_string(i) + "].id";
      std::string pk_key = "$tx.endorser[" + to_string(i) + "].pk";
      m_tx_data_storage.updateValue(id_key, json::get<std::string>(tx_endorsers_json[i], "id").value_or(""));
      m_tx_data_storage.updateValue(pk_key, json::get<std::string>(tx_endorsers_json[i], "pk").value_or(""));
    }

    m_tx_data_storage.updateValue("$tx.endorser.count", std::to_string(tx_endorsers_json.size()));

    return true;
  }

  void readUserAttributes(){

    auto user_attr = m_tx_data_storage.getUserInfo("$user");
    auto receiver_attr = m_tx_data_storage.getUserInfo("$receiver");
    auto author_attr = m_tx_data_storage.getUserInfo("$author");

    attrToMap(user_attr, "$user");
    attrToMap(receiver_attr, "$receiver");
    attrToMap(author_attr, "$author");

    /*
    int num_endorser;
    try{
      num_endorser = std::stoi(m_tx_data_storage.eval("$tx.endorser.count"));
    }
    catch(...){
      num_endorser = 0;
    }

    for(int i = 0; i < num_endorser; ++i) {
      std::string prefix = "$tx.endorser[" + to_string(i) + "]";
      auto endorser_attr = m_tx_data_storage.getUserInfo(prefix + ".id");
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

    auto& data_map = m_tx_data_storage.getDatamap();

    result_query["txid"] = m_tx_data_storage.eval("$tx.txid");

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
        result_query["info"] = GSCE_ERROR_MSG["RUN_CONDITION"];
        return result_query;
      }
    }

    TimeHandler contract_time_handler;
    if(!contract_time_handler.evalue(head_node.first,data_map)) {
      result_query["status"] = false;
      result_query["info"] = GSCE_ERROR_MSG["RUN_PERIOD"];
      return result_query;
    }

    // OK ready to go

    for(auto &each_condition : condition_nodes) {
      m_condition_manager.evalue(each_condition.first,data_map);
    }
    result_query["authority"]["author"] = m_tx_data_storage.eval("$author");
    result_query["authority"]["user"] = m_tx_data_storage.eval("$user");
    result_query["authority"]["receiver"] = m_tx_data_storage.eval("$receiver");
    result_query["authority"]["self"] = m_tx_data_storage.eval("$tx.body.cid");

    // TODO : result_query["authority"]["friend"]

    // process input directive

    auto& input_node = m_element_parser.getNode("input");
    m_input_handler.parseInput(m_tx_json,input_node.first,m_tx_data_storage);

    // process get directive

    for(auto &each_condition : condition_nodes) {
      m_condition_manager.evalue(each_condition.first,data_map);
    }

    auto& get_nodes = m_element_parser.getNodes("get");
    m_get_handler.parseGet(get_nodes,m_condition_manager,m_tx_data_storage);
    
    // TODO : oracle handler (pending)
    // TODO : script handler (pending)
    
    for(auto &each_condition : condition_nodes) {
      m_condition_manager.evalue(each_condition.first,data_map);
    }

    auto& set_nodes = m_element_parser.getNodes("set");
    auto query = m_set_handler.parseSet(set_nodes, m_condition_manager, m_tx_data_storage);

    if(!query.has_value())
      return std::nullopt;

    result_query["query"] = query.value();

    auto &fee_nodes = m_element_parser.getNodes("fee");
    auto [pay_from_user, pay_from_author] = m_fee_handler.parseGet(fee_nodes,m_condition_manager, m_tx_data_storage);

    result_query["fee"]["user"] = std::to_string(pay_from_user);
    result_query["fee"]["author"] = std::to_string(pay_from_author);

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
      m_tx_data_storage.updateValue(key, each_attr.value);
    }

  }


};


}

#endif
