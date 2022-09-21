#include "GPS-Tracking/database/create.hpp"

#include <iostream>
#include <string>

namespace karlo {
  namespace database {

    bsoncxx::document::value generateDocument (trackingData data) {
      auto builder = bsoncxx::builder::stream::document{};
      bsoncxx::document::value trackingDocValue = builder
        << "driver" << data.driver
        << "gpsOn" << data.gpsOn
        << "betteryLevel" << data.batteryLevel
        << "latitude" << data.latitude
        << "longitude" << data.longitude
        << "altitude" << data.altitude
        << "speed" << data.speed
        << "bearing" << data.bearing
        << "description" << data.description
        << "truck" << data.truck
        << "city" << data.city
        << "createdAt" << data.createdAt
        << "updatedAt" << data.updatedAt
        << "_v" <<  data.version
        << bsoncxx::builder::stream::finalize;
      return trackingDocValue;
    }

    void create (trackingData data, mongocxx::collection collection) {
      bsoncxx::document::value trackingDocValue = generateDocument(data);
      bsoncxx::document::view trackingDocument = trackingDocValue.view();
      bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
      collection.insert_one(trackingDocument);
       }

  } // namespace database
} // namespace karlo
