#include "GPS-Tracking/database/read.hpp"

#include <iostream>

namespace karlo {
  namespace database {
    json read(mongocxx::collection collection) {
      mongocxx::cursor cursor = collection.find({});
      json data;
      for(auto doc : cursor) {
          data = bsoncxx::to_json(doc);
          std::cout << data << "\n";
      }
      return data;
    }
  } // namespace database
} // namespace karlo
