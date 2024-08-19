#include "GPS-Tracking/core/multiThread.hpp"
#include "GPS-Tracking/server/asio.hpp"

#include <iostream>
#include <signal.h>

int main(int argc, char *argv[]) {

  //    signal(SIGPIPE, SIG_IGN);

  //  karlo::core::multiThread();
  try {
    boost::asio::io_context io_context;
    karlo::tcp::Server server(io_context, 8080, 100);
    io_context.run();
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
