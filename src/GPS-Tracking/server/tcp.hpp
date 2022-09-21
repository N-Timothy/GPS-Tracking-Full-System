#ifndef _GPS_TRACKING_SERVER_TCP_HPP_
#define _GPS_TRACKING_SERVER_TCP_HPP_

#define TRUE 1
#define FALSE 0
#define PORT 8080
#define MAX_CLIENT 10
#define MAX 80
#define MAX_PENDING_CONNECTION 3

namespace karlo {
  namespace server {

    void tcpServer();

  } // namespace server
} // namespace karlo

#endif // _GPS_TRACKING_SERVER_TCP_HPP_
