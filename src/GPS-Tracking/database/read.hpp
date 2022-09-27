#ifndef _GPS_TRACKING_DATABASE_READ_HPP_
#define _GPS_TRACKING_DATABASE_READ_HPP_

#include "GPS-Tracking/database/database.hpp"

#include <string>

namespace karlo {
  namespace database {
    json read(mongocxx::collection collection);
  } // namespace database
} // namespace karlo

#endif //_GPS_TRACKING_DATABASE_READ_HPP_
