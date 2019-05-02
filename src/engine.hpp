#ifndef GRUUTSCE_ENGINE_HPP
#define GRUUTSCE_ENGINE_HPP

#include "config.hpp"
#include "data/data_storage.hpp"
#include "runner/contract_manager.hpp"
#include "runner/contract_runner.hpp"
#include "runner/tx_parallelizer.hpp"
#include "runner/query_composer.hpp"
#include "chain/transaction.hpp"


namespace gruut::gsce {


class Block;

class Engine {
private:

  ContractManager m_contract_manager;
  QueryComposer m_query_composer;
  DataStorage m_data_stroage;

public:
  Engine() = default;

  // TODO : change argument from json to Block object

  nlohmann::json procBlock(Block &block) {


    // TODO : Block to Transaction object;

    Transaction tx;

/*
    std::string tx_time = "1555484718";
    std::string tx_seed = "zmHvId0vfgmux7tD0IoM/Q=="; // 128-bit
    std::string tx_body_cid = "VALUE-TRANSFER::82nknGvtyt8jxnkfgWSdGh6PJGhEj7pFGKKiuRov4nNm::TSTCHAIN::GRUUTNET";
    std::string tx_body_receiver = "J2xQ5uTkVDoENSgQAaeGzyibJjMe8AT9XdaoqbM9k6C6";
    std::string tx_body_fee_author = "10";
    std::string tx_body_fee_user = "100";
    std::vector<std::string> tx_body_input = {
        "1000", // amount
        "KRW", // unit
        "", // pid
        "" // condition
    };
    std::string tx_user = "82nknGvtyt8jxnkfgWSdGh6PJGhEj7pFGKKiuRov4nNm";
    std::vector<std::string> tx_endorser = {"DavwZ2hoFR68mrvR1CS8Eag3ZEurvPjbyUMLxrbkZA7F","CZec91E32i9NhAWnUpkWsD2dsEc8fJubKWSJnCaPWUjN"};
*/

    std::vector<nlohmann::json> result_queries;

    nlohmann::json result_fail = R"(
      "txid":"",
      "status":false,
      "info":""
    )"_json;

    auto contract = m_contract_manager.getContract(tx.getTxid());

    if(contract) {
      ContractRunner contract_runner;
      contract_runner.setContract(contract.value());
      contract_runner.setTransaction(tx);
      auto res_query = contract_runner.run();
      if(res_query.has_value())
        result_queries.emplace_back(res_query.value());
      else {
        result_fail["txid"] = tx.getTxid();
        result_fail["info"] = GSCE_ERROR_MSG["RUN_UNKNOWN"];
        result_queries.emplace_back(result_fail);
      }

    } else {
      result_fail["txid"] = tx.getTxid();
      result_fail["info"] = GSCE_ERROR_MSG["NO_CONTRACT"];
      result_queries.emplace_back(result_fail);
    }

    return m_query_composer.compose(result_queries);

  }


};

}

#endif //GRUUTSCE_ENGINE_HPP
