#ifndef GRUUTSCE_CONTRACT_RUNNER_HPP
#define GRUUTSCE_CONTRACT_RUNNER_HPP

#include "../config.hpp"
#include "../data/data_storage.hpp"
#include "../condition/condition_manager.hpp"
#include "../handler/input_handler.hpp"
#include "../handler/get_handler.hpp"
#include "../handler/set_handler.hpp"
#include "../chain/transaction.hpp"
#include "element_parser.hpp"

namespace gruut::gsce {

using namespace nlohmann;

class ContractRunner {

private:
  pugi::xml_node m_contract_node;
  DataStorage m_tx_data_storage;
  ConditionManager m_condition_manager;
  InputHandler m_input_handler;
  GetHandler m_get_handler;
  SetHandler m_set_handler;
  ElementParser m_element_parser;
  TransactionJson m_tx_json;


public:
  ContractRunner() = default;

  void attachReadInterface(std::function<DataRecord(std::string&)> &interface) {
    m_tx_data_storage.attachReadInterface(interface);
  }

  void attachWriteInterface(std::function<void(std::string&, DataRecord&)> &interface){
    m_tx_data_storage.attachWriteInterface(interface);
  }

  void setContract(pugi::xml_node &contract_node) {
    m_contract_node = contract_node;
    m_element_parser.setContract(contract_node);
  }

  void setTransaction(Transaction &tx) {
    nlohmann::json tx_agg_json = tx.getJson();
    setTransaction(tx_agg_json);
  }

  void setTransaction(TransactionJson &tx_agg_json) {

    m_tx_json = tx_agg_json;

    m_tx_data_storage.updateValue("$tx.txid", tx_agg_json["txid"].get<std::string>());
    m_tx_data_storage.updateValue("$tx.world", tx_agg_json["world"].get<std::string>());
    m_tx_data_storage.updateValue("$tx.chain", tx_agg_json["chain"].get<std::string>());
    m_tx_data_storage.updateValue("$tx.time", tx_agg_json["time"].get<std::string>());
    m_tx_data_storage.updateValue("$time", tx_agg_json["time"].get<std::string>());

    auto cid = tx_agg_json["cid"].get<std::string>();
    std::vector<std::string> cid_components = vs::split(cid,"::");

    m_tx_data_storage.updateValue("$tx.body.cid", cid);
    m_tx_data_storage.updateValue("$author", cid_components[1]);
    m_tx_data_storage.updateValue("$chain", cid_components[2]);
    m_tx_data_storage.updateValue("$world", cid_components[3]);

    auto receiver = tx_agg_json["receiver"].get<std::string>();

    m_tx_data_storage.updateValue("$tx.body.receiver", receiver);
    m_tx_data_storage.updateValue("$receiver", receiver);

    auto fee = tx_agg_json["fee"].get<std::string>();

    m_tx_data_storage.updateValue("$tx.body.fee", fee);
    m_tx_data_storage.updateValue("$fee", fee);

    auto user_id = tx_agg_json["user"]["id"].get<std::string>();

    m_tx_data_storage.updateValue("$tx.user.id", user_id);
    m_tx_data_storage.updateValue("$tx.user.pk", tx_agg_json["user"]["pk"].get<std::string>());
    m_tx_data_storage.updateValue("$user", user_id);

    nlohmann::json tx_endorsers_json = tx_agg_json["body"]["endorser"];

    for(int i = 0 ; i < tx_endorsers_json.size(); ++i){
      std::string id_key = "$tx.endorser[" + to_string(i) + "].id";
      std::string pk_key = "$tx.endorser[" + to_string(i) + "].pk";
      m_tx_data_storage.updateValue(id_key, tx_endorsers_json[i]["id"].get<std::string>());
      m_tx_data_storage.updateValue(pk_key, tx_endorsers_json[i]["pk"].get<std::string>());
    }
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

    result_query["txid"] = data_map.get("$tx.txid");
    result_query["fee"]["user"] = data_map.get("$fee");

    auto& head_node = m_element_parser.getNode("head");
    auto& condition_nodes = m_element_parser.getNodes("condition");


    // check if contract is runnable

    if(!head_node.second.empty()) {

      std::string condition_id = (head_node.second[0] == '~') ? head_node.second.substr(1) : head_node.second;

      for (auto &each_condition : condition_nodes) {
        std::string each_id = each_condition.first.attribute("id").value();
        if(each_id == condition_id){
          m_condition_manager.evalue(each_condition.first,data_map,true);
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

    result_query["authority"]["author"] = data_map.get("$author");
    result_query["authority"]["user"] = data_map.get("$user");
    result_query["authority"]["receiver"] = data_map.get("$receiver");
    result_query["authority"]["self"] = data_map.get("$tx.body.cid");

    auto& input_node = m_element_parser.getNode("input");
    m_input_handler.parseInput(m_tx_json,input_node.first,m_tx_data_storage);

    auto& get_nodes = m_element_parser.getNodes("get");
    m_get_handler.parseGet(get_nodes,m_condition_manager,m_tx_data_storage);

    auto& set_nodes = m_element_parser.getNodes("set");
    auto query = m_set_handler.parseSet(set_nodes, m_condition_manager, m_tx_data_storage);

    if(!query.has_value())
      return {};
    result_query["query"] = query.value();
    return result_query;
  }


private:


};


}

#endif //GRUUTSCE_CONTRACT_RUNNER_HPP
