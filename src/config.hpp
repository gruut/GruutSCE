#ifndef GRUUTSCE_CONFIG_HPP
#define GRUUTSCE_CONFIG_HPP

// common include files
#include <pugixml.hpp>
#include <json.hpp>

#include "config/types.hpp"

#include "utils/json.hpp"
#include "utils/misc_tool.hpp"
#include "utils/gruut-utils/src/sha256.hpp"
#include "utils/gruut-utils/src/type_converter.hpp"
#include "utils/gruut-utils/src/bytes_builder.hpp"
namespace gruut::gsce {

constexpr int NUM_GSCE_THREAD = 4;

}

#endif //GRUUTSCE_CONFIG_HPP
