#ifndef GRUUTSCE_ENGINE_HPP
#define GRUUTSCE_ENGINE_HPP

#include "config.hpp"
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
  DataCollector m_data_collector;

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

    pugi::xml_node contract = m_contract_manager.getContract(tx.getTxid());

    std::function<DataRecord(std::string&)> read_interface;
    read_interface = [this, &read_interface](std::string &key){
      return m_data_collector.getVal(key);
    };

    std::function<void(std::string&, DataRecord &)> write_interface;
    write_interface = [this, &write_interface](std::string &key, DataRecord &value){
      m_data_collector.setVal(key,value);
    };

    std::vector<nlohmann::json> result_queries;

    ContractRunner contract_runner;
    contract_runner.attachReadInterface(read_interface);
    contract_runner.attachWriteInterface(write_interface);
    contract_runner.setContract(contract);
    contract_runner.setTransaction(tx);
    result_queries.emplace_back(contract_runner.run());

    // TODO : generating result json

  }


};

}

#endif //GRUUTSCE_ENGINE_HPP
