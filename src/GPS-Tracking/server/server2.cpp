// TCP Connection, GPS Data-retrieving

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <iomanip>
#include <iostream>
#include <errno.h>

#include <math.h>

#include <sstream>
#include <chrono>

#define MAX 80
#define PORT 8080
#define MAX_CLIENT 10
#define MAX_PENDING_CONNECTION 3
#define SA struct sockaddr

#define IMEI_BYTES 17
#define ZERO_BYTES 4
#define DATA_FIELD_BYTES 4
#define CODEC_ID_BYTES 1
#define NUM_OF_DATA_BYTES 1
#define TIMESTAMP_BYTES 8
#define PRIORITY_BYTES 1
#define LONGITUDE_BYTES 4
#define LATITUDE_BYTES 4
#define ALTITUDE_BYTES 2
#define ANGLE_BYTES 2
#define SATELLITE_BYTES 1
#define SPEED_BYTES 2

//#define ACCEPT (0x01)
//#define DECLINE (0x00)

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
        std::cout << "Zero Bytes\t\t: " << result << std::endl;
        return result;
    }
    std::string getDataFieldLength(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Data Field Length\t: " << result << std::endl;
        return result;
    }
    std::string getCodecID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Codec ID\t\t: " << result << std::endl;
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
        std::cout << "Priority\t\t: " << result << std::endl;
        return result;
    }
    std::string getLongitude(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Longitude\t\t: " << result << std::endl;
        return result;
    }
    std::string getLatitude(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Latitude\t\t: " << result << std::endl;
        return result;
    }
    std::string getAltitude(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Altitude\t\t: " << result << std::endl;
        return result;
    }
    std::string getAngle(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Angle\t\t\t: " << result << std::endl;
        return result;
    }
    std::string getSatellites(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Satellites\t\t: " << result << std::endl;
        return result;
    }
    std::string getSpeed(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Speed\t\t\t: " << result << std::endl;
        return result;
    }
    std::string getEventIOID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "Event IO ID\t\t: " << result << std::endl;
        return result;
    }
    std::string getNumOfTotalID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "N of Total ID\t\t: " << result << std::endl;
        return result;
    }
    std::string getNumOfID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "N1 of One Byte IO\t: " << result << std::endl;
        return result;
    }
    std::string getID(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "IO ID\t\t\t: " << result << std::endl;
        return result;
    }
    std::string getValue(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "IO Value\t\t: " << result << std::endl;
        return result;
    }
    std::string getCRC16(int connfd, char* buff, int byteslen) {
        result = getBytes(connfd, buff, byteslen);
        std::cout << "CRC-16\t\t\t: " << result << std::endl;
        return result;
    }
    void sendConfirmation(int connfd, int numOfData) {
        send(connfd, (char*) &numOfData, sizeof(numOfData), 0);
        printf("Received confirmation: %d data\n\n", numOfData);
    }
};

void timestampToDate (std::string hex) {
    std::istringstream is(hex);
    unsigned long x;
    is >> std::hex >> x;
    std::chrono::milliseconds ms(x);
    std::chrono::time_point<std::chrono::system_clock> sc(ms);

    std::time_t t_c = std::chrono::system_clock::to_time_t(sc);
    std::cout << std::put_time(std::localtime(&t_c), " (%F %T [%Z])\n");
}


// Function designed for chat between client and server.
void func(int connfd) {
    char buff[MAX];
    int n, i, numOfData;
    int numOfOneByteID, numOfTwoBytesID, numOfFourBytesID, numOfEightBytesID;
    std::string hex;
    getData gps;

    // Get IMEI number for initialization
    gps.getImei(connfd, buff, IMEI_BYTES);
    gps.getZeroBytes(connfd, buff, ZERO_BYTES);
    gps.getDataFieldLength(connfd, buff, DATA_FIELD_BYTES);
    gps.getCodecID(connfd, buff, CODEC_ID_BYTES);
    numOfData = std::stoi(gps.getNumOfData(connfd, buff, NUM_OF_DATA_BYTES), 0, 16);
    for (n = 0; n < numOfData; n++) {
        hex = gps.getTimestamp(connfd, buff, TIMESTAMP_BYTES);
        timestampToDate(hex);
        gps.getPriority(connfd, buff, PRIORITY_BYTES);
        gps.getLongitude(connfd, buff, LONGITUDE_BYTES);
        gps.getLatitude(connfd, buff, LATITUDE_BYTES);
        gps.getAltitude(connfd, buff, ALTITUDE_BYTES);
        gps.getAngle(connfd, buff, ANGLE_BYTES);
        gps.getSatellites(connfd, buff, SATELLITE_BYTES);
        gps.getSpeed(connfd, buff, SPEED_BYTES);
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
    std::cout << "=== END OF DATA ===\n\n";
}
  } // namespace server
} //namespace karlo



// Driver function
