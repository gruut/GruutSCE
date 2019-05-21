#ifndef VERONN_SCE_HANDLER_SIGNATURE_HPP
#define VERONN_SCE_HANDLER_SIGNATURE_HPP

#include "../config.hpp"
#include "base_condition_handler.hpp"
#include <algorithm>
#include <cstring>
#include <array>

namespace veronn::vsce {

class SignatureHandler : public BaseConditionHandler {
public:
  SignatureHandler() = default;

  bool evalue(pugi::xml_node &doc_node, Datamap &datamap) override {
    auto signature_type = doc_node.attribute("type").value(); // GAMMA / ECDSA / .... default = GAMMA

    auto sig_node = doc_node.child("sig");
    auto pk_node = doc_node.child("pk");
    auto text_node = doc_node.child("text");
    if (sig_node.empty() || pk_node.empty() || text_node.empty())
      return false;

    std::string sig_val = sig_node.attribute("value").value();
    std::string sig_contents_type = sig_node.attribute("type").value();
    if (sig_val.empty() || sig_val[0] != '$')
      return false;

    std::string pk_val = sig_node.attribute("value").value();
    std::string pk_type = sig_node.attribute("type").value(); // PEM / ENCODED-PK ...
    if (pk_val.empty())
      return false;
    else if (pk_val[0] == '$') {
      auto pk_val_temp = datamap.get(pk_val);
      if (!pk_val_temp.has_value())
        return false;
      pk_val = pk_val_temp.value();
    }

    BytesBuilder bytes_builder;
    pugi::xpath_node_set txt_val_nodes = text_node.select_nodes("/val");
    for (auto &each_node : txt_val_nodes) {
      pugi::xml_node val_node = each_node.node();
      std::string val_value = val_node.attribute("value").value();
      std::string val_type = val_node.attribute("type").value();

      if (val_value.empty())
        continue;
      if (val_value[0] == '$') {
        auto temp_val = datamap.get(val_value);
        if (!temp_val.has_value())
          continue;
        val_value = temp_val.value();
      }
      appendData(val_type, val_value, bytes_builder);
    }
    return verifySig(signature_type, sig_val, pk_type, pk_val, bytes_builder.getString());
  }
private:
  void appendData(const std::string &type, const std::string &data, BytesBuilder &bytes_builder) {
    auto it = INPUT_OPTION_TYPE_MAP.find(type);
    EnumAll var_type;
    var_type = it != INPUT_OPTION_TYPE_MAP.end() ? it->second : EnumAll::NONE;

    switch (var_type) {
    case EnumAll::INT:
    case EnumAll::PINT:
    case EnumAll::NINT:
    case EnumAll::DEC: {
      bytes_builder.appendDec(data);
      break;
    }
    case EnumAll::FLOAT: {
      // TODO : need `appendFloat` function
      std::array<uint8_t, 4> bytes_arr{};
      auto float_val = std::stof(data);
      std::memcpy(bytes_arr.data(), &float_val, 4);
      bytes_builder.append(bytes_arr);
      break;
    }
    case EnumAll::BOOL: {
      vector<uint8_t> bool_data;
      if (std::all_of(data.begin(), data.end(), ::isdigit)) {
        auto num = std::stoi(data);
        (num > 0) ? bool_data.push_back(1) : bool_data.push_back(0);
      } else {
        std::string bool_str = vs::toLower(data);
        std::istringstream iss(bool_str);
        bool b;
        iss >> std::boolalpha >> b;
        b ? bool_data.push_back(1) : bool_data.push_back(0);
      }
      bytes_builder.append(bool_data);
      break;
    }
    case EnumAll::TINYTEXT:
    case EnumAll::TEXT:
    case EnumAll::LONGTEXT: {
      bytes_builder.append(data);
      break;
    }
    case EnumAll::DATETIME: {
      auto t = vs::isotime2timestamp(data);
      bytes_builder.appendDec(t);
      break;
    }
    case EnumAll::DATE: {
      auto t = vs::simpletime2timestamp(data);
      bytes_builder.appendDec(t);
      break;
    }
    case EnumAll::BIN: {
      //TODO : need `appendBin` function
      int len = data.length() / 8;
      vector<uint8_t> temp_vec;
      for (int i = 0; i < len; i++) {
        auto one_byte = (uint8_t) stoi("0b" + string(data.substr(i * 8, 8)), nullptr, 2);
        temp_vec.push_back(one_byte);
      }
      bytes_builder.append(temp_vec);
      break;
    }
    case EnumAll::HEX: {
      bytes_builder.appendHex(data);
      break;
    }
    case EnumAll::BASE58: {
      bytes_builder.appendBase<58>(data);
      break;
    }
    case EnumAll::BASE64: {
      bytes_builder.appendBase<64>(data);
      break;
    }
    case EnumAll::ENUMV: {
      int val;
      if (data == "GRU")
        val = static_cast<int>(EnumV::KEYC);
      else if (data == "FIAT")
        val = static_cast<int>(EnumV::FIAT);
      else if (data == "COIN")
        val = static_cast<int>(EnumV::COIN);
      else if (data == "XCOIN")
        val = static_cast<int>(EnumV::XCOIN);
      else
        val = static_cast<int>(EnumV::MILE);
      bytes_builder.appendDec(val);
      break;
    }
    case EnumAll::ENUMGENDER: {
      int val;
      if (data == "MALE")
        val = static_cast<int>(EnumGender::MALE);
      else if (data == "FEMALE")
        val = static_cast<int>(EnumGender::FEMALE);
      else
        val = static_cast<int>(EnumGender::OTHER);
      bytes_builder.appendDec(val);
      break;
    }
    case EnumAll::PEM:
    case EnumAll::ENUMALL:
    case EnumAll::CONTRACT:
    case EnumAll::XML: {
      //TODO : ???????
      break;
    }
    default:break;
    }
  }
  bool verifySig(std::string_view signature_type,
                 const std::string &signature,
                 std::string_view pk_type,
                 const string &pk_or_pem,
                 const std::string &msg) {
    if (pk_type == "PEM") {
      if (signature_type.empty() || signature_type == "GAMMA") {
        AGS ags;
        auto pub_key =
            ags.getPublicKeyFromPem(pk_or_pem); // currently, `getPublicKeyFromPem` function is private member. it should be public one.
        return ags.verify(pub_key, msg, signature);
      } else if (signature_type == "ECDSA") {
        std::vector<uint8_t> sig_vec(signature.begin(), signature.end());
        return ECDSA::doVerify(pk_or_pem, msg, sig_vec);
      } else { //TODO: need `RSA` verification
        return false;
      }
    } else {
      if (signature_type.empty() || signature_type == "GAMMA") {
        AGS ags;
        return ags.verify(pk_or_pem, msg, signature);
      } else if (signature_type == "ECDSA") {
        try {
          std::vector<uint8_t> vec_x(pk_or_pem.begin() + 1, pk_or_pem.begin() + 33);
          std::vector<uint8_t> vec_y(pk_or_pem.begin() + 33, pk_or_pem.end());
          auto point_x = Botan::BigInt::decode(vec_x);
          auto point_y = Botan::BigInt::decode(vec_y);
          Botan::EC_Group group_domain("secp256k1");

          Botan::ECDSA_PublicKey public_key(group_domain, group_domain.point(point_x, point_y));
          std::vector<uint8_t> msg_vec(msg.begin(), msg.end());
          std::vector<uint8_t> sig_vec(signature.begin(), signature.end());
          return ECDSA::doVerify(public_key, msg_vec, sig_vec);
        } catch (...) {
          return false;
        }
      } else { //TODO: need `RSA` verification
        return false;
      }
    }
  }
};

}

#endif
