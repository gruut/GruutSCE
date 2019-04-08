#ifndef GRUUTSCE_TEMP_DATA_COLLECTOR_HPP
#define GRUUTSCE_TEMP_DATA_COLLECTOR_HPP

#include <string>

#include "../config.hpp"
#include "datamap.hpp"


namespace gruut::gsce {

class TempDataCollector {
private:
  std::function<DataRecord(std::string&)> m_read_storage_interface;
  std::function<void(std::string&, DataRecord&)> m_write_storage_interface;
  Datamap m_datamap;

public:
  TempDataCollector() = default;

  void attachReadInterface(std::function<DataRecord(std::string&)> read_storage_interface){
    m_read_storage_interface = std::move(read_storage_interface);
  }

  void attachWriteInterface(std::function<void(std::string&, DataRecord&)> write_storage_interface){
    m_write_storage_interface = std::move(write_storage_interface);
  }

  void addKeywords(std::vector<std::string> &ext_keywords) {
    for(auto &keyword : ext_keywords) {
      //m_datamap.set(keyword, m_read_storage_interface(keyword));
      m_datamap.set(keyword, "");
    }
  }

  void updateValue(const std::string &key, const std::string &value, bool updatable = true) {
    m_datamap.set(key,value, updatable);
  }

  template <typename D = DataRecord>
  void updateValue(const std::string &key, D && value) {
    m_datamap.set(key,value);
  }

  void flush(){
    // TODO : write all data to write interface
  }

  Datamap& getDatamap() {
    return m_datamap;
  }

};

}

#endif //GRUUTSCE_TEMP_DATA_COLLECTOR_HPP
