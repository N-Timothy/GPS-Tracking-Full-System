#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "GPS-Tracking/httpsRequest/post.hpp"
#include "GPS-Tracking/core/config.hpp"

#include <iostream>
#include <string>
#include <chrono>

namespace karlo {
    namespace httpsRequest {


        json config;

        void setHttpsConfig(json httpsConfig){
            config = httpsConfig;
        }

        void connect() {

//            json config = core::config::config_https();
            std::string URL = config["url"];

            int timeInterval = config["interval"];
//            post(URL);

            //auto start = std::chrono::system_clock::now();
            //auto now = std::chrono::system_clock::now();
            //auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

            while (true) {
                std::this_thread::sleep_for(std::chrono::seconds(timeInterval));
                post(URL, config);
//                now = std::chrono::system_clock::now();
//                seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
//                if (seconds > timeInterval) {
//                    start = std::chrono::system_clock::now();
 //               }
            }
        }

    } // namespace httpRequest
} //namespace karlo
