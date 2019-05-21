#ifndef VERONN_SCE_TYPES_HPP
#define VERONN_SCE_TYPES_HPP

// Please, do not include this file directly.
// Instead, you can include this file by including config.hpp

#include <string>
#include <unordered_map>
#include <json.hpp>

namespace veronn::vsce {

enum class EnumAll : uint8_t {
  KEYC = 0,
  FIAT = 1,
  COIN = 2,
  XCOIN = 3,
  MILE = 4,
  ENUMV = 10,
  ENUMGENDER = 11,
  ENUMALL = 12,
  PEM = 21,
  CONTRACT = 22,
  XML = 23,
  BIN = 31,
  DEC = 32,
  HEX = 33,
  BASE58 = 34,
  BASE64 = 35,
  INT = 50,
  PINT = 51,
  NINT  = 52,
  FLOAT = 53,
  BOOL = 54,
  TINYTEXT = 60,
  TEXT = 61,
  MEDIUMTEXT = 62,
  LONGTEXT = 63,
  DATETIME = 71,
  DATE = 72,
  MALE = 90,
  FEMALE = 90,
  OTHER = 92,
  NONE = 255
};

enum class EnumV : int {
  KEYC = 0,
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

const std::unordered_map<std::string, EnumAll> INPUT_OPTION_TYPE_MAP {
    {"INT", EnumAll::INT},
    {"PINT", EnumAll::PINT},
    {"NINT", EnumAll::NINT},
    {"FLOAT", EnumAll::FLOAT},
    {"BOOL", EnumAll::BOOL},
    {"TINYTEXT", EnumAll::TINYTEXT},
    {"TEXT", EnumAll::TEXT},
    {"MEDIUMTEXT", EnumAll::LONGTEXT},
    {"DATETIME", EnumAll::DATETIME},
    {"DATE", EnumAll::DATE},
    {"BIN", EnumAll::BIN},
    {"DEC", EnumAll::DEC},
    {"HEX", EnumAll::HEX},
    {"BASE58", EnumAll::BASE58},
    {"BASE64", EnumAll::BASE64},
    {"PEM", EnumAll::PEM},
    {"ENUMV", EnumAll::ENUMV},
    {"ENUMGENDER", EnumAll::ENUMGENDER},
    {"ENUMALL", EnumAll::ENUMALL},
    {"CONTRACT", EnumAll::CONTRACT},
    {"XML", EnumAll::XML}
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

std::map<std::string,std::string> VSCE_ERROR_MSG = {
    {"RUN_INPUT", "input is not met"},
    {"RUN_CONDITION", "condition is not met"},
    {"RUN_PERIOD", "runnable time period is not met"},
    {"NOT_ENOUGH_FEE", "transaction fee shortage"},
    {"NO_CONTRACT", "corresponding contract does not exist"},
    {"RUN_UNKNOWN", "unknown error occurred"},
    {"CONFIG_WORLD", "error on configuration of world and chain"},
    {"NO_USER", "error on reading user attributes"}
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
