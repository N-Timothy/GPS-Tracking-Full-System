#include "GPS-Tracking/database/create.hpp"
#include <mongocxx/options/update.hpp>

#include <iostream>
#include <string>
#include <mongocxx/options/update.hpp>

namespace karlo {
  namespace database {

    bsoncxx::document::value generateDocument (trackingData data) {
      auto builder = bsoncxx::builder::stream::document{};
      bsoncxx::document::value trackingDocValue = builder
        << "$set" << open_document
        << "driver" << data.driver
        << "gpsOn" << data.gpsOn
        << "imei" << data.imei
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
        << close_document
        << bsoncxx::builder::stream::finalize;
      return trackingDocValue;
    }

    bsoncxx::document::value generateImeiDocument (trackingData data) {
      auto builder = bsoncxx::builder::stream::document{};
      bsoncxx::document::value trackingDocValue = builder
        << "imei" << data.imei
        << bsoncxx::builder::stream::finalize;
      return trackingDocValue;
    }


    void create (trackingData data, mongocxx::collection collection) {
      bsoncxx::document::value trackingDocValue = generateDocument(data);
      bsoncxx::document::value trackingImeiValue = generateImeiDocument(data);
      bsoncxx::document::view trackingDocument = trackingDocValue.view();
      bsoncxx::document::view trackingImeiDocument = trackingImeiValue.view();
     
      //bsoncxx::stdx::optional<mongocxx::result::insert_one> result =
      //collection.insert_one(trackingDocument);

      mongocxx::v_noabi::options::update options;
      options.upsert(true);
      collection.update_one(trackingImeiDocument, trackingDocument, options);
       }

  } // namespace database
} // namespace karlo
