#ifndef GRUUTSCE_CONDITION_HANDLER_HPP
#define GRUUTSCE_CONDITION_HANDLER_HPP

#include "../config.hpp"

namespace gruut {
namespace gsce {



class ConditionHandler {
public:
  ConditionHandler() = default;

  virtual bool evalue(pugi::xml_node &doc_node, Datamap &datamap);

protected:
  EvalRuleType getEvalRule(const std::string &eval_str) {
    if(eval_str.empty() || vs::toLower(eval_str) != "and")
      return EvalRuleType::OR;

    return EvalRuleType::AND;
  }

  PrimaryConditionType getPrimaryConditionType(const std::string &condition_tag) {
    if(condition_tag.empty())
      return PrimaryConditionType::UNKNOWN;

    std::string cond_tag_lower = vs::toLower(condition_tag);

    static std::map<std::string, PrimaryConditionType> tag_to_type_map = {
        {"condition", PrimaryConditionType::CONDITION},
        {"if", PrimaryConditionType::IF},
        {"nif", PrimaryConditionType::NIF},
        {"compare", PrimaryConditionType::COMPARE},
        {"signature",PrimaryConditionType::SIGNATURE},
        {"certificate", PrimaryConditionType::CERTIFICATE},
        {"time", PrimaryConditionType::TIME},
        {"endorser", PrimaryConditionType::ENDORSER},
        {"receiver", PrimaryConditionType::RECEIVER},
        {"user", PrimaryConditionType::USER}
    };

    auto it_map = tag_to_type_map.find(cond_tag_lower);
    if(it_map == tag_to_type_map.end()){
      return PrimaryConditionType::UNKNOWN;
    }

    return it_map->second;
  }

  SecondaryConditionType getSecondaryConditionType(const std::string &condition_tag) {
    if(condition_tag.empty())
      return SecondaryConditionType::UNKNOWN;

    std::string cond_tag_lower = vs::toLower(condition_tag);

    static std::map<std::string, SecondaryConditionType> tag_to_type_map = {
        {"if", SecondaryConditionType::IF},
        {"nif", SecondaryConditionType::NIF},
        {"age", SecondaryConditionType::AGE},
        {"service",SecondaryConditionType::SERVICE},
        {"id", SecondaryConditionType::ID},
        {"location", SecondaryConditionType::LOCATION}
    };

    auto it_map = tag_to_type_map.find(cond_tag_lower);
    if(it_map == tag_to_type_map.end()){
      return SecondaryConditionType::UNKNOWN;
    }

    return it_map->second;
  }

};

}
}

#endif //GRUUTSCE_CONDITION_HANDLER_HPP
