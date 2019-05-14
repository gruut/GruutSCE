#ifndef VERONN_SCE_ENGINE_HPP
#define VERONN_SCE_ENGINE_HPP

#include "config.hpp"
#include "data/data_manager.hpp"
#include "runner/contract_manager.hpp"
#include "runner/contract_runner.hpp"
#include "runner/tx_parallelizer.hpp"
#include "runner/query_composer.hpp"
#include "chain/transaction.hpp"


namespace veronn::vsce {


class Block;

class Engine {
private:

  ContractManager m_contract_manager;
  QueryComposer m_query_composer;
  DataManager m_data_manager;

public:
  Engine() = default;

  // TODO : change argument from json to Block object

  nlohmann::json procBlock(Block &block) {

    // TODO : Block to Transaction object;

    Transaction tx;

    std::vector<nlohmann::json> result_queries;

    nlohmann::json result_fail = R"(
      "txid":"",
      "status":false,
      "info":""
    )"_json;

    auto contract = m_contract_manager.getContract(tx.getTxid());

    if(contract.has_value()) {
      ContractRunner contract_runner;
      if(!contract_runner.setWorldChain()){
        result_fail["txid"] = tx.getTxid();
        result_fail["info"] = GSCE_ERROR_MSG["CONFIG_WORLD"];
        result_queries.emplace_back(result_fail);

      } else {

        contract_runner.setContract(contract.value());
        contract_runner.setTransaction(tx);

        if (!contract_runner.readUserAttributes()) {
          result_fail["txid"] = tx.getTxid();
          result_fail["info"] = GSCE_ERROR_MSG["NO_USER"];
          result_queries.emplace_back(result_fail);
        }

        auto res_query = contract_runner.run();
        if (res_query.has_value())
          result_queries.emplace_back(res_query.value());
        else {
          result_fail["txid"] = tx.getTxid();
          result_fail["info"] = GSCE_ERROR_MSG["RUN_UNKNOWN"];
          result_queries.emplace_back(result_fail);
        }

      }

    } else {
      result_fail["txid"] = tx.getTxid();
      result_fail["info"] = GSCE_ERROR_MSG["NO_CONTRACT"];
      result_queries.emplace_back(result_fail);
    }

    //TODO: get block informations
    uint64_t block_hgt = 1;
    std::string block_id;
    return m_query_composer.compose(result_queries, block_id, block_hgt);
  }


};

}

#endif //GRUUTSCE_ENGINE_HPP
