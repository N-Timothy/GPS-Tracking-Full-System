#ifndef _GPS_TRACKING_SERVER_TCP_HPP_
#define _GPS_TRACKING_SERVER_TCP_HPP_

#define MAX_PENDING_CONNECTION 3

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

#include <nlohmann/json.hpp>

#include <map>
#include <string>

namespace karlo {
  namespace server {

    using json = nlohmann::json;

    //extern std::map<int, std::pair<int, bool>> timeOutStatus;

    void setTcpConfig(json setTcpConfig);

    void tcpServer();

  } // namespace server
} // namespace karlo

#endif // _GPS_TRACKING_SERVER_TCP_HPP_
