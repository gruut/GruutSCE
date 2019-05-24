#define BOOST_TEST_MODULE component_test

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../src/config.hpp"
#include "../src/condition/condition_manager.hpp"
#include "../src/data/data_manager.hpp"

using namespace veronn::vsce;

BOOST_AUTO_TEST_SUITE(test_compare_handler)
BOOST_AUTO_TEST_CASE(simple_compare) {

  std::string compare_condition = R"(
<condition id="test">
  <compare src="$tt" ref="$tt2" type="EQ" />
</condition>
)";

  pugi::xml_document doc;

  if (doc.load_string(compare_condition.c_str(), pugi::parse_minimal)) {
    pugi::xml_node doc_node = doc.document_element();

    ConditionManager condition_manager;
    DataManager data_manager;

    data_manager.updateValue("$tt","20");
    data_manager.updateValue("$tt2","20");

    BOOST_TEST(condition_manager.evalue(doc_node,data_manager));

  } else {
    BOOST_TEST(false);
  }
}

BOOST_AUTO_TEST_CASE(simple_endorser) {

  std::string endorser_condition = R"(
<condition id="test">
  <endorser>
    <if eval-rule="or">
      <id>bt1fDSTLzc1AdSoxXTy6bfAG5NZFHJBThEHjjWYHN2H</id>
      <id>bt1fDSTLzc1AdSoxXT120380128309808080jWYHN2H</id>
    </if>
  </endorser>
</condition>
)";

  pugi::xml_document doc;

  if (doc.load_string(endorser_condition.c_str(), pugi::parse_minimal)) {
    pugi::xml_node doc_node = doc.document_element();

    ConditionManager condition_manager;
    DataManager data_manager;

    data_manager.updateValue("$tx.endorser.count", "1");
    data_manager.updateValue("$tx.endorser[0].id", "bt1fDSTLzc1AdSoxXTy6bfAG5NZFHJBThEHjjWYHN2H");

    BOOST_TEST(condition_manager.evalue(doc_node,data_manager));

  } else {
    BOOST_TEST(false);
  }

}
BOOST_AUTO_TEST_SUITE_END()