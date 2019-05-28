#ifndef TETHYS_PUBLIC_MERGER_ENDORSER_HPP
#define TETHYS_PUBLIC_MERGER_ENDORSER_HPP

#include "../config.hpp"

namespace tethys {
class Endorser {
public:
  base58_type endorser_id;
  std::string endorser_pk;
  base64_type endorser_signature;

  Endorser() = default;
  Endorser(base58_type &&endorser_id_, std::string &&endorser_pk_, base64_type &&endorser_signature_)
      : endorser_id(endorser_id_), endorser_pk(endorser_pk_), endorser_signature(endorser_signature_) {}
  Endorser(base58_type &endorser_id_, std::string &endorser_pk_, base64_type &endorser_signature_)
      : endorser_id(endorser_id_), endorser_pk(endorser_pk_), endorser_signature(endorser_signature_) {}
};
} // namespace tethys

#endif