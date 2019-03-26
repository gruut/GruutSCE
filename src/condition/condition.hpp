
#ifndef GRUUTSCE_CONDITION_HPP
#define GRUUTSCE_CONDITION_HPP

#include "../config.hpp"
#include "handler_compare.hpp"
#include "handler_signature.hpp"
#include "handler_certificate.hpp"
#include "handler_time.hpp"
#include "handler_endorser.hpp"
#include "handler_receiver.hpp"
#include "handler_user.hpp"

#include <algorithm>
#include <iostream>
#include <map>

namespace gruut {
namespace gsce {

using namespace pugi;

enum class PrimaryConditionType : int {
  CONDITION,
  IF,
  NIF,
  COMPARE,
  SIGNATURE,
  CERTIFICATE,
  TIME,
  ENDORSER,
  RECEIVER,
  USER,
  UNKNOWN
};

class Condition {

public:
  Condition();

  bool evalue(std::string &document, Datamap &datamap) {
    xml_document doc;
    if (!doc.load_string(document.c_str(), pugi::parse_minimal))
      return false;

    auto document_element = doc.document_element();
    return evalue(document_element, datamap);

  }

  bool evalue(pugi::xml_node &document_element, Datamap &datamap) {
    PrimaryConditionType base_condition_type = getPrimaryConditionType(document_element.name());
    EvalRuleType base_eval_rule = getEvalRule(document_element.attribute("eval-rule").value());

    bool eval_result = true;

    switch(base_condition_type) {
    case PrimaryConditionType::CONDITION:
    case PrimaryConditionType::IF:
    case PrimaryConditionType::NIF: {
      if (base_eval_rule == EvalRuleType::AND) {
        eval_result = true;
        for (pugi::xml_node &tags: document_element) {
          eval_result &= evalue(tags, datamap);
          if (!eval_result)
            break;
        }
      } else {
        eval_result = false;
        for (pugi::xml_node &tags: document_element) {
          eval_result |= evalue(tags, datamap);
          if (eval_result)
            break;
        }
      }
      break;
    }
    case PrimaryConditionType::COMPARE: {
      CompareHandler compare_handler;
      eval_result = compare_handler.evalue(document_element, datamap);
      break;
    }
    case PrimaryConditionType::SIGNATURE: {
      SignatureHandler signature_handler;
      eval_result = signature_handler.evalue(document_element, datamap);
      break;
    }
    case PrimaryConditionType::CERTIFICATE: {
      CertificateHandler certificate_handler;
      eval_result = certificate_handler.evalue(document_element, datamap);
      break;
    }
    case PrimaryConditionType::TIME: {
      TimeHandler time_handler;
      eval_result = time_handler.evalue(document_element, datamap);
      break;
    }
    case PrimaryConditionType::ENDORSER: {
      EndorserHandler endorser_handler;
      eval_result = endorser_handler.evalue(document_element, datamap);
      break;
    }
    case PrimaryConditionType::RECEIVER: {
      ReceiverHandler receiver_handler;
      eval_result = receiver_handler.evalue(document_element, datamap);
      break;
    }
    case PrimaryConditionType::USER: {
      UserHandler user_handler;
      eval_result = user_handler.evalue(document_element, datamap);
      break;
    }
    default:
      break;
    }

    if(base_condition_type == PrimaryConditionType::NIF) {
      eval_result = !eval_result;
    }

    return eval_result;
  }


private:

  EvalRuleType getEvalRule(const std::string &eval_str) {
    if(eval_str.empty() || vs::toLower(eval_str) != "and")
      return EvalRuleType::OR;

    return EvalRuleType::AND;
  }

  PrimaryConditionType getPrimaryConditionType(const std::string &condition_tag) {
    if(condition_tag.empty())
      return PrimaryConditionType::UNKNOWN;

    std::string cond_tag_lower = vs::toLower(condition_tag);

    static std::map<std::string, PrimaryConditionType> tag_to_type_map = {
        {"condition", PrimaryConditionType::CONDITION},
        {"if", PrimaryConditionType::IF},
        {"nif", PrimaryConditionType::NIF},
        {"compare", PrimaryConditionType::COMPARE},
        {"signature",PrimaryConditionType::SIGNATURE},
        {"certificate", PrimaryConditionType::CERTIFICATE},
        {"time", PrimaryConditionType::TIME},
        {"endorser", PrimaryConditionType::ENDORSER},
        {"receiver", PrimaryConditionType::RECEIVER},
        {"user", PrimaryConditionType::USER}
    };

    auto it_map = tag_to_type_map.find(cond_tag_lower);
    if(it_map == tag_to_type_map.end()){
      return PrimaryConditionType::UNKNOWN;
    }

    return it_map->second;
  }

};

}}

#endif //GRUUTSCE_CONDITION_HPP
