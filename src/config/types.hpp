#ifndef GRUUTSCE_TYPES_HPP
#define GRUUTSCE_TYPES_HPP

// Please, do not include this file directly.
// Instead, you can include this file by including config.hpp

#include <string>
#include <json.hpp>

namespace gruut::gsce {

enum class VarType : int {
  INT,
  PINT,
  NINT,
  FLOAT,
  BOOL,
  TINYTEXT,
  TEXT,
  MEDIUMTEXT,
  LONGTEXT,
  DATETIME,
  DATE,
  BIN,
  DEC,
  HEX,
  BASE58,
  BASE64,
  PEM,
  ENUMV,
  ENUMGENDER,
  ENUMALL,
  CONTRACT,
  XML,
  NONE
};

enum class EvalRuleType : int {
  AND,
  OR
};

enum class PrimaryConditionType : int {
  ROOT,
  IF,
  NIF,
  COMPARE,
  SIGNATURE,
  CERTIFICATE,
  TIME,
  ENDORSER,
  RECEIVER,
  USER,
  VAR,
  UNKNOWN
};

enum class SecondaryConditionType : int {
  USER,
  RECEIVER,
  ENDORSER,
  IF,
  NIF,
  ID,
  LOCATION,
  SERVICE,
  AGE,
  UNKNOWN
};

std::map<std::string,std::string> GSCE_ERROR_MSG = {
    {"RUN_CONDITION", "condition is not met"},
    {"RUN_PERIOD", "runnable time period is not met"}
};

using BlockJson = nlohmann::json;
using TransactionJson = nlohmann::json;

}

namespace gruut {

using string = std::string;
using bytes = std::vector<uint8_t>;
using hash_t = std::vector<uint8_t>;
using timestamp_t = uint64_t;

using txagg_cbor_b64 = std::string;
using alphanumeric_type = std::string;
using base58_type = std::string;
using base64_type = std::string;

}

#endif //GRUUTSCE_TYPES_HPP
