#ifndef VERONN_SCE_BASE_CONDITION_HANDLER_HPP
#define VERONN_SCE_BASE_CONDITION_HANDLER_HPP

#include "../config.hpp"
#include "../data/datamap.hpp"

namespace veronn::vsce {

class BaseConditionHandler {
public:
  BaseConditionHandler() = default;

  virtual bool evalue(pugi::xml_node &doc_node, Datamap &datamap);

protected:
  EvalRuleType getEvalRule(std::string_view eval_str) {
    if(eval_str.empty() || vs::toLower(eval_str) != "and")
      return EvalRuleType::OR;

    return EvalRuleType::AND;
  }

  PrimaryConditionType getPrimaryConditionType(std::string_view condition_tag) {
    if(condition_tag.empty())
      return PrimaryConditionType::UNKNOWN;

    std::string cond_tag_lower = vs::toLower(condition_tag);

    static std::map<std::string, PrimaryConditionType> tag_to_type_map = {
        {"condition", PrimaryConditionType::ROOT},
        {"update", PrimaryConditionType::ROOT},
        {"delete", PrimaryConditionType::ROOT},
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

  SecondaryConditionType getSecondaryConditionType(std::string_view condition_tag) {
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

#endif
