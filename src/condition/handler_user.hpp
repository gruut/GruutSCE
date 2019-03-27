#ifndef GRUUTSCE_HANDLER_USER_HPP
#define GRUUTSCE_HANDLER_USER_HPP

#include <vector>
#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class UserHandler : public ConditionHandler {
private:
  std::string m_user_key;
public:
  UserHandler() {
    m_user_key = "$user";
  };

  void setUserType(PrimaryConditionType user_type){
    if(user_type == PrimaryConditionType::RECEIVER) {
      m_user_key = "$receiver";
    }
  }

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {

    SecondaryConditionType base_condition_type = getSecondaryConditionType(doc_node.name());

    bool eval_result;

    switch(base_condition_type) {
    case SecondaryConditionType::USER:
    case SecondaryConditionType::RECEIVER:
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
      std::string user_id_b58 = doc_node.value(); // <id>...</id>

      DataRecord user_id_data;
      if(!datamap.get(m_user_key,user_id_data)){
        return false;
      }

      eval_result = (user_id_data.value == user_id_b58);
      break;
    }
    case SecondaryConditionType::LOCATION: {

      // TODO : improve smarter

      std::string location = doc_node.attribute("country").value();
      location += " ";
      location += doc_node.attribute("state").value();

      DataRecord location_data;
      if(!datamap.get(m_user_key + ".location",location_data)){
        return false;
      }

      eval_result = (location_data.value == location);
      break;
    }

    case SecondaryConditionType::SERVICE: {

      // TODO : improve smarter

      std::string service_type = doc_node.attribute("type").value();
      std::string service_code = doc_node.value();

      DataRecord service_type_data;
      if(!datamap.get(m_user_key + ".isc_type",service_type_data)){
        return false;
      }

      DataRecord service_code_data;
      if(!datamap.get(m_user_key + ".isc_code",service_code_data)){
        return false;
      }

      eval_result = (service_type_data.value == service_type && service_code_data.value == service_code);
      break;

    }

    case SecondaryConditionType::AGE: {
      std::string age_after = doc_node.attribute("after").value();
      std::string age_before = doc_node.attribute("before").value();

      vs::trim(age_after);
      vs::trim(age_before);

      if(age_after.empty() && age_before.empty())
        return false;

      DataRecord user_birthday;
      if(!datamap.get(m_user_key + ".birthday", user_birthday)) {
        return false;
      }

      std::istringstream in{user_birthday.value};
      date::sys_time<std::chrono::milliseconds> birthday_time_point;
      in >> date::parse("%F", birthday_time_point);

      auto birthday = date::year_month_day{floor<date::days>(birthday_time_point)};
      auto today = date::year_month_day{floor<date::days>(std::chrono::system_clock::now())};

      if(age_before.empty()) {
        try{
          int age_after_int = std::stoi(age_after);
          return (birthday + date::years{age_after_int} < today);
        }
        catch(...) {
          return false;
        }
      }

      if(age_after.empty()) {
        try{
          int age_before_int = std::stoi(age_before);
          return (birthday + date::years{age_before_int} > today);
        }
        catch(...) {
          return false;
        }
      }

      try{
        int age_after_int = std::stoi(age_after);
        int age_before_int = std::stoi(age_before);
        return (birthday + date::years{age_after_int} < today && today < birthday + date::years{age_before_int});
      }
      catch(...) {
        return false;
      }

    }
    default: {
      eval_result = false;
      break;
    }
    }

    return eval_result;
  }

};

}
}


#endif //GRUUTSCE_HANDLER_USER_HPP
