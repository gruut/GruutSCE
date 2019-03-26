#ifndef GRUUTSCE_VS_TOOL_HPP
#define GRUUTSCE_VS_TOOL_HPP

#include <string>
#include <algorithm>
#include <map>

namespace gruut {
namespace gsce {
class vs {
public:
  static std::string toLower(const std::string &src) {
    std::string dst("",src.size());
    std::transform(src.begin(), src.end(), dst.begin(), ::tolower);
    return dst;
  }
};
}
}

#endif //GRUUTSCE_VS_TOOL_HPP
