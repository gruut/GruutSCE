
#ifndef GRUUTSCE_GET_HANDLER_HPP
#define GRUUTSCE_GET_HANDLER_HPP

#include "../config.hpp"
#include "../data/datamap.hpp"
#include "../data/temp_data_collector.hpp"
#include "../condition/condition_manager.hpp"

namespace gruut::gsce {

class GetHandler {
public:
  GetHandler() = default;

  void parseGet(std::vector<std::pair<pugi::xml_node,std::string>> &get_nodes, ConditionManager &condition_manager, TempDataCollector &temp_data_collector){
    if(get_nodes.empty())
      return;

  }

};

}

#endif //GRUUTSCE_GET_HANDLER_HPP
