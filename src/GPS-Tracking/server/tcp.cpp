#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/server2.hpp"
#include "GPS-Tracking/server/read_imei_json.hpp"
#include "GPS-Tracking/core/config.hpp"

#include <cstring>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>
#include <iomanip>

#include <mutex>
#include <condition_variable>

namespace karlo {
  namespace server {

    std::mutex mtx;
    std::condition_variable con_var;

    std::vector<json> imei_list;
    std::string IMEI_JSON_LOCATION = "/home/" + core::config::getUsername() + "/" + IMEI_JSON_FILENAME;

    std::vector<int> init_socket;
    std::vector<int> thread_socket;
    std::vector<int> diff;

    //int failed_count = 0;

    using json = nlohmann::json;

    json config;

    void setTcpConfig(json setTcpConfig){
        config = setTcpConfig;
    }

    void newClient(int socket, std::vector<json> imei_list) {

        if (std::find(thread_socket.begin(), thread_socket.end(), socket) == thread_socket.end()) {
            
            thread_socket.push_back(socket);

            for(auto thread : init_socket){
                std::cout << " | " << thread;
            } std::cout<< std::endl;
      
            int comm;

            std::cout << "New thread: " << socket << " initialized " << std::endl;
        
            comm = communicate(socket, imei_list);

            if (comm == -1) {
       //         std::cout << "\x1b[31mIMEI is not recognized!\x1b[0m\n";
            }
            else if (comm == -2) {
                imei_list = readImeiJson(IMEI_JSON_LOCATION);
            }
            else if (comm == -3) {
         //       std::cout << "\x1b[31mThread terminated: Error in socket reading\x1b[0m\n";
            }
        } else {
            return;
        }
        
        if (close(socket) < 0) { 
      //      std::cout << "failed"<< std::endl;
            // failed_count++;
        } else {
        //    std::cout << "success" << std::endl;
            init_socket.erase(std::remove(init_socket.begin(), init_socket.end(), socket), init_socket.end());
        }

        thread_socket.erase(std::remove(thread_socket.begin(), thread_socket.end(), socket), thread_socket.end());
        std::cout << "Terminating thread: "  << socket << std::endl;

        //std::cout << "Failed closing socket count: " << failed_count << "\n";
    }
    
    void tcpServer () {

      int opt = true;
      int master_socket, address_len, new_socket; 
      int PrevSocket = 0, socketCounter = 0;
      
      // idk adding another file descriptor prevention

      struct sockaddr_in address;

      fd_set readfds;

      // Read IMEI JSON an
      imei_list = readImeiJson(IMEI_JSON_LOCATION);
    
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

      // accept incoming connection
      address_len = sizeof(address);
      puts("Waiting for connection ...");

      for (;;) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);

        // If something happened on the master socket, then it's an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {

          // If failed to accept connection
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&address_len)) < 0) {
                    perror("Accept");
                    exit(EXIT_FAILURE);
                }

            // Adding socket vector comparison
            std::set_difference(init_socket.begin(), init_socket.end(), thread_socket.begin(), thread_socket.end(),
                std::inserter(diff, diff.begin()));
            std::set_difference(thread_socket.begin(), thread_socket.end(), init_socket.begin(), init_socket.end(),
                std::inserter(diff, diff.begin()));

            if (std::find(init_socket.begin(), init_socket.end(), new_socket) == init_socket.end()) {

 
            init_socket.push_back(new_socket);

//            std::cout << "INIT SOCKET   : ";
//            for (auto i : init_socket) { 
 //               std::cout << i << ' ';
  //          }
   //         std::cout << std::endl;

            std::cout << "THREAD SOCKET : ";
            for (auto i : thread_socket) {
                std::cout << i << ' ';
            }


            if(!thread_socket.empty()){
                std::cout << std::endl;
                if(thread_socket.front() == PrevSocket){
                    socketCounter++;
                    std::cout << "Counter : " << socketCounter << "  Socket Number : " << PrevSocket << std::endl;
                } else {
                    socketCounter = 0;
                    PrevSocket = thread_socket.front();
                }

                // assuming after 10 loops the thread still dosent closed
                if(socketCounter >= 10){
//                    if(close(PrevSocket) < 0) {
  //                      std::cout << "failed to remove socket" << std::endl;
    //                } else {
      //                  std::cout << "sucess to remove socket" << std::endl;
                        init_socket.erase(std::remove(init_socket.begin(), init_socket.end(), PrevSocket), init_socket.end());
                        thread_socket.erase(std::remove(init_socket.begin(), init_socket.end(), PrevSocket), init_socket.end());
                        PrevSocket = 0;
                        socketCounter = 0;
                    //}
                }
            } else {
                PrevSocket = 0;
                socketCounter = 0;
            }
            

                if(!diff.empty()){
        //            std::cout << std::endl;
      //              std::cout << "TRY TO CLOSE UNCLOSED SOCKET" << std::endl;
        //            std::cout << std::endl;
        //
                    for (auto i : diff) {
                        if(close(i) < 0) {
          //                  std::cout << "Failed to close socket try again next time" << std::endl;
                        } else {
            //                std::cout << "\033[1;34mclosing : \033[0m" << i << ' ' << std::endl;
                            init_socket.erase(std::remove(init_socket.begin(), init_socket.end(), i), init_socket.end());
                            thread_socket.erase(std::remove(init_socket.begin(), init_socket.end(), i), init_socket.end());
                            diff.erase(std::remove(diff.begin(), diff.end(), i), diff.end());
                        }
                    }
            }             std::cout << std::endl;

            std::cout << "New connection established! socket : " << new_socket << ", IP : "
                      << inet_ntoa(address.sin_addr) << ", port : " << ntohs(address.sin_port) << std::endl;
            
          // Adding thread on each new connection
            std::thread newClientThread(newClient, std::cref(new_socket), std::ref(imei_list));
            newClientThread.detach();
        }
      }
     }
    }
  } // namespace server
} // namespace karlo
