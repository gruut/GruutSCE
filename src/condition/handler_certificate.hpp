#ifndef GRUUTSCE_HANDLER_CERTIFICATE_HPP
#define GRUUTSCE_HANDLER_CERTIFICATE_HPP

#include "../config.hpp"
#include "base_condition_handler.hpp"

namespace gruut {
namespace gsce {

class CertificateHandler : public BaseConditionHandler {
public:
  CertificateHandler() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {
    return true;
  }


};

}
}

#endif //GRUUTSCE_HANDLER_CERTIFICATE_HPP
