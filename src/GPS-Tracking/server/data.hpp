#ifndef _GPS_TRACKING_SERVER_DATA_HPP_
#define _GPS_TRACKING_SERVER_DATA_HPP_

#include <string>

namespace karlo {

  struct trackingData {
    std::string driver = "None";
    std::string imei = "";
    bool gpsOn = true;
    int batteryLevel = 0;
    float latitude = 0;
    float longitude = 0;
    float altitude = 0;
    int speed = 0;
    int bearing = 0;
    std::string description = "This is default value";
    std::string truck = "None";
    short city = 0;
    std::string createdAt = "Null";
    std::string updatedAt = "Null";
    int version = 0;
  };

} //namespace karlo

#endif //_GPS_TRACKING_SERVER_DATA_HPP_
