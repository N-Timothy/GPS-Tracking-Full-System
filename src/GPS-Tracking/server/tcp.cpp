#include "GPS-Tracking/server/tcp.hpp"

#include <cstring>
#include <string>  
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define PORT 8080
#define MAX_CLIENT 10
#define MAX 80
#define MAX_PENDING_CONNECTION 3

namespace karlo {
  namespace server {

    void tcpServer () {

      int opt = true;
      int master_socket, addrlen, new_socket, client_socket[MAX_CLIENT], imei_flag[MAX_CLIENT],
          activity, valread, sd, max_sd;

      bool imei_test = false;

      struct sockaddr_in address;
      //struct socket_client client;

      fd_set readfds;

      char buffer[MAX];
      int data = 0;

      int ACCEPT = 0x01;

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
          std::cout << "select error !" << std::endl;
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
            for (int i = 0; i < MAX_CLIENT; i++) {
              // Check if position is empty
              if(client_socket[i] == 0 )
                {
                  client_socket[i] = new_socket;
                  std::cout << "Adding socket : " << new_socket << " to list of sockets in position : " << i << std::endl;

                  break;
                }
            }
          }

        // If it's some IO operation on some other socket
        for (int i = 0; i < MAX_CLIENT; i++) {

          sd = client_socket[i];

          if (FD_ISSET(sd, &readfds)) {
            // Check if it was for closing, and also read the incoming message

            // valread = recv(sd, buffer, MAX, MSG_WAITALL);

            // --- re-arrangeing imei data to buffer ---

            if(!imei_flag) {
              
              for (int i = 0; i < IMEI_BYTES; i++) {
                valread = recv(sd, (char*)&data, 1, 0);
                i == 0 ? sprintf(buffer, "%02x", data) : sprintf(buffer + strlen(buffer), "%02x", data);
            }

              std::cout << "Ip : " << inet_ntoa(address.sin_addr) << " Port : " << ntohs(address.sin_port) << " IMEI : " << buffer << std::endl;

              send(sd, (char*)&ACCEPT, 1, 0);
            }

            // --- end of re-arranging imei and send ok -----

            // --- re-arrangeing gps data to buffer ----

            for (int i = 0; i < MAX_BYTES; i++) {
              valread = recv(sd, (char*)&data, 1, 0);
              i == 0 ? sprintf(buffer, "%02x", data) : sprintf(buffer + strlen(buffer), "%02x", data);
            }

            // --- end ----

            std::cout << "Ip : " << inet_ntoa(address.sin_addr) << " Port : " << ntohs(address.sin_port) << " Message : " << buffer << std::endl;

            if (valread == 0) {
                // Somebody disconnected , get his details and print
                getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                std::cout << "Host disconnected, Ip : " << inet_ntoa(address.sin_addr) << " Port : " << ntohs(address.sin_port) << std::endl;

                // Close the socket and mark as 0 in list for reuse
                close(sd);
                client_socket[i] = 0;
              }

            // Echo back the message that came in
            else {
                // set the string terminating NULL byte on the end of the data read
                // buffer[valread] = '\0';
               //  send(sd, reply, strlen(reply), 0);
            }
          }
        }
      }

    }

  } // namespace server
} // namespace karlo
