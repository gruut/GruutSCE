#ifndef TETHYSSCE_PUGIXML_TOOL_HPP
#define TETHYSSCE_PUGIXML_TOOL_HPP

#include <string>
#include <optional>

#include <pugixml.hpp>
#include <vector>

class XmlTool {
public:

  static std::optional<pugi::xml_node> parseChildFrom(pugi::xml_node &from_node, std::string_view element_name) {

    pugi::xml_node parsed_child;
    bool found = false;

    for(auto &each_child : from_node){
      if(each_child.name() == element_name) {
        parsed_child = each_child;
        found = true;
        break;
      }
    }

    if(found) {
      return parsed_child;
    }
    else {
      return std::nullopt;
    }
  }

  static std::vector<std::pair<pugi::xml_node,std::string>> parseChildrenFrom(pugi::xml_node &from_node,
                                                                              std::string_view element_name) {

    std::vector<std::pair<pugi::xml_node,std::string>> parsed_children;

    for(auto &each_child : from_node){
      if(each_child.name() == element_name) {
        parsed_children.push_back({each_child,each_child.attribute("if").value()});
      }
    }

    return parsed_children;
  }

  static std::vector<pugi::xml_node> parseChildrenFromNoIf(pugi::xml_node &from_node, std::string_view element_name) {

    std::vector<pugi::xml_node> parsed_children;

    for(auto &each_child : from_node){
      if(each_child.name() == element_name) {
        parsed_children.push_back(each_child);
      }
    }

    return parsed_children;
  }
};

#endif //TETHYSSCE_PUGIXML_TOOL_HPP
