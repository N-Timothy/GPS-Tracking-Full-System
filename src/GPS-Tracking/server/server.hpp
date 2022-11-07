#ifndef _GPS_TRACKING_SERVER_SERVER2_HPP_
#define _GPS_TRACKING_SERVER_SERVER2_HPP_

// Change value based on GPS configuration
#define NUM_OF_ONE_BYTE_ID 2
#define NUM_OF_TWO_BYTES_ID 1
#define NUM_OF_THREE_BYTES_ID 0
#define NUM_OF_FOUR_BYTES_ID 0
// END

#define IMEI_NOB 17
#define ZERO_NOB 4
#define DATA_FIELD_NOB 4
#define CODEC_ID_NOB 1
#define NUM_OF_DATA_NOB 1
#define TIMESTAMP_NOB 8
#define PRIORITY_NOB 1
#define LONGITUDE_NOB 4
#define LATITUDE_NOB 4
#define ALTITUDE_NOB 2
#define ANGLE_NOB 2
#define SATELLITE_NOB 1
#define SPEED_NOB 2
#define EVENT_IO_ID_NOB 1
#define NUM_OF_TOTAL_ID_NOB 1
#define NUM_OF_IO_NOB 1
#define ID_NOB 1
#define VALUE1_NOB 1
#define VALUE2_NOB 2
#define VALUE4_NOB 4
#define VALUE8_NOB 8
#define CRC16_NOB 4

#define ZERO_POS 0
#define DATA_FIELD_POS 8

// Position calculated from after data field length
#define CODEC_ID_POS 0
#define NUM_OF_DATA1_POS 2

// Position with respect to AVL position (AVL_POS)
#define TIMESTAMP_POS 0
#define PRIORITY_POS 16
#define LONGITUDE_POS 18
#define LATITUDE_POS 26
#define ALTITUDE_POS 34
#define ANGLE_POS 38
#define SATELLITE_POS 42
#define SPEED_POS 44
#define EVENT_IO_ID_POS 48
#define NUM_OF_TOTAL_ID_POS 50
#define NUM_OF_1B_IO_POS 52
#define NUM_OF_2B_IO_POS (NUM_OF_1B_IO_POS + NUM_OF_IO_NOB*2 + NUM_OF_ONE_BYTE_ID*(ID_NOB + VALUE1_NOB)*2)
#define NUM_OF_4B_IO_POS (NUM_OF_2B_IO_POS + NUM_OF_IO_NOB*2 + NUM_OF_TWO_BYTES_ID*(ID_NOB + VALUE2_NOB)*2)
#define NUM_OF_8B_IO_POS (NUM_OF_4B_IO_POS + NUM_OF_IO_NOB*2 + NUM_OF_FOUR_BYTES_ID*(ID_NOB + VALUE4_NOB)*2)


#include "GPS-Tracking/server/read_imei_json.hpp"

#include <string>

namespace karlo {
  namespace server {

    int communicate(int connfd, std::vector<json> imei_list);

    void toRealTime(std::string imei, bool toggle);

  } // namespace server
} // namespace karlo

#endif // _GPS_TRACKING_SERVER_SERVER2_HPP_
