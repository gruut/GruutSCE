#ifndef GRUUTSCE_HANDLER_TIME_HPP
#define GRUUTSCE_HANDLER_TIME_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class TimeHandler : public ConditionHandler {
public:
  TimeHandler();

  bool evalue(pugi::xml_node &document_element, Datamap &datamap) override {
    return true;
  }


};

}
}

#endif //GRUUTSCE_HANDLER_TIME_HPP
