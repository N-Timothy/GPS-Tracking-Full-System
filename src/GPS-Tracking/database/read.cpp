#include "GPS-Tracking/database/read.hpp"

#include <iostream>

namespace karlo {
  namespace database {

      using json = nlohmann::json;

    bsoncxx::document::value generateFilterDocument (std::string imei) {
      auto builder = bsoncxx::builder::stream::document{};
      bsoncxx::document::value filterDocValue = builder
        << "imei" << imei
        << bsoncxx::builder::stream::finalize;
      return filterDocValue;
    }

    json read(mongocxx::collection collection, std::string imei) {

      bsoncxx::document::value filterDocValue = generateFilterDocument(imei);
      bsoncxx::document::view filterDocument = filterDocValue.view();

        json data;

        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
        collection.find_one(filterDocument);
        if(maybe_result) {
            data =  json::parse(bsoncxx::to_json(*maybe_result));
        }
      return data;
    }
  } // namespace database
} // namespace karlo
