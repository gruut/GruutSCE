#ifndef TETHYS_SCE_TAG_HANDLER_HPP
#define TETHYS_SCE_TAG_HANDLER_HPP

#include "../config.hpp"
#include "../data/data_manager.hpp"
#include "condition_handler.hpp"

#include <algorithm>
#include <iostream>
#include <map>

namespace tethys::tsce {

class TagHandler {
  pugi::xml_node m_info_node;
  pugi::xml_node m_update_node;

public:
  TagHandler() = default;

  bool evalue(std::string &tag_str, DataManager &data_manager)  {
    pugi::xml_document doc;
    if (!doc.load_string(tag_str.c_str(), pugi::parse_minimal))
      return false;

    pugi::xml_node doc_node = doc.document_element();
    return evalue(doc_node, data_manager);
  }

  bool evalue(pugi::xml_node &doc_node, DataManager &data_manager) {

    m_info_node = doc_node.first_element_by_path("./info");
    m_update_node = doc_node.first_element_by_path("./update");
    ConditionHandler condition_handler;

    return condition_handler.evalue(m_update_node,data_manager);
  }

  std::string getName() {
    return m_info_node.first_element_by_path("./name").text().as_string();
  }

  std::vector<std::string> getCword(){
    std::vector<std::string> ret_vec;
    auto cword_node = XmlTool::parseChildrenFromNoIf(m_info_node,"cword");
    for(auto &each_node : cword_node){
      ret_vec.emplace_back(each_node.text().as_string());
    }

    return ret_vec;
  }
};

}

#endif
