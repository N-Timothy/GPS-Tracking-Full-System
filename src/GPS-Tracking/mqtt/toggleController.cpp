#include "GPS-Tracking/mqtt/toggleController.hpp"
#include "GPS-Tracking/mqtt/publisher.hpp"

#include <string>
#include <iostream>

#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>


std::mutex m;
std::condition_variable cv;

std::string t = "false";

namespace karlo {
    namespace mqtt {

            using json = nlohmann::json;

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

        void realtimeMessage(std::string toggle, std::string imei) {

            for(;;) {
            
                std::unique_lock<std::mutex> lk(m);

                cv.wait( lk, [&toggle]{return t == "false";} );
                if (t == "false"){
                    std::cout << "hi" << std::endl;
                    break;
                }

            //publishser(imei);
                std::cout << "test" << std::endl;

                std::this_thread::sleep_for (std::chrono::seconds(5));
            }

            std::cout << "end here" << std::endl;
            //publisher(imei);
        }

        void toggleController(json subscribeMessage) {

            std::string imei = subscribeMessage["id"];
            std::string driverId = messageSeparator(subscribeMessage["toggle"], 0);
            std::string toggle = messageSeparator(subscribeMessage["toggle"], 1);
            t = toggle;

            std::cout << "Imei Requested : " << imei << std::endl;
            std::cout << "Driver id : " << driverId << std::endl;
            std::cout << "toggle : " << toggle << std::endl;


            if(toggle == "true") {
            std::thread realtimeMessageThread (realtimeMessage, toggle, imei);
            realtimeMessageThread.detach();
            } else {
                cv.notify_one();
            }

        }

    } // namespace mqtt
} // namespace karlo
