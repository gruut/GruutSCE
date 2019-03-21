
#ifndef GRUUTSCE_CONDITION_HPP
#define GRUUTSCE_CONDITION_HPP

#include "../config/config.hpp"

#include <algorithm>
#include <iostream>

namespace gruut {
namespace gsce {

using namespace pugi;

enum class EvalRuleType : int {
  AND,
  OR
};

class Condition {

public:
  Condition() {

  }

  bool evalue(std::string &document, std::string &root_directive, Datamap &datamap) {
    xml_document doc;
    if (!doc.load_string(document.c_str(), pugi::parse_minimal))
      return false;

    EvalRuleType base_eval_rule = getEvalRule(doc.child(root_directive.c_str()).attribute("eval-rule").value());

    return true;
  }

private:

  EvalRuleType getEvalRule(const std::string &eval_str) {
    std::string eval_str_lower = eval_str;
    std::transform(eval_str_lower.begin(), eval_str_lower.end(), eval_str_lower.begin(), ::tolower);

    if(eval_str_lower == "and")
      return EvalRuleType::AND;

    return EvalRuleType::OR;
  }

};

}}

#endif //GRUUTSCE_CONDITION_HPP
