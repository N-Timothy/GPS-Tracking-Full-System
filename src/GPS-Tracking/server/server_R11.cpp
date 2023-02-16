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
        
         std::stringstream start_bit;
         start_bit << std::hex << rawData.substr(0, START_BIT * 2);
          
         std::cout << "start bit : " << start_bit.str() << std::endl; 

         std::stringstream packet_length;
         packet_length << std::hex << rawData.substr(4, PACKET_LENGTH * 2);
        
         std::cout << "packet length : " << packet_length.str() << std::endl; 

         std::stringstream protocol_number;
         protocol_number << std::hex << rawData.substr(6, PROTOCOL_NUMBER * 2);

         std::cout << "protocol number : " << protocol_number.str() << std::endl; 

         std::stringstream imei;
         imei << std::hex << rawData.substr(8, IMEI * 2);

         std::cout << "imei : " << imei.str() << std::endl; 

         std::stringstream serial_number;
         serial_number << std::hex << rawData.substr(24, SERIAL_NUMBER * 2);
         
         std::cout << "serial number : " << serial_number.str() << std::endl; 
          
         std::stringstream error_check;
         error_check << std::hex << rawData.substr(28, ERROR_CHECK * 2);

         std::cout << "error check : " << error_check.str() << std::endl; 

         std::stringstream stop_bit;
         stop_bit << std::hex << rawData.substr(32, STOP_BIT * 2);
          
         std::cout << "stop bit : " << stop_bit.str() << std::endl; 

         auto package = start_bit.str() + packet_length.str() + protocol_number.str() + serial_number.str() + error_check.str() + stop_bit.str();

         std::cout << "response package : " << package << std::endl;

          // byte package[] = {}
        send(connfd, (char* ) &package, sizeof(package), 0);

        return 0;
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

    for(;;) {

       std::string rawData = gps.getBytes(connfd, 41);

       std::cout << "RAW DATA : " << rawData << std::endl;

       // get raw datetime

       std::stringstream raw_datetime;
       raw_datetime << std::hex << rawData.substr(8, DATE_TIME * 2);

       std::cout << "date time : " << raw_datetime.str()  << std::endl;
       
       // get raw latitude 
       
       std::stringstream raw_latitude;
       raw_latitude << std::hex << rawData.substr(22, LATITUDE * 2);

       std::cout << "latitude : " << raw_latitude.str() << std::endl;
       
       // get raw longitude

       std::stringstream raw_longitude;
       raw_longitude << std::hex << rawData.substr(30, LONGITUDE * 2);

       std::cout << "longitude : " << raw_longitude.str() << std::endl;
       
       // get raw speed

       std::stringstream raw_speed;
       raw_speed << std::hex << rawData.substr(38, SPEED * 2);

       std::cout << "Speed : " << raw_speed.str()  << std::endl;

       // get raw course (bearing)

       std::stringstream raw_course;
       raw_course << std::hex << rawData.substr(40, COURSE * 2);

       std::cout << "course : " << raw_course.str()  << std::endl;

       // get raw voltage

       std::stringstream raw_voltage;
       raw_voltage << std::hex << rawData.substr(62, VOLTAGE * 2);

       std::cout << "voltage : " << raw_voltage.str()  << std::endl;

       std::this_thread::sleep_for(60s); 
    }
    return 0;
    }

  } // namespace server_r11
} // namespace karlo
