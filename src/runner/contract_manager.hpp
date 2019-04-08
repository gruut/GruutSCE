#ifndef GRUUTSCE_CONTRACT_MANAGER_HPP
#define GRUUTSCE_CONTRACT_MANAGER_HPP

#include "../config.hpp"

namespace gruut::gsce {

using namespace nlohmann;

class ContractManager {
private:
  std::map<std::string,pugi::xml_node> m_contract_cache;
  std::function<std::string(std::string)> m_contract_storage_interface;

public:
  ContractManager() = default;

  void attachInterface(std::function<std::string(std::string)> contract_storage_interface){
    m_contract_storage_interface = std::move(contract_storage_interface);
  }

  template <typename S = std::string>
  pugi::xml_node getContract(S &&cid) {

    pugi::xml_node null_node(nullptr);

    // TODO : validating of cid format

    auto it_map = m_contract_cache.find(cid);
    if(it_map != m_contract_cache.end()) {
      return it_map->second;
    }

    std::string xml_doc = m_contract_storage_interface(cid);

    pugi::xml_document doc;

    if (doc.load_string(xml_doc.c_str(), pugi::parse_minimal)) {
      pugi::xml_node doc_node = doc.document_element();
      m_contract_cache.insert({cid,doc_node});

      return doc_node;
    }

    return null_node;
  }

};

}

#endif //GRUUTSCE_CONTRACT_MANAGER_HPP
