#ifndef TETHYS_SCE_ELEMENT_PARSER_HPP
#define TETHYS_SCE_ELEMENT_PARSER_HPP

#include "../config.hpp"
#include "../utils/pugixml_tool.hpp"

namespace tethys::tsce {


class ElementParser {
private:
  pugi::xml_node m_contract;

  pugi::xml_node m_head;
  pugi::xml_node m_body;
  pugi::xml_node m_input;

  std::vector<std::pair<pugi::xml_node,std::string>> m_gets;
  std::vector<std::pair<pugi::xml_node,std::string>> m_sets;
  std::vector<std::pair<pugi::xml_node,std::string>> m_conditions;
  std::vector<std::pair<pugi::xml_node,std::string>> m_oracles;
  std::vector<std::pair<pugi::xml_node,std::string>> m_displays;
  std::vector<std::pair<pugi::xml_node,std::string>> m_calls;
  std::vector<std::pair<pugi::xml_node,std::string>> m_fees;
  std::vector<std::pair<pugi::xml_node,std::string>> m_scripts;

  pugi::xml_node m_single_dummy;
  std::vector<std::pair<pugi::xml_node,std::string>> m_multi_dummy;

public:
  ElementParser() = default;


  // Note: The contract node lost her name before it came here. Why?

  bool setContract(pugi::xml_node &contract) {
    m_contract = contract;
    m_contract.set_name("contract"); // bug?!

    auto head = XmlTool::parseChildFrom(m_contract, "head");
    auto body = XmlTool::parseChildFrom(m_contract, "body");

    if(!head || !body) {
      return false;
    }

    m_head = head.value();
    m_body = body.value();

    auto input = XmlTool::parseChildrenFrom(m_body, "input");

    if(input.empty()) {
      return false;
    }

    m_input = input[0].first;

    m_gets = XmlTool::parseChildrenFrom(m_body, "get");
    m_sets = XmlTool::parseChildrenFrom(m_body, "set");
    m_conditions = XmlTool::parseChildrenFrom(m_body, "condition");
    m_oracles = XmlTool::parseChildrenFrom(m_body, "oracle");
    m_displays = XmlTool::parseChildrenFrom(m_body, "display");
    m_calls = XmlTool::parseChildrenFrom(m_body, "call");
    m_scripts = XmlTool::parseChildrenFrom(m_contract, "script");

    m_fees = XmlTool::parseChildrenFrom(m_contract, "fee");

    return true;
  }

  pugi::xml_node& getNode(std::string_view node_name) {

    if(node_name == "head") {
      return m_head;
    } else if(node_name == "input") {
      return m_input;
    }

    return m_single_dummy;
  }

  std::vector<std::pair<pugi::xml_node,std::string>>& getNodes(std::string_view node_name) {

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


};

}

#endif
