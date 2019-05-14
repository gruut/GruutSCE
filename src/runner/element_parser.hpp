#ifndef VERONN_SCE_ELEMENT_PARSER_HPP
#define VERONN_SCE_ELEMENT_PARSER_HPP

#include "../config.hpp"

namespace veronn::vsce {

class ElementParser {
private:
  std::pair<pugi::xml_node,std::string> m_contract;
  std::pair<pugi::xml_node,std::string> m_head;
  std::pair<pugi::xml_node,std::string> m_body;
  std::pair<pugi::xml_node,std::string> m_input;
  std::vector<std::pair<pugi::xml_node,std::string>> m_gets;
  std::vector<std::pair<pugi::xml_node,std::string>> m_sets;
  std::vector<std::pair<pugi::xml_node,std::string>> m_conditions;
  std::vector<std::pair<pugi::xml_node,std::string>> m_oracles;
  std::vector<std::pair<pugi::xml_node,std::string>> m_displays;
  std::vector<std::pair<pugi::xml_node,std::string>> m_calls;
  std::vector<std::pair<pugi::xml_node,std::string>> m_fees;
  std::vector<std::pair<pugi::xml_node,std::string>> m_scripts;

  std::pair<pugi::xml_node,std::string> m_single_dummy;
  std::vector<std::pair<pugi::xml_node,std::string>> m_multi_dummy;

public:
  ElementParser() = default;

  void setContract(pugi::xml_node &contract) {
    m_contract = {contract, contract.attribute("if").value()};

    m_head = parseElement("/contract/head");
    m_body = parseElement("/contract/body");
    m_input = parseElement("/contract/body/input");

    m_gets = parseElements("/contract/body/get");
    m_sets = parseElements("/contract/body/set");
    m_conditions = parseElements("/contract/body/condition");
    m_oracles = parseElements("/contract/body/oracle");
    m_displays = parseElements("/contract/body/display");
    m_calls = parseElements("/contract/body/call");
    m_scripts = parseElements("/contract/script");
    m_fees = parseElements("/contract/fee");
  }

  template <typename S = std::string>
  std::pair<pugi::xml_node,std::string> &getNode(S &&node_name) {

    if(node_name == "head") {
      return m_head;
    } else if(node_name == "body") {
      return m_body;
    } else if(node_name == "input") {
      return m_input;
    }

    return m_single_dummy;

  }

  template <typename S = std::string>
  std::vector<std::pair<pugi::xml_node,std::string>>& getNodes(S &&node_name) {

    if(node_name == "get") {
      return m_gets;
    } else if(node_name == "set") {
      return m_sets;
    } else if(node_name == "condition") {
      return m_conditions;
    } else if(node_name == "oracle") {
      return m_oracles;
    } else if(node_name == "display") {
      return m_displays;
    } else if(node_name == "call") {
      return m_calls;
    } else if(node_name == "fee") {
      return m_fees;
    } else if(node_name == "script") {
      return m_scripts;
    }

    return m_multi_dummy;
  }

private:
  std::pair<pugi::xml_node,std::string> parseElement(const std::string &xpath) {
    pugi::xml_node t_node = m_contract.first.first_element_by_path(xpath.c_str());
    return {t_node, t_node.attribute("if").value() };
  }

  std::vector<std::pair<pugi::xml_node,std::string>> parseElements(const std::string &xpath) {

    std::vector<std::pair<pugi::xml_node,std::string>> node_set;
    pugi::xpath_node_set select_node_set = m_contract.first.select_nodes(xpath.c_str());
    for(auto &each_node : select_node_set) {
      std::string if_str = each_node.node().attribute("if").value();
      node_set.emplace_back(std::make_pair(each_node.node(),if_str));
    }

    return node_set;
  }

};

}

#endif
