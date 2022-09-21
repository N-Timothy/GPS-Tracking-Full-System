#include "GPS-Tracking/server/decodeMessage.hpp"
#include "GPS-Tracking/server/data.hpp"

#include "GPS-Tracking/database/database.hpp"

#include <thread>

namespace karlo {
  namespace server {

    void getMessage(std::string rawMessage) {
      
      trackingData data;

      //------------- this is the decoding part -----------------------

      // ---------------- end of decoding part --------------------

      data.driver = "new driver";
      data.gpsOn = true;
      data.batteryLevel = 100;
      data.latitude = 0.0;
      data.longitude = 0.0;
      data.altitude = 0.0;
      data.speed = 10;
      data.bearing = 0;
      data.description = "This is a new value";
      data.truck = "new truck";
      data.city = 123;
      data.createdAt = "None";
      data.updatedAt = "None";
      data.version = 0;

      database::database(data);
    }

    void decodeMessage (std::string rawMessage) {
      std::thread decodedMessageThread {getMessage, std::ref(rawMessage)};
      decodedMessageThread.join();
    }
  } // namespace server
} // namespace karlo
