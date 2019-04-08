#ifndef GRUUTSCE_DATA_COLLECTOR_HPP
#define GRUUTSCE_DATA_COLLECTOR_HPP

#include <string>

#include "../config.hpp"
#include "datamap.hpp"

namespace gruut::gsce {

class DataCollector {
private:
  Datamap m_datamap;

public:
  DataCollector() = default;

  template <typename S = std::string>
  DataRecord getVal(S &&key) {
    DataRecord ret_data;
    return ret_data;
  }

  template <typename S = std::string, typename D = DataRecord>
  void setVal(S &&key, D &&value){

  }

  void flush(){
    // TODO : flush all data to storage
  }

};

}

#endif //GRUUTSCE_DATA_COLLECTOR_HPP
