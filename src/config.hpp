#ifndef TETHYS_SCE_CONFIG_HPP
#define TETHYS_SCE_CONFIG_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <optional>
#include <regex>
#include <cctype>
#include <unordered_map>

// common include files
#include "../include/tinyxml2.h"
#include "../include/json.hpp"
#include "../include/date.hpp"

#include "config/types.hpp"

#include "utils/json.hpp"
#include "utils/misc_tool.hpp"
#include "../tethys-utils/src/sha256.hpp"
#include "../tethys-utils/src/type_converter.hpp"
#include "../tethys-utils/src/bytes_builder.hpp"
#include "../tethys-utils/src/ecdsa.hpp"
#include "../tethys-utils/src/ags.hpp"

namespace tethys::tsce {

constexpr int NUM_GSCE_THREAD = 4;
constexpr int MAX_INPUT_SIZE = 5;
constexpr int MIN_USER_FEE = 10;

}

#endif //GRUUTSCE_CONFIG_HPP
