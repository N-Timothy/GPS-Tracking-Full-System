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

      int login_packet_confirmation(int connfd, std::string rawData) {
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

        int heartbeat_packet_confirmation(int connfd ,std::string rawData) {
           std::stringstream start_bit;
         start_bit << std::hex << rawData.substr(0, START_BIT * 2);
          
         std::cout << "start bit : " << start_bit.str() << std::endl; 

         std::stringstream packet_length;
         packet_length << std::hex << rawData.substr(4, PACKET_LENGTH * 2);
        
         std::cout << "packet length : " << packet_length.str() << std::endl; 

         std::stringstream protocol_number;
         protocol_number << std::hex << rawData.substr(6, PROTOCOL_NUMBER * 2);

         std::cout << "protocol number : " << protocol_number.str() << std::endl; 

         std::stringstream serial_number;
         serial_number << std::hex << rawData.substr(18, SERIAL_NUMBER * 2);
         
         std::cout << "serial number : " << serial_number.str() << std::endl; 
          
         std::stringstream error_check;
         error_check << std::hex << rawData.substr(22, ERROR_CHECK * 2);

         std::cout << "error check : " << error_check.str() << std::endl; 

         std::stringstream stop_bit;
         stop_bit << std::hex << rawData.substr(26, STOP_BIT * 2);
          
         std::cout << "stop bit : " << stop_bit.str() << std::endl; 

         auto package = start_bit.str() + packet_length.str() + protocol_number.str() + serial_number.str() + error_check.str() + stop_bit.str();

         std::cout << "response package : " << package << std::endl;

          // byte package[] = {}
        send(connfd, (char* ) &package, sizeof(package), 0);

        return 0;
        }

    }; // getData

    const char* hexCharToBin (char hex_c) {
      switch(hex_c) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'a': return "1010";
        case 'b': return "1011";
        case 'c': return "1100";
        case 'd': return "1101";
        case 'e': return "1110";
        case 'f': return "1111";
        default: return "0000";
      }
    }
      
    void removeSocket(int socket) {
        for (auto it = imeiSocketMap.begin(); it != imeiSocketMap.end(); it++) {
    if(it->second == socket) {
        imeiSocketMap.erase(it);
        break;
        }
      }
    }

    std::string parseDateTime(std::string raw) {
      unsigned int year = 0;
      std::stringstream tYear;
      tYear << std::hex << raw.substr(0,2);
      tYear >> year;

      std::cout << "Date: 20" << year << "-";

      unsigned int month = 0;
      std::stringstream tMonth;
      tMonth << std::hex << raw.substr(2,2);
      tMonth >> month;

      std::cout << month << "-";

      unsigned int day = 0;
      std::stringstream tDay;
      tDay << std::hex << raw.substr(4,2);
      tDay >> day;

      std::cout << day << " ";

      unsigned int hour = 0;
      std::stringstream tHour;
      tHour << std::hex << raw.substr(6,2);
      tHour >> hour;

      std::cout << hour << ":";

      unsigned int minutes = 0;
      std::stringstream tMinutes;
      tMinutes << std::hex << raw.substr(8,2);
      tMinutes >> minutes;

      std::cout << minutes << ":";

      unsigned int seconds = 0;
      std::stringstream tSeconds;
      tSeconds << std::hex << raw.substr(10,2);
      tSeconds >> seconds;

      std::cout << seconds << std::endl;

    }

    float parseLatitiude(std::string raw) {

      float latitude;
      int raw_latitude;
      std::stringstream tLatitude;

      tLatitude << std::hex << raw;
      tLatitude >> raw_latitude;

      latitude = raw_latitude / 3000;

      std::cout << "latitiude : " << latitude << std::endl;

    }

    float parseLongitude(std::string raw) {

      float longitude;
      int raw_longitude;
      std::stringstream tLongitude;

      tLongitude << std::hex << raw;
      tLongitude >> raw_longitude;

      longitude = raw_longitude / 3000;

      std::cout << "Longitude : " << longitude << std::endl;

    }

    int parseSpeed(std::string raw) {

      unsigned int speed;
      std::stringstream tSpeed;

      tSpeed << std::hex << raw;
      tSpeed >> speed;

      std::cout << "Speed : " << speed << std::endl;

    }

    std::string parseCourseAndIgnition(std::string raw) {
        
        std::string raw_bit;

        for(int ch = 0; ch < 4; ch++) {
          raw_bit += hexCharToBin(raw[ch]);
        }

        std::cout << "Raw bit : " << raw_bit << std::endl;

        std::string ignition = raw_bit.substr(1,1);
        std::string bearing = raw_bit.substr(6, 10);

        std::cout << "Ignition : " << ignition << std::endl;
        std::cout << "Bearing : " << bearing << std::endl;
    }

    float parseVoltage(std::string raw) {

      float voltage;
      unsigned int raw_voltage;
      std::stringstream tVoltage;
      
      tVoltage << std::hex << raw;
      tVoltage >> raw_voltage;

      voltage = raw_voltage / 100;

      std::cout << "Voltage : " << voltage << std::endl;

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

    confirm = gps.login_packet_confirmation(connfd, gps.getBytes(connfd, LOGIN_PACKET));

    if(confirm == -1) return -1;
    else if (confirm == -2) return -2;

    for(;;) {

      int byte = GPS_PACKET;
      std::string rawData;

       for(int x = 0; x < byte; x++) {
        std::string bytes;
        bytes += gps.getBytes(connfd, 1);

        std::cout << "bytes : " << bytes << std::endl;

        rawData += bytes;

        if(x == 3 && bytes == "13") {
          byte = HEARTBEAT_PACKET;
        }
       }

       std::cout << "RAW DATA : " << rawData << std::endl;

       if(byte == HEARTBEAT_PACKET) {

          confirm = gps.heartbeat_packet_confirmation(connfd, rawData);
          if(confirm == -1) return -1;
          else if (confirm == -2) return -2;

       } else {
          // get raw datetime

          std::stringstream raw_datetime;
          raw_datetime << std::hex << rawData.substr(8, DATE_TIME * 2);

          std::cout << "date time : " << raw_datetime.str()  << std::endl;

          // tempoarary create model later
          std::string date = parseDateTime(raw_datetime.str());
          
          // get raw latitude 
          
          std::stringstream raw_latitude;
          raw_latitude << std::hex << rawData.substr(22, LATITUDE * 2);

          std::cout << "latitude : " << raw_latitude.str() << std::endl;

          // tempoarary create model later
          float latitiude = parseLatitiude(raw_latitude.str());
          
          // get raw longitude

          std::stringstream raw_longitude;
          raw_longitude << std::hex << rawData.substr(30, LONGITUDE * 2);

          std::cout << "longitude : " << raw_longitude.str() << std::endl;

          // temporary create model later
          float longitude = parseLongitude(raw_longitude.str());
          
          // get raw speed

          std::stringstream raw_speed;
          raw_speed << std::hex << rawData.substr(38, SPEED * 2);

          std::cout << "Speed : " << raw_speed.str()  << std::endl;

          // temporary create model later
          int speed = parseSpeed(raw_speed.str());

          // get raw course (bearing)

          std::stringstream raw_course;
          raw_course << std::hex << rawData.substr(40, COURSE * 2);

          std::cout << "course : " << raw_course.str()  << std::endl;

          // temporary create model later
          std::string course = parseCourseAndIgnition(raw_course.str());

          // get raw voltage

          std::stringstream raw_voltage;
          raw_voltage << std::hex << rawData.substr(62, VOLTAGE * 2);

          std::cout << "voltage : " << raw_voltage.str()  << std::endl;

          //temporary create model later
          float voltage = parseVoltage(raw_voltage.str());

          std::this_thread::sleep_for(60s);
        }
      }
    return 0;
    }

  } // namespace server_r11
} // namespace karlo
