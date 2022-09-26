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

namespace karlo {
  namespace server {

class getData {
private:
    int n = 0;
    int number = 0;
    int ACCEPT = 0x01; // 1 byte
    int valread;
    char buff[MAX];
    std::string result;


public:
    std::string getBytes (int connfd, char* buff, int byteslen) {
        for (n = 0; n < byteslen; n++) {
            valread = recv(connfd, (char *) &number, 1, 0);
            if (n == 0) sprintf(buff, "%02x", number);
            else sprintf(buff + strlen(buff), "%02x", number);

            if (valread == 0) {
                //Somebody disconnected , get his details and print
                printf("\n=== Host disconnected ===\n\n");

                //Close the socket and mark as 0 in list for reuse
                close(connfd);
            }
        }
        return buff;
    }

    std::string getBytesFinal (int connfd, char* buff, int byteslen) {
        for (n = 0; n < byteslen; n++) {
            valread = recv(connfd, (char*)&number, 1, 0);
            if (n == 0) sprintf(buff, "%02x", number);
            else sprintf(buff + strlen(buff), "%02x", number);

            if (valread == 0) {
                //Somebody disconnected , get his details and print
                printf("\n=== Host disconnected ===\n\n");

                //Close the socket and mark as 0 in list for reuse
//                close(connfd);
            }
            else {
                printf("[Nelson Checkpoint] Valread = %d\n", valread);
            }
        }
        return buff;
    }

    std::string getImei(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "IMEI\t\t\t: " << result << std::endl;
        send(connfd, (char*) &ACCEPT, sizeof(ACCEPT), 0);
        printf("Request confirmation: %x\n\n", ACCEPT);
        return result;
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
    std::chrono::milliseconds ms(x);
    std::chrono::time_point<std::chrono::system_clock> sc(ms);

    std::time_t t_c = std::chrono::system_clock::to_time_t(sc);
    std::stringstream timeStamp;
    timeStamp << std::put_time(std::localtime(&t_c), " (%F %T [%Z])\n");
    std::cout << std::put_time(std::localtime(&t_c), " (%F %T [%Z])\n");
    return timeStamp.str();
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
    }
}

std::string hexToBin(std::string hex) {
    std::string bin;
    for (unsigned i = 0; i < hex.size(); i++)
        bin += hexCharToBin(hex[i]);
    return bin;
}

std::string TwoComplement(std::string binary) {
    for(int i = 0; i < binary.size(); i++) {
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
    number += std::stoi(bin, 0, 2);
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
void func(int connfd) {

    // init struct data
    trackingData data;

    char buff[MAX];
    int n, i, numOfData;
    int numOfOneByteID, numOfTwoBytesID, numOfFourBytesID, numOfEightBytesID;
    std::string hex;
    float result;
    getData gps;

    // Get IMEI number for initialization
    gps.getImei(connfd, buff, IMEI_BYTES);

    gps.getZeroBytes(connfd, buff, ZERO_BYTES);

    gps.getDataFieldLength(connfd, buff, DATA_FIELD_BYTES);

    gps.getCodecID(connfd, buff, CODEC_ID_BYTES);

    numOfData = std::stoi(gps.getNumOfData(connfd, buff, NUM_OF_DATA_BYTES), 0, 16);

    for (n = 0; n < numOfData; n++) {
        hex = gps.getTimestamp(connfd, buff, TIMESTAMP_BYTES);

        data.createdAt = timestampToDate(hex);

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

        hex = std::stoi(gps.getSpeed(connfd, buff, SPEED_BYTES), 0, 16);
        hex == "" ? hex = '0' : hex = hex;
        std::cout << " (" << hex << ")" << std::endl;
        data.speed = stoi(hex);

        gps.getEventIOID(connfd, buff, 1);

        gps.getNumOfTotalID(connfd, buff, 1);

        numOfOneByteID = std::stoi(gps.getNumOfID(connfd, buff, 1), 0, 16);
        for (i = 0; i < numOfOneByteID; i++) {
            gps.getID(connfd, buff, 1);
            gps.getValue(connfd, buff, 1);
        }
        numOfTwoBytesID = std::stoi(gps.getNumOfID(connfd, buff, 1), 0, 16);
        for (i = 0; i < numOfTwoBytesID; i++) {
            gps.getID(connfd, buff, 1);
            gps.getValue(connfd, buff, 2);
        }
        numOfFourBytesID = std::stoi(gps.getNumOfID(connfd, buff, 1), 0, 16);
        for (i = 0; i < numOfFourBytesID; i++) {
            gps.getID(connfd, buff, 1);
            gps.getValue(connfd, buff, 4);
        }
        numOfEightBytesID = std::stoi(gps.getNumOfID(connfd, buff, 1), 0, 16);
        for (i = 0; i < numOfEightBytesID; i++) {
            gps.getID(connfd, buff, 1);
            gps.getValue(connfd, buff, 8);
        }
    }
    gps.getNumOfData(connfd, buff, NUM_OF_DATA_BYTES);

    gps.getCRC16(connfd, buff, 4);

    gps.sendConfirmation(connfd, numOfData);
    memset(buff, 0, sizeof(buff));

    // send to database to be saved
    database::database(data);

    std::cout << "=== END OF DATA ===\n\n";

}

} // namespace server
} //namespace karlo



// Driver function
