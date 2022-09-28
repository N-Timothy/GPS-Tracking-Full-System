// #include "GPS-Tracking/database/database.hpp"
// #include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/mqtt/subscriber.hpp"
#include "GPS-Tracking/mqtt/publisher.hpp"
// #include "GPS-Tracking/httpsRequest/httpsRequest.hpp"

int main(int argc, char *argv[]){

  // karlo::server::tcpServer();
  // karlo::database::database();
  karlo::mqtt::subscriber();
  // karlo::mqtt::publisher("5f105ae8a629de65677a0ce7");
  // karlo::httpsRequest::connect();

  return 0;
}
