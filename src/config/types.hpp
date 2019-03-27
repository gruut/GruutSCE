// Please, do not include this file directly.
// Instead, you can include this file by including config.hpp

#ifndef GRUUTSCE_TYPES_HPP
#define GRUUTSCE_TYPES_HPP

#include <string>

namespace gruut {
namespace gsce {

enum class VariableTyp : int {
  INT,
  FLOAT,
  BOOL,
  TEXT
};

enum class EvalRuleType : int {
  AND,
  OR
};

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
  VAR,
  UNKNOWN
};

enum class SecondaryConditionType : int {
  USER,
  RECEIVER,
  IF,
  NIF,
  ID,
  LOCATION,
  SERVICE,
  AGE,
  UNKNOWN
};



}}

#endif //GRUUTSCE_TYPES_HPP
