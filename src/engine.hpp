#ifndef TETHYS_SCE_ENGINE_HPP
#define TETHYS_SCE_ENGINE_HPP

#include "config.hpp"
#include "data/data_manager.hpp"
#include "runner/contract_manager.hpp"
#include "runner/contract_runner.hpp"
#include "runner/tx_parallelizer.hpp"
#include "runner/query_composer.hpp"
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

  std::optional<nlohmann::json> procBlock(nlohmann::json &block) {

    if(m_storage_interface == nullptr)
      return std::nullopt;

    uint64_t block_hgt = tt::str2num<uint64_t>(json::get<std::string>(block["block"],"height").value_or(""));
    std::string block_id = json::get<std::string>(block["block"],"id").value_or("");

    std::vector<nlohmann::json> result_queries;

    ContractRunner contract_runner;
    contract_runner.attachReadInterface(m_storage_interface);

    if(!contract_runner.setWorldChain())
      return std::nullopt;

    if(!block["tx"].is_array())
      return std::nullopt;

    for (auto &each_tx_cbor : block["tx"]) {

      nlohmann::json each_tx;

      try {
        each_tx = nlohmann::json::from_cbor(TypeConverter::decodeBase<64>(each_tx_cbor.get<std::string>()));
      }
      catch (...) {
        continue;
      }

      auto txid = json::get<std::string>(each_tx, "txid");
      auto cid = json::get<std::string>(each_tx["body"], "cid");

      if (!txid || !cid)
        continue;

      nlohmann::json result_fail;
      result_fail["status"] = false;
      result_fail["txid"] = txid.value();

      std::cout << "txid = " << txid.value() << std::endl;
      std::cout << "cid = " << cid.value() << std::endl;

      auto contract = m_contract_manager.getContract(cid.value());

      if (contract.has_value()) {

        if(!contract_runner.setContract(contract.value())) {
          std::cout << "failed to setContract()" << std::endl;
          continue;
        }

        std::cout << "finished setContract()" << std::endl;

        contract_runner.setTransaction(each_tx);

        std::cout << "finished setTransaction()" << std::endl;

        if (!contract_runner.readUserAttributes()) {
          result_fail["info"] = TSCE_ERROR_MSG["NO_USER"];
          result_queries.emplace_back(result_fail);
        }

        auto res_query = contract_runner.run();

        if (res_query.has_value()) {
          result_queries.emplace_back(res_query.value());
        } else {
          result_fail["info"] = TSCE_ERROR_MSG["RUN_UNKNOWN"];
          result_queries.emplace_back(result_fail);
        }

        contract_runner.clear();

      } else {
        result_fail["info"] = TSCE_ERROR_MSG["NO_CONTRACT"];
        result_queries.emplace_back(result_fail);
      }

    }

    return m_query_composer.compose(result_queries, block_id, block_hgt);
  }


};

}

#endif //GRUUTSCE_ENGINE_HPP
