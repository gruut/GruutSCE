#ifndef GRUUTSCE_HANDLER_COMPARE_HPP
#define GRUUTSCE_HANDLER_COMPARE_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class CompareHandler : public ConditionHandler {
public:
  CompareHandler();

  bool evalue(pugi::xml_node &document_element, Datamap &datamap) override {
    return true;
  }

};
}

}

#endif //GRUUTSCE_HANDLER_COMPARE_HPP
