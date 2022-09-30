#include "GPS-Tracking/httpsRequest/httpsRequest.hpp"
#include "GPS-Tracking/httpsRequest/post.hpp"

#include <iostream>
#include <string>
#include <chrono>

namespace karlo {
    namespace httpsRequest {

        void connect(int timeInterval = INTERVAL) {

            std::string URL = "https://server-staging.karlo.id";

            // temporary, can be used or not
            post(URL);

            auto start = std::chrono::system_clock::now();
            auto now = std::chrono::system_clock::now();
            auto mseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();

            while (true) {
                now = std::chrono::system_clock::now();
                mseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
                if (mseconds > timeInterval) {
                    post(URL);
                    start = std::chrono::system_clock::now();
                }
            }
        }

    } // namespace httpRequest
} //namespace karlo