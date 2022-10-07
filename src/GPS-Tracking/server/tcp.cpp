#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/server2.hpp"
#include "GPS-Tracking/server/read_imei_json.hpp"

#include <cstring>
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

    std::mutex mtx;
    std::condition_variable con_var;

    std::vector<json> imei_list;
    std::string IMEI_JSON_LOCATION = "/home/" + getUsername() + "/" + IMEI_JSON_FILENAME;

    std::vector<int> threads;
    std::vector<int> failed_socket;

    int failed_count = 0;

    using json = nlohmann::json;

    json config;

    int time;

    bool threadReady = true;

    // std::map<int, std::pair<int, bool>> timeOutStatus;

    void setTcpConfig(json setTcpConfig){
        config = setTcpConfig;
    }
    
    //void timer() {
      //  for(;;){
        //    std::this_thread::sleep_for(std::chrono::seconds(1));
          //  time++;
            //if(time == 60) {
              //  time = 0;
            //}

            //if()
       // }
   // }

    void newClient(int socket, std::vector<json> imei_list) {

        threadReady = true;
        con_var.notify_one();


        // inserting into map need to be warap with std::make_pair
      //  timeOutStatus.insert(std::make_pair(socket, std::make_pair(time, false)));
      //
        for(auto thread : threads){
            std::cout << " | " << thread;
        } std::cout<< std::endl;
      
        int comm;

        std::cout << "New thread: " << socket << " initialized " << std::endl;

        
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
        
        //close(socket);

        if (close(socket) < 0) { 
            failed_count++;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (close(socket) < 0) {
                failed_socket.push_back(socket);
                std::cout << "\x1b[31msocket that failed : \x1b[0m" << socket << std::endl;
            } else {
                threads.erase(std::remove(threads.begin(), threads.end(), socket), threads.end());
                failed_count--;
            }
        } else {
            threads.erase(std::remove(threads.begin(), threads.end(), socket), threads.end());
        }

        std::cout << "Terminating thread: "  << socket << std::endl;
        std::cout << "Failed closing socket count: " << failed_count << "\n";
    }

    void tcpServer () {

      int opt = true;
      int master_socket, address_len, new_socket; 

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


      //std::thread timerThread(timer);
      //timerThread.detach();

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

            if(failed_socket.size() >= 3) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                for(int fSocket : failed_socket){
                    std::cout << "closing socket : " << fSocket << std::endl;
                    failed_socket.erase(std::remove(failed_socket.begin(), failed_socket.end(), fSocket), failed_socket.end());
                    close(fSocket);
                    failed_count = 0;
                } 
            }

          // If failed to accept connection
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&address_len)) < 0) {
                    perror("Accept");
                    exit(EXIT_FAILURE);
                }

            if (std::find(threads.begin(), threads.end(), new_socket) == threads.end()) {
            threads.push_back(new_socket);
          // inform server of socket number used in send and receive commands
          //
            std::unique_lock<std::mutex> lock(mtx);
            con_var.wait(lock, [] {return threadReady;});

            threadReady = false;

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
