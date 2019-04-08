#ifndef GRUUTSCE_CONDITION_MANAGER_HPP
#define GRUUTSCE_CONDITION_MANAGER_HPP

#include <string>
#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut::gsce {

class ConditionManager {

private:
  ConditionHandler m_condition_handler;
  std::map<std::string, bool> m_result_cache;

public:
  ConditionManager() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap, bool invalidate = false) {

    std::string condition_id = doc_node.attribute("id").value();
    if(condition_id.empty()) {
      return true;
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

  template <typename S = std::string>
  bool getEvalResultById(S &&id) {

    if(id.empty())
      return true;

    std::string condition_id = id;
    bool is_neg = false;

    if(id[0] == '~') {
      is_neg = true;
      condition_id = id.substr(1);
    }

    if(condition_id.empty()) {
      return true;
    }

    auto it_cache = m_result_cache.find(condition_id);
    if(it_cache == m_result_cache.end()) {
      return true;
    }

    if(is_neg)
      return !it_cache->second;
    else
      return it_cache->second;
  }
};

}

#endif //GRUUTSCE_CONDITION_MANAGER_HPP
