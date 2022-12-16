// TCP Connection, GPS Data-retrieving
#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/data.hpp"
#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/server/server.hpp"
#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "server.hpp" // for development purpose only

#include <stdio.h> // sprintf
#include <unistd.h>
#include <iomanip> // put_time, setprecision
#include <iostream> // cin, cout
#include <cstring> // memset
#include <string> // std::string
#include <algorithm> // find
#include <vector>
#include <map>
#include <thread>

#include <sstream>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace karlo {
  namespace server {

    std::vector<std::string> imeiRealTimeVec;
    std::vector<std::string> imeiNormalVec;
    std::map<std::string, int> imeiSocketMap;

    std::mutex m;
    std::condition_variable cv;

    using namespace std::literals::chrono_literals;

    class GetData {
    private:
      int n = 0;
      int number = 0;
      int connectivity;
      std::string result;
      bool realTimeState = false;

    public:

      bool getRealTimeState() {
        return this->realTimeState;  
      }

      void setRealTimeState(bool newState) {
        this->realTimeState = newState;
      }

      std::string getBytes (int connfd, int byteslen) {
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
        return imei;
      }
      int imeiRecognition(std::string imei_raw, std::vector<json> imei_list) {
        std::string imei = slice_imei(imei_raw);

        if (imei_raw.substr(0, 4) != "000f") return -2;

        for (auto it: imei_list) {
          if (imei == it) return 0;
        }
        return -1;
      }
      int imeiConfirmation(int connfd, int recognized) {
        unsigned char ACCEPT[] = {0x01};
        unsigned char DECLINE[] = {0x00};
        if (recognized == 0) {
          send(connfd, (char *) &ACCEPT, sizeof(ACCEPT), 0);
          return 0;
        }
        else {
          if (recognized == -1) {
            send(connfd, (char *) &DECLINE, sizeof(DECLINE), 0);
            return -1;
          }
          else return -2;
        }
      }
      int imeiCheckForDatabase(std::string imei, std::vector<json> imei_list) {
        for (auto it: imei_list) {
          if (imei == it) return 0;
        }
        return -1;
      }

      void sendConfirmation(int connfd, int numOfData) {
        unsigned char bNOD[4];
        bNOD[0] = (numOfData & 0xff000000) >> 24; // 0
        bNOD[1] = (numOfData & 0x00ff0000) >> 16; // 0
        bNOD[2] = (numOfData & 0x0000ff00) >> 8; // 0
        bNOD[3] = (numOfData & 0x000000ff) >> 0; // numOfData
        send(connfd, (char*) &bNOD, sizeof(bNOD), 0);
      }

      void sendGPRSCommand(int connfd, bool realTimeFlag) {
        typedef unsigned char byte;

        // setparam 10050:5;10150:5;10250:5
        byte realtime_command[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28,
                                   0x0c, 0x01, 0x05,
                                   0x00, 0x00, 0x00, 0x20,
                                   's', 'e', 't', 'p', 'a', 'r', 'a', 'm', ' ',
                                   '1', '0', '0', '5', '0', ':', '5', ';',
                                   '1', '0', '1', '5', '0', ':', '5', ';',
                                   '1', '0', '2', '5', '0', ':', '5',
                                   0x01,
                                   0x00, 0x00, 0x99, 0x74};

        // setparam 10050:300;10150:300;10250:300
        byte normal_command[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2e,
                                 0x0c, 0x01, 0x05,
                                 0x00, 0x00, 0x00, 0x26,
                                 's', 'e', 't', 'p', 'a', 'r', 'a', 'm', ' ',
                                 '1', '0', '0', '5', '0', ':', '3', '0', '0', ';',
                                 '1', '0', '1', '5', '0', ':', '3', '0', '0', ';',
                                 '1', '0', '2', '5', '0', ':', '3', '0', '0',
                                 0x01,
                                 0x00, 0x00, 0xb7, 0x14};

        if (realTimeFlag) send(connfd, (char *) &realtime_command, sizeof(realtime_command), 0);
        else send(connfd, (char *) &normal_command, sizeof(normal_command), 0);
      }
    };

    void toRealTime(std::string imei, bool toggle) {
      if (toggle) imeiRealTimeVec.push_back(imei);
      else imeiNormalVec.push_back(imei);
    }

    std::vector<std::string> removeImei(std::vector<std::string> vector, std::string imei) {
      auto it = std::find(vector.begin(), vector.end(), imei);
      if (it != vector.end()) {
        vector.erase(it);
      }
      return vector;
    }

    void removeSocket(int socket) {
      for (auto it = imeiSocketMap.begin(); it != imeiSocketMap.end(); it++) {
        if (it->second == socket) {
          imeiSocketMap.erase(it);
          break;
        }
      }
    }

    std::string timestampToDate (std::string hex) {
      std::istringstream is(hex);
      unsigned long x;
      is >> std::hex >> x;
      std::chrono::milliseconds ms(x + 3600*7*1000);
      std::chrono::time_point<std::chrono::system_clock> sc(ms);

      std::time_t t_c = std::chrono::system_clock::to_time_t(sc);
      std::stringstream dateAndTime;
      std::stringstream year;
      dateAndTime << std::put_time(std::localtime(&t_c), "%FT%TZ");

      return dateAndTime.str();
    }

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

    std::string hexToBin(std::string hex) {
      std::string bin;
      for (unsigned i = 0; i < hex.size(); i++)
        bin += hexCharToBin(hex[i]);
      return bin;
    }

    std::string TwoComplement(std::string binary) {
      for (unsigned i = 0; i < binary.size(); i++) {
        if (binary[i] == '1'){
          binary[i] = '0';
        } else {
          binary[i] = '1';
        }
      }
      return binary;
    }

    int binToDecimal(std::string bin) {
      int number = 0;
      try {
        number += std::stoi(bin, 0, 2);
      } catch (const std::out_of_range& oor) {
        return -4;
      }
      return number;
    }

    double hexToLongitudeLatitude(std::string hex) {
      std::string bin;
      bin = hexToBin(hex);
      if (bin[0] == '0') {
        return binToDecimal(bin) / 1e7;
      } else {
        return -(binToDecimal(TwoComplement(bin)) + 1) / 1e7;
      }
    }

    std::string stringSubstr(std::string hex, int start, int off) {
      std::string res;
      try {
        res = hex.substr(start, off);
      } catch (const std::out_of_range& e) {
        res = "0";
      }
      return res;
    }

    std::string dateAndTimeNow(std::string time_zone = "UTC") {
      std::stringstream dateAndTime;
      int secondsFromUTC;
      if (time_zone == "WIB") secondsFromUTC = 3600*7;
      else secondsFromUTC = 0;

      auto now = std::chrono::system_clock::now();
      auto in_time_t = std::chrono::system_clock::to_time_t(now) + secondsFromUTC;
      dateAndTime << std::put_time(std::localtime(&in_time_t), "%A, %F, %T [WIB]");
      
      return dateAndTime.str();
    }


// Function designed for chat between client and server.
    int communicate(int connfd, std::vector<json> imei_list) {

      // init struct data
      trackingData data;
      json postData;

      std::vector<json> postDataVec;

      std::string buff;
      unsigned i, dataCount, numOfData1, numOfData2;
      unsigned realTimeElapsed = 0;
      int confirm;
      unsigned numOfOneByteID, numOfTwoBytesID, numOfFourBytesID, numOfEightBytesID;
      std::string imei_raw, codec;
      std::string hex;
      std::string event, id;

      int data_NOB, AVL_NOB;
      int AVL_POS, ID_POS, VALUE_POS, NUM_OF_DATA2_POS;
      std::string hex_stream;

      int activity;
      struct timeval tv;

      GetData gps;

      fd_set readfds;

      // Clear the socket set
      FD_ZERO(&readfds);
      FD_SET(connfd, &readfds);

      // Initialize timeval
      tv.tv_sec = 5;
      tv.tv_usec = 0;

      activity = select(connfd + 1, &readfds, NULL, NULL, &tv);
      if (activity == 0) return -3;

      // Get IMEI number from new connection
      imei_raw = gps.getBytes(connfd, IMEI_NOB);
      if (imei_raw == "") return -3;

      confirm = gps.imeiConfirmation(connfd, gps.imeiRecognition(imei_raw, imei_list));
      if (confirm == -1) return -1;
      else if (confirm == -2) return -2;
      data.imei = gps.slice_imei(imei_raw);
      postData["imei"] = data.imei;
      std::cout << "IMEI\t\t\t: " << data.imei << std::endl;

      // Register imei and socket file descriptor pair into map
      if (imeiSocketMap.find(data.imei) == imeiSocketMap.end()) {
        imeiSocketMap.emplace(data.imei, connfd);
      } else {
        imeiSocketMap.find(data.imei)->second = connfd;
      }
      for (auto it = imeiSocketMap.begin(); it != imeiSocketMap.end(); it++) {
        std::cout << it->first << ": " << it->second << std::endl;
      }

      // Continuously reading AVL Data as long as connection is linked
      for (;;) {
        // Check for GPRS command
        if (std::find(imeiRealTimeVec.begin(), imeiRealTimeVec.end(), data.imei) != imeiRealTimeVec.end()) {
          gps.sendGPRSCommand(connfd, true);
          imeiRealTimeVec = removeImei(imeiRealTimeVec, data.imei);
          gps.setRealTimeState(true);
        }

        if (std::find(imeiNormalVec.begin(), imeiNormalVec.end(), data.imei) != imeiNormalVec.end()) {
          gps.sendGPRSCommand(connfd, false);
          imeiNormalVec = removeImei(imeiNormalVec, data.imei);
          gps.setRealTimeState(false);
        }

        // Turn GPS to normal period once ignition is turned off
        if (gps.getRealTimeState() && data.description == "Ignition turned off.") {
          gps.sendGPRSCommand(connfd, false);
          gps.setRealTimeState(false);
        }

        // Check activity in read file descriptor
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        FD_SET(connfd, &readfds);
        activity = select(connfd + 1, &readfds, NULL, NULL, &tv);

        if (FD_ISSET(connfd, &readfds)) {

          hex = gps.getBytes(connfd, ZERO_NOB);
          if (hex == "" || hex != "00000000") {
            std::cout << "Zero Bytes\t\t: " << hex << std::endl;
            std::cout << dateAndTimeNow("WIB") << std::endl;
            return -3;
          }

          hex = gps.getBytes(connfd, DATA_FIELD_NOB);
          if (hex == "") {
            std::cout << "Data Field Length\t: " << hex << std::endl;
            return -3;
          }

          try {
            data_NOB = std::stoi(hex, 0, 16);
          } catch (const std::out_of_range& oor) {
            return -4;
          }

          // Reading all remaining hexadecimal stream that comes after data field length
          hex_stream = gps.getBytes(connfd, data_NOB + CRC16_NOB);
          if (hex_stream == "") return -3;

          codec = stringSubstr(hex_stream, CODEC_ID_POS, CODEC_ID_NOB*2);

          if (codec == "08") {
            numOfData1 = std::stoi(stringSubstr(hex_stream ,NUM_OF_DATA1_POS, NUM_OF_DATA_NOB*2), 0, 16);
            NUM_OF_DATA2_POS = 2 * (data_NOB - NUM_OF_DATA_NOB);
            numOfData2 = std::stoi(stringSubstr(hex_stream, NUM_OF_DATA2_POS, NUM_OF_DATA_NOB*2), 0, 16);
            if (numOfData1 != numOfData2) return -3;
            
            AVL_NOB = data_NOB - CODEC_ID_NOB - 2 * NUM_OF_DATA_NOB;

            for (dataCount = 0; dataCount < numOfData1; dataCount++) {
              AVL_POS = CODEC_ID_POS + NUM_OF_DATA1_POS + 2*NUM_OF_DATA_NOB + 2*(AVL_NOB/numOfData1)*dataCount;

              data.createdAt = timestampToDate(stringSubstr(hex_stream, AVL_POS + TIMESTAMP_POS, TIMESTAMP_NOB*2));
              postData["timestamp"] = data.createdAt;

              data.longitude = hexToLongitudeLatitude(stringSubstr(hex_stream, AVL_POS + LONGITUDE_POS, LONGITUDE_NOB*2));
              postData["longitude"] = data.longitude;
              
              data.latitude = hexToLongitudeLatitude(stringSubstr(hex_stream, AVL_POS + LATITUDE_POS, LATITUDE_NOB*2));
              postData["latitude"] = data.latitude;

              data.altitude = std::stoi(stringSubstr(hex_stream, AVL_POS + ALTITUDE_POS, ALTITUDE_NOB*2), 0, 16);
              postData["altitude"] = data.altitude;

              data.bearing = std::stoi(stringSubstr(hex_stream, AVL_POS + ANGLE_POS, ANGLE_NOB*2), 0, 16);
              postData["bearing"] = data.bearing;

              data.speed = std::stoi(stringSubstr(hex_stream, AVL_POS + SPEED_POS, SPEED_NOB*2), 0, 16);
              postData["speed"] = data.speed;

              event = stringSubstr(hex_stream, AVL_POS + EVENT_IO_ID_POS, EVENT_IO_ID_NOB*2);

              numOfOneByteID = std::stoi(stringSubstr(hex_stream, AVL_POS + NUM_OF_1B_IO_POS, NUM_OF_IO_NOB*2), 0, 16);
              for (i = 0; i < numOfOneByteID; i++) {
                ID_POS = NUM_OF_1B_IO_POS + NUM_OF_IO_NOB*2 + 2*i * (ID_NOB + VALUE1_NOB);
                VALUE_POS = ID_POS + 2 * ID_NOB;
                id = stringSubstr(hex_stream, AVL_POS + ID_POS, ID_NOB*2);

                // Ignition ID = 239
                if (id == "ef") {
                  data.ignitionOn = std::stoi(stringSubstr(hex_stream, AVL_POS + VALUE_POS, VALUE1_NOB*2), 0, 16);
                  postData["ignitionOn"] = data.ignitionOn;
                }
                // Sleep Mode ID = 200
                else if (id == "c8") {
                  data.sleepMode = std::stoi(stringSubstr(hex_stream, AVL_POS + VALUE_POS, VALUE1_NOB*2), 0, 16); 
                }
              }

              numOfTwoBytesID = std::stoi(stringSubstr(hex_stream, AVL_POS + NUM_OF_2B_IO_POS, NUM_OF_IO_NOB*2), 0, 16);
              for (i = 0; i < numOfTwoBytesID; i++) {
                ID_POS = NUM_OF_2B_IO_POS + NUM_OF_IO_NOB*2 + 2*i * (ID_NOB + VALUE2_NOB);
                VALUE_POS = ID_POS + ID_NOB*2;
                id = stringSubstr(hex_stream, AVL_POS + ID_POS, ID_NOB*2);

                // Battery ID = 66
                if (id == "42") {
                  data.exBattVoltage = std::stoi(stringSubstr(hex_stream, AVL_POS + VALUE_POS, VALUE2_NOB*2), 0, 16);
                  postData["exBattVoltage"] = data.exBattVoltage;
                  break;
                }
              }

              // Description of event
              if (event == "ef" && data.ignitionOn == true) data.description = "Ignition turned on!";
              else if (event == "ef" && data.ignitionOn == false) data.description = "Ignition turned off.";
              else data.description = "This is default value";

              if (gps.getRealTimeState()) {
                realTimeElapsed += 5;
                if (realTimeElapsed > 300) {
                  httpsRequest::singleConnect(postData);
                  realTimeElapsed = 0;
                }
              } else {
                postDataVec.push_back(postData);
              }
            }

            std::cout << "IMEI\t\t\t: " << data.imei << std::endl;
            std::cout << "Number of Data\t\t: " << numOfData1 << std::endl;
            std::cout << "Timestamp\t\t: " << "" << data.createdAt << std::endl;

            gps.sendConfirmation(connfd, numOfData2);

            // Print time right after AVL data is received
            data.updatedAt = dateAndTimeNow("WIB");
            std::cout << "Updated At: " << data.updatedAt << "\n\n";
          }
          else if (codec == "0c") {
            std::cout << "GPRS RESPONSE:\n";
            std::cout << hex_stream << "\n\n";
          }

          // Save newest AVL data to database
          std::unique_lock<std::mutex> lk(m);
          if (!cv.wait_until(lk, std::chrono::system_clock::now() + 3s, []{return ready;})){
            return -6;
          }
          else {
            database::createData(data);
          }

          // Post all AVL data one-by-one
          int postDataCount = 0;
          if (!postDataVec.empty() && !gps.getRealTimeState()) {
            for (auto postDataIt: postDataVec) {
              postDataCount++;
              std::cout << "POSTING DATA " << postDataCount << "\n";
              httpsRequest::singleConnect(postDataIt);
            }
            // Clear postDataVec
            postDataVec.clear();
          }

          // Terminate thread if ignition is off
          if (!gps.getRealTimeState() && data.ignitionOn == false) return 1;

        } // if FD_ISSET()

        // Close connection if imei-socket pair is not found
        if (imeiSocketMap.find(data.imei)->second != connfd) return -5;

      } // for(;;)

      return 0;
    } // communicate()

  } // namespace server
} //namespace karlo
