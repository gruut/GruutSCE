#ifndef TETHYS_SCE_HANDLER_USER_HPP
#define TETHYS_SCE_HANDLER_USER_HPP

#include <vector>
#include "../config.hpp"
#include "base_condition_handler.hpp"

namespace tethys::tsce {

class UserHandler : public BaseConditionHandler {
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

  bool evalue(pugi::xml_node &doc_node, DataManager &data_manager) override {

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
      std::string user_id_b58 = doc_node.text().as_string(); // <id>...</id>
      tt::trim(user_id_b58);

      auto data = data_manager.get(m_user_key);
      if(!data.has_value()){
        return false;
      }

      eval_result = (data.value() == user_id_b58);
      break;
    }
    case SecondaryConditionType::LOCATION: {

      // TODO : improve smarter

      std::string location = doc_node.attribute("country").value();
      location += " ";
      location += doc_node.attribute("state").value();

      auto data = data_manager.get(m_user_key + ".location");
      if(!data.has_value()){
        return false;
      }

      eval_result = (data.value() == location);
      break;
    }

    case SecondaryConditionType::SERVICE: {

      // TODO : improve smarter

      std::string service_type = doc_node.attribute("type").value();
      std::string service_code = doc_node.text().as_string();

      tt::trim(service_code);

      auto type_data = data_manager.get(m_user_key + ".isc_type");
      if(!type_data.has_value()){
        return false;
      }

      auto code_data = data_manager.get(m_user_key + ".isc_code");
      if(!code_data.has_value()){
        return false;
      }

      eval_result = (type_data.value() == service_type && code_data.value() == service_code);
      break;

    }

    case SecondaryConditionType::AGE: {
      std::string age_after = doc_node.attribute("after").value();
      std::string age_before = doc_node.attribute("before").value();

      tt::trim(age_after);
      tt::trim(age_before);

      if(age_after.empty() && age_before.empty())
        return false;

      auto data = data_manager.get(m_user_key + ".birthday");
      if(!data.has_value()) {
        return false;
      }

      std::istringstream in{data.value()};
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

    if(base_condition_type == SecondaryConditionType::NIF) {
      eval_result = !eval_result;
    }

    return eval_result;
  }

};

}


#endif
