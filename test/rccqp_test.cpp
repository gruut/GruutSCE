#define BOOST_TEST_MODULE rccqp_test

#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "../include/rcc_qp.hpp"


BOOST_AUTO_TEST_SUITE(rccqp_test)

BOOST_AUTO_TEST_CASE(simple_push_get) {

  tethys::RCCQP<std::string> rcc_cache;
  std::string key = "abcdefg";
  std::string data = "hello, world";

  rcc_cache.get(key);
  rcc_cache.push(key,data);
  rcc_cache.get(key);
  rcc_cache.push(key,data);
  rcc_cache.get(key);
  rcc_cache.push(key,data);
  rcc_cache.get(key);
  rcc_cache.push(key,data);
  rcc_cache.get(key);
  rcc_cache.push(key,data);
  rcc_cache.get(key);
  rcc_cache.push(key,data);
  rcc_cache.get(key);
  rcc_cache.push(key,data);
  rcc_cache.get(key);
  rcc_cache.push(key,data);
  rcc_cache.get(key);
  rcc_cache.push(key,data);


  std::cout << rcc_cache.get(key).value_or("") << std::endl;

  BOOST_TEST(rcc_cache.get(key).value_or("") == data);

}

BOOST_AUTO_TEST_SUITE_END()