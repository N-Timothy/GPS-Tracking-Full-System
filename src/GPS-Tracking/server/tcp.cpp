#include "GPS-Tracking/server/tcp.hpp"
#include "GPS-Tracking/server/decodeMessage.hpp"

#include <cstring>
#include <string>  
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>

#include <thread>

#define TRUE 1
#define FALSE 0
#define PORT 8080
#define MAX_CLIENT 10
#define MAX 80
#define MAX_PENDING_CONNECTION 3

namespace karlo {
  namespace server {

    void newClient(int client_socket, fd_set readfds, int sd, sockaddr_in address) {
      int ACCEPT = 0x01;
      int data = 0;
      int valread;

      bool imei_guard = false;

      char buffer[MAX_BYTES*2];

      //  If it's some IO operation on some other socket

      std::cout << "New thread : " << client_socket << " initialized"<< std::endl;

      sd = client_socket;

      for(;;) {

        if (FD_ISSET(sd, &readfds)) {

         // Check if it was for closing, and also read the incoming message

         // --- re-arrangeing imei data to buffer ---

            for (int j = 0; j < IMEI_BYTES; j++) {
              valread = recv(sd, (char*)&data, 1, MSG_WAITALL);
              j == 0 ? sprintf(buffer, "%02x", data) : sprintf(buffer + strlen(buffer), "%02x", data);
            }

            // ------ antisipating wierd behavior on displying imei afer message was received ----

            for(int j = 0; j < IMEI_BYTES * 2; j++) {
              if(buffer[j] != '0'){
                imei_guard = false;
                break;
              } else {
                imei_guard = true;
              }
            }

            if(imei_guard){
              imei_guard = false;
              continue;
            }

            // ------ end antisipating wierd behavior on displying imei afer message was received ----

            if(valread == 0){
              goto connectionTerminated;
            }

            std::cout << "Ip : " << inet_ntoa(address.sin_addr) << " Port : " << ntohs(address.sin_port) << " IMEI : " << buffer << std::endl;

            // second parameters is indicating if this is an IMEI data.(true)
            decodeMessage(buffer, true);

            send(sd, (char*)&ACCEPT, 1, 0);


          // --- end of re-arranging imei and send ok -----

          // --- re-arrangeing gps data to buffer ----

          for (int j = 0; j < MAX_BYTES; j++) {
            valread = recv(sd, (char*)&data, 1, MSG_WAITALL);
            j == 0 ? sprintf(buffer, "%02x", data) : sprintf(buffer + strlen(buffer), "%02x", data);
          }

          if(valread == 0){
            goto connectionTerminated;
          }

            //// --- end ----

          std::cout << "Ip : " << inet_ntoa(address.sin_addr) << " Port : " << ntohs(address.sin_port) << " Message : " << buffer << std::endl;

          // second parameters is indicating if this is an IMEI data.(false)
          decodeMessage(buffer, false);

          send(sd, (char*)&ACCEPT, 1, 0);

        connectionTerminated:
          if (valread == 0) {
            //Somebody disconnected , get his details and print
            std::cout << "Host Disconnected on socket : " << client_socket << std::endl;;
            // Close the socket and mark as 0 in list for reuse
            std::cout << "Terminating thread : "  << client_socket << std::endl;
            close(sd);
            client_socket = 0;
            break;
          }
        }
      }
    }

    void tcpServer () {

      int opt = true;
      int master_socket, addrlen, new_socket, client_socket[MAX_CLIENT],
        activity, sd, max_sd;

      struct sockaddr_in address;

      fd_set readfds;

      // initialise all client socket to 0
      for (int i = 0; i < MAX_CLIENT; i++) {
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
      address.sin_port = htons(PORT);

      // bind the socket into port
      if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
      }
      std::cout << "Listener on port : " << PORT << std::endl;

      //specify maximum pending connection for the master socket
      if(listen(master_socket, MAX_PENDING_CONNECTION) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
      }

      // accept incoming connection
      addrlen = sizeof(address);
      puts("Waiting for connection ...");

      for(;;) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        // master socket = 3
        // std::cout << "master_socket = " << max_sd << std::endl; 

        // add child sockets to set
        for (int i = 0 ; i < MAX_CLIENT ; i++) {
          // socket descriptor
          sd = client_socket[i];

            // if valid socket descriptor then add to read list
            if(sd > 0) {
              FD_SET(sd , &readfds);
            }

            // highest file descriptor number, need it for the select function
            if(sd > max_sd) {
              max_sd = sd;
            }
        }

        // wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
          // std::cout << "select error !" << std::endl;
        }

        // If something happened on the master socket, then it's an incoming connection
        if (FD_ISSET(master_socket, &readfds))
          {
          // If failed to accept connection
          if ((new_socket = accept(master_socket,
                                   (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
              {
              perror("Accept");
              exit(EXIT_FAILURE);
            }

            // inform user of socket number - used in send and receive commands
            std::cout << "New connection established, socket : " << new_socket << " Ip : " << inet_ntoa(address.sin_addr) << " Port : " << ntohs(address.sin_port) << std::endl;

            // Add new socket to array of sockets
            int i;
            for (i = 0; i < MAX_CLIENT; i++) {
              // Check if position is empty
              if(client_socket[i] == 0 )
                {
                  client_socket[i] = new_socket;
                  std::cout << "Adding socket : " << new_socket << " to list of sockets in position : " << i << std::endl;

                  break;
                }
            }
            // Adding thread on each new connection
            std::thread newClientThread(newClient, std::cref(client_socket[i]), std::ref(readfds), std::ref(sd), std::ref(address));
            newClientThread.detach();
          }
      }
    }

  } // namespace server
} // namespace karlo
