#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/server.hpp"
#include "GPS-Tracking/server/read_imei_json.hpp"
#include "GPS-Tracking/core/config.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <iostream> // cin, cout
#include <algorithm> // find
#include <vector>
#include <thread>

#include <mutex>
#include <condition_variable>

namespace karlo {
  namespace server {

    std::mutex mtx;
    std::condition_variable con_var;

    std::vector<json> imei_list;
    std::string IMEI_JSON_LOCATION = "/home/" + core::config::getUsername() + "/" + IMEI_JSON_FILENAME;

    std::vector<int> failed_socket;
    int failed_count = 0;

    using json = nlohmann::json;

    json config;

    void setTcpConfig(json setTcpConfig){
      config = setTcpConfig;
    }

    void newClient(int socket, std::vector<json> imei_list) {

      int comm;

      std::cout << "New thread: " << socket << " initialized " << std::endl;

      comm = communicate(socket, imei_list);

      if (comm == -1) {
        imei_list = readImeiJson(IMEI_JSON_LOCATION);
      }
      else if (comm == -2) {
        std::cout << "\x1b[31mSocket " << socket << ": IMEI not recognized\x1b[0m\n";
      }
      else if (comm == -3) {
        std::cout << "\x1b[31mSocket " << socket << ": error in reading\x1b[0m\n";
      }
      else if (comm == -4) {
        std::cout << "\x1b[31mSocket " << socket << ": stoi out of range\x1b[0m\n";
      }
      else if (comm == -5) {
        std::cout << "\x1b[31mSocket " << socket << ": socket is not used\x1b[0m\n";
      }

      removeSocket(socket);

      if (close(socket) < 0) {
        std::cout << "\x1b[Failed to close socket: \x1b[0m"<< socket << std::endl;
        failed_socket.push_back(socket);
        failed_count++;
      }
      std::cout << "Failed socket: [ ";
      for (auto it: failed_socket) {
        std::cout << it << " ";
      }
      std::cout << " ] | Failed count: " << failed_count << "\n";

      std::cout << "Terminating thread: "  << socket << std::endl;

    }

    void tcpServer () {

      int opt = true;
      int master_socket, address_len, new_socket;

      struct sockaddr_in address;
      struct timeval tv;

      fd_set readfds;

      // Read IMEI JSON
      imei_list = readImeiJson(IMEI_JSON_LOCATION);

      // Create master socket
      if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
      }

      // Set master socket to allow multiple connection
      if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsocketopt");
        exit(EXIT_FAILURE);
      }

      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons(config["port"]);

      // Bind the socket into port
      if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
      }
      std::cout << "Listener on port : " << config["port"] << std::endl;

      // Specify maximum pending connection for the master socket
      if (listen(master_socket, MAX_PENDING_CONNECTION) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
      }

      // Accept incoming connection
      address_len = sizeof(address);
      puts("Waiting for connection ...");

      for (;;) {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add master socket to set
        FD_SET(master_socket, &readfds);

        // Assign time val every loop
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        // Check if there's activity on socket
        select(master_socket + 1, &readfds, NULL, NULL, &tv);

        // If master_socket is still set to the file descriptor, then it's an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {

          // If failed to accept connection
          if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&address_len)) < 0) {
            perror("Accept");
            exit(EXIT_FAILURE);
          }


          std::cout << "\x1b[32m[New connection] Socket: " << new_socket << " | IP: "
                    << inet_ntoa(address.sin_addr) << " | Port: " << ntohs(address.sin_port) << "\x1b[0m\n";

          // Adding thread on each new connection
          std::thread newClientThread(newClient, std::cref(new_socket), std::ref(imei_list));
          newClientThread.detach();

        }

        for (auto it: failed_socket) {
          if (close(it) == 0) {
            failed_socket.erase(std::find(failed_socket.begin(), failed_socket.end(), it));
          }
          else {
            std::cout << "Failed to close socket " << it << " again\n";
          }
        }

      } // for(;;)
    }
  } // namespace server
} // namespace karlo
