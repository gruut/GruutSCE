#ifndef VERONN_SCE_HANDLER_ENDORSER_HPP
#define VERONN_SCE_HANDLER_ENDORSER_HPP

#include "../config.hpp"
#include "base_condition_handler.hpp"

namespace veronn::vsce {

class EndorserHandler : public BaseConditionHandler {
public:
  EndorserHandler() = default;

  bool evalue(pugi::xml_node &doc_node, DataManager &data_manager) override {

    SecondaryConditionType base_condition_type = getSecondaryConditionType(doc_node.name());

    bool eval_result = false;

    switch(base_condition_type) {
    case SecondaryConditionType::ENDORSER:
    case SecondaryConditionType::IF:
    case SecondaryConditionType::NIF: {
      EvalRuleType base_eval_rule = getEvalRule(doc_node.attribute("eval-rule").value());

      if (base_eval_rule == EvalRuleType::AND) {
        eval_result = true;
        for (pugi::xml_node &tags: doc_node) {
          eval_result &= evalue(tags, data_manager);
          if (!eval_result)
            break;
        }
      } else {
        eval_result = false;
        for (pugi::xml_node &tags: doc_node) {
          eval_result |= evalue(tags, data_manager);
          if (eval_result)
            break;
        }
      }
      break;
    }
    case SecondaryConditionType::ID: {

      std::string endorser_id_b58 = doc_node.text().as_string();
      vs::trim(endorser_id_b58);

      auto data = data_manager.evalOpt("$tx.endorser.count");
      if(!data.has_value()){
        return false;
      }

      int num_endorsers = vs::str2num<int>(data.value());

      if(num_endorsers <= 0) {
        return false;
      }

      for(int i = 0 ; i < num_endorsers; ++i) {
        std::string nth_endorder_id = "$tx.endorser[" + std::to_string(i) + "].id";
        auto tx_endorser = data_manager.evalOpt(nth_endorder_id).value_or("");

        if(!tx_endorser.empty() && tx_endorser == endorser_id_b58) {
          eval_result = true;
          break;
        }
      }

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

#endif
