#include "GPS-Tracking/mqtt/toggleController.hpp"
#include "GPS-Tracking/mqtt/publisher.hpp"

#include <string>
#include <iostream>

#include <thread>
#include <chrono>

#include <vector>
#include <algorithm>

namespace karlo {
    namespace mqtt {

            using json = nlohmann::json;


            std::vector<std::string> vectorId;
            // not the best solution but temporarily working

            std::string messageSeparator(std::string message, int index) {
                char delimiter = ',';
                int i = 0;

                for (i = 0; i < message.length(); i++ ) {
                    if(message[i] == delimiter){
                        break;
                    }
                }

                if(index == 0) {
                    return message.substr(0,i);
                } else {
                    return message.substr(i+1, message.length());
                }
                
        }

        void realtimeMessage(std::string imei) {

            for(;;) {

                if (vectorId.empty()){
                    std::cout << "Toggle switched off !" << std::endl;
                    break;
                }

                std::cout << "Publishing Data !" << std::endl;
                publisher(imei);
                std::cout << std::endl;
                std::this_thread::sleep_for (std::chrono::seconds(5));
            }
        }

        void removeVectorElement(std::string id) {

            auto it = find(vectorId.begin(), vectorId.end(), id);
            if (it != vectorId.end()){
                 vectorId.erase(it);
            } 
            
        }

        void toggleController(json subscribeMessage) {

            std::string imei = subscribeMessage["id"];
            std::string driverId = messageSeparator(subscribeMessage["toggle"], 0);
            std::string toggle = messageSeparator(subscribeMessage["toggle"], 1);

            std::cout << "Imei Requested : " << imei << std::endl;
            std::cout << "Driver id : " << driverId << std::endl;
            std::cout << "toggle : " << toggle << std::endl;


            if(toggle == "true") {
                if(vectorId.empty()){
                    vectorId.push_back(driverId);
                    std::thread realtimeMessageThread (realtimeMessage, imei);
                    realtimeMessageThread.detach();
                } else {
                    if(std::find(vectorId.begin(), vectorId.end(), driverId) == vectorId.end()) {
                        vectorId.push_back(driverId);
                    }
                }
            } else {

                removeVectorElement(driverId);

                std::cout << "Last Location Update !" << std::endl;
                publisher(imei);
                std::cout << std::endl;
            }
        }

    } // namespace mqtt
} // namespace karlo
