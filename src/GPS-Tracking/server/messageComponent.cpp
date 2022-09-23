#include "GPS-Tracking/server/messageComponent.hpp"

#include "GPS-Tracking/server/hexToDecimal.hpp"

#include <iomanip>
#include <chrono>
#include <sstream>
#include <cstring>
#include <string>
#include <iostream>

namespace karlo {
  namespace server {

    std::string getImei(std::string buffer){
      char buff[(IMEI_BYTES-2)*2];
      for (int i = 4; i < (IMEI_BYTES * 2); i++){
        buff[i-4] = buffer[i];
      }
      return buff;
    }

    std::string getTimeStamp(std::string buffer){
      char buff[TIMESTAMP_BYTES*2];

      for (int i = 20; i < (20 + (TIMESTAMP_BYTES * 2)); i++){
        buff[i-20] = buffer[i];
      }

      //  convert hex to timestamp
      std::istringstream is(buff);
      long long x;
      is >> std::hex >> x;
      std::chrono::microseconds us(x);
      std::chrono::time_point<std::chrono::system_clock> sc(us);
      std::time_t t_c = std::chrono::system_clock::to_time_t(sc);
      std::stringstream timeStamp;
      timeStamp << std::put_time(std::gmtime(&t_c), "%FT%TZ");
      return timeStamp.str();
    }

    long getLongitude(std::string buffer){
      char buff[LONGITUDE_BYTES*2];

      for (int i = 36; i < (36 + (LONGITUDE_BYTES * 2)); i++){
        buff[i-36] = buffer[i];
      }

      long res = hexToDecimal(buff);
      res = res / 10^7;
      return res;
    }

    long getLatitude(std::string buffer){
      char buff[LATITUDE_BYTES*2];
      for (int i = 46; i < (46 + (LONGITUDE_BYTES * 2)); i++){
        buff[i-46] = buffer[i];
      }
      std::cout << std::endl;

      long res = hexToDecimal(buff);
      res = res / 10^7;
      return res;
    }

    long getAltitude(std::string buffer){
      char buff[ALTITUDE_BYTES*2];

      for (int i = 54; i < (54 + (ALTITUDE_BYTES * 2)); i++){
        buff[i-54] = buffer[i];
      }

      long res = hexToDecimal(buff);
      return res;
    }

  } // namespace server
} // namespace karlo
