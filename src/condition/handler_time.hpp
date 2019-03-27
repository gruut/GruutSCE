#ifndef GRUUTSCE_HANDLER_TIME_HPP
#define GRUUTSCE_HANDLER_TIME_HPP

#include "../config.hpp"
#include "condition_handler.hpp"

namespace gruut {
namespace gsce {

class TimeHandler : public ConditionHandler {
public:
  TimeHandler() = default;

  bool evalue(pugi::xml_node &document_element, Datamap &datamap) override {

    DataRecord time_data;
    if(!datamap.get("$time", time_data)) {
      return false;
    }

    uint64_t current_timestamp = vs::isotime2timestamp(time_data.value);

    std::string time_after = document_element.child("after").value();
    std::string time_before = document_element.child("before").value();

    vs::trim(time_after);
    vs::trim(time_before);

    if(time_after.empty() && time_before.empty())
        return false;

    if(time_before.empty())
        return (vs::isotime2timestamp(time_before) >= current_timestamp);

    if(time_before.empty())
        return (vs::isotime2timestamp(time_after) <= current_timestamp);

    return (vs::isotime2timestamp(time_after) <= current_timestamp && current_timestamp <= vs::isotime2timestamp(time_before));

  }
};

}
}

#endif //GRUUTSCE_HANDLER_TIME_HPP
