#ifndef VERONNSCE_HANDLER_VAR_HPP
#define VERONNSCE_HANDLER_VAR_HPP

#include "base_condition_handler.hpp"
namespace veronn::vsce {

class VarHandler : public BaseConditionHandler {
public:
  VarHandler() = default;

  bool evalue(pugi::xml_node &doc_node, DataManager &data_manager) override {

    return true;
  }
};

}

#endif
