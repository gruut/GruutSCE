#ifndef GRUUTSCE_INPUT_HANDLER_HPP
#define GRUUTSCE_INPUT_HANDLER_HPP

#include "../config.hpp"
#include "../data/datamap.hpp"
#include "../data/data_storage.hpp"
#include <botan-2/botan/x509cert.h>

namespace gruut::gsce {

class InputHandler {
public:
  InputHandler() = default;


  void parseInput(nlohmann::json &input_json, pugi::xml_node &input_node, DataStorage &data_collector){

    struct InputOption {
      std::string key;
      std::string validation;
      std::string type;
      InputOption(std::string key_, std::string validation_, std::string type_)
        : key(std::move(key_)), validation(std::move(validation_)), type(std::move(type_)) {
      }
    };

    std::string allow_multi_str = input_node.attribute("allow-multi").value();
    bool is_allow_multi = (vs::toLower(allow_multi_str) == "true");

    pugi::xpath_node_set option_nodes = input_node.select_nodes("/option");

    std::vector<InputOption> input_options;
    std::map<std::string, std::vector<std::string>> input_value_groups;

    for(auto &each_node : option_nodes) {
      pugi::xml_node option_node = each_node.node();
      std::string option_name = option_node.attribute("name").value();
      std::string option_type = option_node.attribute("type").value();
      std::string option_validation = option_node.attribute("validation").value();

      input_options.emplace_back(option_name,option_type,option_validation); // TODO : check work well
    }

    for(int i = 0; i < input_json.size(); ++i) {

      if(!is_allow_multi && i >= 1)
        break;

      for(auto &each_input : input_json[i]) {
        for(auto &each_item : each_input.items()) {
          std::string input_key = each_item.key();
          std::string input_value = each_item.value();

          bool is_valid_key = false;

          int opt_idx = -1;
          for(int j = 0; j < input_options.size(); ++j) {
            if(input_options[j].key == input_key) {
              is_valid_key = true;
              opt_idx = j;
              break;
            }
          }

          if(!is_valid_key || !isValidValue(input_value,input_options[opt_idx].type,input_options[opt_idx].validation))
            continue;

          std::string data_key = "$tx.contract.input[" + to_string(i) + "]." + input_key;
          std::string short_key = "$" + to_string(i) + "." + input_key;

          data_collector.updateValue(data_key,input_value);
          data_collector.updateValue(short_key,input_value);

          input_value_groups[input_key].emplace_back(input_value);

          if(input_options[opt_idx].type == "PEM") {
            Botan::DataSource_Memory cert_datasource(input_value);\
            Botan::X509_Certificate cert(cert_datasource);
            auto not_after_x509 = cert.not_after();
            auto not_before_x509 = cert.not_befre();
            auto not_after_epoch = not_after_x509.time_since_epoch();
            auto not_before_epoch = not_before_x509.time_since_epoch();
            std::string not_after_str = to_string(not_after_epoch);
            std::string not_before_str = to_string(not_before_epoch);

            auto data_key_prefix = "$tx.contract.input[" + to_string(i) + "].";
            auto short_key_prefix ="$" + to_string(i) + ".";
            data_collector.updateValue(data_key_prefix + "notafter", not_after_str);
            data_collector.updateValue(short_key_prefix + "notafter", not_after_str);
            data_collector.updateValue(data_key_prefix + "notbefore", not_before_str);
            data_collector.updateValue(short_key_prefix + "notbefore", not_before_str);

            auto sn = cert.serial_number();
            std::string sn_str(sn.begin(), sn.end());
            data_collector.updateValue(data_key_prefix + "sn", sn_str);
            data_collector.updateValue(short_key_prefix + "sn", sn_str);
          }
        }
      }
    }

    for(auto &it_map : input_value_groups) {
      std::string data_key = "$input@" + it_map.first;
      std::string data_value = nlohmann::json(it_map.second).dump();
      data_collector.updateValue(data_key,data_value);
    }
  }

private:

  template <typename S = std::string>
  bool isValidValue(S &&value, S &&type, S &&validation){
    // TODO : validate value according to option_type and option_validate

    return true;
  }


};

}

#endif //GRUUTSCE_INPUT_HANDLER_HPP
