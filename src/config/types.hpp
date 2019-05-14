#ifndef VERONN_SCE_TYPES_HPP
#define VERONN_SCE_TYPES_HPP

// Please, do not include this file directly.
// Instead, you can include this file by including config.hpp

#include <string>
#include <unordered_map>
#include <json.hpp>

namespace veronn::vsce {

enum class EnumV : int {
  GRU = 0,
  FIAT = 1,
  COIN = 2,
  XCOIN = 3,
  MILE = 4
};

enum class EnumGender : int {
  MALE = 90,
  FEMALE = 91,
  OTHER = 92
};

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

const std::unordered_map<std::string, VarType> VarTypeMap {
    {"", VarType::TEXT},
    {"INT", VarType::INT},
    {"PINT", VarType::PINT},
    {"NINT", VarType::NINT},
    {"FLOAT", VarType::FLOAT},
    {"BOOL", VarType::BOOL},
    {"TINYTEXT", VarType::TINYTEXT},
    {"TEXT", VarType::TEXT},
    {"MEDIUMTEXT", VarType::LONGTEXT},
    {"DATETIME", VarType::DATETIME},
    {"DATE", VarType::DATE},
    {"BIN", VarType::BIN},
    {"DEC", VarType::DEC},
    {"HEX", VarType::HEX},
    {"BASE58", VarType::BASE58},
    {"BASE64", VarType::BASE64},
    {"PEM", VarType::PEM},
    {"ENUMV", VarType::ENUMV},
    {"ENUMGENDER", VarType::ENUMGENDER},
    {"ENUMALL", VarType::ENUMALL},
    {"CONTRACT", VarType::CONTRACT},
    {"XML", VarType::XML}
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
    {"RUN_PERIOD", "runnable time period is not met"},
    {"NO_CONTRACT", "corresponding contract does not exist"},
    {"RUN_UNKNOWN", "unknown error occurred"},
    {"CONFIG_WORLD", "error on configuration of world and chain"},
    {"NO_USER", "error on reading user attributes"},
};

using BlockJson = nlohmann::json;
using TransactionJson = nlohmann::json;

}

namespace veronn {

using string = std::string;
using bytes = std::vector<uint8_t>;
using hash_t = std::vector<uint8_t>;
using timestamp_t = uint64_t;

using txagg_cbor_b64 = std::string;
using alphanumeric_type = std::string;
using base58_type = std::string;
using base64_type = std::string;

}

#endif
