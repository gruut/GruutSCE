#ifndef GRUUTSCE_HANDLER_COMPARE_HPP
#define GRUUTSCE_HANDLER_COMPARE_HPP

#include "../config.hpp"
#include "base_condition_handler.hpp"

namespace gruut::gsce {

enum class CompareType : int {
  EQ,
  NE,
  GE,
  LE,
  GT,
  LT,
  AGE,
  ALE,
  AGT,
  ALT,
  UNKNOWN
};

class CompareHandler : public BaseConditionHandler {
public:
  CompareHandler() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {

    std::string src_str = doc_node.attribute("src").value();
    std::string ref_str = doc_node.attribute("ref").value();
    CompareType comp_type = getCompareType(doc_node.attribute("type").value());

    vs::trim(src_str);
    vs::trim(ref_str);

    if(src_str.empty() || ref_str.empty()) {
      return false;
    }

    if(src_str[0] == '$'){
      DataRecord src_data;
      auto data = datamap.get(src_str);
      if(!data.has_value())
        return false;

      src_str = src_data.value;

    }

    if(ref_str[0] == '$'){
      DataRecord ref_data;
      auto data = datamap.get(src_str);
      if(!data.has_value())
        return false;

      ref_str = ref_data.value;
    }

    bool eval_result;

    if(comp_type == CompareType::EQ) {
      eval_result = (src_str == ref_str);
    } else if(comp_type == CompareType::NE){
      eval_result = (src_str != ref_str);
    } else {

      int src_int, ref_int, abs_val;

      try {
        src_int = std::stoi(src_str);
        ref_int = std::stoi(ref_str);
      }
      catch (...) {
        return false;
      }

      std::string abs_str = doc_node.attribute("abs").value();
      vs::trim(abs_str);

      if(abs_str.empty())
        abs_val = 0;
      else {
        try {
          abs_val = std::stoi(abs_str);
        }
        catch(...) {
          return false;
        }
      }

      switch (comp_type) {
      case CompareType::GE:
        eval_result = (src_int >= ref_int);
        break;
      case CompareType::LE:
        eval_result = (src_int <= ref_int);
        break;
      case CompareType::GT:
        eval_result = (src_int > ref_int);
        break;
      case CompareType::LT:
        eval_result = (src_int < ref_int);
        break;
      case CompareType::AGE:
        eval_result = (abs(src_int - ref_int) >= abs_val);
        break;
      case CompareType::ALE:
        eval_result = (abs(src_int - ref_int) <= abs_val);
        break;
      case CompareType::AGT:
        eval_result = (abs(src_int - ref_int) > abs_val);
        break;
      case CompareType::ALT:
        eval_result = (abs(src_int - ref_int) < abs_val);
        break;
      default:
        eval_result = false;
      }
    }

    return eval_result;
  }

private:
  CompareType getCompareType(const std::string &type_str){
    std::string type_str_lower = vs::toLower(type_str);

    static std::map<std::string, CompareType> tag_to_type_map = {
        {"eq", CompareType::EQ},
        {"ne", CompareType::NE},
        {"ge", CompareType::GE},
        {"le", CompareType::LE},
        {"gt",CompareType::GT},
        {"lt", CompareType::LT},
        {"age", CompareType::AGE},
        {"ale", CompareType::ALE},
        {"agt", CompareType::AGT},
        {"alt", CompareType::ALT}
    };

    auto it_map = tag_to_type_map.find(type_str_lower);
    if(it_map == tag_to_type_map.end()){
      return CompareType::UNKNOWN;
    }

    return it_map->second;
  }

};

}

#endif //GRUUTSCE_HANDLER_COMPARE_HPP
