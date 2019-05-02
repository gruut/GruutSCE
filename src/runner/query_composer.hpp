#ifndef GRUUTSCE_QUERY_COMPOSER_HPP
#define GRUUTSCE_QUERY_COMPOSER_HPP

#include "../config.hpp"


namespace gruut::gsce {

class QueryComposer {
public:
  QueryComposer() = default;

  nlohmann::json compose(std::vector<nlohmann::json> &result_queries) {

    // TODO : compose result queries


    return nlohmann::json();
  }

};

}

#endif //GRUUTSCE_QUERY_COMPOSER_HPP
