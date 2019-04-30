#ifndef GRUUTSCE_DATA_STORAGE_HPP
#define GRUUTSCE_DATA_STORAGE_HPP

#include <string>

#include "../config.hpp"
#include "datamap.hpp"

namespace gruut::gsce {

class DataStorage {
private:
  Datamap m_tx_datamap;
  Datamap m_datamap;

  std::function<DataRecord(std::string&)> m_read_storage_interface;
  std::function<void(std::string&, DataRecord&)> m_write_storage_interface;

public:
  DataStorage() = default;

  void attachReadInterface(std::function<DataRecord(std::string&)> read_storage_interface){
    m_read_storage_interface = std::move(read_storage_interface);
  }

  void attachWriteInterface(std::function<void(std::string&, DataRecord&)> write_storage_interface){
    m_write_storage_interface = std::move(write_storage_interface);
  }

  template <typename S1 = std::string, typename S2 = std::string>
  void updateValue(S1 &&key,S2 &value, bool updatable = true) {
    m_tx_datamap.set(key,value, updatable);
  }

  template <typename S = std::string, typename D = DataRecord>
  void updateValue(S &&key, D && value) {
    m_tx_datamap.set(key,value);
  }

  template <typename S1 = std::string, typename S2 = std::string, typename S3 = std::string>
  std::vector<std::pair<std::string,std::string>> getVariables(S1 && scope, S2 && id, S3 && name){

  }

  template <typename S = std::string>
  std::string eval(S && key){
    std::string ret_val;
    if(key.empty())
      return ret_val;

    if(key[0] != '$') {
      return key;
    }

    return m_tx_datamap.get(key);
  }

  void flush(){
    // TODO : write all data to write interface
  }

  void flushToStorage(){
    // TODO : flush all data to storage
  }

  void clear(){
    m_tx_datamap.clear();
  }

  Datamap& getDatamap() {
    return m_tx_datamap;
  }

};

}

#endif //GRUUTSCE_DATA_STORAGE_HPP
