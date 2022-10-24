#include "GPS-Tracking/mqtt/toggleController.hpp"
#include "GPS-Tracking/mqtt/publisher.hpp"

#include <string>
#include <iostream>

#include <thread>
#include <chrono>

#include <vector>
#include <algorithm>
#include <map>

namespace karlo {
    namespace mqtt {

            using json = nlohmann::json;

            std::map<std::string, std::vector<std::string>> realTimeReq;

            std::vector<std::string> vectorId;

            bool threadActive = false;
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

        void realTimeMessage(std::string imei) {

            int counter;

            for(;;) {

                counter = 0;
                for(auto it = realTimeReq.cbegin(); it != realTimeReq.cend(); ++it) {
                    if(!it->second.empty()){

                   //     std::cout << "Publishing Data ... " << std::endl;
                        int ret = publisher(it->first);
                        // check if data is null
                        if (ret == -1) {
                            realTimeReq.erase(it->first);
               //             std::cout << "No GPS FOUND ... : " << realTimeReq.size() << std::endl;
                            if(realTimeReq.size() > 1 || counter > 0){
                                continue;
                            } else {
                                break;
                            }
                        } else if (ret == -3) {
                 //           std::cout << "MQTT TIMEOUT" << std::endl;
                            continue;
                        } else {

                   //         std::cout << "Succeed ... " << std::endl;
                            counter++;
                        }
                    }
                }

                if (counter < 1){
             //       std::cout << "Toggle switched off !" << std::endl;
                    threadActive = false;
                    break;
                }

           //     std::cout << " 10 sec timer " << std::endl;
                std::cout << std::endl;
                std::this_thread::sleep_for (std::chrono::seconds(config["interval"]));
            }
        }

        std::vector<std::string> removeVectorElement(std::vector<std::string> vecId, std::string id) {
            auto it = find(vecId.begin(), vecId.end(), id);
            if (it != vecId.end()){
                 vecId.erase(it);
            } 
            return vecId;
        }

        void toggleController (json subscribeMessage) {

            std::string imei = subscribeMessage["id"];
            std::string driverId = messageSeparator(subscribeMessage["toggle"], 0);
            std::string toggle = messageSeparator(subscribeMessage["toggle"], 1);

          //  std::cout << "Imei Requested : " << imei << std::endl;
          //  std::cout << "Driver id : " << driverId << std::endl;
          //  std::cout << "toggle : " << toggle << std::endl;

            std::vector<std::string> emptyVec;

            // try to make a new imei map, if imei exist does nothing
            realTimeReq.try_emplace(imei, emptyVec);

            auto data = realTimeReq.find(imei);
            std::string imeiData = data->first;
            std::vector<std::string> idVector = data->second;


            if(toggle == "true") {
                if(idVector.empty()) {
                    idVector.push_back(driverId);
                } else {
                    if(std::find(idVector.begin(), idVector.end(), driverId) == idVector.end()) {
                        idVector.push_back(driverId);
                    }
                }
            } else {
                if(std::find(idVector.begin(), idVector.end(), driverId) != idVector.end()) {
                    idVector = removeVectorElement(idVector, driverId);
                }
                //std::cout << "Last Location Update ! for imei : " << imei << std::endl;
                int res = publisher(imei);
                std::cout << std::endl;

                if (res == -1) {
                    realTimeReq.erase(imei);
                }
            }

            data->second = idVector;

            int idCounter = 0;

                for(auto it = realTimeReq.cbegin(); it != realTimeReq.cend(); ++it) {
                     if(!threadActive && !it->second.empty()) {
                       // std::cout << "new thread" << std::endl;
                        std::thread realTimeMessageThread (realTimeMessage, imei);
                        realTimeMessageThread.detach();
                        threadActive = true;
                        idCounter++;
                        break;
                    } else if (!it->second.empty()){
                        idCounter++;
                        break;
                    } 
                }
                if(idCounter == 0){
                    std::cout << "no thread active" << std::endl;
                    threadActive = false;
                }

            // temporary printing MAP value
            //for(auto it = realTimeReq.cbegin(); it != realTimeReq.cend(); ++it) {
                //std::cout << "imei : " << it->first << " id : ";
              //  for(auto cur : it->second){
                //    std::cout << cur << " | ";
               // }
               // std::cout << std::endl;
            //}
        }

    } // namespace mqtt
} // namespace karlo
