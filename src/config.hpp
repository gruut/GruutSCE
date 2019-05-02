#ifndef GRUUTSCE_CONFIG_HPP
#define GRUUTSCE_CONFIG_HPP

// common include files
#include <pugixml.hpp>
#include <json.hpp>

#include "config/types.hpp"

#include "utils/json.hpp"
#include "utils/misc_tool.hpp"
#include "utils/bytes_builder.hpp"
#include "utils/type_converter.hpp"
#include "utils/sha256.hpp"

namespace gruut::gsce {

constexpr int NUM_GSCE_THREAD = 4;

}

#endif //GRUUTSCE_CONFIG_HPP
