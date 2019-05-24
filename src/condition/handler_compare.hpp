#ifndef VERONN_SCE_HANDLER_COMPARE_HPP
#define VERONN_SCE_HANDLER_COMPARE_HPP

#include "../config.hpp"
#include "base_condition_handler.hpp"

namespace veronn::vsce {

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

  bool evalue(pugi::xml_node &doc_node, DataManager &data_manager) override {

    std::string src_str = doc_node.attribute("src").value();
    std::string ref_str = doc_node.attribute("ref").value();
    CompareType comp_type = getCompareType(doc_node.attribute("type").value());

    vs::trim(src_str);
    vs::trim(ref_str);

    if(src_str.empty() || ref_str.empty()) {
      return false;
    }

    auto src_opt = data_manager.evalOpt(src_str);
    if(!src_opt)
      return false;

    src_str = src_opt.value();

    auto ref_opt = data_manager.evalOpt(ref_str);
    if(!ref_opt)
      return false;

    ref_str = ref_opt.value();

    bool eval_result;

    if(comp_type == CompareType::EQ) {
      eval_result = (src_str == ref_str);
    } else if(comp_type == CompareType::NE){
      eval_result = (src_str != ref_str);
    } else {

      std::string abs_str = doc_node.attribute("abs").value();
      vs::trim(abs_str);

      int src_int = vs::str2num<int>(src_str);
      int ref_int = vs::str2num<int>(ref_str);
      int abs_val = vs::str2num<int>(abs_str);

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
  CompareType getCompareType(std::string_view type_str){
    std::string type_str_lower = vs::toUpper(type_str);

    static std::map<std::string, CompareType> tag_to_type_map = {
        {"EQ", CompareType::EQ},
        {"=", CompareType::EQ},
        {"==", CompareType::EQ},
        {"NE", CompareType::NE},
        {"!=", CompareType::NE},
        {"GE", CompareType::GE},
        {">=", CompareType::GE},
        {"=>", CompareType::GE},
        {"LE", CompareType::LE},
        {"<=", CompareType::LE},
        {"=<", CompareType::LE},
        {"GT",CompareType::GT},
        {">",CompareType::GT},
        {"LT", CompareType::LT},
        {"<", CompareType::LT},
        {"AGE", CompareType::AGE},
        {"ALE", CompareType::ALE},
        {"AGT", CompareType::AGT},
        {"ALT", CompareType::ALT}
    };

    auto it_map = tag_to_type_map.find(type_str_lower);
    if(it_map == tag_to_type_map.end()){
      return CompareType::UNKNOWN;
    }

    return it_map->second;
  }

};

}

#endif
