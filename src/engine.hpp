#ifndef TETHYS_SCE_ENGINE_HPP
#define TETHYS_SCE_ENGINE_HPP

#include "config.hpp"
#include "data/data_manager.hpp"
#include "runner/contract_manager.hpp"
#include "runner/contract_runner.hpp"
#include "runner/tx_parallelizer.hpp"
#include "runner/query_composer.hpp"
#include "chain/block.hpp"
#include "chain/transaction.hpp"


namespace tethys::tsce {


class ContractEngine {
private:
  ContractManager m_contract_manager;
  QueryComposer m_query_composer;
  std::function<nlohmann::json(nlohmann::json&)> m_storage_interface;

public:
  ContractEngine() = default;

  void attachReadInterface(std::function<nlohmann::json(nlohmann::json&)> read_storage_interface){
    m_storage_interface = read_storage_interface;
    m_contract_manager.attachReadInterface(read_storage_interface);
  }

  // TODO : change argument from json to Block object

  std::optional<nlohmann::json> procBlock(Block &block) {

    if(m_storage_interface == nullptr)
      return std::nullopt;

    //uint64_t block_hgt = mt::str2num<uint64_t>(JsonTool::get<std::string>(block["block"],"height").value_or(""));
    //std::string block_id = JsonTool::get<std::string>(block["block"],"id").value_or("");

    if(block.getNumTransaction() <= 0) {
      return std::nullopt;
    }

    block_height_type block_hgt = block.getHeight();
    base58_type  block_id = block.getBlockId();

    std::vector<nlohmann::json> result_queries;

    ContractRunner contract_runner;
    contract_runner.attachReadInterface(m_storage_interface);

    if(!contract_runner.setWorldChain())
      return std::nullopt;

//    if(!block["tx"].is_array())
//      return std::nullopt;

    //for (auto &each_tx_cbor : block["tx"]) {
    for(auto &each_tx : block.getTransactions()) {

      contract_runner.clear();

      //auto txid = JsonTool::get<std::string>(each_tx, "txid");
      //auto cid = JsonTool::get<std::string>(each_tx["body"], "cid");
      auto txid = each_tx.getTxId();
      auto cid = each_tx.getContractId();

      std::string error;

      nlohmann::json result_fail;
      result_fail["status"] = false;
      result_fail["txid"] = txid;

      if (txid.empty() || cid.empty()) {
        result_fail["info"] = TSCE_ERROR_MSG["INVALID_TX"];
        result_queries.emplace_back(result_fail);
        continue;
      }

      auto contract = m_contract_manager.getContract(cid);

      if (!contract.has_value()) {
        result_fail["info"] = TSCE_ERROR_MSG["NO_CONTRACT"];
        result_queries.emplace_back(result_fail);
        continue;
      }

      if(!contract_runner.setContract(contract.value())) {
        result_fail["info"] = TSCE_ERROR_MSG["NO_CONTRACT"];
        result_queries.emplace_back(result_fail);
        continue;
      }

      if(!contract_runner.setTransaction(each_tx, error)) {
        result_fail["info"] = TSCE_ERROR_MSG["INVALID_TX"] + " (" + error + ")";
        result_queries.emplace_back(result_fail);
        continue;
      }

      if (!contract_runner.readUserAttributes()) {
        result_fail["info"] = TSCE_ERROR_MSG["NO_USER"];
        result_queries.emplace_back(result_fail);
        continue;
      }

      auto res_query = contract_runner.run();
      result_queries.emplace_back(res_query);

    }

    return m_query_composer.compose(result_queries, block_id, block_hgt);
  }


};

}

#endif //GRUUTSCE_ENGINE_HPP
