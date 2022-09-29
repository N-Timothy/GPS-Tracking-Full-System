#ifndef _GPS_TRACKING_DATABASE_READ_HPP_
#define _GPS_TRACKING_DATABASE_READ_HPP_

#include "GPS-Tracking/database/database.hpp"

#include <string>
#include <vector>

namespace karlo {
  namespace database {

    json readOne(mongocxx::collection collection, std::string imei);
    std::vector<json> readAll(mongocxx::collection collection);

  } // namespace database
} // namespace karlo

#endif //_GPS_TRACKING_DATABASE_READ_HPP_
