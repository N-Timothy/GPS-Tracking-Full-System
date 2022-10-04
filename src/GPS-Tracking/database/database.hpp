#ifndef _GPS_TRACKING_DATABASE_HPP_
#define _GPS_TRACKING_DATABASE_HPP_

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <mongocxx/model/update_one.hpp>

#include "GPS-Tracking/server/data.hpp"

#include <nlohmann/json.hpp>
#include <vector>

extern bool ready;

namespace karlo {
  namespace database {
      
    using json = nlohmann::json;

    using bsoncxx::builder::stream::close_array;
    using bsoncxx::builder::stream::close_document;
    using bsoncxx::builder::stream::document;
    using bsoncxx::builder::stream::finalize;
    using bsoncxx::builder::stream::open_array;
    using bsoncxx::builder::stream::open_document;

    void setDatabaseConfig(json databaseConfig);

    void createData(trackingData data);

    json readData(std::string imei);

    std::vector<json> readData();


  } // namespace database
} // namespace karlo

#endif // _GPS_TRACKING_DATABASE_HPP_
