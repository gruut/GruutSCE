#ifndef GRUUTSCE_CONTRACT_MANAGER_HPP
#define GRUUTSCE_CONTRACT_MANAGER_HPP

#include "../config.hpp"

namespace gruut::gsce {

using namespace nlohmann;

class ContractManager {
private:
  std::map<std::string,std::pair<std::string,pugi::xml_node>> m_contract_cache;
  std::function<nlohmann::json(nlohmann::json&)> m_contract_storage_interface;

public:
  ContractManager() = default;

  void attachReadInterface(std::function<nlohmann::json(nlohmann::json&)> contract_storage_interface){
    m_contract_storage_interface = std::move(contract_storage_interface);
  }

  template <typename S = std::string>
  std::optional<pugi::xml_node> getContract(S &&cid) {

    // TODO : validating of cid format

    pugi::xml_node null_node(nullptr);

    auto it_map = m_contract_cache.find(cid);
    if(it_map != m_contract_cache.end()) {

      if(it_map->second.first.empty())
        return std::nullopt;
      else {
        if(it_map->second.second::empty())
          return std::nullopt;

        return it_map->second.second;
      }
    }

    nlohmann::json query = {
        {"type", "contract.get"},
        {"where", {
            {"cid", cid}
        }}
    };

    nlohmann::json query_result = m_contract_storage_interface(query);

    if(query_result.empty()) {
      m_contract_cache.insert({cid,{"",null_node}});
      return std::nullopt;
    }

    auto xml_doc = query_result[0]["contract"].get<std::string>();

    if(xml_doc.empty()) {
      m_contract_cache.insert({cid,{"",null_node}});
      return std::nullopt;
    }

    pugi::xml_document doc;

    if (doc.load_string(xml_doc.c_str(), pugi::parse_minimal)) {
      pugi::xml_node doc_node = doc.document_element();
      m_contract_cache.insert({cid,{xml_doc,doc_node}});

      return doc_node;
    }

    m_contract_cache.insert({cid,{xml_doc,null_node}});
    return std::nullopt;
  }

};

}

#endif //GRUUTSCE_CONTRACT_MANAGER_HPP
