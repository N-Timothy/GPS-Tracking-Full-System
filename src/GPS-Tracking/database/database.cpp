#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/database/create.hpp"
#include "GPS-Tracking/database/read.hpp"

#include <iostream>

namespace karlo {
  namespace database {

    // Connecting to database
    mongocxx::instance instance{};
    mongocxx::uri uri("mongodb://localhost:27017");
    mongocxx::client client(uri);
    mongocxx::database db = client["tracking-data"];
    mongocxx::collection collection = db["tracking"];

    void createData(trackingData data) {
      create(data, collection);
    }

    json readData(std::string imei) {
      return read(collection, imei);
    }

  } // namespace database
} // namesapce karlo
