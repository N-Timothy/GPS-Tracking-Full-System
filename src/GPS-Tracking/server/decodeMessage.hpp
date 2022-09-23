#ifndef _GPS_TRACKING_SERVER_DECODE_MESSAGE_HPP_
#define _GPS_TRACKING_SERVER_DECODE_MESSAGE_HPP_

#include "GPS-Tracking/server/tcp.hpp"

#include <string>

namespace karlo {
  namespace server {

    void decodeMessage(std::string buffer, bool imei_data);

  } // namespace server
} // namespace karlo

#endif // _GPS_TRACKING_SERVER_DECODE_MESSAGE_HPP_
