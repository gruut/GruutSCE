#ifndef GRUUT_PUBLIC_MERGER_TRANSACTION_HPP
#define GRUUT_PUBLIC_MERGER_TRANSACTION_HPP

#include "../config.hpp"
#include "endorser.hpp"

#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace gruut {

class Transaction {
private:
  base58_type m_txid;
  timestamp_t m_tx_time;
  base64_type m_seed;

  std::string m_contract_id;
  base58_type m_receiver_id;
  int m_fee;
  std::string m_tx_input_cbor; // to_cbor 된 상태

  base58_type m_tx_user_id;
  std::string m_tx_user_pk;
  base64_type m_tx_user_sig;

  std::vector<Endorser> m_tx_endorsers;

  base64_type m_tx_agg_cbor;
  base58_type m_block_id;
  int m_tx_pos; // static_merkle_tree에서의 idx
  base64_type m_tx_output;
  nlohmann::json m_tx_json;

public:
  bool setJson(nlohmann::json &tx_json) {

    m_tx_json = tx_json;

    m_txid = json::get<string>(tx_json, "txid").value();
    m_tx_time = static_cast<gruut::timestamp_t>(stoll(json::get<string>(tx_json, "time").value()));
    m_seed = json::get<string>(tx_json, "txid").value();

    m_contract_id = json::get<string>(tx_json["body"], "cid").value(); // 현재 cid 관련 내용은 message에 없음
    m_receiver_id = json::get<string>(tx_json["body"], "receiver").value();
    m_fee = stoi(json::get<string>(tx_json["body"], "fee").value());
    setTxInputCbor(tx_json["body"]["input"]);

    m_tx_user_id = json::get<string>(tx_json["user"], "id").value();
    m_tx_user_pk = json::get<string>(tx_json["user"], "pk").value();
    m_tx_user_sig = json::get<string>(tx_json["user"], "agga").value();

    setEndorsers(tx_json["endorser"]);

    return true;
  }

  nlohmann::json getJson(){
    return m_tx_json;
  }

  void setTxInputCbor(nlohmann::json &input_array) {
    m_tx_input_cbor = TypeConverter::encodeBase<64>(nlohmann::json::to_cbor(input_array));
  }

  bool setEndorsers(nlohmann::json &endorser_array) {
    if (!endorser_array.is_array())
      return false;

    m_tx_endorsers.clear();
    for (auto &each_endorser : endorser_array) {
      Endorser tmp;
      tmp.endorser_id = json::get<string>(each_endorser, "id").value();
      tmp.endorser_pk = json::get<string>(each_endorser, "pk").value();
      tmp.endorser_signature = json::get<string>(each_endorser, "sig").value();
      m_tx_endorsers.emplace_back(tmp);
    }
    return true;
  }

  base58_type getTxid() {
    return m_txid;
  }

  timestamp_t getTxTime() {
    return m_tx_time;
  }

  base64_type getSeed() {
    return m_seed;
  }

  string getContractId() {
    return m_contract_id;
  }

  base58_type getReceiverId() {
    return m_receiver_id;
  }

  int getFee() {
    return m_fee;
  }

  string getTxInputCbor() {
    return m_tx_input_cbor;
  }

  base58_type getUserId() {
    return m_tx_user_id;
  }

  base64_type getProdSig() {
    return m_tx_user_sig;
  }

  std::vector<Endorser> getEndorsers() {
    return m_tx_endorsers;
  }
};
} // namespace gruut
#endif
