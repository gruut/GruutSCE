#ifndef VERONN_SCE_HANDLER_CERTIFICATE_HPP
#define VERONN_SCE_HANDLER_CERTIFICATE_HPP

#include "../config.hpp"
#include "base_condition_handler.hpp"

namespace veronn::vsce {

class CertificateHandler : public BaseConditionHandler {
public:
  CertificateHandler() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {
    auto pk_node = doc_node.child("pk");
    auto by_node = doc_node.child("by");

    if(pk_node.empty() || by_node.empty())
      return false;

    std::string pk = pk_node.attribute("value").value();
    std::string pk_type = pk_node.attribute("type").value();
    if(pk.empty() || pk[0] != '$')
      return false;
    else{
      auto pk_temp = datamap.get(pk);
      if(!pk_temp.has_value())
        return false;
      pk = pk_temp.value();
    }

    std::string by = by_node.attribute("value").value();
    std::string by_type = by_node.attribute("type").value();
    if(by.empty())
      return false;
    else if(by[0] == '$'){
	  auto by_temp = datamap.get(by);
	  if(!by_temp.has_value())
		return false;
	  by = by_temp.value();
    }

    //TODO : may be handled by type ( `PEM` / `DER` )
    //now just check `PEM`
    try {
      Botan::DataSource_Memory by_cert_datasource(by);
      Botan::X509_Certificate by_cert(by_cert_datasource);
      Botan::ECDSA_PublicKey ecdsa_by_pk(by_cert.subject_public_key_algo(), by_cert.subject_public_key_bitstring());

      Botan::DataSource_Memory cert_datasource(pk);
      Botan::X509_Certificate cert(cert_datasource);

      return cert.check_signature(ecdsa_by_pk);
    }
    catch(Botan::Exception &exception){
      return false;
    }
  }
};

}

#endif
