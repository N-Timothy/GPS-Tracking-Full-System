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

    json readOne(mongocxx::collection collection, std::string imei) {

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

    std::vector<json> readAll (mongocxx::collection collection){

        std::vector<json> data;

        mongocxx::cursor cursor = collection.find({});
        for(auto doc : cursor) {
            data.push_back(json::parse(bsoncxx::to_json(doc)));
        }
        return data;
    }

    bool checkImei(mongocxx::collection collection, std::string imei) {
    
      bsoncxx::document::value filterDocValue = generateFilterDocument(imei);
      bsoncxx::document::view filterDocument = filterDocValue.view();

      bool data = false;

      bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
      collection.find_one(filterDocument);
      if(maybe_result) {
          data = true;
      }
        
      return data;
        
    }

  } // namespace database
} // namespace karlo
