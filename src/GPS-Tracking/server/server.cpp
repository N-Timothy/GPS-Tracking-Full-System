// TCP Connection, GPS Data-retrieving
#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/data.hpp"
#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/server/server.hpp"
#include "server.hpp" // for development purpose only

#include <stdio.h> // sprintf
#include <unistd.h>
#include <iomanip> // put_time, setprecision
#include <iostream> // cin, cout
#include <cstring> // memset
#include <string> // std::string
#include <algorithm> // find
#include <vector>
#include <thread>

#include <sstream>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace karlo {
  namespace server {

    std::vector<std::string> imeiRealTimeVec;
    std::vector<std::string> imeiNormalVec;

//    std::mutex m;
//    std::condition_variable cv;
//
//    using namespace std::literals::chrono_literals;

    class GetData {
    private:
      int n = 0;
      int number = 0;
//      int ACCEPT = 0x01;
//      int DECLINE = 0x00;
      int connectivity;
      std::string result;


    public:
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
      std::string getBytes2 (int connfd, int byteslen) {
        result = getBytes(connfd, byteslen);
        std::cout << result;
        return result;
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

        for (auto i = imei_list.begin(); i != imei_list.end(); i++) {
          if (imei == *i) return 0;
        }
        return -1;
      }
      int imeiConfirmation(int connfd, int recognized) {
        unsigned char ACCEPT[] = {0x01};
        unsigned char DECLINE[] = {0x00};
        if (recognized == 0) {
          send(connfd, (char *) &ACCEPT, sizeof(ACCEPT), 0);
          printf("IMEI Recognized! [1]\n");
          return 0;
        }
        else {
          printf("IMEI NOT Recognized! [0]\n");
          if (recognized == -1) {
            send(connfd, (char *) &DECLINE, sizeof(DECLINE), 0);
            return -1;
          }
          else return -2;
        }
      }
      int imeiCheckForDatabase(std::string imei, std::vector<json> imei_list) {
        for (auto i = imei_list.begin(); i != imei_list.end(); i++) {
          if (imei == *i) return 0;
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
        printf("Received confirmation: %d data\n\n", numOfData);
      }

      void sendGPRSCommand(int connfd, bool realTimeFlag) {
        std::cout << "SERVER COMMAND...\n";

        // GPRS Command
        typedef unsigned char byte;

        // setparam 10050:10;10150:10;10250:10
        byte realtime_command[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b,
                                   0x0c, 0x01, 0x05,
                                   0x00, 0x00, 0x00, 0x23,
                                   's', 'e', 't', 'p', 'a', 'r', 'a', 'm', ' ',
                                   '1', '0', '0', '5', '0', ':', '1', '0', ';',
                                   '1', '0', '1', '5', '0', ':', '1', '0', ';',
                                   '1', '0', '2', '5', '0', ':', '1', '0',
                                   0x01,
                                   0x00, 0x00, 0xee, 0x70};

        byte normal_command[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b,
                                 0x0c, 0x01, 0x05,
                                 0x00, 0x00, 0x00, 0x23,
                                 's', 'e', 't', 'p', 'a', 'r', 'a', 'm', ' ',
                                 '1', '0', '0', '5', '0', ':', '6', '0', ';',
                                 '1', '0', '1', '5', '0', ':', '6', '0', ';',
                                 '1', '0', '2', '5', '0', ':', '6', '0',
                                 0x01,
                                 0x00, 0x00, 0x53, 0x42};


        if (realTimeFlag) send(connfd, (char *) &realtime_command, sizeof(realtime_command), 0);
        else send(connfd, (char *) &normal_command, sizeof(normal_command), 0);
        std::cout << "COMMAND SEND SUCCESS\n";

      }
    };

    void toRealTime(std::string imei, bool toggle) {
      if (toggle) imeiRealTimeVec.push_back(imei);
      else imeiNormalVec.push_back(imei);
    }

    std::vector<std::string> removeImei(std::vector<std::string> vector, std::string value) {
      auto it = std::find(vector.begin(), vector.end(), value);
      if (it != vector.end()) {
        vector.erase(it);
      }
      return vector;
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
      dateAndTime << std::put_time(std::localtime(&t_c), "%A, %F, %T [WIB]");
      year << std::put_time(std::localtime(&t_c), "%F");
      if(year.str() == "1970-01-01" || year.str() == ""){
        return "";
      }
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
      for(unsigned i = 0; i < binary.size(); i++) {
        if(binary[i] == '1'){
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

    float hexToLongitudeLatitude(std::string hex) {
      std::string bin;
      bin = hexToBin(hex);
      if (bin[0] == '0') {
        return binToDecimal(bin) / 1e7;
      } else {
        return -(binToDecimal(TwoComplement(bin)) + 1) / 1e7;
      }
    }


// Function designed for chat between client and server.
    int communicate(int connfd, std::vector<json> imei_list) {

      // init struct data
      trackingData data;

      std::string buff;
      unsigned i, numOfData1, numOfData2;
      int confirm, recognition;
      unsigned numOfOneByteID, numOfTwoBytesID, numOfFourBytesID, numOfEightBytesID;
      std::string imei_raw, codec;
      std::string hex;
      std::string id;

      int data_NOB, AVL_NOB;
      int AVL_POS, ID_POS, VALUE_POS, NUM_OF_DATA2_POS;
      std::string hex_stream;

      int activity;
      struct timeval tv;

      GetData gps;

      fd_set readfds;

      //clear the socket set
      FD_ZERO(&readfds);

      // Get IMEI number from new connection
      imei_raw = gps.getBytes(connfd, IMEI_NOB);
      if (imei_raw == "") {
        return -3;
      }
//      recognition = gps.imeiRecognition(imei_raw, imei_list);
//      if (recognition == -1) return -1;
//      else if (recognition == -2) return -2;
      confirm = gps.imeiConfirmation(connfd, gps.imeiRecognition(imei_raw, imei_list));
      if (confirm == -1) return -1;
      else if (confirm == -2) return -2;
      data.imei = gps.slice_imei(imei_raw);
      std::cout << "IMEI\t\t\t: " << data.imei << std::endl;

      // Continuously reading AVL Data as long as connection is linked
      for(;;) {
        // Check for GPRS command
        if (std::find(imeiRealTimeVec.begin(), imeiRealTimeVec.end(), data.imei) != imeiRealTimeVec.end()) {
          std::cout << "Changing to realtime..\n";
          gps.sendGPRSCommand(connfd, true);
          std::cout << "imeiRealTime: ";
          for (auto it = imeiRealTimeVec.begin(); it != imeiRealTimeVec.end(); it++) {
            std::cout << *it << " ";
          }
          std::cout << std::endl;
          imeiRealTimeVec = removeImei(imeiRealTimeVec, data.imei);
          std::cout << "imeiRealTime: ";
          for (auto it = imeiRealTimeVec.begin(); it != imeiRealTimeVec.end(); it++) {
            std::cout << *it << " ";
          }
          std::cout << std::endl;
          std::cout << "Changed to realtime\n";
        }
        if (std::find(imeiNormalVec.begin(), imeiNormalVec.end(), data.imei) != imeiNormalVec.end()) {
          std::cout << "Changing to normal..\n";
          gps.sendGPRSCommand(connfd, false);
          std::cout << "imeiNormal: ";
          for (auto it = imeiNormalVec.begin(); it != imeiNormalVec.end(); it++) {
            std::cout << *it << " ";
          }
          std::cout << std::endl;
          imeiNormalVec = removeImei(imeiNormalVec, data.imei);
          std::cout << "imeiNormal: ";
          for (auto it = imeiNormalVec.begin(); it != imeiNormalVec.end(); it++) {
            std::cout << *it << " ";
          }
          std::cout << std::endl;
          std::cout << "Changed to normal\n";
        }


        // Read data or response from devices
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        FD_SET(connfd, &readfds);
        activity = select(connfd + 1, &readfds, NULL, NULL, &tv);

        if (FD_ISSET(connfd, &readfds)) {
          std::cout << "Activity: " << activity << "\n";

          std::cout << "=== Beginning of Zero Bytes ===\n";

//          // Uncomment if "network ping timeout" is configured
//          hex = gps.getBytes(connfd, 1);
//          if (hex == "ff") {
//            std::cout << "Network ping\t\t: " << hex << std::endl;
//            continue;
//          }
//          else if (hex == "00") {
//            hex = gps.getBytes(connfd, ZERO_NOB-1);
//          }

          hex = gps.getBytes(connfd, ZERO_NOB);
          std::cout << "Zero Bytes\t\t: " << hex << std::endl;
          if (hex == "" || hex != "00000000") {
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::cout << std::put_time(std::localtime(&in_time_t), "%A, %F, %T [WIB]");
            std::cout << "\n";
            return -3;
          }

          hex = gps.getBytes(connfd, DATA_FIELD_NOB);
          std::cout << "Data Field Length\t: " << hex << std::endl;
          if (hex == "") return -3;

          try {
            data_NOB = std::stoi(hex, 0, 16);
          } catch (const std::out_of_range& oor) {
            return -4;
          }

          // Reading all remaining hexadecimal stream that comes after data field length
          hex_stream = gps.getBytes(connfd, data_NOB + CRC16_NOB);
          if (hex_stream == "") return -3;

          codec = hex_stream.substr(CODEC_ID_POS, CODEC_ID_NOB*2);

          if (codec == "08") {
            numOfData1 = std::stoi(hex_stream.substr(NUM_OF_DATA1_POS, NUM_OF_DATA_NOB*2), 0, 16);

            AVL_NOB = data_NOB - CODEC_ID_NOB - 2 * NUM_OF_DATA_NOB;
            AVL_POS = CODEC_ID_POS + NUM_OF_DATA1_POS + 2 * NUM_OF_DATA_NOB + 2 * (AVL_NOB/numOfData1) * (numOfData1-1);

            data.createdAt = timestampToDate(hex_stream.substr(AVL_POS, TIMESTAMP_NOB*2));
            data.longitude = hexToLongitudeLatitude(hex_stream.substr(AVL_POS + LONGITUDE_POS, LONGITUDE_NOB*2));
            data.latitude = hexToLongitudeLatitude(hex_stream.substr(AVL_POS + LATITUDE_POS, LATITUDE_NOB*2));
            data.bearing = std::stoi(hex_stream.substr(AVL_POS + ANGLE_POS, ANGLE_NOB*2), 0, 16);
            data.speed = std::stoi(hex_stream.substr(AVL_POS + SPEED_POS, SPEED_NOB*2), 0, 16);

            numOfOneByteID = std::stoi(hex_stream.substr(AVL_POS + NUM_OF_1B_IO_POS, NUM_OF_IO_NOB*2), 0, 16);
            for (i = 0; i < numOfOneByteID; i++) {
              ID_POS = ONE_BYTE_ID_POS + 2*i * (ID_NOB + VALUE1_NOB);
              VALUE_POS = ID_POS + 2 * ID_NOB;
              id = hex_stream.substr(AVL_POS + ID_POS, ID_NOB*2);

              // Ignition ID = 239
              if (id == "ef") {
                data.gpsOn = std::stoi(hex_stream.substr(AVL_POS + VALUE_POS, VALUE1_NOB*2), 0, 16);
                break;
              }
            }

            NUM_OF_DATA2_POS = 2 * (data_NOB - NUM_OF_DATA_NOB);
            numOfData2 = std::stoi(hex_stream.substr(NUM_OF_DATA2_POS, NUM_OF_DATA_NOB*2), 0, 16);

            if (numOfData1 != numOfData2) return -3;

            std::cout << "Codec ID\t\t: " << codec << "\n";
            std::cout << "Number of Data\t\t: " << numOfData1 << "\n";
            std::cout << "Timestamp\t\t: " << hex_stream.substr(AVL_POS, TIMESTAMP_NOB*2) << "(" << data.createdAt << ")\n";
            std::cout << "Longitude\t\t: " << hex_stream.substr(AVL_POS + LONGITUDE_POS, LONGITUDE_NOB*2) << "\n";
            std::cout << "Latitude\t\t: " << hex_stream.substr(AVL_POS + LATITUDE_POS, LATITUDE_NOB*2) << "\n";

            gps.sendConfirmation(connfd, numOfData2);

            // Print time right after AVL data is received
            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);
            std::cout << std::put_time(std::localtime(&in_time_t), "%A, %F, %T [WIB]");
            std::cout << "\n\n";
          }
          else if (codec == "0c") {
            std::cout << "GPRS RESPONSE:\n";
            std::cout << hex_stream;
            std::cout << "\n\n";
          }


//          // Save necessary AVL data to database
//          std::unique_lock<std::mutex> lk(m);
//          if(!cv.wait_until(lk, std::chrono::system_clock::now() + 3s, []{return ready;})){
//            //std::cout << std::endl;
//            //std::cout << "\033[1;32mTIMEOUT .... !! \033[0m";
//            //std::cout << std::endl;
//            return -3;
//          }
//          else {
//            if(gps.imeiCheckForDatabase(data.imei, imei_list) == 0) {
//              database::createData(data);
//            } else {
//              return -1;
//            }
//          }

        } // if FD_SET()
      } // for(;;)

      return 0;
    } // communicate()

  } // namespace server
} //namespace karlo
