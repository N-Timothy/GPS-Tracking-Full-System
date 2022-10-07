#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/server2.hpp"
#include "GPS-Tracking/server/read_imei_json.hpp"

#include <cstring>
#include <string>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>

#include <mutex>
#include <condition_variable>

namespace karlo {
  namespace server {

    std::vector<json> imei_list;
    std::string IMEI_JSON_LOCATION = "/home/" + getUsername() + "/" + IMEI_JSON_FILENAME;

    std::vector<int> threads;

    int failed_count = 0;

    using json = nlohmann::json;

    json config;

    int time;
    int client_socket[10], thread_timer[10]; // 10 max client temporary

    void setTcpConfig(json setTcpConfig){
        config = setTcpConfig;
    }
    
    void timer() {
        for(;;){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            time++;
            if(time == 60) {
                time = 0;
            }
        }
    }

    void newClient(int socket, std::vector<json> imei_list, fd_set readfds, sockaddr_in address) {

      if (std::find(threads.begin(), threads.end(), socket) == threads.end()) {

        threads.push_back(socket);

        for(auto thread : threads){
            std::cout << " | " << thread;
        } std::cout<< std::endl;
      
        int comm;

        std::cout << "New thread: " << socket << " initialized"<< std::endl;

        comm = communicate(socket, imei_list);
        if (comm == -1) {
            std::cout << "\x1b[31mIMEI is not recognized!\x1b[0m\n";
        }
        else if (comm == -2) {
            imei_list = readImeiJson(IMEI_JSON_LOCATION);
        }
        else if (comm == -3) {
            std::cout << "\x1b[31mThread terminated: Error in socket reading\x1b[0m\n";
        }
        
        threads.erase(std::remove(threads.begin(), threads.end(), socket), threads.end());
        //close(socket);

        }

        if (close(socket) < 0) { failed_count++; }
        std::cout << "Terminating thread: "  << socket << std::endl;
        std::cout << "Failed closing socket count: " << failed_count << "\n";
    }

    void tcpServer () {

      int opt = true;
      int master_socket, addrlen, new_socket, activity, sd, max_sd; 

      struct sockaddr_in address;

      fd_set readfds;

      bool lock = true;

      // Read IMEI JSON an
      imei_list = readImeiJson(IMEI_JSON_LOCATION);

      // initialise all client socket to 0
      for (int i = 0; i < config["max_client"]; i++) {
        client_socket[i] = 0;
      }

      // create master socket
      if ((master_socket = socket (AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
      }

      // set master socket to allow multiple connection
      if (setsockopt(master_socket,SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsocketopt");
        exit(EXIT_FAILURE);
      }

      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons(config["port"]);

      // bind the socket into port
      if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
      }
      std::cout << "Listener on port : " << config["port"] << std::endl;

      //specify maximum pending connection for the master socket
      if (listen(master_socket, MAX_PENDING_CONNECTION) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
      }


      std::thread timerThread(timer);
      timerThread.detach();

      // accept incoming connection
      addrlen = sizeof(address);
      puts("Waiting for connection ...");

      for (;;) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // add child sockets to set
        for (int i = 0 ; i < config["max_client"] ; i++) {
          // socket descriptor
          sd = client_socket[i];

          // if valid socket descriptor then add to read list
          if (sd > 0) {
            FD_SET(sd , &readfds);
          }

          // highest file descriptor number, need it for the select function
          if (sd > max_sd) {
            max_sd = sd;
          }
        }

        // wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
          std::cout << "select error !" << std::endl;
        }

        // If something happened on the master socket, then it's an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {

          // If failed to accept connection
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                    perror("Accept");
                    exit(EXIT_FAILURE);
                }

            for(int i = 0; i < 10; i++){
                std::cout << client_socket[i] << std::endl;
            }

          // inform server of socket number used in send and receive commands
            std::cout << "New connection established! socket : " << new_socket << ", IP : "
                      << inet_ntoa(address.sin_addr) << ", port : " << ntohs(address.sin_port) << std::endl;

          // Adding thread on each new connection
            std::thread newClientThread(newClient, std::cref(new_socket), std::ref(imei_list), std::ref(readfds), std::ref(address));
             newClientThread.detach();
        }
      }
    }
  } // namespace server
} // namespace khrlo
