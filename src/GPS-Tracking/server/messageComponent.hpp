#ifndef _GPS_TRACKING_SERVER_MESSAGE_COMPONENT_HPP_
#define _GPS_TRACKING_SERVER_MESSAGE_COMPONENT_HPP_

#include "GPS-Tracking/server/tcp.hpp"

#include <string>

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

namespace karlo {
  namespace server {

    std::string getImei(int connfd);

    std::string getTimeStamp(int connfd);

    long getLongitude(int connfd);

    long getLatitude(int connfd);

    long getAltitude(int connfd);

  } // namespace server
} // namespace karlo

#endif _GPS_TRACKING_SERVER_MESSAGE_COMPONENT_HPP_
