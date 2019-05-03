#ifndef GRUUTSCE_HANDLER_TIME_HPP
#define GRUUTSCE_HANDLER_TIME_HPP

#include "../config.hpp"
#include "base_condition_handler.hpp"

namespace gruut::gsce {

class TimeHandler : public BaseConditionHandler {
public:
  TimeHandler() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {

    auto data = datamap.get("$time");
    if(!data.has_value()) {
      return false;
    }

    uint64_t current_timestamp = vs::isotime2timestamp(data.value());

    std::string time_after = doc_node.child("after").text().as_string();
    std::string time_before = doc_node.child("before").text().as_string();

    vs::trim(time_after);
    vs::trim(time_before);

    if(time_after.empty() && time_before.empty())
        return false;

    if(time_before.empty())
        return (vs::isotime2timestamp(time_before) > current_timestamp);

    if(time_before.empty())
        return (vs::isotime2timestamp(time_after) < current_timestamp);

    return (vs::isotime2timestamp(time_after) < current_timestamp && current_timestamp < vs::isotime2timestamp(time_before));

  }
};

}

#endif //GRUUTSCE_HANDLER_TIME_HPP
