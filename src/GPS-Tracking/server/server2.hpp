#ifndef _GPS_TRACKING_SERVER_SERVER2_HPP_
#define _GPS_TRACKING_SERVER_SERVER2_HPP_

#define MAX 32

#define IMEI_BYTES 17
#define ZERO_BYTES 4
#define DATA_FIELD_BYTES 4
#define CODEC_ID_BYTES 1
#define NUM_OF_DATA_BYTES 1
#define TIMESTAMP_BYTES 8
#define PRIORITY_BYTES 1
#define LONGITUDE_BYTES 4
#define LATITUDE_BYTES 4
#define ALTITUDE_BYTES 2
#define ANGLE_BYTES 2
#define SATELLITE_BYTES 1
#define SPEED_BYTES 2

#include "GPS-Tracking/server/read_imei_json.hpp"

namespace karlo {
  namespace server {
    
    int communicate(int connfd, std::vector<json> imei_list);

  } // namespace server
} // namespace karlo

#endif // _GPS_TRACKING_SERVER_SERVER2_HPP_
