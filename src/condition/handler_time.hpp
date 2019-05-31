#ifndef TETHYS_SCE_HANDLER_TIME_HPP
#define TETHYS_SCE_HANDLER_TIME_HPP

#include "../config.hpp"
#include "base_condition_handler.hpp"

namespace tethys::tsce {

class TimeHandler : public BaseConditionHandler {
public:
  TimeHandler() = default;

  bool evalue(pugi::xml_node &doc_node, DataManager &data_manager) override {

    auto timestamp = data_manager.evalOpt("$time");
    if(!timestamp)
      return false;

    uint64_t base_timestamp = MiscTool::str2num<uint64_t>(timestamp.value()) * 1000;

    std::string time_after = doc_node.child("after").text().as_string();
    std::string time_before = doc_node.child("before").text().as_string();

    MiscTool::trim(time_after);
    MiscTool::trim(time_before);

    if(time_after.empty() && time_before.empty())
        return false;

    if(time_before.empty())
        return (MiscTool::timestr2timestamp(time_after) < base_timestamp);

    if(time_after.empty())
        return (MiscTool::timestr2timestamp(time_before) > base_timestamp);

    return (MiscTool::timestr2timestamp(time_after) < base_timestamp && base_timestamp < MiscTool::timestr2timestamp(time_before));

  }
};

}

#endif
