#ifndef GRUUTSCE_CONDITION_HANDLER_HPP
#define GRUUTSCE_CONDITION_HANDLER_HPP

#include "../config.hpp"

namespace gruut {
namespace gsce {

class ConditionHandler {
public:
  ConditionHandler();

  virtual bool evalue(pugi::xml_node &document_element, Datamap &datamap);
  
};

}
}

#endif //GRUUTSCE_CONDITION_HANDLER_HPP
