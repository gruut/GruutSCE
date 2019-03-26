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
  BOOL
};

enum class EvalRuleType : int {
  AND,
  OR
};


}}

#endif //GRUUTSCE_TYPES_HPP
