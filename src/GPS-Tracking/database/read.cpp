#include "GPS-Tracking/database/read.hpp"

#include <iostream>

namespace karlo {
  namespace database {
    void read(mongocxx::collection collection ) {
      mongocxx::cursor cursor = collection.find({});
      for(auto doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << "\n";
      }
    }
  } // namespace database
} // namespace karlo
