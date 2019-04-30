
#ifndef GRUUTSCE_GET_HANDLER_HPP
#define GRUUTSCE_GET_HANDLER_HPP

#include "../config.hpp"
#include "../data/datamap.hpp"
#include "../data/data_storage.hpp"
#include "../condition/condition_manager.hpp"

namespace gruut::gsce {

class GetHandler {
public:
  GetHandler() = default;

  void parseGet(std::vector<std::pair<pugi::xml_node,std::string>> &get_nodes, ConditionManager &condition_manager, DataStorage &data_storage){
    if(get_nodes.empty())
      return;

    for(auto &[get_node, condition_id] : get_nodes) {

      if(!condition_id.empty() && !condition_manager.getEvalResultById(condition_id))
        continue;

      pugi::xpath_node_set var_nodes = get_node.select_nodes("/var");

      for(auto &var_path : var_nodes){
        auto var_node = var_path.node();
        std::string scope = var_node.attribute("scope").value();

        if(!(scope == "author" || scope == "user" || scope == "contract"))
          continue;

        std::string id = var_node.attribute("id").value();
        std::string id_as = var_node.attribute("id-as").value();
        std::string name = var_node.attribute("name").value();
        std::string name_as = var_node.attribute("as").value();

        id = data_storage.eval(id);
        name = data_storage.eval(name);

        if(name.empty())
          continue;

        std::vector<std::pair<std::string,std::string>> values = data_storage.getVariables(scope, id, name);

        if(values.empty())
          continue;

        for(auto &[each_name,each_value] : values) {
          std::string key = "$.";
          key.append(scope).append(".").append(each_name);
          data_storage.updateValue(key, each_value);
        }

        if(!name_as.empty()) {
          data_storage.updateValue("$." + name_as,values[0].second);
        }
      }

    }

  }

};

}

#endif //GRUUTSCE_GET_HANDLER_HPP
