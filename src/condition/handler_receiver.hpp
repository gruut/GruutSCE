#ifndef GRUUTSCE_HANDLER_RECEIVER_HPP
#define GRUUTSCE_HANDLER_RECEIVER_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class ReceiverHandler : public ConditionHandler {
public:
  ReceiverHandler();

  bool evalue(pugi::xml_node &document_element, Datamap &datamap) override {
    return true;
  }


};

}
}


#endif //GRUUTSCE_HANDLER_RECEIVER_HPP
