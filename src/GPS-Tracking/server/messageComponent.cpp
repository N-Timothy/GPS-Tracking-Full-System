#include "GPS-Tracking/server/messageComponent.hpp"

#include <iomanip>
#include <chrono>
#include <sstream>
#include <cstring>
#include <string>

namespace karlo {
  namespace server {

    std::string getImei(int connfd, char* buff){
      int n = 0;
      int number = 0;
      int ACCEPT = 0x01;

      for(;;) {
        // IMEI
        n++;
        recv(connfd, (char*)&number, 1, 0);

        if (n == 1) {
          sprintf(buff, "%02x", number);
          //            printf("result: %s\n", buff);
        } else if (n >= 2 && n <= IMEI_BYTES) {
          sprintf(buff + strlen(buff), "%02x", number);
          if (n == IMEI_BYTES) {
            // printf("IMEI\t\t\t: %s\n", buff);
            send(connfd, (char *) &ACCEPT, sizeof(ACCEPT), 0);
            // printf("Request confirmation: %x\n\n", ACCEPT);
            break;
          }
        }
      }
      return buff;
    }

    std::string getTimeStamp(int connfd, char* buff){
      int n = 0;
      int number = 0;

      for(;;) {
        n++;
        recv(connfd, (char*)&number, 1, 0);

        if (n == 1) {
          sprintf(buff, "%02x", number);
        }
        else if (n >= 2 && n <= TIMESTAMP_BYTES) {
          sprintf(buff + strlen(buff), "%02x", number);
          if (n == TIMESTAMP_BYTES) {
            // printf("Timestamp\t\t: %s\n", buff);
            break;
          }
        }
      }
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

    std::string getLongitude(int connfd, char* buff){
      int n = 0;
      int number = 0;

      for(;;) {
        n++;
        recv(connfd, (char*)&number, 1, 0);

        if (n == 1) {
          sprintf(buff, "%02x", number);
        }
        else if (n >= 2 && n <= LONGITUDE_BYTES) {
          sprintf(buff + strlen(buff), "%02x", number);
          if (n == LONGITUDE_BYTES) {
            // printf("Longitude\t\t: %s\n", buff);
            break;
          }
        }
      }
      return buff;
    }

    std::string getLatitude(int connfd, char* buff){
      int n = 0;
      int number = 0;

      for(;;) {
        n++;
        recv(connfd, (char*)&number, 1, 0);

        if (n == 1) {
          sprintf(buff, "%02x", number);
        }
        else if (n >= 2 && n <= LATITUDE_BYTES) {
          sprintf(buff + strlen(buff), "%02x", number);
          if (n == LATITUDE_BYTES) {
            // printf("Latitude\t\t: %s\n", buff);
            break;
          }
        }
      }
      return buff;
    }

    std::string getAltitude(int connfd, char* buff){
      int n = 0;
      int number = 0;

      for(;;) {
        n++;
        recv(connfd, (char*)&number, 1, 0);

        if (n == 1) {
          sprintf(buff, "%02x", number);
        }
        else if (n >= 2 && n <= ALTITUDE_BYTES) {
          sprintf(buff + strlen(buff), "%02x", number);
          if (n == ALTITUDE_BYTES) {
            // printf("Altitude\t\t: %s\n", buff);
            break;
          }
        }
      }
      return buff;
    }

  } // namespace server
} // namespace karlo
