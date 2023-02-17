#ifndef _GPS_TRACKING_SERVER_R11_HPP_
#define _GPS_TRACKING_SERVER_R11_HPP_

// LOGIN PACKET

#define LOGIN_PACKET 18
#define START_BIT 2
#define PACKET_LENGTH 1
#define PROTOCOL_NUMBER 1
#define IMEI 8
#define SERIAL_NUMBER 2
#define ERROR_CHECK 2
#define STOP_BIT 2

// LOCATION DATA PACKET
#define DATE_TIME 6
#define SATELITE_INFO 1
#define LATITUDE 4
#define LONGITUDE 4 
#define SPEED 1 
#define COURSE 2
#define MCC 2 
#define MNC 1 
#define LAC 2 
#define CELL_ID 3 
#define TERMINAL_INFORMATION 1 
#define VOLTAGE 2 
#define SIGNAL_STENGHT 1 
#define ALARAM 1 

#define GPS_PACKET 41
#define HEARTBEAT_PACKET 15

#include "GPS-Tracking/server/read_imei_json.hpp"

#include <string> 

namespace karlo {
    namespace server_R11 {

        using json = nlohmann::json;

        int communicate(int connfd);

        void removeSocket(int socket);
    
    } // server_r11
} // namespace karlo

#endif // _GPS_TRACKING_SERVER_R11_HPP_
