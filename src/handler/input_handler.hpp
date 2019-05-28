#ifndef TETHYS_SCE_INPUT_HANDLER_HPP
#define TETHYS_SCE_INPUT_HANDLER_HPP

#include "../config.hpp"
#include "../data/datamap.hpp"
#include "../data/data_manager.hpp"
#include <botan-2/botan/x509cert.h>
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include <regex>

namespace tethys::tsce {

constexpr int INT_LENGTH = 17;
constexpr long long MAX_INT = 9007199254740991;
constexpr long long MIN_INT = -9007199254740991;
constexpr int TINYTEXT_LEN = 255;
constexpr int TEXT_LEN = 65535;
constexpr int MEDIUMTEXT_LEN = 16777215;
const auto BOOL_REGEX = "^([Tt][Rr][Uu][Ee]|[Ff][Aa][Ll][Ss][Ee])$";
const auto DATE_REGEX = "([12]\\d{3}-(0[1-9]|1[0-2])-(0[1-9]|[12]\\d|3[01]))";
const auto DATE_TIME_REGEX = "(19|20)[0-9][0-9]-(0[0-9]|1[0-2])-(0[1-9]|([12][0-9]|3[01]))T([01][0-9]|2[0-3]):[0-5][0-9]:[0-5][0-9](\\+|-)[0-1][0-9]:[00]";
const auto BASE64_REGEX = "^(?:[A-Za-z0-9+/]{4})*(?:[A-Za-z0-9+/]{3}=|[A-Za-z0-9+/]{2}==)?$";
const auto BASE58_REGEX = "^[A-HJ-NP-Za-km-z1-9]*$";
const auto BIN_REGEX = "^[0-1]*$";

struct InputOption {
  std::string key;
  std::string validation;
  std::string type;
  InputOption(std::string key_, std::string validation_, std::string type_)
      : key(std::move(key_)), validation(std::move(validation_)), type(std::move(type_)) {
  }
};

class InputHandler {
public:
  InputHandler() = default;


  bool parseInput(nlohmann::json &input_json, pugi::xml_node &input_node, DataManager &data_collector){

    if(input_json.empty())
      return false;

    std::string max_str = input_node.attribute("max").value();
    int num_max = tt::str2num<int>(max_str);
    num_max = (input_json.size() < num_max) ? input_json.size() : num_max;

    if(num_max > 10)
      return false;

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

    for(int i = 0; i < num_max; ++i) {

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
            Botan::DataSource_Memory cert_datasource(input_value);
            Botan::X509_Certificate cert(cert_datasource);
            auto not_after_x509 = cert.not_after();
            auto not_before_x509 = cert.not_before();
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

    return true;
  }

private:
  template <typename S = std::string>
  bool isValidValue(S &&value, S &&type, S &&validation){
    auto it = INPUT_OPTION_TYPE_MAP.find(type);
    auto var_type = (it == INPUT_OPTION_TYPE_MAP.end() ? EnumAll::TEXT : it->second); // default is TEXT
    try {
      switch (var_type) {
      case EnumAll::INT: {
        if (value.length() > INT_LENGTH)
          return false;
        auto val = std::stoll(value);
        if(!(val >= MIN_INT && val <= MAX_INT))
          return false;
        break;
      }
      case EnumAll::PINT: {
        if (value.length() > INT_LENGTH)
          return false;
        auto val = std::stoll(value);
        if(!(val >= 1 && val <= MAX_INT))
          return false;
        break;
      }
      case EnumAll::NINT: {
        if (value.length() > INT_LENGTH)
          return false;
        auto val = std::stoll(value);
        if(!(val >= MIN_INT && val <= -1))
          return false;
        break;
      }
      case EnumAll::FLOAT: {
        std::stod(value);
        break;
      }
      case EnumAll::BOOL: {
        std::regex rgx(BOOL_REGEX);
        if (regex_match(value, rgx))
          return true;
        auto val = std::stoi(value);
        if(val < 0 )
          return false;
        break;
      }
      case EnumAll::TINYTEXT: {
        if(value.length() > TINYTEXT_LEN)
          return false;
        break;
      }
      case EnumAll::TEXT: {
        if(value.length() > TEXT_LEN)
          return false;
        break;
      }
      case EnumAll::MEDIUMTEXT: {
        if(value.length() > MEDIUMTEXT_LEN)
          return false;
        break;
      }
      case EnumAll::DATE: {
        std::regex rgx(DATE_REGEX);
        if(!std::regex_match(value, rgx))
          return false;
        break;
      }
      case EnumAll::DATETIME: {
        std::regex rgx(DATE_TIME_REGEX);
        if(!std::regex_match(value, rgx))
          return false;
        break;
      }
      case EnumAll::BIN: {
        std::regex rgx(BIN_REGEX);
        if(!std::regex_match(value, rgx))
          return false;
        break;
      }
      case EnumAll::DEC: {
        if(!std::all_of(value.begin(), value.end(), ::isdigit))
          return false;
        break;
      }
      case EnumAll::HEX: {
        if(!std::all_of(value.begin(), value.end(), ::isxdigit))
          return false;
        break;
      }
      case EnumAll::BASE58: {
        std::regex rgx(BASE58_REGEX);
        if(!std::regex_match(value, rgx))
          return false;
        break;
      }
      case EnumAll::BASE64: {
        std::regex rgx(BASE64_REGEX);
        if(!std::regex_match(value, rgx))
          return false;
        break;
      }
      case EnumAll::ENUMV: {
        if(!(value == "GRU" || value == "FIAT" || value == "COIN" || value == "XCOIN"))
          return false;
        break;
      }
      case EnumAll::ENUMGENDER: {
        if(!(value == "MALE" || value == "FEMALE" || value == "OTHER"))
          return false;
        break;
      }
      case EnumAll::ENUMALL: {
        //TODO : handle `ENUMALL` type
        break;
      }
      case EnumAll::PEM: {
        string begin_str = "-----BEGIN CERTIFICATE-----";
        string end_str = "-----END CERTIFICATE-----";
        auto found1 = value.find(begin_str);
        auto found2 = value.find(end_str);
        if(found1 == string::npos || found2 ==string::npos) {
          return false;
        }
        auto content_len = value.length() - begin_str.length() - end_str.length();
        auto content = value.substr(begin_str.length(), content_len);
        std::regex rgx(BASE64_REGEX);
        if(!std::regex_match(value, rgx))
          return false;
        break;
      }
      case EnumAll::XML:
      case EnumAll::CONTRACT: {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(value.data());
        if(!result)
          return false;
        if(var_type == EnumAll::XML)
          break;
        else{

          // TODO : enhance contract checking

          std::string check_begin = "<contract";
          std::string check_end = "</contract>";
          auto begin_str = value.substr(0, check_begin.length());
          auto end_str = value.substr(value.length() - check_end.length(), check_end.length());
          if(check_begin != begin_str || check_end != end_str)
            return false;
          break;
        }
      }
      default:
        return false;
      }
      if(!validation.empty()){
        std::regex rgx(validation);
        return std::regex_match(value, rgx);
      }
      return true;
    } catch(...){  // when exceptions occur //ex) stoll, stod, stoi ...
      return false;
    }
  }
};

}

#endif
