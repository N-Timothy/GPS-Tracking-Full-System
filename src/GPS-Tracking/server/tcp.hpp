#ifndef _GPS_TRACKING_SERVER_TCP_HPP_
#define _GPS_TRACKING_SERVER_TCP_HPP_

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#include <nlohmann/json.hpp>

namespace karlo {
  namespace server {

    using json = nlohmann::json;

    void setTcpConfig(json tcpConfig);

    void tcpServer();

  } // namespace server
} // namespace karlo

#endif // _GPS_TRACKING_SERVER_TCP_HPP_
