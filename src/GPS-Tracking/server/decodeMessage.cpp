#include "GPS-Tracking/server/decodeMessage.hpp"
#include "GPS-Tracking/server/data.hpp"
#include "GPS-Tracking/server/messageComponent.hpp"

#include "GPS-Tracking/database/database.hpp"

#include <thread>
#include <iostream>

namespace karlo {
  namespace server {

    void getMessage(std::string buffer, std::string imei) {

      trackingData data;

      data.driver = "new driver";
      data.imei = imei;
      data.gpsOn = true;
      data.batteryLevel = 100;
      data.latitude = getLatitude(buffer);
      data.longitude = getLongitude(buffer);
      data.altitude = getAltitude(buffer);
      data.speed = 10;
      data.bearing = 0;
      data.description = "This is a new value";
      data.truck = "new truck";
      data.city = 123;
      data.createdAt = getTimeStamp(buffer);
      data.updatedAt = data.createdAt; 
      data.version = 0;

      // std::cout << "latitude : " << getLatitude(buffer) << std::endl;

      //database::database(data);
    }

    void decodeMessage (std::string buffer, bool imei_data) {
      std::string imei;

      if(imei_data) {
        imei = getImei(buffer);
        std::cout << "imei : " << imei << std::endl;
      } else {
        getMessage(buffer, imei);
        // std::cout << "done" << std::endl;
      }

    }

  } // namespace server
} // namespace karlo
