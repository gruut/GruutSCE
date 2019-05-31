#define BOOST_TEST_MODULE runner_test

#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "../src/engine.hpp"

BOOST_AUTO_TEST_SUITE(runner_test)

BOOST_AUTO_TEST_CASE(simple_run) {

  std::function<nlohmann::json(nlohmann::json&)> read_storage_interface = [&](nlohmann::json& query){

    nlohmann::json result = R"({
        "name": [],
        "data": []
    })"_json;

    std::string query_type = JsonTool::get<std::string>(query,"type").value_or("");

    if(query_type == "world.get") {

      result["name"].emplace_back("world_id");
      result["name"].emplace_back("created_time");
      result["name"].emplace_back("creator_id");
      result["name"].emplace_back("creator_pk");
      result["name"].emplace_back("authority_id");
      result["name"].emplace_back("authority_pk");
      result["name"].emplace_back("keyc_name");
      result["name"].emplace_back("keyc_initial_amount");
      result["name"].emplace_back("allow_mining");
      result["name"].emplace_back("mining_rule");
      result["name"].emplace_back("allow_anonymous_user");
      result["name"].emplace_back("join_fee");

      nlohmann::json record = {"TETHYS19","0","5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF","","5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF","","THY","100000000000","false","","true","10"};

      result["data"].emplace_back(record);

    } else if(query_type == "chain.get") {

      result["name"].emplace_back("chain_id");
      result["name"].emplace_back("created_time");
      result["name"].emplace_back("creator_id");
      result["name"].emplace_back("creator_pk");
      result["name"].emplace_back("allow_custom_contract");
      result["name"].emplace_back("allow_oracle");
      result["name"].emplace_back("allow_tag");
      result["name"].emplace_back("allow_heavy_contract");

      nlohmann::json record = {"SEOUL@KR", "1", "5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF", "", "false", "false", "false", "false"};

      result["data"].emplace_back(record);

    } else if(query_type == "contract.get") {

      std::string test_contract = R"(<contract>
  <head>
    <cid>VALUE-TRANSFER::5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF::SEOUL@KR::TETHYS19</cid>
    <after>2018-01-01T00:00:00+09:00</after>
    <desc>Official standardcontract for transfering v-type variable</desc>
  </head>
  <body>
    <input>
      <option name="amount" type="PINT" desc="amount of v-type variable for transfer" />
      <option name="unit" type="ENUMV" desc="unit type of v-type variable for transfer" />
      <option name="pid" type="BASE64" desc="id of v-type variable for transfer" />
      <option name="tag" type="XML" desc="condition for use of this v-type value" />
    </input>
    <set type="v.transfer" from="user">
      <option name="to" value="$receiver" />
      <option name="amount" value="$0.amount" />
      <option name="unit" value="$0.unit" />
      <option name="pid" value="$0.pid" />
      <option name="tag" value="$0.tag" />
    </set>
  </body>
  <fee>
    <pay from="user" value="$fee" />
  </fee>
</contract>)";

      result["name"].emplace_back("contract");

      nlohmann::json record = nlohmann::json::array();
      record.emplace_back(test_contract);

      result["data"].emplace_back(record);

    } else if(query_type == "user.info.get") {

      result["name"].emplace_back("register_day");
      result["name"].emplace_back("register_code");
      result["name"].emplace_back("gender");
      result["name"].emplace_back("isc_type");
      result["name"].emplace_back("isc_code");
      result["name"].emplace_back("location");
      result["name"].emplace_back("age_limit");

      nlohmann::json record = {"1980-08-15", "", "MALE", "", "", "", ""};

      result["data"].emplace_back(record);
    } else if(query_type == "user.scope.get") {

      std::cout << query << std::endl;

      result["name"].emplace_back("var_name");
      result["name"].emplace_back("var_value");
      result["name"].emplace_back("var_type");
      result["name"].emplace_back("up_time");
      result["name"].emplace_back("up_block");
      result["name"].emplace_back("tag");
      result["name"].emplace_back("pid");

      if(JsonTool::get<std::string>(query["where"],"name").value_or("") == "KEYC" && JsonTool::get<std::string>(query["where"],"type").value_or("") == "KEYC" && JsonTool::get<bool>(query["where"],"notag").value_or(false)) {
        nlohmann::json record = {"KEYC", "1000", "KEYC", "0", "0", "", "", "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIDE="};
        result["data"].emplace_back(record);
      }

    } else {

      std::cout << query << std::endl;

    }

    return result;
  };

  tethys::tsce::ContractEngine test_engine;
  test_engine.attachReadInterface(read_storage_interface);

  nlohmann::json msg_block = R"({
    "btime": "1559192461",
    "block": {
      "id": "testblock_id",
      "time": "1559191461",
      "world": "TETHYS19",
      "chain": "SEOUL@KR",
      "height": "2",
      "previd": "",
      "link": "",
      "hash": ""
    },
    "tx": [],
    "state": {},
    "signer" : [],
    "certificate" : [],
    "producer" : {}
  })"_json;

  nlohmann::json msg_tx = R"({
    "txid": "5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF",
    "time": "1559191460",
    "body": {
      "cid": "VALUE-TRANSFER::5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF::SEOUL@KR::TETHYS19",
      "receiver": "5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF",
      "fee": "10",
      "input": [
        [
          {"amount": "100"},
          {"unit": "KEYC"},
          {"pid": ""},
          {"tag": ""}
        ]
      ]
    },
    "user": {
      "id": "5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF",
      "pk": "",
      "a": "",
      "z": ""
    },
    "endorser": [
      {
        "id": "5g9CMGLSXbNAKJMbWqBNp7rm78BJCMKhLzZVukBNGHSF",
        "pk": "",
        "a": "",
        "z": ""
      }
    ]
  })"_json;

  msg_block["tx"].emplace_back(TypeConverter::encodeBase<64>(nlohmann::json::to_cbor(msg_tx)));

  auto result_query = test_engine.procBlock(msg_block);

  if(!result_query) {
    BOOST_TEST(false);
  }

  std::cout << result_query.value() << std::endl;

  BOOST_TEST(true);
}

BOOST_AUTO_TEST_SUITE_END()