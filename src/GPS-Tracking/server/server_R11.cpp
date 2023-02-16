#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/server_R11.hpp"

#include <stdio.h>
#include <unistd.h>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <map>
#include <thread>

#include <sstream>
#include <chrono>
#include <mutex>
#include <condition_variable>


namespace karlo {
  namespace server_R11 {

  std::map<std::string, int> imeiSocketMap;

  using namespace std::literals::chrono_literals;

  class GetData {
      private:
          int n = 0;
          int number = 0;
          int connectivity;

      public:
          std::string getBytes(int connfd, int byteslen) {
              std::stringstream buff;
              for (n = 0; n < byteslen; n++) {
                  connectivity = recv(connfd, (char *) &number, 1, 0);
                if (connectivity == 0) {
                    std::cout << "\x1b[31mConnection is closed\x1b[0m\n";
                    break;
                }
            buff << std::setw(2) << std::setfill('0') << std::hex << number;
          }
        return buff.str();
      } 

      std::string slice_imei(std::string imei_raw) {
        std::string imei;
        for (unsigned i = 5; i < imei_raw.length(); i += 2) {
          imei.push_back(imei_raw[i]);
        }
      }

      int packet_confirmation(int connfd, std::string rawData) {
         // prepate the data 
        
          std::stringstream ss;

          unsigned int start_bit;
          ss << std::hex << rawData.substr(0, PACKET_LENGTH * 2);
          ss >> start_bit;

          std::cout << "start bit : " << ss.str() << std::endl; 

          unsigned int packet_length;
          ss << std::hex << rawData.substr(4, PACKET_LENGTH * 2);
          ss >> packet_length;

          std::cout << "packet length : " << ss.str() << std::endl; 

          unsigned int protocol_number;
          ss << std::hex << rawData.substr(6, PROTOCOL_NUMBER * 2);
          ss >> protocol_number;

          std::cout << "protocol number : " << ss.str() << std::endl; 

          unsigned int imei;
          ss << std::hex << rawData.substr(8, IMEI * 2);
          ss >> imei;

          std::cout << "imei : " << ss.str() << std::endl; 

          unsigned int serial_number;
          ss << std::hex << rawData.substr(24, SERIAL_NUMBER * 2);
          ss >> serial_number;

          std::cout << "serial number : " << ss.str() << std::endl; 
          
          unsigned int error_check;
          ss << std::hex << rawData.substr(28, ERROR_CHECK * 2);
          ss >> error_check;

          std::cout << "error check : " << ss.str() << std::endl; 

          unsigned int stop_bit;
          ss << std::hex << rawData.substr(32, STOP_BIT * 2);
          ss >> stop_bit;

          std::cout << "stop bit : " << ss.str() << std::endl; 

          std::cout << "raw data : " << rawData << std::endl; 

          // byte package[] = {}
          // send(connfd, (char* ) &package, sizeof(package), 0);
        }
    }; // getData
      
    void removeSocket(int socket) {
        for (auto it = imeiSocketMap.begin(); it != imeiSocketMap.end(); it++) {
    if(it->second == socket) {
        imeiSocketMap.erase(it);
        break;
        }
      }
    }

    int communicate(int connfd) {

    json postData;

    std::string buff;
    unsigned i, dataCount;
    int confirm;
    
    std::string imei_raw, hex, hex_stream, event, id;

    int activity;
    struct timeval tv;

    GetData gps;

    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(connfd, &readfds);

    tv.tv_sec = 5;
    tv.tv_usec = 0;

    activity = select(connfd + 1, &readfds, NULL, NULL, &tv);
    if(activity == 0) return -3;

    confirm = gps.packet_confirmation(connfd, gps.getBytes(connfd, LOGIN_PACKET));

    if(confirm == -1) return -1;
    else if (confirm == -2) return -2;

    return 0;

    }

  } // namespace server_r11
} // namespace karlo
