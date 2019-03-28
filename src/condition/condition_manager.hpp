#ifndef GRUUTSCE_CONDITION_MANAGER_HPP
#define GRUUTSCE_CONDITION_MANAGER_HPP

#include <string>
#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class ConditionManager {

private:
  ConditionHandler m_condition_handler;
  std::map<std::string, bool> m_result_cache;

public:
  ConditionManager() = default;

  bool evalue(std::string &document, Datamap &datamap, bool invalidate = false) {

    pugi::xml_document doc;
    if (!doc.load_string(document.c_str(), pugi::parse_minimal))
      return false;

    pugi::xml_node doc_node = doc.document_element();
    return evalue(doc_node, datamap, invalidate);

  }

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap, bool invalidate = false) {

    std::string condition_id = doc_node.attribute("id").value();
    if(condition_id.empty()) {
      condition_id = "__null_condition_id__";
    }

    bool eval_result = false;

    if(!invalidate) {
      auto it_cache = m_result_cache.find(condition_id);
      if(it_cache == m_result_cache.end()) {
        eval_result = m_condition_handler.evalue(doc_node,datamap);
        m_result_cache.insert({condition_id,eval_result});
      } else {
        eval_result = it_cache->second;
      }
    } else {
      eval_result = m_condition_handler.evalue(doc_node,datamap);
      m_result_cache[condition_id] = eval_result; // force update
    }

    return eval_result;

  }
};

}}

#endif //GRUUTSCE_CONDITION_MANAGER_HPP
