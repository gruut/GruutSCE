#ifndef VERONN_SCE_GET_HANDLER_HPP
#define VERONN_SCE_GET_HANDLER_HPP

#include "../config.hpp"
#include "../data/datamap.hpp"
#include "../data/data_storage.hpp"
#include "../condition/condition_manager.hpp"

namespace veronn::vsce {

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

        std::string storage_scope = (scope == "author" || scope == "user") ? "user" : "contract";
        std::string storage_id = id;

        if(scope == "author") {
          storage_id = data_storage.eval("$author");
        } else if(scope == "user") {
          if(id.empty())
            storage_id = data_storage.eval("$user");
        } else { // scope == "contract"
          if(id.empty())
            storage_id = data_storage.eval("$tx.body.cid");
        }

        auto values = data_storage.getScopeVariables(storage_scope, storage_id, name);

        if(!values)
          continue;

        if(id.empty() || id == storage_id) {
          for (auto &each_attr : values.value()) {
            std::string key = "$";
            key.append(scope).append(".").append(each_attr.name);
            data_storage.updateValue(key, each_attr.value);
          }
        }

        if(!id.empty() && id != storage_id && !id_as.empty()) {
          for (auto &each_attr : values.value()) {
            std::string key = "$.";
            key.append(id_as).append(".").append(each_attr.name);
            data_storage.updateValue(key, each_attr.value);
          }
        }

        if(!name_as.empty()) {

          if(values.value().size() == 1) {
            data_storage.updateValue("$." + name_as, (*values)[0].value);
          } else {
            for (auto &each_attr : values.value()) {
              std::string key = "$.";
              key.append(name_as).append(".").append(each_attr.name);
              data_storage.updateValue(key, each_attr.value);
            }
          }
        }
      }

    }

  }

};

}

#endif
