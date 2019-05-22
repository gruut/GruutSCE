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

public:
  Engine() = default;

  // TODO : change argument from json to Block object

  nlohmann::json procBlock(Block &block) {

    // TODO : attach real interface

    std::function<nlohmann::json(nlohmann::json&)> read_storage_interface = [&](nlohmann::json& query){
      nlohmann::json result;
      return result;
    };


    // TODO : replace real information
    uint64_t block_hgt = 1;
    std::string block_id;

    std::vector<nlohmann::json> result_queries;

    ContractRunner contract_runner;

    if(!contract_runner.setWorldChain()){
      return m_query_composer.compose(result_queries, block_id, block_hgt);
    }

    contract_runner.attachReadInterface(read_storage_interface);

    for(int i = 0; i < 10; ++i){ // TODO : change block to each each_tx

      Transaction each_tx;

      nlohmann::json result_fail;
      result_fail["status"] = false;
      result_fail["txid"] = each_tx.getTxid();

      auto contract = m_contract_manager.getContract(each_tx.getTxid());

      if (contract.has_value()) {

        contract_runner.setContract(contract.value());
        contract_runner.setTransaction(each_tx);

        if (!contract_runner.readUserAttributes()) {
          result_fail["info"] = VSCE_ERROR_MSG["NO_USER"];
          result_queries.emplace_back(result_fail);
        }

        auto res_query = contract_runner.run();

        if (res_query.has_value()) {
          if (contract_runner.update(res_query.value())) {
            result_queries.emplace_back(res_query.value());
          } else {
            result_fail["info"] = VSCE_ERROR_MSG["INVALID_UPDATE_LV1"];
            result_queries.emplace_back(result_fail);
          }
        } else {
          result_fail["info"] = VSCE_ERROR_MSG["RUN_UNKNOWN"];
          result_queries.emplace_back(result_fail);
        }

        contract_runner.clear();

      } else {
        result_fail["info"] = VSCE_ERROR_MSG["NO_CONTRACT"];
        result_queries.emplace_back(result_fail);
      }

    }

    return m_query_composer.compose(result_queries, block_id, block_hgt);
  }


};

}

#endif //GRUUTSCE_ENGINE_HPP
