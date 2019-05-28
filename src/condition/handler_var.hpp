#ifndef TETHYS_SCE_HANDLER_VAR_HPP
#define TETHYS_SCE_HANDLER_VAR_HPP

#include "base_condition_handler.hpp"
#include "handler_compare.hpp"
namespace tethys::tsce {

class VarHandler : public BaseConditionHandler {
public:
  VarHandler() = default;

  bool evalue(pugi::xml_node &doc_node, DataManager &data_manager) override {

    std::string scope_str = doc_node.attribute("scope").value();
    std::string id_str = doc_node.attribute("id").value();
    std::string name_str = doc_node.attribute("name").value();
    std::string ref_str = doc_node.attribute("ref").value();
    std::string type_str = doc_node.attribute("type").value();
    std::string abs_str = doc_node.attribute("abs").value();

    if(tt::trim(name_str) == "*")
      return false;

    pugi::xml_node compare_node;
    compare_node.set_name("compare");
    compare_node.append_attribute("ref") = ref_str.c_str();
    compare_node.append_attribute("type") = type_str.c_str();
    compare_node.append_attribute("abs") = abs_str.c_str();

    CompareHandler compare_handler;

    if(tt::inArray(scope_str,{"user","author", "receiver","contract"})) {

      id_str = data_manager.eval(id_str);
      std::string real_scope = (scope_str == "contract") ? "contract" : "user";
      auto return_val = data_manager.getScopeVariables(real_scope, id_str, name_str); // should be [{name,value}]
      if (return_val.empty())
        return false;

      compare_node.append_attribute("src") = return_val[0].value.c_str();

      return compare_handler.evalue(compare_node,data_manager);

    } else if(scope_str == "transaction") {

      return false;

    } else if(scope_str == "block") {

      return false;

    } else if(scope_str == "world") {

      std::string keyw = "$world." + name_str;
      std::string src_val = data_manager.eval(keyw);
      compare_node.append_attribute("src") = src_val.c_str();
      return compare_handler.evalue(compare_node,data_manager);

    } else if(scope_str == "chain") {

      std::string keyw = "$chain." + name_str;
      std::string src_val = data_manager.eval(keyw);
      compare_node.append_attribute("src") = src_val.c_str();
      return compare_handler.evalue(compare_node,data_manager);
    } else {
      return false;
    }
  }
};

}

#endif
