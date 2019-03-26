#ifndef GRUUTSCE_HANDLER_ENDORSER_HPP
#define GRUUTSCE_HANDLER_ENDORSER_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class EndorserHandler : public ConditionHandler {
public:
  EndorserHandler();

  bool evalue(pugi::xml_node &document_element, Datamap &datamap) override {
    return true;
  }


};

}
}

#endif //GRUUTSCE_HANDLER_ENDORSER_HPP
