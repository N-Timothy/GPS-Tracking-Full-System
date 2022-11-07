//#include "GPS-Tracking/core/multiThread.hpp"
#include "GPS-Tracking/server/tcp.hpp"

#include <signal.h>

int main(int argc, char *argv[]){

//    signal(SIGPIPE, SIG_IGN);

//    karlo::core::multiThread();
    karlo::server::tcpServer();

  return 0;
}
