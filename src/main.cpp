#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/server/tcp.hpp"
// #include "GPS-Tracking/mqtt/subscriber.hpp"
#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"

int main(int argc, char *argv[]){

  karlo::server::tcpServer();
  // karlo::database::database();
  // karlo::mqtt::client();
  // karlo::httpsRequest::connect(10000);

  return 0;
}
