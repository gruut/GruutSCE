#ifndef GRUUTSCE_HANDLER_ENDORSER_HPP
#define GRUUTSCE_HANDLER_ENDORSER_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class EndorserHandler : public ConditionHandler {
public:
  EndorserHandler() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {
    return true;
  }


};

}
}

#endif //GRUUTSCE_HANDLER_ENDORSER_HPP
