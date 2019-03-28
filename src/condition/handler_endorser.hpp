#ifndef GRUUTSCE_HANDLER_ENDORSER_HPP
#define GRUUTSCE_HANDLER_ENDORSER_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class EndorserHandler : public ConditionHandler {
public:
  EndorserHandler() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {

    SecondaryConditionType base_condition_type = getSecondaryConditionType(doc_node.name());

    bool eval_result;

    switch(base_condition_type) {
    case SecondaryConditionType::ENDORSER:
    case SecondaryConditionType::IF:
    case SecondaryConditionType::NIF: {
      EvalRuleType base_eval_rule = getEvalRule(doc_node.attribute("eval-rule").value());

      if (base_eval_rule == EvalRuleType::AND) {
        eval_result = true;
        for (pugi::xml_node &tags: doc_node) {
          eval_result &= evalue(tags, datamap);
          if (!eval_result)
            break;
        }
      } else {
        eval_result = false;
        for (pugi::xml_node &tags: doc_node) {
          eval_result |= evalue(tags, datamap);
          if (eval_result)
            break;
        }
      }
      break;
    }
    case SecondaryConditionType::ID: {

      std::string endorser_id_b58 = doc_node.value();

      DataRecord endorser_id_data;
      if(!datamap.get("$endorsers",endorser_id_data)){
        return false;
      }

      auto endorser_list = nlohmann::json::parse(endorser_id_data.value);
      if(endorser_list.empty()){
        return false;
      }

      auto it_vec = endorser_list.find(endorser_id_b58);
      eval_result = (it_vec != endorser_list.end());
      break;
    }
    default:
      eval_result = false;
      break;
    }

    if(base_condition_type == SecondaryConditionType::NIF) {
      eval_result = !eval_result;
    }

    return eval_result;
  }




};

}
}

#endif //GRUUTSCE_HANDLER_ENDORSER_HPP
