#include "GPS-Tracking/common/timer.hpp"

#include <chrono>
#include <thread>

namespace karlo {
    namespace common {

        std::map<int, std::pair<std::time_t, bool>> TIMEOUT;

        std::time_t time;

        void timer () {
            for(;;){
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                

                std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

            time = std::chrono::system_clock::to_time_t(now);
            } // for (;;)
        }

        void delete_timeout(int id){
            TIMEOUT.erase(id);
        }

        void add_timeout(int id){
            TIMEOUT.insert(std::make_pair(id, std::make_pair(time, false)));
        }

    } // namespace karlo
} // namespace karlo
