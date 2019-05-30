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
#include "../include/pugixml.hpp"
#include "../include/json.hpp"
#include "../include/date.hpp"

#include "config/types.hpp"

#include "utils/json.hpp"
#include "utils/misc_tool.hpp"
#include "utils/gruut-utils/src/sha256.hpp"
#include "utils/gruut-utils/src/type_converter.hpp"
#include "utils/gruut-utils/src/bytes_builder.hpp"
#include "utils/gruut-utils/src/ecdsa.hpp"
#include "utils/gruut-utils/src/ags.hpp"

namespace tethys::tsce {

constexpr int NUM_GSCE_THREAD = 4;

}

#endif //GRUUTSCE_CONFIG_HPP
