// TCP Connection, GPS Data-retrieving
#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/data.hpp"
#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/server/server2.hpp"

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string>
#include <iomanip>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include <math.h>

#include <sstream>
#include <chrono>
#include <mutex>
#include <condition_variable>

namespace karlo {
  namespace server {

    std::mutex m;
    std::condition_variable cv;

    class GetData {
    private:
      int n = 0;
      int number = 0;
      int ACCEPT = 0x01;
      int DECLINE = 0x00;
      int connectivity;
      char buff[MAX];
      std::string result;


    public:
      std::string getBytes (int connfd, char* buff, int byteslen) {
        for (n = 0; n < byteslen; n++) {
          connectivity = recv(connfd, (char *) &number, 1, 0);
          if (connectivity == 0) break;
          if (n == 0) sprintf(buff, "%02x", number);
          else sprintf(buff + strlen(buff), "%02x", number);
        }
        return buff;
      }

      std::string getImei(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "IMEI\t\t\t: " << result << std::endl;
        return result;
      }
      std::string slice_imei(std::string imei_raw) {
        std::string imei_sliced;
        for (unsigned i = 5; i < imei_raw.length(); i += 2) {
          imei_sliced += imei_raw[i];
        }
        return imei_sliced;
      }
      int imeiRecognition(std::string imei, std::vector<json> imei_list) {
        std::string imei_sliced = slice_imei(imei);

        if (imei.substr(0, 4) != "000f") return -1;

        for (auto i = imei_list.begin(); i != imei_list.end(); i++) {
          if (imei_sliced == *i) return 0;
        }
        return -2;
      }
      int imeiConfirmation(int connfd, int recognized) {
        if (recognized == 0) {
          send(connfd, (char *) &ACCEPT, sizeof(ACCEPT), 0);
          printf("IMEI Recognized! [1]\n");
          return 0;
        }
        else {
          send(connfd, (char *) &DECLINE, sizeof(DECLINE), 0);
          printf("IMEI NOT Recognized! [0]\n");
          if (recognized == -1) return -1;
          else return -2;
        }
      }
      std::string getZeroBytes(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "Zero Bytes\t\t: " << result << std::endl;
        return result;
      }
      std::string getDataFieldLength(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "Data Field Length\t: " << result << std::endl;
        return result;
      }
      std::string getCodecID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "Codec ID\t\t: " << result << std::endl;
        return result;
      }
      std::string getNumOfData(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Number of Data\t\t: " << result << std::endl;
        return result;
      }
      std::string getTimestamp(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Timestamp\t\t: " << result;
        return result;
      }
      std::string getPriority(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "Priority\t\t: " << result << std::endl;
        return result;
      }
      std::string getLongitude(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Longitude\t\t: " << result;
        return result;
      }
      std::string getLatitude(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Latitude\t\t: " << result;
        return result;
      }
      std::string getAltitude(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "Altitude\t\t: " << result << std::endl;
        return result;
      }
      std::string getAngle(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "Angle\t\t\t: " << result << std::endl;
        return result;
      }
      std::string getSatellites(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "Satellites\t\t: " << result << std::endl;
        return result;
      }
      std::string getSpeed(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Speed\t\t\t: " << result;
        return result;
      }
      std::string getEventIOID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "Event IO ID\t\t: " << result << std::endl;
        return result;
      }
      std::string getNumOfTotalID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "N of Total ID\t\t: " << result << std::endl;
        return result;
      }
      std::string getNumOfID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "N1 of One Byte IO\t: " << result << std::endl;
        return result;
      }
      std::string getID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "IO ID\t\t\t: " << result << std::endl;
        return result;
      }
      std::string getValue(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "IO Value\t\t: " << result << std::endl;
        return result;
      }
      std::string getCRC16(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
//        std::cout << "CRC-16\t\t\t: " << result << std::endl;
        return result;
      }
      void sendConfirmation(int connfd, int numOfData) {
        send(connfd, (char*) &numOfData, sizeof(numOfData), 0);
        printf("Received confirmation: %d data\n\n", numOfData);
      }
    };

    std::string timestampToDate (std::string hex) {
      std::istringstream is(hex);
      unsigned long x;
      is >> std::hex >> x;
      std::chrono::milliseconds ms(x + 3600*7*1000);
      std::chrono::time_point<std::chrono::system_clock> sc(ms);

      std::time_t t_c = std::chrono::system_clock::to_time_t(sc);
      std::stringstream dateAndTime;
      std::stringstream Time;
      dateAndTime << std::put_time(std::localtime(&t_c), "%A, %F, %T [WIB])\n");
      std::cout << std::endl;
      Time << std::put_time(std::localtime(&t_c), "%F \n");
      if(Time.str() == "1970-01-01" || Time.str() == ""){
        return "";
      }
      std::cout << std::put_time(std::localtime(&t_c), "(%A, %F, %T [WIB])\n");
      return dateAndTime.str();
    }

    const char* hexCharToBin (char hex_c) {
      switch(toupper(hex_c)) {
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
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
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
          std::cout << "catching out of range " << std::endl;
          return -3;
          
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

      char buff[MAX];
      int n, i, numOfData1, numOfData2;
      int numOfOneByteID, numOfTwoBytesID, numOfFourBytesID, numOfEightBytesID;
      int confirm;
      std::string hex;
      std::string imei_raw;

      GetData gps;

      // Get IMEI number for initialization
      imei_raw = gps.getImei(connfd, buff, IMEI_BYTES);
      confirm = gps.imeiConfirmation(connfd, gps.imeiRecognition(imei_raw, imei_list));
      if (confirm == -2) return -2;
      data.imei = gps.slice_imei(imei_raw);
      memset(buff, 0, sizeof(buff));

      if (gps.getZeroBytes(connfd, buff, ZERO_BYTES) == "") return -3;

      gps.getDataFieldLength(connfd, buff, DATA_FIELD_BYTES);

      gps.getCodecID(connfd, buff, CODEC_ID_BYTES);

      try { 
        numOfData1 = std::stoi(gps.getNumOfData(connfd, buff, NUM_OF_DATA_BYTES), 0, 16);
      } catch (const std::out_of_range& oor) {
          std::cout << "catching out of range " << std::endl;
          return -3;
          
      }

      for (n = 0; n < numOfData1; n++) {
        hex = gps.getTimestamp(connfd, buff, TIMESTAMP_BYTES);

        data.createdAt = timestampToDate(hex);
        if (data.createdAt == ""){
            return -3;
        }

        gps.getPriority(connfd, buff, PRIORITY_BYTES);

        hex = gps.getLongitude(connfd, buff, LONGITUDE_BYTES);
        std::cout << std::fixed << std::setprecision(7) << " (" << hexToLongitudeLatitude(hex) << ")" << std::endl;
        data.longitude = hexToLongitudeLatitude(hex);

        hex = gps.getLatitude(connfd, buff, LATITUDE_BYTES);
        std::cout << std::fixed << std::setprecision(7) << " (" << hexToLongitudeLatitude(hex) << ")" << std::endl;
        data.latitude = hexToLongitudeLatitude(hex);

        gps.getAltitude(connfd, buff, ALTITUDE_BYTES);

        gps.getAngle(connfd, buff, ANGLE_BYTES);

        gps.getSatellites(connfd, buff, SATELLITE_BYTES);

      try { 
        data.speed = std::stoi(gps.getSpeed(connfd, buff, SPEED_BYTES), 0, 16);
      } catch (const std::out_of_range& oor) {
          std::cout << "catching out of range " << std::endl;
          return -3;
          
      }
        std::cout << " (" << data.speed << ")" << std::endl;

        gps.getEventIOID(connfd, buff, 1);
        gps.getNumOfTotalID(connfd, buff, 1);

      try { 
        numOfOneByteID = std::stoi(gps.getNumOfID(connfd, buff, 1), 0, 16);
      } catch (const std::out_of_range& oor) {
          std::cout << "catching out of range " << std::endl;
          return -3;
          
      }
        for (i = 0; i < numOfOneByteID; i++) {
          gps.getID(connfd, buff, 1);
          gps.getValue(connfd, buff, 1);
        }
      try { 
        numOfTwoBytesID = std::stoi(gps.getNumOfID(connfd, buff, 1), 0, 16);
      } catch (const std::out_of_range& oor) {
          std::cout << "catching out of range " << std::endl;
          return -3;
          
      }
        for (i = 0; i < numOfTwoBytesID; i++) {
          gps.getID(connfd, buff, 1);
          gps.getValue(connfd, buff, 2);
        }
      try { 
        numOfFourBytesID = std::stoi(gps.getNumOfID(connfd, buff, 1), 0, 16);
      } catch (const std::out_of_range& oor) {
          std::cout << "catching out of range " << std::endl;
          return -3;
          
      }
        for (i = 0; i < numOfFourBytesID; i++) {
          gps.getID(connfd, buff, 1);
          gps.getValue(connfd, buff, 4);
        }
      try { 
        numOfEightBytesID = std::stoi(gps.getNumOfID(connfd, buff, 1), 0, 16);
      } catch (const std::out_of_range& oor) {
          std::cout << "catching out of range " << std::endl;
          return -3;
          
      }
        for (i = 0; i < numOfEightBytesID; i++) {
          gps.getID(connfd, buff, 1);
          gps.getValue(connfd, buff, 8);
        }
      }
      try { 
        numOfData2 = std::stoi(gps.getNumOfData(connfd, buff, NUM_OF_DATA_BYTES), 0, 16);
      } catch (const std::out_of_range& oor) {
          std::cout << "catching out of range " << std::endl;
          return -3;
      }

      gps.getCRC16(connfd, buff, 4);

      gps.sendConfirmation(connfd, numOfData2);

      // send to database to be saved
      
    //  std::unique_lock<std::mutex> lk(m);
      //cv.wait(lk, []{return ready || timeOutStatus[connfd].second;});
  //    cv.wait(lk, []{return ready;});

      //if(timeOutStatus[connfd].second){
        // std::cout << "timeout" << std::endl;
         //return -3;  
      //} else {
//        database::createData(data);
      //}
      std::cout << "===================\n\n";

      return 0;
    }

  } // namespace server
} //namespace karlo
