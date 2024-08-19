#include "GPS-Tracking/core/multiThread.hpp"
#include "GPS-Tracking/server/asio.hpp"

#include <iostream>
#include <signal.h>

int main(int argc, char *argv[]) {

  //    signal(SIGPIPE, SIG_IGN);

  //  karlo::core::multiThread();
  try {
    boost::asio::io_context io_context;
    karlo::tcp::Server server(io_context, 12345,
                              10); // Port 12345, pool size 10
    io_context.run();              // Single-threaded event loop
  } catch (std::exception &e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
