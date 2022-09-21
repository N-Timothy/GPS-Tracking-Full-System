#ifndef _GPS_TRACKING_DATABASE_CREATE_HPP_
#define _GPS_TRACKING_DATABASE_CREATE_HPP_

#include "GPS-Tracking/database/database.hpp"

namespace karlo {
  namespace database {

    void create(trackingData data, mongocxx::collection collection);
    
  } // namespace database
} // namespace karlo

#endif // _GPS_TRACKING_DATABASE_CREATE_HPP_
 
