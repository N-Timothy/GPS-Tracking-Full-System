//#include "GPS-Tracking/database/database.hpp"
#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/decodeMessage.hpp"

int main(int argc, char *argv[]){

  //karlo::server::decodeMessage("test");
  karlo::server::tcpServer();
  // karlo::database::database();

  return 0;
}