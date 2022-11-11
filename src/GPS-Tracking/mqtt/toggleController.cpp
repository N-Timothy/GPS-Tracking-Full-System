#include "GPS-Tracking/mqtt/toggleController.hpp"
#include "GPS-Tracking/mqtt/publisher.hpp"
#include "GPS-Tracking/server/server.hpp"

#include <string>
#include <iostream>

#include <thread>
#include <chrono>

#include <vector>
#include <algorithm>
#include <map>
#include <condition_variable>
#include <mutex>

namespace karlo {
  namespace mqtt {

    std::mutex mtx;
    std::condition_variable con_var;

    using namespace std::literals::chrono_literals;

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
        return message.substr(i+1, 4);
      }

    }

    int publisher_thread(std::string imei) {

      std::unique_lock<std::mutex> lk(mtx);
      if(!con_var.wait_until(lk, std::chrono::system_clock::now() + 3s, []{return pub_ready;})){
        std::cout << "publish is beeing used" << std::endl;
        return -1;
      } else {
        std::thread pub_thread(publisher, imei);
        pub_thread.detach();
        // wait for this operation to end
        return 0;
      }
    }

    void realTimeMessage(std::string imei) {

      int counter;

      for(;;) {

        if (null_imei != ""){
          realTimeReq.erase(null_imei);
        }

        counter = 0;
        for(auto it = realTimeReq.cbegin(); it != realTimeReq.cend(); ++it) {

          if(!it->second.empty()){


            int res = publisher_thread(it->first);

            // check if data is null
            if (res == -1) {
              realTimeReq.erase(it->first);
              //             std::cout << "No GPS FOUND ... : " << realTimeReq.size() << std::endl;
              if(realTimeReq.size() > 1 || counter > 0){
                continue;
              } else {
                break;
              }
//            } else if (res == -3) {
//              std::cout << "MQTT TIMEOUT" << std::endl;
//              continue;
            } else {

//              std::cout << "Succeed ... " << std::endl;
              counter++;
            }
          }
        }

        if (counter < 1){
//          std::cout << "Toggle switched off !" << std::endl;
          threadActive = false;
          break;
        }

//        std::cout << " 10 sec timer " << std::endl;
//        std::cout << std::endl;
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

      if(!subscribeMessage.is_null()) {

        std::string imei = subscribeMessage["id"];

        //std::cout << "Imei Requested : " << imei << std::endl;

        std::string driverId = messageSeparator(subscribeMessage["toggle"], 0);

        //std::cout << "Driver id : " << driverId << std::endl;

        std::string toggle = messageSeparator(subscribeMessage["toggle"], 1);

        //std::cout << "toggle : " << toggle << std::endl;

        std::vector<std::string> emptyVec;

        // try to make a new imei map, if imei exist does nothing
        realTimeReq.try_emplace(imei, emptyVec);

        auto data = realTimeReq.find(imei);
        std::string imeiData = data->first;
        std::vector<std::string> idVector = data->second;

        if(toggle == "true") {
          if(idVector.empty()) {
            idVector.push_back(driverId);
            std::cout << "== REALTIME ==\n";
            server::toRealTime(imei, true);
//            std::cout << "Toggle True by : " << driverId << std::endl;
          } else {
            if(std::find(idVector.begin(), idVector.end(), driverId) == idVector.end()) {
              idVector.push_back(driverId);
            }
          }
        } else if (toggle == "false") {
          if(std::find(idVector.begin(), idVector.end(), driverId) != idVector.end()) {
            idVector = removeVectorElement(idVector, driverId);

            // If there is no user asking for real-time, turn to normal
            if (idVector.empty()) {
              std::cout << "== NORMAL ==\n";
              server::toRealTime(imei, false);
            }
          }
//          std::cout << "Toggle False by : " << driverId << std::endl;
          Publish:
          int res = publisher_thread(imei);

          std::this_thread::sleep_for(std::chrono::seconds(1));

          std::cout << std::endl;

          if (res == -1) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            goto Publish;
          }

          if (null_imei != ""){
            realTimeReq.erase(null_imei);
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

      } else {
        std::cout << "null" << std::endl;
      }
    }

  } // namespace mqtt
} // namespace karlo
