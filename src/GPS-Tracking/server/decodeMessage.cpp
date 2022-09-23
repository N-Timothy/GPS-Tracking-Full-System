#include "GPS-Tracking/server/decodeMessage.hpp"
#include "GPS-Tracking/server/data.hpp"
#include "GPS-Tracking/server/messageComponent.hpp"

#include "GPS-Tracking/database/database.hpp"

#include <thread>
#include <iostream>

namespace karlo {
  namespace server {

    void getMessage(int connfd) {

      trackingData data;

      //------------- this is the decoding part -----------------------


      // ---------------- end of decoding part --------------------

      data.driver = "new driver";
      data.gpsOn = true;
      data.batteryLevel = 100;
      data.latitude = getLatitude(connfd);
      data.longitude = getLongitude(connfd);
      data.altitude = getAltitude(connfd);
      data.speed = 10;
      data.bearing = 0;
      data.description = "This is a new value";
      data.truck = "new truck";
      data.city = 123;
      data.createdAt = "None";
      data.updatedAt = "None"; 
      data.version = 0;

      std::cout << "Latitude : " << getLatitude(connfd) << std::endl;

      //database::database(data);
    }

    void decodeMessage (int connfd) {
    }

  } // namespace server
} // namespace karlo
