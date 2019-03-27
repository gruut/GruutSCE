#ifndef GRUUTSCE_HANDLER_CERTIFICATE_HPP
#define GRUUTSCE_HANDLER_CERTIFICATE_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class CertificateHandler : public ConditionHandler {
public:
  CertificateHandler() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {
    return true;
  }


};

}
}

#endif //GRUUTSCE_HANDLER_CERTIFICATE_HPP
