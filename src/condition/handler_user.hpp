#ifndef GRUUTSCE_HANDLER_USER_HPP
#define GRUUTSCE_HANDLER_USER_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class UserHandler : public ConditionHandler {
public:
  UserHandler() = default;

  bool evalue(pugi::xml_node &document_element, Datamap &datamap) override {
    return true;
  }


};

}
}


#endif //GRUUTSCE_HANDLER_USER_HPP
