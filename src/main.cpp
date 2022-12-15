#include "GPS-Tracking/core/multiThread.hpp"

#include <signal.h>

int main(int argc, char *argv[]){

    signal(SIGPIPE, SIG_IGN);

    karlo::core::multiThread();

  return 0;
}
