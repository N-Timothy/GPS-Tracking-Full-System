#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/database/create.hpp"
#include "GPS-Tracking/database/read.hpp"

#include <iostream>
#include <condition_variable>


bool ready = true;

namespace karlo {
  namespace database {

    std::condition_variable cv;

    // Connecting to database
    mongocxx::instance instance{};
    mongocxx::uri uri("mongodb://localhost:27017");
    mongocxx::client client(uri);
    mongocxx::database db = client["tracking-data"];
    mongocxx::collection collection = db["tracking"];

    void createData(trackingData data) {
      ready = false;

      std::cout << "creating" << std::endl;

      create(data, collection);

      std::cout << "done creating" << std::endl;

      ready = true;
      cv.notify_one();
    }

    json readData(std::string imei) {
      ready = false;
      
      std::cout << "read One" << std::endl;

      json res = readOne(collection, imei);

      std::cout << "done read One" << std::endl;

      ready = true;
      cv.notify_one();

      return res;
    }

    std::vector<json> readData() {
      ready = false;

      std::cout << "read ALL" << std::endl;

      std::vector<json> res = readAll(collection);

      std::cout << "done read ALL" << std::endl;

      ready = true;
      cv.notify_one();

      return res;
    }

  } // namespace database
} // namesapce karlo
