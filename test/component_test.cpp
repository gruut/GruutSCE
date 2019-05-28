#define BOOST_TEST_MODULE component_test

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "../src/config.hpp"
#include "../src/condition/condition_manager.hpp"
#include "../src/data/data_manager.hpp"

using namespace tethys::tsce;

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

  std::string endorser_condition_2 = R"(
<condition id="test">
  <endorser eval-rule="and">
    <id>bt1fDSTLzc1AdSoxXTy6bfAG5NZFHJBThEHjjWYHN2H</id>
    <id>bt1fDSTLzc1AdSoxXTy6bfAG5NZFHJBThEHjjWYHN3H</id>
  </endorser>
</condition>
)";

  pugi::xml_document doc;
  pugi::xml_document doc2;

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

  if (doc2.load_string(endorser_condition_2.c_str(), pugi::parse_minimal)) {
    pugi::xml_node doc_node = doc2.document_element();

    ConditionManager condition_manager;
    DataManager data_manager;

    data_manager.updateValue("$tx.endorser.count", "1");
    data_manager.updateValue("$tx.endorser[0].id", "bt1fDSTLzc1AdSoxXTy6bfAG5NZFHJBThEHjjWYHN2H");

    BOOST_TEST(!condition_manager.evalue(doc_node,data_manager));

  } else {
    BOOST_TEST(false);
  }

}

BOOST_AUTO_TEST_CASE(simple_time) {

  std::string time_condition = R"(
<condition id="test">
  <time>
    <after>2019-01-01T00:00:01+00:00</after>
    <before>2019-02-01T23:59:59+00:00</before>
  </time>
</condition>
)";

  pugi::xml_document doc;

  if (doc.load_string(time_condition.c_str(), pugi::parse_minimal)) {
    pugi::xml_node doc_node = doc.document_element();

    ConditionManager condition_manager;
    DataManager data_manager;

    data_manager.updateValue("$time", "2019-01-02");

    BOOST_TEST(condition_manager.evalue(doc_node,data_manager));

  } else {
    BOOST_TEST(false);
  }

}
BOOST_AUTO_TEST_SUITE_END()