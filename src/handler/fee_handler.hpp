#ifndef VERONN_SCE_FEE_HANDLER_HPP
#define VERONN_SCE_FEE_HANDLER_HPP

#include "../config.hpp"
#include "../data/data_storage.hpp"
#include "../condition/condition_manager.hpp"

namespace veronn::vsce {

class FeeHandler {
public:
  FeeHandler() = default;

  std::pair<int,int> parseGet(std::vector<std::pair<pugi::xml_node,std::string>> &fee_nodes, ConditionManager &condition_manager, DataStorage &data_storage) {
    if(fee_nodes.empty())
      return {0,0};

    int pay_from_user = 0;
    int pay_from_author = 0;

    for(auto &[fee_node, condition_id] : fee_nodes) {

      if(!condition_id.empty() && !condition_manager.getEvalResultById(condition_id))
        continue;

      pugi::xpath_node_set pay_nodes = fee_node.select_nodes("/pay");

      for(auto &pay_path : pay_nodes) {
        auto pay_node = pay_path.node();

        std::string from = pay_node.attribute("from").value();
        std::string value = pay_node.attribute("value").value();

        if(from == "user") {
          value = data_storage.eval(value);
          try {
            pay_from_user = std::stoi(value);
          }
          catch(...) {

          }
        } else {
          try {
            pay_from_author = std::stoi(value);
          }
          catch(...){

          }
        }

      }

    }

    // if there is multiple valid fee nodes, the last valid fee node will work

    return {pay_from_user,pay_from_author};
  }

};
}

#endif
